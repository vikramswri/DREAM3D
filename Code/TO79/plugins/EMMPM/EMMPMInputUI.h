///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EMMPMPLUGININPUTUI_H_
#define EMMPMPLUGININPUTUI_H_


#include "ui_EMMPMInputUI.h"
#include "AIM/Common/AIMImage.h"
#include "TO79/plugins/QImageProcessingInputFrame.h"

//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QSettings>


class ProcessQueueDialog;
class ProcessQueueController;
class EMMPMTask;

class EMMPMInputUI : public QImageProcessingInputFrame, private Ui::EMMPMInputUI
{
  Q_OBJECT;

  public:
    EMMPMInputUI(QWidget *parent = 0);
    virtual ~EMMPMInputUI();



    int processInputs(QObject* parentGUI);

    /**
     * @brief Reads the Preferences from the users pref file
     */
    void readSettings(QSettings* prefs);

    /**
     * @brief Writes the preferences to the users pref file
     */
    void writeSettings(QSettings* prefs);


  protected slots:
    void on_fixedImageButton_clicked();
    void on_outputImageButton_clicked();

    void on_processFolder_stateChanged(int state  );
    void on_sourceDirectoryBtn_clicked();
    void on_outputDirectoryBtn_clicked();

    void on_fixedImageFile_textChanged(const QString &string);
    void on_outputImageFile_textChanged(const QString & text);
    void on_sourceDirectoryLE_textChanged(const QString & text);
    void on_outputDirectoryLE_textChanged(const QString & text);

    void on_filterPatternLineEdit_textChanged();

    // Over rides from Parent Class
    /* Slots to receive events from the ProcessQueueController */
    void queueControllerFinished();

  protected:
    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    void setupGui();
    void addProcess(EMMPMTask* name);


  private:

    EMMPMInputUI(const EMMPMInputUI&); // Copy Constructor Not Implemented
    void operator=(const EMMPMInputUI&); // Operator '=' Not Implemented
};

#endif /* EMMPMPLUGININPUTUI_H_ */
