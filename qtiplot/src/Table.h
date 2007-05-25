/***************************************************************************
    File                 : Table.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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

#include <q3table.h>
#include <q3header.h>
#include <Q3ValueList>
#include <QVarLengthArray>

#include "Graph.h"
#include "MyWidget.h"
#include "ScriptingEnv.h"
#include "Script.h"

class MyTable : public Q3Table
{
public:
    MyTable(QWidget * parent = 0, const char * name = 0);
    MyTable(int numRows, int numCols, QWidget * parent = 0, const char * name = 0);

    void activateNextCell();
};

/*!\brief MDI window providing a spreadsheet table with column logic.
 *
 * \section future Future Plans
 * Port to the Model/View approach used in Qt4 and get rid of the Qt3Support dependancy.
 * [ assigned to thzs ]
 */
class Table: public MyWidget, public scripted
{
    Q_OBJECT

public:
	enum PlotDesignation{All = -1, None = 0, X = 1, Y = 2, Z = 3, xErr = 4, yErr = 5};
	enum ColType{Numeric = 0, Text = 1, Date = 2, Time = 3, Month = 4, Day = 5};

   	Table(ScriptingEnv *env, const QString &fname,const QString &sep, int ignoredLines, bool renameCols,
		 bool stripSpaces, bool simplifySpaces, const QString &label,
		 QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
	Table(ScriptingEnv *env, int r,int c, const QString &label, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);

	Q3TableSelection getSelection();

	//! Sets the number of significant digits
	void setNumericPrecision(int prec);
	//! Updates the decimal separators in the numerical columns on user request
	void updateDecimalSeparators();

public slots:
	MyTable* table(){return d_table;};
	void copy(Table *m);
	int numRows();
	int numCols();
	void setNumRows(int rows);
	void setNumCols(int cols);
	void resizeRows(int);
	void resizeCols(int);

	//! Return the value of the cell as a double
	double cell(int row, int col);
	void setCell(int row, int col, double val);

	QString text(int row, int col);
	QStringList columnsList();
	QStringList colNames(){return col_label;}
	QString colName(int col);
	QString colLabel(int col){return col_label[col];};
	int colIndex(const QString& name);

	int colPlotDesignation(int col){return col_plot_type[col];};
	void setColPlotDesignation(int col, PlotDesignation d){col_plot_type[col]=d;};
	void setPlotDesignation(PlotDesignation pd);
	Q3ValueList<int> plotDesignations(){return col_plot_type;};

	void setColName(int col,const QString& text);
	void setHeader(QStringList header);
	void loadHeader(QStringList header);
	void setHeaderColType();
	void setText(int row,int col,const QString & text);
	void setRandomValues();
	void setAscValues();

	void cellEdited(int,int col);
	void moveCurrentCell();
	void clearCell(int row, int col);
	QString saveText();
	bool isEmptyRow(int row);
	bool isEmptyColumn(int col);
	int nonEmptyRows();

	void plotL();
	void plotLP();
	void plotP();
	void plotVB();
	void plotHB();
	void plotArea();
	void plotPie();
	void plotVerticalDropLines();
	void plotSpline();
	void plotVertSteps();
    void plotHorSteps();
	void plotHistogram();
	void plotVectXYXY();
	void plotVectXYAM();
	void plotBoxDiagram();

	//! \name 3D Plots
	//@{
	void plot3DRibbon();
	void plot3DScatter();
	void plot3DTrajectory();
	void plot3DBars();

	bool valid2DPlot();
	bool valid3DPlot();
	//@}v

	void print();
	void print(const QString& fileName);
	void exportPDF(const QString& fileName);

	//! \name Event Handlers
	//@{
	bool eventFilter(QObject *object, QEvent *e);
	void contextMenuEvent(QContextMenuEvent *e);
	void customEvent( QEvent* e);
	//@}v

	//! \name Column Operations
	//@{
	void removeCol();
	void removeCol(const QStringList& list);
	void clearCol();
	void insertCol();
	void insertCols(int start, int count);
	void addCol(PlotDesignation pd = Y);
	void addColumns(int c);
	//@}

	//! \name Sorting
	//@{
	/*!\brief Sort the current column in ascending order.
	 * \sa sortColDesc(), sortColumn(), Q3Table::currentColumn()
	 */
	void sortColAsc();
	/*!\brief Sort the current column in descending order.
	 * \sa sortColAsc(), sortColumn(), Q3Table::currentColumn()
	 */
	void sortColDesc();
	/*!\brief Sort the specified column.
	 * \param col the column to be sorted
	 * \param order 0 means ascending, anything else means descending
	 */
	void sortColumn(int col = -1, int order = 0);
	/*!\brief Display a dialog with some options for sorting all columns.
	 *
	 * The sorting itself is done using sort(int,int,const QString&).
	 */
	void sortTableDialog();
	//! Sort all columns as in sortColumns(const QStringList&,int,int,const QString&).
	void sort(int type = 0, int order  = 0, const QString& leadCol = QString());
	//! Sort selected columns as in sortColumns(const QStringList&,int,int,const QString&).
	void sortColumns(int type = 0, int order = 0, const QString& leadCol = QString());
	/*!\brief Sort the specified columns.
	 * \param cols the columns to be sorted
	 * \param type 0 means sort individually (as in sortColumn()), anything else means together
	 * \param order 0 means ascending, anything else means descending
	 * \param leadCol for sorting together, the column which determines the permutation
	 */
	void sortColumns(const QStringList& cols, int type = 0, int order = 0, const QString& leadCol = QString());
	/*!\brief Display a dialog with some options for sorting the selected columns.
	 *
	 * The sorting itself is done using sortColumns(int,int,const QString&).
	 */
	void sortColumnsDialog();
	//@}

	//! \name Normalization
	//@{
	void normalizeCol(int col=-1);
	void normalizeSelection();
	void normalize();
	//@}

	QVarLengthArray<double> col(int ycol);
	int firstXCol();
	bool noXColumn();
	bool noYColumn();
	int colX(int col);
	int colY(int col);

	QStringList getCommands(){return commands;};
	//! Set all column formulae.
	void setCommands(const QStringList& com);
	//! Set all column formulae.
	void setCommands(const QString& com);
	//! Set formula for column col.
	void setCommand(int col, const QString com);
	//! Compute specified cells from column formula.
	bool calculate(int col, int startRow, int endRow);
	//! Compute selected cells from column formulae; use current cell if there's no selection.
	bool calculate();

	//! \name Row Operations
	//@{
	void deleteSelectedRows();
	void insertRow();
	//@}

	//! Selection Operations
	//@{
	void cutSelection();
	void copySelection();
	void clearSelection();
	void pasteSelection();
	void selectAllTable();
	void deselect();
	void clear();
	//@}

	void init(int rows, int cols);
	QStringList selectedColumns();
	QStringList selectedYColumns();
	QStringList selectedErrColumns();
	QStringList selectedYLabels();
	QStringList drawableColumnSelection();
	QStringList YColumns();
	int selectedColsNumber();
	void changeColName(const QString& text);
	void enumerateRightCols(bool checked);

	void changeColWidth(int width, bool allCols);
	void changeColWidth(int width, int col);
	int columnWidth(int col);
	QStringList columnWidths();
	void setColWidths(const QStringList& widths);

	void setSelectedCol(int col){selectedCol = col;};
	int selectedColumn(){return selectedCol;};
	int firstSelectedColumn();
	int numSelectedRows();
	bool isRowSelected(int row, bool full=false) { return d_table->isRowSelected(row, full); }
	bool isColumnSelected(int col, bool full=false) { return d_table->isColumnSelected(col, full); }
	//! Scroll to row (row starts with 1)
	void goToRow(int row);

	void columnNumericFormat(int col, char *f, int *precision);
	void columnNumericFormat(int col, int *f, int *precision);
	int columnType(int col){return colTypes[col];};

	Q3ValueList<int> columnTypes(){return colTypes;};
	void setColumnTypes(Q3ValueList<int> ctl){colTypes = ctl;};
	void setColumnTypes(const QStringList& ctl);
	void setColumnType(int col, ColType val) { colTypes[col] = val; }

	void saveToMemory();
	void freeMemory();

	QString columnFormat(int col){return col_format[col];};
	QStringList getColumnsFormat(){return col_format;};
	void setColumnsFormat(const QStringList& lst);

	void setTextFormat(int col);
	void setColNumericFormat(int f, int prec, int col);
	bool setDateFormat(const QString& format, int col);
	bool setTimeFormat(const QString& format, int col);
	void setMonthFormat(const QString& format, int col);
	void setDayFormat(const QString& format, int col);
	bool setDateTimeFormat(int f, const QString& format, int col);

	bool exportToASCIIFile(const QString& fname, const QString& separator,
							  bool withLabels,bool exportSelection);
	void importASCII(const QString &fname, const QString &sep, int ignoredLines,
						bool renameCols, bool stripSpaces, bool simplifySpaces, bool newTable);
	void importMultipleASCIIFiles(const QString &fname, const QString &sep, int ignoredLines,
					bool renameCols, bool stripSpaces, bool simplifySpaces, int importFileAs);

	//! \name Saving and Restoring
	//@{
	virtual QString saveToString(const QString& geometry);
	QString saveHeader();
	QString saveComments();
	QString saveCommands();
	QString saveColumnWidths();
	QString saveColumnTypes();

	void setSpecifications(const QString& s);
	QString& getSpecifications();
	void restore(QString& spec);
	QString& getNewSpecifications();
	void setNewSpecifications();

	/*!
	 *used for restoring the table old caption stored in specifications string
	 */
	QString oldCaption();

	/*!
	 *used for restoring the table caption stored in new specifications string
	 */
	QString newCaption();
	//@}

	void setBackgroundColor(const QColor& col);
	void setTextColor(const QColor& col);
	void setHeaderColor(const QColor& col);
	void setTextFont(const QFont& fnt);
	void setHeaderFont(const QFont& fnt);

	int verticalHeaderWidth(){return d_table->verticalHeader()->width();};

	QString colComment(int col){return comments[col];};
	void setColComment(int col, const QString& s);
	QStringList colComments(){return comments;};
	void setColComments(const QStringList& lst){comments = lst;};
	void showComments(bool on = true);
	bool commentsEnabled(){return d_show_comments;}

	QString saveAsTemplate(const QString& geometryInfo);
	void restore(const QStringList& lst);

	//! This slot notifies the main application that the table has been modified. Triggers the update of 2D plots.
	void notifyChanges();

	//! Notifies the main application that the width of a table column has been modified by the user.
	void colWidthModified(int, int, int);

signals:
	void plot3DRibbon(Table *,const QString&);
	void plotXYZ(Table *,const QString&, int);
	void plotCol(Table *,const QStringList&, int, int, int);
	void changedColHeader(const QString&, const QString&);
	void removedCol(const QString&);
	void modifiedData(Table *, const QString&);
	void optionsDialog();
	void colValuesDialog();
	void resizedTable(QWidget*);
	void showContextMenu(bool selection);
	void createTable(const QString&,int,int,const QString&);

protected:
	MyTable *d_table;

private:
	bool d_show_comments;
	QString specifications, newSpecifications;
	QStringList commands, col_format, comments, col_label;
	QList<int> colTypes, col_plot_type;
	int selectedCol;
	double **d_saved_cells;

	//! Internal function to change the column header
	void setColumnHeader(int index, const QString& label);
};

#endif
