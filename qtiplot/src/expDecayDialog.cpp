#include "expDecayDialog.h"
#include "graph.h"
#include "colorBox.h"
#include "application.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qmessagebox.h>

expDecayDialog::expDecayDialog(int type, QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "expDecayDialog" );
	
	slopes = type;
	
	setCaption(tr("QtiPlot - Verify initial guesses"));
    setMinimumSize( QSize( 310, 140 ) );
	setMaximumSize( QSize( 310, 140 ) );
	
	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox1" );

	new QLabel( tr("Exponential Fit of"), GroupBox1, "TextLabel1",0 );
	boxName = new QComboBox(GroupBox1, "boxName" );
	
	if (type < 0)
		dampingLabel = new QLabel( tr("Growth time"), GroupBox1, "TextLabel2",0 );
	else if (type == 1)
		dampingLabel = new QLabel( tr("Damping"), GroupBox1, "TextLabel2",0 );
	else
		dampingLabel = new QLabel( tr("First decay time (t1)"), GroupBox1, "TextLabel2",0 );
	
	boxFirst = new QLineEdit(GroupBox1, "boxOrder" );
	boxFirst->setText(tr("1"));
	
	if (type > 1)
	{
	new QLabel( tr("Second decay time (t2)"), GroupBox1, "TextLabel3",0 );
	boxSecond = new QLineEdit(GroupBox1, "boxPoints" );
	boxSecond->setText(tr("1"));
	
	thirdLabel=new QLabel( tr("Third decay time (t3)"), GroupBox1, "TextLabel5",0 );
	
	boxThird = new QLineEdit(GroupBox1, "boxOrder" );
	boxThird->setText(tr("1"));

	if (type < 3)
		{
		thirdLabel->hide();
		boxThird->hide();
		}
	}
	
	if (type <= 1)
		new QLabel( tr("Amplitude"), GroupBox1, "TextLabel4",0 );
	else
		new QLabel( tr("Initial time"), GroupBox1, "TextLabel4",0 );
	
	boxStart = new QLineEdit(GroupBox1, "boxStart" );
	
	if (type == 1)
		boxStart->setText(tr("1"));
	else
		boxStart->setText(tr("0"));
	
	new QLabel( tr("Y Offset"), GroupBox1, "TextLabel5",0 );
	boxYOffset = new QLineEdit(GroupBox1, "boxEnd" );
	boxYOffset->setText(tr("0"));

	new QLabel( tr("Color"), GroupBox1, "TextLabel52",0 );
	boxColor = new ColorBox( FALSE, GroupBox1);
	boxColor->setColor(QColor(red));
	
	GroupBox2 = new QButtonGroup(1,QGroupBox::Horizontal,tr(""),this,"GroupBox2" );
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
	connect( buttonFit, SIGNAL( clicked() ), this, SLOT(fit()));
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT(reject()));
}


expDecayDialog::~expDecayDialog()
{
}


void expDecayDialog::languageChange()
{
	buttonFit->setText( tr( "&Fit" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
}

void expDecayDialog::setGraph(Graph *g)
{
if (!g)
	return;

graph = g;
boxName->insertStringList (graph->curvesList(),-1);

connect (graph, SIGNAL(closedGraph()), this, SLOT(close()));
};

void expDecayDialog::fit()
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

ApplicationWindow *app = (ApplicationWindow *)this->parent();
graph->setFitID(++app->fitNumber);
QString result;
if (slopes == 3)
	result = graph->fitExpDecay3(boxName->currentText(),boxFirst->text().toDouble(),
				boxSecond->text().toDouble(), boxThird->text().toDouble(),
				boxStart->text().toDouble(),boxYOffset->text().toDouble(), boxColor->currentItem());
else if (slopes == 2)
	result = graph->fitExpDecay2(boxName->currentText(),boxFirst->text().toDouble(),
				boxSecond->text().toDouble(), boxStart->text().toDouble(),
				boxYOffset->text().toDouble(), boxColor->currentItem());
else if (slopes == 1)
	result = graph->fitExpDecay(boxName->currentText(),boxFirst->text().toDouble(),
				 boxStart->text().toDouble(), boxYOffset->text().toDouble(), boxColor->currentItem());
else if (slopes == -1)
	result = graph->fitExpGrowth(boxName->currentText(),boxFirst->text().toDouble(),
				 boxStart->text().toDouble(), boxYOffset->text().toDouble(), boxColor->currentItem());

app->updateLog(result);
}


