/***************************************************************************
    File                 : plot.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Plot window class
                           
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
#include "plot.h"
#include "scales.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_map.h>

#include <qapplication.h>
#include <qpixmap.h>

#include "graph.h"

Plot::Plot(QWidget *parent, const char *name)
		: QwtPlot(parent)
{
marker_key = 0;
curve_key = 0;

minTickLength = 5;
majTickLength = 9;

movedGraph=FALSE;
graphToResize=FALSE;
ShiftButton=FALSE;

setGeometry(QRect(0,0,500,400));
setAxisTitle(0, tr("Y Axis Title"));
setAxisTitle(2, tr("X Axis Title"));	

// grid 
d_grid = new QwtPlotGrid;
d_grid->enableX(false);
d_grid->enableY(false);
d_grid->setMajPen(QPen(Qt::blue, 0, Qt::SolidLine));
d_grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
d_grid->attach(this);

//custom scale
for (int i= 0; i<QwtPlot::axisCnt; i++)
	{
	majorTicksType<<Out;
	minorTicksType<<Out;

	QwtScaleWidget *scale = (QwtScaleWidget *) axisWidget(i);
	if (scale)
		{
		scale->setMargin(0);

		ScaleDraw *sd = new ScaleDraw();
		 
		sd->setTickLength  	(QwtScaleDiv::MinorTick, minTickLength); 
		sd->setTickLength  	(QwtScaleDiv::MediumTick, minTickLength);
		sd->setTickLength  	(QwtScaleDiv::MajorTick, majTickLength);

		setAxisScaleDraw (i, sd);
		}
	}
	
QwtPlotLayout *pLayout=plotLayout();
pLayout->setCanvasMargin(0);

QwtPlotCanvas* plCanvas=canvas();
plCanvas->setFocusPolicy(Qt::StrongFocus);
plCanvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
plCanvas->setFocus();
plCanvas->setFrameShadow( QwtPlot::Plain);
plCanvas->setCursor(Qt::arrowCursor);
plCanvas->setLineWidth(0);

setFocusPolicy(Qt::StrongFocus);
setFocusProxy(plCanvas);

setFrameShape (QFrame::Box);
setFrameShadow(QFrame::Plain);
setLineWidth(0);
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

/*void Plot::printCanvas(QPainter *painter, const QRect &canvasRect,
    const QwtArray<QwtScaleMap> &map, const QwtPlotPrintFilter &pfilter) const
{
	const QwtPlotCanvas* plotCanvas=canvas();	
	QRect rect=canvasRect;
	int w=plotCanvas->lineWidth();

	if (w>0)
    	{
		QPalette pal = plotCanvas->palette();
		QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);
		
		painter->save();
		painter->setPen (QPen(color,w,Qt::SolidLine));
				
		if (w == 1 && majorTicksType[QwtPlot::xBottom] == Plot::Out)
			rect.setHeight(canvasRect.height() + 1);	
						
		QwtPainter::drawRect(painter, rect.x(), rect.y(), rect.width(), rect.height());
		painter->restore();
   		}
	
	painter->setClipping(TRUE);
	rect = QRect(canvasRect.x()+1, canvasRect.y()+1, canvasRect.width(), canvasRect.height()-1);
	QwtPainter::setClipRect(painter, rect);

    drawItems(painter, canvasRect, map, pfilter);
}*/

void Plot::drawItems (QPainter *painter, const QRect &rect, 
							const QwtArray< QwtScaleMap > &map, const QwtPlotPrintFilter &pfilter) const
{
QwtPlot::drawItems(painter, rect, map, pfilter);
	
for (int i=0; i<QwtPlot::axisCnt; i++)
 	{
	if ((majorTicksType[i] == In || majorTicksType[i] == Both) &&
		(minorTicksType[i] == In || minorTicksType[i] == Both))	
		drawInwardTicks(painter, rect, map[i], i);
	else 
		{
		if (majorTicksType[i] == In || majorTicksType[i] == Both)	
			drawInwardMajorTicks(painter, rect, map[i], i);

		if (minorTicksType[i] == In || minorTicksType[i] == Both)	
			drawInwardMinorTicks(painter, rect, map[i], i);
		}
 	}
}

