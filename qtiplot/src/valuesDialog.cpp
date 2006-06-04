/***************************************************************************
    File                 : valuesDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Set column values dialog
                           
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
#include "valuesDialog.h"
#include "worksheet.h"
#include "parser.h"

#include <qcombobox.h>
#include <qspinbox.h>
#include <q3textedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <q3vbox.h>
#include <q3hbox.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <Q3Frame>
#include <Q3VBoxLayout>

setColValuesDialog::setColValuesDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "setColValuesDialog" );
    setWindowTitle( tr( "QtiPlot - Set column values" ) );
    setFocusPolicy( Qt::StrongFocus );
	
	Q3HBox *hbox1=new Q3HBox (this, "hbox1"); 
	hbox1->setSpacing (5);
	
	Q3VBox *box1=new Q3VBox (hbox1, "box2"); 
	box1->setSpacing (5);

	explain = new Q3TextEdit(box1, "explain" );
	explain->setReadOnly (true);
	explain->setPaletteBackgroundColor(QColor(197, 197, 197));
	
	colNameLabel = new QLabel(box1, "colNameLabel" );

	Q3VBox *box2=new Q3VBox (hbox1, "box2"); 
	box2->setMargin(5);
	box2->setFrameStyle (Q3Frame::Box);

	Q3HBox *hbox2=new Q3HBox (box2, "hbox2"); 
	hbox2->setMargin(5);
	hbox2->setSpacing (5);
	
	QLabel *TextLabel1 = new QLabel(hbox2, "TextLabel1" );
    TextLabel1->setText( tr( "For row (i)" ) );
	
	start = new QSpinBox(hbox2, "start" );
   
    QLabel *TextLabel2 = new QLabel(hbox2, "TextLabel2" );
    TextLabel2->setText( tr( "to" ) );

    end = new QSpinBox(hbox2, "end" );

    start->setMinValue(1);
    end->setMinValue(1);
    if (sizeof(int)==2)
	 { // 16 bit signed integer
	 start->setMaxValue(0x7fff);
	 end->setMaxValue(0x7fff);
	 }
    else
	 { // 32 bit signed integer
	 start->setMaxValue(0x7fffffff);
	 end->setMaxValue(0x7fffffff);
	 }
  
	Q3ButtonGroup *GroupBox0 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "" ),box2, "GroupBox0" );
	GroupBox0->setFlat(true);

    functions = new QComboBox( false, GroupBox0, "functions" );
	
	PushButton3 = new QPushButton(GroupBox0, "PushButton3" );
    PushButton3->setText( tr( "Add function" ) );
    
    boxColumn = new QComboBox( false, GroupBox0, "boxColumn" );
   
    PushButton4 = new QPushButton(GroupBox0, "PushButton4" );
    PushButton4->setText( tr( "Add column" ) );

	Q3HBox *hbox6=new Q3HBox (GroupBox0, "hbox6"); 
	hbox6->setSpacing (5);

	buttonPrev = new QPushButton( hbox6, "buttonPrev" );
	buttonPrev->setText("&<<");

	buttonNext = new QPushButton( hbox6, "buttonNext" );
	buttonNext->setText("&>>");

	addCellButton = new QPushButton(GroupBox0, "addCellButton" );
    addCellButton->setText( tr( "Add cell" ) );

	Q3HBox *hbox3=new Q3HBox (this, "hbox3"); 
	hbox3->setSpacing (5);
	
	commands = new Q3TextEdit( hbox3, "commands" );
	commands->setTextFormat(Qt::PlainText);
    commands->setGeometry( QRect(10, 100, 260, 70) );
	commands->setFocus();
	
	Q3VBox *box3=new Q3VBox (hbox3,"box3"); 
	box3->setSpacing (5);
	
    btnOk = new QPushButton(box3, "btnOk" );
    btnOk->setText( tr( "OK" ) );

	btnApply = new QPushButton(box3, "btnApply" );
    btnApply->setText( tr( "Apply" ) );

    btnCancel = new QPushButton( box3, "btnCancel" );
    btnCancel->setText( tr( "Cancel" ) );
	
	Q3VBoxLayout* layout = new Q3VBoxLayout(this,5,5, "hlayout3");
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

QStringList com = table->getCommands();
commands->setText(com[sc]);
commands->moveCursor ( Q3TextEdit::MoveEnd, true );
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
QString aux=commands->text();	
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
	table->setColValues(val, start->value(),end->value());
	return true;
	}

int pos1,pos2,pos3,i;
QStringList variables, rowIndexes, columns, colNames=table->colNames();
int n = aux.count("col(");
for (i=0;i<n;i++)
	{
	pos1=aux.find("col(",0,true);
	pos2=aux.find("(",pos1+1);
	pos3=aux.find(")",pos2+1);
	
	QString aux2=aux.mid(pos2+1,pos3-pos2-1);
	if (aux2.contains("col("))
		{
		QMessageBox::critical(0,tr("QtiPlot - Input function error"), tr("You can not use imbricated columns!"));
		return false;
   		}

	QStringList items=QStringList::split(",", aux2, false);
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
				QMessageBox::critical(0,"QtiPlot - Input function error", QString::fromStdString(e.GetMsg()));
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
	QString errString=QString::fromStdString(e.GetMsg());
	for (i=0;i<m;i++)
		errString.replace(variables[i], columns[i], true);

	QMessageBox::critical(0,"QtiPlot - Input function error", errString);
	return false;
    }

delete[] vars;
table->setColValues(commands->text(), aux, variables, rowIndexes, 
				   start->value(),end->value());
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
if (commands->hasSelectedText())
	{	
	f=f.remove(")");
	QString markedText=commands->selectedText();
	commands->insert(f+markedText+")");
	}
else
	{
	commands->insert( f );
	int index, para;
	commands->getCursorPosition (&para,&index);
	commands->setCursorPosition (para,index-1);
	}
}

void setColValuesDialog::insertCol()
{
commands->insert(boxColumn->currentText());
}

void setColValuesDialog::insertCell()
{
QString f=boxColumn->currentText().remove(")")+", i)";
commands->insert(f);
}

void setColValuesDialog::setTable(Table* w)
{
table=w;
QStringList colNames=w->colNames();
int cols = w->tableCols();
for (int i=0; i<cols; i++)
	boxColumn->insertItem("col("+colNames[i]+")",i); 

start->setValue(1);
end->setValue(w->tableRows());
updateColumn(w->selectedColumn());
}

setColValuesDialog::~setColValuesDialog()
{
}
