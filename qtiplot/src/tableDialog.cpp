#include "tableDialog.h"
#include "worksheet.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qdatetime.h> 
#include <qhbox.h>
#include <qspinbox.h>
#include <qvbox.h>
#include <qtextedit.h>
#include <qmessagebox.h>

tableDialog::tableDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "tableDialog" );

	QHBox  *hbox1=new QHBox (this, "hbox1");
	hbox1->setSpacing(5);

	QVBox  *vbox0 = new QVBox (hbox1, "vbox0");
	QHBox  *hboxa=new QHBox (vbox0, "hboxa");
	hboxa->setSpacing(5);

	new QLabel(tr( "Column Name:" ),hboxa, "TextLabel1",0 );
    colName = new QLineEdit( hboxa, "colName" );

	enumerateAllBox = new QCheckBox( vbox0, "enumerateAllBox" );
	enumerateAllBox->setText(tr("Enumerate all to the right" ));

	QHBox  *hboxb=new QHBox (vbox0, "hboxb");
	hboxb->setMaximumWidth(100);

	buttonPrev = new QPushButton( hboxb, "buttonPrev" );
	buttonPrev->setText("&<<");
	buttonPrev->setMaximumWidth(40);

	buttonNext = new QPushButton( hboxb, "buttonNext" );
	buttonNext->setText("&>>");
	buttonNext->setMaximumWidth(40);

	QVBox  *vbox01 = new QVBox (hbox1, "vbox01");
	vbox01->setSpacing(5);
	vbox01->setMargin(5);

	buttonOk = new QPushButton( vbox01, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );

	buttonApply = new QPushButton( vbox01, "buttonApply" );
	buttonApply->setText(tr("&Apply"));

	buttonCancel = new QPushButton( vbox01, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );

	GroupBox2 = new QButtonGroup(1,QGroupBox::Horizontal, tr("Options"),this,"GroupBox4" );
	
	QHBox  *hbox2 = new QHBox (GroupBox2, "hbox4");
	hbox2->setSpacing(5);

	QVBox  *vbox1 = new QVBox (hbox2, "vbox1");
	new QLabel( tr( "Plot Designation:" ), vbox1, "TextLabel2_2",0 );
	new QLabel( tr( "Display" ), vbox1, "TextLabel2_3",0 );
	labelFormat = new QLabel(tr( "Format:" ), vbox1, "TextLabel2",0 );
	labelNumeric = new QLabel(tr( "Precision:" ),vbox1, "TextLabel3",0);

	QVBox *vbox2 = new QVBox (hbox2, "vbox2");
	columnsBox = new QComboBox(vbox2, "columnsBox" );
	columnsBox->insertItem(tr("None"));
	columnsBox->insertItem(tr("X (abscissae)"));
	columnsBox->insertItem(tr("Y (ordinates)"));
	columnsBox->insertItem(tr("Z (height)"));

	displayBox = new QComboBox(vbox2, "displayBox" );
	displayBox->insertItem(tr("Numeric"));
	displayBox->insertItem(tr("Text"));
	displayBox->insertItem(tr("Date"));
	displayBox->insertItem(tr("Time"));
	displayBox->insertItem(tr("Month"));
	displayBox->insertItem(tr("Day of Week"));

	formatBox = new QComboBox( FALSE, vbox2, "formatBox" );
	precisionBox = new QSpinBox(0, 100, 1, vbox2, "precisionBox" );

	applyToRightCols = new QCheckBox( GroupBox2, "applyToRightCols" );
	applyToRightCols->setText(tr( "Apply to all columns to the right" ));
	
	QHBox *GroupBox3 = new QHBox(this,"GroupBox3" );
	GroupBox3->setSpacing(5);

	new QLabel(tr( "Column Width:" ),GroupBox3, "TextLabel1_2",0 );
	colWidth = new QSpinBox(0, 1000, 10, GroupBox3, "colWidth" );
    applyToAllBox = new QCheckBox(GroupBox3, "applyToAllBox" );
	
	QLabel *label1 = new QLabel(tr( "Comment:" ), this, "TextLabel1_22",0);
	comments = new QTextEdit(this,"comments");
	comments->setMaximumHeight(100);

	QVBoxLayout* hlayout = new QVBoxLayout(this, 5, 5, "hlayout");
    hlayout->addWidget(hbox1);
	hlayout->addWidget(GroupBox2);
	hlayout->addWidget(GroupBox3);
	hlayout->addWidget(label1);
	hlayout->addWidget(comments);

	setMaximumHeight(this->height());

    languageChange();
    setFocusProxy (colName);

   // signals and slots connections
	connect(colWidth, SIGNAL(valueChanged(int)), this, SLOT(changeColWidth(int)));
	connect(buttonApply, SIGNAL(clicked()), this, SLOT(apply()));
    connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect(applyToRightCols, SIGNAL(clicked()), this, SLOT(updateFormat()));
	connect(columnsBox, SIGNAL(activated(int)), this, SLOT(setPlotDesignation(int)) );
	connect(displayBox, SIGNAL(activated(int)), this, SLOT(updateDisplay(int)));
	connect(buttonPrev, SIGNAL(clicked()), this, SLOT(prevColumn()));
	connect(buttonNext, SIGNAL(clicked()), this, SLOT(nextColumn()));
	connect(formatBox, SIGNAL(activated(int)), this, SLOT(enablePrecision(int)) );
	connect(precisionBox, SIGNAL(valueChanged(int)), this, SLOT(updatePrecision(int)));
}

