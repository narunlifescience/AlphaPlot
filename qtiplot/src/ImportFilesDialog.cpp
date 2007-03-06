/***************************************************************************
    File                 : ImportFilesDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Import multiple ASCII files dialog
                           
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

#include "ImportFilesDialog.h"

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

