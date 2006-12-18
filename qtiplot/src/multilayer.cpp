/***************************************************************************
    File                 : multilayer.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Multi layer widget
                           
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
#include <qspinbox.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qmenubar.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qtextstream.h>
#include <qrect.h> 
#include <qframe.h> 
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qgroupbox.h>
#include <qfileinfo.h> 
#include <qdir.h>
#include <qpixmap.h> 
#include <qlayout.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <qtoolbutton.h> 
#include <qprinter.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qiconset.h>
#include <q3memarray.h>
#include <qbuttongroup.h>
#include <qregexp.h> 
#include <qcombobox.h>
#include <qpen.h>
#include <qcolor.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qpixmapcache.h> 
#include <qdatetime.h> 
#include <qwmatrix.h> 
#include <qlabel.h> 
#include <qimage.h>
#include <qmessagebox.h>
#include <qclipboard.h>
#include <qbitmap.h>
#include <qpicture.h>

#include <QShortcut>
#include <QList>
#include <QWidgetList>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_text_label.h>

#include "multilayer.h"
#include "graph.h"
#include "plot.h"
#include "worksheet.h"
#include "LegendMarker.h"

#include <math.h>

#include <gsl/gsl_vector.h>

LayerButton::LayerButton(const QString& text, QWidget* parent, const char* name)
        : QWidget (parent,name)
{
int btn_size = 20;
#ifdef Q_OS_MAC // Mac 
	btn_size = 40;
#endif

btn=new QPushButton(text,this,"button");
btn->setToggleButton (TRUE);
btn->setMaximumWidth(btn_size);
btn->setMaximumHeight(btn_size);
btn->installEventFilter(this);

Q3VBoxLayout *l = new Q3VBoxLayout( this, 0,0,"buttonLayout" );
l->addWidget( btn );

setMaximumWidth(btn_size);
setMaximumHeight(btn_size);
}

void LayerButton::setText(const QString& text)
{
btn->setText(text);	
}

void LayerButton::setOn(bool on)
{
btn->setOn(on);	
}

bool LayerButton::eventFilter(QObject *object, QEvent *e)
{
if ( object != (QObject *)btn)
	return FALSE;

switch(e->type())
    {
    case QEvent::MouseButtonDblClick:
		{
		if (btn->isOn())
             emit showCurvesDialog();
		return TRUE; 
		}
	case QEvent::MouseButtonPress:
		{
		const QMouseEvent *me = (const QMouseEvent *)e;
		if (me->button()==Qt::RightButton)	
			emit showLayerMenu();
		else if (me->button()==Qt::LeftButton)	
			{
			if (!btn->isOn())
				emit clicked(this);
			}
		return TRUE; 
		}
	default:
			;
    }
return QObject::eventFilter(object, e);
}

LayerButton::~LayerButton()
{
}

MultiLayer::MultiLayer(const QString& label, QWidget* parent, const char* name, Qt::WFlags f)
        : MyWidget(label,parent,name,f)
{
if ( !name )
	setName( "multilayer plot" );
	
QPalette pal = palette();
pal.setColor(QPalette::Active, QPalette::Window, QColor(Qt::white));
pal.setColor(QPalette::Inactive, QPalette::Window, QColor(Qt::white));
pal.setColor(QPalette::Disabled, QPalette::Window, QColor(Qt::white));
setPalette(pal);

QDateTime dt = QDateTime::currentDateTime ();
setBirthDate(dt.toString(Qt::LocalDate));
	
graphs=0; cols=1; rows=1;
graph_width=500; graph_height=400;
colsSpace=5; rowsSpace=5;
left_margin = 5; right_margin = 5;
top_margin = 5; bottom_margin = 5;
l_canvas_width = 400; l_canvas_height = 300;
hor_align = HCenter;  vert_align = VCenter; 
active_graph=0;
addTextOn=FALSE;
movedGraph=FALSE;
highlightedLayer = false;
ignore_resize = false;
aux_rect = QRect();
cache_pix = QPixmap();
	
hbox1=new Q3HBox(this, "hbox1"); 
int h = layerButtonHeight();
hbox1->setFixedHeight(h);
setGeometry(QRect( 0, 0, graph_width, graph_height + h));
	
canvas=new QWidget (this, "canvas");
canvas->installEventFilter(this);

Q3VBoxLayout* layout = new Q3VBoxLayout(this,0,0, "hlayout3");
layout->addWidget(hbox1);
layout->addWidget(canvas);

setFocusPolicy(Qt::StrongFocus);
connect (this, SIGNAL(resizeCanvas (const QResizeEvent *)),
		 this, SLOT(resizeLayers (const QResizeEvent *)));
}

int MultiLayer::layerButtonHeight()
{
LayerButton *button = new LayerButton(QString::number(1),this);
int h = button->height();
delete button;
return h;
}

Graph *MultiLayer::layer(int num)
{
	return (Graph*) graphsList.at(num-1);
}

LayerButton* MultiLayer::addLayerButton()
{
for (int i=0;i<buttonsList.count();i++)
	{
	LayerButton *btn=(LayerButton*) buttonsList.at(i);
	btn->setOn(FALSE);
	}

LayerButton *button=new LayerButton(QString::number(++graphs),hbox1,0);
connect (button,SIGNAL(clicked(LayerButton*)),this, SLOT(activateGraph(LayerButton*)));
connect (button,SIGNAL(showLayerMenu()),this, SIGNAL(showWindowContextMenu()));
connect (button,SIGNAL(showCurvesDialog()),this, SIGNAL(showCurvesDialog()));
button->setOn(TRUE);
buttonsList.append(button);
hbox1->setMaximumWidth(graphs*button->width());

button->show();
return button;
}

Graph* MultiLayer::insertFirstLayer()
{
LayerButton *btn = addLayerButton();	
Graph* g = new Graph(canvas,0,0);	

g->setGeometry(QRect(0,0,graph_width,graph_height));
g->plotWidget()->resize(QSize(graph_width,graph_height));
connectLayer(g);
setGeometry(QRect(0, 0,graphs*graph_width,rows*graph_height+btn->height()));
g->setIgnoreResizeEvents(true);

active_graph=g;
graphsList.append(g);
return g;
}

Graph* MultiLayer::addLayer()
{
addLayerButton();
	
Graph* g = new Graph(canvas);
g->setAttribute(Qt::WA_DeleteOnClose);

g->setGeometry(QRect(0,0,graph_width, graph_height));
g->plotWidget()->resize(QSize(graph_width, graph_height));	
graphsList.append(g);

active_graph=g;
g->show();
connectLayer(g);
return g;
}

Graph* MultiLayer::addLayer(int x, int y, int width, int height)
{
addLayerButton();
	
Graph* g = new Graph(canvas);
g->setAttribute(Qt::WA_DeleteOnClose);
QSize size=QSize(width,height);
g->plotWidget()->resize(size);
g->setGeometry(x,y,width,height);
g->removeLegend();
graphsList.append(g);

active_graph=g;
g->show();
return g;
}

Graph* MultiLayer::addLayerToOrigin()
{
addLayerButton();
	
Graph* g = new Graph(canvas,0,0);
int w = canvas->width();
int h = canvas->height();
g->setGeometry(QRect(0, 0, w, h));
g->plotWidget()->resize(QSize(w,h));
graphsList.append(g);
	
connectLayer(g);
active_graph=g;

makeTransparentLayer(g);
g->show();
emit modifiedPlot();
return g;
}

void MultiLayer::activateGraph(LayerButton* button)
{
for (int i=0;i<buttonsList.count();i++)
	{
	LayerButton *btn=(LayerButton*)buttonsList.at(i);
	if (btn->isOn())
		btn->setOn(FALSE);

	if (btn == button)	
		{
		active_graph=(Graph*) graphsList.at(i);
		active_graph->setFocus();
		button->setOn(TRUE);
		}
	}
}

void MultiLayer::setActiveGraph(Graph* g)
{
if (g && active_graph != g)
	{
	active_graph = g;
	active_graph->setFocus();

	foreach(QWidget *btn, buttonsList)
		 ((LayerButton *)btn)->setOn(FALSE);
	
	for (int i=0;i<(int)graphsList.count();i++)
		{
		Graph *gr = (Graph *)graphsList.at(i);
		if (gr == g)	
			{
			 LayerButton *btn = (LayerButton *)buttonsList.at(i);
			 btn->setOn(TRUE);
			 return;
			}
		}
	}
}

void MultiLayer::contextMenuEvent(QContextMenuEvent *e)
{
if (highlightedLayer)
	return;

emit showWindowContextMenu();
e->accept();
}

void MultiLayer::resizeLayers (const QResizeEvent *re)
{
QSize oldSize=re->oldSize();
QSize size=re->size();

if ( size == oldSize || (maxSize().width() > oldSize.width()) || !userRequested())
	return;

QApplication::setOverrideCursor(Qt::waitCursor);

double w_ratio=(double)size.width()/(double)oldSize.width();
double h_ratio=(double)(size.height())/(double)(oldSize.height());

for (int i=0;i<graphsList.count();i++)
	{
	Graph *gr=(Graph *)graphsList.at(i);
	if (gr && gr->isVisible() && !gr->ignoresResizeEvents())
		{
		QwtPlot *plot=gr->plotWidget();
		QwtPlotLayout *plotLayout=plot->plotLayout();
		QRect cRect=plotLayout->canvasRect();			
 
		int gx = qRound(gr->x()*w_ratio);
		int gy = qRound(gr->y()*h_ratio);
		int gw = qRound(gr->width()*w_ratio);
		int gh = qRound(gr->height()*h_ratio);
		
		gr->setGeometry(QRect(gx,gy,gw,gh));
		gr->plotWidget()->resize(QSize(gw, gh));

		//we try to maintain the aspect ratio of the plot canvas
		/*plotLayout=plot->plotLayout();
	
		int height = 0;
		QRect tRect=plotLayout->titleRect ();
		if (!tRect.isNull())
			height+=tRect.height() + plotLayout->spacing();

		QwtScaleWidget *scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::xTop);
		if (scale)
			{
			QRect sRect=plotLayout->scaleRect (QwtPlot::xTop);
			height+= sRect.height();
			}
		
		scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::xBottom);			
		if (scale)
			{
			QRect sRect=plotLayout->scaleRect (QwtPlot::xBottom);
			height+= sRect.height();
			}	
			
		height += int(h_ratio*ch);

		int width = 0;
		scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::yLeft);			
		if (scale)
			{
			QRect sRect=plotLayout->scaleRect (QwtPlot::yLeft);
			width+= sRect.width();
			}	
			
		scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::yRight);			
		if (scale)
			{
			QRect sRect=plotLayout->scaleRect (QwtPlot::yRight);
			width+= sRect.width();
			}

		width += int(w_ratio*cw);

		gr->setGeometry(QRect(gx,gy, width, height));
		gr->plotWidget()->resize(QSize(width, height));*/
		}
	}

