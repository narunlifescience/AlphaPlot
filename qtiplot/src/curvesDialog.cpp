/***************************************************************************
    File                 : curvesDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Add/remove curves dialog
                           
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
#include "curvesDialog.h"
#include "graph.h"
#include "worksheet.h"
#include "pixmaps.h"

#include <qlabel.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <q3vbox.h>
#include <q3popupmenu.h>
#include <q3accel.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3VBoxLayout>
#include <Q3ListView>

CurvesDialog::CurvesDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
: QDialog( parent, name, modal, fl )
{
	if ( !name )
		setName( "CurvesDialog" );
	setMinimumSize(QSize(400,200));
	setWindowTitle( tr( "QtiPlot - Add/Remove curves" ) );
	setFocus();

	Q3HBox *box5 = new Q3HBox (this, "box5"); 
	box5->setSpacing (5);
	box5->setMargin(5);

	new QLabel("New curves style", box5, "label0");
	boxStyle = new QComboBox (box5, "boxStyle");
	boxStyle->insertItem( QPixmap(lPlot_xpm), tr( " Line" ) );
	boxStyle->insertItem( QPixmap(pPlot_xpm), tr( " Scatter" ) );
	boxStyle->insertItem( QPixmap(lpPlot_xpm), tr( " Line + Symbol" ) );
	boxStyle->insertItem( QPixmap(dropLines_xpm), tr( " Vertical drop lines" ) );
	boxStyle->insertItem( QPixmap(spline_xpm), tr( " Spline" ) );
	boxStyle->insertItem( QPixmap(vert_steps_xpm), tr( " Vertical steps" ) );
	boxStyle->insertItem( QPixmap(hor_steps_xpm), tr( " Horizontal steps" ) );
	boxStyle->insertItem( QPixmap(area_xpm), tr( " Area" ) );
	boxStyle->insertItem( QPixmap(vertBars_xpm), tr( " Vertical Bars" ) );
	boxStyle->insertItem( QPixmap(hBars_xpm), tr( " Horizontal Bars" ) );

	Q3HBox  *box0 = new Q3HBox (this, "box0"); 
	box0->setSpacing (5);

	Q3VBox  *box1=new Q3VBox (box0, "box1"); 
	box1->setMargin(5);
	box1->setSpacing (5);

	TextLabel1 = new QLabel(box1, "TextLabel1" );
	TextLabel1->setText( tr( "Available data" ) );

	available = new Q3ListBox( box1, "available" );
	available->setSelectionMode (Q3ListBox::Multi);

	Q3VBox  *box2=new Q3VBox (box0, "box2"); 
	box2->setMargin(5);
	box2->setSpacing (5);

	btnAdd = new QPushButton(box2, "btnAdd" );
	btnAdd->setPixmap( QPixmap(next_xpm) );
	btnAdd->setFixedWidth (35);
	btnAdd->setFixedHeight (30);

	btnRemove = new QPushButton(box2, "btnRemove" );
	btnRemove->setPixmap( QPixmap(prev_xpm) );
	btnRemove->setFixedWidth (35);
	btnRemove->setFixedHeight(30);

	Q3VBox  *box3=new Q3VBox (box0, "box3"); 
	box3->setMargin(5);
	box3->setSpacing (5);

	TextLabel2 = new QLabel(box3, "TextLabel2" );
	TextLabel2->setText( tr( "Graph contents" ) );

	contents = new Q3ListBox( box3, "contents" );
	contents->setSelectionMode (Q3ListBox::Multi);

	Q3VBox  *box4=new Q3VBox (box0, "box4"); 
	box4->setMargin(5);
	box4->setSpacing (5);

	btnAssociations = new QPushButton(box4, "btnAssociations" );
	btnAssociations->setText( tr( "&Plot Associations..." ) );
	btnAssociations->setEnabled(false);

	btnEditFunction = new QPushButton(box4, "btnEditFunction" );
	btnEditFunction->setText( tr( "&Edit Function..." ) );
	btnEditFunction->setEnabled(false);

	btnOK = new QPushButton(box4, "btnOK" );
	btnOK->setText( tr( "OK" ) );
	btnOK->setDefault( true );

	btnCancel = new QPushButton(box4, "btnCancel" );
	btnCancel->setText( tr( "Close" ) );

	Q3VBoxLayout* layout = new Q3VBoxLayout(this,5,5, "hlayout3");
	layout->addWidget(box5);
	layout->addWidget(box0);

	connect(btnAssociations, SIGNAL(clicked()),this, SLOT(showPlotAssociations()));
	connect(btnEditFunction, SIGNAL(clicked()),this, SLOT(showFunctionDialog()));

	connect(btnAdd, SIGNAL(clicked()),this, SLOT(addCurve()));
	connect(btnRemove, SIGNAL(clicked()),this, SLOT(removeCurve()));
	connect(btnOK, SIGNAL(clicked()),this, SLOT(close()));
	connect(btnCancel, SIGNAL(clicked()),this, SLOT(close()));
	connect(btnAdd, SIGNAL(clicked()),this, SLOT(enableRemoveBtn()));
	connect(btnRemove, SIGNAL(clicked()),this, SLOT(enableRemoveBtn()));

	connect(contents, SIGNAL(highlighted (int)), this, SLOT(showCurveBtn(int)));
	connect(contents, SIGNAL(rightButtonClicked(Q3ListBoxItem *, const QPoint &)), this, SLOT(deletePopupMenu(Q3ListBoxItem *, const QPoint &)));
	connect(available, SIGNAL(rightButtonClicked(Q3ListBoxItem *, const QPoint &)), this, SLOT(addPopupMenu(Q3ListBoxItem *, const QPoint &)));

	Q3Accel *accel = new Q3Accel(this);
	accel->connectItem( accel->insertItem( Qt::Key_Delete ), this, SLOT(removeCurve()) );
}

void CurvesDialog::showCurveBtn(int) 
{
	QString txt= contents->currentText();
	if (txt.contains("="))
	{
		btnAssociations->setEnabled(false);
		btnEditFunction->setEnabled(true);
	}
	else
	{
		btnAssociations->setEnabled(true);
		btnEditFunction->setEnabled(false);
	}
}

void CurvesDialog::showPlotAssociations() 
{
	int curve = contents->currentItem();
	if (curve < 0)
		curve = 0;
	emit showPlotAssociations(curve);
	close();
}

void CurvesDialog::showFunctionDialog() 
{
	emit showFunctionDialog(g, contents->currentItem());
	close();
}

QSize CurvesDialog::sizeHint() const 
{
	return QSize(400, 200 );
}

void CurvesDialog::deletePopupMenu(Q3ListBoxItem *it, const QPoint &point)
{
	selectedCurve=contents->index (it);

	Q3PopupMenu contextMenu(this);
	contextMenu.insertItem(tr("&Delete"), this, SLOT(removeSelectedCurve()));
	contextMenu.exec(point);
}

void CurvesDialog::addPopupMenu(Q3ListBoxItem *it, const QPoint &point)
{
	selectedCurve=available->index (it);

	Q3PopupMenu contextMenu(this);
	contextMenu.insertItem(tr("&Plot"), this, SLOT(addSelectedCurve()));
	contextMenu.exec(point);
}

void CurvesDialog::insertCurvesToDialog(const QStringList& names)
{
	available->clear();
	int i,n=names.count();	
	for (i=0;i<n;i++)
		available->insertItem(names[i],i);

	if (n==0)
		btnAdd->setDisabled(true);
}

void CurvesDialog::setGraph(Graph *graph)
{
	g = graph;
	contents->insertStringList(g->curvesList());
	enableRemoveBtn();
}

void CurvesDialog::addSelectedCurve()
{
	QStringList emptyColumns;
	QString text=available->text(selectedCurve);
	if (!contents->findItem(text, Q3ListView::ExactMatch))
	{
		if (!addCurve(text))
			emptyColumns << text;
		else
			g->updatePlot();
	}
	Graph::showPlotErrorMessage(this, emptyColumns);
}

void CurvesDialog::addCurve()
{
	QStringList emptyColumns;
	for (int i=0;i<int(available->count());i++)
	{
		if (available->isSelected(i))
		{
			QString text=available->text(i);
			if (!contents->findItem(text, Q3ListView::ExactMatch))
			{
				if (!addCurve(text))
					emptyColumns << text;
			}
		}
	}
	g->updatePlot();
	Graph::showPlotErrorMessage(this, emptyColumns);
}

bool CurvesDialog::addCurve(const QString& name)
{
	int style = curveStyle();
	Table* t = findTable(name);
	if (t && g->insertCurve(t, name, style))
	{
		CurveLayout cl = Graph::initCurveLayout();

		int curve = g->curves() - 1;
		long key = g->curveKey(curve);	
		if (key == (long) curve)
			key++;

		int color = key%16;
		if (color == 13) //avoid white invisible curves
			color = 0;

		cl.lCol=color;
		cl.symCol=color;
		cl.fillCol=color;	
		cl.lWidth = defaultCurveLineWidth;
		cl.sSize = defaultSymbolSize;
		cl.sType=key%9;

		if (style == Graph::Line)
			cl.sType = 0;
		else if (style==Graph::VerticalBars || style==Graph::HorizontalBars )
		{
			cl.filledArea=1;
			cl.lCol=0;
			cl.aCol=color;
			cl.sType = 0;
		}
		else if (style==Graph::Area )
		{
			cl.filledArea=1;
			cl.aCol=color;
			cl.sType = 0;
		}
		else if (style == Graph::VerticalDropLines)
			cl.connectType=2;
		else if (style == Graph::VerticalSteps || style == Graph::HorizontalSteps)
		{
			cl.connectType=3; 	                
			cl.sType = 0;
		}
		else if (style == Graph::Spline)
			cl.connectType=5;

		g->updateCurveLayout(curve, &cl);

		contents->insertItem(name,-1);
		return true;
	}
	return false;
}


void CurvesDialog::setCurveDefaultSettings(int style, int width, int size)
{
	defaultCurveLineWidth = width;
	defaultSymbolSize = size;

	if (style == Graph::Line)
		boxStyle->setCurrentItem(0);
	else if (style == Graph::Scatter)
		boxStyle->setCurrentItem(1);
	else if (style == Graph::LineSymbols)
		boxStyle->setCurrentItem(2);
	else if (style == Graph::VerticalDropLines)
		boxStyle->setCurrentItem(3);
	else if (style == Graph::Spline)
		boxStyle->setCurrentItem(4);
	else if (style == Graph::VerticalSteps)
		boxStyle->setCurrentItem(5);
	else if (style == Graph::HorizontalSteps)
		boxStyle->setCurrentItem(6);
	else if (style == Graph::Area)
		boxStyle->setCurrentItem(7);
	else if (style == Graph::VerticalBars)
		boxStyle->setCurrentItem(8);
	else if (style == Graph::HorizontalBars)
		boxStyle->setCurrentItem(9);	
}

Table * CurvesDialog::findTable(const QString& text)
{
	int pos = text.find("_", 0);
	for (int i=0; i < (int)tables->count(); i++ )
	{
		if (tables->at(i)->name() == text.left(pos))
			return (Table *)tables->at(i);
	}
	return 0;
}

void CurvesDialog::removeSelectedCurve()
{
	g->removeCurve(selectedCurve);
	contents->removeItem (selectedCurve);
}

void CurvesDialog::removeCurve()
{
	int i;
	QStringList texts;	
	for (i=0;i<int(contents->count());i++)
	{
		if (contents->isSelected(i))
			texts<<contents->text(i);	
	}

	for (i=0;i<int(texts.count());i++)
	{
		Q3ListBoxItem *it=contents->findItem (texts[i],Q3ListView::ExactMatch);

		int index=contents->index(it);		
		g->removeCurve(index);
		contents->removeItem(index);	
	}	
}

void CurvesDialog::clear()
{
	contents->clear();
	btnRemove->setDisabled (true);
}

void CurvesDialog::enableRemoveBtn()
{
	if (contents->count()>0)
		btnRemove->setEnabled (true);
	else
		btnRemove->setDisabled (true);
}

int CurvesDialog::curveStyle()
{
	int style = 0;
	switch (boxStyle->currentItem())
	{
		case 0:
			style = Graph::Line;
			break;
		case 1:
			style = Graph::Scatter;
			break;
		case 2:
			style = Graph::LineSymbols;
			break;
		case 3:
			style = Graph::VerticalDropLines;
			break;
		case 4:
			style = Graph::Spline;
			break;
		case 5:
			style = Graph::VerticalSteps;
			break;
		case 6:
			style = Graph::HorizontalSteps;
			break;
		case 7:
			style = Graph::Area;
			break;
		case 8:
			style = Graph::VerticalBars;
			break;
		case 9:
			style = Graph::HorizontalBars;
			break;
	}
	return style;
}

CurvesDialog::~CurvesDialog()
{
	delete tables;
}
