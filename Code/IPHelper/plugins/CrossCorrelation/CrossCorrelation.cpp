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
//-- Our Includes
#include "CrossCorrelation.h"

//-- C++ Includes
#include <iostream>
#include <sstream>

// ITK Includes
#include <itkCastImageFilter.h>
#include <itkIndex.h>
#include <itkResampleImageFilter.h>


//-- MXA Includes
#include "MXA/MXATypes.h"
#include "MXA/Utilities/MXALogger.h"

//-- Our Includes
#include "AIM/ImageFilters/AIMCropGrayScaleImage.h"
#include "AIM/ImageFilters/AIMImagePadFilter.h"
#include "CrossCorrelation/pcm/itkPhaseCorrelationOperator.h"
#include "CrossCorrelation/pcm/itkMaxPhaseCorrelationOptimizer.h"
#include "CrossCorrelation/itk/itkFFTShiftImageFilter.h"

// Typedefs for the itk classes which make the code a bit easier to read
typedef itk::Image< pcm::PixelType, pcm::Dimension >                            ImageType;
typedef pcm::PhaseCorrelationOperator< ImageType, ImageType>                    OperatorType;
typedef pcm::PhaseCorrelationImageRegistrationMethod<ImageType, ImageType>      RegistrationType;
typedef pcm::MaxPhaseCorrelationOptimizer<RegistrationType>                     OptimizerType;
typedef itk::CastImageFilter<UCharImageType, FFTImageType>              CasterType;
typedef itk::CastImageFilter<FFTImageType, UCharImageType>              InverseCasterType;

