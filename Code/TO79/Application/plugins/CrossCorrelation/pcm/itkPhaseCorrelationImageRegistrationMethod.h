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
#ifndef __itkPhaseCorrelationImageRegistrationMethod_h
#define __itkPhaseCorrelationImageRegistrationMethod_h

#include <CrossCorrelation/pcm/itkPhaseCorrelationOperator.h>
#include <CrossCorrelation/pcm/itkPhaseCorrelationOptimizer.h>


#include "itkImage.h"
#include "itkProcessObject.h"
#include <complex>
#include "itkConstantPadImageFilter.h"
#include "itkFFTRealToComplexConjugateImageFilter.h"
#include "itkFFTComplexConjugateToRealImageFilter.h"
#include "itkDataObjectDecorator.h"
#include "itkTranslationTransform.h"



namespace pcm
{

/** \class PhaseCorrelationImageRegistrationMethod
 *  \brief Base class for phase-correlation-based image registration.
 *
 *  Phase Correlation Method (PCM) estimates shift between the Fixed image and
 *  Moving image. See <em>C. D. Kuglin and D. C. Hines, The phase correlation
 *  image alignment method,  in Proc. Int. Conf. on Cybernetics and Society,
 *  pp. 163n165, IEEE, Sep. 1975</em> for method description.
 *
 *  The method consists of 4 (5) steps:
 *    0. Resample the images to same spacing and size.
 *    1. Compute FFT of the two images.
 *    2. Compute the ratio of the two spectrums.
 *    3. Compute the inverse FFT of the cross-power spectrum.
 *    4. Find the maximum peak in cross-power spectrum and estimate the shift.
 *
 *  Step 0. is not included in the method itself - it is a prerequisite of PCM.
 *  It is performed by padding the smaller image by zeros to obtain two images
 *  that has the same real size (in all dimensions). Then it is necessary to
 *  resample images to same spacing. Resampling is made by cropping high
 *  frequencies in step 2.
 *
 *  Step 1. is performed by this class too using FFT filters supplied by
 *  itk::FFTRealToComplexConjugateImageFilter::New() factory.
 *
 *  Step 2. is performed by generic PhaseCorrelationOperator supplied during
 *  run time. It has to crop the high frequencies to subsample the two images to
 *  the same resolution and compute the spectrum ratio. PhaseCorrelationOperator
 *  can be derived to implement some special filtering during this phase.
 *
 *  As some special techniques (e.g. to compute subpixel shifts) require complex
 *  correlation surface, while the others compute the shift from real
 *  correlation surface, step 3. is carried by this class only when necessary.
 *  The IFFT filter is created using
 *  itk::FFTComplexConjugateToRealImageFilter::New() factory.
 *
 *  Step 4. is performed by run time supplied PhaseCorrelationOptimizer. It has
 *  to determine the shift from the real or complex correlation surface.
 *
 *  First, plug in the operator, optimizer and the input images. The method
 *  is executed by calling Update() (or updating some downstream filter).
 *
 *  The output shift can be passed downstreams in the form of
 *  TranslationTransform or can be obtained as transform parameters vector. The
 *  transform can be directly used to resample the Moving image to match the
 *  Fixed image.
 *
 * \author Jakub Bican, jakub.bican@matfyz.cz, Department of Image Processing,
 *         Institute of Information Theory and Automation,
 *         Academy of Sciences of the Czech Republic.
 *
 */
template <typename TFixedImage, typename TMovingImage>
class ITK_EXPORT PhaseCorrelationImageRegistrationMethod : public itk::ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef PhaseCorrelationImageRegistrationMethod  Self;
  typedef itk::ProcessObject  Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PhaseCorrelationImageRegistrationMethod, itk::ProcessObject);

  /**  Type of the Fixed image. */
  typedef          TFixedImage                     FixedImageType;
  typedef typename FixedImageType::PixelType       FixedImagePixelType;
  typedef typename FixedImageType::ConstPointer    FixedImageConstPointer;

  /**  Type of the Moving image. */
  typedef          TMovingImage                    MovingImageType;
  typedef typename MovingImageType::ConstPointer   MovingImageConstPointer;

  /** Dimensionality of input and output data is assumed to be the same. */
  itkStaticConstMacro(ImageDimension, unsigned int, pcm::Dimension);

  /** Pixel type, that will be used by internal filters.
   *  It should be float for integral and float inputs and it should
   *  be double for double inputs */
  typedef typename itk::NumericTraits<FixedImagePixelType>::RealType InternalPixelType;

  /** Type of the image, that is passed between the internal components. */
  typedef typename itk::Image< InternalPixelType,
                               itkGetStaticConstMacro(ImageDimension) > RealImageType;

  /** Type of the image, that is passed between the internal components. */
  typedef typename itk::Image< std::complex< InternalPixelType >,
                               itkGetStaticConstMacro(ImageDimension) > ComplexConjugateImageType;

	/**  Type of the Operator */
	typedef typename pcm::PhaseCorrelationOperator<FixedImageType, MovingImageType>  OperatorType;
	typedef typename OperatorType::Pointer			     OperatorPointer;

	/**  Type of the Optimizer */
  typedef          pcm::PhaseCorrelationOptimizer< RealImageType >   RealOptimizerType;
  typedef typename RealOptimizerType::Pointer			              RealOptimizerPointer;

  typedef          pcm::PhaseCorrelationOptimizer< ComplexConjugateImageType >   ComplexOptimizerType;
  typedef typename ComplexOptimizerType::Pointer	                          ComplexOptimizerPointer;

	/**  Type for the transform. */
  typedef typename itk::TranslationTransform<
                           typename MovingImageType::PointType::ValueType,
                           itkGetStaticConstMacro(ImageDimension) >          TransformType;
	typedef typename TransformType::Pointer                                    TransformPointer;

  /** Type for the output transform parameters (the shift). */
  typedef typename TransformType::ParametersType   ParametersType;

  /** Type for the output: Using Decorator pattern for enabling
   *  the Transform to be passed in the data pipeline */
  typedef  itk::DataObjectDecorator< TransformType >       TransformOutputType;
  typedef typename TransformOutputType::Pointer            TransformOutputPointer;
  typedef typename TransformOutputType::ConstPointer       TransformOutputConstPointer;

  /** Smart Pointer type to a DataObject. */
  typedef typename itk::DataObject::Pointer DataObjectPointer;

  /** Set/Get the Fixed image. */
  void SetFixedImage( const FixedImageType * fixedImage );
  itkGetConstObjectMacro( FixedImage, FixedImageType );

  /** Set/Get the Moving image. */
  void SetMovingImage( const MovingImageType * movingImage );
  itkGetConstObjectMacro( MovingImage, MovingImageType );

  /** Passes ReleaseDataFlag to internal filters. */
  void SetReleaseDataFlag(bool flag);

  /** Passes ReleaseDataBeforeUpdateFlag to internal filters. */
  void SetReleaseDataBeforeUpdateFlag(const bool flag);

  /** Set/Get the Operator. */
  itkSetObjectMacro( Operator, OperatorType );
  itkGetObjectMacro( Operator, OperatorType );

  /** Set/Get the Optimizer. */
  virtual void SetOptimizer (RealOptimizerType *);
  virtual void SetOptimizer (ComplexOptimizerType *);
  itkGetObjectMacro( RealOptimizer,  RealOptimizerType );
  itkGetObjectMacro( ComplexOptimizer,  ComplexOptimizerType );

  /** Set/Get the Transfrom. */
  itkSetObjectMacro( Transform, TransformType );
  itkGetObjectMacro( Transform, TransformType );

  /** Get the last transformation parameters visited by
   * the optimizer. */
  itkGetConstReferenceMacro( LastTransformParameters, ParametersType );

  /** Get the correlation surface.
   *
   *  Use method appropriate to the type (real/complex) of optimizer. If the
   *  complex optimizer is used, the real correlation surface is not available
   *  or is not up-to-date.
   */
  virtual RealImageType * GetRealCorrelationSurface()
    {
    itkDebugMacro("returning RealCorrelationSurface address "
                  << this->m_IFFT->GetOutput() );
    if (m_IFFT.IsNotNull())
      return m_IFFT->GetOutput();
    else
      return 0;
    }
  virtual ComplexConjugateImageType * GetComplexCorrelationSurface()
    {
    itkDebugMacro("returning ComplexCorrelationSurface address "
                  << this->m_Operator->GetOutput() );
    if (m_Operator.IsNotNull())
      return m_Operator->GetOutput();
    else
      return 0;
    }

  /** Returns the transform resulting from the registration process  */
  const TransformOutputType* GetOutput() const;

  /** Make a DataObject of the correct type to be used as the specified
   * output. */
  virtual DataObjectPointer MakeOutput(unsigned int idx);

  /** Method to return the latest modified time of this object or
   * any of its cached ivars */
  unsigned long GetMTime() const;

