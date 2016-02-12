/***************************************************************************
    File                 : tablecommands.cpp
    Project              : SciDAVis
    Description          : Commands used in Table (part of the undo/redo framework)
    --------------------------------------------------------------------
    Copyright            : (C) 2007 Tilman Benkert (thzs*gmx.net)
                           (replace * with @ in the email addresses) 
                           
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

#include "table/tablecommands.h"
#include "table/future_Table.h"
#include "core/column/Column.h"
#include "lib/Interval.h"
#include "core/datatypes/Double2StringFilter.h"
#include <QObject>
#include <QtDebug>

///////////////////////////////////////////////////////////////////////////
// class TableInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////
TableInsertColumnsCmd::TableInsertColumnsCmd( future::Table::Private * private_obj, int before, QList<Column*> cols, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_before(before), d_cols(cols)
{
	setText(QObject::tr("%1: insert %2 column(s)").arg(d_private_obj->name()).arg(d_cols.size()));
}

TableInsertColumnsCmd::~TableInsertColumnsCmd()
{
}

void TableInsertColumnsCmd::redo()
{
	d_rows_before = d_private_obj->rowCount();
	d_private_obj->insertColumns(d_before, d_cols);
}

void TableInsertColumnsCmd::undo()
{
	d_private_obj->removeColumns(d_before, d_cols.size());
	d_private_obj->setRowCount(d_rows_before);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetNumberOfRowsCmd
///////////////////////////////////////////////////////////////////////////
TableSetNumberOfRowsCmd::TableSetNumberOfRowsCmd( future::Table::Private * private_obj, int rows, QUndoCommand * parent )
 : QUndoCommand( parent ), d_private_obj(private_obj), d_rows(rows)
{
	setText(QObject::tr("%1: set the number of rows to %2").arg(d_private_obj->name()).arg(rows));
}

TableSetNumberOfRowsCmd::~TableSetNumberOfRowsCmd()
{
}

void TableSetNumberOfRowsCmd::redo()
{
	d_old_rows = d_private_obj->rowCount();
	d_private_obj->setRowCount(d_rows);
}

void TableSetNumberOfRowsCmd::undo()
{
	d_private_obj->setRowCount(d_old_rows);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableSetNumberOfRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////
TableRemoveColumnsCmd::TableRemoveColumnsCmd( future::Table::Private * private_obj, int first, int count, QList<Column*> cols, QUndoCommand * parent )
 : QUndoCommand( parent ), d_private_obj(private_obj), d_first(first), d_count(count), d_old_cols(cols)
{
	setText(QObject::tr("%1: remove %2 column(s)").arg(d_private_obj->name()).arg(count));
}

TableRemoveColumnsCmd::~TableRemoveColumnsCmd()
{
}

void TableRemoveColumnsCmd::redo()
{
	d_private_obj->removeColumns(d_first, d_count);
}

void TableRemoveColumnsCmd::undo()
{
	d_private_obj->insertColumns(d_first, d_old_cols);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableMoveColumnCmd
///////////////////////////////////////////////////////////////////////////
TableMoveColumnCmd::TableMoveColumnCmd( future::Table::Private * private_obj, int from, int to, QUndoCommand * parent )
 : QUndoCommand( parent ), d_private_obj(private_obj), d_from(from), d_to(to)
{
	setText(QObject::tr("%1: move column %2 from position %3 to %4")
			.arg(d_private_obj->name())
			.arg(d_private_obj->column(from)->name())
			.arg(d_from+1).arg(d_to+1));
}

TableMoveColumnCmd::~TableMoveColumnCmd()
{
}

void TableMoveColumnCmd::redo()
{
	d_private_obj->moveColumn(d_from, d_to);
}

void TableMoveColumnCmd::undo()
{
	d_private_obj->moveColumn(d_to, d_from);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableMoveColumnCmd
///////////////////////////////////////////////////////////////////////////

