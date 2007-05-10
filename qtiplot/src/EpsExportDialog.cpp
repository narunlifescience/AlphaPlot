/***************************************************************************
    File                 : EpsExportDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : EPS export dialog
                           
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
#include "EpsExportDialog.h"

#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QLayout>
#include <QPrinter>

EpsExportDialog::EpsExportDialog(const QString& fileName, QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	setWindowTitle( tr( "QtiPlot - Export options" ) );
	
	f_name = fileName;

	QGroupBox *groupBox1 = new QGroupBox();
	QGridLayout * leftLayout = new QGridLayout( groupBox1 );

	leftLayout->addWidget( new QLabel(tr("Resolution (DPI)")), 1, 0 );
	boxResolution= new QSpinBox();
	boxResolution->setRange(0, 1000);
	boxResolution->setValue(QPrinter().resolution());
	leftLayout->addWidget( boxResolution, 1, 1 );

	boxColor= new QCheckBox();
	boxColor->setText( tr("&Print in color if available") );
	boxColor->setChecked(true);
	leftLayout->addWidget( boxColor, 2, 0, 1, 2 );
	leftLayout->setRowStretch(3,1);
	
	QVBoxLayout * rightLayout = new QVBoxLayout();
	
	buttonOk = new QPushButton(tr( "&OK" ));
	buttonOk->setAutoDefault( true );
	buttonOk->setDefault( true );
	rightLayout->addWidget( buttonOk );

	buttonCancel = new QPushButton(tr( "&Cancel" ));
	buttonCancel->setAutoDefault( true );
	rightLayout->addWidget( buttonCancel );
	rightLayout->addStretch();

	QHBoxLayout * mainLayout = new QHBoxLayout( this );
	mainLayout->addWidget( groupBox1 );
	mainLayout->addLayout( rightLayout );

	resize(minimumSize());
	
	// signals and slots connections
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void EpsExportDialog::accept()
{
	emit exportVector(f_name, boxResolution->value(), boxColor->isChecked());
	close();
}
