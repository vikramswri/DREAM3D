/* ****************************************************************************
 * Original Author:
 *         Jakub Bican, jakub.bican@matfyz.cz, Department of Image Processing,
 *         Institute of Information Theory and Automation,
 *         Academy of Sciences of the Czech Republic.
 *
 *  Changes to allow compilation under newer versions of ITK added by:
 *    Michael A. Jackson aka BlueQuartz Softare, Principle Software Engineer
 *    Dayton Ohio 2009.
 *
 *  This code is in the public domain by way of the ITK (www.itk.org) Insight
 *  Journal Submission and is therefor NOT covered by DoD Distribution D
 *  Statements.
 *  License:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 *    * Neither the name of the Insight Software Consortium nor the names of its
 *       contributors may be used to endorse or promote products derived from this
 *       software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _itkPhaseCorrelationImageRegistrationMethod_txx
#define _itkPhaseCorrelationImageRegistrationMethod_txx

#include "itkPhaseCorrelationImageRegistrationMethod.h"


#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"


namespace pcm
{

/*
 * Constructor
 */
template < typename TFixedImage, typename TMovingImage >
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::PhaseCorrelationImageRegistrationMethod()
{
  this->SetNumberOfRequiredInputs( 2 );
  this->SetNumberOfRequiredOutputs( 1 );  // for the Transform

  m_FixedImage       = 0; // has to be provided by the user.
  m_MovingImage      = 0; // has to be provided by the user.
  m_Operator         = 0; // has to be provided by the user.
  m_RealOptimizer    = 0; // has to be provided by the user.
  m_ComplexOptimizer = 0; // has to be provided by the user.
  m_Transform    = 0; // has to be provided by the user.

  m_FixedPadder  = FixedPadderType::New();
  m_MovingPadder = MovingPadderType::New();
  m_FixedFFT     = FFTFilterType::New();
  m_MovingFFT    = FFTFilterType::New();
  m_IFFT         = IFFTFilterType::New();

  m_FixedPadder->SetConstant( 0 );
  m_MovingPadder->SetConstant( 0 );

  m_FixedFFT->SetInput( m_FixedPadder->GetOutput() );
  m_MovingFFT->SetInput( m_MovingPadder->GetOutput() );

  m_LastTransformParameters = ParametersType(pcm::Dimension);
  m_LastTransformParameters.Fill( 0.0f );

  TransformOutputPointer transformDecorator =
                 static_cast< TransformOutputType * >(
                                  this->MakeOutput(0).GetPointer() );

  this->ProcessObject::SetNthOutput( 0, transformDecorator.GetPointer() );

  itkDebugMacro( "output is " << this->GetOutput()->Get() );
}


/**
 *
 */
template < typename TFixedImage, typename TMovingImage >
unsigned long
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::GetMTime() const
{
  unsigned long mtime = Superclass::GetMTime();
  unsigned long m;

  if (m_Operator)
    {
    m = m_Operator->GetMTime();
    mtime = (m > mtime ? m : mtime);
    }
  if (m_RealOptimizer)
    {
    m = m_RealOptimizer->GetMTime();
    mtime = (m > mtime ? m : mtime);
    }
  if (m_ComplexOptimizer)
    {
    m = m_ComplexOptimizer->GetMTime();
    mtime = (m > mtime ? m : mtime);
    }
  if (m_FixedImage)
    {
    m = m_FixedImage->GetMTime();
    mtime = (m > mtime ? m : mtime);
    }

  if (m_MovingImage)
    {
    m = m_MovingImage->GetMTime();
    mtime = (m > mtime ? m : mtime);
    }
  if (m_Transform)
    {
    m = m_Transform->GetMTime();
    mtime = (m > mtime ? m : mtime);
    }

  return mtime;
}


/*
 * Initialize by setting the interconnects between components.
 */
