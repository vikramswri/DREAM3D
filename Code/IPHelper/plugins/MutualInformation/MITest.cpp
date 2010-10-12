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

#include "CMUMutualInformation.h"

#include <iostream>
#include <fstream>

#include <QtCore/QString>
#include <QtGui/QImage>

#include "AIM/ImageFilters/AIMCropGrayScaleImage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage convertAIMArrayToGrayScaleQImage(AIMArray<uint8_t>::Pointer aimImage)
{
 // AIMArray<uint8_t>::Pointer aimImage = AIMArray<uint8_t>::New();
//  quint8* oImage = aimImage->allocateDataArray(image.width() * image.height(), true);
//  if (NULL == oImage)
//  {
//    return AIMArray<uint8_t>::NullPointer();
//  }

  QImage image(aimImage->getPointer(0), aimImage->getDimensions().at(0), aimImage->getDimensions().at(1), QImage::Format_Indexed8 );
  // Copy the QImage into the AIMImage object, converting to gray scale as we go.
  QVector<QRgb> colorTable(256);
  for (quint32 i = 0; i < 256; ++i)
  {
    colorTable[i] = qRgb(i, i, i);
  }
  image.setColorTable(colorTable);

  return image;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMArray<uint8_t>::Pointer convertQImageToGrayScaleAIMArray(QImage image)
{
  AIMArray<uint8_t>::Pointer aimImage = AIMArray<uint8_t>::New();
  quint8* oImage = aimImage->allocateDataArray(image.width() * image.height(), true);
  if (NULL == oImage)
  {
    return AIMArray<uint8_t>::NullPointer();
  }

  // Copy the QImage into the AIMImage object, converting to gray scale as we go.
  qint32 height = image.height();
  qint32 width = image.width();
  QRgb rgbPixel;
  int gray;
  qint32 index;
  for (qint32 y=0; y<height; y++) {
    for (qint32 x =0; x<width; x++) {
      index = (y *  width) + x;
      rgbPixel = image.pixel(x, y);
      gray = qGray(rgbPixel);
      oImage[index] = static_cast<unsigned char>(gray);
    }
  }
  std::vector<size_t> dims;
  dims.push_back(width);
  dims.push_back(height);
  aimImage->setDimensions(dims);
  return aimImage;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMArray<uint8_t>::Pointer loadImage(QString filePath)
{
  QImage image;
  AIMArray<uint8_t>::Pointer aimImage = AIMArray<uint8_t>::NullPointer();
  if (filePath.isEmpty() == false)
  {
    image = QImage(filePath);
    if (image.isNull() == true)
    {
      return aimImage;
    }
    QVector<QRgb> colorTable(256);
    for (quint32 i = 0; i < 256; ++i)
    {
      colorTable[i] = qRgb(i, i, i);
    }
    image.setColorTable(colorTable);
    aimImage = convertQImageToGrayScaleAIMArray(image);
    if (NULL == aimImage.data())
    {
      return aimImage;
    }
  }
  return aimImage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CropInputImage(AIMArray<uint8_t>::Pointer i0, AIMArray<uint8_t>::Pointer i1)
{
  std::vector<size_t> dimsI0 = i0->getDimensions();
  std::vector<size_t> dimsI1 = i1->getDimensions();

  int32_t insets[4] = { 0, 0, 0, 0};
  insets[3] = dimsI1[1] - dimsI0[1];

  AIMImage::Pointer image = AIMImage::New();
  image->setImageBuffer(i1->getPointer(0), true);
  i1->setManageMemory(false);
  image->setImagePixelDimension(i1->getDimensions().at(0), i1->getDimensions().at(1));

  AIMCropGrayScaleImage crop(image, insets);
  crop.run();
  AIMImage::Pointer croppedImage = crop.getOutputImage();
  croppedImage->setManageMemory(false);
  dimsI1[0] = croppedImage->getImagePixelWidth();
  dimsI1[1] = croppedImage->getImagePixelHeight();
  i1->setArrayData(croppedImage->getImageBuffer(), dimsI1, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << "Mutual Information Test" << std::endl;


  std::vector<AIMArray<uint8_t>::Pointer> images;
  AIMArray<uint8_t>::Pointer i0 = loadImage(QString("/private/tmp/Cropped.tif"));
  AIMArray<uint8_t>::Pointer i1 = loadImage(QString("/private/tmp/out.tif"));
//  AIMArray<uint8_t>::Pointer i2 = loadImage(QString("/Users/mjackson/Contracts/AFRL-TO79/Mutual_Info_CMU/6044.tif"));
//  AIMArray<uint8_t>::Pointer i3 = loadImage(QString("/Users/mjackson/Contracts/AFRL-TO79/Mutual_Info_CMU/7044.tif"));


  // Make sure all images are the same size. for this test we know the first image
  // is shorter vertically than the other images so we will just simply chop off
  // the bottom portion of the other images

#if 0
  CropInputImage(i0, i1);
  CropInputImage(i0, i2);
  CropInputImage(i0, i3);

  QImage qimage1 = convertAIMArrayToGrayScaleQImage(i1);
  bool ok = qimage1.save(QString("/tmp/i1.tif"));
  if (ok == false)
  {
    std::cout << "Error saving QImage" << std::endl;
  }

#endif

  images.push_back(i0);
  images.push_back(i1);
//  images.push_back(i2);
//  images.push_back(i3);

  int binSize = 4;

  CMUMutualInformation mi;

  AIMArray<uint32_t>::Pointer jointHistogram = mi.jointHistogram(images, binSize, false);
  AIMArray<double>::Pointer normalizedJointHistogram = jointHistogram->normalize<double>();
  double min, max;
  normalizedJointHistogram->minMax(min, max);
  std::cout << "Normalized Histogram Min/Max: " << min << "  " << max << std::endl;
  int level = -1;
  double H = mi.mutualInfomation(normalizedJointHistogram, level);
  std::cout << "Mutual Information: " << H << std::endl;
//
//  std::ofstream ofile("/tmp/joinhistogram.raw", std::ios::binary);
//  size_t length = jointHistogram->getNumElements();
//  ofile.write( reinterpret_cast<char *>(jointHistogram->getPointer(0)), length * 4);
//  ofile.flush();

  std::cout << "Done" << std::endl;
  return EXIT_SUCCESS;
}
