///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ProcessQueueDialog.h"

#include <QtCore/QFileInfo>
#include <QtGui/QProgressBar>
#include <QtGui/QLabel>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueueDialog::ProcessQueueDialog(QWidget *parent) :
QDialog(parent)
{
  setupUi(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueueDialog::~ProcessQueueDialog()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueueDialog::clearTable()
{
  this->processTableWidget->clearContents();
  this->processTableWidget->setRowCount(0);
  m_TasksMap.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueueDialog::addProcess(ProcessQueueTask* task)
{
  qint32 rowCount = this->processTableWidget->rowCount();
  QProgressBar* progBar = new QProgressBar(this);
  progBar->setRange(0, 100);
  progBar->setTextVisible(true);
  progBar->setFormat("%p%");
  progBar->setAlignment(Qt::AlignBottom);

  QFileInfo fileInfo(task->getInputFilePath());

  this->processTableWidget->setRowCount(rowCount + 1);
  QLabel* label = new QLabel(fileInfo.fileName());
  label->setAlignment(Qt::AlignCenter);
  this->processTableWidget->setCellWidget(rowCount, 0, label);
  this->processTableWidget->setCellWidget(rowCount, 1, progBar);

  connect(task, SIGNAL(progressValueChanged(int)), progBar, SLOT(setValue(int)));
  connect(task, SIGNAL(finished(QObject*)), this, SLOT(removeRow(QObject*)));
  connect(cancelBtn, SIGNAL(clicked()), task, SLOT(cancel()));
  m_TasksMap[task] = progBar;

  processTableWidget->resizeColumnToContents(0);
  int width = processTableWidget->columnWidth(0);
  width = width + (width * .1);
  processTableWidget->setColumnWidth(0, width);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueueDialog::removeRow(QObject* sender)
{
 // std::cout << "ProcessQueueDialog::removeRow" << std::endl;
  QWidget* widget = m_TasksMap[sender];
  for (int i = 0; i < processTableWidget->rowCount(); ++i)
  {
    if (widget == processTableWidget->cellWidget(i, 1))
    {
      processTableWidget->removeRow(i);
      break;
    }
  }

}
