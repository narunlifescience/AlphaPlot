/***************************************************************************
    File                 : ImageExportDialog.h
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006,2007 by Ion Vasilief,
                           Tilman Benkert, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : QFileDialog extended with options for image export

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
#ifndef IMAGEEXPORTDIALOG_H
#define IMAGEEXPORTDIALOG_H

#include "../../ui/ExtensibleFileDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QPrinter>
#include <QSpinBox>

class QStackedWidget;
class QGroupBox;

//! QFileDialog extended with options for image export
class ImageExportDialog2D : public ExtensibleFileDialog {
  Q_OBJECT

 public:
  explicit ImageExportDialog2D(QWidget *parent = nullptr,
                               bool vector_options = true, bool extended = true,
                               Qt::WindowFlags flags = nullptr);

  // getters
  int raster_resolution() const { return d_raster_resolution->value(); }
  int raster_width() const { return d_raster_width->value(); }
  int raster_height() const { return d_raster_height->value(); }
  double raster_scale() const { return d_raster_scale->value(); }
  int raster_quality() const { return d_raster_quality->value(); }
  int vector_width() const { return d_vector_width->value(); }
  int vector_height() const { return d_vector_height->value(); }

  // setters
  void setraster_resolution(int value) { d_raster_resolution->setValue(value); }
  void setraster_width(int value) { d_raster_width->setValue(value); }
  void setraster_height(int value) { d_raster_height->setValue(value); }
  void setraster_scale(double value) { d_raster_scale->setValue(value); }
  void setraster_quality(int value) { d_raster_quality->setValue(value); }
  void setvector_width(int value) { d_vector_width->setValue(value); }
  void setvector_height(int value) { d_vector_height->setValue(value); }

  void selectFilter(const QString &filter);

 protected slots:
  void closeEvent(QCloseEvent *event);
  // Update which options are visible and enabled based on the output format.
  void updateAdvancedOptions(const QString &filter);

 private:
  // Create #d_advanced_options and everything it contains.
  void initAdvancedOptions();

  //! Container widget for all advanced options.
  QStackedWidget *d_advanced_options;
  // vector format options
  //! Container widget for all options available for vector formats.
  QGroupBox *d_vector_options;
  QSpinBox *d_vector_width;
  QSpinBox *d_vector_height;
  // raster format options
  //! Container widget for all options available for raster formats.
  QGroupBox *d_raster_options;
  QSpinBox *d_raster_resolution;
  QSpinBox *d_raster_width;
  QSpinBox *d_raster_height;
  QDoubleSpinBox *d_raster_scale;
  QSpinBox *d_raster_quality;
};

#endif  // IMAGEEXPORTDIALOG_H
