#include "plot.h"
#include "scaleDraws.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>

//FIXME: All functionality disabled for now (needs port to Qwt5)

	Plot::Plot(QWidget *parent, const char *name)
: QwtPlot(parent)
{
#if false
	movedGraph=FALSE;
	graphToResize=FALSE;
	ShiftButton=FALSE;

	d_lineWidth = 1;		
	minTickLength = 5;
	majTickLength = 9;	

	setGeometry(QRect(0,0,500,400));
	setAutoLegend(FALSE); // We don't want a Qwt legend
	setAxisTitle(0, tr("Y Axis Title"));
	setAxisTitle(2, tr("X Axis Title"));	

	//custom scale
	for (int i= 0; i<QwtPlot::axisCnt; i++)
	{
		ticksType<<Out;

		QwtScale *scale = (QwtScale *) axis(i);
		if (scale)
			scale->setBaselineDist(0);

		ScaleDraw *sd= new ScaleDraw(1);
		setAxisScaleDraw (i, sd);
	}

	QwtPlotLayout *pLayout=plotLayout();
	pLayout->setCanvasMargin(0);

	QwtPlotCanvas* plCanvas=canvas();
	plCanvas->setFocusPolicy(QWidget::StrongFocus);
	plCanvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
	plCanvas->setFocus();
	plCanvas->setFrameShadow( QwtPlot::Plain);
	plCanvas->setCursor(Qt::arrowCursor);
	plCanvas->setLineWidth(0);

	setFocusPolicy(QWidget::StrongFocus);
	setFocusProxy(plCanvas);

	setFrameShape (QFrame::Box);
	setFrameShadow(QFrame::Plain);
	setLineWidth(0);
#endif
}

QColor Plot::frameColor()
{
#if false
	return palette().color(QPalette::Active, QColorGroup::Foreground);
#endif
}

void Plot::printFrame(QPainter *painter, const QRect &rect) const
{
#if false
	int lw = lineWidth();
	if (!lw)
		return;

	QPalette pal = palette();
	QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);

	painter->save();
	painter->setPen (QPen(color, lw, Qt::SolidLine));

	QwtPainter::drawRect(painter, rect.x(), rect.y(), rect.width(), rect.height());
	painter->restore();
#endif
}

void Plot::printCanvas(QPainter *painter, const QRect &canvasRect,
		const QwtArray<QwtScaleMap> &map, const QwtPlotPrintFilter &pfilter) const
{
#if false
	const QwtPlotCanvas* plotCanvas=canvas();	
	QRect rect=canvasRect;
	int w=plotCanvas->lineWidth();

	if (w>0)
	{
		QPalette pal = plotCanvas->palette();
		QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);

		painter->save();
		painter->setPen (QPen(color,w,Qt::SolidLine));

		if (w == 1 && ticksType[QwtPlot::xBottom] == Plot::Out)
			rect.setHeight(canvasRect.height() + 1);	

		QwtPainter::drawRect(painter, rect.x(), rect.y(), rect.width(), rect.height());
		painter->restore();
	}

	painter->setClipping(TRUE);
	rect = QRect(canvasRect.x()+1, canvasRect.y()+1, canvasRect.width(), canvasRect.height()-1);
	QwtPainter::setClipRect(painter, rect);

	drawCanvasItems(painter, canvasRect, map, pfilter);
#endif
}

void Plot::drawCanvasItems (QPainter *painter, const QRect &rect, 
		const QwtArray< QwtScaleMap > &map, const QwtPlotPrintFilter &pfilter) const
{
#if false
	QwtPlot::drawCanvasItems(painter, rect, map, pfilter);

	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		if (ticksType[i] == In)	
			drawInwardTicks(painter, rect, map[i], i);
		else if (ticksType[i] == Both)	
			drawInwardMinorTicks(painter, rect, map[i], i);
	}
#endif
}

