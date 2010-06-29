///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _AIM_IMAGE_H_
#define _AIM_IMAGE_H_

#include "TO79/Common/TO79Types.h"
#include "TO79/Common/MXASetGetMacros.h"

#include <iostream>

/**
* @class AIMImage AIMImage.h AIM/Common/AIMImage.h
* @brief This class represents a 2D image of type unsigned 8 bit characters.
* @author Michael A. Jackson for BlueQuartz Software
* @date Nov 5, 2009
* @version 1.0
*/
class AIMImage
{
  public:

  MXA_SHARED_POINTERS(AIMImage);
  MXA_STATIC_NEW_MACRO(AIMImage);
  MXA_TYPE_MACRO(AIMImage);
  virtual ~AIMImage();

  /**
  * @brief Creates a new AIMImage by copying all the settings from a source AIMImage
  * object and allocate the buffer at the same time. If the 'allocateBuffer' argument is
  * false then the image buffer will NOT be allocated. Note that the actual data from
  * the source mosaic is NOT copied into the new object.
  * @param image The source AIMImage to copy settings from.
  * @param allocateBuffer If false, the memory to hold the image is NOT allocated. Default=TRUE
  * @return AIMImage::Pointer object
  */
  static AIMImage::Pointer NewFromSourceMosaic(AIMImage::Pointer image, bool allocateBuffer = true);

  MXA_INSTANCE_2DVECTOR_PROPERTY(int, ImagePixelSize, _pixelSize);
  const int32_t* getImagePixelSize()
  {
    return _pixelSize;
  }

  void setImagePixelSize(AIMImage::Pointer image);
  int32_t getImagePixelWidth();
  int32_t getImagePixelHeight();


  void setImageBuffer(unsigned char* value, bool manageMemory = false);
  uint8_t* allocateImageBuffer(int32_t width, int32_t height, bool manageMemory = false);
  uint8_t* allocateSameSizeImage(AIMImage::Pointer image);
  unsigned char* getImageBuffer();

  uint8_t* getPointer(size_t index = 0);

  void deallocateImageBuffer();
  MXA_INSTANCE_PROPERTY(bool, ManageMemory, _managememory);

  int32_t initializeImageWithSourceData(int32_t width, int32_t height, uint8_t* source);

  int32_t fillImageBuffer(uint8_t val);

  void printSelf(std::ostream& out);

  size_t getTotalPixels();

  protected:
    AIMImage();

  private:
    uint8_t*      _imageBuffer;
    AIMImage(const AIMImage&);    // Copy Constructor Not Implemented
    void operator=(const AIMImage&);  // Operator '=' Not Implemented
};

#endif /* _AIM_IMAGE_H_ */
