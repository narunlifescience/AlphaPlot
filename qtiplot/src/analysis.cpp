/***************************************************************************
    File                 : analysis.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Image analysis methods for class 'Graph'
                           
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
#include "graph.h"
#include "plot.h"
#include "ImageMarker.h"

#include <QImage>
#include <QMessageBox>

void Graph::calculateLineProfile(const QPoint& start, const QPoint& end)
{	
	ImageMarker* mrk=(ImageMarker*) d_plot->marker(selectedMarker);
	if (!mrk)
	{
		QMessageBox::warning(0,tr("QtiPlot - Pixel selection warning"),  
				"Please select the start line point inside the image rectangle!");

		int linesOnPlot = (int)d_lines.size();
        d_plot->removeMarker(d_lines[--linesOnPlot]);
		d_lines.resize(linesOnPlot);
		return;
	}

	QRect rect=mrk->rect();
	if (!rect.contains(start) || !rect.contains(end))
	{
		QMessageBox::warning(0,tr("QtiPlot - Pixel selection warning"),  
				"Please select the end line point inside the image rectangle!");
		
        int linesOnPlot = (int)d_lines.size();
	    d_plot->removeMarker(d_lines[--linesOnPlot]);
		d_lines.resize(linesOnPlot);		
		return;		
	}

	QPoint o=mrk->origin();
	QPixmap pic=mrk->image();
	QImage image=pic.convertToImage();
	lineProfileOn=FALSE;

	int x1=start.x()-o.x();
	int x2=end.x()-o.x();
	int y1=start.y()-o.y();
	int y2=end.y()-o.y();

	QSize realSize=pic.size();
	QSize actualSize=mrk->size();

	if (realSize != actualSize)
	{
		double ratioX= (double)realSize.width()/(double)actualSize.width();
		double ratioY= (double)realSize.height()/(double)actualSize.height();
		x1=int(x1*ratioX); 
		x2=int(x2*ratioX);
		y1=int(y1*ratioY); 
		y2=int(y2*ratioY); 	
	}

	QString text="pixel\tx\ty\tintensity\n";
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
			text+=QString::number(averageImagePixel(image, px, py, averagePixels, TRUE))+"\n"; 
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
			text+=QString::number(averageImagePixel(image, px, py, averagePixels, FALSE))+"\n"; 
		}
	}
	QString caption="table1";
	emit createTablePlot(caption,n,4, text);
}

int Graph::averageImagePixel(const QImage& image, int px, int py, int average, bool moreHorizontal)
{
	QRgb pixel;
	int sum=0,start,i;
	int middle=int(0.5*(average-1));
	if (moreHorizontal)
	{
		start=py-middle;
		for(i=0;i<average;i++)
		{
			pixel= image.pixel(px,start+i);	
			sum+=qGray(pixel);
		}
	}
	else
	{
		start=px-middle;
		for(i=0;i<average;i++)
		{
			pixel= image.pixel(start+i,py);	
			sum+=qGray(pixel);
		}
	}
	return sum/average;
}

void Graph::calculateProfile(int average, bool ok)
{
	lineProfileOn=ok;	
	if (average % 2 == 0)
		averagePixels=average+1;
	else
		averagePixels=average;
}