void Plot::drawInwardTicks(QPainter *painter, const QRect &rect, 
		const QwtScaleMap &map, int axis) const
{
#if false
	QwtScale *scale=(QwtScale *) QwtPlot::axis (axis);
	if (!scale)
		return;

	int x1=rect.left();
	int x2=rect.right();
	int y1=rect.top();
	int y2=rect.bottom();

	QPalette pal=scale->palette();
	QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);

	painter->save();	
	painter->setPen(QPen(color,d_lineWidth,QPainter::SolidLine));

	QwtScaleDiv *scDiv=(QwtScaleDiv *)axisScale(axis);
	int minTicks=scDiv->minCnt();
	int majTicks= scDiv->majCnt();

	int j, x, y, low,high;
	switch (axis)
	{
		case QwtPlot::yLeft:
			x=x1;
			low=y1+majTickLength;
			high=y2-majTickLength;
			for (j = 0; j < minTicks; j++)
			{
				y = map.transform(scDiv->minMark(j));
				if (y>low && y< high)
					QwtPainter::drawLine(painter, x, y, x+minTickLength, y);
			}		
			for (j = 0; j < majTicks; j++)
			{
				y = map.transform(scDiv->majMark(j));
				if (y>low && y< high)
					QwtPainter::drawLine(painter, x, y, x+majTickLength, y);
			}
			break;

		case QwtPlot::yRight:
			{
				x=x2;
				low=y1+majTickLength;
				high=y2-majTickLength;
				for (j = 0; j < minTicks; j++)
				{
					y = map.transform(scDiv->minMark(j));
					if (y>low && y< high)
						QwtPainter::drawLine(painter, x+1, y, x-minTickLength, y);
				}
				for (j = 0; j <majTicks; j++)
				{
					y = map.transform(scDiv->majMark(j));
					if (y>low && y< high)
						QwtPainter::drawLine(painter, x+1, y, x-majTickLength, y);
				}
			}
			break;

		case QwtPlot::xBottom:
			y=y2;
			low=x1+majTickLength;
			high=x2-majTickLength;
			for (j = 0; j < minTicks; j++)
			{
				x = map.transform(scDiv->minMark(j));
				if (x>low && x<high)
					QwtPainter::drawLine(painter, x, y+1, x, y-minTickLength);
			}	
			for (j = 0; j < majTicks; j++)
			{
				x = map.transform(scDiv->majMark(j));
				if (x>low && x<high)
					QwtPainter::drawLine(painter, x, y+1, x, y-majTickLength);
			}
			break;

		case QwtPlot::xTop:
			y=y1;
			low=x1+majTickLength;
			high=x2-majTickLength;
			for (j = 0; j < minTicks; j++)
			{
				x = map.transform(scDiv->minMark(j));
				if (x>low && x<high)
					QwtPainter::drawLine(painter, x, y, x, y + minTickLength);
			}		
			for (j = 0; j <majTicks; j++)
			{
				x = map.transform(scDiv->majMark(j));
				if (x>low && x<high)
					QwtPainter::drawLine(painter, x, y, x, y + majTickLength);
			}
			break;
	}
	painter->restore();
#endif
}

void Plot::drawInwardMinorTicks(QPainter *painter, const QRect &rect, 
		const QwtScaleMap &map, int axis) const
{
#if false
	QwtScale *scale=(QwtScale *) QwtPlot::axis (axis);
	if (!scale)
		return;

	int x1=rect.left();
	int x2=rect.right();	
	int y1=rect.top();
	int y2=rect.bottom();

	QPalette pal=scale->palette();
	QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);

	painter->save();	
	painter->setPen(QPen(color, d_lineWidth, QPainter::SolidLine));

	QwtScaleDiv *scDiv=(QwtScaleDiv *)axisScale(axis);
	int minTicks=scDiv->minCnt();

	int j, x, y, low,high;
	switch (axis)
	{
		case QwtPlot::yLeft:
			x=x1;
			low=y1+majTickLength;
			high=y2-majTickLength;
			for (j = 0; j < minTicks; j++)
			{
				y = map.transform(scDiv->minMark(j));
				if (y>low && y< high)
					QwtPainter::drawLine(painter, x, y, x+minTickLength, y);
			}		
			break;

		case QwtPlot::yRight:
			{
				x=x2;
				low=y1+majTickLength;
				high=y2-majTickLength;
				for (j = 0; j < minTicks; j++)
				{
					y = map.transform(scDiv->minMark(j));
					if (y>low && y< high)
						QwtPainter::drawLine(painter, x+1, y, x-minTickLength, y);
				}
			}
			break;

		case QwtPlot::xBottom:
			y=y2;
			low=x1+majTickLength;
			high=x2-majTickLength;
			for (j = 0; j < minTicks; j++)
			{
				x = map.transform(scDiv->minMark(j));
				if (x>low && x<high)
					QwtPainter::drawLine(painter, x, y+1, x, y-minTickLength);
			}	
			break;

		case QwtPlot::xTop:
			y=y1;
			low=x1+majTickLength;
			high=x2-majTickLength;
			for (j = 0; j < minTicks; j++)
			{
				x = map.transform(scDiv->minMark(j));
				if (x>low && x<high)
					QwtPainter::drawLine(painter, x, y, x, y + minTickLength);
			}		
			break;
	}
	painter->restore();
