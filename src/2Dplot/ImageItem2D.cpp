#include "ImageItem2D.h"

#include "lib/XmlStreamReader.h"
#include "lib/XmlStreamWriter.h"

const int ImageItem2D::selectionpixelsize_ = 10;

ImageItem2D::ImageItem2D(AxisRect2D *axisrect, Plot2D *plot,
                         const QString &filename)
    : QCPItemPixmap(plot),
      axisrect_(axisrect),
      layername_(
          QString("<Image2DItem>") +
          QDateTime::currentDateTime().toString("yyyy:MM:dd:hh:mm:ss:zzz")),
      draggingimageitem_(false),
      imagefilename_(filename),
      pixmap_(new QPixmap()),
      cursorshape_(axisrect->getParentPlot2D()->cursor().shape()) {
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
  setpixmap_imageitem();
}

ImageItem2D::~ImageItem2D() {
  parentPlot()->removeLayer(layer());
  delete pixmap_;
}

AxisRect2D *ImageItem2D::getaxisrect() const { return axisrect_; }

QString ImageItem2D::getsource_imageitem() const { return imagefilename_; }

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
  position("topLeft")->setPixelPosition(origin);
}

void ImageItem2D::setpixmap_imageitem() {
  pixmap_->load(imagefilename_);
  setPixmap(*pixmap_);
}

void ImageItem2D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("imageitem");
  xmlwriter->writeAttribute("file", imagefilename_);
  xmlwriter->writeAttribute("x",
                            QString::number(position("topLeft")->coords().x()));
  xmlwriter->writeAttribute("y",
                            QString::number(position("topLeft")->coords().y()));
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

void ImageItem2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  Q_UNUSED(details);
  if (event->button() == Qt::LeftButton) {
    if (selectTest(event->pos(), false) > 0) {
      draggingimageitem_ = true;
      dragimageitemorigin_ =
          event->localPos() - position("topLeft")->pixelPosition();
      cursorshape_ = axisrect_->getParentPlot2D()->cursor().shape();
      axisrect_->getParentPlot2D()->setCursor(
          Qt::CursorShape::ClosedHandCursor);
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
      axisrect_->getParentPlot2D()->setCursor(cursorshape_);
      emit axisrect_->ImageItem2DMoved();
    }
  }
}
