/***************************************************************************
    File                 : MatrixDialog.cpp
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
#include "MatrixDialog.h"
#include "Matrix.h"

#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLayout>
#include <QGroupBox>
#include <QSpinBox>

MatrixDialog::MatrixDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl ),
    d_matrix(0)
{
    setWindowTitle( tr( "QtiPlot - Matrix Properties" ) );

	QGridLayout * topLayout = new QGridLayout();
	QHBoxLayout * bottomLayout = new QHBoxLayout();

	topLayout->addWidget( new QLabel(tr( "Cell Width" )), 0, 0 );
	boxColWidth = new QSpinBox();
	boxColWidth->setRange(0,1000);
	boxColWidth->setSingleStep(10);
	topLayout->addWidget( boxColWidth, 0, 1 );

	topLayout->addWidget( new QLabel(tr( "Data Format" )), 1, 0 );
	boxFormat = new QComboBox();
    boxFormat->addItem( tr( "Decimal: 1000" ) );
	boxFormat->addItem( tr( "Scientific: 1E3" ) );

	topLayout->addWidget( boxFormat, 1, 1 );

	topLayout->addWidget( new QLabel( tr( "Numeric Display" )), 2, 0 );
	boxNumericDisplay = new QComboBox();
    boxNumericDisplay->addItem( tr( "Default Decimal Digits" ) );
	boxNumericDisplay->addItem( tr( "Significant Digits=" ) );

	topLayout->addWidget( boxNumericDisplay, 2, 1 );
	boxPrecision = new QSpinBox();
	boxPrecision->setRange(0,100);
	boxPrecision->setEnabled( false );
	topLayout->addWidget( boxPrecision, 2, 2 );

	buttonApply = new QPushButton(tr( "&Apply" ));
	buttonApply->setAutoDefault( true );
	bottomLayout->addWidget( buttonApply );

	buttonOk = new QPushButton(tr( "&OK" ));
	buttonOk->setAutoDefault( true );
	buttonOk->setDefault( true );
	bottomLayout->addWidget( buttonOk );

	buttonCancel = new QPushButton(tr( "&Cancel" ));
	buttonCancel->setAutoDefault( true );
	bottomLayout->addWidget( buttonCancel );

	QVBoxLayout * mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(bottomLayout);

	// signals and slots connections
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT( apply() ) );
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( close() ) );
	connect( boxNumericDisplay, SIGNAL( activated(int) ), this, SLOT( showPrecisionBox(int) ) );
	connect( boxPrecision, SIGNAL( valueChanged(int) ), this, SLOT( changePrecision(int) ) );
}

void MatrixDialog::changePrecision(int precision)
{
    if (boxFormat->currentIndex())
		d_matrix->setNumericFormat('e', precision);
	else
		d_matrix->setNumericFormat('f', precision);
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

void MatrixDialog::apply()
{
	d_matrix->setColumnsWidth(boxColWidth->value());
    changePrecision(boxPrecision->value());
}

void MatrixDialog::setMatrix(Matrix *m)
{
    if (!m)
        return;

    d_matrix = m;
    boxColWidth->setValue(m->columnsWidth());

    if (QString(m->textFormat()) == "f")
		boxFormat->setCurrentIndex(0);
	else
		boxFormat->setCurrentIndex(1);

	boxPrecision->setValue(m->precision());
	if (m->precision() != 6)
	{
		boxPrecision->setEnabled( true );
		boxNumericDisplay->setCurrentIndex(1);
	}

    connect(boxColWidth, SIGNAL(valueChanged(int)), d_matrix, SLOT(setColumnsWidth(int)));
    m->saveCellsToMemory();
}

void MatrixDialog::accept()
{
	apply();
	close();
}

void MatrixDialog::closeEvent(QCloseEvent* e)
{
    if (d_matrix)
        d_matrix->forgetSavedCells();
    e->accept();
}
