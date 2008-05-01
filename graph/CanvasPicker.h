/***************************************************************************
    File                 : CanvasPicker.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Canvas picker

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
#include <QObject>
#include "Plot.h"

class Layer;
class LineEnrichment;

/**
 * \brief Handles parts of the user interaction for a Plot by registering itself as an event filter for its QwtPlotCanvas.
 *
 * CanvasPicker relies heavily on its parent being the Layer that owns the Plot it operates on.
 * Additionally, parts of the code use Layer::plotWidget instead of CanvasPicker::plotWidget.
 */
class CanvasPicker: public QObject
{
    Q_OBJECT
public:
	 CanvasPicker(Layer *layer);
	 virtual bool eventFilter(QObject *, QEvent *);
	 void selectPoints(int n);
	 void selectPeak(const QPoint& p);

	 //! Disable editing of #d_editing_marker on a TAB-key event.
	 /*!\brief Called by Layer::focusNextPrevChild ()
	 */
	void disableEditing();

private:
	void drawTextMarker(const QPoint&);
	void drawLineMarker(const QPoint&, bool endArrow);

	//! Selects and highlights the marker at the given position.
	/**
	 * \return whether a marker was found at #pointSelected
	 */
	bool selectMarker(const QMouseEvent *e);

	/**
	 * \brief Return my parent as a Layer.
	 */
	Layer *layer() { return (Layer *)parent(); }

	/**
	 * \brief The Plot I handle user interaction for.
	 *
	 * %Note that this has to be owned by my parent Layer.
	 */
	Plot* plotWidget;
	QPoint startLinePoint, endLinePoint;

signals:
	void showPlotDialog(int);
	void viewTextDialog();
	void viewLineDialog();
	void viewImageDialog();
	void drawTextOff();
	void showMarkerPopupMenu();
	void modified();
	void selectPlot();

private:
	bool pointSelected;
	/*!\brief The marker that is currently being edited, or NULL.
	 * Editing does explicitly _not_ inlude moving and resizing, which are being
	 * handled by SelectionMoveResizer (see Layer::d_markers_selector).
	 * Currently, only LineEnrichment provides any other form of editing, but this really
	 * should be generalized. See ImageEnrichment for details.
	 */
	LineEnrichment *d_editing_marker;
};