//arrangeLayers(false, false); //restore the layout of the layers

emit modifiedPlot();
emit resizedWindow(this);

QApplication::restoreOverrideCursor();
}

void MultiLayer::confirmRemoveLayer()
{	
if (graphs>1)
	{
	switch(QMessageBox::information(this,
            tr("QtiPlot - Guess best layout?"),
            tr("Do you want QtiPlot to rearrange the remaining layers?"),
            tr("&Yes"), tr("&No"), tr("&Cancel"),
            0, 2) )
			{
			case 0:
				removeLayer();
				arrangeLayers(true, false);
			break;

			case 1:
				removeLayer();
			break;

			case 2:
		   		return;
			break;
			}
	}
else
	removeLayer();
}
		
void MultiLayer::removeLayer()
{			
//remove corresponding button
LayerButton *btn=0;
int i;
for (i=0;i<buttonsList.count();i++)
	{
	btn=(LayerButton*)buttonsList.at(i);	
	if (btn->isOn())	
		{
		buttonsList.removeAt(buttonsList.indexOf(btn));
		btn->close(true);			
		break;
		}
	}
	
//update the texts of the buttons	
for (i=0;i<buttonsList.count();i++)
	{
	 btn=(LayerButton*)buttonsList.at(i);	
	 btn->setText(QString::number(i+1));
	}
	
if (active_graph->selectorsEnabled() || active_graph->zoomOn() || 
	active_graph->removePointActivated() || active_graph->movePointsActivated() || 
	active_graph->enabledCursor()|| active_graph->pickerActivated())
		{
		setPointerCursor();
		}		

int index = graphsList.indexOf(active_graph);
graphsList.removeAt(index);
active_graph->close();
graphs--;
if(index >= graphsList.count())
	index--;
		
if (graphs == 0)
	{
	active_graph = 0;
	return;
	}

active_graph=(Graph*) graphsList.at(index);	

for (i=0;i<(int)graphsList.count();i++)
	{
	Graph *gr=(Graph *)graphsList.at(i);
	if (gr == active_graph)	
		{
		LayerButton *button=(LayerButton *)buttonsList.at(i);	
		button->setOn(TRUE);
		break;
		}
	}	
	
if (graphs)
	hbox1->setMaximumWidth(graphs*((LayerButton *)buttonsList.at(0))->width());

if (hasOverlapingLayers())
	updateTransparency();

emit modifiedPlot();
}

void MultiLayer::setGraphGeometry(int x, int y, int w, int h)
{
if (active_graph->pos() == QPoint (x,y) && 
	active_graph->size() == QSize (w,h))
	return;

active_graph->setGeometry(QRect(QPoint(x,y),QSize(w,h)));
active_graph->plotWidget()->resize(QSize(w, h));	

if (hasOverlapingLayers())
	updateTransparency();

emit modifiedPlot();
}

