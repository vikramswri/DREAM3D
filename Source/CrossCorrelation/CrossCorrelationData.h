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
#ifndef _CrossCorrelationDATA_H_
#define _CrossCorrelationDATA_H_

#include <MXA/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/Common/IO/MXAFileWriter64.h>
#include <MXA/Common/IO/MXAFileReader64.h>

/**
* @class CrossCorrelationData CrossCorrelationData.h CrossCorrelation/CrossCorrelationData.h
* @brief This class encapsulates the data needed as inputs and outputs from a run of the PCM Registration class. This
* class knows how to read and write itself from a binary file and can print itself to any std::ostream as binary or
* ascii data.
* @author Michael A. Jackson for BlueQuartz Software
* @date June 16, 2009
* @version 1.0
*/
class CrossCorrelationData
{
  public:
    virtual ~CrossCorrelationData();

    MXA_SHARED_POINTERS(CrossCorrelationData);
    MXA_STATIC_NEW_MACRO(CrossCorrelationData);
    MXA_TYPE_MACRO(CrossCorrelationData);


    /**
     * @brief Static method to print the column headers for ascii output
     * @param out the ostream to print to
     * @param delimiter What type of delimiter to use: Tab, Comma, Space.
     */
    static void PrintAsciiHeader(std::ostream &out, const std::string &delimiter);


    /* The fixed Slice number */
    MXA_INSTANCE_PROPERTY(int32_t, FixedSlice, fixedSlice);
    /*
     * @brief The Moving Slice Number
     */
    MXA_INSTANCE_PROPERTY(int32_t, MovingSlice, movingSlice);
    /*
     * @brief The const function value
     */
    MXA_INSTANCE_PROPERTY(float, ConstFuncValue, costFuncValue);
    /*
     * @brief The number of iterations that were used to get the output
     */
    MXA_INSTANCE_PROPERTY(uint32_t, NumIterations, numIterations);

    /*
     * @brief The horizontal translation computed by the registration method. The value will be in physical units.
     */
    MXA_INSTANCE_PROPERTY(double, XTrans, xTrans);

    /*
     * @brief The vertical translation computed by the registration method in physical units.
     */
    MXA_INSTANCE_PROPERTY(double, YTrans, yTrans);

    /**
     * @brief Get the translations
     * @param trans Output variable
     */
    void getTranslations(double trans[2]);

    /**
     * @brief Set the translations
     * @param trans Input variable
     */
    void setTranslations(double trans[2]);

    /*
     * @brief The Horizontal origin of the fixed image
     */
    MXA_INSTANCE_PROPERTY(double, XFixedOrigin, xFixedOrigin);
    /*
     * @brief The vertical origin of the fixed image
     */
    MXA_INSTANCE_PROPERTY(double, YFixedOrigin, yFixedOrigin);
    /*
    * @brief The horizontal origin of the moving image
     */
    MXA_INSTANCE_PROPERTY(double, XMovingOrigin, xMovingOrigin);
    /*
     * @brief The vertical origin of the moving image
     */
    MXA_INSTANCE_PROPERTY(double, YMovingOrigin, yMovingOrigin);

    /*
     * @brief The scaling factor (Microns/pixel) value of the image
     */
    MXA_INSTANCE_PROPERTY(double, Scaling, scaling);

    /*
     * @brief The Width of the FFT image in pixels
     */
    MXA_INSTANCE_PROPERTY(int, ImageWidth, imageWidth);
    /*
     * @brief The Height of the FFT image in pixels
     */
    MXA_INSTANCE_PROPERTY(int, ImageHeight, imageHeight);

    /*
     * @brief Is this slice pair complete.
     */
    MXA_INSTANCE_PROPERTY(int, Complete, complete);


    /**
     * @brief Sets the fixed origin
     * @param origin Input values
     */
    void setFixedOrigin(float origin[2]);
    /**
     * @brief Gets the fixed image origin
     * @param origin Output variable
     */
    void getFixedOrigin(float origin[2]);

    /**
     * @brief Sets the moving origin
     * @param origin The input values
     */
    void setMovingOrigin(float origin[2]);
    /**
     * @brief Gets the moving origin
     * @param origin The output variable
     */
    void getMovingOrigin(float origin[2]);

    /**
     * @brief Will initialize all the internal values to defaults. Use this if you plan on reusing an instance of this
     * class for any reason.
     */
    void initValues();

    /**
     * @brief Writes out this object into a binary file
     * @param writer The MXAFileWriter64 Object to use for the writing
     * @return error condition
     */
    int32_t writeToFile(MXAFileWriter64 &writer);

    /**
     * @brief Reads the values from a file in binary form swapping the bytes if
     * requested.
     * @param reader The reader to use
     * @param swap Perform Endian Byte Swapping
     * @return
     */
    int32_t readFromFile(MXAFileReader64 &reader, bool swap = false);

    /**
     * @brief Prints the entry out to the std::ostream
     * @param out std::ostream to print to
     * @param delimiter The Delimiter to use between data
     */
    void print(std::ostream &out, const std::string &delimiter);

protected:
   CrossCorrelationData();


private:

    CrossCorrelationData(const CrossCorrelationData&);    // Copy Constructor Not Implemented
    void operator=(const CrossCorrelationData&);  // Operator '=' Not Implemented
};

#endif /* _CrossCorrelationDATA_H_ */
