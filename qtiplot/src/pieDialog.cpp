/***************************************************************************
    File                 : pieDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Pie plot dialog
                           
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
#include "pieDialog.h"
#include "textDialog.h"
#include "colorBox.h"
#include "colorButton.h"
#include "patternBox.h"
#include "graph.h"
#include "multilayer.h"
#include "plot.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <q3buttongroup.h>
#include <qcolordialog.h>
#include <qwidget.h>
#include <q3popupmenu.h>

#include <qwt_plot.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <Q3PtrList>


PieDialog::PieDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "PieDialog" );
	setFixedWidth(521);
	setFixedHeight(260);
    setWindowTitle( tr( "QtiPlot - Pie Options" ) );
    setSizeGripEnabled( false );

    generalDialog = new QTabWidget( this, "generalDialog" );
	
	initPiePage();
	initBorderPage();
	
	Q3ButtonGroup *GroupBox1 = new Q3ButtonGroup(4,Qt::Horizontal,tr(""),this, "GroupBox1" );
	GroupBox1->setFlat (true);
	
	buttonWrk = new QPushButton( GroupBox1, "buttonWrk" );
    buttonWrk->setText( tr( "&Worksheet" ) );
	
	buttonApply = new QPushButton( GroupBox1, "buttonApply" );
    buttonApply->setText( tr( "&Apply" ) );
	
    buttonOk = new QPushButton(GroupBox1, "buttonOk" );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setDefault( true );
	
    buttonCancel = new QPushButton(GroupBox1, "buttonCancel" );
    buttonCancel->setText( tr( "&Cancel" ) );
	
	Q3VBoxLayout* vl = new Q3VBoxLayout(this,5,5, "vl");
	vl->addWidget(generalDialog);
    vl->addWidget(GroupBox1);
   
    // signals and slots connections
	connect( buttonWrk, SIGNAL(clicked()), this, SLOT(showWorksheet()));
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT(updatePlot() ) );
}

void PieDialog::initPiePage()
{
	pieOptions = new QWidget( generalDialog, "pieOptions" );
	curvesList = new Q3ListBox(pieOptions, "listBox" );
	
	Q3ButtonGroup *GroupBox3 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "Border" ),pieOptions, "GroupBox3" );

    new QLabel( tr( "Color" ), GroupBox3, "TextLabel4",0 );

    boxLineColor = new ColorBox( false, GroupBox3);
	
	new QLabel(tr( "Style" ), GroupBox3, "TextLabel31",0 );  
    boxLineStyle = new QComboBox( false, GroupBox3, "boxLineStyle" );
    boxLineStyle->insertItem("_____");
	boxLineStyle->insertItem("- - -");
	boxLineStyle->insertItem(".....");
	boxLineStyle->insertItem("_._._");
	boxLineStyle->insertItem("_.._..");
	
	new QLabel(tr( "Width" ), GroupBox3, "TextLabel3",0 );  
    boxLineWidth = new QSpinBox( GroupBox3, "boxLineWidth" );
	
	Q3ButtonGroup *GroupBox2 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "Fill" ),pieOptions, "GroupBox2" );

    new QLabel( tr( "First color" ), GroupBox2, "TextLabel4",0 ); 
    boxFirstColor = new ColorBox( false, GroupBox2);
	
	new QLabel( tr( "Pattern" ), GroupBox2, "TextLabel41",0 );
	boxPattern = new PatternBox( false, GroupBox2);
	
	new QLabel(tr( "Pie radius" ), GroupBox2, "radiusLabel",0 );  
    boxRadius= new QSpinBox(0,2000,10,GroupBox2, "boxRadius");
	
	Q3HBoxLayout* hlayout1 = new Q3HBoxLayout(pieOptions,5,5, "hlayout1");
	hlayout1->addWidget(curvesList);
    hlayout1->addWidget(GroupBox3);
	hlayout1->addWidget(GroupBox2);
	
	generalDialog->insertTab(pieOptions, tr( "Pie" ) );

connect(curvesList, SIGNAL(rightButtonClicked(Q3ListBoxItem *, const QPoint &)), this, SLOT(showPopupMenu(Q3ListBoxItem *, const QPoint &)));
}

void PieDialog::initBorderPage()
{
frame = new QWidget( generalDialog, "frame" );	

Q3ButtonGroup *GroupBox1 = new Q3ButtonGroup(2,Qt::Horizontal, tr("Background"), frame, "GroupBox5" );
	
new QLabel(tr("Color"), GroupBox1, "TextLabel1_53",0 );
boxBackgroundColor= new ColorButton(GroupBox1);
	
new QLabel(tr("Border Width"),GroupBox1, "TextLabel1_54",0 );
boxBorderWidth= new QSpinBox(GroupBox1);

new QLabel(tr("Border Color" ),GroupBox1, "TextLabel1_53",0 );
boxBorderColor= new ColorButton(GroupBox1);

Q3ButtonGroup *GroupBox2 = new Q3ButtonGroup(2,Qt::Horizontal,tr("Options"), frame, "GroupBox2" );
new QLabel(tr( "Margin" ),GroupBox2, "TextLabel1_541",0 );
boxMargin= new QSpinBox(0, 1000, 5, GroupBox2);

boxAll = new QCheckBox(tr("Apply to all layers"), GroupBox2, "boxShowAxis" );

Q3HBoxLayout* hlayout = new Q3HBoxLayout(frame, 5, 5, "hlayout");
hlayout->addWidget(GroupBox1);
hlayout->addWidget(GroupBox2);

generalDialog->insertTab(frame, tr( "General" ) );
connect(boxMargin, SIGNAL(valueChanged (int)), this, SLOT(changeMargin(int)));
connect(boxBorderColor, SIGNAL(clicked()), this, SLOT(pickBorderColor()));
connect(boxBackgroundColor, SIGNAL(clicked()), this, SLOT(pickBackgroundColor()));
connect(boxBorderWidth,SIGNAL(valueChanged (int)), this, SLOT(updateBorder(int)));
}

void PieDialog::setMultiLayerPlot(MultiLayer *m)
{
mPlot = m;
Graph* g = (Graph*)mPlot->activeGraph();
Plot *p = g->plotWidget();
	
boxMargin->setValue (p->margin());
boxBorderWidth->setValue(p->lineWidth());
boxBorderColor->setColor(p->frameColor());
boxBackgroundColor->setColor(p->paletteBackgroundColor());
}

void PieDialog::pickBackgroundColor()
{
QColor c = QColorDialog::getColor(boxBackgroundColor->color(), this);
if ( !c.isValid() || c == boxBackgroundColor->color() )
	return;

boxBackgroundColor->setColor ( c ) ;

if (boxAll->isChecked())
	{
	QWidgetList allPlots = mPlot->graphPtrs();
	for (int i=0; i<allPlots.count();i++)
		{
		Graph* g=(Graph*)allPlots.at(i);
		if (g)
			g->setBackgroundColor(c);
		}
	}
else
	{
	Graph* g = (Graph*)mPlot->activeGraph();
	if (g)
		g->setBackgroundColor(c);
	}

if (c == QColor(Qt::white) && mPlot->hasOverlapingLayers())
	mPlot->updateTransparency();
}

void PieDialog::pickBorderColor()
{
QColor c = QColorDialog::getColor(boxBorderColor->color(), this);
if ( !c.isValid() || c == boxBorderColor->color() )
	return;

boxBorderColor->setColor ( c ) ;

if (boxAll->isChecked())
	{
	QWidgetList allPlots = mPlot->graphPtrs();
	for (int i=0; i<allPlots.count();i++)
		{
		Graph* g=(Graph*)allPlots.at(i);
		if (g)
			g->drawBorder(boxBorderWidth->value(), c);
		}
	}
else
	{
	Graph* g = (Graph*)mPlot->activeGraph();
	if (g)
		g->drawBorder(boxBorderWidth->value(), c);
	}
}

void PieDialog::updateBorder(int width)
{
if (generalDialog->currentPage() != frame)
	return;

if (boxAll->isChecked())
	{
	QWidgetList allPlots = mPlot->graphPtrs();
	for (int i=0; i<allPlots.count();i++)
		{
		Graph* g=(Graph*)allPlots.at(i);
		if (g)
			g->drawBorder(width, boxBorderColor->color());
		}
	}
else
	{
	Graph* g = (Graph*)mPlot->activeGraph();
	if (g)
		g->drawBorder(width, boxBorderColor->color());
	}
}

void PieDialog::changeMargin(int width)
{
if (generalDialog->currentPage() != frame)
	return;

if (boxAll->isChecked())
	{
	QWidgetList allPlots = mPlot->graphPtrs();
	for (int i=0; i<allPlots.count();i++)
		{
		Graph* g=(Graph*)allPlots.at(i);
		if (g)
			g->changeMargin(width);
		}
	}
else
	{
	Graph* g = (Graph*)mPlot->activeGraph();
	if (g)
		g->changeMargin(width);
	}
}

void PieDialog::showWorksheet()
{
emit worksheet(curvesList->currentText ());
close();
}

void PieDialog::showPopupMenu(Q3ListBoxItem *, const QPoint &point)
{
Q3PopupMenu contextMenu(this);
contextMenu.insertItem("&Delete", this, SLOT(removeCurve()));
contextMenu.exec(point);
}

void PieDialog::removeCurve()
{
emit toggleCurve();
curvesList->removeItem (0);
}

void PieDialog::setPieSize(int size)
{
boxRadius->setValue(size);
}

void PieDialog::setFramed(bool ok)
{
boxFramed->setChecked(ok);
}

void PieDialog::drawFrame(bool framed)
{
boxFrameWidth->setEnabled(framed);
boxFrameColor->setEnabled(framed);
	
emit drawFrame(framed,boxFrameWidth->text().toInt(),boxFrameColor->color());
}

void PieDialog::setFrameWidth(int w)
{
boxFrameWidth->setValue(w);
}

void PieDialog::setFrameColor(const QColor& c)
{
  boxFrameColor->setColor(c);
}

void PieDialog::insertCurveName(const QString& name)
{
curvesList->clear();
curvesList->insertItem(name,-1);
curvesList->setCurrentItem (0);
}

void PieDialog::accept()
{
updatePlot();
close();
}

void PieDialog::updatePlot()
{
if (generalDialog->currentPage()==(QWidget *)pieOptions)
	{
	QPen pen=QPen(boxLineColor->color(),boxLineWidth->value(), style());
	emit updatePie(pen, pattern(), boxRadius->value(), boxFirstColor->currentItem());	
	}
	
if (generalDialog->currentPage()==(QWidget*)frame)
	{
	if (!boxAll->isChecked())
		return;
	
	QColor c = boxBackgroundColor->color();
	QWidgetList allPlots = mPlot->graphPtrs();
	for (int i=0; i<allPlots.count();i++)
		{
		Graph* g=(Graph*)allPlots.at(i);
		if (g)
			{
			g->drawBorder(boxBorderWidth->value(), boxBorderColor->color());
			g->changeMargin(boxMargin->value());
			g->setBackgroundColor(c);
			}
		}
	if (c == QColor(Qt::white) && mPlot->hasOverlapingLayers())
		mPlot->updateTransparency();
	}
}

void PieDialog::setBorderWidth(int width)
{
boxLineWidth->setValue(width);
}

void PieDialog::setFirstColor(int c)
{
boxFirstColor->setCurrentItem(c);	
}

void PieDialog::setBorderColor(const QColor& c)
{
  boxLineColor->setColor(c);
}

Qt::PenStyle PieDialog::style()
{
Qt::PenStyle style;
switch (boxLineStyle->currentItem())
	{
	case 0:
		style=Qt::SolidLine;
	break;
	case 1:
		style=Qt::DashLine;
	break;
	case 2:
		style=Qt::DotLine;
	break;
	case 3:
		style=Qt::DashDotLine;
	break;
	case 4:
		style=Qt::DashDotDotLine;
	break;
	}
return style;
}

void PieDialog::setBorderStyle(const Qt::PenStyle& style)
{
if(style == Qt::SolidLine)
	boxLineStyle->setCurrentItem(0);
if(style == Qt::DashLine)
	boxLineStyle->setCurrentItem(1);
if(style == Qt::DotLine)
	boxLineStyle->setCurrentItem(2);
if(style == Qt::DashDotLine)
	boxLineStyle->setCurrentItem(3);
if(style == Qt::DashDotDotLine)
	boxLineStyle->setCurrentItem(4);
}

void PieDialog::setPattern(const Qt::BrushStyle& style)
{
  boxPattern->setPattern(style);
}


Qt::BrushStyle PieDialog::pattern()
{
  return boxPattern->getSelectedPattern();
}

void PieDialog::showGeneralPage()
{
generalDialog->showPage (frame);
}

PieDialog::~PieDialog()
{
}
