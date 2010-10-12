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
 **************************************************************************** */

#ifndef __itkMaxPhaseCorrelationOptimizer_h
#define __itkMaxPhaseCorrelationOptimizer_h


#include "itkPhaseCorrelationOptimizer.h"
#include "itkMinimumMaximumImageCalculator.h"


namespace pcm
{

/** \class MaxPhaseCorrelationOptimizer
 *  \brief Implements basic shift estimation from position of maximum peak.
 *
 *  This class is templated over the type of registration method in which it has
 *  to be plugged in.
 *
 *  Operates on real correlation surface, so when set to the registration method,
 *  it should be get back by
 *  PhaseCorrelationImageRegistrationMethod::GetRealOptimizer() method.
 *
 *  The optimizer finds the maximum peak by MinimumMaximumImageCalculator and
 *  estimates the shift with pixel-level precision.
 *
 * \author Jakub Bican, jakub.bican@matfyz.cz, Department of Image Processing,
 *         Institute of Information Theory and Automation,
 *         Academy of Sciences of the Czech Republic.
 *
 */
template < typename TRegistrationMethod >
class ITK_EXPORT MaxPhaseCorrelationOptimizer :
  public PhaseCorrelationOptimizer<typename TRegistrationMethod::RealImageType>
{
public:
  typedef MaxPhaseCorrelationOptimizer  Self;
  typedef PhaseCorrelationOptimizer<
                      typename TRegistrationMethod::RealImageType>  Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MaxPhaseCorrelationOptimizer, PhaseCorrelationOptimizer);

  /**  Type of the input image. */
  typedef typename TRegistrationMethod::RealImageType   ImageType;
  typedef typename ImageType::ConstPointer              ImageConstPointer;

  /** Dimensionality of input and output data. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      itk::GetImageDimension<ImageType>::ImageDimension );

  /** Type for the output parameters.
  *  It defines a position in the optimization search space. */
  typedef typename Superclass::OffsetType               OffsetType;
  typedef typename Superclass::OffsetScalarType         OffsetScalarType;

  /** Method to return the latest modified time of this object or
  * any of its cached ivars */
  unsigned long GetMTime() const;

protected:
  MaxPhaseCorrelationOptimizer();
  virtual ~MaxPhaseCorrelationOptimizer() {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** This method is executed by superclass to execute the computation. */
  virtual void ComputeOffset();

  typedef itk::MinimumMaximumImageCalculator< ImageType >    MaxCalculatorType;

private:
  MaxPhaseCorrelationOptimizer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename MaxCalculatorType::Pointer                   m_MaxCalculator;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMaxPhaseCorrelationOptimizer.txx"
#endif

#endif
