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
class QMenu;

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
		virtual QIcon icon() const;
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
		
		//! Insert columns
		/**
		 * If before == columnCount() this will do the same as appendColumns();
		 */
		void insertColumns(int before, QList< shared_ptr<Column> > new_cols);
		//! Append columns
		/*
		 * Convenience function, same as:
		 * <code>
		 * insertColumns(columnCount(), new_cols);
		 * </code>
		 */
		void appendColumns(QList< shared_ptr<Column> > new_cols) { insertColumns(columnCount(), new_cols); }
		void removeColumns(int first, int count);
		void removeColumn(Column * col);
		void removeColumn(shared_ptr<Column> col) { removeColumn(col.get()); }
		void removeRows(int first, int count);
		void insertRows(int before, int count);
		void appendRows(int count) { insertRows(rowCount(), count); }
		//! Set the number of rows
		void setRowCount(int new_size);
		//! Return the total number of columns in the table
		int columnCount() const;
		//! Return the total number of rows in the table
		int rowCount() const;
		//! Show or hide (if on = false) the column comments
		void showComments(bool on = true);
		//! Return whether comments are show currently
		bool areCommentsShown() const;
		//! Return the number of columns matching the given designation
		int columnCount(SciDAVis::PlotDesignation pd) const;
		shared_ptr<Column> column(int index) const;
		int columnIndex(Column * col) const;
		int columnIndex(shared_ptr<Column> col) const;
		//! Set the number of columns
		void setColumnCount(int new_size);

		// TODO: move this to abstract aspect?
		//! Create a menu for the main application window
		QMenu * createApplicationWindowMenu();
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
		void sortDialog(QList< shared_ptr<Column> > cols);
		//! Set default for comment visibility for table views
		void setDefaultCommentVisibility(bool visible) { d_default_comment_visibility = visible; }
		//! Return the default for comment visibility for table views
		bool defaultCommentVisibility() { return d_default_comment_visibility; }
		//! Return the text displayed in the given cell
		QString text(int row, int col);
		void setSelectionAs(SciDAVis::PlotDesignation pd);

	public slots:
		//! Clear the whole table
		void clear();
		//! Clear all mask in the table
		void clearMasks();

		//! Append one column
		void addColumn();
		//! Append as many columns as are selected
		void addColumns();
		void addRows();
		void cutSelection();
		void copySelection();
		void pasteIntoSelection();
		void clearSelectedCells();
		void goToCell();
		void maskSelection();
		void unmaskSelection();
		void setFormulaForSelection();
		void recalculateSelectedCells();
		void fillSelectedCellsWithRowNumbers();
		void fillSelectedCellsWithRandomNumbers();
		//! Open the sort dialog for all columns
		void sortTable();
		void insertEmptyColumns();
		void removeSelectedColumns();
		void clearSelectedColumns();
		void clearSelectedRows();
		void setSelectedColumnsAsX();
		void setSelectedColumnsAsY();
		void setSelectedColumnsAsZ();
		void setSelectedColumnsAsXError();
		void setSelectedColumnsAsYError();
		void setSelectedColumnsAsNone();
		void normalizeSelectedColumns();
		void sortSelectedColumns();
		void statisticsOnSelectedColumns();
		void statisticsOnSelectedRows();
		void insertEmptyRows();
		void removeSelectedRows();
		void selectAll();
		void editTypeAndFormatOfSelectedColumns();
		void editDescriptionOfCurrentColumn();
		void moveColumn(int from, int to);
		void copy(Table * other);
		//! Sort the given list of column
		/*
		 * If 'leading' is a null pointer, each column is sorted separately.
		 */
		void sortColumns(shared_ptr<Column> leading, QList< shared_ptr<Column> > cols, bool ascending);
		void openFormulaEditor();

	private slots:
		//! Handles context menu requests from TableView
		/**
		 * \param view the view in which the context menu should be displayed
		 * \param pos global position of the event 
		*/
		void handleViewContextMenuRequest(TableView * view, const QPoint& pos);
		//! Handles context menu requests from TableView's horizontal header
		/**
		 * \param view the view in which the context menu should be displayed
		 * \param pos global position of the event 
		*/
		void handleViewColumnContextMenuRequest(TableView * view, const QPoint& pos);
		//! Handles context menu requests from TableView's vertical header
		/**
		 * \param view the view in which the context menu should be displayed
		 * \param pos global position of the event 
		*/
		void handleViewRowContextMenuRequest(TableView * view, const QPoint& pos);
		//! Handles a request from the model to execute a resize command
		void handleModelResizeRequest(int new_size);

		void handleColumnsAboutToBeInserted(int before, QList< shared_ptr<Column> > new_cols);
		void handleColumnsInserted(int first, int count);
		void handleColumnsAboutToBeRemoved(int first, int count);
		void handleColumnsRemoved(int first, int count);

		//! The the model name to the table name
		void setModelName();

	signals:
		void scrollToIndex(const QModelIndex& index);
		void toggleOptionTabBar();
		void showOptionsDescriptionTab();
		void showOptionsTypeTab();
		void showOptionsFormulaTab();

	private:
		void createActions();
		//! Internal helper function
		void addUndoToMenu(QMenu * menu);
		QMenu * d_plot_menu;
		bool d_default_comment_visibility;

		//! \name selection related actions
		//@{
		QAction * action_cut_selection;
		QAction * action_copy_selection;
		QAction * action_paste_into_selection;
		QAction * action_mask_selection;
		QAction * action_unmask_selection;
		QAction * action_set_formula;
		QAction * action_clear_selection;
		QAction * action_recalculate;
		QAction * action_fill_row_numbers;
		QAction * action_fill_random;
		//@}
		//! \name table related actions
		//@{
		QAction * action_toggle_comments;
		QAction * action_toggle_tabbar;
		QAction * action_select_all;
		QAction * action_add_column;
		QAction * action_clear_table;
		QAction * action_clear_masks;
		QAction * action_sort_table;
		QAction * action_go_to_cell;
		QAction * action_formula_editor;
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
	//! Copy another table
	void copy(Table * other);
	//! Fill the selected cells with row numbers
	void setAscendingValues();
	//! Fill the selected cells random values
	void setRandomValues();
	//! Scroll to the specified cell
	void goToCell(int row, int col);
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
