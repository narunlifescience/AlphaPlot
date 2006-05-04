#include "fitDialog.h"
#include "parser.h"
#include "graph.h"
#include "application.h"
#include "colorBox.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qregexp.h> 
#include <qhbox.h> 
#include <qwidgetstack.h>
#include <qvbox.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qregexp.h>
#include <qlibrary.h>
#include <qdir.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qtable.h>
#include <qheader.h>

#include <stdio.h> 

fitDialog::fitDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
if ( !name )
	setName( "fitDialog" );
setCaption(tr("QtiPlot - Non-linear curve fit"));
setSizeGripEnabled( true );
	
tw = new QWidgetStack( this, "tw" );
tw->setSizePolicy(QSizePolicy (QSizePolicy::Preferred, QSizePolicy::Preferred, 2, 0, FALSE ));
initEditPage();
initFitPage();

tw->addWidget(editPage, 0);
tw->addWidget(fitPage, 1);

QVBoxLayout* hlayout = new QVBoxLayout(this, 5, 5, "hlayout");
hlayout->addWidget(tw);

setBuiltInFunctionNames();
setBuiltInFunctions();

categoryBox->setCurrentItem(2);
funcBox->setCurrentItem(0);

loadPlugins();
}

void fitDialog::initFitPage()
{
fitPage = new QWidget( tw, "fitPage" );
QButtonGroup *GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""), fitPage,"GroupBox1" );
	
new QLabel( tr("Curve"), GroupBox1, "TextLabel22",0 );
boxCurve = new QComboBox(GroupBox1, "boxCurve" );

new QLabel( tr("Function"), GroupBox1, "TextLabel2",0 );
lblFunction = new QLabel(GroupBox1, "boxOrder" );

new QLabel(QString::null, GroupBox1, "TextLabel2",0 );
boxFunction = new QTextEdit(GroupBox1, "boxOrder" );
boxFunction->setReadOnly(true);
boxFunction->setMaximumHeight(60);

new QLabel( tr("Initial guesses"), GroupBox1, "TextLabel23",0 );
boxParams = new QTable(GroupBox1, "boxParams");
boxParams->setNumCols(2);
QStringList header;
header << tr("Parameter") << tr("Value");
boxParams->setColumnLabels(header);
boxParams->setColumnReadOnly(0, true);
boxParams->setColumnStretchable(1, true);

new QLabel( tr("Algorithm"), GroupBox1, "TextLabel44",0 );
boxSolver = new QComboBox(GroupBox1, "boxSolver" );
boxSolver->insertItem(tr("Scaled Levenberg-Marquardt"));
boxSolver->insertItem(tr("Unscaled Levenberg-Marquardt"));
boxSolver->insertItem(tr("Nelder-Mead Simplex"));

new QLabel( tr("Color"), GroupBox1, "boxColorLabel",0 );
boxColor = new ColorBox( FALSE, GroupBox1);
boxColor->setColor(QColor(red));
	
QHBox *hbox=new QHBox(fitPage,"hbox");
hbox->setSpacing(5);
	
QButtonGroup* GroupBox4 = new QButtonGroup(2,QGroupBox::Horizontal,tr( "" ),hbox, "GroupBox4" );
	
new QLabel( tr("From x="), GroupBox4, "TextLabel3",0 );
boxFrom = new QLineEdit(GroupBox4, "boxFrom" );
	
new QLabel( tr("To x="), GroupBox4, "TextLabel5",0 );
boxTo = new QLineEdit(GroupBox4, "boxOrder" );
	
QButtonGroup *GroupBox3 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),hbox,"GroupBox3" );
			
new QLabel( tr("Iterations"), GroupBox3, "TextLabel4",0 );
boxPoints = new QSpinBox(10, 10000, 50, GroupBox3, "boxStart" );
boxPoints->setValue(1000);
	
new QLabel( tr("Tolerance"), GroupBox3, "TextLabel41",0 );
boxTolerance = new QLineEdit(GroupBox3, "boxTolerance" );
boxTolerance->setText("1e-4");

QButtonGroup *GroupBox2 = new QButtonGroup(4,QGroupBox::Horizontal,tr(""),fitPage,"GroupBox2" );
GroupBox2->setFlat (TRUE);
GroupBox2->setLineWidth (0);

