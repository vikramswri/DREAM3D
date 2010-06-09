#error DO NOT INCLUDE THIS FILE
#ifndef PROCESSQUEUE_H_
#define PROCESSQUEUE_H_

#include <QtCore/QObject>
#include <QtCore/QRunnable>
#include <QtCore/QString>
#include <QtCore/QVector>

/**
* @class ProcessQueue ProcessQueue ProcessQueue.h
* @brief This class is a task that...
* @author Michael A. Jackson for BlueQuartz Software
* @date Jun 8, 2010
* @version 1.0
*/

class ProcessQueue : public QObject, public QRunnable
{

  Q_OBJECT

  public:
    ProcessQueue (QObject* parent = 0);
    virtual ~ProcessQueue ();

    void addTask(QRunnable* t);


    /**
     * @brief Is the task canceled
     * @return
     */
    bool isCanceled();

  signals:

    /**
     * @brief Signal sent when the encoding task is complete
     */
    void finished();

  public slots:
    /**
     * @brief Slot to receive a signal to cancel the operation
     */
    void cancel();

    virtual void run();

  protected:



  private:
		bool m_Debug;
    bool m_Cancel;

    QVector<QThread*>  m_Tasks;

    ProcessQueue (const ProcessQueue &); // Copy Constructor Not Implemented
    void operator=(const ProcessQueue &); // Operator '=' Not Implemented

};

#endif /* PROCESSQUEUE_H_ */


