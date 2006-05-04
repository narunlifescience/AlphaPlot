#include "worksheet.h"
#include "sortDialog.h"
#include "parser.h"
#include "nrutil.h"

#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qclipboard.h> 
#include <qapplication.h> 
#include <qcursor.h>
#include <qdatetime.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qaccel.h>
#include <qprogressdialog.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_complex.h>

Table::Table(const QString &fname,const QString &sep, int ignoredLines, bool renameCols,
			 bool stripSpaces, bool simplifySpaces, const QString& label, 
			 QWidget* parent, const char* name, WFlags f)
        : myWidget(label, parent,name,f)
{
importASCII(fname, sep, ignoredLines, renameCols, stripSpaces, simplifySpaces, true);
}

Table::Table(int r, int c, const QString& label, QWidget* parent, const char* name, WFlags f)
        : myWidget(label,parent,name,f)
{
init(r,c);	
}

void Table::init(int rows, int cols)
{
selectedCol=0;
LeftButton=FALSE;
	
QDateTime dt = QDateTime::currentDateTime ();
setBirthDate(dt.toString(Qt::LocalDate));

worksheet= new QTable (rows,cols,this,"table");
worksheet->setFocusPolicy(QWidget::StrongFocus);
worksheet->setFocus();
worksheet->setSelectionMode (QTable::Single);

QVBoxLayout* hlayout = new QVBoxLayout(this,0,0, "hlayout1");
hlayout->addWidget(worksheet);

for (int i=0; i<cols; i++)
	{
	commandes << "col(1)";
	colTypes << Numeric;
	col_format << "0/6";
	comments << "";
	col_label << QString::number(i+1);
	col_plot_type << Y;
	}

QHeader* head=(QHeader*)worksheet->horizontalHeader();
head->installEventFilter(this);
head->setMouseTracking(TRUE);

col_plot_type[0] = X;
setHeaderColType();

int w=4*(worksheet->horizontalHeader())->sectionSize(0);
int h;
if (rows>11)
	h=11*(worksheet->verticalHeader())->sectionSize(0);
else
	h=(rows+1)*(worksheet->verticalHeader())->sectionSize(0);
setGeometry(50,50,w + 45, h);

QAccel *accel = new QAccel(this);
accel->connectItem( accel->insertItem( Key_Tab ), this, SLOT(moveCurrentCell()));
accel->connectItem( accel->insertItem( CTRL+Key_A ), this, SLOT(selectAllTable()) );

connect(worksheet, SIGNAL(valueChanged(int,int)),this, SLOT(cellEdited(int,int)));
specifications = saveToString("geometry\n", birthDate());
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
        printer.setFullPage( TRUE );
        QPainter p;
        if ( !p.begin(&printer ) )
            return; // paint on printer
        QPaintDeviceMetrics metrics( p.device() );
        int dpiy = metrics.logicalDpiY();
        const int margin = (int) ( (1/2.54)*dpiy ); // 2 cm margins
		
		QHeader *hHeader = worksheet->horizontalHeader();
		QHeader *vHeader = worksheet->verticalHeader();

		int rows=worksheet->numRows();
		int cols=worksheet->numCols();
		int height=margin;
		int i,vertHeaderWidth=vHeader->width();
		int right = margin + vertHeaderWidth;
		
		// print header
		p.setFont(hHeader->font());
		QRect br=p.boundingRect(br,Qt::AlignCenter,	hHeader->label(0),-1,0);
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
			tr=p.boundingRect(tr,Qt::AlignCenter,text,-1,0);
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
				tr=p.boundingRect(tr,Qt::AlignCenter,text,-1,0);
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

void Table::cellEdited(int,int col)
{
QString name=colName(col);
emit modifiedData(name);
emit modifiedTable(this);
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

void Table::setXCol()
{
if (col_plot_type[selectedCol] == X)
	return;
	
col_plot_type[selectedCol] = X;
setHeaderColType();
emit modifiedTable(this);
}

void Table::disregardCol()
{
if (col_plot_type[selectedCol] == None)
	return;

col_plot_type[selectedCol] = None;
setHeaderColType();
emit modifiedTable(this);
}

void Table::setYCol()
{
if (col_plot_type[selectedCol] == Y)
	return;

col_plot_type[selectedCol] = Y;
setHeaderColType();
emit modifiedTable(this);
}

void Table::setZCol()
{
if (col_plot_type[selectedCol] == Z)
	return;

col_plot_type[selectedCol] = Z;
setHeaderColType();
emit modifiedTable(this);
}

void Table::setPlotDesignation(PlotDesignation pd)
{
QStringList list=selectedColumns();
for (int i=0;i<(int) list.count(); i++)
	col_plot_type[colIndex(list[i])] = pd;

setHeaderColType();
emit modifiedTable(this);
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
for (int i=0; i<(int)ctl.count(); i++)
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
QString s="ColWidth\t";
for (int i=0;i<worksheet->numCols();i++)
	s+=QString::number(worksheet->columnWidth (i))+"\t";
	
return s+"\n";
}

QString Table::saveColumnTypes()
{
QString s="ColType\t";
for (int i=0; i<worksheet->numCols(); i++)
	s+=QString::number(colTypes[i])+";"+col_format[i]+"\t";
	
return s+"\n";
}

QString Table::saveCommandes()
{
QString s="com\t";
s+=commandes.join ("\t")+"\n";
return s;
}

QString Table::saveComments()
{
QString s="Comments\t";
for (int i=0; i<worksheet->numCols(); i++)
	s+= comments[i] + "\t";
return s + "\n";
}

QString Table::saveToString(const QString& geometry, const QString& birth)
{
QString s="<table>\n";
s+=QString(name())+"\t";
s+=QString::number(worksheet->numRows())+"\t";
s+=QString::number(worksheet->numCols())+"\t";
s+=birth+"\n";
s+=geometry;
s+=saveHeader();
s+=saveColumnWidths();
s+=saveCommandes();
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

void Table::setCommandes(const QString& com)
{
QStringList list=QStringList::split ("\t",com,FALSE);
for (int i=0;i<int(list.count());i++)
	{
	commandes[i]=list[i+1];
	}
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
	commandes.gres("col("+colLabel(i)+")", "col("+newLabel+")", true);
	setColName(i, newLabel);
	emit changedColHeader(caption+oldLabels[i],colName(i));
	n++;
	}	
emit modifiedTable(this);
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
	
	emit modifiedTable(this);
	}
else
	{
	if (worksheet->columnWidth(selectedCol) == width)
		return;
	
	worksheet->setColumnWidth (selectedCol, width);
	emit modifiedTable(this);
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

commandes.gres("col("+colLabel(selectedCol)+")", "col("+text+")", true);	
	
setColName(selectedCol, text);	
emit changedColHeader(oldName, newName);
emit modifiedTable(this);
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
	if(worksheet->isColumnSelected (i,TRUE))
		return i;
	}
return -1;
}

int Table::selectedRows()
{
int r=0;
for (int i=0;i<worksheet->numRows();i++)
	{
	if(worksheet->isRowSelected (i,TRUE))
		r++;
	}
return r;
}

int Table::selectedColsNumber()
{
int c=0;
for (int i=0;i<worksheet->numCols(); i++)
	{
	if(worksheet->isColumnSelected (i,TRUE))
		c++;
	}
return c;
}

QString Table::colName(int col)
{//returns the table name + horizontal header text
if (col<0 || col >=worksheet->numCols())
	return QString::null;

return QString(this->name())+"_"+col_label[col];
}

QMemArray<double> Table::col(int ycol)
{
int i;
int rows=worksheet->numRows();
int cols=worksheet->numCols();
QMemArray<double> Y(rows);
if (ycol<=cols)
	{
	for (i=0;i<rows;i++)
		Y[i]=worksheet->text(i,ycol).toDouble();
	}
return Y;
}

void Table::insertCols(int start, int count)
{	
int i, index, max=0, xcols = 0;
int cols=worksheet->numCols();
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
	
QStringList::Iterator it=commandes.at(++start);
commandes.insert(it++, count, "col(" +col_label[0]+")");
it=col_format.at(start);
col_format.insert(it++, count, "0/6");
it=comments.at(start);
comments.insert(it++, count, QString::null);
it=col_label.at(start);
col_label.insert(it++, count, QString::null);

QValueListIterator<int> itt=colTypes.at(start);
colTypes.insert(itt++, count, Numeric);
itt=col_plot_type.at(start);
col_plot_type.insert(itt++, count, Y);
	
worksheet->insertColumns (start, count);

QHeader *head = worksheet->horizontalHeader();
for (i=0; i<count; i++)
	{
	int col = start+i;
	col_label[col] = QString::number(max+i);
	if (xcols>1)
		head->setLabel(col,col_label[col]+"[Y"+QString::number(xcols)+"]", -1);
	else
		head->setLabel(col,col_label[col]+"[Y]",-1);
	}
}

void Table::insertCol()
{
insertCols(selectedCol,1);
emit modifiedTable(this);
}

void Table::insertRow()
{
int cr = worksheet->currentRow();
if (worksheet->isRowSelected (cr, true))
	{
	worksheet->insertRows(cr,1);
	emit modifiedTable(this);
	}
}

void Table::addCol()
{
worksheet->clearSelection();
int index,max=0, cols=worksheet->numCols(), xcols = 0;
for (int i=0; i<cols; i++)
	{
	if (!col_label[i].contains(QRegExp ("\\D")))
		{
		index=col_label[i].toInt();
		if (index>max) 
			max=index;
		}
	if (col_plot_type[i] == X)
		xcols++;
	}
worksheet->insertColumns(cols,1);
if (xcols>1)
	worksheet->horizontalHeader()->setLabel (cols,QString::number(max+1)+"[Y"+QString::number(xcols)+"]",-1);
else
	worksheet->horizontalHeader()->setLabel (cols,QString::number(max+1)+"[Y]",-1);

worksheet->ensureCellVisible ( 0, cols );
	
comments << QString::null;
commandes<<"col("+col_label[0]+")";
colTypes<<Numeric;
col_format<<"0/6";
col_label<< QString::number(max+1);
col_plot_type << Y;

emit modifiedTable(this);
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
	comments << QString::null;
	commandes<<"col("+col_label[0]+")";
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
emit modifiedData(name);
emit modifiedTable(this);
}

void Table::clearCell(int row, int col)
{
worksheet->setText(row, col, "");

QString name=colName(col);
emit modifiedData(name);
emit modifiedTable(this);
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
QTableSelection sel=worksheet->selection(worksheet->currentSelection());
int top=sel.topRow();
int bottom=sel.bottomRow();

int i;
for (i=top; i<=bottom; i++)
	worksheet->removeRow(top);

int cols=worksheet->numCols();
for (i=0; i<cols; i++)
	{
	QString name=colName(i);
	emit modifiedData(name);
	}
emit modifiedTable(this);
}

void Table::cutSelection()
{
copySelection();
clearSelection();
}

bool Table::singleCellSelected()
{
bool single=TRUE;
QTableSelection sel=worksheet->selection(worksheet->currentSelection());
int top=sel.topRow();
int bottom=sel.bottomRow();
int left=sel.leftCol();
int right=sel.rightCol();
	
if (top != bottom || left != right)
	single=FALSE;
return single;
}

bool Table::multipleRowsSelected()
{
bool selected = true;
QTableSelection sel=worksheet->selection(worksheet->currentSelection());
int top=sel.topRow();
int bottom=sel.bottomRow();
	
if (top == bottom)
	return FALSE;

for (int i=top; i<=bottom; i++)
	{
	if (!worksheet->isRowSelected (i, true))
		return FALSE;
	}
return selected;
}

bool Table::singleRowSelected()
{
QTableSelection sel=worksheet->selection(worksheet->currentSelection());
int top=sel.topRow();	
if (top == sel.bottomRow() && worksheet->isRowSelected (top, true))
	return true;

return false;
}

void Table::selectAllTable()
{	
worksheet->addSelection (QTableSelection( 0, 0, worksheet->numRows(), worksheet->numCols() ));
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
QTableSelection sel=worksheet->selection(worksheet->currentSelection());
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
	emit modifiedData(name);		
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
		emit modifiedData(name);
		}
	}
}
emit modifiedTable(this);
}

