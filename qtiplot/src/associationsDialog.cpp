#include "associationsDialog.h"
#include "worksheet.h"

#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qvbox.h>
#include <qtable.h>
#include <qheader.h>
#include <qwidgetlist.h>
#include <qcheckbox.h>

#include <qmessagebox.h>

associationsDialog::associationsDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "associationsDialog" );

    setCaption( tr( "QtiPlot - Plot Associations" ) );
	setFocus();
	
	QVBox *box1 = new QVBox (this, "box1"); 
	box1->setSpacing (5);
	box1->setMargin(5);

	QHBox *hbox1 = new QHBox (box1, "hbox1"); 
	box1->setSpacing (5);

	new QLabel(tr("Spreadsheet: "), hbox1, "table");
	tableCaptionLabel = new QLabel(hbox1, "table");

	table = new QTable(3, 5, box1, "table");
	table->setLeftMargin ( 0 );
	table->setColumnReadOnly (0, true);
	table->setCurrentCell ( -1, -1 );
	table->setMaximumHeight(8*table->rowHeight(0));
	for (int i=0; i < table->numCols(); i++ )
		table->setColumnStretchable (0, true);

	associations = new QListBox( box1, "available" );
	associations->setSelectionMode ( QListBox::Single );

	QHeader *header = table->horizontalHeader ();
	header->setLabel(0, tr("Column"));
	header->setLabel(1, tr("X"));
	header->setLabel(2, tr("Y"));
	header->setLabel(3, tr("xErr"));
	header->setLabel(4, tr("yErr"));
	header->setClickEnabled ( false );

	QHBox  *box2=new QHBox (this, "box2"); 
	box2->setMargin(5);
	box2->setSpacing (5);

	btnApply = new QPushButton(box2, "btnApply" );
    btnApply->setText( tr( "&Update curve" ) );

    btnOK = new QPushButton(box2, "btnOK" );
    btnOK->setText( tr( "&OK" ) );
	btnOK->setDefault( TRUE );
	
    btnCancel = new QPushButton(box2, "btnCancel" );
    btnCancel->setText( tr( "&Cancel" ) );

	QVBoxLayout* layout = new QVBoxLayout(this,5,5, "hlayout3");
    layout->addWidget(box1);
	layout->addWidget(box2);

connect(associations, SIGNAL(highlighted (int)), this, SLOT(updateTable(int)));
connect(btnOK, SIGNAL(clicked()),this, SLOT(accept()));
connect(btnCancel, SIGNAL(clicked()),this, SLOT(close()));
connect(btnApply, SIGNAL(clicked()),this, SLOT(updateCurve()));
}

void associationsDialog::accept()
{
updateCurve();
close();
}

void associationsDialog::updateCurve()
{
int index = associations->currentItem();
Table *t = findTable(index);
if (t && graph)
	{
	graph->changePlotAssociation(t, index, plotAssociation(associations->currentText()));
	graph->updatePlot();
	}
}


QString associationsDialog::plotAssociation(const QString& text)
{
QString s = text;
QStringList lst= QStringList::split(": ", s, false);
QStringList cols = QStringList::split(",", lst[1], false);

QString tableName = lst[0];
s = tableName + "_" + cols[0];
for (int i=1; i < (int)cols.count(); i++ )
	s+="," + tableName + "_" + cols[i];	
return s;
}

void associationsDialog::initTablesList(QWidgetList* lst, int curve)
{
tables = lst;
active_table = 0;
associations->setCurrentItem(curve);
}

Table * associationsDialog::findTable(int index)
{
QString text = associations->text(index);
QStringList lst= QStringList::split(":", text, false);
for (int i=0; i < (int)tables->count(); i++ )
	{
	if (tables->at(i)->name() == lst[0])
		return (Table *)tables->at(i);
	}
return 0;
}

void associationsDialog::updateTable(int index)
{
Table *t = findTable(index);
if (!t)
	return;

if (active_table != t)
	{
	active_table = t;
	tableCaptionLabel->setText(t->name());
	table->setNumRows(t->tableCols());
	QStringList colNames = t->colNames();
	int i;
	for (i=0; i < table->numRows(); i++ )
		table->setText(i, 0, colNames[i]);

	for (int j=1; j < table->numCols(); j++)
		{
		for (i=0; i < table->numRows(); i++ )
			{
			QCheckBox* cb = new QCheckBox(table, 0);
			cb->installEventFilter(this);
			table->setCellWidget(i, j, cb);
			}
		table->adjustColumn (j);
		}
	table->adjustColumn (0);
	}
updateColumnTypes();
}

