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

#include "core/Utilities.h"

#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <QSysInfo>
#include <cmath>
#include <QPalette>
#include <QSettings>
#include <QApplication>
#include <QStyle>

#ifdef Q_OS_WIN
#include <assert.h>
#include <windows.h>
#endif

double Utilities::rgbRandomSeed_ = static_cast<double>(rand()) / RAND_MAX;
int Utilities::rgbCounter_ = 0;

int Utilities::getWordSizeApp() {
#if defined(Q_BUILD_64)
  return 64;
#elif defined(Q_BUILD_32)
  return 32;
#else
  return 0;
#endif
}

QString Utilities::getOperatingSystem() {
#if defined(Q_OS_WIN32)
  switch (QSysInfo::windowsVersion()) {
    case QSysInfo::WV_NT:
      return QString("Windows NT");
    case QSysInfo::WV_2000:
      return QString("Windows 2000");
    case QSysInfo::WV_XP:
      return QString("Windows XP");
    case QSysInfo::WV_2003:
      return QString("Windows Server 2003");
    case QSysInfo::WV_VISTA:
      return QString("Windows Vista");
    case QSysInfo::WV_WINDOWS7:
      return QString("Windows 7");
    case QSysInfo::WV_WINDOWS8:
      return QString("Windows 8");
    case QSysInfo::WV_WINDOWS8_1:
      return QString("Windows 8.1");
    case QSysInfo::WV_WINDOWS10:
      return QString("Windows 10");
    default:
      return QString("Windows");
  }
#elif defined(Q_OS_MAC)
  switch (QSysInfo::MacVersion()) {
    case QSysInfo::MV_10_3:
      return QString("Mac OS X 10.3");
    case QSysInfo::MV_10_4:
      return QString("Mac OS X 10.4");
    case QSysInfo::MV_10_5:
      return QString("Mac OS X 10.5");
    case QSysInfo::MV_10_6:
      return QString("Mac OS X 10.6");
    case QSysInfo::MV_10_7:
      return QString("Mac OS X 10.7");
    case QSysInfo::MV_10_8:
      return QString("Mac OS X 10.8");
    case QSysInfo::MV_10_9:
      return QString("Mac OS X 10.9");
    case QSysInfo::MV_10_10:
      return QString("Mac OS X 10.10");
    // case QSysInfo::MV_10_11: (mot available in qt4)
    // return QString("Mac OS X 10.11");
    default:
      return QString("Mac OS X");
  }
#elif defined(Q_OS_LINUX)
  QString exe("lsb_release");
  QStringList args;
  args << "-ds";
  QProcess proc;
  proc.setEnvironment(QProcess::systemEnvironment());
  proc.start(exe, args);
  if (proc.waitForStarted() && proc.waitForFinished()) {
    QByteArray info = proc.readAll();
    info.replace('\n', "");
    info.replace("\"", "");
    return QString(info);
  }

  return QString("Linux");
#elif defined(Q_OS_UNIX)
  return QString("UNIX");
#else
  return QString("Unknown");
#endif
}

int Utilities::getWordSizeOfOS() {
#if defined(Q_OS_WIN64)
  return 64;  // 64-bit process running on 64-bit windows
#elif defined(Q_OS_WIN32)

  // determine if 32-bit process running on 64-bit windows in WOW64 emulation
  // or 32-bit process running on 32-bit windows
  // default bIsWow64 to false for 32-bit process on 32-bit windows

  BOOL bIsWow64 = false;  // must default to false
  typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

  LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
      GetModuleHandle(L"kernel32"), "IsWow64Process");

  if (NULL != fnIsWow64Process) {
    if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
      assert(false);  // something went majorly wrong
    }
  }
  return bIsWow64 ? 64 : 32;

#elif defined(Q_OS_LINUX)
  // http://stackoverflow.com/questions/246007/how-to-determine-whether-
  // a-given-linux-is-32-bit-or-64-bit
  QString exe(QLatin1String("getconf"));
  QStringList args;
  args << QLatin1String("LONG_BIT");
  QProcess proc;
  proc.setEnvironment(QProcess::systemEnvironment());
  proc.start(exe, args);
  if (proc.waitForStarted() && proc.waitForFinished()) {
    QByteArray info = proc.readAll();
    info.replace('\n', "");
    return info.toInt();
  }

  return 0;  // failed

