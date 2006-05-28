/***************************************************************************
    File                 : smoothCurveDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Smoothing options dialog
                           
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
#include "smoothCurveDialog.h"
#include "graph.h"
#include "parser.h"
#include "colorBox.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>

smoothCurveDialog::smoothCurveDialog(int method, QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	smooth_type = method;

    if ( !name )
		setName( "smoothCurveDialog" );
	setWindowTitle(tr("QtiPlot - Smoothing Options"));
	
	Q3ButtonGroup *GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),this,"GroupBox1" );

	new QLabel( tr("Curve"), GroupBox1, "TextLabel1",0 );
	boxName = new QComboBox(GroupBox1, "boxShow" );
	
	if (method == SavitzkyGolay)
		{
		new QLabel( tr("Polynomial Order"), GroupBox1, "TextLabel2",0 );
		boxOrder = new QSpinBox(0,9,1,GroupBox1, "boxOrder" );
		boxOrder->setValue(2);

		new QLabel( tr("Points to the Left"), GroupBox1, "TextLabel3",0 );
		boxPointsLeft = new QSpinBox(1,25,1,GroupBox1, "boxPointsLeft" );
		boxPointsLeft->setValue(2);

		new QLabel( tr("Points to the Right"), GroupBox1, "TextLabel3",0 );
		boxPointsRight = new QSpinBox(1,25,1,GroupBox1, "boxPointsRight" );
		boxPointsRight->setValue(2);
		}
	else 
		{
		new QLabel( tr("Points"), GroupBox1, "TextLabel3",0 );
		boxPointsLeft = new QSpinBox(1,1000000,10,GroupBox1, "boxPointsLeft" );
		boxPointsLeft->setValue(5);
		}

	new QLabel( tr("Color"), GroupBox1, "TextLabel52",0 );
	boxColor = new ColorBox( FALSE, GroupBox1);
	boxColor->setColor(QColor(Qt::red));

	Q3ButtonGroup *GroupBox2 = new Q3ButtonGroup(1,Qt::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	
	btnSmooth = new QPushButton(GroupBox2, "btnSmooth" );
    btnSmooth->setAutoDefault( TRUE );
    btnSmooth->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
	Q3HBoxLayout* hlayout = new Q3HBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox2);

    languageChange();
   
    // signals and slots connections
	connect( btnSmooth, SIGNAL( clicked() ), this, SLOT( smooth() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( boxName, SIGNAL( activated(int) ), this, SLOT( activateCurve(int) ) );
}

smoothCurveDialog::~smoothCurveDialog()
{
}


void smoothCurveDialog::languageChange()
{
btnSmooth->setText( tr( "&Smooth" ) );
buttonCancel->setText( tr( "&Close" ) );
}

void smoothCurveDialog::smooth()
{
long key = graph->curveKey(boxName->currentItem());
if (key < 0)
	return;

if (smooth_type == SavitzkyGolay)
	graph->smoothSavGol(key, boxOrder->value(), boxPointsLeft->value(),
				   boxPointsRight->value(), boxColor->currentItem());
else if (smooth_type == FFT)
	graph->smoothFFT(key, boxPointsLeft->value(), boxColor->currentItem());
else if (smooth_type == Average)
	graph->smoothAverage(key, boxPointsLeft->value(), boxColor->currentItem());
}

void smoothCurveDialog::setGraph(Graph *g)
{
graph = g;
boxName->insertStringList (g->curvesList(),-1);
activateCurve(0);
}

void smoothCurveDialog::activateCurve(int index)
{
if (smooth_type == Average)
	{
	QwtPlotCurve *c = graph->curve(index);
	if (!c)
		return;

	boxPointsLeft->setMaxValue(c->dataSize()/2);
	}
}


