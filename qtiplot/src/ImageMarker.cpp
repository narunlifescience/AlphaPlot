/***************************************************************************
    File                 : ImageMarker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Image marker (extension to QwtPlotMarker)
                           
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
#include "ImageMarker.h"
#include "LineMarker.h"

#include <qpainter.h>
#include <q3paintdevicemetrics.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
//Added by qt3to4:
#include <QPixmap>

/*#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>*/

//FIXME: All functionality disabled for now (needs port to Qwt5)

ImageMarker::ImageMarker(const QPixmap& p, QwtPlot *plot):
    QwtPlotMarker()
{
#if false
pic=p;
#endif
}

void ImageMarker::draw(QPainter *p, int, int, const QRect& rect)
{
#if false
	if (p->device()->isExtDev())
		{
		QwtPlot *plot = (QwtPlot *)parentPlot();				
		const QwtScaleMap xMap = plot->canvasMap(QwtPlot::xBottom);
		const QwtScaleMap yMap = plot->canvasMap(QwtPlot::yLeft);
	
		double r0_x=xMap.invTransform(origin.x());
		double r0_y=yMap.invTransform(origin.y());	
		double r1_x=xMap.invTransform(origin.x()+picSize.width());
		double r1_y=yMap.invTransform(origin.y()+picSize.height());
			
		QwtScaleMap map=LineMarker::mapCanvasToDevice(p, plot, QwtPlot::xBottom);
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
#endif
}

