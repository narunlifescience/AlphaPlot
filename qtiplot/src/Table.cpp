/***************************************************************************
    File                 : Table.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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
#include "Table.h"
#include "SortDialog.h"

#include <QList>
#include <QEvent>
#include <QVarLengthArray>
#include <QDateTime>
#include <QApplication>
#include <QShortcut>
#include <QMessageBox>
#include <QTableWidgetSelectionRange>
#include <QApplication>
#include <QClipboard>
#include <QTextStream>
#include <QFile>
#include <QProgressDialog>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QGridLayout>
#include <QSizePolicy>
#include <QString>
#include <QPrintDialog>
#include <QStandardItemModel>
#include <QLocale>
#include <QPainter>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>

Table::Table(ScriptingEnv *env, const QString &fname,const QString &sep, int ignoredLines, bool renameCols,
		bool stripSpaces, bool simplifySpaces, const QString& label,
		QWidget* parent, const char* name, Qt::WFlags f)
: MyWidget(label, parent,name,f), scripted(env)
{
	importASCII(fname, sep, ignoredLines, renameCols, stripSpaces, simplifySpaces, true);
}

Table::Table(ScriptingEnv *env, int r, int c, const QString& label, QWidget* parent, const char* name, Qt::WFlags f)
: MyWidget(label,parent,name,f), scripted(env)
{
	init(r,c);
}

void Table::init(int rows, int cols)
{
	selectedCol=0;
	savedCol=-1;

	QDateTime dt = QDateTime::currentDateTime();
	setBirthDate(dt.toString(Qt::LocalDate));

	d_table = new QTableWidget(rows, cols);
	d_table->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	d_table->setFocusPolicy(Qt::StrongFocus);
	d_table->setFocus();
	// only one contiguous selection supported
	d_table->setSelectionMode(QAbstractItemView::ContiguousSelection);

	d_table->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	d_table->verticalHeader()->setMovable(false);

	connect(d_table->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)),
			this, SLOT(headerDoubleClickedHandler(int)));

	QVBoxLayout* hlayout = new QVBoxLayout(this);
	hlayout->setMargin(0);
	hlayout->addWidget(d_table);

	for (int i=0; i<cols; i++)
	{
		commands << "";
		colTypes << Numeric;
		col_format << "0/6";
		comments << "";
		col_label << QString::number(i+1);
		col_plot_type << Y;
	}

	QHeaderView* head=(QHeaderView*)d_table->horizontalHeader();
	head->setResizeMode(QHeaderView::Interactive);
	connect(head, SIGNAL(sectionResized(int, int, int)), this, SLOT(colWidthModified(int, int, int)));

	col_plot_type[0] = X;
	setHeaderColType();

	// calculate initial geometry
	int w=4*(d_table->horizontalHeader())->sectionSize(0);
	int h;
	if (rows>11)
		h=11*(d_table->verticalHeader())->sectionSize(0);
	else
		h=(rows+1)*(d_table->verticalHeader())->sectionSize(0);
	setGeometry(50,50,w + 45, h);

	// keyboard shortcuts
	QShortcut * sel_all = new QShortcut(QKeySequence(tr("Ctrl+A", "Table: select all")), this);
	connect(sel_all, SIGNAL(activated()), this, SLOT(selectAllTable()));
	// remark: the [TAB] behaviour is now nicely done by Qt4

	connect(d_table, SIGNAL(cellChanged(int,int)), this, SLOT(cellEdited(int,int)));
}

void Table::colWidthModified(int, int, int)
{
	emit modifiedWindow(this);
}

void Table::setBackgroundColor(const QColor& col)
{
	QPalette pal = d_table->palette();
	pal.setColor(QPalette::Base, col);
	d_table->setPalette(pal);
}

void Table::setTextColor(const QColor& col)
{
	QPalette pal = d_table->palette();
	pal.setColor(QPalette::Text, col);
	d_table->setPalette(pal);
}

void Table::setTextFont(const QFont& fnt)
{
	d_table->setFont (fnt);
}

void Table::setHeaderColor(const QColor& col)
{
	QPalette pal = d_table->horizontalHeader()->palette();
	pal.setColor(QPalette::ButtonText, col);
	d_table->horizontalHeader()->setPalette(pal);

	pal = d_table->verticalHeader()->palette();
	pal.setColor(QPalette::ButtonText, col);
	d_table->verticalHeader()->setPalette(pal);
}

void Table::setHeaderFont(const QFont& fnt)
{
	d_table->horizontalHeader()->setFont(fnt);
}

void Table::exportPDF(const QString& fileName)
{
	print(fileName);
}

void Table::print()
{
	print(QString());
}

void Table::print(const QString& fileName)
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

void Table::cellEdited(int row, int col)
{
	// if a dialog is open, return
	// Remark: This is needed to prevent overriding the new cell
	// text value by the saved one. The problem is that setText() emits
	// a cellChanged() signal even if it was not altered by user input
	// but by a dialog.
	if(savedCol != -1)
		return;

	QString cell_text = text(row,col).remove(QRegExp("\\s"));
	if (columnType(col) != Numeric || cell_text.isEmpty())
	{
		emit modifiedData(this, colName(col));
		emit modifiedWindow(this);
		return;
	}

	char f;
	int precision;
	columnNumericFormat(col, &f, &precision);

	QString cell_formula = cell_text;

	bool ok = true;
	QLocale locale;
	double res = locale.toDouble(cell_text, &ok);
	if (ok)
		setText(row, col, locale.toString(res, f, precision));
	else
	{
		Script *script = scriptEnv->newScript(cell_formula, this, QString("<%1_%2_%3>").arg(name()).arg(row).arg(col));
		connect(script, SIGNAL(error(const QString&,const QString&,int)), scriptEnv, SIGNAL(error(const QString&,const QString&,int)));

		script->setInt(row+1, "i");
		script->setInt(col+1, "j");

		QVariant ret = script->eval();
		if(ret.type()==QVariant::Int || ret.type()==QVariant::UInt || ret.type()==QVariant::LongLong
				|| ret.type()==QVariant::ULongLong)
			setText(row, col, ret.toString());
		else if(ret.canConvert(QVariant::Double))
			setText(row, col, locale.toString(ret.toDouble(), f, precision));
		else
			setText(row, col, "");
	}
	if(allow_modification_signals)
	{
		emit modifiedData(this, colName(col));
		emit modifiedWindow(this);
	}
}

int Table::colX(int col)
{
	int i, xcol = -1;
	for(i=col-1; i>=0; i--)
	{
		if (col_plot_type[i] == X)
			return i;
	}
	for(i=col+1; i<d_table->columnCount(); i++)
	{
		if (col_plot_type[i] == X)
			return i;
	}
	return xcol;
}

int Table::colY(int col)
{
	int i, yCol = -1;
	for(i=col-1; i>=0; i--)
	{
		if (col_plot_type[i] == Y)
			return i;
	}
	for(i=col+1; i<d_table->columnCount(); i++)
	{
		if (col_plot_type[i] == Y)
			return i;
	}
	return yCol;
}

void Table::setPlotDesignation(PlotDesignation pd)
{
	QStringList list = selectedColumns();
	for (int i=0;i<list.count(); i++)
		col_plot_type[colIndex(list[i])] = pd;

	setHeaderColType();
	emit modifiedWindow(this);
}

void Table::columnNumericFormat(int col, int *f, int *precision)
{
	QStringList format = col_format[col].split("/", QString::SkipEmptyParts);
	*f = format[0].toInt();
	*precision = format[1].toInt();
}

void Table::columnNumericFormat(int col, char *f, int *precision)
{
	QStringList format = col_format[col].split("/", QString::SkipEmptyParts);
	switch(format[0].toInt())
	{
		case 0:
			*f = 'g';
			break;

		case 1:
			*f = 'f';
			break;

		case 2:
			*f = 'e';
			break;
	}
	*precision = format[1].toInt();
}

int Table::columnWidth(int col)
{
	return d_table->columnWidth(col);
}

QStringList Table::columnWidths()
{
	QStringList widths;
	for (int i=0;i<d_table->columnCount();i++)
		widths << QString::number(d_table->columnWidth(i));

	return widths;
}

void Table::setColWidths(const QStringList& widths)
{
	for (int i=0;i<widths.count();i++)
		d_table->setColumnWidth(i, widths[i].toInt() );
}

void Table::setColumnTypes(const QStringList& ctl)
{
	int n = qMin(ctl.count(), numCols());
	for (int i=0; i<n; i++)
	{
		QStringList l = ctl[i].split(";");
		colTypes[i] = l[0].toInt();

		if ((int)l.count() == 2 && !l[1].isEmpty())
			col_format[i] = l[1];
		else
			col_format[i] = "0/6";
	}
}

QString Table::saveColumnWidths()
{
	QString s = "ColWidth\t";
	for (int i=0;i<d_table->columnCount();i++)
		s += QString::number(d_table->columnWidth (i))+"\t";

	return s+"\n";
}

QString Table::saveColumnTypes()
{
	QString s = "ColType";
	for (int i=0; i<d_table->columnCount(); i++)
		s += "\t"+QString::number(colTypes[i])+";"+col_format[i];

	return s+"\n";
}

void Table::setCommands(const QStringList& com)
{
	commands.clear();
	for(int i=0; i<com.size() && i<numCols(); i++)
		commands << com[i].trimmed();
}

void Table::setCommand(int col, const QString com)
{
	if(col<commands.size())
		commands[col] = com.trimmed();
}

void Table::setCommands(const QString& com)
{
	QStringList lst = com.split("\t");
	lst.pop_front();
	setCommands(lst);
}

bool Table::calculate(int col, int startRow, int endRow)
{
	allow_modification_signals = false;
	QApplication::setOverrideCursor(Qt::WaitCursor);

	Script *colscript = scriptEnv->newScript(commands[col], this,  QString("<%1>").arg(colName(col)));
	connect(colscript, SIGNAL(error(const QString&,const QString&,int)), scriptEnv, SIGNAL(error(const QString&,const QString&,int)));
	connect(colscript, SIGNAL(print(const QString&)), scriptEnv, SIGNAL(print(const QString&)));

	if (!colscript->compile())
	{
		QApplication::restoreOverrideCursor();
		allow_modification_signals = true;
		return false;
	}
	if (endRow >= numRows())
		resizeRows(endRow);

	colscript->setInt(col+1, "j");
	colscript->setInt(startRow+1, "sr");
	colscript->setInt(endRow+1, "er");
	QVariant ret;
	saveColToMemory(col);
	for (int i=startRow; i<=endRow; i++)
	{
		colscript->setInt(i+1,"i");
		ret = colscript->eval();
		if(ret.canConvert(QVariant::Double)) {
			int prec;
			char f;
			columnNumericFormat(col, &f, &prec);
			setText(i, col, QLocale().toString(ret.toDouble(), f, prec));
		} else if(ret.canConvert(QVariant::String))
			setText(i, col, ret.toString());
		else {
			QApplication::restoreOverrideCursor();
			allow_modification_signals = true;
			return false;
		}
	}
	forgetSavedCol();

	allow_modification_signals = true;
	emit modifiedData(this, colName(col));
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
	return true;
}

QList<QTableWidgetSelectionRange> Table::getSelection()
{
	return d_table->selectedRanges();
}

bool Table::calculate()
{
	allow_modification_signals = false;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;
	bool success = true;

	if( it.hasNext() )
	{
		cur = it.next();
		for (int col = cur.leftColumn(); col <= cur.rightColumn(); col++)
			if (!calculate(col, cur.topRow(), cur.bottomRow()))
				success = false;
	}

	allow_modification_signals = true;
	emit modifiedWindow(this);

	return success;
}

QString Table::saveCommands()
{
	QString s="<com>\n";
	for (int col=0; col<numCols(); col++)
		if (!commands[col].isEmpty())
		{
			s += "<col nr=\""+QString::number(col)+"\">\n";
			s += commands[col];
			s += "\n</col>\n";
		}
	s += "</com>\n";
	return s;
}

QString Table::saveComments()
{
	QString s = "Comments\t";
	for (int i=0; i<d_table->columnCount(); i++)
	{
		if (comments.count() > i)
			s += comments[i] + "\t";
		else
			s += "\t";
	}
	return s + "\n";
}

QString Table::saveToString(const QString& geometry)
{
	QString s="<table>\n";
	s+=QString(name())+"\t";
	s+=QString::number(d_table->rowCount())+"\t";
	s+=QString::number(d_table->columnCount())+"\t";
	s+=birthDate()+"\n";
	s+=geometry;
	s+=saveHeader();
	s+=saveColumnWidths();
	s+=saveCommands();
	s+=saveColumnTypes();
	s+=saveComments();
	s+="WindowLabel\t" + windowLabel() + "\t" + QString::number(captionPolicy()) + "\n";
	s+=saveText();
	return s+="</table>\n";
}

QString Table::saveHeader()
{
	QString s="header\t";
	for (int j=0; j<d_table->columnCount(); j++)
	{
		if (col_plot_type[j] == X)
			s+= colLabel(j) + "[X]\t";
		else if (col_plot_type[j] == Y)
			s+= colLabel(j) + "[Y]\t";
		else if (col_plot_type[j] == Z)
			s+= colLabel(j) + "[Z]\t";
		else if (col_plot_type[j] == xErr)
			s+= colLabel(j) + "[xEr]\t";
		else if (col_plot_type[j] == yErr)
			s+= colLabel(j) + "[yEr]\t";
		else
			s+= colLabel(j) + "\t";
	}
	return s+="\n";
}

int Table::firstXCol()
{
	int xcol = -1;
	for (int j=0; j<d_table->columnCount(); j++)
	{
		if (col_plot_type[j] == X)
			return j;
	}
	return xcol;
}

void Table::enumerateRightCols(bool checked)
{
	if (!checked)
		return;

	QString oldLabel = colLabel(selectedCol);
	QStringList oldLabels = colNames();
	QString caption = QString(this->name())+"_";
	int n=1;
	for (int i=selectedCol; i<d_table->columnCount(); i++)
	{
		QString newLabel  =oldLabel+QString::number(n);
		commands.replaceInStrings("col(\""+colLabel(i)+"\")", "col(\""+newLabel+"\")");
		setColName(i, newLabel);
		emit changedColHeader(caption+oldLabels[i],colName(i));
		n++;
	}
	emit modifiedWindow(this);
}

void Table::setColComment(const QString& s)
{
	if (comments[selectedCol] == s)
		return;

	comments[selectedCol] = s;
}

void Table::setColComment(int col, const QString& s)
{
	if (comments[col] == s)
		return;

	comments[col] = s;
}

void Table::changeColWidth(int width, bool allCols)
{
	int cols = d_table->columnCount();
	if (allCols)
	{
		for (int i=0;i<cols;i++)
			d_table->setColumnWidth(i, width);

		emit modifiedWindow(this);
	}
	else
	{
		if (d_table->columnWidth(selectedCol) == width)
			return;

		d_table->setColumnWidth(selectedCol, width);
		emit modifiedWindow(this);
	}
}

void Table::changeColWidth(int width, int col)
{

	if (d_table->columnWidth(col) == width)
		return;

	d_table->setColumnWidth (col, width);
	emit modifiedWindow(this);
}

void Table::changeColName(const QString& new_name)
{
	QString caption = this->name();
	QString old_col_name = colName(selectedCol);
	QString new_col_name = caption+"_"+new_name;

	if (old_col_name == new_col_name)
		return;

	if (caption == new_name)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),
				tr("The column name must be different from the table name : <b>"+caption+"</b></b>!<p>Please choose another name!"));
		return;
	}

	QStringList labels=colNames();
	if (labels.contains(new_name)>0)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),
				tr("There is already a column called : <b>"+new_name+"</b> in table <b>"+caption+"</b>!<p>Please choose another name!"));
		return;
	}

	commands.replaceInStrings("col(\""+colLabel(selectedCol)+"\")", "col(\""+new_name+"\")");

	setColName(selectedCol, new_name);
	emit changedColHeader(old_col_name, new_col_name);
	emit modifiedWindow(this);
}

void Table::setColName(int col, const QString& new_name)
{
	if (new_name.isEmpty() || col<0 || col >=d_table->columnCount())
		return;

	col_label[col] = new_name;
	setHeaderColType();
}

QStringList Table::selectedColumns()
{
	QStringList names;
	for (int i=0; i<d_table->columnCount(); i++)
	{
		if(isColumnSelected (i, true))
			names << QString(name()) + "_" + col_label[i];
	}
	return names;
}

QStringList Table::YColumns()
{
	QStringList names;
	for (int i=0;i<d_table->columnCount();i++)
	{
		if(col_plot_type[i] == Y)
			names << QString(name())+"_"+col_label[i];
	}
	return names;
}

QStringList Table::selectedYColumns()
{
	QStringList names;
	for (int i=0;i<d_table->columnCount();i++)
	{
		if(isColumnSelected (i) && col_plot_type[i] == Y)
			names << QString(name())+"_"+col_label[i];
	}
	return names;
}

QStringList Table::selectedErrColumns()
{
	QStringList names;
	for (int i=0;i<d_table->columnCount();i++)
	{
		if(isColumnSelected (i,true) &&
				(col_plot_type[i] == yErr || col_plot_type[i] == xErr))
			names << QString(name())+"_"+col_label[i];
	}
	return names;
}

QStringList Table::drawableColumnSelection()
{
	QStringList names;
	for (int i=0; i<d_table->columnCount(); i++)
	{
		if(isColumnSelected (i) && col_plot_type[i] == Y)
			names << QString(name()) + "_" + col_label[i];
	}

	for (int i=0; i<d_table->columnCount(); i++)
	{
		if(isColumnSelected (i) && (col_plot_type[i] == yErr || col_plot_type[i] == xErr))
			names << QString(name()) + "_" + col_label[i];
	}
	return names;
}

QStringList Table::selectedYLabels()
{
	QStringList names;
	for (int i=0;i<d_table->columnCount();i++)
	{
		if(isColumnSelected (i) && col_plot_type[i] == Y)
			names<<col_label[i];
	}
	return names;
}

QStringList Table::columnsList()
{
	QStringList names;
	for (int i=0;i<d_table->columnCount();i++)
		names << QString(name())+"_"+col_label[i];

	return names;
}

int Table::firstSelectedColumn()
{
	for (int i=0;i<d_table->columnCount();i++)
	{
		if(isColumnSelected(i,true))
			return i;
	}
	return -1;
}

int Table::numSelectedRows()
{
	int r=0;
	for (int i=0;i<d_table->rowCount();i++)
	{
		if(isRowSelected(i,true))
			r++;
	}
	return r;
}

int Table::numSelectedColumns()
{
	int r=0;
	for (int i=0;i<d_table->columnCount();i++)
	{
		if(isColumnSelected(i,true))
			r++;
	}
	return r;
}

int Table::selectedColsNumber()
{
	int c=0;
	for (int i=0;i<d_table->columnCount(); i++)
	{
		if(isColumnSelected(i,true))
			c++;
	}
	return c;
}

QString Table::colName(int col)
{//returns the table name + horizontal header text
	if (col<0 || col >=d_table->columnCount())
		return QString();

	return QString(this->name())+"_"+col_label[col];
}

QVarLengthArray<double> Table::col(int ycol)
{
	int i;
	int rows=d_table->rowCount();
	int cols=d_table->columnCount();
	QVarLengthArray<double> Y(rows);
	if (ycol<=cols)
	{
		for (i=0;i<rows;i++)
			Y[i] = text(i,ycol).toDouble();
	}
	return Y;
}

void Table::insertCols(int start, int count)
{
	if (start < 0)
		start = 0;

	int max = 0;
	for (int i = 0; i<d_table->columnCount(); i++)
	{
		if (!col_label[i].contains(QRegExp ("\\D")))
		{
			int id = col_label[i].toInt();
			if (id > max)
				max = id;
		}
	}
	max++;

	for(int i = 0; i<count; i++ )
		d_table->insertColumn(start);

	for(int i = 0; i<count; i++ )
	{
		int j = start + i;
		commands.insert(j, QString());
		col_format.insert(j, "0/6");
		comments.insert(j, QString());
		col_label.insert(j, QString::number(max + i));
		colTypes.insert(j, Numeric);
		col_plot_type.insert(j, Y);
	}
	setHeaderColType();
	emit modifiedWindow(this);
}

void Table::insertCol()
{
	insertCols(selectedCol, 1);
}

void Table::insertRow()
{
	int cr = d_table->currentRow();
	if (isRowSelected (cr, true))
	{
		d_table->insertRow(cr);
		emit modifiedWindow(this);
	}
}

void Table::addCol(PlotDesignation pd)
{
	d_table->clearSelection();
	int index, max=0, cols=d_table->columnCount();
	for (int i=0; i<cols; i++)
	{
		if (!col_label[i].contains(QRegExp("\\D")))
		{
			index = col_label[i].toInt();
			if (index > max)
				max = index;
		}
	}
	d_table->insertColumn(cols);

	// TODO: find out why this does not work
	// Remark by thzs: this code crashes with the error 
	// "ASSERT failure in QList<T>::operator[]: "index out of range", file /usr/include/qt4/QtCore/qlist.h, line 378"
	// in the setItem line. I have no idea why, the column should exist.
	/*	QTableWidgetItem * the_item = d_table->item(0, cols);	
		if(!the_item)
		{
		the_item = new QTableWidgetItem("");
		d_table->setItem(0, cols, the_item);
		}

		d_table->scrollToItem(the_item);*/

	comments << QString();
	commands << "";
	colTypes << Numeric;
	col_format << "0/6";
	col_label << QString::number(max+1);
	col_plot_type << pd;

	setHeaderColType();
	emit modifiedWindow(this);
}

