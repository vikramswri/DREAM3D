///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////


#include "AngReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "AngConstants.h"

#define PI_OVER_2f       1.57079632679489661f
#define THREE_PI_OVER_2f 4.71238898038468985f
#define TWO_PIf          6.28318530717958647f
#define ONE_PIf          3.14159265358979323f

#define kBufferSize 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngReader::AngReader() :
m_UserOrigin(UpperRightOrigin), m_FileName(""), m_TEMpixPerum(0.0f), m_XStar(0.0f), m_YStar(0.0f),
m_ZStar(0.0f), m_WorkingDistance(0.0f), m_Phase(""),
m_MaterialName(""), m_Formula(""), m_Info(""), m_Symmetry(""), m_LatticeConstants(""),
m_NumberFamilies(""), m_HKLFamilies(""), m_Categories(""),
m_Grid(""), m_XStep(1.0f), m_YStep(1.0f), m_ZStep(1.0f), m_ZPos(0.0f), m_ZMax(0.0f),
m_NumOddCols(-1), m_NumEvenCols(-1),
m_NumRows(-1), m_OIMOperator(""), m_SampleID(""), m_ScanID(""),
m_ManageMemory(true), m_NumberOfElements(0)
{
  // Init all the arrays to NULL
  m_Phi1 = NULL;
  m_Phi = NULL;
  m_Phi2 = NULL;
  m_Iq = NULL;
  m_Ci = NULL;
  m_PhaseData = NULL;
  m_X = NULL;
  m_Y = NULL;
}

