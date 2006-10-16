/***************************************************************************
    File                 : worksheet.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
                           knut.franke@gmx.de
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

#include <QtDebug>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_complex.h>

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
	LeftButton=false;
	savedCol=-1;

	QDateTime dt = QDateTime::currentDateTime ();
	setBirthDate(dt.toString(Qt::LocalDate));

	worksheet = new QTableWidget(rows, cols, 0);
	worksheet->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	worksheet->setFocusPolicy(Qt::StrongFocus);
	worksheet->setFocus();
	worksheet->setSelectionMode(QAbstractItemView::ExtendedSelection);
	// TODO: testing QHeaderView::ResizeToContents instead of setMovable(true)
	worksheet->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	//worksheet->verticalHeader()->setMovable(true);
	// TODO: would be useful, but then we have to interpret
	// signal Q3Header::indexChange ( int section, int fromIndex, int toIndex )
	// and update some variables
	//worksheet->setColumnMovingEnabled(true);

	connect(worksheet->verticalHeader(), SIGNAL(sectionMoved( int, int, int )),
			this, SLOT(notifyChanges()));

	connect(worksheet->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)),
			this, SLOT(headerDoubleClickedHandler(int)));

	QVBoxLayout* hlayout = new QVBoxLayout(this);
	hlayout->setMargin(0);
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

	QHeaderView* head = worksheet->horizontalHeader();
	connect(head, SIGNAL(sectionResized(int, int, int)), this, SLOT(colWidthModified(int, int, int)));

	col_plot_type[0] = X;
	setHeaderColType();

	int w=4*(worksheet->horizontalHeader())->sectionSize(0);
	int h;
	if (rows>11)
		h=11*(worksheet->verticalHeader())->sectionSize(0);
	else
		h=(rows+1)*(worksheet->verticalHeader())->sectionSize(0);
	setGeometry(50,50,w + 45, h);

	worksheet->verticalHeader()->setMovable(false);

	QShortcut *accel_move = new QShortcut(this);
	QShortcut *accel_selectAll = new QShortcut(this);
	accel_move->setKey(Qt::Key_Tab );
	connect(accel_move, SIGNAL(activated()), this, SLOT(moveCurrentCell()) );
	accel_selectAll->setKey( Qt::CTRL+Qt::Key_A );
	connect(accel_selectAll, SIGNAL(activated()), this, SLOT(selectAllTable()) );

	connect(worksheet, SIGNAL(itemChanged(QTableWidgetItem *)),this, SLOT(cellEdited(QTableWidgetItem *)));
	specifications = saveToString("geometry\n");
}

void Table::colWidthModified(int, int, int)
{
	emit modifiedWindow(this);
}


void Table::setBackgroundColor(const QColor& col)
{
	QPalette pal = worksheet->palette();
	pal.setColor(QPalette::Base, col);
	worksheet->setPalette(pal);
}

void Table::setTextColor(const QColor& col)
{
	QPalette pal = worksheet->palette();
	pal.setColor(QPalette::Text, col);
	worksheet->setPalette(pal);
}

void Table::setTextFont(const QFont& fnt)
{
	worksheet->setFont (fnt);
	// TODO: remark: deactivated the following 3 lines in favor of testing QHeaderView::ResizeToContents in constructor
	//QFontMetrics fm(fnt);
	//int lm = fm.width( QString::number(10*worksheet->rowCount()));
	//worksheet->setLeftMargin( lm );
}

void Table::setHeaderColor(const QColor& col)
{
	QPalette pal = worksheet->horizontalHeader()->palette();
	pal.setColor(QPalette::ButtonText, col);
	worksheet->horizontalHeader()->setPalette(pal);

	pal = worksheet->verticalHeader()->palette();
	pal.setColor(QPalette::ButtonText, col);
	worksheet->verticalHeader()->setPalette(pal);
}

void Table::setHeaderFont(const QFont& fnt)
{
	worksheet->horizontalHeader()->setFont (fnt);
}

void Table::print()
{

	// FIXME: port this
	// old code (Qt3):

	/*
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

	   int rows=worksheet->rowCount();
	   int cols=worksheet->columnCount();
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
*/
}

void Table::cellEdited(QTableWidgetItem * item)
{
	int col = worksheet->column(item);
	QString name = colName(col);
	emit modifiedData(this, name);
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
	for(i=col+1; i<worksheet->columnCount(); i++)
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
	for(i=col+1; i<worksheet->columnCount(); i++)
	{
		if (col_plot_type[i] == Y)
			return i;
	}
	return yCol;
}

void Table::setPlotDesignation(PlotDesignation pd)
{
	QStringList list = selectedColumns();
	for (int i=0;i< list.count(); i++)
		col_plot_type[colIndex(list[i])] = pd;

	setHeaderColType();
	emit modifiedWindow(this);
}

void Table::columnNumericFormat(int col, int &f, int &precision)
{
	QStringList format = QStringList::split("/", col_format[col], false);
	f = format[0].toInt();
	precision = format[1].toInt();
}

void Table::columnNumericFormat(int col, char &f, int &precision)
{
	QStringList format = QStringList::split("/", col_format[col], false);
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
	for (int i=0;i<worksheet->columnCount();i++)
		widths << QString::number(worksheet->columnWidth(i));

	return widths;
}

void Table::setColWidths(const QStringList& widths)
{
	for (int i=0;i<widths.count();i++)
		worksheet->setColumnWidth(i, widths[i].toInt() );
}

void Table::setColumnTypes(const QStringList& ctl)
{
	int n = qMin(ctl.count(), tableCols());
	for (int i=0; i<n; i++)
	{
		QStringList l= QStringList::split(";", ctl[i], true);
		colTypes[i] = l[0].toInt();

		if ((int)l.count() > 0 && !l[1].isEmpty())
			col_format[i] = l[1];
		else 
			col_format[i] = "0/6";
	}
}

QString Table::saveColumnWidths()
{
	QString s = "ColWidth\t";
	for (int i=0;i<worksheet->columnCount();i++)
		s+=QString::number(worksheet->columnWidth (i))+"\t";

	return s+"\n";
}

QString Table::saveColumnTypes()
{
	QString s="ColType";
	for (int i=0; i<worksheet->columnCount(); i++)
		s += "\t"+QString::number(colTypes[i])+";"+col_format[i];

	return s+"\n";
}

void Table::setCommands(const QStringList& com)
{
	commands.clear();
	for(int i=0; i<com.size() && i<tableCols(); i++)
		commands << com[i].trimmed();
}

void Table::setCommand(int col, const QString com)
{
	if(col<commands.size())
		commands[col] = com.trimmed();
}

