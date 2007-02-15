/***************************************************************************
    File                 : canvaspicker.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#include "plot.h"

class Graph;
	
/**
 * \brief Handles parts of the user interaction for a Plot by registering itself as an event filter for its QwtPlotCanvas.
 *
 * CanvasPicker relies heavily on its parent being the Graph that owns the Plot it operates on.
 * Additionally, parts of the code use Graph::plotWidget instead of CanvasPicker::plotWidget.
 * 
 * \sa Plot::mousePressEvent, Plot::mouseReleaseEvent, Plot::presspos, Plot::movedGraph
 */
class CanvasPicker: public QObject
{
    Q_OBJECT
public:
	 CanvasPicker(Graph *plot);
	 virtual bool eventFilter(QObject *, QEvent *);
	 void selectPoints(int n);
	 void selectPeak(const QPoint& p);

private:
	void drawTextMarker(const QPoint&);
	void drawLineMarker(const QPoint&, bool endArrow);

	//! Called when the user releases the mouse button after a line marker resize action
	/**
	 * \param point the mouse position
	 */
	void resizeLineMarker(const QPoint& point);

	//! Selects and highlights the marker at the given position.
	/**
	 * \return whether a marker was found at \var point
	 */
	bool selectMarker(const QPoint& point);
	void moveMarker(QPoint& position);
	void releaseMarker();

	/**
	 * \brief Return my parent as a Graph.
	 *
	 * %Note that contrary to the method name, this does NOT return the Plot I operate on.
	 */
	Graph *plot() { return (Graph *)parent(); }
	
	/**
	 * \brief The Plot I handle user interaction for.
	 *
	 * %Note that this has to be owned by my parent Graph.
	 */
	Plot* plotWidget;	
	QPoint startLinePoint, endLinePoint;

	//! Tells whether the user resizes a line marker via the mouse using the start point
	bool resizeLineFromStart;
	
	//! Tells whether the user resizes a line marker via the mouse using the end point
	bool resizeLineFromEnd;	
	
signals:
	void showPieDialog();
	void showPlotDialog(int);
	void viewTextDialog();
	void viewLineDialog();
	void viewImageDialog();
	void drawTextOff();
	void showMarkerPopupMenu();
	void modified();
	void calculateProfile(const QPoint&, const QPoint&);
	void selectPlot();
	void moveGraph(const QPoint&);
	void releasedGraph();
	void highlightGraph();
	
private:
    QPoint presspos;
    int xMouse, yMouse, xMrk, yMrk, n_peaks, selected_points;
	bool moved,	movedGraph, pointSelected, select_peaks;
};
