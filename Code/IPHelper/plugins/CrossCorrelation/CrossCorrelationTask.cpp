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
  if (isCanceled() )
  {
  //  emit finished();
    emit taskFinished(this);
    return;
  }
  UPDATE_PROGRESS(QString("Starting Registration"), 0);
  fxImage = getInputFilePath().toStdString();
  mvImage = getMovingImagePath().toStdString();
  // Load the fixed image into an AIMIMage object
  AIMImage::Pointer fixedImage = loadImage(getInputFilePath());
  if (NULL == fixedImage.RAW_PTR() )
  {
  //  emit finished();
    emit taskFinished(this);
    return;
  }
  UPDATE_PROGRESS(QString("Reading Fixed Image"), 10);

  // Load the moving image into an AIMImage Object
  AIMImage::Pointer movingImage = loadImage(getMovingImagePath());
  if (NULL == movingImage.RAW_PTR() )
  {
 //   emit finished();
    emit taskFinished(this);
    return;
  }
  UPDATE_PROGRESS(QString("Reading Moving Image"), 20);
  // Create the CrossCorrelationData object and set all the fields
//  m_CrossCorrelationData->setFixedSlice(0);
//  m_CrossCorrelationData->setMovingSlice(1);
  m_CrossCorrelationData->setFixedImagePath(getInputFilePath().toStdString());
  m_CrossCorrelationData->setMovingImagePath(getMovingImagePath().toStdString());
  m_CrossCorrelationData->setImageWidth(fixedImage->getImagePixelWidth());
  m_CrossCorrelationData->setImageHeight(fixedImage->getImagePixelHeight());

  UPDATE_PROGRESS(QString("Registering Images"), 50);
  CrossCorrelation::Pointer cc = CrossCorrelation::New();
  cc->setCrossCorrelationData(m_CrossCorrelationData);
  cc->setFixedImage(fixedImage);
  cc->setMovingImage(movingImage);
  cc->setErrorCondition(1000);
  cc->run();
  if (cc->getErrorCondition() < 0)
  {
    QString msg("Error Registering Image ");
    msg.append(getInputFilePath() + " with image " + getMovingImagePath());
    std::cout << logTime() << msg.toStdString() << std::endl;
    UPDATE_PROGRESS(msg, 75)
  }

  UPDATE_PROGRESS(QString("Complete"), 100);
  // Notify observers that we are finished
 // emit finished();
  emit taskFinished(this);
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
    if (NULL == aimImage.RAW_PTR())
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

