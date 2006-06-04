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
#include "LineMarker.h"
#include "graph.h"
#include "pie.h"
#include "VectorCurve.h"

#include <qpainter.h>
#include <q3paintdevicemetrics.h>

#include <qwt_plot.h>
#include <qwt_scale_widget.h>
#include <qwt_painter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_layout_metrics.h>
//Added by qt3to4:
#include <Q3PointArray>

//FIXME: All functionality disabled for now (needs port to Qwt5)

LegendMarker::LegendMarker(QwtPlot *plot):
	QwtPlotMarker()
{
#if false
	d_text= QwtText::makeText(QString(),Qt::RichText,Qt::AlignTop|Qt::AlignLeft, 
			QFont("Arial",12, QFont::Normal, false),Qt::black,QPen(Qt::NoPen),QBrush(Qt::NoBrush));

	bkgType=0;
	angle=0;
	bkgColor = plot->paletteBackgroundColor();
#endif
}

void LegendMarker::draw(QPainter *p, int, int, const QRect &rect)
{ 
#if false
	symbolLineLength=symbolsMaxLineLength();

	int clw = parentPlot()->canvas()->lineWidth();
	QRect rs=scaledLegendRect(p, QPoint(rect.x() - clw, rect.y() - clw), lRect);

	lRect.setWidth(rs.width());
	lRect.setHeight(rs.height());

	drawFrame(p,bkgType,rs);	
	drawSymbols(p,rs,heights);
	drawLegends(p,rs,heights);
#endif
}

QString LegendMarker::getText()
{
#if false
	return d_text->text();
#endif
}

void LegendMarker::setText(const QString& s)
{
#if false
	d_text->setText(s);
#endif
}

int LegendMarker::getBkgType()
{
#if false
	return bkgType; 
#endif
}

void LegendMarker::setBackground(int bkg)
{
#if false
	if (bkgType == bkg)
		return;
	bkgType=bkg;
#endif
}

void LegendMarker::setBackgroundColor(const QColor& c)
{
#if false
	if ( c == bkgColor )
		return;

	bkgColor = c;
#endif
}

QRect LegendMarker::rect()
{
#if false
	return lRect;
#endif
}

QColor LegendMarker::getTextColor()
{
#if false
	return d_text->color();
#endif
}

void LegendMarker::setTextColor(const QColor& c)
{
#if false
	if ( c == d_text->color() )
		return;

	d_text->setColor(c);
#endif
}

void LegendMarker::setOrigin( const QPoint & p )
{
#if false
	lRect.setTopLeft (p);
#endif
}

QFont LegendMarker::getFont()
{
#if false
	return d_text->font();
#endif
}

void LegendMarker::setFont(const QFont& font)
{
#if false
	if ( font == d_text->font() )
		return;

	d_text->setFont(font);	 
#endif
}

int LegendMarker::getAngle()
{
#if false
	return angle; 
#endif
}

void LegendMarker::setAngle(int ang)
{
#if false
	angle=ang;
#endif
}

void LegendMarker::drawFrame(QPainter *p, int type, const QRect& rect)
{
#if false
	p->save();
	p->setPen(QPen(Qt::black,1,Qt::SolidLine));
	if (type == None && bkgColor != parentPlot()->paletteBackgroundColor())
		p->fillRect (rect,QBrush(bkgColor));
	if (type == Line)
	{
		p->fillRect (rect,QBrush(bkgColor));
		p->drawRect(rect);
	}
	else if (type== Shadow)
	{
		QRect shadow=QRect(rect.x()+5,rect.y()+5,rect.width(),rect.height());
		p->fillRect (shadow,QBrush(QColor(Qt::black)));
		p->drawRect(shadow);
		p->fillRect (rect, QBrush(bkgColor));
		p->drawRect(rect);
	}
	p->restore();
#endif
}

void LegendMarker::drawVector(QPainter *p, int x, int y, int l, int curveIndex)
{
#if false
	Graph *g = parentGraph(parentPlot());
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

	const Q3PointArray endArray(3);	
	endArray[0] = QPoint(0, 0);
	endArray[1] = QPoint(-headLength, d);
	endArray[2] = QPoint(-headLength, -d);

	if (v->filledArrowHead())
		p->setBrush(QBrush(pen.color(), Qt::SolidPattern));

	QwtPainter::drawPolygon(p,endArray);
	p->restore();
#endif
}