void MultiLayer::setGraphOrigin(const QPoint& pos)
{
QSize size = QSize(active_graph->width(),active_graph->height());
active_graph->setGeometry(QRect(pos, size));
active_graph->plotWidget()->resize(size);
}

void MultiLayer::moveGraph(Graph* g, const QPoint& pos)
{
setCursor(Qt::PointingHandCursor);
	
if (!movedGraph)
	{
	movedGraph=TRUE;
	showLayers(false);

	xMouse=pos.x();
	yMouse=pos.y();
		
	QPoint aux=g->pos();
	xActiveGraph=aux.x();
	yActiveGraph=aux.y();
	}
	
QPixmap pix = canvasPixmap();//Faster then using cache_pix;
QPainter painter(&pix);
// FIXME: next line
// painter.setRasterOp(Qt::NotROP);

xActiveGraph+=pos.x()-xMouse;
yActiveGraph+=pos.y()-yMouse;
	
painter.drawRect(QRect(QPoint(xActiveGraph,yActiveGraph),g->size()));
painter.end();
	
bitBlt( canvas, 0, 0,&pix, 0, 0, -1, -1 );
	
xMouse=pos.x();
yMouse=pos.y();

emit modifiedPlot();
}

void MultiLayer::releaseGraph(Graph* g)
{
setCursor(Qt::ArrowCursor);

canvas->erase();
g->setGeometry(QRect(QPoint(xActiveGraph,yActiveGraph),g->size()));
	
g->plotWidget()->resize(g->size());
for (int i=0;i<(int)graphsList.count();i++)
	{
	Graph *gr=(Graph *)graphsList.at(i);
	if (gr->plotWidget()->paletteBackgroundColor() == QColor(Qt::white))
		makeTransparentLayer(gr);
	gr->show();
	}
movedGraph=FALSE;
}

QSize MultiLayer::arrangeLayers(bool userSize)
{
const QRect rect=canvas->geometry();

gsl_vector *xTopR = gsl_vector_calloc (graphs);//ratio between top axis + title and canvas height
gsl_vector *xBottomR = gsl_vector_calloc (graphs); //ratio between bottom axis and canvas height
gsl_vector *yLeftR = gsl_vector_calloc (graphs);
gsl_vector *yRightR = gsl_vector_calloc (graphs);
gsl_vector *maxXTopHeight = gsl_vector_calloc (rows);//maximum top axis + title height in a row
gsl_vector *maxXBottomHeight = gsl_vector_calloc (rows);//maximum bottom axis height in a row
gsl_vector *maxYLeftWidth = gsl_vector_calloc (cols);//maximum left axis width in a column
gsl_vector *maxYRightWidth = gsl_vector_calloc (cols);//maximum right axis width in a column
gsl_vector *Y = gsl_vector_calloc (rows);			
gsl_vector *X = gsl_vector_calloc (cols);

int i;
for (i=0; i<graphs; i++)
	{//calculate scales/canvas dimensions reports for each layer and stores them in the above vectors
	Graph *gr=(Graph *)graphsList.at(i);
	QwtPlot *plot=gr->plotWidget();
	QwtPlotLayout *plotLayout=plot->plotLayout();
	QRect cRect=plotLayout->canvasRect();			
	double ch = (double) cRect.height();
	double cw = (double) cRect.width();
	
	QRect tRect=plotLayout->titleRect ();
	QwtScaleWidget *scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::xTop);

	int topHeight = 0;
    if (!tRect.isNull())
		topHeight+=tRect.height() + plotLayout->spacing();		
	if (scale)
		{
		QRect sRect=plotLayout->scaleRect (QwtPlot::xTop);
		topHeight+= sRect.height();
		}
	gsl_vector_set (xTopR, i, double(topHeight)/ch);
		
	scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::xBottom);			
	if (scale)
		{
		QRect sRect=plotLayout->scaleRect (QwtPlot::xBottom);
		gsl_vector_set (xBottomR, i, double(sRect.height())/ch);
		}	
			
	scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::yLeft);			
	if (scale)
		{
		QRect sRect=plotLayout->scaleRect (QwtPlot::yLeft);
		gsl_vector_set (yLeftR, i, double(sRect.width())/cw);
		}	
			
	scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::yRight);			
	if (scale)
		{
		QRect sRect=plotLayout->scaleRect (QwtPlot::yRight);
		gsl_vector_set (yRightR, i, double(sRect.width())/cw);
		}
	
//calculate max scales/canvas dimensions ratio for each line and column and stores them to vectors	
	int row = i / cols;
	if (row >= rows )
		row = rows - 1;

	int col = i % cols;
		
	double aux = gsl_vector_get(xTopR, i);
	double old_max = gsl_vector_get(maxXTopHeight, row);
	if (aux >= old_max)
		gsl_vector_set(maxXTopHeight, row,  aux);
			
	aux = gsl_vector_get(xBottomR, i) ;
	if (aux >= gsl_vector_get(maxXBottomHeight, row))
		gsl_vector_set(maxXBottomHeight, row,  aux);
			
	aux = gsl_vector_get(yLeftR, i);
	if (aux >= gsl_vector_get(maxYLeftWidth, col))
		gsl_vector_set(maxYLeftWidth, col, aux);
				
	aux = gsl_vector_get(yRightR, i);
	if (aux >= gsl_vector_get(maxYRightWidth, col))
		gsl_vector_set(maxYRightWidth, col, aux);	
	}	
						
double c_heights = 0.0;	
for (i=0; i<rows; i++)
	{
	gsl_vector_set (Y, i, c_heights);	
	c_heights+= 1 + gsl_vector_get(maxXTopHeight, i) + gsl_vector_get(maxXBottomHeight, i);	
	}
		
double c_widths = 0.0;
for (i=0; i<cols; i++)
	{
	gsl_vector_set (X, i, c_widths);	
	c_widths+= 1 + gsl_vector_get(maxYLeftWidth, i) + gsl_vector_get(maxYRightWidth, i);	
	}

if (!userSize)
	{
	l_canvas_width = int((rect.width()-(cols-1)*colsSpace - right_margin - left_margin)/c_widths);
	l_canvas_height = int((rect.height()-(rows-1)*rowsSpace - top_margin - bottom_margin)/c_heights);
	}

QSize size = QSize(l_canvas_width, l_canvas_height);

