/***************************************************************************
    File                 : XmlStreamReader.cpp
    Project              : AlphaPlot
    Description          : XML stream parser that supports errors as well as
 warnings
    --------------------------------------------------------------------
    Copyright            : (C) 2008-2009 Tilman Benkert (thzs*gmx.net)
                           (replace * with @ in the email addresses)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "lib/XmlStreamReader.h"

#include <QBrush>
#include <QFont>
#include <QObject>
#include <QPen>
#include <QtDebug>

XmlStreamReader::XmlStreamReader() { init(); }

XmlStreamReader::XmlStreamReader(QIODevice* device) : QXmlStreamReader(device) {
  init();
}

XmlStreamReader::XmlStreamReader(const QByteArray& data)
    : QXmlStreamReader(data) {
  init();
}

XmlStreamReader::XmlStreamReader(const QString& data) : QXmlStreamReader(data) {
  init();
}

XmlStreamReader::XmlStreamReader(const char* data) : QXmlStreamReader(data) {
  init();
}

void XmlStreamReader::init() {
  d_error_prefix =
      QObject::tr("XML reader error: ", "prefix for XML error messages");
  d_error_postfix =
      QObject::tr(" (loading failed)", "postfix for XML error messages");
  d_warning_prefix =
      QObject::tr("XML reader warning: ", "prefix for XML warning messages");
  d_warning_postfix = QObject::tr("", "postfix for XML warning messages");
}

QStringList XmlStreamReader::warningStrings() const { return d_warnings; }

bool XmlStreamReader::hasWarnings() const { return !(d_warnings.isEmpty()); }

void XmlStreamReader::raiseError(const QString& message) {
  QString prefix2 = QString(QObject::tr("line %1, column %2: ")
                                .arg(lineNumber())
                                .arg(columnNumber()));
  QXmlStreamReader::raiseError(d_error_prefix + prefix2 + message +
                               d_error_postfix);
}

void XmlStreamReader::raiseWarning(const QString& message) {
  QString prefix2 = QString(QObject::tr("line %1, column %2: ")
                                .arg(lineNumber())
                                .arg(columnNumber()));
  d_warnings.append(d_warning_prefix + prefix2 + message + d_warning_postfix);
}

bool XmlStreamReader::skipToNextTag() {
  if (atEnd()) {
    raiseError(QObject::tr("unexpected end of document"));
    return false;
  }
  do {
    readNext();
  } while (!(isStartElement() || isEndElement() || atEnd()));
  if (atEnd()) {
    raiseError(QObject::tr("unexpected end of document"));
    return false;
  }
  return true;
}

int XmlStreamReader::readAttributeInt(const QString& name, bool* ok) {
  QString str = attributes().value(namespaceUri().toString(), name).toString();
  if (str.isEmpty()) {
    if (ok) *ok = false;
    return 0;
  }
  *ok = true;
  return str.toInt(ok);
}

double XmlStreamReader::readAttributeDouble(const QString& name, bool* ok) {
  QString str = attributes().value(namespaceUri().toString(), name).toString();
  if (str.isEmpty()) {
    if (ok) *ok = false;
    return 0;
  }
  return str.toDouble(ok);
}

bool XmlStreamReader::readAttributeBool(const QString& name, bool* ok) {
  QString str = attributes().value(namespaceUri().toString(), name).toString();
  if (str.isEmpty()) {
    if (ok) *ok = false;
    return false;
  }
  bool value = false;
  if (str == "true")
    value = true;
  else if (str == "false")
    value = false;
  else {
    if (ok) *ok = false;
    return false;
  }
  *ok = true;
  return value;
}

QString XmlStreamReader::readAttributeString(const QString& name, bool* ok) {
  QString str = attributes().value(namespaceUri().toString(), name).toString();
  if (str.isEmpty()) {
    if (ok) *ok = false;
    return QString();
  }
  *ok = true;
  return str.trimmed();
}

QXmlStreamAttributes XmlStreamReader::readElements(const QString& element,
                                                   bool* ok) {
  QXmlStreamAttributes attribs;
  if (isStartElement()) {
    if (name() == element) {
      attribs = attributes();
    } else {
      // unknown element
      raiseWarning(QObject::tr("unknown element '%1'").arg(name().toString()));
      if (!skipToEndElement())
        if (ok) *ok = false;
    }
  } else {
    if (ok) *ok = false;
  }
  return attribs;
}

QPair<QFont, QColor> XmlStreamReader::readFont(bool* ok) {
  QPair<QFont, QColor> pair;
  if (isStartElement() && name() == "font") {
    // family
    QString family = readAttributeString("family", ok);
    if (ok)
      pair.first.setFamily(family);
    else
      raiseWarning("QFont family property setting error");
    // size
    int size = readAttributeInt("size", ok);
    if (ok)
      pair.first.setPointSize(size);
    else
      raiseWarning("QFont size property setting error");
    // size
    int weight = readAttributeInt("weight", ok);
    if (ok)
      pair.first.setWeight(weight);
    else
      raiseWarning("QFont weight property setting error");
    // bold
    bool bold = readAttributeBool("bold", ok);
    if (ok)
      pair.first.setBold(bold);
    else
      raiseWarning("QFont bold property setting error");
    // italic
    bool italic = readAttributeBool("italic", ok);
    if (ok)
      pair.first.setItalic(italic);
    else
      raiseWarning("QFont italic property setting error");
    // underline
    bool underline = readAttributeBool("underline", ok);
    if (ok)
      pair.first.setUnderline(underline);
    else
      raiseWarning("QFont underline property setting error");
    // overline
    bool overline = readAttributeBool("overline", ok);
    if (ok)
      pair.first.setOverline(overline);
    else
      raiseWarning("QFont overline property setting error");
    // color
    QString color = readAttributeString("color", ok);
    if (ok)
      pair.second = QColor(color);
    else
      raiseWarning("QFont color property setting error");
  } else {
    *ok = false;
    raiseError(QObject::tr("no font element found"));
  }
  return pair;
}

QPen XmlStreamReader::readPen(bool* ok) {
  QPen pen;
  if (isStartElement() && name() == "pen") {
    // color
    QString color = readAttributeString("color", ok);
    if (ok)
      pen.setColor(color);
    else
      raiseWarning("QPen color property setting error");
    // style
    int style = readAttributeInt("style", ok);
    if (ok)
      pen.setStyle(static_cast<Qt::PenStyle>(style));
    else
      raiseWarning("QPen style property setting error");
    // width
    double width = readAttributeDouble("width", ok);
    if (ok)
      pen.setWidthF(width);
    else
      raiseWarning("QPen width property setting error");
    // Capstyle
    int cap = readAttributeInt("cap", ok);
    if (ok)
      pen.setCapStyle(static_cast<Qt::PenCapStyle>(cap));
    else
      raiseWarning("QPen cap property setting error");
    // Join Style
    int join = readAttributeInt("join", ok);
    if (ok)
      pen.setJoinStyle(static_cast<Qt::PenJoinStyle>(join));
    else
      raiseWarning("QPen join property setting error");
  } else {
    *ok = false;
    raiseError(QObject::tr("no pen element found"));
  }
  return pen;
}

QBrush XmlStreamReader::readBrush(bool* ok) {
  QBrush brush;
  if (isStartElement() && name() == "brush") {
    // color
    QString color = readAttributeString("color", ok);
    if (ok)
      brush.setColor(color);
    else
      raiseWarning("QBrush color property setting error");
    // style
    int style = readAttributeInt("style", ok);
    if (ok)
      brush.setStyle(static_cast<Qt::BrushStyle>(style));
    else
      raiseWarning("QBrush style property setting error");
  } else {
    *ok = false;
    raiseError(QObject::tr("no brush element found"));
  }
  return brush;
}

bool XmlStreamReader::skipToEndElement() {
  int depth = 1;
  if (atEnd()) {
    raiseError(QObject::tr("unexpected end of document"));
    return false;
  }
  do {
    readNext();
    if (isEndElement()) depth--;
    if (isStartElement()) depth++;
  } while (!((isEndElement() && depth == 0) || atEnd()));
  if (atEnd()) {
    raiseError(QObject::tr("unexpected end of document"));
    return false;
  }
  return true;
}
