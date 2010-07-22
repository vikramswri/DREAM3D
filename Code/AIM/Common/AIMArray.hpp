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

    virtual ~AIMArray()
    {
      if (this->_imageBuffer != NULL && this->_managememory == true)
      {
        this->deallocateArrayData();
      }
    }

//    MXA_INSTANCE_2DVECTOR_PROPERTY(int, ImagePixelSize, _pixelSize)
    MXA_INSTANCE_PROPERTY_m(size_t, NumElements);

    void setDimensions(std::vector<size_t> dims)
    {
      m_Dimensions = dims;
    }

    std::vector<size_t> getDimensions()
    {
      return m_Dimensions;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template <typename K, typename J>
    typename AIMArray<J>::Pointer add(typename AIMArray<K>::Pointer v1)
    {
      T* v0Ptr = _imageBuffer;
      K* v1Ptr = v1->getPointer(0);

//      size_t nEl0 = getNumElements();
//      size_t nEl1 = v1->getNumElements();
      if (getNumElements() != v1->getNumElements())
      {
        std::cout << "Number of elements in Arrays do not match. " << __FILE__ << "(" << __LINE__ << ")" << std::endl;
        return AIMArray<J>::NullPointer();
      }

      typename AIMArray<J>::Pointer h =  AIMArray<J>::New();

      size_t numElements = getNumElements();
      J* outPtr = h->allocateDataArray(numElements, true);
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

      return h;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    template <typename K>
    typename AIMArray<K>::Pointer ishft( int shift)
    {
      T* inPtr = getPointer(0);
      typename AIMArray<K>::Pointer h = AIMArray<K>::New();
      K* outPtr = h->allocateDataArray(getNumElements(), true);

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
      return h;
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
      this->setNumElements(numElements);
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
    T* _imageBuffer;
    std::vector<size_t> m_Dimensions;
    AIMArray(const AIMArray&); // Copy Constructor Not Implemented
    void operator=(const AIMArray&); // Operator '=' Not Implemented
};

#endif /* AIMARRAY_HPP_ */
