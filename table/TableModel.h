/***************************************************************************
    File                 : TableModel.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Model storing data for a table

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
#include "core/AbstractDataSource.h"
#include "AbstractColumnData.h"
#include "core/AbstractFilter.h"
#include <QColor>
class QUndoStack;

#include <QtDebug>

//! Model storing data for a table
/**
  This model class provides the interface to column based data 
  (i.e. 1D vector based data like x-values and y-values for a plot).
  It contains the overloaded functions of QAbstractItemModel to be 
  called from views as well as functions for fast, direct access
  to the saved data.

  Each column is represented by an object from a class derived from
  AbstractColumnData and can be directly accessed by the pointer 
  returned by output(). Most of the column manipulation is done 
  directly to the columns using the table commands. The commands
  are also responsible for calling emitDataChanged() when any
  cells changed.

  For every column two filters are saved: An input filter that
  can convert a string (e.g. entered by the user in a cell) to
  the column's data type and an output filter that delivers
  the correct string representation to display in a table.
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
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QModelIndex parent(const QModelIndex & child) const;
	//@}

	//! \name Other functions
	//@{
	//! Overloaded from AbstractFilter
	virtual int numInputs() const { return 0; }
	//! Overloaded from AbstractFilter
	virtual int numOutputs() const { return d_column_count; }
	//! Return a pointer to the data source at column number 'port'
	/**
	 * \return returns a pointer to the column data source or zero if 'port' is invalid
	 */
	virtual AbstractDataSource *output(int port) const;
	//! Return a pointer to the column data at column number 'col'
	/**
	 * This is mainly used be the table commands.
	 *
	 * \return returns a pointer to the column data or zero if 'col' is invalid
	 */
	AbstractColumnData *columnPointer(int col) const;
	//! Return the output filter for column 'col'
	AbstractFilter * outputFilter(int col) const;
	//! Return the input filter for column 'col'
	AbstractFilter * inputFilter(int col) const;
	//! Set the input filter for column 'col'
	/**
	 * Input filters must be filters that convert
	 * from string to the type of the corresponding
	 * column. Input filters are automatically
	 * deleted when replaced. Each column needs its
	 * own filter object.
	 */
	void setInputFilter(int col, AbstractFilter * filter);
	//! Set the output filter for column 'col'
	/**
	 * Output filters must be filters that convert
	 * from the type of the corresponding
	 * column to string. Output filters are automatically
	 * deleted when replaced.
	 */
	void setOutputFilter(int col, AbstractFilter * filter);
	//! Replace columns completely
	/**
	 * The new columns must have the same types, otherwise the
	 * filters have to be replaced as well.
	 * TableModel takes over ownership of the new columns.
	 * \param first the first column to be replaced
	 * \param new_cols list of the columns that replace the old ones
	 * This does not delete the replaced columns.
	 */
	void replaceColumns(int first, QList<AbstractColumnData *> new_cols);
	//! Replace columns and their filters completely
	/**
	 * TableModel takes over ownership of the new columns.
	 * \param first the first column to be replaced
	 * \param new_cols list of the columns that replace the old ones
	 * \param in the corresponding input filters
	 * \param out the corresponding output filters
	 * This does not delete the replaced columns.
	 */
	void replaceColumns(int first, QList<AbstractColumnData *> new_col, QList<AbstractFilter *> in, QList<AbstractFilter *> out);
	//! Insert columns before column number 'before'
	/**
	 * If 'first' is higher than (current number of columns -1),
	 * the columns will be appended.
	 * TableModel takes over ownership of the columns. For each column
	 * an appropriate input and output filter must be given.
	 * \param before index of the column to insert before
	 * \param cols a list of column data objects
	 * \param in_filter a list of the corresponding input filters
	 * \param out_filter a list of the corresponding output filters
	 */
	void insertColumns(int before, QList<AbstractColumnData *> cols, QList<AbstractFilter *> in_filters,
		QList<AbstractFilter *> out_filters);
	//! Remove Columns
	 /**
	  * This does not delete the removed columns because this
	  * must be handled by the undo/redo system.
	  * \param first index of the first column to be removed
	  * \param count number of columns to remove
	 */
	void removeColumns(int first, int count);
	//! Insert rows before row number 'first'
	/**
	 * If first is higher than (current number of rows -1),
	 * the rows will be appended.
	 * \param first index of the row to insert before
	 * \param count number of rows to insert
	 */
	void insertRows(int first, int count);
	//! Append rows to the table
	void appendRows(int count);
	//! Remove rows from the table
	void removeRows(int first, int count);
	//! Append columns to the table
	/**
	 * TableModel takes over ownership of the column.
	 *
	 * \sa insertColumns()
	 */
	void appendColumns(QList<AbstractColumnData *> cols, QList<AbstractFilter *> in_filters,
		QList<AbstractFilter *> out_filters);
	//! Return the number of columns in the table
	int columnCount() const { return d_column_count; }
	//! Return the number of rows in the table
	int rowCount() const { return d_row_count; }
	//! Set a column's label
	void setColumnLabel(int column, const QString& label);
	//! Return a column's label
	QString columnLabel(int column) const;
	//! Set a column's comment
	void setColumnComment(int column, const QString& comment);
	//! Return a column's comment
	QString columnComment(int column) const;
	//! Set a column's plot designation
	void setColumnPlotDesignation(int column, AbstractDataSource::PlotDesignation pd);
	//! Return a column's plot designation
	AbstractDataSource::PlotDesignation columnPlotDesignation(int column) const;
	//! Show or hide (if on = false) the column comments
	void showComments(bool on = true);
	//! Return whether comments are show currently
	bool areCommentsShown();
	//! Return the full column header string
	QString columnHeader(int col);
	//! Return the number of columns with a given plot designation
	int numColsWithPD(AbstractDataSource::PlotDesignation pd);
	//! This must be called whenever columns were changed directly
	/**
	 * \param top first modified row
	 * \param left first modified column
	 * \param bottom last modified row
	 * \param right last modified column
	 */
	void emitDataChanged(int top, int left, int bottom, int right);
	//! Return a pointer to the undo stack
	virtual QUndoStack *undoStack() const;
	//@}
	
public slots:
	//! Push a command on the undo stack to provide undo for user input in cells
	void handleUserInput(const QModelIndex& index);

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
	QList<AbstractColumnData *> d_columns;	
	//! List of pointers to the input filters (one for each column)
	QList<AbstractFilter *> d_input_filters;	
	//! List of pointers to the output filters (one for each column)
	QList<AbstractFilter *> d_output_filters;	
	//! Flag: show/high column comments
	bool d_show_comments;
	//! The undo stack
	QUndoStack *d_undo_stack;
	
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
}; 

#endif
