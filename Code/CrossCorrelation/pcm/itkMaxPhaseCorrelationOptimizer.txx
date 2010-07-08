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
#ifndef __itkMaxPhaseCorrelationOptimizer_txx
#define __itkMaxPhaseCorrelationOptimizer_txx

#include "itkMaxPhaseCorrelationOptimizer.h"

namespace pcm
{

/*
 * Constructor
 */
template < typename TRegistrationMethod >
MaxPhaseCorrelationOptimizer<TRegistrationMethod>
::MaxPhaseCorrelationOptimizer() : Superclass()
{
  m_MaxCalculator = MaxCalculatorType::New();
}


/**
 *
 */
template < typename TRegistrationMethod >
void
MaxPhaseCorrelationOptimizer<TRegistrationMethod>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "MaxCalculator: " << m_MaxCalculator << std::endl;
}


/**
 *
 */
template < typename TRegistrationMethod >
void
MaxPhaseCorrelationOptimizer<TRegistrationMethod>
::ComputeOffset()
{
  ImageConstPointer input = static_cast< ImageType * >(this->GetInput(0));

  OffsetType offset;
  offset.Fill( 0 );

  if (!input)
    return;

  m_MaxCalculator->SetImage( input );

  try
    {
    m_MaxCalculator->ComputeMaximum();
    }
  catch( itk::ExceptionObject& err )
    {
    itkDebugMacro( "exception caught during execution of MaxCalculatior - passing " );
    throw err;
    }

  typename ImageType::IndexType
              index   = m_MaxCalculator->GetIndexOfMaximum();
  typename ImageType::SizeType
              size    = input->GetLargestPossibleRegion().GetSize();
  typename ImageType::SpacingType
              spacing = input->GetSpacing();

  for (int i = 0; i < ImageDimension ; i++)
    {
    if ( index[i] > vcl_floor( size[i] / 2.0 ) )
      {
      offset[i] = -1*(index[i] - size[i]) * spacing[i];
      }
    else
      {
      offset[i] = -1*index[i] * spacing[i];
      }
    }

  this->SetOffset( offset );
}

/**
 *
 */
template < typename TRegistrationMethod >
unsigned long
MaxPhaseCorrelationOptimizer<TRegistrationMethod>
::GetMTime() const
{
  unsigned long mtime = Superclass::GetMTime();
  unsigned long m;

  if (m_MaxCalculator)
    {
    m = m_MaxCalculator->GetMTime();
    mtime = (m > mtime ? m : mtime);
    }

  return mtime;
}

} //end namespace itk


#endif
