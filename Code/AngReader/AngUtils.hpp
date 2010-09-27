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

#ifndef _AngUtils_hpp_
#define _AngUtils_hpp_

#include "AngReader.h"
#include "OIMColoring.hpp"
#include "AIM/Common/AIMArray.hpp"

class AngUtils
{
  public:
    typedef AIMArray<unsigned char>           RGBArrayType;
    typedef AIMArray<unsigned char>           GrayScaleType;

    AngUtils() {}
    virtual ~AngUtils() {}

    GrayScaleType::Pointer convertAngToGrayScaleImage(const std::string &angFileToRead)
    {
      int err = 0;

      // Prime some values by reading a single ANG file
      AngReader reader;
      reader.setUserOrigin(AngReader::UpperRightOrigin);
      int curZ = 0;
      std::cout << curZ << " Reading File " << angFileToRead << std::endl;
      reader.setFileName(angFileToRead);
      err = reader.readFile();
      if (1 != err)
      {
        return GrayScaleType::NullPointer();

      }
      int xpoints = reader.getNumEvenCols();
      int ypoints = reader.getNumRows();
      //int zpoints = inputs->angEndSlice - inputs->angStartSlice + 1; // Add 1 because we are inclusive of the last file

      float* phi1F = reader.getPhi1Pointer();
      float* phiF = reader.getPhiPointer();
      float* phi2F = reader.getPhi2Pointer();

      float refDir0 = 0.0f;
      float refDir1 = 0.0f;
      float refDir2 = 1.0f; // ND Normal Direction

      unsigned int rgba = 0x00000000;
      unsigned char* rgbPtr = reinterpret_cast<unsigned char* > (&rgba);

      GrayScaleType::Pointer grayImage = GrayScaleType::New();
      grayImage->allocateDataArray(xpoints * ypoints, true);
      unsigned char* grayBuffer = grayImage->getArrayData();

      size_t index = 0;

      for (int y = 0; y < ypoints; ++y)
      {
        for (int x = 0; x < xpoints; ++x)
        {
          rgba = 0;
          OIMColoring::GenerateIPFColor<float >(phi1F[index], phiF[index], phi2F[index], refDir0, refDir1, refDir2, rgbPtr);
          grayBuffer[index * 3] =  (unsigned char)((float)rgbPtr[0] * 0.299f + (float)rgbPtr[1] * 0.587f + (float)rgbPtr[2] * 0.114f);
          ++index;
        }
      }
      return grayImage;
    }

    /**
     *
     * @param angFileToRead
     * @return
     */
    RGBArrayType::Pointer convertAngToColorIPF(const std::string &angFileToRead)
    {
      int err = 0;

      // Prime some values by reading a single ANG file
      AngReader reader;
      reader.setUserOrigin(AngReader::UpperRightOrigin);
      int curZ = 0;
      std::cout << curZ << " Reading File " << angFileToRead << std::endl;
      reader.setFileName(angFileToRead);
      err = reader.readFile();
      if (1 != err)
      {
        return RGBArrayType::NullPointer();

      }
      int xpoints = reader.getNumEvenCols();
      int ypoints = reader.getNumRows();
      //int zpoints = inputs->angEndSlice - inputs->angStartSlice + 1; // Add 1 because we are inclusive of the last file

      float* phi1F = reader.getPhi1Pointer();
      float* phiF = reader.getPhiPointer();
      float* phi2F = reader.getPhi2Pointer();

      float refDir0 = 0.0f;
      float refDir1 = 0.0f;
      float refDir2 = 1.0f; // ND Normal Direction

      unsigned int rgba = 0x00000000;
      unsigned char* rgbPtr = reinterpret_cast<unsigned char* > (&rgba);

      RGBArrayType::Pointer rgbImage = RGBArrayType::New();
      rgbImage->allocateDataArray(xpoints * ypoints * 3, true);
      unsigned char* tiffBuffer = rgbImage->getArrayData();

      size_t index = 0;
      //      SimpleVolumeIntType labelling(new m3c_basics::datastructures::SimpleVolume<Label >(xpoints, ypoints, zpoints));
      //      std::cout << logTime() << " Volume Size: " << xpoints << " " << ypoints << " " << zpoints << std::endl;

      //      std::set<unsigned int > myLabels;

      for (int y = 0; y < ypoints; ++y)
      {
        for (int x = 0; x < xpoints; ++x)
        {
          rgba = 0;
          OIMColoring::GenerateIPFColor<float >(phi1F[index], phiF[index], phi2F[index], refDir0, refDir1, refDir2, rgbPtr);
          tiffBuffer[index * 3] = rgbPtr[0];
          tiffBuffer[index * 3 + 1] = rgbPtr[1];
          tiffBuffer[index * 3 + 2] = rgbPtr[2];
          ++index;
        }
      }
      return rgbImage;
    }

  protected:


  private:
      AngUtils(const AngUtils&); // Copy Constructor Not Implemented
      void operator=(const AngUtils&); // Operator '=' Not Implemented
};

#endif /* _AngUtils_hpp_  */
