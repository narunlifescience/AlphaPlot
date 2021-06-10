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

#include <QDate>
#include <QIcon>
#include <QMessageBox>
#include <QMetaEnum>
#include <QMetaObject>
#include <QObject>
#include <QtDebug>

const int AlphaPlot::AlphaPlot_versionNo = 112020;
const char* AlphaPlot::AlphaPlot_version = "1.011A";
const char* AlphaPlot::extra_version = "alpha";
const QDate AlphaPlot::release_date =
    QDate::fromString(QString("07-05-2020"), "dd-MM-yyyy");
const char* AlphaPlot::homepage_Uri = "https://alphaplot.sourceforge.io/";
const char* AlphaPlot::manual_Uri =
    "https://alphaplot.sourceforge.io/wiki/index.php";
const char* AlphaPlot::forum_Uri =
    "https://sourceforge.net/p/alphaplot/discussion/";
const char* AlphaPlot::bugreport_Uri =
    "https://github.com/narunlifescience/AlphaPlot/issues/new";
const char* AlphaPlot::download_Uri =
    "https://sourceforge.net/projects/alphaplot/";

QStringList AlphaPlot::locales_ = QStringList();

int AlphaPlot::version() { return AlphaPlot_versionNo; }

QString AlphaPlot::schemaVersion() {
  return "AlphaPlot " + QString::number((version() & 0xFF0000) >> 16) + "." +
         QString::number((version() & 0x00FF00) >> 8) + "." +
         QString::number(version() & 0x0000FF);
}

QString AlphaPlot::versionString() { return AlphaPlot_version; }

QString AlphaPlot::extraVersion() { return QString(extra_version); }

QString AlphaPlot::releaseDateString() {
  return release_date.toString("dd-MM-yyyy");
}

QDate AlphaPlot::releaseDate() { return release_date; }

QString AlphaPlot::originalAuthor() { return "Arun Narayanankutty"; }

QString AlphaPlot::originalAuthorWithMail() {
  return "Arun Narayanankutty: <n.arun.lifescience@gmail.com";
}

QStringList AlphaPlot::getLocales() { return locales_; }

void AlphaPlot::setLocales(QStringList locales) { locales_ = locales; }

QString AlphaPlot::enumValueToString(int key, const QString& enum_name) {
  int index = staticMetaObject.indexOfEnumerator(enum_name.toUtf8());
  if (index == -1) return QString("invalid");
  QMetaEnum meta_enum = staticMetaObject.enumerator(index);
  return QString(meta_enum.valueToKey(key));
}

int AlphaPlot::enumStringToValue(const QString& string,
                                 const QString& enum_name) {
  int index = staticMetaObject.indexOfEnumerator(enum_name.toUtf8());
  if (index == -1) return -1;
  QMetaEnum meta_enum = staticMetaObject.enumerator(index);
  return meta_enum.keyToValue(string.toUtf8());
}