buttonEdit = new QPushButton(GroupBox2, "buttonOk" );
buttonEdit->setText( tr( "<< &Edit function" ) );

btnDeleteTables = new QPushButton(GroupBox2, "btnDeleteTables" );
btnDeleteTables->setText( tr( "&Delete Fit Curves" ) );
	
buttonOk = new QPushButton(GroupBox2, "buttonOk" );
buttonOk->setText( tr( "&Fit" ) );
buttonOk->setAutoDefault( TRUE );
buttonOk->setDefault( TRUE );
  
buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
buttonCancel->setText( tr( "&Cancel" ) );

QVBoxLayout* hlayout = new QVBoxLayout(fitPage, 5, 5, "hlayout");
hlayout->addWidget(GroupBox1);
hlayout->addWidget(hbox);
hlayout->addWidget(GroupBox2);

// signals and slots connections
connect( boxCurve, SIGNAL( activated(int) ), this, SLOT( activateCurve(int) ) );
connect( buttonOk, SIGNAL( clicked() ), this, SLOT(accept()));
connect( buttonCancel, SIGNAL( clicked() ), this, SLOT(close()));
connect( buttonEdit, SIGNAL( clicked() ), this, SLOT(showEditPage()));
connect( btnDeleteTables, SIGNAL( clicked() ), (ApplicationWindow *)this->parent(), SLOT(deleteFitTables()));
	
setFocusProxy(boxFunction);
}

void fitDialog::initEditPage()
{
editPage = new QWidget( tw, "editPage" );

QHBox *hbox1=new QHBox(editPage,"hbox1");
hbox1->setSpacing(5);

QVBox *vbox1=new QVBox(hbox1,"vbox1");
vbox1->setSpacing(5);
new QLabel( tr("Category"), vbox1, "TextLabel41",0 );
categoryBox = new QListBox( vbox1, "categoryBox" );
categoryBox->insertItem(tr("User defined"));
categoryBox->insertItem(tr("Built-in"));
categoryBox->insertItem(tr("Basic"));
categoryBox->insertItem(tr("Plugins"));
categoryBox->setSizePolicy(QSizePolicy (QSizePolicy::Fixed, QSizePolicy::Expanding, 2, 0, FALSE ));

QVBox *vbox2=new QVBox(hbox1,"vbox2");
vbox2->setSpacing(5);
new QLabel( tr("Function"), vbox2, "TextLabel41",0 );
funcBox = new QListBox( vbox2, "funcBox" );

QVBox *vbox3=new QVBox(hbox1,"vbox3");
vbox3->setSpacing(5);
new QLabel( tr("Expresion"), vbox3, "TextLabel41",0 );
explainBox = new QTextEdit( vbox3, "explainBox" );
explainBox->setReadOnly(true);

QHBox *hbox3=new QHBox(editPage,"hbox3");
hbox3->setSpacing(5);

boxUseBuiltIn = new QCheckBox(hbox3,"boxUseBuiltIn");
boxUseBuiltIn->setText(tr("Fit with &built-in function"));
boxUseBuiltIn->hide();

buttonPlugins = new QPushButton(hbox3, "buttonPlugins" );
buttonPlugins->setText( tr( "&Choose plugins folder..." ) );
buttonPlugins->hide();

QButtonGroup *GroupBox1 = new QButtonGroup( 3,QGroupBox::Horizontal,tr(""),editPage,"GroupBox3" );

new QLabel( tr("Name"), GroupBox1, "TextLabel41",0 );
boxName = new QLineEdit(GroupBox1, "boxName" );
boxName->setText("user1");

btnAddFunc = new QPushButton(GroupBox1, "btnAddFunc" );
btnAddFunc->setText( tr( "&Save" ) );

new QLabel( tr("Parameters"), GroupBox1, "TextLabel41",0 );
boxParam = new QLineEdit(GroupBox1, "boxParam" );
boxParam->setText("a, b");
	
btnDelFunc = new QPushButton(GroupBox1, "btnDelFunc" );
btnDelFunc->setText( tr( "&Remove" ) );

QHBox *hbox2=new QHBox(editPage,"hbox2");
hbox2->setSpacing(5);

editBox = new QTextEdit( hbox2, "editBox" );
editBox->setTextFormat(Qt::PlainText);
editBox->setFocus();

QVBox *vbox4=new QVBox(hbox2,"vbox4");
vbox4->setSpacing(5);

btnAddTxt = new QPushButton(vbox4, "btnAddTxt" );
btnAddTxt->setText( tr( "Add &expresion" ) );

btnAddName = new QPushButton(vbox4, "btnClose" );
btnAddName->setText( tr( "Add &name" ) );

buttonClear = new QPushButton(vbox4, "buttonClear" );
buttonClear->setText( tr( "Clear user &list" ) );

btnContinue = new QPushButton(vbox4, "btnContinue" );
btnContinue->setText( tr( "&Fit >>" ) );
	
QVBoxLayout* hlayout = new QVBoxLayout(editPage, 5, 5, "hlayout");
hlayout->addWidget(hbox1);
hlayout->addWidget(hbox3);
hlayout->addWidget(GroupBox1);
hlayout->addWidget(hbox2);

connect( buttonPlugins, SIGNAL( clicked() ), this, SLOT(choosePluginsFolder()));
connect( buttonClear, SIGNAL( clicked() ), this, SLOT(clearList()));
connect( categoryBox, SIGNAL(highlighted(int)), this, SLOT(showFunctionsList(int) ) );
connect( funcBox, SIGNAL(highlighted(int)), this, SLOT(showExpresion(int)));
connect( boxUseBuiltIn, SIGNAL(toggled(bool)), this, SLOT(setFunction(bool) ) );
connect( btnAddName, SIGNAL(clicked()), this, SLOT(addFunctionName() ) );
connect( btnAddTxt, SIGNAL(clicked()), this, SLOT(addFunction() ) );
connect( btnContinue, SIGNAL(clicked()), this, SLOT(showFitPage() ) );
connect( btnAddFunc, SIGNAL(clicked()), this, SLOT(saveUserFunction()));
connect( btnDelFunc, SIGNAL(clicked()), this, SLOT(removeUserFunction()));
}

