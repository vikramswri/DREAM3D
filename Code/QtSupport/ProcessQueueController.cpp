///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ProcessQueueController.h"

#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueueController::ProcessQueueController(QObject* parent) :
QThread(parent),
m_AutoDelete(true)
{
  m_MaxThreads = QThread::idealThreadCount();
  m_ThreadCount = 1; // We need this to be 1 because the first time we will decrement the value
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueueController::~ProcessQueueController()
{
  for (int i = 0; i < m_CompletedTasks.count(); ++i)
  {
    QThread* t = m_CompletedTasks.at(i);
    t->deleteLater(); // Clean up the memory
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueueController::run()
{
  exec(); // <== Starts the Event Loop for this thread. Will BLOCK here until the Quit Slot is called
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueueController::setAutoDeleteQueue(bool deleteQueue)
{
  this->m_AutoDelete = deleteQueue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueueController::addTask(QThread* t)
{
  if (NULL == t){ return; }
  if (m_Tasks.contains( t) == false)
  {
    m_Tasks.push_back(t);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueueController::processTask()
{
  --m_ThreadCount; //decrement the value as this is called when another thread gets finished
  while (m_ThreadCount < m_MaxThreads)
  {
    if (m_Tasks.count() > 0)
    {
      QThread* task = m_Tasks.front();
      m_Tasks.pop_front(); // Remove the thread from the QVector
      m_CompletedTasks.push_back(task);
      connect(task, SIGNAL(finished()), this, SLOT(processTask()));
      task->start();
      m_ThreadCount++;
    }
    else
    {
      break;
    }
  }
  if (m_ThreadCount == 0)
  {
    //  std::cout << "Last Thread has finished." << std::endl;
    quit(); // Tells the event loop to exit, which will then exit the thread.
  }
}
