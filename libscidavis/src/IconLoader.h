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

#ifndef ICONLOADER_H
#define ICONLOADER_H

#include <QIcon>

class IconLoader {
 public:
  static void init();
  static QIcon load(const QString& name);
  static int inline isLight(const QColor &col)
  {
    // convert window background to a scale of darkness to choose which
    // icons(light or dark) to load using something similer to
    // http://stackoverflow.com/questions/12043187/
    // how-to-check-if-hex-color-is-too-black
    return static_cast<int>(0.2126 * col.red() + 0.7152 * col.blue() +
                            0.0722 * col.green());
  }
  static int lumen_;

 private:
  IconLoader() {}

  enum IconType
  {
    Dark = 0,
    Light = 1
  };

  static QList<int> sizes_;
  static QList<QString> icon_path_;
};

#endif  // ICONLOADER_H
