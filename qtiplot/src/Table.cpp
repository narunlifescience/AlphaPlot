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

#include <QMessageBox>
#include <QDateTime>
#include <QTextStream>
#include <QClipboard>
#include <QApplication>
#include <QPainter>
#include <QEvent>
#include <QLayout>
#include <QPrintDialog>
#include <QLocale>
#include <QShortcut>
#include <QProgressDialog>
#include <QFile>

#include <Q3TextStream>
#include <q3paintdevicemetrics.h>
#include <q3dragobject.h>
#include <Q3TableSelection>
#include <Q3MemArray>

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
	selectedCol=-1;
	d_saved_cells = 0;
	d_show_comments = false;
	d_numeric_precision = 14;

	setBirthDate(QDateTime::currentDateTime().toString(Qt::LocalDate));

	d_table = new MyTable(rows, cols, this, "table");
	d_table->setFocusPolicy(Qt::StrongFocus);
	d_table->setFocus();
	d_table->setSelectionMode (Q3Table::Single);
	d_table->setRowMovingEnabled(true);
	// TODO: would be useful, but then we have to interpret
	// signal Q3Header::indexChange ( int section, int fromIndex, int toIndex )
	// and update some variables
	//d_table->setColumnMovingEnabled(true);

	connect(d_table->verticalHeader(), SIGNAL(indexChange ( int, int, int )),
			this, SLOT(notifyChanges()));

	QVBoxLayout* hlayout = new QVBoxLayout(this);
	hlayout->setMargin(0);
	hlayout->addWidget(d_table);

	for (int i=0; i<cols; i++)
	{
		commands << "";
		colTypes << Numeric;
		col_format << "0/14";
		comments << "";
		col_label << QString::number(i+1);
		col_plot_type << Y;
	}

	Q3Header* head=(Q3Header*)d_table->horizontalHeader();
	head->setMouseTracking(true);
	head->setResizeEnabled(true);
	head->installEventFilter(this);
	connect(head, SIGNAL(sizeChange(int, int, int)), this, SLOT(colWidthModified(int, int, int)));

	col_plot_type[0] = X;
	setHeaderColType();

	int w=4*(d_table->horizontalHeader())->sectionSize(0);
	int h;
	if (rows>11)
		h=11*(d_table->verticalHeader())->sectionSize(0);
	else
		h=(rows+1)*(d_table->verticalHeader())->sectionSize(0);
	setGeometry(50, 50, w + 45, h);

	d_table->verticalHeader()->setResizeEnabled(false);
	d_table->verticalHeader()->installEventFilter(this);

	QShortcut *accelTab = new QShortcut(QKeySequence(Qt::Key_Tab), this);
	connect(accelTab, SIGNAL(activated()), this, SLOT(moveCurrentCell()));

	QShortcut *accelAll = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_A), this);
	connect(accelAll, SIGNAL(activated()), this, SLOT(selectAllTable()));

	connect(d_table, SIGNAL(valueChanged(int,int)),this, SLOT(cellEdited(int,int)));
}

void Table::colWidthModified(int, int, int)
{
	emit modifiedWindow(this);
	setHeaderColType();
}


void Table::setBackgroundColor(const QColor& col)
{
	d_table->setPaletteBackgroundColor ( col );
}

void Table::setTextColor(const QColor& col)
{
	d_table->setPaletteForegroundColor (col);
}

void Table::setTextFont(const QFont& fnt)
{
	d_table->setFont (fnt);
	QFontMetrics fm(fnt);
	int lm = fm.width( QString::number(10*d_table->numRows()));
	d_table->setLeftMargin( lm );
}

void Table::setHeaderColor(const QColor& col)
{
	d_table->horizontalHeader()->setPaletteForegroundColor (col);
}

void Table::setHeaderFont(const QFont& fnt)
{
	d_table->horizontalHeader()->setFont (fnt);
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

		Q3PaintDeviceMetrics metrics( p.device() );
		int dpiy = metrics.logicalDpiY();
		const int margin = (int) ( (1/2.54)*dpiy ); // 2 cm margins

		Q3Header *hHeader = d_table->horizontalHeader();
		Q3Header *vHeader = d_table->verticalHeader();

		int rows=d_table->numRows();
		int cols=d_table->numCols();
		int height=margin;
		int i,vertHeaderWidth=vHeader->width();
		int right = margin + vertHeaderWidth;

		// print header
		p.setFont(hHeader->font());
		QRect br=p.boundingRect(br,Qt::AlignCenter,	hHeader->label(0));
		p.drawLine(right,height,right,height+br.height());
		QRect tr(br);

		for (i=0;i<cols;i++)
		{
			int w=d_table->columnWidth (i);
			tr.setTopLeft(QPoint(right,height));
			tr.setWidth(w);
			tr.setHeight(br.height());
			p.drawText(tr,Qt::AlignCenter,hHeader->label(i),-1);
			right+=w;
			p.drawLine(right,height,right,height+tr.height());

			if (right >= metrics.width()-2*margin )
				break;
		}
		p.drawLine(margin + vertHeaderWidth, height, right-1, height);//first horizontal line
		height += tr.height();
		p.drawLine(margin,height,right-1,height);

		// print table values
		for (i=0;i<rows;i++)
		{
			right = margin;
			QString text = vHeader->label(i)+"\t";
			tr = p.boundingRect(tr,Qt::AlignCenter,text);
			p.drawLine(right,height,right,height+tr.height());

			br.setTopLeft(QPoint(right,height));
			br.setWidth(vertHeaderWidth);
			br.setHeight(tr.height());
			p.drawText(br,Qt::AlignCenter,text,-1);
			right += vertHeaderWidth;
			p.drawLine(right,height,right,height+tr.height());

			for (int j=0;j<cols;j++)
			{
				int w=d_table->columnWidth (j);
				text=d_table->text(i,j)+"\t";
				tr=p.boundingRect(tr,Qt::AlignCenter,text);
				br.setTopLeft(QPoint(right,height));
				br.setWidth(w);
				br.setHeight(tr.height());
				p.drawText(br,Qt::AlignCenter,text,-1);
				right+=w;
				p.drawLine(right,height,right,height+tr.height());

				if (right >= metrics.width()-2*margin )
					break;
			}
			height+=br.height();
			p.drawLine(margin,height,right-1,height);

			if (height >= metrics.height()-margin )
			{
				printer.newPage();
				height=margin;
				p.drawLine(margin,height,right,height);
			}
		}
}

void Table::cellEdited(int row, int col)
{
	QString text = d_table->text(row,col).remove(QRegExp("\\s"));
	if (columnType(col) != Numeric || text.isEmpty())
	{
		emit modifiedData(this, colName(col));
		emit modifiedWindow(this);
		return;
	}

	char f;
	int precision;
  	columnNumericFormat(col, &f, &precision);
  	bool ok = true;
  	QLocale locale;
  	double res = locale.toDouble(text, &ok);
  	if (ok)
  		d_table->setText(row, col, locale.toString(res, f, precision));
  	else
  	{
  	Script *script = scriptEnv->newScript(d_table->text(row,col),this,QString("<%1_%2_%3>").arg(name()).arg(row+1).arg(col+1));
  	connect(script, SIGNAL(error(const QString&,const QString&,int)), scriptEnv, SIGNAL(error(const QString&,const QString&,int)));

  	script->setInt(row+1, "i");
  	script->setInt(col+1, "j");
  	QVariant ret = script->eval();
  	if(ret.type()==QVariant::Int || ret.type()==QVariant::UInt || ret.type()==QVariant::LongLong || ret.type()==QVariant::ULongLong)
  		d_table->setText(row, col, ret.toString());
  	else if(ret.canCast(QVariant::Double))
  		d_table->setText(row, col, locale.toString(ret.toDouble(), f, precision));
  	else
  		d_table->setText(row, col, "");
  	}

  	emit modifiedData(this, colName(col));
	emit modifiedWindow(this);
}

int Table::colX(int col)
{
	int i, xcol = -1;
	for(i=col-1; i>=0; i--)
	{
		if (col_plot_type[i] == X)
			return i;
	}
	for(i=col+1; i<(int)d_table->numCols(); i++)
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
	for(i=col+1; i<(int)d_table->numCols(); i++)
	{
		if (col_plot_type[i] == Y)
			return i;
	}
	return yCol;
}

void Table::setPlotDesignation(PlotDesignation pd)
{
	QStringList list=selectedColumns();
	for (int i=0;i<(int) list.count(); i++)
		col_plot_type[colIndex(list[i])] = pd;

	setHeaderColType();
	emit modifiedWindow(this);
}

