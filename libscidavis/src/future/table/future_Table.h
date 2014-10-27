/***************************************************************************
    File                 : Table.h
    Project              : SciDAVis
    Description          : Aspect providing a spreadsheet table with column logic
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2006-2009 Knut Franke (knut.franke*gmx.de)
    Copyright            : (C) 2006-2007 Ion Vasilief (ion_vasilief*yahoo.fr)
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
#ifndef FUTURE_TABLE_H
#define FUTURE_TABLE_H

#include "core/AbstractPart.h"
#ifndef LEGACY_CODE_0_2_x
#include "AbstractScriptingEngine.h"
#endif
#include "globals.h"
#include <QList>
#include <QStringList>

class TableView;
class QUndoStack;
class QMenu;
class Column;
class QPoint;
class QAction;
class AbstractColumn;
class ActionManager;
class TableStatistics;
class TeXTableSettings;

namespace future
{

/*!\brief Aspect providing a spreadsheet table with column logic.
 *
This class (incl. Table::Private and its commands) is one aspect in the projet hierarchy
that represents a spreadsheet table with column logic. Table provides the public API while
Table::Private completely encapsulates the data. The table commands (derived from QUndoCommand) 
encapsulate all write operations which can be undone and redone, if the table has an undo stack 
associated with it (usually by the project root aspect).

The underlying private data object is not visible to any classes other then those meantioned
above with one exeption:
Pointers to columns can be passed around an manipulated directly. The owner Table (parent aspect
of the Column objects) will be notified by emission of signals and react accordingly. 
All public methods of Table and Column are undo aware. 

Table also manages its main view of class TableView. Table and TableView can call each others
API in both directions. User interaction ist party handled in TableView and translated into 
Table API calls (e.g., when a user edits a cell this will be handled by the delegate of
TableView and Table will not know whether a script or a user changed the data.). Other parts 
of the user interaction are handled by actions provides by Table, e.g., via a context menu.

Selections are handled by TableView and can be queried by Table. All selection based functions
do nothing unless the view exists. The view is created by the first call to view();
*/
#ifndef LEGACY_CODE_0_2_x
class Table : public AbstractPart, public scripted
#else
class Table : public AbstractPart
#endif
{
	Q_OBJECT

	public:
		class Private; // This could also be private, but then all commands need to be friend classes
		friend class Private;

#ifndef LEGACY_CODE_0_2_x
		Table(AbstractScriptingEngine *engine, int rows, int columns, const QString &name);
#else
		Table(void *engine, int rows, int columns, const QString &name);
		void setView(TableView * view);
		friend class ::TableStatistics;
#endif
		virtual ~Table();

		//! Return an icon to be used for decorating my views.
		virtual QIcon icon() const;
		//! Return a new context menu.
		/**
		 * The caller takes ownership of the menu.
		 */
		virtual QMenu *createContextMenu() const;
		//! Construct a primary view on me.
		/**
		 * This method may be called multiple times during the life time of an Aspect, or it might not get
		 * called at all. Aspects must not depend on the existence of a view for their operation.
		 */
		virtual QWidget *view();
		
		//! Insert columns
		/**
		 * Ownership of the columns is transferred to this Table.
		 *
		 * If before == columnCount() this will do the same as appendColumns();
		 */
		void insertColumns(int before, QList<Column *> new_cols);
		//! Append columns
		/*
		 * Convenience function, same as:
		 * <code>
		 * insertColumns(columnCount(), new_cols);
		 * </code>
		 */
		void appendColumns(QList<Column*> new_cols) { insertColumns(columnCount(), new_cols); }
		void removeColumns(int first, int count);
		void removeColumn(Column * col);
		void removeRows(int first, int count);
		void insertRows(int before, int count);
		void appendRows(int count) { insertRows(rowCount(), count); }
		//! Set the number of rows of the table
		void setRowCount(int new_size);
		//! Return the total number of columns in the table
		int columnCount() const;
		//! Return the total number of rows in the table
		int rowCount() const;
		//! Return the number of columns matching the given designation
		int columnCount(SciDAVis::PlotDesignation pd) const;
		//! Return column number 'index'
		Column* column(int index) const;
		//! Return the column determined by the given name
		/**
		 * This method should not be used unless absolutely necessary. 
		 * Columns should be addressed by their index. 
		 * This method is mainly meant to be used in scripts.
		 */
		Column* column(const QString & name, bool legacy_kludge=true) const;
		int columnIndex(const Column * col) const;
		//! Set the number of columns
		void setColumnCount(int new_size);
		QVariant headerData(int section, Qt::Orientation orientation,int role) const;

		//! Create a menu with selection related operations
		/**
		 * \param append_to if a pointer to a QMenu is passed
		 * to the function, the actions are appended to
		 * it instead of the creation of a new menu.
		 */
		QMenu * createSelectionMenu(QMenu * append_to = 0);
		//! Create a menu with column related operations
		/**
		 * \param append_to if a pointer to a QMenu is passed
		 * to the function, the actions are appended to
		 * it instead of the creation of a new menu.
		 */
		QMenu * createColumnMenu(QMenu * append_to = 0);
		//! Create a menu with row related operations
		/**
		 * \param append_to if a pointer to a QMenu is passed
		 * to the function, the actions are appended to
		 * it instead of the creation of a new menu.
		 */
		QMenu * createRowMenu(QMenu * append_to = 0);
		//! Create a menu with table related operations
		/**
		 * \param append_to if a pointer to a QMenu is passed
		 * to the function, the actions are appended to
		 * it instead of the creation of a new menu.
		 */
		QMenu * createTableMenu(QMenu * append_to = 0);
		//! Fill the part specific menu for the main window including setting the title
		/**
		 * \return true on success, otherwise false (e.g. part has no actions).
		 */
		virtual bool fillProjectMenu(QMenu * menu);
		//! Fill the part specific tool bar for the main window including setting the title
		/**
		 * \return true on success, otherwise false (e.g. part has no actions to be shown in a toolbar).
		 */
		virtual bool fillProjectToolBar(QToolBar * bar);

		//! Determine the corresponding X column
		int colX(int col);
		//! Determine the corresponding Y column
		int colY(int col);
		//! Set a plot menu 
		/**
		 * The table takes ownership of the menu.
		 */
		void setPlotMenu(QMenu * menu);
		//! Open the sort dialog for the given columns
		void sortDialog(QList<Column*> cols);
		//! Set default for comment visibility for table views
		static void setDefaultCommentVisibility(bool visible) { d_default_comment_visibility = visible; }
		//! Return the default for comment visibility for table views
		static bool defaultCommentVisibility() { return d_default_comment_visibility; }
		//! Return the text displayed in the given cell
		QString text(int row, int col);
		void setSelectionAs(SciDAVis::PlotDesignation pd);
		void copy(Table * other);

		//! \name serialize/deserialize
		//@{
		//! Save as XML
		virtual void save(QXmlStreamWriter *) const;
		//! Load from XML
		virtual bool load(XmlStreamReader *);
		bool readColumnWidthElement(XmlStreamReader * reader);
		//@}
		
	public:
		static ActionManager * actionManager();
		static void initActionManager();
		static int defaultColumnWidth() { return default_column_width; }
		static void setDefaultColumnWidth(int width) { default_column_width = width; }
	private:
		static ActionManager * action_manager;
		// TODO: the default sizes are to be controlled by the global Table settings
		static int default_column_width;
		//! Private ctor for initActionManager() only
		Table();

	public slots:
		//! Clear the whole table
		void clear();
#ifndef LEGACY_CODE_0_2_x
		//! Clear all mask in the table
		void clearMasks();
#endif

		//! Append one column
		void addColumn();
		//! Append as many columns as are selected
		void addColumns();
		//! Append as many rows as are selected
		void addRows();
		void cutSelection();
		void copySelection();
		void pasteIntoSelection();
		void clearSelectedCells();
		void goToCell();
#ifndef LEGACY_CODE_0_2_x
		void maskSelection();
		void unmaskSelection();
#endif
		void setFormulaForSelection();
		void recalculateSelectedCells();
		void fillSelectedCellsWithRowNumbers();
		void fillSelectedCellsWithRandomNumbers();
		//! Open the sort dialog for all columns
		void sortTable();
		//! Insert columns depending on the selection
		void insertEmptyColumns();
		void removeSelectedColumns();
		void clearSelectedColumns();
		void setSelectedColumnsAsX();
		void setSelectedColumnsAsY();
		void setSelectedColumnsAsZ();
		void setSelectedColumnsAsXError();
		void setSelectedColumnsAsYError();
		void setSelectedColumnsAsNone();
		void normalizeColumns(QList< Column* > cols);
		void normalizeSelectedColumns();
		void normalizeSelection();
		void sortSelectedColumns();
		void statisticsOnSelectedColumns();
		void statisticsOnSelectedRows();
		//! Insert rows depending on the selection
		void insertEmptyRows();
		void removeSelectedRows();
		void selectAll();
		void dimensionsDialog();
		void editTypeAndFormatOfSelectedColumns();
		void editDescriptionOfCurrentColumn();
		void moveColumn(int from, int to);
		//! Sort the given list of column
		/*
		 * If 'leading' is a null pointer, each column is sorted separately.
		 */
		void sortColumns(Column * leading, QList<Column*> cols, bool ascending);
		//! Show a context menu for the selected cells
		/**
		 * \param pos global position of the event 
		*/
		void showTableViewContextMenu(const QPoint& pos);
		//! Show a context menu for the selected columns
		/**
		 * \param pos global position of the event 
		*/
		void showTableViewColumnContextMenu(const QPoint& pos);
		//! Show a context menu for the selected rows
		/**
		 * \param pos global position of the event 
		*/
		void showTableViewRowContextMenu(const QPoint& pos);

		//! Open export to TeX dialog 
                void showTeXTableExportDialog();
               
                //! Export to TeX
                /*
                 * \param file name
                 * \param TeX tale settings
                */  
                bool export_to_TeX( QString fileName, TeXTableSettings& tex_settings );

                 

	protected:
		//! Called after a new child has been inserted or added.
		/**
		 * Unlike the aspectAdded() signals, this method does not get called inside undo/redo actions;
		 * allowing subclasses to execute undo commands of their own.
		 */
		virtual void completeAspectInsertion(AbstractAspect * aspect, int index);
		//! Called before a child is removed.
		/**
		 * Unlike the aspectAboutToBeRemoved() signals, this method does not get called inside undo/redo actions;
		 * allowing subclasses to execute undo commands of their own.
		 */
		virtual void prepareAspectRemoval(AbstractAspect * aspect);

	public:
		//! This method should only be called by the view.
		/** This method does not change the view, it only changes the
		 * values that are saved when the table is saved. The view
		 * has to take care of reading and applying these values */
		void setColumnWidth(int col, int width);
		int columnWidth(int col) const;

	private:
		//! Internal function to connect all column signals
		void connectColumn(const Column* col);
		//! Internal function to disconnect a column
		void disconnectColumn(const Column* col);

	private slots:
		//! \name Column event handlers
		//@{
		void handleDescriptionChange(const AbstractAspect * aspect);
		void handleModeChange(const AbstractColumn * col);
		void handlePlotDesignationChange(const AbstractColumn * col);
		void handleDataChange(const AbstractColumn * col);
		void handleRowsAboutToBeInserted(const AbstractColumn * col, int before, int count);
		void handleRowsInserted(const AbstractColumn * col, int before, int count);
		void handleRowsAboutToBeRemoved(const AbstractColumn * col, int first, int count);
		void handleRowsRemoved(const AbstractColumn * col, int first, int count);
		//@}
		void adjustActionNames();

	signals:
		void columnsAboutToBeInserted(int before, QList<Column*> new_cols);
		void columnsInserted(int first, int count);
		void columnsAboutToBeReplaced(int first, int count);
		void columnsReplaced(int first, int count);
		void columnsAboutToBeRemoved(int first, int count);
		void columnsRemoved(int first, int count);
		void rowsAboutToBeInserted(int before, int count);
		void rowsInserted(int first, int count);
		void rowsAboutToBeRemoved(int first, int count);
		void rowsRemoved(int first, int count);
		void dataChanged(int top, int left, int bottom, int right);
		void headerDataChanged(Qt::Orientation orientation, int first, int last);
#ifdef LEGACY_CODE_0_2_x
		void recalculate();
		void requestRowStatistics();
		void requestColumnStatistics();
#endif

	private:
		void createActions();
		void connectActions();
		void addActionsToView();
		QMenu * d_plot_menu;
		static bool d_default_comment_visibility;

		//! \name selection related actions
		//@{
		QAction * action_cut_selection;
		QAction * action_copy_selection;
		QAction * action_paste_into_selection;
#ifndef LEGACY_CODE_0_2_x
		QAction * action_mask_selection;
		QAction * action_unmask_selection;
#endif
		QAction * action_set_formula;
		QAction * action_clear_selection;
		QAction * action_recalculate;
		QAction * action_fill_row_numbers;
		QAction * action_fill_random;
		QAction * action_normalize_selection;
		//@}
		//! \name table related actions
		//@{
		QAction * action_toggle_comments;
		QAction * action_toggle_tabbar;
		QAction * action_select_all;
		QAction * action_add_column;
		QAction * action_clear_table;
                QAction * action_export_to_TeX;
#ifndef LEGACY_CODE_0_2_x
		QAction * action_clear_masks;
#endif
		QAction * action_sort_table;
		QAction * action_go_to_cell;
		QAction * action_dimensions_dialog;
		QAction * action_formula_mode;
		//@}
		//! \name column related actions
		//@{
		QAction * action_insert_columns;
		QAction * action_remove_columns;
		QAction * action_clear_columns;
		QAction * action_add_columns;
		QAction * action_set_as_x;
		QAction * action_set_as_y;
		QAction * action_set_as_z;
		QAction * action_set_as_xerr;
		QAction * action_set_as_yerr;
		QAction * action_set_as_none;
		QAction * action_normalize_columns;
		QAction * action_sort_columns;
		QAction * action_statistics_columns;
		QAction * action_type_format;
		QAction * action_edit_description;
		//@}
		//! \name row related actions
		//@{
		QAction * action_insert_rows;
		QAction * action_remove_rows;
		QAction * action_clear_rows;
		QAction * action_add_rows;
		QAction * action_statistics_rows;
		//@}

		TableView *d_view;
		Private *d_table_private;
};

