///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

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

/*
 *
 */
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

    MXA_INSTANCE_PROPERTY_m(QString, CurrentImageFile)
    MXA_INSTANCE_PROPERTY_m(QString, CurrentProcessedFile)
    MXA_INSTANCE_PROPERTY_m(QSortFilterProxyModel*, ProxyModel)
    MXA_INSTANCE_PROPERTY_m(QList<QWidget* >, WidgetList)
    MXA_INSTANCE_PROPERTY_m(bool, OutputExistsCheck)
    MXA_INSTANCE_PROPERTY_m(ProcessQueueController*, QueueController)
    MXA_INSTANCE_PROPERTY_m(ProcessQueueDialog*, QueueDialog)
    MXA_INSTANCE_PROPERTY_m(QString, OpenDialogLastDirectory)

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