void Plot::drawInwardTicks(QPainter *painter, const QRect &rect, 
							const QwtScaleMap &map, int axis) const
{
	QwtScaleWidget *scale = (QwtScaleWidget *)axisWidget(axis);
	if (!scale)
		return;
	
	int x1=rect.left();
	int x2=rect.right();
	int y1=rect.top();
	int y2=rect.bottom();
	
	QPalette pal=scale->palette();
	QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);
		
    painter->save();	
    painter->setPen(QPen(color, axesLinewidth(), Qt::SolidLine));
		
	QwtScaleDiv *scDiv=(QwtScaleDiv *)axisScaleDiv(axis);
	const QwtValueList minTickList = scDiv->ticks(QwtScaleDiv::MinorTick);
	int minTicks = (int)minTickList.count();

	const QwtValueList medTickList = scDiv->ticks(QwtScaleDiv::MediumTick);
	int medTicks = (int)medTickList.count();

	const QwtValueList majTickList = scDiv->ticks(QwtScaleDiv::MajorTick);
	int majTicks = (int)majTickList.count();
	
int j, x, y, low,high;
switch (axis)
	{
	case QwtPlot::yLeft:
	x=x1;
	low=y1+majTickLength;
	high=y2-majTickLength;
    for (j = 0; j < minTicks; j++)
        {
            y = map.transform(minTickList[j]);
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x, y, x+minTickLength, y);
        }
	for (j = 0; j < medTicks; j++)
        {
            y = map.transform(medTickList[j]);
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x, y, x+minTickLength, y);
        }
	 for (j = 0; j < majTicks; j++)
        {
            y = map.transform(majTickList[j]);
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
            y = map.transform(minTickList[j]);
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x+1, y, x-minTickLength, y);
        	}
		for (j = 0; j < medTicks; j++)
       	 	{
            y = map.transform(medTickList[j]);
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x+1, y, x-minTickLength, y);
        	}
		 for (j = 0; j <majTicks; j++)
        	{
            y = map.transform(majTickList[j]);
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
            x = map.transform(minTickList[j]);
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y+1, x, y-minTickLength);
       		 }
		for (j = 0; j < medTicks; j++)
        	{
            x = map.transform(medTickList[j]);
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y+1, x, y-minTickLength);
       		 }
	 	for (j = 0; j < majTicks; j++)
        	{
            x = map.transform(majTickList[j]);
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
             x = map.transform(minTickList[j]);
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y, x, y + minTickLength);
       	    }
		for (j = 0; j < medTicks; j++)
       		{
             x = map.transform(medTickList[j]);
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y, x, y + minTickLength);
       	    }
	 	for (j = 0; j <majTicks; j++)
        	{
            x = map.transform(majTickList[j]);
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y, x, y + majTickLength);
        	}
	break;
	}
painter->restore();
}

void Plot::drawInwardMajorTicks(QPainter *painter, const QRect &rect, 
							const QwtScaleMap &map, int axis) const
{
	QwtScaleWidget *scale=(QwtScaleWidget *)axisWidget (axis);
	if (!scale)
		return;
	
	const int x1=rect.left();
	const int x2=rect.right();	
	const int y1=rect.top();
	const int y2=rect.bottom();
	
	QPalette pal=scale->palette();
	QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);
		
    painter->save();	
    painter->setPen(QPen(color, axesLinewidth(), Qt::SolidLine));
		
	QwtScaleDiv *scDiv=(QwtScaleDiv *)axisScaleDiv(axis);
	
	const QwtValueList majTickList = scDiv->ticks(QwtScaleDiv::MajorTick);
	int majTicks = (int)majTickList.count();

