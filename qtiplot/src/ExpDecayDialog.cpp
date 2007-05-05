/***************************************************************************
    File                 : ExpDecayDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit exponential decay dialog

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
#include "ExpDecayDialog.h"
#include "Graph.h"
#include "ColorBox.h"
#include "ApplicationWindow.h"
#include "Fit.h"
#include "ExponentialFit.h"

#include <QMessageBox>
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

	ExpDecayDialog::ExpDecayDialog(int type, QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
: QDialog( parent, name, modal, fl )
{
	if ( !name )
		setName( "ExpDecayDialog" );

	slopes = type;

	setWindowTitle(tr("QtiPlot - Verify initial guesses"));

	QGroupBox *gb1 = new QGroupBox();
	QGridLayout *gl1 = new QGridLayout();
	gl1->addWidget(new QLabel(tr("Exponential Fit of")), 0, 0);

	boxName = new QComboBox();
	connect( boxName, SIGNAL( activated(const QString&) ), this, SLOT( activateCurve(const QString&) ) );
	gl1->addWidget(boxName, 0, 1);

	if (type < 0)
		dampingLabel = new QLabel( tr("Growth time"));
	else if (type == 1)
		dampingLabel = new QLabel( tr("Decay time"));
	else
		dampingLabel = new QLabel( tr("First decay time (t1)"));
	gl1->addWidget(dampingLabel, 1, 0);

	boxFirst = new QLineEdit();
	boxFirst->setText(tr("1"));
	gl1->addWidget(boxFirst, 1, 1);

	if (type > 1)
	{
		gl1->addWidget(new QLabel( tr("Second decay time (t2)")), 2, 0);

		boxSecond = new QLineEdit();
		boxSecond->setText(tr("1"));
		gl1->addWidget(boxSecond, 2, 1);

		thirdLabel = new QLabel( tr("Third decay time (t3)"));
		gl1->addWidget(thirdLabel, 3, 0);

		boxThird = new QLineEdit();
		boxThird->setText(tr("1"));
		gl1->addWidget(boxThird, 3, 1);

		if (type < 3)
		{
			thirdLabel->hide();
			boxThird->hide();
		}
	}

	if (type <= 1)
	{
		gl1->addWidget(new QLabel(tr("Amplitude")), 2, 0);
		boxAmplitude = new QLineEdit();
		boxAmplitude->setText(tr("1"));
		gl1->addWidget(boxAmplitude, 2, 1);
	}

	gl1->addWidget(new QLabel(tr("Y Offset")), 4, 0 );
	boxYOffset = new QLineEdit();
	boxYOffset->setText(tr("0"));
	gl1->addWidget(boxYOffset, 4, 1);

    gl1->addWidget(new QLabel(tr("Initial time")), 5, 0);

	boxStart = new QLineEdit();
	boxStart->setText(tr("0"));
	gl1->addWidget(boxStart, 5, 1);

	gl1->addWidget(new QLabel(tr("Color")), 6, 0 );
	boxColor = new ColorBox(false);
	boxColor->setColor(QColor(Qt::red));
	gl1->addWidget(boxColor, 6, 1);

	gb1->setLayout(gl1);

	buttonFit = new QPushButton(tr("&Fit"));
	buttonFit->setDefault(true);

	buttonCancel = new QPushButton(tr("&Close"));

	QBoxLayout *bl1 = new QBoxLayout (QBoxLayout::TopToBottom);
	bl1->addWidget(buttonFit);
	bl1->addWidget(buttonCancel);
	bl1->addStretch();

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->addWidget(gb1);
	hlayout->addLayout(bl1);
	setLayout(hlayout);

	// signals and slots connections
	connect( buttonFit, SIGNAL( clicked() ), this, SLOT(fit()));
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT(close()));
}

void ExpDecayDialog::setGraph(Graph *g)
{
	if (!g)
		return;

    fitter = 0;
	graph = g;

	boxName->addItems(graph->analysableCurvesList());

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

void ExpDecayDialog::activateCurve(const QString& curveName)
{
	QwtPlotCurve *c = graph->curve(curveName);
	if (!c)
		return;

    ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (!app)
        return;

	int precision = app->fit_output_precision;
	double start, end;
	graph->range(graph->curveIndex(curveName), &start, &end);
	boxStart->setText(QString::number(QMIN(start, end)));
	boxYOffset->setText(QString::number(c->minYValue(), 'g', precision));
	if (slopes < 2)
        boxAmplitude->setText(QString::number(c->maxYValue() - c->minYValue(), 'g', precision));

};

void ExpDecayDialog::changeDataRange()
{
double start = graph->selectedXStartValue();
double end = graph->selectedXEndValue();
boxStart->setText(QString::number(QMIN(start, end), 'g', 15));
}

void ExpDecayDialog::fit()
{
	QString curve = boxName->currentText();
	QwtPlotCurve *c = graph->curve(curve);
	QStringList curvesList = graph->analysableCurvesList();
	if (!c || !curvesList.contains(curve))
	{
		QMessageBox::critical(this,tr("QtiPlot - Warning"),
				tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!").arg(curve));
		boxName->clear();
		boxName->addItems(curvesList);
		return;
	}

	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (!app)
        return;

	int precision = app->fit_output_precision;

	if (fitter)
        delete fitter;

	if (slopes == 3)
	{
		double x_init[7] = {1.0, boxFirst->text().toDouble(), 1.0, boxSecond->text().toDouble(),
			1.0, boxThird->text().toDouble(), boxYOffset->text().toDouble()};
		fitter = new ThreeExpFit(app, graph);
		fitter->setInitialGuesses(x_init);
	}
	else if (slopes == 2)
	{
		double x_init[5] = {1.0, boxFirst->text().toDouble(), 1.0, boxSecond->text().toDouble(),
			boxYOffset->text().toDouble()};
		fitter = new TwoExpFit(app, graph);
		fitter->setInitialGuesses(x_init);
	}
	else if (slopes == 1 || slopes == -1)
	{
		double x_init[3] = {boxAmplitude->text().toDouble(), slopes/boxFirst->text().toDouble(), boxYOffset->text().toDouble()};
		fitter = new ExponentialFit(app, graph, slopes == -1);
		fitter->setInitialGuesses(x_init);
	}

  	if (fitter->setDataFromCurve(boxName->currentText(), boxStart->text().toDouble(), c->maxXValue()))
	{
		fitter->setColor(boxColor->currentItem());
		fitter->generateFunction(app->generateUniformFitPoints, app->fitPoints);
		fitter->fit();

		double *results = fitter->results();
		boxFirst->setText(QString::number(results[1], 'g', precision));
		if (slopes < 2)
		{
            boxAmplitude->setText(QString::number(results[0], 'g', precision));
            boxYOffset->setText(QString::number(results[2], 'g', precision));
        }
        else if (slopes == 2)
        {
            boxSecond->setText(QString::number(results[3], 'g', precision));
            boxYOffset->setText(QString::number(results[4], 'g', precision));
        }
        else if (slopes == 3)
        {
            boxSecond->setText(QString::number(results[3], 'g', precision));
            boxThird->setText(QString::number(results[5], 'g', precision));
            boxYOffset->setText(QString::number(results[6], 'g', precision));
        }
	}
}

void ExpDecayDialog::closeEvent (QCloseEvent * e )
{
	if(fitter)
	{
        ApplicationWindow *app = (ApplicationWindow *)this->parent();
        if (app && app->pasteFitResultsToPlot)
            fitter->showLegend();

        delete fitter;
	}

	e->accept();
}
