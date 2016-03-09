/* This file is part of AlphaPlot.

   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : AlphaPlot icon loader
*/

#ifndef ICONLOADER_H
#define ICONLOADER_H

#include <QIcon>

class IconLoader {
 public:
  enum IconMode {
    General,
    LightDark,
  };
  static void init();
  static QIcon load(const QString& name, const IconMode& iconMode);
  static int inline isLight(const QColor& color) {
    // convert window background to a scale of darkness to choose which
    // icons(light or dark) to load using something similer to
    // http://stackoverflow.com/questions/12043187/
    // how-to-check-if-hex-color-is-too-black
    return static_cast<int>(0.2126 * color.red() + 0.7152 * color.blue() +
                            0.0722 * color.green());
  }
  static int lumen_;

 private:
  IconLoader() {}

  enum IconType {
    Dark = 0,
    Light = 1,
    Indiscriminate = 2,
  };

  static QList<int> sizes_;
  static QList<QString> icon_path_;
};

#endif  // ICONLOADER_H
