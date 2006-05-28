/***************************************************************************
    File                 : fftDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Fast Fourier transform options dialog
                           
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
#include "fftDialog.h"
#include "graph.h"
#include "parser.h"
#include "application.h"
#include "multilayer.h"
#include "worksheet.h"
#include "plot.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <q3hbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
#include <q3vbox.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>

FFTDialog::FFTDialog(int type, QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "FFTDialog" );
	setWindowTitle(tr("QtiPlot - FFT Options"));
	
	d_table = 0;
	graph = 0;
	d_type = type;

	Q3VBox *box = new Q3VBox (this, "vbox");
	box->setSpacing(5);
	box->setMargin(5);

	Q3ButtonGroup *GroupFFT = new Q3ButtonGroup(2,Qt::Horizontal,tr(""), box, "GroupBox3" );
	GroupFFT->setRadioButtonExclusive ( TRUE );
	
    forwardBtn = new QRadioButton(GroupFFT, "forwardBtn" );
	forwardBtn->setChecked( TRUE );

    backwardBtn = new QRadioButton(GroupFFT, "backwardBtn" );

	Q3ButtonGroup *GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""), box,"GroupBox1" );

	if (d_type == onGraph)
		new QLabel( tr("Curve"), GroupBox1, "TextLabel1",0 );
	else
		new QLabel( tr("Sampling"), GroupBox1, "TextLabel1",0 );

	boxName = new QComboBox(GroupBox1, "boxShow" );
	
	if (d_type == onTable)
		{
		new QLabel( tr("Real"), GroupBox1, "TextLabel11",0 );
		boxReal = new QComboBox(GroupBox1, "boxReal" );

		new QLabel( tr("Imaginary"), GroupBox1, "TextLabel111",0 );
		boxImaginary = new QComboBox(GroupBox1, "Imaginary" );
		}

	new QLabel( tr("Sampling Interval"), GroupBox1, "TextLabel4",0 );
	boxSampling = new QLineEdit(GroupBox1, "boxStart" );
	
	boxNormalize = new QCheckBox(box, "boxNorm" );
	boxNormalize->setChecked(true);

	boxOrder = new QCheckBox(box, "boxOrder" );
	boxOrder->setChecked(true);

	Q3ButtonGroup *GroupBox2 = new Q3ButtonGroup(1,Qt::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);

	buttonOK = new QPushButton(GroupBox2, "buttonFit" );
    buttonOK->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
	
	Q3HBoxLayout* hlayout = new Q3HBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(box);
	hlayout->addWidget(GroupBox2);

    languageChange();
	setFocusProxy(boxName);
   
    // signals and slots connections
	connect( boxName, SIGNAL( activated(int) ), this, SLOT( activateCurve(int) ) );
	connect( buttonOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

FFTDialog::~FFTDialog()
{
}


void FFTDialog::languageChange()
{
boxNormalize->setText( tr( "&Normalize Amplitude" ) );
boxOrder->setText( tr( "&Shift Results" ) );
backwardBtn->setText( tr( "&Inverse" ) );
forwardBtn->setText( tr( "&Forward" ) );
buttonOK->setText( tr( "&OK" ) );
buttonCancel->setText( tr( "&Close" ) );
}

void FFTDialog::accept()
{
double sampling;
try
	{
	myParser parser;
	parser.SetExpr(boxSampling->text().ascii());
	sampling=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(this, tr("QtiPlot - Sampling value error"), QString::fromStdString(e.GetMsg()));
	boxSampling->setFocus();
	return;
	}		

ApplicationWindow *app = (ApplicationWindow *)this->parent();
if (graph)
	{
	long key = graph->curveKey(boxName->currentItem());
	if (key < 0)
		return;

	graph->fft(key, forwardBtn->isChecked(), sampling, 
		   boxNormalize->isChecked(), boxOrder->isChecked());

	int fitID = app->fitNumber;
	graph->setFitID(++fitID);
	app->fitNumber = fitID;
	}
else
	{
	if (boxReal->currentText().isEmpty())
		{
		QMessageBox::critical(this, tr("QtiPlot - Error"), tr("Please choose a column for the real part of the data!"));
		boxReal->setFocus();
		return;
		}
	else
		d_table->fft(sampling, boxReal->currentText(), boxImaginary->currentText(),
				forwardBtn->isChecked(), boxNormalize->isChecked(), boxOrder->isChecked());
	}

Table* w = app->table(app->tableWindows.last());
QStringList list;
list<<QString(w->name())+"_Amplitude";

MultiLayer* d= app->multilayerPlot(w,list,0);
if (!d)
	{
	close();
	return;
	}

Graph* g = d->activeGraph();
if ( g )
	{
	g->removeLegend();

	Plot* plot = g->plotWidget();
	plot->setTitle(QString::null);
	if (forwardBtn->isChecked())
		plot->setAxisTitle(QwtPlot::xBottom, tr("Frequency")+" (Hz)");
	else
		plot->setAxisTitle(QwtPlot::xBottom, tr("Time")+" (s)");

	plot->setAxisTitle(QwtPlot::yLeft, tr("Amplitude"));
	plot->replot();
	}

d->showMaximized();
close();
}

void FFTDialog::setGraph(Graph *g)
{
graph = g;
boxName->insertStringList (g->curvesList(),-1);
activateCurve(0);
};

void FFTDialog::activateCurve(int index)
{
if (graph)
	{
	QwtPlotCurve *c = graph->curve(index);
	if (!c)
		return;

	boxSampling->setText(QString::number(c->x(1) - c->x(0)));
	}
else if (d_table)
	{
	double x0 = d_table->text(0, index).toDouble();
	double x1 = d_table->text(1, index).toDouble();
	boxSampling->setText(QString::number(x1 - x0));
	}
};

void FFTDialog::setTable(Table *t)
{
d_table = t;
QStringList l = t->columnsList();
boxName->insertStringList (l);
boxReal->insertStringList (l);
boxImaginary->insertStringList (l);

int xcol = t->firstXCol();
if (xcol >= 0)
	{
	boxName->setCurrentItem(xcol);

	double x0 = t->text(0, xcol).toDouble();
	double x1 = t->text(1, xcol).toDouble();
	boxSampling->setText(QString::number(x1 - x0));
	}

l = t->selectedColumns();
int selected = (int)l.size();
if (!selected)
	{
	boxReal->setCurrentText(QString::null);
	boxImaginary->setCurrentText(QString::null);
	}
else if (selected == 1)
	{
	boxReal->setCurrentItem(t->colIndex(l[0]));
	boxImaginary->setCurrentText(QString::null);
	}
else
	{
	boxReal->setCurrentItem(t->colIndex(l[0]));
	boxImaginary->setCurrentItem(t->colIndex(l[1]));
	}
};