for (i=0; i<graphs; i++)
	{	
	int row = i / cols;
	if (row >= rows )
		row = rows - 1;
		
	int col = i % cols;	
		
	//calculate sizes and positions for layers
	const int w = int (l_canvas_width*(1 + gsl_vector_get(yLeftR, i) + gsl_vector_get(yRightR, i)));
	const int h = int (l_canvas_height*(1 + gsl_vector_get(xTopR, i) + gsl_vector_get(xBottomR, i)));
	
	int x = left_margin + col*colsSpace;
	if (hor_align == HCenter)
		x += int (l_canvas_width*(gsl_vector_get(X, col) + gsl_vector_get(maxYLeftWidth, col) - gsl_vector_get(yLeftR, i)));
	else if (hor_align == Left)
		x += int(l_canvas_width*gsl_vector_get(X, col));
	else if (hor_align == Right)
		x += int(l_canvas_width*(gsl_vector_get(X, col) + gsl_vector_get(maxYLeftWidth, col) - gsl_vector_get(yLeftR, i)+
				 gsl_vector_get(maxYRightWidth, col) - gsl_vector_get(yRightR, i)));

	int y = top_margin + row*rowsSpace;
	if (vert_align == VCenter)	
		y += int(l_canvas_height*(gsl_vector_get(Y, row) + gsl_vector_get(maxXTopHeight, row) - gsl_vector_get(xTopR, i)));
	else if (vert_align == Top)	
		y += int(l_canvas_height*gsl_vector_get(Y, row));
	else if (vert_align == Bottom)	
		y += int(l_canvas_height*(gsl_vector_get(Y, row) + gsl_vector_get(maxXTopHeight, row) - gsl_vector_get(xTopR, i)+
				 + gsl_vector_get(maxXBottomHeight, row) - gsl_vector_get(xBottomR, i)));
					
	//resizes and moves layers
	Graph *gr=(Graph *)graphsList.at(i);
	bool autoscaleFonts = false;
	if (!userSize)
		{//When the user specifies the layer canvas size, the window is resized
		 //and the fonts must be scaled accordingly. If the size is calculated 
		 //automatically we don't rescale the fonts in order to prevent problems 
		 //with too small fonts when the user adds new layers or when removing layers

		autoscaleFonts = gr->autoscaleFonts();//save user settings
		gr->setAutoscaleFonts(false);
		}

	gr->setGeometry(QRect(x, y, w, h));
	gr->plotWidget()->resize(QSize(w, h));

	if (!userSize)
		gr->setAutoscaleFonts(autoscaleFonts);//restore user settings
	}

//free memory		
gsl_vector_free (maxXTopHeight); gsl_vector_free (maxXBottomHeight);
gsl_vector_free (maxYLeftWidth); gsl_vector_free (maxYRightWidth);		
gsl_vector_free (xTopR); gsl_vector_free (xBottomR);
gsl_vector_free (yLeftR); gsl_vector_free (yRightR); 
gsl_vector_free (X); gsl_vector_free (Y);

return size;
}

void MultiLayer::findBestLayout(int &rows, int &cols)
{
int NumGraph=graphs;
if(NumGraph%2==0) // NumGraph is an even number
	{
	if(NumGraph<=2)
		cols=NumGraph/2+1;
	else if(NumGraph>2)
		cols=NumGraph/2;
		
	if(NumGraph<8)
		rows=NumGraph/4+1;
	if(NumGraph>=8)
		rows=NumGraph/4;
	}
else if(NumGraph%2!=0) // NumGraph is an odd number
	{
	int Num=NumGraph+1;
			
	if(Num<=2)
		cols=1;
	else if(Num>2)
		cols=Num/2;
		
	if(Num<8)
		rows=Num/4+1;
	if(Num>=8)
		rows=Num/4;
	}
}

void MultiLayer::arrangeLayers(bool fit, bool userSize)
{
if (!graphs)
	return;

QApplication::setOverrideCursor(Qt::waitCursor);

if (fit)
	findBestLayout(rows, cols);	

//the canvas sizes of all layers become equal only after several 
//resize iterations, due to the way Qwt handles the plot layout
int iterations = 0;
QSize size = arrangeLayers(userSize);
QSize canvas_size = QSize(1,1);
while (canvas_size != size && iterations < 10)
	{
	iterations++;
	canvas_size = size;
	size = arrangeLayers(userSize);
	}

if (userSize)
	{//resize window
	int i;
	for (i=0;i<(int)graphsList.count();i++)
		{
		Graph *gr=(Graph *)graphsList.at(i);
		gr->setIgnoreResizeEvents(true);
		}

	this->showNormal();
	QSize size = maxSize();
	this->resize(QSize(size.width() + right_margin,
				  size.height() + bottom_margin + layerButtonHeight()));

	for (i=0;i<(int)graphsList.count();i++)
		{
		Graph *gr=(Graph *)graphsList.at(i);
		gr->setIgnoreResizeEvents(false);
		}
	}

updateTransparency();
emit modifiedPlot();
QApplication::restoreOverrideCursor();
}

QSize MultiLayer::maxSize()
{
int w=0, h=0, maxw=0, maxh=0;	
for (int i=0;i<(int)graphsList.count();i++)
	{
	Graph *gr=(Graph *)graphsList.at(i);
	w = gr->pos().x() + gr->frameGeometry().width();

	if (maxw <= w)
		maxw = w;
	
	h = gr->pos().y() + gr->frameGeometry().height();
	if (maxh <= h)
		maxh = h;
	}	
return QSize(maxw, maxh);
}

void MultiLayer::setCols(int c)
{
if (cols != c)
	cols=c;	
}

void MultiLayer::setRows(int r)
{
if (rows != r)
	rows=r;	
}

QPixmap MultiLayer::canvasPixmap()
{
QSize size=canvas->size();
QPixmap pic(size.width(), size.height());
pic.fill (Qt::white);
	
QPainter paint;
paint.begin(&pic);

QwtPlotPrintFilter filter;
filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle |
				   QwtPlotPrintFilter::PrintCanvasBackground);

for (int i=0;i<(int)graphsList.count();i++)
		{
		Graph *gr=(Graph *)graphsList.at(i);
		Plot *myPlot= gr->plotWidget();

		int lw = myPlot->lineWidth();
		QRect rect = QRect(gr->x() + lw, gr->y() + lw, myPlot->width() - 2*lw, 
						  myPlot->height() - 2*lw);
		myPlot->print(&paint, rect, filter);
		}		
paint.end();
return pic;
}
	
void MultiLayer::exportImage(const QString& fileName, const QString& fileType,
										int quality, bool transparent)
{
QPixmap pic=canvasPixmap();
if (transparent)
	{//save transparency
	QBitmap mask(pic.size());
	mask.fill(Qt::color1);
	QPainter p;
	p.begin(&mask);
	p.setPen(Qt::color0);
	
	QColor background = QColor (Qt::white);
	QRgb backgroundPixel = background.rgb ();

	QImage image = pic.convertToImage();
	for (int y=0; y<image.height(); y++)
		{
		for ( int x=0; x<image.width(); x++ )
			{
			QRgb rgb = image.pixel(x, y);
			if (rgb == backgroundPixel) // we want the frame transparent
				p.drawPoint( x, y );
			}
		}
	p.end();	
	pic.setMask(mask);
	}
pic.save(fileName, fileType, quality);	

if (hasOverlapingLayers())
	updateTransparency();
}

