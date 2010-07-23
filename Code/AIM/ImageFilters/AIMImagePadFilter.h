///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef R3DIMAGEPADFILTER_H_
#define R3DIMAGEPADFILTER_H_

#include "AIM/Common/AIMImage.h"
#include "AIM/ImageFilters/AIMImageFilter.h"

/**
* @class R3DImagePadFilter R3DImagePadFilter.h AIM/ImageFilters/AIMImagePadFilter.h
* @brief This class will pad an existing image by the amount given by the "pixelPad" value in
* the constructor.
* @author Michael A. Jackson for BlueQuartz Software
* @date July 06, 2010
* @version 1.0
*/
class AIMImagePadFilter : public AIMImageFilter
{
  public:
    /**
     *
     * @param imageData The AIMImage to pad
     * @param pixelPad The number of pixels to add to the left, right, top, bottom of the image
     * @param padValue The value of the pixel
     * @return
     */
    AIMImagePadFilter(AIMImage::Pointer imageData, int32_t pixelPad[4], int32_t padValue);
    virtual ~AIMImagePadFilter();

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

  protected:


  private:
    AIMImage::Pointer m_InData;
    int32_t m_PixelPad[4];
    int32_t m_PadValue;
    AIMImage::Pointer m_OutData;

    AIMImagePadFilter(const AIMImagePadFilter&); // Copy Constructor Not Implemented
    void operator=(const AIMImagePadFilter&); // Operator '=' Not Implemented

};

#endif /* R3DIMAGEPADFILTER_H_ */
