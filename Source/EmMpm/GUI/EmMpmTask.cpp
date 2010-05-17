///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "EmMpmTask.h"


#include <QtCore/QDir>
#include <QtCore/QLocale>

#include <iostream>





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmMpmTask::EmMpmTask(QObject* parent) :
  QObject(parent),
  m_Debug(false),
  m_Cancel(false),
  m_Beta(0.1f),
  m_Gamma(0.0f),
  m_EmIterations(5),
  m_MpmIterations(5),
  m_UseSimulatedAnnealing(false)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EmMpmTask::~EmMpmTask()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmMpmTask::run()
{
 // std::cout << "  EmMpmTask::run()" << std::endl;
  UPDATE_PROGRESS(QString("Starting Segmentation"), 0);

  execute<uint8_t, uint8_t>( m_OriginalImage->getImageBuffer(), m_SegmentedImage->getImageBuffer());

  emit sendTaskFinished();

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EmMpmTask::cancelTask()
{
  this->m_Cancel = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EmMpmTask::isCanceled()
{
  return m_Cancel;
}


void EmMpmTask::setBeta(float beta)
{
  m_Beta = beta;
}
void EmMpmTask::setGamma(float gamma)
{
 m_Gamma = gamma;
}
void EmMpmTask::setEmIterations(int emIterations)
{
  m_EmIterations = emIterations;
}
void EmMpmTask::setMpmIterations(int mpmIterations)
{
  m_MpmIterations = mpmIterations;
}
void EmMpmTask::setNumberOfClasses(int numClasses)
{
 m_NumberOfClasses = numClasses;
}

void EmMpmTask::useSimulatedAnnealing()
{
  m_UseSimulatedAnnealing = true;
}

