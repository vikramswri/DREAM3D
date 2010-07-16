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
#ifndef __itkPhaseCorrelationOperator_txx
#define __itkPhaseCorrelationOperator_txx

#include "itkPhaseCorrelationOperator.h"
#include "itkImageRegionIterator.h"
#include "itkObjectFactory.h"
#include "itkProgressReporter.h"
#include "itkMetaDataObject.h"

namespace pcm
{

/**
 *  Constructor
 */
template <  typename TFixedImage, typename TMovingImage >
PhaseCorrelationOperator<TFixedImage,  TMovingImage>
::PhaseCorrelationOperator()
{
  m_FullMatrix = false; //?! what should be the default?

  this->SetNumberOfRequiredInputs( 2 );
}

/**
 *
 */
template <  typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationOperator<TFixedImage,  TMovingImage>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}


template <  typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationOperator<TFixedImage,  TMovingImage>
::SetFixedImage( ComplexImageType * fixedImage )
{
  itkDebugMacro( "setting fixed image to " << fixedImage );
  SetNthInput(0, const_cast<ComplexImageType *>( fixedImage ));
}


template <  typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationOperator<TFixedImage,  TMovingImage>
::SetMovingImage( ComplexImageType * fixedImage )
{
  itkDebugMacro( "setting moving image to " << fixedImage );
  SetNthInput(1, const_cast<ComplexImageType *>( fixedImage ));
}


template <  typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationOperator<TFixedImage,  TMovingImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{
  itkDebugMacro( "initializing execution" );

  // Get the input and output pointers
  ImageConstPointer  fixed     = this->GetInput(0);
  ImageConstPointer  moving    = this->GetInput(1);
  ImagePointer       output    = this->GetOutput();

  //
  // Define a few indices that will be used to translate from an input pixel
  // to an output pixel to "resample the two volumes".
  typename ComplexImageType::IndexType
     movingStartIndex = moving->GetLargestPossibleRegion().GetIndex(),
     outputStartIndex = output->GetLargestPossibleRegion().GetIndex();
  typename ComplexImageType::SizeType
     fixedSize  = fixed->GetLargestPossibleRegion().GetSize(),
     movingSize = moving->GetLargestPossibleRegion().GetSize();
  typename ComplexImageType::IndexType fixedGapStart;
  typename ComplexImageType::IndexType movingGapStart;
  typename ComplexImageType::SizeType  fixedGapSize;
  typename ComplexImageType::SizeType  movingGapSize;
  unsigned int i;

  // crop every dimension to make the size of fixed and moving the same
  for (i=0; i<ComplexImageType::ImageDimension; i++)
    {
    fixedGapSize[i] = (fixedSize[i]-movingSize[i])>0 ?
                                            (fixedSize[i]-movingSize[i]) : 0;
    movingGapSize[i] = (movingSize[i]-fixedSize[i])>0 ?
                                            (movingSize[i]-fixedSize[i]) : 0;
    }

  i = 0; //reset the counter

  // for halved matrixes must be start for the first dimension treated differently
  if (!m_FullMatrix)
    {
    fixedGapStart[0] = fixedGapSize[0]>0 ? movingSize[0] : fixedSize[0];
    movingGapStart[0] = movingGapSize[0]>0 ? fixedSize[0] : movingSize[0];
    i++;
    }
  // all "normal" dimensions exclude central frequencies
  for (; i<ComplexImageType::ImageDimension; i++)
    {
    if ( fixedGapSize[i]>0 ) // fixed is too large
      {
      if ( (fixedSize[i]%2 + fixedGapSize[i]%2) > 1 )
        {
        fixedGapStart[i] = (unsigned long)( vcl_floor(fixedSize[i]/2.0) -
                                            vcl_floor(fixedGapSize[i]/2.0) );
        }
      else
        {
        fixedGapStart[i] = (unsigned long)( vcl_ceil(fixedSize[i]/2.0) -
                                            vcl_floor(fixedGapSize[i]/2.0) );
        }
      movingGapStart[i] = movingSize[i];
      }
    else if ( movingGapSize[i]>0 ) // moving is too large
      {
      if ( (movingSize[i]%2 + movingGapSize[i]%2) > 1 )
        {
        movingGapStart[i] = (unsigned long)( vcl_floor(movingSize[i]/2.0) -
                                             vcl_floor(movingGapSize[i]/2.0) );
        }
      else
        {
        movingGapStart[i] = (unsigned long)( vcl_ceil(movingSize[i]/2.0) -
                                             vcl_floor(movingGapSize[i]/2.0) );
        }
      fixedGapStart[i] = fixedSize[i];
      }
    else // fixed and moving is the same size
      {
      fixedGapStart[i] = fixedSize[i];
      movingGapStart[i] = movingSize[i];
      }
    }

  //
  // Define/declare an iterator that will walk the output region for this
  // thread.
  typedef  itk::ImageRegionIterator<ComplexImageType> OutputIterator;

  OutputIterator outIt(output, outputRegionForThread);
  typename ComplexImageType::IndexType fixedIndex;
  typename ComplexImageType::IndexType movingIndex;
  typename ComplexImageType::IndexType outputIndex;

  itkDebugMacro( "computing correlation surface" );
  // support progress methods/callbacks
  itk::ProgressReporter progress(this, threadId,
                            outputRegionForThread.GetNumberOfPixels());

  // walk the output region, and sample the input image
  while ( !outIt.IsAtEnd() )
    {
    // determine the index of the output pixel
    outputIndex = outIt.GetIndex();

    // determine the input pixels location associated with this output pixel
    for (unsigned int i = 0; i<ComplexImageType::ImageDimension; i++)
      {
      fixedIndex[i] = outputIndex[i]; //fixed and output images have same StartIndex
      movingIndex[i] = outputIndex[i]-outputStartIndex[i]+movingStartIndex[i];

      if (fixedIndex[i] >= fixedGapStart[i])
          fixedIndex[i] += fixedGapSize[i];
      if (movingIndex[i] >= movingGapStart[i])
          movingIndex[i] += movingGapSize[i];
      }

    // compute the phase correlation
    ComplexPixelType fixedVal = fixed->GetPixel(fixedIndex);
    ComplexPixelType movingVal = moving->GetPixel(movingIndex);

    outIt.Set( this->ComputeAtIndex(outputIndex,fixedVal,movingVal) );

    ++outIt;

    progress.CompletedPixel();
    }
}


