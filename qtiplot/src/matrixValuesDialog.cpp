#include "matrixValuesDialog.h"
#include "parser.h"

#include <qcombobox.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qbuttongroup.h>

matrixValuesDialog::matrixValuesDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "matrixValuesDialog" );

    setCaption( tr( "QtiPlot - Set Matrix Values" ) );
    setFocusPolicy( QDialog::StrongFocus );
	
	QHBox *hbox1=new QHBox (this, "hbox1"); 
	hbox1->setSpacing (5);
	
	QVBox *box1=new QVBox (hbox1, "box2"); 
	box1->setSpacing (5);

	explain = new QTextEdit(box1, "explain" );
	explain->setReadOnly (true);
	
	QVBox *box2=new QVBox (hbox1, "box2"); 
	box2->setMargin(5);
	box2->setFrameStyle (QFrame::Box);

	QButtonGroup *GroupBox1 = new QButtonGroup(4,QGroupBox::Horizontal, "",box2, "GroupBox0" );
	GroupBox1->setLineWidth(0);
	GroupBox1->setFlat(true);
	
	QLabel *TextLabel1 = new QLabel(GroupBox1, "TextLabel1" );
    TextLabel1->setText( tr( "For row (i)" ) );
	
	startRow = new QSpinBox(1, 1000000, 1, GroupBox1, "startRow" );
	startRow->setValue(1);

    QLabel *TextLabel2 = new QLabel(GroupBox1, "TextLabel2" );
    TextLabel2->setText( tr( "to" ) );

    endRow =  new QSpinBox(1, 1000000, 1, GroupBox1, "endRow" );
	
	QLabel *TextLabel3 = new QLabel(GroupBox1, "TextLabel3" );
    TextLabel3->setText( tr( "For col (j)" ) );
	
	startCol = new QSpinBox(1, 1000000, 1, GroupBox1, "startCol" );
	startCol->setValue(1);

    QLabel *TextLabel4 = new QLabel(GroupBox1, "TextLabel2" );
    TextLabel4->setText( tr( "to" ) );

    endCol = new QSpinBox(1, 1000000, 1, GroupBox1, "endCol" );

	QHBox *hbox5=new QHBox (box2, "hbox5"); 
	hbox5->setSpacing (5);
	hbox5->setMargin(5);

    functions = new QComboBox( FALSE, hbox5, "functions" );
	
	PushButton3 = new QPushButton(hbox5, "PushButton3" );
    PushButton3->setText( tr( "Add function" ) ); 

	btnAddCell = new QPushButton(hbox5, "btnAddCell" );
    btnAddCell->setText( tr( "Add Cell" ) );

	QHBox *hbox4=new QHBox (this, "hbox4"); 
	hbox4->setSpacing (5);
	
	QLabel *TextLabel5 = new QLabel(hbox4, "TextLabel2" );
    TextLabel5->setText( tr( "Cell(i,j)=" ) );

	commandes = new QTextEdit( hbox4, "commandes" );
	commandes->setTextFormat(Qt::PlainText);
    commandes->setGeometry( QRect(10, 100, 260, 70) );
	commandes->setFocus();
	
	QVBox *box3=new QVBox (hbox4,"box3"); 
	box3->setSpacing (5);
	
	btnOk = new QPushButton(box3, "btnOk" );
    btnOk->setText( tr( "OK" ) );

	btnApply = new QPushButton(box3, "btnApply" );
    btnApply->setText( tr( "Apply" ) );

    btnCancel = new QPushButton( box3, "btnCancel" );
    btnCancel->setText( tr( "Cancel" ) );
	
	QVBoxLayout* layout = new QVBoxLayout(this,5,5, "hlayout3");
    layout->addWidget(hbox1);
	layout->addWidget(hbox5);
	layout->addWidget(hbox4);

setFunctions();
insertExplain(0);

connect(btnAddCell, SIGNAL(clicked()),this, SLOT(addCell()));
connect(PushButton3, SIGNAL(clicked()),this, SLOT(insertFunction()));
connect(btnOk, SIGNAL(clicked()),this, SLOT(accept()));
connect(btnApply, SIGNAL(clicked()),this, SLOT(apply()));
connect(btnCancel, SIGNAL(clicked()),this, SLOT(close()));
connect(functions, SIGNAL(activated(int)),this, SLOT(insertExplain(int)));
}

