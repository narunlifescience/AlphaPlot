/***************************************************************************
    File                 : FunctionDialog.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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
#include "FunctionDialog.h"
#include "MyParser.h"
#include "ApplicationWindow.h"
#include "FunctionCurve.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QWidget>
#include <QMessageBox>

FunctionDialog::FunctionDialog( QWidget* parent, Qt::WFlags fl )
: QDialog( parent, fl )
{
	setWindowTitle( tr( "Add function curve" ) );
	setSizeGripEnabled(true);

	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(new QLabel(tr( "Curve type " )));
	boxType = new QComboBox();
	boxType->addItem( tr( "Function" ) );
	boxType->addItem( tr( "Parametric plot" ) );
	boxType->addItem( tr( "Polar plot" ) );
	hbox1->addWidget(boxType);

	optionStack = new QStackedWidget();
	optionStack->setFrameShape( QFrame::StyledPanel );
	optionStack->setFrameShadow( QStackedWidget::Plain );

	QGridLayout *gl1 = new QGridLayout();
    gl1->addWidget(new QLabel(tr( "f(x)= " )), 0, 0);
	boxFunction = new QTextEdit();
	boxFunction->setMinimumWidth(350);
	gl1->addWidget(boxFunction, 0, 1);
	gl1->addWidget(new QLabel(tr( "From x= " )), 1, 0);
	boxFrom = new QLineEdit();
	boxFrom->setText("0");
	gl1->addWidget(boxFrom, 1, 1);
	gl1->addWidget(new QLabel(tr( "To x= " )), 2, 0);
	boxTo = new QLineEdit();
	boxTo->setText("1");
	gl1->addWidget(boxTo, 2, 1);
	gl1->addWidget(new QLabel(tr( "Points" )), 3, 0);
	boxPoints = new QSpinBox();
	boxPoints->setRange(2, 1000000);
	boxPoints->setSingleStep(100);
	boxPoints->setValue(100);
	gl1->addWidget(boxPoints, 3, 1);

	functionPage = new QWidget();
	functionPage->setLayout(gl1);
	optionStack->addWidget( functionPage );

	QGridLayout *gl2 = new QGridLayout();
    gl2->addWidget(new QLabel(tr( "Parameter" )), 0, 0);
	boxParameter = new QLineEdit();
	boxParameter->setText("m");
	gl2->addWidget(boxParameter, 0, 1);
	gl2->addWidget(new QLabel(tr( "From" )), 1, 0);
	boxParFrom = new QLineEdit();
	boxParFrom->setText("0");
	gl2->addWidget(boxParFrom, 1, 1);
	gl2->addWidget(new QLabel(tr( "To" )), 2, 0);
	boxParTo = new QLineEdit();
	boxParTo->setText("1");
	gl2->addWidget(boxParTo, 2, 1);
	gl2->addWidget(new QLabel(tr( "x = " )), 3, 0);
	boxXFunction = new QComboBox( );
	boxXFunction->setEditable ( true );
	gl2->addWidget(boxXFunction, 3, 1);
	gl2->addWidget(new QLabel(tr( "y = " )), 4, 0);
	boxYFunction = new QComboBox( );
	boxYFunction->setEditable ( true );
	gl2->addWidget(boxYFunction, 4, 1);
	gl2->addWidget(new QLabel(tr( "Points" )), 5, 0);
	boxParPoints = new QSpinBox();
	boxParPoints->setRange(2, 1000000);
	boxParPoints->setSingleStep(100);
	boxParPoints->setValue(100);
	gl2->addWidget(boxParPoints, 5, 1);

	parametricPage = new QWidget();
	parametricPage->setLayout(gl2);
	optionStack->addWidget( parametricPage );

	QGridLayout *gl3 = new QGridLayout();
    gl3->addWidget(new QLabel(tr( "Parameter" )), 0, 0);
	boxPolarParameter = new QLineEdit();
	boxPolarParameter->setText ("t");
	gl3->addWidget(boxPolarParameter, 0, 1);
	gl3->addWidget(new QLabel(tr( "From" )), 2, 0);
	boxPolarFrom = new QLineEdit();
	boxPolarFrom->setText("0");
	gl3->addWidget(boxPolarFrom, 2, 1);
	gl3->addWidget(new QLabel(tr( "To" )), 3, 0);
	boxPolarTo = new QLineEdit();
	boxPolarTo->setText("pi");
	gl3->addWidget(boxPolarTo, 3, 1);
	gl3->addWidget(new QLabel(tr( "R =" )), 4, 0);
	boxPolarRadius = new QComboBox();
	boxPolarRadius->setEditable ( true );
	gl3->addWidget(boxPolarRadius, 4, 1);
	gl3->addWidget(new QLabel(tr( "Theta =" )), 5, 0);
	boxPolarTheta = new QComboBox();
	boxPolarTheta->setEditable ( true );
	gl3->addWidget(boxPolarTheta, 5, 1);
	gl3->addWidget(new QLabel(tr( "Points" )), 6, 0);
	boxPolarPoints = new QSpinBox();
	boxPolarPoints->setRange(2, 1000000);
	boxPolarPoints->setSingleStep(100);
	boxPolarPoints->setValue(100);
	gl3->addWidget(boxPolarPoints, 6, 1);

	polarPage = new QWidget();
	polarPage->setLayout(gl3);
	optionStack->addWidget( polarPage );

	buttonApply = new QPushButton(tr("&Apply"));
	buttonClear = new QPushButton(tr( "Clear Function" ));
	buttonOk = new QPushButton(tr( "Ok" ));
	buttonOk->setDefault(true);
	buttonCancel = new QPushButton(tr( "Close" ));

	QHBoxLayout *hbox2 = new QHBoxLayout();
	hbox2->addWidget(buttonApply);
	hbox2->addStretch();
	hbox2->addWidget(buttonClear);
	hbox2->addWidget(buttonOk);
	hbox2->addWidget(buttonCancel);

	QVBoxLayout *vbox1 = new QVBoxLayout();
    vbox1->addLayout(hbox1);
	vbox1->addWidget(optionStack);
	vbox1->addLayout(hbox2);

	setLayout(vbox1);
	languageChange();
	setFocusProxy (boxFunction);

	connect( boxType, SIGNAL( activated(int) ), this, SLOT( raiseWidget(int) ) );
	connect(buttonApply, SIGNAL(clicked()), this, SLOT(apply()));
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonClear, SIGNAL( clicked() ), this, SLOT(clearList() ) );

	curveID = -1;
	graph = 0;
}

void FunctionDialog::raiseWidget(int index)
{
	if (index)
		buttonClear->setText( tr( "Clear list" ) );
	else
		buttonClear->setText( tr( "Clear Function" ) );

	optionStack->setCurrentIndex(index);
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
		optionStack->setCurrentIndex(2);
		boxType->setCurrentIndex(2);

		boxPolarRadius->setCurrentText(formulas[0]);
		boxPolarTheta->setCurrentText(formulas[1]);
		boxPolarParameter->setText(c->variable());
		boxPolarFrom->setText(QString::number(c->startRange(), 'g', 15));
		boxPolarTo->setText(QString::number(c->endRange(), 'g', 15));
		boxPolarPoints->setValue(c->dataSize());
	}
	else if (c->functionType() == FunctionCurve::Parametric)
	{
		boxType->setCurrentIndex(1);
		optionStack->setCurrentIndex(1);

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

bool FunctionDialog::acceptFunction()
{
	QString from=boxFrom->text().toLower();
	QString to=boxTo->text().toLower();
	QString points=boxPoints->text().toLower();

	double start, end;
	try
	{
		MyParser parser;
		parser.SetExpr(from.toAscii().constData());
		start=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("Start limit error"), QString::fromStdString(e.GetMsg()));
		boxFrom->setFocus();
		return false;
	}
	try
	{
		MyParser parser;
		parser.SetExpr(to.toAscii().constData());
		end=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("End limit error"), QString::fromStdString(e.GetMsg()));
		boxTo->setFocus();
		return false;
	}

	if (start>=end)
	{
		QMessageBox::critical(0, tr("Input error"),
				tr("Please enter x limits that satisfy: from < end!"));
		boxTo->setFocus();
		return false;
	}

	QString formula = boxFunction->text().replace(QChar::ParagraphSeparator,"\n");

	// Collecting all the information
	int type = boxType->currentItem();
	QStringList formulas;
	QList<double> ranges;
	formulas+=formula;
	ranges+=start;
	ranges+=end;

	bool result;
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	app->updateFunctionLists(type,formulas);
	if (!graph)
		result = app->newFunctionPlot(type, formulas, "x", ranges, boxPoints->value());
	else {
		if (curveID >= 0)
			result = graph->modifyFunctionCurve(app, curveID, type, formulas, "x", ranges, boxPoints->value());
		else
			result = graph->addFunctionCurve(app, type,formulas, "x", ranges, boxPoints->value());
	}
	if (!result) boxFunction->setFocus();
	return result;
}

bool FunctionDialog::acceptParametric()
{
	QString from=boxParFrom->text().toLower();
	QString to=boxParTo->text().toLower();
	QString points=boxParPoints->text().toLower();

	double start, end;
	try
	{
		MyParser parser;
		parser.SetExpr(from.toAscii().constData());
		start=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("Start limit error"), QString::fromStdString(e.GetMsg()));
		boxParFrom->setFocus();
		return false;
	}

	try
	{
		MyParser parser;
		parser.SetExpr(to.toAscii().constData());
		end=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("End limit error"), QString::fromStdString(e.GetMsg()));
		boxParTo->setFocus();
		return false;
	}

	if (start>=end)
	{
		QMessageBox::critical(0, tr("Input error"),
				tr("Please enter parameter limits that satisfy: from < end!"));
		boxParTo->setFocus();
		return false;
	}

	QString xformula=boxXFunction->currentText().replace(QChar::ParagraphSeparator,"\n");
	QString yformula=boxYFunction->currentText().replace(QChar::ParagraphSeparator,"\n");

	// Collecting all the information
	int type = boxType->currentItem();
	QStringList formulas;
	QList<double> ranges;
	formulas+=xformula;
	formulas+=yformula;
	ranges+=start;
	ranges+=end;

	bool result;
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	app->updateFunctionLists(type,formulas);
	if (!graph)
		result = app->newFunctionPlot(type, formulas, boxParameter->text(),ranges, boxParPoints->value());
	else {
		if (curveID >= 0)
			result = graph->modifyFunctionCurve(app, curveID, type, formulas, boxParameter->text(),ranges, boxParPoints->value());
		else
			result = graph->addFunctionCurve(app, type,formulas, boxParameter->text(),ranges, boxParPoints->value());
	}
	if (!result) boxXFunction->setFocus();
	return result;
}

bool FunctionDialog::acceptPolar()
{
	QString from=boxPolarFrom->text().toLower();
	QString to=boxPolarTo->text().toLower();
	QString points=boxPolarPoints->text().toLower();

	double start, end;
	try
	{
		MyParser parser;
		parser.SetExpr(from.toAscii().constData());
		start=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("Start limit error"), QString::fromStdString(e.GetMsg()));
		boxPolarFrom->setFocus();
		return false;
	}

	try
	{
		MyParser parser;
		parser.SetExpr(to.toAscii().constData());
		end=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0, tr("End limit error"), QString::fromStdString(e.GetMsg()));
		boxPolarTo->setFocus();
		return false;
	}

	if (start>=end)
	{
		QMessageBox::critical(0, tr("Input error"),
				tr("Please enter parameter limits that satisfy: from < end!"));
		boxPolarTo->setFocus();
		return false;
	}

	QString rformula=boxPolarRadius->currentText().replace(QChar::ParagraphSeparator,"\n");
	QString tformula=boxPolarTheta->currentText().replace(QChar::ParagraphSeparator,"\n");

	// Collecting all the information
	int type = boxType->currentItem();
	QStringList formulas;
	QList<double> ranges;
	formulas+=rformula;
	formulas+=tformula;
	ranges+=start;
	ranges+=end;

	bool result;
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	app->updateFunctionLists(type,formulas);

	if (!graph)
		result = app->newFunctionPlot(type, formulas, boxPolarParameter->text(),ranges, boxPolarPoints->value());
	else {
		if (curveID >= 0)
			result = graph->modifyFunctionCurve(app, curveID, type, formulas, boxPolarParameter->text(),ranges, boxPolarPoints->value());
		else
			result = graph->addFunctionCurve(app, type, formulas, boxPolarParameter->text(),ranges, boxPolarPoints->value());
		}
	if (!result)
		boxPolarRadius->setFocus();
	return result;
}

void FunctionDialog::accept()
{
	if (apply())
		close();
}

bool FunctionDialog::apply() {
	bool result = true;
	switch (boxType->currentIndex())
	{
		case 0:
			result = acceptFunction();
			break;

		case 1:
			result = acceptParametric();
			break;

		case 2:
			result = acceptPolar();
			break;
	}
	return result;
}

void FunctionDialog::insertParamFunctionsList(const QStringList& xList, const QStringList& yList)
{
	boxXFunction->insertItems (0, xList);
	boxYFunction->insertItems (0, yList);
}

void FunctionDialog::insertPolarFunctionsList(const QStringList& rList, const QStringList& thetaList)
{
	boxPolarRadius->insertItems (0, rList);
	boxPolarTheta->insertItems (0, thetaList);
}

