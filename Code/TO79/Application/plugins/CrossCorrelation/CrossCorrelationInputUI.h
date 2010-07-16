///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CROSSCORRELATIONINPUTUI_H_
#define CROSSCORRELATIONINPUTUI_H_

#include <QtGui/QFrame>
#include "ui_CrossCorrelationInputUI.h"
#include "AIM/Common/AIMImage.h"
#include "CrossCorrelation/CrossCorrelationTable.h"

//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QResizeEvent>
#include <QtGui/QSortFilterProxyModel>


class ProcessQueueDialog;
class ProcessQueueController;
class CrossCorrelationTask;


class CrossCorrelationInputUI : public QFrame, private Ui::CrossCorrelationInputUI
{
  Q_OBJECT;

  public:
    CrossCorrelationInputUI(QWidget *parent = 0);
    virtual ~CrossCorrelationInputUI();


    MXA_INSTANCE_PROPERTY_m(QString, CurrentImageFile)
    MXA_INSTANCE_PROPERTY_m(QString, CurrentProcessedFile)

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

  QStringList generateInputFileList();
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


   /**
    * @brief Initializes some of the GUI elements with selections or other GUI related items
    */
   void setupGui();

   AIMImage::Pointer convertQImageToGrayScaleAIMImage(QImage image);

   void populateFileTable();

   void addProcess(CrossCorrelationTask* name);

   AIMImage::Pointer loadImage(QString filePath);

   int writeRegisteredImage(QString file, int i, double &xt, double &yt);

  private:
    QSortFilterProxyModel*  m_ProxyModel;
    QString                 m_OpenDialogLastDirectory;
    QList<QWidget*>         m_WidgetList;

    bool                        m_OutputExistsCheck;

    ProcessQueueController*        m_QueueController;
    ProcessQueueDialog*            m_QueueDialog;

    CrossCorrelationTable::Pointer  m_CrossCorrelationTable;

    CrossCorrelationInputUI(const CrossCorrelationInputUI&); // Copy Constructor Not Implemented
    void operator=(const CrossCorrelationInputUI&); // Operator '=' Not Implemented


};

#endif /* CROSSCORRELATIONINPUTUI_H_ */
