#include "ImageMarker.h"
#include "LineMarker.h"

#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>

/*#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>*/

ImageMarker::ImageMarker(const QPixmap& p, QwtPlot *plot):
    QwtPlotMarker(plot)
{
pic=p;
}

void ImageMarker::draw(QPainter *p, int, int, const QRect& rect)
{
	if (p->device()->isExtDev())
		{
		QwtPlot *plot = (QwtPlot *)parentPlot();				
		const QwtDiMap xMap = plot->canvasMap(QwtPlot::xBottom);
		const QwtDiMap yMap = plot->canvasMap(QwtPlot::yLeft);
	
		double r0_x=xMap.invTransform(origin.x());
		double r0_y=yMap.invTransform(origin.y());	
		double r1_x=xMap.invTransform(origin.x()+picSize.width());
		double r1_y=yMap.invTransform(origin.y()+picSize.height());
			
		QwtDiMap map=LineMarker::mapCanvasToDevice(p, plot, QwtPlot::xBottom);
		int x0=map.transform(r0_x);
		int x1=map.transform(r1_x);
	
	    map=LineMarker::mapCanvasToDevice(p, plot, QwtPlot::yLeft);		
		int y0=map.transform(r0_y);
		int y1=map.transform(r1_y);

		p->drawPixmap(QRect(QPoint(x0,y0),QPoint(x1,y1)),pic);			
		}		
	else	
		{
		QRect ir=QRect(origin,picSize);
		int clw = parentPlot()->canvas()->lineWidth();
		ir.moveBy(rect.x() - clw, rect.y() - clw);
		p->drawPixmap(ir,pic);
		}
}

