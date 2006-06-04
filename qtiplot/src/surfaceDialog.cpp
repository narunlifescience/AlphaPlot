/***************************************************************************
    File                 : surfaceDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Define surface plot dialog
                           
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
#include "surfaceDialog.h"
#include "parser.h"
#include "application.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

sDialog::sDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "sDialog" );
	setWindowTitle(tr("QtiPlot - Define surface plot"));
    setMinimumSize( QSize( 310, 140 ) );
	setMaximumSize( QSize( 310, 140 ) );
    setMouseTracking( true );
    setSizeGripEnabled( false );
	
	GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),this,"GroupBox1" );
	GroupBox1->setFlat (true);

	new QLabel( tr("f(x,y)="), GroupBox1, "TextLabel2",0 );
	boxFunction = new QComboBox(GroupBox1, "boxFunction" );
	boxFunction->setFixedWidth(250);
	boxFunction->setEditable(true);

	GroupBox5 = new Q3ButtonGroup(3,Qt::Horizontal,tr(""),this,"GroupBox5" );
	GroupBox5->setFlat (true);

	GroupBox3 = new Q3ButtonGroup(1,Qt::Horizontal,tr("X - axis"),GroupBox5,"GroupBox3" );
	
	new QLabel( tr("From"), GroupBox3, "TextLabel3",0 );
	boxXFrom = new QLineEdit(GroupBox3, "boxPoints" );
	boxXFrom->setText(tr("-1"));

	new QLabel( tr("To"), GroupBox3, "TextLabel5",0 );
	boxXTo = new QLineEdit(GroupBox3, "boxOrder" );
	boxXTo->setText(tr("1"));

	GroupBox4 = new Q3ButtonGroup(1,Qt::Horizontal,tr("Y - axis"),GroupBox5,"GroupBox4" );
	
	new QLabel( tr("From"), GroupBox4, "TextLabel33",0 );
	boxYFrom = new QLineEdit(GroupBox4, "boxPoints" );
	boxYFrom->setText(tr("-1"));

	new QLabel( tr("To"), GroupBox4, "TextLabel5",0 );
	boxYTo = new QLineEdit(GroupBox4, "boxYto" );
	boxYTo->setText(tr("1"));

	GroupBox6 = new Q3ButtonGroup(1,Qt::Horizontal,tr("Z - axis"),GroupBox5,"GroupBox4" );
	
	new QLabel( tr("From"), GroupBox6, "TextLabel35",0 );
	boxZFrom = new QLineEdit(GroupBox6, "boxZFrom" );
	boxZFrom->setText(tr("-1"));

	new QLabel( tr("To"), GroupBox6, "TextLabel5",0 );
	boxZTo = new QLineEdit(GroupBox6, "boxZto" );
	boxZTo->setText(tr("1"));

	GroupBox2 = new Q3ButtonGroup(3,Qt::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (true);

	buttonClear = new QPushButton(GroupBox2, "buttonClear" );
    buttonClear->setText( tr( "Clear &list" ) );
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setDefault( true );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setText( tr( "&Cancel" ) );
	
	Q3VBoxLayout* hlayout = new Q3VBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox5);
	hlayout->addWidget(GroupBox2);
   
    // signals and slots connections
	connect( buttonClear, SIGNAL( clicked() ), this, SLOT(clearList() ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

	setFocusProxy(boxFunction);
}

void sDialog::clearList()
{
boxFunction->clear();
emit clearFunctionsList();
}

void sDialog::setFunction(const QString& s)
{
boxFunction->setCurrentText(s);
}

void sDialog::setLimits(double xs, double xe, double ys, double ye, double zs, double ze)
{
	boxXFrom->setText(QString::number(xs));
	boxXTo->setText(QString::number(xe));
	boxYFrom->setText(QString::number(ys));
	boxYTo->setText(QString::number(ye));
	boxZFrom->setText(QString::number(zs));
	boxZTo->setText(QString::number(ze));
}

void sDialog::accept()
{
QString Xfrom=boxXFrom->text().lower();
QString Xto=boxXTo->text().lower();
QString Yfrom=boxYFrom->text().lower();
QString Yto=boxYTo->text().lower();
QString Zfrom=boxZFrom->text().lower();
QString Zto=boxZTo->text().lower();

double fromX, toX, fromY,toY, fromZ,toZ;
try
	{
	myParser parser;	
	parser.SetExpr(Xfrom.ascii());
	fromX=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - X Start limit error"), QString::fromStdString(e.GetMsg()));
	boxXFrom->setFocus();
	return;
	}
try
	{
	myParser parser;
	parser.SetExpr(Xto.ascii());
	toX=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - X End limit error"), QString::fromStdString(e.GetMsg()));
	boxXTo->setFocus();
	return;
	}	

try
	{
	myParser parser;
	parser.SetExpr(Yfrom.ascii());
	fromY=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Y Start limit error"), QString::fromStdString(e.GetMsg()));
	boxYFrom->setFocus();
	return;
	}	
try
	{
	myParser parser;	
	parser.SetExpr(Yto.ascii());
	toY=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Y End limit error"), QString::fromStdString(e.GetMsg()));
	boxYTo->setFocus();
	return;
	}	
try
	{
	myParser parser;
	parser.SetExpr(Zfrom.ascii());
	fromZ=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Z Start limit error"), QString::fromStdString(e.GetMsg()));
	boxZFrom->setFocus();
	return;
	}	
try
	{
	myParser parser;
	parser.SetExpr(Zto.ascii());
	toZ=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Z End limit error"), QString::fromStdString(e.GetMsg()));
	boxZTo->setFocus();
	return;
	}

if (fromX >= toX || fromY >= toY || fromZ >= toZ)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input error"),
				tr("Please enter limits that satisfy: from < end!"));
	boxXTo->setFocus();
	return;
	}

double x,y;
QString formula=boxFunction->currentText();
bool error=false;
try
	{
	myParser parser;
	parser.DefineVar("x", &x);	
	parser.DefineVar("y", &y);		
	parser.SetExpr(formula.ascii());
		
	x=fromX; y=fromY;
	parser.Eval();
	x=toX; y=toY;
	parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
	boxFunction->setFocus();
	error=true;	
	}
	
if (!error)
	{
	emit options(boxFunction->currentText(),fromX, toX, fromY, toY, fromZ, toZ);
	emit custom3DToolBar();

	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	app->updateSurfaceFuncList(boxFunction->currentText());
	close();
	}
}

void sDialog::insertFunctionsList(const QStringList& list)
{
boxFunction->insertStringList (list, 1);
}

sDialog::~sDialog()
{
}