#endif
}

void Plot::setTicksLineWidth(int width)
{
#if false
	if (d_lineWidth == width)
		return;

	d_lineWidth = width;
#endif
}

void Plot::setTickLength (int minLength, int majLength)
{
#if false
	if (majTickLength == majLength &&
			minTickLength == minLength)
		return;

	majTickLength = majLength;
	minTickLength = minLength;
#endif
}

void Plot::setTicksType(int axis, int type)
{
#if false
	ticksType[axis]=type;
#endif
}

void Plot::mousePressEvent ( QMouseEvent * e )
{
#if false
	if(e->state()==Qt::ShiftButton)
		ShiftButton=TRUE;

	emit selectPlot();
#endif
}

void Plot::mouseMoveEvent ( QMouseEvent * e )
{
#if false
	if(ShiftButton)
	{
		graphToResize=TRUE;
		emit resizeGraph(e->pos());	
	}				
	else
	{
		movedGraph=TRUE;
		emit moveGraph(e->pos());
	}
#endif
}

void Plot::mouseReleaseEvent ( QMouseEvent *)
{
#if false
	if (movedGraph)
	{
		emit releasedGraph();
		movedGraph=FALSE;
	}

	if (graphToResize)
	{
		emit resizedGraph();
		graphToResize=FALSE;
		ShiftButton=FALSE;
	}
#endif
}

void Plot::drawPixmap(QPainter *painter, const QRect &rect)
{
#if false
	if ( painter == 0 || !painter->isActive() || size().isNull() )
		return;

	painter->save();

	QwtPlotLayout *layout = plotLayout();
	layout->activate(this, rect, 0);

	printTitle(painter, layout->titleRect());

	for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
	{
		const QwtScale *scale = this->axis(axis);
		if (scale)
		{
			int baseDist = scale->baseLineDist();
			int startDist, endDist;
			if (axis == QwtPlot::xTop || axis == QwtPlot::yRight)
			{//synchronize secondary scales 
				scale = this->axis(axis-1);
				if (scale)
					scale->minBorderDist(startDist, endDist);
			}
			else
				scale->minBorderDist(startDist, endDist);
			printScale(painter, axis, startDist, endDist, baseDist, layout->scaleRect(axis));
		}
	}

	const QwtPlotCanvas *canvas = this->canvas();
	const QPixmap *canvasPix = canvas->cache();
	if (canvasPix)
	{
		QRect cr = layout->canvasRect();
		const int clw = canvas->lineWidth();
		if (clw > 0)
		{
			QPalette pal = canvas->palette();
			QColor color = pal.color(QPalette::Active, QColorGroup::Foreground);
			painter->setPen (QPen(color, clw, Qt::SolidLine));

			cr.moveBy(clw/2, clw/2);
			cr.setWidth(cr.width() - clw);
			cr.setHeight(cr.height() - clw);

			painter->drawRect(cr);//draw canvas frame

			cr = layout->canvasRect();
			cr.moveBy(clw, clw);
			cr.setWidth(cr.width() - clw);
			cr.setHeight(cr.height() - clw);
		}

		QPixmap pix = QPixmap (cr.width(), cr.height(), -1);
		copyBlt (&pix, 0, 0, canvasPix, 0, 0, -1, -1);
		painter->drawPixmap (cr, pix);
	}

	painter->restore();
#endif
}

