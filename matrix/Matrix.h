/***************************************************************************
    File                 : Matrix.h
    Project              : SciDAVis
    Description          : Aspect providing a spreadsheet to manage MxN matrix data
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2008 Tilman Hoener zu Siederdissen (thzs*gmx.net)
    Copyright            : (C) 2006-2008 Knut Franke (knut.franke*gmx.de)
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
#ifndef MATRIX_H
#define MATRIX_H

#include "core/AbstractScriptingEngine.h"
#include "core/AbstractPart.h"
#include "MatrixView.h"
#include "lib/macros.h"

class QContextMenuEvent;
class QEvent;
class ActionManager;

// (maximum) initial matrix size (this is the size of the window, not the number of cells)
#define _Matrix_initial_rows_ 10
#define _Matrix_initial_columns_ 3

// TODO: move all selection related stuff to the primary view

//! Aspect providing a spreadsheet to manage MxN matrix data
class Matrix : public AbstractPart, public scripted
{
    Q_OBJECT

	public:
		class Private; 
		friend class Private;

		/*!
		 * \brief Constructor
		 *
		 * \param engine scripting engine
		 * \param rows initial number of rows
		 * \param cols initial number of columns
		 * \param name object name
		 */
		Matrix(AbstractScriptingEngine *engine, int rows, int cols, const QString& name);
		~Matrix();

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
		QMenu * createMatrixMenu(QMenu * append_to = 0);
		//! Fill the part specific menu for the main window including setting the title
		/**
		 * \return true on success, otherwise false (e.g. part has no actions).
		 */
		virtual bool fillProjectMenu(QMenu * menu);

		void insertColumns(int before, int count);
		void appendColumns(int count) { insertColumns(columnCount(), count); }
		void removeColumns(int first, int count);
		void insertRows(int before, int count);
		void appendRows(int count) { insertRows(rowCount(), count); }
		void removeRows(int first, int count);
		//! Set the number of rows and columns
		void setDimensions(int rows, int cols);
		//! Return the total number of columns
		int columnCount() const;
		//! Return the total number of rows
		int rowCount() const;

		//! Set a plot menu 
		/**
		 * The matrix takes ownership of the menu.
		 */
		void setPlotMenu(QMenu * menu);
		//! Return the value in the given cell
		double cell(int row, int col);
		//! Set the value of the cell
		void setCell(int row, int col, double value );
		//! Return the text displayed in the given cell
		QString text(int row, int col);
		void copy(Matrix * other);
		double xStart();
		double yStart();
		double xEnd();
		double yEnd();
		void setXStart(double x);
		void setXEnd(double x);
		void setYStart(double y);
		void setYEnd(double y);
		void setCoordinates(double x1, double x2, double y1, double y2);
		char numericFormat() const;
		int displayedDigits()  const;
		void setNumericFormat(char format);
		void setDisplayedDigits(int digits);

	public:
		static ActionManager * actionManager();
		static void initActionManager();
	private:
		static ActionManager * action_manager;
		//! Private ctor for initActionManager() only
		Matrix();

	public slots:
		//! Clear the whole matrix (i.e. set all cells to 0.0)
		void clear();

		void cutSelection();
		void copySelection();
		void pasteIntoSelection();
		void clearSelectedCells();
		void dimensionsDialog();
		void goToCell();
		//! Insert columns depending on the selection
		void insertEmptyColumns();
		//! Insert rows depending on the selection
		void insertEmptyRows();
		void removeSelectedColumns();
		void removeSelectedRows();
		void clearSelectedColumns();
		void clearSelectedRows();
		void selectAll();
		//! Show a context menu for the selected cells
		/**
		 * \param pos global position of the event 
		*/
		void showMatrixViewContextMenu(const QPoint& pos);
		//! Show a context menu for the selected columns
		/**
		 * \param pos global position of the event 
		*/
		void showMatrixViewColumnContextMenu(const QPoint& pos);
		//! Show a context menu for the selected rows
		/**
		 * \param pos global position of the event 
		*/
		void showMatrixViewRowContextMenu(const QPoint& pos);
		void editFormat();
		void editCoordinates();
		void editFormula();
		//! Append as many columns as are selected
		void addColumns();
		//! Append as many rows as are selected
		void addRows();

	signals:
		void columnsAboutToBeInserted(int before, int count);
		void columnsInserted(int first, int count);
		void columnsAboutToBeRemoved(int first, int count);
		void columnsRemoved(int first, int count);
		void rowsAboutToBeInserted(int before, int count);
		void rowsInserted(int first, int count);
		void rowsAboutToBeRemoved(int first, int count);
		void rowsRemoved(int first, int count);
		void dataChanged(int top, int left, int bottom, int right);

	private:
		void createActions();
		void connectActions();
		QMenu * d_plot_menu;

		//! \name selection related actions
		//@{
		QAction * action_cut_selection;
		QAction * action_copy_selection;
		QAction * action_paste_into_selection;
		QAction * action_clear_selection;
		//@}
		//! \name matrix related actions
		//@{
		QAction * action_toggle_tabbar;
		QAction * action_select_all;
		QAction * action_clear_matrix;
		QAction * action_go_to_cell;
		QAction * action_dimensions_dialog;
		QAction * action_edit_format;
		QAction * action_edit_coordinates;
		QAction * action_set_formula;
		QAction * action_recalculate;
		//@}
		//! \name column related actions
		//@{
		QAction * action_insert_columns;
		QAction * action_remove_columns;
		QAction * action_clear_columns;
		QAction * action_add_columns;
		//@}
		//! \name row related actions
		//@{
		QAction * action_insert_rows;
		QAction * action_remove_rows;
		QAction * action_clear_rows;
		QAction * action_add_rows;
		//@}

		MatrixView *d_view;
		Private *d_matrix_private;
};

