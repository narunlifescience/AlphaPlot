/***************************************************************************
    File                 : interpolationDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Interpolation options dialog
                           
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
#include "interpolationDialog.h"
#include "graph.h"
#include "parser.h"
#include "colorBox.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>

interpolationDialog::interpolationDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "interpolationDialog" );
	setWindowTitle(tr("QtiPlot - Interpolation Options"));
	
	Q3ButtonGroup *GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),this,"GroupBox1" );

	new QLabel( tr("Make curve from"), GroupBox1, "TextLabel1",0 );
	boxName = new QComboBox(GroupBox1, "boxShow" );
	
	new QLabel( tr("Spline"), GroupBox1, "TextLabel2",0 );
	boxMethod = new QComboBox(GroupBox1, "boxMethod" );
	
	new QLabel( tr("Points"), GroupBox1, "TextLabel3",0 );
	boxPoints = new QSpinBox(3,100000,10,GroupBox1, "boxPoints" );
	boxPoints->setValue(1000);

	new QLabel( tr("From Xmin"), GroupBox1, "TextLabel4",0 );
	boxStart = new QLineEdit(GroupBox1, "boxStart" );
	boxStart->setText(tr("0"));
	
	new QLabel( tr("To Xmax"), GroupBox1, "TextLabel5",0 );
	boxEnd = new QLineEdit(GroupBox1, "boxEnd" );

	new QLabel( tr("Color"), GroupBox1, "TextLabel52",0 );
	boxColor = new ColorBox( FALSE, GroupBox1);
	boxColor->setColor(QColor(Qt::red));

	Q3ButtonGroup *GroupBox2 = new Q3ButtonGroup(1,Qt::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	
	buttonFit = new QPushButton(GroupBox2, "buttonFit" );
    buttonFit->setAutoDefault( TRUE );
    buttonFit->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
	Q3HBoxLayout* hlayout = new Q3HBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox2);

    languageChange();
   
    // signals and slots connections
	connect( boxName, SIGNAL( activated(int) ), this, SLOT( activateCurve(int) ) );
	connect( buttonFit, SIGNAL( clicked() ), this, SLOT( interpolate() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

interpolationDialog::~interpolationDialog()
{
}


void interpolationDialog::languageChange()
{
buttonFit->setText( tr( "&Make" ) );
buttonCancel->setText( tr( "&Close" ) );

boxMethod->insertItem(tr("Linear"));
boxMethod->insertItem(tr("Cubic"));
boxMethod->insertItem(tr("Non-rounded Akima"));
}

void interpolationDialog::interpolate()
{
QString curve = boxName->currentText();
QStringList curvesList = graph->curvesList();
if (curvesList.contains(curve) <= 0)
	{
	QMessageBox::critical(this,tr("QtiPlot - Warning"),
		tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!").arg(curve));
	boxName->clear();
	boxName->insertStringList(curvesList);
	return;
	}

double from, to;
try
	{
	myParser parser;
	parser.SetExpr(boxStart->text().ascii());
	from=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(this, tr("QtiPlot - Start limit error"), QString::fromStdString(e.GetMsg()));
	boxStart->setFocus();
	return;
	}		
	
try
	{
	myParser parser;	
	parser.SetExpr(boxEnd->text().ascii());
	to=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(this, tr("QtiPlot - End limit error"), QString::fromStdString(e.GetMsg()));
	boxEnd->setFocus();
	return;
	}	

if (from>=to)
	{
	QMessageBox::critical(this, tr("QtiPlot - Input error"),
				tr("Please enter x limits that satisfy: from < to!"));
	boxEnd->setFocus();
	return;
	}
	
int start, end;
int spline = boxMethod->currentItem();
QwtPlotCurve *c = graph->getFitLimits(boxName->currentText(), from, to, spline+3, start, end);
if (!c)
	return;

graph->interpolate(c, spline, start, end, boxPoints->value(), boxColor->currentItem());
}

void interpolationDialog::setGraph(Graph *g)
{
graph = g;
boxName->insertStringList (g->curvesList(),-1);
	
if (g->selectorsEnabled())
	{
	int index = g->curveIndex(g->selectedCurveID());
	boxName->setCurrentItem(index);
	activateCurve(index);
	}
else
	activateCurve(0);

connect (graph, SIGNAL(closedGraph()), this, SLOT(close()));
connect (graph, SIGNAL(dataRangeChanged()), this, SLOT(changeDataRange()));
};

void interpolationDialog::activateCurve(int index)
{
QwtPlotCurve *c = graph->curve(index);
if (!c)
	return;

if (graph->selectorsEnabled() && graph->selectedCurveID() == graph->curveKey(index))
	{
	double start = graph->selectedXStartValue();
	double end = graph->selectedXEndValue();
	boxStart->setText(QString::number(QMIN(start, end)));
	boxEnd->setText(QString::number(QMAX(start, end)));
	}
else
	{
	boxStart->setText(QString::number(c->minXValue()));
	boxEnd->setText(QString::number(c->maxXValue()));
	}
};

void interpolationDialog::changeDataRange()
{
double start = graph->selectedXStartValue();
double end = graph->selectedXEndValue();
boxStart->setText(QString::number(QMIN(start, end), 'g', 15));
boxEnd->setText(QString::number(QMAX(start, end), 'g', 15));
}

