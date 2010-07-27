///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           F33615-03-C-5220
//
///////////////////////////////////////////////////////////////////////////////
#include "AIMImageFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImageFilter::AIMImageFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImageFilter::~AIMImageFilter()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageFilter::initializeImportFilter(ImportFilterType::Pointer importFilter,
                                                       PixelType* imageData,
                                                       int width, int height)
{
  ImportFilterType::SizeType size;
  size[0] = width; // size along X
  size[1] = height; // size along Y
  //size[2] = 1; // size along Z
  ImportFilterType::IndexType start;
  start.Fill(0);
  ImportFilterType::RegionType region;
  region.SetIndex(start);
  region.SetSize(size);
  importFilter->SetRegion(region);

  double origin[AIM::ImageFilters::Dimension];
  origin[0] = 0.0; // X coordinate
  origin[1] = 0.0; // Y coordinate
//  origin[2] = 0.0; // Z coordinate
  importFilter->SetOrigin(origin);

  double spacing[AIM::ImageFilters::Dimension];
  spacing[0] = 1.0; //  /xScale * 1000.0; // pixels per millimeter along X direction
  spacing[1] = 1.0; //  /yScale * 1000.0; // pixels per millimeter along Y direction
 // spacing[2] = 1.0; // along Z direction
  importFilter->SetSpacing(spacing);

  const bool importImageFilterWillOwnTheBuffer = false;
  importFilter->SetImportPointer(imageData, size[0] * size[1], importImageFilterWillOwnTheBuffer);
  importFilter->Update();
}