#elif defined(Q_OS_UNIX) || defined(Q_OS_MAC)
  QString exe(QLatin1String("uname"));
  QStringList args;
  args << QLatin1String("-m");
  QProcess proc;
  proc.setEnvironment(QProcess::systemEnvironment());
  proc.start(exe, args);
  if (proc.waitForStarted() && proc.waitForFinished()) {
    QByteArray info = proc.readAll();
    info.replace('\n', "");
    if (info.indexOf("x86_64") >= 0)
      return 64;
    else if (info.indexOf("amd64") >= 0)
      return 64;
    else if (info.indexOf("ia64") >= 0)
      return 64;
    else if (info.indexOf("ppc64") >= 0)
      return 64;
    else if (info.indexOf("i386") >= 0)
      return 32;
    else if (info.indexOf("i686") >= 0)
      return 32;
    else if (info.indexOf("x86") >= 0)
      return 32;
  }

  return 0;  // failed
#else
  return 0;  // unknown
#endif
}

QColor Utilities::getRandColorGoldenRatio(const ColorPal& colpal) {
  rgbCounter_++;
  // use golden ratio
  const double goldenRatioConjugate =
      1.6180339887498949025257388711906969547271728515625;
  double hue = rgbRandomSeed_;
  hue += goldenRatioConjugate * rgbCounter_;
  hue = fmod(hue, 1);
  double saturation;
  switch (colpal) {
    case ColorPal::Light:
      saturation = 0.5;
      break;
    case ColorPal::Dark:
      saturation = 0.9;
      break;
  }
  double value = 0.95;

  double p, q, t, ff;
  int i;
  QColor rgb;

  i = static_cast<int>(floor(hue * 6));
  ff = (hue * 6) - i;

  p = value * (1.0 - saturation);
  q = value * (1.0 - (saturation * ff));
  t = value * (1.0 - (saturation * (1.0 - ff)));

  switch (i) {
    case 0:
      rgb.setRedF(value);
      rgb.setGreenF(t);
      rgb.setBlueF(p);
      break;
    case 1:
      rgb.setRedF(q);
      rgb.setGreenF(value);
      rgb.setBlueF(p);
      break;
    case 2:
      rgb.setRedF(p);
      rgb.setGreenF(value);
      rgb.setBlueF(t);
      break;

    case 3:
      rgb.setRedF(p);
      rgb.setGreenF(q);
      rgb.setBlueF(value);
      break;
    case 4:
      rgb.setRedF(t);
      rgb.setGreenF(p);
      rgb.setBlueF(value);
      break;
    case 5:
    default:
      rgb.setRedF(value);
      rgb.setGreenF(p);
      rgb.setBlueF(q);
      break;
  }

  return rgb;
}

