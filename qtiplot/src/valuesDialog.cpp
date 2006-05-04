#include "valuesDialog.h"
#include "worksheet.h"
#include "parser.h"

#include <qcombobox.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qbuttongroup.h>

setColValuesDialog::setColValuesDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "setColValuesDialog" );
    setCaption( tr( "QtiPlot - Set column values" ) );
    setFocusPolicy( QDialog::StrongFocus );
	
	QHBox *hbox1=new QHBox (this, "hbox1"); 
	hbox1->setSpacing (5);
	
	QVBox *box1=new QVBox (hbox1, "box2"); 
	box1->setSpacing (5);

	explain = new QTextEdit(box1, "explain" );
	explain->setReadOnly (true);
	explain->setPaletteBackgroundColor(QColor(197, 197, 197));
	
	colNameLabel = new QLabel(box1, "colNameLabel" );

	QVBox *box2=new QVBox (hbox1, "box2"); 
	box2->setMargin(5);
	box2->setFrameStyle (QFrame::Box);

	QHBox *hbox2=new QHBox (box2, "hbox2"); 
	hbox2->setMargin(5);
	hbox2->setSpacing (5);
	
	QLabel *TextLabel1 = new QLabel(hbox2, "TextLabel1" );
    TextLabel1->setText( tr( "For row (i)" ) );
	
	start = new QLineEdit(hbox2, "start" );
   
    QLabel *TextLabel2 = new QLabel(hbox2, "TextLabel2" );
    TextLabel2->setText( tr( "to" ) );

    end = new QLineEdit(hbox2, "end" );
  
	QButtonGroup *GroupBox0 = new QButtonGroup(2,QGroupBox::Horizontal,tr( "" ),box2, "GroupBox0" );
	GroupBox0->setLineWidth(0);
	GroupBox0->setFlat(true);

    functions = new QComboBox( FALSE, GroupBox0, "functions" );
	
	PushButton3 = new QPushButton(GroupBox0, "PushButton3" );
    PushButton3->setText( tr( "Add function" ) );
    
    boxColumn = new QComboBox( FALSE, GroupBox0, "boxColumn" );
   
    PushButton4 = new QPushButton(GroupBox0, "PushButton4" );
    PushButton4->setText( tr( "Add column" ) );

	QHBox *hbox6=new QHBox (GroupBox0, "hbox6"); 
	hbox6->setSpacing (5);

	buttonPrev = new QPushButton( hbox6, "buttonPrev" );
	buttonPrev->setText("<<");

	buttonNext = new QPushButton( hbox6, "buttonNext" );
	buttonNext->setText(">>");

	addCellButton = new QPushButton(GroupBox0, "addCellButton" );
    addCellButton->setText( tr( "Add cell" ) );

	QHBox *hbox3=new QHBox (this, "hbox3"); 
	hbox3->setSpacing (5);
	
	commandes = new QTextEdit( hbox3, "commandes" );
	commandes->setTextFormat(Qt::PlainText);
    commandes->setGeometry( QRect(10, 100, 260, 70) );
	commandes->setFocus();
	
	QVBox *box3=new QVBox (hbox3,"box3"); 
	box3->setSpacing (5);
	
    btnOk = new QPushButton(box3, "btnOk" );
    btnOk->setText( tr( "OK" ) );

	btnApply = new QPushButton(box3, "btnApply" );
    btnApply->setText( tr( "Apply" ) );

    btnCancel = new QPushButton( box3, "btnCancel" );
    btnCancel->setText( tr( "Cancel" ) );
	
	QVBoxLayout* layout = new QVBoxLayout(this,5,5, "hlayout3");
    layout->addWidget(hbox1);
	layout->addWidget(hbox3);

setFunctions();
insertExplain(0);

connect(PushButton3, SIGNAL(clicked()),this, SLOT(insertFunction()));
connect(PushButton4, SIGNAL(clicked()),this, SLOT(insertCol()));
connect(addCellButton, SIGNAL(clicked()),this, SLOT(insertCell()));
connect(btnOk, SIGNAL(clicked()),this, SLOT(accept()));
connect(btnApply, SIGNAL(clicked()),this, SLOT(apply()));
connect(btnCancel, SIGNAL(clicked()),this, SLOT(close()));
connect(functions, SIGNAL(activated(int)),this, SLOT(insertExplain(int)));
connect(buttonPrev, SIGNAL(clicked()), this, SLOT(prevColumn()));
connect(buttonNext, SIGNAL(clicked()), this, SLOT(nextColumn()));
}

