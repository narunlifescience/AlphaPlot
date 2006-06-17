/***************************************************************************
    File                 : filterDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Filter options dialog
                           
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
#include "filterDialog.h"
#include "graph.h"
#include "parser.h"
#include "colorBox.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <q3hbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

FilterDialog::FilterDialog(int type, QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	filter_type = type;

    if ( !name )
		setName( "FilterDialog" );

	Q3ButtonGroup *GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),this,"GroupBox1" );

	new QLabel( tr("Filter curve: "), GroupBox1, "TextLabel1",0 );
	boxName = new QComboBox(GroupBox1, "boxShow" );
	
	if (type <= HighPass)
		new QLabel( tr("Frequency cutoff (Hz)"), GroupBox1, "TextLabel4",0 );
	else
		new QLabel( tr("Low Frequency (Hz)"), GroupBox1, "TextLabel4",0 );

	boxStart = new QLineEdit(GroupBox1, "boxStart" );
	boxStart->setText(tr("0"));
	
	if (type >= BandPass)
		{
		new QLabel( tr("High Frequency (Hz)"), GroupBox1, "TextLabel5",0 );
		boxEnd = new QLineEdit(GroupBox1, "boxEnd" );
		boxEnd->setText(tr("0"));

		if (type == BandPass)
			new QLabel(tr("Add DC Offset"), GroupBox1, "TextLabel52",0 );
		else
			new QLabel(tr("Substract DC Offset"), GroupBox1, "TextLabel52",0 );

		boxOffset = new QCheckBox(GroupBox1, "boxOffset" );
		}

	new QLabel( tr("Color"), GroupBox1, "TextLabel52",0 );
	boxColor = new ColorBox( false, GroupBox1);
	boxColor->setColor(QColor(Qt::red));

	Q3HBox *hbox1=new Q3HBox (this,"hbox1");	
	hbox1->setMargin(5);
	hbox1->setSpacing(5);

	buttonFilter = new QPushButton(hbox1, "buttonFit" );
    buttonFilter->setDefault( true );
   
    buttonCancel = new QPushButton(hbox1, "buttonCancel" );
	
	Q3VBoxLayout* hlayout = new Q3VBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(hbox1);

    languageChange();
   
    // signals and slots connections
	connect( buttonFilter, SIGNAL( clicked() ), this, SLOT( filter() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

FilterDialog::~FilterDialog()
{
}


void FilterDialog::languageChange()
{
setWindowTitle(tr("QtiPlot - Filter options"));
buttonFilter->setText( tr( "&Filter" ) );
buttonCancel->setText( tr( "&Close" ) );
}

void FilterDialog::filter()
{
double from = 0.0, to = 0.0;
try
	{
	MyParser parser;
	parser.SetExpr(boxStart->text().ascii());
	from=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(this, tr("QtiPlot - Frequency input error"), QString::fromStdString(e.GetMsg()));
	boxStart->setFocus();
	return;
	}		

if (from < 0)
		{
		QMessageBox::critical(this, tr("QtiPlot - Frequency input error"),
				tr("Please enter positive frequency values!"));
		boxStart->setFocus();
		return;
		}

if (filter_type >= BandPass)
	{	
	try
		{
		MyParser parser;	
		parser.SetExpr(boxEnd->text().ascii());
		to=parser.Eval();
		}
	catch(mu::ParserError &e)
		{
		QMessageBox::critical(this, tr("QtiPlot - High Frequency input error"), QString::fromStdString(e.GetMsg()));
		boxEnd->setFocus();
		return;
		}	

	if (to < 0)
		{
		QMessageBox::critical(this, tr("QtiPlot - High Frequency input error"),
				tr("Please enter positive frequency values!"));
		boxEnd->setFocus();
		return;
		}

	if (from>=to)
		{
		QMessageBox::critical(this, tr("QtiPlot - Frequency input error"),
				tr("Please enter frequency limits that satisfy: Low < High !"));
		boxEnd->setFocus();
		return;
		}
	}

long key = graph->curveKey(boxName->currentItem());
if (key < 0)
	return;

if (filter_type >= BandPass)
 graph->filterFFT(key, filter_type, from, to, boxOffset->isChecked(), boxColor->currentItem());
else
 graph->filterFFT(key, filter_type, from, to, false, boxColor->currentItem());

}

void FilterDialog::setGraph(Graph *g)
{
graph = g;
boxName->insertStringList (g->curvesList(),-1);
};


