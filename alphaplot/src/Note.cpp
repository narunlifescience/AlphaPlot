/***************************************************************************
    File                 : Note.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Benkert,
                                          Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Notes window class

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
#include "Note.h"

#include <QApplication>
#include <QDateTime>
#include <QLayout>
#include <QPaintDevice>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QVBoxLayout>
#include <QXmlStreamWriter>

#include "future/lib/XmlStreamReader.h"
#include "scripting/ScriptEdit.h"

Note::Note(ScriptingEnv* env, const QString& label, QWidget* parent,
           const char* name, Qt::WindowFlags f)
    : MyWidget(label, parent, name, f) {
  init(env);
}

Note::~Note() {}

void Note::init(ScriptingEnv* env) {
  autoExec = false;
  QDateTime dt = QDateTime::currentDateTime();
  setBirthDate(dt.toString(Qt::LocalDate));

  textedit_ = new ScriptEdit(env, this, name());
  textedit_->setContext(this);
  setWidget(textedit_);

  setGeometry(0, 0, 500, 200);
  connect(textedit_, SIGNAL(textChanged()), this, SLOT(modifiedNote()));
}

QString Note::getText() { return textedit_->toPlainText().trimmed(); }

void Note::modifiedNote() { emit modifiedWindow(this); }

void Note::save(QXmlStreamWriter* xmlwriter) {
  xmlwriter->writeStartElement("note");
  QDateTime datetime = QDateTime::fromString(birthDate(), Qt::LocalDate);
  xmlwriter->writeAttribute("creation_time",
                            datetime.toString("yyyy-dd-MM hh:mm:ss:zzz"));
  xmlwriter->writeAttribute("caption_spec", QString::number(captionPolicy()));
  xmlwriter->writeAttribute("name", name());
  xmlwriter->writeAttribute("label", windowLabel());
  xmlwriter->writeAttribute("status", QString::number(status()));
  xmlwriter->writeStartElement("geometry");
  xmlwriter->writeAttribute("x1", QString::number(pos().x()));
  xmlwriter->writeAttribute("y1", QString::number(pos().y()));
  xmlwriter->writeAttribute("x2", QString::number(frameGeometry().width()));
  xmlwriter->writeAttribute("y2", QString::number(frameGeometry().height()));
  xmlwriter->writeEndElement();
  xmlwriter->writeStartElement("content");
  xmlwriter->writeAttribute("value", getText());
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
}

bool Note::load(XmlStreamReader* xmlreader) {
  if (xmlreader->isStartElement() && xmlreader->name() == "note") {
    QString prefix(tr("XML read error: ", "prefix for XML error messages"));
    QString postfix(tr(" (non-critical)", "postfix for XML error messages"));

    QXmlStreamAttributes attribs = xmlreader->attributes();
    QString basicattr;

    // read name
    basicattr =
        attribs.value(xmlreader->namespaceUri().toString(), "name").toString();
    if (basicattr.isEmpty()) {
      xmlreader->raiseWarning(prefix + tr("aspect name missing or empty") +
                              postfix);
    }
    setName(basicattr);
    // read creation time
    basicattr =
        attribs.value(xmlreader->namespaceUri().toString(), "creation_time")
            .toString();
    QDateTime creation_time =
        QDateTime::fromString(basicattr, "yyyy-dd-MM hh:mm:ss:zzz");
    if (basicattr.isEmpty() || !creation_time.isValid()) {
      xmlreader->raiseWarning(
          tr("Invalid creation time for '%1'. Using current time.")
              .arg(name()));
      setBirthDate(QDateTime::currentDateTime().toString(Qt::LocalDate));
    } else
      setBirthDate(creation_time.toString(Qt::LocalDate));
    // read caption spec
    basicattr =
        attribs.value(xmlreader->namespaceUri().toString(), "caption_spec")
            .toString();
    setCaptionPolicy(static_cast<MyWidget::CaptionPolicy>(basicattr.toInt()));
    basicattr = attribs.value(xmlreader->namespaceUri().toString(), "status")
                    .toString();
    setStatus(static_cast<MyWidget::Status>(basicattr.toInt()));

    // read child elements
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();

      if (xmlreader->isEndElement() && xmlreader->name() == "note") break;

      if (xmlreader->isStartElement()) {
        if (xmlreader->name() == "content") {
          QXmlStreamAttributes attribs = xmlreader->attributes();
          QString value =
              attribs.value(xmlreader->namespaceUri().toString(), "value")
                  .toString();
          setText(value);
        } else {
          // unknown element
          xmlreader->raiseWarning(
              tr("unknown element '%1'").arg(xmlreader->name().toString()));
          if (!xmlreader->skipToEndElement()) return false;
        }
      } else if (xmlreader->isStartElement()) {
        if (xmlreader->name() == "geometry") {
          QXmlStreamAttributes attribs = xmlreader->attributes();
          QString valuex1 =
              attribs.value(xmlreader->namespaceUri().toString(), "x1")
                  .toString();
          QString valuey1 =
              attribs.value(xmlreader->namespaceUri().toString(), "y1")
                  .toString();
          QString valuex2 =
              attribs.value(xmlreader->namespaceUri().toString(), "x2")
                  .toString();
          QString valuey2 =
              attribs.value(xmlreader->namespaceUri().toString(), "y2")
                  .toString();
          setGeometry(valuex1.toInt(), valuey1.toInt(), valuex2.toInt(),
                      valuey2.toInt());
        } else {
          // unknown element
          xmlreader->raiseWarning(
              tr("unknown element '%1'").arg(xmlreader->name().toString()));
          if (!xmlreader->skipToEndElement()) return false;
        }
      }
    }
  } else  // no note element
    xmlreader->raiseError(tr("no note element found"));

  return !xmlreader->hasError();
}

void Note::setAutoexec(bool exec) { autoExec = exec; }
