/***************************************************************************
    File                 : MultiPeakFitTool.cpp
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
#include "MultiPeakFitTool.h"
#include "core/ApplicationWindow.h"
#include "graph/tools/DataPickerTool.h"
#include "graph/Layer.h"
#include "graph/Plot.h"
#include <qwt_plot_curve.h>
#include <QApplication>

MultiPeakFitTool::MultiPeakFitTool(Layer *layer, ApplicationWindow *app, MultiPeakFit::PeakProfile profile, int num_peaks, const QObject *status_target, const char *status_slot)
	: AbstractGraphTool(layer),
	m_profile(profile),
	m_num_peaks(num_peaks)
{
	m_selected_peaks = 0;
	m_curve = 0;

	m_fit = new MultiPeakFit(app, m_layer, m_profile, m_num_peaks);
	m_fit->enablePeakCurves(app->generatePeakCurves);
	m_fit->setPeakCurvesColor(app->peakCurvesColor);
	m_fit->generateFunction(app->generateUniformFitPoints, app->fitPoints);

	if (status_target)
		connect(this, SIGNAL(statusText(const QString&)), status_target, status_slot);
	m_picker_tool = new DataPickerTool(m_layer, app, DataPickerTool::Display, this, SIGNAL(statusText(const QString&)));
	connect(m_picker_tool, SIGNAL(selected(QwtPlotCurve*,int)), this, SLOT(selectPeak(QwtPlotCurve*,int)));
	m_layer->plotWidget()->canvas()->grabMouse();
	emit statusText(tr("Move cursor and click to select a point and double-click/press 'Enter' to set the position of a peak!"));
}

MultiPeakFitTool::~MultiPeakFitTool()
{
	if (m_picker_tool)
		delete m_picker_tool;
	if (m_fit)
		delete m_fit;
	m_layer->plotWidget()->canvas()->unsetCursor();
}

void MultiPeakFitTool::selectPeak(QwtPlotCurve *curve, int point_index)
{
	// TODO: warn user
	if (!curve || (m_curve && m_curve != curve))
		return;
	m_curve = curve;

	m_fit->setInitialGuess(3*m_selected_peaks, curve->y(point_index));
	m_fit->setInitialGuess(3*m_selected_peaks+1, curve->x(point_index));

	QwtPlotMarker *m = new QwtPlotMarker();
	m->setLineStyle(QwtPlotMarker::VLine);
	m->setLinePen(QPen(Qt::green, 2, Qt::DashLine));
	m->setXValue(curve->x(point_index));
	m_layer->plotWidget()->insertMarker(m);
	m_layer->plotWidget()->replot();

	m_selected_peaks++;
	if (m_selected_peaks == m_num_peaks)
		finalize();
	else
		emit statusText(
				tr("Peak %1 selected! Click to select a point and double-click/press 'Enter' to set the position of the next peak!")
				.arg(QString::number(m_selected_peaks)));
}

void MultiPeakFitTool::finalize()
{
	delete m_picker_tool; m_picker_tool = NULL;
	m_layer->plotWidget()->canvas()->releaseMouse();

	if (m_fit->setDataFromCurve(m_curve->title().text()))
	{
		QApplication::setOverrideCursor(Qt::WaitCursor);
		m_fit->fit();
		delete m_fit; m_fit = NULL;
		QApplication::restoreOverrideCursor();
	}

	//remove peak line markers
	QList<int>mrks = m_layer->plotWidget()->markerKeys();
	int n=(int)mrks.count();
	for (int i=0; i<m_num_peaks; i++)
		m_layer->plotWidget()->removeMarker(mrks[n-i-1]);

	m_layer->plotWidget()->replot();

	m_layer->setActiveTool(NULL);
	// attention: I'm now deleted. Maybe there is a cleaner solution...
}