void Table::addColumns(int c)
{
	int i, index, max=0, cols=d_table->columnCount();
	for (i=0; i<cols; i++)
	{
		if (!col_label[i].contains(QRegExp("\\D")))
		{
			index = col_label[i].toInt();
			if (index>max)
				max=index;
		}
	}
	max++;

	for (i=0; i<c; i++)
		d_table->insertColumn(cols);

	for (i=0; i<c; i++)
	{
		comments << QString();
		commands << "";
		colTypes << Numeric;
		col_format << "0/6";
		col_label << QString::number(max+i);
		col_plot_type << Y;
	}
}

void Table::clearCol()
{
	for (int i=0; i<d_table->rowCount(); i++)
	{
		QTableWidgetItem * the_item = d_table->item(i, selectedCol);
		if(!the_item)
		{
			the_item = new QTableWidgetItem("");
			d_table->setItem(i, selectedCol, the_item);
		}
		else if(the_item->isSelected())
			setText(i,selectedCol, "");
	}
	QString name = colName(selectedCol);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

void Table::clearCell(int row, int col)
{
	setText(row, col, "");

	QString name=colName(col);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

void Table::deleteSelectedRows()
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

	notifyChanges();
}

void Table::cutSelection()
{
	copySelection();
	clearSelection();
}

void Table::selectAllTable()
{
	d_table->selectAll();
}

void Table::deselect()
{
	d_table->clearSelection();
}


void Table::clearSelection()
{
	QStringList list = selectedColumns();
	int n = list.count();

	if (n>0)
	{
		for (int i=0;i<n;i++)
		{
			QString name=list[i];
			selectedCol = colIndex(name);
			clearCol();
		}
	}
	else
	{
		allow_modification_signals = false;

		QList<QTableWidgetSelectionRange> sel = getSelection();
		QListIterator<QTableWidgetSelectionRange> it(sel);
		QTableWidgetSelectionRange cur;

		if( it.hasNext() )
		{
			cur = it.next();
			int top = cur.topRow();
			int bottom = cur.bottomRow();
			int left = cur.leftColumn();
			int right = cur.rightColumn();

			for(int i = top; i <= bottom; i++)
				for(int j = left; j<= right;j++)
					setText(i, j, "");


			for (int i=cur.leftColumn();i<=cur.rightColumn();i++)
			{
				QString name = colName(i);
				emit modifiedData(this, name);
			}
		}

		allow_modification_signals = true;
		emit modifiedWindow(this);
	}
}

void Table::copySelection()
{
	QString the_text;
	int i,j;
	int rows = d_table->rowCount();
	int cols = d_table->columnCount();

	QVarLengthArray<int> selection(1);
	int c=0;
	for (i=0;i<cols;i++)
	{
		if (isColumnSelected(i,true))
		{
			c++;
			selection.resize(c);
			selection[c-1]=i;
		}
	}
	if (c>0)
	{
		for (i=0; i<rows; i++)
		{
			for (j=0;j<c-1;j++)
				the_text += text(i,selection[j])+"\t";
			the_text += text(i,selection[c-1])+"\n";
		}
	}
	else
	{
		QList<QTableWidgetSelectionRange> sel = getSelection();
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

// Paste text from the clipboard
void Table::pasteSelection()
{
	QString the_text = QApplication::clipboard()->text();
	if (the_text.isEmpty())
		return;

	allow_modification_signals = false;
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QTextStream ts( &the_text, QIODevice::ReadOnly );
	QString s = ts.readLine();
	QStringList cellTexts = s.split("\t");
	int cols = cellTexts.count();
	int rows = 1;
	while(!ts.atEnd())
	{
		rows++;
		s = ts.readLine();
	}
	ts.reset();

	int i, j, top, bottom, right, left, firstCol;

	QList<QTableWidgetSelectionRange> sel = d_table->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if (!sel.isEmpty())
	{
		cur = it.next();
		top = cur.topRow();
		bottom = cur.bottomRow();
		left = cur.leftColumn();
		right = cur.rightColumn();
	}
	else
	{
		top = 0;
		bottom = numRows() - 1;
		left = 0;
		right = numCols() - 1;

		firstCol = firstSelectedColumn();

		if (firstCol >= 0)
		{ // columns are selected
			left = firstCol;
			int selectedColsNumber = 0;
			for(i=0; i<numCols(); i++)
			{
				if (isColumnSelected(i, true))
					selectedColsNumber++;
			}
			right = firstCol + selectedColsNumber - 1;
		}
	}

	QTextStream ts2( &the_text, QIODevice::ReadOnly );
	int r = bottom-top+1;
	int c = right-left+1;

	QApplication::restoreOverrideCursor();
	if (rows>r || cols>c)
	{
		// TODO: I find the insert cells option awkward
		// I would prefer the behavior of OpenOffice Calc
		// here - thzs
		switch( QMessageBox::information(0,"QtiPlot",
					tr("The text in the clipboard is larger than your current selection!\
						\nDo you want to insert cells?"),
					tr("Yes"), tr("No"), tr("Cancel"), 0, 0) )
		{
			case 0:
				if(cols > c )
					for(int i=0; i<(cols-c); i++)
						d_table->insertColumn(left);

				if(rows > r)
				{
					if (firstCol >= 0)
						for(int i=0; i<(rows-r); i++)
							d_table->insertRow(top);
					else
						for(int i=0; i<(rows-r+1); i++)
							d_table->insertRow(top);
				}
				break;
			case 1:
				rows = r;
				cols = c;
				break;
			case 2:
				allow_modification_signals = true;
				return;
				break;
		}
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	bool numeric;
	double value;
	QLocale system_locale = QLocale::system();
	for(i=top; i<top+rows; i++)
	{
		s = ts2.readLine();
		cellTexts=s.split("\t");
		for(j=left; j<left+cols; j++)
		{
			char f;
			int precision;
			columnNumericFormat(j, &f, &precision);

			value = system_locale.toDouble(cellTexts[j-left], &numeric);
			if (numeric)
				setText(i, j, QLocale().toString(value, f, precision));
			else
				setText(i, j, cellTexts[j-left]);
		}
	}

	allow_modification_signals = true;
	emit modifiedWindow(this);
	for(j=left; j<left+cols; j++)
	{
		QString name = colName(j);
		emit modifiedData(this, name);
	}
	QApplication::restoreOverrideCursor();
}

void Table::removeCol()
{
	QStringList list = selectedColumns();
	removeCol(list);
}

void Table::removeCol(const QStringList& list)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	for (int i=0; i<list.count(); i++)
	{
		QString name = list[i];
		int id = colIndex(name);
		if (id >= 0)
		{
			if ( id < commands.size() )
				commands.removeAt(id);

			if ( id < col_label.size() )
				col_label.removeAt(id);

			if ( id < col_format.size() )
				col_format.removeAt(id);

			if ( id < comments.size() )
				comments.removeAt(id);

			if ( id < colTypes.size() )
				colTypes.removeAt(id);

			if ( id < col_plot_type.size() )
				col_plot_type.removeAt(id);

			d_table->removeColumn(id);
		}
		emit removedCol(name);
	}
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::normalizeSelection()
{
	QStringList s=selectedColumns();
	for (int i=0; i<(int)s.count(); i++)
	{
		selectedCol=colIndex(s[i]);
		normalizeCol();
	}

	emit modifiedWindow(this);
}

void Table::normalize()
{
	for (int i=0; i<d_table->columnCount(); i++)
	{
		selectedCol = i;
		normalizeCol();
	}
	emit modifiedWindow(this);
}

void Table::normalizeCol(int col)
{
	allow_modification_signals = false;
	if (col<0) col = selectedCol;
	double max = text(0,col).toDouble();
	double aux = 0.0;
	int rows = d_table->rowCount();
	for (int i=0; i<rows; i++)
	{
		QString the_text = text(i,col);
		aux = the_text.toDouble();
		if (!the_text.isEmpty() && fabs(aux)>fabs(max))
			max = aux;
	}

	if (max == 1.0)
		return;

	int prec;
	char f;
	columnNumericFormat(col, &f, &prec);

	for (int i=0; i<rows; i++)
	{
		QString the_text = text(i, col);
		aux = the_text.toDouble();
		if ( !the_text.isEmpty() )
			setText(i, col, QLocale().toString(aux/max, f, prec));
	}

	QString name = colName(col);
	allow_modification_signals = true;
	emit modifiedData(this, name);
}

void Table::sortColumnsDialog()
{
	QStringList s=selectedColumns();
	SortDialog *sortd=new SortDialog(this);
	sortd->setAttribute(Qt::WA_DeleteOnClose);
	connect (sortd,SIGNAL(sort(int, int, const QString&)),
			this,SLOT(sortColumns(int, int, const QString&)));
	sortd->insertColumnsList(s);
	sortd->exec();
}

void Table::sortTableDialog()
{
	SortDialog *sortd=new SortDialog(this);
	sortd->setAttribute(Qt::WA_DeleteOnClose);
	connect (sortd,SIGNAL(sort(int, int, const QString&)),
			this,SLOT(sort(int, int, const QString&)));
	sortd->insertColumnsList(colNames());
	sortd->exec();
}

void Table::sort(int type, int order, const QString& leadCol)
{
	sortColumns(colNames(), type, order, leadCol);
}

void Table::sortColumns(int type, int order, const QString& leadCol)
{
	sortColumns(selectedColumns(), type, order, leadCol);
}

void Table::sortColumns(const QStringList&s, int type, int order, const QString& leadCol)
{
	int cols=s.count();
	if(!type)
	{
		for(int i=0;i<cols;i++)
			sortColumn(colIndex(s[i]), order);
	}
	else
	{
		int leadcol = colIndex(leadCol);
		if (leadcol < 0)
		{
			QMessageBox::critical(this, tr("QtiPlot - Error"),
					tr("Please indicate the name of the leading column!"));
			return;
		}
		if (columnType(leadcol) == Table::Text)
		{
			QMessageBox::critical(this, tr("QtiPlot - Error"),
					tr("The leading column has the type set to 'Text'! Operation aborted!"));
			return;
		}

		allow_modification_signals = false;
		int rows=d_table->rowCount();
		int non_empty_cells = 0;
		QVarLengthArray<int> valid_cell(rows);
		QVarLengthArray<double> data_double(rows);
		for (int j = 0; j <rows; j++)
		{
			if (!text(j, leadcol).isEmpty())
			{
				data_double[non_empty_cells] = text(j,leadcol).toDouble();
				valid_cell[non_empty_cells] = j;
				non_empty_cells++;
			}
		}

		if (!non_empty_cells)
		{
			QMessageBox::critical(this, tr("QtiPlot - Error"),
					tr("The leading column is empty! Operation aborted!"));
			allow_modification_signals = true;
			return;
		}

		data_double.resize(non_empty_cells);
		valid_cell.resize(non_empty_cells);
		QVarLengthArray<QString> data_string(non_empty_cells);
		size_t *p= new size_t[non_empty_cells];

		// Find the permutation index for the lead col
		gsl_sort_index(p, data_double.data(), 1, non_empty_cells);

		for(int i=0;i<cols;i++)
		{// Since we have the permutation index, sort all the columns
			int col=colIndex(s[i]);
			if (columnType(col) == Text)
			{
				for (int j=0; j<non_empty_cells; j++)
					data_string[j] = text(valid_cell[j], col);
				if(!order)
					for (int j=0; j<non_empty_cells; j++)
						setText(valid_cell[j], col, data_string[p[j]]);
				else
					for (int j=0; j<non_empty_cells; j++)
						setText(valid_cell[j], col, data_string[p[non_empty_cells-j-1]]);
			}
			else
			{
				for (int j = 0; j<non_empty_cells; j++)
					data_double[j] = text(valid_cell[j], col).toDouble();
				int prec;
				char f;
				columnNumericFormat(col, &f, &prec);
				if(!order)
					for (int j=0; j<non_empty_cells; j++)
						setText(valid_cell[j], col, QLocale().toString(data_double[p[j]], f, prec));
				else
					for (int j=0; j<non_empty_cells; j++)
						setText(valid_cell[j], col, QLocale().toString(data_double[p[non_empty_cells-j-1]], f, prec));
			}
			emit modifiedData(this, colName(col));
		}
		delete[] p;
	}

	allow_modification_signals = true;
	emit modifiedWindow(this);
}

void Table::sortColumn(int col, int order)
{
	if (col < 0)
		col = d_table->currentColumn();

	int rows=d_table->rowCount();
	int non_empty_cells = 0;
	QVarLengthArray<int> valid_cell(rows);
	QVarLengthArray<double> r(rows);
	QStringList text_cells;
	for (int i = 0; i <rows; i++)
	{
		if (!text(i, col).isEmpty())
		{
			if (columnType(col) == Table::Text)
				text_cells << text(i, col);
			else
				r[non_empty_cells] = text(i,col).toDouble();
			valid_cell[non_empty_cells] = i;
			non_empty_cells++;
		}
	}

	if (!non_empty_cells)
		return;

	valid_cell.resize(non_empty_cells);
	if (columnType(col) == Table::Text)
	{
		r.clear();
		text_cells.sort();
	}
	else
	{
		r.resize(non_empty_cells);
		gsl_sort(r.data(), 1, non_empty_cells);
	}

	allow_modification_signals = false;
	if (columnType(col) == Table::Text)
	{
		if (!order)
		{
			for (int i=0; i<non_empty_cells; i++)
				setText(valid_cell[i], col, text_cells[i]);
		}
		else
		{
			for (int i=0; i<non_empty_cells; i++)
				setText(valid_cell[i], col, text_cells[non_empty_cells-i-1]);
		}
	}
	else
	{
		int prec;
		char f;
		columnNumericFormat(col, &f, &prec);
		if (!order)
		{
			for (int i=0; i<non_empty_cells; i++)
				setText(valid_cell[i], col, QLocale().toString(r[i], f, prec));
		}
		else
		{
			for (int i=0; i<non_empty_cells; i++)
				setText(valid_cell[i], col, QLocale().toString(r[non_empty_cells-i-1], f, prec));
		}
	}

	allow_modification_signals = true;
	emit modifiedData(this, colName(col));
	emit modifiedWindow(this);
}

void Table::sortColAsc()
{
	sortColumn(d_table->currentColumn ());
}

void Table::sortColDesc()
{
	sortColumn(d_table->currentColumn(), 1);
}

int Table::numRows()
{
	return d_table->rowCount();
}

int Table::numCols()
{
	return d_table->columnCount();
}

bool Table::isEmptyRow(int row)
{
	for (int i=0; i<d_table->columnCount(); i++)
	{
		QString the_text = text(row, i);
		if (!the_text.isEmpty())
			return false;
	}
	return true;
}

bool Table::isEmptyColumn(int col)
{
	for (int i=0; i<d_table->rowCount(); i++)
	{
		QString the_text = text(i, col);
		if (!the_text.isEmpty())
			return false;
	}
	return true;
}

QString Table::saveText()
{
	QString the_text = "<data>\n";
	int cols=d_table->columnCount();
	int rows=d_table->rowCount();

	for (int i=0; i<rows; i++)
	{
		if (!isEmptyRow(i))
		{
			the_text += QString::number(i)+"\t";
			for (int j=0; j<cols-1; j++)
				the_text += text(i,j)+"\t";

			the_text += text(i,cols-1)+"\n";
		}
	}
	return the_text + "</data>\n";
}

int Table::nonEmptyRows()
{
	int r=0;
	for (int i=0;i<d_table->rowCount();i++)
	{
		if (!isEmptyRow(i))
			r++;
	}
	return r;
}

double Table::cell(int row, int col)
{
	return stringToDouble(text(row, col));
}

QString Table::text(int row, int col)
{
	if (col == savedCol)
		return savedCells[row];
	else if(d_table->item(row, col))
		return d_table->item(row, col)->text();
	else
		return QString("");
}

void Table::setText(int row, int col, const QString & new_text)
{
	if(d_table->item(row, col))
		d_table->item(row, col)->setText(new_text);
	else
		d_table->setItem(row, col, new QTableWidgetItem(new_text));
}

void Table::saveColToMemory(int col)
{
	int rows=d_table->rowCount();
	savedCells.clear();
	for (int row=0; row<rows; row++)
		savedCells << text(row, col);
	savedCol = col;
}

void Table::forgetSavedCol()
{
	savedCells.clear();
	savedCol = -1;
}

void Table::setTextFormat(int col)
{
	colTypes[col] = Text;
}

void Table::setColNumericFormat(int f, int prec, int col)
{
	if (colTypes[col] == Numeric)
	{
		int old_f, old_prec;
		columnNumericFormat(col, &old_f, &old_prec);
		if (old_f == f && old_prec == prec)
			return;
	}

	colTypes[col] = Numeric;
	col_format[col] = QString::number(f)+"/"+QString::number(prec);

	char format = 'g';
	for (int i=0; i<d_table->rowCount(); i++)
	{
		QString t = text(i, col);
		if (!t.isEmpty())
		{
			if (!f)
				prec = 6;
			else if (f == 1)
				format = 'f';
			else if (f == 2)
				format = 'e';

			setText(i, col, QLocale().toString(stringToDouble(t), format, prec));
		}
	}
}

void Table::setColumnsFormat(const QStringList& lst)
{
	if (col_format == lst)
		return;

	col_format = lst;
}

bool Table::setDateTimeFormat(int f, const QString& format, int col)
{
	switch (f)
	{
		case 2:
			return setDateFormat(format, col);

		case 3:
			return setTimeFormat(format, col);

		case 4:
			setMonthFormat(format, col);
			return true;

		case 5:
			setDayFormat(format, col);
			return true;

		default:
			return false;
	}
}

bool Table::setDateFormat(const QString& format, int col)
{
	if (col_format[col] == format)
		return true;

	for (int i=0; i<numRows(); i++)
	{
		QString s = text(i,col);
		if (!s.isEmpty())
		{
			QDateTime d = QDateTime::fromString (s, col_format[col]);
			if (d.isValid())
				setText(i, col, d.toString(format));
			else
			{//This might be the first time the user assigns a date format.
				//If Qt understands the format we break the loop, assign it to the column and return true!
				d = QDateTime::fromString (s, format);
				if (d.isValid())
					break;
				else
					return false;
			}
		}
	}
	colTypes[col] = Date;
	col_format[col] = format;
	return true;
}

bool Table::setTimeFormat(const QString& format, int col)
{
	if (col_format[col] == format)
		return true;

	for (int i=0; i<numRows(); i++)
	{
		QString s = text(i,col);
		if (!s.isEmpty())
		{
			QTime t = QTime::fromString (s, col_format[col]);
			if (t.isValid())
				setText(i, col, t.toString(format));
			else
			{//This might be the first time the user assigns a date format.
				//If Qt understands the format we break the loop, assign it to the column and return true!
				t = QTime::fromString (s, format);
				if (t.isValid())
					break;
				else
					return false;
			}
		}
	}
	colTypes[col] = Time;
	col_format[col] = format;
	return true;
}

void Table::setMonthFormat(const QString& format, int col)
{
	colTypes[col] = Month;
	int rows=d_table->rowCount();
	if (format == QDate::shortMonthName(QDate::currentDate().month()))
	{
		for (int i=0;i<rows; i++)
		{
			QString s = text(i,col);
			if (!s.isEmpty())
			{
				int month= s.toInt() % 12;
				if (!month)
					month = 12;

				setText(i, col, QDate::shortMonthName(month));
			}
		}
	}
	else if (format == QDate::longMonthName(QDate::currentDate().month()))
	{
		for (int i=0;i<rows; i++)
		{
			QString t = text(i,col);
			if (!t.isEmpty())
			{
				int month= t.toInt() % 12;
				if (!month)
					month = 12;

				setText(i, col, QDate::longMonthName(month));
			}
		}
	}
}

void Table::setDayFormat(const QString& format, int col)
{
	colTypes[col] = Day;
	int rows = numRows();	
	if (format == QDate::shortDayName(QDate::currentDate().dayOfWeek()))
	{
		for (int i=0;i<rows; i++)
		{
			QString t = text(i,col);
			if (!t.isEmpty())
			{
				int day= t.toInt() % 7;
				if (!day)
					day = 7;

				setText(i, col, QDate::shortDayName(day));
			}
		}
	}
	else if (format == QDate::longDayName(QDate::currentDate().dayOfWeek()))
	{
		for (int i=0;i<rows; i++)
		{
			QString t = text(i,col);
			if (!t.isEmpty())
			{
				int day= t.toInt() % 7;
				if (!day)
					day = 7;

				setText(i, col, QDate::longDayName(day));
			}
		}
	}
}

void Table::setRandomValues()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	double max=0.0;
	int rows = d_table->rowCount();
	QStringList list = selectedColumns();
	QVarLengthArray<double> r(rows);

	for (int j=0;j<list.count(); j++)
	{
		QString name = list[j];
		selectedCol = colIndex(name);

		int prec;
		char f;
		columnNumericFormat(selectedCol, &f, &prec);

		srand(rand());

		for (int i=0; i<rows; i++)
		{
			r[i]=rand();
			if (max<r[i]) max=r[i];
		}

		for (int i=0; i<rows; i++)
		{
			r[i]/=max;
			QTableWidgetItem * the_item = d_table->item(i, selectedCol);
			if(!the_item)
			{
				the_item = new QTableWidgetItem("");
				d_table->setItem(i, selectedCol, the_item);
			}

			if(the_item->isSelected())
					setText(i, selectedCol, QLocale().toString(r[i], f, prec));
		}

		emit modifiedData(this, name);
	}

	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::loadHeader(QStringList header)
{
	for (int i=0;i<(int)d_table->columnCount();i++)
	{
		QString s = header[i].replace("_","-");
		if (s.contains("[X]"))
		{
			col_label[i]=s.remove("[X]");
			col_plot_type[i] = X;
		}
		else if (s.contains("[Y]"))
		{
			col_label[i]=s.remove("[Y]");
			col_plot_type[i] = Y;
		}
		else if (s.contains("[Z]"))
		{
			col_label[i]=s.remove("[Z]");
			col_plot_type[i] = Z;
		}
		else if (s.contains("[xEr]"))
		{
			col_label[i]=s.remove("[xEr]");
			col_plot_type[i] = xErr;
		}
		else if (s.contains("[yEr]"))
		{
			col_label[i]=s.remove("[yEr]");
			col_plot_type[i] = yErr;
		}
		else
		{
			col_label[i]=s;
			col_plot_type[i] = None;
		}
	}
	setHeaderColType();
}

void Table::setHeader(QStringList header)
{
	col_label = header;
	setHeaderColType();
}

int Table::colIndex(const QString& name)
{
	int pos=name.find("_",false);
	QString label=name.right(name.length()-pos-1);
	return col_label.findIndex(label);
}

void Table::setHeaderColType()
{
	int xcols=0;
	for (int j=0;j<(int)d_table->columnCount();j++)
	{
		if (col_plot_type[j] == X)
			xcols++;
	}

	if (xcols>1)
	{
		xcols = 0;
		for (int i=0; i<d_table->columnCount(); i++)
		{
			if (col_plot_type[i] == X)
				setColumnHeader(i,  col_label[i]+"[X" + QString::number(++xcols) +"]");
			else if (col_plot_type[i] == Y)
			{
				if(xcols>0)
					setColumnHeader(i,  col_label[i]+"[Y"+ QString::number(xcols) +"]");
				else
					setColumnHeader(i,  col_label[i]+"[Y]");
			}
			else if (col_plot_type[i] == Z)
			{
				if(xcols>0)
					setColumnHeader(i,  col_label[i]+"[Z"+ QString::number(xcols) +"]");
				else
					setColumnHeader(i,  col_label[i]+"[Z]");
			}
			else if (col_plot_type[i] == xErr)
				setColumnHeader(i,  col_label[i]+"[xEr]");
			else if (col_plot_type[i] == yErr)
				setColumnHeader(i,  col_label[i]+"[yEr]");
			else
				setColumnHeader(i,  col_label[i]);
		}
	}
	else
	{
		for (int i=0; i<(int)d_table->columnCount(); i++)
		{
			if (col_plot_type[i] == X)
				setColumnHeader(i,  col_label[i]+"[X]");
			else if (col_plot_type[i] == Y)
				setColumnHeader(i,  col_label[i]+"[Y]");
			else if (col_plot_type[i] == Z)
				setColumnHeader(i,  col_label[i]+"[Z]");
			else if (col_plot_type[i] == xErr)
				setColumnHeader(i,  col_label[i]+"[xEr]");
			else if (col_plot_type[i] == yErr)
				setColumnHeader(i,  col_label[i]+"[yEr]");
			else
				setColumnHeader(i,  col_label[i]);
		}
	}
}

void Table::setAscValues()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows=d_table->rowCount();
	QStringList list=selectedColumns();

	for (int j=0;j<(int) list.count(); j++)
	{
		QString name=list[j];
		selectedCol=colIndex(name);

		if (columnType(selectedCol) != Numeric) {
			colTypes[selectedCol] = Numeric;
			col_format[selectedCol] = "0/6";
		}

		int prec;
		char f;
		columnNumericFormat(selectedCol, &f, &prec);

		for (int i=0;i<rows;i++)
		{
			QTableWidgetItem * the_item = d_table->item(i, selectedCol);
			if(!the_item)
			{
				the_item = new QTableWidgetItem("");
				d_table->setItem(i, selectedCol, the_item);
			}

			if(the_item->isSelected())
				setText(i,selectedCol,QString::number(i+1, f, prec));
		}

		emit modifiedData(this, name);
	}

	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::plotL()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::Line, cur.topRow(), cur.bottomRow());
}

void Table::plotP()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::Scatter, cur.topRow(), cur.bottomRow());
}

