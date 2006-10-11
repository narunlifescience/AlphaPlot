/***************************************************************************
    File                 : canvaspicker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#include "canvaspicker.h"
#include "ImageMarker.h"
#include "LegendMarker.h"
#include "LineMarker.h"

#include <qpainter.h>
#include <qpixmapcache.h> 
#include <qcursor.h> 
#include <qapplication.h> 
#include <qmessagebox.h> 

#include <qwt_text_label.h>
#include <qwt_plot_canvas.h>

#include <Q3MemArray>

CanvasPicker::CanvasPicker(Graph *plot):
    QObject(plot)
{
	moved = FALSE;
	movedGraph = FALSE;
	mousePressed = false;
	pointSelected = false;
	resizeLineFromStart = false;
	resizeLineFromEnd = false;

	plotWidget=plot->plotWidget();

    QwtPlotCanvas *canvas = plotWidget->canvas();
    canvas->installEventFilter(this);
}

bool CanvasPicker::eventFilter(QObject *object, QEvent *e)
{
    Q3MemArray<long> images=plot()->imageMarkerKeys();	
	QList<int> texts=plot()->textMarkerKeys();
	Q3MemArray<long> lines=plot()->lineMarkerKeys();	
	
	if (object != (QObject *)plot()->plotWidget()->canvas())
        return FALSE;

	bool moveRangeSelector=plot()->selectorsEnabled();
	bool pickerActivated=plot()->pickerActivated();
	bool dataCursorEnabled=plot()->enabledCursor();
	bool removePoint=plot()->removePointActivated();
	bool movePoint=plot()->movePointsActivated();
	
    switch(e->type())
    	{
        case QEvent::Enter:
			{
			mousePressed = false;
			return false;
			}
		break;

		case QEvent::Leave:
			{
			mousePressed = false;
			return false;
			}
		break;		
		case QEvent::FocusIn:
			{
			if (plot()->enabledCursor()) 
				plot()->showCursor(TRUE);
			return TRUE;
			}
		break;
			
		case QEvent::FocusOut:
			{
			if (plot()->enabledCursor()) 
				plot()->showCursor(TRUE);
				{
				plotWidget->titleLabel()->repaint();
				plotWidget->replot();
				return TRUE;
				}
			}
		break;
			
		case QEvent::Paint:
        {   
            // The inPaint guard protecs from producing endless paint events.
            static bool inPaint = FALSE;
            if (plotWidget->canvas()->hasFocus() && !inPaint )
            {
            const QPaintEvent *pe = (const QPaintEvent *)e;
            inPaint = TRUE;

            plotWidget->canvas()->repaint(pe->rect().x(), pe->rect().y(),
                    pe->rect().width(), pe->rect().height(), pe->erased());
            inPaint = FALSE;
            return TRUE; 
            }
        break;
        }
			
		case QEvent::MouseButtonPress:
			{
			const QMouseEvent *me = (const QMouseEvent *)e;	
			presspos = me->pos();
			mousePressed = true;
				
			bool allAxisDisabled = true;
			for (int i=0; i < QwtPlot::axisCnt; i++)
				{
				if (plotWidget->axisEnabled(i))
					{
					allAxisDisabled = FALSE;
					break;
					}
				}
			if (allAxisDisabled && plotWidget->margin() < 2 && plotWidget->lineWidth() < 2)
				{
				QRect r = plotWidget->canvas()->rect();
				r.addCoords(2, 2, -2, -2);
				if (!r.contains(me->pos()))
					emit highlightGraph();
				}
							
			emit selectPlot();	

			xMouse=me->pos().x();
			yMouse=me->pos().y();
			
			bool drawText=plot()->drawTextActive();
			bool select=false;
			//first perform all other operations than marker selection
			if (!drawText && !movePoint &&
				!dataCursorEnabled&& !pickerActivated &&
				!moveRangeSelector && !removePoint)	
				select=selectMarker(me->pos());
			
			if (!select)
				plot()->deselectMarker();
				
			if (removePoint || movePoint || 
				(plot()->translationInProgress() && !pickerActivated)) 
				pointSelected = plot()->selectPoint(me->pos());	
						
			if (dataCursorEnabled) 
				plot()->selectCurve(me->pos());			
				
			if (plot()->pickerActivated())
				{
				plot()->movedPicker(me->pos(), true);
				if (plot()->selectPeaksOn())
					{
					pointSelected = plot()->selectPoint(me->pos());	
					return TRUE;
					}
				return TRUE;
				}

			if (me->button()==Qt::LeftButton && (plot()->drawLineActive() || plot()->lineProfile()))
				{ 	
				startLinePoint= me->pos();
				plot()->copyCanvas(TRUE);
				}
			
			if (me->button()==Qt::LeftButton && drawText)
				drawTextMarker(me->pos());		
			
			if (moveRangeSelector)
				{//moves quickly the active selector at the mouse selected point
				plot()->selectPoint(me->pos());	
				plot()->moveRangeSelector();
				}
				
			if (me->button()==Qt::RightButton && select)
				emit showMarkerPopupMenu();				
			return TRUE;	
			}		
		break;
			
		case QEvent::MouseButtonDblClick:
			{	
			const QMouseEvent *me = (const QMouseEvent *)e;	
			if (plot()->translationInProgress() && pointSelected) 
				{
				if (!pickerActivated)
					plot()->startCurveTranslation();
				else
					plot()->translateCurveTo(me->pos());
				return true;
				}

			if (plot()->selectPeaksOn() && pointSelected && pickerActivated)
				{
				plot()->selectPeak(me->pos());
				return TRUE;
				}

			if (movePoint || moveRangeSelector || pickerActivated || dataCursorEnabled)
				return false;
			
			if (removePoint) 
				{
				plot()->removePoint();
				return TRUE;
				}

			long selectedMarker=plot()->selectedMarkerKey();				
		    if (selectedMarker<0)
				{
				if (plot()->isPiePlot())
					emit showPieDialog();
				else
					{
					const QMouseEvent *me = (const QMouseEvent *)e;	
					int dist, point;
					long curveKey = plotWidget->closestCurve(me->pos().x(), me->pos().y(), dist, point);
					if (dist < 10)
						emit showPlotDialog(curveKey);
					else if (plot()->curves() > 0)
						emit showPlotDialog(plot()->curveKey(0));
					}
				return TRUE;
				}
			else
				{
				if (texts.contains(selectedMarker)>0)
					{
					emit viewTextDialog();
					return TRUE;
					}			
				if (lines.contains(selectedMarker)>0)
					{
					emit viewLineDialog();
					return TRUE;
					}
				if (images.contains(selectedMarker)>0)
					{
					emit viewImageDialog();
					return TRUE;
					}
				}	
			}
		break;
			
		case QEvent::MouseMove:
		{
		const QMouseEvent *me = (const QMouseEvent *)e;
		QPoint pos = me->pos();

		if ( plot()->zoomOn() || removePoint || moveRangeSelector || dataCursorEnabled ||
			(presspos - pos).manhattanLength() <= QApplication::startDragDistance())
				return false;
		
		long selectedMarker=plot()->selectedMarkerKey();
		
		if (plot()->drawLineActive())
			drawLineMarker(pos, plot()->drawArrow());
		else if (plot()->lineProfile() || resizeLineFromStart || resizeLineFromEnd)
			drawLineMarker(pos,FALSE);	
		else if (plot()->movePointsActivated())
			plot()->move(pos); 	
		else if (plot()->pickerActivated())
			plot()->movedPicker(pos, false);		
		else if (selectedMarker>=0)
			moveMarker(pos);
		else if (mousePressed) 	
			{
			plotWidget->canvas()->setCursor(Qt::PointingHandCursor);
			movedGraph=TRUE;
			emit moveGraph(pos);
			}
		return TRUE;
		}
       break;
		
		case QEvent::MouseButtonRelease:
		{
			const QMouseEvent *me = (const QMouseEvent *)e;
			
			Graph *g = plot();
			if (moved && !g->drawLineActive() && !g->lineProfile())
				releaseMarker();
			else if (resizeLineFromStart || resizeLineFromEnd)
				resizeLineMarker(me->pos());
			else if (g->drawLineActive())
				{ 	
				LineMarker mrk;
				mrk.attach(g->plotWidget());
				mrk.setStartPoint(startLinePoint);
				mrk.setEndPoint(QPoint(me->x(), me->y()));
				mrk.setColor(g->arrowDefaultColor());
				mrk.setWidth(g->arrowDefaultWidth());
				mrk.setStyle(g->arrowLineDefaultStyle());
				mrk.setHeadLength(g->arrowHeadDefaultLength());
				mrk.setHeadAngle(g->arrowHeadDefaultAngle());
				mrk.fillArrowHead(g->arrowHeadDefaultFill());
				mrk.drawEndArrow(g->drawArrow());
				mrk.drawStartArrow(false);
				
				g->insertLineMarker(&mrk);
				g->drawLine(false);
				mrk.detach();
				plotWidget->replot();
				}
			else if (plot()->lineProfile())
				{ 	
				QPoint endLinePoint=QPoint(me->x(),me->y());	
				if (endLinePoint == startLinePoint)
					return FALSE;
				else
					{					
					LineMarker mrk;	
					mrk.attach(g->plotWidget());
					mrk.setStartPoint(startLinePoint);
					mrk.setEndPoint(endLinePoint);
					mrk.setColor(Qt::red);
					mrk.setWidth(1);
					Qt::PenStyle style=Qt::SolidLine;
					mrk.setStyle(style);
					mrk.drawEndArrow(false);
					mrk.drawStartArrow(false);
				
      				mrk.detach();
					plot()->insertLineMarker(&mrk);
					plotWidget->replot();
					emit calculateProfile(startLinePoint,endLinePoint);
					}
				}
			else if (movedGraph)
				{	
				plotWidget->canvas()->setCursor(Qt::arrowCursor);
				emit releasedGraph();
				movedGraph=FALSE;
				mousePressed = false;
				}
		
		return TRUE;			
		}
		break;
		
		 case QEvent::KeyPress:
        {	
			const int delta = 5;
			int key=((const QKeyEvent *)e)->key();
			
			if ((key == Qt::Key_Enter || key == Qt::Key_Return)&&
				plot()->translationInProgress() && pointSelected) 
				{
				if (!pickerActivated)
					plot()->startCurveTranslation();
				else
					plot()->translateCurveTo(plotWidget->canvas()->mapFromGlobal(QCursor::pos()));
				return true;
				}
				
			if ((key == Qt::Key_Enter || key == Qt::Key_Return) &&
				plot()->selectPeaksOn() && pointSelected && pickerActivated)
				{
				plot()->selectPeak(plotWidget->canvas()->mapFromGlobal(QCursor::pos()));
				return TRUE;
				}

			if (key == Qt::Key_Tab)
				{
				plot()->selectNextMarker();
				return true;					
				}

			if (plot()->enabledCursor())
			{		
            switch(key)
				{
                case Qt::Key_Up:
						plot()->shiftCurveCursor(TRUE);
                    return TRUE;
                    
                case Qt::Key_Down:
                    	plot()->shiftCurveCursor(FALSE);
                    return TRUE;

                case Qt::Key_Right:
                case Qt::Key_Plus:
                    if ( plot()->selectedCurveID() < 0 )
                        plot()->shiftCurveCursor(TRUE);
                    else
                        plot()->shiftPointCursor(TRUE);
                    return TRUE;

                case Qt::Key_Left:
                case Qt::Key_Minus:
                    if ( plot()->selectedCurveID() < 0 )
                        plot()->shiftCurveCursor(TRUE);
                    else
                        plot()->shiftPointCursor(FALSE);
					
                    return TRUE;
					}
				} 
			
			//moving range selector				
			if (plot()->selectorsEnabled())
			{
            switch(key)
				{
                case Qt::Key_Up:
						plot()->shiftCurveSelector(TRUE);
                    return TRUE;
                    
                case Qt::Key_Down:
                    	plot()->shiftCurveSelector(TRUE);
                    return TRUE;

                case Qt::Key_Right:
                case Qt::Key_Plus:
						if (((const QKeyEvent *)e)->state ()==Qt::ControlButton)
                        	plot()->moveRangeSelector(TRUE);
						else
							plot()->shiftRangeSelector(TRUE);
                    return TRUE;

                case Qt::Key_Left:
                case Qt::Key_Minus:
                        if (((const QKeyEvent *)e)->state ()==Qt::ControlButton)
                        	plot()->moveRangeSelector(FALSE);
						else
							plot()->shiftRangeSelector(TRUE);
					
                    return TRUE;
					}
				} 
				
				// The following keys represent a direction, they are
                // organized on the keyboard.
				
				if (plot()->movePointsActivated())
				{
				switch(key)
					{
                case Qt::Key_1: 
                    plot()->moveBy(-delta, delta);
                    break;
                case Qt::Key_2:
                    plot()->moveBy(0, delta);
                    break;
                case Qt::Key_3: 
                    plot()->moveBy(delta, delta);
                    break;
                case Qt::Key_4:
                    plot()->moveBy(-delta, 0);
                    break;
                case Qt::Key_6: 
                    plot()->moveBy(delta, 0);
                    break;
                case Qt::Key_7:
                    plot()->moveBy(-delta, -delta);
                    break;
                case Qt::Key_8:
                    plot()->moveBy(0, -delta);
                    break;
                case Qt::Key_9:
                    plot()->moveBy(delta, -delta);
                    break;
					}
				}
				
			long selectedMarker=plot()->selectedMarkerKey();
			if (selectedMarker>=0)
				{
				int delta = 1;
				switch(key)
					{
					case Qt::Key_Left: 
						plot()->moveMarkerBy(-delta, 0);
                    break;
					case Qt::Key_Right:
						plot()->moveMarkerBy(delta, 0);
                    break;
					case Qt::Key_Up: 
						plot()->moveMarkerBy(0, -delta);
                    break;
					case Qt::Key_Down:
						plot()->moveMarkerBy(0, delta);
                    break;
					}
				}

			if (texts.contains(selectedMarker)>0 &&
				(key==Qt::Key_Enter|| key==Qt::Key_Return))
				{
				emit viewTextDialog();
				return TRUE;
				}			
			if (lines.contains(selectedMarker)>0 &&
				(key==Qt::Key_Enter|| key==Qt::Key_Return))
				{
				emit viewLineDialog();
				return TRUE;
				}
			if (images.contains(selectedMarker)>0 &&
				(key==Qt::Key_Enter|| key==Qt::Key_Return))
				{
				emit viewImageDialog();
				return TRUE;
				}	
			}
			break;
							
        default:
            break;
    	}
    return QObject::eventFilter(object, e);
}

void CanvasPicker::releaseMarker()
{
Q3MemArray<long> images=plot()->imageMarkerKeys();	
QList<int> texts=plot()->textMarkerKeys();
Q3MemArray<long> lines=plot()->lineMarkerKeys();

bool line = false, image = false;
	
long selectedMarker=plot()->selectedMarkerKey();
if (texts.contains(selectedMarker))
	{
	LegendMarker* mrkT = (LegendMarker*) plotWidget->marker(selectedMarker);
	mrkT->setOrigin(QPoint(xMrk,yMrk));	    
	}
else if (images.contains(selectedMarker))
	{
	image = true;

	ImageMarker* mrk = (ImageMarker*) plotWidget->marker(selectedMarker);
	mrk->setOrigin(QPoint(xMrk,yMrk));
	}
else if (lines.contains(selectedMarker))
	{
	line = true;

	LineMarker* mrk=(LineMarker*)plotWidget->marker(selectedMarker);
	int clw=plotWidget->canvas()->lineWidth();				
	mrk->setStartPoint(QPoint(startLinePoint.x() - clw, startLinePoint.y() - clw));
	mrk->setEndPoint(QPoint(endLinePoint.x() - clw, endLinePoint.y() - clw));
	}
	
plotWidget->replot();

if (line)
	plot()->highlightLineMarker(selectedMarker);
else if (image)
	plot()->highlightImageMarker(selectedMarker);
else
	plot()->highlightTextMarker(selectedMarker);
			
moved=FALSE;
emit modified();	

QApplication::restoreOverrideCursor();	
}

void CanvasPicker::moveMarker(QPoint& point)
{
QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor), true);

Q3MemArray<long> images=plot()->imageMarkerKeys();	
QList<int> texts=plot()->textMarkerKeys();
Q3MemArray<long> lines=plot()->lineMarkerKeys();
			
QPainter painter(plotWidget->canvas());
	
int w=plotWidget->canvas()->width();
int h=plotWidget->canvas()->height();

//QPixmap *canvasPic = plotWidget->canvas()->paintCache();
//QPixmap pix(*canvasPic);

QPixmap pix(w, h);
pix.fill( QColor(Qt::white));
QPixmapCache::find ("field",pix);
painter.drawPixmap(0,0,pix,0,0,-1,-1);
	
long selectedMarker=plot()->selectedMarkerKey();
if (lines.contains(selectedMarker))
	{
	LineMarker* mrk=(LineMarker*)plotWidget->marker(selectedMarker);
	
	int clw = plotWidget->canvas()->lineWidth();
	point.setX(point.x() + clw);
	point.setY(point.y() + clw);

	QPoint aux=mrk->startPoint();
    int x = aux.x()+point.x() - xMouse;
	int y = aux.y()+point.y() - yMouse;
			
	mrk->setStartPoint(QPoint(x,y));
	startLinePoint=QPoint(x,y);
		
	aux=mrk->endPoint();
	x=aux.x()+point.x()-xMouse;
	y=aux.y()+point.y()-yMouse;
	mrk->setEndPoint(QPoint(x,y));
	endLinePoint=QPoint(x,y);

	mrk->draw(&painter, plotWidget->canvasMap(QwtPlot::xBottom), 
				plotWidget->canvasMap(QwtPlot::yLeft), QRect(0,0,0,0));
	}
else if (images.contains(selectedMarker))
	{
	ImageMarker* mrk=(ImageMarker*)plotWidget->marker(selectedMarker);

	xMrk+=point.x()-xMouse;
	yMrk+=point.y()-yMouse;

	// FIXME: next line
    // painter.setRasterOp(Qt::NotROP);	
	painter.drawRect(QRect(QPoint(xMrk,yMrk),mrk->size()));
	}
else if (texts.contains(selectedMarker))
	{
	xMrk+=point.x()-xMouse;
	yMrk+=point.y()-yMouse;

	LegendMarker* mrk=(LegendMarker*)plotWidget->marker(selectedMarker);				
	// FIXME: next line
    // painter.setRasterOp(Qt::NotROP);	
	painter.drawRect(QRect(QPoint(xMrk,yMrk),mrk->rect().size()));
	}
xMouse=point.x();
yMouse=point.y();
moved=TRUE;
emit modified();	
}

void CanvasPicker::drawTextMarker(const QPoint& point)
{
LegendMarker mrkT(plotWidget);			
mrkT.setOrigin(point);
mrkT.setBackground(plot()->textMarkerDefaultFrame());
mrkT.setFont(plot()->defaultTextMarkerFont());
mrkT.setTextColor(plot()->textMarkerDefaultColor());
mrkT.setBackgroundColor(plot()->textMarkerDefaultBackground());
mrkT.setText(tr("enter your text here"));
plot()->insertTextMarker(&mrkT);		
plot()->drawText(FALSE);
emit drawTextOff();
}

void CanvasPicker::resizeLineMarker(const QPoint& point)
{	
long selectedMarker=plot()->selectedMarkerKey();
LineMarker* mrk=(LineMarker*)plotWidget->marker(selectedMarker);
if (!mrk)
	return;

int clw=plotWidget->canvas()->lineWidth();	

if (resizeLineFromStart)			
	mrk->setStartPoint(QPoint(point.x() - clw, point.y() - clw));
else if (resizeLineFromEnd)
	mrk->setEndPoint(QPoint(point.x() - clw, point.y() - clw));
	
plotWidget->replot();
plot()->highlightLineMarker(selectedMarker);
			
emit modified();
	
resizeLineFromStart = false;
resizeLineFromEnd = false;	

QApplication::restoreOverrideCursor();		
}

void CanvasPicker::drawLineMarker(const QPoint& point, bool endArrow)
{
plot()->plotWidget()->canvas()->repaint();
LineMarker mrk;
mrk.attach(plotWidget);

int clw = plotWidget->canvas()->lineWidth();	
mrk.setStartPoint(QPoint(startLinePoint.x() + clw, startLinePoint.y() + clw));
mrk.setEndPoint(QPoint(point.x() + clw,point.y() + clw));
mrk.setWidth(1);
mrk.setStyle(Qt::SolidLine);
mrk.drawEndArrow(endArrow);
mrk.drawStartArrow(false);

if (plot()->drawLineActive())
    mrk.setColor(Qt::black);
else
	mrk.setColor(Qt::red);

plotWidget->replot();
mrk.detach();	
}

// Selects and highlights the marker 
bool CanvasPicker::selectMarker(const QPoint& point)
{
if (plot()->zoomOn())
	return false;

Q3MemArray<long> images=plot()->imageMarkerKeys();	
QList<int> texts=plot()->textMarkerKeys();
Q3MemArray<long> lines=plot()->lineMarkerKeys();
int n=texts.size();	
int m=lines.size();			
int p=images.size();

plot()->copyCanvas(true);
plotWidget->replot();

int i;
for (i=0;i<m;i++)
	{//line marker selected(first)?	
	LineMarker* mrkL=(LineMarker*) plotWidget->marker(lines[i]);
	if (mrkL)
		{
		int d=mrkL->width()+(int)floor(mrkL->headLength()*tan(M_PI*mrkL->headAngle()/180.0)+0.5);
		double dist=mrkL->dist(point.x(),point.y());
		if (dist <= d)
			{
			plot()->highlightLineMarker(lines[i]);

			QRect sr = QRect (QPoint(0,0), QSize(7, 7));
			sr.moveCenter (mrkL->startPoint());
			if (sr.contains(point))
				{
				QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor), true);
				resizeLineFromStart = true;
				startLinePoint = mrkL->endPoint();
				return TRUE;
				}

			QRect er = sr;
			er.moveCenter (mrkL->endPoint());
			if (er.contains(point))
				{
				QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor), true);
				resizeLineFromEnd = true;
				startLinePoint = mrkL->startPoint();
				return TRUE;
				}

			return TRUE;
			}
		}
	}

for (i=0;i<n;i++)
	{//text marker selected(second)?	
	LegendMarker* mrk=(LegendMarker*)plotWidget->marker(texts[i]);
	QRect mRect=mrk->rect();		
	if (mRect.contains(point))
		{
		plot()->highlightTextMarker(texts[i]);	
		QPoint origin=mRect.topLeft();	
		xMrk=origin.x(); yMrk=origin.y();			
		return TRUE;
		}
	}

for (i=0;i<p;i++)
	{//image marker selected(last)?	
	ImageMarker* mrkI=(ImageMarker*)plotWidget->marker(images[i]);
	if (mrkI->rect().contains(point))
		{
		plot()->highlightImageMarker(images[i]);	
		QPoint origin=mrkI->getOrigin();
		xMrk=origin.x(); yMrk=origin.y();
		return TRUE;
		}
	}
return FALSE;
}
