/***************************************************************************
    File                 : valuesDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
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
#include "scriptedit.h"

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

#include <QTextCursor>

SetColValuesDialog::SetColValuesDialog( ScriptingEnv *env, QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	scriptEnv = env;
	if ( !name )
		setName( "SetColValuesDialog" );
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

	commands = new ScriptEdit( env, hbox3, "commands" );
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

void SetColValuesDialog::prevColumn()
{
	int sc = table->selectedColumn();
	updateColumn(--sc);
}

void SetColValuesDialog::nextColumn()
{
	int sc = table->selectedColumn();
	updateColumn(++sc);
}

void SetColValuesDialog::updateColumn(int sc)
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
	colNameLabel->setText("col(\""+table->colLabel(sc)+"\")= ");

	QStringList com = table->getCommands();
	commands->setText(com[sc]);
	QTextCursor cursor = commands->textCursor();
	cursor.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
}

QSize SetColValuesDialog::sizeHint() const 
{
	return QSize( 400, 190 );
}

void SetColValuesDialog::accept()
{
	if (apply())
		close();
}

bool SetColValuesDialog::apply()
{
	int col = table->selectedColumn();
	QString formula = commands->text();
	QString oldFormula = table->getCommands()[col];

	table->setCommand(col,formula);
	if(table->calculate(col,start->value()-1,end->value()-1))
		return true;
	table->setCommand(col,oldFormula);
	return false;
}

void SetColValuesDialog::setFunctions()
{
	functions->insertStringList(scriptEnv->mathFunctions(), -1);
}

void SetColValuesDialog::insertExplain(int index)
{
	explain->setText(scriptEnv->mathFunctionDoc(functions->text(index)));
}

void SetColValuesDialog::insertFunction()
{
	commands->insertFunction(functions->currentText());
}

void SetColValuesDialog::insertCol()
{
	commands->insert(boxColumn->currentText());
}

void SetColValuesDialog::insertCell()
{
	QString f=boxColumn->currentText().remove(")")+", i)";
	commands->insert(f);
}

void SetColValuesDialog::setTable(Table* w)
{
	table=w;
	QStringList colNames=w->colNames();
	int cols = w->tableCols();
	for (int i=0; i<cols; i++)
		boxColumn->insertItem("col(\""+colNames[i]+"\")",i); 

	int s = w->table()->currentSelection();
	if (s >= 0)
	{
		Q3TableSelection sel = w->table()->selection(s);
		w->setSelectedCol(sel.leftCol());

		start->setValue(sel.topRow() + 1);
		end->setValue(sel.bottomRow() + 1);
	}
	else
	{
		start->setValue(1);
		end->setValue(w->tableRows());
	}

	updateColumn(w->selectedColumn());
	commands->setContext(w);
}

SetColValuesDialog::~SetColValuesDialog()
{
}