template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::Initialize() throw (itk::ExceptionObject)
{
  itkDebugMacro( "initializing registration" );

  if( !m_FixedImage )
    {
    itkExceptionMacro(<<"FixedImage is not present");
    }

  if( !m_MovingImage )
    {
    itkExceptionMacro(<<"MovingImage is not present");
    }

  if ( !m_Operator )
    {
    itkExceptionMacro(<<"Operator is not present" );
    }

  if ( !m_RealOptimizer && !m_ComplexOptimizer)
    {
    itkExceptionMacro(<<"Optimizer is not present" );
    }
  if( !m_Transform )
    {
    itkExceptionMacro(<<"Transform is not present");
    }


  //
  // Connect the transform to the Decorator.
  //
  TransformOutputType * transformOutput =
     static_cast< TransformOutputType * >( this->ProcessObject::GetOutput(0) );

  transformOutput->Set( m_Transform.GetPointer() );

  // Validate initial transform parameters
  if ( m_LastTransformParameters.Size() !=
       m_Transform->GetNumberOfParameters() )
    {
    itkExceptionMacro(<<"Size mismatch between m_LastTransformParameters parameters and transform." <<
      "Expected " << m_Transform->GetNumberOfParameters() << " parameters and received "
      <<  m_LastTransformParameters.Size() << " parameters");
    }

  //
  // set up the pipeline
  //
  m_FixedPadder->SetInput(        m_FixedImage             );
  m_MovingPadder->SetInput(       m_MovingImage            );
  m_Operator->SetFixedImage(      m_FixedFFT->GetOutput()  );
  m_Operator->SetMovingImage(     m_MovingFFT->GetOutput() );
  if ( m_RealOptimizer )
    {
    m_IFFT->SetInput(             m_Operator->GetOutput()  );
    m_RealOptimizer->SetInput(    m_IFFT->GetOutput()      );
    }
  else
    {
    m_ComplexOptimizer->SetInput( m_Operator->GetOutput()  );
    }

  //
  //set up padding to resize the images to cover the same real-space area
  //
  typename FixedImageType::SizeType fixedSize =
      m_FixedImage->GetLargestPossibleRegion().GetSize();
  typename FixedImageType::SpacingType fixedSpacing =
      m_FixedImage->GetSpacing();
  typename MovingImageType::SizeType movingSize =
      m_MovingImage->GetLargestPossibleRegion().GetSize();
  typename MovingImageType::SpacingType movingSpacing =
      m_MovingImage->GetSpacing();

  unsigned long fixedPad[ ImageDimension ];
  unsigned long movingPad[ ImageDimension ];

  for (int i=0; i<ImageDimension; i++)
    {
    if ( fixedSize[i]*fixedSpacing[i] > movingSize[i]*movingSpacing[i] )
      {
      movingPad[i] = (unsigned long)vcl_floor(
                                fixedSize[i]*fixedSpacing[i]/movingSpacing[i] -
                                movingSize[i] );
      fixedPad[i] = 0;
      }
    else
      {
      fixedPad[i] = (unsigned long)vcl_floor(
                               movingSize[i]*movingSpacing[i]/fixedSpacing[i] -
                               fixedSize[i] );
      movingPad[i] = 0;
      }
    }

  m_FixedPadder->SetPadUpperBound( fixedPad );
  m_MovingPadder->SetPadUpperBound( movingPad );

  // set up the operator
 // m_Operator->SetFullMatrix( m_FixedFFT->FullMatrix() );

}


/*
 * Starts the Optimization process
 */
template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>::StartOptimization( void ) throw (itk::ExceptionObject)
{
  itkDebugMacro( "starting optimization" );
  typedef typename RealImageType::PointType   OffsetType;
  OffsetType offset;
  try
    {
    if ( m_RealOptimizer )
      {
      m_RealOptimizer->Update();
      offset = m_RealOptimizer->GetOffset();
      }
    else
      {
      m_ComplexOptimizer->Update();
      offset = m_ComplexOptimizer->GetOffset();
      }
    }
  catch( itk::ExceptionObject& err )
    {
    // Pass exception to caller
    itkDebugMacro( "exception caught during optimization - passing" );
    throw err;
    }
  itkDebugMacro( "optimization finished" );

  /* *********************************************************************** */
#if 0
  typedef unsigned char                                WritePixelType;
  typedef itk::Image< WritePixelType, pcm::Dimension > WriteImageType;
  typedef typename IFFTFilterType::OutputImageType    ComplexImageType;

  typedef itk::ImageFileWriter< ComplexImageType > ComplexWriterType;

  typename ComplexWriterType::Pointer complexWriter = ComplexWriterType::New();

  complexWriter->SetFileName("/tmp/complexImage.vtk");

  complexWriter->SetInput( m_IFFT->GetOutput() );
  complexWriter->Update();
#endif
  /* *********************************************************************** */

  //
  // now offset is a real-coordinate shift between the two images
  // but with origin offset not included
  typename FixedImageType::PointType fixedOrigin = m_FixedImage->GetOrigin();
  typename MovingImageType::PointType movingOrigin = m_MovingImage->GetOrigin();
  for (int i = 0; i<ImageDimension; i++)
    {
    m_LastTransformParameters[i] = offset[i] - ( movingOrigin[i] - fixedOrigin[i] );
    }

  m_Transform->SetParameters( m_LastTransformParameters );

  itkDebugMacro( "output set to " << m_Transform );
}


/*
 * PrintSelf
 */
