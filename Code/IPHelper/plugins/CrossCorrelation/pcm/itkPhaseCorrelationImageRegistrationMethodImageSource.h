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
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"

/**
 *  This class generates two circles (diameter RegionSize/2).
 *  One is shifted by 7,3 pixels from the other.
 *
 *  These two images are provided to the phase correlation image registration
 *  methods as uniform test data. This should be preffered to original
 *  ImageRegistrationMethodImageSource, because images with edges are more
 *  suitable for phase correlation methods.
 *
 *  Therefore the solution of the registration is |-7 -3|
 *
 * \author Jakub Bican, jakub.bican@matfyz.cz, Department of Image Processing, 
 *         Institute of Information Theory and Automation, 
 *         Academy of Sciences of the Czech Republic.
 *
 */
namespace itk
{

namespace testhelper
{

template< typename TFixedPixelType,
          typename TMovingPixelType,
          unsigned int NDimension     >
class PhaseCorrelationImageRegistrationMethodImageSource : public itk::Object
{
public:

  typedef PhaseCorrelationImageRegistrationMethodImageSource    Self;
  typedef Object                                Superclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;
  typedef Array<double>                         ParametersType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(Image, Object);


  typedef itk::Image<TMovingPixelType,NDimension> MovingImageType;
  typedef itk::Image<TFixedPixelType,NDimension > FixedImageType;



const MovingImageType * GetMovingImage(void) const
  {
  return m_MovingImage.GetPointer();
  }

const FixedImageType * GetFixedImage(void) const
  {
  return m_FixedImage.GetPointer();
  }

const ParametersType & GetActualParameters(void) const
{
  return m_Parameters;
}


void GenerateImages( const typename MovingImageType::SizeType & size )
{
  typename MovingImageType::IndexType index;
  index.Fill(0);
  typename MovingImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( index );

  m_MovingImage->SetLargestPossibleRegion( region );
  m_MovingImage->SetBufferedRegion( region );
  m_MovingImage->SetRequestedRegion( region );
  m_MovingImage->Allocate();

  m_FixedImage->SetLargestPossibleRegion( region );
  m_FixedImage->SetBufferedRegion( region );
  m_FixedImage->SetRequestedRegion( region );
  m_FixedImage->Allocate();

  /* Fill images with a 2D gaussian*/
  typedef  itk::ImageRegionIteratorWithIndex<MovingImageType>
      MovingImageIteratorType;

  typedef  itk::ImageRegionIteratorWithIndex<FixedImageType>
      FixedImageIteratorType;


  itk::Point<double,2> center;
  center[0] = (double)region.GetSize()[0]/2.0;
  center[1] = (double)region.GetSize()[1]/2.0;

  const double s = (double)region.GetSize()[0]/2.0;

  itk::Point<double,2>  p;
  itk::Vector<double,2> d;

  /* Set the displacement */
  itk::Vector<double,2> displacement;
  displacement[0] = m_Parameters[0];
  displacement[1] = m_Parameters[1];


  MovingImageIteratorType ri(m_MovingImage,region);
  FixedImageIteratorType ti(m_FixedImage,region);
  while(!ri.IsAtEnd())
  {
    p[0] = ri.GetIndex()[0];
    p[1] = ri.GetIndex()[1];
    d = p-center;
    d += displacement;
    const double x = d[0];
    const double y = d[1];
    const unsigned char value = sqrt(x*x+y*y)>s ? 0 : 1;
    ri.Set( static_cast<typename MovingImageType::PixelType>(value) );
    ++ri;
  }


  while(!ti.IsAtEnd())
  {
    p[0] = ti.GetIndex()[0];
    p[1] = ti.GetIndex()[1];
    d = p-center;
    const double x = d[0];
    const double y = d[1];
    const double value = sqrt(x*x+y*y)>s ? 0 : 1;
    ti.Set( static_cast<typename FixedImageType::PixelType>(value) );
    ++ti;
  }


}

protected:

PhaseCorrelationImageRegistrationMethodImageSource()
{
  m_MovingImage = MovingImageType::New();
  m_FixedImage  = FixedImageType::New();
  m_Parameters  = ParametersType(2);
  m_Parameters[0] = 7.0;
  m_Parameters[1] = 3.0;
}


private:

  typename FixedImageType::Pointer     m_FixedImage;
  typename MovingImageType::Pointer    m_MovingImage;

  ParametersType                       m_Parameters;

};



}  // end namespace testhelper

}  // end namespace itk

