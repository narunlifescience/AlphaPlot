#include "ImageItem2D.h"

#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Plot2D.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

const int ImageItem2D::selectionpixelsize_ = 10;

ImageItem2D::ImageItem2D(AxisRect2D *axisrect, Plot2D *plot,
                         const QString &filename)
    : QCPItemPixmap(plot),
      axisrect_(axisrect),
      layername_(
          QString("<Image2DItem>") +
          QDateTime::currentDateTime().toString("yyyy:MM:dd:hh:mm:ss:zzz")),
      imageitemclicked_(false),
      dragginganchorimageitem_(nullptr),
      draggingimageitem_(false),
      imagefilename_(filename),
      pixmap_(new QPixmap()),
      rotation_(0),
      cursorshape_(axisrect->getParentPlot2D()->cursor()) {
  // setting Layer
  if (axisrect_->getAxes2D().count() > 0) {
    QThread::msleep(1);
    parentPlot()->addLayer(layername_, axisrect_->getAxes2D().at(0)->layer(),
                           QCustomPlot::limBelow);
    setLayer(layername_);
  }
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setClipAxisRect(axisrect_);
  setClipToAxisRect(true);

  topLeft->setTypeX(QCPItemPosition::ptPlotCoords);
  topLeft->setTypeY(QCPItemPosition::ptPlotCoords);
  bottomRight->setTypeX(QCPItemPosition::ptPlotCoords);
  bottomRight->setTypeY(QCPItemPosition::ptPlotCoords);
  topLeft->setParentAnchor(nullptr);
  bottomRight->setParentAnchor(nullptr);
  setpixmap_imageitem();
}

ImageItem2D::~ImageItem2D() {
  parentPlot()->removeLayer(layer());
  delete pixmap_;
}

AxisRect2D *ImageItem2D::getaxisrect() const { return axisrect_; }

QString ImageItem2D::getsource_imageitem() const { return imagefilename_; }

int ImageItem2D::getrotation_imageitem() const { return rotation_; }

QColor ImageItem2D::getstrokecolor_imageitem() const { return pen().color(); }

double ImageItem2D::getstrokethickness_imageitem() const {
  return pen().widthF();
}

Qt::PenStyle ImageItem2D::getstrokestyle_imageitem() const {
  return pen().style();
}

QPointF ImageItem2D::getposition_imageitem() const {
  return position("topLeft")->pixelPosition();
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

void ImageItem2D::setposition_imageitem(const QPointF origin) {
  QPointF posbottomRight =
      bottomRight->pixelPosition() + (origin - topLeft->pixelPosition());
  bottomRight->setPixelPosition(posbottomRight);
  topLeft->setPixelPosition(origin);
}

void ImageItem2D::setpixmap_imageitem() {
  pixmap_->load(imagefilename_);
  setPixmap(*pixmap_);
}

void ImageItem2D::setrotation_imageitem(int degree) {
  rotation_ = degree;
  QTransform trans;
  trans.rotate(degree);
  int pxw = pixmap_->width();
  int pxh = pixmap_->height();
  QPixmap pix;
  pix =
      pixmap_->transformed(trans, Qt::TransformationMode::SmoothTransformation);
  pix = pix.copy((pix.width() - pxw) / 2, (pix.height() - pxh) / 2, pxw, pxh);
  setPixmap(pix);
}

void ImageItem2D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("imageitem");
  xmlwriter->writeAttribute("file", imagefilename_);
  xmlwriter->writeAttribute("x",
                            QString::number(position("topLeft")->coords().x()));
  xmlwriter->writeAttribute("y",
                            QString::number(position("topLeft")->coords().y()));
  xmlwriter->writeAttribute(
      "bottomrightx", QString::number(position("bottomRight")->coords().x()));
  xmlwriter->writeAttribute(
      "bottomrighty", QString::number(position("bottomRight")->coords().y()));
  xmlwriter->writeAttribute("rotation", QString::number(rotation_));
  xmlwriter->writePen(pen());
  xmlwriter->writeEndElement();
}

