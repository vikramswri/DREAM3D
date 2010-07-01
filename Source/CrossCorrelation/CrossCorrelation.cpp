///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           F33615-03-C-5220
//
///////////////////////////////////////////////////////////////////////////////
//-- Our Includes
#include "CrossCorrelation.h"
//#include <R3D/Common/R3DSliceInfo.h>
//#include <R3D/Common/ZeissTileInfo.h>
//#include <R3D/Registration/CommandIterationUpdate.h>

//-- C++ Includes
#include <iostream>
#include <sstream>

// ITK Includes
#include <itkCastImageFilter.h>
#include <itkIndex.h>

//-- MXA Includes
#include "MXA/MXATypes.h"
#include "MXA/Utilities/MXALogger.h"
//#include "MXA/Utilities/StringUtils.h"

//-- Our Includes
#include "CrossCorrelation/ImageFilters/R3DCropGrayScaleImage.h"
#include "CrossCorrelation/pcm/itkPhaseCorrelationOperator.h"
#include "CrossCorrelation/pcm/itkMaxPhaseCorrelationOptimizer.h"
#include "itkFFTShiftImageFilter.h"





typedef itk::Image< pcm::PixelType, pcm::Dimension >                            ImageType;
typedef pcm::PhaseCorrelationOperator< ImageType, ImageType>                    OperatorType;
typedef pcm::PhaseCorrelationImageRegistrationMethod<ImageType, ImageType>      RegistrationType;
typedef pcm::MaxPhaseCorrelationOptimizer<RegistrationType>                     OptimizerType;
typedef itk::CastImageFilter<UCharImageType, FFTImageType>              CasterType;
typedef itk::CastImageFilter<FFTImageType, UCharImageType>              InverseCasterType;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelation::CrossCorrelation()
{
  _fftResolutions[0] = 256;
  _fftResolutions[1] = 512;
  _fftResolutions[2] = 1024;
  _fftResolutions[3] = 1500;
  _fftResolutions[4] = 2048;
  _fftResolutions[5] = 2500;
  _fftResolutions[6] = 3000;
  _fftResolutions[7] = 3456;
  _fftResolutions[8] = 4096;
  _fftResolutions[9] = 4500;
  _fftResolutions[10] = 5000;
  _fftResolutions[11] = 5625;
  _fftResolutions[12] = 6000;
  _fftResolutions[13] = 6480;
 // _fftResolutions[13] = 6912;
  _allowableError = 0.005f;  // 0.5% allowable error
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

// Cache the loaded mosaic for each run
  err = registerAtFFTResolution(_fftResolutions[0]);
  m_CrossCorrelationData->getTranslations(trans0);
//  std::cout << m_CrossCorrelationData->getFixedSlice() << "/" << m_CrossCorrelationData->getMovingSlice()
//      << "  " << _fftResolutions[0] << " Translation found: " << trans0[0] << ", " << trans0[1] << std::endl;

  float absDiff = R3D::PCM::PCMAbsolutePixelOffset * this->m_CrossCorrelationData->getScaling();
  //std::cout << "Largest Micron difference to be considered a match: " << absDiff << std::endl;

  for(uint32_t i = 1; i < R3D::PCM::FFTResolutionSize; ++i)
  {

    err = registerAtFFTResolution(_fftResolutions[i]);
    m_CrossCorrelationData->getTranslations(trans1);
//    std::cout << m_CrossCorrelationData->getFixedSlice() << "/" << m_CrossCorrelationData->getMovingSlice()
//        << "  " << _fftResolutions[i] << " Translation found: " << trans1[0] << ", " << trans1[1] << std::endl;
    diff[0] = trans0[0] - trans1[0];
    diff[1] = trans0[1] - trans1[1];
    if (diff[0] < 0.0f) { diff[0] *= -1.0f;}
    if (diff[1] < 0.0f) { diff[1] *= -1.0f;}

    if ( diff[0] < absDiff && diff[1] < absDiff) // 5 microns difference
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
    setErrorCondition(-1);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t CrossCorrelation::registerAtFFTResolution(int fftDim )
{
  // Load up the fixed image
  ImportFilterType::Pointer fxImport;
  fxImport = ImportFilterType::New();
  initializeImportFilter(fxImport, fftDim,
                         m_FixedImage,
                         true);

  // Load up the moving image
  ImportFilterType::Pointer mvImport;
  mvImport = ImportFilterType::New();
  initializeImportFilter(mvImport, fftDim,
                         m_MovingImage,
                         false);

  // Create a Correlation Image if wanted
//  createCorrelationImages(fxImport, mvImport);

  // Register the images using the FFT Correlation Method
  return registerImages(fxImport, mvImport);
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

  // Extract the ROI from the center of the image
  int32_t insets[4];

  insets[0] = (imageData->getImagePixelWidth() / 2) - (fftDim / 2);
  insets[1] = imageData->getImagePixelWidth() - (insets[0] + fftDim);
  insets[2] = (imageData->getImagePixelHeight() / 2) - (fftDim / 2);
  insets[3] = imageData->getImagePixelHeight() - (insets[2] + fftDim);

  R3DCropGrayScaleImage crop(imageData, insets);
  crop.run();
  AIMImage::Pointer croppedImage = crop.getOutputImage();
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
  importFilter->SetImportPointer(croppedImage->getImageBuffer(), size[0] * size[1], importImageFilterWillOwnTheBuffer);
  croppedImage->setManageMemory(false);
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
   typedef itk::ImageFileWriter< UCharImageType >                              WriterType;
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
    std::cout << imageSize[0] << "_" << this->m_CrossCorrelationData->getFixedSlice() << "_" << m_CrossCorrelationData->getMovingSlice()
    << "min: " << min << "\tmax: " << max << "\tavg: " << avg << "\tDelta: " << (max-min) << "\tHalfRange: " << ((max-min)/2 + min) << "\tAvg-HalfRange: " << (avg - ((max-min)/2 + min)) << std::endl;

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