switch (axis)
	{
	case QwtPlot::yLeft:
		{
		for (uint i = 0; i < majTicks; i++)
			{
			const int y = map.transform(majTickList[i]);
			if ((y >= y1) && (y <= y2))
			QwtPainter::drawLine(painter, x1, y, x1+majTickLength, y);
			}
		}
	break;
		
	case QwtPlot::yRight:
		{
     	for (uint i = 0; i < majTicks; i++)
       	 	{
            const int y = map.transform(majTickList[i]);
            QwtPainter::drawLine(painter, x2, y, x2-majTickLength, y);
        	}
		}
	  break;
		
	case QwtPlot::xBottom:
		{
     	for (uint i = 0; i < majTicks; i++)
        	{
            const int x = map.transform(majTickList[i]);
            QwtPainter::drawLine(painter, x, y2, x, y2-majTickLength);
       		}	
		}
		break;
	
	case QwtPlot::xTop:
		{
    	for (uint i = 0; i < majTicks; i++)
       		{
            const int x = map.transform(majTickList[i]);
            QwtPainter::drawLine(painter, x, y1, x, y1 + majTickLength);
       	    }
		}
	break;
	}
painter->restore();
}

void Plot::drawInwardMinorTicks(QPainter *painter, const QRect &rect, 
							const QwtScaleMap &map, int axis) const
{
	QwtScaleWidget *scale=(QwtScaleWidget *)axisWidget (axis);
	if (!scale)
		return;
	
	int x1=rect.left();
	int x2=rect.right();	
	int y1=rect.top();
	int y2=rect.bottom();
	
	QPalette pal=scale->palette();
	QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);
		
    painter->save();	
    painter->setPen(QPen(color, axesLinewidth(), Qt::SolidLine));
		
	QwtScaleDiv *scDiv=(QwtScaleDiv *)axisScaleDiv(axis);
	const QwtValueList tickList = scDiv->ticks(QwtScaleDiv::MinorTick);
	int minTicks = (int)tickList.count();

	const QwtValueList medTickList = scDiv->ticks(QwtScaleDiv::MediumTick);
	int medTicks = (int)medTickList.count();

int j, x, y, low,high;
switch (axis)
	{
	case QwtPlot::yLeft:
	x=x1;
	low=y1+majTickLength;
	high=y2-majTickLength;
    for (j = 0; j < minTicks; j++)
        {
            y = map.transform(tickList[j]);
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
            y = map.transform(tickList[j]);
			if (y>low && y< high)
            	QwtPainter::drawLine(painter, x+1, y, x-minTickLength, y);
        	}

		for (j = 0; j < medTicks; j++)
       	 	{
            y = map.transform(medTickList[j]);
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
            x = map.transform(tickList[j]);
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y+1, x, y-minTickLength);
       		}
		for (j = 0; j < medTicks; j++)
        	{
            x = map.transform(medTickList[j]);
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
             x = map.transform(tickList[j]);
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y, x, y + minTickLength);
       	    }
		for (j = 0; j < medTicks; j++)
       		{
             x = map.transform(medTickList[j]);
			if (x>low && x<high)
            	QwtPainter::drawLine(painter, x, y, x, y + minTickLength);
       	    }
	break;
	}
painter->restore();
}

void Plot::setAxesLinewidth(int width)
{
for (int i=0; i<QwtPlot::axisCnt; i++)
	{
	QwtScaleWidget *scale=(QwtScaleWidget*) this->axisWidget(i);
	if (scale)
		{
		scale->setPenWidth(width);
		scale->repaint();
		}
	}
}

int Plot::axesLinewidth() const
{
for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
	{
	const QwtScaleWidget *scale = this->axisWidget(axis);
    if (scale)
		return scale->penWidth();
	}
return 0;
}

int Plot::minorTickLength() const
{
return minTickLength;
}

int Plot::majorTickLength() const
{
return majTickLength;
}

void Plot::setTickLength (int minLength, int majLength)
{
if (majTickLength == majLength &&
	minTickLength == minLength)
	return;

majTickLength = majLength;
minTickLength = minLength;
}

void Plot::mousePressEvent ( QMouseEvent * e )
{
if(e->state()==Qt::ShiftButton)
		ShiftButton=TRUE;
	
presspos = e->pos();
emit selectPlot();
}

