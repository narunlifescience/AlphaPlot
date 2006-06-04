/***************************************************************************
    File                 : matrixValuesDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Set matrix values dialog
                           
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
#include "matrixValuesDialog.h"
#include "parser.h"

#include <qcombobox.h>
#include <qspinbox.h>
#include <q3textedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <q3vbox.h>
#include <q3hbox.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <Q3Frame>
#include <Q3VBoxLayout>

matrixValuesDialog::matrixValuesDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "matrixValuesDialog" );

    setWindowTitle( tr( "QtiPlot - Set Matrix Values" ) );
    setFocusPolicy( Qt::StrongFocus );
	
	Q3HBox *hbox1=new Q3HBox (this, "hbox1"); 
	hbox1->setSpacing (5);
	
	Q3VBox *box1=new Q3VBox (hbox1, "box2"); 
	box1->setSpacing (5);

	explain = new Q3TextEdit(box1, "explain" );
	explain->setReadOnly (true);
	
	Q3VBox *box2=new Q3VBox (hbox1, "box2"); 
	box2->setMargin(5);
	box2->setFrameStyle (Q3Frame::Box);

	Q3ButtonGroup *GroupBox1 = new Q3ButtonGroup(4,Qt::Horizontal, "",box2, "GroupBox0" );
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

	Q3HBox *hbox5=new Q3HBox (box2, "hbox5"); 
	hbox5->setSpacing (5);
	hbox5->setMargin(5);

    functions = new QComboBox( false, hbox5, "functions" );
	
	PushButton3 = new QPushButton(hbox5, "PushButton3" );
    PushButton3->setText( tr( "Add function" ) ); 

	btnAddCell = new QPushButton(hbox5, "btnAddCell" );
    btnAddCell->setText( tr( "Add Cell" ) );

	Q3HBox *hbox4=new Q3HBox (this, "hbox4"); 
	hbox4->setSpacing (5);
	
	QLabel *TextLabel5 = new QLabel(hbox4, "TextLabel2" );
    TextLabel5->setText( tr( "Cell(i,j)=" ) );

	commands = new Q3TextEdit( hbox4, "commands" );
	commands->setTextFormat(Qt::PlainText);
    commands->setGeometry( QRect(10, 100, 260, 70) );
	commands->setFocus();
	
	Q3VBox *box3=new Q3VBox (hbox4,"box3"); 
	box3->setSpacing (5);
	
	btnOk = new QPushButton(box3, "btnOk" );
    btnOk->setText( tr( "OK" ) );

	btnApply = new QPushButton(box3, "btnApply" );
    btnApply->setText( tr( "Apply" ) );

    btnCancel = new QPushButton( box3, "btnCancel" );
    btnCancel->setText( tr( "Cancel" ) );
	
	Q3VBoxLayout* layout = new Q3VBoxLayout(this,5,5, "hlayout3");
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
QString aux=commands->text().lower();	
aux.remove("\n");
	
int pos1,pos2,pos3,i;
QStringList variables, rowIndexes, colIndexes;
int n = aux.count("cell(");
for (i=0; i<n; i++)
	{
	pos1=aux.find("cell(",0,true);
	pos2=aux.find("(",pos1+1);
	pos3=aux.find(")",pos2+1);
	
	QString aux2=aux.mid(pos2+1,pos3-pos2-1);
	if (aux2.contains("cell(") > 0)
		{
		QMessageBox::critical(0,tr("QtiPlot - Input function error"), 
			tr("You can not use cells recursevely!"));
		return false;
   		}

	QStringList items=QStringList::split(",", aux2, false);
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
			QMessageBox::critical(0,"QtiPlot - Row index input error", QString::fromStdString(e.GetMsg()));
			return false;
   			}	
		
		try
    		{	
			rparser.SetExpr(colIndexes[i].ascii());
	        rparser.Eval();
			}
		catch(mu::ParserError &e)
			{
			QMessageBox::critical(0,"QtiPlot - Column index input error", QString::fromStdString(e.GetMsg()));
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
	QString errString = QString::fromStdString(e.GetMsg());
	for (i=0;i<m;i++)
		errString.replace(variables[i], "cell(" + rowIndexes[i] + "," + colIndexes[i] + ")", true);

	QMessageBox::critical(0, tr("QtiPlot - Input function error"), tr(errString));
	return false;
    }
delete[] vars;
emit setValues(commands->text(), aux, rowIndexes, colIndexes,
			   startRow->value(), endRow->value(), startCol->value(), endCol->value());
return true;
}

void matrixValuesDialog::setFormula(const QString& s)
{
	commands->setText(s);
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

void matrixValuesDialog::addCell()
{
commands->insert("cell(i, j)");
}

matrixValuesDialog::~matrixValuesDialog()
{
}
