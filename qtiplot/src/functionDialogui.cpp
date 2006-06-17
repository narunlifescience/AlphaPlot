/***************************************************************************
    File                 : functionDialogui.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Function dialog user interface
                           
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
#include "functionDialogui.h"

#include <qvariant.h>
#include <q3groupbox.h>
#include <qlabel.h>
#include <q3widgetstack.h>
#include <qwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <q3buttongroup.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <q3hbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>

FunctionDialogUi::FunctionDialogUi( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "FunctionDialogUi" );
	 setMinimumSize( QSize( 610, 440 ) );
	setMaximumSize( QSize( 610, 440 ) );
    setMouseTracking( true );

	Q3HBox *hbox1=new Q3HBox(this, "hbox1");
	hbox1->setSpacing(5);
	
	textFunction_2 = new QLabel( hbox1, "textFunction_2" );
	boxType = new QComboBox( false, hbox1, "boxType" );

    optionStack = new Q3WidgetStack( this, "optionStack" );
    optionStack->setFrameShape( QFrame::StyledPanel );
    optionStack->setFrameShadow( Q3WidgetStack::Plain );

    functionPage = new QWidget( optionStack, "functionPage" );
	
	GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),functionPage,"GroupBox1" );
	GroupBox1->setFlat(true);

    textFunction = new QLabel( GroupBox1, "textFunction" );
	boxFunction = new QComboBox( false, GroupBox1, "boxFunction" );
    boxFunction->setEditable( true );

	textFrom = new QLabel( GroupBox1, "textFrom" );
    boxFrom = new QLineEdit( GroupBox1, "boxFrom" );
	boxFrom->setText("0");
	
    textTo = new QLabel( GroupBox1, "textTo" );
	boxTo = new QLineEdit( GroupBox1, "boxTo" );
	boxTo->setText("1");

    textPoints = new QLabel( GroupBox1, "textPoints" );
	boxPoints = new QSpinBox(2,1000000,100, GroupBox1, "boxPoints" );
	boxPoints->setValue(100);

	Q3HBoxLayout* hlayout = new Q3HBoxLayout(functionPage,5,5, "hlayout");
	hlayout->addWidget(GroupBox1);
    
    optionStack->addWidget( functionPage, 0 );

    parametricPage = new QWidget( optionStack, "parametricPage" );

	GroupBox2 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""), parametricPage,"GroupBox2" );
	GroupBox2->setFlat(true);
	
    textParameter = new QLabel( GroupBox2, "textParameter" );
    boxParameter = new QLineEdit( GroupBox2, "boxParameter" );
	boxParameter->setText("m");
	
	textParameterFrom = new QLabel( GroupBox2, "textParameterFrom" );
	boxParFrom = new QLineEdit( GroupBox2, "boxParFrom" );
	boxParFrom->setText("0");
	
	textParameterTo = new QLabel( GroupBox2, "textParameterTo" );
    boxParTo = new QLineEdit( GroupBox2, "boxParTo" );
	boxParTo->setText("1");

    textXPar = new QLabel( GroupBox2, "textXPar" );
    boxXFunction = new QComboBox(true, GroupBox2, "boxXFunction" );
	
	textYPar = new QLabel( GroupBox2, "textYPar" );
    boxYFunction = new QComboBox(true, GroupBox2, "boxYFunction" );
	
	textParPoints = new QLabel( GroupBox2, "textParPoints" );
	boxParPoints = new QSpinBox(2,1000000,100, GroupBox2, "boxParPoints" );
	boxParPoints->setValue(100);
	
	Q3HBoxLayout* hlayout2 = new Q3HBoxLayout(parametricPage, 5, 5, "hlayout");
	hlayout2->addWidget(GroupBox2);
	
    optionStack->addWidget( parametricPage, 1 );

    polarPage = new QWidget( optionStack, "polarPage" );

	GroupBox3 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""), polarPage,"GroupBox3" );
	GroupBox3->setFlat(true);
	
    textPolarParameter = new QLabel( GroupBox3, "textPolarParameter" );
	boxPolarParameter = new QLineEdit( GroupBox3, "boxPolarParameter" );
	boxPolarParameter->setText ("t");
	
    textPolarFrom = new QLabel( GroupBox3, "textPolarFrom" );
	boxPolarFrom = new QLineEdit( GroupBox3, "boxPolarFrom" );
	boxPolarFrom->setText("0");
	
    textPolarTo = new QLabel( GroupBox3, "textPolarTo" );
    boxPolarTo = new QLineEdit( GroupBox3, "boxPolarTo" );
	boxPolarTo->setText("pi");

	textPolarRadius = new QLabel( GroupBox3, "textPolarRadius" );
    boxPolarRadius = new QComboBox(true, GroupBox3, "boxPolarRadius" );
	
    textPolarTheta = new QLabel( GroupBox3, "textPolarTheta" );
    boxPolarTheta = new QComboBox(true, GroupBox3, "boxPolarTheta" );
	
	textPolarPoints = new QLabel( GroupBox3, "textPolarPoints" );
	boxPolarPoints = new QSpinBox(2,1000000,100,GroupBox3, "boxPolarPoints" );
	boxPolarPoints->setValue(100);
	
	Q3HBoxLayout* hlayout3 = new Q3HBoxLayout(polarPage, 5, 5, "hlayout");
	hlayout3->addWidget(GroupBox3);
	
    optionStack->addWidget( polarPage, 2 );

    GroupBox4 = new Q3ButtonGroup( 3, Qt::Horizontal,tr(""), this, "buttonGroup3" );
	GroupBox4->setFlat(true);
	
	buttonClear = new QPushButton( GroupBox4, "buttonClear" );
    buttonOk = new QPushButton( GroupBox4, "buttonOk" );
	buttonOk->setDefault(true);
	
	buttonCancel = new QPushButton( GroupBox4, "buttonCancel" );
	
	Q3VBoxLayout* hlayout1 = new Q3VBoxLayout(this, 5, 5, "hlayout1");
	hlayout1->addWidget(hbox1);
    hlayout1->addWidget(optionStack);
	hlayout1->addWidget(GroupBox4);
	
    languageChange();

    connect( boxType, SIGNAL( activated(int) ), optionStack, SLOT( raiseWidget(int) ) );
}

FunctionDialogUi::~FunctionDialogUi()
{
}

void FunctionDialogUi::languageChange()
{
    setWindowTitle( tr( "QtiPlot - Add function curve" ) );
    textFunction_2->setText( tr( "Curve type " ) );
    textFunction->setText( tr( "f(x)= " ) );
    textFrom->setText( tr( "From x= " ) );
    textTo->setText( tr( "To x= " ) );
    textPoints->setText( tr( "Points" ) );
    buttonClear->setText( tr( "Clear list" ) );
    textParameter->setText( tr( "Parameter" ) );
    textParPoints->setText( tr( "Points" ) );
    textParameterTo->setText( tr( "To" ) );
    textYPar->setText( tr( "y = " ) );
    textXPar->setText( tr( "x = " ) );
    textParameterFrom->setText( tr( "From" ) );
    textPolarPoints->setText( tr( "Points" ) );
    textPolarParameter->setText( tr( "Parameter" ) );
    textPolarFrom->setText( tr( "From" ) );
    textPolarTo->setText( tr( "To" ) );
    textPolarRadius->setText( tr( "R =" ) );
    textPolarTheta->setText( tr( "Theta =" ) );
    boxType->insertItem( tr( "Function" ) );
    boxType->insertItem( tr( "Parametric plot" ) );
    boxType->insertItem( tr( "Polar plot" ) );
    buttonCancel->setText( tr( "Cancel" ) );
    buttonOk->setText( tr( "Ok" ) );
}
