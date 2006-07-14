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

#include <QPixmap>
#include <qwt_plot_marker.h>
	
class ImageMarker: public QwtPlotMarker
{
public:
	ImageMarker(const QPixmap& p);
    virtual void draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const;
	
	QRect rect();

	QSize size();
	void setSize(const QSize& size);
	
	void setFileName(const QString& fn){fileName=fn;};
	QString getFileName(){return fileName;};
	
	QPixmap image(){return pic;};

	void setOrigin(const QPoint& p);
	QPoint getOrigin();

//! Keep the markers on screen each time the scales are modified by adding/removing curves
	void updateOrigin();

	QwtDoubleRect boundingRect() const;

private:
	QPoint origin;
	QPixmap pic;
	QSize picSize;
	QString fileName;
	QwtDoubleRect d_rect;
};

#endif


