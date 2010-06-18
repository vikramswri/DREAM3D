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

#include "QEmMpm.h"
#include "EmMpmTask.h"

#include "ProcessQueueController.h"
#include "ProcessQueueDialog.h"
#include "EMMPM/Common/Qt/AboutBox.h"
#include "EMMPM/Common/Qt/QRecentFileList.h"
#include "EMMPM/Common/Qt/QFileCompleter.h"
#include "EMMPM/Common/Qt/AIMImageGraphicsDelegate.h"


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
  m_SegmentedImageGScene = NULL;

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
           this, SLOT(loadSegmentedImageFile(const QString &)));

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
  m_SegmentedGDelegate->setOverlayImage(m_OriginalGDelegate->getCachedImage());
  m_SegmentedGDelegate->setCompositeImages( compositeWithOriginal->isChecked() );
  m_SegmentedGDelegate->updateGraphicsScene();
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
  this->initWithFile(imageFile, m_CurrentSegmentedFile);

  // Tell the RecentFileList to update itself then broadcast those changes.
  QRecentFileList::instance()->addFile(imageFile);
  setWidgetListEnabled(true);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_aboutBtn_clicked()
{
  AboutBox about(this);
  QString an = QCoreApplication::applicationName();
  about.setApplicationName(an);
  about.exec();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::on_m_SegmentBtn_clicked()
{
//
//  if (m_QueueController != NULL && m_QueueController->isFinished() == false)
//  {
//    emit cancelProcessQueue();
//    return;
//  }



  if (this->m_OutputExistsCheck == false)
  {
    QFile file (this->m_CurrentSegmentedFile );
    if (file.exists() == true)
    {
     int ret = QMessageBox::warning(this, tr("QEmMpm"),
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
          this->m_CurrentSegmentedFile = "";
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
      outputDir.mkpath(".");
    }

  }

  m_QueueDialog->clearTable();

  m_QueueController = new ProcessQueueController(this);
  bool ok;
  if (this->processFolder->isChecked() == false)
  {
    if (m_CurrentImageFile.isEmpty() )
    {
      QMessageBox::critical(this, tr("Input File Error"),
                                    tr("No image file is open in the viewer to segment. Use the File->Open menu itme to open an image first."),
                                    QMessageBox::Ok);
      m_QueueController->deleteLater();
      return;
    }
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
    task->setInputFilePath(m_CurrentImageFile);
    QFileInfo fileInfo(m_CurrentImageFile);
    QString basename = fileInfo.completeBaseName();
    QString extension = fileInfo.suffix();
    QString filepath = fileInfo.absolutePath();
    filepath.append(QDir::separator());
    filepath.append(basename);
    filepath.append("_Segmented");
    filepath.append(".");
    filepath.append(extension);
    task->setOutputFilePath(filepath);

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
  // When the ProcessQueue finishes it will signal the QueueController to 'quit', thus stopping the thread
  connect(m_QueueController, SIGNAL(finished()), this, SLOT(queueControllerFinished()));

  this->m_QueueDialog->setVisible(true);
  m_SegmentBtn->setEnabled(false);

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
    tr("Images (*.tif *.bmp *.jpg *.png)") );

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
  m_CurrentSegmentedFile = QString();
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
  if (NULL == m_SegmentedGDelegate) { return; }
  int index = modeComboBox->currentIndex();
  switch(index)
  {
  case 0: m_SegmentedGDelegate->setExclusionMode(); break;
  case 1: m_SegmentedGDelegate->setSourceMode(); break;
  case 2: m_SegmentedGDelegate->setDestinationMode(); break;
  case 3: m_SegmentedGDelegate->setSourceOverMode(); break;
  case 4: m_SegmentedGDelegate->setDestinationOverMode(); break;
  case 5: m_SegmentedGDelegate->setSourceInMode(); break;
  case 6: m_SegmentedGDelegate->setDestInMode(); break;
  case 7: m_SegmentedGDelegate->setDifferenceMode(); break;
  case 8: m_SegmentedGDelegate->setDestOutMode(); break;
  case 9: m_SegmentedGDelegate->setSourceAtopMode(); break;
  case 10: m_SegmentedGDelegate->setDestAtopMode(); break;
  case 11: m_SegmentedGDelegate->setPlusMode(); break;
  case 12: m_SegmentedGDelegate->setMultiplyMode(); break;
  case 13: m_SegmentedGDelegate->setScreenMode(); break;
  case 14: m_SegmentedGDelegate->setOverlayMode(); break;
  case 15: m_SegmentedGDelegate->setDarkenMode(); break;
  case 16: m_SegmentedGDelegate->setLightenMode(); break;
  case 17: m_SegmentedGDelegate->setColorDodgeMode(); break;
  case 18: m_SegmentedGDelegate->setColorBurnMode(); break;
  case 19: m_SegmentedGDelegate->setHardLightMode(); break;
  case 20: m_SegmentedGDelegate->setSoftLightMode(); break;


  default:
    m_SegmentedGDelegate->setExclusionMode(); break;
  }

  m_SegmentedGDelegate->setOverlayImage(m_OriginalGDelegate->getCachedImage());
  m_SegmentedGDelegate->setCompositeImages( compositeWithOriginal->isChecked() );
  m_SegmentedGDelegate->updateGraphicsScene();
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
  QImage image = m_SegmentedGDelegate->getCachedImage();
  int err = 0;
  if (m_CurrentSegmentedFile.isEmpty())
  {
    QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator() + "Segmented.tif";
    outputFile = QFileDialog::getSaveFileName(this, tr("Save Segmented Image As ..."), outputFile, tr("Images (*.tif *.bmp *.jpg *.png)"));
    if ( !outputFile.isEmpty() )
    {
      m_CurrentSegmentedFile = outputFile;
    }
    else {
      return -1;
    }
  }

  bool ok = image.save(m_CurrentSegmentedFile);
  if (ok == true) {
    segmentedImageTitle->setText(m_CurrentSegmentedFile);
  }
  else
  {
    //TODO: Add in a GUI dialog to help explain the error or give suggestions.
    err = -1;
  }
  this->setWindowModified(false);
  return err;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::receiveTaskMessage(const QString &message)
{
  this->statusBar()->showMessage(message);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::receiveTaskProgress(int p)
{
  this->progressBar->setValue(p);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::receiveTaskFinished()
{
  m_SegmentBtn->setText("Segment");
  // this->statusbar->showMessage("Ready To Encode");

  m_EmMpmThread->deleteLater();
  this->progressBar->setValue(0);

  // Now put the image into the QGraphicsView
  qint32 width = m_SegmentedImage->getImagePixelWidth();
  qint32 height = m_SegmentedImage->getImagePixelHeight();
  quint8* dataPointer = m_SegmentedImage->getImageBuffer();
  QImage image(width, height, QImage::Format_Indexed8);
  QVector<QRgb> colorTable(256);
  for (quint32 i = 0; i < 256; ++i)
  {
    colorTable[i] = qRgb(i, i, i);
  }
  image.setColorTable(colorTable);
  qint32 index;
  for (qint32 j=0; j<height; j++)
  {
    for (qint32 i =0; i<width; i++)
    {
      index = (j * width) + i;
      image.setPixel(i, j, dataPointer[index]);
    }
  }
  m_SegmentedGDelegate->setCachedImage(image);
  m_SegmentedGDelegate->setOverlayImage(m_OriginalGDelegate->getCachedImage());
  m_SegmentedGDelegate->setCompositeImages( compositeWithOriginal->isChecked() );
  m_SegmentedGDelegate->updateGraphicsScene();
  this->setWindowModified(true);
  setWidgetListEnabled(true);
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::queueControllerFinished()
{
  m_SegmentBtn->setEnabled(true);
  m_QueueController->deleteLater();
  m_QueueController = NULL;
 // std::cout << "QEmMpm::queueControllerFinished() --- Complete" << std::endl;
  m_QueueDialog->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 QEmMpm::initGraphicViews()
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
    m_OriginalGDelegate = new AIMImageGraphicsDelegate(this);
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
  if (m_CurrentSegmentedFile.isEmpty() == true && NULL != m_OriginalImage.data() )
  {
    segImage = image;
    m_SegmentedImage = convertQImageToGrayScaleAIMImage(segImage);
    if (NULL == m_SegmentedImage.data())
    {
      return -1;
    }
  }
  else // We have an actual segmented image file that the user wants us to read.
  {
    segImage = QImage(m_CurrentSegmentedFile);
    if (segImage.isNull() == true)
    {
      this->statusbar->showMessage("Error loading Segmented image from file");
      return -1;
    }
    // Convert it to an AIMImage in GrayScale
    m_SegmentedImage = convertQImageToGrayScaleAIMImage(segImage);
    if (NULL == m_SegmentedImage.data() )
    {
      std::cout << "Error loading Segmented image from file" << std::endl;
      return -1;
    }
  }

  if (NULL != m_SegmentedImage.data() )
  {


    // Create the QGraphicsScene Objects
    m_SegmentedImageGScene = new QGraphicsScene(this);
    segmentedImageGView->setScene(m_SegmentedImageGScene);
    m_SegmentedGDelegate = new AIMImageGraphicsDelegate(this);
    m_SegmentedGDelegate->setGraphicsView(segmentedImageGView);
    m_SegmentedGDelegate->setGraphicsScene(m_SegmentedImageGScene);
    m_SegmentedGDelegate->setMainWindow(this);
    m_SegmentedGDelegate->setCachedImage(segImage);
    m_SegmentedGDelegate->fitToWindow();
    connect(this, SIGNAL(parentResized () ),
            m_SegmentedGDelegate, SLOT(on_parentResized () ), Qt::QueuedConnection);

    connect(zoomIn_mount, SIGNAL(clicked()),
            m_SegmentedGDelegate, SLOT(increaseZoom() ));

    connect(zoomOut_mount, SIGNAL(clicked()),
            m_SegmentedGDelegate, SLOT(decreaseZoom() ));

    connect(fitToWindow_mount, SIGNAL(clicked()),
            m_SegmentedGDelegate, SLOT(fitToWindow() ));
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
  m_CurrentSegmentedFile = mountImage;

  qint32 err = initGraphicViews();
  if (err < 0)
  {
    this->statusBar()->showMessage("Error Loading Original Image");
    return;
  }
  this->originalImageTitle->setText(fileInfo.fileName());
  this->originalImageTitle->setToolTip(m_CurrentImageFile);

  if (m_CurrentSegmentedFile.isEmpty())
  {
    this->segmentedImageTitle->setText("Unsaved Segmented Image");
    this->setWindowModified(true);
  }
  else {
    QFileInfo segInfo(m_CurrentSegmentedFile);
    this->segmentedImageTitle->setText(segInfo.fileName());
    this->segmentedImageTitle->setToolTip(m_CurrentSegmentedFile);
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
  bool enabled = false;
  if (state == Qt::Checked)
  {
    enabled = true;
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