void associationsDialog::updateColumnTypes()
{
QString text = associations->currentText();

QStringList lst= QStringList::split(": ", text, false);
QStringList cols = QStringList::split(",", lst[1], false);

QString xColName = cols[0].remove("(X)");
QString yColName = cols[1].remove("(Y)");

QHeader *header = table->horizontalHeader();
int i, n = (int)cols.count();

if (n == 2)
	{
	table->hideColumn(3);
	table->hideColumn(4);
	}

QCheckBox *it = 0;
for (i=0; i < table->numRows(); i++ )
	{
	it = (QCheckBox *)table->cellWidget(i, 1);
	if (table->text(i,0) == xColName)
		it->setChecked(true);
	else
		it->setChecked(false);

	it = (QCheckBox *)table->cellWidget(i, 2);
	if (table->text(i,0) == yColName)
		it->setChecked(true);
	else
		it->setChecked(false);
	}

bool xerr = false, yerr = false, vectXYXY = false;
QString errColName, xEndColName, yEndColName;
if (n > 2)
	{
	table->showColumn(3);
	table->showColumn(4);

	if (cols[2].contains("(xErr)") || cols[2].contains("(yErr)"))
		{//if error bars
		header->setLabel(3, tr("xErr"));
		header->setLabel(4, tr("yErr"));
		}

	if (cols[2].contains("(xErr)"))
		{
		xerr = true;
		errColName = cols[2].remove("(xErr)");
		}
	else if (cols[2].contains("(yErr)"))
		{
		yerr = true;
		errColName = cols[2].remove("(yErr)");
		}
	else if (cols.count() > 3 && cols[2].contains("(X)") && cols[3].contains("(Y)"))
		{
		vectXYXY = true;
		xEndColName = cols[2].remove("(X)");
		yEndColName = cols[3].remove("(Y)");
		header->setLabel(3, tr("xEnd"));
		header->setLabel(4, tr("yEnd"));
		}
	}

for (i=0; i < table->numRows(); i++ )
	{
	it = (QCheckBox *)table->cellWidget(i, 3);
	if (xerr || vectXYXY)
		{
		if (table->text(i,0) == errColName || table->text(i,0) == xEndColName)
			it->setChecked(true);
		else
			it->setChecked(false);
		}
	else
		it->setChecked(false);	

	it = (QCheckBox *)table->cellWidget(i, 4);
	if (yerr || vectXYXY)
		{
		if (table->text(i,0) == errColName || table->text(i,0) == yEndColName)
			it->setChecked(true);
		else
			it->setChecked(false);
		}
	else
		it->setChecked(false);
	}
}

void associationsDialog::uncheckCol(int col)
{
for (int i=0; i < table->numRows(); i++ )
	{
	QCheckBox *it = (QCheckBox *)table->cellWidget(i, col);
	if (it)
		it->setChecked(false);
	}
}

void associationsDialog::setGraph(Graph *g)
{
graph = g;

QStringList names = g->plotAssociations();
QStringList newNames;
for (int i=0;i<(int)names.count();i++)
	{
	QString s=names[i];
	int pos=s.find("_",0);
	if (pos>0)
		{
		QString table=s.left(pos);	
		QString cols=s.right(s.length()-pos-1);			
		newNames<<table+": "+cols.remove(table+"_",TRUE);
		}
	else
		newNames<<s;
	}
associations->insertStringList(newNames, -1);
plotAssociationsList = newNames;
}

void associationsDialog::updatePlotAssociation(int row, int col)
{
int index = associations->currentItem();
QString text = associations->currentText();
QStringList lst= QStringList::split(": ", text, false);
QStringList cols = QStringList::split(",", lst[1], false);

if (col == 1)
	{
	cols[0] = table->text(row, 0) + "(X)";
	text = lst[0] + ": " + cols.join(",");
	}
else if (col == 2)
	{
	cols[1] = table->text(row, 0) + "(Y)";
	text = lst[0] + ": " + cols.join(",");
	}
else if (col == 3)
	{
	if (text.contains("(X)", true) == 1)
		{
		cols[2] = table->text(row, 0) + "(xErr)";
		text = lst[0] + ": " + cols.join(",");
		uncheckCol(4);
		}
	else if (text.contains("(X)", true) == 2)
		{//vect XYXY curve
		cols[2] = table->text(row, 0) + "(X)";
		text = lst[0] + ": " + cols.join(",");
		}
	}
else if (col == 4)
	{
	if (text.contains("(X)", true) == 1)
		{
		cols[2] = table->text(row, 0) + "(yErr)";
		text = lst[0] + ": " + cols.join(",");
		uncheckCol(3);
		}
	else if (text.contains("(Y)", true) == 2)
		{//vect XYXY curve
		cols[3] = table->text(row, 0) + "(Y)";
		text = lst[0] + ": " + cols.join(",");
		}
	}

//change associations for error bars depending on the curve "index"
QString old_as = plotAssociationsList[index];
for (int i=0; i<(int)plotAssociationsList.count(); i++)
	{
	QString as = plotAssociationsList[i];
	if (as.contains(old_as) && (as.contains("(xErr)") || as.contains("(yErr)")))
		{
		QStringList ls = QStringList::split(",", as, false);
		as = text + "," + ls[2];
		plotAssociationsList[i] = as;
		}
	}

plotAssociationsList [index] = text;
associations->clear();
associations->insertStringList(plotAssociationsList, -1);
associations->setCurrentItem (index);
}

bool associationsDialog::eventFilter(QObject *object, QEvent *e)
{
QTableItem* it = (QTableItem*)object;
if (!it)
	return false;

if (e->type() == QEvent::MouseButtonPress)
    {
	if (((QCheckBox*)it)->isChecked())
		return true;

	int col = 0, row = 0;
	for (int j=1; j<table->numCols(); j++)
		{
		for (int i=0; i < table->numRows(); i++ )
			{
			QCheckBox* cb = (QCheckBox*)table->cellWidget(i, j);
			if ( cb == (QCheckBox *)object)
				{
				row = i;
				col = j;
				break;
				}
			}
		}

	uncheckCol(col);
	((QCheckBox*)it)->setChecked(true);
	
	updatePlotAssociation(row, col);
	return true;
	}
else if (e->type() == QEvent::MouseButtonDblClick)
	return true;
else
	return false;
}

associationsDialog::~associationsDialog()
{
delete tables;
}
