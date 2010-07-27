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
#ifndef AIMIMAGEFILTER_H_
#define AIMIMAGEFILTER_H_

#include <itkImage.h>
#include <itkImportImageFilter.h>
#include <itkExtractImageFilter.h>

namespace AIM
{
namespace ImageFilters
{
const unsigned int Dimension = 2;
}
}
typedef unsigned char PixelType;
typedef itk::ImportImageFilter<PixelType, AIM::ImageFilters::Dimension> ImportFilterType;
typedef itk::Image<PixelType, AIM::ImageFilters::Dimension> InputImageType;
typedef itk::Image<PixelType, AIM::ImageFilters::Dimension> OutputImageType;


/**
* @class AIMImageFilter AIMImageFilter.h AIM/ImageFilters/AIMImageFilter.h
* @brief Base class for other filters
* @author Michael A. Jackson for BlueQuartz Software
* @date Mar 19, 2009
* @version 1.0
*/
class AIMImageFilter
{
  public:
    AIMImageFilter();
    virtual ~AIMImageFilter();

    /**
     * @brief Runs the filter
     * @return Non Zero on Success
     */
    virtual int run() = 0;

    /**
     * @brief Initializes an ITK import filter with a raw array of data
     * @param importFilter The import filter to initialize
     * @param imageData Pointer to the raw data
     * @param width The width of the image
     * @param height The height of the image
     */
    virtual void initializeImportFilter(ImportFilterType::Pointer importFilter,
                                                          PixelType* imageData,
                                                          int width, int height);


  private:
    AIMImageFilter(const AIMImageFilter&);    // Copy Constructor Not Implemented
    void operator=(const AIMImageFilter&);  // Operator '=' Not Implemented
};

#endif /* AIMIMAGEFILTER_H_ */
