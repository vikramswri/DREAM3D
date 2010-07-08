///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _QEMMPM_GUI_H_
#define _QEMMPM_GUI_H_

//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QResizeEvent>
#include <QtGui/QSortFilterProxyModel>

//-- UIC generated Header
#include <ui_QEmMpm.h>

#include "AIM/Common/AIMImage.h"


class MXAImageGraphicsDelegate;
class ProcessQueueDialog;
class ProcessQueueController;
class EmMpmTask;

/**
* @class QEmMpm QEmMpm.h EmMpm/GUI/QEmMpm.h
* @brief Main class that drives the GUI elements.
* @author Michael A. Jackson for BlueQuartz Software
* @date Dec 23, 2009
* @version 1.0
*/
class QEmMpm: public QMainWindow, private Ui::QEmMpm
{
  Q_OBJECT;

  public:
    QEmMpm(QWidget *parent = 0);
    virtual ~QEmMpm();

    void initWithFile(const QString imageFile, QString mountImage);

    MXA_INSTANCE_PROPERTY_m(AIMImage::Pointer, OriginalImage)
    MXA_INSTANCE_PROPERTY_m(AIMImage::Pointer, SegmentedImage)

    MXA_INSTANCE_PROPERTY_m(QString, CurrentImageFile)
    MXA_INSTANCE_PROPERTY_m(QString, CurrentSegmentedFile)

    qint32 saveSegmentedImage();

  signals:
    void cancelTask();
    void cancelProcessQueue();
    void parentResized();

  protected slots:
  //Manual Hookup Menu Actions
    void on_actionOpen_triggered(); // Open a Data File
    void on_actionOpen_Segmented_Image_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionClose_triggered();
    void on_actionExit_triggered();

    /* slots for the buttons in the GUI */
    void on_m_SegmentBtn_clicked();
    void on_aboutBtn_clicked();

    void on_compositeWithOriginal_stateChanged(int state);
    void on_modeComboBox_currentIndexChanged();

    void on_processFolder_stateChanged(int state  );
    void on_sourceDirectoryBtn_clicked();
    void on_outputDirectoryBtn_clicked();


    void on_filterPatternLineEdit_textChanged();

    /* Slots to receive events from the ProcessQueueController */
    void queueControllerFinished();

    /* Slots to receive events from the Encoder task
    void receiveTaskMessage(const QString &msg);
    void receiveTaskProgress(int p);
    void receiveTaskFinished();
*/

    /**
     * @brief Updates the QMenu 'Recent Files' with the latest list of files. This
     * should be connected to the Signal QRecentFileList->fileListChanged
     * @param file The newly added file.
     */
    void updateRecentFileList(const QString &file);

    /**
     * @brief Qt Slot that fires in response to a click on a "Recent File' Menu entry.
     */
    void openRecentFile();

    void loadImageFile(const QString &filename);

    void loadSegmentedImageFile(const QString  &filename);

    QStringList generateInputFileList();

  protected:

    /**
     * @brief Implements the CloseEvent to Quit the application and write settings
     * to the preference file
     */
    void closeEvent(QCloseEvent *event);

#if 0
    /**
     * @brief Drag and drop implementation
     */
    void dragEnterEvent(QDragEnterEvent*);

    /**
     * @brief Drag and drop implementation
     */
    void dropEvent(QDropEvent*);
#endif

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
     * @brief Reads the Preferences from the users pref file
     */
    void readSettings();

    /**
     * @brief Writes the preferences to the users pref file
     */
    void writeSettings();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    void setupGui();

    /**
     * @brief Checks the currently open file for changes that need to be saved
     * @return
     */
    qint32 checkDirtyDocument();

    void resizeEvent ( QResizeEvent * event );

     /**
      * @brief Opens an Image file
      * @param imageFile The path to the image file to open.
      */
     void openFile(QString imageFile);

     void openSegmentedImage(QString mountImage);

     AIMImage::Pointer convertQImageToGrayScaleAIMImage(QImage image);

     qint32 initGraphicViews();

     void populateFileTable();

     void addProcess(EmMpmTask* name);

  private:
    QSortFilterProxyModel* m_ProxyModel;
    QString m_OpenDialogLastDirectory;
    QList<QWidget*> m_WidgetList;

  //  EmMpmThread*      m_EmMpmThread;
    bool            m_OutputExistsCheck;

    QGraphicsScene*             m_OriginalImageGScene;
    QGraphicsScene*             m_SegmentedImageGScene;

    MXAImageGraphicsDelegate*      m_OriginalGDelegate;
    MXAImageGraphicsDelegate*      m_SegmentedGDelegate;

    ProcessQueueController*        m_QueueController;
    ProcessQueueDialog*            m_QueueDialog;

    QEmMpm(const QEmMpm&); // Copy Constructor Not Implemented
    void operator=(const QEmMpm&); // Operator '=' Not Implemented
};

#endif /* _QEMMPM_GUI_H_ */
