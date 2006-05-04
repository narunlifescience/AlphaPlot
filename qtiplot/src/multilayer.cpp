#include <qspinbox.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qtextstream.h>
#include <qgrid.h>
#include <qrect.h> 
#include <qframe.h> 
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qaccel.h>
#include <qfileinfo.h> 
#include <qdir.h>
#include <qlistbox.h>
#include <qpixmap.h> 
#include <qkeycode.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <qtoolbutton.h> 
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qiconset.h>
#include <qmemarray.h>
#include <qvbox.h>
#include <qtabdialog.h>
#include <qbuttongroup.h>
#include <qregexp.h> 
#include <qcombobox.h>
#include <qvaluelist.h>
#include <qpen.h>
#include <qcolor.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qpixmapcache.h> 
#include <qstylesheet.h> 
#include <qdatetime.h> 
#include <qwmatrix.h> 
#include <qlabel.h> 
#include <qimage.h>
#include <qmessagebox.h>
#include <qclipboard.h>
#include <qdragobject.h>
#include <qbitmap.h>
#include <qwidgetlist.h>
#include <qpicture.h>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale.h>

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

QVBoxLayout *l = new QVBoxLayout( this, 0,0,"buttonLayout" );
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
			if (me->button()==QMouseEvent::RightButton)	
				emit showLayerMenu();
			else if (me->button()==QMouseEvent::LeftButton)	
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

MultiLayer::MultiLayer(const QString& label, QWidget* parent, const char* name, WFlags f)
        : myWidget(label,parent,name,f)
{
if ( !name )
	setName( "multilayer plot" );

setPaletteBackgroundColor( QColor(white) );

QDateTime dt = QDateTime::currentDateTime ();
setBirthDate(dt.toString(Qt::LocalDate));
	
graphs=0;cols=1;rows=1;
graph_width=500;graph_height=400;
colsSpace=5,rowsSpace=5;
active_graph=0;
addTextOn=FALSE;
movedGraph=FALSE;
resizedGraph=FALSE;
ChangeOrigin=FALSE;
	
buttonsList=new QWidgetList();
buttonsList->setAutoDelete( TRUE );
	
graphsList=new QWidgetList();
graphsList->setAutoDelete( TRUE );
	
hbox1=new QHBox(this, "hbox1"); 
LayerButton *button = new LayerButton(QString::number(1),hbox1,0);
hbox1->setFixedHeight(button->height());
setGeometry(QRect( 0, 0, graph_width, graph_height + button->height()));
delete button;
	
canvas=new QWidget (this, "canvas");
canvas->installEventFilter(this);

QVBoxLayout* layout = new QVBoxLayout(this,0,0, "hlayout3");
layout->addWidget(hbox1);
layout->addWidget(canvas);

setFocusPolicy(QWidget::StrongFocus);
setMouseTracking(TRUE);
}

LayerButton* MultiLayer::addLayerButton()
{
for (int i=0;i<(int)buttonsList->count();i++)
	{
	LayerButton *btn=(LayerButton*) buttonsList->at(i);
	btn->setOn(FALSE);
	}

LayerButton *button=new LayerButton(QString::number(++graphs),hbox1,0);
connect (button,SIGNAL(clicked(LayerButton*)),this, SLOT(activateGraph(LayerButton*)));
connect (button,SIGNAL(showLayerMenu()),this, SIGNAL(showWindowContextMenu()));
connect (button,SIGNAL(showCurvesDialog()),this, SIGNAL(showCurvesDialog()));
button->setOn(TRUE);
buttonsList->append(button);
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
graphsList->append(g);
return g;
}

Graph* MultiLayer::addLayer()
{
addLayerButton();
	
Graph* g = new Graph(canvas,0,WDestructiveClose);
g->setGeometry(QRect(0,0,graph_width, graph_height));
g->plotWidget()->resize(QSize(graph_width, graph_height));	
graphsList->append(g);
active_graph=g;
g->show();
connectLayer(g);
return g;
}

Graph* MultiLayer::addLayer(int x, int y, int width, int height)
{
addLayerButton();
	
Graph* g = new Graph(canvas,0,WDestructiveClose);
g->removeLegend();

QSize size=QSize(width,height);
g->plotWidget()->resize(size);
g->setGeometry(x,y,width,height);

graphsList->append(g);
active_graph=g;
g->show();
return g;
}

Graph* MultiLayer::addLayerToOrigin()
{
addLayerButton();
	
Graph* g = new Graph(canvas,0,0);
g->removeLegend();

int w = canvas->width();
int h = canvas->height();
g->setGeometry(QRect(0, 0, w, h));
g->plotWidget()->resize(QSize(w,h));
graphsList->append(g);
	
connectLayer(g);
active_graph=g;

makeTransparentLayer(g);
g->show();
emit modifiedPlot();
return g;
}

