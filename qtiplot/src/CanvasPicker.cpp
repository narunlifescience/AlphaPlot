/***************************************************************************
    File                 : CanvasPicker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006,2007 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
                           knut.franke*gmx.de
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
#include "CanvasPicker.h"
#include "ImageMarker.h"
#include "LegendMarker.h"
#include "LineMarker.h"

#include <QVector>

#include <qwt_text_label.h>
#include <qwt_plot_canvas.h>

CanvasPicker::CanvasPicker(Graph *graph):
	QObject(graph)
{
	pointSelected = false;
	d_editing_marker = 0;

	plotWidget=graph->plotWidget();

	QwtPlotCanvas *canvas = plotWidget->canvas();
	canvas->installEventFilter(this);
}

bool CanvasPicker::eventFilter(QObject *object, QEvent *e)
{
	QVector<int> images=plot()->imageMarkerKeys();
	QVector<int> texts=plot()->textMarkerKeys();
	QVector<int> lines=plot()->lineMarkerKeys();

	if (object != (QObject *)plot()->plotWidget()->canvas())
		return FALSE;

	switch(e->type())
	{
		case QEvent::MouseButtonPress:
			{
				emit selectPlot();

				const QMouseEvent *me = (const QMouseEvent *)e;

				bool allAxisDisabled = true;
				for (int i=0; i < QwtPlot::axisCnt; i++)
				{
					if (plotWidget->axisEnabled(i))
					{
						allAxisDisabled = false;
						break;
					}
				}

				if (me->button()==Qt::LeftButton && (plot()->drawLineActive() || plot()->lineProfile()))
				{
					startLinePoint = me->pos();
					return true;
				}

				if (me->button()==Qt::LeftButton && plot()->drawTextActive()) {
					drawTextMarker(me->pos());
					return true;
				}

				if (!plot()->zoomOn() && selectMarker(me)) {
					if (me->button() == Qt::RightButton)
						emit showMarkerPopupMenu();
					return true;
				}

				if (d_editing_marker) {
					d_editing_marker->setEditable(false);
					d_editing_marker = 0;
				}

				if(plot()->markerSelected())
					plot()->deselectMarker();

				return !(me->modifiers() & Qt::ShiftModifier);
			}
			break;

		case QEvent::MouseButtonDblClick:
			{
				if (d_editing_marker) {
					return d_editing_marker->eventFilter(plotWidget->canvas(), e);
				} else if (plot()->selectedMarkerKey() >= 0) {
					if (texts.contains(plot()->selectedMarkerKey()))
					{
						emit viewTextDialog();
						return true;
					}
					else if (lines.contains(plot()->selectedMarkerKey()))
					{
						emit viewLineDialog();
						return true;
					}
					else if (images.contains(plot()->selectedMarkerKey()))
					{
						emit viewImageDialog();
						return true;
					}
				} else if (plot()->isPiePlot()){
                        emit showPlotDialog(plot()->curveKey(0));
                        return true;
				}
                  else{
					const QMouseEvent *me = (const QMouseEvent *)e;
                    int dist, point;
                    int curveKey = plotWidget->closestCurve(me->pos().x(), me->pos().y(), dist, point);
                    if (dist < 10)
                        emit showPlotDialog(curveKey);
                    else
                        emit showPlotDialog(-1);
					return true;
				}
			}
			break;

		case QEvent::MouseMove:
			{
				const QMouseEvent *me = (const QMouseEvent *)e;
				if (me->state() != Qt::LeftButton)
  	            	return true;

				QPoint pos = me->pos();

				if (plot()->drawLineActive()) {
					drawLineMarker(pos, plot()->drawArrow());
					return true;
				} else if (plot()->lineProfile()) {
					drawLineMarker(pos,FALSE);
					return true;
				}

				return false;
			}
			break;

		case QEvent::MouseButtonRelease:
			{
				const QMouseEvent *me = (const QMouseEvent *)e;
				Graph *g = plot();

				if (g->drawLineActive()) {
					LineMarker mrk;
					mrk.attach(g->plotWidget());
					mrk.setStartPoint(startLinePoint);
					mrk.setEndPoint(QPoint(me->x(), me->y()));
					mrk.setColor(g->arrowDefaultColor());
					mrk.setWidth(g->arrowDefaultWidth());
					mrk.setStyle(g->arrowLineDefaultStyle());
					mrk.setHeadLength(g->arrowHeadDefaultLength());
					mrk.setHeadAngle(g->arrowHeadDefaultAngle());
					mrk.fillArrowHead(g->arrowHeadDefaultFill());
					mrk.drawEndArrow(g->drawArrow());
					mrk.drawStartArrow(false);

					g->insertLineMarker(&mrk);
					g->drawLine(false);
					mrk.detach();
					plotWidget->replot();

					return true;
				}
				else if (plot()->lineProfile())
				{
					QPoint endLinePoint=QPoint(me->x(),me->y());
					if (endLinePoint == startLinePoint)
						return FALSE;
					LineMarker mrk;
					mrk.attach(g->plotWidget());
					mrk.setStartPoint(startLinePoint);
					mrk.setEndPoint(endLinePoint);
					mrk.setColor(Qt::red);
					mrk.setWidth(1);
					Qt::PenStyle style=Qt::SolidLine;
					mrk.setStyle(style);
					mrk.drawEndArrow(false);
					mrk.drawStartArrow(false);

					mrk.detach();
					plot()->insertLineMarker(&mrk);
					plotWidget->replot();
					emit calculateProfile(startLinePoint,endLinePoint);

					return true;
				}
				return false;
			}
			break;

		case QEvent::KeyPress:
			{
				int key=((const QKeyEvent *)e)->key();

				long selectedMarker = plot()->selectedMarkerKey();
				if (texts.contains(selectedMarker) &&
						(key==Qt::Key_Enter || key==Qt::Key_Return))
				{
					emit viewTextDialog();
					return true;
				}
				if (lines.contains(selectedMarker) &&
						(key==Qt::Key_Enter || key==Qt::Key_Return))
				{
					emit viewLineDialog();
					return true;
				}
				if (images.contains(selectedMarker) &&
						(key==Qt::Key_Enter || key==Qt::Key_Return))
				{
					emit viewImageDialog();
					return true;
				}
			}
			break;

		default:
			break;
	}
	return QObject::eventFilter(object, e);
}

void CanvasPicker::disableEditing()
{
	if (d_editing_marker) {
		d_editing_marker->setEditable(false);
		d_editing_marker = 0;
	}
}

void CanvasPicker::drawTextMarker(const QPoint& point)
{
	LegendMarker mrkT(plotWidget);
	mrkT.setOrigin(point);
	mrkT.setFrameStyle(plot()->textMarkerDefaultFrame());
	mrkT.setFont(plot()->defaultTextMarkerFont());
	mrkT.setTextColor(plot()->textMarkerDefaultColor());
	mrkT.setBackgroundColor(plot()->textMarkerDefaultBackground());
	mrkT.setText(tr("enter your text here"));
	plot()->insertTextMarker(&mrkT);
	plot()->drawText(FALSE);
	emit drawTextOff();
}

void CanvasPicker::drawLineMarker(const QPoint& point, bool endArrow)
{
	plot()->plotWidget()->canvas()->repaint();
	LineMarker mrk;
	mrk.attach(plotWidget);

	int clw = plotWidget->canvas()->lineWidth();
	mrk.setStartPoint(QPoint(startLinePoint.x() + clw, startLinePoint.y() + clw));
	mrk.setEndPoint(QPoint(point.x() + clw,point.y() + clw));
	mrk.setWidth(1);
	mrk.setStyle(Qt::SolidLine);
	mrk.drawEndArrow(endArrow);
	mrk.drawStartArrow(false);

	if (plot()->drawLineActive())
		mrk.setColor(Qt::black);
	else
		mrk.setColor(Qt::red);

	plotWidget->replot();
	mrk.detach();
}

bool CanvasPicker::selectMarker(const QMouseEvent *e)
{
	const QPoint point = e->pos();
	foreach(long i, plot()->textMarkerKeys()) {
		LegendMarker *m = (LegendMarker*)plotWidget->marker(i);
		if (!m) return false;
		if (m->rect().contains(point)) {
			if (d_editing_marker) {
				d_editing_marker->setEditable(false);
				d_editing_marker = 0;
			}
			plot()->setSelectedMarker(i, e->modifiers() & Qt::ShiftModifier);
			return true;
		}
	}
	foreach(long i, plot()->imageMarkerKeys()) {
		ImageMarker* m=(ImageMarker*)plotWidget->marker(i);
		if (!m) return false;
		if (m->rect().contains(point)) {
			if (d_editing_marker) {
				d_editing_marker->setEditable(false);
				d_editing_marker = 0;
			}
			plot()->setSelectedMarker(i, e->modifiers() & Qt::ShiftModifier);
			return true;
		}
	}
	foreach(long i, plot()->lineMarkerKeys()) {
		LineMarker* mrkL=(LineMarker*) plotWidget->marker(i);
		if (!mrkL)
			return false;
		int d=mrkL->width()+(int)floor(mrkL->headLength()*tan(M_PI*mrkL->headAngle()/180.0)+0.5);
		double dist=mrkL->dist(point.x(),point.y());
		if (dist <= d)
		{
			if (d_editing_marker) {
				d_editing_marker->setEditable(false);
				d_editing_marker = 0;
			}
			if (e->modifiers() & Qt::ShiftModifier) {
				plot()->setSelectedMarker(i, true);
				return true;
			} else if (e->button() == Qt::RightButton) {
				mrkL->setEditable(false);
				plot()->setSelectedMarker(i, false);
				return true;
			}
			plot()->deselectMarker();
			mrkL->setEditable(true);
			d_editing_marker = mrkL;
			return true;
		}
	}
	return false;
}
