/***************************************************************************
    File                 : globals.cpp
    Description          : Definition of global constants and enums
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2006-2007 Ion Vasilief (ion_vasilief*yahoo.fr)
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

#include "globals.h"
#include <QMessageBox>
#include <QIcon>
#include <QObject>
#include <QMetaObject>
#include <QMetaEnum>
#include <QtDebug>
#include "ui_AlphaPlotAbout.h"

const int AlphaPlot::AlphaPlot_versionNo = 00001;
const char* AlphaPlot::AlphaPlot_version = "0.01A";
const char* AlphaPlot::extra_version = "-alpha";
const char* AlphaPlot::release_date = "March 10, 2016";
const char* AlphaPlot::homepage_Uri = "http://AlphaPlot.sourceforge.net";
const char* AlphaPlot::manual_Uri =
    "http://sourceforge.net/projects/AlphaPlot/files/AlphaPlot%20Documentation/"
    "0.1/";
const char* AlphaPlot::forum_Uri =
    "http://sourceforge.net/forum/?group_id=199120";
const char* AlphaPlot::bugreport_Uri =
    "http://sourceforge.net/tracker/?group_id=199120&atid=968214";
const char* AlphaPlot::download_Uri =
    "http://sourceforge.net/projects/AlphaPlot/files/AlphaPlot/";
const char* AlphaPlot::copyright_string =
    "=== Credits ===\n\
\n\
--- Developers ---\n\
\n\
The following people have written parts of the AlphaPlot source code, ranging from a few lines to large chunks.\n\
In alphabetical order.\n\
\n\
Tilman Benkert[1], Knut Franke, Dmitriy Pozitron, Russell Standish\n\
\n\
--- Documentation ---\n\
\n\
The following people have written parts of the manual and/or other documentation.\n\
In alphabetical order.\n\
\n\
Knut Franke, Roger Gadiou\n\
\n\
--- Translations ---\n\
\n\
The following people have contributed translations or parts thereof.\n\
In alphabetical order.\n\
\n\
Tilman Benkert[1], Markus Bongard, Tobias Burnus, R�my Claverie, f0ma, Jos� Antonio Lorenzo Fern�ndez, \
Pavel Fric, Jan Helebrant, Daniel Klaer, Peter Landgren, Fellype do Nascimento, Tomomasa Ohkubo, \
Mikhail Shevyakov, Russell Standish, Mauricio Troviano\n\
\n\
--- Packagers ---\n\
\n\
The following people have made installing AlphaPlot easier by providing specialized binary packages.\n\
In alphabetical order.\n\
\n\
Burkhard Bunk (Debian), Quentin Denis (SUSE), Yu-Hung Lien (old Mac OS X), Fellype do Nascimento (Slackware), Russell Standish (new MacOSX)\
Eric Tanguy (Fedora), Mauricio Troviano (Windows installer)\n\
\n\
--- QtiPlot ---\n\
\n\
AlphaPlot uses code from QtiPlot, which consisted (at the time of the fork, i.e. QtiPlot 0.9-rc2) of code by the following people:\n\
\n\
Tilman Benkert[1], Shen Chen, Borries Demeler, Jos� Antonio Lorenzo Fern�ndez, Knut Franke, Vasileios Gkanis, Gudjon Gudjonsson, \
Alex Kargovsky, Michael Mac-Vicar, Tomomasa Ohkubo, Aaron Van Tassle, Branimir Vasilic, Ion Vasilief, Vincent Wagelaar\n\
\n\
The AlphaPlot manual is based on the QtiPlot manual, written by (in alphabetical order):\n\
\n\
Knut Franke, Roger Gadiou, Ion Vasilief\n\
\n\
footnotes:\n\
[1] birth name: Tilman H�ner zu Siederdissen\n\
\n\
=== Special Thanks ===\n\
\n\
We also want to acknowledge the people having helped us indirectly by contributing to the following\n\
fine pieces of software. In no particular order.\n\
\n\
Qt (http://doc.trolltech.com/4.3/credits.html),\n\
Qwt (http://qwt.sourceforge.net/#credits),\n\
Qwtplot3D (http://qwtplot3d.sourceforge.net/),\n\
muParser (http://muparser.sourceforge.net/),\n\
Python (http://www.python.org/),\n\
GSL (http://www.gnu.org/software/gsl/)\n\
\n\
... and many more we just forgot to mention.\n";

int AlphaPlot::version() { return AlphaPlot_versionNo; }

QString AlphaPlot::schemaVersion() {
  return "AlphaPlot " + QString::number((version() & 0xFF0000) >> 16) + "." +
         QString::number((version() & 0x00FF00) >> 8) + "." +
         QString::number(version() & 0x0000FF);
}

QString AlphaPlot::versionString() { return AlphaPlot_version; }

QString AlphaPlot::extraVersion() { return QString(extra_version); }

void AlphaPlot::about() {
  QString text = QString(AlphaPlot::copyright_string);
  text.replace(QRegExp("\\[1\\]"), "<sup>1</sup>");
  text.replace("�", "&eacute;");
  text.replace("�", "&aacute;");
  text.replace("�", "&ouml;");
  text.replace("\n", "<br>");
  text.replace("=== ", "<h1>");
  text.replace(" ===", "</h1>");
  text.replace("--- ", "<h2>");
  text.replace(" ---", "</h2>");
  text.replace(" ---", "</h2>");
  text.replace("</h1><br><br>", "</h1>");
  text.replace("</h2><br><br>", "</h2>");
  text.replace("<br><h1>", "<h1>");
  text.replace("<br><h2>", "<h2>");

  Qt::WindowFlags flags = Qt::WindowTitleHint | Qt::WindowSystemMenuHint;
#if QT_VERSION >= 0x040500
  flags |= Qt::WindowCloseButtonHint;
#endif
  QDialog* dialog = new QDialog(0, flags);
  Ui::AlphaPlotAbout ui;
  ui.setupUi(dialog);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->setWindowTitle(QObject::tr("About AlphaPlot"));
  ui.version_label->setText("AlpaPlot " + versionString() + extraVersion());
  ui.release_date_label->setText(QObject::tr("Released") + ": " +
                                 QString(AlphaPlot::release_date));
  ui.credits_box->setHtml(text);

  dialog->exec();
}

QString AlphaPlot::copyrightString() { return copyright_string; }

QString AlphaPlot::releaseDateString() { return release_date; }

QString AlphaPlot::enumValueToString(int key, const QString& enum_name) {
  int index = staticMetaObject.indexOfEnumerator(enum_name.toAscii());
  if (index == -1) return QString("invalid");
  QMetaEnum meta_enum = staticMetaObject.enumerator(index);
  return QString(meta_enum.valueToKey(key));
}

int AlphaPlot::enumStringToValue(const QString& string,
                                 const QString& enum_name) {
  int index = staticMetaObject.indexOfEnumerator(enum_name.toAscii());
  if (index == -1) return -1;
  QMetaEnum meta_enum = staticMetaObject.enumerator(index);
  return meta_enum.keyToValue(string.toAscii());
}
