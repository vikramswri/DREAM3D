

#include "CMUMutualInformation.h"

#include <iostream>
#include <fstream>

#include <QtCore/QString>
#include <QtGui/QImage>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMArray<uint8_t>::Pointer convertQImageToGrayScaleAIMImage(QImage image)
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
    aimImage = convertQImageToGrayScaleAIMImage(image);
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
int main(int argc, char **argv)
{


  std::cout << "Mutual Information Test" << std::endl;

  std::vector<AIMArray<uint8_t>::Pointer> images;

  AIMArray<uint8_t>::Pointer i0 = loadImage(QString("/Users/mjackson/Contracts/AFRL-TO79/Mutual_Info_CMU/6044.tif"));
  AIMArray<uint8_t>::Pointer i1 = loadImage(QString("/Users/mjackson/Contracts/AFRL-TO79/Mutual_Info_CMU/5044.tif"));
  images.push_back(i0);
  images.push_back(i1);


#if 0
  unsigned char v0[4] =
  { 1,5,9,13};
  unsigned char v1[4] =
  { 2,6,10,14};
  unsigned char v2[4] =
  { 3,7,11,15};

  ImageColumnType::Pointer V0 = ImageColumnType::New();
  V0->initializeImageWithSourceData(4, v0);

  ImageColumnType::Pointer V1 = ImageColumnType::New();
  V1->initializeImageWithSourceData(4, v1);

  ImageColumnType::Pointer V2 = ImageColumnType::New();
  V2->initializeImageWithSourceData(4, v2);

  images.push_back(V0);
  images.push_back(V1);
  images.push_back(V2);
#endif


  int binSize = 1;

  CMUMutualInformation mi;

  AIMArray<uint32_t>::Pointer jointHistogram = mi.jointHistogram(images, binSize, false);
  mi.mutualInfomation(jointHistogram);

  std::ofstream ofile("/tmp/joinhistogram.raw", std::ios::binary);
  size_t length = jointHistogram->getNumElements();
  ofile.write( reinterpret_cast<char *>(jointHistogram->getPointer(0)), length * 4);
  ofile.flush();

  std::cout << "Done" << std::endl;
  return EXIT_SUCCESS;
}
