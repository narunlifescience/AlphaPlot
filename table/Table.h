/***************************************************************************
    File                 : Table.h
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
#ifndef TABLE_H
#define TABLE_H

class QUndoStack;

#include "MyWidget.h"
#include "tablecommands.h"
#include "AbstractDataSource.h"
#include "AbstractColumnData.h"
#include "ScriptingEnv.h"
#include "Script.h"
class TableView;
class TableModel;
class QUndoStack;

/*!\brief MDI window providing a spreadsheet table with column logic.
 *
\section Class Table, the model/view architecture and the undo framework
To use Qt's model/view framework, increase the performance for large datasets, 
and to prepare class Table for the upcoming undo/redo framework, this class 
has been rewritten using the following design:<br>
The whole functionality is split up into several classes each having its special
purpose:
<ul>
<li> class Table: This class communicates with the application, the main window, plot
windows and dialogs, i.e. provide the functionality of an MDI window. It handles script
evaluation, import/export, and saving and loading of a complete table. Most of the communication
with plot windows has been moved out of this class. The underlying model must not 
be visible to the plot windows since the undo/redo framework sits between the 
GUI (Table and TableView) and the model.</li>
<li> class TableView: This class is purely a GUI handling user input and displaying of
the table. It should not contain any data that would be saved in project file. It should
also not have any methods to be called from outside Table.</li>
<li> class TableDataModel: This class stores all data using column logic. It's supposed
to contain all data that would be saved to project file (except things like the 
geometry of Table). It receives commands from Table though the undo stack.</li>
<li> classes derived from AbstractColumnData: These classes store the data related
to one column. Each of them is optimized to store a special type of data (doubles, 
strings, dates, times). TableDataModel wraps around these classes but it is also 
possible to read them directly if this leads to a significant speed increase or 
column type specific data shall be read. The speed advantage mostly applies 
to double columns which can be accessed as contiguous arrays. Writing directly 
to columns is not a good idea since the undo/redo framework must have 
control over all changes.</li>
</ul>
The undo/redo framework works as a layer between Table/TableView
and TableDataModel/...ColumnData. Table and TableView send commands to TableDataModel that
can then be undone.
*/
class Table: public MyWidget, public scripted
{
    Q_OBJECT

public:
	Table(ScriptingEnv *env, int rows,int cols, const QString &label, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);

	//! The column mode (defines output and input filter)
	enum ColumnMode{Numeric = 0, //<! column contains doubles
	Text = 1, //!< column contains strings
	Date = 2,//!< obsolete legacy value, use DateTime instead
	Time = 3, //!< obsolete legacy value, use DateTime instead
	Month = 4, //!< column contains month names
	Day = 5, //!< column containts day of week names
	DateTime = 6 //!< column contains dates and/or times
	};

