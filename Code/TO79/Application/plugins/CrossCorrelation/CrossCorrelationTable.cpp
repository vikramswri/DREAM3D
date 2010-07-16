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
#include "CrossCorrelationTable.h"
#include <MXA/Common/LogTime.h>
#include <MXA/Common/MXAEndian.h>
#include <MXA/Utilities/MXADir.h>
#include <MXA/Utilities/MXAFileInfo.h>
#include <fstream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationTable::CrossCorrelationTable()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationTable::~CrossCorrelationTable()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationTable::addCrossCorrelationData(int32_t sliceNum,
                                                      CrossCorrelationData::Pointer data)
{
  this->_map[sliceNum] = data;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationData::Pointer CrossCorrelationTable::getCrossCorrelationData(int32_t sliceNum)
{
  //std::cout << logTime() << "TileAlignmentTable::getSliceStitchData(" << sliceNum << ")" << " Map Size: " << this->_stitchSliceMap.size() << std::endl;
  return this->_map[sliceNum];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationTable::getMinMaxSlice(int &minSlice, int &maxSlice)
{
  uint32_t min = 0xFFFFFFFF;
  uint32_t max = 0x00000000;
  for (std::map<int32_t, CrossCorrelationData::Pointer>::iterator iter = this->_map.begin(); iter != this->_map.end(); ++iter )
  {
    if ( (uint32_t)((*iter).first) < min) { min = (*iter).first; }
    if ( (uint32_t)((*iter).first) > max) { max = (*iter).first; }
  }
  minSlice = min;
  maxSlice = max;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationTable::Pointer CrossCorrelationTable::ReadFromFile(const std::string &filename)
{

  CrossCorrelationTable::Pointer nullTable = CrossCorrelationTable::NullPointer();

  CrossCorrelationTable::Pointer table = CrossCorrelationTable::New();
  MXAFileReader64 reader(filename);
  bool ok = reader.initReader();
  if (!ok)
  {
    std::cout << logTime() << "Error initializing file for CrossCorrelationTable from file '" << filename << "'" << std::endl;
    return nullTable;
  }
  char endian[4];
  reader.readArray<char>(endian, 4ll);
  bool swap = false;
#if defined(MXA_BIG_ENDIAN)
  if ( ::memcmp(endian, MXA::Endian::BIGE, 4) != 0 )
#else
  if ( ::memcmp(endian, MXA::Endian::LITE, 4) != 0 )
#endif
  {
    swap = true;
  }

  int64_t filesize = (int64_t)(MXAFileInfo::fileSize(filename));
  while (reader.getFilePointer64() < filesize)
  {
    CrossCorrelationData::Pointer data = CrossCorrelationData::New();
    data->readFromFile(reader, swap);
    table->addCrossCorrelationData(data->getFixedSlice(), data);
  }

  return table;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t CrossCorrelationTable::writeToFile(const std::string &filename)
{
  // std::cout << logTime() << "CrossCorrelationTable::writeToFile()" << std::endl;
  if (filename.empty() == true)
  {
    return -2;
  }
  int32_t err = 1;
  bool ok = false;
  MXAFileWriter64 writer(filename);
  ok = writer.initWriter();
  if (!ok)
  {
    std::cout << logTime() << "Error Writing TileAlignmentTable to file '" << filename << "'" << std::endl;
    return -1;
  }
#if defined(MXA_BIG_ENDIAN)
  writer.write((char*)(MXA::Endian::BIGE), sizeof(uint32_t));
#else
  writer.write((char*)(MXA::Endian::LITE), sizeof(uint32_t));
#endif
  CrossCorrelationData::Pointer data;
  for (RegistrationMap::iterator iter = this->_map.begin(); iter != this->_map.end(); ++iter )
  {
    data = (*iter).second;
    if (data.data() != NULL) {
     err = data->writeToFile(writer);
     data->PrintAsciiHeader(std::cout, "\t");
     data->print(std::cout, "\t");
    }
  }
 // std::cout << logTime() << "TileAlignmentTable::writeToFile() - Done Writing" << std::endl;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t CrossCorrelationTable::writeToAsciiFile(const std::string &filename, const std::string &delimiter)
{
  std::ofstream csvFile(filename.c_str(), std::ios::out);
  if (csvFile.is_open() == false)
  {
    std::cout << logTime() << "Error - Could not open output file" << std::endl;
    return -1;
  }
  this->printTable(csvFile, delimiter);
  csvFile.close();
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationTable::printTable(std::ostream &out, const std::string &delimiter)
{
  CrossCorrelationData::PrintAsciiHeader(out, delimiter);
  CrossCorrelationData::Pointer data;
  for (RegistrationMap::iterator iter = this->_map.begin(); iter != this->_map.end(); ++iter )
  {
    data = (*iter).second;
    if (data.data() != NULL) {
      data->print(out, delimiter);
    }
  }
}


