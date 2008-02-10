/***************************************************************************
    File                 : TableModel.h
    Project              : SciDAVis
    Description          : Model storing data for a table
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

#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QStringList>
#include "core/column/Column.h"
#include "core/AbstractFilter.h"
#include <QColor>

class QItemSelectionModel;
class QUndoStack;

//! Model storing data for a table
/**
  This model class provides the interface to column based data 
  (i.e. 1D vector based data like x-values and y-values for a plot).
  It contains the overloaded functions of QAbstractItemModel to be 
  called from views as well as functions for manipulating the
  wraped data using the Column class.

  Each column is represented by a Column object and can be directly 
  accessed by the pointer returned by output(). Most of the column 
  manipulation is done directly to the columns. The signals of
  the columns are connected to various handlers in TableModel which
  acts according to all changes made to the columns.

  In every column two filters are always present: An input filter that
  can convert a string (e.g. entered by the user in a cell) to
  the column's data type and an output filter that delivers
  the correct string representation to display in a table.

  The number of columns in the table will always be equal to
  d_columns.size(). The number of rows is generally indepenent
  of the number of rows in the wrapped columns. It should however
  be large enough to display the longest column. When columns
  are inserted, resized etc., the table is resized automatically.

  TableModel also implements AbstractFilter and thus acts
  as a filter with no inputs and columnCount() outputs. 
  */
class TableModel : public QAbstractItemModel, public AbstractFilter 
{
	Q_OBJECT

	public:
		//! Constructor
		explicit TableModel( QObject * parent = 0 );
		//! Destructor
		~TableModel();

		//! Custom data roles used in addition to Qt::ItemDataRole
		enum CustomDataRole {
			MaskingRole = Qt::UserRole, //!< bool determining whether the cell is masked
			FormulaRole = Qt::UserRole+1, //!< the cells formula
		};

		//! \name Overloaded functions from QAbstractItemModel
		//@{
		Qt::ItemFlags flags( const QModelIndex & index ) const;
		QVariant data(const QModelIndex &index, int role) const;
		QVariant headerData(int section, 
				Qt::Orientation orientation,int role) const;
		int rowCount(const QModelIndex &parent) const;
		int columnCount(const QModelIndex & parent) const;
		bool setData(const QModelIndex & index, const QVariant & value, int role);
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex & child) const;
		//@}

		//! \name Other functions
		//@{
		//! Overloaded from AbstractFilter
		virtual int inputCount() const { return 0; }
		//! Overloaded from AbstractFilter
		virtual int outputCount() const { return d_column_count; }
		//! Return a pointer to the column at index 'port'
		/**
		 * \return returns a pointer to the column or zero if 'port' is invalid
		 */
		virtual shared_ptr<AbstractColumn> output(int port) const;
		//! Replace columns completely
		/**
		 * TableModel takes over ownership of the new columns.
		 * \param first the first column to be replaced
		 * \param new_cols list of the columns that replace the old ones
		 * This does not delete the replaced columns.
		 */
		void replaceColumns(int first, QList< shared_ptr<Column> > new_cols);
		//! Insert columns before column number 'before'
		/**
		 * If 'first' is higher than (current number of columns -1),
		 * the columns will be appended.
		 * TableModel takes over ownership of the columns. 
		 * \param before index of the column to insert before
		 * \param cols a list of column data objects
		 * \param in_filter a list of the corresponding input filters
		 * \param out_filter a list of the corresponding output filters
		 */
		void insertColumns(int before, QList< shared_ptr<Column> > cols);
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
		 * TableModel takes over ownership of the column.
		 *
		 * \sa insertColumns()
		 */
		void appendColumns(QList< shared_ptr<Column> > cols);
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
		//! This must be called whenever columns were changed directly
		/**
		 * \param top first modified row
		 * \param left first modified column
		 * \param bottom last modified row
		 * \param right last modified column
		 */
		void emitDataChanged(int top, int left, int bottom, int right);
		void emitColumnChanged(Column * col);
		void emitColumnChanged(shared_ptr<Column> col) { emitColumnChanged(col.get()); }
		shared_ptr<Column> column(int index) const { return d_columns.at(index); }
		int columnIndex(Column * col) const 
		{ 
			for(int i=0; i<d_columns.size(); i++)
				if(d_columns.at(i).get() == col) return i;
			return -1;
		}
		QString name() const { return d_name; }
		void setName(const QString& name) { d_name = name; }
		QItemSelectionModel * selectionModel() { return d_selection_model; };
		//@}

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
		QList< shared_ptr<Column> > selectedColumns(bool full = false);
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
		//! Return whether a cell is selected
		bool isCellSelected(int row, int col);
		//@}

	public slots:
		void selectAll();

	private slots:
		void handleDescriptionChange(AbstractColumn * col);
		void handlePlotDesignationChange(AbstractColumn * col);
		void handleDataChange(AbstractColumn * col);
		void handleRowsAboutToBeInserted(AbstractColumn * col, int before, int count);
		void handleRowsInserted(AbstractColumn * col, int before, int count);
		void handleRowsAboutToBeRemoved(AbstractColumn * col, int first, int count);
		void handleRowsRemoved(AbstractColumn * col, int first, int count);

	signals:
		void columnsAboutToBeInserted(int before, QList< shared_ptr<Column> > new_cols);
		void columnsInserted(int first, int count);
		void columnsAboutToBeReplaced(int first, int count);
		void columnsReplaced(int first, int count);
		void columnsAboutToBeRemoved(int first, int count);
		void columnsRemoved(int first, int count);
		//! Request resize command
		/**
		 *	Emit this signal to request the owner of the model 
		 *	to apply a resize command to it (change 
		 *	the number of rows). This gives the
		 *	owner the chance to do the resize in an undo
		 *	aware way. If the signal is ignored, the model
		 *	will resize itself.
		 */
		void requestResize(int new_row_count);

	private:
		//! The number of columns
		int d_column_count;
		//! The maximum number of rows of all columns
		int d_row_count;
		//! Vertical header data
		QStringList d_vertical_header_data;
		//! Horizontal header data
		QStringList d_horizontal_header_data;
		//! List of pointers to the column data vectors
		QList< shared_ptr<Column> > d_columns;	
		//! Table name (== aspect name of corresponding Table)
		QString d_name;
		QItemSelectionModel * d_selection_model;

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
		//! Internal function to put together the column header
		/**
		 * Don't use this outside updateHorizontalHeader()
		 */
		void composeColumnHeader(int col, const QString& label);
		//! Internal function to connect all column signals
		void connectColumn(shared_ptr<Column> col);
		//! Internal funciton to disconnect a column
		void disconnectColumn(shared_ptr<Column> col);
}; 

#endif
