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
#include <QTabWidget>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox> 
#include <QScrollArea>
#include "ui_optionstabs.h"

class TableModel;
class TableItemDelegate;

//! Helper class for TableView
class TableViewWidget : public QTableView
{
    Q_OBJECT

	public:
		//! Constructor
		TableViewWidget(QWidget * parent = 0) : QTableView(parent) {};

	protected:
		//! Overloaded function (cf. Qt documentation)
		virtual void keyPressEvent( QKeyEvent * event );

signals:
		void advanceCell();

		protected slots:
			//! Cause a repaint of the header
			void updateHeaderGeometry(Qt::Orientation o, int first, int last);
		public slots:
			void selectAll();
};

//! View class for table data
class TableView : public QWidget
{
    Q_OBJECT

	public:
		//! Constructor
		TableView(QWidget * parent, TableModel * model );
		//! Destructor
		virtual ~TableView();
		bool eventFilter(QObject *object, QEvent *e);
		void setSelectionModel(QItemSelectionModel * selectionModel) { d_view->setSelectionModel(selectionModel); }
		TableModel * model() { return d_model; }
		bool isOptionTabBarVisible() { return d_tool_box->isVisible(); }

	public slots:
		void scrollToIndex(const QModelIndex & index);
		void selectAll();
		void toggleOptionTabBar();

	signals:
		void requestContextMenu(TableView * view, const QPoint& pos);
		void requestColumnContextMenu(TableView * view, const QPoint& pos);
		void requestRowContextMenu(TableView * view, const QPoint& pos);
		//! Request resize command
		/**
		 *	Emit this signal to request the owner of the view's
		 *	model to apply a resize command to its model (change 
		 *	the number of rows). This gives the
		 *	owner the chance to do the resize in an undo
		 *	aware way. If the signal is ignored, the model
		 *	will be resized by the view.
		 */
		void requestResize(int new_rows);

	protected slots:
		//! Advance current cell after [Return] or [Enter] was pressed
		void advanceCell();
		void emitContextMenuRequest(const QPoint& pos);

	protected:
		//! Pointer to the item delegate
		TableItemDelegate * d_delegate;
		//! Pointer to the current underlying model
		TableModel * d_model;

		virtual void changeEvent(QEvent * event);
		void retranslateStrings();


	private:
		//! UI with options tabs (description, format, formula etc.)
		Ui::OptionsTabs ui;
		//! The table view (first part of the UI)
		TableViewWidget * d_view;
		//! The second part of the UI containing #d_tool_box and #d_hide_button
		QWidget * d_options_bar;
		//! Scroll area containing the option tabs widget
		QScrollArea * d_tool_box;
		//! Widget that contains the options tabs UI from #ui
		QWidget * d_options_tabs;
		//! Button to toogle the visibility of #d_tool_box
		QToolButton * d_hide_button;
		QVBoxLayout * d_main_layout;
		QVBoxLayout * d_sub_layout;

		//! Initialization common to all ctors
		void init(TableModel * model);

};


#endif
