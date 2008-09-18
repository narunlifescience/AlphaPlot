/***************************************************************************
    File                 : SelectionMoveResizer.cpp
    Project              : SciDAVis
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

#include "TextEnrichment.h"
#include "enrichments/LineEnrichment.h"
#include "enrichments/ImageEnrichment.h"
#include "AbstractEnrichment.h"

SelectionMoveResizer::SelectionMoveResizer(TextEnrichment *target)
	: QWidget(target->plot()->canvas())
{
	init();
	add(target);
}
SelectionMoveResizer::SelectionMoveResizer(LineEnrichment *target)
	: QWidget(target->plot()->canvas())
{
	init();
	add(target);
}
SelectionMoveResizer::SelectionMoveResizer(ImageEnrichment *target)
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
	m_bounding_rect = QRect(0,0,-1,-1);
	m_op = None;
	m_op_start = QPoint(0,0);
	m_op_dp = QPoint(0,0);

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

void SelectionMoveResizer::add(TextEnrichment *target)
{
	if ((QWidget*)target->plot()->canvas() != parent())
		return;
	m_legend_markers << target;

	if (m_bounding_rect.isValid())
		m_bounding_rect |= boundingRectOf(target);
	else
		m_bounding_rect = boundingRectOf(target);

	update();
}
void SelectionMoveResizer::add(LineEnrichment *target)
{
	if ((QWidget*)target->plot()->canvas() != parent())
		return;
	m_line_markers << target;

	if (m_bounding_rect.isValid())
		m_bounding_rect |= boundingRectOf(target);
	else
		m_bounding_rect = boundingRectOf(target);

	update();
}
void SelectionMoveResizer::add(ImageEnrichment *target)
{
	if ((QWidget*)target->plot()->canvas() != parent())
		return;
	m_image_markers << target;

	if (m_bounding_rect.isValid())
		m_bounding_rect |= boundingRectOf(target);
	else
		m_bounding_rect = boundingRectOf(target);

	update();
}

void SelectionMoveResizer::add(QWidget *target)
{
	if (target->parentWidget() != parent())
		return;
	m_widgets << target;
	target->installEventFilter(this);
	connect(target, SIGNAL(destroyed(QObject*)), this, SLOT(removeWidget(QObject*)));

	if (m_bounding_rect.isValid())
		m_bounding_rect |= target->frameGeometry();
	else
		m_bounding_rect = target->frameGeometry();

	update();
}

QRect SelectionMoveResizer::boundingRectOf(QwtPlotMarker *target) const
{
	return ((AbstractEnrichment *)target)->rect();
}

int SelectionMoveResizer::removeAll(TextEnrichment *target)
{
	int result = m_legend_markers.removeAll(target);
	if (m_legend_markers.isEmpty() && m_line_markers.isEmpty() && m_image_markers.isEmpty() && m_widgets.isEmpty())
		delete this;
	else
		recalcBoundingRect();
	return result;
}
int SelectionMoveResizer::removeAll(LineEnrichment *target)
{
	int result = m_line_markers.removeAll(target);
	if (m_legend_markers.isEmpty() && m_line_markers.isEmpty() && m_image_markers.isEmpty() && m_widgets.isEmpty())
		delete this;
	else
		recalcBoundingRect();
	return result;
}
int SelectionMoveResizer::removeAll(ImageEnrichment *target)
{
	int result = m_image_markers.removeAll(target);
	if (m_legend_markers.isEmpty() && m_line_markers.isEmpty() && m_image_markers.isEmpty() && m_widgets.isEmpty())
		delete this;
	else
		recalcBoundingRect();
	return result;
}

int SelectionMoveResizer::removeAll(QWidget *target)
{
	int result = m_widgets.removeAll(target);
	if (m_legend_markers.isEmpty() && m_line_markers.isEmpty() && m_image_markers.isEmpty() && m_widgets.isEmpty())
		delete this;
	else
		recalcBoundingRect();
	return result;
}

void SelectionMoveResizer::recalcBoundingRect()
{
	m_bounding_rect = QRect(0,0,-1,-1);

	foreach(TextEnrichment *i, m_legend_markers) {
		if(m_bounding_rect.isValid())
			m_bounding_rect |= boundingRectOf(i);
		else
			m_bounding_rect = boundingRectOf(i);
	}
	foreach(LineEnrichment *i, m_line_markers) {
		if(m_bounding_rect.isValid())
			m_bounding_rect |= boundingRectOf(i);
		else
			m_bounding_rect = boundingRectOf(i);
	}
	foreach(ImageEnrichment *i, m_image_markers) {
		if(m_bounding_rect.isValid())
			m_bounding_rect |= boundingRectOf(i);
		else
			m_bounding_rect = boundingRectOf(i);
	}
	foreach(QWidget *i, m_widgets) {
		if(m_bounding_rect.isValid())
			m_bounding_rect |= i->frameGeometry();
		else
			m_bounding_rect = i->frameGeometry();
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
	QRect boundary_out = m_bounding_rect;
	switch (m_op) {
		case Move:
			boundary_out.translate(m_op_dp);
			break;
		case Resize_N:
			boundary_out.setTop(boundary_out.top()+m_op_dp.y());
			break;
		case Resize_E:
			boundary_out.setRight(boundary_out.right()+m_op_dp.x());
			break;
		case Resize_S:
			boundary_out.setBottom(boundary_out.bottom()+m_op_dp.y());
			break;
		case Resize_W:
			boundary_out.setLeft(boundary_out.left()+m_op_dp.x());
			break;
		case Resize_NE:
			boundary_out.setTop(boundary_out.top()+m_op_dp.y());
			boundary_out.setRight(boundary_out.right()+m_op_dp.x());
			break;
		case Resize_SE:
			boundary_out.setBottom(boundary_out.bottom()+m_op_dp.y());
			boundary_out.setRight(boundary_out.right()+m_op_dp.x());
			break;
		case Resize_SW:
			boundary_out.setBottom(boundary_out.bottom()+m_op_dp.y());
			boundary_out.setLeft(boundary_out.left()+m_op_dp.x());
			break;
		case Resize_NW:
			boundary_out.setTop(boundary_out.top()+m_op_dp.y());
			boundary_out.setLeft(boundary_out.left()+m_op_dp.x());
			break;
		default:
			return in;
	}
	boundary_out = boundary_out.normalized();
	if (in == m_bounding_rect)
		return boundary_out;
	double scale_x = ((double)boundary_out.width())/m_bounding_rect.width();
	double scale_y = ((double)boundary_out.height())/m_bounding_rect.height();
	int offset_x = qRound(boundary_out.left()-m_bounding_rect.left()*scale_x);
	int offset_y = qRound(boundary_out.top()-m_bounding_rect.top()*scale_y);
	return QRect(QPoint(qRound(in.left()*scale_x)+offset_x, qRound(in.top()*scale_y)+offset_y),
			QSize(qRound(in.width()*scale_x), qRound(in.height()*scale_y)));
}

void SelectionMoveResizer::operateOnTargets()
{
	foreach(TextEnrichment *i, m_legend_markers) {
		QRect new_rect = operateOn(i->rect());
		i->setOrigin(new_rect.topLeft());
		if (!i->text().isEmpty())
		{
            QFont f = i->font();
            f.setPointSize(f.pointSize() * new_rect.width() * new_rect.height()/(i->rect().width() * i->rect().height()));
            i->setFont(f);
		}
	}
	foreach(LineEnrichment *i, m_line_markers) {
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
	foreach(ImageEnrichment *i, m_image_markers) {
		QRect new_rect = operateOn(i->rect());
		i->setOrigin(new_rect.topLeft());
		i->setSize(new_rect.size());
	}
	foreach(QWidget *i, m_widgets)
		i->setGeometry(operateOn(i->geometry()));
	recalcBoundingRect();

	m_op_start = m_op_dp = QPoint(0,0);

	update();
	emit targetsChanged();
}

void SelectionMoveResizer::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	QRect drawn_rect = operateOn(m_bounding_rect);

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
	if (me->button() != Qt::LeftButton || !m_bounding_rect.contains(me->pos())) {
		me->ignore();
		return;
	}
	m_op_start = me->pos();
	m_op = Move;
	for (int i=0; i<8; i++)
		if (handlerRect(m_bounding_rect, (Operation)i).contains(m_op_start)) {
			m_op = (Operation)i;
			break;
		}

	me->accept();
}

void SelectionMoveResizer::mouseMoveEvent(QMouseEvent *me)
{
	if (m_op == None && m_bounding_rect.contains(me->pos()) && me->button() == Qt::LeftButton) {
		m_op = Move;
		m_op_start = me->pos();
	}
	if (m_op == None || m_op_start == QPoint(-1,-1)) {
		if (m_bounding_rect.contains(me->pos())) {
			setCursor(Qt::SizeAllCursor);
			if (handlerRect(m_bounding_rect, Resize_N).contains(me->pos()) || handlerRect(m_bounding_rect, Resize_S).contains(me->pos()))
				setCursor(Qt::SizeVerCursor);
			else if (handlerRect(m_bounding_rect, Resize_E).contains(me->pos()) || handlerRect(m_bounding_rect, Resize_W).contains(me->pos()))
				setCursor(Qt::SizeHorCursor);
			else if (handlerRect(m_bounding_rect, Resize_NE).contains(me->pos()) || handlerRect(m_bounding_rect, Resize_SW).contains(me->pos()))
				setCursor(Qt::SizeBDiagCursor);
			else if (handlerRect(m_bounding_rect, Resize_NW).contains(me->pos()) || handlerRect(m_bounding_rect, Resize_SE).contains(me->pos()))
				setCursor(Qt::SizeFDiagCursor);
		} else
			unsetCursor();
		return QWidget::mouseMoveEvent(me);
	}
	m_op_dp = me->pos() - m_op_start;
	repaint();
	me->accept();
}

void SelectionMoveResizer::mouseDoubleClickEvent(QMouseEvent *e)
{
	e->ignore();
}

void SelectionMoveResizer::mouseReleaseEvent(QMouseEvent *me)
{
	if (me->button() != Qt::LeftButton || m_op == None || m_op_start == QPoint(-1,-1))
		return QWidget::mouseReleaseEvent(me);

	m_op_dp = me->pos() - m_op_start;
	operateOnTargets();
	m_op = None;
	me->accept();
}

void SelectionMoveResizer::keyPressEvent(QKeyEvent *ke)
{
	switch(ke->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
			if (m_op == None) {
				ke->ignore();
				break;
			}
			operateOnTargets();
			m_op = None;
			ke->accept();
			break;
		case Qt::Key_Escape:
			delete this;
			ke->accept();
			return;
		case Qt::Key_Left:
			if (m_op == None) {
				m_op = Move;
				m_op_start = QPoint(-1,-1);
			}
			m_op_dp += QPoint(-1,0);
			repaint();
			ke->accept();
			break;
		case Qt::Key_Right:
			if (m_op == None) {
				m_op = Move;
				m_op_start = QPoint(-1,-1);
			}
			m_op_dp += QPoint(1,0);
			repaint();
			ke->accept();
			break;
		case Qt::Key_Up:
			if (m_op == None) {
				m_op = Move;
				m_op_start = QPoint(-1,-1);
			}
			m_op_dp += QPoint(0,-1);
			repaint();
			ke->accept();
			break;
		case Qt::Key_Down:
			if (m_op == None) {
				m_op = Move;
				m_op_start = QPoint(-1,-1);
			}
			m_op_dp += QPoint(0,1);
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
