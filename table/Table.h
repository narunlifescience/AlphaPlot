/***************************************************************************
    File                 : Table.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Table aspect class

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

#include "AbstractScriptingEngine.h"
#include "Column.h"
#include "AbstractAspect.h"
#include <QObject>
#include <QModelIndex>
#include <QPoint>
#include <QList>
#include <QIcon>

class TableView;
class TableModel;
class QUndoStack;

/*!\brief Aspect providing a spreadsheet table with column logic.
 *
\section Class Table, the model/view architecture and the undo framework
To use Qt's model/view framework, increase the performance for large datasets, 
and support full undo/redo, this class 
has been rewritten using a design very similar to the model/view/presenter paradigm.<br>
The whole functionality is split up into three classes each having its special
purpose:
<ul>
<li> class Table: This class is on the one hand the presenter of the table functionality
and on the other hand one aspect in the projet hierarchy. It handles all interaction with
with the application and other aspects as well as user input. This includes script
evaluation, import/export, and saving and loading of a complete table. 
The underlying model is not visible to any non table related classes with one exeption:
Pointers to columns can be passed around an manipulated directly. The owner table will
be notified by emission of signals and react accordings. All method calls to table
and the columns are undo aware. The manipulation of the data in TableModel is done
by commands derived from QUndoCommand. If the table has an undo stack associated to it (usually 
by the project root aspect) all commands can be undone (and redone).
/li>
<li> class TableView: This class is purely for displaying the table contents. It relays
all user input to the presenter, i.e., Table. It is notfied by the model whenever changes occur.
</li>
<li> class TableModel: This class stores all data belonging to the table using column logic. 
Most of the data is contained in objects of class Column. Pointers to individual columns 
can be passed to the rest of the application. All operations of Column have undo support. The 
columns notify TableModel of any changes to them.
</ul>
*/
class Table: public QObject, public AbstractAspect, public scripted
{
	Q_OBJECT

	public:
		Table(AbstractScriptingEngine *engine, int rows, int columns, const QString &name);
		~Table();

		//! \name aspect related functions
		//@{
		//! See QMetaObject::className().
		virtual const char* className() const { return metaObject()->className(); }
		//! See QObject::inherits().
		virtual bool inherits(const char *class_name) const { return QObject::inherits(class_name); }
		//! Return an icon to be used for decorating my views.
		virtual QIcon icon() const { return QIcon(":/table.xpm"); }
		//! Return a new context menu for my views.
		/**
		 * Caller takes ownership of the menu.
		 */
		virtual QMenu *createContextMenu();
		//! Construct a standard view on me.
		/**
		 * If a parent is specified, the view is added to it as a child widget and the parent takes over
		 * ownership. If no parent is given, the caller receives ownership of the view.
		 * 
		 * This method may be called multiple times during the life time of an Aspect, or it might not get
		 * called at all. Aspects must not depend on the existence of a view for their operation.
		 */
		virtual QWidget *view(QWidget *parent = 0);
		//@}
		
		void insertColumns(int before, QList< shared_ptr<Column> > new_cols);
		void removeColumns(int first, int count);
		void removeRows(int first, int count);
		void insertRows(int before, int count);
		//! Set the number of rows
		void setRowCount(int new_size);
		//! Return the total number of columns in the table
		int columnCount();
		//! Return the total number of rows in the table
		int rowCount();
		//! Show or hide (if on = false) the column comments
		void showComments(bool on = true);

	private slots:
		//! Handles context menu requests from TableView
		void handleViewContextMenuRequest(TableView * view, const QPoint& pos);
		//! Handles a request from the model to execute a resize command
		void handleModelResizeRequest(int new_size);

		void handleColumnsAboutToBeInserted(int before, QList< shared_ptr<Column> > new_cols);
		void handleColumnsInserted(int first, int count);
		void handleColumnsAboutToBeRemoved(int first, int count);
		void handleColumnsRemoved(int first, int count);

		//! The the model name to the table name
		void setModelName();

	protected:
		//! The model storing the data
		TableModel *d_model;
};

#if false
class Table: public MyWidget, public scripted
{
    Q_OBJECT

public:
	Table(AbstractScriptingEngine *engine, int rows,int cols, const QString &label, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);

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
	int selectedColumnCount(SciDAVis::PlotDesignation pd);
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
	//! Return the number of columns matching the given designation
	int columnCount(SciDAVis::PlotDesignation pd);
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
	//! Return a list of all column labels
	QStringList columnLabels();
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
	//! Set the plot designation for a given column
	void setPlotDesignation(int col, SciDAVis::PlotDesignation pd);
	//! Return the plot designation for the given column
	SciDAVis::PlotDesignation plotDesignation(int col);
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
	SciDAVis::ColumnMode columnMode(int col);
	//! Set the column mode
	void setColumnMode(int col, SciDAVis::ColumnMode mode);
	//! Return the width of column 'col' in pixels
	int columnWidth(int col);
	
protected:
	//! The table widget
	TableView *d_table_view;
	//! The model storing the data
	TableModel *d_table_model;
	void contextMenuEvent(QContextMenuEvent *e);

private:
	//! Initialize table
	void init(int rows, int cols);


	
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
	void columnNumericFormat(int col, char *f, int *precision);
	void columnNumericFormat(int col, int *f, int *precision);
	void changeColWidth(int width, bool all = false);
	void enumerateRightCols(bool checked);
	void setColComment(int col, const QString& s);
	void changeColName(const QString& new_name);
	void setColName(int col,const QString& new_name);
	void setCommand(int col, const QString& com);
	void setColPlotDesignation(int col, SciDAVis::PlotDesignation pd);
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
	void addCol(SciDAVis::PlotDesignation pd = SciDAVis::Y);
	bool noXColumn();
	bool noYColumn();
	QStringList selectedYColumns();
	void setNumericPrecision(int prec);
	QVarLengthArray<double> col(int ycol);
	bool isEmptyColumn(int col);
	QStringList YColumns();
	void updateDecimalSeparators();
	void updateDecimalSeparators(const QLocale& oldSeparators);
	void importMultipleASCIIFiles(const QString &fname, const QString &sep, int ignoredLines,
					bool renameCols, bool stripSpaces, bool simplifySpaces, int importFileAs);
	void importASCII(const QString &fname, const QString &sep, int ignoredLines,
						bool renameCols, bool stripSpaces, bool simplifySpaces, bool newTable);
	bool exportASCII(const QString& fname, const QString& separator,
					bool withLabels = false, bool exportSelection = false);
	bool calculate(int col, int startRow, int endRow);
	bool calculate();
	void sortTableDialog();
	void sortColumnsDialog();
	void normalizeCol(int col=-1);
	void normalizeSelection();
	void normalize();
	QStringList drawableColumnSelection();
	void cutSelection();
	void copySelection();
	void clearSelection();
	void pasteSelection();
	void loadHeader(QStringList header);
	void setColWidths(const QStringList& widths);
	void setCommands(const QStringList& com);
	void setCommands(const QString& com);
	void setColumnTypes(const QStringList& ctl);
	void setCell(int row, int col, double val);
	void setColComments(const QStringList& lst);
	void setPlotDesignation(SciDAVis::PlotDesignation pd);

signals:
	void changedColHeader(const QString&, const QString&);
	void removedCol(const QString&);
	void modifiedData(Table *, const QString&);
	void optionsDialog();
	void colValuesDialog();
	void resizedTable(QWidget*);
	void showContextMenu(bool selection);
	void createTable(const QString&,int,int,const QString&);
};


#endif
#endif
