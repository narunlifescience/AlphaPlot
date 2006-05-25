#include "interpolationDialog.h"
#include "graph.h"
#include "parser.h"
#include "colorBox.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmessagebox.h>

interpolationDialog::interpolationDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "interpolationDialog" );
	setCaption(tr("QtiPlot - Interpolation Options"));
	
	QButtonGroup *GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox1" );

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
	boxColor->setColor(QColor(red));

	QButtonGroup *GroupBox2 = new QButtonGroup(1,QGroupBox::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	GroupBox2->setLineWidth (0);
	
	buttonFit = new QPushButton(GroupBox2, "buttonFit" );
    buttonFit->setAutoDefault( TRUE );
    buttonFit->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
	QHBoxLayout* hlayout = new QHBoxLayout(this,5,5, "hlayout");
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
	QMessageBox::critical(this, tr("QtiPlot - Start limit error"), e.GetMsg());
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
	QMessageBox::critical(this, tr("QtiPlot - End limit error"), e.GetMsg());
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