void Table::columnNumericFormat(int col, int *f, int *precision)
{
	QStringList format = col_format[col].split("/", QString::KeepEmptyParts);
	if (format.count() == 2)
	{
		*f = format[0].toInt();
		*precision = format[1].toInt();
	}
	else
	{
		*f = 0;
		*precision = 14;
	}
}

void Table::columnNumericFormat(int col, char *f, int *precision)
{
	QStringList format = col_format[col].split("/", QString::KeepEmptyParts);
	if (format.count() == 2)
	{
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
	else
	{
		*f = 'g';
		*precision = 14;
	}
}

int Table::columnWidth(int col)
{
	return d_table->columnWidth(col);
}

QStringList Table::columnWidths()
{
	QStringList widths;
	for (int i=0;i<d_table->numCols();i++)
		widths<<QString::number(d_table->columnWidth(i));

	return widths;
}

void Table::setColWidths(const QStringList& widths)
{
	for (int i=0;i<(int)widths.count();i++)
		d_table->setColumnWidth (i, widths[i].toInt() );
}

void Table::setColumnTypes(const QStringList& ctl)
{
	int n = qMin((int)ctl.count(), numCols());
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
	QString s="ColWidth\t";
	for (int i=0;i<d_table->numCols();i++)
		s+=QString::number(d_table->columnWidth (i))+"\t";

	return s+"\n";
}

QString Table::saveColumnTypes()
{
	QString s="ColType";
	for (int i=0; i<d_table->numCols(); i++)
		s += "\t"+QString::number(colTypes[i])+";"+col_format[i];

	return s+"\n";
}

void Table::setCommands(const QStringList& com)
{
	commands.clear();
	for(int i=0; i<(int)com.size() && i<numCols(); i++)
		commands << com[i].stripWhiteSpace();
}

void Table::setCommand(int col, const QString com)
{
	if(col<(int)commands.size())
		commands[col]=com.stripWhiteSpace();
}

void Table::setCommands(const QString& com)
{
	QStringList lst = com.split("\t");
	lst.pop_front();
	setCommands(lst);
}

bool Table::calculate(int col, int startRow, int endRow)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	Script *colscript = scriptEnv->newScript(commands[col], this,  QString("<%1>").arg(colName(col)));
	connect(colscript, SIGNAL(error(const QString&,const QString&,int)), scriptEnv, SIGNAL(error(const QString&,const QString&,int)));
	connect(colscript, SIGNAL(print(const QString&)), scriptEnv, SIGNAL(print(const QString&)));

	if (!colscript->compile())
	{
		QApplication::restoreOverrideCursor();
		return false;
	}
	if (endRow >= numRows())
		resizeRows(endRow + 1);

	colscript->setInt(col+1, "j");
	colscript->setInt(startRow+1, "sr");
	colscript->setInt(endRow+1, "er");
	QVariant ret;
	for (int i=startRow; i<=endRow; i++)
	{
		colscript->setInt(i+1,"i");
		ret = colscript->eval();
		if(ret.type()==QVariant::Double) {
			int prec;
			char f;
			columnNumericFormat(col, &f, &prec);
			d_table->setText(i, col, QLocale().toString(ret.toDouble(), f, prec));
		} else if(ret.canConvert(QVariant::String))
			d_table->setText(i, col, ret.toString());
		else {
			QApplication::restoreOverrideCursor();
			return false;
		}
	}

	emit modifiedData(this, colName(col));
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
	return true;
}

Q3TableSelection Table::getSelection()
{
	Q3TableSelection sel;
	if (d_table->numSelections()==0)
	{
		sel.init(d_table->currentRow(), d_table->currentColumn());
		sel.expandTo(d_table->currentRow(), d_table->currentColumn());
	} else if (d_table->currentSelection()>0)
		sel = d_table->selection(d_table->currentSelection());
	else
		sel = d_table->selection(0);
	return sel;
}

bool Table::calculate()
{
	Q3TableSelection sel = getSelection();
	bool success = true;
	for (int col=sel.leftCol(); col<=sel.rightCol(); col++)
		if (!calculate(col, sel.topRow(), sel.bottomRow()))
			success = false;
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
	for (int i=0; i<d_table->numCols(); i++)
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
	s+=QString::number(d_table->numRows())+"\t";
	s+=QString::number(d_table->numCols())+"\t";
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
	QString s = "header";
	for (int j=0; j<d_table->numCols(); j++){
		if (col_plot_type[j] == X)
			s += "\t" + colLabel(j) + "[X]";
		else if (col_plot_type[j] == Y)
			s += "\t" + colLabel(j) + "[Y]";
		else if (col_plot_type[j] == Z)
			s += "\t" + colLabel(j) + "[Z]";
		else if (col_plot_type[j] == xErr)
			s += "\t" + colLabel(j) + "[xEr]";
		else if (col_plot_type[j] == yErr)
			s += "\t" + colLabel(j) + "[yEr]";
		else
			s += "\t" + colLabel(j);
	}
	return s += "\n";
}

int Table::firstXCol()
{
	int xcol = -1;
	for (int j=0; j<d_table->numCols(); j++)
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

	QString oldLabel=colLabel(selectedCol);
	QStringList oldLabels=colNames();
	QString caption=QString(this->name())+"_";
	int n=1;
	for (int i=selectedCol; i<(int)d_table->numCols(); i++)
	{
		QString newLabel=oldLabel+QString::number(n);
		commands.replaceInStrings("col(\""+colLabel(i)+"\")", "col(\""+newLabel+"\")");
		setColName(i, newLabel);
		emit changedColHeader(caption+oldLabels[i],colName(i));
		n++;
	}
	emit modifiedWindow(this);
}

void Table::setColComment(int col, const QString& s)
{
	if (col < 0 || col >= d_table->numCols())
		return;

	if (comments[col] == s)
		return;

	comments[col] = s;

	if (d_show_comments)
		setHeaderColType();
}

void Table::changeColWidth(int width, bool allCols)
{
	int cols=d_table->numCols();
	if (allCols)
	{
		for (int i=0;i<cols;i++)
			d_table->setColumnWidth (i, width);

		emit modifiedWindow(this);
	}
	else
	{
		if (d_table->columnWidth(selectedCol) == width)
			return;

		d_table->setColumnWidth (selectedCol, width);
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

void Table::changeColName(const QString& text)
{
	QString caption = this->name();
	QString oldName = colName(selectedCol);
	QString newName = caption+"_"+text;

	if (oldName == newName)
		return;

	if (caption == text)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),
				tr("The column name must be different from the table name : <b>"+caption+"</b></b>!<p>Please choose another name!"));
		return;
	}

	QStringList labels=colNames();
	if (labels.contains(text)>0)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),
				tr("There is already a column called : <b>"+text+"</b> in table <b>"+caption+"</b>!<p>Please choose another name!"));
		return;
	}

	commands.replaceInStrings("col(\""+colLabel(selectedCol)+"\")", "col(\""+text+"\")");

	setColName(selectedCol, text);
	emit changedColHeader(oldName, newName);
	emit modifiedWindow(this);
}

void Table::setColName(int col, const QString& text)
{
	if (text.isEmpty() || col<0 || col >=d_table->numCols())
		return;

	col_label[col] = text;
	setHeaderColType();
}

QStringList Table::selectedColumns()
{
	QStringList names;
	for (int i=0; i<d_table->numCols(); i++)
	{
		if(d_table->isColumnSelected (i, true))
			names << QString(name()) + "_" + col_label[i];
	}
	return names;
}

QStringList Table::YColumns()
{
	QStringList names;
	for (int i=0;i<d_table->numCols();i++)
	{
		if(col_plot_type[i] == Y)
			names<<QString(name())+"_"+col_label[i];
	}
	return names;
}

QStringList Table::selectedYColumns()
{
	QStringList names;
	for (int i=0;i<d_table->numCols();i++)
	{
	if(d_table->isColumnSelected (i) && col_plot_type[i] == Y)
  		names<<QString(name())+"_"+col_label[i];
  	}
  	return names;
}

QStringList Table::selectedErrColumns()
{
  	QStringList names;
  	for (int i=0;i<d_table->numCols();i++)
  		{
  	    if(d_table->isColumnSelected (i,true) &&
  	       (col_plot_type[i] == yErr || col_plot_type[i] == xErr))
  	       	names<<QString(name())+"_"+col_label[i];
  	    }
  	return names;
}

QStringList Table::drawableColumnSelection()
{
  	QStringList names;
  	for (int i=0; i<d_table->numCols(); i++)
  	{
	if(d_table->isColumnSelected (i) && col_plot_type[i] == Y)
		names << QString(name()) + "_" + col_label[i];
    }

  	for (int i=0; i<d_table->numCols(); i++)
  	{
  	 	if(d_table->isColumnSelected (i) && (col_plot_type[i] == yErr || col_plot_type[i] == xErr))
  	    	names << QString(name()) + "_" + col_label[i];
  	}
	return names;
}

