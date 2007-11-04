/***************************************************************************
    File                 : TableView.h
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


#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QMessageBox>
#include <QHeaderView>
#include <QSize>

class TableModel;
class TableItemDelegate;

//! View class for table data
class TableView : public QTableView
{
    Q_OBJECT

public:
    //! Constructor
    TableView(QWidget * parent, TableModel * model );
    //! Destructor
    virtual ~TableView();

protected:
	//! Overloaded function (cf. Qt documentation)
	virtual void keyPressEvent( QKeyEvent * event );

signals:
	void requestContextMenu(TableView * view, const QPoint& pos);

protected slots:
	//! Advance current cell after [Return] or [Enter] was pressed
	void advanceCell();
	//! Cause a repaint of the header
	void updateHeaderGeometry(Qt::Orientation o, int first, int last);
	//! Selection change handler
	void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected );
	void emitContextMenuRequest(const QPoint& pos);

protected:
	//! Pointer to the item delegate
	TableItemDelegate * d_delegate;
	//! Pointer to the current underlying model
	TableModel * d_model;

private:
	//! Initialization common to all ctors
	void init(TableModel * model);

};


#endif
