/***************************************************************************
    File                 : polynomFitDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#include "polynomFitDialog.h"
#include "graph.h"
#include "colorBox.h"
#include "application.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>

polynomFitDialog::polynomFitDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "polynomFitDialog" );
	setWindowTitle(tr("QtiPlot - Polynomial Fit Options"));
    setSizeGripEnabled(true);
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	
	GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),this,"GroupBox1" );
	GroupBox1->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	
	new QLabel( tr("Polynomial Fit of"), GroupBox1, "TextLabel1",0 );
	boxName = new QComboBox(GroupBox1, "boxShow" );
	
	new QLabel( tr("Order (1 - 9, 1 = linear)"), GroupBox1, "TextLabel2",0 );
	boxOrder = new QSpinBox(1,9,1,GroupBox1, "boxOrder" );
	boxOrder->setValue(2);
	
	new QLabel( tr("Fit curve # pts"), GroupBox1, "TextLabel3",0 );
	boxPoints = new QSpinBox(1,1000,50,GroupBox1, "boxPoints" );
	
	new QLabel( tr("Fit curve Xmin"), GroupBox1, "TextLabel4",0 );
	boxStart = new QLineEdit(GroupBox1, "boxStart" );
	boxStart->setText(tr("0"));
	
	new QLabel( tr("Fit curve Xmax"), GroupBox1, "TextLabel5",0 );
	boxEnd = new QLineEdit(GroupBox1, "boxEnd" );

	new QLabel( tr("Color"), GroupBox1, "TextLabel52",0 );
	boxColor = new ColorBox( false, GroupBox1);
	boxColor->setColor(QColor(Qt::red));

	new QLabel( tr( "Show Formula on Graph?" ), GroupBox1, "TextLabel6",0 );
    boxShowFormula = new QCheckBox(GroupBox1, "boxShow" );
    boxShowFormula->setChecked( false );
	
	GroupBox2 = new Q3ButtonGroup(1,Qt::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (true);

	buttonFit = new QPushButton(GroupBox2, "buttonFit" );
    buttonFit->setAutoDefault( true );
    buttonFit->setDefault( true );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( true );
	
	Q3HBoxLayout* hlayout = new Q3HBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox2);

    languageChange();
	setMaximumHeight(GroupBox1->height());
   
    // signals and slots connections
	connect( buttonFit, SIGNAL( clicked() ), this, SLOT( fit() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( boxName, SIGNAL( activated(int) ), this, SLOT(activateCurve(int)));
}

polynomFitDialog::~polynomFitDialog()
{
}

void polynomFitDialog::languageChange()
{
buttonFit->setText( tr( "&Fit" ) );
buttonCancel->setText( tr( "&Close" ) );
boxPoints->setSpecialValueText(tr("Not enough points"));
}

void polynomFitDialog::fit()
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

int index = boxName->currentItem();
QwtPlotCurve *c = graph->curve(index);
if (!c || c->dataSize()<2)
	{
	QString s= tr("You can not fit curve:");
	s+="<p><b>'"+boxName->text(index)+"'</b><p>";
	s+=tr("because it has less than 2 points!");
	QMessageBox::warning(0,tr("QtiPlot - Warning"),s);

	changeCurve(index);
	}
else
	{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	app->fitNumber++;
	graph->setFitID(app->fitNumber);

	QString result = graph->fitPolynomial(boxName->currentText(),boxOrder->value(),
			boxPoints->value(), boxStart->text().toDouble(),
			boxEnd->text().toDouble(), boxShowFormula->isChecked(), 
			boxColor->currentItem());

	app->updateLog(result);
	}
}

void polynomFitDialog::setGraph(Graph *g)
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

void polynomFitDialog::activateCurve(int index)
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

boxPoints->setValue(QMAX(c->dataSize(), 100));
};

void polynomFitDialog::changeCurve(int index)
{
QwtPlotCurve *c = graph->curve(index);
while(c->dataSize()<2)
	{
	index++;
	c = graph->curve(index);
	if(!c || index >= graph->curves()) 
		index=0; //Restart from the beginning
	}
boxName->setCurrentItem(index);
activateCurve(index);
}

void polynomFitDialog::changeDataRange()
{
double start = graph->selectedXStartValue();
double end = graph->selectedXEndValue();
boxStart->setText(QString::number(QMIN(start, end), 'g', 15));
boxEnd->setText(QString::number(QMAX(start, end), 'g', 15));
}
