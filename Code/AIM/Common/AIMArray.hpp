///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AIMARRAY_HPP_
#define AIMARRAY_HPP_

//-- C Includes
#include <string.h>

//-- C++ Includes
#include <iostream>
#include <limits>

//-- MXA Includes
#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"
#include "AIM/Common/AIMImage.h"

#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
#include <mm_malloc.h>
#endif



/**
* @class AIMArray AIMArray.hpp AIM/Common/AIMArray.hpp
* @brief This class represents and array of primitives in a contiguous section of memory. If the compile flag
* AIM_USE_SSE is used and SSE is found on the system then SSE specific malloc and free will be used in order
* to allocated properly aligned memory.
* @author Michael A. Jackson for BlueQuartz Software
* @date Nov 5, 2009
* @version 1.0
*/
template<typename T>
class AIMArray
{
  public:

    MXA_SHARED_POINTERS(AIMArray)
    MXA_STATIC_NEW_MACRO(AIMArray)
    MXA_TYPE_MACRO(AIMArray)

    /**
     * @brief
     */
    virtual ~AIMArray()
    {
      if (this->_imageBuffer != NULL && this->_managememory == true)
      {
        this->deallocateArrayData();
      }
    }

//    void setNumElements(size_t value) { this->m_NumElements = value; }
    size_t  getNumElements() { return m_NumElements; }

    /**
     * @brief Sets the dimensions of the array. Does NOT allocate or deallocate
     * any memory. The new dimensions must have the same total number of elements
     * as the previous dimensions.
     * @param dims
     */
    void setDimensions(std::vector<size_t> dims)
    {
      size_t _tot = 1;
      for (std::vector<size_t>::iterator dim = m_Dimensions.begin(); dim != m_Dimensions.end(); ++dim )
      {
        _tot *= *dim;
      }

      size_t dtot = 1;
      for (std::vector<size_t>::iterator d = dims.begin(); d != dims.end(); ++d )
      {
        dtot *= *d;
      }
      if (_tot == dtot) {
        m_Dimensions = dims;
      }
    }

    /**
     * @brief Returns the dimensions of the array
     * @return
     */
    std::vector<size_t> getDimensions()
    {
      return m_Dimensions;
    }

    /**
     * @brief
     * @param dims
     */
    void reform(std::vector<size_t> dims)
    {
      setDimensions(dims);
    }

  /**
   * @brief Adds one array to another array. Note that both arrays MUST contain
   * the same number of elements.
   * @param v1 Another AIMArray to add to this one
   * @return New AIM Array that is the sum of this array and the v1 array.
   */
    template <typename K, typename J>
    typename AIMArray<J>::Pointer add(typename AIMArray<K>::Pointer v1)
    {
      T* v0Ptr = _imageBuffer;
      K* v1Ptr = v1->getPointer(0);

      if (getNumElements() != v1->getNumElements())
      {
        std::cout << "Number of elements in Arrays do not match. " << __FILE__ << "(" << __LINE__ << ")" << std::endl;
        return AIMArray<J>::NullPointer();
      }

      typename AIMArray<J>::Pointer outVector =  AIMArray<J>::New();

      size_t numElements = getNumElements();
      J* outPtr = outVector->allocateDataArray(numElements, true);
      if (NULL == outPtr)
      {
        std::cout << "Could not Allocate new array. " << __FILE__ << "(" << __LINE__ << ")" << std::endl;
        return AIMArray<J>::NullPointer();
      }

      for (size_t e = 0; e < numElements; ++e)
      {
        *outPtr = static_cast<J>(*v0Ptr) + static_cast<J>(*v1Ptr);
        ++outPtr; ++v0Ptr; ++v1Ptr;
      }
      return outVector;
    }

    /**
     * @brief Applies a right or left bit shift to each element
     * @param shift The number of bits to shift where positive is a left and negative
     * is a right shift
     * @return New AIM Array that is bit shifted
     */
    template <typename K>
    typename AIMArray<K>::Pointer bitShift( int shift)
    {
      T* inPtr = getPointer(0);
      typename AIMArray<K>::Pointer outArray = AIMArray<K>::New();
      K* outPtr = outArray->allocateDataArray(getNumElements(), true);

      if (NULL == outPtr)
      {
        return AIMArray<K>::NullPointer();
      }
      size_t numElements = getNumElements();
      K tmp = 0;
      if (shift < 0)  // Right Shift
      {
        for (size_t e = 0; e < numElements; ++e)
        {
          tmp = getValue(e);
          *outPtr = tmp >> shift;
          ++outPtr; ++inPtr;
        }
      }
      else  // Left Shift
      {
        for (size_t e = 0; e < numElements; ++e)
        {
          tmp = getValue(e);
          *outPtr = tmp << shift;
          ++outPtr; ++inPtr;
        }
      }
      return outArray;
    }

