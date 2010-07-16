///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//-- C++ includes
#include <iostream>

//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QThreadPool>
#include <QtCore/QFileInfoList>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>
#include <QtGui/QStringListModel>

// Our Project wide includes
#include "MXA/Qt/ApplicationAboutBoxDialog.h"
#include "MXA/Qt/QRecentFileList.h"
#include "MXA/Qt/QFileCompleter.h"
#include "MXA/Qt/MXAImageGraphicsDelegate.h"

// Our Own Includes
#include "QEmMpm.h"
#include "EmMpmTask.h"
#include "ProcessQueueController.h"
#include "ProcessQueueDialog.h"
#include "TO79/Common/TO79Version.h"



#define READ_STRING_SETTING(prefs, var, emptyValue)\
  var->setText( prefs.value(#var).toString() );\
  if (var->text().isEmpty() == true) { var->setText(emptyValue); }


#define READ_SETTING(prefs, var, ok, temp, default, type)\
  ok = false;\
  temp = prefs.value(#var).to##type(&ok);\
  if (false == ok) {temp = default;}\
  var->setValue(temp);


#define WRITE_STRING_SETTING(prefs, var)\
  prefs.setValue(#var , this->var->text());

#define WRITE_SETTING(prefs, var)\
  prefs.setValue(#var, this->var->value());

#define READ_BOOL_SETTING(prefs, var, emptyValue)\
  { QString s = prefs.value(#var).toString();\
  if (s.isEmpty() == false) {\
    bool bb = prefs.value(#var).toBool();\
  var->setChecked(bb); } else { var->setChecked(emptyValue); } }

#define WRITE_BOOL_SETTING(prefs, var, b)\
    prefs.setValue(#var, (b) );

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QEmMpm::QEmMpm(QWidget *parent) :
  QMainWindow(parent),
  m_ProxyModel(NULL),
#if defined(Q_WS_WIN)
      m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  readSettings();
  setupGui();
//  m_EmMpmThread = NULL;
  m_QueueController = NULL;
  m_OutputExistsCheck = false;
  QRecentFileList* recentFileList = QRecentFileList::instance();
  connect(recentFileList, SIGNAL (fileListChanged(const QString &)), this, SLOT(updateRecentFileList(const QString &)));
  // Get out initial Recent File List
  this->updateRecentFileList(QString::null);
  //this->setAcceptDrops(true);
  m_QueueDialog = new ProcessQueueDialog(this);
  m_QueueDialog->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QEmMpm::~QEmMpm()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_actionExit_triggered()
{
  this->close();
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void QEmMpm::closeEvent(QCloseEvent *event)
{
  qint32 err = checkDirtyDocument();
  if (err < 0)
  {
    event->ignore();
  }
  else
  {
    writeSettings();
    event->accept();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::resizeEvent ( QResizeEvent * event )
{
  QSize osize = originalImageFrame->size();
//  std::cout << "originalImageFrame.size: " << osize.width() << " x " << osize.height() << std::endl;
  QSize psize = processedImageFrame->size();
//  std::cout << "processedImageFrame.size: " << psize.width() << " x " << psize.height() << std::endl;
  if (originalImageGView->isVisible() == true
      && NULL != m_ProcessedImageGScene)
  {
    QRect sceneRect(0, 0, osize.width(), osize.height());
    processedImageFrame->setGeometry(sceneRect);
    processedImageGView->setGeometry(sceneRect);
    m_ProcessedImageGScene->setSceneRect(sceneRect);
  }
  else if (processedImageGView->isVisible() == true
      && NULL != m_OriginalImageGScene)
  {
    QRect sceneRect(0, 0, psize.width(), psize.height());
    originalImageFrame->setGeometry(sceneRect);
    originalImageGView->setGeometry(sceneRect);
    m_OriginalImageGScene->setSceneRect(sceneRect);
  }
  emit parentResized();
}

// -----------------------------------------------------------------------------
//  Read the prefs from the local storage file
// -----------------------------------------------------------------------------
void QEmMpm::readSettings()
{
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif
  QString val;
  bool ok;
  qint32 i;
  READ_STRING_SETTING(prefs, m_Beta, "5.5");
  READ_STRING_SETTING(prefs, m_Gamma, "0.1");
  READ_SETTING(prefs, m_MpmIterations, ok, i, 5, Int);
  READ_SETTING(prefs, m_EmIterations, ok, i, 5, Int);
  READ_SETTING(prefs, m_NumClasses, ok, i, 2, Int);
  READ_BOOL_SETTING(prefs, useSimulatedAnnealing, true);
  READ_BOOL_SETTING(prefs, processFolder, false);
  READ_STRING_SETTING(prefs, fixedImageFile, "");
  READ_STRING_SETTING(prefs, outputImageFile, "");
  READ_STRING_SETTING(prefs, sourceDirectoryLE, "");
  READ_STRING_SETTING(prefs, outputDirectoryLE, "");
  READ_STRING_SETTING(prefs, outputPrefix, "");
  READ_STRING_SETTING(prefs, outputSuffix, "");

  on_processFolder_stateChanged(processFolder->isChecked());

  if (this->sourceDirectoryLE->text().isEmpty() == false)
  {
    this->populateFileTable();
  }

}

// -----------------------------------------------------------------------------
//  Write our prefs to file
// -----------------------------------------------------------------------------
void QEmMpm::writeSettings()
{
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif
  WRITE_STRING_SETTING(prefs, m_Beta);
  WRITE_STRING_SETTING(prefs, m_Gamma);
  WRITE_SETTING(prefs, m_MpmIterations);
  WRITE_SETTING(prefs, m_EmIterations);
  WRITE_SETTING(prefs, m_NumClasses);
  WRITE_BOOL_SETTING(prefs, useSimulatedAnnealing, useSimulatedAnnealing->isChecked());
  WRITE_BOOL_SETTING(prefs, processFolder, processFolder->isChecked());
  WRITE_STRING_SETTING(prefs, fixedImageFile);
  WRITE_STRING_SETTING(prefs, outputImageFile);
  WRITE_STRING_SETTING(prefs, sourceDirectoryLE);
  WRITE_STRING_SETTING(prefs, outputDirectoryLE);
  WRITE_STRING_SETTING(prefs, outputPrefix);
  WRITE_STRING_SETTING(prefs, outputSuffix);
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::dragEnterEvent(QDragEnterEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  QDir parent(file);
  this->m_OpenDialogLastDirectory = parent.dirName();
  QFileInfo fi(file);
  QString ext = fi.suffix();
  if (fi.exists() && fi.isFile())
  {
    e->accept();
  }
  else
  {
    e->ignore();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::dropEvent(QDropEvent* e)
{
  const QMimeData* dat = e->mimeData();
  QList<QUrl> urls = dat->urls();
  QString file = urls.count() ? urls[0].toLocalFile() : QString();
  openFile(file);
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::setupGui()
{

#ifdef Q_WS_MAC
  // Adjust for the size of the menu bar which is at the top of the screen not in the window
  //  QSize minSize = minimumSize();
  QSize mySize = size();
  //  std::cout << "minSize: " << minSize.width() << " x " << minSize.height() << std::endl;
  //  std::cout << "mySize: " << mySize.width() << " x " << mySize.height() << std::endl;
  mySize.setHeight( mySize.height() -30);
  resize(mySize);
#endif
  m_OriginalImageGScene = NULL;
  m_ProcessedImageGScene = NULL;

  modeComboBox->blockSignals(true);

  modeComboBox->insertItem(0, "Exclusion");
  modeComboBox->insertItem(1, "Difference");
  modeComboBox->insertItem(2, "Plus");
  modeComboBox->insertItem(3, "Multiply");
  modeComboBox->insertItem(4, "Screen");
  modeComboBox->insertItem(5, "Darken");
  modeComboBox->insertItem(6, "Lighten");
  modeComboBox->insertItem(7, "Color Dodge");
  modeComboBox->insertItem(8, "Color Burn");
  modeComboBox->insertItem(9, "Hard Light");
  modeComboBox->insertItem(10, "Soft Light");
#if 0
  modeComboBox->insertItem(11, "Source");
  modeComboBox->insertItem(12, "Destination");
  modeComboBox->insertItem(13, "Source Over");
  modeComboBox->insertItem(14, "Destination Over");
  modeComboBox->insertItem(15, "Source In");
  modeComboBox->insertItem(16, "Dest In");

  modeComboBox->insertItem(17, "Dest Out");
  modeComboBox->insertItem(18, "Source Atop");
  modeComboBox->insertItem(19, "Dest Atop");

  modeComboBox->insertItem(20, "Overlay");
#endif


  modeComboBox->setCurrentIndex(0);
  modeComboBox->blockSignals(false);

  modeComboBox->setEnabled(compositeWithOriginal->isChecked());

  connect (originalImageGView, SIGNAL(loadImageFileRequested(const QString &)),
           this, SLOT(loadImageFile(const QString &)), Qt::QueuedConnection);

  connect (processedImageGView, SIGNAL(loadImageFileRequested(const QString &)),
           this, SLOT(loadSegmentedImageFile(const QString &)));

  QFileCompleter* com = new QFileCompleter(this, false);
  fixedImageFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_fixedImageFile_textChanged(const QString &)));


  QFileCompleter* com4 = new QFileCompleter(this, false);
  outputImageFile->setCompleter(com4);
  QObject::connect( com4, SIGNAL(activated(const QString &)),
           this, SLOT(on_outputImageFile_textChanged(const QString &)));


  QFileCompleter* com2 = new QFileCompleter(this, true);
  sourceDirectoryLE->setCompleter(com2);
  QObject::connect( com2, SIGNAL(activated(const QString &)),
           this, SLOT(on_sourceDirectoryLE_textChanged(const QString &)));

  QFileCompleter* com3 = new QFileCompleter(this, true);
  outputDirectoryLE->setCompleter(com3);
  QObject::connect( com3, SIGNAL(activated(const QString &)),
           this, SLOT(on_outputDirectoryLE_textChanged(const QString &)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_compositeWithOriginal_stateChanged(int state)
{
  modeComboBox->setEnabled(compositeWithOriginal->isChecked());
  m_ProcessedGDelegate->setOverlayImage(m_OriginalGDelegate->getCachedImage());
  m_ProcessedGDelegate->setCompositeImages( compositeWithOriginal->isChecked() );
  m_ProcessedGDelegate->updateGraphicsScene();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEmMpm::verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  QDir parent(fileinfo.dir());
  return parent.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEmMpm::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  if (false == fileinfo.exists())
  {
    lineEdit->setStyleSheet("border: 1px solid red;");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 QEmMpm::checkDirtyDocument()
{
  qint32 err = -1;
  {
    err = 1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::updateRecentFileList(const QString &file)
{
  // std::cout << "QEmMpm::updateRecentFileList" << std::endl;

  // Clear the Recent Items Menu
  this->menu_RecentFiles->clear();

  // Get the list from the static object
  QStringList files = QRecentFileList::instance()->fileList();
  foreach (QString file, files)
    {
      QAction* action = new QAction(this->menu_RecentFiles);
      action->setText(QRecentFileList::instance()->parentAndFileName(file));
      action->setData(file);
      action->setVisible(true);
      this->menu_RecentFiles->addAction(action);
      connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::openRecentFile()
{
  //std::cout << "QRecentFileList::openRecentFile()" << std::endl;

  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
  {
    //std::cout << "Opening Recent file: " << action->data().toString().toStdString() << std::endl;
    QString file = action->data().toString();
    openFile( file );
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::openFile(QString imageFile)
{
  if ( true == imageFile.isEmpty() ) // User cancelled the operation
  {
    return;
  }
  this->initWithFile(imageFile, m_CurrentProcessedFile);

  // Tell the RecentFileList to update itself then broadcast those changes.
  QRecentFileList::instance()->addFile(imageFile);
  setWidgetListEnabled(true);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_aboutBtn_clicked()
{
  ApplicationAboutBoxDialog about(this);
  QString an = QCoreApplication::applicationName();
  QString version("");
  version.append(TO79::Version::PackageComplete.c_str());
  about.setApplicationInfo(an, version);
  about.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_processBtn_clicked()
{


  /* If the 'processFolder' checkbox is checked then we need to check for some
   * additional inputs
   */
  if (this->processFolder->isChecked())
  {
    if (this->sourceDirectoryLE->text().isEmpty() == true)
    {
      this->statusBar()->showMessage("Error: Source Directory must be set.");
      QMessageBox::critical(this, tr("Input Parameter Error"),
                                    tr("Source Directory must be set."),
                                    QMessageBox::Ok);
      return;
    }

    if (this->outputDirectoryLE->text().isEmpty() == true)
    {
      this->statusBar()->showMessage("Error: Output Directory must be set");
      QMessageBox::critical(this, tr("Output Parameter Error"),
                                    tr("Output Directory must be set."),
                                    QMessageBox::Ok);
      return;
    }

    if (this->fileListView->model()->rowCount() == 0  )
    {
      this->statusBar()->showMessage("Error: No image files are available in the file list view.");
      QMessageBox::critical(this, tr("Parameter Error"),
                                    tr("No image files are available in the file list view."),
                                    QMessageBox::Ok);
      return;
    }

    QDir outputDir(this->outputDirectoryLE->text());
    if ( outputDir.exists() == false)
    {
      bool ok = outputDir.mkpath(".");
      if (ok == false)
      {
        QMessageBox::critical(this, tr("Output Directory Creation"),
                                      tr("The output directory could not be created."),
                                      QMessageBox::Ok);
        return;
      }
    }

  }
  else
  {
    QFileInfo fi(fixedImageFile->text());
    if (fi.exists() == false)
    {
      QMessageBox::critical(this, tr("Fixed Image File Error"),
                                    tr("Fixed Image does not exist. Please check the path."),
                                    QMessageBox::Ok);
      return;
    }

    if (outputImageFile->text().isEmpty() == true)
    {
      QMessageBox::critical(this, tr("Output Image File Error"),
                                    tr("Please select a file name for the registered image to be saved as."),
                                    QMessageBox::Ok);
      return;
    }
    QFile file (outputImageFile->text() );
       if (file.exists() == true)
       {
         int ret = QMessageBox::warning(this, tr("QEM/MPM"),
             tr("The Output File Already Exists\nDo you want to over write the existing file?"),
             QMessageBox::No | QMessageBox::Default,
             QMessageBox::Yes,
             QMessageBox::Cancel);
         if (ret == QMessageBox::Cancel)
         {
           return;
         }
         else if (ret == QMessageBox::Yes)
         {
           this->m_OutputExistsCheck = true;
         }
         else
         {
           QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator() + "Untitled.tif";
           outputFile = QFileDialog::getSaveFileName(this, tr("Save Output File As ..."), outputFile, tr("TIF (*.tif)"));
           if (!outputFile.isNull())
           {
             this->m_CurrentProcessedFile = "";
             this->m_OutputExistsCheck = true;
           }
           else // The user clicked cancel from the save file dialog

           {
             return;
           }
         }
       }
  }

  m_QueueDialog->clearTable();

  m_QueueController = new ProcessQueueController(this);
  bool ok;
  if (this->processFolder->isChecked() == false)
  {

    EmMpmTask* task = new EmMpmTask(NULL);
    task->setBeta(m_Beta->text().toFloat(&ok));
    task->setGamma(m_Gamma->text().toFloat(&ok));
    task->setEmIterations(m_EmIterations->value() );
    task->setMpmIterations(m_MpmIterations->value());
    task->setNumberOfClasses(m_NumClasses->value() );
    if (useSimulatedAnnealing->isChecked() )
    {
      task->useSimulatedAnnealing();
    }
    task->setInputFilePath(fixedImageFile->text());
    task->setOutputFilePath(outputImageFile->text());

    m_QueueController->addTask(static_cast<QThread*>(task) );
    this->addProcess(task);
  }
  else
  {
    QStringList fileList = generateInputFileList();
    int32_t count = fileList.count();
    for (int32_t i = 0; i < count; ++i)
    {
    //  std::cout << "Adding input file:" << fileList.at(i).toStdString() << std::endl;
      EmMpmTask* task  = new EmMpmTask(NULL);
      task->setBeta(m_Beta->text().toFloat(&ok));
      task->setGamma(m_Gamma->text().toFloat(&ok));
      task->setEmIterations(m_EmIterations->value() );
      task->setMpmIterations(m_MpmIterations->value());
      task->setNumberOfClasses(m_NumClasses->value() );
      if (useSimulatedAnnealing->isChecked() )
      {
        task->useSimulatedAnnealing();
      }
      task->setInputFilePath(sourceDirectoryLE->text() + QDir::separator() + fileList.at(i));
      QFileInfo fileInfo(fileList.at(i));
      QString basename = fileInfo.completeBaseName();
      QString extension = fileInfo.suffix();
      QString filepath = outputDirectoryLE->text();
      filepath.append(QDir::separator());
      filepath.append(outputPrefix->text());
      filepath.append(basename);
      filepath.append(outputSuffix->text());
      filepath.append(".");
      filepath.append(outputImageType->currentText());
      task->setOutputFilePath(filepath);

      m_QueueController->addTask(static_cast<QThread*>(task) );
      this->addProcess(task);
    }

  }

  // When the event loop of the controller starts it will signal the ProcessQueue to run
  connect(m_QueueController, SIGNAL(started()), m_QueueController, SLOT(processTask()));
  // When the QueueController finishes it will signal the QueueController to 'quit', thus stopping the thread
  connect(m_QueueController, SIGNAL(finished()), this, SLOT(processingFinished()));

  this->m_QueueDialog->setVisible(true);
  processBtn->setEnabled(false);

  m_QueueController->start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::addProcess(EmMpmTask* task)
{
  this->m_QueueDialog->addProcess(task);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList QEmMpm::generateInputFileList()
{
  QStringList list;
  int count = this->m_ProxyModel->rowCount();
  // this->fileListView->selectAll();
  QAbstractItemModel* sourceModel = this->m_ProxyModel->sourceModel();
  for (int i = 0; i < count; ++i)
  {
    QModelIndex proxyIndex = this->m_ProxyModel->index(i,0);
    QModelIndex sourceIndex = this->m_ProxyModel->mapToSource(proxyIndex);
    list.append( sourceModel->data(sourceIndex, 0).toString() );
  }
  return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_actionOpen_Segmented_Image_triggered()
{
  //std::cout << "on_actionOpen_triggered" << std::endl;
  QString imageFile = QFileDialog::getOpenFileName(this, tr("Open Segmented Image File"),
    m_OpenDialogLastDirectory,
    tr("Images (*.tif *.bmp *.jpg *.png)") );

  if ( true == imageFile.isEmpty() )
  {
    return;
  }
  openSegmentedImage(imageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_actionOpen_triggered()
{
  //std::cout << "on_actionOpen_triggered" << std::endl;
  QString imageFile = QFileDialog::getOpenFileName(this, tr("Open Image File"),
    m_OpenDialogLastDirectory,
    tr("Images (*.tif *.tiff *.bmp *.jpg *.jpeg *.png)") );

  if ( true == imageFile.isEmpty() )
  {
    return;
  }
  openFile(imageFile);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::loadImageFile(const QString &imageFile)
{
  if ( true == imageFile.isEmpty() )
  {
    return;
  }
  openFile(imageFile);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::loadSegmentedImageFile(const QString  &imageFile)
{
  if ( true == imageFile.isEmpty() )
  {
    return;
  }
  openSegmentedImage(imageFile);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_actionSave_triggered()
{
  saveSegmentedImage();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_actionSave_As_triggered()
{
  m_CurrentProcessedFile = QString();
  saveSegmentedImage();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_actionClose_triggered() {
  // std::cout << "AIMMountMaker::on_actionClose_triggered" << std::endl;
  qint32 err = -1;
  err = checkDirtyDocument();
  if (err >= 0)
  {
    // Close the window. Files have been saved if needed
    if (QApplication::activeWindow() == this)
    {
      this->close();
    }
    else
    {
      QApplication::activeWindow()->close();
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_modeComboBox_currentIndexChanged()
{
  if (NULL == m_ProcessedGDelegate) { return; }
  int index = modeComboBox->currentIndex();
  switch(index)
  {
  case 0: m_ProcessedGDelegate->setExclusionMode(); break;
  case 1: m_ProcessedGDelegate->setDifferenceMode(); break;
  case 2: m_ProcessedGDelegate->setPlusMode(); break;
  case 3: m_ProcessedGDelegate->setMultiplyMode(); break;
  case 4: m_ProcessedGDelegate->setScreenMode(); break;
  case 5: m_ProcessedGDelegate->setDarkenMode(); break;
  case 6: m_ProcessedGDelegate->setLightenMode(); break;
  case 7: m_ProcessedGDelegate->setColorDodgeMode(); break;
  case 8: m_ProcessedGDelegate->setColorBurnMode(); break;
  case 9: m_ProcessedGDelegate->setHardLightMode(); break;
  case 10: m_ProcessedGDelegate->setSoftLightMode(); break;

  case 11: m_ProcessedGDelegate->setSourceMode(); break;
  case 12: m_ProcessedGDelegate->setDestinationMode(); break;
  case 13: m_ProcessedGDelegate->setSourceOverMode(); break;
  case 14: m_ProcessedGDelegate->setDestinationOverMode(); break;
  case 15: m_ProcessedGDelegate->setSourceInMode(); break;
  case 16: m_ProcessedGDelegate->setDestInMode(); break;
  case 17: m_ProcessedGDelegate->setDestOutMode(); break;
  case 18: m_ProcessedGDelegate->setSourceAtopMode(); break;
  case 19: m_ProcessedGDelegate->setDestAtopMode(); break;
  case 20: m_ProcessedGDelegate->setOverlayMode(); break;

  default:
    m_ProcessedGDelegate->setExclusionMode(); break;
  }

  m_ProcessedGDelegate->setOverlayImage(m_OriginalGDelegate->getCachedImage());
  m_ProcessedGDelegate->setCompositeImages( compositeWithOriginal->isChecked() );
  m_ProcessedGDelegate->updateGraphicsScene();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::openSegmentedImage(QString mountImage)
{
  if ( true == mountImage.isEmpty() ) // User cancelled the operation
  {
    return;
  }
  this->initWithFile(m_CurrentImageFile, mountImage);
  setWidgetListEnabled(true);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 QEmMpm::saveSegmentedImage()
{
  QImage image = m_ProcessedGDelegate->getCachedImage();
  int err = 0;
  if (m_CurrentProcessedFile.isEmpty())
  {
    QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator() + "Segmented.tif";
    outputFile = QFileDialog::getSaveFileName(this, tr("Save Segmented Image As ..."), outputFile, tr("Images (*.tif *.bmp *.jpg *.png)"));
    if ( !outputFile.isEmpty() )
    {
      m_CurrentProcessedFile = outputFile;
    }
    else {
      return -1;
    }
  }

  bool ok = image.save(m_CurrentProcessedFile);
  if (ok == true) {
    segmentedImageTitle->setText(m_CurrentProcessedFile);
  }
  else
  {
    //TODO: Add in a GUI dialog to help explain the error or give suggestions.
    err = -1;
  }
  this->setWindowModified(false);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::queueControllerFinished()
{
  m_QueueDialog->setVisible(false);
  if (this->processFolder->isChecked() == false)
     {
      m_CurrentImageFile = fixedImageFile->text();
      m_CurrentProcessedFile = outputImageFile->text();
     }
    else
    {
      QStringList fileList = generateInputFileList();
      m_CurrentImageFile = fileList.at(0);

      QFileInfo fileInfo(fileList.at(0));
      QString basename = fileInfo.completeBaseName();
      QString extension = fileInfo.suffix();
      QString filepath = outputDirectoryLE->text();
      filepath.append(QDir::separator());
      filepath.append(outputPrefix->text());
      filepath.append(basename);
      filepath.append(outputSuffix->text());
      filepath.append(".");
      filepath.append(outputImageType->currentText());
      m_CurrentProcessedFile = filepath;

    }

    initWithFile(m_CurrentImageFile, m_CurrentProcessedFile);
    // Tell the RecentFileList to update itself then broadcast those changes.
    QRecentFileList::instance()->addFile(m_CurrentImageFile);
    QRecentFileList::instance()->addFile(m_CurrentProcessedFile);
    setWidgetListEnabled(true);

    processBtn->setEnabled(true);
    m_QueueController->deleteLater();
    m_QueueController = NULL;
    this->statusBar()->showMessage("Segmentation Complete");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 QEmMpm::initImageViews()
{
  qint32 err = 0;
  QImage image;
  if (m_CurrentImageFile.isEmpty() == false)
  {
    image = QImage(m_CurrentImageFile);
    if (image.isNull() == true)
    {
      this->statusbar->showMessage("Error loading image from file");
      return -1;
    }
    QVector<QRgb> colorTable(256);
    for (quint32 i = 0; i < 256; ++i)
    {
      colorTable[i] = qRgb(i, i, i);
    }
    image.setColorTable(colorTable);


    // Create the QGraphicsScene Objects
    m_OriginalImageGScene = new QGraphicsScene(this);

    QSize baseSize = originalImageFrame->baseSize();
    QRect sceneRect(0, 0, baseSize.width(), baseSize.height());
    originalImageFrame->setGeometry(sceneRect);
    originalImageGView->setGeometry(sceneRect);
    m_OriginalImageGScene->setSceneRect(sceneRect);
    originalImageGView->setScene(m_OriginalImageGScene);


    m_OriginalGDelegate = new MXAImageGraphicsDelegate(this);
    m_OriginalGDelegate->setDelegateName(QString("Original Image"));
    m_OriginalGDelegate->setGraphicsView(originalImageGView);
    m_OriginalGDelegate->setGraphicsScene(m_OriginalImageGScene);
    m_OriginalGDelegate->setMainWindow(this);
    m_OriginalGDelegate->setCachedImage(image);
    fixedFitToWindowBtn->setChecked(true);
    m_OriginalGDelegate->fitToWindow(Qt::Checked);


    connect(this, SIGNAL(parentResized () ),
            m_OriginalGDelegate, SLOT(on_parentResized () ));

    connect(fixedZoomInBtn, SIGNAL(clicked()),
            m_OriginalGDelegate, SLOT(increaseZoom() ));

    connect(fixedZoomOutBtn, SIGNAL(clicked()),
            m_OriginalGDelegate, SLOT(decreaseZoom() ));

    connect(fixedFitToWindowBtn, SIGNAL(stateChanged(int)),
            m_OriginalGDelegate, SLOT(fitToWindow(int) ));

    connect(fixedZoomInBtn, SIGNAL(clicked()),
            this, SLOT(disableFixedFitToWindow()) );
    connect(fixedZoomOutBtn, SIGNAL(clicked()),
            this, SLOT(disableFixedFitToWindow()) );

    // Create the m_OriginalImage and m_Processed Image Objects
    m_OriginalImage = convertQImageToGrayScaleAIMImage(image);
    if (NULL == m_OriginalImage.data())
    {
      return -1;
    }

  }


  // If we have NOT loaded a processed file AND we have a valid Original Image, then
  // create the Processed image based on the input image.
  QImage processedImage;
  if (m_CurrentProcessedFile.isEmpty() == true && NULL != m_OriginalImage.data() )
  {
    processedImage = image;
    m_ProcessedImage = convertQImageToGrayScaleAIMImage(processedImage);
    if (NULL == m_ProcessedImage.data())
    {
      return -1;
    }
  }
  else // We have an actual processed image file that the user wants us to read.
  {
    processedImage = QImage(m_CurrentProcessedFile);
    if (processedImage.isNull() == true)
    {
      this->statusbar->showMessage("Error loading Processed image from file");
      return -1;
    }
    // Convert it to an AIMImage in GrayScale
    m_ProcessedImage = convertQImageToGrayScaleAIMImage(processedImage);
    if (NULL == m_ProcessedImage.data() )
    {
      std::cout << "Error loading Processed image from file" << std::endl;
      return -1;
    }
  }


  if (NULL != m_ProcessedImage.data() )
  {
    // Create the QGraphicsScene Objects
    m_ProcessedImageGScene = new QGraphicsScene(this);
    QSize baseSize = processedImageFrame->baseSize();
    QRect sceneRect(0, 0, baseSize.width(), baseSize.height());
    processedImageFrame->setGeometry(sceneRect);
    processedImageGView->setGeometry(sceneRect);
    m_ProcessedImageGScene->setSceneRect(sceneRect);
    processedImageGView->setScene(m_ProcessedImageGScene);


    processedImageGView->setScene(m_ProcessedImageGScene);
    m_ProcessedGDelegate = new MXAImageGraphicsDelegate(this);
    m_ProcessedGDelegate->setDelegateName(QString("Processed Image"));
    m_ProcessedGDelegate->setGraphicsView(processedImageGView);
    m_ProcessedGDelegate->setGraphicsScene(m_ProcessedImageGScene);
    m_ProcessedGDelegate->setMainWindow(this);
    m_ProcessedGDelegate->setCachedImage(processedImage);
    processedFitToWindowBtn->setChecked(true);
    m_ProcessedGDelegate->fitToWindow(Qt::Checked);
    connect(this, SIGNAL(parentResized () ),
            m_ProcessedGDelegate, SLOT(on_parentResized () ) );

    connect(processedZoomInBtn, SIGNAL(clicked()),
            m_ProcessedGDelegate, SLOT(increaseZoom() ));

    connect(processedZoomOutBtn, SIGNAL(clicked()),
            m_ProcessedGDelegate, SLOT(decreaseZoom() ));

    connect(processedFitToWindowBtn, SIGNAL(stateChanged(int)),
            m_ProcessedGDelegate, SLOT(fitToWindow(int) ));

    connect(processedZoomInBtn, SIGNAL(clicked()),
            this, SLOT(disableProcessedFitToWindow()) );
    connect(processedZoomOutBtn, SIGNAL(clicked()),
            this, SLOT(disableProcessedFitToWindow()) );
  }
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::initWithFile(const QString imageFile, QString mountImage)
{
  QFileInfo fileInfo(imageFile);
  this->m_OpenDialogLastDirectory = fileInfo.path();

  m_CurrentImageFile = imageFile;
  m_CurrentProcessedFile = mountImage;

  qint32 err = initImageViews();
  if (err < 0)
  {
    this->statusBar()->showMessage("Error Loading Original Image");
    return;
  }
  this->originalImageTitle->setText(fileInfo.fileName());
  this->originalImageTitle->setToolTip(m_CurrentImageFile);

  if (m_CurrentProcessedFile.isEmpty())
  {
    this->segmentedImageTitle->setText("Unsaved Segmented Image");
    this->setWindowModified(true);
  }
  else {
    QFileInfo segInfo(m_CurrentProcessedFile);
    this->segmentedImageTitle->setText(segInfo.fileName());
    this->segmentedImageTitle->setToolTip(m_CurrentProcessedFile);
  }
  this->statusBar()->showMessage("Input Image Loaded");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_filterPatternLineEdit_textChanged()
{
 // std::cout << "filterPattern: " << std::endl;
  m_ProxyModel->setFilterFixedString(filterPatternLineEdit->text());
  m_ProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_processFolder_stateChanged(int state)
{
  bool enabled = true;
  if (state == Qt::Unchecked)
  {
    enabled = false;
  }

  sourceDirectoryLE->setEnabled(enabled);
  sourceDirectoryBtn->setEnabled(enabled);
  outputDirectoryLE->setEnabled(enabled);
  outputDirectoryBtn->setEnabled(enabled);
  outputPrefix->setEnabled(enabled);
  outputSuffix->setEnabled(enabled);
  filterPatternLabel->setEnabled(enabled);
  filterPatternLineEdit->setEnabled(enabled);
  fileListView->setEnabled(enabled);
  outputImageTypeLabel->setEnabled(enabled);
  outputImageType->setEnabled(enabled);

  fixedImageFile->setEnabled(!enabled);
  fixedImageButton->setEnabled(!enabled);

  outputImageFile->setEnabled(!enabled);
  outputImageButton->setEnabled(!enabled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_sourceDirectoryBtn_clicked() {
  this->m_OpenDialogLastDirectory = QFileDialog::getExistingDirectory(this, tr("Select Source Directory"),
                                                               this->m_OpenDialogLastDirectory,
                                                               QFileDialog::ShowDirsOnly
                                                               | QFileDialog::DontResolveSymlinks);
  if ( !this->m_OpenDialogLastDirectory.isNull() )
  {
    this->sourceDirectoryLE->setText(this->m_OpenDialogLastDirectory);
  }
  this->populateFileTable();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_outputDirectoryBtn_clicked()
{
  this->m_OpenDialogLastDirectory = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"),
                                                               this->m_OpenDialogLastDirectory,
                                                               QFileDialog::ShowDirsOnly
                                                               | QFileDialog::DontResolveSymlinks);
  if ( !this->m_OpenDialogLastDirectory.isNull() )
  {
    this->outputDirectoryLE->setText(this->m_OpenDialogLastDirectory);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::populateFileTable()
{
  if ( NULL == m_ProxyModel)
  {
    m_ProxyModel = new QSortFilterProxyModel(this);
  }

  QDir sourceDir (this->sourceDirectoryLE->text() );
  sourceDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks );
  QStringList strList = sourceDir.entryList();
  QAbstractItemModel* strModel = new QStringListModel(strList, this->m_ProxyModel);
  m_ProxyModel->setSourceModel(strModel);
  m_ProxyModel->setDynamicSortFilter(true);
  m_ProxyModel->setFilterKeyColumn(0);
  fileListView->setModel(m_ProxyModel);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::Pointer QEmMpm::convertQImageToGrayScaleAIMImage(QImage image)
{
  AIMImage::Pointer aimImage = AIMImage::New();
  quint8* oImage = aimImage->allocateImageBuffer(image.width(), image.height(), true);
  if (NULL == oImage)
  {
    return AIMImage::NullPointer();
  }

  // Copy the QImage into the AIMImage object, converting to gray scale as we go.
  qint32 height = image.height();
  qint32 width = image.width();
  QRgb rgbPixel;
  int gray;
  qint32 index;
  for (qint32 y=0; y<height; y++) {
    for (qint32 x =0; x<width; x++) {
      index = (y *  width) + x;
      rgbPixel = image.pixel(x, y);
      gray = qGray(rgbPixel);
      oImage[index] = static_cast<unsigned char>(gray);
    }
  }
  return aimImage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_fixedImageButton_clicked()
{
  //std::cout << "on_actionOpen_triggered" << std::endl;
  QString imageFile = QFileDialog::getOpenFileName(this, tr("Select Fixed Image"),
    m_OpenDialogLastDirectory,
    tr("Images (*.tif *.tiff *.bmp *.jpg *.jpeg *.png)") );

  if ( true == imageFile.isEmpty() )
  {
    return;
  }
  fixedImageFile->setText(imageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_outputImageButton_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator() + "Untitled.tif";
  outputFile = QFileDialog::getSaveFileName(this, tr("Save Output File As ..."), outputFile, tr("Images (*.tif *.tiff *.bmp *.jpg *.jpeg *.png)"));
  if (outputFile.isEmpty() )
  {
    return;
  }
  outputImageFile->setText(outputFile);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_fixedImageFile_textChanged(const QString & text)
{
  verifyPathExists(fixedImageFile->text(), fixedImageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_outputImageFile_textChanged(const QString & text)
{
//  verifyPathExists(outputImageFile->text(), movingImageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_sourceDirectoryLE_textChanged(const QString & text)
{
  verifyPathExists(sourceDirectoryLE->text(), sourceDirectoryLE);
  this->populateFileTable();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_outputDirectoryLE_textChanged(const QString & text)
{
  verifyPathExists(outputDirectoryLE->text(), outputDirectoryLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::disableFixedFitToWindow()
{
  this->fixedFitToWindowBtn->setChecked(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::disableProcessedFitToWindow()
{
  this->processedFitToWindowBtn->setChecked(false);
}
