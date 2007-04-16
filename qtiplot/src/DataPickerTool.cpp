/***************************************************************************
    File                 : DataPickerTool.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006,2007 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Plot tool for selecting points on curves.

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
#include "DataPickerTool.h"
#include "Graph.h"
#include "Plot.h"
#include "cursors.h"
#include "FunctionCurve.h"
#include "PlotCurve.h"
#include "QwtErrorPlotCurve.h"
#include "ApplicationWindow.h"

#include <qwt_symbol.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_curve.h>
#include <QMessageBox>

DataPickerTool::DataPickerTool(Graph *graph, ApplicationWindow *app, Mode mode, const QObject *status_target, const char *status_slot) :
	QwtPlotPicker(graph->plotWidget()->canvas()),
	PlotToolInterface(graph),
	d_app(app),
	d_mode(mode)
{
	d_selected_curve = NULL;

	d_selection_marker.setSymbol(QwtSymbol(QwtSymbol::Ellipse, QBrush(QColor(255,255,0,128)), QPen(Qt::black,2), QSize(20,20)));
	d_selection_marker.setLineStyle(QwtPlotMarker::Cross);
	d_selection_marker.setLinePen(QPen(Qt::red,1));

	setTrackerMode(QwtPicker::AlwaysOn);
	if (d_mode == Move) {
		setSelectionFlags(QwtPicker::PointSelection | QwtPicker::DragSelection);
		d_graph->plotWidget()->canvas()->setCursor(Qt::pointingHandCursor);
	} else {
		setSelectionFlags(QwtPicker::PointSelection | QwtPicker::ClickSelection);
		d_graph->plotWidget()->canvas()->setCursor(QCursor(QPixmap(vizor_xpm), -1, -1));
	}

	if (status_target)
		connect(this, SIGNAL(statusText(const QString&)), status_target, status_slot);
	switch(d_mode) {
		case Display:
			emit statusText(tr("Click on plot or move cursor to display coordinates!"));
			break;
		case Move:
			emit statusText(tr("Please, click on plot and move cursor!"));
			break;
		case Remove:
			emit statusText(tr("Select point and double click to remove it!"));
			break;
	}
}

DataPickerTool::~DataPickerTool()
{
	d_selection_marker.detach();
	d_graph->plotWidget()->canvas()->unsetCursor();
}

void DataPickerTool::append(const QPoint &pos)
{
	int dist, point_index;
	const int curve = d_graph->plotWidget()->closestCurve(pos.x(), pos.y(), dist, point_index);
	if (curve <= 0 || dist >= 5) { // 5 pixels tolerance
		setSelection(NULL, 0);
		return;
	}
	setSelection((QwtPlotCurve *)d_graph->plotWidget()->curve(curve), point_index);
	if (!d_selected_curve) return;

	QwtPlotPicker::append(transform(QwtDoublePoint(d_selected_curve->x(d_selected_point),
					d_selected_curve->y(d_selected_point))));
}

void DataPickerTool::setSelection(QwtPlotCurve *curve, int point_index)
{
	if (curve == d_selected_curve && point_index == d_selected_point)
		return;

	d_selected_curve = curve;
	d_selected_point = point_index;

	if (!d_selected_curve) {
		d_selection_marker.detach();
		d_graph->plotWidget()->replot();
		return;
	}

	setAxis(d_selected_curve->xAxis(), d_selected_curve->yAxis());

	emit statusText(QString("%1[%2]: x=%3; y=%4")
			.arg(d_selected_curve->title().text())
			.arg(d_selected_point + 1)
			.arg(QString::number(d_selected_curve->x(d_selected_point), 'G', 15))
			.arg(QString::number(d_selected_curve->y(d_selected_point), 'G', 15)) );

	QwtDoublePoint selected_point_value(d_selected_curve->x(d_selected_point), d_selected_curve->y(d_selected_point));
	d_selection_marker.setValue(selected_point_value);
	if (d_selection_marker.plot() == NULL)
		d_selection_marker.attach(d_graph->plotWidget());
	d_graph->plotWidget()->replot();
}

bool DataPickerTool::eventFilter(QObject *obj, QEvent *event)
{
	switch(event->type()) {
		case QEvent::MouseButtonDblClick:
			switch(d_mode) {
				case Remove:
					removePoint();
					return true;
				default:
					if (d_selected_curve)
						emit selected(d_selected_curve, d_selected_point);
					return true;
			}
		case QEvent::KeyPress:
			if (keyEventFilter((QKeyEvent*)event))
				return true;
			break;
		default:
			break;
	}
	return QwtPlotPicker::eventFilter(obj, event);
}

bool DataPickerTool::keyEventFilter(QKeyEvent *ke)
{
	const int delta = 5;
	switch(ke->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
			emit selected(d_selected_curve, d_selected_point);
			return true;

		case Qt::Key_Up:
			{
				int n_curves = d_graph->curves();
				int start = d_graph->curveIndex(d_selected_curve) + 1;
				for (int i = start; i < start + n_curves; ++i)
					if (d_graph->curve(i % n_curves)->dataSize() > 0) {
						setSelection(d_graph->curve(i % n_curves), 0);
						break;
					}
				d_graph->plotWidget()->replot();
				return true;
			}

		case Qt::Key_Down:
			{
				int n_curves = d_graph->curves();
				int start = d_graph->curveIndex(d_selected_curve) + n_curves - 1;
				for (int i = start; i > start - n_curves; --i)
					if (d_graph->curve(i % n_curves)->dataSize() > 0) {
						setSelection(d_graph->curve(i % n_curves), 0);
						break;
					}
				d_graph->plotWidget()->replot();
				return true;
			}

		case Qt::Key_Right:
		case Qt::Key_Plus:
			if (d_selected_curve) {
				int n_points = d_selected_curve->dataSize();
				setSelection(d_selected_curve, (d_selected_point + 1) % n_points);
				d_graph->plotWidget()->replot();
			} else
				setSelection(d_graph->curve(0), 0);
			return true;

		case Qt::Key_Left:
		case Qt::Key_Minus:
			if (d_selected_curve) {
				int n_points = d_selected_curve->dataSize();
				setSelection(d_selected_curve, (d_selected_point - 1 + n_points) % n_points);
				d_graph->plotWidget()->replot();
			} else
				setSelection(d_graph->curve(d_graph->curves()-1), 0);
			return true;

		// The following keys represent a direction, they are
		// organized on the keyboard.
		case Qt::Key_1:
			if (d_mode == Move) {
				moveBy(-delta, delta);
				return true;
			}
			break;
		case Qt::Key_2:
			if (d_mode == Move) {
				moveBy(0, delta);
				return true;
			}
			break;
		case Qt::Key_3:
			if (d_mode == Move) {
				moveBy(delta, delta);
				return true;
			}
			break;
		case Qt::Key_4:
			if (d_mode == Move) {
				moveBy(-delta, 0);
				return true;
			}
			break;
		case Qt::Key_6:
			if (d_mode == Move) {
				moveBy(delta, 0);
				return true;
			}
			break;
		case Qt::Key_7:
			if (d_mode == Move) {
				moveBy(-delta, -delta);
				return true;
			}
			break;
		case Qt::Key_8:
			if (d_mode == Move) {
				moveBy(0, -delta);
				return true;
			}
			break;
		case Qt::Key_9:
			if (d_mode == Move) {
				moveBy(delta, -delta);
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

void DataPickerTool::removePoint()
{
	if ( !d_selected_curve )
		return;
	if (((PlotCurve *)d_selected_curve)->type() == Graph::Function)
	{
		QMessageBox::critical(d_graph, tr("QtiPlot - Remove point error"),
				tr("Sorry, but removing points of a function is not possible."));
		return;
	}

	if (((PlotCurve *)d_selected_curve)->type() == Graph::ErrorBars)
	{
        double val = ((QwtErrorPlotCurve *) d_selected_curve)->errors()[d_selected_point];
		d_app->clearCellFromTable(d_selected_curve->title().text(), val);
    }
    else
        d_app->clearCellFromTable(d_selected_curve->title().text(), d_selected_curve->y(d_selected_point));

	d_selection_marker.detach();
	d_graph->plotWidget()->replot();
	d_graph->setFocus();
	d_selected_curve = NULL;
}

void DataPickerTool::movePoint(const QPoint &pos)
{
	if ( !d_selected_curve )
		return;
	if ( ((PlotCurve *)d_selected_curve)->type() == Graph::Function)
	{
		QMessageBox::critical(d_graph, tr("QtiPlot - Move point error"),
				tr("Sorry, but moving points of a function is not possible."));
		return;
	}

	double new_x_val = d_graph->plotWidget()->invTransform(d_selected_curve->xAxis(), pos.x());
	double new_y_val = d_graph->plotWidget()->invTransform(d_selected_curve->yAxis(), pos.y());

    d_selection_marker.setValue(new_x_val, new_y_val);
	if (d_selection_marker.plot() == NULL)
		d_selection_marker.attach(d_graph->plotWidget());
	d_graph->plotWidget()->replot();

	// update source table which triggers an updateCurves()
	QString text = QString::number(new_x_val)+"\t"+QString::number(new_y_val);
    QString name = ((DataCurve *)d_selected_curve)->plotAssociation();
    d_app->updateTable(name, ((DataCurve *)d_selected_curve)->startRow() + d_selected_point, text);

	emit statusText(QString("%1[%2]: x=%3; y=%4")
			.arg(d_selected_curve->title().text())
			.arg(d_selected_point + 1)
			.arg(QString::number(new_x_val, 'G', 15))
			.arg(QString::number(new_y_val, 'G', 15)) );
}

void DataPickerTool::move(const QPoint &point)
{
	if (d_mode == Move)
		movePoint(point);
	QwtPlotPicker::move(point);
}

bool DataPickerTool::end(bool ok)
{
	if (d_mode == Move)
		d_selected_curve = NULL;
	return QwtPlotPicker::end(ok);
}

void DataPickerTool::moveBy(int dx, int dy)
{
	if ( !d_selected_curve )
		return;
	movePoint(transform(QwtDoublePoint(d_selected_curve->x(d_selected_point),
					d_selected_curve->y(d_selected_point))) + QPoint(dx, dy));
}
