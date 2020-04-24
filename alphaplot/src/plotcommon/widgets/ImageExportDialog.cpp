/***************************************************************************
    File                 : ImageExportDialog.cpp
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
#include "ImageExportDialog.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QImageWriter>
#include <QLabel>
#include <QPrinter>
#include <QPushButton>
#include <QStackedWidget>

ImageExportDialog::ImageExportDialog(QWidget *parent, bool vector_options,
                                     bool extended, Qt::WindowFlags flags)
    : ExtensibleFileDialog(parent, extended, flags),
      d_advanced_options(new QStackedWidget(this)),
      d_vector_options(new QGroupBox(this)),
      d_vector_width(new QSpinBox(this)),
      d_vector_height(new QSpinBox(this)),
      d_raster_options(new QGroupBox(this)),
      d_raster_resolution(new QSpinBox(this)),
      d_raster_width(new QSpinBox(this)),
      d_raster_height(new QSpinBox(this)),
      d_raster_scale(new QDoubleSpinBox(this)),
      d_raster_maa(new QSpinBox(this)),
      d_raster_quality(new QSpinBox(this)) {
  setWindowTitle(tr("Choose a filename to save under"));
  setAcceptMode(QFileDialog::AcceptSave);

  QList<QByteArray> list = QImageWriter::supportedImageFormats();
  list << "PS";
  list << "PDF";
  list << "SVG";

  QStringList filters;
  for (int i = 0; i < list.count(); i++) {
    filters << "*." + list[i].toLower();
  }

  // remove duplicates after toLower()
  filters.removeDuplicates();

  filters.sort();
  setNameFilters(filters);
  setFileMode(QFileDialog::AnyFile);

  initAdvancedOptions();
  d_vector_options->setEnabled(vector_options);
  setExtensionWidget(d_advanced_options);

  connect(this, SIGNAL(filterSelected(const QString &)), this,
          SLOT(updateAdvancedOptions(const QString &)));
  updateAdvancedOptions(selectedNameFilter());
}

void ImageExportDialog::initAdvancedOptions() {
  QGridLayout *vector_layout = new QGridLayout(d_vector_options);
  d_advanced_options->addWidget(d_vector_options);
  d_vector_width->setRange(0, 5000);
  d_vector_width->setValue(0);
  vector_layout->addWidget(new QLabel(tr("Image Width"), this), 1, 0);
  vector_layout->addWidget(d_vector_width, 1, 1);
  d_vector_height->setRange(0, 5000);
  d_vector_height->setValue(0);
  vector_layout->addWidget(new QLabel(tr("Image Width"), this), 2, 0);
  vector_layout->addWidget(d_vector_height, 2, 1);

  QGridLayout *raster_layout = new QGridLayout(d_raster_options);
  d_advanced_options->addWidget(d_raster_options);

  raster_layout->addWidget(new QLabel(tr("Resolution (DPI)")), 1, 0);
  d_raster_resolution->setRange(20, 1500);
  d_raster_resolution->setValue(300);
  raster_layout->addWidget(d_raster_resolution, 1, 1);
  d_raster_width->setRange(0, 5000);
  d_raster_width->setValue(0);
  raster_layout->addWidget(new QLabel(tr("Image Width"), this), 2, 0);
  raster_layout->addWidget(d_raster_width, 2, 1);
  d_raster_height->setRange(0, 5000);
  d_raster_height->setValue(0);
  raster_layout->addWidget(new QLabel(tr("Image Height"), this), 3, 0);
  raster_layout->addWidget(d_raster_height, 3, 1);
  d_raster_scale->setValue(1.0);
  d_raster_scale->setRange(1.0, 10.0);
  raster_layout->addWidget(new QLabel(tr("Image Scale"), this), 4, 0);
  raster_layout->addWidget(d_raster_scale, 4, 1);
  d_raster_maa->setValue(8);
  d_raster_maa->setRange(0, 100);
  raster_layout->addWidget(new QLabel(tr("Image Antialias"), this), 5, 0);
  raster_layout->addWidget(d_raster_maa, 5, 1);
  d_raster_quality->setValue(100);
  d_raster_quality->setRange(1, 100);
  raster_layout->addWidget(new QLabel(tr("Image quality"), this), 6, 0);
  raster_layout->addWidget(d_raster_quality, 6, 1);
}

void ImageExportDialog::updateAdvancedOptions(const QString &filter) {
  d_extension_toggle->setEnabled(true);
  if (filter.contains("*.svg") || filter.contains("*.ps") ||
      filter.contains("*.pdf"))
    d_advanced_options->setCurrentIndex(0);
  else {
    d_advanced_options->setCurrentIndex(1);
  }
}

void ImageExportDialog::closeEvent(QCloseEvent *event) { event->accept(); }

void ImageExportDialog::selectFilter(const QString &filter) {
  QFileDialog::selectNameFilter(filter);
  updateAdvancedOptions(filter);
}