// -----------------------------------------------------------------------------
//  Clean up any Memory that was allocated for this class
// -----------------------------------------------------------------------------
AngReader::~AngReader()
{
  deletePointers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float* deleteArrayPointer(float* array)
{
  if (array != NULL )
  {
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
    _mm_free(array );
#else
    delete[] array;
#endif
    array = NULL;
  }
  return array;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngReader::initPointers()
{
  size_t numElements = m_NumRows * m_NumOddCols;
  size_t numBytes = numElements * sizeof(float);
  m_Phi1 = allocateArray<float > (numElements);
  m_Phi = allocateArray<float > (numElements);
  m_Phi2 = allocateArray<float > (numElements);
  m_Iq = allocateArray<float > (numElements);
  m_Ci = allocateArray<float > (numElements);
  m_PhaseData = allocateArray<float > (numElements);
  m_X = allocateArray<float > (numElements);
  m_Y = allocateArray<float > (numElements);

  ::memset(m_Phi1, 0, numBytes);
  ::memset(m_Phi, 0, numBytes);
  ::memset(m_Phi2, 0, numBytes);
  ::memset(m_Iq, 0, numBytes);
  ::memset(m_Ci, 0, numBytes);
  ::memset(m_PhaseData, 0, numBytes);
  ::memset(m_X, 0, numBytes);
  ::memset(m_Y, 0, numBytes);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngReader::deletePointers()
{
  this->deallocateArrayData<float > (m_Phi1);
  this->deallocateArrayData<float > (m_Phi);
  this->deallocateArrayData<float > (m_Phi2);
  this->deallocateArrayData<float > (m_Iq);
  this->deallocateArrayData<float > (m_Ci);
  this->deallocateArrayData<float > (m_PhaseData);
  this->deallocateArrayData<float > (m_X);
  this->deallocateArrayData<float > (m_Y);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngReader::readHeaderOnly()
{
  int err = 1;
  char buf[kBufferSize];
  bool headerComplete(false);
  std::ifstream in(m_FileName.c_str());

  if (!in.is_open())
  {
    std::cout << "Ang file could not be opened: '" << m_FileName << "'" << std::endl;
    return -100;
  }

  while (!in.eof() && !headerComplete)
  {
    in.getline(buf, kBufferSize);

    if (buf[0] != '#')
    {
      headerComplete = true;
    }
    else
    {
      this->parseHeaderLine(buf);
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AngReader::readFile()
{
  // std::cout << "  vtkAngReader::readFile" << std::endl;
  int err = 1;
  char buf[kBufferSize];
  bool headerComplete(false);
  std::ifstream in(m_FileName.c_str());

  if (!in.is_open())
  {
    std::cout << "Ang file could not be opened: " << m_FileName << std::endl;
    return -100;
  }

  while (!in.eof() && !headerComplete)
  {
    in.getline(buf, kBufferSize);

    if (buf[0] != '#')
    {
      headerComplete = true;
    }
    else
    {
      this->parseHeaderLine(buf);
    }
  }

  // Some headers do NOT list the number of rows and columns
  if (m_NumRows < 0 || m_NumEvenCols < 0 || m_NumOddCols < 0)
  {

  }


  // Initialize the vtkImageData objects and push into a vector
  deletePointers();  // In case this is used more than once
  initPointers();

  if (NULL == m_Phi1 || NULL == m_Phi || NULL == m_Phi2 || NULL == m_Iq || NULL == m_Ci || NULL == m_PhaseData || m_X == NULL || m_Y == NULL)
  {
    return -1;
  }

  //double progress = 0.0;
  int totalDataRows = m_NumRows * m_NumEvenCols;
  int counter = 0;
  while (in.eof() == false)
  {
    ++counter;
    this->readData(buf);
    // Read the next line of data
    in.getline(buf, kBufferSize);
  }
  if (counter != totalDataRows && in.eof() == true)
  {
    std::cout << "Premature End Of File reached while reading the .ang file.\n NumRows=" << m_NumRows << "m_NumOddCols=" << m_NumOddCols << " Counter="
        << counter << std::endl;
  }
  return err;
}

// -----------------------------------------------------------------------------
//  Read the Header part of the ANG file
// -----------------------------------------------------------------------------
void AngReader::parseHeaderLine(const std::string &line)
{
  std::istringstream in(std::string(line.begin() + 1, line.end()));

  std::string word;
  in >> word;

  std::string::iterator end = std::remove(word.begin(), word.end(), ':');
  word.resize(end - word.begin());
  std::transform(word.begin(), word.end(), word.begin(), ::tolower);
  std::string temp;
  if (ANG_TEM_PIXPERUM_LOWER == word)
  {
    in >> this->m_TEMpixPerum;
  }
  else if (ANG_X_STAR_LOWER == word)
  {
    in >> this->m_XStar;
  }
  else if (ANG_Y_STAR_LOWER == word)
  {
    in >> this->m_YStar;
  }
  else if (ANG_Z_STAR_LOWER == word)
  {
    in >> this->m_ZStar;
  }
  else if (ANG_WORKING_DISTANCE_LOWER == word)
  {
    in >> this->m_WorkingDistance;
  }
  else if (ANG_PHASE_LOWER == word)
  {
    in >> m_Phase;
  }
  else if (ANG_MATERIAL_NAME_LOWER == word)
  {
    in >> m_MaterialName;
  }
  else if (ANG_FORMULA_LOWER == word)
  {
    in >> m_Formula;
  }
  else if (ANG_INFO_LOWER == word)
  {
    in >> m_Info;
  }
  else if (ANG_SYMMETRY_LOWER == word)
  {
    in >> m_Symmetry;
  }
  else if (ANG_LATTICE_CONSTANTS_LOWER == word)
  {
    in >> m_LatticeConstants;
  }
  else if (ANG_NUMBER_FAMILIES_LOWER == word)
  {
    in >> m_NumberFamilies;
  }
  else if (ANG_HKL_FAMILIES_LOWER == word)
  {
    in >> temp;
    temp += in.str().substr(word.length() + 1);
    std::replace(temp.begin(), temp.end(), '\t', ' ');
    std::replace(temp.begin(), temp.end(), '\r', '\n');
    m_HKLFamilies = temp;
  }
  else if (ANG_CATEGORIES_LOWER == word)
  {
    in >> m_Categories;
  }
  else if (ANG_GRID_LOWER == word)
  {
    in >> m_Grid;
  }
  else if (ANG_X_STEP_LOWER == word)
  {
    in >> this->m_XStep;
  }
  else if (ANG_Y_STEP_LOWER == word)
  {
    in >> this->m_YStep;
  }
  else if (ANG_NCOLS_ODD_LOWER == word)
  {
    in >> this->m_NumOddCols;
  }
  else if (ANG_NCOLS_EVEN_LOWER == word)
  {
    in >> this->m_NumEvenCols;
  }
  else if (ANG_NROWS_LOWER == word)
  {
    in >> this->m_NumRows;
  }
  else if (ANG_OPERATOR_LOWER == word)
  {
    in >> m_OIMOperator;
  }
  else if (ANG_SAMPLE_ID_LOWER == word)
  {
    in >> m_SampleID;
  }
  else if (ANG_SCAN_ID_LOWER == word)
  {
    in >> m_ScanID;
  }
  else
  {
    in >> temp;
    if (word.compare("") != 0)
    {
      std::cout << "Unknown Header found:'" << word << "'" << std::endl;
    }
  }

}

// -----------------------------------------------------------------------------
//  Read the data part of the ANG file
// -----------------------------------------------------------------------------
void AngReader::readData(const std::string &line)
{
  std::istringstream in(line);
  float p1, p, p2, x, y, iqual, conf, ph, d1, d2;
  in >> p1;
  in >> p;
  in >> p2;
  in >> x;
  in >> y;
  in >> iqual;
  in >> conf;
  in >> ph;
  in >> d1 >> d2;

  float xMaxValue = static_cast<float > ((m_NumOddCols - 1) * m_XStep);
  float yMaxValue = static_cast<float > ((m_NumRows - 1) * m_YStep);

  // Do we transform the data
  if (m_UserOrigin == UpperRightOrigin)
  {
    x = xMaxValue - x;
    if (p1 - PI_OVER_2f < 0.0)
    {
      p1 = p1 + THREE_PI_OVER_2f;
    }
    else
    {
      p1 = p1 - PI_OVER_2f;
    }
  }
  else if (m_UserOrigin == UpperLeftOrigin)
  {
    if (p1 + PI_OVER_2f > TWO_PIf)
    {
      p1 = p1 - THREE_PI_OVER_2f;
    }
    else
    {
      p1 = p1 + PI_OVER_2f;
    }
    if (p + ONE_PIf > TWO_PIf)
    {
      p = p - ONE_PIf;
    }
    else
    {
      p = p + ONE_PIf;
    }
  }
  else if (m_UserOrigin == LowerLeftOrigin)
  {
    y = yMaxValue - y;
    if (p1 + PI_OVER_2f > TWO_PIf)
    {
      p1 = p1 - THREE_PI_OVER_2f;
    }
    else
    {
      p1 = p1 + PI_OVER_2f;
    }
  }
  else if (m_UserOrigin == LowerRightOrigin)
  {
    x = xMaxValue - x;
    y = yMaxValue - y;
  }
  size_t offset = static_cast<size_t > ((y / m_YStep) * m_NumOddCols + (x / m_XStep));

  m_Phi1[offset] = p1;
  m_Phi[offset] = p;
  m_Phi2[offset] = p2;
  m_Iq[offset] = iqual;
  m_Ci[offset] = conf;
  m_PhaseData[offset] = ph;
  m_X[offset] = x;
  m_Y[offset] = y;
}

