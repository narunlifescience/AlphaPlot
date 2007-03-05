/***************************************************************************
    File                 : matrix.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
					  Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Matrix worksheet class
                           
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

#include <q3table.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <QEvent>
#include "widget.h"
#include "Scripting.h"
#include <qwt_double_rect.h>

//! Matrix worksheet class
class Matrix: public MyWidget, public scripted
{
    Q_OBJECT

public:

	Matrix(ScriptingEnv *env, int r, int c, const QString& label, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
	~Matrix(){};
	
	int numRows();
	int numCols();
	
	void init(int rows, int cols);

	bool isEmptyRow(int row);

	//event handlers
	bool eventFilter(QObject *object, QEvent *e);
	void contextMenuEvent(QContextMenuEvent *e);
	void customEvent(QEvent *e);

public slots:

	void print();
	void cellEdited(int,int);
	void moveCurrentCell();

	int columnsWidth();
	void setColumnsWidth(int width);

	void setMatrixDimensions(int rows, int cols);
	void transpose();
	void invert();
	double determinant();

	bool calculate(int startRow, int endRow, int startCol, int endCol);

	//! Return the numeric value in the specified cell.
	double cell(int row, int col);
	//! Set the (numeric) value of the specified cell.
	void setCell(int row, int col, double value);
	//! Return the textual representation of the contents of the specified cell.
	QString text (int row, int col);
	//! Set a cell's value from a string.
	void setText (int row, int col, const QString & text );

	QChar textFormat(){return txt_format;};
	int precision(){return num_precision;};
	void setTextFormat(const QChar &format, int precision);
	void setNumericFormat(const QChar & f, int prec);

	QString formula(){return formula_str;};
	void setFormula(const QString &s);

	void restore(const QStringList &l);
	QString saveAsTemplate(const QString &info);

	QString saveToString(const QString &info);
	QString saveText();

	// selection operations 
	void cutSelection();
	void copySelection();
	void clearSelection();
	void pasteSelection();
	void selectAll();

	void insertRow();
	bool rowsSelected();
	void deleteSelectedRows();
	int numSelectedRows();

	void insertColumn();
	bool columnsSelected();
	void deleteSelectedColumns();
	int numSelectedColumns();

	//! This slot notifies the main application that the matrix has been modified
	void saveCellsToMemory();
	void forgetSavedCells();

	double xStart(){return x_start;};
	double xEnd(){return x_end;};
	double yStart(){return y_start;};
	double yEnd(){return y_end;};
	
	//! Returns the bounding rect of the matrix coordinates
  	QwtDoubleRect boundingRect(){return QwtDoubleRect(x_start, y_start, x_end-x_start, y_end-y_start).normalized();};
	void setCoordinates(double xs, double xe, double ys, double ye);

	 //! Min and max values of the matrix.
  	void range(double *min, double *max);
	Q3Table* table(){return d_table;};

	//! Notifies the main application that the matrix has been modified
	void notifyChanges(){emit modifiedWindow(this);};

signals:
	void showContextMenu();

private:
	Q3Table *d_table;
	QString formula_str;
	QChar txt_format;
	int selectedCol, num_precision;
	//!Stores the matrix data only before the user opens the matrix dialog in order to avoid data loses during number format changes.
	double **dMatrix;
	double x_start, x_end, y_start, y_end;
};
   
#endif
