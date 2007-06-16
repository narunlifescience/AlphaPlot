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
#include "ApplicationWindow.h"

#include <QStackedWidget>
#include <QImageWriter>
#include <QGroupBox>
#include <QPushButton>
#include <QGridLayout>
#include <QPrinter>
#include <QLabel>
#include <QComboBox>

ImageExportDialog::ImageExportDialog(QWidget * parent, bool vector_options, bool extended, Qt::WFlags flags)
	: ExtensibleFileDialog( parent, extended, flags )
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
		filters << "*."+list[i].toLower();

	filters.sort();
	setFilters(filters);
	setFileMode( QFileDialog::AnyFile );

	initAdvancedOptions();
	d_vector_options->setEnabled(vector_options);
	setExtensionWidget(d_advanced_options);

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
    ApplicationWindow *app = (ApplicationWindow *)this->parent();
	d_advanced_options = new QStackedWidget();

	d_vector_options = new QGroupBox();
	QGridLayout *vector_layout = new QGridLayout(d_vector_options);
	d_advanced_options->addWidget(d_vector_options);

	vector_layout->addWidget(new QLabel(tr("Resolution (DPI)")), 1, 0);
	d_resolution = new QSpinBox();
	d_resolution->setRange(0, 1000);
	d_resolution->setValue(app->d_export_resolution);
	vector_layout->addWidget(d_resolution, 1, 1);

	d_color = new QCheckBox();
	d_color->setText(tr("&Export in &color"));
	d_color->setChecked(app->d_export_color);
	vector_layout->addWidget(d_color, 2, 0, 1, 2);

    d_standard_page = new QCheckBox();
	d_standard_page->setText(tr("Export to &standard page size"));
	vector_layout->addWidget(d_standard_page, 3, 0, 1, 2);

	boxPageSize = new QComboBox();
	boxPageSize->addItem("A0 - 841 x 1189 mm");
	boxPageSize->addItem("A1 - 594 x 841 mm");
	boxPageSize->addItem("A2 - 420 x 594 mm");
	boxPageSize->addItem("A3 - 297 x 420 mm");
	boxPageSize->addItem("A4 - 210 x 297 mm");
	boxPageSize->addItem("A5 - 148 x 210 mm");
	boxPageSize->addItem("A6 - 105 x 148 mm");
	boxPageSize->addItem("A7 - 74 x 105 mm");
	boxPageSize->addItem("A8 - 52 x 74 mm");
	boxPageSize->addItem("A9 - 37 x 52 mm");
	boxPageSize->addItem("B0 - 1030 x 1456 mm");
	boxPageSize->addItem("B1 - 728 x 1030 mm");
	boxPageSize->addItem("B2 - 515 x 728 mm");
	boxPageSize->addItem("B3 - 364 x 515 mm");
	boxPageSize->addItem("B4 - 257 x 364 mm");
	boxPageSize->addItem("B5 - 182 x 257 mm");
	boxPageSize->addItem("B6 - 128 x 182 mm");
	boxPageSize->addItem("B7 - 91 x 128 mm");
	boxPageSize->addItem("B8 - 64 x 91 mm");
	boxPageSize->addItem("B9 - 45 x 64 mm");

    setPageSize(app->d_export_vector_size);
	d_standard_page->setChecked(app->d_export_vector_size != QPrinter::Custom);
	boxPageSize->setEnabled(app->d_export_vector_size != QPrinter::Custom);
	vector_layout->addWidget(boxPageSize, 3, 1, 1, 2);

    connect(d_standard_page, SIGNAL(toggled(bool)), boxPageSize, SLOT(setEnabled(bool)));

	d_keep_aspect = new QCheckBox();
	d_keep_aspect->setText(tr("&Keep aspect ratio"));
	d_keep_aspect->setChecked(app->d_keep_plot_aspect);
	vector_layout->addWidget(d_keep_aspect, 4, 0, 1, 2);

	d_raster_options = new QGroupBox();
	QGridLayout *raster_layout = new QGridLayout(d_raster_options);
	d_advanced_options->addWidget(d_raster_options);

	raster_layout->addWidget(new QLabel(tr("Image quality")), 1, 0);
	d_quality = new QSpinBox();
	d_quality->setRange(1, 100);
	d_quality->setValue(app->d_export_quality);
	raster_layout->addWidget(d_quality, 1, 1);

	d_transparency = new QCheckBox();
	d_transparency->setText(tr("Save transparency"));
	d_transparency->setChecked(app->d_export_transparency);
	raster_layout->addWidget(d_transparency, 2, 0, 1, 2);
}

void ImageExportDialog::updateAdvancedOptions (const QString & filter)
{
	if (filter.contains("*.svg")) {
		d_extension_toggle->setChecked(false);
		d_extension_toggle->setEnabled(false);
		return;
	}
	d_extension_toggle->setEnabled(true);
	if (filter.contains("*.eps") || filter.contains("*.ps") || filter.contains("*.pdf"))
		d_advanced_options->setCurrentIndex(0);
	else {
		d_advanced_options->setCurrentIndex(1);
		d_transparency->setEnabled(filter.contains("*.tif") || filter.contains("*.tiff") || filter.contains("*.png") || filter.contains("*.xpm"));
	}
}

void ImageExportDialog::closeEvent(QCloseEvent* e)
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (app){
		app->d_extended_export_dialog = this->isExtended();
		app->d_image_export_filter = this->selectedFilter();
		app->d_export_transparency = d_transparency->isChecked();
        app->d_export_quality = d_quality->value();

        app->d_export_resolution = d_resolution->value();
        app->d_export_color = d_color->isChecked();
        app->d_export_vector_size = (int)pageSize();
        app->d_keep_plot_aspect = d_keep_aspect->isChecked();
	}

	e->accept();
}

QPrinter::PageSize ImageExportDialog::pageSize() const
{
if (!d_standard_page->isChecked())
    return QPrinter::Custom;

QPrinter::PageSize size;
switch (boxPageSize->currentIndex())
	{
	case 0:
		size = QPrinter::A0;
	break;

	case 1:
		size = QPrinter::A1;
	break;

	case 2:
		size = QPrinter::A2;
	break;

	case 3:
		size = QPrinter::A3;
	break;

	case 4:
		size = QPrinter::A4;
	break;

	case 5:
		size = QPrinter::A5;
	break;

	case 6:
		size = QPrinter::A6;
	break;

	case 7:
		size = QPrinter::A7;
	break;

	case 8:
		size = QPrinter::A8;
	break;

	case 9:
		size = QPrinter::A9;
	break;

	case 10:
		size = QPrinter::B0;
	break;

	case 11:
		size = QPrinter::B1;
	break;

	case 12:
		size = QPrinter::B2;
	break;

	case 13:
		size = QPrinter::B3;
	break;

	case 14:
		size = QPrinter::B4;
	break;

	case 15:
		size = QPrinter::B5;
	break;

	case 16:
		size = QPrinter::B6;
	break;

	case 17:
		size = QPrinter::B7;
	break;

	case 18:
		size = QPrinter::B8;
	break;

	case 19:
		size = QPrinter::B9;
	break;
	}
return size;
}

void ImageExportDialog::setPageSize(int size)
{
if (size == QPrinter::Custom)
    return;
if (!size)
    boxPageSize->setCurrentIndex(4);
else if (size == 1)
    boxPageSize->setCurrentIndex(15);
else if (size >= 5 && size <= 8)
    boxPageSize->setCurrentIndex(size - 5);
else if (size > 8 && size <= 23)
    boxPageSize->setCurrentIndex(size - 4);
}