QStringList Table::selectedYLabels()
{
	QStringList names;
	for (int i=0;i<d_table->numCols();i++)
	{
		if(d_table->isColumnSelected (i) && col_plot_type[i] == Y)
			names<<col_label[i];
	}
	return names;
}

QStringList Table::columnsList()
{
	QStringList names;
	for (int i=0;i<d_table->numCols();i++)
		names<<QString(name())+"_"+col_label[i];

	return names;
}

int Table::firstSelectedColumn()
{
	for (int i=0;i<d_table->numCols();i++)
	{
		if(d_table->isColumnSelected (i,true))
			return i;
	}
	return -1;
}

int Table::numSelectedRows()
{
	int r=0;
	for (int i=0;i<d_table->numRows();i++)
	{
		if(d_table->isRowSelected (i,true))
			r++;
	}
	return r;
}

int Table::selectedColsNumber()
{
	int c=0;
	for (int i=0;i<d_table->numCols(); i++)
	{
		if(d_table->isColumnSelected (i,true))
			c++;
	}
	return c;
}

QString Table::colName(int col)
{//returns the table name + horizontal header text
	if (col<0 || col >= col_label.count())
		return QString();

	return QString(this->name())+"_"+col_label[col];
}

QVarLengthArray<double> Table::col(int ycol)
{
	int i;
	int rows=d_table->numRows();
	int cols=d_table->numCols();
	QVarLengthArray<double> Y(rows);
	if (ycol<=cols)
	{
		for (i=0;i<rows;i++)
			Y[i]=d_table->text(i,ycol).toDouble();
	}
	return Y;
}

void Table::insertCols(int start, int count)
{
    if (start < 0)
        start = 0;

	int max = 0;
	for (int i = 0; i<d_table->numCols(); i++)
	{
		if (!col_label[i].contains(QRegExp ("\\D")))
		{
			int id = col_label[i].toInt();
			if (id > max)
				max = id;
		}
	}
    max++;

    d_table->insertColumns(start, count);

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
	if (d_table->isRowSelected (cr, true))
	{
		d_table->insertRows(cr, 1);
		emit modifiedWindow(this);
	}
}

void Table::addCol(PlotDesignation pd)
{
	d_table->clearSelection();
	int index, max=0, cols=d_table->numCols();
	for (int i=0; i<cols; i++)
	{
		if (!col_label[i].contains(QRegExp ("\\D")))
		{
			index = col_label[i].toInt();
			if (index > max)
				max = index;
		}
	}
	d_table->insertColumns(cols,1);
	d_table->ensureCellVisible ( 0, cols );

	comments << QString();
	commands<<"";
	colTypes<<Numeric;
	col_format<<"0/" + QString::number(d_numeric_precision);
	col_label<< QString::number(max+1);
	col_plot_type << pd;

	setHeaderColType();
	emit modifiedWindow(this);
}

void Table::addColumns(int c)
{
	int i, index,max=0, cols=d_table->numCols();
	for (i=0; i<cols; i++)
	{
		if (!col_label[i].contains(QRegExp ("\\D")))
		{
			index=col_label[i].toInt();
			if (index>max)
				max=index;
		}
	}
	max++;
	d_table->insertColumns(cols,c);
	for (i=0; i<c; i++)
	{
		comments << QString();
		commands<<"";
		colTypes<<Numeric;
		col_format<<"0/" + QString::number(d_numeric_precision);
		col_label<< QString::number(max+i);
		col_plot_type << Y;
	}
}

