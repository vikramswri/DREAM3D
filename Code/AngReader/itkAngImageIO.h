/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _itkAngReader_h
#define _itkAngReader_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <stdio.h>

#include <fstream>

#include "itkImageIOBase.h"
#include "itkRGBPixel.h"

namespace itk
{

/** \class AngImageIO
 *
 *  \brief Read ANGImage file format.
 *
 *  \ingroup IOFilters
 *
 */
class ITK_EXPORT AngImageIO : public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef AngImageIO              Self;
  typedef ImageIOBase             Superclass;
  typedef SmartPointer<Self>      Pointer;
  typedef RGBPixel<unsigned char> RGBPixelType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(AngImageIO, Superclass);

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void* buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can write the
   * file specified. */
  virtual bool CanWriteFile(const char*);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation();

  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void* buffer);


  AngImageIO();
  ~AngImageIO();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  AngImageIO(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

 // void SwapBytesIfNecessary(void* buffer, unsigned long numberOfPixels);

  /** This methods ensures that the endianess is respected */
//  void Write32BitsInteger(unsigned int   value);
//  void Write16BitsInteger(unsigned short value);
//
//  std::ifstream             m_Ifstream;
//  std::ofstream             m_Ofstream;
//  long                      m_BitMapOffset;
//  bool                      m_FileLowerLeft;
//  short                     m_Depth;
//  bool                      m_Allow8BitANG;
//  unsigned short            m_NumberOfColors;
//  unsigned int              m_ColorTableSize;
//  long                      m_ANGCompression;
//  unsigned long             m_ANGDataSize;
//  std::vector<RGBPixelType> m_ColorPalette;

};

} // end namespace itk

#endif // _itkAngReader_h
