/***************************************************************************
    File                 : MultiPeakFitTool.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006,2007 by Ion Vasilief,
                           Tilman Benkert, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Plot tool for doing multi-peak fitting.

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
#ifndef MULTI_PEAK_FIT_TOOL
#define MULTI_PEAK_FIT_TOOL

#include "graph/AbstractGraphTool.h"
#include "MultiPeakFit.h"
#include <QObject>

class DataPickerTool;
class ApplicationWindow;
class QwtPlotCurve;

/*! Plot tool for doing multi-peak fitting.
 *
 * This class can be seen as a user-interface wrapper around MultiPeakFit, providing functionality for visually
 * selecting estimated peak positions and finally executing the fit.
 *
 * Maybe some other parts of the multi-peak fitting process (namely, specifying the number of peaks and optionally
 * the peak profile) could be moved here as well.
 */
class MultiPeakFitTool : public QObject, public AbstractGraphTool
{
	Q_OBJECT
	public:
		MultiPeakFitTool(Layer *layer, ApplicationWindow *app, MultiPeakFit::PeakProfile profile, int num_peaks, const QObject *status_target, const char *status_slot);
		virtual ~MultiPeakFitTool();
	signals:
		/*! Emitted whenever a new message should be presented to the user.
		 *
		 * You don't have to connect to this signal if you alreay specified a reciever during initialization.
		 */
		void statusText(const QString&);
	protected slots:
		void selectPeak(QwtPlotCurve *curve, int point_index);
	private:
		void finalize();
		MultiPeakFit::PeakProfile m_profile;
		int m_num_peaks, m_selected_peaks;
		DataPickerTool *m_picker_tool;
		MultiPeakFit *m_fit;
		QwtPlotCurve *m_curve;
};

#endif // ifndef MULTI_PEAK_FIT_TOOL