void Table::setCommands(const QString& com)
{
	QStringList lst = QStringList::split("\t",com,true);
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
			setText(i, col, QString::number(ret.toDouble(), f, prec));
		} else if(ret.canConvert(QVariant::String))
			setText(i, col, ret.toString());
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

QList<QTableWidgetSelectionRange> Table::getSelection()
{
	QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
	// TODO: check if more than the line above is needed
	// old Qt3 code:
	/*
	   if (worksheet->numSelections()==0)
	   {
	   sel.init(worksheet->currentRow(), worksheet->currentColumn());
	   sel.expandTo(worksheet->currentRow(), worksheet->currentColumn());
	   } else if (worksheet->currentSelection()>0)
	   sel = worksheet->selection(worksheet->currentSelection());
	   else
	   sel = worksheet->selection(0);
	   */

	return sel;
}

bool Table::calculate()
{
	QList<QTableWidgetSelectionRange> sel = getSelection();
	QTableWidgetSelectionRange range;

	bool success = true;
	if(sel.count()==0)
			if (!calculate(worksheet->currentColumn(), worksheet->currentRow(), worksheet->currentRow()))
				success = false;

	foreach(range, sel)
		for (int col=range.leftColumn(); col<=range.rightColumn(); col++)
			if (!calculate(col, range.topRow(), range.bottomRow()))
				success = false;
	return success;
}

QString Table::saveCommands()
{
	QString s = "<com>\n";
	for (int col=0; col<tableCols(); col++)
		if (!commands[col].isEmpty())
		{
			s += "<col nr=\"" + QString::number(col) + "\">\n";
			s += commands[col];
			s += "\n</col>\n";
		}
	s += "</com>\n";
	return s;
}

QString Table::saveComments()
{
	QString s = "Comments\t";
	for (int i=0; i<worksheet->columnCount(); i++)
		s+= comments[i] + "\t";
	return s + "\n";
}

QString Table::saveToString(const QString& geometry)
{
	QString s = "<table>\n";
	s += QString(name())+"\t";
	s += QString::number(worksheet->rowCount())+"\t";
	s += QString::number(worksheet->columnCount())+"\t";
	s += birthDate()+"\n";
	s += geometry;
	s += saveHeader();
	s += saveColumnWidths();
	s += saveCommands();
	s += saveColumnTypes();
	s += saveComments();
	s += "WindowLabel\t" + windowLabel() + "\t" + QString::number(captionPolicy()) + "\n";
	s += saveText();
	return s += "</table>\n";
}

QString Table::saveHeader()
{
	QString s="header\t";
	for (int j=0; j<worksheet->columnCount(); j++)
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
	for (int j=0; j<worksheet->columnCount(); j++)
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
	for (int i=selectedCol; i<worksheet->columnCount(); i++)
	{
		QString newLabel = oldLabel+QString::number(n);
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
	int cols=worksheet->columnCount();
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

void Table::changeColName(const QString& newText)
{
	QString caption = this->name();
	QString oldName = colName(selectedCol);
	QString newName = caption+"_"+newText;

	if (oldName == newName)
		return;

	if (caption == newText)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),
				tr("The column name must be different from the table name : <b>"+caption+"</b></b>!<p>Please choose another name!"));
		return;	
	}

	QStringList labels=colNames();
	if (labels.contains(newText)>0)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),
				tr("There is already a column called : <b>"+newText+"</b> in table <b>"+caption+"</b>!<p>Please choose another name!"));
		return;	
	}

	commands.gres("col(\""+colLabel(selectedCol)+"\")", "col(\""+newText+"\")", true);	

	setColName(selectedCol, newText);	
	emit changedColHeader(oldName, newName);
	emit modifiedWindow(this);
}

void Table::setColName(int col, const QString& newText)
{
	if (newText.isEmpty() || col<0 || col >=worksheet->columnCount())
		return; 

	col_label[col] = newText;
	setHeaderColType();
}

QStringList Table::selectedColumns()
{
	QStringList names;
	for (int i=0;i<worksheet->columnCount();i++)
	{
		if(isColumnSelected(i,true))
			names<<QString(name())+"_"+col_label[i];
	}
	return names;
}

QStringList Table::YColumns()
{
	QStringList names;
	for (int i=0;i<worksheet->columnCount();i++)
	{
		if(col_plot_type[i] == Y)
			names<<QString(name())+"_"+col_label[i];
	}
	return names;
}

QStringList Table::selectedYColumns()
{
	QStringList names;
	for (int i=0;i<worksheet->columnCount();i++)
	{
		if(isColumnSelected(i, true) && 
				(col_plot_type[i] == Y || col_plot_type[i] == yErr || col_plot_type[i] == xErr))
			names<<QString(name())+"_"+col_label[i];
	}
	return names;
}

QStringList Table::selectedYLabels()
{
	QStringList names;
	for (int i=0;i<worksheet->columnCount();i++)
	{
		if(isColumnSelected(i,true) && col_plot_type[i] == Y)
			names<<col_label[i];
	}
	return names;
}

QStringList Table::columnsList()
{
	QStringList names;
	for (int i=0;i<worksheet->columnCount();i++)
		names<<QString(name())+"_"+col_label[i];

	return names;
}

int Table::firstSelectedColumn()
{
	for (int i=0;i<worksheet->columnCount();i++)
	{
		if(isColumnSelected(i,true))
			return i;
	}
	return -1;
}

int Table::selectedRows()
{
	int r=0;
	for (int i=0;i<worksheet->rowCount();i++)
	{
		if(isRowSelected(i,true))
			r++;
	}
	return r;
}

int Table::selectedColsNumber()
{
	int c=0;
	for (int i=0;i<worksheet->columnCount(); i++)
	{
		if(isColumnSelected(i,true))
			c++;
	}
	return c;
}

QString Table::colName(int col)
{//returns the table name + horizontal header text
	if (col<0 || col >=worksheet->columnCount())
		return QString();

	return QString(this->name())+"_"+col_label[col];
}

QVarLengthArray<double> Table::col(int ycol)
{
	int i;
	int rows=worksheet->rowCount();
	int cols=worksheet->columnCount();
	QVarLengthArray<double> Y(rows);
	if (ycol<=cols)
	{
		for (i=0;i<rows;i++)
			Y[i]=text(i,ycol).toDouble();
	}
	return Y;
}

