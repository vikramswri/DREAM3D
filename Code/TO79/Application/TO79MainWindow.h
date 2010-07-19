///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef TO79MAINWINDOW_H_
#define TO79MAINWINDOW_H_

//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QResizeEvent>
#include <QtGui/QSortFilterProxyModel>

//-- UIC generated Header
#include <ui_TO79MainWindow.h>


#include "AIM/Common/AIMImage.h"


class MXAImageGraphicsDelegate;
class ProcessQueueDialog;
class ProcessQueueController;
class QImageProcessingInterface;

class TO79MainWindow : public QMainWindow, private Ui::TO79MainWindow
{
  Q_OBJECT;

  public:
    TO79MainWindow(QWidget *parent = 0);
    virtual ~TO79MainWindow();
    void initWithFile(const QString imageFile, QString mountImage);

    MXA_INSTANCE_PROPERTY_m(AIMImage::Pointer, OriginalImage)
    MXA_INSTANCE_PROPERTY_m(AIMImage::Pointer, ProcessedImage)

    MXA_INSTANCE_PROPERTY_m(QString, CurrentImageFile)
    MXA_INSTANCE_PROPERTY_m(QString, CurrentProcessedFile)

    qint32 saveProcessedImage();

  signals:
    void cancelTask();
    void cancelProcessQueue();
    void parentResized();

  protected slots:
  //Manual Hookup Menu Actions
    void on_actionOpen_triggered(); // Open a Data File
    void on_actionOpen_Processed_Image_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionClose_triggered();
    void on_actionExit_triggered();

    /* slots for the buttons in the GUI */
    void on_processBtn_clicked();
    void on_aboutBtn_clicked();

    void on_compositeWithOriginal_stateChanged(int state);
    void on_modeComboBox_currentIndexChanged();

    void disableFixedFitToWindow();
    void disableProcessedFitToWindow();

    /**
     * @brief Qt Slot that fires in response to a click on a "Recent File' Menu entry.
     */
    void openRecentFile();

    void loadImageFile(const QString &filename);

    void loadProcessedImageFile(const QString  &filename);
    /**
     * @brief Updates the QMenu 'Recent Files' with the latest list of files. This
     * should be connected to the Signal QRecentFileList->fileListChanged
     * @param file The newly added file.
     */
    void updateRecentFileList(const QString &file);

    void setInputUI();

    // These slots get called when the plugin starts and finishes processing
    void processingStarted();
    void processingFinished();

  protected:

    void loadPlugins();
    void populateMenus(QObject *plugin);
    void addToPluginMenu(QObject *plugin, const QString &text,
                               QMenu *menu, const char *member,
                               QActionGroup *actionGroup);

    QStringList generateInputFileList();
    /**
    *
    * @param event
    */
   void resizeEvent ( QResizeEvent * event );

   /**
    * @brief Implements the CloseEvent to Quit the application and write settings
    * to the preference file
    */
   void closeEvent(QCloseEvent *event);


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
     * @brief Opens an Image file
     * @param imageFile The path to the image file to open.
     */
    void openFile(QString imageFile);

    void openProcessedImage(QString mountImage);

    AIMImage::Pointer convertQImageToGrayScaleAIMImage(QImage image);

    qint32 initImageViews();

    AIMImage::Pointer loadImage(QString filePath);


  private:
    QDir pluginsDir;
    QStringList pluginFileNames;
    QActionGroup* pluginActionGroup;
    QImageProcessingInterface* m_ActivePlugin;

    QGraphicsScene*             m_OriginalImageGScene;
    QGraphicsScene*             m_ProcessedImageGScene;

    MXAImageGraphicsDelegate*      m_OriginalGDelegate;
    MXAImageGraphicsDelegate*      m_ProcessedGDelegate;

    QVector<QImageProcessingInterface*> m_LoadedPlugins;

    QString m_OpenDialogLastDirectory;
    QList<QWidget*> m_WidgetList;

 //   bool            m_OutputExistsCheck;


    TO79MainWindow(const TO79MainWindow&); // Copy Constructor Not Implemented
    void operator=(const TO79MainWindow&); // Operator '=' Not Implemented
};

#endif /* TO79MAINWINDOW_H_ */
