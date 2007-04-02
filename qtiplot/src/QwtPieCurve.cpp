/***************************************************************************
    File                 : QwtPieCurve.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Pie plot class

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
#include "QwtPieCurve.h"
#include "ColorBox.h"

#include <qwt_painter.h>
#include <QPaintDevice>
#include <QPainter>
#include <QMessageBox>

QwtPieCurve::QwtPieCurve(const char *name):
    QwtPlotCurve(name)
{
d_pie_ray = 100;
d_first_color = 0;
setPen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
setBrush(QBrush(Qt::black, Qt::SolidPattern));
}

void QwtPieCurve::draw(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
    if ( !painter || dataSize() <= 0 )
        return;

    if (to < 0)
        to = dataSize() - 1;

    drawPie(painter, xMap, yMap, from, to);
}

void QwtPieCurve::drawPie(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const
{
    int x_center = xMap.transform((xMap.s1() + xMap.s2())/2);
    int y_center = yMap.transform((yMap.s1() + yMap.s2())/2);

    QwtPlot *plot = (QwtPlot *)this->plot();
    double dx = (double)painter->device()->width()/(double)plot->width();
    double dy = (double)painter->device()->height()/(double)plot->height();

    int d = int(2*d_pie_ray*QMIN(dx, dy));

	QRect pieRect;
	pieRect.setX(x_center - d/2);
	pieRect.setY(y_center - d/2);
	pieRect.setWidth(d);
	pieRect.setHeight(d);

	double sum = 0.0;
	for (int i = from; i <= to; i++)
    {
		const double yi = y(i);
		sum += yi;
    }

	int angle = (int)(5760 * 0.75);
	painter->save();
	for (int i = from; i <= to; i++)
    {
		const double yi = y(i);
		const int value = (int)(yi/sum*5760);

		painter->setPen(QwtPlotCurve::pen());
        painter->setBrush(QBrush(color(i), QwtPlotCurve::brush().style()));
		painter->drawPie(pieRect, -angle, -value);

		angle += value;
    }
	painter->restore();
}

QColor QwtPieCurve::color(int i) const
{
int index=(d_first_color+i)%16;
return ColorBox::color(index);
}

void QwtPieCurve::setBrushStyle(const Qt::BrushStyle& style)
{
QBrush br = QwtPlotCurve::brush();
if (br.style() == style)
	return;

br.setStyle(style);
setBrush(br);
}
