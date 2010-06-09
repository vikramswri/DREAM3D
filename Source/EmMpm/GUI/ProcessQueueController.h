///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROCESSQUEUECONTROLLER_H_
#define PROCESSQUEUECONTROLLER_H_

#include <QtCore/QThread>
#include <QtCore/QVector>

class ProcessQueue;


class ProcessQueueController : public QThread
{
    Q_OBJECT;

  public:
    /**
     * @brief Standard Qt constructor.
     * @param queue
     * @param parent The QtObject parent of this object.
     * @return Properly constructed EmMpmThread Object
     */
    ProcessQueueController(QObject* parent = 0);

    /**
     * Destructor
     */
    virtual ~ProcessQueueController();

    /**
     * @brief Sets the flag to automatically delete the task when complete
     * @param deleteTask Delete the Encoder task when encoding is complete.
     */
    void setAutoDeleteQueue(bool deleteQueue);

    void addTask(QThread* t);

  protected:

    /**
     * @brief This is the entry point for the task. This is called from the QThread::Started signal.
     */
    virtual void run();

  public slots:

  void processTask();

  private:
    QVector<QThread*>  m_Tasks;
    QVector<QThread*>  m_CompletedTasks;
    int m_MaxThreads;
    int m_ThreadCount;

    bool            m_AutoDelete;
    ProcessQueueController(const ProcessQueueController&);    // Copy Constructor Not Implemented
    void operator=(const ProcessQueueController&);  // Operator '=' Not Implemented

};

#endif /* PROCESSQUEUECONTROLLER_H_ */