void MultiLayer::exportToEPS(const QString& fname)
{
exportToEPS(fname, 84, QPrinter::Landscape, QPrinter::A4, QPrinter::Color);
}

void MultiLayer::exportToEPS(const QString& fname, int res, QPrinter::Orientation o, 
						QPrinter::PageSize pageSize, QPrinter::ColorMode col)
{	
QPrinter printer;
//printer.setResolution(res);
printer.setPageSize(pageSize);
printer.setColorMode(col);
printer.setOrientation(o);
	
printer.setFullPage(TRUE);
printer.setOutputToFile (TRUE);
printer.setOutputFileName(fname);

QPainter paint(&printer);

int dpiy = printer.logicalDpiY();
int margin = (int) ( (0.5/2.54)*dpiy ); // 5 mm margins
	
QSize size = canvas->size();

double scaleFactorX=(double)(printer.width() - 2*margin)/(double)size.width();
double scaleFactorY=(double)(printer.height() - 2*margin)/(double)size.height();
	
// fit graph to page maintaining the aspect ratio
if(scaleFactorX > scaleFactorY) 
	scaleFactorX = scaleFactorY;
else
	scaleFactorY = scaleFactorX;

for (int i=0;i<(int)graphsList.count();i++)
	{
	Graph *gr=(Graph *)graphsList.at(i);
	Plot *myPlot= (Plot *)gr->plotWidget();
			
	QwtPlotPrintFilter  filter; 
	filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle 
					  |~QwtPlotPrintFilter::PrintCanvasBackground);

	QPoint pos=gr->pos();
	pos=QPoint(int(margin + pos.x()*scaleFactorX),int(margin + pos.y()*scaleFactorY));
	
	int width=int(myPlot->frameGeometry().width()*scaleFactorX);
	int height=int(myPlot->frameGeometry().height()*scaleFactorY);

	myPlot->print(&paint, QRect(pos,QSize(width,height)), filter);
	}

if (hasOverlapingLayers())		
	updateTransparency();
}

void MultiLayer::exportToSVG(const QString& fname)
{	
QPicture picture;
QPainter p(&picture);
for (int i=0;i<(int)graphsList.count();i++)
	{
	Graph *gr=(Graph *)graphsList.at(i);
	Plot *myPlot= (Plot *)gr->plotWidget();

	QPoint pos=gr->pos();
	
	int width=int(myPlot->frameGeometry().width());
	int height=int(myPlot->frameGeometry().height());

	myPlot->print(&p, QRect(pos,QSize(width,height)));
	}

p.end();
picture.save(fname, "svg");
}

void MultiLayer::copyAllLayers()
{
QPixmap pic = canvasPixmap();	
QImage image= pic.convertToImage();		
QApplication::clipboard()->setImage(image);	
}

void MultiLayer::printActiveLayer()
{
active_graph->print();

if (hasOverlapingLayers())
	updateTransparency();
}

void MultiLayer::print()
{
QPrinter printer;
//printer.setResolution(84);
printer.setOrientation(QPrinter::Landscape);
printer.setColorMode (QPrinter::Color);
printer.setFullPage(TRUE);

if (printer.setup())
	{
	QPainter paint(&printer);	
	printAllLayers(&paint);
	paint.end();	
	}		
}

void MultiLayer::printAllLayers(QPainter *painter)
{
if (!painter)
	return;

QPaintDevice *paintDevice = painter->device();
	
int dpiy = paintDevice->logicalDpiY();
int margin = (int) ( (1/2.54)*dpiy ); // 1 cm margins
	
QSize size=maxSize();
double scaleFactorX=(double)(paintDevice->width()-2*margin)/(double)size.width();
double scaleFactorY=(double)(paintDevice->height()-2*margin)/(double)size.height();
		
for (int i=0; i<(int)graphsList.count(); i++)
	{			
	Graph *gr=(Graph *)graphsList.at(i);
	Plot *myPlot= gr->plotWidget();
			
	QwtPlotPrintFilter  filter; 
    filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle |
				      ~ QwtPlotPrintFilter::PrintCanvasBackground);

	QPoint pos=gr->pos();
	pos=QPoint(margin + int(pos.x()*scaleFactorX), margin + int(pos.y()*scaleFactorY));
			
	int width=int(myPlot->frameGeometry().width()*scaleFactorX);
	int height=int(myPlot->frameGeometry().height()*scaleFactorY);
	
	myPlot->print(painter, QRect(pos,QSize(width,height)), filter);
	}

if (hasOverlapingLayers())
	updateTransparency();
}

void MultiLayer::setFonts(const QFont& titleFnt, const QFont& scaleFnt,
												const QFont& numbersFnt, const QFont& legendFnt)
{
for (int i=0;i<(int)graphsList.count();i++)
		{
		Graph *gr=(Graph *)graphsList.at(i);
		QwtPlot *plot=gr->plotWidget();
			
		plot->title().setFont(titleFnt);
		for (int j= 0;j<QwtPlot::axisCnt;j++)
			{
			plot->setAxisFont (j,numbersFnt);
			plot->axisTitle(j).setFont(scaleFnt);
			}
			
		QList<int> keys=gr->textMarkerKeys();
		for (int k=0;k<(int)keys.size();k++)
			{
			LegendMarker* mrk=(LegendMarker*)gr->textMarker(keys[k]);
			if (mrk)
				mrk->setFont(legendFnt);
			}
			
		plot->replot();
		}
emit modifiedPlot();
}

void MultiLayer::makeTransparentLayer(Graph *g)
{
Plot *plot = g->plotWidget();
int lw = plot->lineWidth();
int x = g->x();
int y = g->y();

QRect rect = QRect (plot->x() + lw, plot->y() + lw, plot->width() - 2* lw, plot->height() - 2*lw);
QwtPlotLayout *plotLayout=plot->plotLayout();
plotLayout->activate(plot, rect, 0);

QPixmap pix = QPixmap::grabWidget (canvas, x, y, g->width(), g->height());
plot->setPaletteBackgroundPixmap(pix);
	
QwtTextLabel *title=plot->titleLabel ();
QRect tRect=plotLayout->titleRect ();
if (!tRect.isNull())
	{
	pix = QPixmap::grabWidget (canvas, x + tRect.x(), y + tRect.y(), tRect.width(), tRect.height());		
	title->setPaletteBackgroundPixmap(pix);
	}

for (int i=0;i<QwtPlot::axisCnt;i++)
	{
	QwtScaleWidget *scale=(QwtScaleWidget *) plot->axisWidget (i);
	if (scale)
		{
		QRect sRect=plotLayout->scaleRect (i);
		pix = QPixmap::grabWidget (canvas,x+sRect.x(),y+sRect.y(),sRect.width(), sRect.height());		
		scale->setPaletteBackgroundPixmap(pix);
		}
	}
		
QwtPlotCanvas *plotCanvas = plot->canvas();
QRect cRect=plotLayout->canvasRect ();
pix = QPixmap::grabWidget (canvas,x+cRect.x(),y+cRect.y(),cRect.width(), cRect.height());	
plotCanvas->setPaletteBackgroundPixmap(pix);

plot->replot();
}	