void tableDialog::enablePrecision(int f)
{
if (!f)
	{
	precisionBox->setValue(6);
	precisionBox->setEnabled(false);
	}
else
	precisionBox->setEnabled(true);
}

void tableDialog::accept()
{
apply();
close();
}

void tableDialog::prevColumn()
{
int sc = w->selectedColumn();
apply();
updateColumn(--sc);
}

void tableDialog::nextColumn()
{
int sc = w->selectedColumn();
apply();
updateColumn(++sc);
}

void tableDialog::updateColumn(int sc)
{
int colType = w->columnType(sc);

if (!sc)
	buttonPrev->setEnabled(false);
else
	buttonPrev->setEnabled(true);

if (sc >= w->tableCols() - 1)
	buttonNext->setEnabled(false);
else
	buttonNext->setEnabled(true);

w->setSelectedCol(sc);
w->table()->clearSelection ();
w->table()->selectColumn(sc);
columnsBox->setCurrentItem(w->colPlotDesignation(sc));

QString colLabel = w->colLabel(sc);
colName->setText(colLabel);
colName->selectAll();

comments->setText(w->colComment(sc));
colWidth->setValue(w->columnWidth(sc));  

displayBox->setCurrentItem(colType);
updateDisplay(colType);

if (colType == Table::Numeric)
	{
	int f, prec;
	w->columnNumericFormat(sc, f, prec);
	formatBox->setCurrentItem(f);
	precisionBox->setValue(prec);
	enablePrecision(f);
	}
else if (colType == Table::Time)
	formatBox->setCurrentText(w->columnFormat(sc));
}

void tableDialog::setWorksheet(Table * table)
{
w=table;
w->storeCellsToMemory();
updateColumn(w->selectedColumn());
}

void tableDialog::changeColWidth(int width)
{
w->changeColWidth(width, applyToAllBox->isChecked());
}

