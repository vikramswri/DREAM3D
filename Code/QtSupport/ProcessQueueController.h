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

/**
 * @class ProcessQueueController ProcessQueueController.h QtSupport/ProcessQueueController.h
 * @brief A QThread derived class that uses Signals and Slots to controll a Task Queue
 * system. The maximum number of threads is based on the maximum number of virtual
 * cores/CPUs that are found using the function QThread::idealThreadCount();.
 *  This class works in conbination with the ProcessQueueTask
 *  class in which you define the code that you would like to be run on a separate
 *  thread. This design implementation ensures that Qt's signals and slots between
 *  each task are delivered and executed properly across threads. This is accmplished
 *  by the use of signals and slots to start/end the ProcessTask.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 26, 2010
 * @version 1.0
 */
class ProcessQueueController : public QThread
{
    Q_OBJECT;

  public:
    /**
     * @brief Standard Qt constructor.
     * @param parent The QObject parent of this object.
     * @return Properly constructed EmMpmThread Object
     */
    ProcessQueueController(QObject* parent = 0);

    /**
     * Destructor
     */
    virtual ~ProcessQueueController();

    /**
     * @brief Sets the flag to automatically delete the task when complete
     * @param deleteQueue Delete the Encoder task when encoding is complete.
     */
    void setAutoDeleteQueue(bool deleteQueue);

    /**
     * @brief Adds a ProcessQueueTask object to this controller
     * @param t The task to add.
     */
    void addTask(QThread* t);

  protected:

    /**
     * @brief This is the entry point for the task. This is called from the QThread::started signal.
     */
    virtual void run();

  public slots:

    /**
     * @brief Slot that is hooked up to a "finished()" signal from another task. Chaining
     * the signals and slots in this way will ensure that a single new process is begun
     * processing when another task is finished.
     */
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
