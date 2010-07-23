///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
#include "AIMImage.h"

#include <string.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::Pointer AIMImage::NewFromSourceMosaic(AIMImage::Pointer image, bool allocateBuffer)
{
  AIMImage::Pointer p = AIMImage::New();
  if (allocateBuffer == true)
  {
    int w, h;
    image->getImagePixelDimension(w, h);
    uint8_t* u8 = p->allocateImageBuffer(w, h, image->getManageMemory());
    if (NULL == u8)
    {
      p = AIMImage::NullPointer();
    }
  }
  return p;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::AIMImage()
{
  m_ImagePixelDimension[0] = -1; m_ImagePixelDimension[1] = -1;
  _managememory = false;
  this->_imageBuffer = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::~AIMImage()
{
  if (this->_imageBuffer != NULL
      && this->_managememory == true)
  {
    delete[] this->_imageBuffer;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImage::setImageBuffer(unsigned char* value, bool manageMemory)
{
  if (this->_imageBuffer != NULL
      && this->_managememory == true
      && value != this->_imageBuffer )
  {
    delete[] this->_imageBuffer;
  }
  this->_imageBuffer = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t* AIMImage::allocateImageBuffer(int32_t width, int32_t height, bool manageMemory)
{
  this->deallocateImageBuffer();
  size_t total = (size_t)width * (size_t)height;
  _imageBuffer = new unsigned char[total];
  this->_managememory = manageMemory;
  this->setImagePixelDimension(width, height);

  return _imageBuffer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t* AIMImage::allocateSameSizeImage(AIMImage::Pointer image)
{
  return  allocateImageBuffer(image->getImagePixelWidth(), image->getImagePixelHeight(), true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t AIMImage::initializeImageWithSourceData(int32_t width, int32_t height, uint8_t* source)
{
  this->deallocateImageBuffer();
  size_t total = width * height;
  _imageBuffer = new unsigned char[total];
  this->_managememory = true;
  this->setImagePixelDimension(width, height);

  uint8_t* b = static_cast<uint8_t*>(::memcpy(_imageBuffer, source, total));
  return (b == _imageBuffer) ? 1 : -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t AIMImage::fillImageBuffer(uint8_t val)
{
  size_t total = m_ImagePixelDimension[0] * m_ImagePixelDimension[1];
  ::memset(_imageBuffer, val, total);
  return (NULL != _imageBuffer) ? 1 : -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImage::deallocateImageBuffer()
{
  if (this->_imageBuffer != NULL
      && this->_managememory == true)
  {
    delete[] this->_imageBuffer;
  }
  _imageBuffer = NULL;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned char* AIMImage::getImageBuffer()
{
  return _imageBuffer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint8_t* AIMImage::getPointer(size_t index)
{
  return &(_imageBuffer[index]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImage::printSelf(std::ostream& out)
{

  out << "AIMImage Properties" << std::endl;
 // out << "  Origin:                 " << _origin[0] << ", " << _origin[1] << std::endl;
  //out << "  ImageMicronSize:        " << _micronSize[0] << " x " << _micronSize[1] << std::endl;
  out << "  ImagePixelDimension:         " << m_ImagePixelDimension[0] << " x " << m_ImagePixelDimension[1] << std::endl;
  //out << "  Scaling:                " << _scaling[0] << ", " << _scaling[1] << std::endl;
  out << "  ManageMemory:           " << _managememory << std::endl;
  out << "  ImageBuffer:            " << *_imageBuffer << std::endl;
// _intersectedTile->printSelf(out);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImage::setImagePixelDimension(AIMImage::Pointer image)
{
  int s[2];
  image->getImagePixelDimension(s);
  m_ImagePixelDimension[0] = s[0];
  m_ImagePixelDimension[1] = s[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t AIMImage::getImagePixelWidth()
{
  return m_ImagePixelDimension[0];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t AIMImage::getImagePixelHeight()
{
  return m_ImagePixelDimension[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t AIMImage::getTotalPixels()
{
  return static_cast<size_t> (m_ImagePixelDimension[0] * m_ImagePixelDimension[1]);
}

