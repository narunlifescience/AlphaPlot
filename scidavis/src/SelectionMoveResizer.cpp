/***************************************************************************
    File                 : SelectionMoveResizer.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Selection of Widgets and QwtPlotMarkers

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

#include "SelectionMoveResizer.h"

#include <QPainter>
#include <QMouseEvent>

#include <qwt_scale_map.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>

#include "Legend.h"
#include "ArrowMarker.h"
#include "ImageMarker.h"
#include "PlotEnrichement.h"

SelectionMoveResizer::SelectionMoveResizer(Legend *target)
	: QWidget(target->plot()->canvas())
{
	init();
	add(target);
}
SelectionMoveResizer::SelectionMoveResizer(ArrowMarker *target)
	: QWidget(target->plot()->canvas())
{
	init();
	add(target);
}
SelectionMoveResizer::SelectionMoveResizer(ImageMarker *target)
	: QWidget(target->plot()->canvas())
{
	init();
	add(target);
}

SelectionMoveResizer::SelectionMoveResizer(QWidget *target)
	: QWidget(target->parentWidget())
{
	init();
	add(target);
}

void SelectionMoveResizer::init()
{
	d_bounding_rect = QRect(0,0,-1,-1);
	d_op = None;
	d_op_start = QPoint(0,0);
	d_op_dp = QPoint(0,0);

	setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());
	setMouseTracking(true);
	parentWidget()->installEventFilter(this);
	show();
	setFocus();
}

SelectionMoveResizer::~SelectionMoveResizer()
{
	parentWidget()->removeEventFilter(this);
}

void SelectionMoveResizer::add(Legend *target)
{
	if ((QWidget*)target->plot()->canvas() != parent())
		return;
	d_legend_markers << target;

	if (d_bounding_rect.isValid())
		d_bounding_rect |= boundingRectOf(target);
	else
		d_bounding_rect = boundingRectOf(target);

	update();
}
void SelectionMoveResizer::add(ArrowMarker *target)
{
	if ((QWidget*)target->plot()->canvas() != parent())
		return;
	d_line_markers << target;

	if (d_bounding_rect.isValid())
		d_bounding_rect |= boundingRectOf(target);
	else
		d_bounding_rect = boundingRectOf(target);

	update();
}
void SelectionMoveResizer::add(ImageMarker *target)
{
	if ((QWidget*)target->plot()->canvas() != parent())
		return;
	d_image_markers << target;

	if (d_bounding_rect.isValid())
		d_bounding_rect |= boundingRectOf(target);
	else
		d_bounding_rect = boundingRectOf(target);

	update();
}

void SelectionMoveResizer::add(QWidget *target)
{
	if (target->parentWidget() != parent())
		return;
	d_widgets << target;
	target->installEventFilter(this);
	connect(target, SIGNAL(destroyed(QObject*)), this, SLOT(removeWidget(QObject*)));

	if (d_bounding_rect.isValid())
		d_bounding_rect |= target->frameGeometry();
	else
		d_bounding_rect = target->frameGeometry();

	update();
}

QRect SelectionMoveResizer::boundingRectOf(QwtPlotMarker *target) const
{
	return ((PlotEnrichement *)target)->rect();
}

int SelectionMoveResizer::removeAll(Legend *target)
{
	int result = d_legend_markers.removeAll(target);
	if (d_legend_markers.isEmpty() && d_line_markers.isEmpty() && d_image_markers.isEmpty() && d_widgets.isEmpty())
		delete this;
	else
		recalcBoundingRect();
	return result;
}
int SelectionMoveResizer::removeAll(ArrowMarker *target)
{
	int result = d_line_markers.removeAll(target);
	if (d_legend_markers.isEmpty() && d_line_markers.isEmpty() && d_image_markers.isEmpty() && d_widgets.isEmpty())
		delete this;
	else
		recalcBoundingRect();
	return result;
}
int SelectionMoveResizer::removeAll(ImageMarker *target)
{
	int result = d_image_markers.removeAll(target);
	if (d_legend_markers.isEmpty() && d_line_markers.isEmpty() && d_image_markers.isEmpty() && d_widgets.isEmpty())
		delete this;
	else
		recalcBoundingRect();
	return result;
}

int SelectionMoveResizer::removeAll(QWidget *target)
{
	int result = d_widgets.removeAll(target);
	if (d_legend_markers.isEmpty() && d_line_markers.isEmpty() && d_image_markers.isEmpty() && d_widgets.isEmpty())
		delete this;
	else
		recalcBoundingRect();
	return result;
}

void SelectionMoveResizer::recalcBoundingRect()
{
	d_bounding_rect = QRect(0,0,-1,-1);

	foreach(Legend *i, d_legend_markers) {
		if(d_bounding_rect.isValid())
			d_bounding_rect |= boundingRectOf(i);
		else
			d_bounding_rect = boundingRectOf(i);
	}
	foreach(ArrowMarker *i, d_line_markers) {
		if(d_bounding_rect.isValid())
			d_bounding_rect |= boundingRectOf(i);
		else
			d_bounding_rect = boundingRectOf(i);
	}
	foreach(ImageMarker *i, d_image_markers) {
		if(d_bounding_rect.isValid())
			d_bounding_rect |= boundingRectOf(i);
		else
			d_bounding_rect = boundingRectOf(i);
	}
	foreach(QWidget *i, d_widgets) {
		if(d_bounding_rect.isValid())
			d_bounding_rect |= i->frameGeometry();
		else
			d_bounding_rect = i->frameGeometry();
	}

	update();
}

const QRect SelectionMoveResizer::handlerRect(QRect rect, Operation op)
{
	switch(op) {
		case Resize_N:
			return QRect( QPoint(rect.center().x()-handler_size/2, rect.top()),
					QSize(handler_size,handler_size));
		case Resize_NE:
			return QRect( QPoint(rect.right()-handler_size+1, rect.top()),
				QSize(handler_size,handler_size));
		case Resize_E:
			return QRect( QPoint(rect.right()-handler_size+1, rect.center().y()-handler_size/2),
					QSize(handler_size,handler_size));
		case Resize_SE:
			return QRect( QPoint(rect.right()-handler_size+1, rect.bottom()-handler_size+1),
					QSize(handler_size,handler_size));
		case Resize_S:
			return QRect( QPoint(rect.center().x()-handler_size/2, rect.bottom()-handler_size+1),
					QSize(handler_size,handler_size));
		case Resize_SW:
			return QRect( QPoint(rect.left(),rect.bottom()-handler_size+1),
					QSize(handler_size,handler_size));
		case Resize_W:
			return QRect( QPoint(rect.left(), rect.center().y()-handler_size/2),
					QSize(handler_size,handler_size));
		case Resize_NW:
			return QRect( rect.topLeft(),
					QSize(handler_size,handler_size));
		default:
			return QRect( rect.topLeft(), QSize(0,0));
	}
}

QRect SelectionMoveResizer::operateOn(const QRect in)
{
	QRect boundary_out = d_bounding_rect;
	switch (d_op) {
		case Move:
			boundary_out.translate(d_op_dp);
			break;
		case Resize_N:
			boundary_out.setTop(boundary_out.top()+d_op_dp.y());
			break;
		case Resize_E:
			boundary_out.setRight(boundary_out.right()+d_op_dp.x());
			break;
		case Resize_S:
			boundary_out.setBottom(boundary_out.bottom()+d_op_dp.y());
			break;
		case Resize_W:
			boundary_out.setLeft(boundary_out.left()+d_op_dp.x());
			break;
		case Resize_NE:
			boundary_out.setTop(boundary_out.top()+d_op_dp.y());
			boundary_out.setRight(boundary_out.right()+d_op_dp.x());
			break;
		case Resize_SE:
			boundary_out.setBottom(boundary_out.bottom()+d_op_dp.y());
			boundary_out.setRight(boundary_out.right()+d_op_dp.x());
			break;
		case Resize_SW:
			boundary_out.setBottom(boundary_out.bottom()+d_op_dp.y());
			boundary_out.setLeft(boundary_out.left()+d_op_dp.x());
			break;
		case Resize_NW:
			boundary_out.setTop(boundary_out.top()+d_op_dp.y());
			boundary_out.setLeft(boundary_out.left()+d_op_dp.x());
			break;
		default:
			return in;
	}
	boundary_out = boundary_out.normalized();
	if (in == d_bounding_rect)
		return boundary_out;
	double scale_x = ((double)boundary_out.width())/d_bounding_rect.width();
	double scale_y = ((double)boundary_out.height())/d_bounding_rect.height();
	int offset_x = qRound(boundary_out.left()-d_bounding_rect.left()*scale_x);
	int offset_y = qRound(boundary_out.top()-d_bounding_rect.top()*scale_y);
	return QRect(QPoint(qRound(in.left()*scale_x)+offset_x, qRound(in.top()*scale_y)+offset_y),
			QSize(qRound(in.width()*scale_x), qRound(in.height()*scale_y)));
}

void SelectionMoveResizer::operateOnTargets()
{
	foreach(Legend *i, d_legend_markers) {
		QRect new_rect = operateOn(i->rect());
		i->setOrigin(new_rect.topLeft());
		if (!i->text().isEmpty())
		{
            QFont f = i->font();
            f.setPointSize(f.pointSize() * new_rect.width() * new_rect.height()/(i->rect().width() * i->rect().height()));
            i->setFont(f);
		}
	}
	foreach(ArrowMarker *i, d_line_markers) {
		QPoint p1 = i->startPoint();
		QPoint p2 = i->endPoint();
		QRect new_rect = operateOn(i->rect());
		i->setStartPoint(QPoint(
					p1.x()<p2.x() ? new_rect.left() : new_rect.right(),
					p1.y()<p2.y() ? new_rect.top() : new_rect.bottom() ));
		i->setEndPoint(QPoint(
					p2.x()<p1.x() ? new_rect.left() : new_rect.right(),
					p2.y()<p1.y() ? new_rect.top() : new_rect.bottom() ));
	}
	foreach(ImageMarker *i, d_image_markers) {
		QRect new_rect = operateOn(i->rect());
		i->setOrigin(new_rect.topLeft());
		i->setSize(new_rect.size());
	}
	foreach(QWidget *i, d_widgets)
		i->setGeometry(operateOn(i->geometry()));
	recalcBoundingRect();

	d_op_start = d_op_dp = QPoint(0,0);

	update();
	emit targetsChanged();
}

void SelectionMoveResizer::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	QRect drawn_rect = operateOn(d_bounding_rect);

	QPen white_pen(Qt::white, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	p.setPen(white_pen);
	p.drawRect(QRect(QPoint(drawn_rect.left(), drawn_rect.top()), drawn_rect.size()));
	white_pen.setWidth(2); p.setPen(white_pen);
	for (int i=0; i<8; i++)
		p.drawRect(handlerRect(drawn_rect, (Operation)i));
	p.setPen(QPen(Qt::black,1,Qt::SolidLine));
	p.drawRect(QRect(QPoint(drawn_rect.left(), drawn_rect.top()), drawn_rect.size()));
	for (int i=0; i<8; i++)
		p.fillRect(handlerRect(drawn_rect, (Operation)i), QBrush(Qt::black));

	e->accept();
}

void SelectionMoveResizer::mousePressEvent(QMouseEvent *me)
{
	if (me->button() == Qt::RightButton) {
		// If one of the parents' event handlers deletes me, Qt crashes while trying to send the QContextMenuEvent.
		me->accept();
		return;
	}
	if (me->button() != Qt::LeftButton || !d_bounding_rect.contains(me->pos())) {
		me->ignore();
		return;
	}
	d_op_start = me->pos();
	d_op = Move;
	for (int i=0; i<8; i++)
		if (handlerRect(d_bounding_rect, (Operation)i).contains(d_op_start)) {
			d_op = (Operation)i;
			break;
		}

	me->accept();
}

void SelectionMoveResizer::mouseMoveEvent(QMouseEvent *me)
{
	if (d_op == None && d_bounding_rect.contains(me->pos()) && me->button() == Qt::LeftButton) {
		d_op = Move;
		d_op_start = me->pos();
	}
	if (d_op == None || d_op_start == QPoint(-1,-1)) {
		if (d_bounding_rect.contains(me->pos())) {
			setCursor(Qt::SizeAllCursor);
			if (handlerRect(d_bounding_rect, Resize_N).contains(me->pos()) || handlerRect(d_bounding_rect, Resize_S).contains(me->pos()))
				setCursor(Qt::SizeVerCursor);
			else if (handlerRect(d_bounding_rect, Resize_E).contains(me->pos()) || handlerRect(d_bounding_rect, Resize_W).contains(me->pos()))
				setCursor(Qt::SizeHorCursor);
			else if (handlerRect(d_bounding_rect, Resize_NE).contains(me->pos()) || handlerRect(d_bounding_rect, Resize_SW).contains(me->pos()))
				setCursor(Qt::SizeBDiagCursor);
			else if (handlerRect(d_bounding_rect, Resize_NW).contains(me->pos()) || handlerRect(d_bounding_rect, Resize_SE).contains(me->pos()))
				setCursor(Qt::SizeFDiagCursor);
		} else
			unsetCursor();
		return QWidget::mouseMoveEvent(me);
	}
	d_op_dp = me->pos() - d_op_start;
	repaint();
	me->accept();
}

void SelectionMoveResizer::mouseDoubleClickEvent(QMouseEvent *e)
{
	e->ignore();
}

void SelectionMoveResizer::mouseReleaseEvent(QMouseEvent *me)
{
	if (me->button() != Qt::LeftButton || d_op == None || d_op_start == QPoint(-1,-1))
		return QWidget::mouseReleaseEvent(me);

	d_op_dp = me->pos() - d_op_start;
	operateOnTargets();
	d_op = None;
	me->accept();
}

void SelectionMoveResizer::keyPressEvent(QKeyEvent *ke)
{
	switch(ke->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
			if (d_op == None) {
				ke->ignore();
				break;
			}
			operateOnTargets();
			d_op = None;
			ke->accept();
			break;
		case Qt::Key_Escape:
			delete this;
			ke->accept();
			return;
		case Qt::Key_Left:
			if (d_op == None) {
				d_op = Move;
				d_op_start = QPoint(-1,-1);
			}
			d_op_dp += QPoint(-1,0);
			repaint();
			ke->accept();
			break;
		case Qt::Key_Right:
			if (d_op == None) {
				d_op = Move;
				d_op_start = QPoint(-1,-1);
			}
			d_op_dp += QPoint(1,0);
			repaint();
			ke->accept();
			break;
		case Qt::Key_Up:
			if (d_op == None) {
				d_op = Move;
				d_op_start = QPoint(-1,-1);
			}
			d_op_dp += QPoint(0,-1);
			repaint();
			ke->accept();
			break;
		case Qt::Key_Down:
			if (d_op == None) {
				d_op = Move;
				d_op_start = QPoint(-1,-1);
			}
			d_op_dp += QPoint(0,1);
			repaint();
			ke->accept();
			break;
		default:
			ke->ignore();
	}
}

bool SelectionMoveResizer::eventFilter(QObject *o, QEvent *e)
{
	switch (e->type()) {
		case QEvent::Resize:
			if((QWidget*)o == parentWidget())
				setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());
			recalcBoundingRect();
			return false;
		case QEvent::Move:
			if((QWidget*)o != parentWidget())
				recalcBoundingRect();
			return false;
		default:
			return false;
	}
}