void MultiLayer::updateLayerTransparency(Graph *g)
{
if (overlapsLayers(g))
	updateTransparency();
}

void MultiLayer::updateTransparency()
{
QApplication::setOverrideCursor(Qt::waitCursor);

showLayers(false);

for (int i=0; i<(int)graphsList.count(); i++)
	{	
	Graph *gr=(Graph *)graphsList.at(i);
	if (gr->plotWidget()->paletteBackgroundColor() == QColor(Qt::white))
		makeTransparentLayer(gr);
	gr->show();
	}

QApplication::restoreOverrideCursor();
}		

bool MultiLayer::hasOverlapingLayers()
{
for (int i=0;i<(int)graphsList.count();i++)
	{
	Graph *gr=(Graph *)graphsList.at(i);
	if (overlapsLayers(gr))
		return true;
	}
		
return false;
}

bool MultiLayer::overlapsLayers(Graph *g)
{
int  s=0;
QRect r= g->frameGeometry();
for (int i=0;i<(int)graphsList.count();i++)
		{
		Graph *gr=(Graph *)graphsList.at(i);
		QRect ar = gr->frameGeometry();
		if  ( ar.intersects(r) )
			s++;
		}
		
if (s > 1) 
	return true;
else
	return false;
}

void MultiLayer::connectLayer(Graph *g)
{
connect (g,SIGNAL(drawLineEnded(bool)), this, SIGNAL(drawLineEnded(bool)));
connect (g,SIGNAL(drawTextOff()),this,SIGNAL(drawTextOff()));
connect (g,SIGNAL(showPlotDialog(int)),this,SIGNAL(showPlotDialog(int)));
connect (g,SIGNAL(createHiddenTable(const QString&,int,int,const QString&)),
		 this,SIGNAL(createHiddenTable(const QString&,int,int,const QString&)));
connect (g,SIGNAL(createTable(const QString&,int,int,const QString&)),
		 this,SIGNAL(createTable(const QString&,int,int,const QString&)));
connect (g,SIGNAL(viewLineDialog()),this,SIGNAL(showLineDialog()));
connect (g,SIGNAL(showContextMenu()),this,SIGNAL(showGraphContextMenu()));
connect (g,SIGNAL(showAxisDialog(int)),this,SIGNAL(showAxisDialog(int)));
connect (g,SIGNAL(axisDblClicked(int)),this,SIGNAL(showScaleDialog(int)));
connect (g,SIGNAL(xAxisTitleDblClicked()),this,SIGNAL(showXAxisTitleDialog()));		
connect (g,SIGNAL(yAxisTitleDblClicked()),this,SIGNAL(showYAxisTitleDialog()));
connect (g,SIGNAL(rightAxisTitleDblClicked()),this,SIGNAL(showRightAxisTitleDialog()));
connect (g,SIGNAL(topAxisTitleDblClicked()),this,SIGNAL(showTopAxisTitleDialog()));
connect (g,SIGNAL(showMarkerPopupMenu()),this,SIGNAL(showMarkerPopupMenu()));
connect (g,SIGNAL(showCurveContextMenu(int)),this,SIGNAL(showCurveContextMenu(int)));
connect (g,SIGNAL(cursorInfo(const QString&)),this,SIGNAL(cursorInfo(const QString&)));
connect (g,SIGNAL(viewImageDialog()),this,SIGNAL(showImageDialog()));
connect (g,SIGNAL(createTablePlot(const QString&,int,int,const QString&)),this,SIGNAL(createTablePlot(const QString&,int,int,const QString&)));
connect (g,SIGNAL(showPieDialog()),this,SIGNAL(showPieDialog()));
connect (g,SIGNAL(viewTitleDialog()),this,SIGNAL(viewTitleDialog()));
connect (g,SIGNAL(modifiedGraph()),this,SIGNAL(modifiedPlot()));
connect (g,SIGNAL(selectedGraph(Graph*)),this, SLOT(setActiveGraph(Graph*)));
connect (g,SIGNAL(viewTextDialog()),this,SIGNAL(showTextDialog()));
connect (g,SIGNAL(updateTable(const QString&,int,const QString&)),this,SIGNAL(updateTable(const QString&,int,const QString&)));
connect (g,SIGNAL(updateTableColumn(const QString&, double *, int)),
		 this,SIGNAL(updateTableColumn(const QString&, double *, int)));
connect (g,SIGNAL(clearCell(const QString&,double)),this,SIGNAL(clearCell(const QString&,double)));	
connect (g,SIGNAL(moveGraph(Graph*, const QPoint& )),this, SLOT(moveGraph(Graph*, const QPoint&)));
connect (g,SIGNAL(releaseGraph(Graph*)),this, SLOT(releaseGraph(Graph*)));
connect (g,SIGNAL(modifiedGraph(Graph*)), this, SLOT(updateLayerTransparency(Graph*)));
connect (g,SIGNAL(createIntensityTable(const QPixmap&)),
				this,SIGNAL(createIntensityTable(const QPixmap&)));
connect (g,SIGNAL(createHistogramTable(const QString&,int,int,const QString&)),
		this,SIGNAL(createHistogramTable(const QString&,int,int,const QString&)));
connect (g,SIGNAL(highlightGraph(Graph*)),this,SLOT(highlightLayer(Graph*)));
}

void MultiLayer::addTextLayer(int f, const QFont& font, 
						const QColor& textCol, const QColor& backgroundCol)
{
defaultTextMarkerFont = font;
defaultTextMarkerFrame = f; 
defaultTextMarkerColor = textCol;
defaultTextMarkerBackground = backgroundCol;

addTextOn=TRUE;
QApplication::setOverrideCursor(Qt::IBeamCursor);
canvas->grabMouse();
}

void MultiLayer::addTextLayer(const QPoint& pos)
{
Graph* g=addLayer();
g->removeLegend();
g->setTitle("");
Q3MemArray<bool> axesOn(4);
for (int j=0;j<4;j++)
	axesOn[j]=FALSE;
g->enableAxes(axesOn);
g->setIgnoreResizeEvents(true);
g->setTextMarkerDefaults(defaultTextMarkerFrame, defaultTextMarkerFont, 
						 defaultTextMarkerColor, defaultTextMarkerBackground);
LegendMarker *mrk = g->newLegend(tr("enter your text here"));
QSize size = mrk->rect().size();
setGraphGeometry(pos.x(), pos.y(), size.width()+10, size.height()+10);
g->setIgnoreResizeEvents(false);
g->show();
QApplication::restoreOverrideCursor();
canvas->releaseMouse();
addTextOn=FALSE;
emit drawTextOff();
emit modifiedPlot();
}

