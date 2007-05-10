/***************************************************************************
    File                 : LineProfileTool.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006,2007 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen, Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Plot tool for calculating intensity profiles of
                           image markers.

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
#include "LineProfileTool.h"
#include "ImageMarker.h"
#include "LineMarker.h"
#include "Graph.h"

#include <QPoint>
#include <QImage>
#include <QMessageBox>
#include <QPainter>
#include <qwt_plot_canvas.h>

LineProfileTool::LineProfileTool(Graph *graph, int average_pixels)
	: QWidget(graph->plotWidget()->canvas()),
	PlotToolInterface(graph)
{
	d_op_start = d_op_dp = QPoint(0,0);
	// make sure we average over an odd number of pixels (why?)
	d_average_pixels = (average_pixels % 2) ? average_pixels : average_pixels + 1;
	d_target = dynamic_cast<ImageMarker*>(d_graph->selectedMarkerPtr());
	if (!d_target)
		QMessageBox::critical(d_graph->window(),tr("QtiPlot - Pixel selection warning"),
				"Please select an image marker first.");
	d_graph->deselectMarker();
	setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());
	show();
	setFocus();
}

void LineProfileTool::calculateLineProfile(const QPoint& start, const QPoint& end)
{
	QRect rect = d_target->rect();
	if (!rect.contains(start) || !rect.contains(end))
	{
		QMessageBox::warning(d_graph, tr("QtiPlot - Pixel selection warning"),
				"Please select the end line point inside the image rectangle!");
		return;
	}

	QPoint o = d_target->origin();
	QPixmap pic = d_target->pixmap();
	QImage image = pic.convertToImage();

	int x1 = start.x()-o.x();
	int x2 = end.x()-o.x();
	int y1 = start.y()-o.y();
	int y2 = end.y()-o.y();

	QSize realSize = pic.size();
	QSize actualSize = d_target->size();

	if (realSize != actualSize)
	{
		double ratioX = (double)realSize.width()/(double)actualSize.width();
		double ratioY = (double)realSize.height()/(double)actualSize.height();
		x1 = int(x1*ratioX);
		x2 = int(x2*ratioX);
		y1 = int(y1*ratioY);
		y2 = int(y2*ratioY);
	}

	QString text = tr("pixel") + "\tx\ty\t" + tr("intensity") + "\n";

	//uses the fast Bresenham's line-drawing algorithm
#define sgn(x) ((x<0)?-1:((x>0)?1:0))
	int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py,n;

	dx=x2-x1;      //the horizontal distance of the line
	dy=y2-y1;      //the vertical distance of the line
	dxabs=abs(dx);
	dyabs=abs(dy);
	sdx=sgn(dx);
	sdy=sgn(dy);
	x=dyabs>>1;
	y=dxabs>>1;
	px=x1;
	py=y1;

	if (dxabs>=dyabs) //the line is more horizontal than vertical
	{
		for(i=0;i<dxabs;i++)
		{
			y+=dyabs;
			if (y>=dxabs)
			{
				y-=dxabs;
				py+=sdy;
			}
			px+=sdx;

			n=dxabs;
			text+=QString::number(i)+"\t";
			text+=QString::number(px)+"\t";
			text+=QString::number(py)+"\t";
			text+=QString::number(averageImagePixel(image, px, py, true))+"\n";
		}
	}
	else // the line is more vertical than horizontal
	{
		for(i=0;i<dyabs;i++)
		{
			x+=dxabs;
			if (x>=dyabs)
			{
				x-=dyabs;
				px+=sdx;
			}
			py+=sdy;

			n=dyabs;
			text+=QString::number(i)+"\t";
			text+=QString::number(px)+"\t";
			text+=QString::number(py)+"\t";
			text+=QString::number(averageImagePixel(image, px, py, false))+"\n";
		}
	}
	QString caption = tr("Table") + "1";
	emit createTablePlot(caption, n, 4, text);
}

int LineProfileTool::averageImagePixel(const QImage& image, int px, int py, bool moreHorizontal)
{
	QRgb pixel;
	int sum=0,start,i;
	int middle=int(0.5*(d_average_pixels-1));
	if (moreHorizontal)
	{
		start=py-middle;
		for(i=0; i<d_average_pixels; i++)
		{
			pixel= image.pixel(px,start+i);
			sum+=qGray(pixel);
		}
	}
	else
	{
		start=px-middle;
		for(i=0; i<d_average_pixels; i++)
		{
			pixel= image.pixel(start+i,py);
			sum+=qGray(pixel);
		}
	}
	return sum/d_average_pixels;
}

void LineProfileTool::addLineMarker(const QPoint &start, const QPoint &end)
{
	LineMarker *mrk = new LineMarker();
	mrk->attach(d_graph->plotWidget());

	mrk->setStartPoint(start);
	mrk->setEndPoint(end);
	mrk->setColor(Qt::red);
	mrk->setWidth(1);
	mrk->setStyle(Qt::SolidLine);
	mrk->drawEndArrow(false);
	mrk->drawStartArrow(false);

	d_graph->insertLineMarker(mrk);
	mrk->detach();
	d_graph->replot();
}

void LineProfileTool::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
	p.drawLine(d_op_start, d_op_start+d_op_dp);
}

void LineProfileTool::mousePressEvent(QMouseEvent *e)
{
	if (e->button() != Qt::LeftButton)
		return;
	d_op_start = e->pos();
	e->accept();
}

void LineProfileTool::mouseMoveEvent(QMouseEvent *e)
{
	d_op_dp = e->pos() - d_op_start;
	repaint();
	e->accept();
}

void LineProfileTool::mouseReleaseEvent(QMouseEvent *e)
{
	calculateLineProfile(d_op_start, e->pos());
	addLineMarker(d_op_start, e->pos());
	d_graph->setActiveTool(NULL);
	// attention: I'm now deleted
}
