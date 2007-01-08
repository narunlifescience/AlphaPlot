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
#include "FunctionCurve.h"

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

FunctionDialog::FunctionDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
: FunctionDialogUi( parent, name, modal, fl )
{
	if ( !name )
		setName( "FunctionDialog" );

	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonClear, SIGNAL( clicked() ), this, SLOT(clearList() ) );
	setFocusProxy(boxFunction);

	curveID = -1;
	graph = 0;
}

void FunctionDialog::setCurveToModify(Graph *g, int curve)
{
	if (!g)
		return;

	graph = g;

	FunctionCurve *c = (FunctionCurve *)graph->curve(curve);
	if (!c)
		return;

	curveID = curve;

	QStringList formulas = c->formulas();
	if (c->functionType() == FunctionCurve::Normal)
	{
		boxFunction->setText(formulas[0]);
		boxFrom->setText(QString::number(c->startRange(), 'g', 15));
		boxTo->setText(QString::number(c->endRange(), 'g', 15));
		boxPoints->setValue(c->dataSize());
	}
	else if (c->functionType() == FunctionCurve::Polar)
	{
		optionStack->raiseWidget(2);
		boxType->setCurrentItem(2);

		boxPolarRadius->setCurrentText(formulas[0]);
		boxPolarTheta->setCurrentText(formulas[1]);
		boxPolarParameter->setText(c->variable());
		boxPolarFrom->setText(QString::number(c->startRange(), 'g', 15));
		boxPolarTo->setText(QString::number(c->endRange(), 'g', 15));
		boxPolarPoints->setValue(c->dataSize());
	}
	else if (c->functionType() == FunctionCurve::Parametric)
	{
		boxType->setCurrentItem(1);
		optionStack->raiseWidget(1);

		boxXFunction->setCurrentText(formulas[0]);
		boxYFunction->setCurrentText(formulas[1]);
		boxParameter->setText(c->variable());
		boxParFrom->setText(QString::number(c->startRange(), 'g', 15));
		boxParTo->setText(QString::number(c->endRange(), 'g', 15));
		boxParPoints->setValue(c->dataSize());
	}
}

void FunctionDialog::clearList()
{
	int type=boxType->currentItem();
	switch (type)
	{
		case 0:
			boxFunction->clear();
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

void FunctionDialog::acceptFunction()
{
	QString from=boxFrom->text().lower();
	QString to=boxTo->text().lower();
	QString points=boxPoints->text().lower();

	double start, end;
	try
	{
		MyParser parser;
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
		MyParser parser;
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
	QString formula=boxFunction->text().remove("\n");
	bool error=false;

	try
	{
		MyParser parser;	
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
		error=true;	
	}	

	// Collecting all the information
	int type = boxType->currentItem();
	QStringList formulas;
	QList<double> ranges;
	formulas+=formula;
	ranges+=start;
	ranges+=end;
	if (!error)
	{
		if (!graph)
			emit newFunctionPlot(type, formulas, "x", ranges, boxPoints->value());
		else
		{
			if (curveID >= 0)
				graph->modifyFunctionCurve(curveID, type, formulas, "x", ranges, boxPoints->value());
			else
				graph->addFunctionCurve(type,formulas, "x", ranges, boxPoints->value());

			ApplicationWindow *app = (ApplicationWindow *)this->parent();
			app->updateFunctionLists(type,formulas);
		}
	}		

}
void FunctionDialog::acceptParametric()
{
	QString from=boxParFrom->text().lower();
	QString to=boxParTo->text().lower();
	QString points=boxParPoints->text().lower();

	double start, end;
	try
	{
		MyParser parser;
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
		MyParser parser;
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
	bool error=false;

	try
	{
		MyParser parser;				
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
		error=true;	
	}	
	try
	{
		MyParser parser;				
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
		error=true;	
	}
	// Collecting all the information
	int type = boxType->currentItem();
	QStringList formulas;
	QList<double> ranges;
	formulas+=xformula;
	formulas+=yformula;
	ranges+=start;
	ranges+=end;
	if (!error)
	{
		if (!graph)
			emit newFunctionPlot(type, formulas, boxParameter->text(),ranges, boxParPoints->value());
		else
		{
			if (curveID >= 0)
				graph->modifyFunctionCurve(curveID, type, formulas, boxParameter->text(),ranges, boxParPoints->value());
			else
				graph->addFunctionCurve(type,formulas, boxParameter->text(),ranges, boxParPoints->value());

			ApplicationWindow *app = (ApplicationWindow *)this->parent();
			app->updateFunctionLists(type,formulas);
		}
	}
}

void FunctionDialog::acceptPolar()
{
	QString from=boxPolarFrom->text().lower();
	QString to=boxPolarTo->text().lower();
	QString points=boxPolarPoints->text().lower();

	double start, end;
	try
	{
		MyParser parser;
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
		MyParser parser;	
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
	bool error=false;

	try
	{
		MyParser parser;				
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
		error=true;	
	}	
	try
	{
		MyParser parser;				
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
		error=true;	
	}
	// Collecting all the information
	int type = boxType->currentItem();
	QStringList formulas;
	QList<double> ranges;
	formulas+=rformula;
	formulas+=tformula;
	ranges+=start;
	ranges+=end;
	if (!error)
	{
		ApplicationWindow *app = (ApplicationWindow *)this->parent();
		app->updateFunctionLists(type,formulas);

		if (!graph)
			emit newFunctionPlot(type, formulas, boxParameter->text(),ranges, boxPolarPoints->value());
		else
		{
			if (curveID >= 0)
				graph->modifyFunctionCurve(curveID, type, formulas, boxPolarParameter->text(),ranges, boxPolarPoints->value());
			else
				graph->addFunctionCurve(type, formulas, boxPolarParameter->text(),ranges, boxPolarPoints->value());
		}
	}
}

void FunctionDialog::accept()
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

void FunctionDialog::insertParamFunctionsList(const QStringList& xList, const QStringList& yList)
{
	boxXFunction->insertStringList (xList, 1);
	boxYFunction->insertStringList (yList, 1);
}

void FunctionDialog::insertPolarFunctionsList(const QStringList& rList, const QStringList& tetaList)
{
	boxPolarRadius->insertStringList (rList, 1);
	boxPolarTheta->insertStringList (tetaList, 1);	
}

FunctionDialog::~FunctionDialog()
{
}
