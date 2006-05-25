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
#include <qhbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qmessagebox.h>

FFTDialog::FFTDialog(int type, QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "FFTDialog" );
	setCaption(tr("QtiPlot - FFT Options"));
	
	d_table = 0;
	graph = 0;
	d_type = type;

	QVBox *box = new QVBox (this, "vbox");
	box->setSpacing(5);
	box->setMargin(5);

	QButtonGroup *GroupFFT = new QButtonGroup(2,QGroupBox::Horizontal,tr(""), box, "GroupBox3" );
	GroupFFT->setRadioButtonExclusive ( TRUE );
	
    forwardBtn = new QRadioButton(GroupFFT, "forwardBtn" );
	forwardBtn->setChecked( TRUE );

    backwardBtn = new QRadioButton(GroupFFT, "backwardBtn" );

	QButtonGroup *GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""), box,"GroupBox1" );

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

	QButtonGroup *GroupBox2 = new QButtonGroup(1,QGroupBox::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	GroupBox2->setLineWidth (0);

	buttonOK = new QPushButton(GroupBox2, "buttonFit" );
    buttonOK->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
	
	QHBoxLayout* hlayout = new QHBoxLayout(this,5,5, "hlayout");
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
	QMessageBox::critical(this, tr("QtiPlot - Sampling value error"), e.GetMsg());
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