void Table::copySelection()
{
QString text;
int i,j;
int rows=worksheet->numRows();
int cols=worksheet->numCols();

QMemArray<int> selection(1);
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
	int selnum = worksheet->currentSelection();
	if (selnum==-1)
		text=worksheet->text(worksheet->currentRow(),worksheet->currentColumn());
	else
		{
		QTableSelection sel=worksheet->selection(selnum);
		int top=sel.topRow();
		int bottom=sel.bottomRow();
		int left=sel.leftCol();
		int right=sel.rightCol();
		for (i=top; i<=bottom; i++)
			{
			for (j=left; j<right; j++)
				text+=worksheet->text(i,j)+"\t";
			text+=worksheet->text(i,right)+"\n";
			}
		}
	}		
	
// Copy text into the clipboard
QApplication::clipboard()->setData(new QTextDrag(text,worksheet,0));
}

// Paste text from the clipboard
void Table::pasteSelection()
{	
QString text;		
if (!QTextDrag::decode(QApplication::clipboard()->data(), text) || text.isNull())
	return;

QApplication::setOverrideCursor(waitCursor);
	
QTextStream ts( &text, IO_ReadOnly );
QString s = ts.readLine(); 
QStringList cellTexts=QStringList::split ("\t", s, TRUE);
int cols=int(cellTexts.count());
int rows= 1;
while(!ts.atEnd()) 
	{
	rows++;
	s = ts.readLine(); 
	}
ts.reset();
	
int i, j, top, bottom, right, left, firstCol=firstSelectedColumn();
QTableSelection sel=worksheet->selection(worksheet->currentSelection());
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

QTextStream ts2( &text, IO_ReadOnly );	
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

QApplication::setOverrideCursor(waitCursor);	
for (i=top; i<top+rows; i++)
	{
	s = ts2.readLine();
	cellTexts=QStringList::split ("\t", s, TRUE);
	for (j=left; j<left+cols; j++)					
		worksheet->setText(i, j, cellTexts[j-left]);
	}
			
for (i=left; i<left+cols; i++)
	emit modifiedData(colName(i));

emit modifiedTable(this);
QApplication::restoreOverrideCursor();
}

