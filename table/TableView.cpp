/***************************************************************************
    File                 : TableView.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : View class for table data

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

#include "TableView.h"
#include "TableModel.h"
#include "TableItemDelegate.h"
#include "tablecommands.h"

#include "Column.h"
#include "core/AbstractFilter.h"
#include "core/datatypes/SimpleCopyThroughFilter.h"
#include "core/datatypes/Double2StringFilter.h"
#include "core/datatypes/String2DoubleFilter.h"
#include "core/datatypes/DateTime2StringFilter.h"
#include "core/datatypes/String2DateTimeFilter.h"

#include <QKeyEvent>
#include <QtDebug>
#include <QHeaderView>
#include <QRect>
#include <QSize>
#include <QFontMetrics>
#include <QFont>
#include <QItemSelectionModel>
#include <QItemSelection>

TableView::TableView(QWidget * parent, TableModel * model )
 : QTableView( parent ), d_model(model)
{
	init(model);
}

void TableView::init(TableModel * model)
{
	setModel(model);

	d_delegate = new TableItemDelegate;
	setItemDelegate(d_delegate);
	
	QHeaderView * v_header = verticalHeader();
	QHeaderView * h_header = horizontalHeader();
	// Remark: ResizeToContents works in Qt 4.2.3 but is broken in 4.3.0
	// Should be fixed in 4.3.1 though, see:
	// http://trolltech.com/developer/task-tracker/index_html?method=entry&id=165567
	v_header->setResizeMode(QHeaderView::ResizeToContents);
	v_header->setMovable(false);
	h_header->setDefaultAlignment(Qt::AlignTop);
	h_header->setResizeMode(QHeaderView::Interactive);
	h_header->setMovable(false);

	connect(d_model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(updateHeaderGeometry(Qt::Orientation,int,int)) ); 

    setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(emitContextMenuRequest(const QPoint&)));
}
	
void TableView::updateHeaderGeometry(Qt::Orientation o, int first, int last)
{
	Q_UNUSED(first)
	Q_UNUSED(last)
	if(o != Qt::Horizontal) return;
	horizontalHeader()->setStretchLastSection(true);  // ugly hack (flaw in Qt? Does anyone know a better way?)
	horizontalHeader()->updateGeometry();
	horizontalHeader()->setStretchLastSection(false); // ugly hack part 2
}

TableView::~TableView() 
{
	delete d_delegate;
}

void TableView::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		advanceCell();
	QTableView::keyPressEvent(event);
}

void TableView::advanceCell()
{
	QModelIndex idx = currentIndex();
	// TODO
//    if(idx.row()+1 >= d_model->rowCount())
//        d_model->undoStack()->push(new TableAppendRowsCmd(d_model, 1) );

//	setCurrentIndex(idx.sibling(idx.row()+1, idx.column()));
}

void TableView::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected )
{
	//TODO
	/*
	int left, right, top, bottom, i, col;
	for(i=0; i<selected.size(); i++)
	{
		QItemSelectionRange range = selected.at(i);
		left = range.left();
		right = range.right();
		top = range.top();
		bottom = range.bottom();
		for(col=left; col<=right && col>= 0; col++)
			d_model->columnPointer(col)->setSelected(Interval<int>(top, bottom));
	}
	for(i=0; i<deselected.size(); i++)
	{
		QItemSelectionRange range = deselected.at(i);
		left = range.left();
		right = range.right();
		top = range.top();
		bottom = range.bottom();
		for(col=left; col<=right && col>= 0; col++)
			d_model->columnPointer(col)->setSelected(Interval<int>(top, bottom), false);
	}
	*/
	QTableView::selectionChanged(selected, deselected);
}

void TableView::emitContextMenuRequest(const QPoint& pos)
{
	emit requestContextMenu(this, pos);
}

