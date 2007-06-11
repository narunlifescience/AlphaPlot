/***************************************************************************
    File                 : Matrix.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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
#include "Matrix.h"

#include <QtGlobal>
#include <QTextStream>
#include <QList>
#include <QEvent>
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QHeaderView>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>
#include <QVarLengthArray>
#include <QClipboard>
#include <QShortcut>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QLocale>

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>

Matrix::Matrix(ScriptingEnv *env, int r, int c, const QString& label, QWidget* parent, const char* name, Qt::WFlags f)
: MyWidget(label, parent, name, f), scripted(env)
{
	init(r, c);
}

void Matrix::init(int rows, int cols)
{
	formula_str = "";
	txt_format = 'f';
	num_precision = 6;
	x_start = 1.0;
	x_end = 10.0;
	y_start = 1.0;
	y_end = 10.0;
	dMatrix = 0;
	allow_modification_signals = true;

	QDateTime dt = QDateTime::currentDateTime();
	setBirthDate(dt.toString(Qt::LocalDate));

	// create the main table widget
	d_table = new QTableWidget(rows, cols, 0);
	d_table->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	d_table->setFocusPolicy(Qt::StrongFocus);
	d_table->setFocus();
	// only one contiguous selection supported
	d_table->setSelectionMode(QAbstractItemView::ContiguousSelection);
	d_table->verticalHeader()->setMovable(true);

	// background color to yellow to distinguish between matrix and table
	QPalette pal = d_table->palette();
	pal.setColor(QColorGroup::Base, QColor(255, 255, 128));
	d_table->setPalette(pal);

	// give all space to the table widget
	QVBoxLayout* main_layout = new QVBoxLayout(this);
	main_layout->setMargin(0);
	main_layout->addWidget(d_table);

	// set header properties
	QHeaderView* hHeader=(QHeaderView*)d_table->horizontalHeader();
	hHeader->setMovable(false);
	hHeader->setResizeMode(QHeaderView::Fixed);
	QHeaderView* vHeader=(QHeaderView*)d_table->verticalHeader();
	vHeader->setMovable(false);
	vHeader->setResizeMode(QHeaderView::ResizeToContents);

	// resize the table
	setGeometry(50, 50, qMin(_Matrix_initial_columns_, cols)*hHeader->sectionSize(0) + 55,
                (qMin(_Matrix_initial_rows_,rows)+1)*vHeader->sectionSize(0));

	// keyboard shortcuts
	QShortcut * sel_all = new QShortcut(QKeySequence(tr("Ctrl+A", "Matrix: select all")), this);
	connect(sel_all, SIGNAL(activated()), d_table, SLOT(selectAll()));
	// remark: the [TAB] behaviour is now nicely done by Qt4

	connect(d_table, SIGNAL(cellChanged(int,int)), this, SLOT(cellEdited(int,int)));
}

void Matrix::cellEdited(int row,int col)
{
	QString cell_text = text(row,col);
	if(cell_text.isEmpty()) return;

	QString cell_formula = cell_text;

	bool ok = true;
    QLocale locale;
  	double res = locale.toDouble(cell_text, &ok);
	if (ok)
		setText(row, col, locale.toString(res, txt_format.toAscii(), num_precision));
	else
	{
		Script *script = scriptEnv->newScript(cell_formula, this, QString("<%1_%2_%3>").arg(name()).arg(row).arg(col));
		connect(script, SIGNAL(error(const QString&,const QString&,int)), scriptEnv, SIGNAL(error(const QString&,const QString&,int)));

		script->setInt(row+1, "row");
		script->setInt(row+1, "i");
		script->setInt(col+1, "col");
		script->setInt(col+1, "j");

		QVariant ret = script->eval();
		if(ret.type()==QVariant::Int || ret.type()==QVariant::UInt || ret.type()==QVariant::LongLong
				|| ret.type()==QVariant::ULongLong)
			setText(row, col, ret.toString());
		else if(ret.canConvert(QVariant::Double))
			setText(row, col, locale.toString(ret.toDouble(), txt_format.toAscii(), num_precision));
		else
			setText(row, col, "");
	}

    if(row+1 >= numRows())
        d_table->setRowCount(row + 2);

	d_table->setCurrentCell(row+1, col);

	if(allow_modification_signals)
		emit modifiedWindow(this);
}

double Matrix::cell(int row, int col)
{
	if(dMatrix)
		return dMatrix[row][col];
	else
	{
		if(d_table->item(row, col))
		    return stringToDouble(d_table->item(row, col)->text());
		else
			return 0.0;
	}
}

void Matrix::setCell(int row, int col, double value)
{
	if(d_table->item(row, col))
		d_table->item(row, col)->setText(QLocale().toString(value, txt_format.toAscii(), num_precision));
	else
		d_table->setItem(row, col, new QTableWidgetItem(QLocale().toString(value, txt_format.toAscii(), num_precision)));
}

QString Matrix::text(int row, int col)
{
		if(d_table->item(row, col))
			return d_table->item(row, col)->text();
		else
			return QString("");
}

void Matrix::setText (int row, int col, const QString & new_text )
{
	if(d_table->item(row, col))
		d_table->item(row, col)->setText(new_text);
	else
		d_table->setItem(row, col, new QTableWidgetItem(new_text));
}

bool Matrix::isEmptyRow(int row)
{
	int cols = d_table->columnCount();

	for(int i=0; i<cols; i++)
		if (!text(row, i).isEmpty())
			return false;

	return true;
}

void Matrix::setCoordinates(double xs, double xe, double ys, double ye)
{
	if (x_start == xs && x_end == xe &&	y_start == ys && y_end == ye)
		return;

	x_start = xs;
	x_end = xe;
	y_start = ys;
	y_end = ye;

	emit modifiedWindow(this);
}

QString Matrix::saveToString(const QString &info)
{
	QString s = "<matrix>\n";
	s += QString(name()) + "\t";
	s += QString::number(numRows())+"\t";
	s += QString::number(numCols())+"\t";
	s += birthDate() + "\n";
	s += info;
	s += "ColWidth\t" + QString::number(d_table->columnWidth(0))+"\n";
	s += "<formula>\n" + formula_str + "\n</formula>\n";
	s += "TextFormat\t" + QString(txt_format) + "\t" + QString::number(num_precision) + "\n";
	s += "WindowLabel\t" + windowLabel() + "\t" + QString::number(captionPolicy()) + "\n";
	s += "Coordinates\t" + QString::number(x_start,'g',15) + "\t" +QString::number(x_end,'g',15) + "\t";
	s += QString::number(y_start,'g',15) + "\t" + QString::number(y_end,'g',15) + "\n";
	s += saveText();
	s +="</matrix>\n";
	return s;
}

QString Matrix::saveAsTemplate(const QString &info)
{
	QString s= "<matrix>\t";
	s+= QString::number(numRows())+"\t";
	s+= QString::number(numCols())+"\n";
	s+= info;
	s+= "ColWidth\t" + QString::number(d_table->columnWidth(0))+"\n";
	s+= "<formula>\n" + formula_str + "\n</formula>\n";
	s+= "TextFormat\t" + QString(txt_format) + "\t" + QString::number(num_precision) + "\n";
	s+= "Coordinates\t" + QString::number(x_start,'g',15) + "\t" +QString::number(x_end,'g',15) + "\t";
	s+= QString::number(y_start,'g',15) + "\t" + QString::number(y_end,'g',15) + "\n";
	return s;
}

void Matrix::restore(const QStringList &lst)
{
	QStringList l;
	QStringList::const_iterator i = lst.begin();

	l= (*i++).split("\t");
	setColumnsWidth(l[1].toInt());

	l= (*i++).split("\t");
	if (l[0] == "Formula")
		formula_str = l[1];
	else if (l[0] == "<formula>")
	{
		for(formula_str=""; i != lst.end() && *i != "</formula>"; i++)
			formula_str += *i + "\n";
		formula_str.truncate(formula_str.length()-1);
		i++;
	}

	l= (*i++).split("\t");
	if (l[1] == "f")
		setTextFormat('f', l[2].toInt());
	else
		setTextFormat('e', l[2].toInt());

	l= (*i++).split("\t");
	x_start = l[1].toDouble();
	x_end = l[2].toDouble();
	y_start = l[3].toDouble();
	y_end = l[4].toDouble();
}

QString Matrix::saveText()
{
	QString out_text = "<data>\n";
	int cols = d_table->columnCount() - 1;
	for(int i=0; i<d_table->rowCount(); i++)
	{
		if (!isEmptyRow(i))
		{
			out_text += QString::number(i)+"\t";
			for(int j=0; j<cols; j++)
				out_text += text(i, j) + "\t";

			out_text += text(i, cols)+"\n";
		}
	}
	return out_text + "</data>\n";
}

void Matrix::setFormula(const QString &s)
{
	formula_str = s;
}

QString Matrix::formula()
{
	return formula_str;
}

// TODO: Port this class to the model/view framework and make
// sure the diplayed precision and the saved precision
// are independent
void Matrix::setNumericFormat(const QChar& f, int prec)
{
	if (txt_format == f && num_precision == prec)
		return;

	allow_modification_signals = false;
    d_table->blockSignals(true);
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	txt_format = f;
	num_precision = prec;

	int rows = numRows();
	int cols = numCols();
	for(int i=0; i<rows; i++)
	{
		for(int j=0; j<cols; j++)
		{
			QString t = text(i, j);
			if (!t.isEmpty())
				setCell(i, j, dMatrix[i][j]);
		}
	}
    d_table->blockSignals(false);
	allow_modification_signals = true;
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Matrix::setTextFormat(const QChar &format, int precision)
{
	txt_format = format;
	num_precision = precision;
}

int Matrix::columnsWidth()
{
	return d_table->columnWidth(0);
}

void Matrix::setColumnsWidth(int width)
{
	if (width == columnsWidth())
		return;

	for(int i=0; i<d_table->columnCount(); i++)
		d_table->setColumnWidth(i, width);

	emit modifiedWindow(this);
}

void Matrix::setMatrixDimensions(int rows, int cols)
{
	int r = numRows();
	int c = numCols();

	if (r == rows && c == cols)
		return;

	if (rows < r || cols < c)
	{
		QString msg_text = tr("Deleting rows/columns from the matrix!","set matrix dimensions");
		msg_text += tr("<p>Do you really want to continue?","set matrix dimensions");
		switch( QMessageBox::information(0,tr("QtiPlot"), msg_text,tr("Yes"), tr("Cancel"), 0, 1 ) )
		{
			case 0: // Yes
				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				if (cols != c)
					d_table->setColumnCount(cols);
				if (rows != r)
					d_table->setRowCount(rows);
				QApplication::restoreOverrideCursor();
				emit modifiedWindow(this);
				break;

			case 1: // Cancel
				return;
				break;
		}
	}
	else
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		if (cols != c)
			d_table->setColumnCount(cols);
		if (rows != r)
			d_table->setRowCount(rows);
		QApplication::restoreOverrideCursor();
		emit modifiedWindow(this);
	}
}

int Matrix::numRows()
{
	return d_table->rowCount();
}

int Matrix::numCols()
{
	return d_table->columnCount();
}

double Matrix::determinant()
{
	int rows = numRows();
	int cols = numCols();

	if (rows != cols)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),
				tr("Calculation failed, the matrix is not square!"));
		return GSL_POSINF;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	gsl_matrix *A = gsl_matrix_alloc(rows, cols);
	int i, j;
	for(i=0; i<rows; i++)
	{
		for(j=0; j<cols; j++)
		{
			QString s = text(i,j);
			gsl_matrix_set(A, i, j, s.toDouble());
		}
	}

	gsl_permutation * p = gsl_permutation_alloc(rows);
	gsl_linalg_LU_decomp(A, p, &i);

	double det = gsl_linalg_LU_det(A, i);

	gsl_matrix_free(A);
	gsl_permutation_free(p);

	QApplication::restoreOverrideCursor();
	return det;
}

void Matrix::invert()
{
	allow_modification_signals = false;
	int rows = numRows();
	int cols = numCols();

	if (rows != cols)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),
				tr("Inversion failed, the matrix is not square!"));
		allow_modification_signals = true;
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	gsl_matrix *A = gsl_matrix_alloc(rows, cols);
	int i, j;
	for(i=0; i<rows; i++)
	{
		for(j=0; j<cols; j++)
		{
			QString s = text(i,j);
			gsl_matrix_set(A, i, j, s.toDouble());
		}
	}

	gsl_permutation * p = gsl_permutation_alloc(cols);
	gsl_linalg_LU_decomp(A, p, &i);

	gsl_matrix *inverse = gsl_matrix_alloc(rows, cols);
	gsl_linalg_LU_invert(A, p, inverse);

	gsl_matrix_free(A);
	gsl_permutation_free(p);

	for(i=0; i<rows; i++)
	{
		for(j=0; j<cols; j++)
			setCell(i, j, gsl_matrix_get(inverse, i, j));
	}

	gsl_matrix_free(inverse);
	QApplication::restoreOverrideCursor();
	allow_modification_signals = true;
	emit modifiedWindow(this);
}

// TODO: Mirror matrix horizontally/vertically would also be nice
void Matrix::transpose()
{
	allow_modification_signals = false;

	int i, j;
	int rows = numRows();
	int cols = numCols();
	int temp_size = qMax(rows, cols);
	QString temp;

	// blow up matrix to a square one
	d_table->setColumnCount(temp_size);
	d_table->setRowCount(temp_size);

	for(i = 0; i<temp_size; i++)
		for(j = 0; j<=i; j++)
		{
			temp = text(i,j);
			setText(i, j, text(j,i));
			setText(j, i, temp);
		}

	// shrink matrix to desired size
	d_table->setColumnCount(rows);
	d_table->setRowCount(cols);
	allow_modification_signals = true;
	emit modifiedWindow(this);
}

void Matrix::saveCellsToMemory()
{
	int rows = numRows();
	int cols = numCols();
	dMatrix = allocateMatrixData(rows, cols);
	for(int i=0; i<rows; i++)
	{// initialize the matrix to zero
		for(int j=0; j<cols; j++)
			dMatrix[i][j] = 0.0;
	}

    bool ok = true;
	for (int i=0; i<rows; i++)
	{
        for (int j=0; j<cols; j++)
        {
            dMatrix[i][j] = QLocale().toDouble(text(i, j), &ok);
            if (!ok)
                break;
        }
	}
	if (!ok){// fall back to C locale
	    ok = true;
        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                dMatrix[i][j] = QLocale::c().toDouble(text(i, j), &ok);
                if (!ok)
                    break;
            }
        }
	}
	if (!ok){// fall back to German locale
	    ok = true;
        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                dMatrix[i][j] = QLocale(QLocale::German).toDouble(text(i, j), &ok);
                if (!ok)
                    break;
            }
        }
	}
	if (!ok){// fall back to French locale
	    ok = true;
        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                dMatrix[i][j] = QLocale(QLocale::French).toDouble(text(i, j), &ok);
                if (!ok)
                    break;
            }
        }
	}
}

void Matrix::forgetSavedCells()
{
	freeMatrixData(dMatrix, numRows());
	dMatrix = 0;
}

bool Matrix::calculate(int startRow, int endRow, int startCol, int endCol)
{
	allow_modification_signals = false;
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	Script *script = scriptEnv->newScript(formula_str, this, QString("<%1>").arg(name()));
	connect(script, SIGNAL(error(const QString&,const QString&,int)), scriptEnv, SIGNAL(error(const QString&,const QString&,int)));
	connect(script, SIGNAL(print(const QString&)), scriptEnv, SIGNAL(print(const QString&)));
	if (!script->compile())
	{
		allow_modification_signals = true;
		QApplication::restoreOverrideCursor();
		return false;
	}

    d_table->blockSignals(true);
	int rows = numRows();
	int cols = numCols();

	if (endRow < 0)
		endRow = rows - 1;
	if (endCol < 0)
		endCol = cols - 1;
	if (endCol >= cols)
		d_table->setColumnCount(endCol+1);
	if (endRow >= rows)
		d_table->setRowCount(endRow+1);

	QVariant ret;
	saveCellsToMemory();
	double dx = fabs(x_end-x_start)/(double)(numRows()-1);
	double dy = fabs(y_end-y_start)/(double)(numCols()-1);
	for(int row = startRow; row <= endRow; row++)
		for(int col = startCol; col <= endCol; col++)
		{
			script->setInt(row+1, "i");
			script->setInt(row+1, "row");
			script->setDouble(y_start+row*dy, "y");
			script->setInt(col+1, "j");
			script->setInt(col+1, "col");
			script->setDouble(x_start+col*dx, "x");
			ret = script->eval();
			if (ret.type()==QVariant::Int || ret.type()==QVariant::UInt || ret.type()==QVariant::LongLong
					|| ret.type()==QVariant::ULongLong)
				setText(row, col, ret.toString());
			else if (ret.canConvert(QVariant::Double))
				setText(row, col, QLocale().toString(ret.toDouble(), txt_format.toAscii(), num_precision));
			else
			{
				setText(row, col, "");
				allow_modification_signals = true;
				QApplication::restoreOverrideCursor();
				return false;
			}
		}
	forgetSavedCells();

	allow_modification_signals = true;
    d_table->blockSignals(false);
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
	return true;
}

void Matrix::clearSelection()
{
	allow_modification_signals = false;

	QList<QTableWidgetSelectionRange> sel = d_table->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if( it.hasNext() )
	{
		cur = it.next();
		for(int i = cur.topRow(); i <= cur.bottomRow(); i++)
			for(int j = cur.leftColumn(); j<= cur.rightColumn();j++)
				setText(i, j, "");
	}

	allow_modification_signals = true;
	emit modifiedWindow(this);
}


void Matrix::copySelection()
{
	QString the_text;
	QList<QTableWidgetSelectionRange> sel = d_table->selectedRanges();
	if (sel.isEmpty())
		the_text = text(d_table->currentRow(),d_table->currentColumn());
	else
	{
		QListIterator<QTableWidgetSelectionRange> it(sel);
		QTableWidgetSelectionRange cur;

		if(!it.hasNext())return;
		cur = it.next();

		int top = cur.topRow();
		int bottom = cur.bottomRow();
		int left = cur.leftColumn();
		int right = cur.rightColumn();
		for(int i=top; i<=bottom; i++)
		{
			for(int j=left; j<right; j++)
				the_text += text(i,j)+"\t";
			the_text += text(i,right)+"\n";
		}
	}

	// Copy text into the clipboard
	QApplication::clipboard()->setText(the_text);
}

void Matrix::cutSelection()
{
	copySelection();
	clearSelection();
}

bool Matrix::rowsSelected()
{
	QList<QTableWidgetSelectionRange> sel = d_table->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if( it.hasNext() )
	{
		cur = it.next();
		for(int i=cur.topRow(); i<=cur.bottomRow(); i++)
		{
			if (!isRowSelected (i, true))
				return false;
		}
	}
	return true;
}

void Matrix::deleteSelectedRows()
{
	QVarLengthArray<int> rows(1);
	int n=0;
	for (int i=0; i<numRows(); i++)
	{
		if (isRowSelected(i, true))
		{
			n++;
			rows.resize(n);
			rows[n-1]= i;
		}
	}

	// rows need to be removed from bottom to top
	for(int i=rows.count()-1; i>=0; i--)
		d_table->removeRow(rows[i]);
	emit modifiedWindow(this);
}

// TODO: Maybe change this to insertColumns and make the
// number of inserted columns depend on the number
// of selected columns
void Matrix::insertColumn()
{
	int cc = d_table->currentColumn();
	d_table->insertColumn(cc);
	emit modifiedWindow(this);
}

bool Matrix::columnsSelected()
{
	for(int i=0; i<numCols(); i++)
	{
		if (isColumnSelected (i, true))
			return true;
	}
	return false;
}

void Matrix::deleteSelectedColumns()
{
	QVarLengthArray<int> cols(1);
	int n=0;
	for (int i=0; i<numCols(); i++)
	{
		if (isColumnSelected(i, true))
		{
			n++;
			cols.resize(n);
			cols[n-1]= i;
		}
	}

	// columns need to be removed from right to left
	for(int i=cols.count()-1; i>=0; i--)
		d_table->removeColumn(cols[i]);
	emit modifiedWindow(this);
}

int Matrix::numSelectedRows()
{
	int r=0;
	for(int i=0; i<numRows(); i++)
		if (isRowSelected(i, true))
			r++;
	return r;
}

int Matrix::numSelectedColumns()
{
	int c=0;
	for(int i=0; i<numCols(); i++)
		if (isColumnSelected(i, true))
			c++;
	return c;
}

// TODO: Maybe change this to insertRows and make the
// number of inserted rows depend on the number
// of selected row
void Matrix::insertRow()
{
	int cr = d_table->currentRow();
	d_table->insertRow(cr);
	emit modifiedWindow(this);
}

void Matrix::pasteSelection()
{
	QString the_text = QApplication::clipboard()->text();
	if (the_text.isEmpty())
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QTextStream ts( &the_text, QIODevice::ReadOnly );
	QString s = ts.readLine();
	QStringList cellTexts = s.split("\t");
	int cols = cellTexts.count();
	int rows = 1;
	while(!ts.atEnd()){
		rows++;
		s = ts.readLine();
	}
	ts.reset();

	QList<QTableWidgetSelectionRange> sel = d_table->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

    int top, left, firstCol;
	if (!sel.isEmpty()){
		cur = it.next();
		top = cur.topRow();
		left = cur.leftColumn();
	} else {
		top = 0;
		left = 0;

		firstCol = firstSelectedColumn();

		if (firstCol >= 0){ // columns are selected
			left = firstCol;
			int selectedColsNumber = 0;
			for(int i=0; i<numCols(); i++) {
				if (isColumnSelected(i, true))
					selectedColsNumber++;
			}
		}
	}

	QTextStream ts2( &the_text, QIODevice::ReadOnly );

    d_table->blockSignals(true);
	if (top + rows > d_table->rowCount())
        d_table->setRowCount(top + rows);
    if (left + cols > d_table->columnCount())
        d_table->setColumnCount(left + cols);

	bool numeric;
	QLocale system_locale = QLocale::system();
	for(int i=top; i<top+rows; i++){
		s = ts2.readLine();
		cellTexts=s.split("\t");
		for(int j=left; j<left+cols; j++){
			double value = system_locale.toDouble(cellTexts[j-left], &numeric);
			if (numeric)
				setText(i, j, QLocale().toString(value, txt_format.toAscii(), num_precision));
			else
				setText(i, j, cellTexts[j-left]);
		}
	}

	d_table->blockSignals(false);
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Matrix::contextMenuEvent(QContextMenuEvent *e)
{
	emit showContextMenu();
	e->accept();
}

void Matrix::customEvent(QEvent *e)
{
	if (e->type() == SCRIPTING_CHANGE_EVENT)
		scriptingChangeEvent((ScriptingChangeEvent*)e);
}

bool Matrix::eventFilter(QObject *object, QEvent *e)
{
	if (e->type()==QEvent::ContextMenu && object == titleBar)
	{
		emit showTitleBarMenu();
		((QContextMenuEvent*)e)->accept();
		return true;
	}

	return MyWidget::eventFilter(object, e);
}

void Matrix::exportPDF(const QString& fileName)
{
	print(fileName);
}

void Matrix::print()
{
    print(QString());
}

void Matrix::print(const QString& fileName)
{
	QPrinter printer;
	printer.setColorMode (QPrinter::GrayScale);

	if (!fileName.isEmpty())
	{
	    printer.setCreator("QtiPlot");
	    printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
	}
    else
    {
        QPrintDialog printDialog(&printer);
        if (printDialog.exec() != QDialog::Accepted)
            return;
    }
		printer.setFullPage( true );
		QPainter p;
		if ( !p.begin(&printer ) )
			return; // paint on printer
		int dpiy = printer.logicalDpiY();
		const int margin = (int) ( (1/2.54)*dpiy ); // 1 cm margins

		QHeaderView *vHeader = d_table->verticalHeader();

		int rows = numRows();
		int cols = numCols();
		int height = margin;
		int i, vertHeaderWidth = vHeader->width();
		int right = margin + vertHeaderWidth;

		// print header
		p.setFont(QFont());
		QString header_label = d_table->model()->headerData(0, Qt::Horizontal).toString();
		QRect br = p.boundingRect(br, Qt::AlignCenter, header_label);
		p.drawLine(right, height, right, height+br.height());
		QRect tr(br);

		for(i=0;i<cols;i++)
		{
			int w = d_table->columnWidth(i);
			tr.setTopLeft(QPoint(right,height));
			tr.setWidth(w);
			tr.setHeight(br.height());
			header_label = d_table->model()->headerData(i, Qt::Horizontal).toString();
			p.drawText(tr, Qt::AlignCenter, header_label,-1);
			right += w;
			p.drawLine(right, height, right, height+tr.height());

			if (right >= printer.width()-2*margin )
				break;
		}

		p.drawLine(margin + vertHeaderWidth, height, right-1, height);//first horizontal line
		height += tr.height();
		p.drawLine(margin, height, right-1, height);

		// print table values
		for(i=0;i<rows;i++)
		{
			right = margin;
			QString cell_text = d_table->model()->headerData(i, Qt::Horizontal).toString()+"\t";
			tr = p.boundingRect(tr, Qt::AlignCenter, cell_text);
			p.drawLine(right, height, right, height+tr.height());

			br.setTopLeft(QPoint(right,height));
			br.setWidth(vertHeaderWidth);
			br.setHeight(tr.height());
			p.drawText(br,Qt::AlignCenter,cell_text,-1);
			right += vertHeaderWidth;
			p.drawLine(right, height, right, height+tr.height());

			for(int j=0;j<cols;j++)
			{
				int w = d_table->columnWidth (j);
				cell_text = text(i,j)+"\t";
				tr = p.boundingRect(tr,Qt::AlignCenter,cell_text);
				br.setTopLeft(QPoint(right,height));
				br.setWidth(w);
				br.setHeight(tr.height());
				p.drawText(br, Qt::AlignCenter, cell_text, -1);
				right += w;
				p.drawLine(right, height, right, height+tr.height());

				if (right >= printer.width()-2*margin )
					break;
			}
			height += br.height();
			p.drawLine(margin, height, right-1, height);

			if (height >= printer.height()-margin )
			{
				printer.newPage();
				height = margin;
				p.drawLine(margin, height, right, height);
			}
		}
}

void Matrix::range(double *min, double *max)
{
	double d_min = cell(0, 0);
	double d_max = d_min;

	for(int i=0; i<numRows(); i++)
	{
		for(int j=0; j<numCols(); j++)
		{
			double aux = cell(i, j);
			if (aux <= d_min)
				d_min = aux;

			if (aux >= d_max)
				d_max = aux;
		}
	}

	*min = d_min;
	*max = d_max;
}

bool Matrix::isColumnSelected(int col, bool full)
{
	QList<QTableWidgetSelectionRange> sel = d_table->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if ( !full )
	{
		if( it.hasNext() )
		{
			cur = it.next();
			if ( (col >= cur.leftColumn()) && (col <= cur.rightColumn() ) )
				return true;
		}
	}
	else
	{
		if( it.hasNext() )
		{
			cur = it.next();
			if ( col >= cur.leftColumn() &&
					col <= cur.rightColumn() &&
					cur.topRow() == 0 &&
					cur.bottomRow() == numRows() - 1 )
				return true;
		}
	}
	return false;
}

bool Matrix::isRowSelected(int row, bool full)
{
	QList<QTableWidgetSelectionRange> sel = d_table->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if ( !full )
	{
		if( it.hasNext() )
		{
			cur = it.next();
			if ( (row >= cur.topRow()) && (row <= cur.bottomRow() ) )
				return true;
		}
	}
	else
	{
		if( it.hasNext() )
		{
			cur = it.next();
			if ( row >= cur.topRow() &&
					row <= cur.bottomRow() &&
					cur.leftColumn() == 0 &&
					cur.rightColumn() == numCols() - 1 )
				return true;
		}
	}
	return false;
}


int Matrix::firstSelectedColumn()
{
	for(int i=0;i<numCols();i++)
	{
		if(isColumnSelected(i,true))
			return i;
	}
	return -1;
}

double** Matrix::allocateMatrixData(int rows, int columns)
{
	double** data = new double* [rows];
	for ( int i = 0; i < rows; ++i)
	{
		data[i] = new double [columns];
	}
	return data;
}

void Matrix::freeMatrixData(double **data, int rows)
{
	for ( int i = 0; i < rows; i++)
	{
		delete [] data[i];
	}
	delete [] data;
}

// TODO: in a matrix goToCell would make more sense since rows and columns are equally important
void Matrix::goToRow(int row)
{
	if( (row < 1) || (row > numRows()) ) return;

	QTableWidgetItem * the_item = d_table->item(row-1, 0);
	if(!the_item)
	{
		the_item = new QTableWidgetItem("");
		d_table->setItem(row-1, 0, the_item);
	}

	d_table->scrollToItem(the_item);
	d_table->selectRow(row-1);
}

void Matrix::updateDecimalSeparators()
{
    saveCellsToMemory();

    d_table->blockSignals(true);
    for(int i=0; i<d_table->rowCount(); i++)
	{
		for(int j=0; j<d_table->columnCount(); j++)
		{
			if (!text(i, j).isEmpty())
				setCell(i, j, dMatrix[i][j]);
		}
	}
    d_table->blockSignals(false);
    forgetSavedCells();
}