void MultiLayer::activateGraph(LayerButton* button)
{
for (int i=0;i<(int)buttonsList->count();i++)
	{
	LayerButton *btn=(LayerButton*)buttonsList->at(i);
	if (btn->isOn())
		btn->setOn(FALSE);
	if (btn ==button)	
		{
		active_graph=(Graph*) graphsList->at(i);
		active_graph->setFocus();
		button->setOn(TRUE);	
		}
	}
}

void MultiLayer::setActiveGraph(Graph* g)
{
int i;
if (g)
	{
	active_graph=g;
	active_graph->setFocus();

	LayerButton *btn=0;
	for (i=0;i<(int)buttonsList->count();i++)
		{
		 btn=(LayerButton*)buttonsList->at(i);
		 btn->setOn(FALSE);
		}
	
	for (i=0;i<(int)graphsList->count();i++)
		{
		Graph *gr=(Graph *)graphsList->at(i);
		if (gr ==g)	
			{
			 btn=(LayerButton *)buttonsList->at(i);
			 btn->setOn(TRUE);
			 return;
			}
		}
	}
}

void MultiLayer::contextMenuEvent(QContextMenuEvent *e)
{
emit showWindowContextMenu();
e->accept();
}

void MultiLayer::closeEvent( QCloseEvent *e )
{
if (confirmClose())
 {
 switch( QMessageBox::information(0,tr("QtiPlot"),
				      tr("Do you want to hide or delete <p><b>'"
					   + QString(name()) +"'</b> ?"),
				      "Delete", "Hide", "Cancel",
				      0,2) ) 
	{
    case 0:	
	 emit closedPlot(this);
	 e->accept(); 
	break;

    case 1:
	 emit hidePlot(this);
     e->ignore();
	break;

	case 2:
	  e->ignore();
	break;
    }  
	}
else
	{
	emit closedPlot(this);
	e->accept(); 
	}
}

bool MultiLayer::resizeLayers (const QResizeEvent *re)
{
QSize oldSize=re->oldSize();
QSize size=re->size();

if ( size == oldSize || maxSize().width() > oldSize.width() )
	return false;

QApplication::setOverrideCursor(waitCursor);

double w_ratio=(double)size.width()/(double)oldSize.width();
double h_ratio=(double)(size.height())/(double)(oldSize.height());

for (int i=0;i<(int)graphsList->count();i++)
	{
	Graph *gr=(Graph *)graphsList->at(i);
	if (gr && gr->isVisible() && !gr->ignoresResizeEvents())
		{
		int gx = qRound(gr->x()*w_ratio);
		int gy = qRound(gr->y()*h_ratio);
		int gw = qRound(gr->width()*w_ratio);
		int gh = qRound(gr->height()*h_ratio);
		
		gr->setGeometry(QRect(gx,gy,gw,gh));
		gr->plotWidget()->resize(QSize(gw, gh));	
		gr->resizeMarkers(w_ratio, h_ratio);
		}
	}

if (hasOverlapingLayers())
	updateTransparency();

emit modifiedPlot();
emit resizedPlot(this);

QApplication::restoreOverrideCursor();
return TRUE; 
}

