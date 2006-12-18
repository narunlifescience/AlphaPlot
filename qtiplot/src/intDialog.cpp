/***************************************************************************
    File                 : intDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Integration options dialog
                           
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
#include "intDialog.h"
#include "parser.h"
#include "graph.h"
#include "application.h"

#include <QGroupBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QLayout>

IntDialog::IntDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	   setName( "IntegrationDialog" );
	setWindowTitle(tr("QtiPlot - Integration Options"));
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    QGridLayout *gl1 = new QGridLayout();
	gl1->addWidget(new QLabel(tr("Integration of")), 0, 0);
	boxName = new QComboBox();
	gl1->addWidget(boxName, 0, 1);
	
	gl1->addWidget(new QLabel(tr("Order (1 - 5, 1 = Trapezoid Rule)")), 1, 0);
	boxOrder = new QSpinBox();
	boxOrder->setRange(1, 5);
	gl1->addWidget(boxOrder, 1, 1);
	
	gl1->addWidget(new QLabel(tr("Number of iterations (Max=40)")), 2, 0);
	boxSteps = new QSpinBox();
	boxSteps->setRange(2, 40);
	boxSteps->setValue(40);
	gl1->addWidget(boxSteps, 2, 1);

	gl1->addWidget(new QLabel(tr("Tolerance")), 3, 0);
	boxTol = new QLineEdit();
	boxTol->setText("0.01");
	gl1->addWidget(boxTol, 3, 1);
	
	gl1->addWidget(new QLabel(tr("Lower limit")), 4, 0);
	boxStart = new QLineEdit();
	gl1->addWidget(boxStart, 4, 1);
	
	gl1->addWidget(new QLabel(tr("Upper limit")), 5, 0);
	boxEnd = new QLineEdit();
	gl1->addWidget(boxEnd, 5, 1);

    QGroupBox *gb1 = new QGroupBox();
    gb1->setLayout(gl1);
	
	buttonOk = new QPushButton(tr( "&Integrate" ));
    buttonOk->setDefault( true );
	buttonHelp = new QPushButton(tr("&Help"));
    buttonCancel = new QPushButton(tr("&Close" ));
	
	QHBoxLayout *hbox1 = new QHBoxLayout(); 
    hbox1->addWidget(buttonOk);
    hbox1->addWidget(buttonHelp);
    hbox1->addWidget(buttonCancel);
    
    QVBoxLayout *vl = new QVBoxLayout();
 	vl->addWidget(gb1);
	vl->addLayout(hbox1);	
	setLayout(vl);
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL(clicked()),this, SLOT(help()));
    connect( boxName, SIGNAL( activated(int) ), this, SLOT(activateCurve(int)));
}

IntDialog::~IntDialog()
{
}

void IntDialog::accept()
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

int index=boxName->currentItem();
QwtPlotCurve *c = graph->curve(index);
if (!c || c->dataSize()<2)
	{
	QString s= tr("You cannot fit index:");
	s+="<p><b>'"+boxName->currentText()+"'</b><p>";
	s+=tr("because it has less than 2 points!");
	QMessageBox::warning(0,tr("QtiPlot - Warning"),s);

	changeCurve(index);
	return;
	}
	    
try
	{
	mu::Parser parser;
	parser.SetExpr(boxTol->text().ascii());
	parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0,tr("QtiPlot - Tolerance value error"),QString::fromStdString(e.GetMsg()));
	boxTol->clear();
	boxTol->setFocus();
	return;
	}	

double start,stop;    
double minx = c->minXValue();
double maxx = c->maxXValue();
// Check the Xmin 
QString from=boxStart->text().lower();
if(from=="min")
	{
	boxStart->setText(QString::number(minx));
	return;
	}
else if(from=="max")
	{
	boxStart->setText(QString::number(maxx));
	return;
	}
else
	{
	try
		{
		MyParser parser;			
		parser.SetExpr((boxStart->text()).ascii());
		start=parser.Eval();
			
		if(start<minx)
			{
			QMessageBox::warning(0, tr("QtiPlot - Input error"),
				tr("Please give a number larger or equal to the minimum value of X, for the lower limit.\n If you do not know that value, type min in the box."));
			boxStart->clear();
			boxStart->setFocus();
			return;
		}
		if(start > maxx)
			{
			QMessageBox::warning(0, tr("QtiPlot - Input error"),
				tr("Please give a number smaller or equal to the maximum value of X, for the lower limit.\n If you do not know that value, type max in the box."));
			boxStart->clear();
			boxStart->setFocus();
			return;
			}
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical(0,tr("QtiPlot - Start limit error"),QString::fromStdString(e.GetMsg()));
		boxStart->clear();
		boxStart->setFocus();
		return;
		}	
	}
	
// Check Xmax
QString end=boxEnd->text().lower();    
if(end=="min")
	{
	boxEnd->setText(QString::number(minx));
	return;
	}
else if(end=="max")
	{
	boxEnd->setText(QString::number(maxx));
	return;
	}
else
	{
	try
		{
		MyParser parser;
		parser.SetExpr((boxEnd->text()).ascii());
		stop=parser.Eval();	
		if(stop>maxx)
			{
			QMessageBox::warning(0, tr("QtiPlot - Input error"),
				tr("Please give a number smaller or equal to the maximum value of X, for the upper limit.\n If you do not know that value, type max in the box."));
			boxEnd->clear();
			boxEnd->setFocus();
			return;
			}
		if(stop<minx)
			{
			QMessageBox::warning(0, tr("QtiPlot - Input error"),
				tr("Please give a number larger or equal to the minimum value of X, for the upper limit.\n If you do not know that value, type min in the box."));
			boxEnd->clear();
			boxEnd->setFocus();
			return;
			}
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical(0,tr("QtiPlot - End limit error"),QString::fromStdString(e.GetMsg()));
		boxEnd->clear();
		boxEnd->setFocus();
		return;
		}	
	}

start=boxStart->text().toDouble();
stop=boxEnd->text().toDouble();

QString res=graph->integrateCurve(c,boxOrder->value(),boxSteps->value(),boxTol->text().toDouble(),start,stop);
if ( !res.isEmpty() )
	{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	app->logInfo+=res;
	app->showResults(true);
	app->modifiedProject();
	}
}

void IntDialog::setGraph(Graph *g)
{
graph = g;
boxName->insertStringList (g->curvesList(),-1);
int index = 0;
if (graph->selectorsEnabled())
	index = graph->curveIndex(graph->selectedCurveID());

activateCurve(index);
boxName->setCurrentItem(index);

connect (graph, SIGNAL(closedGraph()), this, SLOT(close()));
connect (graph, SIGNAL(dataRangeChanged()), this, SLOT(changeDataRange()));
};

void IntDialog::activateCurve(int index)
{
QwtPlotCurve *c = graph->curve(index);
if (!c)
	return;

if (graph->selectorsEnabled() && graph->selectedCurveID() == graph->curveKey(index))
	{
	double start = graph->selectedXStartValue();
	double end = graph->selectedXEndValue();
	boxStart->setText(QString::number(QMIN(start, end), 'g', 15));
	boxEnd->setText(QString::number(QMAX(start, end), 'g', 15));
	}
else
	{
	boxStart->setText(QString::number(c->minXValue(), 'g', 15));
	boxEnd->setText(QString::number(c->maxXValue(), 'g', 15));
	}
};

void IntDialog::changeCurve(int index)
{
QwtPlotCurve *c = graph->curve(index);
while(c->dataSize()<2)
	{
	index++;
	c = graph->curve(index);
	if(!c || index >= graph->curves()) 
		index=0; //Restart from the beginning
	}
activateCurve(index);
boxName->setCurrentItem(index);
}

void IntDialog::changeDataRange()
{
double start = graph->selectedXStartValue();
double end = graph->selectedXEndValue();
boxStart->setText(QString::number(QMIN(start, end), 'g', 15));
boxEnd->setText(QString::number(QMAX(start, end), 'g', 15));
}

void IntDialog::help()
{
QMessageBox::about(0, tr("QtiPlot - Help for Integration"),
				   tr("The integration of a curve consists of the following five steps:\n 1) Choose which curve you want to integrate\n 2) Set the order of the integration. The higher it is the more accurate the calculation is\n 3) Choose the number of iterations \n 4) Choose the tolerance \n 5) Choose the lower and the upper limit.\n The code integrates the curve with an iterative algorithm. The tolerance determines the termination criteria for the solver.\n Because, sometimes we ask for too much accuracy, the number of iterations makes sure that the solver will not work for ever.\n IMPORTANT \nThe limits must be within the range of x; If you do not know the maximum (minimum) value of x, type max (min) in the boxes."));
}
