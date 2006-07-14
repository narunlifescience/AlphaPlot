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
#include <qobject.h>
#include "plot.h"

class Graph;
	
//! Canvas picker
class CanvasPicker: public QObject
{
    Q_OBJECT
public:
    CanvasPicker(Graph *plot);
    virtual bool eventFilter(QObject *, QEvent *);
	void selectPoints(int n);
	int selectedPoints(){return selected_points;};
	void selectPeak(const QPoint& p);

private:
	void drawTextMarker(const QPoint&);
	void drawLineMarker(const QPoint&, bool endArrow);

	//! Called when the user releases the mouse button after a line marker resize action
	/**
	 * \param point the mouse position
	*/
	void resizeLineMarker(const QPoint& point);

	//! Selects and highlights the marker 
	/**
	 * \param point the mouse position
	*/
	bool selectMarker(const QPoint& point);
	void moveMarker(QPoint& );
	void releaseMarker();

	Graph *plot() { return (Graph *)parent(); }
	
	Plot* plotWidget;	
	QPoint startLinePoint, endLinePoint;
	
	int xMouse, yMouse, xMrk, yMrk, n_peaks, selected_points;
	bool moved,	movedGraph, pointSelected, select_peaks;

	//! Tells if the user resizes a line marker via the mouse using the start point
	bool resizeLineFromStart;
	
	//! Tells if the user resizes a line marker via the mouse using the end point
	bool resizeLineFromEnd;	
	
signals:
	void showPieDialog();
	void showPlotDialog(long);
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
};

