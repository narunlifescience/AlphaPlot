/***************************************************************************
    File                 : Matrix.cpp
    Project              : SciDAVis
    Description          : Matrix worksheet class
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2006-2009 Knut Franke (knut.franke*gmx.de)
    Copyright            : (C) 2006-2007 Ion Vasilief (ion_vasilief*yahoo.fr)
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
#include "Matrix.h"
#include "future/matrix/MatrixView.h"
#include "ScriptEdit.h"

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
#include <QXmlStreamWriter>
#include <QtDebug>

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>

Matrix::Matrix(ScriptingEnv *env, int r, int c, const QString& label, QWidget* parent, const char* name, Qt::WFlags f)
	: MatrixView(label, parent, name, f), scripted(env)
{
	d_future_matrix = new future::Matrix(0, r, c, label);
	init(r, c);
}
	
Matrix::Matrix(future::Matrix *future_matrix, ScriptingEnv *env, int r, int c, const QString& label, QWidget* parent, const char* name, Qt::WFlags f)
	: MatrixView(label, parent, name, f), scripted(env)
{
	d_future_matrix = future_matrix;
	init(r, c);
}

void Matrix::init(int rows, int cols)
{	
	MatrixView::setMatrix(d_future_matrix);	
	d_future_matrix->setView(this);	
	d_future_matrix->setNumericFormat('f');
	d_future_matrix->setDisplayedDigits(6);
	d_future_matrix->setCoordinates(1.0, 10.0, 1.0, 10.0);
	dMatrix = 0;

	birthdate = d_future_matrix->creationTime().toString(Qt::LocalDate);

	// this is not very nice but works for the moment
	ui.gridLayout2->removeWidget(ui.formula_box);
	delete ui.formula_box;
    ui.formula_box = new ScriptEdit(scriptEnv, ui.formula_tab);
    ui.formula_box->setObjectName(QString::fromUtf8("formula_box"));
    ui.formula_box->setMinimumSize(QSize(60, 10));
    ui.formula_box->setAcceptRichText(false);
    ui.formula_box->setLineWrapMode(QTextEdit::WidgetWidth);
	ui.gridLayout2->addWidget(ui.formula_box, 1, 0, 1, 3);

	ui.add_cell_combobox->addItem("cell(i, j)");
	ui.add_function_combobox->addItems(scriptEnv->mathFunctions());
	updateFunctionDoc();
	
	connect(ui.add_function_combobox, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(updateFunctionDoc()));
	connect(ui.button_set_formula, SIGNAL(pressed()), 
		this, SLOT(applyFormula()));
	connect(ui.add_function_button, SIGNAL(pressed()), 
		this, SLOT(addFunction()));
	connect(ui.add_cell_button, SIGNAL(pressed()), 
		this, SLOT(addCell()));

	connect(d_future_matrix, SIGNAL(columnsInserted(int, int)), this, SLOT(handleChange()));
	connect(d_future_matrix, SIGNAL(columnsRemoved(int, int)), this, SLOT(handleChange()));
	connect(d_future_matrix, SIGNAL(rowsInserted(int, int)), this, SLOT(handleChange()));
	connect(d_future_matrix, SIGNAL(rowsRemoved(int, int)), this, SLOT(handleChange()));
	connect(d_future_matrix, SIGNAL(dataChanged(int, int, int, int)), this, SLOT(handleChange()));
	connect(d_future_matrix, SIGNAL(coordinatesChanged()), this, SLOT(handleChange()));
	connect(d_future_matrix, SIGNAL(formulaChanged()), this, SLOT(handleChange()));
	connect(d_future_matrix, SIGNAL(formatChanged()), this, SLOT(handleChange()));
	connect(d_future_matrix, SIGNAL(recalculate()), this, SLOT(recalculate()));

	connect(d_future_matrix, SIGNAL(aspectDescriptionChanged(const AbstractAspect*)), 
			this, SLOT(handleAspectDescriptionChange(const AbstractAspect *)));
}

Matrix::~Matrix()
{
}

void Matrix::handleChange()
{
    emit modifiedWindow(this);
}

double Matrix::cell(int row, int col)
{
	if(dMatrix)
		return dMatrix[row][col];
	else 
		return d_future_matrix->cell(row, col);
}

void Matrix::setCell(int row, int col, double value)
{
	d_future_matrix->setCell(row, col, value);
}

QString Matrix::text(int row, int col)
{
	return d_future_matrix->text(row, col);
}

void Matrix::setText(int row, int col, const QString & new_text )
{
	bool ok = true;
    QLocale locale;
  	double res = locale.toDouble(new_text, &ok);
	if (ok)
		d_future_matrix->setCell(row, col, res);
	else
	{
		Script *script = scriptEnv->newScript(new_text, this, QString("<%1_%2_%3>").arg(name()).arg(row).arg(col));
		connect(script, SIGNAL(error(const QString&,const QString&,int)), scriptEnv, SIGNAL(error(const QString&,const QString&,int)));

		script->setInt(row+1, "row");
		script->setInt(row+1, "i");
		script->setInt(col+1, "col");
		script->setInt(col+1, "j");

		QVariant ret = script->eval();
		setCell(row, col, ret.toDouble());
	}
}

void Matrix::setCoordinates(double xs, double xe, double ys, double ye)
{
	d_future_matrix->setCoordinates(xs, xe, ys, ye);
}

QString Matrix::saveToString(const QString &geometry)
{
	QString s = "<matrix>\n";
	QString xml;
	QXmlStreamWriter writer(&xml);
	d_future_matrix->save(&writer);
	s += QString::number(xml.length()) + "\n"; // this is need in case there are newlines in the XML
	s += xml + "\n";
	s += geometry + "\n";
	s +="</matrix>\n";
	return s;

#if 0
	QString s = "<matrix>\n";
	s += QString(name()) + "\t";
	s += QString::number(numRows())+"\t";
	s += QString::number(numCols())+"\t";
	s += birthDate() + "\n";
	s += info;
	s += "ColWidth\t" + QString::number(columnWidth(0))+"\n";
	s += "<formula>\n" + formula() + "\n</formula>\n";
	s += "TextFormat\t" + QString(d_future_matrix->numericFormat()) + "\t" + QString::number(d_future_matrix->displayedDigits()) + "\n";
	s += "WindowLabel\t" + windowLabel() + "\t" + QString::number(captionPolicy()) + "\n";
	s += "Coordinates\t" + QString::number(xStart(),'g',15) + "\t" +QString::number(xEnd(),'g',15) + "\t";
	s += QString::number(yStart(),'g',15) + "\t" + QString::number(yEnd(),'g',15) + "\n";
	s += saveText();
	s +="</matrix>\n";
	return s;
#endif
}

QString Matrix::saveAsTemplate(const QString &info)
{
	QString s= "<matrix>\t";
	s+= QString::number(numRows())+"\t";
	s+= QString::number(numCols())+"\n";
	s+= info;
	s+= "ColWidth\t" + QString::number(columnWidth(0))+"\n";
	s+= "<formula>\n" + formula() + "\n</formula>\n";
	s+= "TextFormat\t" + QString(d_future_matrix->numericFormat()) + "\t" + QString::number(d_future_matrix->displayedDigits()) + "\n";
	s+= "Coordinates\t" + QString::number(xStart(),'g',15) + "\t" +QString::number(xEnd(),'g',15) + "\t";
	s+= QString::number(yStart(),'g',15) + "\t" + QString::number(yEnd(),'g',15) + "\n";
	s +="</matrix>\n";
	return s;
}

QString Matrix::saveText()
{
	QString out_text = "<data>\n";
	int cols = d_future_matrix->columnCount();
	for(int i=0; i<d_future_matrix->rowCount(); i++)
	{
		out_text += QString::number(i)+"\t";
		for (int j=0; j<cols-1; j++)
			out_text += QString::number(cell(i,j), 'e', 16)+"\t";

		out_text += QString::number(cell(i,cols-1), 'e', 16)+"\n";
	}
	return out_text + "</data>\n";
}

void Matrix::setFormula(const QString &s)
{
	d_future_matrix->setFormula(s);
}

QString Matrix::formula()
{
	return d_future_matrix->formula();
}

void Matrix::setNumericFormat(const QChar& f, int prec)
{
	if (d_future_matrix->numericFormat() == f && d_future_matrix->displayedDigits() == prec)
		return;

	d_future_matrix->setNumericFormat(f.toAscii());
	d_future_matrix->setDisplayedDigits(prec);

	emit modifiedWindow(this);
}

void Matrix::setTextFormat(const QChar &format, int precision)
{
	d_future_matrix->setNumericFormat(format.toAscii());
	d_future_matrix->setDisplayedDigits(precision);
}

void Matrix::setColumnsWidth(int width)
{
	for(int i=0; i<d_future_matrix->columnCount(); i++)
		setColumnWidth(i, width);
}

void Matrix::setDimensions(int rows, int cols)
{
	d_future_matrix->setDimensions(rows, cols);
}

int Matrix::numRows()
{
	return d_future_matrix->rowCount();
}

void Matrix::setNumRows(int rows)
{
	d_future_matrix->setDimensions(rows, d_future_matrix->columnCount());
}

int Matrix::numCols()
{
	return d_future_matrix->columnCount();
}

void Matrix::setNumCols(int cols)
{
	d_future_matrix->setDimensions(d_future_matrix->rowCount(), cols);
}

double Matrix::determinant()
{
	int rows = numRows();
	int cols = numCols();

	if (rows != cols){
		QMessageBox::critical(0,tr("Error"),
				tr("Calculation failed, the matrix is not square!"));
		return GSL_POSINF;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	gsl_matrix *A = gsl_matrix_alloc(rows, cols);
	int i;
	for(i=0; i<rows; i++)
		for(int j=0; j<cols; j++)
			gsl_matrix_set(A, i, j, cell(i, j));

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
	int rows = numRows();
	int cols = numCols();

	if (rows != cols){
		QMessageBox::critical(0,tr("Error"),
				tr("Inversion failed, the matrix is not square!"));
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	gsl_matrix *A = gsl_matrix_alloc(rows, cols);
	int i;
	for(i=0; i<rows; i++){
		for(int j=0; j<cols; j++)
			gsl_matrix_set(A, i, j, cell(i, j));
	}

	gsl_permutation * p = gsl_permutation_alloc(cols);
	gsl_linalg_LU_decomp(A, p, &i);

	gsl_matrix *inverse = gsl_matrix_alloc(rows, cols);
	gsl_linalg_LU_invert(A, p, inverse);

	gsl_matrix_free(A);
	gsl_permutation_free(p);

    this->blockSignals(true);
	for(i=0; i<rows; i++){
		for(int j=0; j<cols; j++)
			setCell(i, j, gsl_matrix_get(inverse, i, j));
	}
    this->blockSignals(false);

	gsl_matrix_free(inverse);
	QApplication::restoreOverrideCursor();
	emit modifiedWindow(this);
}

void Matrix::transpose()
{
	d_future_matrix->transpose();
}

void Matrix::saveCellsToMemory()
{
	int rows = numRows();
	int cols = numCols();
	dMatrix = allocateMatrixData(rows, cols);
	for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
            dMatrix[i][j] = d_future_matrix->cell(i, j);
}

void Matrix::forgetSavedCells()
{
	freeMatrixData(dMatrix, numRows());
	dMatrix = 0;
}

bool Matrix::recalculate()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	Script *script = scriptEnv->newScript(formula(), this, QString("<%1>").arg(name()));
	connect(script, SIGNAL(error(const QString&,const QString&,int)), scriptEnv, SIGNAL(error(const QString&,const QString&,int)));
	connect(script, SIGNAL(print(const QString&)), scriptEnv, SIGNAL(print(const QString&)));
	if (!script->compile())
	{
		QApplication::restoreOverrideCursor();
		return false;
	}

    this->blockSignals(true);

	int startRow = firstSelectedRow(false);
	int	endRow = lastSelectedRow(false);
	int startCol = firstSelectedColumn(false);
	int	endCol = lastSelectedColumn(false);

	QVariant ret;
	saveCellsToMemory();
	double dx = fabs(xEnd()-xStart())/(double)(numRows()-1);
	double dy = fabs(yEnd()-yStart())/(double)(numCols()-1);
	for(int row = startRow; row <= endRow; row++)
		for(int col = startCol; col <= endCol; col++)
		{
			if (!isCellSelected(row, col)) continue;
			script->setInt(row+1, "i");
			script->setInt(row+1, "row");
			script->setDouble(yStart()+row*dy, "y");
			script->setInt(col+1, "j");
			script->setInt(col+1, "col");
			script->setDouble(xStart()+col*dx, "x");
			ret = script->eval();
			if (!ret.isValid()) {
				forgetSavedCells();
				blockSignals(false);
				emit modifiedWindow(this);
				QApplication::restoreOverrideCursor();
				return false;
			}
			setCell(row, col, ret.toDouble());
		}
	forgetSavedCells();

	blockSignals(false);
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
	return true;
}

void Matrix::clearSelection()
{
	d_future_matrix->clearSelectedCells();
}

void Matrix::copySelection()
{
	d_future_matrix->copySelection();
}

void Matrix::cutSelection()
{
	copySelection();
	clearSelection();
}

bool Matrix::rowsSelected()
{
	for(int i=0; i<numRows(); i++)
	{
		if (this->isRowSelected (i, true))
			return true;
	}
	return false;
}

void Matrix::deleteSelectedRows()
{
	d_future_matrix->removeSelectedRows();
}

void Matrix::insertColumn()
{
	d_future_matrix->insertEmptyColumns();
}

bool Matrix::columnsSelected()
{
	for(int i=0; i<numCols(); i++)
	{
		if (this->isColumnSelected (i, true))
			return true;
	}
	return false;
}

void Matrix::deleteSelectedColumns()
{
	d_future_matrix->removeSelectedColumns();
}

int Matrix::numSelectedRows()
{
	int r=0;
	for(int i=0; i<numRows(); i++)
		if (this->isRowSelected(i, true))
			r++;
	return r;
}

int Matrix::numSelectedColumns()
{
	int c=0;
	for(int i=0; i<numCols(); i++)
		if (this->isColumnSelected(i, true))
			c++;
	return c;
}

void Matrix::insertRow()
{
	d_future_matrix->insertEmptyRows();
}

void Matrix::pasteSelection()
{
	d_future_matrix->pasteIntoSelection();
}

void Matrix::customEvent(QEvent *e)
{
	if (e->type() == SCRIPTING_CHANGE_EVENT)
		scriptingChangeEvent((ScriptingChangeEvent*)e);
}

void Matrix::closeEvent( QCloseEvent *e )
{
	if (askOnClose)
	{
		switch( QMessageBox::information(this,tr("SciDAVis"),
					tr("Do you want to hide or delete") + "<p><b>'" + objectName() + "'</b> ?",
					tr("Delete"), tr("Hide"), tr("Cancel"), 0,2))
		{
			case 0:
				e->accept();
				d_future_matrix->remove();
				return;

			case 1:
				e->ignore();
				emit hiddenWindow(this);
				break;

			case 2:
				e->ignore();
				break;
		}
	}
	else
	{
		e->accept();
		d_future_matrix->remove();
		return;
	}
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
	printer.setColorMode(QPrinter::GrayScale);

	if (!fileName.isEmpty())
	{
	    printer.setCreator("SciDAVis");
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
	if ( !p.begin(&printer) )
		return; // paint on printer
	int dpiy = printer.logicalDpiY();
	const int margin = (int) ( (1/2.54)*dpiy ); // 1 cm margins

	QHeaderView *vHeader = d_view_widget->verticalHeader();

	int rows = numRows();
	int cols = numCols();
	int height = margin;
	int i, vertHeaderWidth = vHeader->width();
	int right = margin + vertHeaderWidth;

	// print header
	p.setFont(QFont());
	QString header_label = d_view_widget->model()->headerData(0, Qt::Horizontal).toString();
	QRect br = p.boundingRect(br, Qt::AlignCenter, header_label);
	p.drawLine(right, height, right, height+br.height());
	QRect tr(br);

	for(i=0;i<cols;i++)
	{
		int w = columnWidth(i);
		tr.setTopLeft(QPoint(right,height));
		tr.setWidth(w);
		tr.setHeight(br.height());
		header_label = d_view_widget->model()->headerData(i, Qt::Horizontal).toString();
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
		QString cell_text = d_view_widget->model()->headerData(i, Qt::Vertical).toString()+"\t";
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
			int w = columnWidth (j);
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

double** Matrix::allocateMatrixData(int rows, int columns)
{
	double** data = new double* [rows];
	for ( int i = 0; i < rows; ++i)
		data[i] = new double [columns];

	return data;
}

void Matrix::freeMatrixData(double **data, int rows)
{
	for ( int i = 0; i < rows; i++)
		delete [] data[i];

	delete [] data;
}

void Matrix::updateDecimalSeparators()
{
	this->update();
}

void Matrix::copy(Matrix *m)
{
	if (!m)
        return;
	
	d_future_matrix->copy(m->d_future_matrix);
}

Matrix * Matrix::fromImage(const QImage & image, ScriptingEnv *env)
{
	future::Matrix * fm = future::Matrix::fromImage(image);
	if (!fm) return NULL;
	return new Matrix(fm, env, image.height(), image.width(), tr("Matrix %1").arg(1));
}

void Matrix::applyFormula()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	d_future_matrix->beginMacro(tr("%1: apply formula to selection").arg(name()));

	setFormula(ui.formula_box->toPlainText());
	recalculate();

	d_future_matrix->endMacro();
	QApplication::restoreOverrideCursor();
}

void Matrix::addFunction()
{
	static_cast<ScriptEdit *>(ui.formula_box)->insertFunction(ui.add_function_combobox->currentText());
}

void Matrix::addCell()
{
	ui.formula_box->insertPlainText(ui.add_cell_combobox->currentText());
}

void Matrix::updateFunctionDoc()
{
	ui.add_function_combobox->setToolTip(scriptEnv->mathFunctionDoc(ui.add_function_combobox->currentText()));
}

void Matrix::handleAspectDescriptionChange(const AbstractAspect *aspect)
{
	if (aspect != d_future_matrix) return;
	setObjectName(d_future_matrix->name());
	updateCaption();
}



