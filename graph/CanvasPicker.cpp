/***************************************************************************
    File                 : CanvasPicker.cpp
    Project              : SciDAVis
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

#include "Layer.h"
#include "TextEnrichment.h"
#include "enrichments/ImageEnrichment.h"
#include "enrichments/LineEnrichment.h"

#include <QVector>
#include <QEvent>

#include <qwt_text_label.h>
#include <qwt_plot_canvas.h>

CanvasPicker::CanvasPicker(Layer *layer):
	QObject(layer)
{
	pointSelected = false;
	d_editing_marker = 0;

	plotWidget=layer->plotWidget();

	QwtPlotCanvas *canvas = plotWidget->canvas();
	canvas->installEventFilter(this);
}

bool CanvasPicker::eventFilter(QObject *object, QEvent *e)
{
	QVector<int> images=layer()->imageMarkerKeys();
	QVector<int> texts=layer()->textMarkerKeys();
	QVector<int> lines=layer()->lineMarkerKeys();

	if (object != (QObject *)layer()->plotWidget()->canvas())
		return false;

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

				if (me->button()==Qt::LeftButton && (layer()->drawLineActive()))
				{
					startLinePoint = me->pos();
					return true;
				}

				if (me->button()==Qt::LeftButton && layer()->drawTextActive()) {
					drawTextMarker(me->pos());
					return true;
				}

				if (!layer()->zoomOn() && selectMarker(me)) {
					if (me->button() == Qt::RightButton)
						emit showMarkerPopupMenu();
					return true;
				}

				if (d_editing_marker) {
					d_editing_marker->setEditable(false);
					d_editing_marker = 0;
				}

				if(layer()->markerSelected())
					layer()->deselectMarker();

				return !(me->modifiers() & Qt::ShiftModifier);
			}
			break;

		case QEvent::MouseButtonDblClick:
			{
				if (d_editing_marker) {
					return d_editing_marker->eventFilter(plotWidget->canvas(), e);
				} else if (layer()->selectedMarkerKey() >= 0) {
					if (texts.contains(layer()->selectedMarkerKey()))
					{
						emit viewTextDialog();
						return true;
					}
					else if (lines.contains(layer()->selectedMarkerKey()))
					{
						emit viewLineDialog();
						return true;
					}
					else if (images.contains(layer()->selectedMarkerKey()))
					{
						emit viewImageDialog();
						return true;
					}
				} else if (layer()->isPiePlot()){
                        emit showPlotDialog(layer()->curveKey(0));
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

				if (layer()->drawLineActive()) {
					drawLineMarker(pos, layer()->drawArrow());
					return true;
				}

				return false;
			}
			break;

		case QEvent::MouseButtonRelease:
			{
				const QMouseEvent *me = (const QMouseEvent *)e;

				if (layer()->drawLineActive()) {
					LineEnrichment mrk;
					mrk.attach(layer()->plotWidget());
					mrk.setStartPoint(startLinePoint);
					mrk.setEndPoint(QPoint(me->x(), me->y()));
					mrk.setColor(layer()->arrowDefaultColor());
					mrk.setWidth(layer()->arrowDefaultWidth());
					mrk.setStyle(layer()->arrowLineDefaultStyle());
					mrk.setHeadLength(layer()->arrowHeadDefaultLength());
					mrk.setHeadAngle(layer()->arrowHeadDefaultAngle());
					mrk.fillArrowHead(layer()->arrowHeadDefaultFill());
					mrk.drawEndArrow(layer()->drawArrow());
					mrk.drawStartArrow(false);

					layer()->addArrow(&mrk);
					layer()->drawLine(false);
					mrk.detach();
					plotWidget->replot();

					return true;
				}
				return false;
			}
			break;

		case QEvent::KeyPress:
			{
				int key=((const QKeyEvent *)e)->key();

				long selectedMarker = layer()->selectedMarkerKey();
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
	TextEnrichment mrkT(plotWidget);
	mrkT.setOrigin(point);
	mrkT.setFrameStyle(layer()->textMarkerDefaultFrame());
	mrkT.setFont(layer()->defaultTextMarkerFont());
	mrkT.setTextColor(layer()->textMarkerDefaultColor());
	mrkT.setBackgroundColor(layer()->textMarkerDefaultBackground());
	mrkT.setText(tr("enter your text here"));
	layer()->insertTextMarker(&mrkT);
	layer()->drawText(FALSE);
	emit drawTextOff();
}

void CanvasPicker::drawLineMarker(const QPoint& point, bool endArrow)
{
	layer()->plotWidget()->canvas()->repaint();
	LineEnrichment mrk;
	mrk.attach(plotWidget);

	int clw = plotWidget->canvas()->lineWidth();
	mrk.setStartPoint(QPoint(startLinePoint.x() + clw, startLinePoint.y() + clw));
	mrk.setEndPoint(QPoint(point.x() + clw,point.y() + clw));
	mrk.setWidth(1);
	mrk.setStyle(Qt::SolidLine);
	mrk.drawEndArrow(endArrow);
	mrk.drawStartArrow(false);

	if (layer()->drawLineActive())
		mrk.setColor(Qt::black);
	else
		mrk.setColor(Qt::red);

	plotWidget->replot();
	mrk.detach();
}

bool CanvasPicker::selectMarker(const QMouseEvent *e)
{
	const QPoint point = e->pos();
	foreach(long i, layer()->textMarkerKeys()) {
		TextEnrichment *m = (TextEnrichment*)plotWidget->marker(i);
		if (!m) return false;
		if (m->rect().contains(point)) {
			if (d_editing_marker) {
				d_editing_marker->setEditable(false);
				d_editing_marker = 0;
			}
			layer()->setSelectedMarker(i, e->modifiers() & Qt::ShiftModifier);
			return true;
		}
	}
	foreach(long i, layer()->imageMarkerKeys()) {
		ImageEnrichment* m=(ImageEnrichment*)plotWidget->marker(i);
		if (!m) return false;
		if (m->rect().contains(point)) {
			if (d_editing_marker) {
				d_editing_marker->setEditable(false);
				d_editing_marker = 0;
			}
			layer()->setSelectedMarker(i, e->modifiers() & Qt::ShiftModifier);
			return true;
		}
	}
	foreach(long i, layer()->lineMarkerKeys()) {
		LineEnrichment* mrkL=(LineEnrichment*) plotWidget->marker(i);
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
				layer()->setSelectedMarker(i, true);
				return true;
			} else if (e->button() == Qt::RightButton) {
				mrkL->setEditable(false);
				layer()->setSelectedMarker(i, false);
				return true;
			}
			layer()->deselectMarker();
			mrkL->setEditable(true);
			d_editing_marker = mrkL;
			return true;
		}
	}
	return false;
}