    /**
     * @brief Computes the min and max values of the array
     * @param min
     * @param max
     */
    void minMax(T &min, T &max)
    {
      size_t nelements = getNumElements();
      T* ptr = getPointer(0);
      max = std::numeric_limits<T>::min();
      min = std::numeric_limits<T>::max();
      for (size_t i = 0; i < nelements; ++i)
      {
        if (ptr[i] < min) min = ptr[i];
        if (ptr[i] > max) max = ptr[i];
      }
    }

    /**
     * @brief Computes the sum of all the elements in the array
     * @return
     */
    template<typename K>
    K sumElements()
    {
      size_t nelements = getNumElements();
      T* ptr = getPointer(0);
      K total = static_cast<K>(0);
      for (size_t i = 0; i < nelements; ++i)
      {
        total += static_cast<K>(ptr[i]);
      }
      return total;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template <typename K>
    typename AIMArray<K>::Pointer normalize()
    {

      typename AIMArray<K>::Pointer out = AIMArray<K>::New();
      out->allocateDataArray(getNumElements(), true);
      out->setDimensions(getDimensions() );
      K* outPtr = out->getPointer(0);

      K total = sumElements<double>();
      size_t nelements = getNumElements();
      T* ptr = getPointer(0);
      for (size_t i = 0; i < nelements; ++i)
      {
        outPtr[i] =  static_cast<K>(ptr[i]/total);
      }
      return out;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    void setArrayData(T* value, bool manageMemory = false)
    {
      if (this->_imageBuffer != NULL && this->_managememory == true && value != this->_imageBuffer)
      {
        this->deallocateArrayData();
      }
      this->_imageBuffer = value;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    T* getArrayData()
    {
      return _imageBuffer;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    T* getPointer(size_t index = 0)
    {
      return &(_imageBuffer[index]);
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setValue(T value, size_t index)
    {
      _imageBuffer[index] = value;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    T getValue(size_t index)
    {
      return _imageBuffer[index];
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    T* allocateDataArray(size_t numElements, bool manageMemory = false)
    {
      this->deallocateArrayData();

#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      _imageBuffer = static_cast<T*>( _mm_malloc (numElements * sizeof(T), 16) );
#else
        _imageBuffer = new T[numElements];
#endif
      this->_managememory = manageMemory;
      m_NumElements = numElements;
      m_Dimensions.clear();
      m_Dimensions.push_back(numElements);
      return _imageBuffer;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    template<typename K>
    T* allocateSameSizeArray(typename AIMArray<K>::Pointer array)
    {
      allocateDataArray(array->getNumElements(), true);
      setDimensions( array->getDimensions() );
      return _imageBuffer;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    void deallocateArrayData()
    {
      if (this->_imageBuffer != NULL && this->_managememory == true)
      {
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
        _mm_free( this->_imageBuffer );
#else
        delete[] this->_imageBuffer;
#endif
      }
      _imageBuffer = NULL;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    MXA_INSTANCE_PROPERTY(bool, ManageMemory, _managememory)

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    int32_t initializeImageWithSourceData(size_t numElements, T* source)
    {
      this->allocateDataArray(numElements, true);

      T* b = static_cast<T*> (::memcpy(_imageBuffer, source, sizeof(T) * numElements));
      return (b == _imageBuffer) ? 1 : -1;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    int32_t zeroArrayData()
    {
     // size_t total = _pixelSize[0] * _pixelSize[1] * sizeof(T);
      ::memset(_imageBuffer, 0, m_NumElements);
      return (NULL != _imageBuffer) ? 1 : -1;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    void printSelf(std::ostream& out, int valuesPerLine)
    {
      //      out << "AIMImage Properties" << std::endl;
      //      out << "  ImagePixelSize:         " << _pixelSize[0] << " x " << _pixelSize[1] << std::endl;
      //      out << "  ManageMemory:           " << _managememory << std::endl;
      //      out << "  ImageBuffer:            " << *_imageBuffer << std::endl;
      for (int y = 0; y < m_NumElements; ++y)
      {
        //   out << "[" << x << "]  ";
        //  printf ("[%04d]  ", y);

        printf("%04d ", (int)_imageBuffer[y]);
        //          out << (int)_imageBuffer[ (_pixelSize[0] * y) + x];
        /* if (x < _pixelSize[0] - 1) */
        {
          out << " ";
        }
        if (y % valuesPerLine == 0)
        {
          printf("\n");
        }
        //         out << std::endl;
      }
    }

  protected:
    AIMArray()
    {
      m_Dimensions.push_back(0);
      m_NumElements = 0;
      _managememory = false;
      this->_imageBuffer = NULL;
    }

  private:
    size_t      m_NumElements;
    T* _imageBuffer;
    std::vector<size_t> m_Dimensions;
    AIMArray(const AIMArray&); // Copy Constructor Not Implemented
    void operator=(const AIMArray&); // Operator '=' Not Implemented
};

#endif /* AIMARRAY_HPP_ */
