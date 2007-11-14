/***************************************************************************
    File                 : Table.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Table worksheet class

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
#include "Table.h"

#include <QItemSelectionModel>
#include <QTime>
#include <QtGlobal>
#include <QHBoxLayout>
#include <QShortcut>
#include <QApplication>
#include <QContextMenuEvent>
#include <climits> // for RAND_MAX
#include <QMenu>
#include <QItemSelection>
#include <QModelIndex>
#include <QModelIndexList>
#include <QInputDialog>

#include "AbstractScript.h"
#include "AspectPrivate.h"
#include "TableModel.h"
#include "TableView.h"
#include "tablecommands.h"
#include "table/SortDialog.h"
#include "Column.h"
#include "core/AbstractFilter.h"
#include "core/datatypes/String2DoubleFilter.h"
#include "core/datatypes/Double2StringFilter.h"
#include "core/datatypes/DateTime2StringFilter.h"
#include "core/datatypes/String2DayOfWeekFilter.h"
#include "core/datatypes/String2MonthFilter.h"
#include "core/datatypes/Double2DateTimeFilter.h"
#include "core/datatypes/Double2MonthFilter.h"
#include "core/datatypes/Double2DayOfWeekFilter.h"
#include "core/datatypes/String2DateTimeFilter.h"
#include "core/datatypes/DateTime2DoubleFilter.h"
#include "core/datatypes/SimpleCopyThroughFilter.h"

#ifndef _NO_TR1_
#include "tr1/memory" 
using std::tr1::dynamic_pointer_cast;
#else // if your compiler does not have TR1 support, you can use boost instead:
#include <boost/shared_ptr.hpp>
using boost::dynamic_pointer_cast;
#endif

#define OBSOLETE qDebug("obsolete Table function called");

#define WAIT_CURSOR QApplication::setOverrideCursor(QCursor(Qt::WaitCursor))
#define RESET_CURSOR QApplication::restoreOverrideCursor()

Table::Table(AbstractScriptingEngine *engine, int rows, int columns, const QString& name)
: AbstractAspect(name), scripted(engine)
{
	d_model = new TableModel(this);
	connect(d_model, SIGNAL(requestResize(int)),
			this, SLOT(handleModelResizeRequest(int)));

	connect(d_model, SIGNAL(columnsAboutToBeInserted(int, QList< shared_ptr<Column> >)),
			this, SLOT(handleColumnsAboutToBeInserted(int, QList< shared_ptr<Column> >)));
	connect(d_model, SIGNAL(columnsInserted(int, int)),
			this, SLOT(handleColumnsInserted(int, int)));
	connect(d_model, SIGNAL(columnsAboutToBeReplaced(int, int)),
			this, SLOT(handleColumnsAboutToBeRemoved(int, int)));
	connect(d_model, SIGNAL(columnsReplaced(int, int)),
			this, SLOT(handleColumnsInserted(int, int)));
	connect(d_model, SIGNAL(columnsAboutToBeRemoved(int, int)),
				this, SLOT(handleColumnsAboutToBeRemoved(int, int)));
	connect(d_model, SIGNAL(columnsRemoved(int, int)),
			this, SLOT(handleColumnsRemoved(int, int)));

	setModelName();
	connect(abstractAspectSignalEmitter(), SIGNAL(aspectDescriptionChanged(AbstractAspect *)),
		this, SLOT(setModelName()));

	// set initial number of rows and columns
	QList< shared_ptr<Column> > cols;
	for(int i=0; i<columns; i++)
		cols << shared_ptr<Column>(new Column(QString::number(i+1), SciDAVis::Numeric));
	d_model->setRowCount(rows);
	d_model->appendColumns(cols);

	createActions();
}

Table::~Table()
{

}

void Table::setModelName()
{
	d_model->setName(name());
}

QWidget *Table::view(QWidget *parent_widget)
{
	TableView * table_view = new TableView(parent_widget, d_model);
	connect(table_view, SIGNAL(requestContextMenu(TableView *,const QPoint&)), 
		this, SLOT(handleViewContextMenuRequest(TableView *,const QPoint&)));
	connect(table_view, SIGNAL(requestColumnContextMenu(TableView *,const QPoint&)), 
		this, SLOT(handleViewColumnContextMenuRequest(TableView *,const QPoint&)));
	connect(table_view, SIGNAL(requestRowContextMenu(TableView *,const QPoint&)), 
		this, SLOT(handleViewRowContextMenuRequest(TableView *,const QPoint&)));
	connect(table_view, SIGNAL(requestResize(int)),
			this, SLOT(handleModelResizeRequest(int)));
	table_view->setSelectionModel(d_model->selectionModel());

	return table_view;
}

void Table::handleModelResizeRequest(int new_size)
{
	exec(new TableSetNumberOfRowsCmd(d_model, new_size));
}

void Table::handleColumnsAboutToBeInserted(int before, QList< shared_ptr<Column> > new_cols)
{
	Q_UNUSED(before)
	Q_UNUSED(new_cols)
}

void Table::handleColumnsInserted(int first, int count)
{
	for(int i=first; i<first+count; i++)
	{
		shared_ptr<AbstractAspect> child = dynamic_pointer_cast<AbstractAspect>(d_model->output(i));
		int index = d->childCount();
		emit abstractAspectSignalEmitter()->aspectAboutToBeAdded(this, index);
		d->insertChild(index, dynamic_pointer_cast<AbstractAspect>(child));
		child->setParentAspect(this);
		emit abstractAspectSignalEmitter()->aspectAdded(this, index);
		emit child->abstractAspectSignalEmitter()->aspectAdded(child.get());
	}
}

void Table::handleColumnsAboutToBeRemoved(int first, int count)
{
	for(int i=first; i<first+count; i++)
	{
		shared_ptr<AbstractAspect> child = dynamic_pointer_cast<AbstractAspect>(d_model->output(i));
		int index = d->indexOfChild(child);
		Q_ASSERT(index != -1);
		emit abstractAspectSignalEmitter()->aspectAboutToBeRemoved(this, index);
		emit child->abstractAspectSignalEmitter()->aspectAboutToBeRemoved(child.get());
		d->removeChild(child);
		child->setParentAspect(0);
		emit abstractAspectSignalEmitter()->aspectRemoved(this, index);
	}
}

void Table::handleColumnsRemoved(int first, int count)
{
	Q_UNUSED(first)
	Q_UNUSED(count)
}

void Table::insertColumns(int before, QList< shared_ptr<Column> > new_cols)
{
	if( new_cols.size() < 1 || before < 0 || before > columnCount()) return;
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	stack->beginMacro(QObject::tr("%1: insert %2 column(s)").arg(name()).arg(new_cols.size()));
	stack->push(new TableInsertColumnsCmd(d_model, before, new_cols));
	stack->endMacro();
	RESET_CURSOR;
}

void Table::removeColumns(int first, int count)
{
	if( count < 1 || first < 0 || first+count > columnCount()) return;
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	stack->beginMacro(QObject::tr("%1: remove %2 column(s)").arg(name()).arg(count));
	stack->push(new TableRemoveColumnsCmd(d_model, first, count));
	stack->endMacro();
	RESET_CURSOR;
}

void Table::removeRows(int first, int count)
{
	if( count < 1 || first < 0 || first+count > rowCount()) return;
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	stack->beginMacro(QObject::tr("%1: remove %2 row(s)").arg(name()).arg(count));
	int end = d_model->columnCount();
	for(int col=0; col<end; col++)
		d_model->column(col)->removeRows(first, count);
	stack->push(new TableSetNumberOfRowsCmd(d_model, d_model->rowCount()-count));
	stack->endMacro();
	RESET_CURSOR;
}

void Table::insertRows(int before, int count)
{
	if( count < 1 || before < 0 || before > rowCount()) return;
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	stack->beginMacro(QObject::tr("%1: insert %2 row(s)").arg(name()).arg(count));
	int end = d_model->columnCount();
	for(int col=0; col<end; col++)
		d_model->column(col)->insertRows(before, count);
	// the table will be resized automatically if necessary
	stack->endMacro();
	RESET_CURSOR;
}

void Table::setRowCount(int new_size)
{
	if( new_size < 0 ) return;
	WAIT_CURSOR;
	exec(new TableSetNumberOfRowsCmd(d_model, new_size));
	RESET_CURSOR;
}

int Table::columnCount() const
{
	return d_model->columnCount();
}

int Table::rowCount() const
{
	return d_model->rowCount();
}

void Table::showComments(bool on)
{
	WAIT_CURSOR;
	exec(new TableShowCommentsCmd(d_model, on));
	RESET_CURSOR;
}

int Table::columnCount(SciDAVis::PlotDesignation pd) const
{
	int count = 0;
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		if(column(i)->plotDesignation() == pd) count++;
	
	return count;
}

void Table::setColumnCount(int new_size)
{
	int old_size = columnCount();
	if ( old_size == new_size || new_size < 0 )
		return;

	WAIT_CURSOR;
	if (new_size < old_size)
		exec(new TableRemoveColumnsCmd(d_model, new_size, old_size-new_size));
	else
	{
		QList< shared_ptr<Column> > cols;
		for(int i=0; i<new_size-old_size; i++)
			cols << shared_ptr<Column>(new Column(QString::number(i+1), SciDAVis::Numeric));
		appendColumns(cols);
	}
	RESET_CURSOR;
}
		
shared_ptr<Column> Table::column(int index) const 
{ 
	if( index >= 0 & index < columnCount() )
		return d_model->column(index); 
	else
		return shared_ptr<Column>();
}

int Table::columnIndex(Column * col) const 
{ 
	return d_model->columnIndex(col); 
}

int Table::columnIndex(shared_ptr<Column> col) const 
{ 
	return columnIndex(col.get()); 
}

void Table::clear()
{
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	stack->beginMacro(QObject::tr("%1: clear").arg(name()));
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		column(i)->clear();
	stack->endMacro();
	RESET_CURSOR;
}

void Table::clearMasks()
{
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	stack->beginMacro(QObject::tr("%1: clear masks").arg(name()));
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		column(i)->clearMasks();
	stack->endMacro();
	RESET_CURSOR;
}

bool Table::areCommentsShown() const
{
	return d_model->areCommentsShown();
}

void Table::toggleComments()
{
	WAIT_CURSOR;
	showComments(!areCommentsShown());
	RESET_CURSOR;
}

void Table::addColumn()
{
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	stack->beginMacro(QObject::tr("%1: add column").arg(name()));
	setColumnCount(columnCount()+1);
	stack->endMacro();
	RESET_CURSOR;
}

void Table::addColumns()
{
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	int count = selectedColumnCount(false);
	stack->beginMacro(QObject::tr("%1: add %2 column(s)").arg(name()).arg(count));
	setColumnCount(columnCount() + count);
	stack->endMacro();
	RESET_CURSOR;
}

void Table::cutSelection()
{
	// TODO
}

void Table::copySelection()
{
	// TODO
}

void Table::pasteIntoSelection()
{
	// TODO
}

void Table::maskSelection()
{
	QModelIndexList list = d_model->selectionModel()->selectedIndexes();

	WAIT_CURSOR;
	undoStack()->beginMacro(tr("%1: mask selected cell(s)").arg(name()));
	foreach(QModelIndex index, list)
		column(index.column())->setMasked(index.row());
	undoStack()->endMacro();
	RESET_CURSOR;
}

void Table::unmaskSelection()
{
	QModelIndexList list = d_model->selectionModel()->selectedIndexes();

	WAIT_CURSOR;
	undoStack()->beginMacro(tr("%1: unmask selected cell(s)").arg(name()));
	foreach(QModelIndex index, list)
		column(index.column())->setMasked(index.row(), false);
	undoStack()->endMacro();
	RESET_CURSOR;
}

void Table::setFormulaForSelection()
{
	// TODO
}

void Table::recalculateSelectedCells()
{
	// TODO
}

void Table::fillSelectedCellsWithRowNumbers()
{
	if(selectedColumnCount() < 1) return;
	
	QModelIndexList list = d_model->selectionModel()->selectedIndexes();

	WAIT_CURSOR;
	undoStack()->beginMacro(tr("%1: fill cells with row numbers").arg(name()));
	foreach(QModelIndex index, list)
		d_model->setData(index, QString::number(index.row()+1), Qt::EditRole);
	undoStack()->endMacro();
	RESET_CURSOR;
}

void Table::fillSelectedCellsWithRandomNumbers()
{
	if(selectedColumnCount() < 1) return;
	
	QModelIndexList list = d_model->selectionModel()->selectedIndexes();

	WAIT_CURSOR;
	undoStack()->beginMacro(tr("%1: fill cells with random values").arg(name()));
	qsrand(QTime::currentTime().msec());
	foreach(QModelIndex index, list)
	{
		double value = double(qrand())/double(RAND_MAX);
		if (column(index.column())->columnMode() == SciDAVis::Numeric)
			column(index.column())->setValueAt(index.row(), value);
		else
			d_model->setData(index, QString::number(value), Qt::EditRole);
	}
	undoStack()->endMacro();
	RESET_CURSOR;
}

void Table::sortTable()
{
	// TODO
}

void Table::insertEmptyColumns()
{
	// TODO
}

void Table::removeSelectedColumns()
{
	// TODO
}

void Table::clearSelectedColumns()
{
	// TODO
}

void Table::setSelectionAs(SciDAVis::PlotDesignation pd)
{
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	stack->beginMacro(QObject::tr("%1: set plot designation(s)").arg(name()));

	QList< shared_ptr<Column> > list = selectedColumns();
	foreach(shared_ptr<Column> ptr, list)
		ptr->setPlotDesignation(pd);

	stack->endMacro();
	RESET_CURSOR;
}

void Table::setSelectedColumnsAsX()
{
	setSelectionAs(SciDAVis::X);
}

void Table::setSelectedColumnsAsY()
{
	setSelectionAs(SciDAVis::Y);
}

void Table::setSelectedColumnsAsZ()
{
	setSelectionAs(SciDAVis::Z);
}

void Table::setSelectedColumnsAsYError()
{
	setSelectionAs(SciDAVis::yErr);
}

void Table::setSelectedColumnsAsXError()
{
	setSelectionAs(SciDAVis::xErr);
}

void Table::setSelectedColumnsAsNone()
{
	setSelectionAs(SciDAVis::noDesignation);
}

void Table::normalizeSelectedColumns()
{
	// TODO
}

void Table::sortSelectedColumns()
{
	// TODO
}

void Table::statisticsOnSelectedColumns()
{
	// TODO
}

void Table::statisticsOnSelectedRows()
{
	// TODO
}

void Table::insertEmptyRows()
{
	// TODO
}

void Table::removeSelectedRows()
{
	// TODO
}

void Table::clearSelectedRows()
{
	// TODO
}

void Table::convertSelectedColumns()
{
	// TODO
}

void Table::addRows()
{
	WAIT_CURSOR;
	QUndoStack * stack = undoStack();
	int count = selectedRowCount(false);
	stack->beginMacro(QObject::tr("%1: add %2 rows(s)").arg(name()).arg(count));
	exec(new TableSetNumberOfRowsCmd(d_model, rowCount() + count));
	stack->endMacro();
	RESET_CURSOR;
}

void Table::clearSelectedCells()
{
	QModelIndexList list = d_model->selectionModel()->selectedIndexes();

	WAIT_CURSOR;
	undoStack()->beginMacro(tr("%1: clear selected cell(s)").arg(name()));
	foreach(QModelIndex index, list)
	{
		if(column(index.column())->rowCount() > index.row())
			d_model->setData(index, QString(""), Qt::EditRole);
	}
	undoStack()->endMacro();
	RESET_CURSOR;
}

QMenu *Table::createContextMenu()
{
	QMenu *menu = AbstractAspect::createContextMenu();

	// TODO menu->addAction( ....

	// Export to ASCII
	// Print
	// ----
	// Rename
	// Duplicate
	// Hide/Show
	// Activate ?
	// Resize
	// Close
	// ----
	// Properties
	
	return menu;
}
		

void Table::createActions()
{
	// selection related actions
	action_cut_selection = new QAction(QIcon(QPixmap(":/cut.xpm")), tr("Cut"), this);
	connect(action_cut_selection, SIGNAL(triggered()), this, SLOT(cutSelection()));

	action_copy_selection = new QAction(QIcon(QPixmap(":/copy.xpm")), tr("Copy"), this);
	connect(action_copy_selection, SIGNAL(triggered()), this, SLOT(copySelection()));

	action_paste_into_selection = new QAction(QIcon(QPixmap(":/paste.xpm")), tr("Paste"), this);
	connect(action_paste_into_selection, SIGNAL(triggered()), this, SLOT(pasteIntoSelection()));

	action_mask_selection = new QAction(QIcon(QPixmap(":/mask.xpm")), tr("Mask","mask selection"), this);
	connect(action_mask_selection, SIGNAL(triggered()), this, SLOT(maskSelection()));

	action_unmask_selection = new QAction(QIcon(QPixmap(":/unmask.xpm")), tr("Unmask","unmask selection"), this);
	connect(action_unmask_selection, SIGNAL(triggered()), this, SLOT(unmaskSelection()));

	action_set_formula = new QAction(QIcon(QPixmap()), tr("Set Formula"), this);
	connect(action_set_formula, SIGNAL(triggered()), this, SLOT(setFormulaForSelection()));

	action_clear_selection = new QAction(QIcon(QPixmap(":/erase.xpm")), tr("Clear","clear selection"), this);
	connect(action_clear_selection, SIGNAL(triggered()), this, SLOT(clearSelectedCells()));

	action_recalculate = new QAction(QIcon(QPixmap()), tr("Recalculate"), this);
	connect(action_recalculate, SIGNAL(triggered()), this, SLOT(recalculateSelectedCells()));

	action_fill_row_numbers = new QAction(QIcon(QPixmap(":/rowNumbers.xpm")), tr("Row Numbers"), this);
	connect(action_fill_row_numbers, SIGNAL(triggered()), this, SLOT(fillSelectedCellsWithRowNumbers()));

	action_fill_random = new QAction(QIcon(QPixmap(":/randomNumbers.xpm")), tr("Random Values"), this);
	connect(action_fill_random, SIGNAL(triggered()), this, SLOT(fillSelectedCellsWithRandomNumbers()));
	
	//table related actions
	action_toggle_comments = new QAction(QIcon(QPixmap()), QString(), this); // show/hide column comments
	connect(action_toggle_comments, SIGNAL(triggered()), this, SLOT(toggleComments()));

	action_toggle_tabbar = new QAction(QIcon(QPixmap()), QString(), this); // show/hide sidebar

	action_select_all = new QAction(QIcon(QPixmap()), tr("Select All"), this);
	connect(action_select_all, SIGNAL(triggered()), this, SLOT(selectAll()));

	action_add_column = new QAction(QIcon(QPixmap(":/addCol.xpm")), tr("Add Column"), this);
	connect(action_add_column, SIGNAL(triggered()), this, SLOT(addColumn()));

	action_clear_table = new QAction(QIcon(QPixmap(":/erase")), tr("Clear Table"), this);
	connect(action_clear_table, SIGNAL(triggered()), this, SLOT(clear()));

	action_clear_masks = new QAction(QIcon(QPixmap(":/unmask.xpm")), tr("Clear Masks"), this);
	connect(action_clear_masks, SIGNAL(triggered()), this, SLOT(clearMasks()));

	action_sort_table = new QAction(QIcon(QPixmap()), tr("Sort Table"), this);
	connect(action_sort_table, SIGNAL(triggered()), this, SLOT(sortTable()));

	action_go_to_cell = new QAction(QIcon(QPixmap()), tr("Go to Cell"), this);
	connect(action_go_to_cell, SIGNAL(triggered()), this, SLOT(goToCell()));

	// column related actions
	action_insert_columns = new QAction(QIcon(QPixmap()), tr("Insert Empty Columns"), this);
	connect(action_insert_columns, SIGNAL(triggered()), this, SLOT(insertEmptyColumns()));

	action_remove_columns = new QAction(QIcon(QPixmap()), tr("Remove"), this);
	connect(action_remove_columns, SIGNAL(triggered()), this, SLOT(removeSelectedColumns()));

	action_clear_columns = new QAction(QIcon(QPixmap(":/erase.xpm")), tr("Clear"), this);
	connect(action_clear_columns, SIGNAL(triggered()), this, SLOT(clearSelectedColumns()));

	action_add_columns = new QAction(QIcon(QPixmap(":/addCol.xpm")), tr("Add Columns"), this);
	connect(action_add_columns, SIGNAL(triggered()), this, SLOT(addColumns()));

	action_set_as_x = new QAction(QIcon(QPixmap()), tr("X","plot designation"), this);
	connect(action_set_as_x, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsX()));

	action_set_as_y = new QAction(QIcon(QPixmap()), tr("Y","plot designation"), this);
	connect(action_set_as_y, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsY()));

	action_set_as_z = new QAction(QIcon(QPixmap()), tr("Z","plot designation"), this);
	connect(action_set_as_z, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsZ()));

	action_set_as_xerr = new QAction(QIcon(QPixmap(":/xerror.xpm")), tr("X Error","plot designation"), this);
	connect(action_set_as_xerr, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsXError()));

	action_set_as_yerr = new QAction(QIcon(QPixmap(":/yerror.xpm")), tr("Y Error","plot designation"), this);
	connect(action_set_as_yerr, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsYError()));

	action_set_as_none = new QAction(QIcon(QPixmap()), tr("None","plot designation"), this);;
	connect(action_set_as_none, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsNone()));

	action_normalize_columns = new QAction(QIcon(QPixmap()), tr("Normalize"), this);;
	connect(action_normalize_columns, SIGNAL(triggered()), this, SLOT(normalizeSelectedColumns()));

	action_sort_columns = new QAction(QIcon(QPixmap()), tr("Sort"), this);;
	connect(action_sort_columns, SIGNAL(triggered()), this, SLOT(sortSelectedColumns()));

	action_statistics_columns = new QAction(QIcon(QPixmap()), tr("Statistics"), this);;
	connect(action_statistics_columns, SIGNAL(triggered()), this, SLOT(statisticsOnSelectedColumns()));

	action_convert_columns = new QAction(QIcon(QPixmap()), tr("Convert"), this);;
	connect(action_convert_columns, SIGNAL(triggered()), this, SLOT(convertSelectedColumns()));

	// row related actions
	action_insert_rows = new QAction(QIcon(QPixmap()), tr("Insert Empty Rows"), this);;
	connect(action_insert_rows, SIGNAL(triggered()), this, SLOT(insertEmptyRows()));

	action_remove_rows = new QAction(QIcon(QPixmap()), tr("Remove"), this);;
	connect(action_remove_rows, SIGNAL(triggered()), this, SLOT(removeSelectedRows()));

	action_clear_rows = new QAction(QIcon(QPixmap(":/erase.xpm")), tr("Clear"), this);;
	connect(action_clear_rows, SIGNAL(triggered()), this, SLOT(clearSelectedRows()));

	action_add_rows = new QAction(QIcon(QPixmap()), tr("Add Rows"), this);;
	connect(action_add_rows, SIGNAL(triggered()), this, SLOT(addRows()));

	action_statistics_rows = new QAction(QIcon(QPixmap()), tr("Statistics"), this);;
	connect(action_statistics_rows, SIGNAL(triggered()), this, SLOT(statisticsOnSelectedRows()));

}

void Table::handleViewContextMenuRequest(TableView *view, const QPoint& pos)
{
	QMenu context_menu;
	QAction * temp_action;

	temp_action = undoStack()->createUndoAction(&context_menu);
	temp_action->setIcon(QIcon(QPixmap(":/undo.xpm")));
	context_menu.addAction(temp_action);
	temp_action = undoStack()->createRedoAction(&context_menu);
	temp_action->setIcon(QIcon(QPixmap(":/redo.xpm")));
	context_menu.addAction(temp_action);
	context_menu.addSeparator();

	createSelectionMenu(&context_menu);
	context_menu.addSeparator();
	createTableMenu(&context_menu);
	context_menu.addSeparator();

	QString action_name;
	if(areCommentsShown()) 
		action_name = tr("Hide Comments");
	else
		action_name = tr("Show Comments");
	action_toggle_comments->setText(action_name);

	if(view->isOptionTabBarVisible()) 
		action_name = tr("Hide Options");
	else
		action_name = tr("Show Options");
	action_toggle_tabbar->setText(action_name);

	// connections for "Go to Cell" and "Show/Hide OptionTabBar" to the relevant view
	connect(this, SIGNAL(scrollToIndex(const QModelIndex&)), view, SLOT(scrollToIndex(const QModelIndex&)));
	connect(action_toggle_tabbar, SIGNAL(triggered()), view, SLOT(toggleOptionTabBar()));
	context_menu.exec(pos);
	disconnect(this, SIGNAL(scrollToIndex(const QModelIndex&)), view, SLOT(scrollToIndex(const QModelIndex&)));
	disconnect(action_toggle_tabbar, SIGNAL(triggered()), view, SLOT(toggleOptionTabBar()));
}

void Table::handleViewColumnContextMenuRequest(TableView *view, const QPoint& pos)
{
	QMenu context_menu;
	QAction * temp_action;

	temp_action = undoStack()->createUndoAction(&context_menu);
	temp_action->setIcon(QIcon(QPixmap(":/undo.xpm")));
	context_menu.addAction(temp_action);
	temp_action = undoStack()->createRedoAction(&context_menu);
	temp_action->setIcon(QIcon(QPixmap(":/redo.xpm")));
	context_menu.addAction(temp_action);
	context_menu.addSeparator();

	createColumnMenu(&context_menu);
	context_menu.addSeparator();

	context_menu.exec(pos);
}

void Table::handleViewRowContextMenuRequest(TableView *view, const QPoint& pos)
{
	QMenu context_menu;
	QAction * temp_action;

	temp_action = undoStack()->createUndoAction(&context_menu);
	temp_action->setIcon(QIcon(QPixmap(":/undo.xpm")));
	context_menu.addAction(temp_action);
	temp_action = undoStack()->createRedoAction(&context_menu);
	temp_action->setIcon(QIcon(QPixmap(":/redo.xpm")));
	context_menu.addAction(temp_action);
	context_menu.addSeparator();

	createRowMenu(&context_menu);

	context_menu.exec(pos);
}

// This probably needs to be moved to the main window
QMenu * Table::createApplicationWindowMenu()
{
	QMenu * menu = new QMenu(tr("Table"));

	// [Context Menu]
	// Show/Hide Column Options (Dock)
	// Set Dimensions
	// Convert to Matrix
	return menu;
}

QMenu * Table::createSelectionMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();


	menu->addAction(action_cut_selection);
	menu->addAction(action_copy_selection);
	menu->addAction(action_paste_into_selection);
	menu->addAction(action_clear_selection);
	menu->addSeparator();
	menu->addAction(action_mask_selection);
	menu->addAction(action_unmask_selection);
	menu->addSeparator();
	menu->addAction(action_set_formula);
	menu->addAction(action_recalculate);
	menu->addSeparator();

	QMenu * submenu = new QMenu("Fill with");
	submenu->addAction(action_fill_row_numbers);
	submenu->addAction(action_fill_random);
	menu->addMenu(submenu);

	return menu;
}


QMenu * Table::createColumnMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_insert_columns);
	menu->addAction(action_remove_columns);
	menu->addAction(action_clear_columns);
	menu->addAction(action_add_columns);
	menu->addSeparator();
	
	QMenu * submenu = new QMenu("Set As");

	submenu->addAction(action_set_as_x);
	submenu->addAction(action_set_as_y);
	submenu->addAction(action_set_as_z);
	submenu->addAction(action_set_as_xerr);
	submenu->addAction(action_set_as_yerr);
	submenu->addAction(action_set_as_none);
	menu->addMenu(submenu);
	menu->addSeparator();

	menu->addAction(action_normalize_columns);
	menu->addAction(action_sort_columns);
	menu->addSeparator();
	menu->addAction(action_statistics_columns);

	return menu;
}

QMenu * Table::createTableMenu(QMenu * append_to) 
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_toggle_comments);
	menu->addAction(action_toggle_tabbar);
	menu->addSeparator();
	menu->addAction(action_select_all);
	menu->addAction(action_clear_table);
	menu->addAction(action_clear_masks);
	menu->addAction(action_sort_table);
	menu->addSeparator();
	menu->addAction(action_add_column);
	menu->addSeparator();
	menu->addAction(action_go_to_cell);

	return menu;
}

QMenu * Table::createRowMenu(QMenu * append_to) 
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_insert_rows);
	menu->addAction(action_remove_rows);
	menu->addAction(action_clear_rows);
	menu->addAction(action_add_rows);
	menu->addSeparator();
	menu->addAction(action_statistics_rows);

	return menu;
}

void Table::goToCell()
{
	bool ok;

	int col = QInputDialog::getInteger(0, tr("Go to Cell"), tr("Enter column"),
			1, 1, columnCount(), 1, &ok);
	if ( !ok ) return;

	int row = QInputDialog::getInteger(0, tr("Go to Cell"), tr("Enter row"),
			1, 1, rowCount(), 1, &ok);
	if ( !ok ) return;

	emit scrollToIndex(d_model->index(row-1, col-1, QModelIndex()));
}

int Table::selectedColumnCount(bool full)
{
	int count = 0;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i, full)) count++;
	return count;
}

int Table::selectedColumnCount(SciDAVis::PlotDesignation pd)
{
	int count = 0;
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		if( isColumnSelected(i, false) && (column(i)->plotDesignation() == pd) ) count++;

	return count;
}

bool Table::isColumnSelected(int col, bool full)
{
	QItemSelectionModel * sel_model = d_model->selectionModel();
	if(full)
		return sel_model->isColumnSelected(col, QModelIndex());
	else
		return sel_model->columnIntersectsSelection(col, QModelIndex());
}

QList< shared_ptr<Column> > Table::selectedColumns(bool full)
{
	QList< shared_ptr<Column> > list;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i, full)) list << column(i);

	return list;
}

int Table::selectedRowCount(bool full)
{
	int count = 0;
	int rows = rowCount();
	for (int i=0; i<rows; i++)
		if(isRowSelected(i, full)) count++;
	return count;
}

bool Table::isRowSelected(int row, bool full)
{
	QItemSelectionModel * sel_model = d_model->selectionModel();
	if(full)
		return sel_model->isRowSelected(row, QModelIndex());
	else
		return sel_model->rowIntersectsSelection(row, QModelIndex());
}

void Table::selectAll()
{
	QItemSelectionModel * sel_model = d_model->selectionModel();
	QItemSelection sel(d_model->index(0, 0, QModelIndex()), d_model->index(rowCount()-1, columnCount()-1, QModelIndex()));
	sel_model->select(sel, QItemSelectionModel::Select);
}


#if false


	Table::Table(AbstractScriptingEngine *engine, int rows, int cols, const QString& label, QWidget* parent, const char* name, Qt::WFlags f)
: MyWidget(label,parent,name,f), scripted(engine)
{
	// TODO: check the lines marked with ### whether they can be removed
	//###	d_selected_col = -1;
	//###	d_saved_cells = 0;

	QDateTime dt = QDateTime::currentDateTime();
	setBirthDate(dt.toString(Qt::LocalDate));

	// create model and view
	d_model = new TableModel(this);
	d_table_view = new TableView(this, d_model, rows, cols);
	d_table_view->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	d_table_view->setFocusPolicy(Qt::StrongFocus);
	d_table_view->setFocus();

	d_table_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

	// header stuff
	QHeaderView * v_header = d_table_view->verticalHeader();
	QHeaderView * h_header = d_table_view->horizontalHeader();

	h_header->viewport()->installEventFilter(this);

	//###	connect(h_header, SIGNAL(sectionDoubleClicked(int)),
	//###			this, SLOT(headerDoubleClickedHandler(int)));

	//###	connect(h_header, SIGNAL(sectionResized(int, int, int)),
	//###			this, SLOT(colWidthModified(int, int, int)));

	// main layout
	QHBoxLayout* main_layout = new QHBoxLayout(this);
	main_layout->setMargin(0);
	main_layout->addWidget(d_table_view);

	// initialized the columns
	for (int i=0; i<cols; i++)
	{
		d_model->setColumnLabel(i, QString::number(i+1));
		d_model->setColumnPlotDesignation(i, SciDAVis::Y);
	}
	if(cols > 0)
		d_model->setColumnPlotDesignation(0, SciDAVis::X);

	// calculate initial geometry
	int w=4*h_header->sectionSize(0);
	int h;
	if (rows>11)
		h=11*v_header->sectionSize(0);
	else
		h=(rows+1)*v_header->sectionSize(0);
	setGeometry(50, 50, w + 45, h);

	// keyboard shortcuts
	QShortcut * sel_all = new QShortcut(QKeySequence(tr("Ctrl+A", "Table: select all")), this);
	connect(sel_all, SIGNAL(activated()), d_table_view, SLOT(selectAll()));
}

void Table::setBackgroundColor(const QColor& col)
{
	QPalette pal = d_table_view->palette();
	pal.setColor(QPalette::Base, col);
	d_table_view->setPalette(pal);
}

void Table::setTextColor(const QColor& col)
{
	QPalette pal = d_table_view->palette();
	pal.setColor(QPalette::Text, col);
	d_table_view->setPalette(pal);
}

void Table::setTextFont(const QFont& fnt)
{
	d_table_view->setFont(fnt);
}

void Table::setHeaderColor(const QColor& col)
{
	QPalette pal = d_table_view->horizontalHeader()->palette();
	pal.setColor(QPalette::ButtonText, col);
	d_table_view->horizontalHeader()->setPalette(pal);

	pal = d_table_view->verticalHeader()->palette();
	pal.setColor(QPalette::ButtonText, col);
	d_table_view->verticalHeader()->setPalette(pal);
}

void Table::setHeaderFont(const QFont& fnt)
{
	d_table_view->horizontalHeader()->setFont(fnt);
}

QStringList Table::columnLabels()
{
	QStringList list;
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		list << d_model->columnLabel(i);

	return list;
}

void Table::copy(Table * other)
{
	// TODO
}

void Table::setAscendingValues()
{
	if(selectedColumnCount() < 1) return;

	WAIT_CURSOR;
	int rows = rowCount();

	QStringList data;

	int end = lastSelectedColumn();
	for(int i=firstSelectedColumn(); i<=end; i++)
	{
		if(isColumnSelected(i))
		{
			data.clear();
			for(int j=0; j<rows; j++)
			{
				if(isCellSelected(j, i))
					data << QString::number(j+1);
			}
			undoStack()->push(new TableSetColumnValuesCmd(d_model, i, data) );
		}
	}

	// TODO: remove this later
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::setRandomValues()
{
	if(selectedColumnCount() < 1) return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows = rowCount();

	qsrand(QTime::currentTime().msec());

	QStringList data;

	int end = lastSelectedColumn();
	for(int i=firstSelectedColumn(); i<=end; i++)
	{
		if(isColumnSelected(i))
		{
			data.clear();
			for(int j=0; j<rows; j++)
			{
				if(isCellSelected(j, i))
					data << QString::number(double(qrand())/double(RAND_MAX));
			}
			undoStack()->push(new TableSetColumnValuesCmd(d_model, i, data) );
		}
	}

	// TODO: remove this later
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

int Table::firstSelectedColumn(bool full)
{
	int cols = columnCount();
	for (int i=0; i<cols; i++)
	{
		if(isColumnSelected(i, full))
			return i;
	}
	return -1;
}

int Table::lastSelectedColumn(bool full)
{
	int cols = columnCount();
	for(int i=cols-1; i>=0; i--)
		if(isColumnSelected(i, full)) return i;

	return -1;
}

int Table::firstSelectedRow(bool full)
{
	int rows = rowCount();
	for (int i=0; i<rows; i++)
	{
		if(isRowSelected(i, full))
			return i;
	}
	return -1;
}

int Table::lastSelectedRow(bool full)
{
	int rows = rowCount();
	for(int i=rows-1; i>=0; i--)
		if(isRowSelected(i, full)) return i;

	return -1;
}


bool Table::isCellSelected(int row, int col)
{
	if(row < 0 || col < 0 || row >= rowCount() || col >= columnCount()) return false;

	return d_model->output(col)->isSelected(row);
	// Remark: This could also be done like this:
	// <code>
	// return d_table_view->selectionModel()->isSelected( d_model->index(row, col, QModelIndex()) );
	// </code>
	// But since the selection is synchronized between columns and selection
	// model there should be at most a slight speed difference, if any. - thzs
}

void Table::clearColumn(int col)
{
	if( col >= 0 && col <= columnCount() )
		undoStack()->push(new TableClearColumnCmd(d_model, col));

	//TODO: remove this later
	emit modifiedWindow(this);
}

void Table::clear()
{
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		clearColumn(i);

	//TODO: remove this later
	emit modifiedWindow(this);
}

void Table::goToCell(int row, int col)
{
	if( (row < 0) || (row >= rowCount()) ) return;
	if( (col < 0) || (col >= columnCount()) ) return;

	d_table_view->scrollTo(d_model->index(row, col, QModelIndex()), QAbstractItemView::PositionAtCenter);
}

int Table::colX(int col)
{
	for(int i=col-1; i>=0; i--)
	{
		if (d_model->columnPlotDesignation(i) == SciDAVis::X)
			return i;
	}
	int cols = columnCount();
	for(int i=col+1; i<cols; i++)
	{
		if (d_model->columnPlotDesignation(i) == SciDAVis::X)
			return i;
	}
	return -1;
}

int Table::colY(int col)
{
	int cols = columnCount();
	// look to the right first
	for(int i=col+1; i<cols; i++)
	{
		if (d_model->columnPlotDesignation(i) == SciDAVis::Y)
			return i;
	}
	for(int i=col-1; i>=0; i--)
	{
		if (d_model->columnPlotDesignation(i) == SciDAVis::Y)
			return i;
	}
	return -1;
}

SciDAVis::ColumnMode Table::columnMode(int col)
{
	AbstractDataSource * col_ptr = d_model->output(col);

	if(col_ptr->inherits("DoubleColumnData"))
		return SciDAVis::Numeric;

	if(col_ptr->inherits("DateTimeColumnData"))
	{
		AbstractFilter * filter = d_model->inputFilter(col);
		if(dynamic_cast<String2MonthFilter *>(filter))
			return SciDAVis::Month;
		if(dynamic_cast<String2DayOfWeekFilter *>(filter))
			return SciDAVis::Day;
		return SciDAVis::DateTime;
	}

	return SciDAVis::Text;
}

void Table::setColumnMode(int col, SciDAVis::ColumnMode mode)
{
	AbstractDataSource * old_col = d_model->output(col);
	AbstractColumnData * new_col = 0;
	AbstractFilter *filter, *new_in_filter, *new_out_filter;

	// convert the column
	if(old_col->inherits("DoubleColumnData"))
	{
		switch(mode)
		{		
			case SciDAVis::Numeric:
				return;
			case SciDAVis::Text:
				filter = new Double2StringFilter();
				new_col = new StringColumnData();
				break;
			case SciDAVis::Date:
			case SciDAVis::Time:
			case SciDAVis::DateTime:
				filter = new Double2DateTimeFilter();
				new_col = new DateTimeColumnData();
				break;
			case SciDAVis::Month:
				filter = new Double2MonthFilter();
				new_col = new DateTimeColumnData();
				break;
			case SciDAVis::Day:
				filter = new Double2DayOfWeekFilter();
				new_col = new DateTimeColumnData();
				break;
		} // switch(mode)
	}
	else if(old_col->inherits("StringColumnData"))
	{
		switch(mode)
		{		
			case SciDAVis::Text:
				return;
			case SciDAVis::Numeric:
				filter = new String2DoubleFilter();
				new_col = new DoubleColumnData();
				break;
			case SciDAVis::Date:
			case SciDAVis::Time:
			case SciDAVis::DateTime:
				filter = new String2DateTimeFilter();
				new_col = new DateTimeColumnData();
				break;
			case SciDAVis::Month:
				filter = new String2MonthFilter();
				new_col = new DateTimeColumnData();
				break;
			case SciDAVis::Day:
				filter = new String2DayOfWeekFilter();
				new_col = new DateTimeColumnData();
				break;
		} // switch(mode)
	}
	else if(old_col->inherits("DateColumnData"))
	{
		switch(mode)
		{		
			case SciDAVis::Date:
			case SciDAVis::Time:
			case SciDAVis::DateTime:
				return;
			case SciDAVis::Text:
				filter = new DateTime2StringFilter();
				new_col = new StringColumnData();
				break;
			case SciDAVis::Numeric:
				filter = new DateTime2DoubleFilter();
				new_col = new DoubleColumnData();
				break;
			case SciDAVis::Month:
				filter = new CopyThroughFilter();
				new_col = new DateTimeColumnData();
				break;
			case SciDAVis::Day:
				filter = new CopyThroughFilter();
				new_col = new DateTimeColumnData();
				break;
		} // switch(mode)
	}

	// determine the new input and output filters
	switch(mode)
	{		
		case SciDAVis::Numeric:
			new_in_filter = new String2DoubleFilter();
			new_out_filter = new Double2StringFilter();
			break;
		case SciDAVis::Text:
			new_in_filter = new CopyThroughFilter();
			new_out_filter = new CopyThroughFilter();
			break;
		case SciDAVis::Date:
		case SciDAVis::Time:
		case SciDAVis::DateTime:
			new_in_filter = new String2DateTimeFilter();
			new_out_filter = new DateTime2StringFilter();
			break;
		case SciDAVis::Month:
			new_in_filter = new String2MonthFilter();
			new_out_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(new_out_filter)->setFormat("MMMM");
			break;
		case SciDAVis::Day:
			new_in_filter = new String2DayOfWeekFilter();
			new_out_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(new_out_filter)->setFormat("dddd");
			break;
	} // switch(mode)


	if(new_col == 0) return;

	// copy the filtered, i.e. converted, column
	filter->input(0, old_col);
	new_col->copy(filter->output(0));
	// keep the designation, label and comment
	new_col->copyDescription(old_col);
	// keep the formulas
	QList< Interval<int> > formulas = old_col->formulaIntervals();
	foreach(Interval<int> i, formulas)
		new_col->setFormula(i, old_col->formula(i.start()));

	// replace the old column
	QList<AbstractColumnData *> cols;
	QList<AbstractFilter *> in_filters;
	QList<AbstractFilter *> out_filters;
	cols.append(new_col);
	in_filters.append(new_in_filter);
	out_filters.append(new_out_filter);
	// TODO: For DataTime<->Day/Month conversion a ChangeInputFilterCmd could be used to save memory
	QUndoCommand * cmd = new TableReplaceColumnsCmd(d_model, col, cols, in_filters, out_filters);
	cmd->setText(tr("change column type")); 
	undoStack()->push(cmd);
}

int Table::columnWidth(int col)
{
	return d_table_view->columnWidth(col);
}

void Table::contextMenuEvent(QContextMenuEvent *e)
{
	int right;
	right = d_table_view->columnViewportPosition(columnCount()-1) +
		d_table_view->columnWidth(columnCount()-1) - 1;

	setFocus();
	if (e->pos().x() > right + d_table_view->verticalHeader()->width()) // outside rightmost column
		emit showContextMenu(false);
	else
		emit showContextMenu(true);
	e->accept();
}



// -------------------------------------------------------------------------
// transition functions
// -------------------------------------------------------------------------
int Table::colIndex(const QString& name)
{
	OBSOLETE
		int pos=name.find("_",false);
	QString label=name.right(name.length()-pos-1);
	for(int i=0; i<columnCount(); i++)
		if(d_model->columnLabel(i) == label)
			return i;
}

QString Table::text(int row, int col)
{
	OBSOLETE
		return d_model->data(d_model->index(row, col, QModelIndex()), Qt::EditRole).toString();
}

QString Table::colName(int col)
{ //returns the table name + underscore + column label
	OBSOLETE
		if (col<0 || col >= columnCount())
			return QString();

	return QString(this->name())+"_"+d_model->columnLabel(col);
}

int Table::columnType(int col)
{
	OBSOLETE
		AbstractDataSource * ptr = d_model->output(col);
	if(ptr->inherits("DoubleDataSource"))
		return SciDAVis::Numeric;
	if(ptr->inherits("StringDataSource"))
		return SciDAVis::Text;
	if(ptr->inherits("DateTimeDataSource"))
		return SciDAVis::Date;
}

QString Table::columnFormat(int col)
{
	OBSOLETE
		AbstractFilter * fltr = d_model->outputFilter(col);
	QObject * ptr = dynamic_cast<QObject *>(fltr);
	if(ptr && ptr->inherits("DateTime2StringFilter"))
		return static_cast<DateTime2StringFilter *>(ptr)->format();
	else
		return QString();
}

QStringList Table::selectedYLabels()
{
	OBSOLETE
		QStringList names;
	for (int i=0; i<columnCount(); i++)
	{
		if(isColumnSelected (i) && plotDesignation(i) == SciDAVis::Y)
			names << columnLabel(i);
	}
	return names;
}

double Table::cell(int row, int col)
{
	OBSOLETE
		return d_model->data(d_model->index(row, col, QModelIndex()), Qt::EditRole).toDouble();
}

int Table::selectedColumn()
{
	OBSOLETE
		return firstSelectedColumn();
}


void Table::setSelectedCol(int col)
{
	OBSOLETE
		Q_UNUSED(col)
}

QStringList Table::colNames()
{
	OBSOLETE
		QStringList names;
	for (int i=0; i<columnCount(); i++)
	{
		names << columnLabel(i);
	}
	return names;
}

void Table::setText(int row, int col, QString text)
{
	OBSOLETE
		QModelIndex index = d_model->index(row, col, QModelIndex());
	d_model->setData(index, text, Qt::EditRole);
	undoStack()->push(new TableUserInputCmd(d_model, index) );		
}

void Table::setHeader(QStringList header)
{
	OBSOLETE
		for(int i=0; i<header.size(); i++)
			undoStack()->push(new TableSetColumnLabelCmd(d_model, i, header.at(i)) );		
}

int Table::colPlotDesignation(int col)
{
	OBSOLETE
		return int(plotDesignation(col));
}

void Table::setColPlotDesignation(int col, SciDAVis::PlotDesignation pd)
{
	OBSOLETE
		setPlotDesignation(col, pd);
}

QString Table::colLabel(int col)
{
	OBSOLETE
		return columnLabel(col);
}

QString Table::colComment(int col)
{
	OBSOLETE
		return columnComment(col);
}

void Table::columnNumericFormat(int col, char *f, int *precision)
{
	OBSOLETE
		AbstractFilter * fltr = d_model->outputFilter(col);
	QObject * ptr = dynamic_cast<QObject *>(fltr);

	if( ptr && ptr->inherits("Double2StringFilter"))
	{
		Double2StringFilter * d2sf = static_cast<Double2StringFilter *>(ptr);
		*f = d2sf->numericFormat();
		*precision = d2sf->numDigits();
	}
	else
	{
		*f = 0;
		*precision = 0;
	}
}

void Table::columnNumericFormat(int col, int *f, int *precision)
{
	OBSOLETE
		char format;
	columnNumericFormat(col, &format, precision);

	if( !format )
		*f = 0;
	else
		switch(format)
		{
			case 'g':
				*f = 0;
				break;

			case 'f':
				*f = 1;
				break;

			case 'e':
				*f = 2;
				break;
		}
}

void Table::changeColWidth(int width, bool all)
{
	OBSOLETE
		int cols = columnCount();
	if (all)
	{
		for (int i=0;i<cols;i++)
			d_table_view->setColumnWidth(i, width);
		emit modifiedWindow(this);
	}
	else
	{
		if(firstSelectedColumn() >= 0)
			d_table_view->setColumnWidth(firstSelectedColumn(), width);
		emit modifiedWindow(this);
	}
}

void Table::enumerateRightCols(bool checked)
{
	OBSOLETE
}


void Table::setColComment(int col, const QString& s)
{
	OBSOLETE
		setColumnComment(col, s);
}

void Table::changeColName(const QString& new_name)
{
	OBSOLETE
		if(selectedColumnCount() < 1) return;

	QString old_name = columnLabel(firstSelectedColumn());
	setColumnLabel(firstSelectedColumn(), new_name);
	emit changedColHeader(old_name, new_name);
	emit modifiedWindow(this);
}

void Table::setColName(int col,const QString& new_name)
{
	OBSOLETE
		setColumnLabel(col, new_name);
}

void Table::setCommand(int col, const QString& com)
{
	OBSOLETE
		undoStack()->push(new TableSetFormulaCmd(d_model, col, 
					Interval<int>(0, d_model->output(col)->rowCount()-1), com.trimmed()));
}

void Table::setColNumericFormat(int f, int prec, int col)
{
	OBSOLETE
}

void Table::setTextFormat(int col)
{
	OBSOLETE
		if (col >= 0 && col < columnCount())
			setColumnMode(col, SciDAVis::Text);
	emit modifiedWindow(this);
}

void Table::setDateFormat(const QString& format, int col)
{
	OBSOLETE
}

void Table::setTimeFormat(const QString& format, int col)
{
	OBSOLETE
}

void Table::setMonthFormat(const QString& format, int col)
{
	OBSOLETE
}

void Table::setDayFormat(const QString& format, int col)
{
	OBSOLETE
}

bool Table::setDateTimeFormat(int col, int f, const QString& format)
{
	OBSOLETE
}

int Table::verticalHeaderWidth()
{
	OBSOLETE
		return d_table_view->verticalHeader()->width();
}

QStringList Table::columnsList()
{
	OBSOLETE
		QStringList names;
	for (int i=0;i<columnCount();i++)
		names << QString(name())+"_"+columnLabel(i);

	return names;
}

int Table::firstXCol()
{
	OBSOLETE
		for (int j=0; j<columnCount(); j++)
		{
			if (plotDesignation(j) == SciDAVis::X)
				return j;
		}
	return -1;
}

QStringList Table::selectedColumnsOld()
{
	OBSOLETE
		QStringList list;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i)) list << columnLabel(i);

	return list;
}

void Table::addCol(SciDAVis::PlotDesignation pd)
{
	OBSOLETE
		setColumnCount(columnCount() + 1);
	setPlotDesignation(columnCount()-1, pd);
	emit modifiedWindow(this);
}

bool Table::noXColumn()
{
	OBSOLETE
		return columnCount(SciDAVis::X) == 0;
}

bool Table::noYColumn()
{
	OBSOLETE
		return columnCount(SciDAVis::Y) == 0;
}

QStringList Table::selectedYColumns()
{
	OBSOLETE
		QStringList list;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if ( (plotDesignation(i) == SciDAVis::Y) && isColumnSelected(i) )
			list << columnLabel(i);

	return list;
}

void Table::setNumericPrecision(int prec)
{
	OBSOLETE
		for (int i=0; i<columnCount(); i++)
		{
			if(d_model->output(i)->inherits("DoubleColumnData"))
			{
				char format = static_cast<Double2StringFilter *>(d_model->outputFilter(i))->numericFormat();
				undoStack()->push(new TableSetColumnNumericDisplayCmd(d_model, i, format, prec));
			}
		}

}

QVarLengthArray<double> Table::col(int ycol)
{
	OBSOLETE
		int i;
	int rows = rowCount();
	int cols = columnCount();
	QVarLengthArray<double> Y(rows);

	if (ycol<=cols)
	{
		// TODO: Improve this fast hack
		for (i=0;i<rows;i++)
			Y[i] = text(i,ycol).toDouble();
	}
	return Y;
}

bool Table::isEmptyColumn(int col)
{
	OBSOLETE
		return d_model->output(col)->rowCount() == 0;
}

QStringList Table::YColumns()
{
	OBSOLETE
		QStringList list;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if (plotDesignation(i) == SciDAVis::Y)
			list << columnLabel(i);

	return list;
}

void Table::updateDecimalSeparators(const QLocale& oldSeparators)
{
	OBSOLETE
		d_table_view->update();
}

void Table::updateDecimalSeparators()
{
	OBSOLETE
		d_table_view->update();
}

void Table::importMultipleASCIIFiles(const QString &fname, const QString &sep, int ignoredLines,
		bool renameCols, bool stripSpaces, bool simplifySpaces,
		int importFileAs)
{
	OBSOLETE
		// TODO
}

void Table::importASCII(const QString &fname, const QString &sep, int ignoredLines,
		bool renameCols, bool stripSpaces, bool simplifySpaces, bool newTable)
{
	OBSOLETE
		// TODO
}

bool Table::exportASCII(const QString& fname, const QString& separator,
		bool withLabels,bool exportSelection)
{
	OBSOLETE
		// TODO
		return false;
}

bool Table::calculate(int col, int startRow, int endRow)
{
	OBSOLETE
		QApplication::setOverrideCursor(Qt::WaitCursor);

	AbstractScript *colscript =  d_scripting_engine->newScript(d_model->output(col)->formula(0), this,  QString("<%1>").arg(colName(col)));
	connect(colscript, SIGNAL(error(const QString&,const QString&,int)), d_scripting_engine, SIGNAL(error(const QString&,const QString&,int)));
	connect(colscript, SIGNAL(print(const QString&)), d_scripting_engine, SIGNAL(print(const QString&)));

	if (!colscript->compile())
	{
		QApplication::restoreOverrideCursor();
		return false;
	}
	if (endRow >= rowCount())
		setRowCount(endRow + 1);

	colscript->setInt(col+1, "j");
	colscript->setInt(startRow+1, "sr");
	colscript->setInt(endRow+1, "er");
	QVariant ret;
	for (int i=startRow; i<=endRow; i++)
	{
		colscript->setInt(i+1,"i");
		ret = colscript->eval();
		if(ret.type()==QVariant::Double) {
			int prec;
			char f;
			columnNumericFormat(col, &f, &prec);
			setText(i, col, QLocale().toString(ret.toDouble(), f, prec));
		} else if(ret.canConvert(QVariant::String))
			setText(i, col, ret.toString());
		else {
			QApplication::restoreOverrideCursor();
			return false;
		}
	}

	emit modifiedData(this, colName(col));
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
	return true;
}

bool Table::calculate()
{
	OBSOLETE
		bool success = true;
	if(selectedColumnCount() < 1) return false;
	for (int col = firstSelectedColumn(); col<=lastSelectedColumn(); col++)
		if (!calculate(col, firstSelectedRow(), lastSelectedRow()))
			success = false;
	return success;
}

void Table::sortTableDialog()
{
	OBSOLETE
		// TODO: merge sortTableDialog and sortColumnsDialog
		SortDialog *sortd = new SortDialog(this);
	sortd->setAttribute(Qt::WA_DeleteOnClose);
	connect (sortd, SIGNAL(sort(int, int, const QString&)), this, SLOT(sort(int, int, const QString&)));
	sortd->insertColumnsList(colNames());
	sortd->exec();
}

void Table::sortColumnsDialog()
{
	OBSOLETE
		SortDialog *sortd = new SortDialog(this);
	sortd->setAttribute(Qt::WA_DeleteOnClose);
	connect (sortd, SIGNAL(sort(int, int, const QString&)), this, SLOT(sortColumns(int, int, const QString&)));
	sortd->insertColumnsList(selectedColumnsOld());
	sortd->exec();
}

void Table::normalizeSelection()
{
	OBSOLETE
		// TODO: normalization should be done by a filter
		if(selectedColumnCount() < 1) return;
	for (int i=firstSelectedColumn(); i<=lastSelectedColumn(); i++)
		normalizeCol(i);
	emit modifiedWindow(this);
}

void Table::normalize()
{
	OBSOLETE
		// TODO: normalization should be done by a filter
		for (int i=0; i<columnCount(); i++)
			normalizeCol(i);
	emit modifiedWindow(this);
}

void Table::normalizeCol(int col)
{
	OBSOLETE
		if (col<0) 
		{
			col = firstSelectedColumn();
			if(selectedColumnCount() < 1) return;
		}
	double max = text(0,col).toDouble();
	double aux = 0.0;
	int rows = rowCount();
	for (int i=0; i<rows; i++)
	{
		QString the_text = text(i,col);
		aux = the_text.toDouble();
		if (!the_text.isEmpty() && fabs(aux)>fabs(max))
			max=aux;
	}

	if (max == 1.0)
		return;

	int prec;
	char f;
	columnNumericFormat(col, &f, &prec);

	for (int i=0; i<rows; i++)
	{
		QString the_text = text(i, col);
		aux = the_text.toDouble();
		if ( !the_text.isEmpty() )
			setText(i, col, QLocale().toString(aux/max, f, prec));
	}

	QString name=colName(col);
	emit modifiedData(this, name);
}


QStringList Table::drawableColumnSelection()
{
	OBSOLETE
		QStringList names;
	for (int i=0; i<columnCount(); i++)
	{
		if(isColumnSelected(i) && plotDesignation(i) == SciDAVis::Y)
			names << QString(name()) + "_" + columnLabel(i);
	}

	for (int i=0; i<columnCount(); i++)
	{
		if(isColumnSelected(i) && (plotDesignation(i) == SciDAVis::yErr || plotDesignation(i) == SciDAVis::xErr))
			names << QString(name()) + "_" + columnLabel(i);
	}
	return names;
}

void Table::clearSelection()
{
	OBSOLETE
		// TODO
}

void Table::copySelection()
{
	OBSOLETE
		// TODO
}

void Table::cutSelection()
{
	OBSOLETE
		// TODO
}

void Table::pasteSelection()
{
	OBSOLETE
		// TODO
}


void Table::loadHeader(QStringList header)
{
	OBSOLETE
		QStringList col_label;
	QList<SciDAVis::PlotDesignation> col_plot_type;
	for (int i=0; i<header.count();i++)
	{
		if (header.at(i).isEmpty())
			continue;

		QString s = header.at(i);
		s.replace("_","-");
		if (s.contains("[X]"))
		{
			col_label << s.remove("[X]");
			col_plot_type << SciDAVis::X;
		}
		else if (s.contains("[Y]"))
		{
			col_label << s.remove("[Y]");
			col_plot_type << SciDAVis::Y;
		}
		else if (s.contains("[Z]"))
		{
			col_label << s.remove("[Z]");
			col_plot_type << SciDAVis::Z;
		}
		else if (s.contains("[xEr]"))
		{
			col_label << s.remove("[xEr]");
			col_plot_type << SciDAVis::xErr;
		}
		else if (s.contains("[yEr]"))
		{
			col_label << s.remove("[yEr]");
			col_plot_type << SciDAVis::yErr;
		}
		else
		{
			col_label << s;
			col_plot_type << SciDAVis::noDesignation;
		}
	}
	setHeader(col_label);
	for (int i=0; i<col_plot_type.count();i++)
	{
		setColPlotDesignation(i, col_plot_type.at(i));
	}
}

void Table::setColWidths(const QStringList& widths)
{
	OBSOLETE
		for (int i=0;i<(int)widths.count();i++)
			d_table_view->setColumnWidth(i, widths[i].toInt() );
}

void Table::setCommands(const QStringList& com)
{
	OBSOLETE
		for(int i=0; i<com.size() && i<columnCount(); i++)
			undoStack()->push(new TableSetFormulaCmd(d_model, i, 
						Interval<int>(0, d_model->output(i)->rowCount()-1), com.at(i).trimmed()));
}

void Table::setCommands(const QString& com)
{
	OBSOLETE
		QStringList lst = com.split("\t");
	lst.pop_front();
	setCommands(lst);
}

void Table::setColumnTypes(const QStringList& ctl)
{
	OBSOLETE
		int n = qMin(ctl.count(), columnCount());

	for (int i=0; i<n; i++)
	{
		QStringList l = ctl[i].split(";");
		setColumnMode(i, (SciDAVis::ColumnMode)l[0].toInt());
	}
}

void Table::setCell(int row, int col, double val)
{
	OBSOLETE
		char format;
	int prec;
	columnNumericFormat(col, &format, &prec);
	setText(row, col, QLocale().toString(val, format, prec));
}


void Table::setColComments(const QStringList& lst)
{
	OBSOLETE
		for(int i=0; i<lst.size() && i<columnCount(); i++)
			undoStack()->push(new TableSetColumnCommentCmd(d_model, i, lst.at(i)));
}

void Table::setPlotDesignation(SciDAVis::PlotDesignation pd)
{
	OBSOLETE
		for(int i=0;i<columnCount(); i++)
			if(isColumnSelected(i, false))
				undoStack()->push(new TableSetColumnPlotDesignationCmd(d_model, i, pd));
}


#endif

