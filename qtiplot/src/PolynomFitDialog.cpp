/***************************************************************************
    File                 : PolynomFitDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit polynomial dialog

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
#include "PolynomFitDialog.h"
#include "Graph.h"
#include "ColorBox.h"
#include "ApplicationWindow.h"
#include "PolynomialFit.h"

#include <QSpinBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

PolynomFitDialog::PolynomFitDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
: QDialog( parent, name, modal, fl )
{
	if ( !name )
		setName( "PolynomFitDialog" );
	setWindowTitle(tr("QtiPlot - Polynomial Fit Options"));
    setSizeGripEnabled( true );

    QGroupBox *gb1 = new QGroupBox();
	QGridLayout *gl1 = new QGridLayout(gb1);
	gl1->addWidget(new QLabel(tr("Polynomial Fit of")), 0, 0);

	boxName = new QComboBox();
    gl1->addWidget(boxName, 0, 1);

    gl1->addWidget(new QLabel( tr("Order (1 - 9, 1 = linear)")), 1, 0);
	boxOrder = new QSpinBox();
    boxOrder->setRange(1, 9);
	boxOrder->setValue(2);
    gl1->addWidget(boxOrder, 1, 1);

    gl1->addWidget(new QLabel( tr("Fit curve # pts")), 2, 0);
	boxPoints = new QSpinBox();
    boxPoints->setRange(1, 1000);
    boxPoints->setSingleStep(50);
    boxPoints->setSpecialValueText(tr("Not enough points"));
    gl1->addWidget(boxPoints, 2, 1);

    gl1->addWidget(new QLabel( tr("Fit curve Xmin")), 3, 0);
	boxStart = new QLineEdit(tr("0"));
    gl1->addWidget(boxStart, 3, 1);

    gl1->addWidget(	new QLabel( tr("Fit curve Xmax")), 4, 0);
	boxEnd = new QLineEdit();
    gl1->addWidget(boxEnd, 4, 1);

    gl1->addWidget(new QLabel( tr("Color")), 5, 0);
	boxColor = new ColorBox( false);
	boxColor->setColor(QColor(Qt::red));
    gl1->addWidget(boxColor, 5, 1);

	boxShowFormula = new QCheckBox(tr( "Show Formula on Graph?" ));
	boxShowFormula->setChecked( false );
    gl1->addWidget(boxShowFormula, 6, 1);
    gl1->setRowStretch(7, 1);

	buttonFit = new QPushButton(tr( "&Fit" ));
	buttonFit->setDefault( true );

	buttonCancel = new QPushButton(tr( "&Close" ));

    QVBoxLayout* vl = new QVBoxLayout();
    vl->addWidget(buttonFit);
    vl->addWidget(buttonCancel);
    vl->addStretch();

	QHBoxLayout* hlayout = new QHBoxLayout(this);
	hlayout->addWidget(gb1);
	hlayout->addLayout(vl);

	languageChange();
	resize(minimumSize());

	connect( buttonFit, SIGNAL( clicked() ), this, SLOT( fit() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( boxName, SIGNAL( activated(const QString&) ), this, SLOT(activateCurve(const QString&)));
}

void PolynomFitDialog::fit()
{
	QString curveName = boxName->currentText();
	QStringList curvesList = graph->analysableCurvesList();
	if (!curvesList.contains(curveName))
	{
		QMessageBox::critical(this, tr("QtiPlot - Warning"),
				tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!").arg(curveName));
		boxName->clear();
		boxName->insertStringList(curvesList);
		return;
	}

	ApplicationWindow *app = (ApplicationWindow *)this->parent();
    PolynomialFit *fitter = new PolynomialFit(app, graph, boxOrder->value(), boxShowFormula->isChecked());
    if (fitter->setDataFromCurve(curveName, boxStart->text().toDouble(), boxEnd->text().toDouble()))
    {
        fitter->setColor(boxColor->currentItem());
        fitter->scaleErrors(app->fit_scale_errors);
        fitter->setOutputPrecision(app->fit_output_precision);
		fitter->generateFunction(app->generateUniformFitPoints, app->fitPoints);
        fitter->fit();
        delete fitter;
	}
}

void PolynomFitDialog::setGraph(Graph *g)
{
	graph = g;
	boxName->addItems (g->analysableCurvesList());

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

void PolynomFitDialog::activateCurve(const QString& curveName)
{
	double start, end;
	int n_points = graph->range(graph->curveIndex(curveName), &start, &end);

	boxStart->setText(QString::number(start, 'g', 15));
	boxEnd->setText(QString::number(end, 'g', 15));
	boxPoints->setValue(QMAX(n_points, 100));
};

void PolynomFitDialog::changeDataRange()
{
	double start = graph->selectedXStartValue();
	double end = graph->selectedXEndValue();
	boxStart->setText(QString::number(QMIN(start, end), 'g', 15));
	boxEnd->setText(QString::number(QMAX(start, end), 'g', 15));
}