void Table::clearCol()
{
	for (int i=0; i<d_table->numRows(); i++)
	{
		if (d_table->isSelected (i, selectedCol))
			d_table->setText(i, selectedCol, "");
	}
	QString name=colName(selectedCol);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

void Table::clearCell(int row, int col)
{
	d_table->setText(row, col, "");

	QString name=colName(col);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

void Table::deleteSelectedRows()
{
	Q3TableSelection sel=d_table->selection(0);
	int top=sel.topRow();
	int bottom=sel.bottomRow();
	int numberOfRows=bottom-top+1;
	Q3MemArray<int> rowsToDelete(numberOfRows);
	for (int i=0; i<numberOfRows; i++)
		rowsToDelete[i]=top+i;
	d_table->removeRows(rowsToDelete);
	notifyChanges();
}

void Table::cutSelection()
{
	copySelection();
	clearSelection();
}

void Table::selectAllTable()
{
	d_table->addSelection (Q3TableSelection( 0, 0, d_table->numRows(), d_table->numCols() ));
}

void Table::deselect()
{
	d_table->clearSelection();
}

void Table::clearSelection()
{
	QStringList list=selectedColumns();
	int n=int(list.count());

	if (n>0)
	{
		for (int i=0; i<n; i++)
		{
			QString name = list[i];
			selectedCol = colIndex(name);
			clearCol();
		}
	}
	else
	{
		Q3TableSelection sel=d_table->selection(0);
		int top=sel.topRow();
		int bottom=sel.bottomRow();
		int left=sel.leftCol();
		int right=sel.rightCol();

		if (sel.isEmpty ())
		{
			int row=d_table->currentRow ();
			int col=d_table->currentColumn ();
			d_table->setText(row,col, "");

			QString name=colName(col);
			emit modifiedData(this, name);
		}
		else
		{
			for (int i=top;i<=bottom;i++)
			{
				for (int j=left;j<=right;j++)
				{
					d_table->setText(i, j, "");
				}
			}

			for (int i=left;i<=right;i++)
			{
				QString name=colName(i);
				emit modifiedData(this, name);
			}
		}
	}
	emit modifiedWindow(this);
}

void Table::copySelection()
{
	QString text;
	int i,j;
	int rows=d_table->numRows();
	int cols=d_table->numCols();

	QVarLengthArray<int> selection(1);
	int c=0;
	for (i=0;i<cols;i++)
	{
		if (d_table->isColumnSelected(i,true))
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
				text+=d_table->text(i,selection[j])+"\t";
			text+=d_table->text(i,selection[c-1])+"\n";
		}
	}
	else
	{
		Q3TableSelection sel = d_table->selection(0);
		int right=sel.rightCol();
		int bottom=sel.bottomRow();
		for (i=sel.topRow(); i<=bottom; i++)
		{
			for (j=sel.leftCol(); j<right; j++)
				text+=d_table->text(i,j)+"\t";
			text+=d_table->text(i,right)+"\n";
		}
	}

	// Copy text into the clipboard
	QApplication::clipboard()->setText(text);
}

// Paste text from the clipboard
void Table::pasteSelection()
{
	QString text = QApplication::clipboard()->text();
	if (text.isEmpty())
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QTextStream ts( &text, QIODevice::ReadOnly );
	QString s = ts.readLine();
	QStringList cellTexts=s.split("\t");
	int cols=int(cellTexts.count());
	int rows= 1;
	while(!ts.atEnd())
	{
		rows++;
		s = ts.readLine();
	}
	ts.reset();

	int i, j, top, bottom, right, left, firstCol=firstSelectedColumn();
	Q3TableSelection sel=d_table->selection(0);
	if (!sel.isEmpty())
	{// not columns but only cells are selected
		top=sel.topRow();
		bottom=sel.bottomRow();
		left=sel.leftCol();
		right=sel.rightCol();
	}
	else
	{
		if(cols==1 && rows==1)
		{
			top=bottom=d_table->currentRow();
			left=right=d_table->currentColumn();
		}
		else
		{
			top=0;
			bottom=d_table->numRows() - 1;
			left=0;
			right=d_table->numCols() - 1;
			if (firstCol>=0)
			{// columns are selected
				left=firstCol;
				right=firstCol+selectedColsNumber()-1;
			}
		}
	}

	QTextStream ts2( &text, QIODevice::ReadOnly );
	int r = bottom-top+1;
	int c = right-left+1;

	QApplication::restoreOverrideCursor();
	if (rows>r || cols>c)
	{
		switch( QMessageBox::information(0,"QtiPlot",
					tr("The text in the clipboard is larger than your current selection!\
						\nDo you want to insert cells?"),
					tr("Yes"), tr("No"), tr("Cancel"),0,0))
		{
			case 0:
				if(cols > c)
					insertCols(left, cols - c);

				if(rows > r)
				{
					if (firstCol >= 0)
						d_table->insertRows(top, rows - r);
					else
						d_table->insertRows(top, rows - r + 1);
				}
				break;
			case 1:
				rows = r;
				cols = c;
				break;
			case 2:
				return;
				break;
		}
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	int prec;
	char f;
	bool numeric;
	double value;
	QLocale system_locale = QLocale::system();
	QLocale locale;
	for (i=top; i<top+rows; i++)
	{
		s = ts2.readLine();
		cellTexts=s.split("\t");
		for (j=left; j<left+cols; j++)
		{
			value = system_locale.toDouble(cellTexts[j-left], &numeric);
			if (numeric)
			{
				columnNumericFormat(j, &f, &prec);
				d_table->setText(i, j, locale.toString(value, f, prec));
			}
			else
				d_table->setText(i, j, cellTexts[j-left]);
		}
	}

	for (i=left; i<left+cols; i++)
		emit modifiedData(this, colName(i));

	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::removeCol()
{
	QStringList list=selectedColumns();
	removeCol(list);
}

void Table::removeCol(const QStringList& list)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	for (int i=0; i<list.count(); i++){
		QString name = list[i];
		int id = colIndex(name);
		if (id >= 0){
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
	for (int i=0; i<d_table->numCols(); i++)
	{
		selectedCol = i;
		normalizeCol();
	}
	emit modifiedWindow(this);
}

void Table::normalizeCol(int col)
{
	if (col<0) col = selectedCol;
	double max=d_table->text(0,col).toDouble();
	double aux=0.0;
	int rows=d_table->numRows();
	for (int i=0; i<rows; i++)
	{
		QString text=this->text(i,col);
		aux=text.toDouble();
		if (!text.isEmpty() && fabs(aux)>fabs(max))
			max=aux;
	}

	if (max == 1.0)
		return;

    int prec;
    char f;
    columnNumericFormat(col, &f, &prec);

	for (int i=0; i<rows; i++)
	{
		QString text = this->text(i, col);
		aux=text.toDouble();
		if ( !text.isEmpty() )
			d_table->setText(i, col, QLocale().toString(aux/max, f, prec));
	}

	QString name=colName(col);
	emit modifiedData(this, name);
}

void Table::sortColumnsDialog()
{
	SortDialog *sortd = new SortDialog(this);
	sortd->setAttribute(Qt::WA_DeleteOnClose);
	connect (sortd, SIGNAL(sort(int, int, const QString&)), this, SLOT(sortColumns(int, int, const QString&)));
	sortd->insertColumnsList(selectedColumns());
	sortd->exec();
}

void Table::sortTableDialog()
{
	SortDialog *sortd = new SortDialog(this);
	sortd->setAttribute(Qt::WA_DeleteOnClose);
	connect (sortd, SIGNAL(sort(int, int, const QString&)), this, SLOT(sort(int, int, const QString&)));
	sortd->insertColumnsList(colNames());
	sortd->exec();
}

void Table::sort(int type, int order, const QString& leadCol)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	sortColumns(col_label, type, order, leadCol);
    QApplication::restoreOverrideCursor();
}

void Table::sortColumns(int type, int order, const QString& leadCol)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	sortColumns(selectedColumns(), type, order, leadCol);
    QApplication::restoreOverrideCursor();
}

void Table::sortColumns(const QStringList&s, int type, int order, const QString& leadCol)
{
	int cols=s.count();
	if(!type){//Sort columns separately
		for(int i=0;i<cols;i++)
			sortColumn(colIndex(s[i]), order);
	}else{
		int leadcol = colIndex(leadCol);
		if (leadcol < 0){
			QMessageBox::critical(this, tr("QtiPlot - Error"),
			tr("Please indicate the name of the leading column!"));
			return;
		}
		if (columnType(leadcol) == Table::Text){
			QMessageBox::critical(this, tr("QtiPlot - Error"),
			tr("The leading column has the type set to 'Text'! Operation aborted!"));
			return;
		}

		int rows=d_table->numRows();
		int non_empty_cells = 0;
		QVarLengthArray<int> valid_cell(rows);
		QVarLengthArray<double> data_double(rows);
		for (int j = 0; j <rows; j++){
			if (!d_table->text(j, leadcol).isEmpty()){
				data_double[non_empty_cells] = cell(j,leadcol);
				valid_cell[non_empty_cells] = j;
				non_empty_cells++;
			}
		}

		if (!non_empty_cells){
			QMessageBox::critical(this, tr("QtiPlot - Error"),
			tr("The leading column is empty! Operation aborted!"));
			return;
		}

		data_double.resize(non_empty_cells);
		valid_cell.resize(non_empty_cells);
		QVarLengthArray<QString> data_string(non_empty_cells);
		size_t *p= new size_t[non_empty_cells];

		// Find the permutation index for the lead col
		gsl_sort_index(p, data_double.data(), 1, non_empty_cells);

		for(int i=0;i<cols;i++){// Since we have the permutation index, sort all the columns
            int col=colIndex(s[i]);
            if (columnType(col) == Text){
                for (int j=0; j<non_empty_cells; j++)
                    data_string[j] = text(valid_cell[j], col);
                if(!order)
                    for (int j=0; j<non_empty_cells; j++)
                        d_table->setText(valid_cell[j], col, data_string[p[j]]);
                else
                    for (int j=0; j<non_empty_cells; j++)
                        d_table->setText(valid_cell[j], col, data_string[p[non_empty_cells-j-1]]);
            }else{
                for (int j = 0; j<non_empty_cells; j++)
                    data_double[j] = cell(valid_cell[j], col);
                int prec;
                char f;
                columnNumericFormat(col, &f, &prec);
                if(!order)
                    for (int j=0; j<non_empty_cells; j++)
                        d_table->setText(valid_cell[j], col, QLocale().toString(data_double[p[j]], f, prec));
                else
                    for (int j=0; j<non_empty_cells; j++)
                        d_table->setText(valid_cell[j], col, QLocale().toString(data_double[p[non_empty_cells-j-1]], f, prec));
            }
            emit modifiedData(this, colName(col));
        }
        delete[] p;
    }
	emit modifiedWindow(this);
}

void Table::sortColumn(int col, int order)
{
	if (col < 0)
		col = d_table->currentColumn();

	int rows=d_table->numRows();
	int non_empty_cells = 0;
	QVarLengthArray<int> valid_cell(rows);
	QVarLengthArray<double> r(rows);
    QStringList text_cells;
	for (int i = 0; i <rows; i++)
	{
		if (!d_table->text(i, col).isEmpty())
		{
            if (columnType(col) == Table::Text)
                text_cells << d_table->text(i, col);
            else
			    r[non_empty_cells] = this->text(i,col).toDouble();
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

    if (columnType(col) == Table::Text)
    {
        if (!order)
        {
            for (int i=0; i<non_empty_cells; i++)
                d_table->setText(valid_cell[i], col, text_cells[i]);
        }
		else
        {
            for (int i=0; i<non_empty_cells; i++)
                d_table->setText(valid_cell[i], col, text_cells[non_empty_cells-i-1]);
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
                d_table->setText(valid_cell[i], col, QLocale().toString(r[i], f, prec));
        }
        else
        {
            for (int i=0; i<non_empty_cells; i++)
                d_table->setText(valid_cell[i], col, QLocale().toString(r[non_empty_cells-i-1], f, prec));
        }
    }
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
	return d_table->numRows();
}

int Table::numCols()
{
	return d_table->numCols();
}

bool Table::isEmptyRow(int row)
{
	for (int i=0; i<d_table->numCols(); i++)
	{
		QString text = d_table->text(row,i);
		if (!text.isEmpty())
			return false;
	}
	return true;
}

bool Table::isEmptyColumn(int col)
{
	for (int i=0; i<d_table->numRows(); i++)
	{
		QString text=d_table->text(i,col);
		if (!text.isEmpty())
			return false;
	}
	return true;
}

QString Table::saveText()
{
	QString text = "<data>\n";
	int cols=d_table->numCols();
	int rows=d_table->numRows();

	for (int i=0; i<rows; i++)
	{
		if (!isEmptyRow(i))
		{
			text+=QString::number(i)+"\t";
			for (int j=0; j<cols-1; j++)
				text+=d_table->text(i,j)+"\t";

			text+=d_table->text(i,cols-1)+"\n";
		}
	}
	return text + "</data>\n";
}

int Table::nonEmptyRows()
{
	int r=0;
	for (int i=0;i<d_table->numRows();i++)
	{
		if (!isEmptyRow(i))
			r++;
	}
	return r;
}

double Table::cell(int row, int col)
{
	return stringToDouble(d_table->text(row, col));
}

void Table::setCell(int row, int col, double val)
{
	char format;
    int prec;
    columnNumericFormat(col, &format, &prec);
    d_table->setText(row, col, QLocale().toString(val, format, prec));
}

QString Table::text(int row, int col)
{
	return d_table->text(row,col);
}

void Table::setText (int row, int col, const QString & text )
{
	d_table->setText(row, col, text);
}

void Table::saveToMemory()
{
    d_saved_cells = new double* [d_table->numCols()];
	for ( int i = 0; i < d_table->numCols(); ++i)
		d_saved_cells[i] = new double [d_table->numRows()];

    for (int col = 0; col<d_table->numCols(); col++){// initialize the matrix to zero
        for (int row=0; row<d_table->numRows(); row++)
            d_saved_cells[col][row] = 0.0;}

	for (int col = 0; col<d_table->numCols(); col++){
		if (colTypes[col] == Time){
			QTime ref = QTime(0, 0);
			for (int row=0; row<d_table->numRows(); row++){
				QTime t = QTime::fromString(d_table->text(row, col), col_format[col]);
                d_saved_cells[col][row] = ref.msecsTo(t);
				}
            }
		else if (colTypes[col] == Date){
			QTime ref = QTime(0, 0);
			for (int row=0; row<d_table->numRows(); row++){
				QDateTime dt = QDateTime::fromString(d_table->text(row, col), col_format[col]);
				d_saved_cells[col][row] = dt.date().toJulianDay() - 1 + (double)ref.msecsTo(dt.time())/864.0e5;
				}
            }
        }

    bool wrongLocale = false;
	for (int col = 0; col<d_table->numCols(); col++){
	    if (colTypes[col] == Numeric){
            bool ok = false;
            for (int row=0; row<d_table->numRows(); row++){
                if (!d_table->text(row, col).isEmpty()){
                    d_saved_cells[col][row] = QLocale().toDouble(d_table->text(row, col), &ok);
                    if (!ok){
                        wrongLocale = true;
                        break;
                    }
                }
            }
            if (wrongLocale)
                break;
	    }
	}

	if (wrongLocale){// fall back to C locale
	    wrongLocale = false;
        for (int col = 0; col<d_table->numCols(); col++){
            if (colTypes[col] == Numeric){
                bool ok = false;
                for (int row=0; row<d_table->numRows(); row++){
                    if (!d_table->text(row, col).isEmpty()){
                        d_saved_cells[col][row] = QLocale::c().toDouble(d_table->text(row, col), &ok);
                        if (!ok){
                            wrongLocale = true;
                            break;
                        }
                    }
                }
            if (wrongLocale)
                break;
            }
        }
	}
	if (wrongLocale){// fall back to German locale
	    wrongLocale = false;
        for (int col = 0; col<d_table->numCols(); col++){
            if (colTypes[col] == Numeric){
                bool ok = false;
                for (int row=0; row<d_table->numRows(); row++){
                    if (!d_table->text(row, col).isEmpty()){
                        d_saved_cells[col][row] = QLocale(QLocale::German).toDouble(d_table->text(row, col), &ok);
                        if (!ok){
                            wrongLocale = true;
                            break;
                        }
                    }
                }
            if (wrongLocale)
                break;
            }
        }
	}
	if (wrongLocale){// fall back to French locale
	    wrongLocale = false;
        for (int col = 0; col<d_table->numCols(); col++){
            if (colTypes[col] == Numeric){
                bool ok = false;
                for (int row=0; row<d_table->numRows(); row++){
                    if (!d_table->text(row, col).isEmpty()){
                        d_saved_cells[col][row] = QLocale(QLocale::French).toDouble(d_table->text(row, col), &ok);
                        if (!ok){
                            wrongLocale = true;
                            break;
                        }
                    }
                }
            if (wrongLocale)
                break;
            }
        }
	}
}

void Table::freeMemory()
{
    for ( int i = 0; i < d_table->numCols(); i++)
        delete[] d_saved_cells[i];

	delete[] d_saved_cells;
	d_saved_cells = 0;
}

void Table::setTextFormat(int col)
{
	if (col >= 0 && col < colTypes.count())
		colTypes[col] = Text;
}

void Table::setColNumericFormat(int f, int prec, int col, bool updateCells)
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

    if (!updateCells)
        return;

    char format = 'g';
	for (int i=0; i<d_table->numRows(); i++)
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

			if (d_saved_cells)
				setText(i, col, QLocale().toString(d_saved_cells[col][i], format, prec));
			else
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

bool Table::setDateFormat(const QString& format, int col, bool updateCells)
{
	if (colTypes[col] == Date && col_format[col] == format)
		return true;

	bool first_time = false;
    if (updateCells){
	for (int i=0; i<d_table->numRows(); i++){
		QString s = d_table->text(i,col);
		if (!s.isEmpty()){
		    QDateTime d = QDateTime::fromString (s, format);
			if (colTypes[col] != Date && d.isValid()){
			//This might be the first time the user assigns a date format.
            //If Qt understands the format we break the loop, assign it to the column and return true!
				first_time = true;
                break;
			}

		    if (d_saved_cells){
                d = QDateTime(QDate::fromJulianDay(d_saved_cells[col][i]+1));
                double secs = (d_saved_cells[col][i] - int(d_saved_cells[col][i]))*86400;
                d.setTime(d.time().addSecs(int(secs)+1));

				if (d.isValid())
					d_table->setText(i, col, d.toString(format));
		    }
		}
	}
    }
	colTypes[col] = Date;
	col_format[col] = format;
	QTime ref = QTime(0, 0);
	if (first_time){//update d_saved_cells in case the user changes the time format before pressing OK in the column dialog
		for (int i=0; i<d_table->numRows(); i++){
			QDateTime dt = QDateTime::fromString(d_table->text(i, col), format);
			d_saved_cells[col][i] = dt.date().toJulianDay() - 1 + (double)ref.msecsTo(dt.time())/864.0e5;
		}
	}
	return true;
}

bool Table::setTimeFormat(const QString& format, int col, bool updateCells)
{
	if (colTypes[col] == Time && col_format[col] == format)
		return true;

	QTime ref = QTime(0, 0);
	bool first_time = false;
    if (updateCells){
	for (int i=0; i<d_table->numRows(); i++){
		QString s = d_table->text(i,col);
		if (!s.isEmpty()){
			QTime t = QTime::fromString (s, format);
			if (colTypes[col] != Time && t.isValid()){
			//This is the first time the user assigns a time format.
            //If Qt understands the format we break the loop, assign it to the column and return true!
            	first_time = true;
				break;
			}

		    if (d_saved_cells){
				if (d_saved_cells[col][i] < 1)// import of Origin files
                	t = ref.addMSecs(int(d_saved_cells[col][i]*86400000));
				else
					t = ref.addMSecs(d_saved_cells[col][i]);

				if (t.isValid())
					d_table->setText(i, col, t.toString(format));
			}
		}
	}
    }
	colTypes[col] = Time;
	col_format[col] = format;
	if (first_time){//update d_saved_cells in case the user changes the time format before pressing OK in the column dialog
		for (int i=0; i<d_table->numRows(); i++){
			QTime t = QTime::fromString(d_table->text(i, col), format);
			d_saved_cells[col][i] = ref.msecsTo(t);
		}
	}
	return true;
}

void Table::setMonthFormat(const QString& format, int col, bool updateCells)
{
    if (colTypes[col] == Month && col_format[col] == format)
        return;

	colTypes[col] = Month;
	col_format[col] = format;

	if (!updateCells)
        return;

    for (int i=0; i<numRows(); i++){
        QString t = d_table->text(i,col);
        if (!t.isEmpty()){
            int day;
            if (d_saved_cells)
                day = int(d_saved_cells[col][i]) % 12;
            else
                day = t.toInt() % 12;
            if (!day)
                day = 12;

            if (format == "M")
                d_table->setText(i, col, QDate::shortMonthName(day).left(1));
            else if (format == "MMM")
                d_table->setText(i, col, QDate::shortMonthName(day));
            else if (format == "MMMM")
                d_table->setText(i, col, QDate::longMonthName(day));
        }
    }
}

void Table::setDayFormat(const QString& format, int col, bool updateCells)
{
    if (colTypes[col] == Day && col_format[col] == format)
        return;

	colTypes[col] = Day;
	col_format[col] = format;

	if (!updateCells)
        return;

	for (int i=0; i<numRows(); i++){
        QString t = d_table->text(i,col);
        if (!t.isEmpty()){
            int day;
            if (d_saved_cells)
                day = int(d_saved_cells[col][i]) % 7;
            else
                day = t.toInt() % 7;
            if (!day)
                day = 7;

            if (format == "d")
                d_table->setText(i, col, QDate::shortDayName(day).left(1));
            else if (format == "ddd")
                d_table->setText(i, col, QDate::shortDayName(day));
            else if (format == "dddd")
                d_table->setText(i, col, QDate::longDayName(day));
        }
    }
}

void Table::setRandomValues()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows=d_table->numRows();
	QStringList list=selectedColumns();

    time_t tmp;
    srand(time(&tmp));

	for (int j=0;j<(int) list.count(); j++)
	{
		QString name=list[j];
		selectedCol=colIndex(name);

		int prec;
		char f;
		columnNumericFormat(selectedCol, &f, &prec);

		for (int i=0; i<rows; i++)
			d_table->setText(i, selectedCol, QLocale().toString(double(rand())/double(RAND_MAX), f, prec));

		emit modifiedData(this, name);
	}

	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::loadHeader(QStringList header)
{
	col_label = QStringList();
	col_plot_type = QList <int>();
	for (int i=0; i<header.count();i++)
	{
		if (header[i].isEmpty())
			continue;

		QString s = header[i].replace("_","-");
		if (s.contains("[X]"))
		{
			col_label << s.remove("[X]");
			col_plot_type << X;
		}
		else if (s.contains("[Y]"))
		{
			col_label << s.remove("[Y]");
			col_plot_type << Y;
		}
		else if (s.contains("[Z]"))
		{
			col_label << s.remove("[Z]");
			col_plot_type << Z;
		}
		else if (s.contains("[xEr]"))
		{
			col_label << s.remove("[xEr]");
			col_plot_type << xErr;
		}
		else if (s.contains("[yEr]"))
		{
			col_label << s.remove("[yEr]");
			col_plot_type << yErr;
		}
		else
		{
			col_label << s;
			col_plot_type << None;
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
	for (int j=0;j<(int)d_table->numCols();j++)
	{
		if (col_plot_type[j] == X)
			xcols++;
	}

	if (xcols>1)
	{
		xcols = 0;
		for (int i=0; i<(int)d_table->numCols(); i++)
		{
			if (col_plot_type[i] == X)
				setColumnHeader(i, col_label[i]+"[X" + QString::number(++xcols) +"]");
			else if (col_plot_type[i] == Y)
			{
				if(xcols>0)
					setColumnHeader(i, col_label[i]+"[Y"+ QString::number(xcols) +"]");
				else
					setColumnHeader(i, col_label[i]+"[Y]");
			}
			else if (col_plot_type[i] == Z)
			{
				if(xcols>0)
					setColumnHeader(i, col_label[i]+"[Z"+ QString::number(xcols) +"]");
				else
					setColumnHeader(i, col_label[i]+"[Z]");
			}
			else if (col_plot_type[i] == xErr)
				setColumnHeader(i, col_label[i]+"[xEr]");
			else if (col_plot_type[i] == yErr)
				setColumnHeader(i, col_label[i]+"[yEr]");
			else
				setColumnHeader(i, col_label[i]);
		}
	}
	else
	{
		for (int i=0; i<(int)d_table->numCols(); i++)
		{
			if (col_plot_type[i] == X)
				setColumnHeader(i, col_label[i]+"[X]");
			else if (col_plot_type[i] == Y)
				setColumnHeader(i, col_label[i]+"[Y]");
			else if (col_plot_type[i] == Z)
				setColumnHeader(i, col_label[i]+"[Z]");
			else if (col_plot_type[i] == xErr)
				setColumnHeader(i, col_label[i]+"[xEr]");
			else if (col_plot_type[i] == yErr)
				setColumnHeader(i, col_label[i]+"[yEr]");
			else
				setColumnHeader(i, col_label[i]);
		}
	}
}

void Table::setAscValues()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows=d_table->numRows();
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

		for (int i=0; i<rows; i++)
			setText(i,selectedCol,QString::number(i+1, f, prec));

		emit modifiedData(this, name);
	}

	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::plotL()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::Line, sel.topRow(), sel.bottomRow());
}

void Table::plotP()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::Scatter, sel.topRow(), sel.bottomRow());
}

void Table::plotLP()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::LineSymbols, sel.topRow(), sel.bottomRow());
}

