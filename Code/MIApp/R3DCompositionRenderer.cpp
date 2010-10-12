/* --------------------------------------------------------------------------*
* This source code has been cleared for public release by the                *
* US Air Force 88th Air Base Wing Public Affairs Office under                *
* case number 88ABW-2010-4857 on Sept. 7, 2010.                              *
* -------------------------------------------------------------------------- */
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           F33615-03-C-5220
//
///////////////////////////////////////////////////////////////////////////////
#include "R3DCompositionRenderer.h"

#include <QMouseEvent>
#include <qmath.h>

#include <MXA/Common/LogTime.h>

#include <iostream>

const int alpha = 155;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
R3DCompositionRenderer::R3DCompositionRenderer(QWidget *parent) :
  QWidget(parent), zoomFactor(1.0),
  fixedGridX(-1),
  fixedGridY(-1),
  movingGridX(-1),
  movingGridY(-1),
  _enableFixedGrid(false),
  _enableMovingGrid(false),
  imageModified(true),
  hoverPts(NULL),
  m_rotation(0.0) 
{
  m_current_object = Image;
  m_composition_mode = QPainter::CompositionMode_Exclusion;
  m_movingImagePos = QPoint(0, 0);
  ctrlPoints << QPointF(0, 0);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::setFixedImage(QPixmap fixedImage)
{
  //std::cout << logTime() << "setFixedImage()" << std::endl;
  _fixedImage = fixedImage;
  QSize size(_fixedImage.size().width() * zoomFactor, _fixedImage.size().height() * zoomFactor);
  m_image = _fixedImage.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  imageModified = true;
  setMinimumSize(m_image.size());
  setBaseSize(m_image.size());
  QRectF r = geometry();
  setGeometry(r.topLeft().x(), r.topLeft().y(), m_image.size().width(), m_image.size().height());
  resize(m_image.size());
  updateGeometry();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::setMovingImage(QPixmap movingImage)
{
  //std::cout << logTime() << "setMovingImage()" << std::endl;
  _movingImage = movingImage;
  QSize size(_movingImage.size().width() * zoomFactor, _movingImage.size().height() * zoomFactor);
  m_top_image = _movingImage.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  imageModified = true;

  initHoverPoints();
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::initHoverPoints()
{
  if (hoverPts != NULL)
  {
    disconnect(hoverPts, SIGNAL(pointsChanged(const QPolygonF&)),
          this, SLOT(updateCtrlPoints(const QPolygonF &)));
    hoverPts->setParent(NULL);
    delete hoverPts;
    hoverPts = NULL;
  }
  hoverPts = new HoverPoints(this, HoverPoints::CircleShape);
  hoverPts->setConnectionType(HoverPoints::NoConnection);
  hoverPts->setEditable(false);
  hoverPts->setPointSize(QSize(15, 15));
  hoverPts->setShapeBrush(QBrush(QColor(151, 0, 0, alpha)));
  hoverPts->setShapePen(QPen(QColor(255, 100, 50, alpha)));
  hoverPts->setConnectionPen(QPen(QColor(151, 0, 0, 50)));
  //hoverPts->setBoundingRect(m_top_image.rect());

  hoverPts->setPoints(ctrlPoints);
  connect(hoverPts, SIGNAL(pointsChanged(const QPolygonF&)),
          this, SLOT(updateCtrlPoints(const QPolygonF &)));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HoverPoints* R3DCompositionRenderer::getHoverPoints()
{
  return hoverPts;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::setMovingImageOffset(float x, float y)
{
  QPointF pos(x, y);
  m_movingImagePos = pos;
  imageModified = true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::setZoomFactor(double zoom)
{
  //std::cout << logTime() << "setZoomFactor()" << std::endl;
  this->zoomFactor = zoom;
  setFixedImage(_fixedImage);
  setMovingImage(_movingImage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::updateMovingImagePos()
{
    if (m_current_object != NoObject)
        return;
    setMovingImagePos(QLineF(m_movingImagePos, QPointF(0.0f, 0.0f)).pointAt(0.00));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::drawFixedImageGrid(QPainter &p)
{
  if (fixedGridX < 0 || fixedGridY < 0) { return; }
  p.setPen(Qt::blue);
  p.setRenderHint(QPainter::Antialiasing, false);

  // Draw the Vertical lines
  int increment = width() / fixedGridX;
  for (int i = 0; i < width(); i=i+increment)
  {
    QPoint p1 ( i, 0);
    QPoint p2 ( i, m_image.size().height());
    p.drawLine(p1, p2);
  }

  increment = height() / fixedGridY;
  for (int i = 0; i < height(); i=i+increment)
  {
    QPoint p1 ( 0, i);
    QPoint p2 ( m_image.size().width(), i);
    p.drawLine(p1, p2);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::drawMovingImageGrid(QPainter &p)
{
  if (movingGridX < 0 || movingGridY < 0) { return; }
  p.setPen(Qt::red);
  p.setRenderHint(QPainter::Antialiasing, false);
  // Draw the Vertical lines
  int increment = m_top_image.size().width() / fixedGridX;
  int xStart = (int)(m_movingImagePos.x());
  for (int i = xStart; i < m_top_image.size().width() + xStart; i=i+increment)
  {
    QPoint p1 ( i, m_movingImagePos.y());
    QPoint p2 ( i, m_top_image.size().height());
    p.drawLine(p1, p2);
  }

  increment = m_top_image.size().height() / fixedGridY;
  xStart = (int)(m_movingImagePos.y());
  for (int i = xStart; i < m_top_image.size().height() + xStart; i=i+increment)
  {
    QPoint p1 ( m_movingImagePos.x(), i);
    QPoint p2 ( m_top_image.size().width(), i);
    p.drawLine(p1, p2);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int R3DCompositionRenderer::saveToFile(QString &filename)
{
  int err = 0;
  QPainter painter;
  QImage image(this->size(), QImage::Format_ARGB32_Premultiplied);

  painter.begin(&image);
  paint(&painter);
  painter.end();
  hoverPts->paintPoints(image);
  err = (int) (image.save(filename));
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::paint(QPainter *painter)
{
  //std::cout << logTime() << "  R3DCompositionRenderer::paint()" << std::endl;
  // using a QPixmap
  if (imageModified)
  {
    // Draw the fixed Image first
    painter->setPen(Qt::NoPen);
    painter->drawPixmap(rect(), m_image);
    imageModified = false;
  }
  else
  {
    // Draw the fixed Image first
    painter->setPen(Qt::NoPen);
    painter->drawPixmap(rect(), m_image);
  }
    // Draw the moving image next
    painter->setCompositionMode(m_composition_mode);
    QPointF center(m_top_image.width() / qreal(2), m_top_image.height() / qreal(2));
    painter->translate(ctrlPoints.at(0) );
    painter->rotate(m_rotation);
    painter->translate(-ctrlPoints.at(0) );
    painter->drawPixmap(m_movingImagePos, m_top_image);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::paintEvent(QPaintEvent *e)
{
  //std::cout << logTime() << " R3DCompositionRenderer::paintEvent" << std::endl;
  QPainter painter;
  painter.begin(this);
  paint(&painter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::resizeEvent(QResizeEvent *e)
{
  //std::cout << logTime() << "R3DCompositionRenderer::resizeEvent" << std::endl;
  QWidget::resizeEvent(e);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::mousePressEvent(QMouseEvent *e)
{
  //std::cout << "mousePressedEvent: pos=" << e->pos().x() << "," << e->pos().y() << std::endl;
    QRectF movImageRect = QRectF(m_movingImagePos, m_top_image.size());
    if (movImageRect.contains(e->pos())) {
      //  std::cout << "  circle contains... " << std::endl;
        movImageRect.translate(-m_top_image.size().width()/2, -m_top_image.size().height()/2);
        m_current_object = Image;
        m_offset = movImageRect.center() - e->pos();
    } else {
        m_current_object = NoObject;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::mouseMoveEvent(QMouseEvent *e)
{
 // std::cout << "mouseMoveEvent" << std::endl;
    if (m_current_object == Image) setMovingImagePos(e->pos() + m_offset);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::mouseReleaseEvent(QMouseEvent *)
{
 // std::cout << "mouseReleaseEvent" << std::endl;
    m_current_object = NoObject;
    updateMovingImagePos();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::setMovingImagePos(const QPointF &pos)
{
    const QRect oldRect = QRectF(m_movingImagePos, m_top_image.size()).toAlignedRect();
    m_movingImagePos = pos;

    const QRect newRect = QRectF(m_movingImagePos, m_top_image.size()).toAlignedRect();
    update();

    emit updateOffsets(m_movingImagePos.x()/zoomFactor, m_movingImagePos.y()/zoomFactor);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::setFixedImageGrid(int x, int y)
{
  fixedGridX = x;
  fixedGridY = y;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::setMovingImageGrid(int x, int y)
{
  movingGridX = x;
  movingGridY = y;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::updateCtrlPoints(const QPolygonF &points)
{
    ctrlPoints = points;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::changeRotation(double r)
{
 // std::cout << logTime() << "changeRotation(" << r << ")" << std::endl;
  setRotation(qreal(r));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DCompositionRenderer::setRotation(qreal r)
{
  //std::cout << logTime() << "setRotation(" << r << ")" << std::endl;
  qreal old_rot = m_rotation;
  m_rotation = r;

  QPointF center(hoverPts->points().at(0));
  QMatrix m;
  m.translate(center.x(), center.y());
  m.rotate(m_rotation - old_rot);
  m.translate(-center.x(), -center.y());
  hoverPts->setPoints(hoverPts->points() * m);

  update();
}