void Table::insertCols(int start, int count)
{	
	int i, index, max=0, xcols = 0;
	int cols=worksheet->columnCount();
	for (i=0; i<cols; i++)
	{
		if (col_label[i].contains(QRegExp ("\\D"))==0)
		{
			index=col_label[i].toInt();
			if (index>max) 
				max=index;
		}
		if (col_plot_type[i] == X)
			xcols++;
	}
	max++;

	for(i=start+1 ; i<(count+start+1) ; i++ );
	{
		commands.insert(i, "col(" +col_label[0]+")");
		col_format.insert(i, "0/6");
		comments.insert(i, QString());
		col_label.insert(i, QString());
		colTypes.insert(i, Numeric);
		col_plot_type.insert(i, Y);
	}

	for(i=0; i<count; i++)
		worksheet->insertColumn(start);

//	QHeaderView *head = worksheet->horizontalHeader();
	for (i=0; i<count; i++)
	{
		int col = start+i;
		col_label[col] = QString::number(max+i);
		if (xcols>1)
			worksheet->model()->setHeaderData(col, Qt::Horizontal, col_label[col]+"[Y"+QString::number(xcols)+"]");
		else
			worksheet->model()->setHeaderData(col, Qt::Horizontal, col_label[col]+"[Y]");
	}
}

void Table::insertCol()
{
	insertCols(selectedCol,1);
	emit modifiedWindow(this);
}

void Table::insertRow()
{
	int cr = worksheet->currentRow();
	if (isRowSelected(cr, true))
	{
		worksheet->insertRow(cr);
		emit modifiedWindow(this);
	}
}

void Table::addCol(PlotDesignation pd)
{
	worksheet->clearSelection();
	int index, max=0, cols=worksheet->columnCount();
	for (int i=0; i<cols; i++)
	{
		if (!col_label[i].contains(QRegExp ("\\D")))
		{
			index = col_label[i].toInt();
			if (index > max) 
				max = index;
		}
	}
	worksheet->insertColumn(cols);
	worksheet->scrollToItem( worksheet->item(0, cols) );

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
	int i, index,max=0, cols=worksheet->columnCount();
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
	for(i=0; i<c; i++)
		worksheet->insertColumn(cols);
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
	int rows=worksheet->rowCount();
	for (int i=0;i<rows;i++)
	{
		setText(i,selectedCol,"");
	}
	QString name=colName(selectedCol);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

void Table::clearCell(int row, int col)
{
	setText(row, col,"");

	QString name=colName(col);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

int Table::atRow(int col, double value)
{
	if (colTypes[col] == Numeric)
	{
		int row = -1;
		for (int i=0; i<worksheet->rowCount(); i++)
		{
			if (text(i,col).toDouble() == value)
				return i;
		}
		return row;
	}
	else
		return  -1;
}

void Table::deleteSelectedRows()
{
	QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	while( it.hasNext() )
	{
		int top = cur.topRow();
		int bottom = cur.bottomRow();
		int numberOfRows = bottom-top+1;
		QVarLengthArray<int> rowsToDelete(numberOfRows);
		for (int i=0; i<numberOfRows; i++)
			worksheet->removeRow(top+i);
		notifyChanges();
	}
}

void Table::cutSelection()
{
	copySelection();
	clearSelection();
}

bool Table::multipleRowsSelected()
{
	// TODO: This only returns true if multiple rows are selected in a contiguous block.
	// Is this intended?
	QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if( !it.hasNext() )
		return false;
	else if( sel.count() > 1 )
		return false;  
	else
	{
		cur = it.next();
		int top = cur.topRow();
		int bottom = cur.bottomRow();

		if (top == bottom)
			return false;

		for (int i=top; i<=bottom; i++)
		{
			if (!isRowSelected(i, true))
				return false;
		}
	}
	return true;
}

bool Table::singleRowSelected()
{
	QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if( !it.hasNext() )
		return false;

	if( sel.count() > 1)
		return false;
	
	cur = it.next();

	int top = cur.topRow();	

	if ( top == cur.bottomRow() && isRowSelected(top, true) )
		return true;

	return false;
}

void Table::selectAllTable()
{	
	worksheet->clearSelection();
	for(int i=0; i<worksheet->columnCount(); i++)
		worksheet->selectColumn(i);
}

void Table::deselect()
{	
	worksheet->clearSelection();
}

void Table::clearSelection()
{	
	QStringList list = selectedColumns();
	int i, n = list.count();

	if (n>0)
	{
		for (i=0;i<n;i++)
		{
			QString name = list[i];
			int id = colIndex(name);
			selectedCol = id;
			clearCol();
		}
	}
	else
	{
		QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
		QListIterator<QTableWidgetSelectionRange> it(sel);
		QTableWidgetSelectionRange cur;

		if (sel.isEmpty())
		{
			int row = worksheet->currentRow();
			int col = worksheet->currentColumn();
			setText(row, col,"");

			QString name = colName(col);
			emit modifiedData(this, name);		
		}
		else while( it.hasNext() )
		{
			cur = it.next();
			int top = cur.topRow();
			int bottom = cur.bottomRow();
			int left = cur.leftColumn();
			int right = cur.rightColumn();

			for (i=top;i<=bottom;i++)
			{
				for (int j=left;j<=right;j++)
				{
					setText(i, j,"");
				}
			}

			for (i=left;i<=right;i++)
			{
				QString name = colName(i);
				emit modifiedData(this, name);
			}
		}
	}
	emit modifiedWindow(this);
}

void Table::copySelection()
{
	QString theText;
	int i,j;
	int rows=worksheet->rowCount();
	int cols=worksheet->columnCount();

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
				theText += text(i,selection[j])+"\t";
			theText += text(i,selection[c-1])+"\n";
		}	
	}
	else
	{
		QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
		if (sel.isEmpty())
			theText = text(worksheet->currentRow(),worksheet->currentColumn());
		else
		{
			QListIterator<QTableWidgetSelectionRange> it(sel);
			QTableWidgetSelectionRange cur;

			int top = cur.topRow();
			int bottom = cur.bottomRow();
			int left = cur.leftColumn();
			int right = cur.rightColumn();
			for (i=top; i<=bottom; i++)
			{
				for (j=left; j<right; j++)
					theText += text(i,j)+"\t";
				theText += text(i,right)+"\n";
			}
		}
	}		

	// Copy text into the clipboard
	QApplication::clipboard()->setText(theText);
}

