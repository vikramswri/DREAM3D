///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "QEmMpm.h"
#include "EmMpmTask.h"
#include "EmMpmThread.h"
#include "EmMpm/Common/Qt/AboutBox.h"
#include "EmMpm/Common/Qt/QRecentFileList.h"
#include "EmMpm/Common/Qt/QFileCompleter.h"
#include "EmMpm/Common/Qt/AIMImageGraphicsDelegate.h"


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



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QEmMpm::QEmMpm(QWidget *parent) :
  QMainWindow(parent),
#if defined(Q_WS_WIN)
      m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  readSettings();
  setupGui();
  m_EmMpmThread = NULL;
  m_OutputExistsCheck = false;
  QRecentFileList* recentFileList = QRecentFileList::instance();
  connect(recentFileList, SIGNAL (fileListChanged(const QString &)), this, SLOT(updateRecentFileList(const QString &)));
  // Get out initial Recent File List
  this->updateRecentFileList(QString::null);
  //this->setAcceptDrops(true);
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
  qint32 err = _checkDirtyDocument();
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
  _openFile(file);
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
bool QEmMpm::_verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  QDir parent(fileinfo.dir());
  return parent.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEmMpm::_verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
qint32 QEmMpm::_checkDirtyDocument()
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
    _openFile( file );
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEmMpm::_openFile(QString imageFile)
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
  if (m_SegmentBtn->text().compare("Cancel") == 0)
  {
    if (m_EmMpmThread != NULL)
    {
      emit cancelTask();
    }
    return;
  }

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



  EmMpmTask* task = new EmMpmTask(NULL);

  task->setOriginalImage(m_OriginalImage);
  task->setSegmentedImage(m_SegmentedImage);

  bool ok;
  task->setBeta(m_Beta->text().toFloat(&ok));
  task->setGamma(m_Gamma->text().toFloat(&ok));
  task->setEmIterations(m_EmIterations->value() );
  task->setMpmIterations(m_MpmIterations->value());
  task->setNumberOfClasses(m_NumClasses->value() );
  if (useSimulatedAnnealing->isChecked() )
  {
    task->useSimulatedAnnealing();
  }



  m_EmMpmThread = new EmMpmThread(task, this);
  task->moveToThread(m_EmMpmThread);
  connect(m_EmMpmThread, SIGNAL(started()), task, SLOT(run()));
  connect(task, SIGNAL(sendTaskFinished() ), m_EmMpmThread, SLOT(quit()), Qt::DirectConnection);
  connect(m_EmMpmThread, SIGNAL(finished()), this, SLOT(receiveTaskFinished() ));
  connect(task, SIGNAL(sendTaskMessage(const QString &) ), this, SLOT(receiveTaskMessage(const QString &) ) );
  connect(task, SIGNAL(sendTaskProgress(int) ), this, SLOT(receiveTaskProgress(int) ) );
  connect(this, SIGNAL(cancelTask()), task, SLOT(cancelTask()));

  m_SegmentBtn->setText("Cancel");
  m_EmMpmThread->start();
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
  _openSegmentedImage(imageFile);
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
  _openFile(imageFile);

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
  _openFile(imageFile);
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
  _openSegmentedImage(imageFile);
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
  err = _checkDirtyDocument();
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
void QEmMpm::_openSegmentedImage(QString mountImage)
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

  // m_SegmentedImage = m_ThreadedFillHolesFilter->getOutputImage();
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
  for (qint32 j=0; j<height; j++) {
    for (qint32 i =0; i<width; i++) {
      index = (j *  width) + i;
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
AIMImage::Pointer QEmMpm::convertQImageToGrayScaleAIMImage(QImage image)
{
  AIMImage::Pointer aimImage = AIMImage::New();
  quint8* oImage = aimImage->allocateImageBuffer(image.width(), image.height(), true);
  if (NULL == oImage)
  {
    statusbar->showMessage("Error creating AIMImage object from QImage");
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

