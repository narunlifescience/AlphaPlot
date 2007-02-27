/***************************************************************************
    File                 : dataSetDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Multi purpose dialog for choosing a data set
                           
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
#include "dataSetDialog.h"

#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

DataSetDialog::DataSetDialog( const QString& text, QWidget* parent,  Qt::WFlags fl )
: QDialog( parent, fl )
{
	setWindowTitle(tr("QtiPlot - Select data set"));

	operation = QString();

	QVBoxLayout * mainLayout = new QVBoxLayout( this );
	QHBoxLayout * bottomLayout = new QHBoxLayout();

	groupBox1 = new QGroupBox();
	QHBoxLayout * topLayout = new QHBoxLayout( groupBox1 );

	topLayout->addWidget( new QLabel(text) );
	boxName = new QComboBox();
	topLayout->addWidget(boxName);

	buttonOk = new QPushButton();
	buttonOk->setAutoDefault( true );
	buttonOk->setDefault( true );
	bottomLayout->addWidget( buttonOk );

	buttonCancel = new QPushButton();
	buttonCancel->setAutoDefault( true );
	bottomLayout->addWidget( buttonCancel );

	mainLayout->addWidget( groupBox1 );
	mainLayout->addLayout( bottomLayout );

	languageChange();

	// signals and slots connections
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

DataSetDialog::~DataSetDialog()
{
}

void DataSetDialog::languageChange()
{
	buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
}

void DataSetDialog::accept()
{
	if (operation.isEmpty())
		emit options(boxName->currentText());
	else
		emit analyze(operation, boxName->currentText());
	close();
}

void DataSetDialog::setCurveNames(const QStringList& names)
{
	boxName->insertStringList (names,-1);
}
