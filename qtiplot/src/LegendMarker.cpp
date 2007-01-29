/***************************************************************************
    File                 : LegendMarker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Legend marker (extension to QwtPlotMarker)
                           
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
#include "LegendMarker.h"
#include "pie.h"
#include "VectorCurve.h"

#include <QPainter>
#include <QMessageBox>
#include <QPolygon>

#include <qwt_plot.h>
#include <qwt_scale_widget.h>
#include <qwt_painter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_layout_metrics.h>

LegendMarker::LegendMarker(Plot *plot):
    d_plot(plot),
	d_frame (0),
	angle(0),
	bkgColor(plot->paletteBackgroundColor())
{
	d_text = new QwtText(QString::null, QwtText::RichText);
		d_text->setFont(QFont("Arial",12, QFont::Normal, FALSE));
		d_text->setRenderFlags(Qt::AlignTop|Qt::AlignLeft);
		d_text->setBackgroundBrush(QBrush(Qt::NoBrush));
		d_text->setColor(Qt::black);
		d_text->setBackgroundPen (QPen(Qt::NoPen));

		hspace = 30;
	left_margin = 10;
	top_margin = 5;
}

void LegendMarker::draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const
{ 
	const int x = xMap.transform(xValue());
	const int y = yMap.transform(yValue());

	const int symbolLineLength = symbolsMaxLineLength();

	int width, height;
	QwtArray<long> heights = itemsHeight(y, symbolLineLength, width, height);

	QRect rs = QRect(QPoint(x, y), QSize(width, height));

	drawFrame(p, d_frame, rs);	
	drawSymbols(p, rs, heights, symbolLineLength);
	drawLegends(p, rs, heights, symbolLineLength);
}

QString LegendMarker::getText()
{
	return d_text->text();
}

void LegendMarker::setText(const QString& s)
{
	d_text->setText(s);
}

int LegendMarker::getBkgType()
{
	return d_frame; 
}

void LegendMarker::setBackground(int bkg)
{
	if (d_frame == bkg)
		return;

	d_frame = bkg;
}

void LegendMarker::setBackgroundColor(const QColor& c)
{
	if ( c == bkgColor )
		return;

	bkgColor = c;
}

QRect LegendMarker::rect()
{
	const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
	const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

	const int x = xMap.transform(xValue());
	const int y = yMap.transform(yValue());

	int width, height;
	itemsHeight(y, symbolsMaxLineLength(), width, height);

	return QRect(QPoint(x, y), QSize(width, height));
}

QColor LegendMarker::getTextColor()
{
	return d_text->color();
}

void LegendMarker::setTextColor(const QColor& c)
{
	if ( c == d_text->color() )
		return;

	d_text->setColor(c);
}

void LegendMarker::setOrigin( const QPoint & p )
{
	d_pos = p;

	const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
	const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());

	setXValue (xMap.invTransform(p.x()));
	setYValue (yMap.invTransform(p.y()));
}

void LegendMarker::updateOrigin()
{
	if (!d_plot)
		return;
			
			const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
			const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());
			
			setXValue (xMap.invTransform(d_pos.x()));
	setYValue (yMap.invTransform(d_pos.y()));
}

void LegendMarker::setOriginCoord(double x, double y)
{
	if (xValue() == x && yValue() == y)
		return;
			
			setXValue(x);
			setYValue(y);
			
			const QwtScaleMap &xMap = d_plot->canvasMap(xAxis());
			const QwtScaleMap &yMap = d_plot->canvasMap(yAxis());
			
			d_pos = QPoint(xMap.transform(x), yMap.transform(y));
}

QFont LegendMarker::getFont()
{
	return d_text->font();
}

void LegendMarker::setFont(const QFont& font)
{
	if ( font == d_text->font() )
		return;

	d_text->setFont(font);	 
}

void LegendMarker::drawFrame(QPainter *p, int type, const QRect& rect) const
{
	p->save();
	p->setPen(QPen(Qt::black,1,Qt::SolidLine));
	if (type == None && bkgColor != d_plot->paletteBackgroundColor())
		p->fillRect (rect,QBrush(bkgColor));
			
			if (type == Line)
			{
				p->setBrush(QBrush(bkgColor));
					p->drawRect(rect);
			}
			else if (type == Shadow)
			{
				QRect shadow=QRect(rect.x()+5,rect.y()+5,rect.width(),rect.height());
					p->setBrush(QBrush(Qt::black));
				p->drawRect(shadow);
				p->setBrush(QBrush(bkgColor));
				p->drawRect(rect);
			}
	p->restore();
}

void LegendMarker::drawVector(QPainter *p, int x, int y, int l, int curveIndex) const
{
	Graph *g = (Graph *)d_plot->parent();
	if (!g)
		return;

	VectorCurve *v = (VectorCurve*)g->curve(curveIndex);
	if (!v)
		return;
	
		p->save();
		
		QPen pen(v->color(), v->width(), Qt::SolidLine);
		p->setPen(pen);
		QwtPainter::drawLine(p, x, y, x + l, y);
		
		p->translate(x+l, y);

	double pi=4*atan(-1.0);
	int headLength = v->headLength();	
		int d=qRound(headLength*tan(pi*(double)v->headAngle()/180.0));	
		
		QPolygon endArray(3);	
		endArray[0] = QPoint(0, 0);
		endArray[1] = QPoint(-headLength, d);
		endArray[2] = QPoint(-headLength, -d);
		
		if (v->filledArrowHead())
			p->setBrush(QBrush(pen.color(), Qt::SolidPattern));
				
				QwtPainter::drawPolygon(p,endArray);
				p->restore();
}

void LegendMarker::drawSymbols(QPainter *p, const QRect& rect, 
		QwtArray<long> height, int symbolLineLength) const
{ 	
	Graph *g = (Graph *) d_plot->parent();

	int w = rect.x() + 10;
	int l = symbolLineLength + 20;

	QString text=d_text->text();	
	QStringList titles=QStringList::split ("\n",text,FALSE);

	for (int i=0;i<(int)titles.count();i++)
	{
		if (titles[i].contains("\\c{"))
		{
			int pos=titles[i].find("{",0);
				int pos2=titles[i].find("}",pos);
				QString aux=titles[i].mid(pos+1,pos2-pos-1);
				int cv = aux.toInt() - 1;	
				if (cv < 0)
					continue;
						
						if (g->curveType(cv) == Graph :: VectXYXY || g->curveType(cv) == Graph :: VectXYAM)
							drawVector(p, w, height[i], l, cv);
						else
						{
							const QwtPlotCurve *curve = g->curve(cv);
								if (curve)
								{
									const QwtSymbol symb=curve->symbol(); 
										const QBrush br=curve->brush();
										QPen pen=curve->pen();
										
										p->save();
										
										if (curve->style()!=0)
										{	
											p->setPen (pen);					
												if (br.style() != Qt::NoBrush || g->curveType(cv) == Graph::Box)
												{
													QRect lr=QRect(w,height[i]-4,l,10);						
														p->setBrush(br);
														p->drawRect (lr);
												}			
												else 			
													p->drawLine (w,height[i],w+l,height[i]);						
										}
									symb.draw(p,w+l/2,height[i]);
										p->restore();
								}
						}
		}	
		else if (titles[i].contains("\\p{"))
		{
			int pos=titles[i].find("{",0);
			int pos2=titles[i].find("}",pos);
			QString aux=titles[i].mid(pos+1,pos2-pos-1);

			int id=aux.toInt();

			Graph* g=(Graph*)d_plot->parent();
			if (g->isPiePlot())
			{			
				QwtPieCurve *curve = (QwtPieCurve *)d_plot->curve(1);
				if (curve)
				{
					const QBrush br=QBrush(curve->color(id-1), curve->pattern());
					QPen pen=curve->pen();	

					p->save();						
					p->setPen (QPen(pen.color(),1,Qt::SolidLine));					
					QRect lr=QRect(w,height[i]-4,l,10);						
					p->setBrush(br);
					p->drawRect(lr);
					p->restore();
				}
			}
		}
	}	 
}

void LegendMarker::drawLegends(QPainter *p, const QRect& rect, 
		QwtArray<long> height, int symbolLineLength) const
{ 	
	int w = rect.x() + left_margin;

	QString text=d_text->text();
	QStringList titles=QStringList::split ("\n",text,FALSE);

	for (int i=0;i<(int)titles.count();i++)
	{
		QString str=titles[i];
		int x = w;
		if (str.contains("\\c{",TRUE)>0 || str.contains("\\p{",TRUE)>0)
		{
			x += symbolLineLength + hspace;
			int pos=str.find("}",0);
			str=str.right(str.length()-pos-1);
		}

		QwtText aux(str);
		aux.setFont(d_text->font());
		aux.setColor(d_text->color());

		QSize size = aux.textSize();

		QRect tr = QRect(QPoint(x, height[i] - size.height()/2), size);
		aux.draw(p, tr);
	}	
}

QwtArray<long> LegendMarker::itemsHeight(int y, int symbolLineLength, int &width, int &height) const
{ 
	int maxL=0;

	width = 0;
	height = 0;

	QString text=d_text->text();
	QStringList titles=QStringList::split ("\n",text,FALSE);
	int n=(int)titles.count();
	QwtArray<long> heights(n);

	int h = top_margin;

	for (int i=0; i<n; i++)
	{
		QString str=titles[i];		 
		int textL=0;	 
		if (str.contains("\\c{",TRUE)>0 || str.contains("\\p{",TRUE)>0)
		{
			textL = symbolLineLength + hspace; 
			int pos=str.find("}",0);
			str = str.right(str.length()-pos-1);
		}	

		QwtText aux(str);
		QSize size = aux.textSize(d_text->font());
		textL += size.width();
		if (textL>maxL) 
			maxL = textL;

		int textH = size.height();
		height += textH;

		heights[i] = y + h + textH/2;
		h += textH;
	}	

	height += 2*top_margin;
	width = 2*left_margin + maxL;

	return heights;
}

int LegendMarker::symbolsMaxLineLength() const
{
	QList<int> cvs = d_plot->curveKeys();
		
		int maxL=0;
		QString text=d_text->text();	
		QStringList titles=QStringList::split ("\n",text,FALSE);	
	for (int i=0;i<(int)titles.count();i++)
	{
		if (titles[i].contains("\\c{") && (int)cvs.size()>0)
		{
			int pos=titles[i].find("{",0);
				int pos2=titles[i].find("}",pos);
				QString aux=titles[i].mid(pos+1,pos2-pos-1);
				int cv = aux.toInt()-1;
				if (cv < 0)
					continue;
						
						const QwtPlotCurve *c = d_plot->curve(cvs[cv]);
						if (c)
						{
							int l=c->symbol().size().width();
								if (l>maxL && c->symbol().style() != QwtSymbol::NoSymbol)
									maxL=l;
						}
		}
		
			if (titles[i].contains("\\p{"))
				maxL=10;
	}
	return maxL;
}

LegendMarker::~LegendMarker()
{
	delete d_text;
}
