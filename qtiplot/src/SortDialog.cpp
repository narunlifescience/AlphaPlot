/***************************************************************************
    File                 : SortDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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
#include "SortDialog.h"

#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <QApplication>

SortDialog::SortDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
    setWindowIcon(qApp->windowIcon());
	setWindowTitle(tr("QtiPlot - Sorting Options"));
	setSizeGripEnabled(true);
	
	QGroupBox *groupBox1 = new QGroupBox();
	QGridLayout * topLayout = new QGridLayout(groupBox1);
	QHBoxLayout * hl = new QHBoxLayout();
	hl->addStretch();
	
	topLayout->addWidget( new QLabel(tr("Sort columns")), 0, 0 );
	boxType = new QComboBox();
	topLayout->addWidget(boxType, 0, 1 );
	
	topLayout->addWidget( new QLabel( tr("Order")), 1, 0 );
	boxOrder = new QComboBox();
	topLayout->addWidget(boxOrder, 1, 1 );
	
	topLayout->addWidget( new QLabel(tr("Leading column")), 2, 0 );
	columnsList = new QComboBox();
	topLayout->addWidget(columnsList, 2, 1);
	topLayout->setRowStretch(3, 1);
	
	buttonOk = new QPushButton();
    buttonOk->setDefault( true );
	hl->addWidget(buttonOk);
   
    buttonCancel = new QPushButton();    
	hl->addWidget(buttonCancel);
    
	QVBoxLayout * mainlayout = new QVBoxLayout(this);
    mainlayout->addWidget(groupBox1);
	mainlayout->addLayout(hl);
	
    languageChange();

    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( boxType, SIGNAL( activated(int) ), this, SLOT(changeType(int)));
}

void SortDialog::languageChange()
{
    buttonOk->setText( tr("&Sort") );
	buttonCancel->setText( tr("&Close") );
	
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
}

void SortDialog::insertColumnsList(const QStringList& cols)
{
	columnsList->addItems(cols);
	columnsList->setCurrentIndex(0);
	
	boxType->setCurrentIndex(1);
}

void SortDialog::changeType(int Type)
{
	boxType->setCurrentIndex(Type);
	if(Type==1)
		columnsList->setEnabled(true);
	else
		columnsList->setEnabled(false);	
}
