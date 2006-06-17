/***************************************************************************
    File                 : lineDlg.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Line options dialog
                           
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
#include "lineDlg.h"
#include "colorButton.h"

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <q3buttongroup.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qcolordialog.h>
#include <qtabwidget.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>

LineDialog::LineDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "LineDialog" );
    setWindowTitle( tr( "QtiPlot - Line options" ) );

	tw = new QTabWidget( this, "tw" );
	options = new QWidget( tw, "options" );
    GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal, QString(),options,"GroupBox1" );

	new QLabel(tr( "Color" ), GroupBox1, "TextLabel1",0);
	colorBox = new ColorButton(GroupBox1);

	new QLabel(tr( "Line type" ),GroupBox1, "TextLabel1_2_2",0 );
    styleBox = new QComboBox( false, GroupBox1, "styleBox" );
	styleBox->insertItem("_____");
	styleBox->insertItem("- - -");
	styleBox->insertItem(".....");
	styleBox->insertItem("_._._");
	styleBox->insertItem("_.._..");
	
	new QLabel(tr( "Line width" ),GroupBox1, "TextLabel1_2",0 );
    widthBox = new QComboBox( false, GroupBox1, "widthBox" );
	widthBox->insertItem( tr( "1" ) );
    widthBox->insertItem( tr( "2" ) );
    widthBox->insertItem( tr( "3" ) );
    widthBox->insertItem( tr( "4" ) );
    widthBox->insertItem( tr( "5" ) );
	widthBox->setEditable (true);
	widthBox->setCurrentItem(0);
	
	startBox = new QCheckBox(GroupBox1, "startBox" ); 
    startBox->setText( tr( "Arrow at &start" ) );
	
	endBox = new QCheckBox(GroupBox1, "endBox" );
    endBox->setText( tr( "Arrow at &end" ) );
	endBox->setChecked(true);

	Q3HBoxLayout* hl1 = new Q3HBoxLayout(options,5,5, "hl1");
    hl1->addWidget(GroupBox1);
	
	tw->insertTab(options, tr( "Opti&ons" ) );
	
	head = new QWidget( tw, "head" );
    Q3ButtonGroup *GroupBox4 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),head,"GroupBox4" );

	new QLabel(tr( "Length" ), GroupBox4, "TextLabel111",0);
	boxHeadLength = new QSpinBox( 0,100,1,GroupBox4, "boxHeadLength" );
	
	new QLabel(tr( "Angle" ),GroupBox4, "TextLabel1112",0 );
	boxHeadAngle = new QSpinBox(0,85,5,GroupBox4, "boxHeadAngle" );

	filledBox = new QCheckBox(GroupBox4, "filledBox" ); 
    filledBox->setText( tr( "&Filled" ) );
	
	Q3HBoxLayout* hl3 = new Q3HBoxLayout(head,5,5, "hl3");
    hl3->addWidget(GroupBox4);

	tw->insertTab(head, tr( "Arrow &Head" ) );

	geometry = new QWidget( tw, "geometry" );
    Q3ButtonGroup *GroupBox2 = new Q3ButtonGroup( 2,Qt::Horizontal,tr("Start Point"),geometry,"GroupBox2" );

	new QLabel(tr( "X" ), GroupBox2, "TextLabel11",0);
	xStartBox = new QSpinBox( 0,10000,1,GroupBox2, "xstart" );
	
	new QLabel(tr( "Y" ),GroupBox2, "TextLabel111",0 );
	yStartBox = new QSpinBox(0,10000,1,GroupBox2, "ystart" );

	Q3ButtonGroup *GroupBox3 = new Q3ButtonGroup( 2,Qt::Horizontal,tr("End Point"),geometry,"GroupBox3" );

	new QLabel(tr( "X" ), GroupBox3, "TextLabel11",0);
	xEndBox = new QSpinBox(0,10000,1,GroupBox3, "xstart" );
	
	new QLabel(tr( "Y" ),GroupBox3, "TextLabel111",0 );
	yEndBox = new QSpinBox(0,10000,1,GroupBox3, "ystart" );

	Q3HBoxLayout* hl2 = new Q3HBoxLayout(geometry,5,5, "hl2");
    hl2->addWidget(GroupBox2);
	hl2->addWidget(GroupBox3);

	tw->insertTab(geometry, tr( "&Geometry" ) );

	GroupBox2 = new Q3ButtonGroup(3,Qt::Horizontal, QString(),this,"GroupBox2" );
	
    btnOk = new QPushButton(GroupBox2, "btnOk" );
    btnOk->setText( tr( "&Ok" ) );
	
	btnApply = new QPushButton(GroupBox2, "btnApply" );
    btnApply->setText( tr( "&Apply" ) );

    btnCancel = new QPushButton(GroupBox2, "btnCancel" );
    btnCancel->setText( tr( "&Cancel" ) );
	
	Q3VBoxLayout* hlayout = new Q3VBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(tw);
	hlayout->addWidget(GroupBox2);

	connect( colorBox, SIGNAL( clicked() ), this, SLOT(pickColor() ) );
	connect( btnOk, SIGNAL( clicked() ), this, SLOT(accept() ) );
	connect( btnApply, SIGNAL( clicked() ), this, SLOT(apply() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT(close() ) );
}

void LineDialog::apply()
{
if (tw->currentPage()==(QWidget *)options)
	{
	Qt::PenStyle style;

	switch (styleBox->currentItem())
		{
		case 0:
			style=Qt::SolidLine;
		break;
		case 1:
			style=Qt::DashLine;
		break;
		case 2:
			style=Qt::DotLine;
		break;
		case 3:
			style=Qt::DashDotLine;
		break;
		case 4:
			style=Qt::DashDotDotLine;
		break;
		}

	emit values(colorBox->color(),widthBox->currentText().toInt(),style, endBox->isChecked(),startBox->isChecked());
	}
else if (tw->currentPage()==(QWidget *)head)
	{
	emit setHeadGeometry(boxHeadLength->value(),boxHeadAngle->value(), 
		filledBox->isChecked());
	}
else if (tw->currentPage()==(QWidget *)geometry)
	{
	emit setLineGeometry(QPoint(xStartBox->value(),yStartBox->value()),
			QPoint(xEndBox->value(),yEndBox->value()));
	}
enableHeadTab();
}

void LineDialog::accept()
{
apply();
close();
}

void LineDialog::setEndArrow(bool on)
{
endBox->setChecked(on);
}

void LineDialog::setStartArrow(bool on)
{
startBox->setChecked(on);
}

void LineDialog::setStyle(Qt::PenStyle style)
{
if (style==Qt::SolidLine)
	styleBox->setCurrentItem(0);
else if (style==Qt::DashLine)
	styleBox->setCurrentItem(1);
else if (style==Qt::DotLine)
	styleBox->setCurrentItem(2);	
else if (style==Qt::DashDotLine)
	styleBox->setCurrentItem(3);		
else if (style==Qt::DashDotDotLine)
	styleBox->setCurrentItem(4);
}

void LineDialog::setWidth(int w)
{
widthBox->setEditText(QString::number(w));
}

void LineDialog::setColor(QColor c)
{
  colorBox->setColor(c);
}

void LineDialog::setStartPoint(const QPoint& p)
{
xStartBox->setValue(p.x());
yStartBox->setValue(p.y());
}

void LineDialog::setEndPoint(const QPoint& p)
{
xEndBox->setValue(p.x());
yEndBox->setValue(p.y());
}

void LineDialog::initHeadGeometry(int length, int angle, bool filled)
{
boxHeadLength->setValue(length);
boxHeadAngle->setValue(angle);
filledBox->setChecked(filled);	
}
	
void LineDialog::enableHeadTab()
{
if (startBox->isChecked() || endBox->isChecked())
	tw->setTabEnabled (head, true);
else
	tw->setTabEnabled (head, false);
}

void LineDialog::pickColor()
{
QColor c = QColorDialog::getColor(colorBox->color(), this);
if ( !c.isValid() || c == colorBox->color() )
	return;

colorBox->setColor ( c ) ;
}
	
LineDialog::~LineDialog()
{
}
