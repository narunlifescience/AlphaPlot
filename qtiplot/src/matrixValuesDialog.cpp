/***************************************************************************
    File                 : MatrixValuesDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
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
#include "Scripting.h"
#include "scriptedit.h"
#include "matrix.h"

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

MatrixValuesDialog::MatrixValuesDialog( ScriptingEnv *env, QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
: QDialog( parent, name, modal, fl )
{
	scriptEnv = env;
	if ( !name )
		setName( "MatrixValuesDialog" );

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

	commands = new ScriptEdit( scriptEnv, hbox4, "commands" );
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

QSize MatrixValuesDialog::sizeHint() const 
{
	return QSize( 400, 190 );
}

void MatrixValuesDialog::accept()
{
	if (apply())
		close();
}

bool MatrixValuesDialog::apply()
{
	QString formula = commands->text();
	QString oldFormula = matrix->formula();

	matrix->setFormula(formula);
	if (matrix->calculate(startRow->value()-1, endRow->value()-1, startCol->value()-1, endCol->value()-1))
		return true;
	matrix->setFormula(oldFormula);
	return false;
}

void MatrixValuesDialog::setMatrix(Matrix* m)
{
	matrix = m;
	commands->setText(m->formula());
	endCol->setValue(m->numCols());
	endRow->setValue(m->numRows());
	commands->setContext(m);
}

void MatrixValuesDialog::setFunctions()
{
	functions->insertStringList(scriptEnv->mathFunctions(), -1);
}

void MatrixValuesDialog::insertExplain(int index)
{
	explain->setText(scriptEnv->mathFunctionDoc(functions->text(index)));
}

void MatrixValuesDialog::insertFunction()
{
	commands->insertFunction(functions->currentText());
}

void MatrixValuesDialog::addCell()
{
	commands->insert("cell(i, j)");
}

MatrixValuesDialog::~MatrixValuesDialog()
{
}
