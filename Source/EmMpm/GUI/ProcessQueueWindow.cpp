///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ProcessQueueWindow.h"

#include <QtGui/QProgressBar>
#include <QtGui/QLabel>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueueWindow::ProcessQueueWindow(QWidget *parent) :
QMainWindow(parent)
{
  setupUi(this);
//  this->processTableWidget->setRowCount(1);
//  this->processTableWidget->setCellWidget(0, 0, new QLabel("File 1.tiff"));
//  this->processTableWidget->setCellWidget(0, 1, new QProgressBar());
//
//  this->processTableWidget->setRowCount(2);
//  this->processTableWidget->setCellWidget(1, 0, new QLabel("File 2.tiff"));
//  this->processTableWidget->setCellWidget(1, 1, new QProgressBar());

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ProcessQueueWindow::~ProcessQueueWindow()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ProcessQueueWindow::addProcess(QString name)
{
  qint32 rowCount = this->processTableWidget->rowCount();
  this->processTableWidget->setRowCount(rowCount + 1);
  this->processTableWidget->setCellWidget(rowCount, 0, new QLabel(name));
  this->processTableWidget->setCellWidget(rowCount, 1, new QProgressBar());
}
