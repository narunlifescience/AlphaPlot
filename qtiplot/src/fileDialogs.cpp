/***************************************************************************
    File                 : fileDialogs.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : 2 File dialogs: Import multiple ASCII/Export image
                           
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

#include "fileDialogs.h"

/*****************************************************************************
 *
 * Class ImportFilesDialog
 *
 *****************************************************************************/

ImportFilesDialog::ImportFilesDialog(bool importTypeEnabled, QWidget * parent, Qt::WFlags flags ) 
: QFileDialog( parent, flags )
{
	setWindowTitle(tr("QtiPlot - Import Multiple ASCII Files"));

	QStringList filters;
	filters << tr("All files") + " (*)";
	filters << tr("Text files") + " (*.TXT *.txt)";
	filters << tr("Data files") + " (*.DAT *.dat)";
	filters << tr("Comma Separated Values") + " (*.CSV *.csv)";
	setFilters( filters );

	setFileMode( QFileDialog::ExistingFiles );

	if (importTypeEnabled)
	{
		QLabel* label = new QLabel( tr("Import each file as: ") );

		importType = new QComboBox();
		importType->addItem(tr("New Table"));
		importType->addItem(tr("New Columns"));
		importType->addItem(tr("New Rows"));

		// FIXME: The following code may not work anymore
		// if the internal layout of QFileDialog changes
		layout()->addWidget( label );
		layout()->addWidget( importType );
	}
}

int ImportFilesDialog::importFileAs()
{
	return importType->currentIndex();
}


/*****************************************************************************
 *
 * Class ImageExportDialog
 *
 *****************************************************************************/

ImageExportDialog::ImageExportDialog( QWidget * parent, Qt::WFlags flags)
: QFileDialog( parent, flags )
{
	setWindowTitle( tr( "QtiPlot - Choose a filename to save under" ) );

	QList<QByteArray> list = QImageWriter::supportedImageFormats();
	list<<"EPS";
    list<<"PDF";
	list<<"SVG";

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
}

void ImageExportDialog::showOptionsBox ( const QString & filter)
{
	if (filter.contains("svg"))
		boxOptions->hide();
	else
		boxOptions->show();
}