void Table::plotLP()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::LineSymbols, cur.topRow(), cur.bottomRow());
}

void Table::plotVB()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::VerticalBars, cur.topRow(), cur.bottomRow());
}

void Table::plotHB()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::HorizontalBars, cur.topRow(), cur.bottomRow());
}

void Table::plotArea()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::Area, cur.topRow(), cur.bottomRow());
}

bool Table::noXColumn()
{
	bool notSet = true;
	for (int i=0; i<d_table->columnCount(); i++)
	{
		if (col_plot_type[i] == X)
			return false;
	}
	return notSet;
}

bool Table::noYColumn()
{
	bool notSet = true;
	for (int i=0; i<d_table->columnCount(); i++)
	{
		if (col_plot_type[i] == Y)
			return false;
	}
	return notSet;
}

void Table::plotPie()
{
	if (noXColumn())
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"), tr("Please set a default X column for this table, first!"));
		return;
	}

	QStringList s=selectedColumns();
	if (s.count()>0)
	{
		QList<QTableWidgetSelectionRange> sel = getSelection();
		QListIterator<QTableWidgetSelectionRange> it(sel);
		if(!it.hasNext())
			return;
		QTableWidgetSelectionRange cur = it.next();
		emit plotCol(this, s, Graph::Pie, cur.topRow(), cur.bottomRow());
	}
	else
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("Please select a column to plot!"));
}

