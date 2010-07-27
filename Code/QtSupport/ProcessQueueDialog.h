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

#include "ProcessQueueTask.h"
#include <QtCore/QMap>
#include <QtGui/QDialog>


/**
 * @class ProcessQueueDialog ProcessQueueDialog.h QtSupport/ProcessQueueDialog.h
 * @brief A QtDialog based class that can display the progress of ProcessQueueTask
 * objects that are running in separate threads as part of a ProcessQueueController.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 26, 2010
 * @version 1.0
 */
class ProcessQueueDialog : public QDialog, private Ui::ProcessQueueDialog
{
    Q_OBJECT
  public:
    ProcessQueueDialog(QWidget *parent = 0);
    virtual ~ProcessQueueDialog();

    /**
     * @brief Adds a ProcessQueueTask to the dialog for monitoring
     * @param task A ProcessQueueTask
     */
    void addProcess(ProcessQueueTask* task);

    /**
     * @brief removes all the ProcessQueueTask Objects from being monitored by
     * the dialog.
     */
    void clearTable();

   signals:
   /**
    * @brief Signal emitted when a ProcessQueueTask is completed.
    */
     void rowComplete(QWidget* widget);

   protected slots:

   /**
    * @brief Will remove a row from the Dialog. Each row represents a single ProcessQueueTask
    * object.
    * @param sender The sender of the message
    */
    void removeRow(QObject* sender);

  private:
    QMap<QObject*, QWidget*> m_TasksMap;

    ProcessQueueDialog(const ProcessQueueDialog&); // Copy Constructor Not Implemented
    void operator=(const ProcessQueueDialog&); // Operator '=' Not Implemented
};

#endif /* PROCESSQUEUEDIALOG_H_ */
