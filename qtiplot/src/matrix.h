#ifndef MATRIX_H
#define MATRIX_H

#include <qtable.h>
#include "widget.h"
	
class Matrix: public myWidget
{
    Q_OBJECT

public:

	Matrix(int r, int c, const QString& label, QWidget* parent=0, const char* name=0, WFlags f=0);
	~Matrix(){};
	
	int numRows();
	int numCols();
	
	void init(int rows, int cols);

	bool isEmptyRow(int row);
	void addDataRow(const QString& s, int cols);

	//event handlers
	bool eventFilter(QObject *object, QEvent *e);
	void contextMenuEvent(QContextMenuEvent *e);

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

	void setValues (const QString& txt, const QString& formula,
					const QStringList& rowIndexes, const QStringList& colIndexes,
					int startRow, int endRow, int startCol, int endCol);

	QString text (int row, int col);
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

	void insertColumn();
	bool columnsSelected();
	void deleteSelectedColumns();

	void storeCellsToMemory();
	void freeMemory();

	double xStart(){return x_start;};
	double xEnd(){return x_end;};
	double yStart(){return y_start;};
	double yEnd(){return y_end;};
	void setCoordinates(double xs, double xe, double ys, double ye);

	//!Slot: notifies the main application that the matrix has been modified
	void notifyChanges(){emit modifiedWindow(this);};

signals:
	void showContextMenu();

private:
	QTable *table;
	QString formula_str;
	QChar txt_format;
	int selectedCol, lastSelectedCol, num_precision;
	bool LeftButton;
	//!Stores the matrix data only before the user opens the matrix dialog in order to avoid data loses during number format changes.
	double **dMatrix;
	double x_start, x_end, y_start, y_end;
};
   
#endif
