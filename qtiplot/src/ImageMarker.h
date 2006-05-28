/***************************************************************************
    File                 : ImageMarker.h
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
#ifndef IMAGEMARKER_H
#define IMAGEMARKER_H

#include <q3paintdevicemetrics.h>
#include <qpixmap.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
	
//! Image marker (extension to QwtPlotMarker)
class ImageMarker: public QwtPlotMarker
{
public:
	ImageMarker(const QPixmap& p, QwtPlot *plot);
    virtual void draw(QPainter *p, int x, int y, const QRect &);
	
	QRect rect(){return QRect(origin,picSize);};

	QSize size(){return picSize;};
	void setSize(const QSize& size)
		{
		if (picSize == size) return;
		picSize=size;
		}
	
	void setFileName(const QString& fn){fileName=fn;};
	QString getFileName(){return fileName;};
	
	QPixmap image(){return pic;};

	void setOrigin(const QPoint& p){origin=p;};
	QPoint getOrigin(){return origin;};

private:
	QPoint origin;
	QPixmap pic;
	QSize picSize;
	QString fileName;
};

#endif