void Plot::print(QPainter *painter, const QRect &plotRect,
		const QwtPlotPrintFilter &pfilter) const
{
#if false
	int axis;

	if ( painter == 0 || !painter->isActive() ||
			!plotRect.isValid() || size().isNull() )
		return;

	painter->save();

	// All paint operations need to be scaled according to
	// the paint device metrics. 

	QwtPainter::setMetricsMap(this, painter->device());

#if QT_VERSION < 300 
	if ( painter->device()->isExtDev() )
	{
		QPaintDeviceMetrics metrics(painter->device());
		if ( metrics.logicalDpiX() == 72 && metrics.logicalDpiY() == 72 )
		{
			// In Qt 2.x QPrinter returns hardcoded wrong metrics.
			// So scaling won´t work: we reset to screen resolution

			QwtPainter::setMetricsMap(this, this);
		}
	}
#endif

	const QwtMetricsMap &metricsMap = QwtPainter::metricsMap();

	pfilter.apply((QwtPlot *)this);

	int baseLineDists[QwtPlot::axisCnt];
	if ( !(pfilter.options() & QwtPlotPrintFilter::PrintCanvasBackground) )
	{
		// In case of no background we set the backbone of
		// the scale on the frame of the canvas.

		for (axis = 0; axis < QwtPlot::axisCnt; axis++ )
		{
			QwtScale *scale = (QwtScale *)this->axis(axis);
			if ( scale )
			{
				baseLineDists[axis] = scale->baseLineDist();
				scale->setBaselineDist(0);
			}
		}
	}
	// Calculate the layout for the print.

	int layoutOptions = QwtPlotLayout::IgnoreScrollbars 
		| QwtPlotLayout::IgnoreFrames;
	if ( !(pfilter.options() & QwtPlotPrintFilter::PrintMargin) )
		layoutOptions |= QwtPlotLayout::IgnoreMargin;
	if ( !(pfilter.options() & QwtPlotPrintFilter::PrintLegend) )
		layoutOptions |= QwtPlotLayout::IgnoreLegend;

	QwtPlotLayout *layout = (QwtPlotLayout *)this->plotLayout();
	layout->activate(this, QwtPainter::metricsMap().deviceToLayout(plotRect), 
			layoutOptions);

	if ((pfilter.options() & QwtPlotPrintFilter::PrintTitle)
			&& (!title().isEmpty()))
	{
		printTitle(painter, layout->titleRect());
	}

	for ( axis = 0; axis < QwtPlot::axisCnt; axis++ )
	{
		QwtScale *scale = (QwtScale *)this->axis(axis);
		if (scale)
		{
			int baseDist = scale->baseLineDist();

			int startDist, endDist;
			if ( axis == xTop || axis == yRight)
			{
				scale = (QwtScale *)this->axis(axis-1);
				if (scale)
					scale->minBorderDist(startDist, endDist);
			}
			else
				scale->minBorderDist(startDist, endDist);

			printScale(painter, axis, startDist, endDist,
					baseDist, layout->scaleRect(axis));
		}
	}

	const QRect canvasRect = metricsMap.layoutToDevice(layout->canvasRect());

	QwtArray<QwtScaleMap> map(axisCnt);
	for (axis = 0; axis < axisCnt; axis++)
	{
		const QwtScaleDiv *scaleDiv = this->axisScale(axis);
		map[axis].setDblRange(scaleDiv->lBound(),
				scaleDiv->hBound(), scaleDiv->logScale());

		double from, to;
		if ( axisEnabled(axis) )
		{
			QwtScale *scale = (QwtScale *)this->axis(axis);
			const int sDist = scale->startBorderDist();
			const int eDist = scale->endBorderDist();
			const QRect &scaleRect = layout->scaleRect(axis);

			if ( axis == xTop || axis == xBottom )
			{
				from = metricsMap.layoutToDeviceX(scaleRect.left() + sDist);
				to = metricsMap.layoutToDeviceX(scaleRect.right() - eDist);
			}
			else
			{
				from = metricsMap.layoutToDeviceY(scaleRect.bottom() - sDist);
				to = metricsMap.layoutToDeviceY(scaleRect.top() + eDist);
			}
		}
		else
		{
			const int margin = plotLayout()->canvasMargin(axis);

			const QRect &canvasRect = plotLayout()->canvasRect();
			if ( axis == yLeft || axis == yRight )
			{
				from = metricsMap.layoutToDeviceX(canvasRect.bottom() - margin);
				to = metricsMap.layoutToDeviceX(canvasRect.top() + margin);
			}
			else
			{
				from = metricsMap.layoutToDeviceY(canvasRect.left() + margin);
				to = metricsMap.layoutToDeviceY(canvasRect.right() - margin);
			}
		}
		map[axis].setIntRange( qwtInt(from), qwtInt(to));
	}


	// The maps are already scaled. 
	QwtPainter::setMetricsMap(painter->device(), painter->device());

	printCanvas(painter, canvasRect, map, pfilter);

	QwtPainter::resetMetricsMap();

	layout->invalidate();

	// reset all widgets with their original attributes.
	if ( !(pfilter.options() & QwtPlotPrintFilter::PrintCanvasBackground) )
	{
		// restore the previous base line dists

		for (axis = 0; axis < QwtPlot::axisCnt; axis++ )
		{
			QwtScale *scale = (QwtScale *)this->axis(axis);
			if ( scale )
				scale->setBaselineDist(baseLineDists[axis]);
		}
	}

	pfilter.reset((QwtPlot *)this);

	painter->restore();
#endif
}


