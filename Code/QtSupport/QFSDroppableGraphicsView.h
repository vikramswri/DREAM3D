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
* @class QFSDroppableGraphicsView QFSDroppableGraphicsView.h QtSupport/QFSDroppableGraphicsView.h
* @brief A QGraphicsView object that handles files being dropped on it from the
* filesystem. After the file is dropped the signal "loadImageFileRequested(QString)" is
* emitted. Hooking up to this signal with a slot will allow the developer to open
* the file. The complete path to the file is containted in the QString argument.
* @author Michael A. Jackson for BlueQuartz Software
* @date Apr 5, 2010
* @version 1.0
*/
class QFSDroppableGraphicsView : public QGraphicsView
{
    Q_OBJECT

  public:
    QFSDroppableGraphicsView(QWidget *parent = NULL);

    /**
     * @brief Over-riding implementation from base class
     * @param event QDragEnterEvent Event fired when dragging enters the QGraphicsView
     */
    void dragEnterEvent(QDragEnterEvent *event);

    /**
     * @brief Over-riding implementation from base class
     * @param event QDropEvent Event fired when object is dropped on QGraphicsView
     */
    void dropEvent(QDropEvent *event);

    /**
     * @brief Over-riding implementation from base class
     * @param event QDragLeaveEvent Event fired when dragging leaves QGraphicsView
     */
    void dragLeaveEvent(QDragLeaveEvent *event);

  protected:

  signals:
   void loadImageFileRequested(const QString &filename);

};

#endif /* QFSDROPPABLEGRAPHICSVIEW_H_ */