void Table::plotVerticalDropLines()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::VerticalDropLines, cur.topRow(), cur.bottomRow());
}

void Table::plotSpline()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::Spline, cur.topRow(), cur.bottomRow());
}

void Table::plotVertSteps()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::VerticalSteps, cur.topRow(), cur.bottomRow());
}

void Table::plotHorSteps()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::HorizontalSteps, cur.topRow(), cur.bottomRow());
}

void Table::plotHistogram()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::Histogram, cur.topRow(), cur.bottomRow());
}

void Table::plotBoxDiagram()
{
	if (!valid2DPlot())
		return;

	QList<QTableWidgetSelectionRange> sel = getSelection();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	if(!it.hasNext())
		return;
	QTableWidgetSelectionRange cur = it.next();
	emit plotCol(this, drawableColumnSelection(), Graph::Box, cur.topRow(), cur.bottomRow());
}

void Table::plotVectXYXY()
{
	if (!valid2DPlot())
		return;

	QStringList s = selectedColumns();
	if ((int)s.count() == 4)
	{
		QList<QTableWidgetSelectionRange> sel = getSelection();
		QListIterator<QTableWidgetSelectionRange> it(sel);
		if(!it.hasNext())
			return;
		QTableWidgetSelectionRange cur = it.next();
		emit plotCol(this, s, Graph::VectXYXY, cur.topRow(), cur.bottomRow());
	}
	else
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("Please select four columns for this operation!"));
}

