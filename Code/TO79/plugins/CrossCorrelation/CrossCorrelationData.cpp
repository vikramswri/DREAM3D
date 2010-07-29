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
#include "CrossCorrelationData.h"
#include <MXA/Common/MXAEndian.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationData::CrossCorrelationData() :
  m_FixedSlice(0),
  m_MovingSlice(0),
//  fixedImageNum(1),
//  movingImageNum(0.0f),
  m_CostFuncValue(0),
  m_NumIterations(0),
  m_XTrans(0.0),
  m_YTrans(0.0),
  m_XFixedOrigin(0.0),
  m_YFixedOrigin(0.0),
  m_XMovingOrigin(0.0),
  m_YMovingOrigin(0.0),
  m_Scaling(1.0),
  m_ImageWidth(0),
  m_ImageHeight(0),
 // imageColorChannels(0),
  m_Complete(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationData::~CrossCorrelationData()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::initValues()
{
  m_FixedSlice = 0;
  m_MovingSlice = 0;
//  fixedImageNum = 1;
//  movingImageNum = 0.0f;
  m_CostFuncValue = 0;
  m_NumIterations = 0;
  m_XTrans = 0.0;
  m_YTrans = 0.0;
  m_XFixedOrigin = 0.0;
  m_YFixedOrigin = 0.0;
  m_XMovingOrigin = 0.0;
  m_YMovingOrigin = 0.0;
  m_Scaling = 0;
  m_ImageWidth = 0;
  m_ImageHeight = 0;
//  imageColorChannels = 0;
  m_Complete = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t CrossCorrelationData::writeToFile(MXAFileWriter64 &writer)
{
  bool ok = false;
  ok = writer.writeValue<int>( &m_FixedSlice); if( !ok ) { return -1; }
  ok = writer.writeValue<int32_t>( &m_MovingSlice);if( !ok ) { return -1; }
  ok = writer.writeValue<float>( &m_CostFuncValue);if( !ok ) { return -1; }
  ok = writer.writeValue<uint32_t>( &m_NumIterations);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &m_XTrans);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &m_YTrans);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &m_XFixedOrigin);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &m_YFixedOrigin);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &m_XMovingOrigin);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &m_YMovingOrigin);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &m_Scaling);if( !ok ) { return -1; }
  ok = writer.writeValue<int32_t>( &m_ImageWidth);if( !ok ) { return -1; }
  ok = writer.writeValue<int32_t>( &m_ImageHeight);if( !ok ) { return -1; }
  ok = writer.writeValue<int32_t>( &m_Complete);if( !ok ) { return -1; }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t CrossCorrelationData::readFromFile(MXAFileReader64 &reader, bool swap)
{
  bool ok = false;
  ok = reader.readValue<int32_t>( m_FixedSlice); if( !ok ) { return -1; }
  ok = reader.readValue<int32_t>( m_MovingSlice);if( !ok ) { return -1; }
  ok = reader.readValue<float>( m_CostFuncValue);if( !ok ) { return -1; }
  ok = reader.readValue<uint32_t>( m_NumIterations);if( !ok ) { return -1; }
  ok = reader.readValue<double>( m_XTrans);if( !ok ) { return -1; }
  ok = reader.readValue<double>( m_YTrans);if( !ok ) { return -1; }
  ok = reader.readValue<double>( m_XFixedOrigin);if( !ok ) { return -1; }
  ok = reader.readValue<double>( m_YFixedOrigin);if( !ok ) { return -1; }
  ok = reader.readValue<double>( m_XMovingOrigin);if( !ok ) { return -1; }
  ok = reader.readValue<double>( m_YMovingOrigin);if( !ok ) { return -1; }
  ok = reader.readValue<double>( m_Scaling);if( !ok ) { return -1; }
  ok = reader.readValue<int32_t>( m_ImageWidth);if( !ok ) { return -1; }
  ok = reader.readValue<int32_t>( m_ImageHeight);if( !ok ) { return -1; }
  ok = reader.readValue<int32_t>( m_Complete);if( !ok ) { return -1; }

  if (swap)
  {
    MXA::Endian::reverseBytes<int32_t>( m_FixedSlice);
    MXA::Endian::reverseBytes<int32_t>( m_MovingSlice);
    MXA::Endian::reverseBytes<float>( m_CostFuncValue);
    MXA::Endian::reverseBytes<uint32_t>( m_NumIterations);
    MXA::Endian::reverseBytes<double>( m_XTrans);
    MXA::Endian::reverseBytes<double>( m_YTrans);
    MXA::Endian::reverseBytes<double>( m_XFixedOrigin);
    MXA::Endian::reverseBytes<double>( m_YFixedOrigin);
    MXA::Endian::reverseBytes<double>( m_XMovingOrigin);
    MXA::Endian::reverseBytes<double>( m_YMovingOrigin);
    MXA::Endian::reverseBytes<double>( m_Scaling);
    MXA::Endian::reverseBytes<int32_t>( m_ImageWidth);
    MXA::Endian::reverseBytes<int32_t>( m_ImageHeight);
    MXA::Endian::reverseBytes<int32_t>( m_Complete);
  }
  return 1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::print(std::ostream &out, const std::string &delimiter)
{

  out << m_FixedSlice << delimiter;
  out << m_MovingSlice << delimiter;
  out << m_CostFuncValue << delimiter;
  out << m_NumIterations << delimiter;
  out << m_XTrans << delimiter;
  out << m_YTrans << delimiter;
  out << m_XFixedOrigin << delimiter;
  out << m_YFixedOrigin << delimiter;
  out << m_XMovingOrigin << delimiter;
  out << m_YMovingOrigin << delimiter;
  out << m_Scaling << delimiter;
  out << m_ImageWidth << delimiter;
  out << m_ImageHeight << delimiter;
  out << m_Complete << delimiter;
  out << std::endl;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::PrintAsciiHeader(std::ostream &out, const std::string &delimiter)
{
  out << "Fixed Slice"<< delimiter << "Moving Slice"<< delimiter << "Image Metric"<< delimiter << "Num Iterations"
      << delimiter << "X Trans"<< delimiter << "Y Trans"<< delimiter << "X Fixed Origin"<< delimiter << "Y FixedOrigin"
      << delimiter << "X Moving Origin" << delimiter << "Y Moving Origin" << delimiter << "Scaling"
      << delimiter << "Image Width" << delimiter << "Image Height" << delimiter << "Complete" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::getTranslations(double trans[2])
{
  trans[0] = m_XTrans;
  trans[1] = m_YTrans;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::setTranslations(double trans[2])
{
  m_XTrans = trans[0];
  m_YTrans = trans[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::setFixedOrigin(float origin[2])
{
  m_XFixedOrigin = origin[0];
  m_YFixedOrigin = origin[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::getFixedOrigin(float origin[2])
{
  origin[0] = m_XFixedOrigin;
  origin[1] = m_YFixedOrigin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::setMovingOrigin(float origin[2])
{
  m_XMovingOrigin = static_cast<double>(origin[0]);
  m_YMovingOrigin = static_cast<double>(origin[1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::getMovingOrigin(float origin[2])
{
  origin[0] = static_cast<float>(m_XMovingOrigin);
  origin[1] = static_cast<float>(m_YMovingOrigin);
}

