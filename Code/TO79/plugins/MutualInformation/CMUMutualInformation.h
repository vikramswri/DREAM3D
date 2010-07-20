///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

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


    int mutualInfomation(AIMArray<uint32_t>::Pointer jointHistogram, int level = -1);


  private:
    CMUMutualInformation(const CMUMutualInformation&); // Copy Constructor Not Implemented
    void operator=(const CMUMutualInformation&); // Operator '=' Not Implemented
};

#endif /* CMUMUTUALINFORMATION_H_ */
