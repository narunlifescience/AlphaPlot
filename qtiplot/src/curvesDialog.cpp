/***************************************************************************
    File                 : curvesDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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
#include "FunctionCurve.h"
#include "pixmaps.h"

#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLayout>
#include <QListWidget>
#include <QGroupBox>
#include <QPixmap>
#include <QShortcut>
#include <QKeySequence>
#include <QMenu>

CurvesDialog::CurvesDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
: QDialog( parent, name, modal, fl )
{
	if ( !name )
		setName( "CurvesDialog" );

	setWindowTitle( tr( "QtiPlot - Add/Remove curves" ) );
    setSizeGripEnabled(true);
	setFocus();

    QHBoxLayout* hl1 = new QHBoxLayout();
	hl1->addWidget(new QLabel(tr("New curves style")));
	boxStyle = new QComboBox ();
	boxStyle->addItem( QPixmap(lPlot_xpm), tr( " Line" ) );
	boxStyle->addItem( QPixmap(pPlot_xpm), tr( " Scatter" ) );
	boxStyle->addItem( QPixmap(lpPlot_xpm), tr( " Line + Symbol" ) );
	boxStyle->addItem( QPixmap(dropLines_xpm), tr( " Vertical drop lines" ) );
	boxStyle->addItem( QPixmap(spline_xpm), tr( " Spline" ) );
	boxStyle->addItem( QPixmap(vert_steps_xpm), tr( " Vertical steps" ) );
	boxStyle->addItem( QPixmap(hor_steps_xpm), tr( " Horizontal steps" ) );
	boxStyle->addItem( QPixmap(area_xpm), tr( " Area" ) );
	boxStyle->addItem( QPixmap(vertBars_xpm), tr( " Vertical Bars" ) );
	boxStyle->addItem( QPixmap(hBars_xpm), tr( " Horizontal Bars" ) );
    hl1->addWidget(boxStyle);
    hl1->addStretch();

    QGridLayout *gl = new QGridLayout(this);
    gl->addLayout(hl1, 0, 0);

    gl->addWidget(new QLabel( tr( "Available data" )), 1, 0);
    gl->addWidget(new QLabel( tr( "Graph contents" )), 1, 2);

	available = new QListWidget();
	available->setSelectionMode (QAbstractItemView::ExtendedSelection);
    gl->addWidget(available, 2, 0);

    QVBoxLayout* vl1 = new QVBoxLayout();
	btnAdd = new QPushButton();
	btnAdd->setPixmap( QPixmap(next_xpm) );
	btnAdd->setFixedWidth (35);
	btnAdd->setFixedHeight (30);
    vl1->addWidget(btnAdd);

	btnRemove = new QPushButton();
	btnRemove->setPixmap( QPixmap(prev_xpm) );
	btnRemove->setFixedWidth (35);
	btnRemove->setFixedHeight(30);
    vl1->addWidget(btnRemove);
    vl1->addStretch();

    gl->addLayout(vl1, 2, 1);
	contents = new QListWidget();
	contents->setSelectionMode (QAbstractItemView::ExtendedSelection);
    gl->addWidget(contents, 2, 2);

    QVBoxLayout* vl2 = new QVBoxLayout();
	btnAssociations = new QPushButton(tr( "&Plot Associations..." ));
	btnAssociations->setEnabled(false);
    vl2->addWidget(btnAssociations);

	btnEditFunction = new QPushButton(tr( "&Edit Function..." ));
	btnEditFunction->setEnabled(false);
    vl2->addWidget(btnEditFunction);

	btnOK = new QPushButton(tr( "OK" ));
	btnOK->setDefault( true );
    vl2->addWidget(btnOK);

	btnCancel = new QPushButton(tr( "Close" ));
    vl2->addWidget(btnCancel);
    vl2->addStretch();
    gl->addLayout(vl2, 2, 3);

	connect(btnAssociations, SIGNAL(clicked()),this, SLOT(showPlotAssociations()));
	connect(btnEditFunction, SIGNAL(clicked()),this, SLOT(showFunctionDialog()));
	connect(btnAdd, SIGNAL(clicked()),this, SLOT(addCurves()));
	connect(btnRemove, SIGNAL(clicked()),this, SLOT(removeCurves()));
	connect(btnOK, SIGNAL(clicked()),this, SLOT(close()));
	connect(btnCancel, SIGNAL(clicked()),this, SLOT(close()));
	connect(contents, SIGNAL(currentRowChanged(int)), this, SLOT(showCurveBtn(int)));
    connect(contents, SIGNAL(itemSelectionChanged()), this, SLOT(enableRemoveBtn()));
    connect(available, SIGNAL(itemSelectionChanged()), this, SLOT(enableAddBtn()));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(removeCurves()));
    shortcut = new QShortcut(QKeySequence("-"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(removeCurves()));
    shortcut = new QShortcut(QKeySequence("+"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(addCurves()));
}

void CurvesDialog::showCurveBtn(int) 
{
	QwtPlotCurve *c = g->curve(contents->currentRow());
	if (!c)
		return;

	if (c->rtti() == FunctionCurve::RTTI)
	{
		btnAssociations->setEnabled(false);
		btnEditFunction->setEnabled(true);
	}
	else if (c->rtti() == QwtPlotItem::Rtti_PlotCurve)
	{
		btnAssociations->setEnabled(true);
		btnEditFunction->setEnabled(false);
	}
	else if (c->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
  	{
  		btnAssociations->setEnabled(false);
  	    btnEditFunction->setEnabled(false);
  	}
}

void CurvesDialog::showPlotAssociations() 
{
	int curve = contents->currentRow ();
	if (curve < 0)
		curve = 0;
	emit showPlotAssociations(curve);
	close();
}

void CurvesDialog::showFunctionDialog() 
{
	emit showFunctionDialog(g, contents->currentRow());
	close();
}

QSize CurvesDialog::sizeHint() const 
{
	return QSize(600, 300);
}

void CurvesDialog::contextMenuEvent(QContextMenuEvent *e)
{
	QPoint pos = available->viewport()->mapFromGlobal(QCursor::pos());
	QRect rect = available->visualItemRect(available->currentItem());
	if (rect.contains(pos))
	{
	   QMenu contextMenu(this);
       QList<QListWidgetItem *> lst = available->selectedItems();
       if (lst.size() > 1)
	       contextMenu.insertItem(tr("&Plot Selection"), this, SLOT(addCurves()));
       else if (lst.size() == 1)
	       contextMenu.insertItem(tr("&Plot"), this, SLOT(addCurves()));
	   contextMenu.exec(QCursor::pos());
    }

	pos = contents->viewport()->mapFromGlobal(QCursor::pos());
	rect = contents->visualItemRect(contents->currentItem());
	if (rect.contains(pos))
	{
	   QMenu contextMenu(this);
       QList<QListWidgetItem *> lst = contents->selectedItems();
       if (lst.size() > 1)
	       contextMenu.insertItem(tr("&Delete Selection"), this, SLOT(removeCurves()));
       else if (lst.size() == 1)
	       contextMenu.insertItem(tr("&Delete Curve"), this, SLOT(removeCurves()));
	   contextMenu.exec(QCursor::pos());
    }

    e->accept();
}

void CurvesDialog::insertCurvesToDialog(const QStringList& names)
{
	available->clear();
	available->addItems (names);
	if (!names.count())
		btnAdd->setDisabled(true);
}

void CurvesDialog::setGraph(Graph *graph)
{
	g = graph;
	contents->addItems(g->plotItemsList());
	enableRemoveBtn();
    enableAddBtn();
}

void CurvesDialog::addCurves()
{
	QStringList emptyColumns;
    QList<QListWidgetItem *> lst = available->selectedItems();
    for (int i = 0; i < lst.size(); ++i)
    {
        QString text = lst.at(i)->text();
        if (contents->findItems(text, Qt::MatchExactly ).isEmpty ())
			{
			if (!addCurve(text))
				emptyColumns << text;
			}
    }
	g->updatePlot();
	Graph::showPlotErrorMessage(this, emptyColumns);
    enableRemoveBtn();
}

bool CurvesDialog::addCurve(const QString& name)
{
	int style = curveStyle();
	Table* t = findTable(name);
	if (t && g->insertCurve(t, name, style))
	{
		CurveLayout cl = Graph::initCurveLayout();
		int color, symbol;
		g->guessUniqueCurveLayout(color, symbol);

		cl.lCol = color;
		cl.symCol = color;
		cl.fillCol = color;	
		cl.lWidth = defaultCurveLineWidth;
		cl.sSize = defaultSymbolSize;
		cl.sType = symbol;

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

		g->updateCurveLayout(g->curves() - 1, &cl);

		contents->addItem(name);
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

void CurvesDialog::removeCurves()
{
QList<QListWidgetItem *> lst = contents->selectedItems();
for (int i = 0; i < lst.size(); ++i)
     {
     QListWidgetItem *it = lst.at(i);
     int index = contents->row(it);
	 g->removeCurve(index);
     contents->takeItem(index);
     delete it;
     }
enableRemoveBtn();
}

void CurvesDialog::enableAddBtn()
{
    btnAdd->setEnabled (available->count()>0 && !available->selectedItems().isEmpty());
}

void CurvesDialog::enableRemoveBtn()
{
    btnRemove->setEnabled (contents->count()>0 && !contents->selectedItems().isEmpty());
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