template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Operator: " << m_Operator.GetPointer() << std::endl;
  os << indent << "Real Optimizer: " << m_RealOptimizer.GetPointer() << std::endl;
  os << indent << "Complex Optimizer: " << m_ComplexOptimizer.GetPointer() << std::endl;
  os << indent << "Transform: " << m_Transform.GetPointer() << std::endl;
  os << indent << "Fixed Image: " << m_FixedImage.GetPointer() << std::endl;
  os << indent << "Moving Image: " << m_MovingImage.GetPointer() << std::endl;
  os << indent << "Transform Parameters: " << m_LastTransformParameters << std::endl;

  typename TransformType::ConstPointer t(this->GetOutput()->Get());
  os << indent << "Output transform: " << t.GetPointer() << std::endl;
}


/*
 * Generate Data
 */
template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::GenerateData() throw (itk::ExceptionObject)
{
  if (!m_Updating)
    {
    this->Update();
    }
  else
    {
    ParametersType empty(ImageDimension);
    empty.Fill( 0.0 );
    try
      {
      // initialize the interconnects between components
      this->Initialize();
      }
    catch( itk::ExceptionObject& err )
      {
      itkDebugMacro( "exception caught during intialization - passing" );

      m_LastTransformParameters = empty;

      // pass exception to caller
      throw err;
      }
    //execute the computation
    this->StartOptimization();
    }

}


/*
 *  Get Output
 */
template < typename TFixedImage, typename TMovingImage >
const typename PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
    ::TransformOutputType *
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::GetOutput() const
{
  return static_cast< const TransformOutputType * >(
                                            this->ProcessObject::GetOutput(0) );
}


template < typename TFixedImage, typename TMovingImage >
itk::DataObject::Pointer
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>::MakeOutput(unsigned int output)
{
  switch (output)
    {
    case 0:
      return static_cast<itk::DataObject*>(TransformOutputType::New().GetPointer());
      break;
    default:
      itkExceptionMacro("MakeOutput request for an output number larger than the expected number of outputs");
      return 0;
    }
}


template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::SetFixedImage( const FixedImageType * fixedImage )
{
  itkDebugMacro("setting Fixed Image to " << fixedImage );

  if (this->m_FixedImage.GetPointer() != fixedImage )
    {
    this->m_FixedImage = fixedImage;

    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(0,
                                  const_cast< FixedImageType *>( fixedImage ) );

    this->Modified();
    }
}


template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::SetMovingImage( const MovingImageType * movingImage )
{
  itkDebugMacro("setting Moving Image to " << movingImage );

  if (this->m_MovingImage.GetPointer() != movingImage )
    {
    this->m_MovingImage = movingImage;

    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(1,
                                const_cast< MovingImageType *>( movingImage ) );

    this->Modified();
    }
}


template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::SetReleaseDataFlag( bool a_flag )
{
  Superclass::SetReleaseDataFlag( a_flag );
  m_FixedPadder->SetReleaseDataFlag( a_flag );
  m_MovingPadder->SetReleaseDataFlag( a_flag );
  m_FixedFFT->SetReleaseDataFlag( a_flag );
  m_MovingFFT->SetReleaseDataFlag( a_flag );
  m_IFFT->SetReleaseDataFlag( a_flag );
}


template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::SetReleaseDataBeforeUpdateFlag( bool a_flag )
{
  Superclass::SetReleaseDataBeforeUpdateFlag( a_flag );
  m_FixedPadder->SetReleaseDataBeforeUpdateFlag( a_flag );
  m_MovingPadder->SetReleaseDataBeforeUpdateFlag( a_flag );
  m_FixedFFT->SetReleaseDataBeforeUpdateFlag( a_flag );
  m_MovingFFT->SetReleaseDataBeforeUpdateFlag( a_flag );
  m_IFFT->SetReleaseDataBeforeUpdateFlag( a_flag );
}


template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::SetOptimizer( RealOptimizerType * optimizer )
{
  itkDebugMacro("setting RealOptimizer to " << optimizer );
  if (this->m_RealOptimizer != optimizer)
    {
    this->m_RealOptimizer = optimizer;
    this->m_ComplexOptimizer = 0;
    this->Modified();
    }
}


template < typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationImageRegistrationMethod<TFixedImage,TMovingImage>
::SetOptimizer( ComplexOptimizerType * optimizer )
{
  itkDebugMacro("setting ComplexOptimizer to " << optimizer );
  if (this->m_ComplexOptimizer != optimizer)
    {
    this->m_ComplexOptimizer = optimizer;
    this->m_RealOptimizer = 0;
    this->Modified();
    }
}


} // end namespace itk

#endif
