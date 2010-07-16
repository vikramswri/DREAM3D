///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "CrossCorrelationInputUI.h"
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
#include "CrossCorrelationTask.h"
#include "CrossCorrelation/CrossCorrelation.h"

#include "TO79/Common/TO79Version.h"


#define READ_STRING_SETTING(prefs, var, emptyValue)\
  var->setText( prefs->value(#var).toString() );\
  if (var->text().isEmpty() == true) { var->setText(emptyValue); }

#define READ_SETTING(prefs, var, ok, temp, default, type)\
  ok = false;\
  temp = prefs->value(#var).to##type(&ok);\
  if (false == ok) {temp = default;}\
  var->setValue(temp);

#define WRITE_STRING_SETTING(prefs, var)\
  prefs->setValue(#var , this->var->text());

#define WRITE_SETTING(prefs, var)\
  prefs->setValue(#var, this->var->value());

#define READ_BOOL_SETTING(prefs, var, emptyValue)\
  { QString s = prefs->value(#var).toString();\
  if (s.isEmpty() == false) {\
    bool bb = prefs->value(#var).toBool();\
  var->setChecked(bb); } else { var->setChecked(emptyValue); } }

#define WRITE_BOOL_SETTING(prefs, var, b)\
    prefs->setValue(#var, (b) );

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationInputUI::CrossCorrelationInputUI(QWidget *parent) :
QFrame(parent),
m_ProxyModel(NULL),
#if defined(Q_WS_WIN)
      m_OpenDialogLastDirectory("C:\\")
#else
m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);

  setupGui();
  //  m_CrossCorrelationThread = NULL;
  m_QueueController = NULL;
  m_OutputExistsCheck = false;
  m_QueueDialog = new ProcessQueueDialog(this);
  m_QueueDialog->setVisible(false);

  m_CrossCorrelationTable = CrossCorrelationTable::New();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationInputUI::~CrossCorrelationInputUI()
{

}

// -----------------------------------------------------------------------------
//  Read the prefs from the local storage file
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::readSettings(QSettings* prefs)
{

  QString val;
//  bool ok;
//  qint32 i;
  prefs->beginGroup("CrossCorrelationPlugin");
  READ_BOOL_SETTING(prefs, processFolder, false);
  READ_STRING_SETTING(prefs, fixedImageFile, "");
  READ_STRING_SETTING(prefs, movingImageFile, "");
  READ_STRING_SETTING(prefs, outputImageFile, "");
  READ_STRING_SETTING(prefs, sourceDirectoryLE, "");
  READ_STRING_SETTING(prefs, outputDirectoryLE, "");
  READ_STRING_SETTING(prefs, outputPrefix, "");
  READ_STRING_SETTING(prefs, outputSuffix, "");
  prefs->endGroup();
  on_processFolder_stateChanged(processFolder->isChecked());

  if (this->sourceDirectoryLE->text().isEmpty() == false)
  {
    this->populateFileTable();
  }

}

// -----------------------------------------------------------------------------
//  Write our prefs to file
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::writeSettings(QSettings* prefs)
{
  std::cout << "CrossCorrelationInputUI::writeSettings" << std::endl;

  prefs->beginGroup("CrossCorrelationPlugin");
  WRITE_BOOL_SETTING(prefs, processFolder, processFolder->isChecked());
  WRITE_STRING_SETTING(prefs, fixedImageFile);
  WRITE_STRING_SETTING(prefs, movingImageFile);
  WRITE_STRING_SETTING(prefs, outputImageFile);
  WRITE_STRING_SETTING(prefs, sourceDirectoryLE);
  WRITE_STRING_SETTING(prefs, outputDirectoryLE);
  WRITE_STRING_SETTING(prefs, outputPrefix);
  WRITE_STRING_SETTING(prefs, outputSuffix);
  prefs->endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::setupGui()
{
  QFileCompleter* com = new QFileCompleter(this, false);
  fixedImageFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_fixedImageFile_textChanged(const QString &)));

  QFileCompleter* com1 = new QFileCompleter(this, false);
  movingImageFile->setCompleter(com1);
  QObject::connect( com1, SIGNAL(activated(const QString &)),
           this, SLOT(on_movingImageFile_textChanged(const QString &)));

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
void CrossCorrelationInputUI::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CrossCorrelationInputUI::verifyOutputPathParentExists(QString outFilePath, QLineEdit* lineEdit)
{
  QFileInfo fileinfo(outFilePath);
  QDir parent(fileinfo.dir());
  return parent.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CrossCorrelationInputUI::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
int CrossCorrelationInputUI::processInputs(QObject* parentGUI)
{

  /* If the 'processFolder' checkbox is checked then we need to check for some
   * additional inputs
   */
  if (this->processFolder->isChecked())
  {
    if (this->sourceDirectoryLE->text().isEmpty() == true)
    {
      QMessageBox::critical(this, tr("Input Parameter Error"),
                                    tr("Source Directory must be set."),
                                    QMessageBox::Ok);
      return -1;
    }

    if (this->outputDirectoryLE->text().isEmpty() == true)
    {
      QMessageBox::critical(this, tr("Output Parameter Error"),
                                    tr("Output Directory must be set."),
                                    QMessageBox::Ok);
      return -1;
    }

    if (this->fileListView->model()->rowCount() == 0  )
    {
      QMessageBox::critical(this, tr("Parameter Error"),
                                    tr("No image files are available in the file list view."),
                                    QMessageBox::Ok);
      return -1;
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
        return -1;
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
     return -1;
   }
   QFileInfo mfi(movingImageFile->text());
   if (mfi.exists() == false)
   {
     QMessageBox::critical(this, tr("Moving Image File Error"),
                                   tr("Moving Image does not exist. Please check the path."),
                                   QMessageBox::Ok);
     return -1;
   }
   if (outputImageFile->text().isEmpty() == true)
   {
     QMessageBox::critical(this, tr("Output Image File Error"),
                                   tr("Please select a file name for the registered image to be saved as."),
                                   QMessageBox::Ok);
     return -1;
   }
   QFile file (outputImageFile->text() );
   if (file.exists() == true)
   {
     int ret = QMessageBox::warning(this, tr("CrossCorrelationInputUI"),
         tr("The Output File Already Exists\nDo you want to over write the existing file?"),
         QMessageBox::No | QMessageBox::Default,
         QMessageBox::Yes,
         QMessageBox::Cancel);
     if (ret == QMessageBox::Cancel)
     {
       return -1;
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
         return -1;
       }
     }
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
      return -1;
    }
    if (movingImageFile->text().isEmpty() )
    {
      QMessageBox::critical(this, tr("Input File Error"),
                                    tr("Moving Image is not selected"),
                                    QMessageBox::Ok);
      m_QueueController->deleteLater();
      return -1;
    }

    CrossCorrelationTask* task = new CrossCorrelationTask(NULL);

    task->setInputFilePath(fixedImageFile->text());
    task->setMovingImagePath(movingImageFile->text());
    task->setOutputFilePath(outputImageFile->text());
    CrossCorrelationData::Pointer crossCorrelationData = CrossCorrelationData::New();
    crossCorrelationData->setFixedSlice(0);
    crossCorrelationData->setMovingSlice(1);
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
      crossCorrelationData->setFixedSlice(i);
      crossCorrelationData->setMovingSlice(i+1);
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

  connect(m_QueueController, SIGNAL(started()), parentGUI, SLOT(processingStarted()));
  connect(m_QueueController, SIGNAL(finished()), parentGUI, SLOT(processingFinished()));

  this->m_QueueDialog->setVisible(true);

  m_QueueController->start();
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::addProcess(CrossCorrelationTask* task)
{
  this->m_QueueDialog->addProcess(task);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList CrossCorrelationInputUI::generateInputFileList()
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
void CrossCorrelationInputUI::queueControllerFinished()
{
  m_QueueDialog->setVisible(false);

  CrossCorrelation::Pointer cc = CrossCorrelation::New();
  CrossCorrelationData::Pointer ccData = CrossCorrelationData::NullPointer();
  if (this->processFolder->isChecked() == false)
   {
    AIMImage::Pointer image = loadImage(movingImageFile->text());
    ccData = m_CrossCorrelationTable->getCrossCorrelationData(0);

    cc->writeRegisteredImage(image, ccData, outputImageFile->text().toStdString());
    m_CurrentImageFile = fixedImageFile->text();
    m_CurrentProcessedFile = outputImageFile->text();
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

  setWidgetListEnabled(true);


  m_QueueController->deleteLater();
  m_QueueController = NULL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImage::Pointer CrossCorrelationInputUI::loadImage(QString filePath)
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
AIMImage::Pointer CrossCorrelationInputUI::convertQImageToGrayScaleAIMImage(QImage image)
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
int CrossCorrelationInputUI::writeRegisteredImage(QString file, int i, double &xt, double &yt)
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
  cc->writeRegisteredImage(image, ccData, filepath.toStdString());
  if (i == 0)
  {
    m_CurrentImageFile = filepath;
  }
  if ( i == 1)
  {
    m_CurrentProcessedFile = filepath;
  }
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::on_filterPatternLineEdit_textChanged()
{
 // std::cout << "filterPattern: " << std::endl;
  m_ProxyModel->setFilterFixedString(filterPatternLineEdit->text());
  m_ProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::on_processFolder_stateChanged(int state)
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
  movingImageFile->setEnabled(!enabled);
  movingImageButton->setEnabled(!enabled);

  outputImageFile->setEnabled(!enabled);
  outputImageButton->setEnabled(!enabled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::on_sourceDirectoryBtn_clicked() {
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
void CrossCorrelationInputUI::on_outputDirectoryBtn_clicked()
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
void CrossCorrelationInputUI::populateFileTable()
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
void CrossCorrelationInputUI::on_fixedImageButton_clicked()
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
void CrossCorrelationInputUI::on_movingImageButton_clicked()
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
void CrossCorrelationInputUI::on_outputImageButton_clicked()
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
void CrossCorrelationInputUI::on_fixedImageFile_textChanged(const QString & text)
{
  verifyPathExists(fixedImageFile->text(), fixedImageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::on_movingImageFile_textChanged(const QString & text)
{
  verifyPathExists(movingImageFile->text(), movingImageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::on_outputImageFile_textChanged(const QString & text)
{
//  verifyPathExists(outputImageFile->text(), movingImageFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::on_sourceDirectoryLE_textChanged(const QString & text)
{
  verifyPathExists(sourceDirectoryLE->text(), sourceDirectoryLE);
  this->populateFileTable();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationInputUI::on_outputDirectoryLE_textChanged(const QString & text)
{
  verifyPathExists(outputDirectoryLE->text(), outputDirectoryLE);
}


