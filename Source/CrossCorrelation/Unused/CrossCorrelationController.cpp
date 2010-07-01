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
#include "CrossCorrelation.h"
#include <R3D/Common/R3DMacros.h>
#include <R3D/Common/R3DConstants.h>
#include <R3D/Common/ZeissTagMapping.h>
#include <R3D/Common/R3DMosaicExportProperties.h>
#include <R3D/MosaicExporter/MosaicExporter.h>

//-- MXADataModel Includes
#include <MXA/Common/MXATypeDefs.h>
#include <MXA/Base/IDataModel.h>
#include <MXA/Base/IDataDimension.h>
#include <MXA/Base/IDataRecord.h>
#include <MXA/Base/IMXAArray.h>
#include <MXA/HDF5/H5MXADataFile.h>
#include <MXA/HDF5/H5MXAUtilities.h>
#include <MXA/HDF5/H5Lite.h>
#include <MXA/HDF5/H5Image.h>
#include <MXA/HDF5/H5TiffIO.h>
#include <MXA/Utilities/StringUtils.h>
#include <MXA/Utilities/MXADir.h>

//--C++ Includes
#include <iostream>
#include <sstream>
#include <limits>


//TODO: Use the DebugOutput so that GUIS will get the messages

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationController::CrossCorrelationController(const std::string &mxaFileName,
                                           const std::string &dataRecordName,
                                           const std::string &logFilename,
                                           int maxThreads, int sliceStart, int sliceEnd) :
