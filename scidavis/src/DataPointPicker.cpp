/***************************************************************************
    File                 : DataPointPicker.cpp
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

#include "DataPointPicker.h"
#include "Graph.h"
#include "DataPickerTool.h"

/**
 * \class DataPointPicker
 * \brief Let the user pick single data points from a plot.
 *
 * While DataPickerTool does the actual selection work, DataPointPicker provides the framework
 * necessary for script-driven usage. In particular, the event-oriented interface of DataPickerTool
 * doesn't fit very well into the typical imparative structure of simple scripts. DataPointPicker
 * bridges this gap by starting a new event loop (see QEventLoop) which allows the user to pick a
 * data point while the script is still running within the main event loop. The inner event loop
 * exits when the user has selected a data point, pick() returns its coordinates and control flows
 * back to the caller.
 */

//! Constructor.
DataPointPicker::DataPointPicker(Graph *g, ApplicationWindow *app)
	: m_graph(g), m_app(app), m_result(NAN,NAN)
{
}

//! Destructor (exits the event loop).
DataPointPicker::~DataPointPicker() {
	m_picking_loop.exit(1);
}

/**
 * \brief Let the user pick a single data point and return its coordinates.
 *
 * Starts a new event loop, which runs until either the user has selected a point or the
 * DataPointPicker is destructed.
 */
QPointF DataPointPicker::pick() {
	if (m_graph->isPiePlot() || !m_graph->validCurvesDataSize())
		return m_result;
	DataPickerTool *tool = new DataPickerTool(m_graph, m_app, DataPickerTool::Display);
	connect(tool, SIGNAL(selected(QwtPlotCurve*,int)),
                this, SLOT(pointSelected(QwtPlotCurve*,int)));
	m_graph->setActiveTool(tool);
	m_picking_loop.exec();
	m_graph->setActiveTool(0);
	return m_result;
}

//! Handle selection of a point by the user.
void DataPointPicker::pointSelected(QwtPlotCurve *curve, int pointIndex) {
	m_result = QPointF(curve->x(pointIndex), curve->y(pointIndex));
	m_picking_loop.exit(0);
}