/**
  This private class manages column based data (i.e., 1D vector based 
  data such as x-values and y-values for a plot) for a Table. Its
  API is to be called by Table and table commands only. Table 
  may only call the reading functions to ensure that undo/redo
  is possible for all data changing operations.

  Each column is represented by a Column object and can be directly 
  accessed by the pointer returned by column(). Most of the column 
  manipulation is done directly to the columns. The signals of
  the columns are connected to various handlers in Table which
  acts according to all changes made to the columns.

  The Column objects are managed as child aspects by Table.

  Every column has two filters as children: An input filter that
  can convert a string (e.g., entered by the user in a cell) to
  the column's data type and an output filter that delivers
  the correct string representation to display in a table.

  The number of columns in the Table will always be equal to
  d_columns.size(). The number of rows is generally indepenent
  of the number of rows in the wrapped columns. It is however
  always adjusted to be large enough to display the longest column. 
  When columns are inserted, resized etc., the table is resized 
  automatically.
  */
class Table::Private
{
	public:
		Private(Table *owner) : d_owner(owner), d_column_count(0), d_row_count(0) {}
		//! Replace columns completely
		/**
		 * \param first the first column to be replaced
		 * \param new_cols list of the columns that replace the old ones
		 * This does not delete the replaced columns.
		 */
		void replaceColumns(int first, QList<Column*> new_cols);
		//! Insert columns before column number 'before'
		/**
		 * If 'first' is equal to the number of columns,
		 * the columns will be appended.
		 * \param before index of the column to insert before
		 * \param cols a list of column data objects
		 */
		void insertColumns(int before, QList<Column*> cols);
		//! Remove Columns
		/**
		 * This does not delete the removed columns because this
		 * must be handled by the undo/redo system.
		 * \param first index of the first column to be removed
		 * \param count number of columns to remove
		 */
		void removeColumns(int first, int count);
		//! Append columns to the table
		/**
		 * \sa insertColumns()
		 */
		void appendColumns(QList<Column*> cols);
		//! Move a column to another position
		void moveColumn(int from, int to);
		//! Return the number of columns in the table
		int columnCount() const { return d_column_count; }
		//! Return the number of rows in the table
		int rowCount() const { return d_row_count; }
		//! Set the number of rows of the table
		void setRowCount(int count);
		//! Return the full column header string
		QString columnHeader(int col);
		//! Return the number of columns with a given plot designation
		int numColsWithPD(SciDAVis::PlotDesignation pd);
		//! Return column number 'index'
		Column* column(int index) const;
		//! Return the index of the given column in the table.
		/**
		 * \return the index or -1 if the column is not in the table
		 */
		int columnIndex(const Column * col) const 
		{ 
			for(int i=0; i<d_columns.size(); i++)
				if(d_columns.at(i) == col) return i;
			return -1;
		}
		QString name() const { return d_owner->name(); }
		QVariant headerData(int section, Qt::Orientation orientation,int role) const;

