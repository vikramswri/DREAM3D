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
#ifndef _CrossCorrelation_H_
#define _CrossCorrelation_H_

//-- Our own includes
#include <R3D/Common/R3DTypeDefs.h>
#include <R3D/Common/IR3DProgress.h>
#include <R3D/Common/R3DSliceInfo.h>
#include <R3D/Common/ZeissTileInfo.h>
#include <R3D/Threads/R3DThreadGroup.h>
#include <CrossCorrelation/CrossCorrelationThread.h>
#include <CrossCorrelation/CrossCorrelationData.h>
#include <CrossCorrelation/CrossCorrelationTable.h>

//-- MXA Includes
#include <MXA/Base/IDataFile.h>

//-- Boost Thread Includes
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

//-- STL includes
#include <string>
#include <vector>


//-- Typedef for conveneience
typedef boost::mutex::scoped_lock ScopedLock;



class CrossCorrelation;

/**
* @class CrossCorrelation CrossCorrelation.h R3D/ImageProcessing/Registration/CrossCorrelation.h
* @brief This is the master controller for the Image Registration process that uses the Phase
* Correlation Method (Cross Correlation using FFT) to find the 2D translation of a fixed and
* moving image.
* @author Michael A. Jackson for BlueQuartz Software
* @date June 20, 2009
* @version 1.0
*/
class CrossCorrelationController : private boost::noncopyable
{
  public:
    MXA_SHARED_POINTERS(CrossCorrelationController);

    /**
     * @brief Constructor
     * @param mxaFileName The path to the MXA Data file holding the data to be registered
     * @param dataRecordName The name of the data record to perform the registration on
     * @param logFilename The name/path of a file to use for logging.
     * @param maxThreads The maximum number of threads to use for the registration
     * @param sliceStart The first slice to register
     * @param sliceEnd The last slice to register
     * @return
     */
    CrossCorrelationController(const std::string &mxaFileName,
                              const std::string &dataRecordName,
                              const std::string &logFilename,
                              int maxThreads,
                              int sliceStart,
                              int sliceEnd);

    virtual ~CrossCorrelationController();

    /**
     * @brief This is the main entry point for this class to begin the running of the code. The
     * code is multi-threaded by design.
     */
    virtual void run();

    /**
     * @brief Sets a flag so that more detailed process information is output
     * @param dbg True to have more detailed process output delivered to the selected output device.
     */
    void setDebug(bool dbg);

    /**
     * @brief Sets the starting and ending (inclusive) indices of the slices to aligned
     * @param sliceStart The starting slice
     * @param sliceEnd The ending slice
     */
    virtual void setSliceBoundaries(int sliceStart, int sliceEnd);

   /**
    * @brief Method will initialize a new Hint Table object
    * @param dataFile The MXA Datafile
    */
   virtual CrossCorrelationTable::Pointer createNewRegistrationTable(IDataFile::Pointer dataFile);

   /**
    * @brief Updates a StitchDataTable with newly initialized StitchData pointers for slices
    * that were not found in an existing file. This can happen if the user asks for a wider
    * slice range than what is stored in the hint file.
    * @param dataFile The MXA Data File
    * @param table The TileAlignmentTable object
    */
   virtual void updateRegistrationTable(IDataFile::Pointer dataFile,
                                          CrossCorrelationTable::Pointer table);


   /**
    * @brief Called when a thread has completed its operations
    * @param thread The completed Thread
    * @param stitcherData The TileAlignmentData pointer
    */
   virtual void threadIsComplete(CrossCorrelation* thread,
                                 CrossCorrelationData::Pointer stitcherData);

   /**
    * @brief Reads the image data from an HDF5/MXA file
    * @param sliceInfo The Zeiss Slice Info Object
    * @param fftDim The dimension of the FFT, or basically the size of the image to read from the mxa file
    * @param regData The RegistrationData object for this class
    * @param localBuffer Preallocated uchar buffer
    * @return Negative value on error
    */
   virtual int readImageData( R3DSliceInfo::Pointer sliceInfo,
                              int fftDim,
                              CrossCorrelationData::Pointer regData,
                              unsigned char* &localBuffer);

   /**
    * @brief Delivers a message to the selected output device.
    * @param threadName The name of the thread delivering the message
    * @param str The message to deliver
    */
   virtual void debugOutput(const std::string &threadName, const std::string &str);



   /**
    * @brief Gets information about an image stored in an HDF5/MXA file
    * @param fileId The HDF5 fileid value
    * @param datasetName The path to the data set
    * @param cols The number of columns (width) of the image (Output variable)
    * @param rows The number of rows (height) of the image (Output variable)
    * @param planes The number of color channels in the image (Output variable)
    * @param interlace Are the color channels interlaced? (Output variable)
    * @param npals The color palette (Output variable)
    * @param attrValSubclass The Attribute value subclass to be filled in. (Output variable)
    * @return
    */
   int getImageInfo(hid_t fileId, const std::string &datasetName,
                    hsize_t* cols, hsize_t* rows, hsize_t* planes,
                                  std::string &interlace, hssize_t* npals,
                                  std::string &attrValSubclass);

   /**
    * @brief Returns the name of the mxa file that is being used
    * @return
    */
   std::string getMXAFileName() { return this->_mxaFileName; }

   /**
    * @brief returns the name of the DataRecord being used
    * @return
    */
   std::string getDataRecordName() { return this->_dataRecordName; }

   /**
    * @brief Sets the filename to use for the hint table.
    * @param fn The filename for the hint table
    */
   void setHintTableFileName(const std::string &fn) { this->_registrationTableFileName = fn; }

   /**
    * @brief Returns the name of the hint file being used.
    * @return
    */
   std::string getHintTableFileName() {return this->_registrationTableFileName; }

   /**
    * @brief Returns the R3DRegistrationStats table that is being used.
    * @return
    */
   CrossCorrelationTable::Pointer getHintTablePtr() { return this->_registrationTable; }


   /**
    * @brief Sets the object that reports progress
    */
   MXA_INSTANCE_PROPERTY(IR3DProgress::Pointer, ProgressObject, _prog);

   /**
    * @brief Initializes the Registration Table
    */
   CrossCorrelationTable::Pointer initCrossCorrelationTable();

  protected:
   // Thread related
   boost::mutex monitor;

   /**
    * @brief This method will take all the values stored in the hint table and update
    * the 'AsAligned' values of each Data Records in the MXA file so that stitching
    * operations will correctly produce a mosaic image.
    *
    * WARNING: This method should ONLY be called from another method where the
    * mutex has been locked otherwise havoc will probably ensue in your HDF5 file.
    * This method is exclusively called from the 'threadFinished' method and nowhere
    * else.
    */
   void _updateAsAlignedValues();

  private:
   std::string _mxaFileName;
   std::string _dataRecordName;
   std::string _logFilename;
   std::string _registrationTableFileName;
   IDataFile::Pointer _dataFile;
   CrossCorrelationTable::Pointer _registrationTable;
   double _scaling;
   int _maxThreads;
   int _sliceStart;
   int _sliceEnd;
   bool _debug;

   R3DThreadGroup::Pointer              _threadGroup;

   MXALOGGER_CLASS_VARIABLE_DECLARATION;
};

#endif /* _CrossCorrelation_H_ */