void Table::plotVectXYAM()
{
	if (!valid2DPlot())
		return;

	QStringList s = selectedColumns();
	if ((int)s.count() == 4)
	{
		QList<QTableWidgetSelectionRange> sel = getSelection();
		QListIterator<QTableWidgetSelectionRange> it(sel);
		if(!it.hasNext())
			return;
		QTableWidgetSelectionRange cur = it.next();
		emit plotCol(this, s, Graph::VectXYAM, cur.topRow(), cur.bottomRow());
	}
	else
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("Please select four columns for this operation!"));
}

bool Table::valid2DPlot()
{
	if (!selectedYColumns().count())
	{
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
		return false;
	}
	else if (d_table->columnCount()<2)
	{
		QMessageBox::critical(this, tr("QtiPlot - Error"),tr("You need at least two columns for this operation!"));
		return false;
	}
	else if (noXColumn())
	{
		QMessageBox::critical(this, tr("QtiPlot - Error"), tr("Please set a default X column for this table, first!"));
		return false;
	}

	return true;
}

void Table::plot3DRibbon()
{
	if (!valid3DPlot())
		return;

	emit plot3DRibbon(this,colName(selectedCol));
}

void Table::plot3DBars()
{
	if (!valid3DPlot())
		return;

	emit plotXYZ(this,colName(selectedCol),2);
}

