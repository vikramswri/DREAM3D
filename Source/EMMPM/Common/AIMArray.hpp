///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AIMARRAY_HPP_
#define AIMARRAY_HPP_


#include <EMMPM/Common/MXASetGetMacros.h>
#include <EMMPM/Common/AIMImage.h>

#include <iostream>
#include <string.h>

#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
#include <mm_malloc.h>
#endif



/**
* @class AIMArray AIMArray.h AIM/Common/AIMArray.h
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

    MXA_INSTANCE_2DVECTOR_PROPERTY(int, ImagePixelSize, _pixelSize)

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    const int32* getImagePixelSize()
    {
      return _pixelSize;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    template<typename K>
    void setImagePixelSize(typename AIMArray<K>::Pointer image)
    {
      int s[2];
      image->getImagePixelSize(s);
      _pixelSize[0] = s[0];
      _pixelSize[1] = s[1];
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    int32 getImagePixelWidth()
    {
      return _pixelSize[0];
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    int32 getImagePixelHeight()
    {
      return _pixelSize[1];
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    size_t getNumberOfElements()
    {
      return _pixelSize[0] * _pixelSize[1];
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
    // Tested
    // -----------------------------------------------------------------------------
    T* allocateDataArray(int32 width, int32 height, bool manageMemory = false)
    {
      this->deallocateArrayData();
      size_t total = (size_t) width * (size_t) height;
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      _imageBuffer = static_cast<T*>( _mm_malloc (total * sizeof(T), 16) );
#else
        _imageBuffer = new T[total];
#endif
      this->_managememory = manageMemory;
      this->setImagePixelSize(width, height);
      return _imageBuffer;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    template<typename K>
    T* allocateSameSizeArray(typename AIMArray<K>::Pointer array)
    {
      return allocateDataArray(array->getImagePixelWidth(), array->getImagePixelHeight(), true);
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
    int32 initializeImageWithSourceData(int32 width, int32 height, T* source)
    {
      this->allocateDataArray(width, height, true);

      T* b = static_cast<T*> (::memcpy(_imageBuffer, source, sizeof(T) * width * height));
      return (b == _imageBuffer) ? 1 : -1;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    int32 zeroArrayData()
    {
      size_t total = _pixelSize[0] * _pixelSize[1] * sizeof(T);
      ::memset(_imageBuffer, 0, total);
      return (NULL != _imageBuffer) ? 1 : -1;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    void printSelf(std::ostream& out)
    {
      out << "AIMImage Properties" << std::endl;
      // out << "  Origin:                 " << _origin[0] << ", " << _origin[1] << std::endl;
      //out << "  ImageMicronSize:        " << _micronSize[0] << " x " << _micronSize[1] << std::endl;
      out << "  ImagePixelSize:         " << _pixelSize[0] << " x " << _pixelSize[1] << std::endl;
      //out << "  Scaling:                " << _scaling[0] << ", " << _scaling[1] << std::endl;
      out << "  ManageMemory:           " << _managememory << std::endl;
      out << "  ImageBuffer:            " << *_imageBuffer << std::endl;
      // _intersectedTile->printSelf(out);
    }

  protected:
    AIMArray()
    {
      _pixelSize[0] = -1;
      _pixelSize[1] = -1;
      _managememory = false;
      this->_imageBuffer = NULL;
    }

  private:
    T* _imageBuffer;
    AIMArray(const AIMArray&); // Copy Constructor Not Implemented
    void operator=(const AIMArray&); // Operator '=' Not Implemented
};

#endif /* AIMARRAY_HPP_ */
