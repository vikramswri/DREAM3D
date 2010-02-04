///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SHPTHREAD_H_
#define SHPTHREAD_H_

#include <QtCore/QThread>

#include "EmMpmTask.h"

/**
* @class EmMpmThread EmMpmThread.h SHP/GUI/EmMpmThread.h
* @brief This class extends the QThread to take ownership of the EmMpmTask object, run the encoding and delete
* the object when the encoding is complete.
* @author Michael A. Jackson for BlueQuartz Software
* @date Dec 20, 2009
* @version 1.0
*/
class EmMpmThread : public QThread
{
    Q_OBJECT;

  public:
    /**
     * @brief Standard Qt constructor.
     * @param task The EmMpmTask Object which has already been instantiated
     * @param parent The QtObject parent of this object.
     * @return Properly constructed EmMpmThread Object
     */
    EmMpmThread(EmMpmTask* task, QObject* parent = 0);

    /**
     * Destructor
     */
    virtual ~EmMpmThread();

    /**
     * @brief Sets the flag to automatically delete the task when complete
     * @param deleteTask Delete the Encoder task when encoding is complete.
     */
    void setAutoDeleteTask(bool deleteTask);

  protected:

    /**
     * @brief This is the entry point for the SHP encoding task. This is called from the QThread::Started signal.
     */
    virtual void run();

  private:
    EmMpmTask* m_Task;
    bool            m_AutoDelete;
    EmMpmThread(const EmMpmThread&);    // Copy Constructor Not Implemented
    void operator=(const EmMpmThread&);  // Operator '=' Not Implemented
};

#endif /* SHPTHREAD_H_ */
