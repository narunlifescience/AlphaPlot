#include "smoothCurveDialog.h"
#include "graph.h"
#include "parser.h"
#include "colorBox.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmessagebox.h>

smoothCurveDialog::smoothCurveDialog(int method, QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	smooth_type = method;

    if ( !name )
		setName( "smoothCurveDialog" );
	setCaption(tr("QtiPlot - Smoothing Options"));
	
	QButtonGroup *GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox1" );

	new QLabel( tr("Curve"), GroupBox1, "TextLabel1",0 );
	boxName = new QComboBox(GroupBox1, "boxShow" );
	
	if (method == SavitzkyGolay)
		{
		new QLabel( tr("Polynomial Order"), GroupBox1, "TextLabel2",0 );
		boxOrder = new QSpinBox(0,9,1,GroupBox1, "boxOrder" );
		boxOrder->setValue(2);

		new QLabel( tr("Points to the Left"), GroupBox1, "TextLabel3",0 );
		boxPointsLeft = new QSpinBox(1,25,1,GroupBox1, "boxPointsLeft" );
		boxPointsLeft->setValue(2);

		new QLabel( tr("Points to the Right"), GroupBox1, "TextLabel3",0 );
		boxPointsRight = new QSpinBox(1,25,1,GroupBox1, "boxPointsRight" );
		boxPointsRight->setValue(2);
		}
	else 
		{
		new QLabel( tr("Points"), GroupBox1, "TextLabel3",0 );
		boxPointsLeft = new QSpinBox(1,1000000,10,GroupBox1, "boxPointsLeft" );
		boxPointsLeft->setValue(5);
		}

	new QLabel( tr("Color"), GroupBox1, "TextLabel52",0 );
	boxColor = new ColorBox( FALSE, GroupBox1);
	boxColor->setColor(QColor(red));

	QButtonGroup *GroupBox2 = new QButtonGroup(1,QGroupBox::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	GroupBox2->setLineWidth (0);
	
	btnSmooth = new QPushButton(GroupBox2, "btnSmooth" );
    btnSmooth->setAutoDefault( TRUE );
    btnSmooth->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
	QHBoxLayout* hlayout = new QHBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox2);

    languageChange();
   
    // signals and slots connections
	connect( btnSmooth, SIGNAL( clicked() ), this, SLOT( smooth() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( boxName, SIGNAL( activated(int) ), this, SLOT( activateCurve(int) ) );
}

smoothCurveDialog::~smoothCurveDialog()
{
}


void smoothCurveDialog::languageChange()
{
btnSmooth->setText( tr( "&Smooth" ) );
buttonCancel->setText( tr( "&Close" ) );
}

void smoothCurveDialog::smooth()
{
long key = graph->curveKey(boxName->currentItem());
if (key < 0)
	return;

if (smooth_type == SavitzkyGolay)
	graph->smoothSavGol(key, boxOrder->value(), boxPointsLeft->value(),
				   boxPointsRight->value(), boxColor->currentItem());
else if (smooth_type == FFT)
	graph->smoothFFT(key, boxPointsLeft->value(), boxColor->currentItem());
else if (smooth_type == Average)
	graph->smoothAverage(key, boxPointsLeft->value(), boxColor->currentItem());
}

void smoothCurveDialog::setGraph(Graph *g)
{
graph = g;
boxName->insertStringList (g->curvesList(),-1);
activateCurve(0);
}

void smoothCurveDialog::activateCurve(int index)
{
if (smooth_type == Average)
	{
	QwtPlotCurve *c = graph->curve(index);
	if (!c)
		return;

	boxPointsLeft->setMaxValue(c->dataSize()/2);
	}
}