#ifdef ITK_USE_CONCEPT_CHECKING
  itkStaticConstMacro(MovingImageDimension, unsigned int, itk::GetImageDimension<FixedImageType>::ImageDimension );
  /** Start concept checking */
  itkConceptMacro(SameDimensionCheck, (itk::Concept::SameDimension<ImageDimension, MovingImageDimension>));
#endif

protected:
  PhaseCorrelationImageRegistrationMethod();
  virtual ~PhaseCorrelationImageRegistrationMethod() {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Initialize by setting the interconnects between the components. */
  virtual void Initialize() throw (itk::ExceptionObject);

  /** Method that initiates the optimization process. */
  void StartOptimization(void) throw (itk::ExceptionObject);

  /** Method invoked by the pipeline in order to trigger the computation of
   * the registration. */
  void  GenerateData () throw (itk::ExceptionObject);

  /** Provides derived classes with the ability to set this private var */
  itkSetMacro( LastTransformParameters, ParametersType );

  /** Types for internal componets. */
  typedef itk::ConstantPadImageFilter< FixedImageType, RealImageType >
                                                   FixedPadderType;
  typedef itk::ConstantPadImageFilter< MovingImageType, RealImageType >
                                                   MovingPadderType;
  typedef itk::FFTRealToComplexConjugateImageFilter< InternalPixelType,
                                                     ImageDimension >
                                                   FFTFilterType;
  typedef itk::FFTComplexConjugateToRealImageFilter< InternalPixelType,
                                                     ImageDimension >
                                                   IFFTFilterType;

private:
  PhaseCorrelationImageRegistrationMethod(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  OperatorPointer                       m_Operator;
  RealOptimizerPointer                  m_RealOptimizer;
  ComplexOptimizerPointer               m_ComplexOptimizer;

  MovingImageConstPointer               m_MovingImage;
  FixedImageConstPointer                m_FixedImage;

  //ParametersType                        m_TransformParameters;
  TransformPointer                      m_Transform;
  ParametersType                        m_LastTransformParameters;

  typename FixedPadderType::Pointer     m_FixedPadder;
  typename MovingPadderType::Pointer    m_MovingPadder;
  typename FFTFilterType::Pointer       m_FixedFFT;
  typename FFTFilterType::Pointer       m_MovingFFT;
  typename IFFTFilterType::Pointer      m_IFFT;

};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPhaseCorrelationImageRegistrationMethod.txx"
#endif

#endif
