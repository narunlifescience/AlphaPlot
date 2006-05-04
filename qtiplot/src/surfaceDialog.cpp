#include "surfaceDialog.h"
#include "parser.h"
#include "application.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>

sDialog::sDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "sDialog" );
	setCaption(tr("QtiPlot - Define surface plot"));
    setMinimumSize( QSize( 310, 140 ) );
	setMaximumSize( QSize( 310, 140 ) );
    setMouseTracking( TRUE );
    setSizeGripEnabled( FALSE );
	
	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox1" );
	GroupBox1->setFlat (TRUE);
	GroupBox1->setLineWidth (0);

	new QLabel( tr("f(x,y)="), GroupBox1, "TextLabel2",0 );
	boxFunction = new QComboBox(GroupBox1, "boxFunction" );
	boxFunction->setFixedWidth(250);
	boxFunction->setEditable(TRUE);

	GroupBox5 = new QButtonGroup(3,QGroupBox::Horizontal,tr(""),this,"GroupBox5" );
	GroupBox5->setFlat (TRUE);
	GroupBox5->setLineWidth (0);

	GroupBox3 = new QButtonGroup(1,QGroupBox::Horizontal,tr("X - axis"),GroupBox5,"GroupBox3" );
	
	new QLabel( tr("From"), GroupBox3, "TextLabel3",0 );
	boxXFrom = new QLineEdit(GroupBox3, "boxPoints" );
	boxXFrom->setText(tr("-1"));

	new QLabel( tr("To"), GroupBox3, "TextLabel5",0 );
	boxXTo = new QLineEdit(GroupBox3, "boxOrder" );
	boxXTo->setText(tr("1"));

	GroupBox4 = new QButtonGroup(1,QGroupBox::Horizontal,tr("Y - axis"),GroupBox5,"GroupBox4" );
	
	new QLabel( tr("From"), GroupBox4, "TextLabel33",0 );
	boxYFrom = new QLineEdit(GroupBox4, "boxPoints" );
	boxYFrom->setText(tr("-1"));

	new QLabel( tr("To"), GroupBox4, "TextLabel5",0 );
	boxYTo = new QLineEdit(GroupBox4, "boxYto" );
	boxYTo->setText(tr("1"));

	GroupBox6 = new QButtonGroup(1,QGroupBox::Horizontal,tr("Z - axis"),GroupBox5,"GroupBox4" );
	
	new QLabel( tr("From"), GroupBox6, "TextLabel35",0 );
	boxZFrom = new QLineEdit(GroupBox6, "boxZFrom" );
	boxZFrom->setText(tr("-1"));

	new QLabel( tr("To"), GroupBox6, "TextLabel5",0 );
	boxZTo = new QLineEdit(GroupBox6, "boxZto" );
	boxZTo->setText(tr("1"));

	GroupBox2 = new QButtonGroup(3,QGroupBox::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	GroupBox2->setLineWidth (0);

	buttonClear = new QPushButton(GroupBox2, "buttonClear" );
    buttonClear->setText( tr( "Clear &list" ) );
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setText( tr( "&Cancel" ) );
	
	QVBoxLayout* hlayout = new QVBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox5);
	hlayout->addWidget(GroupBox2);
   
    // signals and slots connections
	connect( buttonClear, SIGNAL( clicked() ), this, SLOT(clearList() ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

	setFocusProxy(boxFunction);
}

void sDialog::clearList()
{
boxFunction->clear();
emit clearFunctionsList();
}

void sDialog::setFunction(const QString& s)
{
boxFunction->setCurrentText(s);
}

void sDialog::setLimits(double xs, double xe, double ys, double ye, double zs, double ze)
{
	boxXFrom->setText(QString::number(xs));
	boxXTo->setText(QString::number(xe));
	boxYFrom->setText(QString::number(ys));
	boxYTo->setText(QString::number(ye));
	boxZFrom->setText(QString::number(zs));
	boxZTo->setText(QString::number(ze));
}

void sDialog::accept()
{
QString Xfrom=boxXFrom->text().lower();
QString Xto=boxXTo->text().lower();
QString Yfrom=boxYFrom->text().lower();
QString Yto=boxYTo->text().lower();
QString Zfrom=boxZFrom->text().lower();
QString Zto=boxZTo->text().lower();

double fromX, toX, fromY,toY, fromZ,toZ;
try
	{
	myParser parser;	
	parser.SetExpr(Xfrom.ascii());
	fromX=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - X Start limit error"), e.GetMsg());
	boxXFrom->setFocus();
	return;
	}
try
	{
	myParser parser;
	parser.SetExpr(Xto.ascii());
	toX=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - X End limit error"), e.GetMsg());
	boxXTo->setFocus();
	return;
	}	

try
	{
	myParser parser;
	parser.SetExpr(Yfrom.ascii());
	fromY=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Y Start limit error"), e.GetMsg());
	boxYFrom->setFocus();
	return;
	}	
try
	{
	myParser parser;	
	parser.SetExpr(Yto.ascii());
	toY=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Y End limit error"), e.GetMsg());
	boxYTo->setFocus();
	return;
	}	
try
	{
	myParser parser;
	parser.SetExpr(Zfrom.ascii());
	fromZ=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Z Start limit error"), e.GetMsg());
	boxZFrom->setFocus();
	return;
	}	
try
	{
	myParser parser;
	parser.SetExpr(Zto.ascii());
	toZ=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Z End limit error"), e.GetMsg());
	boxZTo->setFocus();
	return;
	}

if (fromX >= toX || fromY >= toY || fromZ >= toZ)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input error"),
				tr("Please enter limits that satisfy: from < end!"));
	boxXTo->setFocus();
	return;
	}

double x,y;
QString formula=boxFunction->currentText();
bool error=FALSE;
try
	{
	myParser parser;
	parser.DefineVar("x", &x);	
	parser.DefineVar("y", &y);		
	parser.SetExpr(formula.ascii());
		
	x=fromX; y=fromY;
	parser.Eval();
	x=toX; y=toY;
	parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input function error"), e.GetMsg());
	boxFunction->setFocus();
	error=TRUE;	
	}
	
if (!error)
	{
	emit options(boxFunction->currentText(),fromX, toX, fromY, toY, fromZ, toZ);
	emit custom3DToolBar();

	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	app->updateSurfaceFuncList(boxFunction->currentText());
	close();
	}
}

void sDialog::insertFunctionsList(const QStringList& list)
{
boxFunction->insertStringList (list, 1);
}

sDialog::~sDialog()
{
}
