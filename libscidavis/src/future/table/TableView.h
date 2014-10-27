/***************************************************************************
    File                 : TableView.h
    Project              : SciDAVis
    Description          : View class for Table
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
#include "ui_controltabs.h"
#include <QtDebug>
#include "globals.h"
#include "MyWidget.h"
#include "lib/IntervalAttribute.h"

class Column;
namespace future{ class Table; }
class TableModel;
class TableItemDelegate;
class TableDoubleHeaderView;
class AbstractAspect;

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
class TableView : public MyWidget
{
    Q_OBJECT

	public:
		//! Constructor
#ifndef LEGACY_CODE_0_2_x
		TableView(future::Table * table);
#else
		TableView(const QString & label, QWidget * parent=0, const QString name=0, Qt::WFlags f=0);
		void setTable(future::Table * table);
#endif
		//! Destructor
		virtual ~TableView();
		bool isControlTabBarVisible() { return d_control_tabs->isVisible(); }
		//! Show or hide (if on = false) the column comments
		void showComments(bool on = true);
		//! Return whether comments are show currently
		bool areCommentsShown() const;

		//! \name selection related functions
		//@{
		//! Return how many columns are selected
		/**
		 * If full is true, this function only returns the number of fully 
		 * selected columns.
		 */
		int selectedColumnCount(bool full = false);
		//! Return how many columns with the given plot designation are (at least partly) selected
		int selectedColumnCount(SciDAVis::PlotDesignation pd);
		//! Returns true if column 'col' is selected; otherwise false
		/**
		 * If full is true, this function only returns true if the whole 
		 * column is selected.
		 */
		bool isColumnSelected(int col, bool full = false);
		//! Return all selected columns
		/**
		 * If full is true, this function only returns a column if the whole 
		 * column is selected.
		 */
		QList<Column *> selectedColumns(bool full = false);
		//! Return how many rows are (at least partly) selected
		/**
		 * If full is true, this function only returns the number of fully 
		 * selected rows.
		 */
		int selectedRowCount(bool full = false);
		//! Returns true if row 'row' is selected; otherwise false
		/**
		 * If full is true, this function only returns true if the whole 
		 * row is selected.
		 */
		bool isRowSelected(int row, bool full = false);
		//! Return the index of the first selected column
		/**
		 * If full is true, this function only looks for fully 
		 * selected columns.
		 */
		int firstSelectedColumn(bool full = false);
		//! Return the index of the last selected column
		/**
		 * If full is true, this function only looks for fully 
		 * selected columns.
		 */
		int lastSelectedColumn(bool full = false);
		//! Return the index of the first selected row
		/**
		 * If full is true, this function only looks for fully 
		 * selected rows.
		 */
		int firstSelectedRow(bool full = false);
		//! Return the index of the last selected row
		/**
		 * If full is true, this function only looks for fully 
		 * selected rows.
		 */
		int lastSelectedRow(bool full = false);
		//! Get the complete set of selected rows.
		IntervalAttribute<bool> selectedRows(bool full = false);
		//! Return whether a cell is selected
		bool isCellSelected(int row, int col);
		//! Select/Deselect a cell
		void setCellSelected(int row, int col, bool select = true);
		//! Select/Deselect a range of cells
		void setCellsSelected(int first_row, int first_col, int last_row, int last_col, bool select = true);
		//! Determine the current cell (-1 if no cell is designated as the current)
		void getCurrentCell(int * row, int * col);
		//@}

		void setColumnWidth(int col, int width);
		int columnWidth(int col) const;
		bool formulaModeActive() const;

	public slots:
		void activateFormulaMode(bool on);
		void goToCell(int row, int col);
		void rereadSectionSizes();
		void selectAll();
		void deselectAll();
		void toggleControlTabBar();
		void toggleComments();
		void showControlDescriptionTab();
		void showControlTypeTab();
		void showControlFormulaTab();
		void handleHorizontalSectionResized(int logicalIndex, int oldSize, int newSize); 
		void goToNextColumn();
		void goToPreviousColumn();

	protected slots:
		//! Advance current cell after [Return] or [Enter] was pressed
		void advanceCell();
		void handleHorizontalSectionMoved(int index, int from, int to);
		void handleHorizontalHeaderDoubleClicked(int index);
		void handleFormatLineEditChange();
		void handleAspectDescriptionChanged(const AbstractAspect * aspect);
		void handleAspectAdded(const AbstractAspect *aspect);
		void handleAspectAboutToBeRemoved(const AbstractAspect *aspect, int index);
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

		virtual void changeEvent(QEvent * event);
		void retranslateStrings();
		void setColumnForControlTabs(int col);

		bool eventFilter( QObject * watched, QEvent * event);

		//! UI with options tabs (description, format, formula etc.)
		Ui::ControlTabs ui;
		//! The table view (first part of the UI)
		TableViewWidget * d_view_widget;
		//! Widget that contains the control tabs UI from #ui
		QWidget * d_control_tabs;
		//! Button to toogle the visibility of #d_tool_box
		QToolButton * d_hide_button;
		QHBoxLayout * d_main_layout;
		TableDoubleHeaderView * d_horizontal_header;
		future::Table * d_table;

		//! Initialization
		void init();
};


#endif
