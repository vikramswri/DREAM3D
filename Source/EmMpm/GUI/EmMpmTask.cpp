///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "EmMpmTask.h"


#include <QtCore/QDir>
#include <QtCore/QLocale>

#include <iostream>





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmMpmTask::EmMpmTask(QObject* parent) :
  QThread(parent),
  m_Debug(false),
  m_Cancel(false),
  m_Beta(0.1f),
  m_Gamma(0.0f),
  m_EmIterations(5),
  m_MpmIterations(5),
  m_UseSimulatedAnnealing(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmMpmTask::~EmMpmTask()
{
//  std::cout << "   ~EmMpmTask()" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmMpmTask::run()
{
//  std::cout << "  EmMpmTask::run()" << std::endl;
  UPDATE_PROGRESS(QString("Starting Segmentation"), 0);

  // Load the image from the input file
  QImage image(m_InputFilePath);
  m_OriginalImage = convertQImageToGrayScaleAIMImage(image);
  if (NULL == m_OriginalImage.data())
  {
    std::cout << "Error loading image '" << m_InputFilePath.toStdString() << "'" << std::endl;
    emit sendTaskFinished();
    return;
  }

  // Allocate memory for the segmented image
  m_SegmentedImage = AIMImage::New();
  m_SegmentedImage->allocateSameSizeImage(m_OriginalImage);
  if (NULL == m_SegmentedImage)
  {
    std::cout << "Error Allocating Memory for segmented Image." << std::endl;
    emit sendTaskFinished();
    return;
  }


  execute<uint8_t, uint8_t>( m_OriginalImage->getImageBuffer(), m_SegmentedImage->getImageBuffer());


  // Save the image to the output file:
  qint32 width = m_SegmentedImage->getImagePixelWidth();
  qint32 height = m_SegmentedImage->getImagePixelHeight();
  quint8* dataPointer = m_SegmentedImage->getImageBuffer();
  QImage outImage(width, height, QImage::Format_Indexed8);
  QVector<QRgb> colorTable(256);
  for (quint32 i = 0; i < 256; ++i)
  {
    colorTable[i] = qRgb(i, i, i);
  }
  outImage.setColorTable(colorTable);
  qint32 index;
  for (qint32 j=0; j<height; j++)
  {
    for (qint32 i =0; i<width; i++)
    {
      index = (j * width) + i;
      outImage.setPixel(i, j, dataPointer[index]);
    }
  }
  bool ok = outImage.save(m_OutputFilePath);
  if (false == ok)
  {
    std::cout << "Error saving segmented image to '" << m_OutputFilePath.toStdString() << "'" << std::endl;
  }
  // Clean up memory usage
  m_SegmentedImage = AIMImage::NullPointer();
  m_OriginalImage = AIMImage::NullPointer();

  // Notify observers that we are finished
  emit sendTaskFinished();
  std::cout << "  EmMpm Task Finished." << std::endl;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmMpmTask::cancelTask()
{
  this->m_Cancel = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EmMpmTask::isCanceled()
{
  return m_Cancel;
}


void EmMpmTask::setBeta(float beta)
{
  m_Beta = beta;
}
void EmMpmTask::setGamma(float gamma)
{
 m_Gamma = gamma;
}
void EmMpmTask::setEmIterations(int emIterations)
{
  m_EmIterations = emIterations;
}
void EmMpmTask::setMpmIterations(int mpmIterations)
{
  m_MpmIterations = mpmIterations;
}
void EmMpmTask::setNumberOfClasses(int numClasses)
{
 m_NumberOfClasses = numClasses;
}

void EmMpmTask::useSimulatedAnnealing()
{
  m_UseSimulatedAnnealing = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::Pointer EmMpmTask::convertQImageToGrayScaleAIMImage(QImage image)
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

