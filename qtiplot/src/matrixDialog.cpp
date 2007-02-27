/***************************************************************************
    File                 : matrixDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Matrix properties dialog
                           
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
#include "matrixDialog.h"

#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLayout>
#include <QGroupBox>
#include <QSpinBox>


MatrixDialog::MatrixDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	QGridLayout * topLayout = new QGridLayout();
	QHBoxLayout * bottomLayout = new QHBoxLayout();

	topLayout->addWidget( new QLabel(tr( "Cell Width" )), 0, 0 );
	boxColWidth = new QSpinBox();
	boxColWidth->setRange(0,1000);
	boxColWidth->setSingleStep(10);
	topLayout->addWidget( boxColWidth, 0, 1 );

	topLayout->addWidget( new QLabel(tr( "Data Format" )), 1, 0 );
	boxFormat = new QComboBox();
	topLayout->addWidget( boxFormat, 1, 1 );

	topLayout->addWidget( new QLabel( tr( "Numeric Display" )), 2, 0 );
	boxNumericDisplay = new QComboBox();
	topLayout->addWidget( boxNumericDisplay, 2, 1 );
	boxPrecision = new QSpinBox();
	boxPrecision->setRange(0,100);
	boxPrecision->setEnabled( false );
	topLayout->addWidget( boxPrecision, 2, 2 );

	buttonApply = new QPushButton();
	buttonApply->setAutoDefault( true );
	bottomLayout->addWidget( buttonApply );

	buttonOk = new QPushButton();
	buttonOk->setAutoDefault( true );
	buttonOk->setDefault( true );
	bottomLayout->addWidget( buttonOk );

	buttonCancel = new QPushButton();
	buttonCancel->setAutoDefault( true );
	bottomLayout->addWidget( buttonCancel );

	QVBoxLayout * mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(bottomLayout);

	languageChange();

	// signals and slots connections
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT( apply() ) );
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( boxNumericDisplay, SIGNAL( activated(int) ), this, SLOT( showPrecisionBox(int) ) );
	connect( boxColWidth, SIGNAL( valueChanged(int) ), this, SIGNAL( changeColumnsWidth(int) ) );
	connect( boxPrecision, SIGNAL( valueChanged(int) ), this, SLOT( changePrecision(int) ) );
}

MatrixDialog::~MatrixDialog()
{
}

void MatrixDialog::changePrecision(int precision)
{
	if (boxFormat->currentItem())
		emit changeTextFormat('e', precision);
	else
		emit changeTextFormat('f', precision);
}

void MatrixDialog::setTextFormat(const QString& format, int precision)
{
	if (format == "f")
		boxFormat->setCurrentItem(0);
	else
		boxFormat->setCurrentItem(1);

	boxPrecision->setValue(precision);
	if (precision != 6)
	{
		boxPrecision->setEnabled( true );
		boxNumericDisplay->setCurrentItem(1);
	}
}

void MatrixDialog::showPrecisionBox(int item)
{
	if (item)
		boxPrecision->setEnabled( true );
	else
	{
		boxPrecision->setValue(6);
		boxPrecision->setEnabled( false );
	}
}

void MatrixDialog::setColumnsWidth(int width)
{
	boxColWidth->setValue(width);
}

void MatrixDialog::languageChange()
{
	setWindowTitle( tr( "QtiPlot - Matrix Properties" ) );
	buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
	buttonApply->setText( tr( "&Apply" ) );

	boxFormat->clear();
	boxFormat->insertItem( tr( "Decimal: 1000" ) );
	boxFormat->insertItem( tr( "Scientific: 1E3" ) );

	boxNumericDisplay->clear();
	boxNumericDisplay->insertItem( tr( "Default Decimal Digits" ) );
	boxNumericDisplay->insertItem( tr( "Significant Digits=" ) );
}

void MatrixDialog::apply()
{
	emit changeColumnsWidth(boxColWidth->value());
	if (boxFormat->currentItem())
		emit changeTextFormat('e', boxPrecision->value());
	else
		emit changeTextFormat('f', boxPrecision->value());
}

void MatrixDialog::accept()
{
	apply();
	close();
}