void fitDialog::setGraph(Graph *g)
{
if (!g)
	return;

graph = g;
boxCurve->clear();
boxCurve->insertStringList (graph->curvesList(), -1);
	
if (g->selectorsEnabled())
	{
	int index = g->curveIndex(g->selectedCurveID());
	boxCurve->setCurrentItem(index);
	activateCurve(index);
	}
else
	activateCurve(0);

connect (graph, SIGNAL(closedGraph()), this, SLOT(close()));
connect (graph, SIGNAL(dataRangeChanged()), this, SLOT(changeDataRange()));
};

void fitDialog::activateCurve(int index)
{
QwtPlotCurve *c = graph->curve(index);
if (!c)
	return;

if (graph->selectorsEnabled() && graph->selectedCurveID() == graph->curveKey(index))
	{
	double start = graph->selectedXStartValue();
	double end = graph->selectedXEndValue();
	boxFrom->setText(QString::number(QMIN(start, end), 'g', 15));
	boxTo->setText(QString::number(QMAX(start, end), 'g', 15));
	}
else
	{
	boxFrom->setText(QString::number(c->minXValue(), 'g', 15));
	boxTo->setText(QString::number(c->maxXValue(), 'g', 15));
	}
};

void fitDialog::saveUserFunction()
{
if (editBox->text().isEmpty())
	{
	QMessageBox::critical(0, tr("QtiPlot - Input function error"),
				tr("Please enter a valid function!"));
	editBox->setFocus();
	return;
	}
else if (boxName->text().isEmpty())
	{
	QMessageBox::critical(0, tr("QtiPlot - Input function error"),
				tr("Please enter a function name!"));
	boxName->setFocus();
	return;
	}	
else if (boxParam->text().remove(QRegExp("[,;\\s]")).isEmpty())
	{
	QMessageBox::critical(0, tr("QtiPlot - Input function error"),
				tr("Please enter at least one parameter name!"));
	boxParam->setFocus();
	return;
	}

if (builtInFunctionNames.contains(boxName->text()))
	{
	QMessageBox::critical(0, tr("QtiPlot - Error: function name"),
				"<p><b>"+boxName->text()+ "</b>"+tr(" is a built-in function name"
				"<p>You must choose another name for your function!"));
	editBox->setFocus();
	return;
	}
if (editBox->text().contains(boxName->text()))
	{
	QMessageBox::critical(0, tr("QtiPlot - Input function error"),
				tr("You can't define functions recursevely!"));
	editBox->setFocus();
	return;
	}
QString name = boxName->text();
QString f = name +"(x, " + boxParam->text() + ")="+editBox->text().remove("\n");
if (userFunctionNames.contains(name))
	{
	int index = userFunctionNames.findIndex(name);
	userFunctions[index] = f;
	userFunctionParams[index] = boxParam->text();
		
	if (funcBox->currentText() == name)
		showExpresion(index);
	}
else
	{
	userFunctionNames << name;
	userFunctions << f;
	userFunctionParams << boxParam->text();

	if (categoryBox->currentItem() == 0)
		{
		funcBox->insertItem(name, -1);
		funcBox->setCurrentItem(funcBox->numRows()-1);
		}

	if ((int)userFunctionNames.count()>0 && !boxUseBuiltIn->isEnabled() && categoryBox->currentItem() == 0)
		boxUseBuiltIn->setEnabled(true);
	}

emit saveFunctionsList(userFunctions);
}