bool ImageItem2D::load(XmlStreamReader *xmlreader) {
  if (xmlreader->isStartElement() && xmlreader->name() == "imageitem") {
    bool ok;

    double orix = xmlreader->readAttributeDouble("x", &ok);
    if (ok) {
      double oriy = xmlreader->readAttributeDouble("y", &ok);
      if (ok) {
        position("topLeft")->setCoords(QPointF(orix, oriy));
      } else
        xmlreader->raiseWarning(
            tr("ImageItem2D set position property y setting error"));
    } else
      xmlreader->raiseWarning(
          tr("ImageItem2D set position property x setting error"));

    double botx = xmlreader->readAttributeDouble("bottomrightx", &ok);
    if (ok) {
      double boty = xmlreader->readAttributeDouble("bottomrighty", &ok);
      if (ok) {
        position("bottomRight")->setCoords(QPointF(botx, boty));
      } else
        xmlreader->raiseWarning(
            tr("ImageItem2D set position property bottomrighty setting error"));
    } else
      xmlreader->raiseWarning(
          tr("ImageItem2D set position property bottomrightx setting error"));

    // rotation
    int rotation = xmlreader->readAttributeInt("rotation", &ok);
    (ok)
        ? setrotation_imageitem(rotation)
        : xmlreader->raiseWarning(tr("ImageItem2D set rotation setting error"));

    // strokepen property
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
      // pen
      if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
        QPen strokep = xmlreader->readPen(&ok);
        if (ok)
          setPen(strokep);
        else
          xmlreader->raiseWarning(
              tr("ImageItem2D strokepen property setting error"));
      }
    }

  } else  // no element
    xmlreader->raiseError(tr("no ImageItem2D item element found"));

  return !xmlreader->hasError();
}

