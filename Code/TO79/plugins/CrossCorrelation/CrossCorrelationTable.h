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
#ifndef _CrossCorrelationTABLE_H_
#define _CrossCorrelationTABLE_H_
#include <CrossCorrelation/CrossCorrelationData.h>

//-- MXA includes
#include <MXA/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/Common/IO/MXAFileWriter64.h>

//-- STL includes
#include <map>



/**
* @class CrossCorrelationTable CrossCorrelationTable.h CrossCorrelation/CrossCorrelationTable.h
* @brief Holds a list of the CrossCorrelationData objects
* @author Michael A. Jackson for BlueQuartz Software
* @date May 8, 2009
* @version 1.0
*/
class CrossCorrelationTable
{
  public:
    MXA_SHARED_POINTERS(CrossCorrelationTable);
    MXA_STATIC_NEW_MACRO(CrossCorrelationTable);
    MXA_TYPE_MACRO(CrossCorrelationTable);
    typedef std::map <int32_t, CrossCorrelationData::Pointer> RegistrationMap;

    virtual ~CrossCorrelationTable();

    static Pointer ReadFromFile(const std::string &filename);

    int32_t writeToFile(const std::string &filename);

    int32_t writeToAsciiFile(const std::string &filename, const std::string &delimiter);

    void addCrossCorrelationData(int32_t sliceNum, CrossCorrelationData::Pointer data);

    CrossCorrelationData::Pointer getCrossCorrelationData(int32_t sliceNum);

    void getMinMaxSlice(int &minSlice, int &maxSlice);

    /**
     * @brief Prints the table to the iostream passed as the argument
     * @param out The std::iostream to write the table to
     * @param delimiter The delimiter to use between data
     */
    void printTable(std::ostream &out, const std::string &delimiter);
    void printAsciiHeader(std::ostream &out, const std::string &delimiter);

  protected:
    CrossCorrelationTable();

  private:
    RegistrationMap  _map;


    CrossCorrelationTable(const CrossCorrelationTable&);    // Copy Constructor Not Implemented
    void operator=(const CrossCorrelationTable&);  // Operator '=' Not Implemented

};

#endif /* _CrossCorrelationTABLE_H_ */
