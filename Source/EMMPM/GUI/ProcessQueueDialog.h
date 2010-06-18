///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROCESSQUEUEDIALOG_H_
#define PROCESSQUEUEDIALOG_H_

#include <ui_ProcessQueueDialog.h>

#include "EmMpmTask.h"
#include <QtCore/QMap>
#include <QtGui/QDialog>


/**
 *
 */
class ProcessQueueDialog : public QDialog, private Ui::ProcessQueueDialog
{
    Q_OBJECT
  public:
    ProcessQueueDialog(QWidget *parent = 0);
    virtual ~ProcessQueueDialog();

    void addProcess(EmMpmTask* task);

    void clearTable();

   signals:
     void rowComplete(QWidget* widget);

   protected slots:
    void removeRow(QObject* sender);

  private:
    QMap<QObject*, QWidget*> m_TasksMap;

    ProcessQueueDialog(const ProcessQueueDialog&); // Copy Constructor Not Implemented
    void operator=(const ProcessQueueDialog&); // Operator '=' Not Implemented
};

#endif /* PROCESSQUEUEDIALOG_H_ */
