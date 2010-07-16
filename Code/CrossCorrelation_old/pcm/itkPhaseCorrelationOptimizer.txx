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
#ifndef __itkPhaseCorrelationOptimizer_txx
#define __itkPhaseCorrelationOptimizer_txx

#include "itkPhaseCorrelationOptimizer.h"

namespace pcm
{

/*
 * Constructor
 */
template < typename TImage >
PhaseCorrelationOptimizer<TImage>::PhaseCorrelationOptimizer()
{
  this->SetNumberOfRequiredInputs( 1 );
  this->SetNumberOfRequiredOutputs( 1 );  // for the parameters

  m_Offset.Fill( 0 );

  OffsetOutputPointer offsetDecorator =
      static_cast< OffsetOutputType * >( this->MakeOutput(0).GetPointer() );
  this->ProcessObject::SetNthOutput( 0, offsetDecorator.GetPointer() );
  itkDebugMacro( "output is " << this->GetOutput()->Get() );
}


/**
 *
 */
template < typename TImage >
void
PhaseCorrelationOptimizer<TImage>::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Offset: " << m_Offset << std::endl;
}


/**
 *
 */
template < typename TImage >
void
PhaseCorrelationOptimizer<TImage>::GenerateData()
{
  if (!m_Updating)
    {
    this->Update();
    }
  else
    {
    OffsetType empty;
    empty.Fill( 0 );
    try
      {
      this->ComputeOffset();
      }
    catch( itk::ExceptionObject& err )
      {
      itkDebugMacro( "exception called while computing offset - passing" );

      m_Offset = empty;

      // pass exception to caller
      throw err;
      }
    }

  // write the result to the output
  OffsetOutputType * output = static_cast< OffsetOutputType * >( this->ProcessObject::GetOutput(0) );
  output->Set(m_Offset);
}


template < typename TImage >
void
PhaseCorrelationOptimizer<TImage>::SetInput( const ImageType * image )
{
  itkDebugMacro("setting input image to " << image );
  if ( this->GetInput(0) != image )
    {
    this->ProcessObject::SetNthInput(0, const_cast< ImageType * >( image ) );

    this->Modified();
    }
}

/*
 *  Get Output
 */
template < typename TImage >
const typename PhaseCorrelationOptimizer<TImage>::OffsetOutputType *
PhaseCorrelationOptimizer<TImage>::GetOutput() const
{
  return static_cast< const OffsetOutputType * >(
                                          this->ProcessObject::GetOutput(0) );
}

template < typename TImage >
itk::DataObject::Pointer  PhaseCorrelationOptimizer<TImage>::MakeOutput(unsigned int output)
{
  switch (output)
    {
    case 0:
      return static_cast<itk::DataObject*>(OffsetOutputType::New().GetPointer());
      break;
    default:
      itkExceptionMacro("MakeOutput request for an output number larger than the expected number of outputs");
      return 0;
    }
}

} //end namespace itk


#endif
