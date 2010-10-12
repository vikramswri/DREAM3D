/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAngImageIO.cxx,v $
  Language:  C++
  Date:      $Date: 2009-10-27 16:05:53 $
  Version:   $Revision: 1.33 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkAngImageIO.h"
#include "itkExceptionObject.h"

#include "OIMColoring.hpp"

//#include "itkByteSwapper.h"
//#include "itkRGBPixel.h"
//#include "itkRGBAPixel.h"
//#include <itksys/SystemTools.hxx>
//#include <iostream>
//#include <list>
//#include <string>
//#include <math.h>

#include "AngReader.h"

namespace itk
{

/** Constructor */
AngImageIO::AngImageIO()
{
  this->SetNumberOfComponents(3);
  this->SetNumberOfDimensions(2);
  m_PixelType = SCALAR;
  m_ComponentType = UCHAR;
  m_Spacing[0] = 1.0;
  m_Spacing[1] = 1.0;

  m_Origin[0] = 0.0;
  m_Origin[1] = 0.0;


 // this->AddSupportedWriteExtension(".ang");

  this->AddSupportedReadExtension(".ang");
  this->AddSupportedReadExtension(".ANG");

}


/** Destructor */
AngImageIO::~AngImageIO()
{
}


bool AngImageIO::CanReadFile( const char* filename )
{
  // First check the filename extension
  std::string fname = filename;
  if ( fname == "" )
    {
    itkDebugMacro(<< "No filename specified.");
    }

  bool extensionFound = false;
  std::string::size_type ANGPos = fname.rfind(".ang");
  if ((ANGPos != std::string::npos)
      && (ANGPos == fname.length() - 4))
    {
    extensionFound = true;
    }

  ANGPos = fname.rfind(".ANG");
  if ((ANGPos != std::string::npos)
      && (ANGPos == fname.length() - 4))
    {
    extensionFound = true;
    }

  if( !extensionFound )
    {
    itkDebugMacro(<<"The filename extension is not recognized");
    return false;
    }

  return true;
}

bool AngImageIO::CanWriteFile( const char * name )
{
#if 0
  std::string filename = name;
  if ( filename == "" )
  {
    itkDebugMacro(<< "No filename specified.");
  }

  bool extensionFound = false;
  std::string::size_type ANGPos = filename.rfind(".bmp");
  if ((ANGPos != std::string::npos)
      && (ANGPos == filename.length() - 4))
  {
    extensionFound = true;
  }

  ANGPos = filename.rfind(".ANG");
  if ((ANGPos != std::string::npos)
      && (ANGPos == filename.length() - 4))
  {
    extensionFound = true;
  }

  if( !extensionFound )
  {
    itkDebugMacro(<<"The filename extension is not recognized");
    return false;
  }

  if( extensionFound )
  {
    return true;
  }
#endif

  return false;
}

void AngImageIO::Read(void* buffer)
{
  unsigned char* rgbImage = static_cast<unsigned char *>(buffer);

  m_Spacing[0] = 0.25;
  m_Spacing[1] = 0.25;

  m_Origin[0] = 0.0;
  m_Origin[1] = 0.0;

  AngReader reader;
  reader.setUserOrigin(AngReader::UpperLeftOrigin);
  reader.setFileName(GetFileName());
  int err = reader.readFile();
  if ( err < 0)
  {
    itkExceptionMacro( "itkAngImageIO could not read the Ang File Header " );
    return;
  }

  int xpoints = reader.getNumEvenCols();
  int ypoints = reader.getNumRows();
  //int zpoints = inputs->angEndSlice - inputs->angStartSlice + 1; // Add 1 because we are inclusive of the last file

  float* phi1F = reader.getPhi1Pointer();
  float* phiF = reader.getPhiPointer();
  float* phi2F = reader.getPhi2Pointer();

  float* xpos = reader.getXPosPointer();
  float* ypos = reader.getYPosPointer();


  this->SetSpacing(0, reader.getXStep());
  this->SetSpacing(1, reader.getYStep());

  this->SetOrigin(0, xpos[0]);
  this->SetOrigin(1, ypos[0]);

  std::cout << "Ang Spacing: " << m_Spacing[0] << ", " << m_Spacing[1] << std::endl;
  std::cout << "Ang Origin : " << m_Origin[0] << ", " << m_Origin[1] << std::endl;

  float refDir0 = 0.0f;
  float refDir1 = 0.0f;
  float refDir2 = 1.0f; // ND Normal Direction

  unsigned int rgba = 0x00000000;
  unsigned char* rgbPtr = reinterpret_cast<unsigned char* > (&rgba);

  size_t index = 0;

  for (int y = 0; y < ypoints; ++y)
  {
    for (int x = 0; x < xpoints; ++x)
    {
      rgba = 0;
      OIMColoring::GenerateIPFColor<float >(phi1F[index], phiF[index], phi2F[index], refDir0, refDir1, refDir2, rgbPtr);
      rgbImage[index * 3] = rgbPtr[0];
      rgbImage[index * 3 + 1] = rgbPtr[1];
      rgbImage[index * 3 + 2] = rgbPtr[2];
      ++index;
    }
  }

}

/**
 *  Read Information about the ANG file
 *  and put the cursor of the stream just before the first data pixel
 */
void AngImageIO::ReadImageInformation()
{
  AngReader reader;
  reader.setFileName(GetFileName());
  int err = reader.readHeaderOnly();
  if ( err < 0)
  {
    itkExceptionMacro( "itkAngImageIO could not read the Ang File Header " );
    return;
  }

  this->SetNumberOfDimensions(2);
  m_Dimensions[0] = reader.getNumEvenCols();
  m_Dimensions[1] = reader.getNumRows();

  this->SetSpacing(0, reader.getXStep());
  this->SetSpacing(1, reader.getYStep());
}


void AngImageIO::WriteImageInformation(void)
{

}

/** The write function is not implemented */
void AngImageIO::Write( const void* buffer)
{
  itkExceptionMacro(<< "AngImageIO cannot write native ANG files");
}

/** Print Self Method */
void AngImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Ang Image IO Class " << "\n";
}

} // end namespace itk
