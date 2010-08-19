///////////////////////////////////////////////////////////////////////////////
//
/* ============================================================================
* Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of Michael A. Jackson nor the names of its contributors may
* be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
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