void Table::removeCol()
{
QStringList list=selectedColumns();
removeCol(list);
}

void Table::removeCol(const QStringList& list)
{
QApplication::setOverrideCursor(waitCursor);

for (int i=0; i<(int) list.count(); i++)
	{
	QString name=list[i];
	int id=colIndex(name);
	if (id >= 0)
		{
		QStringList::Iterator it=commandes.at(id);
		if ( it != commandes.end() )
			commandes.remove (it);

		it=col_label.at(id);
		if ( it != col_label.end() )
			col_label.remove (it);
	
		it=col_format.at(id);
		if ( it != col_format.end() )
			col_format.remove (it);
	
		it=comments.at(id);
		if ( it != comments.end() )
			comments.remove (it);

		QValueListIterator<int> itt=colTypes.at(id);
		if ( itt != colTypes.end() )
			colTypes.remove (itt);
	
		itt=col_plot_type.at(id);
		if ( itt != col_plot_type.end() )
			col_plot_type.remove (itt);

		worksheet->removeColumn (id);
		}
	emit removedCol(name);
	}	
emit modifiedTable(this);
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

QApplication::setOverrideCursor(waitCursor);	
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
emit modifiedTable(this);
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
	
emit modifiedTable(this);	
}

void Table::normalizeTable()
{
for (int i=0; i<worksheet->numCols(); i++)
	{
	selectedCol = i;
	normalizeCol();
	}
emit modifiedTable(this);	
}

void Table::normalizeCol()
{
double max=worksheet->text(0,selectedCol).toDouble();
double aux=0.0;
int i;
int rows=worksheet->numRows();
for (i=0; i<rows; i++)
	{
	QString text=this->text(i,selectedCol);
	aux=text.toDouble();
	if (!text.isEmpty() && fabs(aux)>fabs(max)) 
		max=aux;
	}
	
if (max == 1.0)
	return;
	
for (i=0;i<rows;i++)
	{
	QString text=this->text(i,selectedCol);
	aux=text.toDouble();
	if ( !text.isEmpty() )
		worksheet->setText(i,selectedCol,QString::number(aux/max));
	}

QString name=colName(selectedCol);
emit modifiedData(name);
}

void Table::sortColumnsDialog()
{
QStringList s=selectedColumns();
sortDialog *sortd=new sortDialog(this,"sortDialog",TRUE,WStyle_Tool|WDestructiveClose);
connect (sortd,SIGNAL(sort(int, int, const QString&)),
		 this,SLOT(sortColumns(int, int, const QString&)));
sortd->insertColumnsList(s);
sortd->showNormal();
sortd->setActiveWindow();
}

void Table::sortTableDialog()
{
sortDialog *sortd=new sortDialog(this,"sortDialog",TRUE,WStyle_Tool|WDestructiveClose);
connect (sortd,SIGNAL(sort(int, int, const QString&)),
		 this,SLOT(sortTable(int, int, const QString&)));
sortd->insertColumnsList(colNames());
sortd->showNormal();
sortd->setActiveWindow();
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
	int rows=worksheet->numRows();
	QMemArray<double> r(rows), rtemp(rows);
	// Find the permutation index for the lead col
	size_t *p= new size_t[rows];
	for (j = 0; j <rows; j++)
			r[j]=this->text(j,leadcol).toDouble();

	gsl_sort_index(p,r,1,rows);
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
					worksheet->setText(j,scol,QString::number(r[j], f, prec)); 
				}
			else
				{
				for (j=0;j<rows;j++)
					worksheet->setText(j,scol,QString::number(r[rows-j-1], f, prec)); 
				}
			emit modifiedData(colName(scol));
			}
		}
	delete[] p;
	}
emit modifiedTable(this);	
}

