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

#ifndef QIMAGEPROCESSINGINPUTUI_H_
#define QIMAGEPROCESSINGINPUTUI_H_

#include "MXA/Common/MXASetGetMacros.h"
#include "AIM/Common/AIMImage.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtGui/QFrame>
#include <QtGui/QImage>
#include <QtGui/QLineEdit>
#include <QtGui/QSortFilterProxyModel>

class ProcessQueueDialog;
class ProcessQueueController;
class QListView;


class QImageProcessingInputFrame : public QFrame
{
    Q_OBJECT;

  public:
    QImageProcessingInputFrame(QWidget *parent = 0);
    virtual ~QImageProcessingInputFrame();

    /**
      * @brief Reads the Preferences from the users pref file
      */
    virtual void readSettings(QSettings* prefs) = 0;

     /**
      * @brief Writes the preferences to the users pref file
      */
    virtual void writeSettings(QSettings* prefs) = 0;
    
    typedef QPair<QString, QString>        InputOutputFilePair;
    typedef QList<InputOutputFilePair>     InputOutputFilePairList;

    MXA_INSTANCE_PROPERTY(QString, CurrentImageFile)
    MXA_INSTANCE_PROPERTY(QString, CurrentProcessedFile)
    MXA_INSTANCE_PROPERTY(QSortFilterProxyModel*, ProxyModel)
    MXA_INSTANCE_PROPERTY(QList<QWidget* >, WidgetList)
    MXA_INSTANCE_PROPERTY(bool, OutputExistsCheck)
    MXA_INSTANCE_PROPERTY(ProcessQueueController*, QueueController)
    MXA_INSTANCE_PROPERTY(ProcessQueueDialog*, QueueDialog)
    MXA_INSTANCE_PROPERTY(QString, OpenDialogLastDirectory)
    MXA_INSTANCE_PROPERTY(InputOutputFilePairList, InputOutputFilePairList)


  protected slots:


    protected:

      /**
      * @brief Enables or Disables all the widgets in a list
      * @param b
      */
      void setWidgetListEnabled(bool b);

     /**
      * @brief Verifies that a path exists on the file system.
      * @param outFilePath The file path to check
      * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
      */
      bool verifyPathExists(QString outFilePath, QLineEdit* lineEdit);

     /**
      * @brief Verifies that a parent path exists on the file system.
      * @param outFilePath The parent file path to check
      * @param lineEdit The QLineEdit object to modify visuals of (Usually by placing a red line around the QLineEdit widget)
      */
      bool verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit);


      AIMImage::Pointer convertQImageToGrayScaleAIMImage(QImage image);

      void populateFileTable(QLineEdit* sourceDirectoryLE, QListView *fileListView);


      AIMImage::Pointer loadImage(QString filePath);

      QStringList generateInputFileList();

    private:
    QImageProcessingInputFrame(const QImageProcessingInputFrame&); // Copy Constructor Not Implemented
    void operator=(const QImageProcessingInputFrame&); // Operator '=' Not Implemented

};

#endif /* QIMAGEPROCESSINGINPUTUI_H_ */
