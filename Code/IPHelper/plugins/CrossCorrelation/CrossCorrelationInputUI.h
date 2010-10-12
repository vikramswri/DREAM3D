/* ============================================================================
* Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of Michael A. Jackson nor the names of its contributors may
* be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef CROSSCORRELATIONINPUTUI_H_
#define CROSSCORRELATIONINPUTUI_H_


#include "ui_CrossCorrelationInputUI.h"
#include "AIM/Common/AIMImage.h"
#include "CrossCorrelation/CrossCorrelationTable.h"
#include "IPHelper/plugins/QImageProcessingInputFrame.h"


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

    void on_outputPrefix_textChanged();
    void on_outputSuffix_textChanged();
    void on_outputImageType_currentIndexChanged(int index);

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
