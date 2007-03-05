/***************************************************************************
    File                 : worksheet.cpp
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
#include "worksheet.h"
#include "sortDialog.h"
#include "nrutil.h"

#include <q3popupmenu.h>
#include <q3filedialog.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qclipboard.h> 
#include <qapplication.h> 
#include <qcursor.h>
#include <qdatetime.h>
#include <qprinter.h>
#include <qpainter.h>
#include <q3paintdevicemetrics.h>
#include <q3dragobject.h>
#include <qlayout.h>
#include <q3accel.h>
#include <q3progressdialog.h>
//Added by qt3to4:
#include <QEvent>
#include <QContextMenuEvent>
#include <Q3VBoxLayout>
#include <QMouseEvent>
#include <Q3TableSelection>
#include <Q3MemArray>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_fft_halfcomplex.h>
//#include <gsl/gsl_fft_real.h>
//#include <gsl/gsl_fft_complex.h>

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

	QDateTime dt = QDateTime::currentDateTime ();
	setBirthDate(dt.toString(Qt::LocalDate));

	worksheet= new Q3Table (rows,cols,this,"table");
	worksheet->setFocusPolicy(Qt::StrongFocus);
	worksheet->setFocus();
	worksheet->setSelectionMode (Q3Table::Single);
	worksheet->setRowMovingEnabled(true);
	// TODO: would be useful, but then we have to interpret
	// signal Q3Header::indexChange ( int section, int fromIndex, int toIndex )
	// and update some variables
	//worksheet->setColumnMovingEnabled(true);

	connect(worksheet->verticalHeader(), SIGNAL(indexChange ( int, int, int )),
			this, SLOT(notifyChanges()));

	Q3VBoxLayout* hlayout = new Q3VBoxLayout(this);
	hlayout->addWidget(worksheet);

	for (int i=0; i<cols; i++)
	{
		commands << "";
		colTypes << Numeric;
		col_format << "0/6";
		comments << "";
		col_label << QString::number(i+1);
		col_plot_type << Y;
	}

	Q3Header* head=(Q3Header*)worksheet->horizontalHeader();
	head->setMouseTracking(true);
	head->setResizeEnabled(true);
	head->installEventFilter(this);
	connect(head, SIGNAL(sizeChange(int, int, int)), this, SLOT(colWidthModified(int, int, int)));

	col_plot_type[0] = X;
	setHeaderColType();

	int w=4*(worksheet->horizontalHeader())->sectionSize(0);
	int h;
	if (rows>11)
		h=11*(worksheet->verticalHeader())->sectionSize(0);
	else
		h=(rows+1)*(worksheet->verticalHeader())->sectionSize(0);
	setGeometry(50,50,w + 45, h);

	worksheet->verticalHeader()->setResizeEnabled(false);
	worksheet->verticalHeader()->installEventFilter(this);

	Q3Accel *accel = new Q3Accel(this);
	accel->connectItem( accel->insertItem( Qt::Key_Tab ), this, SLOT(moveCurrentCell()));
	accel->connectItem( accel->insertItem( Qt::CTRL+Qt::Key_A ), this, SLOT(selectAllTable()) );

	connect(worksheet, SIGNAL(valueChanged(int,int)),this, SLOT(cellEdited(int,int)));
}

void Table::colWidthModified(int, int, int)
{
	emit modifiedWindow(this);
}


void Table::setBackgroundColor(const QColor& col)
{
	worksheet->setPaletteBackgroundColor ( col );
}

void Table::setTextColor(const QColor& col)
{
	worksheet->setPaletteForegroundColor (col);
}

void Table::setTextFont(const QFont& fnt)
{
	worksheet->setFont (fnt);
	QFontMetrics fm(fnt);
	int lm = fm.width( QString::number(10*worksheet->numRows()));
	worksheet->setLeftMargin( lm );
}

void Table::setHeaderColor(const QColor& col)
{
	worksheet->horizontalHeader()->setPaletteForegroundColor (col);
}

void Table::setHeaderFont(const QFont& fnt)
{
	worksheet->horizontalHeader()->setFont (fnt);
}

void Table::print()
{
	QPrinter printer;
	printer.setColorMode (QPrinter::GrayScale);
	if (printer.setup()) 
	{
		printer.setFullPage( true );
		QPainter p;
		if ( !p.begin(&printer ) )
			return; // paint on printer
		Q3PaintDeviceMetrics metrics( p.device() );
		int dpiy = metrics.logicalDpiY();
		const int margin = (int) ( (1/2.54)*dpiy ); // 2 cm margins

		Q3Header *hHeader = worksheet->horizontalHeader();
		Q3Header *vHeader = worksheet->verticalHeader();

		int rows=worksheet->numRows();
		int cols=worksheet->numCols();
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
			int w=worksheet->columnWidth (i);
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
		height+=tr.height();	
		p.drawLine(margin,height,right-1,height);		

		p.setFont(worksheet->font());
		// print table values
		for (i=0;i<rows;i++)
		{
			right=margin;
			QString text=vHeader->label(i)+"\t";
			tr=p.boundingRect(tr,Qt::AlignCenter,text);
			p.drawLine(right,height,right,height+tr.height());

			br.setTopLeft(QPoint(right,height));	
			br.setWidth(vertHeaderWidth);	
			br.setHeight(tr.height());
			p.drawText(br,Qt::AlignCenter,text,-1);
			right+=vertHeaderWidth;
			p.drawLine(right,height,right,height+tr.height());

			for (int j=0;j<cols;j++)
			{
				int w=worksheet->columnWidth (j);
				text=worksheet->text(i,j)+"\t";
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
}

void Table::cellEdited(int row, int col)
{
	if (columnType(col) != Numeric)
	{
		emit modifiedData(this, colName(col));
		emit modifiedWindow(this);
		return;		
	}
	
	char f;
	int precision;
  	columnNumericFormat(col, f, precision);
  	 
  	QString text = worksheet->text(row,col).replace(",", ".");
  	bool ok = false;
  	double res = text.toDouble(&ok);
  	if (!text.isEmpty() && ok)
  		worksheet->setText(row, col, QString::number(res, f, precision));
  	else
  	{
  	Script *script = scriptEnv->newScript(worksheet->text(row,col),this,QString("<%1_%2_%3>").arg(name()).arg(row+1).arg(col+1));
  	connect(script, SIGNAL(error(const QString&,const QString&,int)), scriptEnv, SIGNAL(error(const QString&,const QString&,int)));
  	 
  	script->setInt(row+1, "i");
  	script->setInt(col+1, "j");
  	QVariant ret = script->eval();
  	if(ret.type()==QVariant::Int || ret.type()==QVariant::UInt || ret.type()==QVariant::LongLong || ret.type()==QVariant::ULongLong)
  		worksheet->setText(row, col, ret.toString());
  	else if(ret.canCast(QVariant::Double))
  		worksheet->setText(row, col, QString::number(ret.toDouble(), f, precision));
  	else
  		worksheet->setText(row, col, "");
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
	for(i=col+1; i<(int)worksheet->numCols(); i++)
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
	for(i=col+1; i<(int)worksheet->numCols(); i++)
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

void Table::columnNumericFormat(int col, int &f, int &precision)
{
	QStringList format = col_format[col].split("/", QString::SkipEmptyParts);
	f = format[0].toInt();
	precision = format[1].toInt();
}

void Table::columnNumericFormat(int col, char &f, int &precision)
{
	QStringList format = col_format[col].split("/", QString::SkipEmptyParts);
	switch(format[0].toInt())
	{
		case 0:
			f = 'g';
			break;

		case 1:
			f = 'f';
			break;

		case 2:
			f = 'e';
			break;
	}
	precision = format[1].toInt();
}

int Table::columnWidth(int col)
{
	return worksheet->columnWidth(col);
}

QStringList Table::columnWidths()
{
	QStringList widths;
	for (int i=0;i<worksheet->numCols();i++)
		widths<<QString::number(worksheet->columnWidth(i));

	return widths;
}

void Table::setColWidths(const QStringList& widths)
{
	for (int i=0;i<(int)widths.count();i++)
		worksheet->setColumnWidth (i, widths[i].toInt() );
}

void Table::setColumnTypes(const QStringList& ctl)
{
	int n = qMin((int)ctl.count(), tableCols());
	for (int i=0; i<n; i++)
	{
		QStringList l= ctl[i].split(";");
		colTypes[i] = l[0].toInt();

		if ((int)l.count() > 0 && !l[1].isEmpty())
			col_format[i] = l[1];
		else 
			col_format[i] = "0/6";
	}
}

QString Table::saveColumnWidths()
{
	QString s="ColWidth\t";
	for (int i=0;i<worksheet->numCols();i++)
		s+=QString::number(worksheet->columnWidth (i))+"\t";

	return s+"\n";
}

QString Table::saveColumnTypes()
{
	QString s="ColType";
	for (int i=0; i<worksheet->numCols(); i++)
		s += "\t"+QString::number(colTypes[i])+";"+col_format[i];

	return s+"\n";
}

void Table::setCommands(const QStringList& com)
{
	commands.clear();
	for(int i=0; i<(int)com.size() && i<tableCols(); i++)
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
	if (endRow >= tableRows())
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
		if(ret.type()==QVariant::Double) {
			int prec;
			char f;
			columnNumericFormat(col, f, prec);
			worksheet->setText(i,col,QString::number(ret.toDouble(), f, prec));
		} else if(ret.canConvert(QVariant::String))
			worksheet->setText(i, col, ret.toString());
		else {
			QApplication::restoreOverrideCursor();
			return false;
		}
	}
	forgetSavedCol();

	emit modifiedData(this, colName(col));
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
	return true;
}

Q3TableSelection Table::getSelection()
{
	Q3TableSelection sel;
	if (worksheet->numSelections()==0)
	{
		sel.init(worksheet->currentRow(), worksheet->currentColumn());
		sel.expandTo(worksheet->currentRow(), worksheet->currentColumn());
	} else if (worksheet->currentSelection()>0)
		sel = worksheet->selection(worksheet->currentSelection());
	else
		sel = worksheet->selection(0);
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
	for (int col=0; col<tableCols(); col++)
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
	QString s="Comments\t";
	for (int i=0; i<worksheet->numCols(); i++)
		s+= comments[i] + "\t";
	return s + "\n";
}

QString Table::saveToString(const QString& geometry)
{
	QString s="<table>\n";
	s+=QString(name())+"\t";
	s+=QString::number(worksheet->numRows())+"\t";
	s+=QString::number(worksheet->numCols())+"\t";
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
	for (int j=0; j<worksheet->numCols(); j++)
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
	for (int j=0; j<worksheet->numCols(); j++)
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
	for (int i=selectedCol; i<(int)worksheet->numCols(); i++)
	{
		QString newLabel=oldLabel+QString::number(n);
		commands.gres("col(\""+colLabel(i)+"\")", "col(\""+newLabel+"\")", true);
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

void Table::changeColWidth(int width, bool allCols)
{
	int cols=worksheet->numCols();
	if (allCols)
	{
		for (int i=0;i<cols;i++)
			worksheet->setColumnWidth (i, width);

		emit modifiedWindow(this);
	}
	else
	{
		if (worksheet->columnWidth(selectedCol) == width)
			return;

		worksheet->setColumnWidth (selectedCol, width);
		emit modifiedWindow(this);
	}
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

	commands.gres("col(\""+colLabel(selectedCol)+"\")", "col(\""+text+"\")", true);	

	setColName(selectedCol, text);	
	emit changedColHeader(oldName, newName);
	emit modifiedWindow(this);
}

void Table::setColName(int col, const QString& text)
{
	if (text.isEmpty() || col<0 || col >=worksheet->numCols())
		return; 

	col_label[col] = text;
	setHeaderColType();
}

QStringList Table::selectedColumns()
{
	QStringList names;
	for (int i=0;i<worksheet->numCols();i++)
	{
		if(worksheet->isColumnSelected (i,true))
			names<<QString(name())+"_"+col_label[i];
	}
	return names;
}

QStringList Table::YColumns()
{
	QStringList names;
	for (int i=0;i<worksheet->numCols();i++)
	{
		if(col_plot_type[i] == Y)
			names<<QString(name())+"_"+col_label[i];
	}
	return names;
}

QStringList Table::selectedYColumns()
{
	QStringList names;
	for (int i=0;i<worksheet->numCols();i++)
	{
	if(worksheet->isColumnSelected (i,true) && col_plot_type[i] == Y)
  		names<<QString(name())+"_"+col_label[i];
  	}
  	return names;
}
  	 
QStringList Table::selectedErrColumns()
{
  	QStringList names;
  	for (int i=0;i<worksheet->numCols();i++)
  		{
  	    if(worksheet->isColumnSelected (i,true) &&
  	       (col_plot_type[i] == yErr || col_plot_type[i] == xErr))
  	       	names<<QString(name())+"_"+col_label[i];
  	    }
  	return names;
}
  	 
QStringList Table::drawableColumnSelection()
{
  	QStringList names;
  	for (int i=0;i<worksheet->numCols();i++)
  	{
	if(worksheet->isColumnSelected (i,true) && col_plot_type[i] == Y)
		names<<QString(name())+"_"+col_label[i];
    } 	       
  	 
  	for (int j=0; j<worksheet->numCols(); j++)
  	{
  	 	if(worksheet->isColumnSelected (j,true) && (col_plot_type[j] == yErr || col_plot_type[j] == xErr))
  	    	names<<QString(name())+"_"+col_label[j];
  	}
	return names;
}

QStringList Table::selectedYLabels()
{
	QStringList names;
	for (int i=0;i<worksheet->numCols();i++)
	{
		if(worksheet->isColumnSelected (i,true) && col_plot_type[i] == Y)
			names<<col_label[i];
	}
	return names;
}

QStringList Table::columnsList()
{
	QStringList names;
	for (int i=0;i<worksheet->numCols();i++)
		names<<QString(name())+"_"+col_label[i];

	return names;
}

int Table::firstSelectedColumn()
{
	for (int i=0;i<worksheet->numCols();i++)
	{
		if(worksheet->isColumnSelected (i,true))
			return i;
	}
	return -1;
}

int Table::selectedRows()
{
	int r=0;
	for (int i=0;i<worksheet->numRows();i++)
	{
		if(worksheet->isRowSelected (i,true))
			r++;
	}
	return r;
}

int Table::selectedColsNumber()
{
	int c=0;
	for (int i=0;i<worksheet->numCols(); i++)
	{
		if(worksheet->isColumnSelected (i,true))
			c++;
	}
	return c;
}

QString Table::colName(int col)
{//returns the table name + horizontal header text
	if (col<0 || col >=worksheet->numCols())
		return QString();

	return QString(this->name())+"_"+col_label[col];
}

QVarLengthArray<double> Table::col(int ycol)
{
	int i;
	int rows=worksheet->numRows();
	int cols=worksheet->numCols();
	QVarLengthArray<double> Y(rows);
	if (ycol<=cols)
	{
		for (i=0;i<rows;i++)
			Y[i]=worksheet->text(i,ycol).toDouble();
	}
	return Y;
}

void Table::insertCols(int start, int count)
{	
	start--;//insert new columns before the start/selected column

	int max=0,i;

	int cols=worksheet->numCols();
	for (i=0; i<cols; i++)
	{
		if (col_label[i].contains(QRegExp ("\\D"))==0)
		{
			int index = col_label[i].toInt();
			if (index>max) 
				max=index;
		}
	}
	max++;

	for(i=start+1 ; i<(count+start+1) ; i++ );
	{
		commands.insert(i, QString());
		col_format.insert(i, "0/6");
		comments.insert(i, QString());
		col_label.insert(i, QString());
		colTypes.insert(i, Numeric);
		col_plot_type.insert(i, Y);
	}

	worksheet->insertColumns(start, count);
	for (i=0; i<count; i++)
		col_label[start+i] = QString::number(max+i);

	setHeaderColType();
	emit modifiedWindow(this);	
}

void Table::insertCol()
{
	insertCols(selectedCol,1);
	emit modifiedWindow(this);
}

void Table::insertRow()
{
	int cr = worksheet->currentRow();
	if (worksheet->isRowSelected (cr, true))
	{
		worksheet->insertRows(cr,1);
		emit modifiedWindow(this);
	}
}

void Table::addCol(PlotDesignation pd)
{
	worksheet->clearSelection();
	int index, max=0, cols=worksheet->numCols();
	for (int i=0; i<cols; i++)
	{
		if (!col_label[i].contains(QRegExp ("\\D")))
		{
			index = col_label[i].toInt();
			if (index > max) 
				max = index;
		}
	}
	worksheet->insertColumns(cols,1);
	worksheet->ensureCellVisible ( 0, cols );

	comments << QString();
	commands<<"";
	colTypes<<Numeric;
	col_format<<"0/6";
	col_label<< QString::number(max+1);
	col_plot_type << pd;

	setHeaderColType();
	emit modifiedWindow(this);
}

void Table::addColumns(int c)
{
	int i, index,max=0, cols=worksheet->numCols();
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
	worksheet->insertColumns(cols,c);
	for (i=0; i<c; i++)
	{	
		comments << QString();
		commands<<"";
		colTypes<<Numeric;
		col_format<<"0/6";
		col_label<< QString::number(max+i);
		col_plot_type << Y;
	}
}

void Table::clearCol()
{
	int rows=worksheet->numRows();
	for (int i=0;i<rows;i++)
	{
		worksheet->setText(i,selectedCol, "");
	}
	QString name=colName(selectedCol);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

void Table::clearCell(int row, int col)
{
	worksheet->setText(row, col, "");

	QString name=colName(col);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

int Table::atRow(int col, double value)
{
	if (colTypes[col] == Numeric)
	{
		int row = -1;
		for (int i=0; i<worksheet->numRows(); i++)
		{
			if (worksheet->text(i,col).toDouble() == value)
				return i;
		}
		return row;
	}
	else
		return  -1;
}

void Table::deleteSelectedRows()
{
	Q3TableSelection sel=worksheet->selection(0);
	int top=sel.topRow();
	int bottom=sel.bottomRow();
	int numberOfRows=bottom-top+1;
	Q3MemArray<int> rowsToDelete(numberOfRows);
	for (int i=0; i<numberOfRows; i++)
		rowsToDelete[i]=top+i;
	worksheet->removeRows(rowsToDelete);
	notifyChanges();
}

void Table::cutSelection()
{
	copySelection();
	clearSelection();
}

void Table::selectAllTable()
{	
	worksheet->addSelection (Q3TableSelection( 0, 0, worksheet->numRows(), worksheet->numCols() ));
}

void Table::deselect()
{	
	worksheet->clearSelection();
}

void Table::clearSelection()
{	
	QStringList list=selectedColumns();
	int i,n=int(list.count());

	if (n>0)
	{
		for (i=0;i<n;i++)
		{
			QString name=list[i];
			int id=colIndex(name);
			selectedCol=id;
			clearCol();
		}
	}
	else
	{
		Q3TableSelection sel=worksheet->selection(0);
		int top=sel.topRow();
		int bottom=sel.bottomRow();
		int left=sel.leftCol();
		int right=sel.rightCol();

		if (sel.isEmpty ())
		{
			int row=worksheet->currentRow ();
			int col=worksheet->currentColumn ();
			worksheet->setText(row,col, "");

			QString name=colName(col);
			emit modifiedData(this, name);		
		}
		else
		{	
			for (i=top;i<=bottom;i++)
			{
				for (int j=left;j<=right;j++)
				{
					worksheet->setText(i, j, "");
				}
			}

			for (i=left;i<=right;i++)
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
	int rows=worksheet->numRows();
	int cols=worksheet->numCols();

	QVarLengthArray<int> selection(1);
	int c=0;	
	for (i=0;i<cols;i++)
	{
		if (worksheet->isColumnSelected(i,true))
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
				text+=worksheet->text(i,selection[j])+"\t";
			text+=worksheet->text(i,selection[c-1])+"\n";
		}	
	}
	else
	{
		Q3TableSelection sel = worksheet->selection(0);
		int right=sel.rightCol();
		int bottom=sel.bottomRow();
		for (i=sel.topRow(); i<=bottom; i++)
		{
			for (j=sel.leftCol(); j<right; j++)
				text+=worksheet->text(i,j)+"\t";
			text+=worksheet->text(i,right)+"\n";
		}
	}

	// Copy text into the clipboard
	QApplication::clipboard()->setData(new Q3TextDrag(text,worksheet,0));
}

// Paste text from the clipboard
void Table::pasteSelection()
{	
	QString text;		
	if (!Q3TextDrag::decode(QApplication::clipboard()->data(), text) || text.isEmpty())
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
	Q3TableSelection sel=worksheet->selection(0);
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
			top=bottom=worksheet->currentRow();
			left=right=worksheet->currentColumn();
		}
		else
		{
			top=0;
			bottom=worksheet->numRows() - 1;	
			left=0;
			right=worksheet->numCols() - 1;
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
						worksheet->insertRows(top, rows - r);
					else
						worksheet->insertRows(top, rows - r + 1);
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
	for (i=top; i<top+rows; i++)
	{
		s = ts2.readLine();
		cellTexts=s.split("\t");
		for (j=left; j<left+cols; j++)					
		{
			value = cellTexts[j-left].toDouble(&numeric);
			if (numeric)
			{
				columnNumericFormat(j, f, prec);
				worksheet->setText(i, j, QString::number(value, f, prec));
			}
			else
				worksheet->setText(i, j, cellTexts[j-left]);
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

			worksheet->removeColumn(id);
		}
		emit removedCol(name);
	}	
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::correlate()
{
	QStringList s=selectedColumns();
	if ((int)s.count() != 2)
	{
		QMessageBox::warning(0, tr("QtiPlot - Error"), tr("Please select two columns for this operation!"));
		return;
	}

	int col1 = colIndex(s[0]);
	int col2 = colIndex(s[1]);
	int i, rows=worksheet->numRows();
	int N = 16;
	while (N < rows) 
		N*=2;// round N up to the nearest power of 2

	double *tmpdata = new double[N];
	double *tmpdata2 = new double[N];
	if(tmpdata && tmpdata2) 
	{
		// zero-pad the two arrays...
		memset( tmpdata, 0, N * sizeof( double ) );
		memset( tmpdata2, 0, N * sizeof( double ) );
		for(i=0;i<rows;i++) 
		{
			tmpdata[i]=worksheet->text(i, col1).toDouble();
			tmpdata2[i]=worksheet->text(i, col2).toDouble();
		}
	}
	else
	{
		QMessageBox::warning(0,"QtiPlot", tr("Could not allocate memory, operation aborted!"));
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));	
	// calculate the FFTs of the two functions
	if( gsl_fft_real_radix2_transform( tmpdata, 1, N ) == 0 && 
			gsl_fft_real_radix2_transform( tmpdata2, 1, N ) == 0) 
	{
		// multiply the FFT by its complex conjugate
		for(i=0; i<N/2; i++ ) 
		{
			if( i==0 || i==(N/2)-1 )
				tmpdata[i] *= tmpdata[i];
			else 
			{
				int ni = N-i;
				double dReal = tmpdata[i] * tmpdata2[i] + tmpdata[ni] * tmpdata2[ni];
				double dImag = tmpdata[i] * tmpdata2[ni] - tmpdata[ni] * tmpdata2[i];					
				tmpdata[i] = dReal;
				tmpdata[ni] = dImag;
			}
		}
	}
	else
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::warning(0,"QtiPlot", tr("Error in GSL forward FFT operation!"));
		return;
	}

	gsl_fft_halfcomplex_radix2_inverse(tmpdata, 1, N );	//inverse FFT

	int cols=worksheet->numCols();
	int cols2 = cols+1;
	addCol();
	addCol();
	int n = rows/2;
	for ( i = 0; i<rows; i++) 
	{
		double y=0;
		if(i < n)
			y = tmpdata[N - n + i];
		else
			y = tmpdata[i-n];

		worksheet->setText(i, cols, QString::number(i - n));
		worksheet->setText(i, cols2, QString::number(y));
	}

	delete[] tmpdata;
	delete[] tmpdata2;

	s=colNames();
	QStringList l = s.grep("Lag");
	QString id = QString::number((int)l.size()+1);
	QString label="Corr"+id;

	col_label[cols]= "Lag"+id;
	col_label[cols2]= label;
	col_plot_type[cols] = X;
	setHeaderColType();

	emit plotCol(this, QStringList(QString(this->name())+"_"+label), 0);
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
	for (int i=0; i<worksheet->numCols(); i++)
	{
		selectedCol = i;
		normalizeCol();
	}
	emit modifiedWindow(this);	
}

void Table::normalizeCol(int col)
{
	if (col<0) col = selectedCol;
	double max=worksheet->text(0,col).toDouble();
	double aux=0.0;
	int i;
	int rows=worksheet->numRows();
	for (i=0; i<rows; i++)
	{
		QString text=this->text(i,col);
		aux=text.toDouble();
		if (!text.isEmpty() && fabs(aux)>fabs(max)) 
			max=aux;
	}

	if (max == 1.0)
		return;

	for (i=0;i<rows;i++)
	{
		QString text=this->text(i,col);
		aux=text.toDouble();
		if ( !text.isEmpty() )
			worksheet->setText(i,col,QString::number(aux/max));
	}

	QString name=colName(col);
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
		
		int rows=worksheet->numRows();
		int non_empty_cells = 0;
		QVarLengthArray<int> valid_cell(rows);
		QVarLengthArray<double> data_double(rows);
		for (int j = 0; j <rows; j++)
		{
			if (!worksheet->text(j, leadcol).isEmpty())
			{
				data_double[non_empty_cells] = this->text(j,leadcol).toDouble();
				valid_cell[non_empty_cells] = j;
				non_empty_cells++;
			}
		}
		
		if (!non_empty_cells)
		{
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
		
		for(int i=0;i<cols;i++)
		{// Since we have the permutation index, sort all the columns
            int col=colIndex(s[i]);
            if (columnType(col) == Text)
            {
                for (int j=0; j<non_empty_cells; j++)
                    data_string[j] = text(valid_cell[j], col);
                if(!order)
                    for (int j=0; j<non_empty_cells; j++)
                        worksheet->setText(valid_cell[j], col, data_string[p[j]]);
                else
                    for (int j=0; j<non_empty_cells; j++)
                        worksheet->setText(valid_cell[j], col, data_string[p[non_empty_cells-j-1]]);
            }
            else
            {
                for (int j = 0; j<non_empty_cells; j++)
                    data_double[j] = text(valid_cell[j], col).toDouble();
                int prec;
                char f;
                columnNumericFormat(col, f, prec);
                if(!order)
                    for (int j=0; j<non_empty_cells; j++)
                        worksheet->setText(valid_cell[j], col, QString::number(data_double[p[j]], f, prec));
                else
                    for (int j=0; j<non_empty_cells; j++)
                        worksheet->setText(valid_cell[j], col, QString::number(data_double[p[non_empty_cells-j-1]], f, prec));
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
		col = worksheet->currentColumn();
	
	int rows=worksheet->numRows();
	int non_empty_cells = 0;
	QVarLengthArray<int> valid_cell(rows);
	QVarLengthArray<double> r(rows);
    QStringList text_cells;
	for (int i = 0; i <rows; i++)
	{
		if (!worksheet->text(i, col).isEmpty())
		{
            if (columnType(col) == Table::Text)
                text_cells << worksheet->text(i, col);
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
                worksheet->setText(valid_cell[i], col, text_cells[i]);
        }
		else
        {
            for (int i=0; i<non_empty_cells; i++)
                worksheet->setText(valid_cell[i], col, text_cells[non_empty_cells-i-1]);
        }
    }
    else
    {
	   int prec;
	   char f;
	   columnNumericFormat(col, f, prec);
        if (!order)
        {
	       for (int i=0; i<non_empty_cells; i++)
                worksheet->setText(valid_cell[i], col, QString::number(r[i], f, prec));
        }
        else
        {
            for (int i=0; i<non_empty_cells; i++)
                worksheet->setText(valid_cell[i], col, QString::number(r[non_empty_cells-i-1], f, prec));
        }
    }
	emit modifiedData(this, colName(col));
	emit modifiedWindow(this);
}

void Table::sortColAsc()
{
sortColumn(worksheet->currentColumn ());
}

void Table::sortColDesc()
{
sortColumn(worksheet->currentColumn(), 1);
}

int Table::tableRows()
{
	return worksheet->numRows();
}

int Table::tableCols()
{
	return worksheet->numCols();
}

bool Table::isEmptyRow(int row)
{
	bool empty=true;
	int cols=worksheet->numCols();
	for (int i=0;i<cols;i++)
	{
		QString text=worksheet->text(row,i);
		if (!text.isEmpty())
		{
			empty=false;
			break;
		}
	}	
	return empty;
}

bool Table::isEmptyColumn(int col)
{
	bool empty=true;
	int rows=worksheet->numRows();
	for (int i=0;i<rows;i++)
	{
		QString text=worksheet->text(i,col);
		if (!text.isEmpty())
		{
			empty=false;
			break;
		}
	}	
	return empty;
}

QString Table::saveText()
{
	QString text = "<data>\n";
	int cols=worksheet->numCols();
	int rows=worksheet->numRows();

	for (int i=0; i<rows; i++)
	{
		if (!isEmptyRow(i))
		{
			text+=QString::number(i)+"\t";
			for (int j=0; j<cols-1; j++)
				text+=worksheet->text(i,j)+"\t";

			text+=worksheet->text(i,cols-1)+"\n";
		}
	}	
	return text + "</data>\n";
}

int Table::nonEmptyRows()
{
	int r=0; 	
	for (int i=0;i<worksheet->numRows();i++)
	{
		if (!isEmptyRow(i))
			r++;
	}	
	return r;
}

QString Table::text(int row, int col)
{
	if (col == savedCol)
		return savedCells[row].replace(",",".");
	else
		return worksheet->text(row,col).replace(",", ".");
}

void Table::setText (int row, int col, const QString & text )
{
	worksheet->setText(row, col, text); 
}

void Table::saveColToMemory(int col)
{
	int rows=worksheet->numRows();
	savedCells.clear();
	for (int row=0; row<rows; row++)
		savedCells << worksheet->text(row, col);
	savedCol = col;
}

void Table::forgetSavedCol()
{
	savedCells.clear();
	savedCol = -1;
}

void Table::setTextFormat(bool applyToAll)
{
	if (applyToAll)
	{
		for (int i=selectedCol; i<worksheet->numCols(); i++)
			colTypes[i] = Text;
	}
	else
		colTypes[selectedCol] = Text;
}

void Table::setNumericFormat(int f, int prec, bool applyToAll)
{
	int cols=worksheet->numCols();
	if (applyToAll)
	{
		for (int i=selectedCol; i<cols; i++)
			setColNumericFormat(f, prec, i);
	}
	else
		setColNumericFormat(f, prec, selectedCol);

	emit modifiedWindow(this);
}

void Table::setColNumericFormat(int f, int prec, int col)
{
	int old_f, old_prec;
	columnNumericFormat(col, old_f, old_prec);
	if (colTypes[col] == Numeric && old_f == f && old_prec == prec)
		return;

	colTypes[col] = Numeric;
	col_format[col] = QString::number(f)+"/"+QString::number(prec);

	int rows=worksheet->numRows();
	for (int i=0;i<rows;i++)
	{
		QString t = worksheet->text(i, col);
		if (!t.isEmpty())
		{
			double val = t.toDouble();
			if (!f) 
				t.setNum(val, 'g', 6);
			else if (f==1)
				t.setNum(val,'f',prec);
			else if (f==2)
				t.setNum(val,'e',prec);

			worksheet->setText(i, col, t);
		}		
	}
}

void Table::setColumnsFormat(const QStringList& lst)
{
	if (col_format == lst)
		return;

	col_format = lst;
}

void Table::setDateTimeFormat(int f, const QString& format, bool applyToAll)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int cols=worksheet->numCols();
	if (applyToAll)
	{
		for (int j=selectedCol; j<cols; j++)
			setDateTimeFormat(f, format, j);
	}
	else
		setDateTimeFormat(f, format, selectedCol);

	emit modifiedWindow(this);

	QApplication::restoreOverrideCursor();	
}

void Table::setDateTimeFormat(int f, const QString& format, int col)
{
	switch (f)
	{
		case 2:
			setDateFormat(format, col);
			break;

		case 3:
			setTimeFormat(format, col);
			break;

		case 4:
			setMonthFormat(format, col);
			break;

		case 5:
			setDayFormat(format, col);
			break;

		default:
			break;
	}
}

void Table::setDateFormat(const QString& format, int col)
{
	if (col_format[col] == format)
		return;

	colTypes[col] = Date;
	col_format[col] = format;
	int rows=worksheet->numRows();

	for (int i=0; i<rows; i++)
	{
		QString s = worksheet->text(i,col);
		if (!s.isEmpty())
		{
			QDate d = QDate::fromString (s, Qt::ISODate);
			if (d.isValid())
				worksheet->setText(i, col, d.toString(format));
			else
				worksheet->setText(i, col, "-");
		}
	}
}

void Table::setTimeFormat(const QString& format, int col)
{
	if (col_format[col] == format)
		return;

	int rows=worksheet->numRows();

	colTypes[col] = Time;
	col_format[col] = format;

	for (int i=0; i<rows; i++)
	{
		QString s = worksheet->text(i,col);
		if (!s.isEmpty())
		{
			QTime t = QTime::fromString (s,Qt::TextDate);
			if (t.isValid())
				worksheet->setText(i, col, t.toString(format));
			else
				worksheet->setText(i, col, "-");
		}
	}
}

void Table::setMonthFormat(const QString& format, int col)
{
	colTypes[col] = Month;
	int rows=worksheet->numRows();
	if (format == "shortMonthName")
	{
		for (int i=0;i<rows; i++)
		{
			QString s = worksheet->text(i,col);
			if (!s.isEmpty())
			{
				int month= s.toInt() % 12;
				if (!month)
					month = 12;

				worksheet->setText(i, col, QDate::shortMonthName(month));
			}
		}
	}
	else if (format == "longMonthName")
	{
		for (int i=0;i<rows; i++)
		{
			QString t = worksheet->text(i,col);
			if (!t.isEmpty())
			{
				int month= t.toInt() % 12;
				if (!month)
					month = 12;

				worksheet->setText(i, col, QDate::longMonthName(month));
			}
		}
	}
}

void Table::setDayFormat(const QString& format, int col)
{
	colTypes[col] = Day;
	int rows=worksheet->numRows();
	if (format == "shortDayName")
	{
		for (int i=0;i<rows; i++)
		{
			QString t = worksheet->text(i,col);
			if (!t.isEmpty())
			{
				int day= t.toInt() % 7;
				if (!day)
					day = 7;

				worksheet->setText(i, col, QDate::shortDayName(day));
			}
		}
	}
	else if (format == "longDayName")
	{
		for (int i=0;i<rows; i++)
		{
			QString t = worksheet->text(i,col);
			if (!t.isEmpty())
			{
				int day= t.toInt() % 7;
				if (!day)
					day = 7;

				worksheet->setText(i, col, QDate::longDayName(day));
			}
		}
	}
}

void Table::setRandomValues()
{	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int i;
	double max=0.0;
	int rows=worksheet->numRows();
	QStringList list=selectedColumns();
	QVarLengthArray<double> r(rows);

	for (int j=0;j<(int) list.count(); j++)
	{
		QString name=list[j];
		selectedCol=colIndex(name);

		int prec;
		char f;
		columnNumericFormat(selectedCol, f, prec);

		srand(rand());

		for (i=0;i<rows;i++)
		{
			r[i]=rand();
			if (max<r[i]) max=r[i];
		}

		for (i=0;i<rows;i++) 
		{
			r[i]/=max;
			worksheet->setText(i,selectedCol,QString::number(r[i], f, prec));	
		}

		emit modifiedData(this, name);
	}

	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::loadHeader(QStringList header)
{
	for (int i=0;i<(int)worksheet->numCols();i++) 
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
	Q3Header *head=worksheet->horizontalHeader();
	int xcols=0;
	for (int j=0;j<(int)worksheet->numCols();j++) 
	{
		if (col_plot_type[j] == X)
			xcols++;
	}

	if (xcols>1)
	{
		xcols = 0;
		for (int i=0; i<(int)worksheet->numCols(); i++) 
		{
			if (col_plot_type[i] == X)
				head->setLabel(i, col_label[i]+"[X" + QString::number(++xcols) +"]");
			else if (col_plot_type[i] == Y)
			{
				if(xcols>0)
					head->setLabel(i, col_label[i]+"[Y"+ QString::number(xcols) +"]");
				else
					head->setLabel(i, col_label[i]+"[Y]");
			}
			else if (col_plot_type[i] == Z)
			{
				if(xcols>0)
					head->setLabel(i, col_label[i]+"[Z"+ QString::number(xcols) +"]");
				else
					head->setLabel(i, col_label[i]+"[Z]");
			}
			else if (col_plot_type[i] == xErr)
				head->setLabel(i, col_label[i]+"[xEr]");
			else if (col_plot_type[i] == yErr)
				head->setLabel(i, col_label[i]+"[yEr]");
			else
				head->setLabel(i, col_label[i]);
		}
	}
	else
	{
		for (int i=0; i<(int)worksheet->numCols(); i++) 
		{
			if (col_plot_type[i] == X)
				head->setLabel(i, col_label[i]+"[X]");
			else if (col_plot_type[i] == Y)
				head->setLabel(i, col_label[i]+"[Y]");
			else if (col_plot_type[i] == Z)
				head->setLabel(i, col_label[i]+"[Z]");
			else if (col_plot_type[i] == xErr)
				head->setLabel(i, col_label[i]+"[xEr]");
			else if (col_plot_type[i] == yErr)
				head->setLabel(i, col_label[i]+"[yEr]");
			else
				head->setLabel(i, col_label[i]);
		}
	}
}

void Table::setAscValues()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows=worksheet->numRows();
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
		columnNumericFormat(selectedCol, f, prec);

		for (int i=0;i<rows;i++) 
			worksheet->setText(i,selectedCol,QString::number(i+1, f, prec));

		emit modifiedData(this, name);
	}

	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::plotL()
{	
	if (!valid2DPlot())
		return;

	emit plotCol(this, drawableColumnSelection(), Graph::Line);
}

void Table::plotP()
{
	if (!valid2DPlot())
		return;

	emit plotCol(this, drawableColumnSelection(), Graph::Scatter);
}

void Table::plotLP()
{
	if (!valid2DPlot())
		return;

	emit plotCol(this, drawableColumnSelection(), Graph::LineSymbols);
}

void Table::plotVB()
{
	if (!valid2DPlot())
		return;

	emit plotCol(this, drawableColumnSelection(), Graph::VerticalBars);
}

void Table::plotHB()
{
	if (!valid2DPlot())
		return;

	emit plotCol(this, drawableColumnSelection(), Graph::HorizontalBars);
}

void Table::plotArea()
{
	if (!valid2DPlot())
		return;

	emit plotCol(this, drawableColumnSelection(), Graph::Area);
}

bool Table::noXColumn()
{
	bool notSet = true;
	for (int i=0; i<worksheet->numCols(); i++)
	{
		if (col_plot_type[i] == X)
			return false;
	}
	return notSet;
}

bool Table::noYColumn()
{
	bool notSet = true;
	for (int i=0; i<worksheet->numCols(); i++)
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
		emit plotCol(this,s, Graph::Pie);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a column to plot!"));
}

void Table::plotVerticalDropLines()
{
	if (!valid2DPlot())
		return;

	emit plotCol(this, drawableColumnSelection(), Graph::VerticalDropLines);
}

void Table::plotSpline()
{
	if (!valid2DPlot())
		return;

	emit plotCol(this, drawableColumnSelection(), Graph::Spline);
}

void Table::plotVertSteps()
{
	if (!valid2DPlot())
		return;

	emit plotCol(this, drawableColumnSelection(), Graph::VerticalSteps);
}

void Table::plotHorSteps()
{
	if (!valid2DPlot())
		return;
	
	emit plotCol(this, drawableColumnSelection(), Graph::HorizontalSteps);
}

void Table::plotHistogram()
{
	if (!valid2DPlot())
		return;
	
	 emit plotCol(this, drawableColumnSelection(), Graph::Histogram);
}

void Table::plotBoxDiagram()
{
	if (!valid2DPlot())
		return;
	emit plotCol(this, drawableColumnSelection(), Graph::Box);
}

void Table::plotVectXYXY()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedColumns();
	if ((int)s.count() == 4)
		emit plotCol(this, s, Graph::VectXYXY);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"),tr("Please select four columns for this operation!"));
}

void Table::plotVectXYAM()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedColumns();
	if ((int)s.count() == 4)
		emit plotCol(this, s, Graph::VectXYAM);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"),tr("Please select four columns for this operation!"));
}

bool Table::valid2DPlot()
{
	if (!selectedYColumns().count())
  	{
  		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
  	    return false;
  	}
  	else if (worksheet->numCols()<2)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),tr("You need at least two columns for this operation!"));
		return false;
	}
	else if (noXColumn())
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"), tr("Please set a default X column for this table, first!"));
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
	if (worksheet->numCols()<2)
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
		int r = worksheet->numRows();
		int c = worksheet->numCols(); 
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
			s = s.simplifyWhiteSpace();
		else if (stripSpaces)
			s = s.stripWhiteSpace();
		QStringList line = s.split(sep);
		cols = (int)line.count();

		bool allNumbers = true;
		line.gres ( ",", ".", false ); //Qt uses decimal dot
		for (i=0; i<cols; i++)
		{//verify if the strings in the line used to rename the columns are not all numbers
			(line[i]).toDouble(&allNumbers);
			if (!allNumbers)
				break;
		}

		if (renameCols && !allNumbers)
			rows--;

		Q3ProgressDialog progress(0, "progress", true, Qt::WindowStaysOnTopHint|Qt::Tool);
		progress.setWindowTitle("Qtiplot - Reading file...");
		progress.setLabelText(fname);
		progress.setActiveWindow();
		progress.setAutoClose(true);
		progress.setAutoReset(true);

		int steps = int(rows/1000);
		progress.setTotalSteps(steps+1);

		QApplication::restoreOverrideCursor();

		if (!importFileAs)
			init (rows, cols);
		else if (importFileAs == 1)
		{//new cols
			addColumns(cols);
			if (r < rows)
				worksheet->setNumRows(rows);
		}
		else if (importFileAs == 2)
		{//new rows
			if (c < cols)
				addColumns(cols-c);			
			worksheet->setNumRows(r+rows);
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
					s = s.simplifyWhiteSpace();
				else if (stripSpaces)
					s = s.stripWhiteSpace();
				line = s.split(sep);
				for (int j=startCol; j<worksheet->numCols(); j++)
					worksheet->setText(startRow + k, j, line[j-startCol]);
			}

			startRow+= 1000;
			progress.setProgress(i);
		}

		for (i=startRow; i<worksheet->numRows(); i++)
		{
			s = t.readLine(); 
			if (simplifySpaces)
				s = s.simplifyWhiteSpace();
			else if (stripSpaces)
				s = s.stripWhiteSpace();
			line = s.split(sep);
			for (int j=startCol; j<worksheet->numCols(); j++)
				worksheet->setText(i, j, line[j-startCol]);
		}
		progress.setProgress(steps+1);
		f.close();

		if (importFileAs)
		{
			for (i=0; i<worksheet->numCols(); i++)
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
			s = s.simplifyWhiteSpace();
		else if (stripSpaces)
			s = s.stripWhiteSpace();

		QStringList line = s.split(sep);
		cols = (int)line.count();

		bool allNumbers = true;
		line.gres ( ",", ".", false ); //Qt uses decimal dot
		for (i=0; i<cols; i++)
		{//verify if the strings in the line used to rename the columns are not all numbers
			(line[i]).toDouble(&allNumbers);
			if (!allNumbers)
				break;
		}

		if (renameCols && !allNumbers)
			rows--;
		int steps = int(rows/1000);

		Q3ProgressDialog progress(0, "progress", true, Qt::WindowStaysOnTopHint|Qt::Tool);
		progress.setWindowTitle("Qtiplot - Reading file...");
		progress.setLabelText(fname);
		progress.setActiveWindow();
		progress.setAutoClose(true);
		progress.setAutoReset(true);
		progress.setTotalSteps(steps+1);

		QApplication::restoreOverrideCursor();

		QStringList oldHeader;	
		if (newTable)
			init (rows, cols);
		else
		{
			if (worksheet->numRows() != rows)
				worksheet->setNumRows(rows);

			c = worksheet->numCols();
			oldHeader = col_label;
			if (c != cols)
			{
				if (c < cols)
					addColumns(cols - c);
				else 
				{
					worksheet->setNumCols(cols);
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
					worksheet->setText(start + k, j, line[j]);
			}
			progress.setProgress(i);
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
				worksheet->setText(i, j, line[j]);
		}
		progress.setProgress(steps+1);
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

	QString text;
	int i,j;
	int rows=worksheet->numRows();
	int cols=worksheet->numCols();
	int selectedCols = 0;
	int topRow = 0, bottomRow = 0;
	int *sCols;
	if (exportSelection)
	{
		for (i=0; i<cols; i++)
		{
			if (worksheet->isColumnSelected(i)) 
				selectedCols++;
		}

		sCols = new int[selectedCols];
		int aux = 1;
		for (i=0; i<cols; i++)
		{
			if (worksheet->isColumnSelected(i)) 
			{
				sCols[aux] = i;
				aux++;
			}
		}

		for (i=0; i<rows; i++)
		{
			if (worksheet->isRowSelected(i)) 
			{
				topRow = i;
				break;
			}
		}

		for (i=rows - 1; i>0; i--)
		{
			if (worksheet->isRowSelected(i)) 
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
				text+=worksheet->text(i, sCols[j]) + separator;			
			text+=worksheet->text(i, sCols[selectedCols]) + "\n";
		}
		delete[] sCols;//free memory
	}
	else
	{
		for (i=0;i<rows;i++)
		{
			for (j=0;j<cols-1;j++)
				text+=worksheet->text(i,j)+separator;
			text+=worksheet->text(i,cols-1)+"\n";
		}
	}
	QTextStream t( &f );
	t << text;
	f.close();
	return true;
}

void Table::contextMenuEvent(QContextMenuEvent *e)
{
	QRect r = worksheet->horizontalHeader()->sectionRect(worksheet->numCols()-1);
	if (e->pos().x() > r.right() + worksheet->verticalHeader()->width())
		emit showContextMenu(false);
	else
		emit showContextMenu(true);
	e->accept();
}

void Table::moveCurrentCell()
{
	int cols=worksheet->numCols();
	int row=worksheet->currentRow();
	int col=worksheet->currentColumn();
	worksheet->clearSelection (true);

	if (col+1<cols)
	{
		worksheet->setCurrentCell (row,col+1);
		worksheet->selectCells(row,col+1,row,col+1);
	}
	else
	{
		worksheet->setCurrentCell (row+1,0);
		worksheet->selectCells(row+1,0,row+1,0);	
	}
}

bool Table::eventFilter(QObject *object, QEvent *e)
{
	Q3Header *hheader = worksheet->horizontalHeader();
	Q3Header *vheader = worksheet->verticalHeader();

	if (e->type() == QEvent::MouseButtonDblClick && object == (QObject*)hheader) {
		const QMouseEvent *me = (const QMouseEvent *)e;
		selectedCol = hheader->sectionAt (me->pos().x() + hheader->offset());

		QRect rect = hheader->sectionRect (selectedCol);
		rect.setLeft(rect.right() - 2);
		rect.setWidth(4);

		if (rect.contains (me->pos())) {
			worksheet->adjustColumn(selectedCol);
			emit modifiedWindow(this);
		} else
			emit optionsDialog();
		return true;
	} else if (e->type() == QEvent::MouseButtonPress && object == (QObject*)hheader) {
		const QMouseEvent *me = (const QMouseEvent *)e;
		if (me->button() == Qt::LeftButton && me->state() == Qt::ControlButton) {		
			selectedCol = hheader->sectionAt (me->pos().x() + hheader->offset());
			worksheet->selectColumn (selectedCol);
			worksheet->setCurrentCell (0, selectedCol);
			return true;
		} else if (selectedColsNumber() <= 1) {
			selectedCol = hheader->sectionAt (me->pos().x() + hheader->offset());
			worksheet->clearSelection();
			worksheet->selectColumn (selectedCol);
			worksheet->setCurrentCell (0, selectedCol);
			return false;
		}
	} else if (e->type() == QEvent::MouseButtonPress && object == (QObject*)vheader) {
		const QMouseEvent *me = (const QMouseEvent *)e;
		if (me->button() == Qt::RightButton && selectedRows() <= 1) {
			worksheet->clearSelection();
			int row = vheader->sectionAt(me->pos().y() + vheader->offset());
			worksheet->selectRow (row);
			worksheet->setCurrentCell (row, 0);
		}
	} else if (e->type()==QEvent::ContextMenu && object == titleBar) {
		emit showTitleBarMenu();
		((QContextMenuEvent*)e)->accept();
		return true;
	}

	return MyWidget::eventFilter(object, e);
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
	int cols=worksheet->numCols();
	int rows=worksheet->numRows();

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
		worksheet->setNumRows(r);

	int c=list[2].toInt();
	if (cols != c)
		worksheet->setNumCols(c);

	//clear all cells
	for (i=0;i<rows;i++)
	{
		for (j=0; j<c; j++)
			worksheet->setText(i,j, "");
	} 

	t.readLine();	//table geometry useless info when restoring
	s = t.readLine();//header line

	list = s.split("\t");
	list.remove(list.first());

	if (!col_label.isEmpty() && col_label != list)
	{
		loadHeader(list);
		list.gres("[X]","");
		list.gres("[Y]","");
		list.gres("[Z]","");
		list.gres("[xEr]","");
		list.gres("[yEr]","");

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
		for (int i=0; i<tableCols(); i++)
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
			worksheet->setText(row, j, list[j+1]);	
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
			worksheet->setText(row, j, list[j+1]);	

		s= t.readLine();	
	}	

	for (j=0; j<c; j++)
		emit modifiedData(this, colName(j));	
}

void Table::setNumRows(int rows)
{
	worksheet->setNumRows(rows);
}

void Table::setNumCols(int cols)
{
	worksheet->setNumCols(cols);
}

void Table::resizeRows(int r)
{
	int rows = worksheet->numRows();
	if (rows == r)
		return;

	if (rows > r)
	{
		QString text= tr("Rows will be deleted from the table!");
		text+="<p>"+tr("Do you really want to continue?");
		int i,cols = worksheet->numCols();
		switch( QMessageBox::information(this,tr("QtiPlot"), text, tr("Yes"), tr("Cancel"), 0, 1 ) ) 
		{
			case 0:
				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				worksheet->setNumRows(r);
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
		worksheet->setNumRows(r);
		QApplication::restoreOverrideCursor();
	}

	emit modifiedWindow(this);
}

void Table::resizeCols(int c)
{
	int i, cols = worksheet->numCols();	
	if (cols == c)
		return;

	if (cols > c)
	{
		QString text= tr("Columns will be deleted from the table!");
		text+="<p>"+tr("Do you really want to continue?");
		Q3MemArray<int> columns(cols-c);
		switch( QMessageBox::information(this,tr("QtiPlot"), text, tr("Yes"), tr("Cancel"), 0, 1 ) ) 
		{
			case 0:
				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				for (i=cols-1; i>=c; i--)
				{
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

				worksheet->removeColumns(columns);
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

void Table::convolute(int sign)
{
	QStringList s=selectedColumns();
	if ((int)s.count() != 2)
	{
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select two columns for this operation:\n the first represents the signal and the second the response function!"));
		return;
	}

	int col1 = colIndex(s[0]);
	int col2 = colIndex(s[1]);
	int i, rows = worksheet->numRows();
	int m = 0;
	for (i=0;i<rows;i++) 
	{
		if (!worksheet->text(i, col2).isEmpty())
			m++;
	}
	if (m >= rows/2)
	{
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("The response dataset '%1' must be less then half the size of the signal dataset '%2'!").arg(s[1]).arg(s[0]));
		return;
	}
	else if (m%2 == 0)
	{
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("The response dataset '%1' must contain an odd number of points!").arg(s[1]));
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int n = 16;// tmp number of points
	while (n < rows + m/2) 
		n*=2;

	double *sig = new double[n];
	double *res = new double[m];

	memset(sig,0,n*sizeof(double));
	for (i=0;i<rows;i++) 
		sig[i]=worksheet->text(i, col1).toDouble();
	for (i=0;i<m;i++) 
		res[i]=worksheet->text(i, col2).toDouble();

	convlv(sig, n, res, m, sign);

	int cols=worksheet->numCols();
	int cols2 = cols+1;
	addCol();
	addCol();
	for (i = 0; i<rows; i++) 
	{
		worksheet->setText(i, cols, QString::number(i+1));
		worksheet->setText(i, cols2, QString::number(sig[i]));
	}

	delete[] res;
	delete[] sig;

	QString label = "Conv";
	if (sign == -1)
		label = "Deconv";

	s=colNames();
	QStringList l = s.grep("Index");
	QString id = QString::number((int)l.size()+1);
	label+=id;

	col_label[cols] = "Index"+id;
	col_label[cols2] = label;
	col_plot_type[cols] = X;
	setHeaderColType();

	emit plotCol(this, QStringList(QString(this->name())+"_"+label), 0);
	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();	
}

void Table::convlv(double *sig, int n, double *dres, int m, int sign)
{
	double *res = new double[n];
	memset(res,0,n*sizeof(double));
	int i, m2 = m/2;
	for (i=0;i<m2;i++) 
	{//store the response in wrap around order, see Numerical Recipes doc
		res[i] = dres[m2+i];
		res[n-m2+i] = dres[i];
	}

	if(m2%2==1)
		res[m2]=dres[m-1];	

	// calculate ffts
	gsl_fft_real_radix2_transform(res,1,n);
	gsl_fft_real_radix2_transform(sig,1,n);

	double re, im, size;
	for (i=0;i<n/2;i++) 
	{// multiply/divide both ffts
		if(i==0 || i==n/2-1) 
		{
			if(sign == 1)
				sig[i] = res[i]*sig[i];
			else 
				sig[i] = sig[i]/res[i];
		}
		else 
		{
			int ni = n-i;
			if(sign == 1) 
			{
				re = res[i]*sig[i]-res[ni]*sig[ni];
				im = res[i]*sig[ni]+res[ni]*sig[i];
			}
			else 
			{
				size = res[i]*res[i]+res[ni]*res[ni];
				re = res[i]*sig[i]+res[ni]*sig[ni];
				im = res[i]*sig[ni]-res[ni]*sig[i];
				re /= size;
				im /= size;
			}

			sig[i] = re;
			sig[ni] = im;
		}
	}
	delete[] res;
	gsl_fft_halfcomplex_radix2_inverse(sig,1,n);// inverse fft
}

void Table::copy(Table *m)
{
	for (int i=0; i<worksheet->numRows(); i++)
	{
		for (int j=0; j<worksheet->numCols(); j++)
			worksheet->setText(i,j,m->text(i,j));
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
	QString s="<table>\t"+QString::number(worksheet->numRows())+"\t";
	s+=QString::number(worksheet->numCols())+"\n";
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
		for (int col=0; col<tableCols(); col++)
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
	for (int i=0; i<worksheet->numCols(); i++)
		emit modifiedData(this, colName(i));

	emit modifiedWindow(this);
}

void Table::clear()
{
	for (int i=0; i<worksheet->numCols(); i++)
	{
		for (int j=0; j<worksheet->numRows(); j++)
			worksheet->setText(j, i, QString::null);

		emit modifiedData(this, colName(i));
	}

	emit modifiedWindow(this);
}

Table::~Table()
{
}
