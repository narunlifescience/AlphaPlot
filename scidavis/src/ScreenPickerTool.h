/***************************************************************************
    File                 : ScreenPickerTool.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006,2007 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Plot tool for selecting arbitrary points.

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
#ifndef SCREEN_PICKER_TOOL_H
#define SCREEN_PICKER_TOOL_H

#include "PlotToolInterface.h"
#include <QObject>
#include <qwt_double_rect.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_picker.h>

/*!Plot tool for selecting arbitrary points.
 *
 * This is a rather thin wrapper around QwtPlotPicker, providing selection of points
 * on a Graph/Plot and displaying coordinates.
 */
class ScreenPickerTool : public QwtPlotPicker, public PlotToolInterface
{
	Q_OBJECT
	public:
		ScreenPickerTool(Graph *graph, const QObject *status_target=NULL, const char *status_slot="");
		virtual ~ScreenPickerTool();
		virtual bool eventFilter(QObject *obj, QEvent *event);
	signals:
		/*! Emitted whenever a new message should be presented to the user.
		 *
		 * You don't have to connect to this signal if you alreay specified a reciever during initialization.
		 */
		void statusText(const QString&);
	protected:
		virtual void append(const QPoint &point);
		QwtPlotMarker d_selection_marker;
};

#endif // ifndef SCREEN_PICKER_TOOL_H

