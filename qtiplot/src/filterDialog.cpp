#include "filterDialog.h"
#include "graph.h"
#include "parser.h"
#include "colorBox.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmessagebox.h>

filterDialog::filterDialog(int type, QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	filter_type = type;

    if ( !name )
		setName( "filterDialog" );

	QButtonGroup *GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox1" );

	new QLabel( tr("Filter curve: "), GroupBox1, "TextLabel1",0 );
	boxName = new QComboBox(GroupBox1, "boxShow" );
	
	if (type <= HighPass)
		new QLabel( tr("Frequency cutoff (Hz)"), GroupBox1, "TextLabel4",0 );
	else
		new QLabel( tr("Low Frequency (Hz)"), GroupBox1, "TextLabel4",0 );

	boxStart = new QLineEdit(GroupBox1, "boxStart" );
	boxStart->setText(tr("0"));
	
	if (type >= BandPass)
		{
		new QLabel( tr("High Frequency (Hz)"), GroupBox1, "TextLabel5",0 );
		boxEnd = new QLineEdit(GroupBox1, "boxEnd" );
		boxEnd->setText(tr("0"));

		if (type == BandPass)
			new QLabel(tr("Add DC Offset"), GroupBox1, "TextLabel52",0 );
		else
			new QLabel(tr("Substract DC Offset"), GroupBox1, "TextLabel52",0 );

		boxOffset = new QCheckBox(GroupBox1, "boxOffset" );
		}

	new QLabel( tr("Color"), GroupBox1, "TextLabel52",0 );
	boxColor = new ColorBox( FALSE, GroupBox1);
	boxColor->setColor(QColor(red));

	QHBox *hbox1=new QHBox (this,"hbox1");	
	hbox1->setMargin(5);
	hbox1->setSpacing(5);

	buttonFilter = new QPushButton(hbox1, "buttonFit" );
    buttonFilter->setDefault( TRUE );
   
    buttonCancel = new QPushButton(hbox1, "buttonCancel" );
	
	QVBoxLayout* hlayout = new QVBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(hbox1);

    languageChange();
   
    // signals and slots connections
	connect( buttonFilter, SIGNAL( clicked() ), this, SLOT( filter() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

filterDialog::~filterDialog()
{
}


void filterDialog::languageChange()
{
setCaption(tr("QtiPlot - Filter options"));
buttonFilter->setText( tr( "&Filter" ) );
buttonCancel->setText( tr( "&Close" ) );
}

void filterDialog::filter()
{
double from = 0.0, to = 0.0;
try
	{
	myParser parser;
	parser.SetExpr(boxStart->text().ascii());
	from=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(this, tr("QtiPlot - Frequency input error"), e.GetMsg());
	boxStart->setFocus();
	return;
	}		

if (from < 0)
		{
		QMessageBox::critical(this, tr("QtiPlot - Frequency input error"),
				tr("Please enter positive frequency values!"));
		boxStart->setFocus();
		return;
		}

if (filter_type >= BandPass)
	{	
	try
		{
		myParser parser;	
		parser.SetExpr(boxEnd->text().ascii());
		to=parser.Eval();
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical(this, tr("QtiPlot - High Frequency input error"), e.GetMsg());
		boxEnd->setFocus();
		return;
		}	

	if (to < 0)
		{
		QMessageBox::critical(this, tr("QtiPlot - High Frequency input error"),
				tr("Please enter positive frequency values!"));
		boxEnd->setFocus();
		return;
		}

	if (from>=to)
		{
		QMessageBox::critical(this, tr("QtiPlot - Frequency input error"),
				tr("Please enter frequency limits that satisfy: Low < High !"));
		boxEnd->setFocus();
		return;
		}
	}

long key = graph->curveKey(boxName->currentItem());
if (key < 0)
	return;

if (filter_type >= BandPass)
 graph->filterFFT(key, filter_type, from, to, boxOffset->isChecked(), boxColor->currentItem());
else
 graph->filterFFT(key, filter_type, from, to, false, boxColor->currentItem());

}

void filterDialog::setGraph(Graph *g)
{
graph = g;
boxName->insertStringList (g->curvesList(),-1);
};