QPalette Utilities::getApplicationPalette()
{
  QPalette pal;
  QSettings settings;
  settings.beginGroup("General");
  QString appstyle =
      settings.value("Style", qApp->style()->objectName()).toString();
  int colorscheme = settings.value("ColorScheme", 0).toInt();
  settings.beginGroup("Colors");
  bool ncustomcolors_ = settings.value("Custom", false).toBool();
  QColor nworkspacecolor_ =
      settings.value("Workspace", "darkGray").value<QColor>();
  QColor npanelcolor_ =
      settings.value("Panels", qApp->palette().window().color()).value<QColor>();
  QColor npaneltextcolor_ =
      settings.value("PanelsText", qApp->palette().windowText().color())
          .value<QColor>();
  settings.endGroup();
  settings.endGroup();
  switch (colorscheme) {
    case 0: {
        pal = qApp->palette();
        pal.setBrush(QPalette::NoRole, pal.color(QPalette::Active, QPalette::Base));
    } break;
    case 1: {
      // QFile schemefile(":style/alpha/dark.qss");
      pal.setBrush(QPalette::Window, qApp->palette().base());
      pal.setBrush(QPalette::WindowText, qApp->palette().windowText());
      pal.setBrush(QPalette::NoRole, QBrush(QColor(32, 31, 31)));
    } break;
    case 2: {
      //QFile schemefile(":style/smooth/dark-blue.qss");
      pal.setBrush(QPalette::Window, qApp->palette().base());
      pal.setBrush(QPalette::WindowText, qApp->palette().windowText());
      pal.setBrush(QPalette::NoRole, QBrush(QColor(200, 200, 200)));
    } break;
    case 3: {
      //QFile schemefile(":style/smooth/dark-green.qss");
      pal.setBrush(QPalette::Window, qApp->palette().base());
      pal.setBrush(QPalette::WindowText, qApp->palette().windowText());
      pal.setBrush(QPalette::NoRole, QBrush(QColor(200, 200, 200)));
    } break;
    case 4: {
      //QFile schemefile(":style/smooth/dark-orange.qss");
      pal.setBrush(QPalette::Window, qApp->palette().base());
      pal.setBrush(QPalette::WindowText, qApp->palette().windowText());
      pal.setBrush(QPalette::NoRole, QBrush(QColor(200, 200, 200)));
    } break;
    case 5: {
      //QFile schemefile(":style/smooth/light-blue.qss");
      pal.setBrush(QPalette::Window, qApp->palette().base());
      pal.setBrush(QPalette::WindowText, qApp->palette().windowText());
      pal.setBrush(QPalette::NoRole, QBrush(QColor(230, 230, 230)));
    } break;
    case 6: {
      //QFile schemefile(":style/smooth/light-green.qss");
      pal.setBrush(QPalette::Window, qApp->palette().base());
      pal.setBrush(QPalette::WindowText, qApp->palette().windowText());
      pal.setBrush(QPalette::NoRole, QBrush(QColor(230, 230, 230)));
    } break;
    case 7: {
      //QFile schemefile(":style/smooth/light-orange.qss");
      pal.setBrush(QPalette::Window, qApp->palette().base());
      pal.setBrush(QPalette::WindowText, qApp->palette().windowText());
      pal.setBrush(QPalette::NoRole, QBrush(QColor(230, 230, 230)));
    } break;
    default:
      // should not reach
      qDebug() << "color scheme index out of range";
      break;
  }

  return pal;
}

QString Utilities::splitstring(const QString& string) {
  if (string.isEmpty()) return string;

  // line break
  QStringList stringlist = string.split("<|>");
  QString final = stringlist.at(0);
  for (int i = 1; i < stringlist.count(); i++) {
    final += "\n" + stringlist.at(i);
  }

  return final;
}

QString Utilities::joinstring(const QString& string) {
  if (string.isEmpty()) return string;

  QStringList stringlist = string.split("\n");
  QString final = stringlist.at(0);
  for (int i = 1; i < stringlist.count(); i++) {
    final += "<|>" + stringlist.at(i);
  }
  return final;
}

QDateTime Utilities::stripDateTimeToFormat(const QDateTime& datetime,
                                           const QString& format) {
  return QDateTime::fromString(datetime.toString(format), format);
}

QImage Utilities::convertToGrayScale(const QImage& srcImage) {
  // Convert to 32bit pixel format
  QImage dstImage = srcImage.convertToFormat(srcImage.hasAlphaChannel()
                                                 ? QImage::Format_ARGB32
                                                 : QImage::Format_RGB32);

  for (int ii = 0; ii < dstImage.width(); ii++) {
    for (int jj = 0; jj < dstImage.height(); jj++) {
      int gray = qGray(dstImage.pixel(ii, jj));
      dstImage.setPixel(ii, jj, QColor(gray, gray, gray).rgb());
    }
  }
  return dstImage;
}

QImage Utilities::convertToGrayScaleFast(const QImage& srcImage) {
  QImage image = srcImage.convertToFormat(srcImage.hasAlphaChannel()
                                              ? QImage::Format_ARGB32
                                              : QImage::Format_RGB32);

  for (int ii = 0; ii < image.height(); ii++) {
    uchar* scan = image.scanLine(ii);
    int depth = 4;
    for (int jj = 0; jj < image.width(); jj++) {
      QRgb* rgbpixel = reinterpret_cast<QRgb*>(scan + jj * depth);
      int gray = qGray(*rgbpixel);
      *rgbpixel = QColor(gray, gray, gray).rgba();
    }
  }
  return image;
}

