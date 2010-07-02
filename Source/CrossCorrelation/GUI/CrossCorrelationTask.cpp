///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "CrossCorrelationTask.h"

// C++ Includes
#include <iostream>
#include <sstream>

// Qt Includes
#include <QtCore/QDir>
#include <QtCore/QLocale>

#include "CrossCorrelation/CrossCorrelation.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationTask::CrossCorrelationTask(QObject* parent) :
  ProcessQueueTask(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationTask::~CrossCorrelationTask()
{
 // std::cout << "   ~CrossCorrelationTask()" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationTask::run()
{
//  std::cout << "  CrossCorrelationTask::run()" << std::endl;
//  std::cout << "    FixedImage: " << getInputFilePath().toStdString() << std::endl;
//  std::cout << "    MovingImage: " << getMovingImagePath().toStdString() << std::endl;
  UPDATE_PROGRESS(QString("Starting Registration"), 0);

  // Load the fixed image into an AIMIMage object
  AIMImage::Pointer fixedImage = loadImage(getInputFilePath());
  if (NULL == fixedImage.data() )
  {
    emit finished();
    emit finished(this);
    return;
  }
  UPDATE_PROGRESS(QString("Reading Fixed Image"), 10);

  // Load the moving image into an AIMImage Object
  AIMImage::Pointer movingImage = loadImage(getMovingImagePath());
  if (NULL == movingImage.data() )
  {
    emit finished();
    emit finished(this);
    return;
  }
  UPDATE_PROGRESS(QString("Reading Moving Image"), 20);
  // Create the CrossCorrelationData object and set all the fields
  m_CrossCorrelationData->setFixedSlice(0);
  m_CrossCorrelationData->setMovingSlice(1);
  m_CrossCorrelationData->setImageWidth(fixedImage->getImagePixelWidth());
  m_CrossCorrelationData->setImageHeight(fixedImage->getImagePixelHeight());

  CrossCorrelation::Pointer cc = CrossCorrelation::New();
  cc->setCrossCorrelationData(m_CrossCorrelationData);
  cc->setFixedImage(fixedImage);
  cc->setMovingImage(movingImage);
  cc->run();

  UPDATE_PROGRESS(QString("Complete"), 100);
  // Notify observers that we are finished
  emit finished();
  emit finished(this);
//  std::cout << "  CrossCorrelation Task Finished." << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::Pointer CrossCorrelationTask::loadImage(QString filePath)
{
  QImage image;
  AIMImage::Pointer aimImage = AIMImage::NullPointer();
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
AIMImage::Pointer CrossCorrelationTask::convertQImageToGrayScaleAIMImage(QImage image)
{
  AIMImage::Pointer aimImage = AIMImage::New();
  quint8* oImage = aimImage->allocateImageBuffer(image.width(), image.height(), true);
  if (NULL == oImage)
  {
    return AIMImage::NullPointer();
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

