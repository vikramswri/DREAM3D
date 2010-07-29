///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CROSSCORRELATIONINPUTUI_H_
#define CROSSCORRELATIONINPUTUI_H_


#include "ui_CrossCorrelationInputUI.h"
#include "AIM/Common/AIMImage.h"
#include "CrossCorrelation/CrossCorrelationTable.h"
#include "TO79/plugins/QImageProcessingInputFrame.h"


//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtGui/QFrame>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QResizeEvent>
#include <QtGui/QSortFilterProxyModel>


class ProcessQueueDialog;
class ProcessQueueController;
class CrossCorrelationTask;

/**
 * @class CrossCorrelationInputUI CrossCorrelationInputUI.h CrossCorrelation/CrossCorrelationInputUI.h
 * @brief This class contais the GUI code that presents the input widgets for the
 * cross correlation plugin. It is derived from the QImageProcessingInputFrame base
 * class so that standard functionality is reused.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 08, 2010
 * @version 1.0
 */
class CrossCorrelationInputUI : public QImageProcessingInputFrame, private Ui::CrossCorrelationInputUI
{
  Q_OBJECT;
  Q_PROPERTY(QString m_OpenDialogLastDirectory READ getOpenDialogLastDirectory WRITE setOpenDialogLastDirectory);

  public:
    CrossCorrelationInputUI(QWidget *parent = 0);
    virtual ~CrossCorrelationInputUI();


    MXA_INSTANCE_PROPERTY(QString, CurrentImageFile)
    MXA_INSTANCE_PROPERTY(QString, CurrentProcessedFile)

    int processInputs(QObject* parentGUI);

    /**
    * @brief Reads the Preferences from the users pref file
    */
    void readSettings(QSettings* prefs);

    /**
    * @brief Writes the preferences to the users pref file
    */
    void writeSettings(QSettings* prefs);

  signals:

  protected slots:
    void on_fixedImageButton_clicked();

    void on_movingImageButton_clicked();
    void on_outputImageButton_clicked();


    void on_processFolder_stateChanged(int state  );
    void on_sourceDirectoryBtn_clicked();
    void on_outputDirectoryBtn_clicked();

    void on_fixedImageFile_textChanged(const QString &string);
    void on_movingImageFile_textChanged(const QString & text);
    void on_outputImageFile_textChanged(const QString & text);
    void on_sourceDirectoryLE_textChanged(const QString & text);
    void on_outputDirectoryLE_textChanged(const QString & text);

    void on_filterPatternLineEdit_textChanged();

    /* Slots to receive events from the ProcessQueueController */
    void queueControllerFinished();


  protected:
    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    void setupGui();
   void addProcess(CrossCorrelationTask* name);
   int writeRegisteredImage(QString file, int i, double &xt, double &yt);

  private:

    CrossCorrelationTable::Pointer  m_CrossCorrelationTable;

    CrossCorrelationInputUI(const CrossCorrelationInputUI&); // Copy Constructor Not Implemented
    void operator=(const CrossCorrelationInputUI&); // Operator '=' Not Implemented


};

#endif /* CROSSCORRELATIONINPUTUI_H_ */
