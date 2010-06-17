///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef QFSDROPPABLEGRAPHICSVIEW_H_
#define QFSDROPPABLEGRAPHICSVIEW_H_

#include <QtGui/QGraphicsView>

/**
* @class QFSDroppableGraphicsView QFSDroppableGraphicsView.h EmMpm/Common/Qt/QFSDroppableGraphicsView.h
* @brief
* @author Michael A. Jackson for BlueQuartz Software
* @date Apr 5, 2010
* @version 1.0
*/
class QFSDroppableGraphicsView : public QGraphicsView
{
    Q_OBJECT

  public:
    QFSDroppableGraphicsView(QWidget *parent = NULL);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);

  protected:

  signals:
   void loadImageFileRequested(const QString &filename);

};

#endif /* QFSDROPPABLEGRAPHICSVIEW_H_ */