template <  typename TFixedImage, typename TMovingImage >
//typename PhaseCorrelationOperator<TFixedImage,  TMovingImage>::ComplexPixelType
ComplexPixelType
PhaseCorrelationOperator<TFixedImage,  TMovingImage>
::ComputeAtIndex(typename ComplexImageType::IndexType & outputIndex,
                          ComplexPixelType          & fixedValue,
                          ComplexPixelType          & movingValue)
{
  PixelType real = fixedValue.real()*movingValue.real() +
                   fixedValue.imag()*movingValue.imag();
  PixelType imag = fixedValue.imag()*movingValue.real() -
                   fixedValue.real()*movingValue.imag();
  PixelType magn = 1.0; //vcl_sqrt( real*real + imag*imag );

  if (magn != 0 )
    {
    return ComplexPixelType(real/magn,imag/magn);
    }
  else
    {
    return ComplexPixelType( 0, 0);
    }
}


/**
 *  Request all available data. This filter is cropping from the center.
 */
template <  typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationOperator<TFixedImage,  TMovingImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the inputs
  ImagePointer fixed  = const_cast<ComplexImageType *> (this->GetInput(0));
  ImagePointer moving = const_cast<ComplexImageType *> (this->GetInput(1));

  if ( !fixed || !moving )
    {
    return;
    }

  fixed->SetRequestedRegion(  fixed->GetLargestPossibleRegion()  );
  moving->SetRequestedRegion( moving->GetLargestPossibleRegion() );
}

