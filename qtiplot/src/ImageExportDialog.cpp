/***************************************************************************
    File                 : ImageExportDialog.cpp
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
#include <QMessageBox>
#include "ImageExportDialog.h"

ImageExportDialog::ImageExportDialog( QWidget * parent, Qt::WFlags flags)
: QFileDialog( parent, flags )
{
	setWindowTitle( tr( "QtiPlot - Choose a filename to save under" ) );
    setAcceptMode(QFileDialog::AcceptSave);

	QList<QByteArray> list = QImageWriter::supportedImageFormats();
	list<<"EPS";
	list<<"PS";
    list<<"PDF";
	//list<<"SVG";

	QStringList filters, selectedFilter;
    for(int i=0 ; i<list.count() ; i++)
	{
		filters << "*."+list[i].toLower();
	}
    filters.sort();
	setFilters(filters);
	setFileMode( QFileDialog::AnyFile );

	boxOptions = new QCheckBox(tr("Show export &options"));
	boxOptions->setChecked( false );

	// FIXME: The following code may not work anymore
	// if the internal layout of QFileDialog changes
	QSpacerItem * si1 = new QSpacerItem( 20, 20 );
	QSpacerItem * si2 = new QSpacerItem( 20, 20 );
	layout()->addItem( si1 );
	layout()->addItem( si2 );
	layout()->addWidget( boxOptions );
	
	//TODO: The following lines must be uncommented when building with Qt 4.3 (ion) 
	//connect(this, SIGNAL(filterSelected ( const QString & )), 
		//this, SLOT(showOptionsBox ( const QString & )));
}

/*void ImageExportDialog::showOptionsBox (const QString & filter)
{
	if (filter.contains("svg"))
		boxOptions->hide();
	else
		boxOptions->show();
}*/
