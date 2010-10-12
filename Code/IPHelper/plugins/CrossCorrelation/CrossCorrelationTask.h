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

    MXA_INSTANCE_PROPERTY(QString, MovingImagePath);
    MXA_INSTANCE_PROPERTY(CrossCorrelationData::Pointer, CrossCorrelationData)

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