// Paste text from the clipboard
void Table::pasteSelection()
{	
	QString theText = QApplication::clipboard()->text();		
	if (theText.isEmpty())
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QTextStream ts( &theText, QIODevice::ReadOnly );
	QString s = ts.readLine(); 
	QStringList cellTexts = s.split("\t");
	int cols = int(cellTexts.count());
	int rows = 1;
	while(!ts.atEnd()) 
	{
		rows++;
		s = ts.readLine(); 
	}
	ts.reset();

	int i, j, top, bottom, right, left, firstCol = firstSelectedColumn();
	
	QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if (!sel.isEmpty())
	{// not columns but only cells are selected
		cur = it.next();
		top = cur.topRow();
		bottom = cur.bottomRow();
		left = cur.leftColumn();
		right = cur.rightColumn();
	}
	else
	{
		if(cols==1 && rows==1)
		{
			top = bottom = worksheet->currentRow();
			left = right = worksheet->currentColumn();
		}
		else
		{
			top = 0;
			bottom = worksheet->rowCount() - 1;	
			left=0;
			right=worksheet->columnCount() - 1;
			if (firstCol>=0)
			{// columns are selected
				left = firstCol;
				right = firstCol+selectedColsNumber()-1;
			}
		}
	}

	QTextStream ts2( &theText, QIODevice::ReadOnly );	
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
					int numRows = rows -r +1;
					if (firstCol >= 0)
						numRows = rows -r;

					for(int i=0; i<numRows; i++)
						worksheet->insertRow(top);
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
		cellTexts = s.split("\t");
		for (j=left; j<left+cols; j++)					
		{
			value = cellTexts[j-left].toDouble(&numeric);
			if (numeric)
			{
				columnNumericFormat(j, f, prec);
				setText(i, j,QString::number(value, f, prec));
			}
			else
				setText(i, j,cellTexts[j-left]);
		}
	}

	for (i=left; i<left+cols; i++)
		emit modifiedData(this, colName(i));

	emit modifiedWindow(this);
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
	int i, rows=worksheet->rowCount();
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
			tmpdata[i]=text(i, col1).toDouble();
			tmpdata2[i]=text(i, col2).toDouble();
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

	int cols=worksheet->columnCount();
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

		setText(i, cols,QString::number(i - n));
		setText(i, cols2,QString::number(y));
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

void Table::normalizeTable()
{
	for (int i=0; i<worksheet->columnCount(); i++)
	{
		selectedCol = i;
		normalizeCol();
	}
	emit modifiedWindow(this);	
}

void Table::normalizeCol(int col)
{
	if (col<0) col = selectedCol;
	double max=text(0,col).toDouble();
	double aux=0.0;
	int i;
	int rows=worksheet->rowCount();
	for (i=0; i<rows; i++)
	{
		QString theText=this->text(i,col);
		aux=theText.toDouble();
		if (!theText.isEmpty() && fabs(aux)>fabs(max)) 
			max=aux;
	}

	if (max == 1.0)
		return;

	for (i=0;i<rows;i++)
	{
		QString theText=this->text(i,col);
		aux=theText.toDouble();
		if ( !theText.isEmpty() )
			setText(i,col,QString::number(aux/max));
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
			this,SLOT(sortTable(int, int, const QString&)));
	sortd->insertColumnsList(colNames());
	sortd->exec();
}

void Table::sortTable(int type, int order, const QString& leadCol)
{
	sortColumns(colNames(), type, order, leadCol);
}

void Table::sortColumns(int type, int order,const QString& leadCol)
{
	sortColumns(selectedColumns(), type, order, leadCol);
}

void Table::sortColumns(const QStringList&s, int type, int order, const QString& leadCol)
{
	int cols=s.count();
	if(!type)
	{
		for(int i=0;i<cols;i++)
		{
			selectedCol=colIndex(s[i]);
			if(!order)
				sortColAsc();
			else
				sortColDesc();
		}
	}
	else
	{
		int i,j, leadcol=colIndex(leadCol);
		int rows=worksheet->rowCount();
		QVarLengthArray<double> r(rows), rtemp(rows);
		// Find the permutation index for the lead col
		size_t *p= new size_t[rows];
		for (j = 0; j <rows; j++)
			r[j]=this->text(j,leadcol).toDouble();

		gsl_sort_index(p,r.data(),1,rows);
		// Since we have the permutation index, sort all the columns
		for(i=0;i<cols;i++)
		{
			int scol=colIndex(s[i]);
			if (!isEmptyColumn(scol))
			{
				for (j = 0; j <rows; j++)
					r[j]=this->text(j,scol).toDouble();

				for (j=0;j<rows;j++)
				{
					int aux=p[j];
					rtemp[j]=r[aux];
				}
				for (j=0;j<rows;j++)
					r[j]=rtemp[j];

				int prec;
				char f;
				columnNumericFormat(scol, f, prec);
				if(!order)
				{
					for (j=0;j<rows;j++)
						setText(j,scol,QString::number(r[j], f, prec)); 
				}
				else
				{
					for (j=0;j<rows;j++)
						setText(j,scol,QString::number(r[rows-j-1], f, prec)); 
				}
				emit modifiedData(this, colName(scol));
			}
		}
		delete[] p;
	}
	emit modifiedWindow(this);	
}

