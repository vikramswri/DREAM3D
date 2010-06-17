///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "QFSDroppableGraphicsView.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtGui/QWidget>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QtDebug>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFSDroppableGraphicsView::QFSDroppableGraphicsView(QWidget *parent)
: QGraphicsView(parent)
{
  setAcceptDrops(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFSDroppableGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
 // qWarning("QFSDroppableGraphicsView::dragEnterEvent(QDragEnterEvent *event)");
  // accept just text/uri-list mime format
  if (event->mimeData()->hasFormat("text/uri-list"))
  {
    event->acceptProposedAction();
  }
  this->setStyleSheet("border: 1px solid green;");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFSDroppableGraphicsView::dragLeaveEvent(QDragLeaveEvent *event)
{
//  qWarning("QFSDroppableGraphicsView::dragLeaveEvent(QDragLeaveEvent *event)");
  this->setStyleSheet("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFSDroppableGraphicsView::dropEvent(QDropEvent *event)
{
  this->setStyleSheet("");
//  qWarning("QFSDroppableGraphicsView::dropEvent(QDropEvent *event)");
  QList<QUrl> urlList;
  QString fName;
  QFileInfo info;

  if (event->mimeData()->hasUrls())
  {
    urlList = event->mimeData()->urls(); // returns list of QUrls
    // if just text was dropped, urlList is empty (size == 0)

    if ( urlList.size() > 0) // if at least one QUrl is present in list
    {
      fName = urlList[0].toLocalFile(); // convert first QUrl to local path
      info.setFile( fName ); // information about file
      QString ext = info.suffix();
      if (ext.compare("tif") == 0
          || ext.compare("jpg") == 0
          || ext.compare("png") == 0
          || ext.compare("bmp") == 0)
      {
        emit loadImageFileRequested(fName);
      }
    }
  }
  event->acceptProposedAction();
}



