///////////////////////////////////////////////////////////////////////////////
//
/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//
//  This code was written under United States Air Force Contract number
//                           F33615-03-C-5220
//
///////////////////////////////////////////////////////////////////////////////
#include "AIMCropGrayScaleImage.h"

#include <itkRegionOfInterestImageFilter.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMCropGrayScaleImage::AIMCropGrayScaleImage(AIMImage::Pointer imageData, int32_t* insets)
{
  this->_numChannels = 1;
  ::memcpy(this->_insets, insets, sizeof(int32_t) * 4);

  this->_inData = imageData;
  this->_outData = AIMImage::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMCropGrayScaleImage::~AIMCropGrayScaleImage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::Pointer AIMCropGrayScaleImage::getOutputImage()
{
  return _outData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AIMCropGrayScaleImage::run()
{
  int err = 0;
  int32_t _inDimensions[2] = {0, 0};
  _inData->getImagePixelDimension(_inDimensions);
  PixelType* data = this->_inData->getImageBuffer();
  ImportFilterType::Pointer importFilter = ImportFilterType::New();
  initializeImportFilter(importFilter, data, _inDimensions[0], _inDimensions[1]);

  typedef itk::RegionOfInterestImageFilter< InputImageType, OutputImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();

  OutputImageType::IndexType start;
  start[0] = this->_insets[0];
  start[1] = this->_insets[2];
  OutputImageType::SizeType size;
  size[0] = _inDimensions[0] - this->_insets[0] - this->_insets[1];
  size[1] = _inDimensions[1] - this->_insets[2] - this->_insets[3];

  OutputImageType::RegionType desiredRegion;
  desiredRegion.SetSize( size );
  desiredRegion.SetIndex( start );

  filter->SetRegionOfInterest( desiredRegion );
  filter->SetInput( importFilter->GetOutput() );

  try
  {
    filter->Update();
    PixelType* outImagePtr = filter->GetOutput()->GetBufferPointer();
    int32_t _outDimensions[2] = {size[0],size[1]};

    //Copy the data to the output array
    size_t m_length = _outDimensions[0] * _outDimensions[1] * sizeof(PixelType);
    this->_outData = AIMImage::New();
    uint8_t* b = this->_outData->allocateImageBuffer( _outDimensions[0], _outDimensions[1], true);
    std::memcpy(b, outImagePtr, m_length);
#if 0
    float f[2] = {0.0f, 0.0f};
    float o[2] = {0.0f, 0.0f};


    _inData->getScaling(f);
    _outData->setScaling(f);
    _inData->getOrigin(o);

    o[0] = o[0] + (f[0] * _insets[0]);
    o[1] = o[1] + (f[1] * _insets[1]);
    _outData->setOrigin(o);
    f[0] = f[0] * size[0];
    f[1] = f[1] * size[0];
    _outData->setImageMicronSize(f);
#endif

  }
  catch (itk::ExceptionObject & excep)
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    err = -1;
  }

  return err;
}

