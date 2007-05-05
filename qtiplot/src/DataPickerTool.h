/***************************************************************************
    File                 : DataPickerTool.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006,2007 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Plot tool for selecting individual points of a curve.

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
#ifndef DATA_PICKER_TOOL_H
#define DATA_PICKER_TOOL_H

#include "PlotToolInterface.h"
#include <qwt_plot_marker.h>
#include <qwt_plot_picker.h>

class ApplicationWindow;
class QwtPlotCurve;
class QPoint;

//! Plot tool for selecting, moving or removing individual points of a curve.
class DataPickerTool : public QwtPlotPicker, public PlotToolInterface
{
	Q_OBJECT
	public:
		enum Mode { Display, Move, Remove };
		DataPickerTool(Graph *graph, ApplicationWindow *app, Mode mode, const QObject *status_target=NULL, const char *status_slot="");
		virtual ~DataPickerTool();
		virtual bool eventFilter(QObject *obj, QEvent *event);
		bool keyEventFilter(QKeyEvent *ke);
		QwtPlotCurve *selectedCurve() const { return d_selected_curve; }
	signals:
		/*! Emitted whenever a new message should be presented to the user.
		 *
		 * You don't have to connect to this signal if you alreay specified a reciever during initialization.
		 */
		void statusText(const QString&);
		//! Emitted whenever a new data point has been selected.
		void selected(QwtPlotCurve*, int);
	protected:
		void movePoint(const QPoint &cursor);
		void removePoint();
		virtual void append(const QPoint &point);
		virtual void move(const QPoint &point);
		virtual bool end(bool ok);
		void setSelection(QwtPlotCurve *curve, int point_index);
		void moveBy(int dx, int dy);
	private:
		ApplicationWindow *d_app;
		QwtPlotMarker d_selection_marker;
		Mode d_mode;
		QwtPlotCurve *d_selected_curve;
		int d_selected_point;
};

#endif // ifndef DATA_PICKER_TOOL_H

