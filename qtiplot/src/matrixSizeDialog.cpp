#include "matrixSizeDialog.h"
#include "parser.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qspinbox.h>
#include <qhbox.h>
#include <qlineedit.h>
#include <qmessagebox.h>

matrixSizeDialog::matrixSizeDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "matrixSizeDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
		
	QButtonGroup *GroupBox1 = new QButtonGroup(4, QGroupBox::Horizontal,tr("Dimensions"),this,"GroupBox1" );
	new QLabel( tr( "Rows" ), GroupBox1, "TextLabel2",0 );
    boxRows = new QSpinBox(0,1000000,1, GroupBox1, "boxRows" );

	new QLabel( tr( "Columns" ), GroupBox1, "TextLabel2",0 );
    boxCols = new QSpinBox(0,1000000,1, GroupBox1, "boxCols" );

	QButtonGroup *GroupBox2 = new QButtonGroup(3,QGroupBox::Horizontal,tr("Coordinates"),this,"GroupBox2" );
	new QLabel(GroupBox2, "TextLabel2",0 );
	new QLabel( tr( "X (Columns)" ), GroupBox2, "TextLabel2",0 );
	new QLabel( tr( "Y (Rows)" ), GroupBox2, "TextLabel2",0 );

	new QLabel( tr( "First" ), GroupBox2, "TextLabel2",0 );
    boxXStart = new QLineEdit(GroupBox2, "boxXStart" );
    boxYStart = new QLineEdit(GroupBox2, "boxYStart" );

	new QLabel( tr( "Last" ), GroupBox2, "TextLabel2",0 );
    boxXEnd = new QLineEdit(GroupBox2, "boxXStart" );
    boxYEnd = new QLineEdit(GroupBox2, "boxYStart" );

	QHBox  *hbox1=new QHBox (this, "hbox1"); 
	hbox1->setMargin (5);
	hbox1->setSpacing (5);
	
	buttonOk = new QPushButton(hbox1, "buttonOk" );
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(hbox1, "buttonCancel" );
	
	QVBoxLayout* hlayout = new QVBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox2);
	hlayout->addWidget(hbox1);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

matrixSizeDialog::~matrixSizeDialog()
{
}

void matrixSizeDialog::setColumns(int c)
{
boxCols->setValue(c);
}

void matrixSizeDialog::setRows(int r)
{
boxRows->setValue(r);
}

void matrixSizeDialog::setCoordinates(double xs, double xe, double ys, double ye)
{
boxXStart->setText(QString::number(xs, 'g', 6));
boxYStart->setText(QString::number(ys, 'g', 6));
boxXEnd->setText(QString::number(xe, 'g', 6));
boxYEnd->setText(QString::number(ye, 'g', 6));
}

void matrixSizeDialog::languageChange()
{
    setCaption(tr("QtiPlot - Matrix Dimensions"));
    buttonOk->setText(tr("&OK"));
	buttonCancel->setText(tr("&Cancel"));
}

void matrixSizeDialog::accept()
{
double fromX, toX, fromY, toY;
myParser parser;	
try
	{
	parser.SetExpr(boxXStart->text().lower().ascii());
	fromX=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input error"), e.GetMsg());
	boxXStart->setFocus();
	return;
	}
try
	{
	parser.SetExpr(boxXEnd->text().lower().ascii());
	toX=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input error"), e.GetMsg());
	boxXEnd->setFocus();
	return;
	}
try
	{
	parser.SetExpr(boxYStart->text().lower().ascii());
	fromY=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input error"), e.GetMsg());
	boxYStart->setFocus();
	return;
	}
try
	{
	parser.SetExpr(boxYEnd->text().lower().ascii());
	toY=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input error"), e.GetMsg());
	boxYEnd->setFocus();
	return;
	}

emit changeDimensions(boxRows->value(), boxCols->value());
emit changeCoordinates(fromX, toX, fromY, toY);
close();
}
