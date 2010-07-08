///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROCESSQUEUETASK_H_
#define PROCESSQUEUETASK_H_

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtGui/QImage>

#include <MXA/Common/MXASetGetMacros.h>
#include "AIM/Common/AIMImage.h"

class ProcessQueueTask : public QThread
{
    Q_OBJECT;

  public:
    ProcessQueueTask(QObject* parent = 0);
    virtual ~ProcessQueueTask();

    /**
     * @brief Is the task canceled
     * @return
     */
    bool isCanceled();
    MXA_INSTANCE_PROPERTY_m(bool, Debug);
    MXA_VIRTUAL_INSTANCE_PROPERTY_m(QString, InputFilePath);
    MXA_VIRTUAL_INSTANCE_PROPERTY_m(QString, OutputFilePath);

    AIMImage::Pointer convertQImageToGrayScaleAIMImage(QImage image);

    signals:

    /**
     * @brief Signal sent when the encoder has a message to relay to the GUI or other output device.
     */
      void progressTextChanged ( const QString & progressText );

      /**
       * @brief Signal sent to the GUI to indicate progress of the encoder which is an integer value between 0 and 100.
       * @param value
       */
      void progressValueChanged(int value);

      /**
       * @brief Signal sent when the encoding task is complete
       */
      void finished();
      void finished(QObject *);

    public slots:

      /**
       * @brief Slot to receive a signal to cancel the operation
       */
      void cancel();

      virtual void run() = 0;

    private:
      bool m_Cancel;

      ProcessQueueTask(const ProcessQueueTask&); // Copy Constructor Not Implemented
      void operator=(const ProcessQueueTask&); // Operator '=' Not Implemented


};

#endif /* PROCESSQUEUETASK_H_ */
