/***************************************************************************
    File                 : analysisDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Analysis options dialog
                           
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
#include "analysisDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HButtonGroup>

analysisDialog::analysisDialog( QWidget* parent, const QString& text, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "analysisDialog" );
	setWindowTitle(tr("QtiPlot - Analysis Options"));
	
	operation = QString();

	GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),this,"GroupBox1" );

	new QLabel( tr(text), GroupBox1, "TextLabel1",0 );
	boxName = new QComboBox(GroupBox1, "boxShow" );
	
	GroupBox2 = new Q3HButtonGroup(this,"GroupBox2" );
	GroupBox2->setFlat (true);
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setAutoDefault( true );
    buttonOk->setDefault( true );
    
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( true );
	
    Q3VBoxLayout* vlayout = new Q3VBoxLayout(this,5,5, "vlayout");
	vlayout->addWidget(GroupBox1);
	vlayout->addWidget(GroupBox2);
	
    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

analysisDialog::~analysisDialog()
{
}

void analysisDialog::languageChange()
{
    buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
}

void analysisDialog::accept()
{
if (operation.isEmpty())
	emit options(boxName->currentText());
else
	emit analyse(operation, boxName->currentText());
close();
}

void analysisDialog::setCurveNames(const QStringList& names)
{
boxName->insertStringList (names,-1);
}