QSize matrixValuesDialog::sizeHint() const 
{
return QSize( 400, 190 );
}

void matrixValuesDialog::accept()
{
if (apply())
	close();
}

bool matrixValuesDialog::apply()
{
QString aux=commandes->text().lower();	
aux.remove("\n");
	
int pos1,pos2,pos3,i;
QStringList variables, rowIndexes, colIndexes;
int n=aux.contains("cell(");
for (i=0; i<n; i++)
	{
	pos1=aux.find("cell(",0,TRUE);
	pos2=aux.find("(",pos1+1);
	pos3=aux.find(")",pos2+1);
	
	QString aux2=aux.mid(pos2+1,pos3-pos2-1);
	if (aux2.contains("cell(") > 0)
		{
		QMessageBox::critical(0,tr("QtiPlot - Input function error"), 
			tr("You can not use cells recursevely!"));
		return false;
   		}

	QStringList items=QStringList::split(",", aux2, FALSE);
	QString ir = items[0].stripWhiteSpace();
	QString ic = items[1].stripWhiteSpace();
	if (ir == "0" || ic == "0")
		{
		QMessageBox::critical(0,tr("QtiPlot - Input function error"), 
			tr("Column and row indexes must be greater than zero!"));
		return false;
   		}

	rowIndexes << ir;
	colIndexes << ic;
	
	QString s = "cell"+ QString::number(i);
	variables << s;
	aux.replace(pos1,5 + pos3 - pos2, s);
	}
	
int m=(int)variables.count();
double *vars = new double[m]; 
myParser parser;
try
    {
	for (i=0; i<m; i++)
		{
		parser.DefineVar(variables[i].ascii(), &vars[i]);
		myParser rparser;
		double l = 1;
		rparser.DefineVar("i", &l);
		rparser.DefineVar("j", &l);
		try
    		{	
			rparser.SetExpr(rowIndexes[i].ascii());
	        rparser.Eval();
			}
		catch(mu::ParserError &e)
			{
			QMessageBox::critical(0,"QtiPlot - Row index input error", e.GetMsg());
			return false;
   			}	
		
		try
    		{	
			rparser.SetExpr(colIndexes[i].ascii());
	        rparser.Eval();
			}
		catch(mu::ParserError &e)
			{
			QMessageBox::critical(0,"QtiPlot - Column index input error", e.GetMsg());
			return false;
			}

		vars[i]=1.0;//dumy value
		}

	double index = 1;
	parser.DefineVar("i", &index);
	parser.DefineVar("j", &index);
	parser.SetExpr(aux.ascii());
	parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QString errString = e.GetMsg();
	for (i=0;i<m;i++)
		errString.replace(variables[i], "cell(" + rowIndexes[i] + "," + colIndexes[i] + ")", TRUE);

	QMessageBox::critical(0, tr("QtiPlot - Input function error"), tr(errString));
	return false;
    }
delete[] vars;
emit setValues(commandes->text(), aux, rowIndexes, colIndexes,
			   startRow->value(), endRow->value(), startCol->value(), endCol->value());
return true;
}

void matrixValuesDialog::setFormula(const QString& s)
{
	commandes->setText(s);
}

void matrixValuesDialog::setColumns(int c)
{
	endCol->setValue(c);
}

void matrixValuesDialog::setRows(int r)
{
	endRow->setValue(r);
}

void matrixValuesDialog::setFunctions()
{
functions->insertStringList(myParser::functionsList(), -1);
}

void matrixValuesDialog::insertExplain(int index)
{
explain->setText(myParser::explainFunction(index));
}

void matrixValuesDialog::insertFunction()
{
QString f=functions->currentText();
if (commandes->hasSelectedText())
	{	
	f=f.remove(")");
	QString markedText=commandes->selectedText();
	commandes->insert(f+markedText+")");
	}
else
	{
	commandes->insert( f );
	int index, para;
	commandes->getCursorPosition (&para,&index);
	commandes->setCursorPosition (para,index-1);
	}
}

void matrixValuesDialog::addCell()
{
commandes->insert("cell(i, j)");
}

matrixValuesDialog::~matrixValuesDialog()
{
}