void tableDialog::apply()
{
QString name=colName->text();
if (name.contains(QRegExp("\\W")))
	{
	QMessageBox::warning(this,tr("QtiPlot - Error"), 
						tr("The column names must only contain letters and digits!"));
	name.remove(QRegExp("\\W"));
	}
w->changeColName(name.remove("_"));
w->enumerateRightCols(enumerateAllBox->isChecked());
w->changeColWidth(colWidth->value(), applyToAllBox->isChecked());
w->setColComment(comments->text().replace("\n", " ").replace("\t", " "));

int colType = displayBox->currentItem();
int format = formatBox->currentItem();

switch(colType)   
	{
	case 0:
		w->setNumericFormat(format,precisionBox->value(),applyToRightCols->isChecked());
	break;

	case 1:
		w->setTextFormat(applyToRightCols->isChecked()); 
	break;

	case 2:
		 w->setDateTimeFormat(colType, formatBox->currentText(), 
								applyToRightCols->isChecked()); 
	break;

	case 3:
		w->setDateTimeFormat(colType, formatBox->currentText(), 
							applyToRightCols->isChecked()); 
	break;

	case 4:
	if (!format)
		w->setDateTimeFormat(colType, "shortMonthName", applyToRightCols->isChecked()); 
	else
		w->setDateTimeFormat(colType, "longMonthName", applyToRightCols->isChecked());
	break;

	case 5:
	if (!format)
		w->setDateTimeFormat(colType, "shortDayName", applyToRightCols->isChecked()); 
	else
		w->setDateTimeFormat(colType, "longDayName", applyToRightCols->isChecked());
	break;
	}
}

void tableDialog::closeEvent( QCloseEvent* ce )
{
w->freeMemory();
ce->accept();
}

void tableDialog::setPlotDesignation(int i)
{
switch(i)
	{
	case 0:
		w->disregardCol();
	break;

	case 1:
		w->setXCol();
	break;

	case 2:
		w->setYCol();
	break;

	case 3:
		w->setZCol();
	break;
	}
}

void tableDialog::showPrecisionBox(int item)
{
switch(item)   
        {
            case 0:
            {
                precisionBox->hide();
                break;
            }
            case 1:
            {
                precisionBox->show();
                break;
            }
			case 2:
            {
                precisionBox->show();
                break;
            }
		}
}

void tableDialog::updatePrecision(int prec)
{
w->setNumericFormat(formatBox->currentItem(),prec,applyToRightCols->isChecked());
}

void tableDialog::updateFormat()
{
w->setNumericFormat(formatBox->currentItem(),precisionBox->value(),applyToRightCols->isChecked());
}

tableDialog::~tableDialog()
{
}

void tableDialog::updateDisplay(int item)
{
labelFormat->show();
formatBox->show();
formatBox->clear();
formatBox->setEditable ( false );
labelNumeric->hide();
precisionBox->hide();

if (item == 0)
	{
	formatBox->insertItem( tr( "Default" ) );
    formatBox->insertItem( tr( "Decimal: 1000" ) );
    formatBox->insertItem( tr( "Scientific: 1E3" ) );

	labelNumeric->show();
	precisionBox->show();
	}
else
	{
	QDate date=QDate::currentDate();
	switch (item)
		{
		case 1:
			labelFormat->hide();
			formatBox->hide();
		break;

		case 2:
			formatBox->insertItem(tr("yyyy-MM-dd"));
		break;

		case 3:
			{
			formatBox->setEditable ( true );
			
			formatBox->insertItem(tr("h") );
			formatBox->insertItem(tr("h ap") );
			formatBox->insertItem(tr("h AP") );
			formatBox->insertItem(tr("h:mm"));
			formatBox->insertItem(tr("h:mm ap") );
			formatBox->insertItem(tr("hh:mm"));
			formatBox->insertItem(tr("h:mm:ss") );
			formatBox->insertItem(tr("h:mm:ss.zzz") );
			formatBox->insertItem(tr("mm:ss") );
			formatBox->insertItem(tr("mm:ss.zzz") );
			formatBox->insertItem(tr("hmm") );
			formatBox->insertItem(tr("hmmss") );
			formatBox->insertItem(tr("hhmmss") );
			}
		break;

		case 4:
			formatBox->insertItem(QDate::shortMonthName (date.month()));
			formatBox->insertItem(QDate::longMonthName (date.month()));
		break;

		case 5:
			formatBox->insertItem(QDate::shortDayName (date.dayOfWeek ()) );
			formatBox->insertItem(QDate::longDayName (date.dayOfWeek ()));
		break;
		}
	}
}

void tableDialog::languageChange()
{
    setCaption( tr( "QtiPlot - Column options" ) );
    buttonCancel->setText( tr( "&Cancel" ) );  
    buttonOk->setText( tr( "&OK" ) );
    		
    applyToAllBox->setText( tr( "Apply to all" ) );
}