void fitDialog::removeUserFunction()
{
QString name = funcBox->currentText();
if (userFunctionNames.contains(name))
	{
	explainBox->setText(QString::null);

	int index = userFunctionNames.findIndex(name);
	userFunctionNames.remove(name);

	QString f = userFunctions[index];
	userFunctions.remove(f);

	f = userFunctionParams[index];
	userFunctionParams.remove(f);

	funcBox->clear();
	funcBox->insertStringList(userFunctionNames, -1);
	funcBox->setCurrentItem(0);

	if (!userFunctionNames.count())
		boxUseBuiltIn->setEnabled(false);

	emit saveFunctionsList(userFunctions);
	}
}

void fitDialog::showFitPage()
{
QString par = boxParam->text().simplifyWhiteSpace();
QStringList paramList = QStringList::split(QRegExp("[,;]+[\\s]*"), par, false);
boxParams->setNumRows((int)paramList.count());
if (!boxUseBuiltIn->isChecked() || 
	(boxUseBuiltIn->isChecked()&& categoryBox->currentItem()!=3 && categoryBox->currentItem()!=1))
	{
	boxParams->setNumCols(3);
	boxParams->horizontalHeader()->setLabel(2, tr("Constant"));
	for (int j=0; j<boxParams->numRows(); j++ )
		{
		QCheckTableItem *cb = new QCheckTableItem(boxParams, QString::null );
		boxParams->setItem(j, 2, cb);
		}
	boxParams->showColumn(2);
	boxParams->adjustColumn(2);
	}
else
	boxParams->setNumCols(2);

for (int i=0; i<(int)paramList.count(); i++)
	{
	boxParams->setText(i, 0, paramList[i]);
	if (boxParams->text(i, 1).isEmpty())
		boxParams->setText(i, 1, "1");
	}
boxParams->adjustColumn(0);
boxFunction->setText(editBox->text().remove("\n"));
lblFunction->setText(boxName->text() +"(x, " + par + ")");

tw->raiseWidget(fitPage);
}

void fitDialog::showEditPage()
{
tw->raiseWidget(editPage);
}

void fitDialog::setFunction(bool ok)
{
editBox->setEnabled(!ok);
boxParam->setEnabled(!ok);
boxName->setEnabled(!ok);
btnAddFunc->setEnabled(!ok);
btnAddName->setEnabled(!ok);
btnAddTxt->setEnabled(!ok);
buttonClear->setEnabled(!ok);

if (ok)
	{
	boxName->setText(funcBox->currentText());
	editBox->setText(explainBox->text());

	if (categoryBox->currentItem() == 0 && (int)userFunctionParams.size() > 0)
		boxParam->setText(userFunctionParams[funcBox->currentItem()]);
	else if (categoryBox->currentItem() == 1)
		{
		switch(funcBox->currentItem())
			{
			case 0:
				boxParam->setText("A1, A2, x0, dx");
			break;
			case 1:
				boxParam->setText("a, t, y0");
			break;
			case 2:
				boxParam->setText("a, t, y0");
			break;
			case 3:
				boxParam->setText("a1, t1, a2, t2, y0");
			break;
			case 4:
				boxParam->setText("a1, t1, a2, t2, a3, t3, y0");
			break;
			case 5:
				boxParam->setText("y0, a, xc, w");
			break;
			case 6:
				boxParam->setText("y0, a, xc, w");
			break;
			}
		}
	else if (categoryBox->currentItem() == 3 && (int)pluginParameters.size() > 0 )
			boxParam->setText(pluginParameters[funcBox->currentItem()]);
	}
}