void LegendMarker::drawSymbols(QPainter *p, const QRect& rect, QwtArray<long> height)
{ 	
#if false
	QwtPlot *plot = (QwtPlot *)parentPlot();
	Graph *g=parentGraph(plot);

	int w=rect.x()+10;
	int l=symbolLineLength+20;

	QString text=d_text->text();	
	QStringList titles=QStringList::split ("\n",text,false);

	for (int i=0;i<(int)titles.count();i++)
	{
		if (titles[i].contains("\\c{"))
		{
			int pos=titles[i].find("{",0);
			int pos2=titles[i].find("}",pos);
			QString aux=titles[i].mid(pos+1,pos2-pos-1);
			int cv=aux.toInt() - 1;		
			if (cv >= 0)
			{
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
								p->fillRect(lr,br);
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
		}	
		else if (titles[i].contains("\\p{"))
		{
			int pos=titles[i].find("{",0);
			int pos2=titles[i].find("}",pos);
			QString aux=titles[i].mid(pos+1,pos2-pos-1);

			int id=aux.toInt();

			Graph* g=(Graph*)plot->parent();
			if (g->isPiePlot())
			{			
				QwtPieCurve *curve = (QwtPieCurve *)plot->curve(1);
				if (curve)
				{
					const QBrush br=QBrush(curve->color(id-1), curve->pattern());
					QPen pen=curve->pen();	

					p->save();						
					p->setPen (QPen(pen.color(),1,Qt::SolidLine));					
					QRect lr=QRect(w,height[i]-4,l,10);						
					p->fillRect(lr,br);
					p->drawRect (lr);
					p->restore();
				}
			}
		}
	}	 
#endif
}

void LegendMarker::drawLegends(QPainter *p, const QRect& rect, QwtArray<long> height)
{ 	
#if false
	int hspace=30;//determines distance between symbols and legend text
	int w=rect.x()+10;
	int textL=w,textH=0;
	QString text=d_text->text();
	QStringList titles=QStringList::split ("\n",text,false);
	QwtText *text_copy= d_text->clone();

	for (int i=0;i<(int)titles.count();i++)
	{
		QString str=titles[i];
		textL=w;
		if (str.contains("\\c{",true)>0)
		{
			textL+=symbolLineLength+hspace;
			int pos=str.find("}",0);
			str=str.right(str.length()-pos-1);
		}

		if (str.contains("\\p{",true)>0)
		{
			textL+=symbolLineLength+hspace;
			int pos=str.find("}",0);
			str=str.right(str.length()-pos-1);
		}

		text_copy->setText(str);
		QRect tr=text_copy->boundingRect(p);
		textH=tr.height();	
		tr.moveTopLeft(QPoint(textL,height[i]-textH/2));
		text_copy->draw(p,tr);
	}		

	delete text_copy;
#endif
}

QRect LegendMarker::scaledLegendRect(QPainter *p, const QPoint& canvas_origin, const QRect& rect)
{ 
#if false
	int textH=0,maxL=0,rectH=0,rectL=0;
	int textL,margin=10;	
	int hspace=30;//determines distance between symbols and legend text

	QRect copy= rect;	
	copy.moveBy(canvas_origin.x(), canvas_origin.y());

	if (p->device()->isExtDev())
	{	
		QwtPlot *plot = (QwtPlot *)parentPlot();		
		QwtScaleMap xMap = plot->canvasMap(QwtPlot::xBottom);
		QwtScaleMap yMap = plot->canvasMap(QwtPlot::yLeft);		

		double dx=xMap.invTransform(lRect.topLeft().x());
		double dy=yMap.invTransform(lRect.topLeft().y());			

		QwtScaleMap map=LineMarker::mapCanvasToDevice(p, plot, QwtPlot::xBottom);		
		copy.setX(map.transform(dx));
		map=LineMarker::mapCanvasToDevice(p, plot, QwtPlot::yLeft);	
		copy.setY(map.transform(dy));
	}

	QwtText *text_copy= d_text->clone();
	QString text=d_text->text();
	QStringList titles=QStringList::split ("\n",text,false);
	int n=(int)titles.count();
	heights.resize(n);
	int h=copy.y()+5;	
	for (int i=0;i<n;i++)
	{
		QString str=titles[i];		 
		textL=0;	 
		if (str.contains("\\c{",true)>0)
		{
			textL=symbolLineLength+hspace; 
			int pos=str.find("}",0);
			str=str.right(str.length()-pos-1);
		}

		if (str.contains("\\p{",true)>0)
		{
			textL=symbolLineLength+hspace; 
			int pos=str.find("}",0);
			str=str.right(str.length()-pos-1);
		}

		text_copy->setText(str);
		QRect tr=text_copy->boundingRect(p);
		textL+=tr.width();
		if (textL>maxL) maxL=textL;

		textH=tr.height();
		rectH+=textH;

		heights[i]=h+textH/2;
		h+=textH;
	}	

	rectH+=margin;
	rectL=margin+maxL+5;

	delete text_copy;

	copy.setWidth(rectL);
	copy.setHeight(rectH);
	return copy;
#endif
}

int LegendMarker::symbolsMaxLineLength()
{
#if false
	QwtPlot *plot = (QwtPlot *)parentPlot();		
	QwtArray<long>  cvs=plot->curveKeys();

	int maxL=0;
	QString text=d_text->text();	
	QStringList titles=QStringList::split ("\n",text,false);	
	for (int i=0;i<(int)titles.count();i++)
	{
		if (titles[i].contains("\\c{") && (int)cvs.size()>0)
		{
			int pos=titles[i].find("{",0);
			int pos2=titles[i].find("}",pos);
			QString aux=titles[i].mid(pos+1,pos2-pos-1);
			const QwtPlotCurve *c = plot->curve(cvs[aux.toInt()-1]);
			int l=c->symbol().size().width();
			if (l>maxL && c->symbol().style() != QwtSymbol::None) 
				maxL=l;		
		}

		if (titles[i].contains("\\p{"))
			maxL=10;
	}
	return maxL;
#endif
}

Graph * LegendMarker::parentGraph(QwtPlot *plot) 
{ 
#if false
	return (Graph *) plot->parent();
#endif
}

LegendMarker::~LegendMarker()
{
#if false
	delete d_text;
#endif
}