void ImageItem2D::draw(QCPPainter *painter) {
  QCPItemPixmap::draw(painter);
  if (imageitemclicked_) {
    painter->setBrush(QBrush(QColor(0, 0, 0, 50)));
    painter->setPen(QPen(Qt::NoPen));
    painter->setAntialiasing(false);
    QRectF rect;
    rect.setCoords(this->topLeft->pixelPosition().x() - selectionpixelsize_,
                   this->topLeft->pixelPosition().y() - selectionpixelsize_,
                   this->topLeft->pixelPosition().x() + selectionpixelsize_,
                   this->topLeft->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
    rect.adjust(+2, +2, -3, -3);
    painter->setBrush(QBrush(Qt::NoBrush));
    painter->setPen(QColor(Qt::red));
    painter->setAntialiasing(true);
    painter->drawEllipse(rect);
    painter->setBrush(QBrush(QColor(0, 0, 0, 50)));
    painter->setPen(QPen(Qt::NoPen));
    painter->setAntialiasing(false);
    rect.setCoords(this->top->pixelPosition().x() - selectionpixelsize_,
                   this->top->pixelPosition().y() - selectionpixelsize_,
                   this->top->pixelPosition().x() + selectionpixelsize_,
                   this->top->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
    rect.setCoords(this->topRight->pixelPosition().x() - selectionpixelsize_,
                   this->topRight->pixelPosition().y() - selectionpixelsize_,
                   this->topRight->pixelPosition().x() + selectionpixelsize_,
                   this->topRight->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
    rect.setCoords(this->left->pixelPosition().x() - selectionpixelsize_,
                   this->left->pixelPosition().y() - selectionpixelsize_,
                   this->left->pixelPosition().x() + selectionpixelsize_,
                   this->left->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
    rect.setCoords(this->right->pixelPosition().x() - selectionpixelsize_,
                   this->right->pixelPosition().y() - selectionpixelsize_,
                   this->right->pixelPosition().x() + selectionpixelsize_,
                   this->right->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
    rect.setCoords(this->bottomLeft->pixelPosition().x() - selectionpixelsize_,
                   this->bottomLeft->pixelPosition().y() - selectionpixelsize_,
                   this->bottomLeft->pixelPosition().x() + selectionpixelsize_,
                   this->bottomLeft->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
    rect.setCoords(this->bottom->pixelPosition().x() - selectionpixelsize_,
                   this->bottom->pixelPosition().y() - selectionpixelsize_,
                   this->bottom->pixelPosition().x() + selectionpixelsize_,
                   this->bottom->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
    rect.setCoords(bottomRight->pixelPosition().x() - selectionpixelsize_,
                   bottomRight->pixelPosition().y() - selectionpixelsize_,
                   bottomRight->pixelPosition().x() + selectionpixelsize_,
                   bottomRight->pixelPosition().y() + selectionpixelsize_);
    painter->drawRect(rect);
    rect.adjust(+2, +2, -3, -3);
    painter->setBrush(QBrush(Qt::NoBrush));
    painter->setPen(QColor(Qt::red));
    painter->setAntialiasing(true);
    painter->drawLine(rect.topLeft(), rect.bottomRight());
    painter->drawLine(rect.topRight(), rect.bottomLeft());
  }
}

void ImageItem2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  Q_UNUSED(details);
  if (event->button() == Qt::LeftButton) {
    if (selectTest(event->pos(), false) > 0) {
      imageitemclicked_ = true;
      if (topLeft->pixelPosition().x() >
              (event->pos().x() - selectionpixelsize_) &&
          topLeft->pixelPosition().x() <
              (event->pos().x() + selectionpixelsize_) &&
          topLeft->pixelPosition().y() >
              (event->pos().y() - selectionpixelsize_) &&
          topLeft->pixelPosition().y() <
              (event->pos().y() + selectionpixelsize_)) {
        dragginganchorimageitem_ = topLeft;
        cursorshape_ = axisrect_->getParentPlot2D()->cursor();
        parentPlot()->setCursor(Qt::CursorShape::CrossCursor);
      } else if (top->pixelPosition().x() >
                     (event->pos().x() - selectionpixelsize_) &&
                 top->pixelPosition().x() <
                     (event->pos().x() + selectionpixelsize_) &&
                 top->pixelPosition().y() >
                     (event->pos().y() - selectionpixelsize_) &&
                 top->pixelPosition().y() <
                     (event->pos().y() + selectionpixelsize_)) {
        dragginganchorimageitem_ = top;
        cursorshape_ = axisrect_->getParentPlot2D()->cursor();
        parentPlot()->setCursor(Qt::CursorShape::SizeVerCursor);
      } else if (topRight->pixelPosition().x() >
                     (event->pos().x() - selectionpixelsize_) &&
                 topRight->pixelPosition().x() <
                     (event->pos().x() + selectionpixelsize_) &&
                 topRight->pixelPosition().y() >
                     (event->pos().y() - selectionpixelsize_) &&
                 topRight->pixelPosition().y() <
                     (event->pos().y() + selectionpixelsize_)) {
        dragginganchorimageitem_ = topRight;
        cursorshape_ = axisrect_->getParentPlot2D()->cursor();
        axisrect_->getParentPlot2D()->setCursor(Qt::CursorShape::CrossCursor);
      } else if (left->pixelPosition().x() >
                     (event->pos().x() - selectionpixelsize_) &&
                 left->pixelPosition().x() <
                     (event->pos().x() + selectionpixelsize_) &&
                 left->pixelPosition().y() >
                     (event->pos().y() - selectionpixelsize_) &&
                 left->pixelPosition().y() <
                     (event->pos().y() + selectionpixelsize_)) {
        dragginganchorimageitem_ = left;
        cursorshape_ = axisrect_->getParentPlot2D()->cursor();
        parentPlot()->setCursor(Qt::CursorShape::SizeHorCursor);
      } else if (right->pixelPosition().x() >
                     (event->pos().x() - selectionpixelsize_) &&
                 right->pixelPosition().x() <
                     (event->pos().x() + selectionpixelsize_) &&
                 right->pixelPosition().y() >
                     (event->pos().y() - selectionpixelsize_) &&
                 right->pixelPosition().y() <
                     (event->pos().y() + selectionpixelsize_)) {
        dragginganchorimageitem_ = right;
        cursorshape_ = axisrect_->getParentPlot2D()->cursor();
        parentPlot()->setCursor(Qt::CursorShape::SizeHorCursor);
      } else if (bottomLeft->pixelPosition().x() >
                     (event->pos().x() - selectionpixelsize_) &&
                 bottomLeft->pixelPosition().x() <
                     (event->pos().x() + selectionpixelsize_) &&
                 bottomLeft->pixelPosition().y() >
                     (event->pos().y() - selectionpixelsize_) &&
                 bottomLeft->pixelPosition().y() <
                     (event->pos().y() + selectionpixelsize_)) {
        dragginganchorimageitem_ = bottomLeft;
        cursorshape_ = axisrect_->getParentPlot2D()->cursor();
        axisrect_->getParentPlot2D()->setCursor(Qt::CursorShape::CrossCursor);
      } else if (bottom->pixelPosition().x() >
                     (event->pos().x() - selectionpixelsize_) &&
                 bottom->pixelPosition().x() <
                     (event->pos().x() + selectionpixelsize_) &&
                 bottom->pixelPosition().y() >
                     (event->pos().y() - selectionpixelsize_) &&
                 bottom->pixelPosition().y() <
                     (event->pos().y() + selectionpixelsize_)) {
        dragginganchorimageitem_ = bottom;
        cursorshape_ = axisrect_->getParentPlot2D()->cursor();
        parentPlot()->setCursor(Qt::CursorShape::SizeVerCursor);
      } else if (bottomRight->pixelPosition().x() >
                     (event->pos().x() - selectionpixelsize_) &&
                 bottomRight->pixelPosition().x() <
                     (event->pos().x() + selectionpixelsize_) &&
                 bottomRight->pixelPosition().y() >
                     (event->pos().y() - selectionpixelsize_) &&
                 bottomRight->pixelPosition().y() <
                     (event->pos().y() + selectionpixelsize_)) {
        dragginganchorimageitem_ = bottomRight;
        cursorshape_ = axisrect_->getParentPlot2D()->cursor();
        axisrect_->getParentPlot2D()->setCursor(Qt::CursorShape::CrossCursor);
      } else {
        draggingimageitem_ = true;
        dragimageitemorigintopleft_ =
            event->localPos() - topLeft->pixelPosition();
        dragimageitemoriginbottomright_ =
            event->localPos() - bottomRight->pixelPosition();
        cursorshape_ = axisrect_->getParentPlot2D()->cursor();
        axisrect_->getParentPlot2D()->setCursor(
            Qt::CursorShape::ClosedHandCursor);
      }
    }
    this->layer()->replot();
  }
}

void ImageItem2D::mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) {
  Q_UNUSED(startPos);
  if (dragginganchorimageitem_ == topLeft) {
    topLeft->setPixelPosition(event->pos());
    this->layer()->replot();
  } else if (dragginganchorimageitem_ == top) {
    topLeft->setPixelPosition(
        QPointF(topLeft->pixelPosition().x(), event->pos().y()));
    this->layer()->replot();
  } else if (dragginganchorimageitem_ == topRight) {
    topLeft->setPixelPosition(
        QPointF(topLeft->pixelPosition().x(), event->pos().y()));
    bottomRight->setPixelPosition(
        QPointF(event->pos().x(), bottomRight->pixelPosition().y()));
    this->layer()->replot();
  } else if (dragginganchorimageitem_ == left) {
    topLeft->setPixelPosition(
        QPointF(event->pos().x(), topLeft->pixelPosition().y()));
    this->layer()->replot();
  } else if (dragginganchorimageitem_ == right) {
    bottomRight->setPixelPosition(
        QPointF(event->pos().x(), bottomRight->pixelPosition().y()));
    this->layer()->replot();
  } else if (dragginganchorimageitem_ == bottomLeft) {
    topLeft->setPixelPosition(
        QPointF(event->pos().x(), topLeft->pixelPosition().y()));
    bottomRight->setPixelPosition(
        QPointF(bottomRight->pixelPosition().x(), event->pos().y()));
    this->layer()->replot();
  } else if (dragginganchorimageitem_ == bottom) {
    bottomRight->setPixelPosition(
        QPointF(bottomRight->pixelPosition().x(), event->pos().y()));
    this->layer()->replot();
  } else if (dragginganchorimageitem_ == bottomRight) {
    bottomRight->setPixelPosition(event->pos());
    this->layer()->replot();
  } else if (draggingimageitem_) {
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
    topLeft->setPixelPosition(eventpos - dragimageitemorigintopleft_);
    bottomRight->setPixelPosition(eventpos - dragimageitemoriginbottomright_);
    layer()->replot();
  }
}

void ImageItem2D::mouseReleaseEvent(QMouseEvent *event,
                                    const QPointF &startPos) {
  Q_UNUSED(startPos);
  if (event->button() == Qt::LeftButton) {
    imageitemclicked_ = false;
    dragginganchorimageitem_ = nullptr;
    draggingimageitem_ = false;
    axisrect_->getParentPlot2D()->setCursor(cursorshape_);
    emit axisrect_->ImageItem2DMoved();
  }
}
