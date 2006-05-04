#include "LegendMarker.h"
#include "LineMarker.h"
#include "graph.h"
#include "pie.h"
#include "VectorCurve.h"

#include <qpainter.h>
#include <qpaintdevicemetrics.h>

#include <qwt_plot.h>
#include <qwt_scale.h>
#include <qwt_painter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_layout_metrics.h>

LegendMarker::LegendMarker(QwtPlot *plot):
    QwtPlotMarker(plot)
{
d_text= QwtText::makeText(QString::null,Qt::RichText,Qt::AlignTop|Qt::AlignLeft, 
		QFont("Arial",12, QFont::Normal, FALSE),Qt::black,QPen(Qt::NoPen),QBrush(Qt::NoBrush));
	
bkgType=1;
angle=0;
bkgColor = plot->paletteBackgroundColor();
}

void LegendMarker::draw(QPainter *p, int, int, const QRect &rect)
{ 
symbolLineLength=symbolsMaxLineLength();
	
int clw = parentPlot()->canvas()->lineWidth();
QRect rs=scaledLegendRect(p, QPoint(rect.x() - clw, rect.y() - clw), lRect);

lRect.setWidth(rs.width());
lRect.setHeight(rs.height());

drawFrame(p,bkgType,rs);	
drawSymbols(p,rs,heights);
drawLegends(p,rs,heights);
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
return bkgType; 
}

void LegendMarker::setBackground(int bkg)
{
if (bkgType == bkg)
	return;
bkgType=bkg;
}

void LegendMarker::setBackgroundColor(const QColor& c)
{
if ( c == bkgColor )
    return;
	
bkgColor = c;
}

QRect LegendMarker::rect()
{
return lRect;
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
lRect.setTopLeft (p);
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

int LegendMarker::getAngle()
{
return angle; 
}

void LegendMarker::setAngle(int ang)
{
angle=ang;
}

void LegendMarker::drawFrame(QPainter *p, int type, const QRect& rect)
{
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
}

void LegendMarker::drawVector(QPainter *p, int x, int y, int l, int curveIndex)
{
Graph *g = parentGraph(parentPlot());
if (!g)
	return;

VectorCurve *v = (VectorCurve*)g->curve(curveIndex);
if (!v)
	return;

p->save();

QPen pen(v->color(), v->width(), QPen::SolidLine);
p->setPen(pen);
QwtPainter::drawLine(p, x, y, x + l, y);

p->translate(x+l, y);
	
double pi=4*atan(-1.0);
int headLength = v->headLength();	
int d=qRound(headLength*tan(pi*(double)v->headAngle()/180.0));	
	
const QPointArray endArray(3);	
endArray[0] = QPoint(0, 0);
endArray[1] = QPoint(-headLength, d);
endArray[2] = QPoint(-headLength, -d);

if (v->filledArrowHead())
	p->setBrush(QBrush(pen.color(), QBrush::SolidPattern));

QwtPainter::drawPolygon(p,endArray);
p->restore();
}

void LegendMarker::drawSymbols(QPainter *p, const QRect& rect, QwtArray<long> height)
{ 	
QwtPlot *plot = (QwtPlot *)parentPlot();
Graph *g=parentGraph(plot);
	
int w=rect.x()+10;
int l=symbolLineLength+20;
	
QString text=d_text->text();	
QStringList titles=QStringList::split ("\n",text,FALSE);
	
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
}

void LegendMarker::drawLegends(QPainter *p, const QRect& rect, QwtArray<long> height)
{ 	
int hspace=30;//determines distance between symbols and legend text
int w=rect.x()+10;
int textL=w,textH=0;
QString text=d_text->text();
QStringList titles=QStringList::split ("\n",text,FALSE);
QwtText *text_copy= d_text->clone();
		
for (int i=0;i<(int)titles.count();i++)
	{
	QString str=titles[i];
	textL=w;
	if (str.contains("\\c{",TRUE)>0)
		{
		textL+=symbolLineLength+hspace;
		int pos=str.find("}",0);
		str=str.right(str.length()-pos-1);
		}
		
	if (str.contains("\\p{",TRUE)>0)
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
}

QRect LegendMarker::scaledLegendRect(QPainter *p, const QPoint& canvas_origin, const QRect& rect)
{ 
int textH=0,maxL=0,rectH=0,rectL=0;
int textL,margin=10;	
int hspace=30;//determines distance between symbols and legend text

QRect copy= rect;	
copy.moveBy(canvas_origin.x(), canvas_origin.y());

if (p->device()->isExtDev())
	{	
	QwtPlot *plot = (QwtPlot *)parentPlot();		
	QwtDiMap xMap = plot->canvasMap(QwtPlot::xBottom);
	QwtDiMap yMap = plot->canvasMap(QwtPlot::yLeft);		
		
	double dx=xMap.invTransform(lRect.topLeft().x());
	double dy=yMap.invTransform(lRect.topLeft().y());			
	
	QwtDiMap map=LineMarker::mapCanvasToDevice(p, plot, QwtPlot::xBottom);		
	copy.setX(map.transform(dx));
	map=LineMarker::mapCanvasToDevice(p, plot, QwtPlot::yLeft);	
	copy.setY(map.transform(dy));
	}

QwtText *text_copy= d_text->clone();
QString text=d_text->text();
QStringList titles=QStringList::split ("\n",text,FALSE);
int n=(int)titles.count();
heights.resize(n);
int h=copy.y()+5;	
for (int i=0;i<n;i++)
	 {
	 QString str=titles[i];		 
	 textL=0;	 
	 if (str.contains("\\c{",TRUE)>0)
	 	{
		textL=symbolLineLength+hspace; 
		int pos=str.find("}",0);
		str=str.right(str.length()-pos-1);
		}
		
	if (str.contains("\\p{",TRUE)>0)
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
}

int LegendMarker::symbolsMaxLineLength()
{
QwtPlot *plot = (QwtPlot *)parentPlot();		
QwtArray<long>  cvs=plot->curveKeys();
	
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
		const QwtPlotCurve *c = plot->curve(cvs[aux.toInt()-1]);
		int l=c->symbol().size().width();
		if (l>maxL && c->symbol().style() != QwtSymbol::None) 
			maxL=l;		
		}
		
	if (titles[i].contains("\\p{"))
		maxL=10;
	}
return maxL;
}

Graph * LegendMarker::parentGraph(QwtPlot *plot) 
{ 
return (Graph *) plot->parent();
}

LegendMarker::~LegendMarker()
{
    delete d_text;
}