/**
 *  The output will have the lower size of the two input images in all
 *  dimensions.
 */
template <  typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationOperator<TFixedImage,  TMovingImage>
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // get pointers to the inputs and output
  ImageConstPointer fixed  = this->GetInput(0);
  ImageConstPointer moving = this->GetInput(1);
  ImagePointer      output = this->GetOutput();

  if ( !fixed || !moving || !output )
    {
    return;
    }

  itkDebugMacro( "adjusting size of output image" );
  // we need to compute the output spacing, the output image size, and the
  // output image start index
  unsigned int i;
  const typename ComplexImageType::SpacingType&
    fixedSpacing     =  fixed->GetSpacing(),
    movingSpacing    = moving->GetSpacing();
  const typename ComplexImageType::SizeType&
    fixedSize        =  fixed->GetLargestPossibleRegion().GetSize(),
    movingSize       = moving->GetLargestPossibleRegion().GetSize();
  const typename ComplexImageType::IndexType&
    fixedStartIndex  =  fixed->GetLargestPossibleRegion().GetIndex();

  typename ComplexImageType::SpacingType  outputSpacing;
  typename ComplexImageType::SizeType     outputSize;
  typename ComplexImageType::IndexType    outputStartIndex;

  for (i = 0; i < ComplexImageType::ImageDimension; i++)
    {
    outputSpacing[i]    = fixedSpacing[i] >= movingSpacing[i] ?
                                            fixedSpacing[i] : movingSpacing[i];
    outputSize[i]       = fixedSize[i] <= movingSize[i] ?
                                            fixedSize[i] : movingSize[i];
    outputStartIndex[i] = fixedStartIndex[i];
    }

  // additionally adjust the data size
  this->AdjustOutputInformation( outputSpacing, outputStartIndex, outputSize );

  output->SetSpacing( outputSpacing );

  typename ComplexImageType::RegionType outputLargestPossibleRegion;
  outputLargestPossibleRegion.SetSize( outputSize );
  outputLargestPossibleRegion.SetIndex( outputStartIndex );

  output->SetLargestPossibleRegion( outputLargestPossibleRegion );

  //
  // Pass the metadata with the actual size of the image.
  // The size must be adjusted according to the cropping and scaling
  // that will be made on the image!
  itkDebugMacro( "storing size of pre-FFT image in MetaData" );
  typedef typename ComplexImageType::SizeType::SizeValueType SizeScalarType;
  SizeScalarType fixedX = 0;
  SizeScalarType movingX = 0;
  SizeScalarType outputX = 0;
  itk::MetaDataDictionary &fixedDic =
      const_cast<itk::MetaDataDictionary &>(fixed->GetMetaDataDictionary());
  itk::MetaDataDictionary &movingDic =
      const_cast<itk::MetaDataDictionary &>(moving->GetMetaDataDictionary());
  itk::MetaDataDictionary &outputDic=
      const_cast<itk::MetaDataDictionary &>(output->GetMetaDataDictionary());

  if(itk::ExposeMetaData < SizeScalarType >
          (fixedDic,std::string("FFT_Actual_RealImage_Size"),fixedX)
     &&
     itk::ExposeMetaData < SizeScalarType >
          (movingDic,std::string("FFT_Actual_RealImage_Size"),movingX))
    {
    //SetFullMatrix( fixedSize[0] == fixedX );

    outputX = fixedX > movingX ? movingX : fixedX;

    itk::EncapsulateMetaData<SizeScalarType>(outputDic,
                                        std::string("FFT_Actual_RealImage_Size"),
                                        outputX);
    }
}


/**
 *  Generate everything.
 */
template <  typename TFixedImage, typename TMovingImage >
void
PhaseCorrelationOperator<TFixedImage,  TMovingImage>
::EnlargeOutputRequestedRegion(itk::DataObject *output)
{
  Superclass::EnlargeOutputRequestedRegion(output);
  output->SetRequestedRegionToLargestPossibleRegion();
}

} //end namespace itk

#endif
