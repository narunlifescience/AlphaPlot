/***************************************************************************
    File                 : iconloader.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2016 by Arun Narayanankutty
    Email                : n.arun.lifescience@gmail.com
    Description          : Load icons 
                           
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

/* TODO: 
   1) use diffrent size icons 16x16, 22x22, 32x32 etc..(up to 64x64)
   2) use only svg icons insted of xpm
   3) use freedesktop icon naming when possible */

#include "IconLoader.h"

#include <QFile>
#include <QtDebug>

int IconLoader::lumen_;
QList<int> IconLoader::sizes_;
QList<QString> IconLoader::icon_path_;

void IconLoader::init()
{
  sizes_.clear();
  sizes_ << 16 << 22 << 24;
  icon_path_.clear();
  icon_path_ << ":icons/dark" << ":icons/light";
}

QIcon IconLoader::load(const QString& name) 
{
  QIcon ret;
  // If the icon name is empty
  if (name.isEmpty()) 
  {
    qDebug() << "Icon name is null";
    return ret;
  }

  // comment out this for now as we dont
  // want to load icons from system theme
  // atleast not for now.
/*#if QT_VERSION >= 0x040600
  ret = QIcon::fromTheme(name);
  if (!ret.isNull()) return ret;
#endif*/

  QString filename;
  (lumen_ < 100) ? filename = icon_path_.at(Dark)
                 : filename = icon_path_.at(Light);

  const QString locate(filename + "/%1/%2.svg");
  for (int i=0; i < sizes_.size(); i++)
  {
    QString filename_custom(locate.arg(sizes_.at(i)).arg(name));

    if (QFile::exists(filename_custom))
    {
      ret.addFile(filename_custom, QSize(sizes_.at(i), sizes_.at(i)));
    }
    
  }

  if (ret.isNull())
  {
    qDebug() << "Couldn't load icon" << name;
  }
  return ret;
}
