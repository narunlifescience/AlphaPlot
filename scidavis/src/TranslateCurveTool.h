/***************************************************************************
    File                 : TranslateCurveTool.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006,2007 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Plot tool for translating curves.

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
#ifndef TRANSLATE_CURVE_TOOL_H
#define TRANSLATE_CURVE_TOOL_H

#include "PlotToolInterface.h"
#include <QObject>
#include <qwt_double_rect.h>

class ApplicationWindow;
class QwtPlotCurve;

//! Plot tool for translating curves.
class TranslateCurveTool : public QObject, public PlotToolInterface
{
	Q_OBJECT
	public:
		enum Direction { Vertical, Horizontal };
		/*!\brief Standard constructor.
		 * \param graph the Graph to operate on (or rather, on whose image markers to operate on)
		 * \param app parent window of graph
		 * \param dir the direction in which to translate curves
		 * \param status_target target to which the statusText(const QString&) signal will be connected
		 * \param status_slot slot on status_target to which the statusText(const QString&) signal will be connected
		 * The status_target/status_slot arguments are provided, because statusText(const QString&) is emitted
		 * during initialization, before there's any other chance of connecting to it.
		 */
		TranslateCurveTool(Graph *graph, ApplicationWindow *app, Direction dir, const QObject *status_target=NULL, const char *status_slot="");
	signals:
		/*!\brief Emitted whenever a new message should be presented to the user.
		 *
		 * You don't have to connect to this signal if you alreay specified a reciever during initialization.
		 */
		void statusText(const QString&);
	public slots:
		//! Select curve to translate, using the specified point as base point.
		void selectCurvePoint(QwtPlotCurve *curve, int point_index);
		//! Select the coordinates where the base point should end up (modulo projection on #d_dir direction).
		void selectDestination(const QwtDoublePoint &point);
	private:
		Direction d_dir;
		PlotToolInterface *d_sub_tool;
		QwtPlotCurve *d_selected_curve;
		QwtDoublePoint d_curve_point;
		ApplicationWindow *d_app;
};

#endif // TRANSLATE_CURVE_TOOL_H

