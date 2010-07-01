///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ProcessQueueTask.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueueTask::ProcessQueueTask(QObject* parent) :
QThread(parent),
m_Debug(true),
m_Cancel(false)
{
  // TODO Auto-generated constructor stub

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueueTask::~ProcessQueueTask()
{
  // TODO Auto-generated destructor stub
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueueTask::cancel()
{
  this->m_Cancel = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ProcessQueueTask::isCanceled()
{
  return m_Cancel;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::Pointer ProcessQueueTask::convertQImageToGrayScaleAIMImage(QImage image)
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