void Table::plot3DScatter()
{
	if (!valid3DPlot())
		return;

	emit plotXYZ(this, colName(selectedCol), 0);
}

void Table::plot3DTrajectory()
{
	if (!valid3DPlot())
		return;

	emit plotXYZ(this,colName(selectedCol),1);
}

bool Table::valid3DPlot()
{
	if (d_table->columnCount()<2)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),tr("You need at least two columns for this operation!"));
		return false;
	}
	if (selectedCol < 0 || col_plot_type[selectedCol] != Z)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),tr("Please select a Z column for this operation!"));
		return false;
	}
	if (noXColumn())
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),tr("You need to define a X column first!"));
		return false;
	}
	if (noYColumn())
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),tr("You need to define a Y column first!"));
		return false;
	}
	return true;
}

void Table::importMultipleASCIIFiles(const QString &fname, const QString &sep, int ignoredLines,
		bool renameCols, bool stripSpaces, bool simplifySpaces,
		int importFileAs)
{
	QFile f(fname);
	QTextStream t( &f );// use a text stream
	if ( f.open(QIODevice::ReadOnly) )
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

		int i, rows = 1, cols = 0;
		int r = d_table->rowCount();
		int c = d_table->columnCount();
		for (i=0; i<ignoredLines; i++)
			t.readLine();

		QString s = t.readLine();//read first line after the ignored ones
		while (!t.atEnd())
		{
			t.readLine();
			rows++;
			qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
		}

		if (simplifySpaces)
			s = s.simplified();
		else if (stripSpaces)
			s = s.trimmed();
		QStringList line = s.split(sep);
		cols = (int)line.count();

		bool allNumbers = true;
		line.replaceInStrings ( ",", "." ); //Qt uses decimal dot
		for (i=0; i<cols; i++)
		{//verify if the strings in the line used to rename the columns are not all numbers
			(line[i]).toDouble(&allNumbers);
			if (!allNumbers)
				break;
		}

		if (renameCols && !allNumbers)
			rows--;

		QProgressDialog progress(0, "progress", true, Qt::WindowStaysOnTopHint|Qt::Tool);
		progress.setWindowTitle("Qtiplot - Reading file...");
		progress.setLabelText(fname);
		progress.setActiveWindow();
		progress.setAutoClose(true);
		progress.setAutoReset(true);

		int steps = int(rows/1000);
		progress.setMaximum(steps+1);

		QApplication::restoreOverrideCursor();

		if (!importFileAs)
			init (rows, cols);
		else if (importFileAs == 1)
		{//new cols
			addColumns(cols);
			if (r < rows)
				d_table->setRowCount(rows);
		}
		else if (importFileAs == 2)
		{//new rows
			if (c < cols)
				addColumns(cols-c);
			d_table->setRowCount(r+rows);
		}

		f.reset();
		for (i=0; i<ignoredLines; i++)
			t.readLine();

		int startRow = 0, startCol =0;
		if (importFileAs == 2)
			startRow = r;
		else if (importFileAs == 1)
			startCol = c;

		if (renameCols && !allNumbers)
		{//use first line to set the table header
			s = t.readLine();
			if (simplifySpaces)
				s = s.simplified();
			else if (stripSpaces)
				s = s.trimmed();

			line = s.split(sep, QString::SkipEmptyParts);
			int end = startCol+(int)line.count();
			for (i=startCol; i<end; i++)
				col_label[i] = QString::null;
			for (i=startCol; i<end; i++)
			{
				comments[i] = line[i-startCol];
				s = line[i-startCol].replace("-","_").remove(QRegExp("\\W")).replace("_","-");
				int n = col_label.count(s);
				if(n)
				{
					//avoid identical col names
					while (col_label.contains(s+QString::number(n)))
						n++;
					s += QString::number(n);
				}
				col_label[i] = s;
			}
		}
		setHeaderColType();

		for (i=0; i<steps; i++)
		{
			if (progress.wasCanceled())
			{
				f.close();
				return;
			}

			for (int k=0; k<1000; k++)
			{
				s = t.readLine();
				if (simplifySpaces)
					s = s.simplified();
				else if (stripSpaces)
					s = s.trimmed();
				line = s.split(sep);
				for (int j=startCol; j<d_table->columnCount(); j++)
					setText(startRow + k, j, line[j-startCol]);
			}

			startRow+= 1000;
			progress.setValue(i);
		}

		for (i=startRow; i<d_table->rowCount(); i++)
		{
			s = t.readLine();
			if (simplifySpaces)
				s = s.simplified();
			else if (stripSpaces)
				s = s.trimmed();
			line = s.split(sep);
			for (int j=startCol; j<d_table->columnCount(); j++)
				setText(i, j, line[j-startCol]);
		}
		progress.setValue(steps+1);
		f.close();

		if (importFileAs)
		{
			for (i=0; i<d_table->columnCount(); i++)
				emit modifiedData(this, colName(i));
		}
	}
}