void fitDialog::clearList()
{
userFunctions.clear();
userFunctionNames.clear();
if (categoryBox->currentItem() == 0)
	{
	funcBox->clear();
	explainBox->clear();
	}
emit clearFunctionsList();
}

void fitDialog::insertFunctionsList(const QStringList& list)
{
if (!list.count())
	{
	boxUseBuiltIn->setEnabled(false);
	return;
	}

userFunctions = list;

for (int i = 0; i<(int)list.count(); i++)
	{
	QString s = list[i];
	int pos1 = s.find("(",0);
	userFunctionNames << s.left(pos1);

	int pos2 = s.find(")",pos1);
	userFunctionParams << s.mid(pos1+4, pos2-pos1-4);
	}
}

void fitDialog::showFunctionsList(int category)
{
boxUseBuiltIn->setChecked(false);
boxUseBuiltIn->setEnabled(false);
boxUseBuiltIn->hide();
buttonPlugins->hide();
btnDelFunc->setEnabled(false);
funcBox->clear();

switch (category)
	{
	case 0:
		if ((int)userFunctionNames.size() > 0)
			{
			showUserFunctions();
			boxUseBuiltIn->setEnabled(true);
			}

		boxUseBuiltIn->setText(tr("Fit with selected &user function"));
		boxUseBuiltIn->show();
		btnDelFunc->setEnabled(true);
	break;

	case 1:
		boxUseBuiltIn->setText(tr("Fit using &built-in function"));
		boxUseBuiltIn->show();
		boxUseBuiltIn->setEnabled(true);
		funcBox->insertStringList(builtInFunctionNames, -1);
	break;

	case 2:
		showParseFunctions();
	break;

	case 3:
		buttonPlugins->show();
		boxUseBuiltIn->setText(tr("Fit using &plugin function"));
		boxUseBuiltIn->show();
		if ((int)pluginFunctionNames.size() > 0)
			{
			funcBox->insertStringList(pluginFunctionNames, -1);
			boxUseBuiltIn->setEnabled(true);
			}
	break;
	}

funcBox->setCurrentItem(0);
showExpresion(0);
}

void fitDialog::choosePluginsFolder()
{
ApplicationWindow *app = (ApplicationWindow *)this->parent();
QString dir = QFileDialog::getExistingDirectory(QDir::currentDirPath(), this, "get directory",
                    tr("Choose the plugins folder"), true, true);
if (!dir.isEmpty())
	{
	pluginFilesList.clear();
	pluginFunctionNames.clear();
	pluginFunctions.clear();
	pluginParameters.clear();
	funcBox->clear();
	explainBox->clear();

	app->fitPluginsPath = dir;
	loadPlugins();
	if ((int)pluginFunctionNames.size() > 0)
		{
		funcBox->insertStringList(pluginFunctionNames, -1);
		if (!boxUseBuiltIn->isEnabled())
			boxUseBuiltIn->setEnabled(true);

		funcBox->setCurrentItem(0);
		}
	else
		boxUseBuiltIn->setEnabled(false);
	}
}

void fitDialog::loadPlugins()
{
typedef char* (*fitFunc)();

ApplicationWindow *app = (ApplicationWindow *)this->parent();
QString path = app->fitPluginsPath + "/";
QDir dir(path);
QStringList lst = dir.entryList(QDir::Files|QDir::NoSymLinks);

for (int i=0; i<(int)lst.count(); i++)
	{
	QLibrary lib(path + lst[i]);
	lib.setAutoUnload(true);

	fitFunc name = (fitFunc) lib.resolve( "name" );
	fitFunc function = (fitFunc) lib.resolve("function");
	fitFunc params = (fitFunc) lib.resolve("parameters");

	if ( name && function && params )
		{
		pluginFunctionNames << QString(name());
		pluginFunctions << QString(function());
		pluginParameters << QString(params());
		pluginFilesList << lib.library();
		}
	}
}