void Table::sortColAsc()
{
	//changed from version 0.5.9
	int rows=worksheet->rowCount();
	QVarLengthArray<int> aux(rows);
	QVarLengthArray<double> r(rows);
	int i,n=0;
	for (i = 0; i <rows; i++)
	{
		QString theText=this->text(i,selectedCol);
		if (!theText.isEmpty())
		{
			n++;
			aux[i]=i;
			r[i]=theText.toDouble();
		}
		else
		{
			aux[i]=-1;
			r[i]=0.0;
		}
	}

	if (!n)
		return;

	gsl_vector * v = gsl_vector_alloc (n);
	int index;
	n=0;
	for (i = 0; i <rows; i++)
	{
		index=aux[i];
		if (index>=0)
		{
			gsl_vector_set (v, n, r[index]);
			n++;
		}	
	}

	int prec;
	char f;
	columnNumericFormat(selectedCol, f, prec);

	gsl_sort_vector (v);
	n=0;
	for (i=0;i<rows;i++)
	{
		index=aux[i];
		if (index>=0)
		{
			setText(i,selectedCol,QString::number(gsl_vector_get (v, n), f, prec)); 
			n++;
		}
	}
	gsl_vector_free (v);
	QString name=colName(selectedCol);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

void Table::sortColDesc()
{//changed from version 0.5.9
	int rows=worksheet->rowCount();
	QVarLengthArray<int> aux(rows);
	QVarLengthArray<double> r(rows);
	int i,n=0;
	for (i = 0; i <rows; i++)
	{
		QString theText=this->text(i,selectedCol);
		if (!theText.isEmpty())
		{
			n++;
			aux[i]=i;
			r[i]=theText.toDouble();
		}
		else
		{
			aux[i]=-1;
			r[i]=0.0;
		}
	}

	if (!n)
		return;

	gsl_vector * v = gsl_vector_alloc (n);
	int index;
	n=0;
	for (i = 0; i <rows; i++)
	{
		index=aux[i];
		if (index>=0)
		{
			gsl_vector_set (v, n, r[index]);
			n++;
		}	
	}

	gsl_sort_vector (v);

	int prec;
	char f;
	columnNumericFormat(selectedCol, f, prec);
	for (i=0;i<rows;i++)
	{
		index=aux[i];
		if (index>=0)
		{
			n--;
			setText(i,selectedCol,QString::number(gsl_vector_get (v, n), f, prec)); 
		}
	}
	gsl_vector_free (v);
	QString name=colName(selectedCol);
	emit modifiedData(this, name);
	emit modifiedWindow(this);
}

int Table::tableRows()
{
	return worksheet->rowCount();
}

int Table::tableCols()
{
	return worksheet->columnCount();
}

bool Table::isEmptyRow(int row)
{
	bool empty=true;
	int cols=worksheet->columnCount();
	for (int i=0;i<cols;i++)
	{
		QString theText = text(row,i);
		if (!theText.isEmpty())
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
	int rows=worksheet->rowCount();
	for (int i=0;i<rows;i++)
	{
		QString theText = text(i,col);
		if (!theText.isEmpty())
		{
			empty=false;
			break;
		}
	}	
	return empty;
}

QString Table::saveText()
{
	QString theText = "<data>\n";
	int cols = worksheet->columnCount();
	int rows = worksheet->rowCount();

	for (int i=0; i<rows; i++)
	{
		if (!isEmptyRow(i))
		{
			theText += QString::number(i)+"\t";
			for (int j=0; j<cols-1; j++)
				theText += text(i,j)+"\t";

			theText += text(i,cols-1)+"\n";
		}
	}	
	return theText + "</data>\n";
}

int Table::nonEmptyRows()
{
	int r=0; 	
	for (int i=0;i<worksheet->rowCount();i++)
	{
		if (!isEmptyRow(i))
			r++;
	}	
	return r;
}

QString Table::text(int row, int col)
{
	// TODO: the decimal point character should not be hardcoded
	if (col == savedCol)
		return savedCells[row].replace(",",".");
	else if(worksheet->item(row,col))
		return worksheet->item(row,col)->text().replace(",", ".");
	else
		return QString();
}

void Table::setText (int row, int col, const QString & newText )
{
	if(worksheet->item(row, col))
		worksheet->item(row, col)->setText(newText); 
	else
	{
		QTableWidgetItem * newItem = new QTableWidgetItem(newText);
		worksheet->setItem(row, col, newItem);
	}
}

void Table::saveColToMemory(int col)
{
	int rows=worksheet->rowCount();
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

void Table::setTextFormat(bool applyToAll)
{
	if (applyToAll)
	{
		for (int i=selectedCol; i<worksheet->columnCount(); i++)
			colTypes[i] = Text;
	}
	else
		colTypes[selectedCol] = Text;
}

void Table::setNumericFormat(int f, int prec, bool applyToAll)
{
	int cols=worksheet->columnCount();
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

	int rows=worksheet->rowCount();
	for (int i=0;i<rows;i++)
	{
		QString t = text(i, col);
		if (!t.isEmpty())
		{
			double val = t.toDouble();
			if (!f) 
				t.setNum(val, 'g', 6);
			else if (f==1)
				t.setNum(val,'f',prec);
			else if (f==2)
				t.setNum(val,'e',prec);

			setText(i, col, t);
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

	int cols=worksheet->columnCount();
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
	int rows=worksheet->rowCount();

	for (int i=0; i<rows; i++)
	{
		QString s = text(i,col);
		if (!s.isEmpty())
		{
			QDate d = QDate::fromString (s, Qt::ISODate);
			if (d.isValid())
				setText(i, col, d.toString(format));
			else
				setText(i, col, "-");
		}
	}
}

void Table::setTimeFormat(const QString& format, int col)
{
	if (col_format[col] == format)
		return;

	int rows=worksheet->rowCount();

	colTypes[col] = Time;
	col_format[col] = format;

	for (int i=0; i<rows; i++)
	{
		QString s = text(i,col);
		if (!s.isEmpty())
		{
			QTime t = QTime::fromString (s,Qt::TextDate);
			if (t.isValid())
				setText(i, col,t.toString(format));
			else
				setText(i, col,"-");
		}
	}
}

void Table::setMonthFormat(const QString& format, int col)
{
	colTypes[col] = Month;
	int rows=worksheet->rowCount();
	if (format == "shortMonthName")
	{
		for (int i=0;i<rows; i++)
		{
			QString s = text(i,col);
			if (!s.isEmpty())
			{
				int month= s.toInt() % 12;
				if (!month)
					month = 12;

				setText(i, col,QDate::shortMonthName(month));
			}
		}
	}
	else if (format == "longMonthName")
	{
		for (int i=0;i<rows; i++)
		{
			QString t = text(i,col);
			if (!t.isEmpty())
			{
				int month= t.toInt() % 12;
				if (!month)
					month = 12;

				setText(i, col,QDate::longMonthName(month));
			}
		}
	}
}

void Table::setDayFormat(const QString& format, int col)
{
	colTypes[col] = Day;
	int rows=worksheet->rowCount();
	if (format == "shortDayName")
	{
		for (int i=0;i<rows; i++)
		{
			QString t = text(i,col);
			if (!t.isEmpty())
			{
				int day= t.toInt() % 7;
				if (!day)
					day = 7;

				setText(i, col,QDate::shortDayName(day));
			}
		}
	}
	else if (format == "longDayName")
	{
		for (int i=0;i<rows; i++)
		{
			QString t = text(i,col);
			if (!t.isEmpty())
			{
				int day= t.toInt() % 7;
				if (!day)
					day = 7;

				setText(i, col,QDate::longDayName(day));
			}
		}
	}
}

void Table::setRandomValues()
{	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int i;
	double max=0.0;
	int rows=worksheet->rowCount();
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
			setText(i,selectedCol,QString::number(r[i], f, prec));	
		}

		emit modifiedData(this, name);
	}

	emit modifiedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Table::loadHeader(QStringList header)
{
	for (int i=0;i<(int)worksheet->columnCount();i++) 
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
	// FIXME: This method does not work, still need to figure out how to change the header data
	int xcols=0;
	for (int j=0;j<(int)worksheet->columnCount();j++) 
	{
		if (col_plot_type[j] == X)
			xcols++;
	}

	if (xcols>1)
	{
		xcols = 0;
		for (int i=0;i<worksheet->columnCount();i++) 
		{
			if (col_plot_type[i] == X)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[X" + QString::number(++xcols) +"]", Qt::DisplayRole);
			else if (col_plot_type[i] == Y)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[Y"+ QString::number(xcols) +"]");
			else if (col_plot_type[i] == Z)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[Z"+ QString::number(xcols) +"]");
			else if (col_plot_type[i] == xErr)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[xEr]");
			else if (col_plot_type[i] == yErr)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[yEr]");
			else
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]);
		}
	}
	else
	{
		for (int i=0;i<worksheet->columnCount();i++) 
		{
			if (col_plot_type[i] == X)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[X]", Qt::DisplayRole);
			else if (col_plot_type[i] == Y)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[Y]");
			else if (col_plot_type[i] == Z)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[Z]");
			else if (col_plot_type[i] == xErr)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[xEr]");
			else if (col_plot_type[i] == yErr)
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]+"[yEr]");
			else
				worksheet->model()->setHeaderData(i, Qt::Horizontal, col_label[i]);
		}
	}
}