void Table::importASCII(const QString &fname, const QString &sep, int ignoredLines,
		bool renameCols, bool stripSpaces, bool simplifySpaces, bool newTable)
{
	QFile f(fname);
	QTextStream t( &f );// use a text stream
	if ( f.open(QIODevice::ReadOnly) )
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

		int i, c, rows = 1, cols = 0;
		for (i=0; i<ignoredLines; i++)
			t.readLine();

		QString s = t.readLine();//read first line after the ignored ones
		while ( !t.atEnd() )
		{
			t.readLine();
			rows++;
			qApp->processEvents(QEventLoop::ExcludeUserInput);
		}

		if (simplifySpaces)
			s = s.simplified();
		else if (stripSpaces)
			s = s.trimmed();

		QStringList line = s.split(sep);
		cols = (int)line.count();

		bool allNumbers = true;
		line.replaceInStrings ( ",", "." ); //Qt uses decimal dot
		for (i=0; i<cols; i++)
		{//verify if the strings in the line used to rename the columns are not all numbers
			(line[i]).toDouble(&allNumbers);
			if (!allNumbers)
				break;
		}

		if (renameCols && !allNumbers)
			rows--;
		int steps = int(rows/1000);

		QProgressDialog progress(0, "progress", true, Qt::WindowStaysOnTopHint|Qt::Tool);
		progress.setWindowTitle("Qtiplot - Reading file...");
		progress.setLabelText(fname);
		progress.setActiveWindow();
		progress.setAutoClose(true);
		progress.setAutoReset(true);
		progress.setMaximum(steps+1);

		QApplication::restoreOverrideCursor();

		QStringList oldHeader;
		if (newTable)
			init (rows, cols);
		else
		{
			if (d_table->rowCount() != rows)
				d_table->setRowCount(rows);

			c = d_table->columnCount();
			oldHeader = col_label;
			if (c != cols)
			{
				if (c < cols)
					addColumns(cols - c);
				else
				{
					d_table->setColumnCount(cols);
					for (i=cols; i<c; i++)
						emit removedCol(QString(name()) + "_" + oldHeader[i]);
				}
			}
		}

		f.reset();
		for (i=0; i<ignoredLines; i++)
			t.readLine();

		if (renameCols && !allNumbers)
		{//use first line to set the table header
			s = t.readLine();
			if (simplifySpaces)
				s = s.simplified();
			else if (stripSpaces)
				s = s.trimmed();
			line = s.split(sep, QString::SkipEmptyParts);
			for (i=0; i<(int)line.count(); i++)
				col_label[i] = QString::null;

			for (i=0; i<(int)line.count(); i++)
			{
				comments[i] = line[i];
				s = line[i].replace("-","_").remove(QRegExp("\\W")).replace("_","-");
				int n = col_label.count(s);
				if(n)
				{
					//avoid identical col names
					while (col_label.contains(s+QString::number(n)))
						n++;
					s += QString::number(n);
				}
				col_label[i] = s;
			}
		}
		setHeaderColType();

		int start = 0;
		for (i=0; i<steps; i++)
		{
			if (progress.wasCanceled())
			{
				f.close();
				return;
			}

			start = i*1000;
			for (int k=0; k<1000; k++)
			{
				s = t.readLine();
				if (simplifySpaces)
					s = s.simplified();
				else if (stripSpaces)
					s = s.trimmed();
				line = s.split(sep);
				int lc = (int)line.count();
				if (lc > cols) {
					addColumns(lc - cols);
					cols = lc;
				}
				for (int j=0; j<cols && j<lc; j++)
					setText(start + k, j, line[j]);
			}
			progress.setValue(i);
			qApp->processEvents();
		}

		start = steps*1000;
		for (i=start; i<rows; i++)
		{
			s = t.readLine();
			if (simplifySpaces)
				s = s.simplified();
			else if (stripSpaces)
				s = s.trimmed();
			line = s.split(sep);
			int lc = (int)line.count();
			if (lc > cols) {
				addColumns(lc - cols);
				cols = lc;
			}
			for (int j=0; j<cols && j<lc; j++)
				setText(i, j, line[j]);
		}
		progress.setValue(steps+1);
		qApp->processEvents();
		f.close();

		if (!newTable)
		{
			if (cols > c)
				cols = c;
			for (i=0; i<cols; i++)
			{
				emit modifiedData(this, colName(i));
				if (colLabel(i) != oldHeader[i])
					emit changedColHeader(QString(name())+"_"+oldHeader[i],
							QString(name())+"_"+colLabel(i));
			}
		}
	}
}

bool Table::exportToASCIIFile(const QString& fname, const QString& separator,
		bool withLabels,bool exportSelection)
{
	QFile f(fname);
	if ( !f.open( QIODevice::WriteOnly ) )
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, tr("QtiPlot - ASCII Export Error"),
				tr("Could not write to file: <br><h4>"+fname+ "</h4><p>Please verify that you have the right to write to this location!").arg(fname));
		return false;
	}

	QString the_text;
	int i,j;
	int rows=d_table->rowCount();
	int cols=d_table->columnCount();
	int selectedCols = 0;
	int topRow = 0, bottomRow = 0;
	int *sCols;
	if (exportSelection)
	{
		for (i=0; i<cols; i++)
		{
			if (isColumnSelected(i))
				selectedCols++;
		}

		sCols = new int[selectedCols];
		int aux = 1;
		for (i=0; i<cols; i++)
		{
			if (isColumnSelected(i))
			{
				sCols[aux] = i;
				aux++;
			}
		}

		for (i=0; i<rows; i++)
		{
			if (isRowSelected(i))
			{
				topRow = i;
				break;
			}
		}

		for (i=rows - 1; i>0; i--)
		{
			if (isRowSelected(i))
			{
				bottomRow = i;
				break;
			}
		}
	}

	if (withLabels)
	{
		QStringList header=colNames();
		QStringList ls=header.grep ( QRegExp ("\\D"));
		if (exportSelection)
		{
			for (i=1;i<selectedCols;i++)
			{
				if (ls.count()>0)
					the_text+=header[sCols[i]]+separator;
				else
					the_text+="C"+header[sCols[i]]+separator;
			}

			if (ls.count()>0)
				the_text+=header[sCols[selectedCols]] + "\n";
			else
				the_text+="C"+header[sCols[selectedCols]] + "\n";
		}
		else
		{
			if (ls.count()>0)
			{
				for (j=0; j<cols-1; j++)
					the_text+=header[j]+separator;
				the_text+=header[cols-1]+"\n";
			}
			else
			{
				for (j=0; j<cols-1; j++)
					the_text+="C"+header[j]+separator;
				the_text+="C"+header[cols-1]+"\n";
			}
		}
	}// finished writting labels

	if (exportSelection)
	{
		for (i=topRow;i<=bottomRow; i++)
		{
			for (j=1;j<selectedCols;j++)
				the_text+=text(i, sCols[j]) + separator;
			the_text+=text(i, sCols[selectedCols]) + "\n";
		}
		delete[] sCols;//free memory
	}
	else
	{
		for (i=0;i<rows;i++)
		{
			for (j=0;j<cols-1;j++)
				the_text+=text(i,j)+separator;
			the_text+=text(i,cols-1)+"\n";
		}
	}
	QTextStream t( &f );
	t << the_text;
	f.close();
	return true;
}

void Table::contextMenuEvent(QContextMenuEvent *e)
{
	int right;
	right = d_table->columnViewportPosition(d_table->columnCount()-1) +
		d_table->columnWidth(d_table->columnCount()-1) - 1;

	setFocus();
	if (e->pos().x() > right + d_table->verticalHeader()->width())
		emit showContextMenu(false);
	else
		emit showContextMenu(true);
	e->accept();
}

void Table::headerDoubleClickedHandler(int section)
{
	emit optionsDialog();
	setActiveWindow();
}

void Table::customEvent(QCustomEvent *e)
{
	if (e->type() == SCRIPTING_CHANGE_EVENT)
		scriptingChangeEvent((ScriptingChangeEvent*)e);
}

QString& Table::getSpecifications()
{
	return specifications;
}

void Table::setSpecifications(const QString& s)
{
	if (specifications == s)
		return;

	specifications=s;
}