void Plot::mouseMoveEvent ( QMouseEvent * e )
{
if(ShiftButton)
	{
	graphToResize=TRUE;
	emit resizeGraph(e->pos());	
	}				
/*else if ((presspos - e->pos()).manhattanLength() > QApplication::startDragDistance())
	{
	if (!((Graph *)parent())->zoomOn())
		{
		movedGraph=TRUE;
		emit moveGraph(e->pos());
		}
	}*/
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

void Plot::drawPixmap(QPainter *painter, const QRect &rect)
{
    if ( painter == 0 || !painter->isActive() || size().isNull() )
       return;

    painter->save();

	QwtPlotLayout *layout = plotLayout();
    layout->activate(this, rect, 0);
   
	printTitle(painter, layout->titleRect());

    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
		{
		const QwtScaleWidget *scale = this->axisWidget(axis);
        if (scale)
			{
            int baseDist = scale->margin();
            int startDist, endDist;
			if (axis == QwtPlot::xTop || axis == QwtPlot::yRight)
				{//synchronize secondary scales 
				scale = this->axisWidget(axis-1);
				if (scale)
					{
					startDist = scale->startBorderDist();
					endDist = scale->endBorderDist();
					}
				}
			else
				{
				startDist = scale->startBorderDist();
				endDist = scale->endBorderDist();
				}
            printScale(painter, axis, startDist, endDist, baseDist, layout->scaleRect(axis));
			}
		}

	const QwtPlotCanvas *canvas = this->canvas();
	const QPixmap *canvasPix = canvas->paintCache();
	if (canvasPix)
		{
		QRect cr = layout->canvasRect();
		const int clw = canvas->lineWidth();
		if (clw > 0)
    		{
			QPalette pal = canvas->palette();
			QColor color = pal.color(QPalette::Active, QColorGroup::Foreground);
			painter->setPen (QPen(color, clw, Qt::SolidLine));

			cr.moveBy(clw/2, clw/2);
			cr.setWidth(cr.width() - clw);
			cr.setHeight(cr.height() - clw);

			painter->drawRect(cr);//draw canvas frame

			cr = layout->canvasRect();
			cr.moveBy(clw, clw);
			cr.setWidth(cr.width() - clw);
			cr.setHeight(cr.height() - clw);
			}

		QPixmap pix = QPixmap (cr.width(), cr.height());
		copyBlt (&pix, 0, 0, canvasPix, 0, 0, -1, -1);
		painter->drawPixmap (cr, pix);
		}

    painter->restore();
}

void Plot::print(QPainter *painter, const QRect &plotRect,
        const QwtPlotPrintFilter &pfilter) const
{
 /*   int axis;

    if ( painter == 0 || !painter->isActive() ||
            !plotRect.isValid() || size().isNull() )
       return;

    painter->save();

    // All paint operations need to be scaled according to
    // the paint device metrics. 

    QwtPainter::setMetricsMap(this, painter->device());

#if QT_VERSION < 300 
    if ( painter->device()->isExtDev() )
    {
        QPaintDeviceMetrics metrics(painter->device());
        if ( metrics.logicalDpiX() == 72 && metrics.logicalDpiY() == 72 )
        {
            // In Qt 2.x QPrinter returns hardcoded wrong metrics.
            // So scaling won´t work: we reset to screen resolution

            QwtPainter::setMetricsMap(this, this);
        }
    }
#endif

    const QwtMetricsMap &metricsMap = QwtPainter::metricsMap();

    pfilter.apply((QwtPlot *)this);

    int baseLineDists[QwtPlot::axisCnt];
    if ( !(pfilter.options() & QwtPlotPrintFilter::PrintCanvasBackground) )
    {
        // In case of no background we set the backbone of
        // the scale on the frame of the canvas.

        for (axis = 0; axis < QwtPlot::axisCnt; axis++ )
        {
			QwtScaleWidget *scale = (QwtScaleWidget *)this->axisWidget(axis);
            if ( scale )
            {
                baseLineDists[axis] = scale->baseLineDist();
                scale->setBaselineDist(0);
            }
        }
    }
    // Calculate the layout for the print.

    int layoutOptions = QwtPlotLayout::IgnoreScrollbars 
        | QwtPlotLayout::IgnoreFrames;
    if ( !(pfilter.options() & QwtPlotPrintFilter::PrintMargin) )
        layoutOptions |= QwtPlotLayout::IgnoreMargin;
    if ( !(pfilter.options() & QwtPlotPrintFilter::PrintLegend) )
        layoutOptions |= QwtPlotLayout::IgnoreLegend;

	QwtPlotLayout *layout = (QwtPlotLayout *)this->plotLayout();
    layout->activate(this, QwtPainter::metricsMap().deviceToLayout(plotRect), 
        layoutOptions);

    if ((pfilter.options() & QwtPlotPrintFilter::PrintTitle)
        && (!title().isEmpty()))
    {
        printTitle(painter, layout->titleRect());
    }

    for ( axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
		QwtScaleWidget *scale = (QwtScaleWidget *)this->axisWidget(axis);
        if (scale)
        {
            int baseDist = scale->baseLineDist();

            int startDist, endDist;
			if ( axis == xTop || axis == yRight)
				{//TO DO: try to synchronize scales, it doesn't work for the moment
				QwtScaleWidget *op_scale = (QwtScaleWidget *)this->axisWidget(axis-1);
				if (op_scale)
					{
					op_scale->minBorderDist(startDist, endDist);
					const QwtScaleDraw *scld = 	op_scale->scaleDraw ();
					scale->setScaleDiv (scld->scaleDiv ());
					}
				}
			else
				scale->minBorderDist(startDist, endDist);

            printScale(painter, axis, startDist, endDist,
                baseDist, layout->scaleRect(axis));
        }
    }

    const QRect canvasRect = metricsMap.layoutToDevice(layout->canvasRect());

    QwtArray<QwtScaleMap> map(axisCnt);
    for (axis = 0; axis < axisCnt; axis++)
    {
        const QwtScaleDiv *scaleDiv = this->axisScaleDiv(axis);
        map[axis].setDblRange(scaleDiv->lBound(),
            scaleDiv->hBound(), scaleDiv->logScale());

        double from, to;
        if ( axisEnabled(axis) )
        {
			QwtScaleWidget *scale = (QwtScaleWidget *)this->axisWidget(axis);
            const int sDist = scale->startBorderDist();
            const int eDist = scale->endBorderDist();
            const QRect &scaleRect = layout->scaleRect(axis);

            if ( axis == xTop || axis == xBottom )
            {
                from = metricsMap.layoutToDeviceX(scaleRect.left() + sDist);
                to = metricsMap.layoutToDeviceX(scaleRect.right() - eDist);
            }
            else
            {
                from = metricsMap.layoutToDeviceY(scaleRect.bottom() - sDist);
                to = metricsMap.layoutToDeviceY(scaleRect.top() + eDist);
            }
        }
        else
        {
            const int margin = plotLayout()->canvasMargin(axis);

            const QRect &canvasRect = plotLayout()->canvasRect();
            if ( axis == yLeft || axis == yRight )
            {
                from = metricsMap.layoutToDeviceX(canvasRect.bottom() - margin);
                to = metricsMap.layoutToDeviceX(canvasRect.top() + margin);
            }
            else
            {
                from = metricsMap.layoutToDeviceY(canvasRect.left() + margin);
                to = metricsMap.layoutToDeviceY(canvasRect.right() - margin);
            }
        }
        map[axis].setIntRange( qwtInt(from), qwtInt(to));
    }


    // The maps are already scaled. 
    QwtPainter::setMetricsMap(painter->device(), painter->device());

    printCanvas(painter, canvasRect, map, pfilter);

    QwtPainter::resetMetricsMap();

    layout->invalidate();

    // reset all widgets with their original attributes.
    if ( !(pfilter.options() & QwtPlotPrintFilter::PrintCanvasBackground) )
    {
        // restore the previous base line dists

        for (axis = 0; axis < QwtPlot::axisCnt; axis++ )
        {
			QwtScaleWidget *scale = (QwtScaleWidget *)this->axisWidget(axis);
            if ( scale )
                scale->setBaselineDist(baseLineDists[axis]);
        }
    }

    pfilter.reset((QwtPlot *)this);

    painter->restore();*/

QwtPlot::print(painter, plotRect, pfilter);
}

int Plot::closestCurve(int xpos, int ypos, int &dist, int &point)
{
QwtScaleMap map[QwtPlot::axisCnt];
for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
	map[axis] = canvasMap(axis);

double dmin = 1.0e10;
int key = -1;
for (QMap<int, QwtPlotCurve *>::iterator it = d_curves.begin(); it != d_curves.end(); ++it ) 
	{
	QwtPlotCurve *c = (QwtPlotCurve *)it.data();
	if (!c)
		continue;

	for (int i=0; i<c->dataSize(); i++)
		{
		double cx = map[c->xAxis()].xTransform(c->x(i)) - double(xpos);
		double cy = map[c->yAxis()].xTransform(c->y(i)) - double(ypos);

		double f = qwtSqr(cx) + qwtSqr(cy);

		if (f < dmin)
			{
			dmin = f;
			key = it.key();
			point = i;
			}
		 }
	}

dist = int(sqrt(dmin));
return key;
}

void Plot::removeMarker(int index)
{
QwtPlotMarker *m = d_markers[index];
m->detach();
d_markers.remove (index);
}

int Plot::insertMarker(QwtPlotMarker *m)
{
marker_key++;
d_markers.insert (marker_key, m, false );
m->attach(((QwtPlot *)this));
return marker_key;
}

int Plot::insertCurve(QwtPlotCurve *c)
{
curve_key++;
d_curves.insert (curve_key, c, false);
c->attach(this);
return curve_key;
}

void Plot::removeCurve(int index)
{
QwtPlotCurve *c = d_curves[index];
c->detach();
d_curves.remove (index);
}

/*!
  \return the number format for the major scale labels of a specified axis
  \param axis axis index
  \retval f format character
  \retval prec precision
*/
void Plot::axisLabelFormat(int axis, char &f, int &prec) const
{
if (axisValid(axis))
	{
	ScaleDraw *sd = (ScaleDraw *)axisScaleDraw (axis);
    sd->labelFormat(f, prec);
	}
else
    {
    //for a bad call we return the default values
    f = 'g'; 
    prec = 4;
    }
}

/*!
  Change the number format for the major scale of a selected axis
  \param axis axis index
  \param f format
  \param prec precision
*/
void Plot::setAxisLabelFormat(int axis, char f, int prec)
{
    if (axisValid(axis))
		{
		ScaleDraw *sd = (ScaleDraw *)axisScaleDraw (axis);
        sd->setLabelFormat(f, prec);
		}
}

QwtDoubleRect Plot::boundingRect ()
{
QMap<int, QwtPlotCurve *>::iterator it = d_curves.begin();
QwtPlotCurve *c = (QwtPlotCurve *)it.data();

double minX = c->minXValue();
double minY = c->minYValue();
double maxX = c->maxXValue();
double maxY = c->maxYValue();

it++;

for (it; it != d_curves.end(); ++it) 
	{
	QwtPlotCurve *c = (QwtPlotCurve *)it.data();
	if (!c)
		continue;

	minX = (c->minXValue() < minX) ? c->minXValue() : minX;
	maxX = (c->maxXValue() > maxX) ? c->maxXValue() : maxX;
	minY = (c->minYValue() < minY) ? c->minYValue() : minY;
	maxY = (c->maxYValue() > maxY) ? c->maxYValue() : maxY;
	}

QwtDoubleRect r;
r.setLeft(minX);
r.setRight(maxX);
r.setTop(minY);
r.setBottom(maxY);
return r;
}

/*const QColor& Plot::canvasBackground() const
{
return canvas()->paletteBackgroundColor();
}*/