void Table::setAscValues()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows=worksheet->rowCount();
	QStringList list=selectedColumns();

	for (int j=0;j<(int) list.count(); j++)
	{
		QString name=list[j];
		selectedCol=colIndex(name);

		int prec;
		char f;
		columnNumericFormat(selectedCol, f, prec);

		for (int i=0;i<rows;i++) 
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

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this, s,Graph::Line);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

void Table::plotP()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this, s,Graph::Scatter);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

void Table::plotLP()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this, s, Graph::LineSymbols);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"),tr("Please select a Y column to plot!"));
}

void Table::plotVB()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this, s,Graph::VerticalBars);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

void Table::plotHB()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this, s,Graph::HorizontalBars);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

void Table::plotArea()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this, s,Graph::Area);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

bool Table::noXColumn()
{
	bool notSet = true;
	for (int i=0; i<worksheet->columnCount(); i++)
	{
		if (col_plot_type[i] == X)
			return false;
	}
	return notSet;
}

bool Table::noYColumn()
{
	bool notSet = true;
	for (int i=0; i<worksheet->columnCount(); i++)
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

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this,s,Graph::VerticalDropLines);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

void Table::plotSpline()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this,s,Graph::Spline);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

void Table::plotVertSteps()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this, s, Graph::VerticalSteps);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

void Table::plotHorSteps()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this, s, Graph::HorizontalSteps);	
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

void Table::plotHistogram()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this,s,Graph::Histogram);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
}

