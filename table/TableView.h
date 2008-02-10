/***************************************************************************
    File                 : TableView.h
    Project              : SciDAVis
    Description          : View class for Table
    --------------------------------------------------------------------
    Copyright            : (C) 2007 Tilman Hoener zu Siederdissen (thzs*gmx.net)
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
#include <QtDebug>
#include "AspectView.h"

class Table;
class TableModel;
class TableItemDelegate;
class TableDoubleHeaderView;

//! Helper class for TableView
class TableViewWidget : public QTableView
{
    Q_OBJECT

	public:
		//! Constructor
		TableViewWidget(QWidget * parent = 0) : QTableView(parent) {};

	protected:
		//! Overloaded function (cf. Qt documentation)
		virtual void keyPressEvent(QKeyEvent * event);

	signals:
		void advanceCell();

		protected slots:
			//! Cause a repaint of the header
			void updateHeaderGeometry(Qt::Orientation o, int first, int last);
		public slots:
			void selectAll();
};

//! View class for Table
class TableView : public AspectView
{
    Q_OBJECT

	public:
		//! Constructor
		TableView(Table *table);
		//! Destructor
		virtual ~TableView();
		TableModel * model() { return d_model; }
		bool isOptionTabBarVisible() { return d_tool_box->isVisible(); }
		//! Show or hide (if on = false) the column comments
		void showComments(bool on = true);
		//! Return whether comments are show currently
		bool areCommentsShown() const;

	public slots:
		void scrollToIndex(const QModelIndex & index);
		void selectAll();
		void toggleOptionTabBar();
		void toggleComments();
		void showOptionsDescriptionTab();
		void showOptionsTypeTab();
		void showOptionsFormulaTab();

	signals:
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
		void horizontalSectionMovedHandler(int index, int from, int to);
		void updateTypeInfo();
		void updateFormatBox();
		void handleHeaderDataChanged(Qt::Orientation orientation, int first, int last);
		void currentColumnChanged(const QModelIndex & current, const QModelIndex & previous);
		void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
		void applyDescription();
		void applyType();

	protected:
		//! Pointer to the item delegate
		TableItemDelegate * d_delegate;
		//! Pointer to the current underlying model
		TableModel * d_model;

		virtual void contextMenuEvent(QContextMenuEvent *event);
		virtual void changeEvent(QEvent * event);
		void retranslateStrings();
		void setColumnForDescriptionTab(int col);

		static QRect mapToGlobal(QWidget *widget, const QRect& rect);
		static QRect mapToParent(QWidget *widget, const QRect& rect);
		QRect mapToThis(QWidget *widget, const QRect& rect);

	public:
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
		TableDoubleHeaderView * d_horizontal_header;
		Table * d_table;
		QWidget * d_main_widget;

		//! Initialization common to all ctors
		void init(TableModel * model);
		
};


#endif