void MultiLayer::confirmRemoveLayer()
{	
if (graphs>1)
	{
	switch(QMessageBox::information(this,
            tr("QtiPlot - Guess best origin for the new layer?"),
            tr("Do you want QtiPlot to rearrange the remaining layers?"),
            tr("&Yes"), tr("&No"), tr("&Cancel"),
            0, 2) )
			{
			case 0:
				removeLayer();
				arrangeLayers(0, 0, 5, 5, true);
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
for (i=0;i<(int)buttonsList->count();i++)
	{
	btn=(LayerButton*)buttonsList->at(i);	
	if (btn->isOn())	
		{
		buttonsList->take(buttonsList->find(btn));
		btn->close(TRUE);			
		break;
		}
	}
	
//update the texts of the buttons	
for (i=0;i<(int)buttonsList->count();i++)
	{
	 btn=(LayerButton*)buttonsList->at(i);	
	 btn->setText(QString::number(i+1));
	}
	
if (active_graph->selectorsEnabled() || active_graph->zoomOn() || 
	active_graph->removePointActivated() || active_graph->movePointsActivated() || 
	active_graph->enabledCursor()|| active_graph->pickerActivated())
		{
		setPointerCursor();
		}		
		
graphsList->take (graphsList->find(active_graph));
active_graph->close();
graphs--;
		
if (graphs == 0)
	{
	active_graph = 0;
	return;
	}

active_graph=(Graph*) graphsList->current();	

for (i=0;i<(int)graphsList->count();i++)
	{
	Graph *gr=(Graph *)graphsList->at(i);
	if (gr == active_graph)	
		{
		LayerButton *button=(LayerButton *)buttonsList->at(i);	
		button->setOn(TRUE);
		break;
		}
	}	
	
if (graphs)
	hbox1->setMaximumWidth(graphs*((LayerButton *)buttonsList->at(0))->width());

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
	
QPoint aux;
int w=canvas->width();
int h=canvas->height();

if (!movedGraph)
	{
	movedGraph=TRUE;
	QPixmap pix=pix.grabWidget(canvas,0,0,-1,-1 );
	QPixmapCache::insert ("QTIPLOT_multilayer",pix);

	for (int i=0;i<(int)graphsList->count();i++)
		{
		Graph *gr=(Graph *)graphsList->at(i);
		gr->hide();
		}

	xMouse=pos.x();
	yMouse=pos.y();
		
	aux=g->pos();
	xActiveGraph=aux.x();
	yActiveGraph=aux.y();
	}
	
QPixmap pix(w,h,-1);
pix.fill( QColor(white));
QPixmapCache::find ("QTIPLOT_multilayer",pix);
	
QPainter painter(&pix);
painter.setRasterOp(Qt::NotROP);

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

QPixmap pix(canvas->width(), canvas->height(), -1);
pix.fill( QColor(white));
bitBlt( canvas, 0, 0, &pix, 0, 0, -1, -1 );

g->setGeometry(QRect(QPoint(xActiveGraph,yActiveGraph),g->size()));	
g->plotWidget()->resize(g->size());
for (int i=0;i<(int)graphsList->count();i++)
	{
	Graph *gr=(Graph *)graphsList->at(i);
	if (gr->plotWidget()->paletteBackgroundColor() == QColor(white))
		makeTransparentLayer(gr);
	gr->show();
	}
	
movedGraph=FALSE;
QPixmapCache::remove ("QTIPLOT_multilayer");	
}

void MultiLayer::newSizeGraph(Graph* g)
{
if(!resizedGraph)
	{
	int w=canvas->width();
	int h=canvas->height();

	QPixmap pix(w,h,-1);
	pix.fill( QColor(white));
	bitBlt( canvas, 0, 0,&pix, 0, 0, -1, -1 );
	QPixmapCache::remove ("QTIPLOT_multilayer");
	}
else 
	{
	int w=canvas->width();
	int h=canvas->height();

	QPixmap pix(w,h,-1);
	pix.fill( QColor(white));
	bitBlt( canvas, 0, 0,&pix, 0, 0, -1, -1 );

	g->show();
	g->setGeometry(QRect(QPoint(xInt,yInt),QSize(oldw,oldh)));
	g->plotWidget()->resize(QSize(oldw,oldh));
	g->hide();
	for (int i=0;i<(int)graphsList->count();i++)
		{
		Graph *gr=(Graph *)graphsList->at(i);

		if (gr->plotWidget()->paletteBackgroundColor() == QColor(white))
			makeTransparentLayer(gr);

		gr->show();
		}
	resizedGraph=FALSE;
	ChangeOrigin=FALSE;
	QPixmapCache::remove ("QTIPLOT_multilayer");
	}
this->setCursor(arrowCursor);
}

void MultiLayer::resizeGraph(Graph* g, const QPoint& pos)
{	
yesResize=FALSE;
QPoint aux;
// Get the coordinates of the layout, within which draging the  mouse will resize the plot
// These coordinates actually define the size of the plot. 
// We have, however, a margin of 3 pixels. These are also named virtual borders

xlb=1; ytb=5; 
if(!resizedGraph)
{
oldw=g->plotWidget()->size().width();
oldh=g->plotWidget()->size().height();

xrb=g->plotWidget()->canvas()->size().width()-3; 
ybb=g->plotWidget()->canvas()->size().height()-3;

Save_oldh=oldh;
Save_oldw=oldw;

QPixmap pix(oldw+10,oldh+10,-1);
pix=pix.grabWidget(canvas,0,0,-1,-1);
QPixmapCache::insert ("QTIPLOT_multilayer",pix);

QPainter painter(&pix);
painter.setRasterOp(Qt::NotROP);

painter.drawRect(QRect(QPoint(g->pos().x()-5,g->pos().y()-5),QSize(oldw+10,oldh+10)));
painter.end();

bitBlt( canvas, 0, 0,&pix, 0, 0, -1, -1 );
}

if(!resizedGraph) // This is the first time we press the shift button. We will start resizing only if
//we are close to the virtual borders
	{
	if(abs(pos.x()-xlb)<2 || abs(pos.x()-xrb)<4)
		{
		this->setCursor(SizeHorCursor);
		yesResize=TRUE;
		}
	if(abs(pos.y()-ytb)<4 || abs(pos.y()-ybb)<4) 
		{
		this->setCursor(SizeVerCursor);
		yesResize=TRUE;
		}
	if((abs(pos.y()-ytb)<2 && abs(pos.x()-xlb)<2) || (abs(pos.y()-ybb)<2 && abs(pos.x()-xrb)<2 ))
		{
		this->setCursor(SizeFDiagCursor);
		yesResize=TRUE;
		}
	if((abs(pos.y()-ytb)<2 && abs(pos.x()-xrb)<2) || (abs(pos.y()-ybb)<2 && abs(pos.x()-xlb)<2) )
		{
		this->setCursor(SizeBDiagCursor);
		yesResize=TRUE;
		}
	}
else // In the second time, we do not need to check how close we are to the virtual borders. We determine
// how will resize the layout by the difference in the old and new position of the mouse
	{
	int deltax=pos.x()-xrMouse;
	int deltay=pos.y()-yrMouse;

	if(deltax!=0 && deltay==0)
	this->setCursor(SizeHorCursor);
	
	else if(deltax==0 && deltay!=0) 
	this->setCursor(SizeVerCursor);
	
	else if( (deltax>0 && deltay>0) || (deltax<0 && deltay<0))
	this->setCursor(SizeFDiagCursor);
	
	else if( (deltax<0 && deltay>0) || (deltax>0 && deltay<0))
	this->setCursor(SizeBDiagCursor);
	}

if(!resizedGraph) // First time
	{
	if(yesResize)
		{
		// Do we resize it from left or top?
		if(abs(pos.y()-ytb)<4 || abs(pos.x()-xlb)<4 || abs(pos.y()-ytb)<2 && abs(pos.x()-xlb)<2 || abs(pos.y()-ytb)<2 && abs(pos.x()-xrb)<2  ||  (abs(pos.y()-ybb)<2 && abs(pos.x()-xlb)<2) )
		ChangeOrigin=TRUE;
		
		resizedGraph=TRUE;
		// Get the initial location of the mouse
		xrMouse=pos.x();
		yrMouse=pos.y();
		QPixmap pix(oldw,oldh,-1);
		pix=pix.grabWidget(canvas,0,0,-1,-1);
		QPixmapCache::insert ("QTIPLOT_multilayer",pix);
		for (int i=0;i<(int)graphsList->count();i++)
			{
			Graph *gr=(Graph *)graphsList->at(i);
			gr->hide();
			}
	
		aux=g->pos();
		xInt=aux.x();
		yInt=aux.y();
		}
	}
else // What happens in later times
	{
	int deltax=pos.x()-xrMouse;
	int deltay=pos.y()-yrMouse;
	
	if(deltax!=0 &&deltay==0)
		{
		if(!ChangeOrigin)
			oldw+=deltax;
		else if(ChangeOrigin) 
			{
			oldw-=deltax;
			xInt+=deltax;
			}
		oldh=Save_oldh;
		Save_oldw=oldw;
		}
	else if(deltax==0 && deltay!=0)
		{
		if(!ChangeOrigin)
			oldh+=deltay;
		else if(ChangeOrigin)
			{
			oldh-=deltay;
			yInt+=deltay;
			}
		oldw=Save_oldw;
		Save_oldh=oldh;
		}
	else if(deltax!=0 && deltay!=0)
		{
		if(!ChangeOrigin)
			{
			oldh+=deltay;
			oldw+=deltax;
			}
		else if(ChangeOrigin)
			{
			oldh-=deltay;
			oldw-=deltax;
			yInt+=deltay;
			xInt+=deltax;
			}
		Save_oldw=oldw;
		Save_oldh=oldh;
		}
		
	else if(deltax==0 && deltay==0)
		{
		oldw=Save_oldw;
		oldh=Save_oldh;
		}

	QPixmap pix(oldw,oldh,-1);
	pix.fill( QColor(white));
	QPixmapCache::find ("QTIPLOT_multilayer",pix);

	QPainter painter(&pix);
	painter.setRasterOp(Qt::NotROP);

	painter.drawRect(QRect(QPoint(xInt,yInt),QSize(oldw,oldh)));
	painter.end();

	bitBlt( canvas, 0, 0,&pix, 0, 0, -1, -1 );
	
	xrMouse=pos.x();
	yrMouse=pos.y();

	emit modifiedPlot();
	Save_oldw=oldw;
	Save_oldh=oldh;
	}
}

void MultiLayer::arrangeLayers(int c, int r, int colsGap, int rowsGap)
{
QApplication::setOverrideCursor(waitCursor);
int i;

cols=c;
rows=r;
colsSpace=colsGap;
rowsSpace=rowsGap;

int iterations = 0;
QSize maxSize = calculateMaxCanvasSize(c, r);
QSize canvas_size = QSize(1,1);
while (canvas_size != maxSize && iterations < 10)
	{
	iterations++;
	canvas_size = maxSize;
	maxSize = calculateMaxCanvasSize(c, r);
	}

bool arrangeFailed = false;		
for (i=0; i<graphs; i++)
	{
	Graph *gr=(Graph *)graphsList->at(i);
	QwtPlotLayout *pl = gr->plotWidget()->plotLayout();
	QSize size = pl->canvasRect().size();

	if (size.width() < 10 || size.height() < 10)
		{
		arrangeFailed = true;
		break;
		}
	}

updateTransparency();

emit modifiedPlot();
QApplication::restoreOverrideCursor();

if (arrangeFailed)
	{
	QMessageBox::warning(this,tr("QtiPlot - Error: arranging layers failed!"),
				tr("There is not enaugh space available in this window."
				"<p>You could try to maximize it first and to rearrange the layers using the automatic option!</p>"));
	}
}

QSize MultiLayer::calculateMaxCanvasSize(int c, int r)
{
const QRect rect=canvas->geometry();
int margin = 5;
int margin2 = 10;

gsl_vector *xTopR = gsl_vector_calloc (graphs);
gsl_vector *xBottomR = gsl_vector_calloc (graphs);
gsl_vector *yLeftR = gsl_vector_calloc (graphs);
gsl_vector *yRightR = gsl_vector_calloc (graphs);
gsl_vector *maxXTopHeight = gsl_vector_calloc (r);
gsl_vector *maxXBottomHeight = gsl_vector_calloc (r);
gsl_vector *maxYLeftWidth = gsl_vector_calloc (c);
gsl_vector *maxYRightWidth = gsl_vector_calloc (c);
gsl_vector *Y = gsl_vector_calloc (r);			
gsl_vector *X = gsl_vector_calloc (c);

int i;
for (i=0; i<graphs; i++)
	{//calculate scales/canvas dimensions reports for each layer and stores them in the above vectors
	Graph *gr=(Graph *)graphsList->at(i);
	QwtPlot *plot=gr->plotWidget();
	QwtPlotLayout *plotLayout=plot->plotLayout();
	QRect cRect=plotLayout->canvasRect();			
	double ch = (double) cRect.height();
	double cw = (double) cRect.width();
	
	QRect tRect=plotLayout->titleRect ();
	QwtScale *scale=(QwtScale *) plot->axis (QwtPlot::xTop);

	int topHeight = 0;
    if (!tRect.isNull())
		topHeight+=tRect.height() + plotLayout->spacing();		
	if (scale)
		{
		QRect sRect=plotLayout->scaleRect (QwtPlot::xTop);
		topHeight+= sRect.height();
		}
	gsl_vector_set (xTopR, i, double(topHeight)/ch);
		
	scale=(QwtScale *) plot->axis (QwtPlot::xBottom);			
	if (scale)
		{
		QRect sRect=plotLayout->scaleRect (QwtPlot::xBottom);
		gsl_vector_set (xBottomR, i, double(sRect.height())/ch);
		}	
			
	scale=(QwtScale *) plot->axis (QwtPlot::yLeft);			
	if (scale)
		{
		QRect sRect=plotLayout->scaleRect (QwtPlot::yLeft);
		gsl_vector_set (yLeftR, i, double(sRect.width())/cw);
		}	
			
	scale=(QwtScale *) plot->axis (QwtPlot::yRight);			
	if (scale)
		{
		QRect sRect=plotLayout->scaleRect (QwtPlot::yRight);
		gsl_vector_set (yRightR, i, double(sRect.width())/cw);
		}
	
//calculate max scales/canvas dimensions ratio for each line and column and stores them to vectors	
	int row = i / c;
	if (row >= r )
		row = r - 1;

	int col = i % c;
		
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
for (i=0; i<r; i++)
	{
	gsl_vector_set (Y, i, c_heights);	
	c_heights+= 1 + gsl_vector_get(maxXTopHeight, i) + gsl_vector_get(maxXBottomHeight, i);	
	}
		
double c_widths = 0.0;
for (i=0; i<c; i++)
	{
	gsl_vector_set (X, i, c_widths);	
	c_widths+= 1 + gsl_vector_get(maxYLeftWidth, i) + gsl_vector_get(maxYRightWidth, i);	
	}

const int canvas_width=int((rect.width()-(c-1)*colsSpace - margin2)/c_widths);
const int canvas_height=int((rect.height()-(r-1)*rowsSpace - margin2)/c_heights);

QSize size = QSize(canvas_width, canvas_height);

for (i=0; i<graphs; i++)
	{	
	int row = i / c;
	if (row >=r )
		row = r - 1;
		
	int col = i % c;	
		
	//calculate sizes and positions for layers
	const int w =int (canvas_width*(1 + gsl_vector_get(yLeftR, i) + gsl_vector_get(yRightR, i)));
	const int h =int (canvas_height*(1 + gsl_vector_get(xTopR, i) + gsl_vector_get(xBottomR, i)));
	
	int x = margin + col*colsSpace + int (canvas_width*
			(gsl_vector_get(X, col) + gsl_vector_get(maxYLeftWidth, col) - gsl_vector_get(yLeftR, i))) ;
				
	int y = margin + row*rowsSpace + int (canvas_height*
			(gsl_vector_get(Y, row) + gsl_vector_get(maxXTopHeight, row) - gsl_vector_get(xTopR, i)));
						
	//resizes and moves layers
	Graph *gr=(Graph *)graphsList->at(i);		
	gr->setGeometry(QRect(x, y, w, h));
	gr->plotWidget()->resize(QSize(w, h));
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

void MultiLayer::arrangeLayers(int c, int r, int colsGap, int rowsGap, bool fit)
{
if (fit)
	{
	int rows, cols;
	findBestLayout(rows, cols);	

	double canvas_width=double(canvas->width()-(cols-1)*colsSpace - 10)/(double)cols;
	double canvas_height=double(canvas->height()-(rows-1)*rowsSpace - 10)/(double)rows;;

	QSize layer_size = QSize((int)canvas_width, (int)canvas_height);
	for (int i=0; i<graphs; i++)
		{
		Graph *gr=(Graph *)graphsList->at(i);
		gr->resize(layer_size);
		}

	arrangeLayers(cols, rows, colsGap, rowsGap);
	}
else
	arrangeLayers(c, r, colsGap, rowsGap);	
}

QSize MultiLayer::maxSize()
{
int w=0, h=0, maxw=0, maxh=0;	
for (int i=0;i<(int)graphsList->count();i++)
	{
	Graph *gr=(Graph *)graphsList->at(i);
	w = gr->pos().x() + gr->frameGeometry().width();

	if (maxw <= w)
		maxw = w;
	
	h = gr->pos().y() + gr->frameGeometry().height();
	if (maxh <= h)
		maxh = h;
	}	
return QSize(maxw,maxh);
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
QPixmap pic(size.width(), size.height(), -1, QPixmap::BestOptim);
pic.fill (Qt::white);
	
QPainter paint;
paint.begin(&pic);

QwtPlotPrintFilter filter;
filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle |
				   QwtPlotPrintFilter::PrintCanvasBackground);
	
for (int i=0;i<(int)graphsList->count();i++)
		{
		Graph *gr=(Graph *)graphsList->at(i);
		Plot *myPlot= gr->plotWidget();

		int lw = myPlot->lineWidth();
		QRect rect = QRect(gr->x() + lw, gr->y() + lw, myPlot->width() - 2*lw, 
						  myPlot->height() - 2*lw);

		if (myPlot->paletteBackgroundColor() != QColor(white))
			paint.fillRect(rect, myPlot->paletteBackgroundColor());

		myPlot->print(&paint, rect, filter);

		if (lw > 0)
			myPlot->printFrame(&paint, rect);
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
				{
				p.drawPoint( x, y );
				}
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
printer.setResolution(res);
printer.setPageSize(pageSize);
printer.setColorMode(col);
printer.setOrientation(o);
	
printer.setFullPage(TRUE);
printer.setOutputToFile (TRUE);
printer.setOutputFileName(fname);

QPainter paint(&printer);
QPaintDeviceMetrics pdmTo(&printer);

int dpiy = pdmTo.logicalDpiY();
int margin = (int) ( (0.5/2.54)*dpiy ); // 5 mm margins
	
QSize size = canvas->size();

double scaleFactorX=(double)(pdmTo.width() - 2*margin)/(double)size.width();
double scaleFactorY=(double)(pdmTo.height() - 2*margin)/(double)size.height();
	
for (int i=0;i<(int)graphsList->count();i++)
		{
		Graph *gr=(Graph *)graphsList->at(i);
		Plot *myPlot= (Plot *)gr->plotWidget();
			
		PrintFilter  filter(myPlot); 
	    filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle 
						| QwtPlotPrintFilter::PrintCanvasBackground);

		QPoint pos=gr->pos();
		pos=QPoint(int(margin + pos.x()*scaleFactorX),int(margin + pos.y()*scaleFactorY));
	
		int width=int(myPlot->frameGeometry().width()*scaleFactorX);
		int height=int(myPlot->frameGeometry().height()*scaleFactorY);

		QRect rect = QRect(pos,QSize(width,height));

		if (myPlot->paletteBackgroundColor() != QColor(white))
			paint.fillRect(rect, myPlot->paletteBackgroundColor());

		int lw = myPlot->lineWidth();
		if ( lw > 0)
			{			
			myPlot->printFrame(&paint, rect);
				
			rect.moveBy ( lw, lw);
			rect.setWidth(rect.width() - 2*lw);
			rect.setHeight(rect.height() - 2*lw);
			}
		
		myPlot->print(&paint, rect, filter);
		}

if (hasOverlapingLayers())		
	updateTransparency();
}

void MultiLayer::copyAllLayers()
{
QPixmap pic = canvasPixmap();	
QImage image= pic.convertToImage();		
QApplication::clipboard()->setData( new QImageDrag (image,canvas,0) );	
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
printer.setResolution(84);
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

QPaintDeviceMetrics metrics(painter->device());
	
int dpiy = metrics.logicalDpiY();
int margin = (int) ( (1/2.54)*dpiy ); // 1 cm margins
	
QSize size=maxSize();
double scaleFactorX=(double)(metrics.width()-2*margin)/(double)size.width();
double scaleFactorY=(double)(metrics.height()-2*margin)/(double)size.height();
		
for (int i=0; i<(int)graphsList->count(); i++)
	{			
	Graph *gr=(Graph *)graphsList->at(i);
	Plot *myPlot= gr->plotWidget();
			
	PrintFilter  filter(myPlot); 
    filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle |
				      QwtPlotPrintFilter::PrintCanvasBackground);

	QPoint pos=gr->pos();
	pos=QPoint(margin + int(pos.x()*scaleFactorX), margin + int(pos.y()*scaleFactorY));
			
	int width=int(myPlot->frameGeometry().width()*scaleFactorX);
	int height=int(myPlot->frameGeometry().height()*scaleFactorY);

	QRect rect = QRect(pos,QSize(width,height));
	if (myPlot->paletteBackgroundColor() != QColor(white))
		painter->fillRect(rect, myPlot->paletteBackgroundColor());

    int lw = myPlot->lineWidth();
	if ( lw > 0)
		{			
		myPlot->printFrame(painter, rect);
				
		rect.moveBy (lw, lw);
		rect.setWidth(rect.width() - 2*lw);
		rect.setHeight(rect.height() - 2*lw);
		}
	
	myPlot->print(painter, rect, filter);
	}

if (hasOverlapingLayers())
	updateTransparency();
}

void MultiLayer::setFonts(const QFont& titleFnt, const QFont& scaleFnt,
												const QFont& numbersFnt, const QFont& legendFnt)
{
for (int i=0;i<(int)graphsList->count();i++)
		{
		Graph *gr=(Graph *)graphsList->at(i);
		QwtPlot *plot=gr->plotWidget();
			
		plot->setTitleFont(titleFnt);
		for (int j= 0;j<QwtPlot::axisCnt;j++)
			{
			plot->setAxisFont (j,numbersFnt);
			plot->setAxisTitleFont (j,scaleFnt);
			}
			
		QwtArray<long> keys=gr->textMarkerKeys();
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
	
QLabel *title=plot->titleLabel ();
QRect tRect=plotLayout->titleRect ();
if (!tRect.isNull())
	{
	pix = QPixmap::grabWidget (canvas, x + tRect.x(), y + tRect.y(), tRect.width(), tRect.height());		
	title->setPaletteBackgroundPixmap(pix);
	}

for (int i=0;i<QwtPlot::axisCnt;i++)
	{
	QwtScale *scale=(QwtScale *) plot->axis (i);
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
int i;
int graphs=(int)graphsList->count();
bool allColored = true;
for (i=0;i<graphs;i++)
	{
	Graph *gr=(Graph *)graphsList->at(i);
	QColor c = gr->plotWidget()->paletteBackgroundColor();
	if (c == QColor(white))
		{
		allColored = false;
		break;
		}
	}
if (allColored)
	return;

QApplication::setOverrideCursor(waitCursor);

QPixmap pic = canvasPixmap();
bitBlt( canvas, 0, 0, &pic, 0, 0, -1, -1 );
	
for (i=0;i<graphs;i++)
	{
	Graph *gr=(Graph *)graphsList->at(i);
	gr->hide();
	}

for (i=0; i<graphs; i++)
	{	
	Graph *gr=(Graph *)graphsList->at(i);
	if (gr->plotWidget()->paletteBackgroundColor() == QColor(white))
		makeTransparentLayer(gr);
	gr->show();
	}
QApplication::restoreOverrideCursor();
}		

bool MultiLayer::hasOverlapingLayers()
{
for (int i=0;i<(int)graphsList->count();i++)
	{
	Graph *gr=(Graph *)graphsList->at(i);
	if (overlapsLayers(gr))
		return true;
	}
		
return false;
}

bool MultiLayer::overlapsLayers(Graph *g)
{
int  s=0;
QRect r= g->frameGeometry();
for (int i=0;i<(int)graphsList->count();i++)
		{
		Graph *gr=(Graph *)graphsList->at(i);
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
connect (g,SIGNAL(drawTextOff()),this,SIGNAL(drawTextOff()));
connect (g,SIGNAL(showPlotDialog(long)),this,SIGNAL(showPlotDialog(long)));
connect (g,SIGNAL(createTable(const QString&,int,int,const QString&)),this,SIGNAL(createTable(const QString&,int,int,const QString&)));
connect (g,SIGNAL(viewLineDialog()),this,SIGNAL(showLineDialog()));
connect (g,SIGNAL(showContextMenu()),this,SIGNAL(showGraphContextMenu()));
connect (g,SIGNAL(showAxisDialog(int)),this,SIGNAL(showAxisDialog(int)));
connect (g,SIGNAL(axisDblClicked(int)),this,SIGNAL(showScaleDialog(int)));
connect (g,SIGNAL(xAxisTitleDblClicked()),this,SIGNAL(showXAxisTitleDialog()));		
connect (g,SIGNAL(yAxisTitleDblClicked()),this,SIGNAL(showYAxisTitleDialog()));
connect (g,SIGNAL(rightAxisTitleDblClicked()),this,SIGNAL(showRightAxisTitleDialog()));
connect (g,SIGNAL(topAxisTitleDblClicked()),this,SIGNAL(showTopAxisTitleDialog()));
connect (g,SIGNAL(showMarkerPopupMenu()),this,SIGNAL(showMarkerPopupMenu()));
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

//when resizing a layer by mouse-dragging
connect (g,SIGNAL(resizeGraph(Graph*, const QPoint& )),this, SLOT(resizeGraph(Graph*, const QPoint&)));
connect (g,SIGNAL(newSizeGraph(Graph*)),this, SLOT(newSizeGraph(Graph*)));
}

void MultiLayer::addTextLayer()
{
addTextOn=TRUE;
QApplication::setOverrideCursor(IbeamCursor);
canvas->grabMouse();
}

void MultiLayer::addTextLayer(const QPoint& pos)
{
Graph* g=addLayer();
g->setTitle("");
QMemArray<bool> axesOn(4);
for (int j=0;j<4;j++)
		axesOn[j]=FALSE;
g->enableAxes(axesOn);
g->plotWidget()->setLineWidth(1);
g->setIgnoreResizeEvents(true);
QSize size=g->newLegend(tr("enter your text here"));
setGraphGeometry(pos.x(), pos.y(), size.width()+10, size.height()+10);
g->setIgnoreResizeEvents(false);
g->show();
connectLayer(g);
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
			
		if (me->button()==QMouseEvent::LeftButton && addTextOn)	
			addTextLayer(me->pos());
	
		return TRUE; 
		}
			
	case QEvent::Resize:
		{
		const QResizeEvent *re = (const QResizeEvent *)e;
		resizeLayers(re);
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
	int index=graphsList->findRef ((QWidget *)active_graph);
	Graph *g=(Graph *)graphsList->at(index+1);
	if (g)
		setActiveGraph(g);
	}
	
if (e->key() == Qt::Key_F10)	
	{
	int index=graphsList->findRef ((QWidget *)active_graph);
	Graph *g=(Graph *)graphsList->at(index-1);
	if (g)
		setActiveGraph(g);
	}
	
if (e->key() == Qt::Key_F11)
	emit showWindowContextMenu();
}

void MultiLayer::wheelEvent ( QWheelEvent * e ) 
{
QApplication::setOverrideCursor(waitCursor);

bool resize=FALSE;
QPoint aux;
QSize intSize;
Graph *resize_graph;
// Get the position of the mouse
xMouse=e->x();
yMouse=e->y();
for (int i=0;i<(int)graphsList->count();i++)
	{
	Graph *gr=(Graph *)graphsList->at(i);
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

QString MultiLayer::saveAsTemplate(const QString& geometryInfo) 
{
QString s="<multiLayer>\t";
s+=QString::number(rows)+"\t";
s+=QString::number(cols)+"\n";
s+= geometryInfo;
for (int j=0;j<(int)graphsList->count();j++)
	{
	Graph* ag=(Graph*)graphsList->at(j);
	s+=ag->saveAsTemplate();
	}
return s;
}
