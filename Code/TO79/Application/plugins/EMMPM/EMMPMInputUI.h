/*
 * EMMPMInputUI.h
 *
 *  Created on: Jul 16, 2010
 *      Author: mjackson
 */

#ifndef EMMPMPLUGININPUTUI_H_
#define EMMPMPLUGININPUTUI_H_

#include <QtGui/QFrame>
#include "ui_EMMPMInputUI.h"
#include "AIM/Common/AIMImage.h"

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
class EMMPMTask;

class EMMPMInputUI : public QFrame, private Ui::EMMPMInputUI
{
  Q_OBJECT;

  public:
    EMMPMInputUI(QWidget *parent = 0);
    virtual ~EMMPMInputUI();

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
    void on_outputImageButton_clicked();

    void on_processFolder_stateChanged(int state);
    void on_sourceDirectoryBtn_clicked();
    void on_outputDirectoryBtn_clicked();

    void on_fixedImageFile_textChanged(const QString &string);
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

    void addProcess(EMMPMTask* name);

  private:
    QSortFilterProxyModel* m_ProxyModel;
    QString m_OpenDialogLastDirectory;
    QList<QWidget* > m_WidgetList;

    bool m_OutputExistsCheck;

    ProcessQueueController* m_QueueController;
    ProcessQueueDialog* m_QueueDialog;

    EMMPMInputUI(const EMMPMInputUI&); // Copy Constructor Not Implemented
    void operator=(const EMMPMInputUI&); // Operator '=' Not Implemented
};

#endif /* EMMPMPLUGININPUTUI_H_ */
