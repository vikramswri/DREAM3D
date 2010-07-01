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


#include <MXA/Common/LogTime.h>
#include <MXA/Common/MXATypeDefs.h>
#include <MXA/Base/IDataModel.h>
#include <MXA/Base/IDataFile.h>
#include <MXA/Base/IDataRecord.h>
#include <MXA/HDF5/H5MXADataFile.h>
#include <MXA/Utilities/StringUtils.h>
#include <MXA/Utilities/MXAFileInfo.h>
#include <MXA/Utilities/MXALogger.h>

#include <iostream>

#include <R3D/R3DVersion.h>
#include <CrossCorrelation/CrossCorrelation.h>

//-- Boost Program Options
#include <boost/program_options.hpp>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void listRecordNames(const std::string &inputfile)
{
  std::cout << logTime() << "Listing Data Records for '" << inputfile << "'" << std::endl;
  IDataFile::Pointer dataFile = H5MXADataFile::OpenFile(inputfile, true);
  if (NULL == dataFile.get())
  {
    return;
  }
  IDataRecord::Container records = dataFile->getDataModel()->getDataRecords();
  std::cout << logTime() << "Data Records in File:" << std::endl;
  for (IDataRecord::Container::iterator record = records.begin(); record != records.end(); ++record ) {
    std::cout << logTime() << "  " << (*record)->getRecordName() << std::endl;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::string inputFile;
  std::string recname;
  std::string logFile;
  int32_t threads = 1;
  int32_t sliceStart = -1;
  int32_t sliceEnd = -1;
  bool verbose = false;
  bool vv = false;
  std::string restartFile;
  std::string listRecordsInputFile;

  // Handle program options passed on command line.
  boost::program_options::options_description desc("Required Parameters");
   desc.add_options()
   ("help", "Produce help message")
   ("inputfile,i", boost::program_options::value<std::string>(&inputFile), "MXA File to be used as input")
   ("recname,r", boost::program_options::value<std::string>(&recname), "Name of the Data Record to Register")
   ("sliceStart,s", boost::program_options::value<int32_t>(&sliceStart), "The value of the starting slice to fix")
   ("sliceEnd,e", boost::program_options::value<int32_t>(&sliceEnd), "The value of the ending slice to fix")
   ("numThreads,t", boost::program_options::value<int32_t>(&threads), "The number of threads to use when registering")
   ("logfile,l", boost::program_options::value<std::string>(&logFile), "Name of the Log file to store any output into")
   ("restartfile,R", boost::program_options::value<std::string>(&restartFile), "Location of the temporary file to initialize from.")
   ("listRecords,L", boost::program_options::value<std::string>(&listRecordsInputFile), "List the possible Data Model Records for the given MXA file")
   ("verbose,v", boost::program_options::bool_switch(&verbose), "Verbose Output")
   ("very_verbose,V", boost::program_options::bool_switch(&vv), "Very Verbose Output - Good for debugging the export")
   ;

   boost::program_options::variables_map vm;
   boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
   boost::program_options::notify(vm);
  try
  {
    // Print help message if requested by user and return.
    if (vm.count("help") || argc < 2)
    {
      std::cout << desc << std::endl;
      return EXIT_SUCCESS;
    }
    if (vm.count("listRecords"))
    {
      listRecordsInputFile = MXAFileInfo::toNativeSeparators(listRecordsInputFile);
      listRecordNames(listRecordsInputFile);
      return EXIT_SUCCESS;
    }

  } catch (...)
  {
    std::cout << "** Unknown Arguments. Displaying help listing instead. **" << std::endl;
    std::cout << desc << std::endl;
    return EXIT_FAILURE;
  }


   if (inputFile.empty() || recname.empty() )
   {
     std::cout << logTime() << "Arguments must be given for inputfile and recname. Use --help for more information" << std::endl;
     return EXIT_FAILURE;
   }

   inputFile = MXAFileInfo::toNativeSeparators(inputFile);
   logFile = MXAFileInfo::toNativeSeparators(logFile);
   restartFile = MXAFileInfo::toNativeSeparators(restartFile);

   MXALOGGER_METHOD_VARIABLE_INSTANCE;
   if (false == logFile.empty()) {
     mxa_log.open(logFile);
   }
   mxa_log << logTime() << "PCM Registration Version " << R3D::Version::Complete <<  " Starting " << std::endl;

  // for (int slice = sliceStart; slice <= sliceEnd; ++slice)
   {
     //std::cout << logTime() << "************** Starting Slice ********************************" << std::endl;
     CrossCorrelationController::Pointer registration( new CrossCorrelationController(inputFile, recname, logFile, threads, sliceStart, sliceEnd) );
     if (verbose || vv) { registration->setDebug(true); }
     registration->setHintTableFileName(restartFile);
     registration->run();
   }

   // std::cout << logTime() << "TileAlignment Complete" << std::endl;
   return EXIT_SUCCESS;
}

