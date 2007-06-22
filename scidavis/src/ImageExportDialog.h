/***************************************************************************
    File                 : ImageExportDialog.h
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
#ifndef IMAGEEXPORTDIALOG_H
#define IMAGEEXPORTDIALOG_H

#include "ExtensibleFileDialog.h"

#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPrinter>

class QStackedWidget;
class QGroupBox;

//! QFileDialog extended with options for image export
class ImageExportDialog: public ExtensibleFileDialog
{
	Q_OBJECT

private:
	//! Create #d_advanced_options and everything it contains.
	void initAdvancedOptions();

	//! Container widget for all advanced options.
	QStackedWidget *d_advanced_options;
	// vector format options
	//! Container widget for all options available for vector formats.
	QGroupBox *d_vector_options;
	QSpinBox *d_resolution;
	QCheckBox *d_color;
	QCheckBox *d_keep_aspect;
	QCheckBox *d_standard_page;
	QComboBox *boxPageSize;
	// raster format options
	//! Container widget for all options available for raster formats.
	QGroupBox *d_raster_options;
	QSpinBox *d_quality;
	QCheckBox *d_transparency;

public:
	//! Constructor
	/**
	 * \param parent parent widget
	 * \param vector_options whether advanced options are to be provided for export to vector formats
	 * \param extended flag: show/hide the advanced options on start-up
	 * \param flags window flags
	 */
	ImageExportDialog(QWidget * parent = 0, bool vector_options = true, bool extended = true, Qt::WFlags flags = 0 );
	//! For vector formats: returns the output resolution the user selected, defaulting to the screen resolution.
	int resolution() const { return d_resolution->value(); }
	//! For vector formats: returns whether colors should be enabled for ouput (default: true).
	bool color() const { return d_color->isChecked(); }
	//! For vector formats: returns whether the output should preserve aspect ratio of the plot (default: true).
	bool keepAspect() const { return d_keep_aspect->isChecked(); }
    //! For vector formats: returns a standard output page size (default: QPrinter::Custom).
	QPrinter::PageSize pageSize() const;
	void setPageSize(int size);
	//! Return the quality (in percent) the user selected for export to raster formats.
	int quality() const { return d_quality->value(); }
	//! Return whether the output's background should be transparent.
	bool transparency() const { return d_transparency->isChecked(); }

	void selectFilter(const QString & filter);
	
protected slots:
	void closeEvent(QCloseEvent*);
	//! Update which options are visible and enabled based on the output format.
	void updateAdvancedOptions (const QString &filter);
};

#endif
