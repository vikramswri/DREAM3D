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
  fixedSlice(0),
  movingSlice(0),
//  fixedImageNum(1),
//  movingImageNum(0.0f),
  costFuncValue(0),
  numIterations(0),
  xTrans(0.0),
  yTrans(0.0),
  xFixedOrigin(0.0),
  yFixedOrigin(0.0),
  xMovingOrigin(0.0),
  yMovingOrigin(0.0),
  scaling(1.0),
  imageWidth(0),
  imageHeight(0),
 // imageColorChannels(0),
  complete(0)
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
  fixedSlice = 0;
  movingSlice = 0;
//  fixedImageNum = 1;
//  movingImageNum = 0.0f;
  costFuncValue = 0;
  numIterations = 0;
  xTrans = 0.0;
  yTrans = 0.0;
  xFixedOrigin = 0.0;
  yFixedOrigin = 0.0;
  xMovingOrigin = 0.0;
  yMovingOrigin = 0.0;
  scaling = 0;
  imageWidth = 0;
  imageHeight = 0;
//  imageColorChannels = 0;
  complete = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t CrossCorrelationData::writeToFile(MXAFileWriter64 &writer)
{
  bool ok = false;
  ok = writer.writeValue<int>( &fixedSlice); if( !ok ) { return -1; }
  ok = writer.writeValue<int32_t>( &movingSlice);if( !ok ) { return -1; }
  ok = writer.writeValue<float>( &costFuncValue);if( !ok ) { return -1; }
  ok = writer.writeValue<uint32_t>( &numIterations);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &xTrans);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &yTrans);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &xFixedOrigin);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &yFixedOrigin);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &xMovingOrigin);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &yMovingOrigin);if( !ok ) { return -1; }
  ok = writer.writeValue<double>( &scaling);if( !ok ) { return -1; }
  ok = writer.writeValue<int32_t>( &imageWidth);if( !ok ) { return -1; }
  ok = writer.writeValue<int32_t>( &imageHeight);if( !ok ) { return -1; }
  ok = writer.writeValue<int32_t>( &complete);if( !ok ) { return -1; }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t CrossCorrelationData::readFromFile(MXAFileReader64 &reader, bool swap)
{
  bool ok = false;
  ok = reader.readValue<int32_t>( fixedSlice); if( !ok ) { return -1; }
  ok = reader.readValue<int32_t>( movingSlice);if( !ok ) { return -1; }
  ok = reader.readValue<float>( costFuncValue);if( !ok ) { return -1; }
  ok = reader.readValue<uint32_t>( numIterations);if( !ok ) { return -1; }
  ok = reader.readValue<double>( xTrans);if( !ok ) { return -1; }
  ok = reader.readValue<double>( yTrans);if( !ok ) { return -1; }
  ok = reader.readValue<double>( xFixedOrigin);if( !ok ) { return -1; }
  ok = reader.readValue<double>( yFixedOrigin);if( !ok ) { return -1; }
  ok = reader.readValue<double>( xMovingOrigin);if( !ok ) { return -1; }
  ok = reader.readValue<double>( yMovingOrigin);if( !ok ) { return -1; }
  ok = reader.readValue<double>( scaling);if( !ok ) { return -1; }
  ok = reader.readValue<int32_t>( imageWidth);if( !ok ) { return -1; }
  ok = reader.readValue<int32_t>( imageHeight);if( !ok ) { return -1; }
  ok = reader.readValue<int32_t>( complete);if( !ok ) { return -1; }

  if (swap)
  {
    MXA::Endian::reverseBytes<int32_t>( fixedSlice);
    MXA::Endian::reverseBytes<int32_t>( movingSlice);
    MXA::Endian::reverseBytes<float>( costFuncValue);
    MXA::Endian::reverseBytes<uint32_t>( numIterations);
    MXA::Endian::reverseBytes<double>( xTrans);
    MXA::Endian::reverseBytes<double>( yTrans);
    MXA::Endian::reverseBytes<double>( xFixedOrigin);
    MXA::Endian::reverseBytes<double>( yFixedOrigin);
    MXA::Endian::reverseBytes<double>( xMovingOrigin);
    MXA::Endian::reverseBytes<double>( yMovingOrigin);
    MXA::Endian::reverseBytes<double>( scaling);
    MXA::Endian::reverseBytes<int32_t>( imageWidth);
    MXA::Endian::reverseBytes<int32_t>( imageHeight);
    MXA::Endian::reverseBytes<int32_t>( complete);
  }
  return 1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::print(std::ostream &out, const std::string &delimiter)
{

  out << fixedSlice << delimiter;
  out << movingSlice << delimiter;
  out << costFuncValue << delimiter;
  out << numIterations << delimiter;
  out << xTrans << delimiter;
  out << yTrans << delimiter;
  out << xFixedOrigin << delimiter;
  out << yFixedOrigin << delimiter;
  out << xMovingOrigin << delimiter;
  out << yMovingOrigin << delimiter;
  out << scaling << delimiter;
  out << imageWidth << delimiter;
  out << imageHeight << delimiter;
  out << complete << delimiter;
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
  trans[0] = xTrans;
  trans[1] = yTrans;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::setTranslations(double trans[2])
{
  xTrans = trans[0];
  yTrans = trans[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::setFixedOrigin(float origin[2])
{
  xFixedOrigin = origin[0];
  yFixedOrigin = origin[1];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::getFixedOrigin(float origin[2])
{
  origin[0] = xFixedOrigin;
  origin[1] = yFixedOrigin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::setMovingOrigin(float origin[2])
{
  xMovingOrigin = static_cast<double>(origin[0]);
  yMovingOrigin = static_cast<double>(origin[1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationData::getMovingOrigin(float origin[2])
{
  origin[0] = static_cast<float>(xMovingOrigin);
  origin[1] = static_cast<float>(yMovingOrigin);
}