typedef itk::ResampleImageFilter<ImageType, ImageType >                 ResampleFilterType;
typedef itk::ImageFileWriter< UCharImageType >                          WriterType;




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelation::CrossCorrelation()
{
  m_FFTResolutions[0] = 128;
  m_FFTResolutions[1] = 256;
  m_FFTResolutions[2] = 512;
  m_FFTResolutions[3] = 1024;
  m_FFTResolutions[4] = 1500;
  m_FFTResolutions[5] = 2048;
  m_FFTResolutions[6] = 2500;
  m_FFTResolutions[7] = 3000;
  m_FFTResolutions[8] = 3456;
  m_FFTResolutions[9] = 4096;
  m_FFTResolutions[10] = 4500;
  m_FFTResolutions[11] = 5000;
  m_FFTResolutions[12] = 5625;
  m_FFTResolutions[13] = 6000;
  m_AllowableError = 0.005f;  // 0.5% allowable error
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelation::~CrossCorrelation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelation::run()
{
  int err = 0;
  double trans0[2];
  double trans1[2];
  double diff[2];
  bool didConverge = false;
  MXALOGGER_METHOD_VARIABLE_INSTANCE

  if (NULL == m_FixedImage)
  {
    setErrorCondition(-2);
    return;
  }
  if (NULL == m_MovingImage)
  {
    setErrorCondition(-3);
    return;
  }
  if (NULL == m_CrossCorrelationData)
  {
    setErrorCondition(-4);
    return;
  }


// Cache the loaded mosaic for each run
  err = registerAtFFTResolution(m_FFTResolutions[0]);
  if (err < 0)
  {
    setErrorCondition(err);
    return;
  }
  m_CrossCorrelationData->getTranslations(trans0);

  float absDiff = R3D::PCM::PCMAbsolutePixelOffset * this->m_CrossCorrelationData->getScaling();

  for(uint32_t i = 1; i < R3D::PCM::FFTResolutionSize; ++i)
  {
//    if (i == 3)
//    {
//      mxa_log << logTime() << "CrossCorrelation::run() break point. Should have registered in last iteration" << std::endl;
//    }
    setErrorCondition(0);
    err = registerAtFFTResolution(m_FFTResolutions[i]);
    if (err < 0)
    {
      mxa_log << logTime() << "CrossCorrelation::run() Error returned from CrossCorrelation::registerAtFFTResolution(" << m_FFTResolutions[i] << ");" << std::endl;
      break;
    }

    m_CrossCorrelationData->getTranslations(trans1);
    diff[0] = trans0[0] - trans1[0];
    diff[1] = trans0[1] - trans1[1];
    if (diff[0] < 0.0f) { diff[0] *= -1.0f;}
    if (diff[1] < 0.0f) { diff[1] *= -1.0f;}

    if ( diff[0] <= absDiff && diff[1] <= absDiff)
    {
      didConverge = true;
      break;
    }

    // Did not match good enough so increment our resolution and try again.
    trans0[0] = trans1[0];
    trans0[1] = trans1[1];
  }

  if (true == didConverge) {
    this->m_CrossCorrelationData->setXTrans( (trans0[0] + trans1[0])/2.0 );
    this->m_CrossCorrelationData->setYTrans( (trans0[1] + trans1[1])/2.0 );
    this->m_CrossCorrelationData->setComplete(1);
    setErrorCondition(0);
  }
  else
  {
    this->m_CrossCorrelationData->setXTrans(0.0);
    this->m_CrossCorrelationData->setYTrans(0.0);
    setErrorCondition(-100);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CrossCorrelation::writeRegisteredImage(AIMImage::Pointer image,
                                       CrossCorrelationData::Pointer ccData,
                                       const std::string &filename)
{
  //std::cout << "Transform complete - Writing tmp image" << std::endl;
  ImportFilterType::Pointer movingImageFilter = ImportFilterType::New();
  itkImportFilterFromAIMImage(image, movingImageFilter, ccData, false);

  CasterType::Pointer castToFloat = CasterType::New();
  castToFloat->SetInput(movingImageFilter->GetOutput());

  RegistrationType::ParametersType parameters(2);
  parameters[0] = ccData->getXTrans();
  parameters[1] = ccData->getYTrans();

  RegistrationType::TransformPointer transform = RegistrationType::TransformType::New();
  transform->SetParameters(parameters);

  ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  resampler->SetInput( castToFloat->GetOutput() );
  resampler->SetTransform(transform);

  resampler->SetSize( movingImageFilter->GetRegion().GetSize());
  resampler->SetOutputOrigin( movingImageFilter->GetOrigin() );
  resampler->SetOutputSpacing( movingImageFilter->GetSpacing() );
  resampler->SetDefaultPixelValue( 0 );

  InverseCasterType::Pointer castToUChar = InverseCasterType::New();
  castToUChar->SetInput(resampler->GetOutput());

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( castToUChar->GetOutput() );
  writer->SetFileName(filename.c_str());
  writer->Update();

 // std::cout << "Writing complete" << std::endl;
  return 1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t CrossCorrelation::registerAtFFTResolution(int fftDim )
{
  MXALOGGER_METHOD_VARIABLE_INSTANCE

  // Load up the fixed image
  ImportFilterType::Pointer fxImport;
  fxImport = ImportFilterType::New();
  initializeImportFilter(fxImport, fftDim,
                         m_FixedImage,
                         true);
  if (getErrorCondition() < 0)
  {
    return getErrorCondition();
  }

  // Load up the moving image
  ImportFilterType::Pointer mvImport;
  mvImport = ImportFilterType::New();
  initializeImportFilter(mvImport, fftDim,
                         m_MovingImage,
                         false);

  if (getErrorCondition() < 0)
  {
    return getErrorCondition();
  }
  // Create a Correlation Image if wanted
 // createCorrelationImages(fxImport, mvImport);

  // Register the images using the FFT Correlation Method
  return registerImages(fxImport, mvImport);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelation::itkImportFilterFromAIMImage(AIMImage::Pointer image,
                                                   ImportFilterType::Pointer importFilter,
                                                   CrossCorrelationData::Pointer ccData,
                                                   bool isFixedImage)
{
  ImportFilterType::SizeType size;
  size[0] = image->getImagePixelWidth(); // size along X
  size[1] = image->getImagePixelHeight(); // size along Y

  ImportFilterType::IndexType start;
  start.Fill(0);
  ImportFilterType::RegionType region;
  region.SetIndex(start);
  region.SetSize(size);
  importFilter->SetRegion(region);

  double origin[pcm::Dimension];
  if ( true == isFixedImage)
  {
    origin[0] = ccData->getXFixedOrigin(); // X coordinate
    origin[1] = ccData->getYFixedOrigin(); // Y coordinate
  }
  else
  {
    origin[0] = ccData->getXMovingOrigin(); // X coordinate
    origin[1] = ccData->getYMovingOrigin(); // Y coordinate
  }

  importFilter->SetOrigin(origin);

  double spacing[pcm::Dimension];
  spacing[0] = ccData->getScaling(); //  /xScale * 1000.0; // pixels per millimeter along X direction
  spacing[1] = ccData->getScaling(); //  /yScale * 1000.0; // pixels per millimeter along Y direction
  importFilter->SetSpacing(spacing);

  const bool importImageFilterWillOwnTheBuffer = false;
  importFilter->SetImportPointer(image->getImageBuffer(), size[0] * size[1], importImageFilterWillOwnTheBuffer);
  image->setManageMemory(true);
  importFilter->Update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelation::initializeImportFilter(ImportFilterType::Pointer importFilter,
                                              int fftDim,
                                              AIMImage::Pointer imageData,
                                              bool isFixedImage)
{
  MXALOGGER_METHOD_VARIABLE_INSTANCE
  AIMImage::Pointer fftImageRegion = AIMImage::NullPointer();

  // Can we crop out a fftDim size image or do we pad
  if ( imageData->getImagePixelWidth() >= fftDim
      && imageData->getImagePixelHeight() >= fftDim)
  {
    // Extract the ROI from the center of the image
    int32_t insets[4]; // Left Right Top Bottom
    // Compute the insets on each side of the image
    insets[0] = (imageData->getImagePixelWidth() / 2) - (fftDim / 2);
    insets[1] = imageData->getImagePixelWidth() - (insets[0] + fftDim);
    insets[2] = (imageData->getImagePixelHeight() / 2) - (fftDim / 2);
    insets[3] = imageData->getImagePixelHeight() - (insets[2] + fftDim);
    // Instantiate our "Cropper" object
    AIMCropGrayScaleImage crop(imageData, insets);
    crop.run();  // Crop the fftDim sized region from the input image
    fftImageRegion = crop.getOutputImage();
  }
  else
  {
    // Pad the image to the size of the FFT Dimension
    int32_t pad[4] = {0, 0, 0, 0}; // Left Right top bottom
    int32_t extra[4] = {0, 0, 0, 0};
    int32_t dims[2] = {imageData->getImagePixelWidth(), imageData->getImagePixelHeight() };

    if (dims[0] % 2 != 0)
    {
      extra[0] = 1;
    }
    if (dims[1] % 2 != 0)
    {
      extra[2] = 1;
    }

    pad[0] = (fftDim - imageData->getImagePixelWidth() - extra[0])/2 + extra[0];
    pad[1] = (fftDim - imageData->getImagePixelWidth() - extra[1])/2 + extra[1];

    pad[2] = (fftDim - imageData->getImagePixelHeight() - extra[2])/2 + extra[2];
    pad[3] = (fftDim - imageData->getImagePixelHeight() - extra[3])/2 + extra[3];

    AIMImagePadFilter padFilter(imageData, pad, 0);
    padFilter.run();
    fftImageRegion = padFilter.getOutputImage();
  }

  if (NULL == fftImageRegion)
  {
    mxa_log << logTime() << "Cross Correlation::initializeImportFilter() with FFT Dim of " << fftDim << " threw an error.";
    setErrorCondition(-10);
    return;
  }


  m_CrossCorrelationData->setImageWidth(fftDim);
  m_CrossCorrelationData->setImageHeight(fftDim);

  ImportFilterType::SizeType size;
  size[0] = m_CrossCorrelationData->getImageWidth(); // size along X
  size[1] = m_CrossCorrelationData->getImageHeight(); // size along Y

  ImportFilterType::IndexType start;
  start.Fill(0);
  ImportFilterType::RegionType region;
  region.SetIndex(start);
  region.SetSize(size);
  importFilter->SetRegion(region);

  double origin[pcm::Dimension];
  if ( true == isFixedImage)
  {
    origin[0] = m_CrossCorrelationData->getXFixedOrigin(); // X coordinate
    origin[1] = m_CrossCorrelationData->getYFixedOrigin(); // Y coordinate
  }
  else
  {
    origin[0] = m_CrossCorrelationData->getXMovingOrigin(); // X coordinate
    origin[1] = m_CrossCorrelationData->getYMovingOrigin(); // Y coordinate
  }

  //mxa_log << logTime() << "Slice: " << sliceInfo->getSliceNumber() << " origin: " << origin[0] << ", " << origin[1] << std::endl;

  importFilter->SetOrigin(origin);

  double spacing[pcm::Dimension];
  spacing[0] = m_CrossCorrelationData->getScaling(); //  /xScale * 1000.0; // pixels per millimeter along X direction
  spacing[1] = m_CrossCorrelationData->getScaling(); //  /yScale * 1000.0; // pixels per millimeter along Y direction
  importFilter->SetSpacing(spacing);

  const bool importImageFilterWillOwnTheBuffer = true;
  importFilter->SetImportPointer(fftImageRegion->getImageBuffer(), size[0] * size[1], importImageFilterWillOwnTheBuffer);
  fftImageRegion->setManageMemory(false);
  importFilter->Update();

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CrossCorrelation::registerImages(ImportFilterType::Pointer fixedImageImportFilter,
                                              ImportFilterType::Pointer movingImageImportFilter )
{
  int32_t err = 0;

   RegistrationType::Pointer pcmRegistration  = RegistrationType::New();
   RegistrationType::TransformPointer transform = RegistrationType::TransformType::New();
   OperatorType::Pointer     pcmOperator      = OperatorType::New();
   OptimizerType::Pointer    pcmOptimizer     = OptimizerType::New();

   CasterType::Pointer fxUcharToFft = CasterType::New();
   fxUcharToFft->SetInput(fixedImageImportFilter->GetOutput());
   CasterType::Pointer mvUcharToFft = CasterType::New();
   mvUcharToFft->SetInput(movingImageImportFilter->GetOutput());

   pcmRegistration->SetOperator(    pcmOperator               );
   pcmRegistration->SetOptimizer(   pcmOptimizer              );
   pcmRegistration->SetFixedImage(  fxUcharToFft->GetOutput()  );
   pcmRegistration->SetMovingImage( mvUcharToFft->GetOutput() );
   pcmRegistration->SetTransform( transform);

   // execute the registration
   try
     {
     pcmRegistration->Update();
     }
   catch ( itk::ExceptionObject & e )
     {
     std::cout << "Some error during registration:" << std::endl;
     std::cout << e << std::endl;
     err = -1;
     }

   RegistrationType::ParametersType parameters = pcmRegistration->GetLastTransformParameters();
   m_CrossCorrelationData->setXTrans(parameters(0));
   m_CrossCorrelationData->setYTrans(parameters(1));

   err = 1;
   return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string CrossCorrelation::makeAnalysisImage(const std::string &path,
                                                         const std::string &slice,
                                                         const std::string &num,
                                                         const std::string &label)
{
  std::string image(path);
  image = image + "/MNML_5_500x_" + slice + "-Raw_p" + num + "-" + label + ".bmp";
  return image;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelation::createCorrelationImages(  ImportFilterType::Pointer fxImport,
                                                      ImportFilterType::Pointer mvImport)
{
  typedef itk::VnlFFTRealToComplexConjugateImageFilter<FFTPixelType, pcm::Dimension >  FFTFilterType;
   typedef itk::VnlFFTComplexConjugateToRealImageFilter<FFTPixelType, pcm::Dimension >  InverseFFTType;

   typedef      FFTFilterType::OutputImageType                          ComplexImageType;
   typedef itk::ImageRegionIterator<ComplexImageType>                   OutputIterator;
   typedef itk::NumericTraits<FFTPixelType>::RealType                     InternalPixelType;
   typedef std::complex<InternalPixelType>                             ComplexPixelType;
   typedef itk::ImageRegionIterator<FFTImageType>                       IFFTIteratorType;
   typedef itk::CastImageFilter<FFTImageType, UCharImageType>        InverseCasterType;
   typedef itk::RescaleIntensityImageFilter<FFTImageType, UCharImageType > RescaleFilterType;
   typedef itk::FFTShiftImageFilter< UCharImageType, UCharImageType >          FFTShiftImageFilterType;

    FFTFilterType::Pointer fxFFTFilter = FFTFilterType::New();


    CasterType::Pointer uCharToFloatCaster = CasterType::New();
    uCharToFloatCaster->SetInput( fxImport->GetOutput() );

    fxFFTFilter->SetInput( uCharToFloatCaster->GetOutput() );
    /*  --------------------Run FFT on Fixed Image  --------------------------- */
    try
      {
      fxFFTFilter->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Error: " << std::endl;
      std::cerr << excp << std::endl;
      return;
      }

    ImportFilterType::SizeType imageSize = mvImport->GetOutput()->GetLargestPossibleRegion().GetSize();
    ComplexImageType* fxComplexImage = fxFFTFilter->GetOutput();

    /*  ----------------------------------------------------------------------- */
    /*  -------------------- Get the Moving Image  --------------------------- */
    FFTFilterType::Pointer mvFFTFilter = FFTFilterType::New();

    CasterType::Pointer mvCharToFloatCaster = CasterType::New();
    mvCharToFloatCaster->SetInput( mvImport->GetOutput() );

    mvFFTFilter->SetInput( mvCharToFloatCaster->GetOutput() );
    /*  --------------------Run FFT on Moving Image  --------------------------- */
    try
      {
      mvFFTFilter->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Error: " << std::endl;
      std::cerr << excp << std::endl;
      return;
      }

    ComplexImageType* mvComplexImage = mvFFTFilter->GetOutput();

    OutputIterator fxOutIter(fxComplexImage, fxComplexImage->GetLargestPossibleRegion());
    OutputIterator mvOutIter(mvComplexImage, mvComplexImage->GetLargestPossibleRegion());

    /*  ------ Multiply FFT of Fixed by Complex Conjugate of Moving ----------- */
    FFTPixelType real, imag;
    ComplexPixelType A, B;
    FFTPixelType magn = imageSize[0] * imageSize[1];
    ComplexImageType::IndexType fxIndex;
    ComplexImageType::IndexType mvIndex;
    while (!fxOutIter.IsAtEnd() && !mvOutIter.IsAtEnd() )
    {
      // determine the index of the output pixel
      fxIndex = fxOutIter.GetIndex();
      mvIndex = mvOutIter.GetIndex();
      // compute the phase correlation
      A = fxComplexImage->GetPixel(fxIndex);
      B = mvComplexImage->GetPixel(mvIndex);
      real = A.real() * B.real() + A.imag() * B.imag();
      imag = A.imag() * B.real() - A.real() * B.imag();
      fxOutIter.Set(OutputIterator::PixelType(real/magn, imag/magn));
      ++fxOutIter;
      ++mvOutIter;
    }

    /*  ------ Run an Inverse FFT on the result from the correlation operation --- */
    InverseFFTType::Pointer inverseFFTFilter =InverseFFTType::New();
    inverseFFTFilter->SetInput( fxComplexImage );
    inverseFFTFilter->Update();

    /* ----- Calculate some stats for the data ---------------------------------- */
    IFFTIteratorType iFFTiter(inverseFFTFilter->GetOutput(), inverseFFTFilter->GetOutput()->GetLargestPossibleRegion());
    FFTPixelType avg = 0;
    FFTPixelType max = std::numeric_limits<FFTPixelType>::min();
    FFTPixelType min = std::numeric_limits<FFTPixelType>::max();
    FFTPixelType sample;
    int samples = 0;
    while (!iFFTiter.IsAtEnd())
    {
        fxIndex = iFFTiter.GetIndex();
        sample = inverseFFTFilter->GetOutput()->GetPixel(fxIndex);
        if (sample < min) min = sample;
        if (sample > max) max = sample;
        ++samples;
        avg += sample;
        ++iFFTiter;
    }

    avg /= samples;
//    std::cout << imageSize[0] << "_" << this->m_CrossCorrelationData->getFixedSlice() << "_" << m_CrossCorrelationData->getMovingSlice()
//    << "min: " << min << "\tmax: " << max << "\tavg: " << avg << "\tDelta: " << (max-min) << "\tHalfRange: " << ((max-min)/2 + min) << "\tAvg-HalfRange: " << (avg - ((max-min)/2 + min)) << std::endl;

    RescaleFilterType::Pointer intensityRescaler = RescaleFilterType::New();
    intensityRescaler->SetOutputMinimum(  0  );
    intensityRescaler->SetOutputMaximum( 255 );
    intensityRescaler->SetInput( inverseFFTFilter->GetOutput() );

    /* --- Use the FFTShift Filter to put the "spot" where it should be -------- */
    FFTShiftImageFilterType::Pointer fftShift = FFTShiftImageFilterType::New();
    fftShift->SetInput(intensityRescaler->GetOutput());

    std::stringstream ss;
    ss << "/tmp/"  << this->m_CrossCorrelationData->getFixedSlice() << "_" << m_CrossCorrelationData->getMovingSlice() << "_" << imageSize[0]  << "_Correlation_Slice" << ".tif";

    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( fftShift->GetOutput() );
    writer->SetFileName(ss.str() );
    writer->Update();

    ss.str("");
    ss << "/tmp/"  << this->m_CrossCorrelationData->getFixedSlice() << "_" << m_CrossCorrelationData->getMovingSlice() << "_" << imageSize[0]  << "_Fixed" << ".tif";
    writer->SetInput(fxImport->GetOutput());
    writer->SetFileName(ss.str());
    writer->Update();

    ss.str("");
    ss << "/tmp/"  << this->m_CrossCorrelationData->getFixedSlice() << "_" << m_CrossCorrelationData->getMovingSlice() << "_" << imageSize[0]  << "_Moving" << ".tif";
    writer->SetInput(mvImport->GetOutput());
    writer->SetFileName(ss.str());
    writer->Update();

}
