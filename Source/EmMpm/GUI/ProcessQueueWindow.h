///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROCESSQUEUEDIALOG_H_
#define PROCESSQUEUEDIALOG_H_

#include <ui_ProcessQueueWindow.h>

#include <QtGui/QMainWindow>

/**
 *
 */
class ProcessQueueWindow : public QMainWindow, private Ui::ProcessQueueWindow
{
    Q_OBJECT
  public:
    ProcessQueueWindow(QWidget *parent = 0);
    virtual ~ProcessQueueWindow();

    void addProcess(QString name);


  private:
    ProcessQueueWindow(const ProcessQueueWindow&); // Copy Constructor Not Implemented
    void operator=(const ProcessQueueWindow&); // Operator '=' Not Implemented
};

#endif /* PROCESSQUEUEDIALOG_H_ */
