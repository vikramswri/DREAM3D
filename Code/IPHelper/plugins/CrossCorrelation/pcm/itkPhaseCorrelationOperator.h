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
#ifndef __itkPhaseCorrelationOperator_h
#define __itkPhaseCorrelationOperator_h

#include "itkImageToImageFilter.h"
#include <complex>
#include "itkPhaseCorrelationImageRegistrationMethod.h"

namespace pcm
{

/** \class PhaseCorrelationOperator
 *  \brief Computes the spectrum ratio in phase correlation method.
 *
 *  The class is templated over the registration method type, in which it has to
 *  be plugged in.
 *
 *  The two input spectrums may have different size, while their real size is
 *  the same. To subsample them to same resolution, high frequencies must be
 *  excluded.
 *
 *  Then is computed freaquency ratio at every index of output correlation
 *  surface.
 *
 *  This class provides interface for further techniques to improve the
 *  registration performance. Method AdjustOutputInformation() enables for
 *  example to limit the computation only to low frequencies. Method
 *  ComputeAtIndex() computes a frequency ratio at single index of output image
 *  and overriding it may enable some special weighting or filtering.
 *
 * \author Jakub Bican, jakub.bican@matfyz.cz, Department of Image Processing,
 *         Institute of Information Theory and Automation,
 *         Academy of Sciences of the Czech Republic.
 *
 */


const unsigned int                                                  Dimension = 2;
typedef float                                                       PixelType;

typedef itk::NumericTraits<PixelType>::RealType                     InternalPixelType;
typedef std::complex<InternalPixelType>                             ComplexPixelType;
typedef itk::Image <ComplexPixelType, Dimension>                    ComplexImageType;
typedef itk::ImageToImageFilter<ComplexImageType, ComplexImageType>         PCM_SuperClass;


template <  typename TFixedImage, typename TMovingImage >
class ITK_EXPORT PhaseCorrelationOperator : public PCM_SuperClass
{


public:

 // typedef typename RegistrationMethod::InternalePixelType PT;

  //typedef TRegistrationMethod::InternalPixelType InternalPixelType;
  typedef PhaseCorrelationOperator                      Self;
  typedef PCM_SuperClass                                Superclass;
  typedef itk::SmartPointer<Self>                       Pointer;
  typedef itk::SmartPointer<const Self>                 ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PhaseCorrelationOperator, ImageToImageFilter);

#if 1
  /** ImageDimension enumeration. */
  itkStaticConstMacro(ImageDimension, unsigned int, Dimension);

  /** Typedef to images. */
 // typedef typename TRegistrationMethod::InternalPixelType PixelType;
 // typedef std::complex<PixelType>                       ComplexType;
  typedef itk::Image< ComplexPixelType, pcm::Dimension > ComplexImageType;

  typedef typename ComplexImageType::Pointer                   ImagePointer;
  typedef typename ComplexImageType::ConstPointer              ImageConstPointer;
  typedef typename Superclass::OutputImageRegionType       OutputImageRegionType;

  /** Connect the fixed image. */
  void SetFixedImage( ComplexImageType * );

  /** Connect the moving image. */
  void SetMovingImage( ComplexImageType * );

  /** Determines, whether the complex conjugate input (FFT transformed image)
   *  is a full matrix or if the first dimension is halved (N/2+1). */
//  itkSetMacro(FullMatrix, bool);
//  itkGetMacro(FullMatrix, bool);

  /** PhaseCorrelationOperator produces an image which is a different
   * resolution and with a different pixel spacing than its input
   * images. */
  virtual void GenerateOutputInformation();

  /** PhaseCorrelationOperator needs a larger input requested region than the
   *  output requested region. */
  virtual void GenerateInputRequestedRegion();
  virtual void EnlargeOutputRequestedRegion(itk::DataObject *output);

protected:
  PhaseCorrelationOperator();
  virtual ~PhaseCorrelationOperator() {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** PhaseCorrelationOperator can be implemented as a multithreaded filter.
   *  This method performs the computation. */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId );

  /** After the largest possible output data size is determined is this method
   *  called to additionally adjust the output parameters (reduce the size).
   *
   *  The method is called in GenerateOutputInformation() method, so the input
   *  spacing, index and size can be determined from the inputs 0 (fixed image)
   *  and 1 (moving image).
   *
   *  This method empty here and can be reimplemented by child filters.
   */
  virtual void AdjustOutputInformation(
                 typename ComplexImageType::SpacingType & spacing,
                 typename ComplexImageType::IndexType   & index,
                 typename ComplexImageType::SizeType    & size      ) {};

  /** Computes a phase correlation ratio for single frequency index.
   *
   *  This method is taken out from the computation in ThreadedGenerateData()
   *  to enable child filters to reimplement it in order to perform special
   *  computations at certain frequencies.
   *
   *  ?! This is still open problem, whether to ease the development of new
   *  operator filter and slow the computation by calling such method at every
   *  index, or whether to let the implementor of new operator reimplement entire
   *  ThreadedGenerateData() method (copying common parts and rewriting new parts).
   */
  virtual ComplexPixelType ComputeAtIndex(
                          typename ComplexImageType::IndexType & outputIndex,
                          ComplexPixelType          & fixedValue,
                          ComplexPixelType          & movingValue);

private:
  PhaseCorrelationOperator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool m_FullMatrix;

#endif

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPhaseCorrelationOperator.txx"
#endif

#endif
