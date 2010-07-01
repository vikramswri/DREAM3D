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
#ifndef __itkPhaseCorrelationOptimizer_h
#define __itkPhaseCorrelationOptimizer_h


#include "itkImage.h"
#include "itkProcessObject.h"
#include "itkSimpleDataObjectDecorator.h"


namespace pcm
{

/** \class PhaseCorrelationOptimizer
 *  \brief Defines common interface for optimizers, that estimates the shift
 *         from correlation surface.
 *
 *  The class is templated over the input image type, as some optimizers operate
 *  real correlation surface and some on complex correlation surface.
 *
 *  This class implements input and output handling, while the computation has
 *  to be performed by ComputeOffset() method, that must be overriden in childs.
 *
 * \author Jakub Bican, jakub.bican@matfyz.cz, Department of Image Processing,
 *         Institute of Information Theory and Automation,
 *         Academy of Sciences of the Czech Republic.
 *
 */
template <typename TImage>
class ITK_EXPORT PhaseCorrelationOptimizer : public itk::ProcessObject
{
public:
  typedef PhaseCorrelationOptimizer  Self;
  typedef itk::ProcessObject  Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(PhaseCorrelationOptimizer, itk::ProcessObject);

  /**  Type of the input image. */
  typedef          TImage                             ImageType;
  typedef typename ImageType::ConstPointer            ImageConstPointer;

  /** Dimensionality of input and output data. */
  itkStaticConstMacro(ImageDimension, unsigned int, Dimension);

  /** Type for the output parameters.
   *  It defines a position in the optimization search space. */
  typedef typename ImageType::PointType               OffsetType;
  typedef typename OffsetType::ValueType              OffsetScalarType;

  /** Type for the output: Using Decorator pattern for enabling
  *  the offset to be passed in the data pipeline */
  typedef  itk::SimpleDataObjectDecorator< OffsetType >    OffsetOutputType;
  typedef typename OffsetOutputType::Pointer          OffsetOutputPointer;
  typedef typename OffsetOutputType::ConstPointer     OffsetOutputConstPointer;

  /** Smart Pointer type to a DataObject. */
  typedef typename itk::DataObject::Pointer                DataObjectPointer;

  /** Get the computed offset. */
  itkGetConstReferenceMacro( Offset, OffsetType );

  /** Sets the input image to the optimizer. */
  void SetInput( const ImageType * image );

  /** Returns the offset resulting from the registration process  */
  const OffsetOutputType * GetOutput() const;

  /** Make a DataObject of the correct type to be used as the specified
   *  output. */
  virtual DataObjectPointer MakeOutput(unsigned int idx);

protected:
  PhaseCorrelationOptimizer();
  virtual ~PhaseCorrelationOptimizer() {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Method invoked by the pipeline in order to trigger the computation of
   * the output values. */
  void  GenerateData ();


  /** This method is executed by this type and must be reimplemented by child
   *  filter to perform the computation.
   */
  virtual void ComputeOffset() = 0;

  /** Provides derived classes with the ability to set this private var */
  itkSetMacro( Offset, OffsetType );

private:
  PhaseCorrelationOptimizer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  OffsetType    m_Offset;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPhaseCorrelationOptimizer.txx"
#endif

#endif