void Table::plotBoxDiagram()
{
	if (!valid2DPlot())
		return;

	QStringList s=selectedYColumns();
	if (int(s.count())>0)
		emit plotCol(this, s, Graph::Box);
	else
		QMessageBox::warning(0,tr("QtiPlot - Error"), tr("Please select a Y column to plot!"));
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
	if (worksheet->columnCount()<2)
	{
		QMessageBox::critical(0,tr("QtiPlot - Error"),tr("You need at least two columns for this operation!"));
		return false;
	}

	if (noXColumn())
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
	if (worksheet->columnCount()<2)
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
		int r = worksheet->rowCount();
		int c = worksheet->columnCount(); 
		for (i=0; i<ignoredLines; i++)
			t.readLine();

		QString s = t.readLine();//read first line after the ignored ones
		while ( !t.atEnd() ) 
		{
			t.readLine(); 
			rows++;
		}

		if (simplifySpaces)
			s = s.simplifyWhiteSpace();
		else if (stripSpaces)
			s = s.trimmed();
		QStringList line = QStringList::split(sep, s, true);
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

		QProgressDialog progress(0, "progress", true, Qt::WindowStaysOnTopHint|Qt::Tool);
		progress.setWindowTitle("Qtiplot - Reading file...");
		progress.setLabelText(fname);
		progress.setActiveWindow();
		progress.setAutoClose(true);
		progress.setAutoReset(true);

		int steps = int(rows/1000);
		progress.setRange(0,steps+1);

		QApplication::restoreOverrideCursor();

		if (!importFileAs)
			init (rows, cols);
		else if (importFileAs == 1)
		{//new cols
			addColumns(cols);
			if (r < rows)
				worksheet->setRowCount(rows);
		}
		else if (importFileAs == 2)
		{//new rows
			if (c < cols)
				addColumns(cols-c);			
			worksheet->setRowCount(r+rows);
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
				s = s.trimmed();

			line = QStringList::split(sep, s, false);	
			int end = startCol+(int)line.count();
			for (i=startCol; i<end; i++)
			{
				comments[i] = line[i-startCol];
				s = line[i-startCol].remove(QRegExp("\\W")).replace("_","-");
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
					s = s.trimmed();
				line = QStringList::split(sep, s, true);
				for (int j=startCol; j<worksheet->columnCount(); j++)
					setText(startRow + k, j, line[j-startCol]);
			}

			startRow+= 1000;
			progress.setValue(i);
		}

		for (i=startRow; i<worksheet->rowCount(); i++)
		{
			s = t.readLine(); 
			if (simplifySpaces)
				s = s.simplifyWhiteSpace();
			else if (stripSpaces)
				s = s.trimmed();
			line = QStringList::split(sep, s, true);
			for (int j=startCol; j<worksheet->columnCount(); j++)
				setText(i, j, line[j-startCol]);
		}
		progress.setValue(steps+1);
		f.close();

		if (importFileAs)
		{
			for (i=0; i<worksheet->columnCount(); i++)
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
			s = s.trimmed();

		QStringList line = QStringList::split(sep, s, true);
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

		QProgressDialog progress(0, "progress", true, Qt::WindowStaysOnTopHint|Qt::Tool);
		progress.setWindowTitle("Qtiplot - Reading file...");
		progress.setLabelText(fname);
		progress.setActiveWindow();
		progress.setAutoClose(true);
		progress.setAutoReset(true);
		progress.setRange(0,steps+1);

		QApplication::restoreOverrideCursor();

		QStringList oldHeader;	
		if (newTable)
			init (rows, cols);
		else
		{
			if (worksheet->rowCount() != rows)
				worksheet->setRowCount(rows);

			c = worksheet->columnCount();
			oldHeader = col_label;
			if (c != cols)
			{
				if (c < cols)
					addColumns(cols - c);
				else 
				{
					worksheet->setColumnCount(cols);
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
				s = s.trimmed();
			line = QStringList::split(sep, s, false);	
			for (i=0; i<(int)line.count(); i++)
			{
				comments[i] = line[i];
				s = line[i].remove(QRegExp("\\W")).replace("_","-");
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
					s = s.trimmed();
				line = QStringList::split(sep, s, true);
				int lc = (int)line.count();
				if (lc > cols) {
					addColumns(lc - cols);
					cols = lc;
				}
				for (int j=0; j<cols; j++)
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
				s = s.simplifyWhiteSpace();
			else if (stripSpaces)
				s = s.trimmed();
			line = QStringList::split(sep, s, true);
			int lc = (int)line.count();
			if (lc > cols) {
				addColumns(lc - cols);
				cols = lc;
			}
			for (int j=0; j<cols; j++)
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

	QString theText;
	int i,j;
	int rows=worksheet->rowCount();
	int cols=worksheet->columnCount();
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
					theText+=header[sCols[i]]+separator;
				else
					theText+="C"+header[sCols[i]]+separator;
			}

			if (ls.count()>0)
				theText+=header[sCols[selectedCols]] + "\n";
			else
				theText+="C"+header[sCols[selectedCols]] + "\n";
		}
		else
		{
			if (ls.count()>0)
			{
				for (j=0; j<cols-1; j++) 
					theText+=header[j]+separator;
				theText+=header[cols-1]+"\n";	
			}			
			else
			{
				for (j=0; j<cols-1; j++) 
					theText+="C"+header[j]+separator;
				theText+="C"+header[cols-1]+"\n";	
			}	
		} 
	}// finished writting labels

	if (exportSelection)
	{
		for (i=topRow;i<=bottomRow; i++)
		{
			for (j=1;j<selectedCols;j++)
				theText += text(i, sCols[j]) + separator;			
			theText += text(i, sCols[selectedCols]) + "\n";
		}
		delete[] sCols;//free memory
	}
	else
	{
		for (i=0;i<rows;i++)
		{
			for (j=0;j<cols-1;j++)
				theText += text(i,j)+separator;
			theText += text(i,cols-1)+"\n";
		}
	}
	QTextStream t( &f );
	t << theText;
	f.close();
	return true;
}

void Table::contextMenuEvent(QContextMenuEvent *e)
{
	int w = 0;
	for (int i = 0; i < worksheet->columnCount(); i++)
		w += worksheet->columnWidth (i);

	if (e->pos().x() > w)
		emit showContextMenu(false);
	else
		emit showContextMenu(true);
	e->accept();
}

void Table::moveCurrentCell()
{
	int cols=worksheet->columnCount();
	int row=worksheet->currentRow();
	int col=worksheet->currentColumn();
	worksheet->clearSelection();

	if (col+1<cols)
	{
		worksheet->setCurrentCell (row,col+1);
		worksheet->setRangeSelected(QTableWidgetSelectionRange(row,col+1,row,col+1), true);
	}
	else
	{
		worksheet->setCurrentCell (row+1,0);
		worksheet->setRangeSelected(QTableWidgetSelectionRange(row+1,0,row+1,0), true);
	}
}

void Table::mouseMoveEvent ( QMouseEvent * e )
{
	QHeaderView *header = worksheet->horizontalHeader();
	int offset = header->offset();
	selectedCol = header->logicalIndexAt(e->pos().x() + offset);

	if(selectedCol != lastSelectedCol)
	{//This means that we are in the next column
		if(isColumnSelected(selectedCol,true))
		{//Since this column is selected, deselect it
			worksheet->setRangeSelected(QTableWidgetSelectionRange(0,lastSelectedCol,
						worksheet->rowCount()-1,lastSelectedCol), false );
		}
		else
			worksheet->selectColumn(selectedCol);
	}
	lastSelectedCol = selectedCol;
	worksheet->setCurrentCell(0, selectedCol);
}

void Table::mousePressEvent ( QMouseEvent * e )
{
	if (e->button() == Qt::LeftButton)	
	{				
		if (e->state ()==Qt::ControlButton)
		{
			QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
			QListIterator<QTableWidgetSelectionRange> it(sel);
			QTableWidgetSelectionRange cur = it.next();
			if (cur.topRow() != 0 || cur.bottomRow() != (worksheet->rowCount() - 1))
				//select only full columns
				worksheet->setRangeSelected(cur, false);						
		}
		else
			worksheet->clearSelection();

		QHeaderView *header = worksheet->horizontalHeader();
		int offset = header->offset();

		selectedCol=header->logicalIndexAt(e->pos().x()+offset);
		lastSelectedCol = selectedCol;
		worksheet->selectColumn(selectedCol);
		worksheet->setCurrentCell(0, selectedCol);
	}			
}

bool Table::headerDoubleClickedHandler(int logicalIndex)
{
	Q_UNUSED(logicalIndex);
	// remark: autoresize is done by Qt4 build-in code
	// so it does not need to be implemented in Table anymore
	emit optionsDialog();
	return true;
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
	int cols=worksheet->columnCount();
	int rows=worksheet->rowCount();

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
		worksheet->setRowCount(r);

	int c=list[2].toInt();
	if (cols != c)
		worksheet->setColumnCount(c);

	//clear all cells
	for (i=0;i<rows;i++)
	{
		for (j=0; j<c; j++)
			setText(i,j, "");
	} 

	t.readLine();	//table geometry useless info when restoring
	s = t.readLine();//header line

	list = QStringList::split ("\t",s,true);
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
	list = QStringList::split ("\t", s,true);
	list.remove(list.first());
	if (columnWidths() != list)
		setColWidths(list);

	s = t.readLine();
	list = QStringList::split ("\t", s,true);
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
	list = QStringList::split ("\t", s,true);
	if (s.contains ("ColType",true))
	{
		list.remove(list.first());	
		for (i=0; i<int(list.count()); i++)
		{
			QStringList l= QStringList::split(";", list[i], true);
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
	list = QStringList::split ("\t", s,true);
	if (s.contains ("Comments",true))
	{
		list.remove(list.first());
		comments = list;	
	}

	s= t.readLine();
	list = QStringList::split ("\t", s,true);
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
		list = QStringList::split ("\t", s,true);

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
	worksheet->setRowCount(rows);
}

void Table::setNumCols(int cols)
{
	worksheet->setColumnCount(cols);
}

void Table::resizeRows(int r)
{
	int rows = worksheet->rowCount();
	if (rows == r)
		return;

	if (rows > r)
	{
		QString theText= tr("Rows will be deleted from the table!");
		theText+="<p>"+tr("Do you really want to continue?");
		int i,cols = worksheet->columnCount();
		switch( QMessageBox::information(0,tr("QtiPlot"), theText, tr("Yes"), tr("Cancel"), 0, 1 ) ) 
		{
			case 0:
				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				worksheet->setRowCount(r);
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
		worksheet->setRowCount(r);
		QApplication::restoreOverrideCursor();
	}

	emit modifiedWindow(this);
}

void Table::resizeCols(int c)
{
	int i, cols = worksheet->columnCount();	
	if (cols == c)
		return;

	if (cols > c)
	{
		QString theText= tr("Columns will be deleted from the table!");
		theText+="<p>"+tr("Do you really want to continue?");
		QVarLengthArray<int> columns(cols-c);
		switch( QMessageBox::information(0,tr("QtiPlot"), theText, tr("Yes"), tr("Cancel"), 0, 1 ) ) 
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

				for (i=0; i<columns.count(); i++)
					worksheet->removeColumn(columns[i]);

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
	int i, rows = worksheet->rowCount();
	int m = 0;
	for (i=0;i<rows;i++) 
	{
		if (!text(i, col2).isEmpty())
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
		sig[i]=text(i, col1).toDouble();
	for (i=0;i<m;i++) 
		res[i]=text(i, col2).toDouble();

	convlv(sig, n, res, m, sign);

	int cols=worksheet->columnCount();
	int cols2 = cols+1;
	addCol();
	addCol();
	for (i = 0; i<rows; i++) 
	{
		setText(i, cols, QString::number(i+1));
		setText(i, cols2, QString::number(sig[i]));
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

void Table::fft(double sampling, const QString& realColName, const QString& imagColName,
		bool forward, bool normalize, bool order)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int i, i2;
	int rows = worksheet->rowCount();
	int n = 2*rows;
	double *dat = new double[n];
	double *amp = new double[rows];
	double *x = new double[rows];

	gsl_fft_complex_wavetable *wavetable = gsl_fft_complex_wavetable_alloc (rows);
	gsl_fft_complex_workspace *workspace = gsl_fft_complex_workspace_alloc (rows);

	int realCol = colIndex(realColName);
	int imCol = -1;
	if (!imagColName.isEmpty())
		imCol = colIndex(imagColName);

	if(dat && amp && x && wavetable && workspace) 
	{// zero-pad data array
		memset( dat, 0, n* sizeof( double ) );
		for(i=0;i<rows;i++) 
		{
			i2 = 2*i;
			dat[i2]=text(i, realCol).toDouble();
			if (imCol >= 0)
				dat[i2+1]=text(i, imCol).toDouble();
		}
	}
	else
	{
		QMessageBox::warning(0,"QtiPlot", tr("Could not allocate memory, operation aborted!"));
		return;
	}

	double df = 1.0/(double)(rows*sampling);//frequency sampling
	double aMax = 0.0;//max amplitude
	QString label, theText;
	if(forward)
	{
		label="ForwardFFT-"+QString(this->name());
		theText= tr("Frequency");
		gsl_fft_complex_forward (dat, 1, rows, wavetable, workspace);
	}
	else
	{
		label="InverseFFT-"+QString(this->name());
		theText= tr("Time");
		gsl_fft_complex_inverse (dat, 1, rows, wavetable, workspace);
	}

	gsl_fft_complex_wavetable_free (wavetable);
	gsl_fft_complex_workspace_free (workspace);

	if (order)
	{
		int n2 = rows/2;
		for(i=0;i<rows;i++)
		{
			x[i] = (i-n2)*df;
			int j = i + rows;
			double aux = dat[i];
			dat[i] = dat[j];
			dat[j] = aux;
		}
	}
	else
	{
		for(i=0;i<rows;i++)
			x[i] = i*df;
	}

	for(i=0;i<rows;i++)
	{
		i2 = 2*i;
		double real_part = dat[i2];
		double im_part = dat[i2+1];
		double a = sqrt(real_part*real_part + im_part*im_part);
		amp[i]= a;
		if (a > aMax)
			aMax = a;
	}
	theText+="\t"+tr("Real")+"\t"+tr("Imaginary")+"\t"+tr("Amplitude")+"\t"+tr("Angle")+"\n";
	for (i=0; i<rows; i++)
	{
		i2 = 2*i;
		theText+=QString::number(x[i])+"\t";
		theText+=QString::number(dat[i2])+"\t";
		theText+=QString::number(dat[i2+1])+"\t";
		if (normalize)
			theText+=QString::number(amp[i]/aMax)+"\t";
		else
			theText+=QString::number(amp[i])+"\t";
		theText+=QString::number(atan(dat[i2+1]/dat[i2]))+"\n";
	}

	delete[] x;
	delete[] amp;
	delete[] dat;

	emit createTable(label, rows, 5, theText);	
	QApplication::restoreOverrideCursor();	
}

void Table::copy(Table *m)
{
	for (int i=0; i<worksheet->rowCount(); i++)
	{
		for (int j=0; j<worksheet->columnCount(); j++)
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
	QString s = "<table>\t"+QString::number(worksheet->rowCount())+"\t";
	s += QString::number(worksheet->columnCount())+"\n";
	s += geometryInfo;
	s += saveHeader();
	s += saveColumnWidths();
	s += saveCommands();
	s += saveColumnTypes();
	s += saveComments();
	return s;
}

void Table::restore(const QStringList& lst)
{
	QStringList l;
	QStringList::const_iterator i = lst.begin();

	l = QStringList::split("\t", *i++, true);
	l.remove(l.first());
	loadHeader(l);

	setColWidths(QStringList::split ("\t",(*i).right((*i).length()-9), false ));
	i++;

	l = QStringList::split ("\t", *i++, true);
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

	l = QStringList::split ("\t", *i++, true);
	l.remove(l.first());
	setColumnTypes(l);

	l = QStringList::split ("\t", *i++, true);
	l.remove(l.first());
	setColComments(l);
}

void Table::notifyChanges()
{
	for (int i=0; i<worksheet->columnCount(); i++)
		emit modifiedData(this, colName(i));

	emit modifiedWindow(this);
}

Table::~Table()
{
}

bool Table::isRowSelected(int row, bool full) 
{ 
	QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if ( !full ) 
	{
		while ( it.hasNext() ) 
		{
			cur = it.next();
			if ( (row >= cur.topRow()) && (row <= cur.bottomRow() ) )
				return true;
		}
	} 
	else 
	{
		while ( it.hasNext() ) 
		{
			cur = it.next();
			if ( row >= cur.topRow() &&
					row <= cur.bottomRow() &&
					cur.leftColumn() == 0 &&
					cur.rightColumn() == worksheet->columnCount() - 1 )
				return true;
		}
	}
	return false;
}

bool Table::isColumnSelected(int col, bool full)
{ 
	QList<QTableWidgetSelectionRange> sel = worksheet->selectedRanges();
	QListIterator<QTableWidgetSelectionRange> it(sel);
	QTableWidgetSelectionRange cur;

	if ( !full ) 
	{
		while ( it.hasNext() ) 
		{
			cur = it.next();
			if ( (col >= cur.leftColumn()) && (col <= cur.rightColumn() ) )
				return true;
		}
	} 
	else 
	{
		while ( it.hasNext() ) 
		{
			cur = it.next();
			if ( col >= cur.leftColumn() &&
					col <= cur.rightColumn() &&
					cur.topRow() == 0 &&
					cur.bottomRow() == worksheet->rowCount() - 1 )
				return true;
		}
	}
	return false;
}

