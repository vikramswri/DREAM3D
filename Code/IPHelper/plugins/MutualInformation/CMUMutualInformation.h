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

#ifndef CMUMUTUALINFORMATION_H_
#define CMUMUTUALINFORMATION_H_

#include <vector>

#include "AIM/Common/AIMArray.hpp"

typedef AIMArray<unsigned char>           ImageColumnType;
//typedef AIMArray<unsigned char>::Pointer  ImageColumnPtr;


/*
 *
 */
class CMUMutualInformation
{
  public:
    CMUMutualInformation();
    virtual ~CMUMutualInformation();

    enum ErrorCodes
    {
      NO_ERROR = 0,
      NOT_ENOUGH_IMAGES_ERROR = -100,

    };

    AIMArray<uint32_t>::Pointer jointHistogram(std::vector<ImageColumnType::Pointer> &inData,
                                               int binSize,
                                               bool normalize);


    double mutualInfomation(AIMArray<double>::Pointer normalizedJointHistogram, int &level);



    void printArray(size_t ndims, size_t* S, size_t* i, size_t curDimIdx, int* n, int* array);

//    void calc_sums(unsigned int* array,int actDim, int curDimIdx, size_t* dimsizes,
//                   int* n, size_t* S, int ndims, size_t* i,  unsigned int* totals, int &totalsIndex);
  private:
    CMUMutualInformation(const CMUMutualInformation&); // Copy Constructor Not Implemented
    void operator=(const CMUMutualInformation&); // Operator '=' Not Implemented
};

#endif /* CMUMUTUALINFORMATION_H_ */