void fitDialog::showUserFunctions()
{
funcBox->insertStringList (userFunctionNames, 1);
}

void fitDialog::setBuiltInFunctionNames()
{
builtInFunctionNames << "Boltzmann" << "ExpGrowth" << "ExpDecay1" << "ExpDecay2" << "ExpDecay3" 
<< "Gauss" << "Lorentz";
}

void fitDialog::setBuiltInFunctions()
{
builtInFunctions << "(A1-A2)/(1+exp((x-x0)/dx))+A2";
builtInFunctions << "y0+a*exp(x/t)";
builtInFunctions << "y0+a*exp(-x/t)";
builtInFunctions << "y0+a1*exp(-x/t1)+a2*exp(-x/t2)";
builtInFunctions << "y0+a1*exp(-x/t1)+a2*exp(-x/t2)+a3*exp(-x/t3)";
builtInFunctions << "y0+a*exp(-(x-xc)*(x-xc)/(2*w*w))";
builtInFunctions << "y0+2*a/pi*w/(4*(x-xc)*(x-xc)+w*w)";
}

void fitDialog::showParseFunctions()
{
funcBox->insertStringList(myParser::functionsList(), -1);
}

void fitDialog::showExpresion(int function)
{
if (categoryBox->currentItem() == 2)
	{
	explainBox->setText(myParser::explainFunction(function));
	}
else if (categoryBox->currentItem() == 1)
	{
	explainBox->setText(builtInFunctions[function]);
	setFunction(boxUseBuiltIn->isChecked());
	}
else if (categoryBox->currentItem() == 0)
	{
	QStringList l = QStringList::split("=", userFunctions[function], true);
	explainBox->setText(l[1]);
	setFunction(boxUseBuiltIn->isChecked());
	}
else if (categoryBox->currentItem() == 3)
	{
	if ((int)pluginFunctions.size() > 0)
		{
		explainBox->setText(pluginFunctions[function]);
		setFunction(boxUseBuiltIn->isChecked());
		}
	else
		explainBox->clear();
	}
}

void fitDialog::addFunction()
{
QString f = explainBox->text();
if (categoryBox->currentItem() == 2)
	{//basic parser function
	f = f.left(f.find("(", 0)+1);
	if (editBox->hasSelectedText())
		{	
		QString markedText=editBox->selectedText();
		editBox->insert(f+markedText+")");
		}
	else
		editBox->insert(f+")");
	}
else
	editBox->insert(f);

editBox->setFocus();
}

void fitDialog::addFunctionName()
{
editBox->insert(funcBox->currentText());
editBox->setFocus();
}

