/***************************************************************************
    File                 : errDlg.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Add error bars dialog
                           
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
#include "errDlg.h"
#include "worksheet.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QList>
#include <QLabel>
#include <QComboBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QButtonGroup>
#include <QList>
#include <QWidget>


ErrDialog::ErrDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
    setFocusPolicy( Qt::StrongFocus );
    setSizeGripEnabled( true );

	QVBoxLayout *vbox1 = new QVBoxLayout();
	vbox1->setSpacing (5);
	
	QHBoxLayout *hbox1 = new QHBoxLayout();
	vbox1->addLayout(hbox1);

    textLabel1 = new QLabel();
	hbox1->addWidget(textLabel1);

    nameLabel = new QComboBox();
	hbox1->addWidget(nameLabel);

    groupBox1 = new QGroupBox(QString(tr("Source of errors")));
	QGridLayout * gridLayout = new QGridLayout(groupBox1);
	vbox1->addWidget(groupBox1);

	buttonGroup1 = new QButtonGroup();
	buttonGroup1->setExclusive( true );
	
	columnBox = new QRadioButton();
    columnBox->setChecked( true );
	buttonGroup1->addButton(columnBox);
	gridLayout->addWidget(columnBox, 0, 0 );
	
	colNamesBox = new QComboBox();
    tableNamesBox = new QComboBox();
	
    QHBoxLayout * comboBoxes = new QHBoxLayout();
	comboBoxes->addWidget(tableNamesBox);
	comboBoxes->addWidget(colNamesBox);

	gridLayout->addLayout(comboBoxes, 0, 1);

    percentBox = new QRadioButton();
	buttonGroup1->addButton(percentBox);
	gridLayout->addWidget(percentBox, 1, 0 );

    valueBox = new QLineEdit();
    valueBox->setAlignment( Qt::AlignHCenter );
	valueBox->setEnabled(false);
	gridLayout->addWidget(valueBox, 1, 1);
	
	standardBox = new QRadioButton();
	buttonGroup1->addButton(standardBox);
	gridLayout->addWidget(standardBox, 2, 0 );

	groupBox3 = new QGroupBox(QString());
	vbox1->addWidget(groupBox3);
	QHBoxLayout * hbox2 = new QHBoxLayout(groupBox3);

	buttonGroup2 = new QButtonGroup();
	buttonGroup2->setExclusive( true );
	
    xErrBox = new QRadioButton();
	buttonGroup2->addButton(xErrBox);
	hbox2->addWidget(xErrBox );

    yErrBox = new QRadioButton();
	buttonGroup2->addButton(yErrBox);
	hbox2->addWidget(yErrBox );
    yErrBox->setChecked( true );
	
	QVBoxLayout * vbox2 = new QVBoxLayout();
	buttonAdd = new QPushButton();
    buttonAdd->setDefault( true );
	vbox2->addWidget(buttonAdd);

    buttonCancel = new QPushButton();
	vbox2->addWidget(buttonCancel);

	vbox2->addStretch(1);
	
	QHBoxLayout * hlayout1 = new QHBoxLayout(this);
	hlayout1->addLayout(vbox1);
    hlayout1->addLayout(vbox2);
	
    languageChange();

  // signals and slots connections
	connect( buttonAdd, SIGNAL( clicked() ), this, SLOT( add() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( percentBox, SIGNAL( toggled(bool) ), valueBox, SLOT( setEnabled(bool) ) );
	connect( columnBox, SIGNAL( toggled(bool) ), tableNamesBox, SLOT( setEnabled(bool) ) );
 	connect( columnBox, SIGNAL( toggled(bool) ), colNamesBox, SLOT( setEnabled(bool) ) );
	connect( tableNamesBox, SIGNAL( activated(int) ), this, SLOT( selectSrcTable(int) ));
}

void ErrDialog::setCurveNames(const QStringList& names)
{
	nameLabel->addItems(names);
}

void ErrDialog::setSrcTables(QWidgetList* tables)
{
	srcTables = tables;
	tableNamesBox->clear();

	QList<QWidget *>::const_iterator i;
    for (i = srcTables->begin(); i != srcTables->end(); i++)
		tableNamesBox->insertItem((*i)->name());

	if (!nameLabel->currentText().contains("="))
		tableNamesBox->setCurrentIndex(tableNamesBox->findText(nameLabel->currentText().split("_", QString::SkipEmptyParts)[0]));
	selectSrcTable(tableNamesBox->currentIndex());
}

void ErrDialog::selectSrcTable(int tabnr)
{
	colNamesBox->clear();
	colNamesBox->addItems(((Table*)srcTables->at(tabnr))->colNames());
}

void ErrDialog::add()
{
	int direction=-1;
	if (xErrBox->isChecked()) 
		direction = 0; 
	else 
		direction = 1;

	if (columnBox->isChecked())
		emit options(nameLabel->currentText(), tableNamesBox->currentText()+"_"+colNamesBox->currentText(), direction);	
	else
	{
		int type;
		if (percentBox->isChecked()) 
			type = 0; 
		else 
			type = 1;

		emit options(nameLabel->currentText(),type, valueBox->text(), direction);
	}
}

ErrDialog::~ErrDialog()
{
}

void ErrDialog::languageChange()
{
    setWindowTitle( tr( "QtiPlot - Error Bars" ) );
    xErrBox->setText( tr( "&X Error Bars" ) );
	buttonAdd->setText( tr( "&Add" ) );
    textLabel1->setText( tr( "Add Error Bars to" ) );
    groupBox1->setTitle( tr( "Source of errors" ) );
    percentBox->setText( tr( "Percent of data (%)" ) );
    valueBox->setText( tr( "5" ) );
    standardBox->setText( tr( "Standard Deviation of Data" ) );
    yErrBox->setText( tr( "&Y Error Bars" ) );
    buttonCancel->setText( tr( "&Close" ) );
	columnBox->setText("Existing column");
}
