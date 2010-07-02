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
#include "MXA/Qt/ProcessQueueController.h"
#include "MXA/Qt/ProcessQueueDialog.h"

// Our Own Includes
#include "QCrossCorrelation.h"
#include "CrossCorrelationTask.h"
#include "CrossCorrelation/CrossCorrelation.h"

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
QCrossCorrelation::QCrossCorrelation(QWidget *parent) :
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
//  m_CrossCorrelationThread = NULL;
  m_QueueController = NULL;
  m_OutputExistsCheck = false;
  QRecentFileList* recentFileList = QRecentFileList::instance();
  connect(recentFileList, SIGNAL (fileListChanged(const QString &)), this, SLOT(updateRecentFileList(const QString &)));
  // Get out initial Recent File List
  this->updateRecentFileList(QString::null);
  //this->setAcceptDrops(true);
  m_QueueDialog = new ProcessQueueDialog(this);
  m_QueueDialog->setVisible(false);

  m_CrossCorrelationTable = CrossCorrelationTable::New();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QCrossCorrelation::~QCrossCorrelation()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_actionExit_triggered()
{
  this->close();
}

// -----------------------------------------------------------------------------
//  Called when the main window is closed.
// -----------------------------------------------------------------------------
void QCrossCorrelation::closeEvent(QCloseEvent *event)
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
void QCrossCorrelation::resizeEvent ( QResizeEvent * event )
{
  emit parentResized();
}