void Table::sortColAsc()
{
//changed from version 0.5.9
int rows=worksheet->numRows();
QMemArray<int> aux(rows);
QMemArray<double> r(rows);
int i,n=0;
for (i = 0; i <rows; i++)
	{
	QString text=this->text(i,selectedCol);
	if (!text.isEmpty())
		{
		n++;
		aux[i]=i;
		r[i]=text.toDouble();
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
		worksheet->setText(i,selectedCol,QString::number(gsl_vector_get (v, n), f, prec)); 
		n++;
		}
	}
gsl_vector_free (v);
QString name=colName(selectedCol);
emit modifiedData(name);
emit modifiedTable(this);
}

void Table::sortColDesc()
{//changed from version 0.5.9
int rows=worksheet->numRows();
QMemArray<int> aux(rows);
QMemArray<double> r(rows);
int i,n=0;
for (i = 0; i <rows; i++)
	{
	QString text=this->text(i,selectedCol);
	if (!text.isEmpty())
		{
		n++;
		aux[i]=i;
		r[i]=text.toDouble();
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
		worksheet->setText(i,selectedCol,QString::number(gsl_vector_get (v, n), f, prec)); 
		}
	}
gsl_vector_free (v);
QString name=colName(selectedCol);
emit modifiedData(name);
emit modifiedTable(this);
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
bool empty=TRUE;
int cols=worksheet->numCols();
for (int i=0;i<cols;i++)
	{
	QString text=worksheet->text(row,i);
	if (!text.isEmpty())
		{
		empty=FALSE;
		break;
		}
	}	
return empty;
}