QString Utilities::makeHtmlTable(const int row, const int column,
                                 const bool hasheader,
                                 const TableColorProfile& profile) {
  QString strokecolor;
  QString fillcolor;
  QString headerfillcolor;
  switch (profile) {
    case TableColorProfile::Success:
      strokecolor = "rgba(0,180,0,0.8)";
      headerfillcolor = "#00de6c";
      fillcolor = "#00ffa9";
      break;
    case TableColorProfile::Failure:
      strokecolor = "rgba(255,0,0,0.8)";
      headerfillcolor = "#ff5b5b";
      fillcolor = "#ffa9a9";
      break;
    case TableColorProfile::Generic:
      strokecolor = "trnsparent";
      headerfillcolor = "transparent";
      fillcolor = "transparent";
      break;
  }

  QString string =
      "<p><table style = \"border-collapse:collapse;width:100%;\"> ";
  QString thstyle = "style = \"border:1px solid " + strokecolor +
                    ";background-color:" + headerfillcolor +
                    ";text-align:left;padding:5px;color:black;\"";
  QString tdstyle = "style = \"border:1px solid " + strokecolor +
                    ";background-color:" + fillcolor +
                    ";text-align:left;padding:5px;color:black;\"";
  QString rowcolumnstring;
  int count = 1;
  // title
  if (hasheader) {
    rowcolumnstring += "<tr>";
    for (int j = 0; j < column; j++) {
      rowcolumnstring +=
          "<th " + thstyle + ">%" + QString::number(count++) + "</th>";
    }
    rowcolumnstring += "</tr>";
  }
  // rows
  for (int i = 0; i < row; i++) {
    rowcolumnstring += "<tr>";
    for (int j = 0; j < column; j++) {
      rowcolumnstring +=
          "<td " + tdstyle + ">%" + QString::number(count++) + "</td>";
    }
    rowcolumnstring += "</tr>";
  }
  rowcolumnstring += "</table></p>";
  return string + rowcolumnstring;
}

bool Utilities::isSameDouble(const double value1, const double value2) {
  if (std::fabs(value1 - value2) > std::numeric_limits<double>::epsilon())
    return false;
  else
    return true;
}

QString Utilities::getTooltipText(const TooltipType& type) {
  switch (type) {
    case Utilities::TooltipType::x:
      return QString(
          "<tr> <td align=\"right\">Table :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Column :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%5</td></tr>");
      break;
    case Utilities::TooltipType::xy:
      return QString(
          "<tr> <td align=\"right\">Table :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Column X :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">Column Y :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%5</td></tr>");
      break;
    case Utilities::TooltipType::xyy:
      return QString(
          "<tr> <td align=\"right\">Table :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Column X :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">Column Y1 :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">Column Y2 :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%6</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%7</td></tr>");
      break;
    case Utilities::TooltipType::xyyy:
      return QString(
          "<tr> <td align=\"right\">Table :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Column :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">Column :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">Column :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">Column :</td><td>%5</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%6</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%7</td></tr>");
      break;
    case Utilities::TooltipType::matrix:
      return QString(
          "<tr> <td align=\"right\">Matrix :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Rows :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">Columns :</td><td>%4</td></tr>");
      break;
    case Utilities::TooltipType::funcxy:
      return QString(
          "<tr> <td align=\"right\">Type :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Function :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">Points :</td><td>%5</td></tr>");
      break;
    case Utilities::TooltipType::funcparam:
      return QString(
          "<tr> <td align=\"right\">Type :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Function X :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">Function Y :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">Parameter :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%5</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%6</td></tr>"
          "<tr> <td align=\"right\">Points :</td><td>%7</td></tr>");
      break;
    case Utilities::TooltipType::funcpolar:
      return QString(
          "<tr> <td align=\"right\">Type :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Function R :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">Function Theta :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">Parameter :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%5</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%6</td></tr>"
          "<tr> <td align=\"right\">Points :</td><td>%7</td></tr>");
      break;
    case Utilities::TooltipType::funcsurface:
      return QString(
          "<tr> <td align=\"right\">Function :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">xl :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">xu :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">yl :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">yu :</td><td>%5</td></tr>"
          "<tr> <td align=\"right\">zl :</td><td>%6</td></tr>"
          "<tr> <td align=\"right\">zu :</td><td>%7</td></tr>"
          "<tr> <td align=\"right\">Points :</td><td>%8</td></tr>");
      break;
  }
  return QString();
}
