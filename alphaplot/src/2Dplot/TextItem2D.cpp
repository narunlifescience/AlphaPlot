#include "TextItem2D.h"

#include <QPen>

#include "core/Utilities.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

TextItem2D::TextItem2D(AxisRect2D *axisrect, Plot2D *plot)
    : QCPItemText(plot),
      axisrect_(axisrect),
      layername_(
          QDateTime::currentDateTime().toString("yyyy:MM:dd:hh:mm:ss:zzz")),
      draggingtextitem_(false),
      cursorshape_(axisrect->getParentPlot2D()->cursor().shape()) {
  if (axisrect_->getAxes2D().count() > 0) {
    QThread::msleep(1);
    parentPlot()->addLayer(layername_, axisrect_->getAxes2D().at(0)->layer(),
                           QCustomPlot::limBelow);
    setLayer(layername_);
  }
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setClipToAxisRect(true);
  setClipAxisRect(axisrect_);
  setAntialiased(false);
  setstrokestyle_textitem(Qt::SolidLine);
  QBrush b = brush();
  b.setStyle(Qt::SolidPattern);
  b.setColor(Qt::white);
  setBrush(b);
  settextalignment_textitem(TextAlignment::CenterCenter);
}

TextItem2D::~TextItem2D() { parentPlot()->removeLayer(layer()); }

AxisRect2D *TextItem2D::getaxisrect() const { return axisrect_; }

QColor TextItem2D::getstrokecolor_textitem() const { return pen().color(); }

double TextItem2D::getstrokethickness_textitem() const {
  return pen().widthF();
}

Qt::PenStyle TextItem2D::getstrokestyle_textitem() const {
  return pen().style();
}

TextItem2D::TextAlignment TextItem2D::gettextalignment_textitem() const {
  return textalignment_;
}

void TextItem2D::setstrokecolor_textitem(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void TextItem2D::setstrokethickness_textitem(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void TextItem2D::setstrokestyle_textitem(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void TextItem2D::settextalignment_textitem(const TextAlignment &value) {
  textalignment_ = value;
  switch (value) {
    case TextItem2D::TextAlignment::TopLeft:
      setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
      break;
    case TextItem2D::TextAlignment::TopCenter:
      setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
      break;
    case TextItem2D::TextAlignment::TopRight:
      setTextAlignment(Qt::AlignTop | Qt::AlignRight);
      break;
    case TextItem2D::TextAlignment::CenterLeft:
      setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
      break;
    case TextItem2D::TextAlignment::CenterCenter:
      setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
      break;
    case TextItem2D::TextAlignment::CenterRight:
      setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
      break;
    case TextItem2D::TextAlignment::BottomLeft:
      setTextAlignment(Qt::AlignBottom | Qt::AlignLeft);
      break;
    case TextItem2D::TextAlignment::BottomCenter:
      setTextAlignment(Qt::AlignBottom | Qt::AlignHCenter);
      break;
    case TextItem2D::TextAlignment::BottomRight:
      setTextAlignment(Qt::AlignBottom | Qt::AlignRight);
      break;
  }
}

void TextItem2D::setpixelposition_textitem(const QPointF &point) {
  position->setPixelPosition(point);
}

void TextItem2D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("textitem");
  xmlwriter->writeAttribute("xposition",
                            QString::number(position->pixelPosition().x()));
  xmlwriter->writeAttribute("yposition",
                            QString::number(position->pixelPosition().y()));
  switch (gettextalignment_textitem()) {
    case TextAlignment::TopLeft:
      xmlwriter->writeAttribute("textalignment", "topleft");
      break;
    case TextAlignment::TopCenter:
      xmlwriter->writeAttribute("textalignment", "topcenter");
      break;
    case TextAlignment::TopRight:
      xmlwriter->writeAttribute("textalignment", "topright");
      break;
    case TextAlignment::CenterLeft:
      xmlwriter->writeAttribute("textalignment", "centerleft");
      break;
    case TextAlignment::CenterCenter:
      xmlwriter->writeAttribute("textalignment", "centercenter");
      break;
    case TextAlignment::CenterRight:
      xmlwriter->writeAttribute("textalignment", "centerright");
      break;
    case TextAlignment::BottomLeft:
      xmlwriter->writeAttribute("textalignment", "bottomleft");
      break;
    case TextAlignment::BottomCenter:
      xmlwriter->writeAttribute("textalignment", "bottomcenter");
      break;
    case TextAlignment::BottomRight:
      xmlwriter->writeAttribute("textalignment", "bottomright");
      break;
  }
  xmlwriter->writeAttribute("text", text());
  (antialiased()) ? xmlwriter->writeAttribute("antialiased", "true")
                  : xmlwriter->writeAttribute("antialiased", "false");
  xmlwriter->writeStartElement("margin");
  xmlwriter->writeAttribute("left", QString::number(padding().left()));
  xmlwriter->writeAttribute("top", QString::number(padding().top()));
  xmlwriter->writeAttribute("right", QString::number(padding().right()));
  xmlwriter->writeAttribute("bottom", QString::number(padding().bottom()));
  xmlwriter->writeEndElement();
  xmlwriter->writeFont(font(), color());
  xmlwriter->writePen(pen());
  xmlwriter->writeBrush(brush());
  xmlwriter->writeEndElement();
}

bool TextItem2D::load(XmlStreamReader *xmlreader) {
  if (xmlreader->isStartElement() && xmlreader->name() == "textitem") {
    bool ok;
    QPoint itemposition;
    // item X position property
    double positionx = xmlreader->readAttributeDouble("xposition", &ok);
    if (ok) {
      itemposition.setX(static_cast<int>(positionx));
    } else {
      itemposition.setX(axisrect_->center().x());
      xmlreader->raiseWarning(
          tr("TextItem2D x position property setting error"));
    }
    // item Y position property
    double positiony = xmlreader->readAttributeDouble("yposition", &ok);
    if (ok) {
      itemposition.setY(static_cast<int>(positiony));
    } else {
      itemposition.setY(axisrect_->center().y());
      xmlreader->raiseWarning(
          tr("TextItem2D y position property setting error"));
    }

    // item text property
    QString itemtext = xmlreader->readAttributeString("text", &ok);
    if (ok)
      setText(itemtext);
    else {
      setText("Text");
      xmlreader->raiseWarning(
          tr("TextItem2D y position property setting error"));
    }
    setpixelposition_textitem(itemposition);
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "textitem") break;
    }
  } else  // no element
    xmlreader->raiseError(tr("no text item element found"));

  return !xmlreader->hasError();
}

void TextItem2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  Q_UNUSED(details);
  if (event->button() == Qt::LeftButton) {
    if (selectTest(event->pos(), false) > 0) {
      draggingtextitem_ = true;
      dragtextitemorigin_ = event->localPos() - position->pixelPosition();
      cursorshape_ = axisrect_->getParentPlot2D()->cursor().shape();
      axisrect_->getParentPlot2D()->setCursor(
          Qt::CursorShape::ClosedHandCursor);
    }
  }
}

void TextItem2D::mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) {
  Q_UNUSED(startPos);
  if (draggingtextitem_) {
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
    setpixelposition_textitem(eventpos - dragtextitemorigin_);
    this->layer()->replot();
  }
}

void TextItem2D::mouseReleaseEvent(QMouseEvent *event,
                                   const QPointF &startPos) {
  Q_UNUSED(startPos);
  if (event->button() == Qt::LeftButton) {
    if (draggingtextitem_) {
      draggingtextitem_ = false;
      axisrect_->getParentPlot2D()->setCursor(cursorshape_);
    }
  }
}
