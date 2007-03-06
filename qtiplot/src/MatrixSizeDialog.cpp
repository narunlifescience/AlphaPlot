/***************************************************************************
    File                 : MatrixSizeDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Matrix dimensions dialog
                           
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
#include "MatrixSizeDialog.h"
#include "MyParser.h"

#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

MatrixSizeDialog::MatrixSizeDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	groupBox1 = new QGroupBox(tr("Dimensions"));
	groupBox2 = new QGroupBox(tr("Coordinates"));
	QHBoxLayout *topLayout = new QHBoxLayout(groupBox1);
	QGridLayout *centerLayout = new QGridLayout(groupBox2);
	QHBoxLayout *bottomLayout = new QHBoxLayout();

	topLayout->addWidget( new QLabel(tr( "Rows" )) );
    boxRows = new QSpinBox();
	boxRows->setRange(0,1000000);
	topLayout->addWidget(boxRows);

	topLayout->addWidget( new QLabel(tr( "Columns" )) );
    boxCols = new QSpinBox();
	boxCols->setRange(0,1000000);
	topLayout->addWidget(boxCols);

	centerLayout->addWidget( new QLabel(tr( "X (Columns)" )), 0, 1 );
	centerLayout->addWidget( new QLabel(tr( "Y (Rows)" )), 0, 2 );

	centerLayout->addWidget( new QLabel(tr( "First" )), 1, 0 );
    boxXStart = new QLineEdit();
    boxYStart = new QLineEdit();
	centerLayout->addWidget( boxXStart, 1, 1 );
	centerLayout->addWidget( boxYStart, 1, 2 );

	centerLayout->addWidget( new QLabel(tr( "Last" )), 2, 0 );
    boxXEnd = new QLineEdit();
    boxYEnd = new QLineEdit();
	centerLayout->addWidget( boxXEnd, 2, 1 );
	centerLayout->addWidget( boxYEnd, 2, 2 );

	buttonOk = new QPushButton();
    buttonOk->setDefault( true );
	bottomLayout->addWidget( buttonOk );
    buttonCancel = new QPushButton();
	bottomLayout->addWidget( buttonCancel );
	
	QVBoxLayout * mainLayout = new QVBoxLayout( this );
    mainLayout->addWidget(groupBox1);
	mainLayout->addWidget(groupBox2);
	mainLayout->addLayout(bottomLayout);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

MatrixSizeDialog::~MatrixSizeDialog()
{
}

void MatrixSizeDialog::setColumns(int c)
{
	boxCols->setValue(c);
}

void MatrixSizeDialog::setRows(int r)
{
	boxRows->setValue(r);
}

void MatrixSizeDialog::setCoordinates(double xs, double xe, double ys, double ye)
{
	boxXStart->setText(QString::number(xs, 'g', 6));
	boxYStart->setText(QString::number(ys, 'g', 6));
	boxXEnd->setText(QString::number(xe, 'g', 6));
	boxYEnd->setText(QString::number(ye, 'g', 6));
}

void MatrixSizeDialog::languageChange()
{
	setWindowTitle(tr("QtiPlot - Matrix Dimensions"));
	buttonOk->setText(tr("&OK"));
	buttonCancel->setText(tr("&Cancel"));
}

void MatrixSizeDialog::accept()
{
	double fromX, toX, fromY, toY;
	MyParser parser;	
	try
	{
		parser.SetExpr(boxXStart->text().lower().ascii());
		fromX=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("QtiPlot - Input error"), QString::fromStdString(e.GetMsg()));
		boxXStart->setFocus();
		return;
	}
	try
	{
		parser.SetExpr(boxXEnd->text().lower().ascii());
		toX=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("QtiPlot - Input error"), QString::fromStdString(e.GetMsg()));
		boxXEnd->setFocus();
		return;
	}
	try
	{
		parser.SetExpr(boxYStart->text().lower().ascii());
		fromY=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("QtiPlot - Input error"), QString::fromStdString(e.GetMsg()));
		boxYStart->setFocus();
		return;
	}
	try
	{
		parser.SetExpr(boxYEnd->text().lower().ascii());
		toY=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("QtiPlot - Input error"), QString::fromStdString(e.GetMsg()));
		boxYEnd->setFocus();
		return;
	}

	emit changeDimensions(boxRows->value(), boxCols->value());
	emit changeCoordinates(fromX, toX, fromY, toY);
	close();
}
