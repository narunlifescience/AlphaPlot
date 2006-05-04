#include "plotWizard.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>

plotWizard::plotWizard( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "plotWizard" );
	setCaption( tr("QtiPlot - Select Columns to Plot") );
    setMouseTracking( TRUE );
    setSizeGripEnabled( TRUE );
	
	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox1" );

    new QLabel( tr( "Worksheet" ), GroupBox1, "TextLabel3",0 );
	
    boxTables = new QComboBox( FALSE,GroupBox1, "boxTables" );
	
	columnsList = new QListBox(GroupBox1, "listBox" );
	columnsList->setSizePolicy(QSizePolicy (QSizePolicy::Expanding, QSizePolicy::Fixed, 2, 0, FALSE ));

	GroupBox3 = new QButtonGroup(2,QGroupBox::Horizontal,tr(""),GroupBox1,"GroupBox3" );
	GroupBox3->setFlat (TRUE);
	GroupBox3->setLineWidth (0);
	
	buttonX = new QPushButton(GroupBox3, "buttonX" );
    buttonX->setText("<->&X");
	
	buttonXErr = new QPushButton(GroupBox3, "buttonXErr" );
    buttonXErr->setText("<->x&Err");
	
	buttonY = new QPushButton(GroupBox3, "buttonY" );
    buttonY->setText("<->&Y");
   
    buttonYErr = new QPushButton(GroupBox3, "buttonYErr" );
    buttonYErr->setText("<->yE&rr");
	
	buttonZ = new QPushButton(GroupBox3, "buttonZ" );
    buttonZ->setText("<->&Z");
	
	columnsList->setMaximumHeight(3*buttonX->height() + 20);

	buttonNew = new QPushButton(GroupBox1, "buttonCurve" );
    buttonNew->setText(tr("&New curve"));
	
	buttonDelete = new QPushButton(GroupBox1, "buttonDelete" );
    buttonDelete->setText(tr("&Delete curve"));
	
	plotAssociations = new QListBox(this, "listBox" );
	
	GroupBox2 = new QButtonGroup(2,QGroupBox::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	GroupBox2->setLineWidth (0);
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setText(tr("&Plot"));
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setText(tr("&Cancel"));
	
	QVBoxLayout* vlayout = new QVBoxLayout(this,5,5, "hlayout");
    vlayout->addWidget(GroupBox1);
	vlayout->addWidget(plotAssociations);
	vlayout->addWidget(GroupBox2);
   
    // signals and slots connections
	connect( boxTables, SIGNAL(activated(const QString &)),this, SLOT(changeColumnsList(const QString &)));
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonNew, SIGNAL( clicked() ), this, SLOT( addCurve() ) );
    connect( buttonDelete, SIGNAL( clicked() ), this, SLOT( toggleCurve() ) );
	connect( buttonX, SIGNAL( clicked() ), this, SLOT(addXCol()));
    connect( buttonY, SIGNAL( clicked() ), this, SLOT(addYCol()));
	connect( buttonXErr, SIGNAL( clicked() ), this, SLOT(addXErrCol()));
	connect( buttonYErr, SIGNAL( clicked() ), this, SLOT(addYErrCol()));
	connect( buttonZ, SIGNAL( clicked() ), this, SLOT(addZCol()));
}

void plotWizard::accept()
{
QStringList curves, curves3D, ribbons;
bool multiple=FALSE;
int i=0;
for (i=0;i<(int)plotAssociations->count ();i++)
	{
	QString text=plotAssociations->text (i);
	if (text.contains("(Z)", TRUE)>0 )
		{
		if (text.contains("(Y)", TRUE)>0 && curves3D.contains(text) == 0)
			curves3D<<text;
		else if ( text.contains("(Y)", TRUE) ==0 && ribbons.contains(text) == 0)
			ribbons<<text;
		}
	else
		{
		if (curves.contains(text)>0)
			multiple=TRUE;
		else
			curves<<text;
		}
	}
	
if (multiple)
QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("Redefinitions of the same curve are ignored!"));

if ((int)curves.count ()>0)	
	emit plot(curves);

for (i=0;i<(int)curves3D.count ();i++)
	emit plot3D(curves3D[i]);