/**
  This private class manages matrix based data (i.e., mathematically
  a MxN matrix with M rows, N columns). These data are typically
  used to for 3D plots.
  
  The API of this private class is to be called by Matrix and matrix
  commands only. Matrix may only call the reading functions to ensure 
  that undo/redo is possible for all data changing operations.

  The values of the matrix are stored as double precision values. They
  are managed by QVector<double> objects. Although rows and columns
  are equally important in a matrix, the columns are chosen to
  be contiguous in memory to allow easier copying between
  column and matrix data.
  */
class Matrix::Private
{
	public:
		Private(Matrix *owner); 
		//! Insert columns before column number 'before'
		/**
		 * If 'first' is equal to the current number of columns,
		 * the columns will be appended.
		 * \param before index of the column to insert before
		 * \param count the number of columns to be inserted
		 */
		void insertColumns(int before, int count);
		//! Remove Columns
		/**
		 * \param first index of the first column to be removed
		 * \param count number of columns to remove
		 */
		void removeColumns(int first, int count);
		//! Insert rows before row number 'before'
		/**
		 * If 'first' is equal to the current number of rows,
		 * the rows will be appended.
		 * \param before index of the row to insert before
		 * \param count the number of rows to be inserted
		 */
		void insertRows(int before, int count);
		//! Remove Columns
		/**
		 * \param first index of the first row to be removed
		 * \param count number of rows to remove
		 */
		void removeRows(int first, int count);
		//! Return the number of columns in the table
		int columnCount() const { return d_column_count; }
		//! Return the number of rows in the table
		int rowCount() const { return d_row_count; }
		QString name() const { return d_owner->name(); }
		//! Return the value in the given cell
		double cell(int row, int col);
		//! Set the value in the given cell
		void setCell(int row, int col, double value);
		//! Return the values in the given cells as double vector
		QVector<double> columnCells(int col, int first_row, int last_row);
		//! Return the values in the given cells as double vector
		void setColumnCells(int col, int first_row, int last_row, QVector<double> values);
		char numericFormat() const { return d_numeric_format; }
		void setNumericFormat(char format) { d_numeric_format = format; }
		int displayedDigits()  const { return d_displayed_digits; }
		void setDisplayedDigits(int digits) { d_displayed_digits = digits; }
		//! Fill column with zeroes
		void clearColumn(int col);
		BASIC_ACCESSOR(double, d_x_start, xStart, XStart);
		BASIC_ACCESSOR(double, d_y_start, yStart, YStart);
		BASIC_ACCESSOR(double, d_x_end, xEnd, XEnd);
		BASIC_ACCESSOR(double, d_y_end, yEnd, YEnd);


		// TODO: signals for format change etc.
		
	private:
		//! The owner aspect
		Matrix *d_owner;
		//! The number of columns
		int d_column_count;
		//! The number of rows
		int d_row_count;
		//! The matrix data
		QVector< QVector<double> > d_data;	
		//! Last formula used to calculate cell values
		QString d_formula; // TODO: should we support interval/rectangle based formulas?
		//! Format code for displaying numbers
		char d_numeric_format;
		//! Number of significant digits
		int d_displayed_digits;
		double d_x_start, //!< X value corresponding to column 1
			   d_x_end,  //!< X value corresponding to the last column
			   d_y_start,  //!< Y value corresponding to row 1
			   d_y_end;  //!< Y value corresponding to the last row

};

#endif
