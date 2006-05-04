#include "plot.h"
#include "scaleDraws.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale.h>

Plot::Plot(QWidget *parent, const char *name)
		: QwtPlot(parent,name)
{
movedGraph=FALSE;
graphToResize=FALSE;
ShiftButton=FALSE;

d_lineWidth = 1;		
minTickLength = 5;
majTickLength = 9;	

setGeometry(QRect(0,0,500,400));
setAutoLegend(FALSE); // We don't want a Qwt legend
setAxisTitle(0, tr("Y Axis Title"));
setAxisTitle(2, tr("X Axis Title"));	

//custom scale
for (int i= 0; i<QwtPlot::axisCnt; i++)
	{
	ticksType<<Out;

	QwtScale *scale = (QwtScale *) axis(i);
	if (scale)
		scale->setBaselineDist(0);
	
	ScaleDraw *sd= new ScaleDraw(1);
	setAxisScaleDraw (i, sd);
	}
	
QwtPlotLayout *pLayout=plotLayout();
pLayout->setCanvasMargin(0);

QwtPlotCanvas* plCanvas=canvas();
plCanvas->setFocusPolicy(QWidget::StrongFocus);
plCanvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
plCanvas->setFocus();
plCanvas->setFrameShadow( QwtPlot::Plain);
plCanvas->setCursor(Qt::arrowCursor);
plCanvas->setLineWidth(0);

setFocusPolicy(QWidget::StrongFocus);
setFocusProxy(plCanvas);

setFrameShape (QFrame::Box);
setFrameShadow(QFrame::Plain);
setLineWidth(0);

//setCanvasBackground (QColor(gray));
}

QColor Plot::frameColor()
{
return palette().color(QPalette::Active, QColorGroup::Foreground);
}

void Plot::printFrame(QPainter *painter, const QRect &rect) const
{
int lw = lineWidth();
if (!lw)
	return;

QPalette pal = palette();
QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);
		
painter->save();
painter->setPen (QPen(color, lw, Qt::SolidLine));
								
QwtPainter::drawRect(painter, rect.x(), rect.y(), rect.width(), rect.height());
painter->restore();
}

void Plot::printCanvas(QPainter *painter, const QRect &canvasRect,
    const QwtArray<QwtDiMap> &map, const QwtPlotPrintFilter &pfilter) const
{
	const QwtPlotCanvas* plotCanvas=canvas();	
	QRect rect=canvasRect;
	int w=plotCanvas->lineWidth();

	if (w>0)
    	{
		QPalette pal =plotCanvas->palette();
		QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);
		
		painter->save();
		painter->setPen (QPen(color,w,Qt::SolidLine));
				
		if (w == 1 && ticksType[QwtPlot::xBottom] == Plot::Out)
			rect.setHeight(canvasRect.height() + 1);	
						
		QwtPainter::drawRect(painter, rect.x(), rect.y(), rect.width(), rect.height());
		painter->restore();
   		}
	
	painter->setClipping(TRUE);
	rect = QRect(canvasRect.x()+1, canvasRect.y()+1, canvasRect.width(), canvasRect.height()-1);
	QwtPainter::setClipRect(painter, rect);

    drawCanvasItems(painter, canvasRect, map, pfilter);
}

void Plot::drawCanvasItems (QPainter *painter, const QRect &rect, 
							const QwtArray< QwtDiMap > &map, const QwtPlotPrintFilter &pfilter) const
{
QwtPlot::drawCanvasItems(painter, rect, map, pfilter);
	
for (int i=0; i<QwtPlot::axisCnt; i++)
 	{
	if (ticksType[i] == In)	
		drawInwardTicks(painter, rect, map[i], i);
	else if (ticksType[i] == Both)	
		drawInwardMinorTicks(painter, rect, map[i], i);
 	}
}

void Plot::drawInwardTicks(QPainter *painter, const QRect &rect, 
							const QwtDiMap &map, int axis) const
{
	QwtScale *scale=(QwtScale *) QwtPlot::axis (axis);
	if (!scale)
		return;
	
	int x1=rect.left();
	int x2=rect.right();
	int y1=rect.top();
	int y2=rect.bottom();
	
	QPalette pal=scale->palette();
	QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);
		
    painter->save();	
    painter->setPen(QPen(color,d_lineWidth,QPainter::SolidLine));
		
	QwtScaleDiv *scDiv=(QwtScaleDiv *)axisScale(axis);
	int minTicks=scDiv->minCnt();
	int majTicks= scDiv->majCnt();