// -----------------------------------------------------------------------------
//  Read the prefs from the local storage file
// -----------------------------------------------------------------------------
void QCrossCorrelation::readSettings()
{
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif
  QString val;
//  bool ok;
//  qint32 i;

  READ_BOOL_SETTING(prefs, processFolder, false);
  READ_STRING_SETTING(prefs, fixedImageFile, "");
  READ_STRING_SETTING(prefs, movingImageFile, "");
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
void QCrossCorrelation::writeSettings()
{
#if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#endif

  WRITE_BOOL_SETTING(prefs, processFolder, processFolder->isChecked());
  WRITE_STRING_SETTING(prefs, fixedImageFile);
  WRITE_STRING_SETTING(prefs, movingImageFile);
  WRITE_STRING_SETTING(prefs, sourceDirectoryLE);
  WRITE_STRING_SETTING(prefs, outputDirectoryLE);
  WRITE_STRING_SETTING(prefs, outputPrefix);
  WRITE_STRING_SETTING(prefs, outputSuffix);
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::dragEnterEvent(QDragEnterEvent* e)
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
void QCrossCorrelation::dropEvent(QDropEvent* e)
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
void QCrossCorrelation::setupGui()
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

  connect (segmentedImageGView, SIGNAL(loadImageFileRequested(const QString &)),
           this, SLOT(loadProcessedImageFile(const QString &)));

  QFileCompleter* com = new QFileCompleter(this, false);
  fixedImageFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_fixedImageFile_textChanged(const QString &)));

  QFileCompleter* com1 = new QFileCompleter(this, false);
  movingImageFile->setCompleter(com1);
  QObject::connect( com1, SIGNAL(activated(const QString &)),
           this, SLOT(on_movingImageFile_textChanged(const QString &)));


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
void QCrossCorrelation::on_modeComboBox_currentIndexChanged()
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
void QCrossCorrelation::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_compositeWithOriginal_stateChanged(int state)
{
  modeComboBox->setEnabled(compositeWithOriginal->isChecked());
  m_ProcessedGDelegate->setOverlayImage(m_OriginalGDelegate->getCachedImage());
  m_ProcessedGDelegate->setCompositeImages( compositeWithOriginal->isChecked() );
  m_ProcessedGDelegate->updateGraphicsScene();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QCrossCorrelation::verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  QDir parent(fileinfo.dir());
  return parent.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QCrossCorrelation::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
qint32 QCrossCorrelation::checkDirtyDocument()
{
  qint32 err = -1;

  if (this->isWindowModified() == true)
  {
    int r = QMessageBox::warning(this, tr("EM-MPM"), tr("The Image has been modified.\nDo you want to save your changes?"), QMessageBox::Save
        | QMessageBox::Default, QMessageBox::Discard, QMessageBox::Cancel | QMessageBox::Escape);
    if (r == QMessageBox::Save)
    {
      //TODO: Save the current document or otherwise save the state.
    }
    else if (r == QMessageBox::Discard)
    {
      err = 1;
    }
    else if (r == QMessageBox::Cancel)
    {
      err = -1;
    }
  }
  else
  {
    err = 1;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::updateRecentFileList(const QString &file)
{
  // std::cout << "QCrossCorrelation::updateRecentFileList" << std::endl;

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
void QCrossCorrelation::openRecentFile()
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
void QCrossCorrelation::openFile(QString imageFile)
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
void QCrossCorrelation::on_aboutBtn_clicked()
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
void QCrossCorrelation::on_registerButton_clicked()
{
//
//  if (m_QueueController != NULL && m_QueueController->isFinished() == false)
//  {
//    emit cancelProcessQueue();
//    return;
//  }



  if (this->m_OutputExistsCheck == false)
  {
    QFile file (this->m_CurrentProcessedFile );
    if (file.exists() == true)
    {
     int ret = QMessageBox::warning(this, tr("QCrossCorrelation"),
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
   QFileInfo mfi(movingImageFile->text());
   if (mfi.exists() == false)
   {
     QMessageBox::critical(this, tr("Moving Image File Error"),
                                   tr("Moving Image does not exist. Please check the path."),
                                   QMessageBox::Ok);
     return;
   }
  }

  m_QueueDialog->clearTable();

  m_QueueController = new ProcessQueueController(this);
//  bool ok;
  if (this->processFolder->isChecked() == false)
  {
    if (fixedImageFile->text().isEmpty() )
    {
      QMessageBox::critical(this, tr("Input File Error"),
                                    tr("Fixed Image is not selected"),
                                    QMessageBox::Ok);
      m_QueueController->deleteLater();
      return;
    }
    if (movingImageFile->text().isEmpty() )
    {
      QMessageBox::critical(this, tr("Input File Error"),
                                    tr("Moving Image is not selected"),
                                    QMessageBox::Ok);
      m_QueueController->deleteLater();
      return;
    }

    CrossCorrelationTask* task = new CrossCorrelationTask(NULL);

    task->setInputFilePath(fixedImageFile->text());
    task->setMovingImagePath(movingImageFile->text());
    QFileInfo fileInfo(movingImageFile->text());
    QString basename = fileInfo.completeBaseName();
    QString extension = fileInfo.suffix();
    QString filepath = fileInfo.absolutePath();
    filepath.append(QDir::separator());
    filepath.append(basename);
    filepath.append("_Segmented");
    filepath.append(".");
    filepath.append(extension);
    task->setOutputFilePath(filepath);
    CrossCorrelationData::Pointer crossCorrelationData = CrossCorrelationData::New();
    task->setCrossCorrelationData(crossCorrelationData);
    m_CrossCorrelationTable->addCrossCorrelationData(0, crossCorrelationData);

    m_QueueController->addTask(static_cast<QThread*>(task) );
    this->addProcess(task);
  }
  else
  {
    QStringList fileList = generateInputFileList();
    int32_t count = fileList.count();
    CrossCorrelationData::Pointer crossCorrelationData = CrossCorrelationData::New();
    m_CrossCorrelationTable->addCrossCorrelationData(0, crossCorrelationData);
    for (int32_t i = 0; i < count-1; ++i)
    {
    //  std::cout << "Adding input file:" << fileList.at(i).toStdString() << std::endl;
      CrossCorrelationTask* task  = new CrossCorrelationTask(NULL);

      task->setInputFilePath(sourceDirectoryLE->text() + QDir::separator() + fileList.at(i));
      task->setMovingImagePath(sourceDirectoryLE->text() + QDir::separator() + fileList.at(i+1));
      QFileInfo fileInfo(fileList.at(i+1));
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
      CrossCorrelationData::Pointer crossCorrelationData = CrossCorrelationData::New();
      task->setCrossCorrelationData(crossCorrelationData);
      m_CrossCorrelationTable->addCrossCorrelationData(i+1, crossCorrelationData);

      m_QueueController->addTask(static_cast<QThread*>(task) );
      this->addProcess(task);
    }

  }

  // When the event loop of the controller starts it will signal the ProcessQueue to run
  connect(m_QueueController, SIGNAL(started()), m_QueueController, SLOT(processTask()));
  // When the ProcessQueue finishes it will signal the QueueController to 'quit', thus stopping the thread
  connect(m_QueueController, SIGNAL(finished()), this, SLOT(queueControllerFinished()));

  this->m_QueueDialog->setVisible(true);
  registerButton->setEnabled(false);

  m_QueueController->start();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::addProcess(CrossCorrelationTask* task)
{
  this->m_QueueDialog->addProcess(task);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList QCrossCorrelation::generateInputFileList()
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
void QCrossCorrelation::on_actionOpen_Processed_Image_triggered()
{
  //std::cout << "on_actionOpen_triggered" << std::endl;
  QString imageFile = QFileDialog::getOpenFileName(this, tr("Open Segmented Image File"),
    m_OpenDialogLastDirectory,
    tr("Images (*.tif *.bmp *.jpg *.png)") );

  if ( true == imageFile.isEmpty() )
  {
    return;
  }
  openProcessedImage(imageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_actionOpen_triggered()
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
void QCrossCorrelation::loadImageFile(const QString &imageFile)
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
void QCrossCorrelation::loadProcessedImageFile(const QString  &imageFile)
{
  if ( true == imageFile.isEmpty() )
  {
    return;
  }
  openProcessedImage(imageFile);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_actionSave_triggered()
{
  saveProcessedImage();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_actionSave_As_triggered()
{
  m_CurrentProcessedFile = QString();
  saveProcessedImage();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_actionClose_triggered() {
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
void QCrossCorrelation::openProcessedImage(QString processedImage)
{
  if ( true == processedImage.isEmpty() ) // User cancelled the operation
  {
    return;
  }
  this->initWithFile(m_CurrentImageFile, processedImage);
  setWidgetListEnabled(true);
  on_modeComboBox_currentIndexChanged();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 QCrossCorrelation::saveProcessedImage()
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
    processedImageTitle->setText(m_CurrentProcessedFile);
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
void QCrossCorrelation::queueControllerFinished()
{
  this->statusBar()->showMessage("Accumulating Translations and writing final images");
  m_QueueDialog->setVisible(false);

  CrossCorrelation::Pointer cc = CrossCorrelation::New();
  CrossCorrelationData::Pointer ccData = CrossCorrelationData::NullPointer();
  if (this->processFolder->isChecked() == false)
   {
    AIMImage::Pointer image = loadImage(movingImageFile->text());
    ccData = m_CrossCorrelationTable->getCrossCorrelationData(0);

    QFileInfo fileInfo(movingImageFile->text());
    QString basename = fileInfo.completeBaseName();
    QString extension = fileInfo.suffix();
    QString filepath = fileInfo.absolutePath();
    filepath.append(QDir::separator());
    filepath.append(basename);
    filepath.append("_Registered");
    filepath.append(".");
    filepath.append(extension);

    cc->writeOutputImage(image, ccData, filepath.toStdString());
    loadImageFile(fixedImageFile->text());
    loadProcessedImageFile(filepath);

   }
  else
  {
    QStringList fileList = generateInputFileList();
    double xt = 0.0;
    double yt = 0.0;
    int32_t count = fileList.count();
    for (int32_t i = 0; i < count; ++i)
    {
      writeRegisteredImage(fileList.at(i), i, xt, yt);
    }
  }

  registerButton->setEnabled(true);
  m_QueueController->deleteLater();
  m_QueueController = NULL;
  this->statusBar()->showMessage("Registration Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QCrossCorrelation::writeRegisteredImage(QString file, int i, double &xt, double &yt)
{
  QString inFile(sourceDirectoryLE->text() + QDir::separator() + file);
  AIMImage::Pointer image = loadImage(inFile);
  if (NULL == image)
  {
    qDebug("Error Loading image to translate.");
    return -1;
  }
  CrossCorrelationData::Pointer ccData = m_CrossCorrelationTable->getCrossCorrelationData(i);
  // Accumulate the translations
  xt = xt + ccData->getXTrans();
  yt = yt + ccData->getYTrans();
  ccData->setXTrans(xt);
  ccData->setYTrans(yt);

  QFileInfo fileInfo(file);
  QString basename = fileInfo.completeBaseName();
  QString extension = fileInfo.suffix();
  QString filepath = outputDirectoryLE->text();
  filepath.append(QDir::separator());
  filepath.append(outputPrefix->text());
  filepath.append(basename);
  filepath.append(outputSuffix->text());
  filepath.append(".");
  filepath.append(outputImageType->currentText());

  CrossCorrelation::Pointer cc = CrossCorrelation::New();
  cc->writeOutputImage(image, ccData, filepath.toStdString());
  if (i == 0)
  {
    loadImageFile(filepath);
  }
  if ( i == 1)
  {
    loadProcessedImageFile(filepath);
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::Pointer QCrossCorrelation::loadImage(QString filePath)
{
//  std::cout << " loadImage(): filePath: " << filePath.toStdString() << std::endl;
  QImage image;
  AIMImage::Pointer aimImage = AIMImage::NullPointer();
  if (filePath.isEmpty() == false)
  {
    image = QImage(filePath);
    if (image.isNull() == true)
    {
      QString m("Error loading image from ");
      m.append(filePath);
      qDebug(m.toAscii());
      return aimImage;
    }
    QVector<QRgb> colorTable(256);
    for (quint32 i = 0; i < 256; ++i)
    {
      colorTable[i] = qRgb(i, i, i);
    }
    image.setColorTable(colorTable);
    aimImage = convertQImageToGrayScaleAIMImage(image);
    if (NULL == aimImage.data())
    {
      return aimImage;
    }
  }
  return aimImage;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 QCrossCorrelation::initGraphicViews()
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
    originalImageGView->setScene(m_OriginalImageGScene);
    m_OriginalGDelegate = new MXAImageGraphicsDelegate(this);
    m_OriginalGDelegate->setGraphicsView(originalImageGView);
    m_OriginalGDelegate->setGraphicsScene(m_OriginalImageGScene);
    m_OriginalGDelegate->setMainWindow(this);
    m_OriginalGDelegate->setCachedImage(image);
    m_OriginalGDelegate->fitToWindow();
    connect(this, SIGNAL(parentResized () ),
            m_OriginalGDelegate, SLOT(on_parentResized () ), Qt::QueuedConnection);

    connect(zoomIn, SIGNAL(clicked()),
            m_OriginalGDelegate, SLOT(increaseZoom() ));

    connect(zoomOut, SIGNAL(clicked()),
            m_OriginalGDelegate, SLOT(decreaseZoom() ));

    connect(fitToWindow, SIGNAL(clicked()),
            m_OriginalGDelegate, SLOT(fitToWindow() ));

    // Create the m_OriginalImage and m_Segmented Image Objects


    m_OriginalImage = convertQImageToGrayScaleAIMImage(image);
    if (NULL == m_OriginalImage.data())
    {
      return -1;
    }

  }


  // If we have NOT loaded a segmented file AND we have a valid Original Image, then
  // create the Segmented image based on the input image.
  QImage segImage;
  if (m_CurrentProcessedFile.isEmpty() == true && NULL != m_OriginalImage.data() )
  {
    segImage = image;
    m_ProcessedImage = convertQImageToGrayScaleAIMImage(segImage);
    if (NULL == m_ProcessedImage.data())
    {
      return -1;
    }
  }
  else // We have an actual segmented image file that the user wants us to read.
  {
    segImage = QImage(m_CurrentProcessedFile);
    if (segImage.isNull() == true)
    {
      this->statusbar->showMessage("Error loading Segmented image from file");
      return -1;
    }
    // Convert it to an AIMImage in GrayScale
    m_ProcessedImage = convertQImageToGrayScaleAIMImage(segImage);
    if (NULL == m_ProcessedImage.data() )
    {
      std::cout << "Error loading Segmented image from file" << std::endl;
      return -1;
    }
  }

  if (NULL != m_ProcessedImage.data() )
  {


    // Create the QGraphicsScene Objects
    m_ProcessedImageGScene = new QGraphicsScene(this);
    segmentedImageGView->setScene(m_ProcessedImageGScene);
    m_ProcessedGDelegate = new MXAImageGraphicsDelegate(this);
    m_ProcessedGDelegate->setGraphicsView(segmentedImageGView);
    m_ProcessedGDelegate->setGraphicsScene(m_ProcessedImageGScene);
    m_ProcessedGDelegate->setMainWindow(this);
    m_ProcessedGDelegate->setCachedImage(segImage);
    m_ProcessedGDelegate->fitToWindow();
    connect(this, SIGNAL(parentResized () ),
            m_ProcessedGDelegate, SLOT(on_parentResized () ), Qt::QueuedConnection);

    connect(zoomIn_mount, SIGNAL(clicked()),
            m_ProcessedGDelegate, SLOT(increaseZoom() ));

    connect(zoomOut_mount, SIGNAL(clicked()),
            m_ProcessedGDelegate, SLOT(decreaseZoom() ));

    connect(fitToWindow_mount, SIGNAL(clicked()),
            m_ProcessedGDelegate, SLOT(fitToWindow() ));
  }
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::initWithFile(const QString imageFile, QString processedImage)
{
  QFileInfo fileInfo(imageFile);
  this->m_OpenDialogLastDirectory = fileInfo.path();

  m_CurrentImageFile = imageFile;
  m_CurrentProcessedFile = processedImage;

  qint32 err = initGraphicViews();

  if (err < 0)
  {
    this->statusBar()->showMessage("Error Loading Original Image");
    return;
  }
  this->originalImageTitle->setText(fileInfo.fileName());
  this->originalImageTitle->setToolTip(m_CurrentImageFile);

  if (m_CurrentProcessedFile.isEmpty())
  {
    this->processedImageTitle->setText("Unsaved Segmented Image");
    this->setWindowModified(true);
  }
  else {
    QFileInfo segInfo(m_CurrentProcessedFile);
    this->processedImageTitle->setText(segInfo.fileName());
    this->processedImageTitle->setToolTip(m_CurrentProcessedFile);
  }
  this->statusBar()->showMessage("Input Image Loaded");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_filterPatternLineEdit_textChanged()
{
 // std::cout << "filterPattern: " << std::endl;
  m_ProxyModel->setFilterFixedString(filterPatternLineEdit->text());
  m_ProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_processFolder_stateChanged(int state)
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_sourceDirectoryBtn_clicked() {
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
void QCrossCorrelation::on_outputDirectoryBtn_clicked()
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
void QCrossCorrelation::populateFileTable()
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
AIMImage::Pointer QCrossCorrelation::convertQImageToGrayScaleAIMImage(QImage image)
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
void QCrossCorrelation::on_movingImageButton_clicked()
{
  QString imageFile = QFileDialog::getOpenFileName(this, tr("Select Moving Image"),
    m_OpenDialogLastDirectory,
    tr("Images (*.tif *.tiff *.bmp *.jpg *.jpeg *.png)") );

  if ( true == imageFile.isEmpty() )
  {
    return;
  }
  movingImageFile->setText(imageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_fixedImageButton_clicked()
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
void QCrossCorrelation::on_fixedImageFile_textChanged(const QString & text)
{
  verifyPathExists(fixedImageFile->text(), fixedImageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_movingImageFile_textChanged(const QString & text)
{
  verifyPathExists(movingImageFile->text(), movingImageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_sourceDirectoryLE_textChanged(const QString & text)
{
  verifyPathExists(sourceDirectoryLE->text(), sourceDirectoryLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QCrossCorrelation::on_outputDirectoryLE_textChanged(const QString & text)
{
  verifyPathExists(outputDirectoryLE->text(), outputDirectoryLE);
}