for (i=0;i<(int)ribbons.count ();i++)
	emit plot3DRibbon(ribbons[i]);

if (!noCurves())
	close();
}

void plotWizard::changeColumnsList(const QString &table)
{	
QStringList newList;
for (int i=0;i<(int)columns.count();i++)
	{
	QString s=columns[i];
	if (s.contains(table))					
		newList<< s.remove(table+"_",TRUE);
	}
insertColumnsList(newList);
}

void plotWizard::addXCol()
{
if (noCurves())
	return;

QString text=plotAssociations->currentText();
if (text.contains("(X)",TRUE)>0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined a X column!"));
else
	{
	int item=plotAssociations->currentItem ();
	plotAssociations->changeItem (text+columnsList->currentText()+"(X)",item);
	}
}

void plotWizard::addYCol()
{
if (noCurves())
	return;

QString text=plotAssociations->currentText();
if (text.contains("(X)",TRUE) == 0)
	QMessageBox::warning(this, tr("QtiPlot - Error"),tr("You must define a X column first!"));
else if (text.contains("(Y)",TRUE)>0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined a Y column!"));
else
	{
	int item=plotAssociations->currentItem ();
	plotAssociations->changeItem (text+", "+columnsList->currentText()+"(Y)",item);
	}
}

void plotWizard::addZCol()
{
if (noCurves())
	return;

QString text=plotAssociations->currentText();
if (text.contains("(xErr)",TRUE) > 0 || text.contains("(yErr)",TRUE) > 0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("This kind of curve is not handled by QtiPlot!"));
else if (text.contains("(X)",TRUE) == 0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a X column first!"));
else if (text.contains("(Z)",TRUE)>0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined a Z column!"));
else
	{
	int item=plotAssociations->currentItem ();
	plotAssociations->changeItem (text+", "+columnsList->currentText()+"(Z)",item);
	}
}

void plotWizard::addXErrCol()
{
if (noCurves())
	return;

QString text=plotAssociations->currentText();
if (text.contains("(Z)",TRUE) > 0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("This kind of curve is not handled by QtiPlot!"));
else if (text.contains("(X)",TRUE) == 0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a X column first!"));
else if (text.contains("(Y)",TRUE) == 0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a Y column first!"));
else if (text.contains("(xErr)",TRUE)>0 || text.contains("(yErr)",TRUE)>0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined an error-bars column!"));
else
	{
	int item=plotAssociations->currentItem ();
	plotAssociations->changeItem (text+", "+columnsList->currentText()+"(xErr)",item);
	}
}

void plotWizard::addYErrCol()
{
if (noCurves())
	return;

QString text=plotAssociations->currentText();
if (text.contains("(Z)",TRUE) > 0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("This kind of curve is not handled by QtiPlot!"));
else if (text.contains("(X)",TRUE) == 0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a X column first!"));
else if (text.contains("(Y)",TRUE) == 0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a Y column first!"));
else if (text.contains("(xErr)",TRUE)>0 || text.contains("(yErr)",TRUE)>0)
	QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined an error-bars column!"));
else
	{
	int item=plotAssociations->currentItem ();
	plotAssociations->changeItem (text+", "+columnsList->currentText()+"(yErr)",item);
	}
}

void plotWizard::addCurve()
{
plotAssociations->insertItem(boxTables->currentText()+": ",-1);
plotAssociations->setCurrentItem((plotAssociations->count())-1);
}

void plotWizard::toggleCurve()
{
plotAssociations->removeItem (plotAssociations->currentItem ());
}

void plotWizard::insertTablesList(const QStringList& tables)
{
boxTables->insertStringList (tables,-1);
}

void plotWizard::insertColumnsList(const QStringList& cols)
{
columnsList->clear();
int i,n=cols.count();
for (i=0;i<n;i++)
	columnsList->insertItem(cols[i],i);

columnsList->setCurrentItem(0);
}

void plotWizard::setColumnsList(const QStringList& cols)
{
columns=cols;
}

bool plotWizard::noCurves()
{
if (plotAssociations->count () == 0)
	{
	QMessageBox::warning(0, tr("QtiPlot - Error"), tr("You must add a new curve first!"));
	return true;
	}
else
	return false;
}

plotWizard::~plotWizard()
{
}