_mxaFileName(mxaFileName),
_dataRecordName(dataRecordName),
_logFilename(logFilename),
_scaling(1.0),
_maxThreads(maxThreads),
_sliceStart(sliceStart),
_sliceEnd(sliceEnd),
_debug(false)
{
  _threadGroup = R3DThreadGroup::New(maxThreads);
  MXALOGGER_CLASS_VARIABLE_INSTANCE;
  mxa_log.open(logFilename);
  //mxa_log << logTime() << "-->CrossCorrelation Constructing" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationController::~CrossCorrelationController()
{
 // mxa_log << logTime() << "-->CrossCorrelation Destructing" << std::endl;
  mxa_log.close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationController::setDebug(bool dbg)
{
  this->_debug = dbg;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationController::run()
{
  if (_debug) mxa_log << logTime() << "Starting Threaded Phase Correlation Registration with "<< this->_maxThreads << " Threads." << std::endl;
  // MXADataModel Loading Code. ------------------------------------------------
  //----First load the Data file
   _dataFile = H5MXADataFile::OpenFile(_mxaFileName, false);

  if (NULL == _dataFile.get())
  {
    mxa_log << logTime() << "Error Opening MXA Data File: '" << _mxaFileName << "'" << std::endl;
    return;
  }
  if (_dataRecordName.empty() == true)
  {
    IDataModel::Pointer model = _dataFile->getDataModel();
    IDataDimension::Container dataDims = model->getDataDimensions();
    // The slice dimension is the first dimension
    IDataDimension::Pointer sliceDim = model->getDataDimension(0);
    // The 'Tile Number' Dimension is the second
    IDataDimension::Pointer tileDim = model->getDataDimension(1);

    // Get the first data record
    IDataRecord::Pointer rec = model->getDataRecords().at(0);
    _dataRecordName = rec->getRecordName();
  }

  this->_registrationTable = this->initCrossCorrelationTable();
  if (this->_registrationTable.get() == NULL)
  {
    mxa_log << logTime() << "Could not initialize rigid Registration Table." << std::endl;
    return;
  }

  std::stringstream ss;
  float progress = 0.0f;
  float total = 0.0f;
  CrossCorrelationData::Pointer registrationData;
  ss << "Phase Correlation Method Registering Slices " << _sliceStart << " to " << _sliceEnd;
  if (this->_prog.get() != NULL)
   {
    this->_prog->setMessage(ss.str());
   }
  if (_debug) {
    mxa_log << logTime() << ss.str() << std::endl;
  }


  int minSlice, maxSlice;
  this->_registrationTable->getMinMaxSlice(minSlice, maxSlice);
  total = maxSlice - minSlice;
  for (int32_t currentSlice = minSlice; currentSlice <= maxSlice; ++currentSlice)
  {
    registrationData = this->_registrationTable->getCrossCorrelationData(currentSlice);
    if (NULL == registrationData.get())
    {
      mxa_log << logTime() << " Skipping Slice " << currentSlice << " because no slice data was found. Is there data for the slice in the input file?" << std::endl;
      continue;
    }

    if (registrationData->getComplete() == 1)
    {
      if (_debug) { mxa_log << logTime() << " Skipping Slice " << currentSlice << " Already Complete" << std::endl;}
      continue;
    }
    else
    {
      ss.str("");
      ss << "Thread-" << registrationData->getFixedSlice() << "/" << registrationData->getMovingSlice();
      std::string threadName = ss.str();
      boost::thread* thrd = _threadGroup->startThread( CrossCorrelation(_threadGroup, -1, threadName, this, registrationData));
      BOOST_ASSERT(thrd != NULL);
      ss.str("");
      ss << "Starting Slice " << registrationData->getFixedSlice() << "/" << registrationData->getMovingSlice();
      if (this->_prog.get() != NULL)
      {
        this->_prog->setMessage(ss.str());
      }
      if (_debug) {
        mxa_log << logTime() << ss.str() << std::endl;
      }


    } // End 'else'

      if (this->_prog.get() != NULL && this->_prog->isCancelled() == true)
      {
        break;
      }

      if (this->_prog.get() != NULL)
      {
        this->_prog->setProgress(progress / total);
        progress = progress + 1.0f;
      }


    if (this->_prog.get() != NULL && this->_prog->isCancelled() == true)
    {
      break; // Break from the loop as we have been cancelled.
    }

  } // End Slice Loop
  _threadGroup->join_all();
  _threadGroup = R3DThreadGroup::NullPointer();
  if (this->_registrationTableFileName.empty() == false)
  {
    this->_registrationTable->writeToFile(this->_registrationTableFileName);
    this->_registrationTable->writeToAsciiFile(this->_registrationTableFileName.append("_Ascii.txt"), ",");
  }
  this->_updateAsAlignedValues();
  // Close the MXA File
  this->_dataFile->closeFile(false);

  ss.str("");
  ss << "Ending Threaded RoboMet PCM Registration with " << this->_maxThreads << " Threads.";
  if (this->_prog.get() != NULL)
  {
    this->_prog->setMessage(ss.str());
    this->_prog->setProgress(1.0f);
    this->_prog->finished();
  }

  if (this->_debug)
  {
    mxa_log << logTime() << ss.str() << std::endl;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationController::_updateAsAlignedValues()
{
  // Check for NULL pointer and return if we get one.
  if (NULL == _registrationTable.get())
  {
    return;
  }

  std::stringstream ss;
  int minSlice, maxSlice;
  CrossCorrelationData::Pointer regData = CrossCorrelationData::NullPointer();
  _registrationTable->getMinMaxSlice(minSlice, maxSlice);
  float origin[2];
  float accumTranslation[2] = {0.0f, 0.0f};
  float absoluteOrigin[2];
  float mosaicMaxDims[2];
  int err = 0;
  int retErr = 0;
  float xAsAligned = 0.0f, yAsAligned = 0.0f;
  float halfWidth = 0.0f;
  float halfHeight = 0.0f;
  float scaling = 0.0f;
  int32_t mosaicPixelDims[2];
  float frameOrigin[2];
  int32_t tileDims[2];

 // mxa_log << logTime() << this->_mxaFileName << " IS NOT BEING UPDATED." << std::endl;
  for (int i = minSlice; i <= maxSlice; ++i)
  {
    regData = _registrationTable->getCrossCorrelationData(i);
    if (regData.get() != NULL)
    {
      // If we are on the first slice then get the initial translations

      accumTranslation[0] += regData->getXTrans();
      accumTranslation[1] += regData->getYTrans();

      origin[0] = regData->getXMovingOrigin();
      origin[1] = regData->getYMovingOrigin();
      if (_debug) {
        ss.str("");
        ss << "++++++++++++ Updating Slice " << regData->getMovingSlice() << "+++++++++++++++++++++" << std::endl;
        ss << " Fixed Slice: " << regData->getFixedSlice() << std::endl;
        ss << " Moving Slice: " << regData->getMovingSlice() << std::endl;
        ss << " Origin: " << origin[0] << ", " << origin[1] << std::endl;
        ss << " Trans: " << regData->getXTrans() << ", " << regData->getYTrans() << std::endl;
        ss << " Accum Trans: " << accumTranslation[0] << ", " << accumTranslation[1] << std::endl;
        origin[0] = origin[0] - accumTranslation[0];
        origin[1] = origin[1] - accumTranslation[1];
        ss << " New Org: " << origin[0] << ", " << origin[1] << std::endl;
        mxa_log << logTime() << ss.str() << std::endl;
      }
      hid_t fileId = _dataFile->getFileId();
      if (fileId < 0)
      {
        if (_debug)
        {
          mxa_log << logTime() << "--FileId was Negative." << std::endl;
        }
        return;
      }
      IDataRecord::Pointer rec = _dataFile->getDataModel()->getDataRecordByNamedPath(this->getDataRecordName(), NULL);
      if (NULL == rec.get())
      {
        if (_debug)
        {
          mxa_log << logTime() << "--DataRecord was NULL" << std::endl;
        }
        return;
      };

      absoluteOrigin[0] = std::numeric_limits<float>::max();
      absoluteOrigin[1] = std::numeric_limits<float>::max();

      mosaicMaxDims[0] = std::numeric_limits<float>::min();
      mosaicMaxDims[1] = std::numeric_limits<float>::min();

      int slice = regData->getMovingSlice();
      std::string datasetPath = "/" + _dataFile->getDataModel()->getDataRoot();
      datasetPath = datasetPath.append(StringUtils::numToString(slice));
      R3DSliceInfo::Pointer sliceInfo = R3DSliceInfo::New(slice);
      sliceInfo->setSliceNumber(slice);
      err = R3DSliceInfo::ReadMinimalSliceInfo(fileId, datasetPath, sliceInfo);

      scaling = regData->getScaling();
      // Apply the accumulated translations to each of the tiles.
      int totalTiles = sliceInfo->getTileCount();
      for (int tile = 0; tile < totalTiles; ++tile)
      {
        //mxa_log << "     Updating Tile #" << tile << " with new ImageCoordinates" << std::endl;
        std::string movingImagePath = datasetPath;
        movingImagePath = movingImagePath.append("/").append(StringUtils::numToString(tile)).append("/");
        movingImagePath = movingImagePath.append(rec->generatePath());


        //  Apply the accumulated translations to the current image AsAligned Value
        err = H5Lite::readScalarAttribute(fileId, movingImagePath, CONST_CAST_STRING(Zeiss::MetaXML::ImageCoordinateU), xAsAligned);
        CPP_CHECK_ATTRIBUTE_READ(movingImagePath, Zeiss::MetaXML::ImageCoordinateU, xAsAligned);

        err = H5Lite::readScalarAttribute(fileId, movingImagePath, CONST_CAST_STRING(Zeiss::MetaXML::ImageCoordinateV), yAsAligned);
        CPP_CHECK_ATTRIBUTE_READ(movingImagePath, Zeiss::MetaXML::ImageCoordinateU, yAsAligned);

        xAsAligned = xAsAligned - accumTranslation[0];
        yAsAligned = yAsAligned - accumTranslation[1];

        // Write the values back out the to HDF5 file
        err = H5Lite::writeScalarAttribute(fileId, movingImagePath, Zeiss::MetaXML::ImageCoordinateU, xAsAligned);
        CPP_CHECK_ATTRIBUTE_WRITE(movingImagePath, Zeiss::MetaXML::ImageCoordinateU, xAsAligned);

        err = H5Lite::writeScalarAttribute(fileId, movingImagePath, Zeiss::MetaXML::ImageCoordinateV, yAsAligned);
        CPP_CHECK_ATTRIBUTE_WRITE(movingImagePath, Zeiss::MetaXML::ImageCoordinateV, yAsAligned);

        // Recalculate the Absolute X and Y Origin and the Mosaic Pixel Dimensions
        err = H5Lite::readScalarAttribute(fileId, movingImagePath, CONST_CAST_STRING(Zeiss::MetaXML::ImageWidth), tileDims[0]);
        CPP_CHECK_ATTRIBUTE_READ(movingImagePath, Zeiss::MetaXML::ImageWidth, tileDims[0]);

        err = H5Lite::readScalarAttribute(fileId, movingImagePath, CONST_CAST_STRING(Zeiss::MetaXML::ImageHeight), tileDims[1]);
        CPP_CHECK_ATTRIBUTE_READ(movingImagePath, Zeiss::MetaXML::ImageHeight, tileDims[1]);

        halfWidth = ((tileDims[0] * scaling) / 2.0f);
        halfHeight = ((tileDims[1] * scaling) / 2.0f);
        frameOrigin[0] = xAsAligned - halfWidth;
        frameOrigin[1] = yAsAligned - halfHeight;
        if (frameOrigin[0] < absoluteOrigin[0])
        {
          absoluteOrigin[0] = frameOrigin[0];
        }
        if (frameOrigin[1] < absoluteOrigin[1])
        {
          absoluteOrigin[1] = frameOrigin[1];
        }
        if (xAsAligned + halfWidth > mosaicMaxDims[0])
        {
          mosaicMaxDims[0] = xAsAligned + halfWidth;
        }
        if (yAsAligned + halfHeight > mosaicMaxDims[1])
        {
          mosaicMaxDims[1] = yAsAligned + halfHeight;
        }

      }
      // Get the first image, which will be the upper left image
      std::string fixedImagePath = datasetPath;
      int tile = 0;
      fixedImagePath = fixedImagePath.append("/").append(StringUtils::numToString(tile)).append("/");
      fixedImagePath = fixedImagePath.append(rec->generatePath());
      ZeissTileInfo::Pointer frameInfo = ZeissTileInfo::ReadMinimalZeissTileInfo(_dataFile->getFileId(), fixedImagePath);
      if (NULL == frameInfo.get())
      {
        mxa_log << logTime() << "Error reading frame info for path " << fixedImagePath << std::endl;
        return;
      }


      float sliceAbsOrigin[2];
      frameInfo->getAsAligned(sliceAbsOrigin);
      int32_t imageDims[2];
      frameInfo->getImageDimensions(imageDims);

      sliceAbsOrigin[0] = sliceAbsOrigin[0] - (imageDims[0] * scaling / 2.0f);
      sliceAbsOrigin[1] = sliceAbsOrigin[1] - (imageDims[1] * scaling / 2.0f);

      // Recalculate the Slice Info for the mosaic origin (microns) and size (pixels)
      sliceInfo->setImageOrigin(sliceAbsOrigin);

      mosaicPixelDims[0] = (int32_t) ((mosaicMaxDims[0] - absoluteOrigin[0]) / scaling);
      mosaicPixelDims[1] = (int32_t) ((mosaicMaxDims[1] - absoluteOrigin[1]) / scaling);
      sliceInfo->setImagePixelDimensions(mosaicPixelDims);

      sliceInfo->setCenterOfRotation(0.0f, 0.0f);
      sliceInfo->setRotationAngleRadians(0.0);
      mxa_log << logTime() << "Writing New Slice Meta Data for Slice #" << sliceInfo->getSliceNumber() << std::endl;

      err = R3DSliceInfo::WriteSliceMetaData(fileId, datasetPath, sliceInfo);
      if (err < 0)
      {
        mxa_log << logTime() << "Error: CrossCorrelation::_updateAsAlignedValues - Problem updating Slice Meta Data for Slice " << slice << std::endl;
    }
    }
  }


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationTable::Pointer CrossCorrelationController::initCrossCorrelationTable()
{
  if (_debug) mxa_log << logTime() << "CrossCorrelation::initCrossCorrelationTable" << std::endl;

  IDataModel::Pointer model = _dataFile->getDataModel();
  IDataDimension::Container dataDims = model->getDataDimensions();
  // The slice dimension is the first dimension
  IDataDimension::Pointer sliceDim = model->getDataDimension(0);
  // The 'Tile Number' Dimension is the second
  IDataDimension::Pointer tileNumDim = model->getDataDimension(1);

 // std::vector<IDataDimension*>::size_type index = 0;
  std::string pathTemplate = "/" + model->getDataRoot();

  CrossCorrelationTable::Pointer table = CrossCorrelationTable::NullPointer();

  int32_t sliceStartIndex, sliceEndIndex, imageStartIndex, imageEndIndex;
  sliceStartIndex = sliceDim->getStartValue();
  sliceEndIndex = sliceDim->getEndValue();
  imageStartIndex = tileNumDim->getStartValue();
  imageEndIndex = tileNumDim->getEndValue();

  // Check to make sure the user has not asked for a subset of the data to be aligned
//  if (_sliceStart > sliceStartIndex) { sliceStartIndex = _sliceStart; }
//  if (_sliceEnd < sliceEndIndex && _sliceEnd >= sliceStartIndex) { sliceEndIndex = _sliceEnd; }
#if 0
  if (this->_registrationTableFileName.empty() == false)
  {

    if (MXADir::exists(this->_registrationTableFileName))
    {
      if (_debug) mxa_log << logTime() << "Initializing CrossCorrelationTable from file: " << this->_registrationTableFileName << std::endl;
      table = CrossCorrelationTable::ReadFromFile(this->_registrationTableFileName);
      int32_t minSlice, maxSlice;
      table->getMinMaxSlice(minSlice, maxSlice);
      if (sliceStartIndex < minSlice || sliceEndIndex > maxSlice )
      {
        updateRegistrationTable(_dataFile, table);
      }
    }
  }
#endif

  if (NULL == table.get())
  {
    // The hint table was null because there was no file or there was an error reading the file
    if (_debug) mxa_log << logTime() << "Initializing NEW CrossCorrelationTable File: " << this->_registrationTableFileName << std::endl;
    table = createNewRegistrationTable(_dataFile);

    if (this->_registrationTableFileName.empty() == false) {
      table->writeToFile(this->_registrationTableFileName);
    }
  }

  return table;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationController::updateRegistrationTable(IDataFile::Pointer datafile, CrossCorrelationTable::Pointer table)
{
  if (_debug) mxa_log << logTime() << "CrossCorrelation::updateRegistrationTable()" << std::endl;

  IDataModel::Pointer model = _dataFile->getDataModel();
  IDataDimension::Container dataDims = model->getDataDimensions();
  // The slice dimension is the first dimension
  IDataDimension::Pointer sliceDim = model->getDataDimension(0);
  // The 'Tile Number' Dimension is the second
  IDataDimension::Pointer tileNumDim = model->getDataDimension(1);

  std::string pathTemplate = "/" + model->getDataRoot();

  int32_t sliceStartIndex, sliceEndIndex, imageStartIndex, imageEndIndex;
  sliceStartIndex = sliceDim->getStartValue();
  sliceEndIndex = sliceDim->getEndValue();
  imageStartIndex = tileNumDim->getStartValue();
  imageEndIndex = tileNumDim->getEndValue();

  // Check to make sure the user has not asked for a subset of the data to be aligned
  if (_sliceStart > sliceStartIndex) { sliceStartIndex = _sliceStart; }
  if (_sliceEnd < sliceEndIndex && _sliceEnd > sliceStartIndex) { sliceEndIndex = _sliceEnd; }
  int err = 0;
  hid_t fileId = _dataFile->getFileId();
  if (fileId < 0)
  {
    mxa_log << logTime() << "Error: CrossCorrelation::updateRegistrationTable --FileId was Negative." << std::endl;
    return;
  }
  IDataRecord::Pointer rec = _dataFile->getDataModel()->getDataRecordByNamedPath(this->_dataRecordName, NULL);
  if (NULL == rec.get())
  {
    mxa_log << logTime() << "Error: CrossCorrelation::updateRegistrationTable --DataRecord was NULL" << std::endl;
    return;
  }

  std::string datasetPath = "/" + _dataFile->getDataModel()->getDataRoot();
  R3DSliceInfo::Pointer sliceInfoPtr = R3DSliceInfo::New(0);
  CrossCorrelationData::Pointer d = CrossCorrelationData::NullPointer();
  for (int slice = sliceStartIndex; slice < sliceEndIndex; ++slice)
  {

    std::string slicePath = datasetPath + StringUtils::numToString(slice);
    sliceInfoPtr->setSliceNumber(slice);
    err = R3DSliceInfo::ReadMinimalSliceInfo(fileId, slicePath, sliceInfoPtr);
    if (err < 0)
    {
      mxa_log << logTime() << "Error: CrossCorrelation::updateMosaicStitcherTable - Could not read the slice info from the MXA file for dataset '" << slicePath << "'" << std::endl;
      return;
    }
    d = table->getCrossCorrelationData(slice);
    if (d.get() == NULL)
    {
      d =  CrossCorrelationData::New();
      d->setFixedSlice(slice);
      d->setMovingSlice(slice + 1);
      table->addCrossCorrelationData(slice, d);
    }
    else // Already have the data from the hint file for this slice. Move to the next slice
    {
      continue;
    }
  }
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationTable::Pointer CrossCorrelationController::createNewRegistrationTable(IDataFile::Pointer datafile)
{
  if (_debug) mxa_log << logTime() << "CrossCorrelation::createNewRegistrationTable()" << std::endl;

  IDataModel::Pointer model = _dataFile->getDataModel();
  IDataDimension::Container dataDims = model->getDataDimensions();
  // The slice dimension is the first dimension
  IDataDimension::Pointer sliceDim = model->getDataDimension(0);
  // The 'Tile Number' Dimension is the second
  IDataDimension::Pointer tileNumDim = model->getDataDimension(1);

  std::string pathTemplate = "/" + model->getDataRoot();

  CrossCorrelationTable::Pointer nullTable = CrossCorrelationTable::NullPointer();
  CrossCorrelationTable::Pointer table = CrossCorrelationTable::New();

  int32_t sliceStartIndex, sliceEndIndex, imageStartIndex, imageEndIndex;
  sliceStartIndex = sliceDim->getStartValue();
  sliceEndIndex = sliceDim->getEndValue();
  imageStartIndex = tileNumDim->getStartValue();
  imageEndIndex = tileNumDim->getEndValue();

  // Check to make sure the user has not asked for a subset of the data to be aligned
  if (_sliceStart > sliceStartIndex) { sliceStartIndex = _sliceStart; }
  if (_sliceEnd < sliceEndIndex && _sliceEnd > sliceStartIndex) { sliceEndIndex = _sliceEnd; }
  int err = 0;
  hid_t fileId = _dataFile->getFileId();
  if (fileId < 0)
  {
    mxa_log << logTime() << "--FileId was Negative." << std::endl;
    return nullTable;
  }
  IDataRecord::Pointer rec = _dataFile->getDataModel()->getDataRecordByNamedPath(this->_dataRecordName, NULL);
  if (NULL == rec.get())
  {
    mxa_log << logTime() << "--DataRecord was NULL" << std::endl;
    return nullTable;
  }
  std::string datasetPath = "/" + _dataFile->getDataModel()->getDataRoot();
  R3DSliceInfo::Pointer sliceInfoPtr = R3DSliceInfo::New(0);
  for (int slice = sliceStartIndex; slice < sliceEndIndex; ++slice)
  {

    std::string slicePath = datasetPath + StringUtils::numToString(slice);
    sliceInfoPtr->setSliceNumber(slice);
    err = R3DSliceInfo::ReadMinimalSliceInfo(fileId, slicePath, sliceInfoPtr);
    if (err < 0)
    {
      return nullTable;
    }
    CrossCorrelationData::Pointer d =  CrossCorrelationData::New();
    d->setFixedSlice(slice);
    d->setMovingSlice(slice + 1);
    table->addCrossCorrelationData(slice, d);
  }
  return table;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CrossCorrelationController::readImageData( R3DSliceInfo::Pointer sliceInfo,
                                    int fftDim,
                                    CrossCorrelationData::Pointer regData,
                                    unsigned char* &localBuffer)
{
  ScopedLock lk(monitor);
  int err = 0;
  hid_t fileId = _dataFile->getFileId();
  if (fileId < 0)
  {
   mxa_log << logTime() << "--FileId was Negative." << std::endl;
   return -1;
  }
  IDataRecord::Pointer rec = _dataFile->getDataModel()->getDataRecordByNamedPath(this->_dataRecordName, NULL);
  if (NULL == rec.get())
  {
   mxa_log << logTime() << "--DataRecord was NULL" << std::endl;
   return -1;
  }
  int sliceNumber = sliceInfo->getSliceNumber();
  std::string datasetPath = "/" + _dataFile->getDataModel()->getDataRoot();
  datasetPath = datasetPath.append(StringUtils::numToString(sliceNumber));
  err = R3DSliceInfo::ReadMinimalSliceInfo(fileId, datasetPath, sliceInfo);
  if (err < 0)
  {
   return -4;
  }

  float scaling[2];
  sliceInfo->getScaleFactor(scaling);
  regData->setScaling(scaling[0]);

  regData->setImageWidth(fftDim);
  regData->setImageHeight(fftDim);

  float center[2];
  sliceInfo->getImageCoordinate(center);
  float voiOrigin[2];
  if (sliceInfo->getSliceNumber() == regData->getFixedSlice())
  {
    voiOrigin[0] = center[0] - ((regData->getImageWidth() * scaling[0])/2.0);
    voiOrigin[1] = center[1] - ((regData->getImageHeight() * scaling[1])/2.0);
    regData->setFixedOrigin(voiOrigin);
  }
  else if (sliceInfo->getSliceNumber() == regData->getMovingSlice() )
  {
    regData->getFixedOrigin(voiOrigin);
    regData->setMovingOrigin(voiOrigin);
  }
  else
  {
    mxa_log << logTime() << "Error: CrossCorrelation::readImageData could not match Slice Number from the slice info object with value [";
    mxa_log << logTime() << sliceInfo->getSliceNumber() << "] to either the fixed Slice [" << regData->getFixedSlice() << "] or the moving slice [";
    mxa_log << logTime() << regData->getMovingSlice() << "]" << std::endl;
    return -1;
  }

  // Retrieve our VOI Origin from the fixed slice
  regData->getFixedOrigin(voiOrigin);

  // Export a mosaic of the proper size
  R3DMosaicExportProperties::Pointer p = R3DMosaicExportProperties::New();
  p->setSlice(sliceInfo->getSliceNumber());
  p->setDataRecordName(this->_dataRecordName);
  p->setVerbose(false);
  p->setVoiOrigin(voiOrigin);
  p->setUseVOISize(false);
  int d[2] = {fftDim, fftDim};
  p->setOutputPixelSize(d);


  R3DMosaic::Pointer mosaic = MosaicExporter::ExportMosaic(p, _dataFile);
  if (NULL != mosaic.get() )
  {
    localBuffer = mosaic->getImageBuffer();
    mosaic->setManageMemory(false); // We need to keep the memory buffer around
  }

#if 0
  std::stringstream ss;
  ss << "/tmp/" << fftDim  << "_" << sliceNumber << "_Mosaic.tif";
  p->setTifFilename(ss.str());
  p->setMXAFileName(this->_mxaFileName);
  R3DMosaic::Pointer m2;
  err = MosaicExporter::ExportMosaicToFile(p, m2);
#endif

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CrossCorrelationController::getImageInfo(hid_t fileId,
                                        const std::string &datasetName,
                                        hsize_t* cols,
                                        hsize_t* rows,
                                        hsize_t* planes,
                                        std::string &interlace,
                                        hssize_t* npals,
                                        std::string &attrValSubclass)
{
  ScopedLock lk(monitor);
  int err = H5Image::H5IMget_image_info(fileId, datasetName, cols, rows, planes, interlace, npals);
  if (err < 0)
    return -1;
  err = H5Lite::readStringAttribute(fileId, datasetName, MXA::H5Image::ImageSubclass, attrValSubclass);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationController::debugOutput(const std::string &threadName, const std::string &str)
{
  ScopedLock lk(monitor);
  std::stringstream ss;
  ss <<  "{" << threadName << "} " << str ;
  if (this->_prog.get() != NULL)
  {
    this->_prog->setMessage(ss.str());
  }
  else if (this->_debug)
  {
     mxa_log << logTime() << ss.str() << std::endl;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationController::threadIsComplete(CrossCorrelation* thread, CrossCorrelationData::Pointer regData)
{
  ScopedLock lk(monitor);
  // Do any writing to the HDF5 file or what ever needs a lock
  std::stringstream ss;
  ss << "Ending Slice " << regData->getFixedSlice() << "/" << regData->getMovingSlice() << " Complete.";
  if (this->_prog.get() != NULL)
  {
    this->_prog->setMessage(ss.str());
  }
  //  else
  if (_debug)
  {
    mxa_log << logTime() << ss.str() << std::endl;
  }

  //if (_debug) { mxa_log << logTime() << "  Checking if Slice " stitcherData->getSlice() << " is Complete...." << std::endl;}



  if (this->_registrationTableFileName.empty() == false)
  {
    ss.str("");
    ss << "Updating Registration Data file for slice " << regData->getMovingSlice() << " '" << this->_registrationTableFileName << "'";
    //    if (this->_prog.get() != NULL)
    //    {
    //      this->_prog->setMessage(ss.str());
    //    }
    //    else
    if (_debug)
    {
      mxa_log << logTime() << ss.str() << std::endl;
    }
    int err = this->_registrationTable->writeToFile(this->_registrationTableFileName);
    if (err < 0)
    {
      mxa_log << logTime() << "Error Writing Registration Table to temp file '" << this->_registrationTableFileName << "'" << std::endl;
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationController::setSliceBoundaries(int sliceStart, int sliceEnd)
{
  this->_sliceStart = sliceStart;
  this->_sliceEnd = sliceEnd;
}



