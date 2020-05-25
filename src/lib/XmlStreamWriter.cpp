#include "XmlStreamWriter.h"

#include <QColor>
#include <QFont>
#include <QPen>

XmlStreamWriter::XmlStreamWriter() : QXmlStreamWriter() {}

XmlStreamWriter::XmlStreamWriter(QIODevice *device)
    : QXmlStreamWriter(device) {}

void XmlStreamWriter::writeFont(const QFont &font, const QColor &color) {
  writeStartElement("font");
  writeAttribute("family", font.family());
  writeAttribute("size", QString::number(font.pointSize()));
  writeAttribute("weight", QString::number(font.weight()));
  (font.bold()) ? writeAttribute("bold", "true")
                : writeAttribute("bold", "false");
  (font.italic()) ? writeAttribute("italic", "true")
                  : writeAttribute("italic", "false");
  (font.underline()) ? writeAttribute("underline", "true")
                     : writeAttribute("underline", "false");
  (font.overline()) ? writeAttribute("overline", "true")
                    : writeAttribute("overline", "false");
  writeAttribute("color", color.name(QColor::HexArgb));
  writeEndElement();
}

void XmlStreamWriter::writePen(const QPen &pen) {
  writeStartElement("pen");
  writeAttribute("color", pen.color().name(QColor::HexArgb));
  writeAttribute("style", QString::number(pen.style()));
  writeAttribute("width", QString::number(pen.widthF()));
  writeAttribute("cap", QString::number(pen.capStyle()));
  writeAttribute("join", QString::number(pen.joinStyle()));
  writeEndElement();
}

void XmlStreamWriter::writeBrush(const QBrush &brush) {
  writeStartElement("brush");
  writeAttribute("color", brush.color().name(QColor::HexArgb));
  writeAttribute("style", QString::number(brush.style()));
  writeEndElement();
}
