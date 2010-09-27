///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include "AngDirectoryPatterns.h"

#include <sstream>
#include <iomanip>

#include <MXA/Utilities/MXADir.h>
#include <MXA/Utilities/StringUtils.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDirectoryPatterns::Pointer AngDirectoryPatterns::New(const std::string &parentDirectory,
    const std::string &fileprefix,
    int32_t width)
{
  Pointer sharedPtr (new AngDirectoryPatterns);
  sharedPtr->setParentDirectory(parentDirectory);
  sharedPtr->setPrefix(fileprefix);
  sharedPtr->setExtension("ang");
  sharedPtr->setMaxSlice(width);
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDirectoryPatterns::AngDirectoryPatterns()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngDirectoryPatterns::~AngDirectoryPatterns()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AngDirectoryPatterns::generateFullPathAngFileName(int slice)
{
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');

  ss << m_ParentDirectory << MXADir::Separator << m_Prefix << std::setw(m_MaxSlice) << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension;
  return ss.str();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string AngDirectoryPatterns::generateAngFileName(int slice)
{
  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');

  ss << m_Prefix << std::setw(m_MaxSlice) << slice << m_Suffix << Ang::DirectoryPatterns::Dot << m_Extension;
  return ss.str();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngDirectoryPatterns::print(std::ostream &ostream)
{
  ostream << "Prefix: " << getPrefix() << std::endl;
  ostream << "Suffix: " << getSuffix() << std::endl;
  ostream << "Extension: " << getExtension() << std::endl;
  ostream << "Width: " << getMaxSlice() << std::endl;
}