void Table::plotVB()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::VerticalBars, sel.topRow(), sel.bottomRow());
}

void Table::plotHB()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::HorizontalBars, sel.topRow(), sel.bottomRow());
}

void Table::plotArea()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::Area, sel.topRow(), sel.bottomRow());
}

bool Table::noXColumn()
{
	bool notSet = true;
	for (int i=0; i<d_table->numCols(); i++)
	{
		if (col_plot_type[i] == X)
			return false;
	}
	return notSet;
}

bool Table::noYColumn()
{
	bool notSet = true;
	for (int i=0; i<d_table->numCols(); i++)
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
	if (int(s.count())>0)
	{
		Q3TableSelection sel = d_table->selection(d_table->currentSelection());
		emit plotCol(this, s, Graph::Pie, sel.topRow(), sel.bottomRow());
	}
	else
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("Please select a column to plot!"));
}

void Table::plotVerticalDropLines()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::VerticalDropLines, sel.topRow(), sel.bottomRow());
}

void Table::plotSpline()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::Spline, sel.topRow(), sel.bottomRow());
}

void Table::plotVertSteps()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::VerticalSteps, sel.topRow(), sel.bottomRow());
}

void Table::plotHorSteps()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::HorizontalSteps, sel.topRow(), sel.bottomRow());
}