bool MultiLayer::eventFilter(QObject *object, QEvent *e)
{
if ( object != (QObject *)canvas)
	return FALSE;

switch(e->type())
    { 
	case QEvent::MouseButtonPress:
		{
		const QMouseEvent *me = (const QMouseEvent *)e;
			
		if (me->button()==Qt::LeftButton && addTextOn)	
			addTextLayer(me->pos());
	
		return false; 
		}

	case QEvent::Resize:
		{
		resizeLayers((const QResizeEvent *)e);
		}

	default:
		;
    }
return QObject::eventFilter(object, e);
}

void MultiLayer::keyPressEvent(QKeyEvent * e)
{
if (e->key() == Qt::Key_F12)	
	{
	int index = graphsList.indexOf((QWidget *)active_graph);
	Graph *g=(Graph *)graphsList.at(index+1);
	if (g)
		{
		setActiveGraph(g);
		highlightLayer(g);
		}
	return;
	}
	
if (e->key() == Qt::Key_F10)	
	{
	int index=graphsList.indexOf((QWidget *)active_graph);
	Graph *g=(Graph *)graphsList.at(index-1);
	if (g)
		{
		setActiveGraph(g);
		highlightLayer(g);
		}
	return;
	}
	
if (e->key() == Qt::Key_F11)
	{
	emit showWindowContextMenu();
	return;
	}

if (highlightedLayer && (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return))
	{
	releaseLayer();
	return;
	}

if (highlightedLayer && !aux_rect.isNull())
	{
	if (e->key() == Qt::Key_Left)
		aux_rect.moveBy(-1, 0);
	else if (e->key() == Qt::Key_Right)
		aux_rect.moveBy(1, 0);
	else if (e->key() == Qt::Key_Up)
		aux_rect.moveBy(0, -1);
	else if (e->key() == Qt::Key_Down)
		aux_rect.moveBy(0, 1);

	drawLayerFocusRect(aux_rect);
	return;
	}
}

void MultiLayer::wheelEvent ( QWheelEvent * e ) 
{
QApplication::setOverrideCursor(Qt::waitCursor);

bool resize=FALSE;
QPoint aux;
QSize intSize;
Graph *resize_graph = 0;
// Get the position of the mouse
xMouse=e->x();
yMouse=e->y();
for (int i=0;i<(int)graphsList.count();i++)
	{
	Graph *gr=(Graph *)graphsList.at(i);
	intSize=gr->plotWidget()->size();
	aux=gr->pos();	
	if(xMouse>aux.x() && xMouse<(aux.x()+intSize.width()))
		{
		if(yMouse>aux.y() && yMouse<(aux.y()+intSize.height()))
			{
			resize_graph=gr;
			resize=TRUE;
			}
		}
	}
if(resize && (e->state()==Qt::AltButton || e->state()==Qt::ControlButton || e->state()==Qt::ShiftButton))
	{
	intSize=resize_graph->plotWidget()->size();
	// If alt is pressed then change the width
	if(e->state()==Qt::AltButton)
	{
	if(e->delta()>0)
		{
		intSize.rwidth()+=5;
		}
	else if(e->delta()<0)
		{
		intSize.rwidth()-=5;
		}
	}
	// If crt is pressed then changed the height
	else if(e->state()==Qt::ControlButton)
	{
	if(e->delta()>0)
		{
		intSize.rheight()+=5;
		}
	else if(e->delta()<0)
		{
		intSize.rheight()-=5;
		}
	}
	// If shift is pressed then resize 
	else if(e->state()==Qt::ShiftButton)
	{
	if(e->delta()>0)
		{
		intSize.rwidth()+=5;
		intSize.rheight()+=5;
		}
	else if(e->delta()<0)
		{
		intSize.rwidth()-=5;
		intSize.rheight()-=5;
		}
	}

	aux=resize_graph->pos();
	resize_graph->setGeometry(QRect(QPoint(aux.x(),aux.y()),intSize));
	resize_graph->plotWidget()->resize(intSize);
	updateLayerTransparency(resize_graph);

	emit modifiedPlot();
	}
QApplication::restoreOverrideCursor();
}

bool MultiLayer::isEmpty () 
{
if (graphs <= 0)
	return true;
else
	return false;
}

QString MultiLayer::saveToString(const QString& geometry)
{
QString s="<multiLayer>\n";
s+=QString(name())+"\t";
s+=QString::number(cols)+"\t";
s+=QString::number(rows)+"\t";
s+=birthDate()+"\n";
s+=geometry;
s+="WindowLabel\t" + windowLabel() + "\t" + QString::number(captionPolicy()) + "\n";
s+="Margins\t"+QString::number(left_margin)+"\t"+QString::number(right_margin)+"\t"+
	QString::number(top_margin)+"\t"+QString::number(bottom_margin)+"\n";
s+="Spacing\t"+QString::number(rowsSpace)+"\t"+QString::number(colsSpace)+"\n";
s+="LayerCanvasSize\t"+QString::number(l_canvas_width)+"\t"+QString::number(l_canvas_height)+"\n";
s+="Alignement\t"+QString::number(hor_align)+"\t"+QString::number(vert_align)+"\n";

for (int i=0;i<(int)graphsList.count();i++)
	{
	Graph* ag=(Graph*)graphsList.at(i);
	s+=ag->saveToString();
	}
return s+"</multiLayer>\n";
}

QString MultiLayer::saveAsTemplate(const QString& geometryInfo) 
{
QString s="<multiLayer>\t";
s+=QString::number(rows)+"\t";
s+=QString::number(cols)+"\n";
s+= geometryInfo;
s+="Margins\t"+QString::number(left_margin)+"\t"+QString::number(right_margin)+"\t"+
	QString::number(top_margin)+"\t"+QString::number(bottom_margin)+"\n";
s+="Spacing\t"+QString::number(rowsSpace)+"\t"+QString::number(colsSpace)+"\n";
s+="LayerCanvasSize\t"+QString::number(l_canvas_width)+"\t"+QString::number(l_canvas_height)+"\n";
s+="Alignement\t"+QString::number(hor_align)+"\t"+QString::number(vert_align)+"\n";

for (int i=0;i<(int)graphsList.count();i++)
	{
	Graph* ag=(Graph*)graphsList.at(i);
	s+=ag->saveAsTemplate();
	}
return s;
}

bool MultiLayer::allLayersTransparent()
{
int graphs=(int)graphsList.count();
bool allTransparent = true;
for (int i=0;i<graphs;i++)
	{
	Graph *gr=(Graph *)graphsList.at(i);
	QColor c = gr->plotWidget()->paletteBackgroundColor();
	if (c != QColor(Qt::white))
		return  false;
	}
return allTransparent;
}

void MultiLayer::highlightLayer(Graph*g)
{
active_graph = g;
cache_pix = canvasPixmap();
showLayers(false);
aux_rect = active_graph->geometry();	
drawLayerFocusRect(aux_rect);
highlightedLayer = true;
}

