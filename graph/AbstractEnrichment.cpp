/***************************************************************************
    File                 : AbstractEnrichment.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Abstract enrichement class for 2D Plots.

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

#include "AbstractEnrichment.h"

AbstractEnrichment::AbstractEnrichment():
	m_x_right(0),
    m_y_bottom(0),
    m_pos(QPoint()),
    m_size(QSize())
{
}

void AbstractEnrichment::setOrigin(const QPoint& p)
{
    m_pos = p;

    if (!plot())
        return;

    setXValue(plot()->invTransform(xAxis(), p.x()));
    setYValue(plot()->invTransform(yAxis(), p.y()));

    m_size = calculatePaintingRect().size();
    updateBoundingRect();
}

void AbstractEnrichment::setBoundingRect(double left, double top, double right, double bottom)
{
    if (xValue() == left && yValue() == top && m_x_right == right && m_y_bottom == bottom)
        return;

    setXValue(left);
    setYValue(top);
    m_x_right = right;
    m_y_bottom = bottom;

    QRect r = calculatePaintingRect();
    m_pos = r.topLeft();
    m_size = r.size();
}

void AbstractEnrichment::updateBoundingRect()
{
    if (!plot())
        return;

    setXValue(plot()->invTransform(xAxis(), m_pos.x()));
    m_x_right = plot()->invTransform(xAxis(), m_pos.x() + m_size.width());

    setYValue(plot()->invTransform(yAxis(), m_pos.y()));
    m_y_bottom = plot()->invTransform(yAxis(), m_pos.y() + m_size.height());
}

QwtDoubleRect AbstractEnrichment::boundingRect() const
{
    return QwtDoubleRect(xValue(), yValue(), qAbs(m_x_right - xValue()), qAbs(m_y_bottom - yValue()));
}

void AbstractEnrichment::setRect(int x, int y, int w, int h)
{
    if (m_pos == QPoint(x, y) && m_size == QSize(w, h))
        return;

    m_pos = QPoint(x, y);
    m_size = QSize(w, h);
    updateBoundingRect();
}

QRect AbstractEnrichment::calculatePaintingRect()
{
    if (!plot())
        return QRect();

    plot()->updateLayout();

    const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
    const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

    const int x0 = xMap.transform(xValue());
    const int y0 = yMap.transform(yValue());
    const int x1 = xMap.transform(m_x_right);
    const int y1 = yMap.transform(m_y_bottom);

    return QRect(x0, y0, abs(x1 - x0), abs(y1 - y0));
}