	//! Return a pointer to the undo stack
	virtual QUndoStack *undoStack() const;
	//! Return all selected columns
	/**
	 * If full is true, this function only returns a column if the whole 
	 * column is selected.
	*/
	QList<AbstractDataSource *> selectedColumns(bool full = false);
	//! Return how many columns are selected
	/**
	 * If full is true, this function only returns the number of fully 
	 * selected columns.
	*/
	int selectedColumnCount(bool full = false);
	//! Return how many columns with the given plot designation are (at least partly) selected
	int selectedColumnCount(AbstractDataSource::PlotDesignation pd);
	//! Returns true if column 'col' is selected; otherwise false
	/**
	 * If full is true, this function only returns true if the whole 
	 * column is selected.
	*/
	bool isColumnSelected(int col, bool full = false);
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
	//! Return a column's label
	QString columnLabel(int col);
	//! The the column's label
	void setColumnLabel(int col, const QString& label);
	//! Return the column's comment
	QString columnComment(int col);
	//! The the column's comment
	void setColumnComment(int col, const QString& comment);
	//! Return a column's plot designation
	AbstractDataSource::PlotDesignation columnPlotDesignation(int col);
	//! Return the number of columns matching the given designation
	int columnCount(AbstractDataSource::PlotDesignation pd);
	//! Return the total number of columns in the table
	int columnCount();
	//! Return the total number of rows in the table
	int rowCount();
	//! Set the color of the table background
	void setBackgroundColor(const QColor& col);
	//! Set the text color
	void setTextColor(const QColor& col);
	//! Set the header color
	void setHeaderColor(const QColor& col);
	//! Set the cell text font
	void setTextFont(const QFont& fnt);
	//! Set the font for both headers
	void setHeaderFont(const QFont& fnt);
	//! Show or hide (if on = false) the column comments
	void showComments(bool on = true);
	//! Return a list of all column labels
	QStringList columnLabels();
	//! Set the number of rows
	void setRowCount(int new_size);
	//! Set the number of columns
	void setColumnCount(int new_size);
	//! Copy another table
	void copy(Table * other);
	//! Fill the selected cells with row numbers
	void setAscendingValues();
	//! Fill the selected cells random values
	void setRandomValues();
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
	//! Return whether a cell is selected
	bool isCellSelected(int row, int col);
	//! Set the plot designation for a given column or all selected
	/*
	 *  If 'col' is -1 (the default), the plot designation is set for all
	 *  (at least partially) selected columns.
	 */
	void setPlotDesignation(AbstractDataSource::PlotDesignation pd, int col = -1);
	//! Return the plot designation for the given column
	AbstractDataSource::PlotDesignation plotDesignation(int col);
	//! Set the plot designation for the given column
	void setPlotDesignation(int col, AbstractDataSource::PlotDesignation pd);
	//! Clear the given column
	void clearColumn(int col);
	//! Clear the whole table
	void clear();
	//! Scroll to the specified cell
	void goToCell(int row, int col);
	//! Determine the corresponding X column
	int colX(int col);
	//! Determine the corresponding Y column
	int colY(int col);
	//! Return the column mode
	Table::ColumnMode columnMode(int col);
	//! Return the width of column 'col' in pixels
	int columnWidth(int col);

	
protected:
	//! The table widget
	TableView *d_table_view;
	//! The model storing the data
	TableModel *d_table_model;

private:
	//! Initialize table
	void init(int rows, int cols);

signals:

	
public:
	// obsolete transition functions (to be removed or rewritten later)
	int colIndex(const QString& name);
	QString text(int row, int col);
	QString colName(int col);
	int columnType(int col);
	QString columnFormat(int col);
	QStringList selectedYLabels();
	double cell(int row, int col);
	int selectedColumn();
	void setSelectedCol(int col);
	QStringList colNames();
	void setText(int row, int col, QString text);
	void setHeader(QStringList header);
	int colPlotDesignation(int col);
	QString colLabel(int col);
	QString colComment(int col);
	int numRows();
	int numCols();
	void columnNumericFormat(int col, char *f, int *precision);
	void columnNumericFormat(int col, int *f, int *precision);
	void changeColWidth(int width, bool all = false);
	void enumerateRightCols(bool checked);
	void setColComment(int col, const QString& s);
	void changeColName(const QString& new_name);
	void setColName(int col,const QString& new_name);
	void setCommand(int col, const QString& com);
	void setColPlotDesignation(int col, AbstractDataSource::PlotDesignation pd);
	void setColNumericFormat(int f, int prec, int col);
	void setTextFormat(int col);
	void setDateFormat(const QString& format, int col);
	void setTimeFormat(const QString& format, int col);
	void setMonthFormat(const QString& format, int col);
	void setDayFormat(const QString& format, int col);
	bool setDateTimeFormat(int col, int f, const QString& format);
	int verticalHeaderWidth();
	QStringList columnsList();
	QStringList selectedColumnsOld();
	int firstXCol();
	void addCol();

};

#endif