int j, x, y, low,high;
switch (axis)
	{
	case QwtPlot::yLeft:
	x=x1;
	low=y1+majTickLength;
	high=y2-majTickLength;
    for (j = 0; j < minTicks; j++)
        {
            y = map.transform(scDiv->minMark(j));
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x, y, x+minTickLength, y);
        }		
	 for (j = 0; j < majTicks; j++)
        {
            y = map.transform(scDiv->majMark(j));
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x, y, x+majTickLength, y);
        }
	break;
		
	case QwtPlot::yRight:
		{
		x=x2;
		low=y1+majTickLength;
		high=y2-majTickLength;
     	for (j = 0; j < minTicks; j++)
       	 	{
            y = map.transform(scDiv->minMark(j));
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x+1, y, x-minTickLength, y);
        	}
		 for (j = 0; j <majTicks; j++)
        	{
            y = map.transform(scDiv->majMark(j));
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x+1, y, x-majTickLength, y);
        	}
		}
	  break;
		
	case QwtPlot::xBottom:
		y=y2;
		low=x1+majTickLength;
		high=x2-majTickLength;
     	for (j = 0; j < minTicks; j++)
        	{
            x = map.transform(scDiv->minMark(j));
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y+1, x, y-minTickLength);
       		 }	
	 	for (j = 0; j < majTicks; j++)
        	{
            x = map.transform(scDiv->majMark(j));
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y+1, x, y-majTickLength);
        	}
		break;
	
	case QwtPlot::xTop:
		y=y1;
		low=x1+majTickLength;
		high=x2-majTickLength;
    	 for (j = 0; j < minTicks; j++)
       		{
             x = map.transform(scDiv->minMark(j));
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y, x, y + minTickLength);
       	    }		
	 	for (j = 0; j <majTicks; j++)
        	{
            x = map.transform(scDiv->majMark(j));
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y, x, y + majTickLength);
        	}
	break;
	}
painter->restore();
}

void Plot::drawInwardMinorTicks(QPainter *painter, const QRect &rect, 
							const QwtDiMap &map, int axis) const
{
	QwtScale *scale=(QwtScale *) QwtPlot::axis (axis);
	if (!scale)
		return;
	
	int x1=rect.left();
	int x2=rect.right();	
	int y1=rect.top();
	int y2=rect.bottom();
	
	QPalette pal=scale->palette();
	QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);
		
    painter->save();	
    painter->setPen(QPen(color, d_lineWidth, QPainter::SolidLine));
		
	QwtScaleDiv *scDiv=(QwtScaleDiv *)axisScale(axis);
	int minTicks=scDiv->minCnt();

int j, x, y, low,high;
switch (axis)
	{
	case QwtPlot::yLeft:
	x=x1;
	low=y1+majTickLength;
	high=y2-majTickLength;
    for (j = 0; j < minTicks; j++)
        {
            y = map.transform(scDiv->minMark(j));
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x, y, x+minTickLength, y);
        }		
	break;
		
	case QwtPlot::yRight:
		{
		x=x2;
		low=y1+majTickLength;
		high=y2-majTickLength;
     	for (j = 0; j < minTicks; j++)
       	 	{
            y = map.transform(scDiv->minMark(j));
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x+1, y, x-minTickLength, y);
        	}
		}
	  break;
		
	case QwtPlot::xBottom:
		y=y2;
		low=x1+majTickLength;
		high=x2-majTickLength;
     	for (j = 0; j < minTicks; j++)
        	{
            x = map.transform(scDiv->minMark(j));
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y+1, x, y-minTickLength);
       		 }	
		break;
	
	case QwtPlot::xTop:
		y=y1;
		low=x1+majTickLength;
		high=x2-majTickLength;
    	 for (j = 0; j < minTicks; j++)
       		{
             x = map.transform(scDiv->minMark(j));
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y, x, y + minTickLength);
       	    }		
	break;
	}
painter->restore();
}

void Plot::setTicksLineWidth(int width)
{
if (d_lineWidth == width)
	return;

d_lineWidth = width;
}

void Plot::setTickLength (int minLength, int majLength)
{
if (majTickLength == majLength &&
	minTickLength == minLength)
	return;

majTickLength = majLength;
minTickLength = minLength;
}

void Plot::setTicksType(int axis, int type)
{
ticksType[axis]=type;
}

void Plot::mousePressEvent ( QMouseEvent * e )
{
if(e->state()==Qt::ShiftButton)
		ShiftButton=TRUE;
			
emit selectPlot();
}

void Plot::mouseMoveEvent ( QMouseEvent * e )
{
if(ShiftButton)
	{
	graphToResize=TRUE;
	emit resizeGraph(e->pos());	
	}				
else
	{
	movedGraph=TRUE;
	emit moveGraph(e->pos());
	}
}

void Plot::mouseReleaseEvent ( QMouseEvent *)
{
if (movedGraph)
	{
	emit releasedGraph();
	movedGraph=FALSE;
	}

if (graphToResize)
	{
	emit resizedGraph();
	graphToResize=FALSE;
	ShiftButton=FALSE;
	}
}


