/***************************************************************************
    File                 : ImageMarker.h
    Project              : QtiPlot
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
#ifndef IMAGEMARKER_H
#define IMAGEMARKER_H

#include <qwt_plot.h>
#include <qwt_plot_marker.h>
	
#include <QPixmap>

/*!\brief Draw images on a QwtPlot.
 *
 * ImageMarker draws #d_pic at the position specified by #d_pos,
 * scaled to #d_size. #d_file_name isn't used internally, but only provided to help
 * Graph manage save/restore.
 *
 * \section future Future Plans
 * Add a QwtPlotItem subclass (PlotEnrichment ?) and make ImageMarker, LegendMarker and LineMarker
 * subclasses of that. These (usually) don't really mark a specific point in a plot and they don't
 * use the symbol/label functionality of QwtPlotMarker. Instead, it would make sense to provide a
 * unified move/resize (or even general affine transformations via QMatrix) interface and support for
 * positioning them either at fixed plot coordinates (like QwtPlotMarker) or at a fixed drawing
 * position within a QwtPlot (like a QWidget child); leaving the choice of positioning policy to the
 * user.
 * If PlotEnrichment (ideas for a better name?) inherits from both QWidget and QwtPlotItem (which
 * is luckily no QObject) and provides a unified drawing framework, its instances could be added
 * directly to MultiLayer without the need for a dummy Graph in between.
 * Could also help to avoid the hack in MultiLayer::updateMarkersBoundingRect().
 *
 * Following the above thoughts, it might help clarify the purpose of ImageMarker, LegendMarker
 * and LineMarker if they are renamed according to the new superclasse's name
 * (e.g. ImageEnrichment, TextEnrichment and LineEnrichment).
 *
 * See the documentation of SelectionMoveResizer for other advantages of this approach.
 *
 * \sa LegendMarker, LineMarker
 */
class ImageMarker: public QwtPlotMarker
{
public:
	//! Construct an image marker from a pixmap.
	ImageMarker(const QPixmap& p);
	//! Does the actual drawing; see QwtPlotItem::draw.
	virtual void draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const;
	
	//! Return bounding rectangle in paint coordinates.
	QRect rect() const {return transform(plot()->canvasMap(xAxis()), plot()->canvasMap(yAxis()), d_rect);};
	//! Set value (position) and #d_size, giving everything in paint coordinates.
	void setRect(int x, int y, int w, int h);

	//! Return bounding rectangle in plot coordinates.
	virtual QwtDoubleRect boundingRect() const;
	//! Set value (position) and #d_size, giving everything in plot coordinates.
	void setBoundingRect(const QwtDoubleRect& rect);

	//! Return #d_size.
	QSize size() { return rect().size(); };
	//! Set #d_size.
	void setSize(const QSize& size);
	
	//! Return position in paint coordinates.
	QPoint origin() const { return rect().topLeft(); };
	//! Set QwtPlotMarker::value() in paint coordinates.
	void setOrigin(const QPoint &p);
	
	//! Set #d_file_name.
	void setFileName(const QString& fn) { d_file_name = fn; };
	//! Return #d_file_name.
	QString getFileName(){return d_file_name;};
	
	//! Return the image to be drawin, #d_pic.
	QPixmap image() const { return d_pic; };

	void updateBoundingRect();

private:
	QPoint d_pos;
	//! The pixmap to be drawn.
	QPixmap d_pic;
	//! The size (in paint coordinates) to which #d_pic will be scaled in draw().
	QSize d_size;
	//! The file from which the image was loaded.
	QString d_file_name;
	QwtDoubleRect d_rect;
};

#endif
