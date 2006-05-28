/***************************************************************************
    File                 : functionDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Function dialog
                           
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
#include "functionDialog.h"
#include "parser.h"
#include "application.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <q3widgetstack.h>
//Added by qt3to4:
#include <Q3ValueList>

fDialog::fDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : functionDialogui( parent, name, modal, fl )
{
    if ( !name )
	setName( "fDialog" );

	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonClear, SIGNAL( clicked() ), this, SLOT(clearList() ) );
	setFocusProxy(boxFunction);

	curveID = -1;
	graph = 0;
}

void fDialog::setCurveToModify(const QString& s, int curve)
{
curveID = curve;

int equations = s.count("=");
if (equations == 1)
	{
	uint pos = s.find("=", 0);
	QString eq = s;
	eq.remove(0, pos+1);

	QStringList lst = QStringList::split(",", eq, false);
	boxFunction->setCurrentText(lst[0]);
	boxFrom->setText(lst[2]);
	boxTo->setText(lst[3]);
	boxPoints->setValue(graph->curveDataSize(curve));
	}
else if (equations == 2 && s.contains("Theta"))
	{
	optionStack->raiseWidget(2);
	boxType->setCurrentItem(2);

	uint pos = s.find(",", 0);
	QString eq1 = s.left(pos);
	uint pos2 = eq1.find("=", 0);
	eq1.remove(0, pos2+1);
	boxPolarRadius->setCurrentText(eq1);

	QString eq2 = s;
	eq2.remove(0, pos+1);
	pos2 = eq2.find("=", 0);
	eq2.remove(0, pos2+1);

	QStringList lst = QStringList::split(",", eq2, false);
	boxPolarTheta->setCurrentText(lst[0]);
	boxPolarParameter->setText(lst[1]);
	boxPolarFrom->setText(lst[2]);
	boxPolarTo->setText(lst[3]);
	boxPolarPoints->setValue(graph->curveDataSize(curve));
	}
else if (equations == 2 && !s.contains("Theta"))
	{
	boxType->setCurrentItem(1);
	optionStack->raiseWidget(1);

	uint pos = s.find(",", 0);
	QString eq1 = s.left(pos);
	uint pos2 = eq1.find("=", 0);
	eq1.remove(0, pos2+1);
	boxXFunction->setCurrentText(eq1);

	QString eq2 = s;
	eq2.remove(0, pos+1);
	pos2 = eq2.find("=", 0);
	eq2.remove(0, pos2+1);

	QStringList lst = QStringList::split(",", eq2, false);
	boxYFunction->setCurrentText(lst[0]);
	boxParameter->setText(lst[1]);
	boxParFrom->setText(lst[2]);
	boxParTo->setText(lst[3]);
	boxParPoints->setValue(graph->curveDataSize(curve));
	}
}

void fDialog::clearList()
{
int type=boxType->currentItem();
switch (type)
	{
	case 0:
		boxFunction->clear();
		emit clearFunctionsList();
	break;
	
	case 1:
		boxXFunction->clear();
		boxYFunction->clear();
		emit clearParamFunctionsList();
	break;
	
	case 2:
		boxPolarTheta->clear();
		boxPolarRadius->clear();
		emit clearPolarFunctionsList();
	break;
	}
}

void fDialog::acceptFunction()
{
QString from=boxFrom->text().lower();
QString to=boxTo->text().lower();
QString points=boxPoints->text().lower();

double start, end;
try
	{
	myParser parser;
	parser.SetExpr(from.ascii());
	start=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Start limit error"), QString::fromStdString(e.GetMsg()));
	boxFrom->setFocus();
	return;
	}	
try
	{
	myParser parser;
	parser.SetExpr(to.ascii());
	end=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - End limit error"), QString::fromStdString(e.GetMsg()));
	boxTo->setFocus();
	return;
	}

if (start>=end)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input error"),
				tr("Please enter x limits that satisfy: from < end!"));
	boxTo->setFocus();
	return;
	}

	double x;
	QString formula=boxFunction->currentText();
	bool error=FALSE;

	try
		{
		myParser parser;	
		parser.DefineVar("x", &x);	
		parser.SetExpr(formula.ascii());
		
		x=start;
		parser.Eval();
		x=end;
		parser.Eval();
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical(0, tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
		boxFunction->setFocus();
		error=TRUE;	
		}	

	// Collecting all the information
	QString type=QString("Function");
	QStringList formulas;
	QStringList variables;	
	Q3ValueList<double> ranges;
	Q3ValueList<int> varpoints;
	formulas+=formula;
	variables+="x";
	ranges+=start;
	ranges+=end;
	varpoints[0]=boxPoints->value();
	if (!error)
		{
		if (!graph)
			emit newFunctionPlot(type,formulas,variables,ranges,varpoints);
		else
			{
			if (curveID >= 0)
				graph->modifyFunctionCurve(curveID, type,formulas,variables,ranges,varpoints);
			else
				graph->addFunctionCurve(type,formulas,variables,ranges,varpoints);

			ApplicationWindow *app = (ApplicationWindow *)this->parent();
			app->updateFunctionLists(type,formulas);
			}
		close();
		}		

}
void fDialog::acceptParametric()
{
QString from=boxParFrom->text().lower();
QString to=boxParTo->text().lower();
QString points=boxParPoints->text().lower();

double start, end;
try
	{
	myParser parser;
	parser.SetExpr(from.ascii());
	start=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Start limit error"), QString::fromStdString(e.GetMsg()));
	boxParFrom->setFocus();
	return;
	}	

try
	{
	myParser parser;
	parser.SetExpr(to.ascii());
	end=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - End limit error"), QString::fromStdString(e.GetMsg()));
	boxParTo->setFocus();
	return;
	}	

if (start>=end)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input error"),
				tr("Please enter parameter limits that satisfy: from < end!"));
	boxParTo->setFocus();
	return;
	}

	double parameter;
	QString xformula=boxXFunction->currentText();
	QString yformula=boxYFunction->currentText();	
	bool error=FALSE;

	try
		{
		myParser parser;				
		parser.DefineVar((boxParameter->text()).ascii(), &parameter);	
		parser.SetExpr(xformula.ascii());
		
		parameter=start;
		parser.Eval();
		parameter=end;
		parser.Eval();
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical(0, tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
		boxXFunction->setFocus();
		error=TRUE;	
		}	
	try
		{
		myParser parser;				
		parser.DefineVar((boxParameter->text()).ascii(), &parameter);	
		parser.SetExpr(yformula.ascii());
		
		parameter=start;
		parser.Eval();
		parameter=end;
		parser.Eval();
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical(0, tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
		boxYFunction->setFocus();
		error=TRUE;	
		}
	// Collecting all the information
	QString type=QString("Parametric plot");
	QStringList formulas;
	QStringList variables;	
	Q3ValueList<double> ranges;
	Q3ValueList<int> varpoints;
	formulas+=xformula;
	formulas+=yformula;
	variables+=boxParameter->text();
	ranges+=start;
	ranges+=end;
	varpoints[0]=boxParPoints->value();
	if (!error)
		{
		if (!graph)
			emit newFunctionPlot(type,formulas,variables,ranges,varpoints);
		else
			{
			if (curveID >= 0)
				graph->modifyFunctionCurve(curveID, type,formulas,variables,ranges,varpoints);
			else
				graph->addFunctionCurve(type,formulas,variables,ranges,varpoints);

			ApplicationWindow *app = (ApplicationWindow *)this->parent();
			app->updateFunctionLists(type,formulas);
			}
		close();
		}
}

void fDialog::acceptPolar()
{
QString from=boxPolarFrom->text().lower();
QString to=boxPolarTo->text().lower();
QString points=boxPolarPoints->text().lower();

double start, end;
try
	{
	myParser parser;
	parser.SetExpr(from.ascii());
	start=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Start limit error"), QString::fromStdString(e.GetMsg()));
	boxPolarFrom->setFocus();
	return;
	}

try
	{
	myParser parser;	
	parser.SetExpr(to.ascii());
	end=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - End limit error"), QString::fromStdString(e.GetMsg()));
	boxPolarTo->setFocus();
	return;
	}

if (start>=end)
	{
	QMessageBox::critical(0, tr("QtiPlot - Input error"),
				tr("Please enter parameter limits that satisfy: from < end!"));
	boxPolarTo->setFocus();
	return;
	}

	double parameter;
	QString rformula=boxPolarRadius->currentText();
	QString tformula=boxPolarTheta->currentText();	
	bool error=FALSE;

	try
		{
		myParser parser;				
		parser.DefineVar((boxPolarParameter->text()).ascii(), &parameter);	
		parser.SetExpr(rformula.ascii());
		
		parameter=start;
		parser.Eval();
		parameter=end;
		parser.Eval();
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical(0, tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
		boxPolarRadius->setFocus();
		error=TRUE;	
		}	
	try
		{
		myParser parser;				
		parser.DefineVar((boxPolarParameter->text()).ascii(), &parameter);	
		parser.SetExpr(tformula.ascii());
		
		parameter=start;
		parser.Eval();
		parameter=end;
		parser.Eval();
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical(0, tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
		boxPolarTheta->setFocus();
		error=TRUE;	
		}
	// Collecting all the information
	QString type=QString("Polar plot");
	QStringList formulas;
	QStringList variables;	
	Q3ValueList<double> ranges;
	Q3ValueList<int> varpoints;
	formulas+=rformula;
	formulas+=tformula;
	variables+=boxPolarParameter->text();
	ranges+=start;
	ranges+=end;
	varpoints[0]=boxPolarPoints->value();
	if (!error)
		{
		if (!graph)
			emit newFunctionPlot(type,formulas,variables,ranges,varpoints);
		else
			{
			if (curveID >= 0)
				graph->modifyFunctionCurve(curveID, type,formulas,variables,ranges,varpoints);
			else
				graph->addFunctionCurve(type,formulas,variables,ranges,varpoints);

			ApplicationWindow *app = (ApplicationWindow *)this->parent();
			app->updateFunctionLists(type,formulas);
			}
		close();
		}
}

void fDialog::accept()
{
int type=boxType->currentItem();
switch (type)
	{
	case 0:
		acceptFunction();
	break;
	
	case 1:
		acceptParametric();
	break;
	
	case 2:
		acceptPolar();
	break;
	}
}

void fDialog::insertFunctionsList(const QStringList& list)
{
boxFunction->insertStringList (list, 1);
}

void fDialog::insertParamFunctionsList(const QStringList& xList, const QStringList& yList)
{
boxXFunction->insertStringList (xList, 1);
boxYFunction->insertStringList (yList, 1);
}

void fDialog::insertPolarFunctionsList(const QStringList& rList, const QStringList& tetaList)
{
boxPolarRadius->insertStringList (rList, 1);
boxPolarTheta->insertStringList (tetaList, 1);	
}

fDialog::~fDialog()
{
}
