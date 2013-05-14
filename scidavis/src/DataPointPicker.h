/***************************************************************************
    File                 : DataPointPicker.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2010 Knut Franke
    Email (use @ for *)  : Knut.Franke*gmx.net
    Description          : Let the user pick single data points from a plot.

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

#ifndef DATA_POINT_PICKER_H
#define DATA_POINT_PICKER_H

#include <QEventLoop>
#include <QPointF>

class Graph;
class ApplicationWindow;
class QwtPlotCurve;

class DataPointPicker : public QObject
{
	Q_OBJECT

	public:
		DataPointPicker(Graph *g, ApplicationWindow *app);
		~DataPointPicker();
		QPointF pick();
	protected slots:
		void pointSelected(QwtPlotCurve *curve, int pointIndex);
	private:
		QEventLoop m_picking_loop;
		Graph *m_graph;
		ApplicationWindow *m_app;
		QPointF m_result;
};

#endif // ifndef DATA_POINT_PICKER_H
