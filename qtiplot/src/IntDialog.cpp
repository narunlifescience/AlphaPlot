/***************************************************************************
    File                 : IntDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Vasileios Gkanis, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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
#include "IntDialog.h"
#include "MyParser.h"
#include "Graph.h"
#include "ApplicationWindow.h"
#include "Integration.h"
#include "FunctionCurve.h"
#include "Differentiation.h"

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

    QGroupBox *gb1 = new QGroupBox();
    QGridLayout *gl1 = new QGridLayout(gb1);
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
    gl1->setRowStretch(6, 1);

	buttonOk = new QPushButton(tr( "&Integrate" ));
    buttonOk->setDefault( true );
	buttonHelp = new QPushButton(tr("&Help"));
    buttonCancel = new QPushButton(tr("&Close" ));

	QVBoxLayout *vl = new QVBoxLayout();
 	vl->addWidget(buttonOk);
    vl->addWidget(buttonHelp);
	vl->addWidget(buttonCancel);
    vl->addStretch();

    QHBoxLayout *hb = new QHBoxLayout(this);
    hb->addWidget(gb1);
    hb->addLayout(vl);

    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL(clicked()),this, SLOT(help()));
    connect( boxName, SIGNAL( activated(const QString&) ), this, SLOT(activateCurve(const QString&)));
}

void IntDialog::accept()
{
QString curveName = boxName->currentText();
QwtPlotCurve *c = graph->curve(curveName);
QStringList curvesList = graph->analysableCurvesList();
if (!c || !curvesList.contains(curveName))
	{
	QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot") +" - "+ tr("Warning"),
		tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!").arg(curveName));
	boxName->clear();
	boxName->insertStringList(curvesList);
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
	QMessageBox::critical((ApplicationWindow *)parent(),tr("QtiPlot - Tolerance value error"),QString::fromStdString(e.GetMsg()));
	boxTol->clear();
	boxTol->setFocus();
	return;
	}

double start = 0, stop = 0;
double minx = c->minXValue();
double maxx = c->maxXValue();

// Check the Xmin
QString from = boxStart->text().lower();
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
			QMessageBox::warning((ApplicationWindow *)parent(), tr("QtiPlot - Input error"),
				tr("Please give a number larger or equal to the minimum value of X, for the lower limit.\n If you do not know that value, type min in the box."));
			boxStart->clear();
			boxStart->setFocus();
			return;
		}
		if(start > maxx)
			{
			QMessageBox::warning((ApplicationWindow *)parent(), tr("QtiPlot - Input error"),
				tr("Please give a number smaller or equal to the maximum value of X, for the lower limit.\n If you do not know that value, type max in the box."));
			boxStart->clear();
			boxStart->setFocus();
			return;
			}
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical((ApplicationWindow *)parent(),tr("QtiPlot - Start limit error"),QString::fromStdString(e.GetMsg()));
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
		stop = parser.Eval();
		if(stop > maxx)
			{
			//FIXME: I don't understand why this doesn't work for FunctionCurves!!(Ion)
			/*QMessageBox::warning((ApplicationWindow *)parent(), tr("QtiPlot - Input error"),
				tr("Please give a number smaller or equal to the maximum value of X, for the upper limit.\n If you do not know that value, type max in the box."));
			boxEnd->clear();
			boxEnd->setFocus();
			return;
			*/
			boxEnd->setText(QString::number(maxx));
			}
		if(stop < minx)
			{
			QMessageBox::warning((ApplicationWindow *)parent(), tr("QtiPlot - Input error"),
				tr("Please give a number larger or equal to the minimum value of X, for the upper limit.\n If you do not know that value, type min in the box."));
			boxEnd->clear();
			boxEnd->setFocus();
			return;
			}
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical((ApplicationWindow *)parent(), tr("QtiPlot - End limit error"),QString::fromStdString(e.GetMsg()));
		boxEnd->clear();
		boxEnd->setFocus();
		return;
		}
	}

Integration *i = new Integration((ApplicationWindow *)this->parent(), graph, curveName,
                                 boxStart->text().toDouble(), boxEnd->text().toDouble());
i->setTolerance(boxTol->text().toDouble());
i->setMaximumIterations(boxSteps->value());
i->setMethod(boxOrder->value());
i->run();
delete i;
}

void IntDialog::setGraph(Graph *g)
{
    graph = g;
    boxName->insertStringList (g->analysableCurvesList());

    QString selectedCurve = g->selectedCurveTitle();
	if(!selectedCurve.isEmpty())
	{
	    int index = boxName->findText(selectedCurve);
		boxName->setCurrentItem(index);
	}
    activateCurve(boxName->currentText());

    connect (graph, SIGNAL(closedGraph()), this, SLOT(close()));
    connect (graph, SIGNAL(dataRangeChanged()), this, SLOT(changeDataRange()));
}

void IntDialog::activateCurve(const QString& curveName)
{
	QwtPlotCurve *c = graph->curve(curveName);
	if (!c)
		return;

	double start, end;
	graph->range(graph->curveIndex(curveName), &start, &end);
	boxStart->setText(QString::number(QMIN(start, end), 'g', 15));
	boxEnd->setText(QString::number(QMAX(start, end), 'g', 15));
};

void IntDialog::changeDataRange()
{
double start = graph->selectedXStartValue();
double end = graph->selectedXEndValue();
boxStart->setText(QString::number(QMIN(start, end), 'g', 15));
boxEnd->setText(QString::number(QMAX(start, end), 'g', 15));
}

void IntDialog::help()
{
QMessageBox::about(this, tr("QtiPlot - Help for Integration"),
				   tr("The integration of a curve consists of the following five steps:\n 1) Choose which curve you want to integrate\n 2) Set the order of the integration. The higher it is the more accurate the calculation is\n 3) Choose the number of iterations \n 4) Choose the tolerance \n 5) Choose the lower and the upper limit.\n The code integrates the curve with an iterative algorithm. The tolerance determines the termination criteria for the solver.\n Because, sometimes we ask for too much accuracy, the number of iterations makes sure that the solver will not work for ever.\n IMPORTANT \nThe limits must be within the range of x; If you do not know the maximum (minimum) value of x, type max (min) in the boxes."));
}