void MultiLayer::mousePressEvent ( QMouseEvent * e )
{	
if (!highlightedLayer || e->button() != Qt::LeftButton)
	return;

int margin = 5;
QRect ar = active_graph->geometry();
ar.addCoords(-margin, -margin, margin, margin );
QPoint pos = canvas->mapFromParent(e->pos());
if (ar.contains(pos))
	{// Get the initial location of the mouse
	xMouse=pos.x();
	yMouse=pos.y();		
	}
else
	{
	canvas->erase();
	showLayers(true);
	highlightedLayer = false;
	aux_rect = QRect();
	cache_pix = QPixmap();//invalidate pix cache
	}
}

void MultiLayer::mouseMoveEvent ( QMouseEvent * e )
{
if (!highlightedLayer)
	return;

// Get the position of the mouse
QPoint pos = canvas->mapFromParent(e->pos());

int dx = pos.x() - xMouse;
int dy = pos.y() - yMouse;

if(dx!=0 && dy==0)
	this->setCursor(Qt::SizeHorCursor);	
else if(dx==0 && dy!=0) 
	this->setCursor(Qt::SizeVerCursor);
else if( (dx>0 && dy>0) || (dx<0 && dy<0))
	this->setCursor(Qt::SizeFDiagCursor);
else if( (dx<0 && dy>0) || (dx>0 && dy<0))
	this->setCursor(Qt::SizeBDiagCursor);

QPoint center = QPoint(aux_rect.x()+aux_rect.width()/2, aux_rect.y()+aux_rect.height()/2);	
	
if (pos.x() > center.x() && pos.y() > center.y())
	aux_rect.addCoords(0, 0, dx, dy);
else if (pos.x() > center.x() && pos.y() < center.y())
	aux_rect.addCoords(0, dy, dx, 0);
else if (pos.x() < center.x() && pos.y() < center.y())
	aux_rect.addCoords(dx, dy, 0, 0);
else if (pos.x() < center.x() && pos.y() > center.y())
	aux_rect.addCoords(dx, 0, 0, dy);

xMouse = pos.x();
yMouse = pos.y();

drawLayerFocusRect(aux_rect);
}

void MultiLayer::mouseReleaseEvent ( QMouseEvent *)
{
releaseLayer();
}

void MultiLayer::drawLayerFocusRect(const QRect& fr)
{
int lw = active_graph->plotWidget()->lineWidth() + 3; 
QPixmap pix = cache_pix;
QPainter painter(&pix);
// FIXME: next line
// painter.setRasterOp(Qt::NotXorROP);
painter.setPen(QPen(QColor(Qt::red), lw, Qt::SolidLine));

painter.drawRect(fr);
painter.setBrush(QBrush(QColor(Qt::red), Qt::SolidPattern));

QRect sr = QRect (QPoint(0,0), QSize(lw, lw));
sr.moveCenter (fr.topLeft());
painter.drawRect(sr);

sr.moveBy(fr.width()/2, 0);
painter.drawRect(sr);

sr.moveCenter (fr.topRight());
painter.drawRect(sr);

sr.moveBy(0, fr.height()/2);
painter.drawRect(sr);

sr.moveCenter (fr.bottomRight());
painter.drawRect(sr);

sr.moveBy(-fr.width()/2, 0);
painter.drawRect(sr);

sr.moveCenter (fr.bottomLeft());
painter.drawRect(sr);

sr.moveBy(0, -fr.height()/2);
painter.drawRect(sr);

painter.setBrush(Qt::NoBrush);

painter.end();
bitBlt( canvas, 0, 0,&pix, 0, 0, -1, -1 );
}

void MultiLayer::showLayers(bool ok)
{
if (ok)
	{
	for (int i=0;i<(int)graphsList.count();i++)
		{
		Graph *gr=(Graph *)graphsList.at(i);
		if (!gr->isVisible())
			gr->show();
		}
	}
else
	{
	for (int i=0;i<(int)graphsList.count();i++)
		{
		Graph *gr=(Graph *)graphsList.at(i);
		if (gr->isVisible())
			gr->hide();
		}
	}
}

void MultiLayer::setMargins (int lm, int rm, int tm, int bm)
{
if (left_margin != lm)
	left_margin = lm;
if (right_margin != rm)
	right_margin = rm;
if (top_margin != tm)
	top_margin = tm;
if (bottom_margin != bm)
	bottom_margin = bm;
}

void MultiLayer::setSpacing (int rgap, int cgap)
{
if (rowsSpace != rgap)
	rowsSpace = rgap;
if (colsSpace != cgap)
	colsSpace = cgap;
}

void MultiLayer::setLayerCanvasSize (int w, int h)
{
if (l_canvas_width != w)
	l_canvas_width = w;
if (l_canvas_height != h)
	l_canvas_height = h;
}

void MultiLayer::releaseLayer()
{
if (aux_rect.isNull())
	return;

active_graph->setGeometry(aux_rect);
active_graph->plotWidget()->resize(active_graph->size());

canvas->erase();
updateTransparency();
showLayers(true);//must be called for coloured background layers

highlightedLayer = false;
aux_rect = QRect();
cache_pix = QPixmap();

this->setCursor(Qt::arrowCursor);
emit modifiedPlot();
}

void MultiLayer::setAlignement (int ha, int va)
{
if (hor_align != ha)
	hor_align = ha;

if (vert_align != va)
	vert_align = va;
}

void MultiLayer::setLayersNumber(int n)
{
if (graphs == n)
	return;

int dn = graphs - n;
if (dn > 0)
	{
	for (int i = 0; i < dn; i++)
		{//remove layer buttons
		LayerButton *btn=(LayerButton*)buttonsList.last();	
		if (btn)
			{
			btn->close();
			buttonsList.removeLast();
			}
		
		Graph *g = (Graph *)graphsList.last();
		if (g)
			{//remove layers
			if (g->selectorsEnabled() || g->zoomOn() || g->removePointActivated() || 
			g->movePointsActivated() || g->enabledCursor()|| g->pickerActivated())
				setPointerCursor();

			g->close();
			graphsList.removeLast();
			}
		}
	graphs = n;
	if (!graphs)
		{
		active_graph = 0;
		return;
		}

	// check whether the active graph has been deleted
	if(graphsList.indexOf(active_graph) == -1)
		active_graph=(Graph*) graphsList.last();	
	for (int j=0;j<(int)graphsList.count();j++)
		{
		Graph *gr=(Graph *)graphsList.at(j);
		if (gr == active_graph)	
			{
			LayerButton *button=(LayerButton *)buttonsList.at(j);	
			button->setOn(TRUE);
			break;
			}
		}	
	hbox1->setMaximumWidth(graphs*((LayerButton *)buttonsList.at(0))->width());
	}	
else
	{
	for (int i = 0; i < abs(dn); i++)
		addLayer();
	}

emit modifiedPlot();
}