		//! Update the vertical header labels
		/**
		 * This must be called whenever rows are added
		 * or removed.
		 * \param start_row first row that needs to be updated
		 */
		void updateVerticalHeader(int start_row);
		//! Update the horizontal header labels
		/**
		 * This must be called whenever columns are added or
		 * removed and when comments, labels, and column types
		 * change.
		 * \param start_col first column that needs to be updated
		 * \param end_col last column that needs to be updated
		 */
		void updateHorizontalHeader(int start_col, int end_col);
		void setColumnWidth(int col, int width) { d_column_widths[col] = width; }
		int columnWidth(int col) const { return d_column_widths.at(col); }

	private:
		//! The owner aspect
		Table *d_owner;
		//! The number of columns
		int d_column_count;
		//! The maximum number of rows of all columns
		int d_row_count;
		//! Vertical header data
		QList<int> d_vertical_header_data;
		//! Horizontal header data
		QStringList d_horizontal_header_data;
		//! List of pointers to the column data vectors
		QList<Column *> d_columns;	
		//! Internal function to put together the column header
		/**
		 * Don't use this outside updateHorizontalHeader()
		 */
		void composeColumnHeader(int col, const QString& label);
		//! Columns widths
		QList<int> d_column_widths;
};

} // namespace

#endif