void setColValuesDialog::prevColumn()
{
int sc = table->selectedColumn();
updateColumn(--sc);
}

void setColValuesDialog::nextColumn()
{
int sc = table->selectedColumn();
updateColumn(++sc);
}

void setColValuesDialog::updateColumn(int sc)
{
if (!sc)
	buttonPrev->setEnabled(false);
else
	buttonPrev->setEnabled(true);

if (sc >= table->tableCols() - 1)
	buttonNext->setEnabled(false);
else
	buttonNext->setEnabled(true);

table->setSelectedCol(sc);
colNameLabel->setText("col("+table->colLabel(sc)+")= ");

QStringList com = table->getCommandes();
commandes->setText(com[sc]);
commandes->moveCursor ( QTextEdit::MoveEnd, true );
}

QSize setColValuesDialog::sizeHint() const 
{
return QSize( 400, 190 );
}

void setColValuesDialog::accept()
{
if (apply())
	close();
}

bool setColValuesDialog::apply()
{
QString aux=commandes->text();	
aux.remove("\n");

myParser parser;
parser.SetExpr(aux.ascii());
bool numeric = true;
double val;
try
	{
	val = parser.Eval();
	}
catch (mu::ParserError &)
	{
	numeric = false;
	}

if (numeric)
	{
	table->setColValues(val, start->text().toInt(),end->text().toInt());
	return true;
	}

int pos1,pos2,pos3,i;
QStringList variables, rowIndexes, columns, colNames=table->colNames();
int n=aux.contains("col(");
for (i=0;i<n;i++)
	{
	pos1=aux.find("col(",0,TRUE);
	pos2=aux.find("(",pos1+1);
	pos3=aux.find(")",pos2+1);
	
	QString aux2=aux.mid(pos2+1,pos3-pos2-1);
	if (aux2.contains("col("))
		{
		QMessageBox::critical(0,tr("QtiPlot - Input function error"), tr("You can not use imbricated columns!"));
		return false;
   		}

	QStringList items=QStringList::split(",", aux2, FALSE);
	int index=colNames.findIndex (items[0]);	
	if (items.count() == 2)
		rowIndexes<<items[1];
	else
		rowIndexes<<"i";
			
	QString s="c"+ QString::number(i)+"_"+QString::number(index);
	aux.replace(pos1,4+pos3-pos2,s);
		
	if (variables.contains(s)<=0)
		{
		variables<<s;
		columns<<"col("+aux2+")";
		}
	}
	
int m=(int)variables.count();
double *vars = new double[m];	
try
    {
	for (i=0; i<m; i++)
		{
		parser.DefineVar(variables[i].ascii(), &vars[i]);
		if (rowIndexes[i] != "i")	
			{
			myParser rparser;
			double l=0;
			try
    			{	
				rparser.DefineVar("i", &l);
				rparser.SetExpr(rowIndexes[i].ascii());
	        	rparser.Eval();
				}
			catch(mu::ParserError &e)
				{
				QMessageBox::critical(0,"QtiPlot - Input function error", e.GetMsg());
				return false;
   				}
			}			
		vars[i]=1.0;//dumy value
		}

	double index = 1;
	parser.DefineVar("i", &index);
	parser.SetExpr(aux.ascii());
	parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QString errString=e.GetMsg();
	for (i=0;i<m;i++)
		errString.replace(variables[i], columns[i], TRUE);

	QMessageBox::critical(0,"QtiPlot - Input function error", errString);
	return false;
    }

delete[] vars;
table->setColValues(commandes->text(), aux, variables, rowIndexes, 
				   start->text().toInt(),end->text().toInt());
return true;
}

void setColValuesDialog::setFunctions()
{
functions->insertStringList(myParser::functionsList(), -1);
}

void setColValuesDialog::insertExplain(int index)
{
explain->setText(myParser::explainFunction(index));
}

void setColValuesDialog::insertFunction()
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

void setColValuesDialog::insertCol()
{
commandes->insert(boxColumn->currentText());
}

void setColValuesDialog::insertCell()
{
QString f=boxColumn->currentText().remove(")")+", i)";
commandes->insert(f);
}

void setColValuesDialog::setTable(Table* w)
{
table=w;
QStringList colNames=w->colNames();
int cols = w->tableCols();
for (int i=0; i<cols; i++)
	boxColumn->insertItem("col("+colNames[i]+")",i); 

start->setText("1");
end->setText(QString::number(w->tableRows())); 
updateColumn(w->selectedColumn());
}

setColValuesDialog::~setColValuesDialog()
{
}
