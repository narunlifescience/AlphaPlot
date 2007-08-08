/***************************************************************************
    File                 : ImageEnrichment.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, knut.franke*gmx.de
    Description          : Draw images on a QwtPlot.

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
#ifndef IMAGE_ENRICHMENT_H
#define IMAGE_ENRICHMENT_H

#include "../AbstractEnrichment.h"

#include <qwt_plot.h>
#include <QPixmap>

/*!\brief Draw images on a QwtPlot.
 *
 * ImageEnrichment draws #d_pic at the position specified by #d_pos,
 * scaled to #d_size. #d_file_name isn't used internally, but only provided to help
 * Graph manage save/restore.
 *
 */
class ImageEnrichment: public AbstractEnrichment
{
public:
	//! Construct an image marker from a file name.
	ImageEnrichment(const QString& fn);

	//! Return bounding rectangle in paint coordinates.
	QRect rect() const;
	//! Set value (position) and #d_size, giving everything in paint coordinates.
	void setRect(int x, int y, int w, int h);

	//! Return bounding rectangle in plot coordinates.
	virtual QwtDoubleRect boundingRect() const;
	//! Set position (xValue() and yValue()), right and bottom values giving everything in plot coordinates.
	void setBoundingRect(double left, double top, double right, double bottom);

	double right(){return d_x_right;};
	double bottom(){return d_y_bottom;};

	//! Return #d_size.
	QSize size() {return rect().size();};
	//! Set #d_size.
	void setSize(const QSize& size);
	//! Set #d_size. Provided for convenience in scripts 
	void setSize(int w, int h){setSize(QSize(w, h));};

	//! Return position in paint coordinates.
	QPoint origin() const { return rect().topLeft(); };
	//! Set QwtPlotMarker::value() in paint coordinates.
	void setOrigin(const QPoint &p);

	//! Set #d_file_name.
	void setFileName(const QString& fn) { d_file_name = fn; };
	//! Return #d_file_name.
	QString fileName(){return d_file_name;};

	//! Return the pixmap to be drawn, #d_pic.
	QPixmap pixmap() const {return d_pic;};

	void updateBoundingRect();

private:
	//! Does the actual drawing; see QwtPlotItem::draw.
	void draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const;
    //! The position in paint coordiantes.
	QPoint d_pos;
	//! The pixmap to be drawn.
	QPixmap d_pic;
	//! The size (in paint coordinates) to which #d_pic will be scaled in draw().
	QSize d_size;
	//! The file from which the image was loaded.
	QString d_file_name;
	//! The right side position in scale coordinates.
	double d_x_right;
    //! The bottom side position in scale coordinates.
    double d_y_bottom;
};

#endif // ifndef IMAGE_ENRICHMENT_H
