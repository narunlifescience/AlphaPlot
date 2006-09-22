/***************************************************************************
    File                 : worksheet.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#ifndef WORKSHEET_H
#define WORKSHEET_H

#include <qwidget.h>
#include <q3table.h>
#include <q3header.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <Q3ValueList>
#include <QEvent>
#include <Q3MemArray>
#include <Q3TableSelection>

#include "graph.h"
#include "widget.h"
#include "Scripting.h"

class ScriptingEnv;

//! Table worksheet class
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
	~Table();

	Q3TableSelection getSelection();
	
public slots:
	Q3Table* table(){return worksheet;};
	void copy(Table *m);
	int tableRows();
	int tableCols();
	void setNumRows(int rows);
	void setNumCols(int cols);
	void resizeRows(int);
	void resizeCols(int);

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
	void plotSteps();
	void plotHistogram();
	void plotVectXYXY();
	void plotVectXYAM();
	void plotBoxDiagram();
	
	//3D plots
	void plot3DRibbon();
	void plot3DScatter();
	void plot3DTrajectory();
	void plot3DBars();
	
	bool valid2DPlot();
	bool valid3DPlot();
	
	void print();
	
	// event handlers 
	bool eventFilter(QObject *object, QEvent *e);
	void contextMenuEvent(QContextMenuEvent *e);
	void mouseMoveEvent( QMouseEvent * e);
	void mousePressEvent( QMouseEvent * e);
	void customEvent( QCustomEvent* e);
	
	// column operations 
	void removeCol();
	void removeCol(const QStringList& list);
	void clearCol();
	void insertCol();
	void insertCols(int start, int count);
	void addCol(PlotDesignation pd = Y);
	void addColumns(int c);
	
	//sorting
	void sortColAsc();
	void sortColDesc();
	void sortTableDialog();
	void sortTable(int type, int order, const QString& leadCol);
	void sortColumns(int type, int order, const QString& leadCol);
	void sortColumns(const QStringList&s, int type, int order, const QString& leadCol);
	void sortColumnsDialog();
	
	//normalization
	void normalizeCol(int col=-1);
	void normalizeSelection();
	void normalizeTable();

	void correlate();
	void convolute(int sign);
	void convlv(double *sig, int n, double *dres, int m, int sign);
	void fft(double sampling, const QString& realColName, const QString& imagColName,
			bool forward, bool normalize, bool order);

	Q3MemArray<double> col(int ycol);
	int firstXCol();
	bool noXColumn();
	bool noYColumn();
	int colX(int col);
	int colY(int col);

	int atRow(int col, double value);

	QStringList getCommands(){return commands;};
	//!Slot: Set all column formulae.
	void setCommands(const QStringList& com);
	//!Slot: Set all column formulae.
	void setCommands(const QString& com);
	//!Slot: Set formula for column col.
	void setCommand(int col, const QString com);
	//!Slot: Compute specified cells from column formula.
	bool calculate(int col, int startRow, int endRow);
	//!Slot: Compute selected cells from column formulae; use current cell if there's no selection.
	bool calculate();
	
	// row operations 
	void deleteSelectedRows();
	void insertRow();

	// selection operations 
	void cutSelection();
	void copySelection();
	void clearSelection();
	void pasteSelection();
	void selectAllTable();
	void deselect();
	bool singleRowSelected();
	bool multipleRowsSelected();

	void init(int rows, int cols);
	QStringList selectedColumns();
	QStringList selectedYColumns();
	QStringList selectedYLabels();
	QStringList YColumns();
	int selectedColsNumber();
	void changeColName(const QString& text);
	void enumerateRightCols(bool checked);

	void changeColWidth(int width, bool allCols);	
	int columnWidth(int col);
	QStringList columnWidths();
	void setColWidths(const QStringList& widths);
	
	void setSelectedCol(int col){selectedCol = col;};
	int selectedColumn(){return selectedCol;};
	int firstSelectedColumn();
	int selectedRows();
	bool isRowSelected(int row, bool full=false) { return worksheet->isRowSelected(row, full); }
	bool isColumnSelected(int col, bool full=false) { return worksheet->isColumnSelected(col, full); }
	
	void columnNumericFormat(int col, char &f, int &precision);
	void columnNumericFormat(int col, int &f, int &precision);
	int columnType(int col){return colTypes[col];};

	Q3ValueList<int> columnTypes(){return colTypes;};
	void setColumnTypes(Q3ValueList<int> ctl){colTypes = ctl;};
	void setColumnTypes(const QStringList& ctl);

	//!Slot: Use a copy of column col when accessing it via text() until forgetSavedCol() is called.
	void saveColToMemory(int col);
	//!Slot: Use spreadsheat data again for all columns after saveColToMemory(int) was called.
	void forgetSavedCol();

	QString columnFormat(int col){return col_format[col];};
	QStringList getColumnsFormat(){return col_format;};
	void setColumnsFormat(const QStringList& lst);
	
	void setTextFormat(bool applyToAll);
	void setColNumericFormat(int f, int prec, int col);
	void setDateFormat(const QString& format, int col);
	void setTimeFormat(const QString& format, int col);
	void setMonthFormat(const QString& format, int col);
	void setDayFormat(const QString& format, int col);
	void setDateTimeFormat(int f, const QString& format,int col);
	void setDateTimeFormat(int f, const QString& format,bool applyToAll);
	void setNumericFormat(int f, int prec,bool applyToAll);

	bool exportToASCIIFile(const QString& fname, const QString& separator,
							  bool withLabels,bool exportSelection);
	void importASCII(const QString &fname, const QString &sep, int ignoredLines, 
						bool renameCols, bool stripSpaces, bool simplifySpaces, bool newTable);
	void importMultipleASCIIFiles(const QString &fname, const QString &sep, int ignoredLines,
					bool renameCols, bool stripSpaces, bool simplifySpaces, int importFileAs);

	//saving
	virtual QString saveToString(const QString& geometry);
	QString saveHeader();
	QString saveComments();
	QString saveCommands();
	QString saveColumnWidths();
	QString saveColumnTypes();

	//restoring
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

	void setBackgroundColor(const QColor& col);
	void setTextColor(const QColor& col);
	void setHeaderColor(const QColor& col);
	void setTextFont(const QFont& fnt);
	void setHeaderFont(const QFont& fnt);
		
	int verticalHeaderWidth(){return worksheet->verticalHeader()->width();};

	QString colComment(int col){return comments[col];};
	void setColComment(const QString& s);

	QStringList colComments(){return comments;};
	void setColComments(const QStringList& lst){comments = lst;};

	QString saveAsTemplate(const QString& geometryInfo);
	void restore(const QStringList& lst);

	//! This slot notifies the main application that the table has been modified. Triggers the update of 2D plots.
	void notifyChanges();

	//!Slot: notifies the main application that the width of a table column has been modified by the user
	void colWidthModified(int, int, int);
				
signals:
	void plot3DRibbon(Table *,const QString&);
	void plotXYZ(Table *,const QString&, int);
	void plotCol(Table *,const QStringList&, int);
	void changedColHeader(const QString&, const QString&);
	void removedCol(const QString&);
	void modifiedData(Table *, const QString&);
	void optionsDialog();
	void colValuesDialog();
	void resizedTable(QWidget*);
	void showContextMenu(bool selection);
	void createTable(const QString&,int,int,const QString&);
	
protected:
	Q3Table *worksheet;

private:
	QString specifications, newSpecifications;
	QStringList commands, col_format, comments, col_label;
	QList<int> colTypes, col_plot_type;
	int selectedCol, lastSelectedCol;
	QStringList savedCells;
	int savedCol;
	bool LeftButton;
};

#endif
