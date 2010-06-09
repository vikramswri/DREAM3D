///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include "ProcessQueue.h"
#include "EmMpmTask.h"
#include <QtGui/QApplication>

#define UPDATE_PROGRESS(m, p)\
  emit sendTaskMessage( (m) );\
  emit sendTaskProgress( (p) );


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueue::ProcessQueue(QObject* parent) :
  QObject(parent),
  m_Debug(false),
  m_Cancel(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueue::~ProcessQueue()
{
  std::cout << "   ~ProcessQueue()" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueue::addTask(QRunnable* t)
{
  if (NULL == t){ return; }
  if (m_Tasks.contains( t) == false)
  {
    m_Tasks.push_back(t);

  }
}


// -----------------------------------------------------------------------------
// This is hooked up to the ProcessQueueController::Started() signal
// -----------------------------------------------------------------------------
void ProcessQueue::run()
{
  std::cout << " ProcessQueue::run()" << std::endl;
  EmMpmTask* task = NULL;
  qint32 count = m_Tasks.count();

  for (qint32 i = 0; i < count; ++i)
  {
    task = dynamic_cast<EmMpmTask*>(m_Tasks.at(i));
    std::cout << "   EmMpmTask with input file: " << task->getInputFilePath().toStdString() << std::endl;
    task->run();
  //  QApplication::processEvents();
  }
  emit finished();
}


// -----------------------------------------------------------------------------
// This is usually hooked up from the Main UI
// -----------------------------------------------------------------------------
void ProcessQueue::cancel()
{
  this->m_Cancel = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ProcessQueue::isCanceled()
{
  return m_Cancel;
}

