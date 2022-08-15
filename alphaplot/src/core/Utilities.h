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

   Description : AlphaPlot utilities : handle application wide

*/
#include <QColor>
#include <QImage>
#include <QString>

class Utilities {
 public:
  enum class ColorPal { Light, Dark };
  enum class TableColorProfile { Success, Failure, Generic };
  enum class TooltipType {
    x,
    xy,
    xyy,
    xyyy,
    matrix,
    funcxy,
    funcparam,
    funcpolar,
    funcsurface
  };
  static int getWordSizeApp();
  static QString getOperatingSystem();
  static int getWordSizeOfOS();
  static QColor getRandColorGoldenRatio(const ColorPal &colpal);
  static QPalette getApplicationPalette();
  static QString splitstring(const QString &string);
  static QString joinstring(const QString &string);

  static QDateTime stripDateTimeToFormat(const QDateTime &datetime,
                                         const QString &format);

  static QImage convertToGrayScale(const QImage &srcImage);
  static QImage convertToGrayScaleFast(const QImage &srcImage);

  // html Formatting
  static QString makeHtmlTable(const int row, const int column,
                               const bool hasheader,
                               const TableColorProfile &profile);
  static bool isSameDouble(const double value1, const double value2);

  static QString getTooltipText(const Utilities::TooltipType &type);

 private:
  Utilities() {}
  static double rgbRandomSeed_;
  static int rgbCounter_;
};