void Table::plotHistogram()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::Histogram, sel.topRow(), sel.bottomRow());
}

void Table::plotBoxDiagram()
{
	if (!valid2DPlot())
		return;

	Q3TableSelection sel = d_table->selection(d_table->currentSelection());
	emit plotCol(this, drawableColumnSelection(), Graph::Box, sel.topRow(), sel.bottomRow());
}

void Table::plotVectXYXY()
{
	if (!valid2DPlot())
		return;

	QStringList s = selectedColumns();
	if ((int)s.count() == 4)
	{
		Q3TableSelection sel = d_table->selection(d_table->currentSelection());
		emit plotCol(this, s, Graph::VectXYXY, sel.topRow(), sel.bottomRow());
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
		Q3TableSelection sel = d_table->selection(d_table->currentSelection());
		emit plotCol(this, s, Graph::VectXYAM, sel.topRow(), sel.bottomRow());
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
  	else if (d_table->numCols()<2)
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
	if (d_table->numCols()<2)
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
	Q3TextStream t( &f );// use a text stream
	if ( f.open(QIODevice::ReadOnly) ){
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

		int i, rows = 1, cols = 0;
		int r = d_table->numRows();
		int c = d_table->numCols();
		for (i=0; i<ignoredLines; i++)
			t.readLine();

		QString s = t.readLine();//read first line after the ignored ones
		while (!t.atEnd()){
			t.readLine();
			rows++;
			qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
		}

		if (simplifySpaces)
			s = s.simplifyWhiteSpace();
		else if (stripSpaces)
			s = s.stripWhiteSpace();
		QStringList line = s.split(sep);
		cols = (int)line.count();

		bool allNumbers = true;
		for (i=0; i<cols; i++)
		{//verify if the strings in the line used to rename the columns are not all numbers
			QLocale().toDouble(line[i], &allNumbers);
			if (!allNumbers)
				break;
		}

		if (renameCols && !allNumbers)
			rows--;

		QProgressDialog progress(this);
		int steps = int(rows/1000);
        progress.setRange(0, steps+1);
		progress.setWindowTitle("Qtiplot - Reading file...");
		progress.setLabelText(fname);
		progress.setActiveWindow();

		QApplication::restoreOverrideCursor();

		if (!importFileAs)
			init (rows, cols);
		else if (importFileAs == 1){//new cols
			addColumns(cols);
			if (r < rows)
				d_table->setNumRows(rows);
		}
		else if (importFileAs == 2){//new rows
			if (c < cols)
				addColumns(cols-c);
			d_table->setNumRows(r+rows);
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
				s = s.simplifyWhiteSpace();
			else if (stripSpaces)
				s = s.stripWhiteSpace();

			line = s.split(sep, QString::SkipEmptyParts);
			int end = startCol+(int)line.count();
			for (i=startCol; i<end; i++)
  	        	col_label[i] = QString::null;
			for (i=startCol; i<end; i++){
				comments[i] = line[i-startCol];
				s = line[i-startCol].replace("-","_").remove(QRegExp("\\W")).replace("_","-");
				int n = col_label.count(s);
				if(n){
					//avoid identical col names
					while (col_label.contains(s+QString::number(n)))
						n++;
					s += QString::number(n);
				}
				col_label[i] = s;
			}
		}
		d_table->blockSignals(true);
		setHeaderColType();

		for (i=0; i<steps; i++){
			if (progress.wasCanceled()){
				f.close();
				return;
			}

			for (int k=0; k<1000; k++){
				s = t.readLine();
				if (simplifySpaces)
					s = s.simplifyWhiteSpace();
				else if (stripSpaces)
					s = s.stripWhiteSpace();
				line = s.split(sep);
				for (int j=startCol; j<d_table->numCols(); j++)
					d_table->setText(startRow + k, j, line[j-startCol]);
			}

			startRow += 1000;
			progress.setValue(i);
		}

		for (i=startRow; i<d_table->numRows(); i++){
			s = t.readLine();
			if (simplifySpaces)
				s = s.simplifyWhiteSpace();
			else if (stripSpaces)
				s = s.stripWhiteSpace();
			line = s.split(sep);
			for (int j=startCol; j<d_table->numCols(); j++)
				d_table->setText(i, j, line[j-startCol]);
		}
		progress.setValue(steps+1);
		d_table->blockSignals(false);
		f.close();

		if (importFileAs)
		{
			for (i=0; i<d_table->numCols(); i++)
				emit modifiedData(this, colName(i));
		}
	}
}

void Table::importASCII(const QString &fname, const QString &sep, int ignoredLines,
		bool renameCols, bool stripSpaces, bool simplifySpaces, bool newTable)
{
	QFile f(fname);
	if (f.open(QIODevice::ReadOnly)) //| QIODevice::Text | QIODevice::Unbuffered ))
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        Q3TextStream t(&f);//TODO: use QTextStream instead and find a way to make it read the end-of-line char correctly.
                         //Opening the file with the above combination doesn't seem to help: problems on Mac OS X generated ASCII files!

		int i, c, rows = 1, cols = 0;
		for (i=0; i<ignoredLines; i++)
			t.readLine();

		QString s = t.readLine();//read first line after the ignored ones
		while ( !t.atEnd() ){
			t.readLine();
			rows++;
			qApp->processEvents(QEventLoop::ExcludeUserInput);
		}

		if (simplifySpaces)
			s = s.simplifyWhiteSpace();
		else if (stripSpaces)
			s = s.stripWhiteSpace();

		QStringList line = s.split(sep);
		cols = (int)line.count();

		bool allNumbers = true;
		for (i=0; i<cols; i++)
		{//verify if the strings in the line used to rename the columns are not all numbers
			QLocale().toDouble(line[i], &allNumbers);
			if (!allNumbers)
				break;
		}

		if (renameCols && !allNumbers)
			rows--;
		int steps = int(rows/1000);

		QProgressDialog progress(this);
		progress.setWindowTitle("Qtiplot - Reading file...");
		progress.setLabelText(fname);
		progress.setActiveWindow();
		progress.setAutoClose(true);
		progress.setAutoReset(true);
		progress.setRange(0, steps+1);

		QApplication::restoreOverrideCursor();

		QStringList oldHeader;
		if (newTable)
			init (rows, cols);
		else{
			if (d_table->numRows() != rows)
				d_table->setNumRows(rows);

			c = d_table->numCols();
			oldHeader = col_label;
			if (c != cols){
				if (c < cols)
					addColumns(cols - c);
				else{
					d_table->setNumCols(cols);
                    for (int i=c-1; i>=cols; i--){
                        emit removedCol(QString(name()) + "_" + oldHeader[i]);
                        commands.removeLast();
                        comments.removeLast();
                        col_format.removeLast();
                        col_label.removeLast();
                        colTypes.removeLast();
                        col_plot_type.removeLast();
                    }
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
				s = s.simplifyWhiteSpace();
			else if (stripSpaces)
				s = s.stripWhiteSpace();
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

        d_table->blockSignals(true);
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
					s = s.simplifyWhiteSpace();
				else if (stripSpaces)
					s = s.stripWhiteSpace();
				line = s.split(sep);
				int lc = (int)line.count();
				if (lc > cols) {
					addColumns(lc - cols);
					cols = lc;
				}
				for (int j=0; j<cols && j<lc; j++)
					d_table->setText(start + k, j, line[j]);
			}
			progress.setValue(i);
			qApp->processEvents();
		}

		start = steps*1000;
		for (i=start; i<rows; i++)
		{
			s = t.readLine();
			if (simplifySpaces)
				s = s.simplifyWhiteSpace();
			else if (stripSpaces)
				s = s.stripWhiteSpace();
			line = s.split(sep);
			int lc = (int)line.count();
			if (lc > cols) {
				addColumns(lc - cols);
				cols = lc;
			}
			for (int j=0; j<cols && j<lc; j++)
				d_table->setText(i, j, line[j]);
		}
		progress.setValue(steps+1);
		qApp->processEvents();
		d_table->blockSignals(false);
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

bool Table::exportASCII(const QString& fname, const QString& separator,
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

	QString text;
	int i,j;
	int rows=d_table->numRows();
	int cols=d_table->numCols();
	int selectedCols = 0;
	int topRow = 0, bottomRow = 0;
	int *sCols;
	if (exportSelection)
	{
		for (i=0; i<cols; i++)
		{
			if (d_table->isColumnSelected(i))
				selectedCols++;
		}

		sCols = new int[selectedCols];
		int aux = 1;
		for (i=0; i<cols; i++)
		{
			if (d_table->isColumnSelected(i))
			{
				sCols[aux] = i;
				aux++;
			}
		}

		for (i=0; i<rows; i++)
		{
			if (d_table->isRowSelected(i))
			{
				topRow = i;
				break;
			}
		}

		for (i=rows - 1; i>0; i--)
		{
			if (d_table->isRowSelected(i))
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
					text+=header[sCols[i]]+separator;
				else
					text+="C"+header[sCols[i]]+separator;
			}

			if (ls.count()>0)
				text+=header[sCols[selectedCols]] + "\n";
			else
				text+="C"+header[sCols[selectedCols]] + "\n";
		}
		else
		{
			if (ls.count()>0)
			{
				for (j=0; j<cols-1; j++)
					text+=header[j]+separator;
				text+=header[cols-1]+"\n";
			}
			else
			{
				for (j=0; j<cols-1; j++)
					text+="C"+header[j]+separator;
				text+="C"+header[cols-1]+"\n";
			}
		}
	}// finished writting labels

	if (exportSelection)
	{
		for (i=topRow;i<=bottomRow; i++)
		{
			for (j=1;j<selectedCols;j++)
				text+=d_table->text(i, sCols[j]) + separator;
			text+=d_table->text(i, sCols[selectedCols]) + "\n";
		}
		delete[] sCols;//free memory
	}
	else
	{
		for (i=0;i<rows;i++)
		{
			for (j=0;j<cols-1;j++)
				text+=d_table->text(i,j)+separator;
			text+=d_table->text(i,cols-1)+"\n";
		}
	}
	QTextStream t( &f );
	t << text;
	f.close();
	return true;
}

void Table::contextMenuEvent(QContextMenuEvent *e)
{
	QRect r = d_table->horizontalHeader()->sectionRect(d_table->numCols()-1);
	setFocus();
	if (e->pos().x() > r.right() + d_table->verticalHeader()->width())
		emit showContextMenu(false);
	else
		emit showContextMenu(true);
	e->accept();
}

void Table::moveCurrentCell()
{
	int cols=d_table->numCols();
	int row=d_table->currentRow();
	int col=d_table->currentColumn();
	d_table->clearSelection (true);

	if (col+1<cols)
	{
		d_table->setCurrentCell(row, col+1);
		d_table->selectCells(row, col+1, row, col+1);
	}
	else
	{
        if(row+1 >= numRows())
            d_table->setNumRows(row + 11);

		d_table->setCurrentCell (row+1, 0);
		d_table->selectCells(row+1, 0, row+1, 0);
	}
}

bool Table::eventFilter(QObject *object, QEvent *e)
{
	Q3Header *hheader = d_table->horizontalHeader();
	Q3Header *vheader = d_table->verticalHeader();

	if (e->type() == QEvent::MouseButtonDblClick && object == (QObject*)hheader) {
		const QMouseEvent *me = (const QMouseEvent *)e;
		selectedCol = hheader->sectionAt (me->pos().x() + hheader->offset());

		QRect rect = hheader->sectionRect (selectedCol);
		rect.setLeft(rect.right() - 2);
		rect.setWidth(4);

		if (rect.contains (me->pos())) {
			d_table->adjustColumn(selectedCol);
			emit modifiedWindow(this);
		} else
			emit optionsDialog();
        setActiveWindow();
		return true;
	} else if (e->type() == QEvent::MouseButtonPress && object == (QObject*)hheader) {
		const QMouseEvent *me = (const QMouseEvent *)e;
		if (me->button() == Qt::LeftButton && me->state() == Qt::ControlButton) {
			selectedCol = hheader->sectionAt (me->pos().x() + hheader->offset());
			d_table->selectColumn (selectedCol);
			d_table->setCurrentCell (0, selectedCol);
			setActiveWindow();
			return true;
		} else if (selectedColsNumber() <= 1) {
			selectedCol = hheader->sectionAt (me->pos().x() + hheader->offset());
			d_table->clearSelection();
			d_table->selectColumn (selectedCol);
			d_table->setCurrentCell (0, selectedCol);
			setActiveWindow();
			return false;
		}
	} else if (e->type() == QEvent::MouseButtonPress && object == (QObject*)vheader) {
		const QMouseEvent *me = (const QMouseEvent *)e;
		if (me->button() == Qt::RightButton && numSelectedRows() <= 1) {
			d_table->clearSelection();
			int row = vheader->sectionAt(me->pos().y() + vheader->offset());
			d_table->selectRow (row);
			d_table->setCurrentCell (row, 0);
			setActiveWindow();
		}
	} else if (e->type()==QEvent::ContextMenu && object == titleBar) {
		emit showTitleBarMenu();
		((QContextMenuEvent*)e)->accept();
		setActiveWindow();
		return true;
	}

	return MyWidget::eventFilter(object, e);
}

void Table::customEvent(QEvent *e)
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

	newSpecifications = saveToString("geometry\n");
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
	int cols=d_table->numCols();
	int rows=d_table->numRows();

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
		d_table->setNumRows(r);

	int c=list[2].toInt();
	if (cols != c)
		d_table->setNumCols(c);

	//clear all cells
	for (i=0; i<r; i++)
	{
		for (j=0; j<c; j++)
			d_table->setText(i,j, "");
	}

	t.readLine();	//table geometry useless info when restoring
	s = t.readLine();//header line

	list = s.split("\t");
	list.remove(list.first());

	if (col_label != list)
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

	s = t.readLine();	//colWidth line
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

	s = t.readLine();	//colType line ?
	list = s.split("\t");
	colTypes.clear();
	col_format.clear();
	if (s.contains ("ColType"))
	{
		list.remove(list.first());
		for (i=0; i<list.count(); i++)
		{
			colTypes << Numeric;
			col_format << "0/14";

			QStringList l = list[i].split(";");
			if (l.count() >= 1)
				colTypes[i] = l[0].toInt();
			if (l.count() >= 2)
				col_format[i] = l[1];
		}
	}
	else //if fileVersion < 65 set table values
	{
		row = list[0].toInt();
		for (j=0; j<cols; j++)
			d_table->setText(row, j, list[j+1]);
	}

	s = t.readLine();	//comments line ?
	list = s.split("\t");
	if (s.contains ("Comments"))
	{
		list.remove(list.first());
		comments = list;
	}

	s = t.readLine();
	list = s.split("\t");

	if (s.contains ("WindowLabel"))
	{
		setWindowLabel(list[1]);
		setCaptionPolicy((MyWidget::CaptionPolicy)list[2].toInt());
	}

	s = t.readLine();
	if (s == "<data>")
		s = t.readLine();

	while (!t.atEnd () && s != "</data>")
	{
		list = s.split("\t");
		row = list[0].toInt();
		for (j=0; j<c; j++)
			d_table->setText(row, j, list[j+1]);

		s = t.readLine();
	}

	for (j=0; j<c; j++)
		emit modifiedData(this, colName(j));
}

void Table::setNumRows(int rows)
{
	d_table->setNumRows(rows);
}

void Table::setNumCols(int cols)
{
	d_table->setNumCols(cols);
}

void Table::resizeRows(int r)
{
	int rows = d_table->numRows();
	if (rows == r)
		return;

	if (rows > r)
	{
		QString text= tr("Rows will be deleted from the table!");
		text+="<p>"+tr("Do you really want to continue?");
		int i,cols = d_table->numCols();
		switch( QMessageBox::information(this,tr("QtiPlot"), text, tr("Yes"), tr("Cancel"), 0, 1 ) )
		{
			case 0:
				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				d_table->setNumRows(r);
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
		d_table->setNumRows(r);
		QApplication::restoreOverrideCursor();
	}

	emit modifiedWindow(this);
}

void Table::resizeCols(int c)
{
	int cols = d_table->numCols();
	if (cols == c)
		return;

	if (cols > c){
		QString text= tr("Columns will be deleted from the table!");
		text+="<p>"+tr("Do you really want to continue?");
		switch( QMessageBox::information(this,tr("QtiPlot"), text, tr("Yes"), tr("Cancel"), 0, 1 ) ){
			case 0: {
				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                Q3MemArray<int> columns(cols-c);
				for (int i=cols-1; i>=c; i--){
					QString name = colName(i);
					emit removedCol(name);
					columns[i-c]=i;

					commands.removeLast();
					comments.removeLast();
					col_format.removeLast();
					col_label.removeLast();
					colTypes.removeLast();
					col_plot_type.removeLast();
				}

				d_table->removeColumns(columns);
				QApplication::restoreOverrideCursor();
				break;
			}

			case 1:
				return;
				break;
		}
	}
	else{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		addColumns(c-cols);
		setHeaderColType();
		QApplication::restoreOverrideCursor();
	}
	emit modifiedWindow(this);
}

void Table::copy(Table *m)
{
	for (int i=0; i<d_table->numRows(); i++)
	{
		for (int j=0; j<d_table->numCols(); j++)
			d_table->setText(i,j,m->text(i,j));
	}

	setColWidths(m->columnWidths());
	col_label = m->colNames();
	col_plot_type = m->plotDesignations();
	d_show_comments = m->commentsEnabled();
    comments = m->colComments();
	setHeaderColType();

	commands = m->getCommands();
	setColumnTypes(m->columnTypes());
	col_format = m->getColumnsFormat();
}

QString Table::saveAsTemplate(const QString& geometryInfo)
{
	QString s="<table>\t"+QString::number(d_table->numRows())+"\t";
	s+=QString::number(d_table->numCols())+"\n";
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
	for (int i=0; i<d_table->numCols(); i++)
		emit modifiedData(this, colName(i));

	emit modifiedWindow(this);
}

void Table::clear()
{
	for (int i=0; i<d_table->numCols(); i++)
	{
		for (int j=0; j<d_table->numRows(); j++)
			d_table->setText(j, i, QString::null);

		emit modifiedData(this, colName(i));
	}

	emit modifiedWindow(this);
}

void Table::goToRow(int row)
{
	if( (row < 1) || (row > numRows()) ) return;

	d_table->ensureCellVisible ( row-1, 0 );
	d_table->selectRow(row-1);
}

void Table::setColumnHeader(int index, const QString& label)
{
	Q3Header *head = d_table->horizontalHeader();

	if (d_show_comments)
	{
		QString s = label;

		int lines = d_table->columnWidth(index)/d_table->horizontalHeader()->fontMetrics().averageCharWidth();
		head->setLabel(index, s.remove("\n") + "\n" + QString(lines, '_') + "\n" + comments[index]);
	}
	else
		head->setLabel(index, label);
}

void Table::showComments(bool on)
{
	if (d_show_comments == on)
		return;

	d_show_comments = on;
	setHeaderColType();

	if(!on)
		d_table->setTopMargin (d_table->horizontalHeader()->height()/2);
}

void Table::setNumericPrecision(int prec)
{
	d_numeric_precision = prec;
	for (int i=0; i<d_table->numCols(); i++)
        col_format[i] = "0/"+QString::number(prec);
}

void Table::updateDecimalSeparators()
{
    saveToMemory();

	for (int i=0; i<d_table->numCols(); i++)
	{
	    if (colTypes[i] != Numeric)
            continue;

        char format;
        int prec;
        columnNumericFormat(i, &format, &prec);

        for (int j=0; j<d_table->numRows(); j++){
            if (!d_table->text(j, i).isEmpty())
                d_table->setText(j, i, QLocale().toString(d_saved_cells[i][j], format, prec));
		}
	}

    freeMemory();
}

/*****************************************************************************
 *
 * Class MyTable
 *
 *****************************************************************************/

MyTable::MyTable(QWidget * parent, const char * name)
:Q3Table(parent, name)
{}

MyTable::MyTable(int numRows, int numCols, QWidget * parent, const char * name)
:Q3Table(numRows, numCols, parent, name)
{}

void MyTable::activateNextCell()
{
	int row = currentRow();
	int col = currentColumn();

	clearSelection (true);

    if(row+1 >= numRows())
        setNumRows(row + 11);

	setCurrentCell (row + 1, col);
    selectCells(row+1, col, row+1, col);
}
