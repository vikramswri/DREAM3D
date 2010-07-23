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
#ifndef R3DCROPIMAGE_H_
#define R3DCROPIMAGE_H_

#include <AIM/Common/AIMImage.h>
#include <AIM/ImageFilters/AIMImageFilter.h>

/**
* @class R3DCropGrayScaleImage R3DCropGrayScaleImage.h AIM/ImageFilters/AIMCropGrayScaleImage.h
* @brief This class will crop an existing image using the supplied insets.
* @author Michael A. Jackson for BlueQuartz Software
* @date Mar 19, 2009
* @version 1.0
*/
class AIMCropGrayScaleImage : public AIMImageFilter
{
  public:
    /**
     * @brief Constructor
     * @param imageData The Image to crop
     * @param insets The number of pixels to remove from the left, right, top, bottom of the image
     */
    AIMCropGrayScaleImage(AIMImage::Pointer imageData, int32_t* insets);

    virtual ~AIMCropGrayScaleImage();

    /**
     * @brief Crops the input image
     * @return Error code
     */
    int run();

    /**
     * @brief Returns the output of this filter. The contained pointer may be NULL if there was
     * an error during the filtering process.
     * @return Smart Pointer to the cropped image
     */
    AIMImage::Pointer getOutputImage();

  private:
    AIMImage::Pointer _inData;
    int _numChannels;
    int _insets[4];
    AIMImage::Pointer _outData;

    AIMCropGrayScaleImage(const AIMCropGrayScaleImage&);    // Copy Constructor Not Implemented
    void operator=(const AIMCropGrayScaleImage&);  // Operator '=' Not Implemented

};

#endif /* R3DCROPIMAGE_H_ */
