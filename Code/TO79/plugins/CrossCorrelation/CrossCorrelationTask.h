///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CrossCorrelation_TASK_H_
#define _CrossCorrelation_TASK_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtGui/QImage>

#include "QtSupport/ProcessQueueTask.h"
#include "AIM/Common/AIMImage.h"

#include "CrossCorrelation/CrossCorrelationData.h"


#define UPDATE_PROGRESS(m, p)\
  emit progressTextChanged( (m) );\
  emit progressValueChanged( (p) );

/**
* @class CrossCorrelationTask CrossCorrelationTask.h CrossCorrelation/GUI/CrossCorrelationTask.h
* @brief THis is the wrapper code for the code. This is called as a "worker" class from a separate thread
* of execution in order to not lock up the GUI.
* @author Michael A. Jackson for BlueQuartz Software
* @date Dec 20, 2009
* @version 1.0
*/
class CrossCorrelationTask : public ProcessQueueTask
{

  Q_OBJECT;

  public:
    CrossCorrelationTask(QObject* parent = 0);
    virtual ~CrossCorrelationTask();

    MXA_INSTANCE_PROPERTY_m(QString, MovingImagePath);
    MXA_INSTANCE_PROPERTY_m(CrossCorrelationData::Pointer, CrossCorrelationData)

    virtual void run();

protected:
    AIMImage::Pointer loadImage(QString filePath);

    AIMImage::Pointer convertQImageToGrayScaleAIMImage(QImage image);

  private:
    std::string fxImage;
    std::string mvImage;

    AIMImage::Pointer m_FixedImage;
    AIMImage::Pointer m_MovingImage;

    CrossCorrelationTask(const CrossCorrelationTask&); // Copy Constructor Not Implemented
    void operator=(const CrossCorrelationTask&); // Operator '=' Not Implemented

};





#endif /* _CrossCorrelation_TASK_H_ */
