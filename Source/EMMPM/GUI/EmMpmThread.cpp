///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#include "EmMpmThread.h"

#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmMpmThread::EmMpmThread(EmMpmTask* task, QObject* parent) :
QThread(parent),
m_Task(task),
m_AutoDelete(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmMpmThread::~EmMpmThread()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmMpmThread::run()
{
  exec(); // <== Starts the Event Loop for this thread. Will BLOCK here until the Quit Slot is called
  if (m_AutoDelete == true)
  {
    delete m_Task;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmMpmThread::setAutoDeleteTask(bool deleteTask)
{
  this->m_AutoDelete = deleteTask;
}
