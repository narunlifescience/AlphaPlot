/***************************************************************************
    File                 : ImageExportDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Export image dialog
                           
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

#include <QFileDialog>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QImage>
#include <QImageWriter>
#include <QPicture>
#include <QHBoxLayout>
#include <QtAlgorithms>

//! Export as image dialog
class ImageExportDialog: public QFileDialog
{
	Q_OBJECT

private:
	QCheckBox* boxOptions;

public:
	//! Constructor
	/**
	 * \param parent parent widget
	 * \param flags window flags
	 */
	ImageExportDialog( QWidget * parent = 0, Qt::WFlags flags = 0 );
	bool showExportOptions(){return boxOptions->isChecked();};

public slots:
	void showOptionsBox ( const QString & filter);
};

#endif
