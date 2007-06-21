/***************************************************************************
    File                 : PlotEnrichement.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, knut.franke*gmx.de
    Description          : Abstract enrichement class for 2D Plots

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
#ifndef PLOTENRICHEMENT_H
#define PLOTENRICHEMENT_H

#include <qwt_plot.h>
#include <qwt_plot_marker.h>

/*!\brief Draw images on a QwtPlot.
 *
 * PlotEnrichement draws #d_pic at the position specified by #d_pos,
 * scaled to #d_size. #d_file_name isn't used internally, but only provided to help
 * Graph manage save/restore.
 *
 * \section future Future Plans
 * Add a QwtPlotItem subclass (PlotEnrichment ?) and make PlotEnrichement, Legend and ArrowMarker
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
 * Following the above thoughts, it might help clarify the purpose of PlotEnrichement, Legend
 * and ArrowMarker if they are renamed according to the new superclasse's name
 * (e.g. ImageEnrichment, TextEnrichment and LineEnrichment).
 *
 * See the documentation of SelectionMoveResizer for other advantages of this approach.
 *
 * \sa Legend, ArrowMarker
 */
class PlotEnrichement: public QwtPlotMarker
{
public:
	PlotEnrichement();

	//! Return bounding rectangle in paint coordinates.
	virtual QRect rect() const {return QRect(d_pos, d_size);};
	//! Set value (position) and #d_size, giving everything in paint coordinates.
	virtual void setRect(int x, int y, int w, int h);

	//! Return bounding rectangle in plot coordinates.
	virtual QwtDoubleRect boundingRect() const;
	//! Set position (xValue() and yValue()), right and bottom values giving everything in plot coordinates.
	virtual void setBoundingRect(double left, double top, double right, double bottom);

	double right(){return d_x_right;};
	double bottom(){return d_y_bottom;};

	//! Return position in paint coordinates.
	QPoint origin() const { return d_pos; };
	//! Set QwtPlotMarker::value() in paint coordinates.
	void setOrigin(const QPoint &p);

    //! Return #d_size.
	QSize size() {return d_size;};
	//! Set #d_size.
	void setSize(const QSize& size);

	virtual void updateBoundingRect();

private:
    QRect calculatePaintingRect();
	//! The right side position in scale coordinates.
	double d_x_right;
    //! The bottom side position in scale coordinates.
    double d_y_bottom;
    //! The position in paint coordiantes.
	QPoint d_pos;
	//! The size (in paint coordinates).
	QSize d_size;
};

#endif

