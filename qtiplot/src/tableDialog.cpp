/***************************************************************************
    File                 : tableDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Column options dialog
                           
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
#include "tableDialog.h"
#include "worksheet.h"

#include <QMessageBox>
#include <QLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QRegExp>
#include <QDate>

TableDialog::TableDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "TableDialog" );
    setWindowTitle( tr( "QtiPlot - Column options" ) );
    setSizeGripEnabled(true);
	
	QHBoxLayout *hboxa = new QHBoxLayout();
	hboxa->addWidget(new QLabel(tr( "Column Name:" )));
    colName = new QLineEdit();
    hboxa->addWidget(colName);

	enumerateAllBox = new QCheckBox();
	enumerateAllBox->setText(tr("Enumerate all to the right" ));

	buttonPrev = new QPushButton();
	buttonPrev->setText("&<<");
	buttonPrev->setMaximumWidth(40);

	buttonNext = new QPushButton();
	buttonNext->setText("&>>");
	buttonNext->setMaximumWidth(40);
	
	QHBoxLayout *hboxb = new QHBoxLayout();
    hboxb->addWidget(buttonPrev);
    hboxb->addWidget(buttonNext);
    hboxb->addStretch();
    
    QVBoxLayout *vbox1 = new QVBoxLayout();
    vbox1->addLayout(hboxa);
    vbox1->addWidget(enumerateAllBox);
    vbox1->addLayout(hboxb);

	buttonOk = new QPushButton(tr( "&OK" ));
    buttonOk->setDefault(true);

	buttonApply = new QPushButton();
	buttonApply->setText(tr("&Apply"));

	buttonCancel = new QPushButton(tr( "&Cancel" ));
	
	QVBoxLayout  *vbox2 = new QVBoxLayout();
	vbox2->setSpacing(5);
	vbox2->setMargin(5);
	vbox2->addWidget(buttonOk);
	vbox2->addWidget(buttonApply);
	vbox2->addWidget(buttonCancel);

    QHBoxLayout  *hbox1 = new QHBoxLayout();
	hbox1->setSpacing(5);
	hbox1->addLayout(vbox1);
	hbox1->addLayout(vbox2);
	
	QGridLayout *gl1 = new QGridLayout();
    gl1->addWidget(new QLabel( tr("Plot Designation:")), 0, 0);
    
   	columnsBox = new QComboBox();
	columnsBox->insertItem(tr("None"));
	columnsBox->insertItem(tr("X (abscissae)"));
	columnsBox->insertItem(tr("Y (ordinates)"));
	columnsBox->insertItem(tr("Z (height)"));
	columnsBox->insertItem(tr("X Error"));
	columnsBox->insertItem(tr("Y Error"));
    gl1->addWidget(columnsBox, 0, 1);
    
    gl1->addWidget(new QLabel(tr("Display")), 1, 0);
    
   	displayBox = new QComboBox();
	displayBox->insertItem(tr("Numeric"));
	displayBox->insertItem(tr("Text"));
	displayBox->insertItem(tr("Date"));
	displayBox->insertItem(tr("Time"));
	displayBox->insertItem(tr("Month"));
	displayBox->insertItem(tr("Day of Week"));
    gl1->addWidget(displayBox, 1, 1);
    
    labelFormat = new QLabel(tr( "Format:" ));
 	gl1->addWidget(labelFormat, 2, 0);
	 
    formatBox = new QComboBox(false);
    gl1->addWidget(formatBox, 2, 1);
    
	labelNumeric = new QLabel(tr( "Precision:" ));
	gl1->addWidget(labelNumeric, 3, 0);

    precisionBox = new QSpinBox();
    gl1->addWidget(precisionBox, 3, 1);

	applyToRightCols = new QCheckBox(tr( "Apply to all columns to the right" ));
		
    QVBoxLayout *vbox3 = new QVBoxLayout();
    vbox3->addLayout(gl1);
    vbox3->addWidget(applyToRightCols);
    
    QGroupBox *gb = new QGroupBox(tr("Options"));
    gb->setLayout(vbox3);

    QHBoxLayout  *hbox2 = new QHBoxLayout();
    hbox2->addWidget(new QLabel(tr( "Column Width:" )));
    
	colWidth = new QSpinBox();
    colWidth->setRange(0, 1000);
	colWidth->setSingleStep(10);

	hbox2->addWidget(colWidth);
	
    applyToAllBox = new QCheckBox(tr( "Apply to all" ));
	hbox2->addWidget(applyToAllBox);
	
	comments = new QTextEdit();

	QVBoxLayout* vbox4 = new QVBoxLayout();
    vbox4->addLayout(hbox1);
	vbox4->addWidget(gb);
	vbox4->addLayout(hbox2);
	vbox4->addWidget(new QLabel(tr( "Comment:" )));
	vbox4->addWidget(comments);
 
    setLayout(vbox4);
    setFocusProxy (colName);
    resize(minimumSize());

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

void TableDialog::enablePrecision(int f)
{
if(displayBox->currentItem())
	return;//the col type != "Numeric"

if (!f)
	{
	precisionBox->setValue(6);
	precisionBox->setEnabled(false);
	}
else
	precisionBox->setEnabled(true);
}

void TableDialog::accept()
{
apply();
close();
}

void TableDialog::prevColumn()
{
int sc = w->selectedColumn();
apply();
updateColumn(--sc);
}

void TableDialog::nextColumn()
{
int sc = w->selectedColumn();
apply();
updateColumn(++sc);
}

void TableDialog::updateColumn(int sc)
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

void TableDialog::setWorksheet(Table * table)
{
w=table;
updateColumn(w->selectedColumn());
}

void TableDialog::changeColWidth(int width)
{
w->changeColWidth(width, applyToAllBox->isChecked());
}

void TableDialog::apply()
{
if (colName->text().contains("_")){
	QMessageBox::warning(this, tr("QtiPlot - Warning"),
  	tr("For internal consistency reasons the underscore character is replaced with a minus sign."));}
  	 
QString name=colName->text().replace("-", "_");
if (name.contains(QRegExp("\\W")))
	{
	QMessageBox::warning(this,tr("QtiPlot - Error"), 
						tr("The column names must only contain letters and digits!"));
	name.remove(QRegExp("\\W"));
	}
w->changeColName(name.replace("_", "-"));
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
		 w->setDateTimeFormat(colType, "yyyy-MM-dd", applyToRightCols->isChecked()); 
	break;

	case 3:
		w->setDateTimeFormat(colType, formatBox->currentText(), applyToRightCols->isChecked()); 
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

void TableDialog::closeEvent( QCloseEvent* ce )
{
ce->accept();
}

void TableDialog::setPlotDesignation(int i)
{
switch(i)
	{
	case 0:
		w->setPlotDesignation(Table::None);
	break;

	case 1:
		w->setPlotDesignation(Table::X);
	break;

	case 2:
		w->setPlotDesignation(Table::Y);
	break;

	case 3:
		w->setPlotDesignation(Table::Z);
	break;

	case 4:
		w->setPlotDesignation(Table::xErr);
	break;

	case 5:
		w->setPlotDesignation(Table::yErr);
	break;
	}
}

void TableDialog::showPrecisionBox(int item)
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

void TableDialog::updatePrecision(int prec)
{
w->setNumericFormat(formatBox->currentItem(),prec,applyToRightCols->isChecked());
}

void TableDialog::updateFormat()
{
w->setNumericFormat(formatBox->currentItem(),precisionBox->value(),applyToRightCols->isChecked());
}

TableDialog::~TableDialog()
{
}

void TableDialog::updateDisplay(int item)
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
