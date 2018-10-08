#include "LineItem2D.h"

const int LineItem2D::selectionpixelsize_ = 10;

LineItem2D::LineItem2D(AxisRect2D *axisrect, Plot2D *plot)
    : QCPItemLine(plot),
      axisrect_(axisrect),
      draggingendlineitem_(false),
      draggingstartlineitem_(false),
      lineitemclicked_(false) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setClipAxisRect(axisrect_);
  setClipToAxisRect(true);
}

LineItem2D::~LineItem2D() {}

QColor LineItem2D::getstrokecolor_lineitem() const { return pen().color(); }

double LineItem2D::getstrokethickness_lineitem() const {
  return pen().widthF();
}

Qt::PenStyle LineItem2D::getstrokestyle_lineitem() const {
  return pen().style();
}

void LineItem2D::setstrokecolor_lineitem(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void LineItem2D::setstrokethickness_lineitem(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void LineItem2D::setstrokestyle_lineitem(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void LineItem2D::draw(QCPPainter *painter) {
  QCPItemLine::draw(painter);
  if (lineitemclicked_) {
    painter->setBrush(QBrush(QColor(0, 0, 0, 50)));
    painter->setPen(QPen(Qt::NoPen));
    painter->setAntialiasing(false);
    QRectF rect;
    rect.setCoords(this->end->pixelPosition().x() - selectionpixelsize_,
                   this->end->pixelPosition().y() - selectionpixelsize_,
                   this->end->pixelPosition().x() + selectionpixelsize_,
                   this->end->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
    rect.setCoords(this->start->pixelPosition().x() - selectionpixelsize_,
                   this->start->pixelPosition().y() - selectionpixelsize_,
                   this->start->pixelPosition().x() + selectionpixelsize_,
                   this->start->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
  }
}

void LineItem2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  Q_UNUSED(details);
  if (event->button() == Qt::LeftButton) {
    if (selectTest(event->pos(), false) > 0) {
      lineitemclicked_ = true;
      if (end->pixelPosition().x() > (event->pos().x() - selectionpixelsize_) &&
          end->pixelPosition().x() < (event->pos().x() + selectionpixelsize_) &&
          end->pixelPosition().y() > (event->pos().y() - selectionpixelsize_) &&
          end->pixelPosition().y() < (event->pos().y() + selectionpixelsize_))
        draggingendlineitem_ = true;
      if (start->pixelPosition().x() >
              (event->pos().x() - selectionpixelsize_) &&
          start->pixelPosition().x() <
              (event->pos().x() + selectionpixelsize_) &&
          start->pixelPosition().y() >
              (event->pos().y() - selectionpixelsize_) &&
          start->pixelPosition().y() < (event->pos().y() + selectionpixelsize_))
        draggingstartlineitem_ = true;
    }
    this->layer()->replot();
  }
}

void LineItem2D::mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) {
  Q_UNUSED(startPos);
  if (draggingendlineitem_) {
    // set bounding rect for drag event
    QPoint eventpos = event->pos();
    if (event->pos().x() < axisrect_->left()) {
      eventpos.setX(axisrect_->left());
    }
    if (event->pos().x() > axisrect_->right()) {
      eventpos.setX(axisrect_->right());
    }
    if (event->pos().y() < axisrect_->top()) {
      eventpos.setY(axisrect_->top());
    }
    if (event->pos().y() > axisrect_->bottom()) {
      eventpos.setY(axisrect_->bottom());
    }
    end->setPixelPosition(eventpos);
    this->layer()->replot();
  }
  if (draggingstartlineitem_) {
    QPoint eventpos = event->pos();
    if (event->pos().x() < axisrect_->left()) {
      eventpos.setX(axisrect_->left());
    }
    if (event->pos().x() > axisrect_->right()) {
      eventpos.setX(axisrect_->right());
    }
    if (event->pos().y() < axisrect_->top()) {
      eventpos.setY(axisrect_->top());
    }
    if (event->pos().y() > axisrect_->bottom()) {
      eventpos.setY(axisrect_->bottom());
    }
    start->setPixelPosition(eventpos);
    this->layer()->replot();
  }
}

void LineItem2D::mouseReleaseEvent(QMouseEvent *event,
                                   const QPointF &startPos) {
  Q_UNUSED(startPos);
  if (event->button() == Qt::LeftButton) {
    lineitemclicked_ = false;
    if (draggingendlineitem_) {
      draggingendlineitem_ = false;
    }
    if (draggingstartlineitem_) {
      draggingstartlineitem_ = false;
    }
  }
}
