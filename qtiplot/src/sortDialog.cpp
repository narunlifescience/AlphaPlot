/***************************************************************************
    File                 : SortDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Sorting options dialog
                           
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
#include "sortDialog.h"

#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

SortDialog::SortDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	setWindowTitle(tr("QtiPlot - Sorting Options"));
	
	groupBox1 = new QGroupBox();
	QGridLayout * layoutTop = new QGridLayout();
	QHBoxLayout * layoutBottom = new QHBoxLayout();

	layoutTop->addWidget( new QLabel(tr("Sort columns")), 0, 0 );
	boxType = new QComboBox();
	layoutTop->addWidget(boxType, 0, 1 );
	
	layoutTop->addWidget( new QLabel( tr("Order")), 1, 0 );
	boxOrder = new QComboBox();
	layoutTop->addWidget(boxOrder, 1, 1 );
	
	layoutTop->addWidget( new QLabel(tr("Leading column")), 2, 0 );
	columnsList = new QComboBox();
	columnsList->setEnabled(false);
	layoutTop->addWidget(columnsList, 2, 1);
	
	buttonOk = new QPushButton();
    buttonOk->setDefault( true );
	layoutBottom->addWidget(buttonOk);
   
    buttonCancel = new QPushButton();    
	layoutBottom->addWidget(buttonCancel);
    
	QVBoxLayout * mainlayout = new QVBoxLayout(this);
    mainlayout->addLayout(layoutTop);
	mainlayout->addLayout(layoutBottom);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( boxType, SIGNAL( activated(int) ), this, SLOT(changeType(int)));
}

SortDialog::~SortDialog()
{
}


void SortDialog::languageChange()
{
    buttonOk->setText( tr("&OK") );
	buttonCancel->setText( tr("&Cancel") );
	
	boxType->clear();
	boxType->addItem(tr("Separately"));
	boxType->addItem(tr("Together"));

	boxOrder->clear();
	boxOrder->addItem(tr("Ascending"));
	boxOrder->addItem(tr("Descending"));
}

void SortDialog::accept()
{
	emit sort(boxType->currentIndex(),boxOrder->currentIndex(),columnsList->currentText());
	close();
}

void SortDialog::insertColumnsList(const QStringList& cols)
{
	columnsList->addItems(cols);
	columnsList->setCurrentIndex(0);
}

void SortDialog::changeType(int Type)
{
	boxType->setCurrentIndex(Type);
	if(Type==1)
		columnsList->setEnabled(true);
	else
		columnsList->setEnabled(false);	
}