void fitDialog::accept()
{
QString curve = boxCurve->currentText();
QStringList curvesList = graph->curvesList();
if (curvesList.contains(curve) <= 0)
	{
	QMessageBox::critical(this,tr("QtiPlot - Warning"),
		tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!").arg(curve));
	boxCurve->clear();
	boxCurve->insertStringList(curvesList);
	return;
	}

if (!validInitialValues())
	return;

QString from=boxFrom->text().lower();
QString to=boxTo->text().lower();
QString tolerance=boxTolerance->text().lower();
double start,end,eps;
try
	{
	myParser parser;
	parser.SetExpr(from.ascii());
	start=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Start limit error"),e.GetMsg());
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
	QMessageBox::critical(0, tr("QtiPlot - End limit error"),e.GetMsg());
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
	
try
	{
	myParser parser;
	parser.SetExpr(tolerance.ascii());
	eps=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0, tr("QtiPlot - Tolerance input error"),e.GetMsg());
	boxTolerance->setFocus();
	return;
	}	
	
if (eps<0 || eps>=1)
	{
	QMessageBox::critical(0, tr("QtiPlot - Tolerance input error"),
				tr("The tolerance value must be positive and less than 1!"));
	boxTolerance->setFocus();
	return;
	}
	
int i, n=0, rows=boxParams->numRows();
if (boxParams->numCols() == 3)
	{
	for (i=0;i<rows;i++)
		{//count the non-constant parameters
		QCheckTableItem *it = (QCheckTableItem *)boxParams->item (i, 2);
		if (!it->isChecked())
			n++;
		}
	}
else 
	n=rows;
	
QStringList parameters, initialValues;
myParser parser;
bool error=FALSE;
QString formula = boxFunction->text();
try
	{
	bool withNames = containsUserFunctionName(formula);
	while(withNames)
		{
		for (i=0; i<(int)userFunctionNames.count(); i++)
			{
			if (formula.contains(userFunctionNames[i]))
				{
				QStringList l = QStringList::split("=", userFunctions[i], true);
				formula.replace(userFunctionNames[i], "(" + l[1] + ")");
				}
			}
		withNames = containsUserFunctionName(formula);
		}

	for (i=0; i<(int)builtInFunctionNames.count(); i++)
		{
		if (formula.contains(builtInFunctionNames[i]))
			formula.replace(builtInFunctionNames[i], "(" + builtInFunctions[i] + ")");
		}	

	double *paramsInit = new double[n];
	if (boxParams->numCols() == 3)
		{
		for (i=0;i<rows;i++)
			{
			QCheckTableItem *it = (QCheckTableItem *)boxParams->item (i, 2);
			int j = 0;
			if (!it->isChecked())
				{
				paramsInit[j]=boxParams->text(i,1).toDouble();
				parser.DefineVar(boxParams->text(i,0).ascii(), &paramsInit[j]);
				parameters<<boxParams->text(i,0);
				initialValues<<boxParams->text(i,1);
				j++;
				}
			else
				formula.replace(boxParams->text(i,0), boxParams->text(i,1));
			}
		}
	else
		{
		for (i=0;i<n;i++)
			{
			paramsInit[i]=boxParams->text(i,1).toDouble();
			parser.DefineVar(boxParams->text(i,0).ascii(), &paramsInit[i]);
			parameters<<boxParams->text(i,0);
			initialValues<<boxParams->text(i,1);
			}
		}

	parser.SetExpr(formula.ascii());		
	double x=start;
	parser.DefineVar("x", &x);	
	parser.Eval();
	delete[] paramsInit;
	}
catch(mu::ParserError &e)
	{
	QString errorMsg = boxFunction->text() + " = " + formula + "\n" + e.GetMsg() + "\n" +
					tr("Please verify that you have initialized all the parameters!");

	QMessageBox::critical(0, tr("QtiPlot - Input function error"), errorMsg);
	boxFunction->setFocus();
	error = true;	
	}

if (!error)
	{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	graph->setFitID(++app->fitNumber);
	QString result;
	if (boxUseBuiltIn->isChecked() && categoryBox->currentItem() == 1)
		result = fitBuiltInFunction(curve,funcBox->currentText(),initialValues, start,end,
			    	boxPoints->value(),boxSolver->currentItem(),eps, boxColor->currentItem());
	else if (boxUseBuiltIn->isChecked() && categoryBox->currentItem() == 3)
		{
		result = graph->fitPluginFunction(curve, pluginFilesList[funcBox->currentItem()],
									 initialValues, start, end, boxPoints->value(),
									 boxSolver->currentItem(), eps, boxColor->currentItem());
		}
	else
		result = graph->fitNonlinearCurve(curve, lblFunction->text()+"="+formula, parameters, initialValues,
				start, end, boxPoints->value(), boxSolver->currentItem(), eps, boxColor->currentItem());

	QStringList res = graph->fitResults();
	if (boxParams->numCols() == 3)
		{
		int j = 0;
		for (i=0;i<rows;i++)
			{
			QCheckTableItem *it = (QCheckTableItem *)boxParams->item (i, 2);
			if (!it->isChecked())
				boxParams->setText(i, 1, res[j++]);
			}
		}
	else
		{
		for (i=0;i<rows;i++)
			boxParams->setText(i, 1, res[i]);
		}

	app->updateLog(result);
	}
}

QString fitDialog::fitBuiltInFunction(const QString& curve, const QString& function, 
								   const QStringList& initialValues, double from,
								   double to, int iterations, int solver,
								   double tolerance, int colorIndex)
{
QString result; 
if (function == "ExpDecay1")
	{
	double amplitude = initialValues[0].toDouble();
	double damping = initialValues[1].toDouble();
	double yOffset = initialValues[2].toDouble();
	result = graph->fitExpDecay(curve, damping, amplitude, yOffset, 
							 from, to, iterations, solver, tolerance, colorIndex);
	}
else if (function == "ExpDecay2")
	{
	double amp1 = initialValues[0].toDouble();
	double t1 = initialValues[1].toDouble();
	double amp2 = initialValues[2].toDouble();
	double t2 = initialValues[3].toDouble();
	double yOffset = initialValues[4].toDouble();
	result = graph->fitExpDecay2(curve, amp1, t1, amp2, t2, yOffset, 
							 from, to, iterations, solver, tolerance, colorIndex);
	}
else if (function == "ExpDecay3")
	{
	double amp1 = initialValues[0].toDouble();
	double t1 = initialValues[1].toDouble();
	double amp2 = initialValues[2].toDouble();
	double t2 = initialValues[3].toDouble();
	double amp3 = initialValues[4].toDouble();
	double t3 = initialValues[5].toDouble();
	double yOffset = initialValues[6].toDouble();
	result = graph->fitExpDecay3(curve, amp1, t1, amp2, t2, amp3, t3, yOffset, 
							 from, to, iterations, solver, tolerance, colorIndex);
	}
else if (function == "ExpGrowth")
	{
	double amplitude = initialValues[0].toDouble();
	double damping = initialValues[1].toDouble();
	double yOffset = initialValues[2].toDouble();
	result = graph->fitExpGrowth(curve, damping, amplitude, yOffset, 
							 from, to, iterations, solver, tolerance, colorIndex);
	}
else if (function == "Boltzmann")
	{
	double A1 = initialValues[0].toDouble();
	double A2 = initialValues[1].toDouble();
	double x0 = initialValues[2].toDouble();
	double dx = initialValues[3].toDouble();
	result = graph->fitBoltzmann(curve, A1, A2, x0, dx, 
							 from, to, iterations, solver, tolerance, colorIndex);
	}
else if (function == "Gauss")
	{
	double offset = initialValues[0].toDouble();
	double amplitude = initialValues[1].toDouble();
	double center = initialValues[2].toDouble();
	double width = initialValues[3].toDouble();
	result = graph->fitGauss(curve, amplitude, center, width, offset, 
							 from, to, iterations, solver, tolerance, colorIndex);
	}
else if (function == "Lorentz")
	{
	double offset = initialValues[0].toDouble();
	double amplitude = initialValues[1].toDouble();
	double center = initialValues[2].toDouble();
	double width = initialValues[3].toDouble();
	result = graph->fitLorentz(curve, amplitude, center, width, offset, 
							 from, to, iterations, solver, tolerance, colorIndex);
	}
return result;
}

bool fitDialog::containsUserFunctionName(const QString& s)
{
bool contains = false;
for (int i=0; i<(int)userFunctionNames.count(); i++)
	{
	if (s.contains(userFunctionNames[i]))
		{
		contains = true;
		break;
		}
	}
return contains;
}

bool fitDialog::validInitialValues()
{
for (int i=0; i<boxParams->numRows(); i++)
	{
	if(boxParams->text(i,1).isEmpty())
		{
		QMessageBox::critical(0, tr("QtiPlot - Input error"),
				tr("Please enter initial guesses for your parameters!"));
		boxParams->setCurrentCell (i,1);
		return false;
		}

	try 
		{
		myParser parser;
		parser.SetExpr(boxParams->text(i,1).ascii());
		parser.Eval();
		}
	catch (mu::ParserError &e)
		{
		QMessageBox::critical(0, tr("QtiPlot - Start limit error"),e.GetMsg());
		boxParams->setCurrentCell (i,1);
		return false;
		}
	}
return true;
}

void fitDialog::changeDataRange()
{
double start = graph->selectedXStartValue();
double end = graph->selectedXEndValue();
boxFrom->setText(QString::number(QMIN(start, end), 'g', 15));
boxTo->setText(QString::number(QMAX(start, end), 'g', 15));
}

fitDialog::~fitDialog()
{
}