void Table::setNewSpecifications()
{

	newSpecifications= saveToString("geometry\n");
}

QString& Table::getNewSpecifications()
{
	return newSpecifications;
}

QString Table::oldCaption()
{
	QTextStream ts( &specifications, QIODevice::ReadOnly );
	ts.readLine();
	QString s=ts.readLine();
	int pos=s.find("\t",0);
	return s.left(pos);
}

QString Table::newCaption()
{
	QTextStream ts(&newSpecifications, QIODevice::ReadOnly );
	ts.readLine();
	QString s=ts.readLine();
	int pos=s.find("\t",0);
	return s.left(pos);
}

// TODO: This should probably be changed to restore(QString * spec)
void Table::restore(QString& spec)
{
	int i, j, row;
	int cols=d_table->columnCount();
	int rows=d_table->rowCount();

	QString specCopy = spec;

	QTextStream t(&specCopy, QIODevice::ReadOnly);
	t.readLine();	//table tag
	QString s = t.readLine();
	QStringList list = s.split("\t");

	QString oldCaption = name();
	QString newCaption=list[0];
	if (oldCaption != newCaption)
		this->setName(newCaption);

	int r=list[1].toInt();
	if (rows != r)
		d_table->setRowCount(r);

	int c=list[2].toInt();
	if (cols != c)
		d_table->setColumnCount(c);

	//clear all cells
	for (i=0;i<rows;i++)
	{
		for (j=0; j<c; j++)
			setText(i,j, "");
	}

	t.readLine();	//table geometry useless info when restoring
	s = t.readLine();//header line

	list = s.split("\t");
	list.remove(list.first());

	if (!col_label.isEmpty() && col_label != list)
	{
		loadHeader(list);
		list.replaceInStrings("[X]","");
		list.replaceInStrings("[Y]","");
		list.replaceInStrings("[Z]","");
		list.replaceInStrings("[xEr]","");
		list.replaceInStrings("[yEr]","");

		for (j=0;j<c;j++)
		{
			if (!list.contains(col_label[j]))
				emit changedColHeader(newCaption + "_"+col_label[j], newCaption+"_"+list[j]);
		}

		if (c<cols)
		{
			for (j=0;j<c;j++)
			{
				if (!list.contains(col_label[j]))
					emit removedCol(oldCaption + "_" + col_label[j]);
			}
		}
	}

	s= t.readLine();	//colWidth line
	list = s.split("\t");
	list.remove(list.first());
	if (columnWidths() != list)
		setColWidths(list);

	s = t.readLine();
	list = s.split("\t");
	if (list[0] == "com") //commands line
	{
		list.remove(list.first());
		if (list != commands)
			commands = list;
	} else { // commands block
		commands.clear();
		for (int i=0; i<numCols(); i++)
			commands << "";
		for (s=t.readLine(); s != "</com>"; s=t.readLine())
		{
			int col = s.mid(9,s.length()-11).toInt();
			QString formula;
			for (s=t.readLine(); s != "</col>"; s=t.readLine())
				formula += s + "\n";
			formula.truncate(formula.length()-1);
			setCommand(col,formula);
		}
	}

	s= t.readLine();	//colType line ?
	list = s.split("\t");
	if (s.contains ("ColType",true))
	{
		list.remove(list.first());
		for (i=0; i<int(list.count()); i++)
		{
			QStringList l= list[i].split(";");
			colTypes[i] = l[0].toInt();

			if ((int)l.count() > 0)
				col_format[i] = l[1];
		}
	}
	else //if fileVersion < 65 set table values
	{
		row = list[0].toInt();
		for (j=0; j<cols; j++)
			setText(row, j, list[j+1]);
	}

	s= t.readLine();	//comments line ?
	list = s.split("\t");
	if (s.contains ("Comments",true))
	{
		list.remove(list.first());
		comments = list;
	}

	s= t.readLine();
	list = s.split("\t");
	if (s.contains ("WindowLabel",true))
	{
		setWindowLabel(list[1]);
		setCaptionPolicy((MyWidget::CaptionPolicy)list[2].toInt());
	}

	s= t.readLine();
	if (s == "<data>")
		s = t.readLine();

	while (!t.atEnd () && s != "</data>")
	{
		list = s.split("\t");

		row = list[0].toInt();
		for (j=0; j<c; j++)
			setText(row, j, list[j+1]);

		s= t.readLine();
	}

	for (j=0; j<c; j++)
		emit modifiedData(this, colName(j));
}

void Table::setNumRows(int rows)
{
	d_table->setRowCount(rows);
}

void Table::setNumCols(int cols)
{
	d_table->setColumnCount(cols);
}

void Table::resizeRows(int r)
{
	int rows = d_table->rowCount();
	if (rows == r)
		return;

	if (rows > r)
	{
		QString the_text= tr("Rows will be deleted from the table!");
		the_text+="<p>"+tr("Do you really want to continue?");
		int i,cols = d_table->columnCount();
		switch( QMessageBox::information(this,tr("QtiPlot"), the_text, tr("Yes"), tr("Cancel"), 0, 1 ) )
		{
			case 0:
				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				d_table->setRowCount(r);
				for (i=0; i<cols; i++)
					emit modifiedData(this, colName(i));

				QApplication::restoreOverrideCursor();
				break;

			case 1:
				return;
				break;
		}
	}
	else
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		d_table->setRowCount(r);
		QApplication::restoreOverrideCursor();
	}

	emit modifiedWindow(this);
}

void Table::resizeCols(int c)
{
	int i, cols = d_table->columnCount();
	if (cols == c)
		return;

	if (cols > c)
	{
		QString the_text= tr("Columns will be deleted from the table!");
		the_text+="<p>"+tr("Do you really want to continue?");
		switch( QMessageBox::information(this,tr("QtiPlot"), the_text, tr("Yes"), tr("Cancel"), 0, 1 ) )
		{
			case 0:
				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				for (i=cols-1; i>=c; i--)
				{
					QString name = colName(i);
					emit removedCol(name);

					commands.removeLast();
					comments.removeLast();
					col_format.removeLast();
					col_label.removeLast();
					colTypes.removeLast();
					col_plot_type.removeLast();
				}

				d_table->setColumnCount(c);
				QApplication::restoreOverrideCursor();
				break;

			case 1:
				return;
				break;
		}
	}
	else
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		addColumns(c-cols);
		setHeaderColType();
		QApplication::restoreOverrideCursor();
	}
	emit modifiedWindow(this);
}

void Table::copy(Table *m)
{
	for (int i=0; i<d_table->rowCount(); i++)
	{
		for (int j=0; j<d_table->columnCount(); j++)
			setText(i,j,m->text(i,j));
	}

	setColWidths(m->columnWidths());
	col_label = m->colNames();
	col_plot_type = m->plotDesignations();
	setHeaderColType();

	commands = m->getCommands();
	setColumnTypes(m->columnTypes());
	col_format = m->getColumnsFormat();
	comments = m->colComments();
}

QString Table::saveAsTemplate(const QString& geometryInfo)
{
	QString s="<table>\t"+QString::number(d_table->rowCount())+"\t";
	s+=QString::number(d_table->columnCount())+"\n";
	s+=geometryInfo;
	s+=saveHeader();
	s+=saveColumnWidths();
	s+=saveCommands();
	s+=saveColumnTypes();
	s+=saveComments();
	return s;
}

void Table::restore(const QStringList& lst)
{
	QStringList l;
	QStringList::const_iterator i=lst.begin();

	l= (*i++).split("\t");
	l.remove(l.first());
	loadHeader(l);

	setColWidths((*i).right((*i).length()-9).split("\t", QString::SkipEmptyParts));
	i++;

	l = (*i++).split("\t");
	if (l[0] == "com")
	{
		l.remove(l.first());
		setCommands(l);
	} else if (l[0] == "<com>") {
		commands.clear();
		for (int col=0; col<numCols(); col++)
			commands << "";
		for (; i != lst.end() && *i != "</com>"; i++)
		{
			int col = (*i).mid(9,(*i).length()-11).toInt();
			QString formula;
			for (i++; i!=lst.end() && *i != "</col>"; i++)
				formula += *i + "\n";
			formula.truncate(formula.length()-1);
			commands[col] = formula;
		}
		i++;
	}

	l = (*i++).split("\t");
	l.remove(l.first());
	setColumnTypes(l);

	l = (*i++).split("\t");
	l.remove(l.first());
	setColComments(l);
}

void Table::notifyChanges()
{
	for (int i=0; i<d_table->columnCount(); i++)
		emit modifiedData(this, colName(i));

	emit modifiedWindow(this);
}

void Table::clear()
{
	for (int i=0; i<d_table->columnCount(); i++)
	{
		for (int j=0; j<d_table->rowCount(); j++)
			setText(j, i, QString::null);

		emit modifiedData(this, colName(i));
	}

	emit modifiedWindow(this);
}

bool Table::isRowSelected(int row, bool full)
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

bool Table::isColumnSelected(int col, bool full)
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

void Table::goToRow(int row)
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

void Table::setColumnHeader(int index, QString label)
{
	QTableWidgetItem *item;
	item = d_table->horizontalHeaderItem(index);
	if (!item) 
	{
		item = new QTableWidgetItem();
		d_table->setHorizontalHeaderItem(index, item);
	}
	item->setText(label);
}


Table::~Table()
{
}
