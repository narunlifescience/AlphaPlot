/***************************************************************************
    File                 : InterpolationDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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
#include "InterpolationDialog.h"
#include "Graph.h"
#include "MyParser.h"
#include "ColorBox.h"
#include "Interpolation.h"

#include <QGroupBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QLayout>

InterpolationDialog::InterpolationDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "InterpolationDialog" );
	setWindowTitle(tr("QtiPlot - Interpolation Options"));

    QGroupBox *gb1 = new QGroupBox();
	QGridLayout *gl1 = new QGridLayout(gb1);
	gl1->addWidget(new QLabel(tr("Make curve from")), 0, 0);

	boxName = new QComboBox();
	gl1->addWidget(boxName, 0, 1);

	gl1->addWidget(new QLabel(tr("Spline")), 1, 0);
	boxMethod = new QComboBox();
	boxMethod->insertItem(tr("Linear"));
    boxMethod->insertItem(tr("Cubic"));
    boxMethod->insertItem(tr("Non-rounded Akima"));
	gl1->addWidget(boxMethod, 1, 1);

	gl1->addWidget(new QLabel(tr("Points")), 2, 0);
	boxPoints = new QSpinBox();
	boxPoints->setRange(3,100000);
	boxPoints->setSingleStep(10);
	boxPoints->setValue(1000);
	gl1->addWidget(boxPoints, 2, 1);

	gl1->addWidget(new QLabel(tr("From Xmin")), 3, 0);
	boxStart = new QLineEdit();
	boxStart->setText(tr("0"));
	gl1->addWidget(boxStart, 3, 1);

	gl1->addWidget(new QLabel(tr("To Xmax")), 4, 0);
	boxEnd = new QLineEdit();
	gl1->addWidget(boxEnd, 4, 1);

	gl1->addWidget(new QLabel(tr("Color")), 5, 0);

	boxColor = new ColorBox(false);
	boxColor->setColor(QColor(Qt::red));
	gl1->addWidget(boxColor, 5, 1);
	gl1->setRowStretch(6, 1);

	buttonFit = new QPushButton(tr( "&Make" ));
    buttonFit->setDefault( true );
    buttonCancel = new QPushButton(tr( "&Close" ));

	QVBoxLayout *vl = new QVBoxLayout();
 	vl->addWidget(buttonFit);
	vl->addWidget(buttonCancel);
    vl->addStretch();

    QHBoxLayout *hb = new QHBoxLayout(this);
    hb->addWidget(gb1);
    hb->addLayout(vl);

	connect( boxName, SIGNAL(activated(const QString&)), this, SLOT( activateCurve(const QString&)));
	connect( buttonFit, SIGNAL( clicked() ), this, SLOT( interpolate() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void InterpolationDialog::interpolate()
{
QString curve = boxName->currentText();
QStringList curvesList = graph->analysableCurvesList();
if (!curvesList.contains(curve))
	{
	QMessageBox::critical(this,tr("QtiPlot - Warning"),
		tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!").arg(curve));
	boxName->clear();
	boxName->addItems(curvesList);
	return;
	}

double from, to;
try
	{
	MyParser parser;
	parser.SetExpr(boxStart->text().replace(",", ".").ascii());
	from = parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(this, tr("QtiPlot - Start limit error"), QString::fromStdString(e.GetMsg()));
	boxStart->setFocus();
	return;
	}

try
	{
	MyParser parser;
	parser.SetExpr(boxEnd->text().replace(",", ".").ascii());
	to = parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(this, tr("QtiPlot - End limit error"), QString::fromStdString(e.GetMsg()));
	boxEnd->setFocus();
	return;
	}

if (from >= to)
	{
	QMessageBox::critical(this, tr("QtiPlot - Input error"), tr("Please enter x limits that satisfy: from < to!"));
	boxEnd->setFocus();
	return;
	}

Interpolation *i = new Interpolation((ApplicationWindow *)this->parent(), graph, curve,
                                      from, to, boxMethod->currentIndex());
i->setOutputPoints(boxPoints->value());
i->setColor(boxColor->currentIndex());
i->run();
delete i;
}

void InterpolationDialog::setGraph(Graph *g)
{
	graph = g;
	boxName->addItems(g->analysableCurvesList());

    QString selectedCurve = g->selectedCurveTitle();
	if (!selectedCurve.isEmpty())
	{
	    int index = boxName->findText (selectedCurve);
		boxName->setCurrentItem(index);
	}

    activateCurve(boxName->currentText());

	connect (graph, SIGNAL(closedGraph()), this, SLOT(close()));
	connect (graph, SIGNAL(dataRangeChanged()), this, SLOT(changeDataRange()));
};

void InterpolationDialog::activateCurve(const QString& curveName)
{
	QwtPlotCurve *c = graph->curve(curveName);
	if (!c)
		return;

	double start, end;
	graph->range(graph->curveIndex(curveName), &start, &end);
	boxStart->setText(QString::number(QMIN(start, end)));
	boxEnd->setText(QString::number(QMAX(start, end)));
};

void InterpolationDialog::changeDataRange()
{
double start = graph->selectedXStartValue();
double end = graph->selectedXEndValue();
boxStart->setText(QString::number(QMIN(start, end), 'g', 15));
boxEnd->setText(QString::number(QMAX(start, end), 'g', 15));
}
