/***************************************************************************
    File                 : sortDialog.cpp
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

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HButtonGroup>

sortDialog::sortDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "sortDialog" );
	setWindowTitle(tr("QtiPlot - Sorting Options"));
    setMinimumSize( QSize( 310, 140 ) );
	setMaximumSize( QSize( 310, 140 ) );
    setMouseTracking( true );
    setSizeGripEnabled( false );
	
	GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal, QString(),this,"GroupBox1" );

	new QLabel( tr("Sort columns"), GroupBox1, "TextLabel1",0 );
	boxType = new QComboBox(GroupBox1, "boxShow" );
	
	new QLabel( tr("Order"), GroupBox1, "TextLabel2",0 );
	boxOrder = new QComboBox(GroupBox1, "boxOrder" );
	
	new QLabel(tr("Leading column"),GroupBox1, "TextLabel3",0);
	columnsList = new QComboBox(GroupBox1, "listBox" );
	columnsList->setEnabled(false);	
	
	GroupBox2 = new Q3HButtonGroup(this,"GroupBox2" );
	GroupBox2->setFlat (true);
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setDefault( true );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );    
    
	Q3VBoxLayout* vlayout = new Q3VBoxLayout(this,5,5, "vlayout");
    vlayout->addWidget(GroupBox1);
	vlayout->addWidget(GroupBox2);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( boxType, SIGNAL( activated(int) ), this, SLOT(changeType(int)));
}

sortDialog::~sortDialog()
{
}


void sortDialog::languageChange()
{
    buttonOk->setText( tr("&OK") );
	buttonCancel->setText( tr("&Cancel") );
	
	boxType->insertItem(tr("Separately"));
	boxType->insertItem(tr("Together"));

	boxOrder->insertItem(tr("Ascending"));
	boxOrder->insertItem(tr("Descending"));
}

void sortDialog::accept()
{
emit sort(boxType->currentItem(),boxOrder->currentItem(),columnsList->currentText());
close();
}

void sortDialog::insertColumnsList(const QStringList& cols)
{
int i,n=cols.count();
for (i=0;i<n;i++)
	columnsList->insertItem(cols[i],i);

columnsList->setCurrentItem(0);
}

void sortDialog::changeType(int Type)
{
boxType->setCurrentItem(Type);
if(Type==1)
	columnsList->setEnabled(true);
else
	columnsList->setEnabled(false);	
}