bool Table::isEmptyColumn(int col)
{
bool empty=TRUE;
int rows=worksheet->numRows();
for (int i=0;i<rows;i++)
	{
	QString text=worksheet->text(i,col);
	if (!text.isEmpty())
		{
		empty=FALSE;
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

void Table::addDataRow(const QString& s, int cols)
{
QStringList	list=QStringList::split("\t", s, TRUE);
int row = list[0].toInt();
for (int i=0;i<cols;i++)
	worksheet->setText(row, i, list[i+1]);
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
return worksheet->text(row,col).replace(",", ".");
}

void Table::setText (int row, int col, const QString & text )
{
worksheet->setText(row, col, text); 
}

void Table::freeMatrix()
{
int rows=worksheet->numRows();
int cols=worksheet->numCols();
free_matrix(wMatrix,0,rows-1,0,cols-1);
}

void Table::freeMemory()
{
int rows=worksheet->numRows();
int cols=worksheet->numCols();
free_matrix(wMatrix,0,rows-1,0,cols-1);

cells.clear();//clear cells list
}

void Table::storeCellsToMatrix()
{
int rows=worksheet->numRows();
int cols=worksheet->numCols();
wMatrix=matrix(0,rows-1,0,cols-1);
for (int i=0;i<rows;i++)
	{
	for (int j=0;j<cols;j++)
		{
		QString s = text(i, j);
		if (!s.isEmpty())
			wMatrix[i][j]=s.toDouble();
		}
	}
}

void Table::storeCellsToMemory()
{
int rows=worksheet->numRows();
int cols=worksheet->numCols();
wMatrix=matrix(0,rows-1,0,cols-1);
for (int i=0;i<rows;i++)
	{
	for (int j=0;j<cols;j++)
		{
		QString s = text(i, j);
		cells << s;
		if (!s.isEmpty())
			wMatrix[i][j]=s.toDouble();
		}
	}
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

emit modifiedTable(this);
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
		double val = wMatrix[i][col];
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
QApplication::setOverrideCursor(waitCursor);

int cols=worksheet->numCols();
if (applyToAll)
	{
	for (int j=selectedCol; j<cols; j++)
		setDateTimeFormat(f, format, j);
	}
else
	setDateTimeFormat(f, format, selectedCol);

emit modifiedTable(this);

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
int cols=worksheet->numCols();

for (int i=0; i<rows; i++)
	{
	QString s = cells[cols*i + col];
	if (!s.isEmpty())
		{
		QDate d = QDate::fromString (s,Qt::ISODate);
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
int cols=worksheet->numCols();

colTypes[col] = Time;
col_format[col] = format;

for (int i=0; i<rows; i++)
	{
	QString s = cells[cols*i + col];
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
			int month= (int(wMatrix[i][col]))%12;
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
			int month= (int(wMatrix[i][col]))%12;
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
			int day= (int(wMatrix[i][col]))%7;
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
			int day= (int(wMatrix[i][col]))%7;
			if (!day)
				day = 7;
		
			worksheet->setText(i, col, QDate::longDayName(day));
			}
		}
	}
}

void Table::setRandomValues()
{	
QApplication::setOverrideCursor(waitCursor);

int i;
double max=0.0;
int rows=worksheet->numRows();
QStringList list=selectedColumns();
QMemArray<double> r(rows);

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

	emit modifiedData(name);
	}

emit modifiedTable(this);
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
int pos=name.find("_",FALSE);
QString label=name.right(name.length()-pos-1);
return col_label.findIndex(label);
}

void Table::setHeaderColType()
{
QHeader *head=worksheet->horizontalHeader();
int xcols=0;
for (int j=0;j<(int)worksheet->numCols();j++) 
	{
	if (col_plot_type[j] == X)
		xcols++;
	}

if (xcols>1)
	{
	xcols = 0;
	for (int i=0;i<(int)worksheet->numCols();i++) 
		{
		if (col_plot_type[i] == X)
			head->setLabel(i, col_label[i]+"[X" + QString::number(++xcols) +"]", -1);
		else if (col_plot_type[i] == Y)
			head->setLabel(i, col_label[i]+"[Y"+ QString::number(xcols) +"]", -1);
		else if (col_plot_type[i] == Z)
			head->setLabel(i, col_label[i]+"[Z"+ QString::number(xcols) +"]", -1);
		else
			head->setLabel(i, col_label[i], -1);
		}
	}
else
	{
	for (int i=0;i<(int)worksheet->numCols();i++) 
		{
		if (col_plot_type[i] == X)
			head->setLabel(i, col_label[i]+"[X]", -1);
		else if (col_plot_type[i] == Y)
			head->setLabel(i, col_label[i]+"[Y]", -1);
		else if (col_plot_type[i] == Z)
			head->setLabel(i, col_label[i]+"[Z]", -1);
		else
			head->setLabel(i, col_label[i], -1);
		}
	}
}

void Table::setAscValues()
{
QApplication::setOverrideCursor(waitCursor);

int rows=worksheet->numRows();
QStringList list=selectedColumns();

for (int j=0;j<(int) list.count(); j++)
	{
	QString name=list[j];
	selectedCol=colIndex(name);

	int prec;
	char f;
	columnNumericFormat(selectedCol, f, prec);

	for (int i=0;i<rows;i++) 
		worksheet->setText(i,selectedCol,QString::number(i+1, f, prec));

	emit modifiedData(name);
	}

emit modifiedTable(this);
QApplication::restoreOverrideCursor();
}

const QString Table::parseComand(int line,const QString& comand, 
						   const QStringList& colLabels, const QStringList& rowIndexes)
{
QString s,aux=comand;
QStringList labels=colLabels;
double result;

int m=(int)colLabels.count();
QMemArray<double> vars(m); 

bool error=FALSE;
myParser parser, rparser;
try
    {
	for (int i=0; i<m; i++)
		{	
		parser.DefineVar(colLabels[i].ascii(), &vars[i]);	
			
		int row=line;
		if (rowIndexes[i] != "i")
			{
			double l=(double)(line + 1);
			rparser.ClearVar();
			rparser.DefineVar("i", &l);
			rparser.SetExpr(rowIndexes[i].ascii());
	        row=qRound(rparser.Eval()) - 1;
			if (row  < 0 && row >= worksheet->numRows())
				return "";
			}		
		QStringList list=QStringList::split("_", labels[i], FALSE);	
		int col=list[1].toInt();
		s=worksheet->text(row, col);
		if (!s.isEmpty())
			vars[i]=wMatrix[row][col];
		else 
			vars[i] = 0.0;
		}

	double index = (double)(line+1);
	parser.DefineVar("i", &index);
	parser.SetExpr(aux.ascii());
	result=parser.Eval();
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0,"QtiPlot - Input function error",e.GetMsg());
	error=TRUE;	
    }

if (error)
	return "";
else
	{
	int prec;
	char f;
	columnNumericFormat(selectedCol, f, prec);
	QString rs = QString::number(result, f, prec);
	if (rs.contains ("#") || rs.contains ("inf"))//inf or undefined values
		rs = "";
	return rs;
	}
}

void Table::setColValues(const QString& text, const QString& com, 
						const QStringList& colLabels, const QStringList& rowIndexes,
						int startRow, int endRow)
{
QApplication::setOverrideCursor(waitCursor);

int rows=worksheet->numRows();
if (endRow>rows) 
	{
	rows=endRow;
	worksheet->setNumRows(rows);
	}

storeCellsToMatrix();
//we store the table values in memory and we use them instead of the table 
//cells texts which are changing during the following for loop

for (int i=startRow-1; i<endRow; i++) 
	worksheet->setText(i, selectedCol, parseComand(i, com, colLabels, rowIndexes));

//we free the memory	
freeMatrix();

commandes[selectedCol]=text;
emit modifiedData(colName(selectedCol));
emit modifiedTable(this);
QApplication::restoreOverrideCursor();
}

void Table::setColValues(double val, int startRow, int endRow)
{
QApplication::setOverrideCursor(waitCursor);

int rows=worksheet->numRows();
if (endRow>rows) 
	{
	rows=endRow;
	worksheet->setNumRows(rows);
	}

int prec;
char f;
columnNumericFormat(selectedCol, f, prec);
QString	rs = QString::number(val, f, prec);

if (rs.contains ("#") || rs.contains ("inf"))//inf or undefined values
	rs = "";

for (int i=startRow-1; i<endRow; i++) 
	worksheet->setText(i, selectedCol, rs);

commandes[selectedCol]=rs;
emit modifiedData(colName(selectedCol));
emit modifiedTable(this);
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

void Table::plotSteps()
{
if (!valid2DPlot())
	return;
	
QStringList s=selectedYColumns();
if (int(s.count())>0)
	emit plotCol(this,s,Graph::Steps);
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
if (worksheet->numCols()<2)
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
if ( f.open(IO_ReadOnly) )
	{
	QApplication::setOverrideCursor(waitCursor);

	int i, rows = 1, cols = 0;
	int r = worksheet->numRows();
	int c = worksheet->numCols(); 
    for (i=0; i<ignoredLines; i++)
		t.readLine();

	QString s = t.readLine();//read first line after the ignored ones
	while ( !t.atEnd() ) 
		{
		t.readLine(); 
		rows++;
		}

	if (stripSpaces)
		s = s.stripWhiteSpace();
	else if (simplifySpaces)
		s = s.simplifyWhiteSpace();
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

	QProgressDialog progress(0, "progress", true, WStyle_StaysOnTop|WStyle_Tool);
	progress.setCaption("Qtiplot - Reading file...");
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
		if (stripSpaces)
			s = s.stripWhiteSpace();
		else if (simplifySpaces)
			s = s.simplifyWhiteSpace();
		line = QStringList::split(sep, s, false);	
		int end = startCol+(int)line.count();
		for (i=startCol; i<end; i++)
			{
			comments[i] = line[i-startCol];
			s = line[i-startCol].remove(QRegExp("\\W")).replace("_","-");
			int n = (int)col_label.contains(s);
			if (n)
				{//avoid identical col names
				while ((int)col_label.contains(s+QString::number(n)))
					n++;
				s+=QString::number(n);
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
			if (!importFileAs)
				{
				askOnCloseEvent(false);
				this->close();
				}
			return;
			}

		for (int k=0; k<1000; k++)
			{
			s = t.readLine();
			if (stripSpaces)
				s = s.stripWhiteSpace();
			else if (simplifySpaces)
				s = s.simplifyWhiteSpace();
			line = QStringList::split(sep, s, true);
			for (int j=startCol; j<worksheet->numCols(); j++)
				worksheet->setText(startRow + k, j, line[j-startCol]);
			}

		startRow+= 1000;
		progress.setProgress(i);
		}

	 for (i=startRow; i<worksheet->numRows(); i++)
		{
		s = t.readLine(); 
		if (stripSpaces)
			s = s.stripWhiteSpace();
		else if (simplifySpaces)
			s = s.simplifyWhiteSpace();
		line = QStringList::split(sep, s, true);
		for (int j=startCol; j<worksheet->numCols(); j++)
			worksheet->setText(i, j, line[j-startCol]);
		}
	progress.setProgress(steps+1);
	f.close();

	if (importFileAs)
		{
		for (i=0; i<worksheet->numCols(); i++)
			emit modifiedData(colName(i));
		}
	}
}

void Table::importASCII(const QString &fname, const QString &sep, int ignoredLines, 
						bool renameCols, bool stripSpaces, bool simplifySpaces, bool newTable)
{
QFile f(fname);
QTextStream t( &f );// use a text stream
if ( f.open(IO_ReadOnly) )
	{
	QApplication::setOverrideCursor(waitCursor);

	int i, c, rows = 1, cols = 0;
    for (i=0; i<ignoredLines; i++)
		t.readLine();

	QString s = t.readLine();//read first line after the ignored ones
	while ( !t.atEnd() ) 
		{
		t.readLine(); 
		rows++;
		}

	if (stripSpaces)
		s = s.stripWhiteSpace();
	else if (simplifySpaces)//All space chars (including TAB) are replaces with a single white space
		s = s.simplifyWhiteSpace();
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

	QProgressDialog progress(0, "progress", true, WStyle_StaysOnTop|WStyle_Tool);
	progress.setCaption("Qtiplot - Reading file...");
	progress.setLabelText(fname);
	progress.setActiveWindow();
	progress.setAutoClose(true);
	progress.setAutoReset(true);

	int steps = int(rows/1000);
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
		if (stripSpaces)
			s = s.stripWhiteSpace();
		else if (simplifySpaces)
			s = s.simplifyWhiteSpace();
		line = QStringList::split(sep, s, false);	
		for (i=0; i<(int)line.count(); i++)
			{
			comments[i] = line[i];
			s = line[i].remove(QRegExp("\\W")).replace("_","-");
			int n = (int)col_label.contains(s);
			if (n)
				{//avoid identical col names
				while ((int)col_label.contains(s+QString::number(n)))
					n++;
				s+=QString::number(n);
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
			if (newTable)
				{
				askOnCloseEvent(false);
				this->close();
				}
			return;
			}

		start = i*1000;
		for (int k=0; k<1000; k++)
			{
			s = t.readLine();
			if (stripSpaces)
				s = s.stripWhiteSpace();
			else if (simplifySpaces)
				s = s.simplifyWhiteSpace();
			line = QStringList::split(sep, s, true);
			for (int j=0; j<cols; j++)
				worksheet->setText(start + k, j, line[j]);
			}
		progress.setProgress(i);
		}

	 start = steps*1000;
	 for (i=start; i<rows; i++)
		{
		s = t.readLine(); 
		if (stripSpaces)
			s = s.stripWhiteSpace();
		else if (simplifySpaces)
			s = s.simplifyWhiteSpace();
		line = QStringList::split(sep, s, true);
		for (int j=0; j<cols; j++)
			worksheet->setText(i, j, line[j]);
		}
	progress.setProgress(steps+1);
	f.close();
		
	if (!newTable)
		{
		if (cols > c)
			cols = c;
		for (i=0; i<cols; i++)
			{
			emit modifiedData(colName(i));			
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
if ( !f.open( IO_WriteOnly ) ) 
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

void Table::resizeEvent(QResizeEvent *)
{
emit resizedTable(this);
}

void Table::contextMenuEvent(QContextMenuEvent *e)
{
emit showContextMenu();
e->accept();
}

void Table::closeEvent( QCloseEvent *e )
{
if (confirmClose())
    {
    switch( QMessageBox::information(0,tr("QtiPlot"),
				      tr("Do you want to hide or delete <p><b>'"
					   +QString(name())+"'</b> ?"),
				      "Delete", "Hide", "Cancel",
				      0,2) ) 
	{
    case 0:	
	e->accept();
	emit closedTable(this);
	break;

    case 1:
	e->ignore();
	emit hiddenTable(this);
	break;

	case 2:
	e->ignore();
	break;
    } 
    }
else 
    {
    e->accept();
    emit closedTable(this);
    }
}

void Table::moveCurrentCell()
{
int cols=worksheet->numCols();
int row=worksheet->currentRow();
int col=worksheet->currentColumn();
worksheet->clearSelection (TRUE);
	
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
QHeader *header = worksheet->horizontalHeader();
if (object != (QObject *)header)
	return FALSE;

int offset =header->offset();
switch(e->type())
    {
        case QEvent::MouseButtonDblClick:
			{
            const QMouseEvent *me = (const QMouseEvent *)e;
			selectedCol = header->sectionAt (me->pos().x()+offset);
			emit optionsDialog();			
			return TRUE; 
			}

		case QEvent::MouseButtonPress:
			{
			const QMouseEvent *me = (const QMouseEvent *)e;
			if (me->button() == QMouseEvent::RightButton)	
				{
				const QMouseEvent *me = (const QMouseEvent *)e;
				selectedCol=header->sectionAt (me->pos().x() + offset);
				emit colMenu(selectedCol);
				worksheet->setCurrentCell (0, selectedCol);
				}
			
			if (me->button() == QMouseEvent::LeftButton)	
				{
				LeftButton=TRUE;
				const QMouseEvent *me = (const QMouseEvent *)e;
				
				if (((const QMouseEvent *)e)->state ()==Qt::ControlButton)
					{
					int current=worksheet->currentSelection();
					QTableSelection sel=worksheet->selection(current);
					if (sel.topRow() != 0 || sel.bottomRow() != (worksheet->numRows() - 1))
						//select only full columns
						worksheet->removeSelection(sel);						
					}
				else
					worksheet->clearSelection (TRUE);
				
				selectedCol=header->sectionAt (me->pos().x()+offset);
				lastSelectedCol=selectedCol;
				worksheet->selectColumn (selectedCol);
				worksheet->setCurrentCell (0, selectedCol);
				}		
			return TRUE; 
			}
		
		case QEvent::MouseMove:
			{
			if(LeftButton)
				{
				const QMouseEvent *me = (const QMouseEvent *)e;
				selectedCol=header->sectionAt (me->pos().x() + offset);

				if(selectedCol != lastSelectedCol)
					{// This means that we are in the next column
					if(worksheet->isColumnSelected(selectedCol,TRUE))
						{//Since this column is selected, deselect it
						worksheet->removeSelection(QTableSelection (0,lastSelectedCol,worksheet->numRows()-1,lastSelectedCol));
						}
					else
						worksheet->selectColumn (selectedCol);
					}
				lastSelectedCol=selectedCol;
				worksheet->setCurrentCell (0, selectedCol);
				}
			return TRUE;
			}
			
		case QEvent::MouseButtonRelease:
			{
			LeftButton=FALSE;
			return TRUE;
			}
			
		default:
				;
    }
return QObject::eventFilter(object, e);
}

void Table::showColStatistics()
{
QString caption=QString(name())+"_stat\tStatistics of "+QString(name());
	
int rows=worksheet->numRows();
QStringList columns=selectedColumns();	
int n=(int)columns.count();
int colsNonEmpty = 0;

QString text="Col Name\t";
text+="Rows\t";
text+="Mean\t";
text+="Standard Dev\t";
text+="Variance\t";	
text+="Sum\t";
text+="iMax\t";	
text+="Max\t";
text+="iMin\t";	
text+="Min\t";
text+="N\n";

for (int i=0; i<n; i++)
	{
	int index=colIndex(columns[i]);
	text+=colLabel(index)+"\t";
	text+="[1:"+QString::number(rows)+"]\t";
	
	int j, start = 0, m = 0;
	for (j=0; j<rows; j++)
		{
		if (!worksheet->text(j,index).isEmpty())
			{
			start = j;
			m++;
			break;
			}
		}
	for (j = start + 1; j<rows; j++)
		{
		if (!worksheet->text(j,index).isEmpty())
			m++;
		}

	if (m > 0 && colTypes[index] == Numeric)
		{
		double *dat = new double[m];
		gsl_vector *y = gsl_vector_alloc (m);
		
		int aux = 0, min_index = start, max_index = start;
		double val = worksheet->text(start, index).toDouble();
		gsl_vector_set (y, 0, val);
		dat[0] = val;
		double min = val, max = val;
		for (j = start + 1; j<rows; j++)
			{
			if (!worksheet->text(j,index).isEmpty())
				{
				aux++;
				val = worksheet->text(j, index).toDouble();
				gsl_vector_set (y, aux, val);
				dat[aux] = val;
				if (val < min)
					{
					min = val;
					min_index = j;
					}
				if (val > max)
					{
					max = val;
					max_index = j;
					}
				}
			}
		double mean=gsl_stats_mean (dat,1,m);	
		text+=QString::number(mean)+"\t";
		text+=QString::number(gsl_stats_sd (dat,1,m))+"\t";
		text+=QString::number(gsl_stats_variance (dat,1,m))+"\t";
		text+=QString::number(mean*m)+"\t";

		text+=QString::number(max_index + 1)+"\t";
		text+=QString::number(max)+"\t";
		
		text+=QString::number(min_index + 1)+"\t";
		text+=QString::number(min)+"\t";	
		text+=QString::number(m)+"\n";	
		
		gsl_vector_free (y);
		delete[] dat;
		}
	else
		text+="\t\t\t\t\t\t\t\t\n";

	if (m > 0)
		colsNonEmpty++;
	}
emit createTable(caption,colsNonEmpty,11,text);
}

void Table::showRowStatistics()
{
QString caption=QString(name())+"\tstatistics";
int rows=nonEmptyRows();
int cols=worksheet->numCols();
int r=0;
	
QString text="Row\t";
text+="Cols\t";
text+="Mean\t";
text+="Standard Dev\t";
text+="Variance\t";	
text+="Sum\t";
text+="Max\t";
text+="Min\t";
text+="N\n";
	
for (int i=0;i<rows;i++)
	{
	if (worksheet->isRowSelected(i,TRUE))
		{
		r++;
		text+=QString::number(i+1)+"\t";
		text+=QString::number(cols)+"\t";
	
		int j, m = 0;		
		for (j = 0; j<cols; j++)
			{
			if (!worksheet->text(i,j).isEmpty() && colTypes[j] == Numeric)
				m++;	
			}

		if (m > 0)
			{
			double *dat = new double[m];
			gsl_vector *y = gsl_vector_alloc (m);
			int aux = 0;
			for (j = 0; j<cols; j++)
				{
				QString text = worksheet->text(i,j);
				if (!text.isEmpty() && colTypes[j] == Numeric)
					{					
					double val = text.toDouble();
					gsl_vector_set (y, aux, val);
					dat[aux] = val;
					aux++;
					}
				}
			double mean=gsl_stats_mean (dat,1,m);	
			text+=QString::number(mean)+"\t";
			text+=QString::number(gsl_stats_sd (dat,1,m))+"\t";
			text+=QString::number(gsl_stats_variance (dat,1,m))+"\t";
			text+=QString::number(mean*m)+"\t";

			double min, max;
			gsl_vector_minmax (y, &min, &max);
			text+=QString::number(max)+"\t";		
			text+=QString::number(min)+"\t";		
			text+=QString::number(m)+"\n";	
		
			gsl_vector_free (y);
			delete[] dat;
			}
		}
	}
emit createTable(caption, r, 9, text);
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
newSpecifications= saveToString("geometry\n", birthDate());
}

QString& Table::getNewSpecifications()
{
return newSpecifications;
}

QString Table::oldCaption()
{
QTextStream ts( &specifications, IO_ReadOnly );
ts.readLine();
QString s=ts.readLine();
int pos=s.find("\t",0);
return s.left(pos);
}

QString Table::newCaption()
{
QTextStream ts(&newSpecifications, IO_ReadOnly );
ts.readLine();
QString s=ts.readLine();
int pos=s.find("\t",0);
return s.left(pos);
}

void Table::restore(QString& spec)
{
int i, j, row;
int cols=worksheet->numCols();
int rows=worksheet->numRows();

QTextStream t(spec, IO_ReadOnly);	
t.readLine();	//table tag
QString s= t.readLine();
QStringList list=QStringList::split ("\t",s,TRUE);

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
	
list = QStringList::split ("\t",s,TRUE);
list.remove(list.first());
			
if (!col_label.isEmpty() && col_label != list)
	{
	loadHeader(list);
	list.gres("[X]","");
	list.gres("[Y]","");
	list.gres("[Z]","");

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
list = QStringList::split ("\t", s,TRUE);
list.remove(list.first());
if (columnWidths() != list)
	setColWidths(list);

s = t.readLine();	//commandes line
list = QStringList::split ("\t", s,TRUE);
list.remove(list.first());
if (list != commandes)
	commandes = list;

s= t.readLine();	//colType line ?
list = QStringList::split ("\t", s,TRUE);
if (s.contains ("ColType",TRUE))
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
		worksheet->setText(row, j, list[j+1]);	
	}

s= t.readLine();	//comments line ?
list = QStringList::split ("\t", s,TRUE);
if (s.contains ("Comments",TRUE))
	{
	list.remove(list.first());
	comments = list;	
	}

s= t.readLine();
list = QStringList::split ("\t", s,TRUE);
if (s.contains ("WindowLabel",TRUE))
	{
	setWindowLabel(list[1]);
	setCaptionPolicy((myWidget::CaptionPolicy)list[2].toInt());
	}

s= t.readLine();	
if (s == "<data>")	
	s = t.readLine();	

while (!t.atEnd () && s != "</data>")
	{
	list = QStringList::split ("\t", s,TRUE);

	row = list[0].toInt();
	for (j=0; j<c; j++)
		worksheet->setText(row, j, list[j+1]);	
	
	s= t.readLine();	
	}	

for (j=0; j<c; j++)
	emit modifiedData(colName(j));	
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
	QString text="Rows will be deleted from the table!";
	text+="<p>Do you really want to continue?";
	int i,cols = worksheet->numCols();
	switch( QMessageBox::information(0,"QtiPlot", text,"Yes", "Cancel", 0, 1 ) ) 
		{
    	case 0:
		QApplication::setOverrideCursor(waitCursor);
		worksheet->setNumRows(r);
		for (i=0; i<cols; i++)
			emit modifiedData(colName(i));
		
		QApplication::restoreOverrideCursor();
		break;
		
   	    case 1:
        	return;
		break;
    	}
	}
else
	{
	QApplication::setOverrideCursor(waitCursor);
	worksheet->setNumRows(r);
	QApplication::restoreOverrideCursor();
	}

emit modifiedTable(this);
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
	QMemArray<int> columns (cols-c);
	switch( QMessageBox::information(0,tr("QtiPlot"), text, tr("Yes"), tr("Cancel"), 0, 1 ) ) 
		{
    	case 0:
		QApplication::setOverrideCursor(waitCursor);
		for (i=c; i<cols; i++)
			{
			QString name = colName(i);
			emit removedCol(name);
			columns[i-c]=i;

			QStringList::Iterator it=commandes.end();
			commandes.remove (--it);
	
			it=comments.end(); comments.remove(--it);
			it=col_format.end(); col_format.remove(--it);
			it=col_label.end(); col_label.remove(--it);
 	
 			QValueListIterator<int> itt=colTypes.end();
 			colTypes.remove (--itt);
			itt=col_plot_type.end(); col_plot_type.remove(--itt);
			}
		
		worksheet->removeColumns (columns);
		QApplication::restoreOverrideCursor();
		break;
		
   	    case 1:
        	return;
		break;
    	}
	}
else
	{
	QApplication::setOverrideCursor(waitCursor);
	addColumns(c-cols);
	setHeaderColType();
	QApplication::restoreOverrideCursor();
	}
emit modifiedTable(this);
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

QApplication::setOverrideCursor(waitCursor);

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
emit modifiedTable(this);
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
QApplication::setOverrideCursor(waitCursor);

int i, i2;
int rows = worksheet->numRows();
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
		dat[i2]=worksheet->text(i, realCol).toDouble();
		if (imCol >= 0)
			dat[i2+1]=worksheet->text(i, imCol).toDouble();
		}
	}
else
	{
	QMessageBox::warning(0,"QtiPlot", tr("Could not allocate memory, operation aborted!"));
	return;
	}

double df = 1.0/(double)(rows*sampling);//frequency sampling
double aMax = 0.0;//max amplitude
QString label, text;
if(forward)
	{
	label="ForwardFFT-"+QString(this->name());
	text= tr("Frequency");
	gsl_fft_complex_forward (dat, 1, rows, wavetable, workspace);
	}
else
	{
	label="InverseFFT-"+QString(this->name());
	text= tr("Time");
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
text+="\t"+tr("Real")+"\t"+tr("Imaginary")+"\t"+tr("Amplitude")+"\t"+tr("Angle")+"\n";
for (i=0; i<rows; i++)
	{
	i2 = 2*i;
	text+=QString::number(x[i])+"\t";
	text+=QString::number(dat[i2])+"\t";
	text+=QString::number(dat[i2+1])+"\t";
	if (normalize)
		text+=QString::number(amp[i]/aMax)+"\t";
	else
		text+=QString::number(amp[i])+"\t";
	text+=QString::number(atan(dat[i2+1]/dat[i2]))+"\n";
	}

delete[] x;
delete[] amp;
delete[] dat;

emit createTable(label, rows, 5, text);	
QApplication::restoreOverrideCursor();	
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

commandes = m->getCommandes();
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
s+=saveCommandes();
s+=saveColumnTypes();
s+=saveComments();
return s;
}

void Table::restore(const QStringList& lst)
{
QStringList l = QStringList::split ("\t", lst[0], false);
l.remove(l.first());
loadHeader(l);
	
setColWidths(QStringList::split ("\t",lst[1].right(lst[1].length()-9), FALSE ));
setCommandes(lst[2]);

l = QStringList::split ("\t", lst[3], false);
l.remove(l.first());
setColumnTypes(l);

l = QStringList::split ("\t", lst[4], true);
l.remove(l.first());
setColComments(l);
}

Table::~Table()
{
}
