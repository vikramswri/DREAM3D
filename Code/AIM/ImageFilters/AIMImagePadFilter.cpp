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

#include "AIMImagePadFilter.h"

#include <itkConstantPadImageFilter.h>


typedef itk::ConstantPadImageFilter<InputImageType, OutputImageType>     FilterType;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImagePadFilter::AIMImagePadFilter(AIMImage::Pointer imageData, int32_t pixelPad[2], int32_t padValue)
{
  m_InData = imageData;
  m_PixelPad[0] = pixelPad[0];
  m_PixelPad[1] = pixelPad[1];
  m_PixelPad[2] = pixelPad[2];
  m_PixelPad[3] = pixelPad[3];
  m_PadValue = padValue;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImagePadFilter::~AIMImagePadFilter()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::Pointer AIMImagePadFilter::getOutputImage()
{
  return m_OutData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AIMImagePadFilter::run()
{
  int err = 0;
  int32_t _inDimensions[2] = {0, 0};
  m_InData->getImagePixelDimension(_inDimensions);
  PixelType* data = this->m_InData->getImageBuffer();
  ImportFilterType::Pointer importFilter = ImportFilterType::New();
  initializeImportFilter(importFilter, data, _inDimensions[0], _inDimensions[1]);


  FilterType::Pointer constantPad = FilterType::New();

  long unsigned int lowerfactors[2];
  long unsigned int upperfactors[2];

  lowerfactors[0] = m_PixelPad[0];
  lowerfactors[1] = m_PixelPad[2];

  upperfactors[0] = m_PixelPad[1];
  upperfactors[1] = m_PixelPad[3];

  constantPad->SetPadLowerBound(lowerfactors);
  constantPad->SetPadUpperBound(upperfactors);
  constantPad->SetInput( importFilter->GetOutput() );
  constantPad->UpdateLargestPossibleRegion();

  try
  {
    constantPad->Update();

    OutputImageType::RegionType requestedRegion;
    requestedRegion = constantPad->GetOutput()->GetRequestedRegion();

    OutputImageType::SizeType size;
    size = requestedRegion.GetSize();


    PixelType* outImagePtr = constantPad->GetOutput()->GetBufferPointer();
    int32_t _outDimensions[2] = {size[0],size[1]};

    //Copy the data to the output array
    size_t m_length = _outDimensions[0] * _outDimensions[1] * sizeof(PixelType);
    m_OutData = AIMImage::New();
    uint8_t* b = m_OutData->allocateImageBuffer( _outDimensions[0], _outDimensions[1], true);
    std::memcpy(b, outImagePtr, m_length);

  }
  catch (itk::ExceptionObject & excep)
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    err = -1;
  }

  return err;
}

