#include "ImageItem2D.h"

const int ImageItem2D::selectionpixelsize_ = 10;

ImageItem2D::ImageItem2D(AxisRect2D *axisrect, Plot2D *plot,
                         const QString &filename)
    : QCPItemPixmap(plot),
      axisrect_(axisrect),
      draggingimageitem_(false),
      imagefilename_(filename),
      pixmap_(new QPixmap()) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setClipAxisRect(axisrect_);
  setClipToAxisRect(true);
  setpixmap_imageitem();
}

ImageItem2D::~ImageItem2D() { delete pixmap_; }

AxisRect2D *ImageItem2D::getaxisrect_imageitem() const { return axisrect_; }

QString ImageItem2D::getsource_imageitem() const { return imagefilename_; }

QColor ImageItem2D::getstrokecolor_imageitem() const { return pen().color(); }

double ImageItem2D::getstrokethickness_imageitem() const {
  return pen().widthF();
}

Qt::PenStyle ImageItem2D::getstrokestyle_imageitem() const {
  return pen().style();
}

void ImageItem2D::setstrokecolor_imageitem(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void ImageItem2D::setstrokethickness_imageitem(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void ImageItem2D::setstrokestyle_imageitem(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void ImageItem2D::setpixmap_imageitem() {
  pixmap_->load(imagefilename_);
  setPixmap(*pixmap_);
}

void ImageItem2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  Q_UNUSED(details);
  if (event->button() == Qt::LeftButton) {
    if (selectTest(event->pos(), false) > 0) {
      draggingimageitem_ = true;
      dragimageitemorigin_ =
          event->localPos() - position("topLeft")->pixelPosition();
      parentPlot()->setCursor(Qt::CursorShape::ClosedHandCursor);
    }
  }
}

void ImageItem2D::mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) {
  Q_UNUSED(startPos);
  if (draggingimageitem_) {
    // set bounding rect for drag event
    QPointF eventpos = event->pos();
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
    position("topLeft")->setPixelPosition(eventpos - dragimageitemorigin_);
    layer()->replot();
  }
}

void ImageItem2D::mouseReleaseEvent(QMouseEvent *event,
                                    const QPointF &startPos) {
  Q_UNUSED(startPos);
  if (event->button() == Qt::LeftButton) {
    if (draggingimageitem_) {
      draggingimageitem_ = false;
      parentPlot()->unsetCursor();
    }
  }
}
