/***************************************************************************
    File                 : ImageExportDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006,2007 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen, Knut Franke
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

#include <QStackedWidget>
#include <QImageWriter>
#include <QGroupBox>
#include <QPushButton>
#include <QGridLayout>
#include <QPrinter>
#include <QLabel>
#include <QComboBox>

ImageExportDialog::ImageExportDialog( QWidget * parent, bool vector_options, Qt::WFlags flags)
	: QFileDialog( parent, flags )
{
	setWindowTitle( tr( "QtiPlot - Choose a filename to save under" ) );
	setAcceptMode(QFileDialog::AcceptSave);

	QList<QByteArray> list = QImageWriter::supportedImageFormats();
	list<<"EPS";
	list<<"PS";
	list<<"PDF";
	#if QT_VERSION >= 0x040300
		list<<"SVG";
	#endif

	QStringList filters;
	for(int i=0 ; i<list.count() ; i++)
	{
		filters << "*."+list[i].toLower();
	}
	filters.sort();
	setFilters(filters);
	setFileMode( QFileDialog::AnyFile );

	initAdvancedOptions();
	if (!vector_options)
		d_vector_options->setEnabled(false);

	d_advanced_toggle = new QPushButton(tr("<< &Advanced"));
	d_advanced_toggle->setCheckable(true);
	connect(d_advanced_toggle, SIGNAL(toggled(bool)), d_advanced_options, SLOT(setVisible(bool)));

	QGridLayout *main_layout = qobject_cast<QGridLayout*>(layout());
	if (main_layout) {
		int advanced_row = main_layout->rowCount();
		main_layout->addWidget(d_advanced_toggle, advanced_row, main_layout->columnCount()-1, 2, 1);
		main_layout->addWidget(d_advanced_options, advanced_row, 0, 2, main_layout->columnCount()-1);
		main_layout->setColumnStretch(1, 1);
	} else {
		// fallback in case QFileDialog uses a different layout in the future
		main_layout->addWidget(d_advanced_toggle);
		main_layout->addWidget(d_advanced_options);
	}
	d_advanced_options->setVisible(false);

#if QT_VERSION >= 0x040300
	connect(this, SIGNAL(filterSelected ( const QString & )), 
			this, SLOT(updateAdvancedOptions ( const QString & )));
#else
	QList<QComboBox*> combo_boxes = findChildren<QComboBox*>();
	if (combo_boxes.size() >= 2)
		connect(combo_boxes[1], SIGNAL(currentIndexChanged ( const QString & )), 
				this, SLOT(updateAdvancedOptions ( const QString & )));
#endif
	updateAdvancedOptions(selectedFilter());
}

void ImageExportDialog::initAdvancedOptions()
{
	d_advanced_options = new QStackedWidget();

	d_vector_options = new QGroupBox();
	QGridLayout *vector_layout = new QGridLayout(d_vector_options);
	d_advanced_options->addWidget(d_vector_options);
	
	vector_layout->addWidget(new QLabel(tr("Resolution (DPI)")), 1, 0);
	d_resolution = new QSpinBox();
	d_resolution->setRange(0, 1000);
	d_resolution->setValue(QPrinter().resolution());
	vector_layout->addWidget(d_resolution, 1, 1);

	d_color = new QCheckBox();
	d_color->setText(tr("&Export in &color"));
	d_color->setChecked(true);
	vector_layout->addWidget(d_color, 2, 0, 1, 2);

	d_raster_options = new QGroupBox();
	QGridLayout *raster_layout = new QGridLayout(d_raster_options);
	d_advanced_options->addWidget(d_raster_options);

	raster_layout->addWidget(new QLabel(tr("Image quality")), 1, 0);
	d_quality = new QSpinBox();
	d_quality->setRange(1, 100);
	d_quality->setValue(100);
	raster_layout->addWidget(d_quality, 1, 1);

	d_transparency = new QCheckBox();
	d_transparency->setText(tr("Save transparency"));
	d_transparency->setChecked(false);
	raster_layout->addWidget(d_transparency, 2, 0, 1, 2);
}

void ImageExportDialog::updateAdvancedOptions (const QString & filter)
{
	if (filter.contains("*.svg")) {
		d_advanced_toggle->setChecked(false);
		d_advanced_toggle->setEnabled(false);
		return;
	}
	d_advanced_toggle->setEnabled(true);
	if (filter.contains("*.eps") || filter.contains("*.ps") || filter.contains("*.pdf"))
		d_advanced_options->setCurrentIndex(0);
	else {
		d_advanced_options->setCurrentIndex(1);
		d_transparency->setEnabled(filter.contains("*.tif") || filter.contains("*.tiff") || filter.contains("*.png") || filter.contains("*.xpm"));
	}
}
