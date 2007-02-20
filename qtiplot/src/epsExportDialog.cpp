/***************************************************************************
    File                 : epsExportDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#include "epsExportDialog.h"

#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QImage>
#include <QPrinter>
#include <QHBoxLayout>
#include <QVBoxLayout>

EpsExportDialog::EpsExportDialog(const QString& fileName, QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	f_name = fileName;

	groupBox1 = new QGroupBox();
	QGridLayout * leftLayout = new QGridLayout( groupBox1 );

	leftLayout->addWidget( new QLabel(tr( "Orientation" )), 0, 0 );
	boxOrientation= new QComboBox();
	leftLayout->addWidget( boxOrientation, 0, 1 );

	leftLayout->addWidget( new QLabel(tr("Page Size")), 1, 0 );
	boxPageSize= new QComboBox();
	leftLayout->addWidget( boxPageSize, 1, 1 );

	leftLayout->addWidget( new QLabel(tr("Resolution (DPI)")), 2, 0 );
	boxResolution= new QSpinBox();
	boxResolution->setRange(0, 1000);
	boxResolution->setValue(QPrinter().resolution());
	leftLayout->addWidget( boxResolution, 2, 1 );

	boxColor= new QCheckBox();
	boxColor->setText( tr("&Print in color if available") );
	boxColor->setChecked(true);
	leftLayout->addWidget( boxColor, 3, 0, 1, 2 );

	QVBoxLayout * rightLayout = new QVBoxLayout();
	
	buttonOk = new QPushButton();
	buttonOk->setAutoDefault( true );
	buttonOk->setDefault( true );
	rightLayout->addWidget( buttonOk );

	buttonCancel = new QPushButton();
	buttonCancel->setAutoDefault( true );
	rightLayout->addWidget( buttonCancel );
	rightLayout->addStretch();

	QHBoxLayout * mainLayout = new QHBoxLayout( this );
	mainLayout->addWidget( groupBox1 );
	mainLayout->addLayout( rightLayout );

	languageChange();

	// signals and slots connections
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void EpsExportDialog::languageChange()
{
	setWindowTitle( tr( "QtiPlot - Export options" ) );
	buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );

	boxOrientation->clear();
	boxOrientation->addItem(tr("Landscape"));
	boxOrientation->addItem(tr("Portrait"));

	boxPageSize->clear();
	boxPageSize->addItem("A0 - 841 x 1189 mm");
	boxPageSize->addItem("A1 - 594 x 841 mm");
	boxPageSize->addItem("A2 - 420 x 594 mm");
	boxPageSize->addItem("A3 - 297 x 420 mm");
	boxPageSize->addItem("A4 - 210 x 297 mm, 8.26 x 11.69 inches");
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
	boxPageSize->addItem("B5 - 182 x 257 mm, 7.17 x 10.13 inches");
	boxPageSize->addItem("B6 - 128 x 182 mm");
	boxPageSize->addItem("B7 - 91 x 128 mm");
	boxPageSize->addItem("B8 - 64 x 91 mm");
	boxPageSize->addItem("B9 - 45 x 64 mm");
	boxPageSize->addItem("B10 - 32 x 45 mm");
	boxPageSize->addItem("U.S. Common 10 Envelope - 105 x 241 mm");
	boxPageSize->addItem("Executive - 7.5 x 10 inches, 191 x 254 mm");
	boxPageSize->addItem("Legal - 8.5 x 14 inches, 216 x 356 mm");
	boxPageSize->addItem("Letter - 8.5 x 11 inches, 216 x 279 mm");

	boxPageSize->setCurrentIndex(5); // A5
}

void EpsExportDialog::accept()
{
	QPrinter::Orientation o;
	if (boxOrientation->currentItem() == 1)
		o = QPrinter::Portrait;
	else
		o = QPrinter::Landscape;

	QPrinter::ColorMode col = QPrinter::Color;
	if (!boxColor->isChecked())
		col = QPrinter::GrayScale;

	QPrinter::PageSize size = pageSize();

	emit exportVector(f_name, boxResolution->value(), o, size, col);
	close();
}

QPrinter::PageSize EpsExportDialog::pageSize()
{
	QPrinter::PageSize size;
	switch (boxPageSize->currentItem())
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
		case 20:
			size = QPrinter::B10;
			break;
		case 21:
			size = QPrinter::Comm10E;
			break;
		case 22:
			size = QPrinter::Executive;
			break;
		case 23:
			size = QPrinter::Legal;
			break;
		case 24:
			size = QPrinter::Letter;
			break;
	}
	return size;
}

EpsExportDialog::~EpsExportDialog()
{
}
