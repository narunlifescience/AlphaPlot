/***************************************************************************
    File                 : graph.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Graph widget
                           
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

#include <QVarLengthArray>

static const char *cut_xpm[]={
"18 18 3 1",
". c None",
"# c #000000",
"a c #00007f",
"..................",
"..................",
"..................",
".......#...#......",
".......#...#......",
".......#...#......",
".......##.##......",
"........#.#.......",
"........###.......",
".........#........",
"........a#a.......",
"........a.aaa.....",
"......aaa.a..a....",
".....a..a.a..a....",
".....a..a.a..a....",
".....a..a..aa.....",
"......aa..........",
".................."};

static const char *copy_xpm[]={
"15 13 4 1",
"# c None",
". c #000000",
"b c #00007f",
"a c #ffffff",
"......#########",
".aaaa..########",
".aaaa.a.#######",
".a..a.bbbbbb###",
".aaaaabaaaabb##",
".a....baaaabab#",
".aaaaaba..abbbb",
".a....baaaaaaab",
".aaaaaba.....ab",
"......baaaaaaab",
"######ba.....ab",
"######baaaaaaab",
"######bbbbbbbbb"};

static const char *unzoom_xpm[]={
"18 17 2 1",
". c None",
"# c #000000",
"..................",
"...#..............",
"..###.............",
".#.#.#.......##...",
"...#.....##..##...",
"...#..##.##.......",
"...#..##....##....",
"...#........##....",
"...#...##.........",
"...#...##.##.##...",
".#.#.#....##.##...",
"..###.............",
"...#...#......#...",
"...#..#........#..",
"...##############.",
"......#........#..",
".......#......#..."};

#include "graph.h"
#include "canvaspicker.h"
#include "ErrorBar.h"
#include "LegendMarker.h"
#include "LineMarker.h"
#include "cursors.h"
#include "scalePicker.h"
#include "pie.h"
#include "ImageMarker.h"
#include "BarCurve.h"
#include "BoxCurve.h"
#include "Histogram.h"
#include "VectorCurve.h"
#include "scales.h"
#include "plot.h"
#include "parser.h"
#include "fileDialogs.h"
#include "colorBox.h"
#include "patternBox.h"
#include "symbolBox.h"
#include "FunctionCurve.h"
#include "Fitter.h"

#include <qapplication.h>
#include <qbitmap.h>
#include <qclipboard.h>
#include <qcursor.h>
#include <q3dragobject.h>
#include <q3filedialog.h> 
#include <qimage.h>
#include <qmessagebox.h>
#include <qpixmap.h> 
#include <q3picture.h> 
#include <q3painter.h> 
#include <qpixmapcache.h>
#include <q3popupmenu.h>
#include <q3ptrlist.h>
#include <q3paintdevicemetrics.h>

#include <QTextStream>

#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_text.h>
#include <qwt_text_label.h>

#include <gsl/gsl_sort.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

Graph::Graph(QWidget* parent, const char* name, Qt::WFlags f)
: QWidget(parent,name,f)
{
	if ( !name )
		setName( "graph" );

	d_functions = 0;
	fitter = 0;
	n_curves=0;
	linesOnPlot=0;
	widthLine=1;mrkX=-1;mrkY=-1;fitID=0;
	selectedCol=0;selectedPoint=-1;
	selectedCurve =-1;selectedMarker=-1;selectedCursor=-1;
	startPoint=0;endPoint=-1;
	startID=-1; endID=-1;
	pieRay=100;	
	lineProfileOn=FALSE;
	drawTextOn=FALSE;
	drawLineOn=FALSE;
	drawArrowOn=FALSE;
	cursorEnabled=FALSE;
	movePointsEnabled=FALSE;
	removePointsEnabled=FALSE;
	pickerEnabled=FALSE;
	rangeSelectorsEnabled=FALSE;
	piePlot=FALSE;
	ignoreResize= false;
	drawAxesBackbone = true;
	autoscale = true;
	autoScaleFonts = true;
	translateOn = false;

	d_user_step = Q3MemArray<bool>(QwtPlot::axisCnt);
	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		axisType << Numeric;
		axesFormatInfo << QString::null;
		axesFormulas << QString::null;
		d_user_step[i] = false;
	}

	d_plot = new Plot(this);		
	cp = new CanvasPicker(this);

	titlePicker = new TitlePicker(d_plot);
	scalePicker = new ScalePicker(d_plot);

	d_zoomer[0]= new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft,
			QwtPicker::DragSelection | QwtPicker::CornerToCorner, QwtPicker::AlwaysOff, d_plot->canvas());
	d_zoomer[0]->setRubberBandPen(QPen(Qt::black));
	d_zoomer[1] = new QwtPlotZoomer(QwtPlot::xTop, QwtPlot::yRight,
			QwtPicker::DragSelection | QwtPicker::CornerToCorner,
			QwtPicker::AlwaysOff, d_plot->canvas());
	zoom(FALSE);

	setGeometry( QRect(0, 0, 520, 420 ) );
	setFocusPolicy(Qt::StrongFocus);
	setFocusProxy(d_plot);
	setMouseTracking(TRUE );

	grid.majorOnX=0;
	grid.majorOnY=0;
	grid.minorOnX=0;
	grid.minorOnY=0;
	grid.majorCol=3;
	grid.majorStyle=0;
	grid.majorWidth=1;
	grid.minorCol=15;
	grid.minorStyle=2;
	grid.minorWidth=1;
	grid.xZeroOn=0;
	grid.yZeroOn=0;	
	setGridOptions(grid);
	grid.xAxis = QwtPlot::xBottom;
	grid.yAxis = QwtPlot::yLeft;

	LegendMarker *mrk = new LegendMarker(d_plot);
	mrk->setOrigin(QPoint(10, 20));
	legendMarkerID = d_plot->insertMarker(mrk);

	connect (d_plot,SIGNAL(selectPlot()), this, SLOT(activateGraph()));
	connect (d_plot,SIGNAL(selectPlot()), this, SLOT(drawFocusRect()));
	connect (d_plot,SIGNAL(moveGraph(const QPoint&)),this,SLOT(moveGraph(const QPoint&)));
	connect (d_plot,SIGNAL(releasedGraph()),this,SLOT(releaseGraph()));

	connect (cp,SIGNAL(moveGraph(const QPoint&)),this,SLOT(moveGraph(const QPoint&)));
	connect (cp,SIGNAL(releasedGraph()),this,SLOT(releaseGraph()));
	connect (cp,SIGNAL(highlightGraph()),this, SLOT(drawFocusRect()));
	connect (cp,SIGNAL(selectPlot()),this,SLOT(activateGraph()));
	connect (cp,SIGNAL(drawTextOff()),this,SIGNAL(drawTextOff()));
	connect (cp,SIGNAL(viewImageDialog()),this,SIGNAL(viewImageDialog()));
	connect (cp,SIGNAL(viewTextDialog()),this,SIGNAL(viewTextDialog()));
	connect (cp,SIGNAL(viewLineDialog()),this,SIGNAL(viewLineDialog()));
	connect (cp,SIGNAL(showPlotDialog(int)),this,SIGNAL(showPlotDialog(int)));
	connect (cp,SIGNAL(showPieDialog()),this,SIGNAL(showPieDialog()));
	connect (cp,SIGNAL(showMarkerPopupMenu()),this,SIGNAL(showMarkerPopupMenu()));
	connect (cp,SIGNAL(modified()), this, SLOT(modified()));
	connect (cp,SIGNAL(modified()), this, SIGNAL(modifiedGraph()));
	connect (cp,SIGNAL(calculateProfile(const QPoint&, const QPoint&)),
			this,SLOT(calculateLineProfile(const QPoint&, const QPoint&)));

	connect (titlePicker,SIGNAL(highlightGraph()), this, SLOT(drawFocusRect()));
	connect (titlePicker,SIGNAL(showTitleMenu()),this,SLOT(showTitleContextMenu()));
	connect (titlePicker,SIGNAL(doubleClicked()),this,SIGNAL(viewTitleDialog()));
	connect (titlePicker,SIGNAL(removeTitle()),this,SLOT(removeTitle()));
	connect (titlePicker,SIGNAL(clicked()), this,SLOT(selectTitle()));
	connect (titlePicker,SIGNAL(moveGraph(const QPoint&)),this,SLOT(moveGraph(const QPoint&)));
	connect (titlePicker,SIGNAL(releasedGraph()),this,SLOT(releaseGraph()));

	connect (scalePicker,SIGNAL(highlightGraph()),this, SLOT(drawFocusRect()));
	connect (scalePicker,SIGNAL(clicked()),this,SLOT(activateGraph()));
	connect (scalePicker,SIGNAL(clicked()),this,SLOT(deselectMarker()));
	connect (scalePicker,SIGNAL(axisDblClicked(int)),this,SIGNAL(axisDblClicked(int)));
	connect (scalePicker,SIGNAL(axisTitleRightClicked(int)),this,SLOT(showAxisTitleMenu(int)));
	connect (scalePicker,SIGNAL(axisRightClicked(int)),this,SLOT(showAxisContextMenu(int)));
	connect (scalePicker,SIGNAL(xAxisTitleDblClicked()),this,SIGNAL(xAxisTitleDblClicked()));		
	connect (scalePicker,SIGNAL(yAxisTitleDblClicked()),this,SIGNAL(yAxisTitleDblClicked()));
	connect (scalePicker,SIGNAL(rightAxisTitleDblClicked()),this,SIGNAL(rightAxisTitleDblClicked()));
	connect (scalePicker,SIGNAL(topAxisTitleDblClicked()),this,SIGNAL(topAxisTitleDblClicked()));
	connect (scalePicker,SIGNAL(moveGraph(const QPoint&)),this,SLOT(moveGraph(const QPoint&)));
	connect (scalePicker,SIGNAL(releasedGraph()),this, SLOT(releaseGraph()));

	connect (d_zoomer[0],SIGNAL(zoomed (const QwtDoubleRect &)),this,SLOT(zoomed (const QwtDoubleRect &)));
}

void Graph::drawFocusRect()
{
	if (translateOn || pickerEnabled)
		return;

	d_plot->grid()->setAxis(grid.xAxis, grid.yAxis);
	emit modifiedGraph(); 
}

void Graph::customLegend()
{
	LegendMarker *mrk = (LegendMarker*) d_plot->marker(legendMarkerID);
	if (!mrk)
		return;

	mrk->setBackground(defaultMarkerFrame);
	mrk->setFont(defaultMarkerFont);
	mrk->setTextColor(defaultTextMarkerColor);
	mrk->setBackgroundColor(defaultTextMarkerBackground);

	if (!n_curves)
	{
		mrk->setValue(50, 990);
		mrk->updateOrigin();
	}
}

void Graph::emitModified()
{
	emit modifiedGraph();
}

void Graph::modified()
{
	emit modifiedGraph(this);
}

void Graph::moveGraph(const QPoint& pos)
{
	emit moveGraph(this, pos);
}

void Graph::releaseGraph()
{
	emit releaseGraph(this);
}

void Graph::activateGraph()
{
	emit selectedGraph(this);
	setFocus();
}

void Graph::deselectMarker()
{
	selectedMarker = -1;
	d_plot->replot();
}

long Graph::selectedMarkerKey()
{
	return selectedMarker;
}

QwtPlotMarker* Graph::selectedMarkerPtr()
{
	return d_plot->marker(selectedMarker);
}

void Graph::setSelectedMarker(long mrk)
{
	selectedMarker=mrk;
}

void Graph::highlightLineMarker(long markerID)
{
	LineMarker* mrkL = (LineMarker*) d_plot->marker(markerID);	
	if (!mrkL)
		return;

	selectedMarker=markerID;		
	QwtPlotCanvas *canvas=d_plot->canvas ();

	// draw highlight line + end resizing rectangles 
	QPainter painter(canvas);
	painter.save();

	// FIXME: fix the next line
	// painter.setRasterOp(Qt::NotXorROP);
	painter.setPen(QPen(QColor(Qt::red), mrkL->width(), Qt::SolidLine));
	painter.drawLine(mrkL->startPoint(), mrkL->endPoint());				

	painter.setPen(QPen(QColor(Qt::black), mrkL->width(), Qt::SolidLine));
	painter.setBrush(QBrush(QColor(Qt::black), Qt::SolidPattern));

	QRect sr = QRect (QPoint(0,0), QSize(7, 7));
	sr.moveCenter (mrkL->startPoint());
	painter.drawRect(sr);

	sr.moveCenter (mrkL->endPoint());
	painter.drawRect(sr);
	painter.restore();	
}

void Graph::highlightTextMarker(long markerID)
{
	LegendMarker* mrk = (LegendMarker*)d_plot->marker(markerID);
	if (!mrk)
		return;

	selectedMarker=markerID;		

	QwtPlotCanvas *canvas=d_plot->canvas ();
	QPainter painter(canvas);
	painter.setPen(QPen(Qt::red,2,Qt::SolidLine));
	// FIXME: next line
	//painter.setRasterOp(Qt::NotXorROP);
	painter.drawRect (mrk->rect());	
}

void Graph::highlightImageMarker(long markerID)
{
	ImageMarker* mrkI = (ImageMarker*)d_plot->marker(markerID);
	if (!mrkI)
		return;	

	selectedMarker=markerID;					
	QwtPlotCanvas *canvas=d_plot->canvas ();
	QPainter painter(canvas);
	painter.setPen(QPen(Qt::red,2,Qt::SolidLine));
	// FIXME: next line
	//painter.setRasterOp(Qt::NotXorROP);
	painter.drawRect (mrkI->rect());	
}

void Graph::selectNextMarker()
{
	Q3ValueList<int> mrkKeys=d_plot->markerKeys();
	int n=mrkKeys.size();
	if (n==0)
		return;

	int min_key=mrkKeys[0], max_key=mrkKeys[0];
	for (int i = 0; i<n; i++ )
	{
		if (mrkKeys[i] >= max_key)
			max_key=mrkKeys[i];
		if (mrkKeys[i] <= min_key)
			min_key=mrkKeys[i];
	}

	d_plot->replot();
	Q3ValueList<int> texts=textMarkerKeys();
	if (selectedMarker >= 0)
	{
		int key = selectedMarker+1;
		if ( key > max_key )
			key=min_key;
		while(key <= max_key)
		{
			if (texts.contains(key))
			{
				highlightTextMarker(key);
				break;
			}
			else if (d_lines.contains(key))
			{
				highlightLineMarker(key);
				break;
			}
			else  if (d_images.contains(key))
			{
				highlightImageMarker(key);
				break;
			}
			else
				key++;
		}
	}
	else
	{
		int key=min_key;
		while(selectedMarker < 0)
		{
			if (texts.contains(key))
				highlightTextMarker(key);
			else if (d_lines.contains(key))
				highlightLineMarker(key);
			else  if (d_images.contains(key))
				highlightImageMarker(key);
			key++;
		}
	}
}

void Graph::replot()
{
	d_plot->replot();
}

void Graph::movedPicker(const QPoint &pos, bool mark)
{
	QString info;
	info.sprintf("x=%g; y=%g",
			d_plot->invTransform(QwtPlot::xBottom, pos.x()),
			d_plot->invTransform(QwtPlot::yLeft, pos.y()));
	emit cursorInfo(info);

	if (mark)
	{
		QwtPlotMarker mrk;
		mrk.setSymbol(QwtSymbol(QwtSymbol::Cross,QBrush(Qt::NoBrush),QPen(Qt::red,1),QSize(15,15)));

		QPainter painter(d_plot->canvas());
		painter.setClipping(TRUE);
		QRect cr = d_plot->canvas()->contentsRect();
		painter.setClipRect(cr);
		// FIXME: next line
		//painter.setRasterOp(Qt::NotXorROP);

		if (translateOn)
		{
			const QwtPlotCurve *c = d_plot->curve(selectedCurve);
			if (!c)
				return;

			if (translationDirection)
				mrk.setValue(c->x(selectedPoint), d_plot->invTransform(QwtPlot::yLeft, pos.y()));
			else
				mrk.setValue(d_plot->invTransform(QwtPlot::xBottom,pos.x()), c->y(selectedPoint));
		}
		else
			mrk.setValue(d_plot->invTransform(QwtPlot::xBottom, pos.x()), 
					d_plot->invTransform(QwtPlot::yLeft, pos.y()));

		mrk.draw(&painter, d_plot->canvasMap(QwtPlot::xBottom), d_plot->canvasMap(QwtPlot::yLeft), cr);
	}
}

bool Graph::pickerActivated()
{
	return pickerEnabled;
}

void Graph::initFonts(const QFont &scaleTitleFnt, const QFont &numbersFnt)
{
	for (int i = 0;i<QwtPlot::axisCnt;i++)
	{
		d_plot->setAxisFont (i,numbersFnt);
		QwtText t = d_plot->axisTitle (i);
		t.setFont (scaleTitleFnt);
		d_plot->setAxisTitle(i, t);
	}
}

void Graph::setAxisFont(int axis,const QFont &fnt)
{
	d_plot->setAxisFont (axis, fnt);
	d_plot->replot();
	emit modifiedGraph();
}

QFont Graph::axisFont(int axis)
{
	return d_plot->axisFont (axis);
}

void Graph::enableAxes(const QStringList& list)
{
	int i;
	for (i = 0;i<QwtPlot::axisCnt;i++)
	{
		bool ok=list[i+1].toInt();
		d_plot->enableAxis(i,ok);
	}

	for (i = 0;i<QwtPlot::axisCnt;i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
			scale->setMargin(0);
	}
	scalePicker->refresh();
}

void Graph::enableAxes(Q3MemArray<bool> axesOn)
{
	int i;
	for (i = 0; i<QwtPlot::axisCnt; i++)
		d_plot->enableAxis(i,axesOn[i]);

	for (i = 0;i<QwtPlot::axisCnt;i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
			scale->setMargin(0);
	}
	scalePicker->refresh();
}

Q3MemArray<bool> Graph::enabledAxes()
{
	Q3MemArray<bool> axesOn(4);
	for (int i = 0; i<QwtPlot::axisCnt; i++)
		axesOn[i]=d_plot->axisEnabled (i);
	return axesOn;
}

Q3ValueList<int> Graph::axesBaseline()
{
	Q3ValueList<int> baselineDist;
	for (int i = 0; i<QwtPlot::axisCnt; i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
			baselineDist << scale->margin();
		else
			baselineDist << 0;
	}
	return baselineDist;
}

void Graph::setAxesBaseline(const Q3ValueList<int> &lst)
{
	for (int i = 0; i<QwtPlot::axisCnt; i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
			scale->setMargin(lst[i]);
	}
}

void Graph::setAxesBaseline(QStringList &lst)
{
	lst.remove(lst.first());
	for (int i = 0; i<QwtPlot::axisCnt; i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
			scale->setMargin((lst[i]).toInt());
	}
}

Q3ValueList<int> Graph::axesType()
{
	return axisType;
}

void Graph::setLabelsNumericFormat(int axis, int format, int prec, const QString& formula)
{	
	axisType[axis] = Numeric;
	axesFormulas[axis] = formula;

	ScaleDraw *sd_old= (ScaleDraw *)d_plot->axisScaleDraw (axis);
	const QwtScaleDiv div = sd_old->scaleDiv ();

	if (format == Plot::Superscripts)
	{
		QwtSupersciptsScaleDraw *sd = new QwtSupersciptsScaleDraw(formula.ascii());
		sd->setLabelFormat('s', prec);
		sd->setScaleDiv(div);
		d_plot->setAxisScaleDraw (axis, sd);	
	}
	else
	{			
		ScaleDraw *sd= new ScaleDraw(formula.ascii());
		sd->setScaleDiv(div);

		if (format == Plot::Automatic)
			sd->setLabelFormat ('g', prec);
		else if (format == Plot::Scientific )
			sd->setLabelFormat ('e', prec);
		else if (format == Plot::Decimal)
			sd->setLabelFormat ('f', prec);

		d_plot->setAxisScaleDraw (axis, sd);	
	}
}

void Graph::setLabelsNumericFormat(int axis, const QStringList& l)
{
	QwtScaleDraw *sd = d_plot->axisScaleDraw (axis);
	if (!sd->hasComponent(QwtAbstractScaleDraw::Labels) ||
			axisType[axis] != Numeric)	return;

	int format=l[2*axis].toInt();
	int prec=l[2*axis+1].toInt();
	setLabelsNumericFormat(axis, format, prec, axesFormulas[axis]);
}

void Graph::setLabelsNumericFormat(const QStringList& l)
{
	for (int axis = 0; axis<4; axis++)
		setLabelsNumericFormat (axis, l);
}

void Graph::setAxesType(const Q3ValueList<int> tl)
{
	axisType = tl;	
}

QString Graph::saveAxesLabelsType()
{
	QString s="AxisType\t";
	for (int i=0; i<4; i++)
	{
		int type = axisType[i];
		s+=QString::number(type);
		if (type == Time || type == Date || type == Txt ||
				type == ColHeader || type == Day || type == Month)
			s += ";" + axesFormatInfo[i];
		s+="\t";
	};

	return s+"\n";
}

QString Graph::saveTicksType()
{
	Q3ValueList<int> ticksTypeList=d_plot->getMajorTicksType();
	QString s="MajorTicks\t";
	int i;
	for (i=0; i<4; i++)
		s+=QString::number(ticksTypeList[i])+"\t";
	s += "\n";

	ticksTypeList=d_plot->getMinorTicksType();
	s += "MinorTicks\t";
	for (i=0; i<4; i++)
		s+=QString::number(ticksTypeList[i])+"\t";

	return s+"\n";
}

QStringList Graph::enabledTickLabels()
{
	QStringList lst;
	for (int axis=0; axis<QwtPlot::axisCnt; axis++)
	{
		const QwtScaleDraw *sd = d_plot->axisScaleDraw (axis);
		lst << QString::number(sd->hasComponent(QwtAbstractScaleDraw::Labels));
	}
	return lst;
}

QString Graph::saveEnabledTickLabels()
{
	QString s="EnabledTickLabels\t";
	for (int axis=0; axis<QwtPlot::axisCnt; axis++)
	{
		const QwtScaleDraw *sd = d_plot->axisScaleDraw (axis);
		s += QString::number(sd->hasComponent(QwtAbstractScaleDraw::Labels))+"\t";
	}
	return s+"\n";
}

QString Graph::saveLabelsFormat()
{
	QString s="LabelsFormat\t";
	for (int axis=0; axis<QwtPlot::axisCnt; axis++)
	{
		s += QString::number(d_plot->axisLabelFormat(axis))+"\t";
		s += QString::number(d_plot->axisLabelPrecision(axis))+"\t";
	}
	return s+"\n";
}

QString Graph::saveAxesBaseline()
{
	QString s="AxesBaseline\t";
	for (int i = 0; i<QwtPlot::axisCnt; i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
			s+= QString::number(scale->margin()) + "\t";
		else
			s+= "0\t";
	}
	return s+"\n";
}

QString Graph::saveLabelsRotation()
{
	QString s="LabelsRotation\t";
	s+=QString::number(labelsRotation(QwtPlot::xBottom))+"\t";
	s+=QString::number(labelsRotation(QwtPlot::xTop))+"\n";
	return s;
}

void Graph::setEnabledTickLabels(const QStringList& labelsOn)
{
	for (int axis=0; axis<QwtPlot::axisCnt; axis++)
	{
		QwtScaleWidget *sc = d_plot->axisWidget(axis);
		if (sc)
		{
			QwtScaleDraw *sd = d_plot->axisScaleDraw (axis);
			sd->enableComponent (QwtAbstractScaleDraw::Labels, labelsOn[axis] == "1");
		}
	}
}

void Graph::setMajorTicksType(const Q3ValueList<int>& lst)
{
	if (d_plot->getMajorTicksType() == lst)
		return;

	for (int i=0;i<(int)lst.count();i++)
	{
		ScaleDraw *sd = (ScaleDraw *)d_plot->axisScaleDraw (i);
		if (lst[i]==ScaleDraw::None || lst[i]==ScaleDraw::In)
			sd->enableComponent (QwtAbstractScaleDraw::Ticks, false);
		else
		{
			sd->enableComponent (QwtAbstractScaleDraw::Ticks);
			sd->setTickLength  	(QwtScaleDiv::MinorTick, d_plot->minorTickLength()); 
			sd->setTickLength  	(QwtScaleDiv::MediumTick, d_plot->minorTickLength());
			sd->setTickLength  	(QwtScaleDiv::MajorTick, d_plot->majorTickLength());
		}
		sd->setMajorTicksStyle((ScaleDraw::TicksStyle)lst[i]);
	}
}

void Graph::setMajorTicksType(const QStringList& lst)
{
	for (int i=0; i<(int)lst.count(); i++)
		d_plot->setMajorTicksType(i, lst[i].toInt());
}

void Graph::setMinorTicksType(const Q3ValueList<int>& lst)
{
	if (d_plot->getMinorTicksType() == lst)
		return;

	for (int i=0;i<(int)lst.count();i++)
		d_plot->setMinorTicksType(i, lst[i]);
}

void Graph::setMinorTicksType(const QStringList& lst)
{
	for (int i=0;i<(int)lst.count();i++)
		d_plot->setMinorTicksType(i,lst[i].toInt());
}

int Graph::minorTickLength()
{
	return d_plot->minorTickLength();
}

int Graph::majorTickLength()
{
	return d_plot->majorTickLength();
}

void Graph::setAxisTicksLength(int axis, int majTicksType, int minTicksType,
		int minLength, int majLength)
{
	QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(axis);
	if (!scale)
		return;

	d_plot->setTickLength(minLength, majLength);	

	ScaleDraw *sd= (ScaleDraw *)d_plot->axisScaleDraw (axis);
	sd->setMajorTicksStyle((ScaleDraw::TicksStyle)majTicksType);	
	sd->setMinorTicksStyle((ScaleDraw::TicksStyle)minTicksType);

	if (majTicksType == ScaleDraw::None && minTicksType == ScaleDraw::None)
		sd->enableComponent (QwtAbstractScaleDraw::Ticks, false);
	else
		sd->enableComponent (QwtAbstractScaleDraw::Ticks);

	sd->setTickLength (QwtScaleDiv::MinorTick, minLength); 
	sd->setTickLength (QwtScaleDiv::MediumTick, minLength);
	sd->setTickLength (QwtScaleDiv::MajorTick, majLength);
}

void Graph::setTicksLength(int minLength, int majLength)
{
	Q3ValueList<int> majTicksType = d_plot->getMajorTicksType();
	Q3ValueList<int> minTicksType = d_plot->getMinorTicksType();

	for (int i=0; i<4; i++)
		setAxisTicksLength (i, majTicksType[i], minTicksType[i], minLength, majLength);
}

void Graph::changeTicksLength(int minLength, int majLength)
{
	if (d_plot->minorTickLength() == minLength &&
			d_plot->majorTickLength() == majLength)
		return;

	setTicksLength(minLength, majLength);

	d_plot->hide();
	for (int i=0; i<4; i++)
	{
		if (d_plot->axisEnabled(i))
		{
			d_plot->enableAxis (i,false);
			d_plot->enableAxis (i,true);
		}
	}
	d_plot->replot();
	d_plot->show();

	emit modifiedGraph();
}

void Graph::showAxis(int axis, int type, const QString& formatInfo, Table *table,
		bool axisOn, int majTicksType, int minTicksType, bool labelsOn, 
		const QColor& c,  int format, int prec, int rotation, int baselineDist,
		const QString& formula)
{
	d_plot->enableAxis(axis, axisOn);
	if (!axisOn)
		return;

	Q3ValueList<int> majTicksTypeList = d_plot->getMajorTicksType();
	Q3ValueList<int> minTicksTypeList = d_plot->getMinorTicksType();

	QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(axis);
	ScaleDraw *sclDraw = (ScaleDraw *)d_plot->axisScaleDraw (axis);	

	if (d_plot->axisEnabled (axis) == axisOn &&
			majTicksTypeList[axis] == majTicksType &&
			minTicksTypeList[axis] == minTicksType &&
			axesColors()[axis] == c.name() &&
			prec == d_plot->axisLabelPrecision (axis) && 
			format == d_plot->axisLabelFormat (axis) &&
			labelsRotation(axis) == rotation &&
			axisType[axis] == type &&
			axesFormatInfo[axis] == formatInfo &&
			axesFormulas[axis] == formula &&
			scale->margin() == baselineDist &&
			sclDraw->hasComponent (QwtAbstractScaleDraw::Labels) == labelsOn)
		return;

	scale->setMargin(baselineDist);	
	QPalette pal = scale->palette();
	if (pal.color(QPalette::Active, QColorGroup::Foreground) != c)
	{
		pal.setColor(QColorGroup::Foreground,c);
		pal.setColor(QColorGroup::Text,c);
		scale->setPalette(pal);
	}

	if (!labelsOn)
		sclDraw->enableComponent (QwtAbstractScaleDraw::Labels, false);
	else
	{
		if (type == Numeric)
			setLabelsNumericFormat(axis, format, prec, formula);
		else if (type == Day)
			setLabelsDayFormat (axis, format);
		else if (type == Month)
			setLabelsMonthFormat (axis, format);
		else if (type == Time || type == Date)
			setLabelsDateTimeFormat (axis, type, formatInfo);
		else	
			setLabelsTextFormat(axis, type, formatInfo, table);

		setAxisLabelRotation(axis, rotation);
	}

	sclDraw = (ScaleDraw *)d_plot->axisScaleDraw (axis);	
	sclDraw->enableComponent (QwtAbstractScaleDraw::Backbone, drawAxesBackbone);

	setAxisTicksLength(axis, majTicksType, minTicksType, 
			d_plot->minorTickLength(), d_plot->majorTickLength());

	if (axisOn && (axis == QwtPlot::xTop || axis == QwtPlot::yRight))
		updateSecondaryAxis(axis);//synchronize scale divisions

	scalePicker->refresh();
	d_plot->updateLayout();	//This is necessary in order to enable/disable tick labels
	scale->repaint();
	d_plot->replot();	
	emit modifiedGraph();
}

void Graph::setLabelsDayFormat(int axis, int format)
{
	axisType[axis] = Day;
	axesFormatInfo[axis] = QString::number(format);

	ScaleDraw *sd_old= (ScaleDraw *)d_plot->axisScaleDraw (axis);
	const QwtScaleDiv div = sd_old->scaleDiv ();

	WeekDayScaleDraw *sd = new WeekDayScaleDraw((WeekDayScaleDraw::NameFormat)format);
	sd->setScaleDiv(div);
	d_plot->setAxisScaleDraw (axis, sd);
}

void Graph::setLabelsMonthFormat(int axis, int format)
{
	axisType[axis] = Month;
	axesFormatInfo[axis] = QString::number(format);

	ScaleDraw *sd_old= (ScaleDraw *)d_plot->axisScaleDraw (axis);
	const QwtScaleDiv div = sd_old->scaleDiv ();

	MonthScaleDraw *sd = new MonthScaleDraw((MonthScaleDraw::NameFormat)format);
	sd->setScaleDiv(div);
	d_plot->setAxisScaleDraw (axis, sd);
}

void Graph::setLabelsTextFormat(int axis, int type, const QString& labelsColName, Table *table)
{
	if (type == Numeric || type == Time || type == Date)
		return;

	QStringList list;
	if (type == Txt)
	{
		if (!table)
			return;

		axesFormatInfo[axis] = labelsColName;
		int r = table->tableRows();
		int col = table->colIndex(labelsColName);

		for (int i=0; i < r; i++)
			list<<table->text(i, col);
	}
	else if (type == ColHeader)
	{
		if (!table)
			return;

		axesFormatInfo[axis] = table->name();
		for (int i=0; i<table->tableCols(); i++)
		{
			if (table->colPlotDesignation(i) == Table::Y)
				list<<table->colLabel(i);
		}
	}
	d_plot->setAxisScaleDraw (axis, new QwtTextScaleDraw(list));
	axisType[axis] = type;
}

void Graph::setLabelsDateTimeFormat(int axis, int type, const QString& formatInfo)
{
	if (type < Time)
		return;

	QStringList list= QStringList::split(";", formatInfo, true);
	if ((int)list.count() < 2 || list[0].isEmpty() || list[1].isEmpty())
		return;

	if (type == Time)
	{
		/*QTime t;
		  if (axis == QwtPlot::xTop &&  axisType[QwtPlot::xBottom] == Time)
		  {
		  TimeScaleDraw *scd = (TimeScaleDraw *)d_plot->axisScaleDraw (QwtPlot::xBottom);	
		  t = QTime::fromString (scd->origin(), Qt::TextDate);
		  }
		  else*/
		QTime t = QTime::fromString (list[0], Qt::TextDate);
		TimeScaleDraw *sd = new TimeScaleDraw (t, list[1]);
		sd->enableComponent (QwtAbstractScaleDraw::Backbone, drawAxesBackbone);
		d_plot->setAxisScaleDraw (axis, sd);	
	}
	else if (type == Date)
	{
		QDate d = QDate::fromString (list[0], Qt::ISODate);
		DateScaleDraw *sd = new DateScaleDraw (d, list[1]);
		sd->enableComponent (QwtAbstractScaleDraw::Backbone, drawAxesBackbone);
		d_plot->setAxisScaleDraw (axis, sd);			
	}	

	axisType[axis] = type;
	axesFormatInfo[axis] = formatInfo;
}

void Graph::setAxisLabelRotation(int axis, int rotation)
{
	if (axis==QwtPlot::xBottom)
	{
		if (rotation > 0)
			d_plot->setAxisLabelAlignment(axis, Qt::AlignRight|Qt::AlignVCenter);
		else if (rotation < 0)
			d_plot->setAxisLabelAlignment(axis, Qt::AlignLeft|Qt::AlignVCenter);
		else if (rotation == 0)
			d_plot->setAxisLabelAlignment(axis, Qt::AlignHCenter|Qt::AlignBottom);
	}
	else if (axis==QwtPlot::xTop)
	{
		if (rotation > 0)
			d_plot->setAxisLabelAlignment(axis, Qt::AlignLeft|Qt::AlignVCenter);
		else if (rotation < 0)
			d_plot->setAxisLabelAlignment(axis, Qt::AlignRight|Qt::AlignVCenter);
		else if (rotation == 0)
			d_plot->setAxisLabelAlignment(axis, Qt::AlignHCenter|Qt::AlignTop);
	}
	d_plot->setAxisLabelRotation (axis, double(rotation));
}

int Graph::labelsRotation(int axis)
{
	ScaleDraw *sclDraw = (ScaleDraw *)d_plot->axisScaleDraw (axis);
	return (int)sclDraw->labelRotation();	
}

void Graph::setYAxisTitleFont(const QFont &fnt)
{
	QwtText t = d_plot->axisTitle (QwtPlot::yLeft);
	t.setFont (fnt);
	d_plot->setAxisTitle (QwtPlot::yLeft, t);
	d_plot->replot();
	emit modifiedGraph();
}

void Graph::setXAxisTitleFont(const QFont &fnt)
{
	QwtText t = d_plot->axisTitle (QwtPlot::xBottom);
	t.setFont (fnt);
	d_plot->setAxisTitle (QwtPlot::xBottom, t);
	d_plot->replot();
	emit modifiedGraph();
}

void Graph::setRightAxisTitleFont(const QFont &fnt)
{
	QwtText t = d_plot->axisTitle (QwtPlot::yRight);
	t.setFont (fnt);
	d_plot->setAxisTitle (QwtPlot::yRight, t);
	d_plot->replot();
	emit modifiedGraph();
}

void Graph::setTopAxisTitleFont(const QFont &fnt)
{
	QwtText t = d_plot->axisTitle (QwtPlot::xTop);
	t.setFont (fnt);
	d_plot->setAxisTitle (QwtPlot::xTop, t);
	d_plot->replot();
	emit modifiedGraph();
}

void Graph::setAxisTitleFont(int axis,const QFont &fnt)
{
	QwtText t = d_plot->axisTitle (axis);
	t.setFont (fnt);
	d_plot->setAxisTitle(axis, t);
	d_plot->replot();
	emit modifiedGraph(); 
}

QFont Graph::axisTitleFont(int axis)
{
	return d_plot->axisTitle(axis).font(); 
}

QColor Graph::axisTitleColor(int axis)
{
	QColor c;
	QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(axis);
	if (scale)
		c = scale->title().color();
	return c; 
}

void Graph::setAxesColors(const QStringList& colors)
{
	for (int i=0;i<4;i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
		{
			QPalette pal =scale->palette();
			pal.setColor(QColorGroup::Foreground,QColor(colors[i]));
			scale->setPalette(pal);
		}
	}
}

QString Graph::saveAxesColors()
{
	QString s="AxesColors\t";
	QStringList colors;
	QPalette pal;
	int i;
	for (i=0;i<4;i++)
		colors<<QColor(Qt::black).name();

	for (i=0;i<4;i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)	
		{
			pal=scale->palette();
			colors[i]=pal.color(QPalette::Active, QColorGroup::Foreground).name();
		}
	}
	s+=colors.join ("\t")+"\n";
	return s;
}

QStringList Graph::axesColors()
{
	QStringList colors;
	QPalette pal;
	int i;
	for (i=0;i<4;i++)
		colors<<QColor(Qt::black).name();

	for (i=0;i<4;i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)	
		{
			pal=scale->palette();
			colors[i]=pal.color(QPalette::Active, QColorGroup::Foreground).name();
		}
	}
	return colors;
}

/*QFont Graph::titleFont()
  {
  return d_plot->title().font();
  }

  QColor Graph::titleColor()
  {
  return d_plot->title().color();
  }*/

void Graph::setTitleColor(const QColor & c)
{
	QwtText t = d_plot->title();
	t.setColor(c);
	d_plot->setTitle (t);
	d_plot->replot();
	emit modifiedGraph(); 
}

void Graph::setTitleAlignment(int align)
{
	QwtText t = d_plot->title();
	t.setRenderFlags(align);
	d_plot->setTitle (t);
	d_plot->replot();
	emit modifiedGraph(); 
}

void Graph::setTitleFont(const QFont &fnt)
{
	QwtText t = d_plot->title();
	t.setFont(fnt);
	d_plot->setTitle (t);
	d_plot->replot();
	emit modifiedGraph(); 
}

void Graph::setYAxisTitle(const QString& text)
{
	d_plot->setAxisTitle(0,text);
	d_plot->replot();
	emit modifiedGraph();
}

void Graph::setXAxisTitle(const QString& text)
{
	d_plot->setAxisTitle(2,text);
	d_plot->replot(); 
	emit modifiedGraph();
}

void Graph::setRightAxisTitle(const QString& text)
{
	d_plot->setAxisTitle(1,text);
	d_plot->replot(); 
	emit modifiedGraph();
}

void Graph::setTopAxisTitle(const QString& text)
{
	d_plot->setAxisTitle(3,text);
	d_plot->replot(); 
	emit modifiedGraph();
}

int Graph::axisTitleAlignment (int axis)
{
	return d_plot->axisTitle(axis).renderFlags();
}

void Graph::setAxesTitlesAlignment(const QStringList& align) 
{
	for (int i=0;i<4;i++)	
	{
		QwtText t = d_plot->axisTitle(i);
		t.setRenderFlags(align[i+1].toInt());
		d_plot->setAxisTitle (i, t);
	}
}

void Graph::setXAxisTitleAlignment(int align) 
{
	QwtText t = d_plot->axisTitle(QwtPlot::xBottom);
	t.setRenderFlags(align);
	d_plot->setAxisTitle (QwtPlot::xBottom, t);

	d_plot->replot(); 
	emit modifiedGraph();	
}

void Graph::setYAxisTitleAlignment(int align) 
{
	QwtText t = d_plot->axisTitle(QwtPlot::yLeft);
	t.setRenderFlags(align);
	d_plot->setAxisTitle (QwtPlot::yLeft, t);

	d_plot->replot(); 
	emit modifiedGraph();	
}

void Graph::setTopAxisTitleAlignment(int align) 
{
	QwtText t = d_plot->axisTitle(QwtPlot::xTop);
	t.setRenderFlags(align);
	d_plot->setAxisTitle (QwtPlot::xTop, t);
	d_plot->replot(); 
	emit modifiedGraph();	
}

void Graph::setRightAxisTitleAlignment(int align) 
{
	QwtText t = d_plot->axisTitle(QwtPlot::yRight);
	t.setRenderFlags(align);
	d_plot->setAxisTitle (QwtPlot::yRight, t);

	d_plot->replot(); 
	emit modifiedGraph();	
}

void Graph::setAxisTitle(int axis, const QString& text)
{
	int a;
	switch (axis)
	{
		case 0:
			a=2;
			break;

		case 1:
			a=0;
			break;

		case 2:
			a=3;
			break;

		case 3:
			a=1;
			break;
	}

	d_plot->setAxisTitle(a,text);
	d_plot->replot();
	emit modifiedGraph(); 
}

void Graph::setGridOptions(const GridOptions& o)
{
	if (grid.majorCol == o.majorCol && grid.majorOnX == o.majorOnX &&
			grid.majorOnY == o.majorOnY && grid.majorStyle == o.majorStyle &&
			grid.majorWidth == o.majorWidth && grid.minorCol == o.minorCol &&
			grid.minorOnX == o.minorOnX && grid.minorOnY == o.minorOnY &&
			grid.minorStyle == o.minorStyle && grid.minorWidth == o.minorWidth &&
			grid.xAxis == o.xAxis && grid.yAxis == o.yAxis &&
			grid.xZeroOn == o.xZeroOn && grid.yZeroOn == o.yZeroOn) return;

	grid=o;

	QColor minColor = color(grid.minorCol);
	QColor majColor = color(grid.majorCol);

	Qt::PenStyle majStyle = getPenStyle(grid.majorStyle);
	Qt::PenStyle minStyle = getPenStyle(grid.minorStyle);

	QPen majPen=QPen (majColor,grid.majorWidth,majStyle);
	d_plot->grid()->setMajPen (majPen);

	QPen minPen=QPen (minColor,grid.minorWidth,minStyle);
	d_plot->grid()->setMinPen(minPen);

	if (grid.majorOnX) d_plot->grid()->enableX (TRUE);
	else if (grid.majorOnX==0) d_plot->grid()->enableX (FALSE);

	if (grid.minorOnX) d_plot->grid()->enableXMin (TRUE);
	else if (grid.minorOnX==0) d_plot->grid()->enableXMin (FALSE);

	if (grid.majorOnY) d_plot->grid()->enableY (TRUE);
	else if (grid.majorOnY==0) d_plot->grid()->enableY (FALSE);

	if (grid.minorOnY) d_plot->grid()->enableYMin (TRUE);
	else d_plot->grid()->enableYMin (FALSE);

	d_plot->grid()->setAxis(grid.xAxis, grid.yAxis);

	if (mrkX<0 && grid.xZeroOn)
	{
		QwtPlotMarker *m = new QwtPlotMarker();
		mrkX = d_plot->insertMarker(m);
		m->setAxis(grid.xAxis, grid.yAxis);
		m->setLineStyle(QwtPlotMarker::VLine);
		m->setValue(0.0,0.0);
		m->setLinePen(QPen(Qt::black, 2,Qt::SolidLine));
	}
	else if (mrkX>=0 && !grid.xZeroOn)
	{
		d_plot->removeMarker(mrkX);
		mrkX=-1;
	}

	if (mrkY<0 && grid.yZeroOn)
	{
		QwtPlotMarker *m = new QwtPlotMarker();
		mrkY = d_plot->insertMarker(m);
		m->setAxis(grid.xAxis, grid.yAxis);
		m->setLineStyle(QwtPlotMarker::HLine);
		m->setValue(0.0,0.0);
		m->setLinePen(QPen(Qt::black, 2,Qt::SolidLine));
	}
	else if (mrkY>=0 && !grid.yZeroOn)
	{
		d_plot->removeMarker(mrkY);
		mrkY=-1;		
	}

	emit modifiedGraph();
}

GridOptions Graph::getGridOptions()
{
	return grid;
}

QStringList Graph::scalesTitles()
{
	QStringList scaleTitles;
	int axis;
	for (int i=0;i<QwtPlot::axisCnt;i++)
	{
		switch (i)
		{
			case 0:
				axis=2;
				scaleTitles<<d_plot->axisTitle(axis).text();
				break;

			case 1:
				axis=0;
				scaleTitles<<d_plot->axisTitle(axis).text();
				break;

			case 2:
				axis=3;
				scaleTitles<<d_plot->axisTitle(axis).text();
				break;

			case 3:
				axis=1;
				scaleTitles<<d_plot->axisTitle(axis).text();
				break;
		}
	}
	return scaleTitles;
}

void Graph::updateSecondaryAxis(int axis)
{
	for (int i=0; i<n_curves; i++)
	{
		QwtPlotCurve *c = this->curve(i);
		if (!c)
			continue;
		if ((axis == QwtPlot::yRight && c->yAxis () == QwtPlot::yRight) ||
				(axis == QwtPlot::xTop && c->xAxis () == QwtPlot::xTop))
			return;
	} 

	int a = QwtPlot::xBottom;
	if (axis == QwtPlot::yRight)
		a = QwtPlot::yLeft;

	if (!d_plot->axisEnabled(a))
		return;

	QwtScaleEngine *se = d_plot->axisScaleEngine(a);
	const QwtScaleDiv *sd = d_plot->axisScaleDiv(a);

	QwtScaleEngine *sc_engine = 0;
	if (se->transformation()->type() == QwtScaleTransformation::Log10)
		sc_engine = new QwtLog10ScaleEngine();
	else if (se->transformation()->type() == QwtScaleTransformation::Linear)
		sc_engine = new QwtLinearScaleEngine();

	if (se->testAttribute(QwtScaleEngine::Inverted))
		sc_engine->setAttribute(QwtScaleEngine::Inverted);

	d_plot->setAxisScaleEngine (axis, sc_engine);
	d_plot->setAxisScaleDiv (axis, *sd);

	d_user_step[axis] = d_user_step[a];

	QwtScaleWidget *scale = d_plot->axisWidget(a);
	int start = scale->startBorderDist();
	int end = scale->endBorderDist();

	scale = d_plot->axisWidget(axis);
	scale->setMinBorderDist (start, end);
}

void Graph::setAutoScale()
{
	for (int i = 0; i < QwtPlot::axisCnt; i++)
		d_plot->setAxisAutoScale(i);

	d_plot->replot();
	d_zoomer[0]->setZoomBase();
	d_zoomer[1]->setZoomBase();
	updateScale();

	emit modifiedGraph();
}

void Graph::setScale(int axis, double start, double step, double end, int majorTicks, int minorTicks, int type, bool inverted)
{
	QwtScaleEngine *sc_engine = 0;
	if (type)
		sc_engine = new QwtLog10ScaleEngine();
	else
		sc_engine = new QwtLinearScaleEngine();

	QwtScaleDiv div = sc_engine->divideScale (QMIN(start, end), QMAX(start, end), majorTicks, minorTicks, step);
	d_plot->setAxisMaxMajor (axis, majorTicks);
	d_plot->setAxisMaxMinor (axis, minorTicks);

	if (inverted)
	{
		sc_engine->setAttribute(QwtScaleEngine::Inverted);
		div.invert();
	}

	d_plot->setAxisScaleEngine (axis, sc_engine);
	d_plot->setAxisScaleDiv (axis, div);

	d_zoomer[0]->setZoomBase();
	d_zoomer[1]->setZoomBase();

	d_user_step[axis] = (step != 0.0);

	d_plot->replot();
	//keep markers on canvas area
	updateMarkersBoundingRect();
}

void Graph::copyCanvas(bool on)
{
	int w=d_plot->canvas()->width();
	int h=d_plot->canvas()->height();
	if (on)
	{
		QPixmap pix(w,h);
		pix=pix.grabWidget(d_plot->canvas(),0,0,-1,-1 );
		QPixmapCache::insert ("field",pix);
	}
}

void Graph::shiftCurveSelector(bool up)
{
	const Q3ValueList<int> keys = d_plot->curveKeys();

	int index = 0;
	if ( selectedCurve >= 0 )
	{
		for ( int i = 0; i < keys.count() - 1; i++ )
		{
			if ( selectedCurve == keys[(int)i] )
			{
				index = i + (up ? 1 : -1);
				break;
			}
		}
	}

	index = (keys.count() + index) % keys.count();
	if ( selectedCurve != keys[index] )
		selectedCurve = keys[index];

	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve || curve->dataSize()<=0)
		return;


	startPoint=0;
	endPoint=curve->dataSize()-1;

	QwtPlotMarker *m = d_plot->marker(startID);
	m->setValue (curve->x(startPoint),curve->y(startPoint)); 
	m = d_plot->marker(endID);
	m->setValue(curve->x(endPoint),curve->y(endPoint));
	d_plot->replot();

	QString info;		
	if ( selectedCursor == startID )
	{
		info = tr("Left") + " <=> ";
		selectedPoint=startPoint;
	}
	else if ( selectedCursor == endID )
	{
		info = tr("Right") + " <=> ";
		selectedPoint=endPoint;
	}

	info+=curve->title().text(); 
	info+="[";
	info+=QString::number(selectedPoint);
	info+="]: x=";
	info+=QString::number(curve->x(selectedPoint), 'G', 15);
	info+="; y=";
	info+=QString::number(curve->y(selectedPoint), 'G', 15);

	emit cursorInfo(info);
	emit dataRangeChanged();

	QPainter painter(d_plot->canvas());
	painter.setClipping(TRUE);
	painter.setClipRect(d_plot->canvas()->contentsRect());
	// FIXME: next line
	//painter.setRasterOp(Qt::NotROP);

	curve->draw(&painter,
			d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
			selectedPoint,selectedPoint);
}

void Graph::shiftRangeSelector(bool shift)
{		
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve || curve->dataSize()<=0)
		return;

	QString info;	
	if (shift)
	{
		QwtPlotMarker *m = d_plot->marker(selectedCursor);
		QwtSymbol symbol = m->symbol();
		if ( selectedCursor == startID )
		{
			symbol.setPen(QPen(Qt::black,2,Qt::SolidLine));
			m->setSymbol (symbol);
			m->setLinePen(QPen(Qt::black,1,Qt::DashLine));

			selectedCursor = endID;
			m = d_plot->marker(selectedCursor);
			symbol = m->symbol();
			symbol.setPen(QPen(Qt::red,2,Qt::SolidLine));
			m->setSymbol (symbol);
			m->setLinePen(QPen(Qt::red,1,Qt::DashLine));

			selectedPoint = endPoint;			
			info = tr("Right") + " <=> ";
		}
		else if ( selectedCursor == endID )
		{
			symbol.setPen(QPen(Qt::black,2,Qt::SolidLine));
			m->setSymbol (symbol);
			m->setLinePen(QPen(Qt::black,1,Qt::DashLine));

			selectedCursor = startID;
			m = d_plot->marker(selectedCursor);
			symbol = m->symbol();
			symbol.setPen(QPen(Qt::red,2,Qt::SolidLine));
			m->setSymbol (symbol);
			m->setLinePen(QPen(Qt::red,1,Qt::DashLine));

			selectedPoint = startPoint;			
			info = tr("Left") + " <=> ";
		}
		d_plot->replot();

		info+=curve->title().text(); 
		info+="[";
		info+=QString::number(selectedPoint+1);
		info+="]: x=";
		info+=QString::number(curve->x(selectedPoint), 'G', 15);
		info+="; y=";
		info+=QString::number(curve->y(selectedPoint), 'G', 15);

		emit cursorInfo(info);

		QPainter painter(d_plot->canvas());
		painter.setClipping(TRUE);
		painter.setClipRect(d_plot->canvas()->contentsRect());
		// FIXME: next line
		//painter.setRasterOp(Qt::NotROP);

		curve->draw(&painter, d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
				selectedPoint,selectedPoint);
	}
}

void Graph::moveRangeSelector(bool up)
{	
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	int points=curve->dataSize();
	if (points == 0)
	{
		QMessageBox::critical(this, tr("QtiPlot - Warning"),
				tr("All the curves on this plot are empty!"));
		disableRangeSelectors();
		return;
	}

	int index = selectedPoint + (up ? 1 : -1);
	index = (index + points) % points;

	QString info;
	if ( index != selectedPoint )
		selectedPoint = index;

	QwtPlotMarker *m = d_plot->marker(selectedCursor);
	m->setValue(curve->x(selectedPoint),curve->y(selectedPoint));
	d_plot->replot();

	if ( selectedCursor == startID )
	{
		startPoint = selectedPoint;
		info = tr("Left") + " <=> ";
	}
	else if ( selectedCursor == endID )
	{
		endPoint = selectedPoint;
		info = tr("Right") + " <=> ";
	}

	info+=curve->title().text();
	info+="[";
	info+=QString::number(index+1);
	info+="]: x=";
	info+=QString::number(curve->x(index), 'G', 15);
	info+="; y=";
	info+=QString::number(curve->y(index), 'G', 15);

	emit cursorInfo(info);
	emit dataRangeChanged();

	QPainter painter(d_plot->canvas());
	painter.setClipping(TRUE);
	painter.setClipRect(d_plot->canvas()->contentsRect());
	// FIXME: next line
	//painter.setRasterOp(Qt::NotROP);

	curve->draw(&painter, d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
			selectedPoint,selectedPoint);
}

//places the active range selector at the selected point
void Graph::moveRangeSelector()
{	
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	int points=curve->dataSize();
	if (!points)
	{
		QMessageBox::critical(this, tr("QtiPlot - Warning"),
				tr("All the curves on this plot are empty!"));
		disableRangeSelectors();
		return;
	}

	int index = selectedPoint;
	index = (index + points) % points;

	QString info;
	if ( index != selectedPoint )
		selectedPoint = index;

	QwtPlotMarker *m = d_plot->marker(selectedCursor);
	m->setValue(curve->x(selectedPoint),curve->y(selectedPoint));

	d_plot->replot();

	if ( selectedCursor == startID )
	{
		startPoint = selectedPoint;
		info = tr("Left") + " <=> ";
	}
	else if ( selectedCursor == endID )
	{
		endPoint = selectedPoint;
		info = tr("Right") + " <=> ";
	}

	info+=curve->title().text(); 
	info+="[";
	info+=QString::number(index+1);
	info+="]: x=";
	info+=QString::number(curve->x(index), 'G', 15);
	info+="; y=";
	info+=QString::number(curve->y(index), 'G', 15);

	emit cursorInfo(info);
	emit dataRangeChanged();

	QPainter painter(d_plot->canvas());
	painter.setClipping(TRUE);
	painter.setClipRect(d_plot->canvas()->contentsRect());
	// FIXME: next line
	//painter.setRasterOp(Qt::NotROP);

	curve->draw(&painter,d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
			selectedPoint,selectedPoint);
}

bool Graph::enableRangeSelectors(bool on)
{
	rangeSelectorsEnabled=on;

	long curveID;
	QwtPlotCurve *curve=0;
	bool success=FALSE;	
	for (int i=n_curves-1;i>=0;i--)
	{
		curveID= c_keys[i]; 
		curve=d_plot->curve(curveID);
		if (curve && curve->dataSize()>0)
		{
			success=TRUE;
			break;
		}			
	}

	if (!success)
	{
		QMessageBox::critical(this, tr("QtiPlot - Warning"),
				tr("All the curves on this plot are empty!"));

		disableRangeSelectors();
		return FALSE;
	}

	startPoint=0;

	if (on)
	{	
		QCursor cursor=QCursor (QPixmap(vizor_xpm),-1,-1);
		d_plot->canvas()->setCursor(cursor);

		endPoint=curve->dataSize()-1;
		selectedCurve=curveID;
		selectedPoint=0;
		d_plot->canvas()->setFocus();

		int d=32;
		QwtPlotCurve *c = d_plot->curve(selectedCurve);
		QwtSymbol symbol = c->symbol();
		if (symbol.style() != QwtSymbol::None)
		{
			QSize sz=symbol.size();
			d+=QMAX(sz.width(),sz.height());
		}

		QwtPlotMarker *m = new QwtPlotMarker();
		m->setSymbol(QwtSymbol(QwtSymbol::Cross, QBrush(Qt::NoBrush), QPen(Qt::red,2), QSize(d,d)));
		m->setLineStyle(QwtPlotMarker::VLine);
		m->setLinePen(QPen(Qt::red,1,Qt::DashLine));
		m->setValue (curve->x(0),curve->y(0));
		startID=d_plot->insertMarker(m);

		selectedCursor=startID;

		m = new QwtPlotMarker();
		m->setLineStyle(QwtPlotMarker::VLine);
		m->setLinePen(QPen(Qt::black,1,Qt::DashLine));
		m->setSymbol(QwtSymbol(QwtSymbol::Cross, QBrush(Qt::NoBrush), QPen(Qt::black,2), QSize(d,d)));	
		m->setValue(curve->x(endPoint),curve->y(endPoint));
		endID=d_plot->insertMarker(m);

		d_plot->replot();

		QPainter painter(d_plot->canvas());

		painter.setClipping(TRUE);
		painter.setClipRect(d_plot->canvas()->contentsRect());
		// FIXME: next line
		//painter.setRasterOp(Qt::NotROP);

		curve->draw(&painter,
				d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),0,0);

		emit dataRangeChanged();
	}
	else
		disableRangeSelectors();	

	return TRUE;
}

void  Graph::disableRangeSelectors()
{
	d_plot->removeMarker(startID);
	d_plot->removeMarker(endID);
	d_plot->replot();
	startID=-1;
	endID=-1;
	startPoint=0;
	endPoint=-1;
	selectedCurve=-1;
	selectedPoint=-1;
	selectedCursor=-1;	
	rangeSelectorsEnabled=FALSE;	
	d_plot->canvas()->setCursor(Qt::arrowCursor);
}

void Graph::showPlotPicker(bool on)
{
	pickerEnabled = on;
	QCursor cursor=QCursor (QPixmap(cursor_xpm),-1,-1);
	if (on)
		d_plot->canvas()->setCursor(cursor);
	else 
		d_plot->canvas()->setCursor(Qt::arrowCursor);
}

void Graph::startCurveTranslation()
{
	showPlotPicker(true);
	emit cursorInfo(tr("Curve selected! Move cursor and click to choose a point and double-click/press 'Enter' to finish!"));
}

void Graph::insertPlottedList(const QStringList& names)
{
	Q3ValueList<int> keys = d_plot->curveKeys();
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotCurve *c = d_plot->curve(keys[i]);
		if (c)
			c->setTitle(names[i]);
	}
}

QStringList Graph::curvesList()
{	
	QStringList cList;
	Q3ValueList<int> keys = d_plot->curveKeys();
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotCurve *c = d_plot->curve(keys[i]);
		if (c)
			cList << c->title().text();
	}	
	return cList;
}

QStringList Graph::plotAssociations()
{
	return associations;
}

void Graph::setPlotAssociations(const QStringList& newList)
{
	associations=newList;
}

void Graph::copyImage()
{
	QPixmap pic = graphPixmap();
	QImage image = pic.convertToImage(); 	
	QApplication::clipboard()->setData( new Q3ImageDrag (image,d_plot,0) );	
}

QPixmap Graph::graphPixmap()
{
	int lw = d_plot->lineWidth();
	int clw = 2*d_plot->canvas()->lineWidth();

	QPixmap pic(d_plot->width() + 2*lw + clw, d_plot->height() + 2*lw + clw);
	pic.fill (QColor(255, 255, 255));
	QPainter paint;
	paint.begin(&pic);

	QRect rect = QRect(lw, lw, d_plot->width() - 2*lw, d_plot->height() - 2*lw);

	QwtPlotLayout *layout= d_plot->plotLayout ();
	layout->activate(d_plot, rect, 0);

	QwtPlotPrintFilter  filter; 
	filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle |
			QwtPlotPrintFilter::PrintCanvasBackground);

	d_plot->print(&paint, rect, filter);
	paint.end();

	//the initial layout is invalidated during the print operation and must be recalculated	
	layout->activate(d_plot, d_plot->rect(), 0);

	return pic;
}

void Graph::exportImage(const QString& fileName, const QString& fileType,
		int quality, bool transparent)
{
	QPixmap pic=graphPixmap();

	if (transparent)
	{//save transparency
		QBitmap mask(pic.size());
		mask.fill(Qt::color1);
		QPainter p;
		p.begin(&mask);
		p.setPen(Qt::color0);

		QColor background = QColor (QColor(255, 255, 255));
		QRgb backgroundPixel = background.rgb ();
		QImage image = pic.convertToImage();
		for (int y=0; y<image.height(); y++)
		{
			for ( int x=0; x<image.width(); x++ )
			{
				QRgb rgb = image.pixel(x, y);
				if (rgb == backgroundPixel) // we want the frame transparent
				{
					p.drawPoint( x, y );
				}
			}
		}
		p.end();	
		pic.setMask(mask);
	}

	pic.save(fileName, fileType, quality);	
}

void Graph::exportToEPS(const QString& fname)
{	
	QPrinter printer;
	printer.setPageSize (QPrinter::A4);
	printer.setColorMode (QPrinter::Color);
	printer.setFullPage(TRUE);
	printer.setOutputToFile(TRUE);
	printer.setOutputFileName(fname);

	QwtPlotPrintFilter  filter; 
	filter.setOptions(QwtPlotPrintFilter::PrintAll |QwtPlotPrintFilter::PrintTitle |
			~QwtPlotPrintFilter::PrintCanvasBackground);

	// export should preserve plot aspect ratio, if possible
	double aspect = double(d_plot->frameGeometry().width())/double(d_plot->frameGeometry().height());	
	if (aspect < 1)
		printer.setOrientation(QPrinter::Portrait);
	else
		printer.setOrientation(QPrinter::Landscape);

	Q3PaintDeviceMetrics mpr(&printer);	
	int dpiy = mpr.logicalDpiY();
	int margin = (int)((0.5/2.54)*dpiy); // 5 mm margins
	int width = int(aspect*mpr.height());	
	int x=qRound(abs(mpr.width()-width)*0.5);
	QRect rect(x, margin, width, mpr.height() - 2*margin);
	if (width > mpr.width())
	{
		rect.setLeft(margin);
		rect.setWidth(mpr.width() - 2*margin);
	}
	QPainter paint(&printer);	
	d_plot->print(&paint, rect, filter);
}

void Graph::exportToEPS(const QString& fname, int res, QPrinter::Orientation o, 
		QPrinter::PageSize size, QPrinter::ColorMode col)
{	
	QPrinter printer;
	printer.setPageSize (size);
	printer.setColorMode (col);
	printer.setOrientation(o);
	printer.setFullPage(TRUE);
	printer.setOutputToFile(TRUE);
	printer.setOutputFileName(fname);

	QwtPlotPrintFilter  filter; 
	filter.setOptions(QwtPlotPrintFilter::PrintAll |QwtPlotPrintFilter::PrintTitle |
			~QwtPlotPrintFilter::PrintCanvasBackground);

	// export should preserve plot aspect ratio, if possible
	double aspect = double(d_plot->frameGeometry().width())/double(d_plot->frameGeometry().height());

	Q3PaintDeviceMetrics mpr(&printer);	
	int dpiy = mpr.logicalDpiY();
	int margin = (int) ( (0.5/2.54)*dpiy ); // 5 mm margins
	int width = int(aspect*mpr.height());	
	int x=qRound(abs(mpr.width()-width)*0.5);
	QRect rect(x, margin, width, mpr.height() - 2*margin);
	if (width > mpr.width())
	{
		rect.setLeft(margin);
		rect.setWidth(mpr.width() - 2*margin);
	}

	QPainter paint(&printer);
	d_plot->print(&paint, rect, filter);
}

void Graph::print()
{
	QPrinter printer;
	printer.setColorMode (QPrinter::Color);
	printer.setFullPage(TRUE);

	//printing should preserve plot aspect ratio, if possible
	double aspect = double(d_plot->width())/double(d_plot->height());
	if (aspect < 1)
		printer.setOrientation(QPrinter::Portrait);
	else
		printer.setOrientation(QPrinter::Landscape);

	if (printer.setup())
	{
		Q3PaintDeviceMetrics mpr(&printer);	
		int dpiy = mpr.logicalDpiY();
		int margin = (int) ((2/2.54)*dpiy ); // 2 cm margins

		int width = qRound(aspect*mpr.height()) - 2*margin;	
		int x=qRound(abs(mpr.width()- width)*0.5);

		QRect rect(x, margin, width, mpr.height() - 2*margin);
		if (x < margin)
		{
			rect.setLeft(margin);
			rect.setWidth(mpr.width() - 2*margin);
		}
		QPainter paint(&printer);
		QwtPlotPrintFilter filter;
		filter.setOptions(QwtPlotPrintFilter::PrintAll | ~QwtPlotPrintFilter::PrintCanvasBackground);
		d_plot->print(&paint, rect, filter);
	}
}

void Graph::exportToWmf(const QString& fname) 
{// FIXME: not working properly
	//using Tobias Burnus's qwmf

	/*PrintFilter filter(d_plot); 	
	  filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle |
	  QwtPlotPrintFilter::PrintCanvasBackground);

	  QWMF m_wmf ;
	  m_wmf.setOutputFileName(fname);

	  QPainter painter( &m_wmf );	
	  d_plot->print(&painter , d_plot->rect(), filter);
	  painter.end();*/
}

void Graph::exportToSVG(const QString& fname) 
{
	// TODO: check if the next 2 lines can be removed
	// enable workaround for Qt3 misalignments
	// QwtPainter::setSVGMode(true); 

	Q3Picture picture;
	QPainter p(&picture);
	d_plot->print(&p, d_plot->rect());
	p.end();

	picture.save(fname, "svg");
}

void Graph::movePoints(bool enabled)
{
	movePointsEnabled=enabled;
	if (enabled)
		d_plot->canvas()->setCursor(Qt::pointingHandCursor);
	else 
		d_plot->canvas()->setCursor(Qt::arrowCursor);
}

bool Graph::movePointsActivated()
{
	return movePointsEnabled;
}

void Graph::removePoints(bool enabled)
{
	removePointsEnabled=enabled;
	QCursor cursor=QCursor (QPixmap(vizor_xpm),-1,-1);

	if (enabled)
		d_plot->canvas()->setCursor(cursor);
	else 
		d_plot->canvas()->setCursor(Qt::arrowCursor);
}

bool Graph::removePointActivated()
{
	return removePointsEnabled;
}

void Graph::translateCurve(int direction)
{
	translateOn=true;
	translationDirection = direction;
	d_plot->canvas()->setCursor(QCursor (QPixmap(vizor_xpm),-1,-1));
}

void Graph::translateCurveTo(const QPoint& p)
{
	QwtPlotCurve *c = d_plot->curve(selectedCurve);
	if (!c)
		return;

	QStringList ass=QStringList::split(",",associations[curveIndex(selectedCurve)],false);

	int n=c->dataSize();
	double *dat= new double[n];
	if (translationDirection)
	{
		double dy= d_plot->invTransform(QwtPlot::yLeft,p.y()) - c->y(selectedPoint);
		for (int i=0; i<n; i++)
			dat[i]=c->y(i)+dy;
		emit updateTableColumn(ass[1].remove("(Y)"), dat, n);
	}
	else
	{
		double dx= d_plot->invTransform(QwtPlot::xBottom,p.x()) - c->x(selectedPoint);
		for (int i=0; i<n; i++)
			dat[i]=c->x(i)+dx;
		emit updateTableColumn(ass[0].remove("(X)"), dat, n);
	}	
	showPlotPicker(false);
	translateOn=false;

	//no need for setCurveData() or replot(), since the main application will call
	//updateCurves() which does this job!
}

QString Graph::selectedCurveTitle()
{
	const QwtPlotCurve *c = d_plot->curve(selectedCurve);
	if (c)
		return c->title().text();
	else
		return QString::null;
}

void Graph::multiPeakFit(ApplicationWindow *app, int profile, int peaks)
{
	showPlotPicker(true);
	selected_peaks = 0;
	fitter = new MultiPeakFit(app, this, (MultiPeakFit::PeakProfile)profile, peaks);
	fitter->enablePeakCurves(app->generatePeakCurves);
	fitter->setPeakCurvesColor(app->peakCurvesColor);
	fitter->setFitCurveParameters(app->generateUniformFitPoints, app->fitPoints);
	d_plot->canvas()->grabMouse();
}

bool Graph::selectPeaksOn()
{
	if (fitter)
		return true;
	else
		return false;
}

void Graph::selectPeak(const QPoint &)
{
	const QwtPlotCurve *c = d_plot->curve(selectedCurve);
	if (!c)
		return;

	fitter->setInitialGuess(3*selected_peaks, c->y(selectedPoint));
	fitter->setInitialGuess(3*selected_peaks+1, c->x(selectedPoint));

	QwtPlotMarker *m = new QwtPlotMarker();
	m->setLineStyle(QwtPlotMarker::VLine);
	m->setLinePen(QPen(Qt::green, 2, Qt::DashLine));
	m->setXValue(c->x(selectedPoint));
	d_plot->insertMarker(m);
	d_plot->replot();

	selected_peaks++;
	int peaks = fitter->peaks();
	if (selected_peaks == peaks)
	{
		showPlotPicker(false);
		d_plot->canvas()->releaseMouse();

		if (fitter->setDataFromCurve(c->title().text()))
		{
			QApplication::setOverrideCursor(Qt::WaitCursor);
			fitter->fit();
			delete fitter;
			QApplication::restoreOverrideCursor();
		}

		//remove peak line markers
		Q3ValueList<int>mrks = d_plot->markerKeys();
		int n=(int)mrks.count();
		for (int i=0; i<peaks; i++)
			d_plot->removeMarker(mrks[n-i-1]);

		d_plot->replot();
		return;
	}
	emit cursorInfo(tr("Peak %1 selected! Click to select a point and double-click/press 'Enter' to set the position of the next peak!").arg(QString::number(selected_peaks)));
}

bool Graph::selectPoint(const QPoint &pos)
{
	int dist, point;
	const int curve = d_plot->closestCurve(pos.x(), pos.y(), dist, point);
	if (curve >= 0 && dist < 5)//5 pixels tolerance
	{
		const QwtPlotCurve *c = d_plot->curve(curve);
		if (!c)
			return false;

		if (rangeSelectorsEnabled && selectedCurve != curve)
		{			
			if ( selectedCursor == startID )
			{
				startPoint=point;
				int n=c->dataSize()-1;				
				endPoint=n;
				if (point ==n)
					endPoint=0;		
			}
			else if ( selectedCursor == endID )
			{
				startPoint=0;
				endPoint=point;
				if (!point)
					startPoint=c->dataSize()-1;		
			}

			QwtPlotMarker *m = d_plot->marker(startID);
			if (m)
				m->setValue(c->x(startPoint),c->y(startPoint));

			m = d_plot->marker(endID);
			if (m)
				m->setValue(c->x(endPoint),c->y(endPoint));
		}

		selectedCurve = curve;
		selectedPoint = point;
		highlightPoint(TRUE);

		QString info;
		info=c->title().text();
		info+="[";
		info+=QString::number(point+1);
		info+="]: x=";
		info+=QString::number(c->x(point), 'G', 15);
		info+="; y=";
		info+=QString::number(c->y(point), 'G', 15);

		emit cursorInfo(info);
		return true;
	}
	else if (!rangeSelectorsEnabled && !translateOn)
	{// deselect
		selectedCurve = -1;
		selectedPoint = -1;
		highlightPoint(FALSE);
		return false;
	}
	return false;
}

void Graph::selectCurve(const QPoint &pos)
{
	int dist, point;
	const int curve = d_plot->closestCurve(pos.x(), pos.y(), dist, point);
	if ( curve >= 0 && dist < 10 ) // 10 pixels tolerance
	{
		selectedCurve = curve;
		selectedPoint = point;
		showCursor(TRUE);

		const QwtPlotCurve *c = d_plot->curve(selectedCurve);
		QString info;
		info=c->title().text();
		info+="[";
		info+=QString::number(point+1);
		info+="]: x=";
		info+=QString::number(c->x(point), 'G', 15);
		info+="; y=";
		info+=QString::number(c->y(point), 'G', 15);

		emit cursorInfo(info);
	}
	else // deselect
	{
		showCursor(FALSE);
		selectedCurve = -1;
		selectedPoint = -1;
	}
}

bool Graph::markerSelected()
{
	bool selected=FALSE;
	if (selectedMarker>=0)
		selected=TRUE;
	return  selected;
}

void Graph::removeMarker()
{
	if (selectedMarker>=0)
	{
		d_plot->removeMarker(selectedMarker);
		d_plot->replot();
		emit modifiedGraph();

		if (selectedMarker==legendMarkerID)
			legendMarkerID=-1;

		if (d_lines.contains(selectedMarker)>0)
		{
			int i,index=d_lines.find(selectedMarker,0);
			for (i=index;i<linesOnPlot;i++)
				d_lines[i]=d_lines[i+1];
			linesOnPlot--;
			d_lines.resize(linesOnPlot);
		}

		else if (d_images.contains(selectedMarker)>0)
		{
			int i,index=d_images.find(selectedMarker,0);
			int imagesOnPlot=d_images.size();

			for (i=index;i<imagesOnPlot;i++)
				d_images[i]=d_images[i+1];
			imagesOnPlot--;
			d_images.resize(imagesOnPlot);
		}

		selectedMarker=-1;
	}
}

void Graph::cutMarker()
{
	copyMarker();
	removeMarker();
}

bool Graph::arrowMarkerSelected()
{
	bool arrow=FALSE;
	if (d_lines.contains(selectedMarker)>0)
		arrow=TRUE;
	return arrow;
}

bool Graph::imageMarkerSelected()
{
	bool image=FALSE;
	if (d_images.contains(selectedMarker)>0)
		image=TRUE;
	return image;
}

void Graph::copyMarker()
{
	if (selectedMarker<0)
	{
		selectedMarkerType=None;
		return ;
	}

	if (d_lines.contains(selectedMarker))
	{
		LineMarker* mrkL=(LineMarker*) d_plot->marker(selectedMarker);
		auxMrkStart=mrkL->startPoint();
		auxMrkEnd=mrkL->endPoint();
		selectedMarkerType=Arrow;
	}
	else if (d_images.contains(selectedMarker))
	{
		ImageMarker* mrkI=(ImageMarker*) d_plot->marker(selectedMarker);
		auxMrkStart=mrkI->getOrigin();
		QRect rect=mrkI->rect();
		auxMrkEnd=rect.bottomRight();
		auxMrkFileName=mrkI->getFileName();
		selectedMarkerType=Image;
	}
	else
		selectedMarkerType=Text;
}

void Graph::pasteMarker()
{
	if (selectedMarkerType==Arrow)
	{
		LineMarker* mrkL=new LineMarker();
		mrkL->setColor(auxMrkColor);
		mrkL->setWidth(auxMrkWidth);
		mrkL->setStyle(auxMrkStyle);
		mrkL->setStartPoint(QPoint(auxMrkStart.x()+10,auxMrkStart.y()+10));
		mrkL->setEndPoint(QPoint(auxMrkEnd.x()+10,auxMrkEnd.y()+10));
		mrkL->drawStartArrow(startArrowOn);
		mrkL->drawEndArrow(endArrowOn);
		mrkL->setHeadLength(auxArrowHeadLength);
		mrkL->setHeadAngle(auxArrowHeadAngle);
		mrkL->fillArrowHead(auxFilledArrowHead);
		long mrkID=d_plot->insertMarker(mrkL);
		linesOnPlot++;
		d_lines.resize(linesOnPlot);
		d_lines[linesOnPlot-1]=mrkID;
		d_plot->replot();

		selectedMarker=-1;
	}
	else if (selectedMarkerType==Image)
	{
		QString fn=auxMrkFileName;
		QRect rect=QRect(auxMrkStart,auxMrkEnd);

		QPixmap photo;
		QFileInfo fi(fn);
		QString baseName = fi.fileName();
		int pos=baseName.find(".",0);
		QString type=baseName.right(baseName.length()-pos-1);

		if (type.upper()=="JPG")
			photo.load(fn,"JPEG",QPixmap::Auto);
		else
			photo.load(fn,type.upper(),QPixmap::Auto);	

		ImageMarker* mrk= new ImageMarker(photo);
		long mrkID=d_plot->insertMarker(mrk);
		mrk->setFileName(fn);

		QPoint o=d_plot->canvas()->mapFromGlobal(QCursor::pos());
		if (!d_plot->canvas()->contentsRect().contains(o))
			o=QPoint(auxMrkStart.x()+20,auxMrkStart.y()+20);

		mrk->setOrigin(o);
		mrk->setSize(rect.size());
		d_plot->replot();

		int imagesOnPlot=d_images.size();
		imagesOnPlot++;
		d_images.resize(imagesOnPlot);
		d_images[imagesOnPlot-1]=mrkID;

		selectedMarker=-1;
	}
	else
	{	
		LegendMarker* mrk=new LegendMarker(d_plot);
		QPoint o=d_plot->canvas()->mapFromGlobal(QCursor::pos());

		if (!d_plot->canvas()->contentsRect().contains(o))
			o=QPoint(auxMrkStart.x()+20,auxMrkStart.y()+20);

		mrk->setOrigin(o);
		mrk->setAngle(auxMrkAngle);
		mrk->setBackground(auxMrkBkg);
		mrk->setFont(auxMrkFont);
		mrk->setText(auxMrkText);
		mrk->setTextColor(auxMrkColor);
		mrk->setBackgroundColor(auxMrkBkgColor);
		d_plot->insertMarker(mrk);
		d_plot->replot();

		selectedMarker=-1;
	}
}

void Graph::setCopiedMarkerEnds(const QPoint& start, const QPoint& end)
{
	auxMrkStart=start;
	auxMrkEnd=end;
}

void Graph::setCopiedTextOptions(int bkg, const QString& text, const QFont& font, 
		const QColor& color, const QColor& bkgColor)
{
	auxMrkBkg=bkg;
	auxMrkText=text;
	auxMrkFont=font;
	auxMrkColor=color;
	auxMrkBkgColor = bkgColor;
}

void Graph::setCopiedArrowOptions(int width, Qt::PenStyle style, const QColor& color,
		bool start, bool end, int headLength, 
		int headAngle, bool filledHead)
{
	auxMrkWidth=width;	
	auxMrkStyle=style;
	auxMrkColor=color;
	startArrowOn=start;
	endArrowOn=end;
	auxArrowHeadLength=headLength;
	auxArrowHeadAngle=headAngle;
	auxFilledArrowHead=filledHead;
}

bool Graph::titleSelected()
{
	return d_plot->titleLabel()->hasFocus(); 
}

void Graph::selectTitle()
{
	if (!d_plot->hasFocus())
	{
		emit selectedGraph(this);
		QwtTextLabel *title = d_plot->titleLabel();
		title->setFocus();
	}

	selectedMarker = -1;
}

void Graph::setTitle(const QString& t)
{
	d_plot->setTitle (t);	
	emit modifiedGraph(); 
}

void Graph::removeTitle()
{
	if (d_plot->titleLabel()->hasFocus())
	{
		QRect rect = d_plot->contentsRect();

		QwtPlotLayout *layout = (QwtPlotLayout *)d_plot->plotLayout();
		int y = 0;
		if (d_plot->axisEnabled(QwtPlot::xTop))
			y = layout->scaleRect(QwtPlot::xTop).y();
		else
			y = layout->canvasRect().y();

		rect.setY(y);

		d_plot->setTitle(QString::null);
		d_plot->resize(rect.size());
		setGeometry(rect);

		emit modifiedGraph(); 
	}
}

void Graph::initTitle(bool on, const QFont& fnt)
{
	if (on)
	{
		QwtText t = d_plot->title();
		t.setFont(fnt);
		t.setText(tr("Title"));
		d_plot->setTitle (t);
	}
}

void Graph::removeLegend()
{
	if (legendMarkerID>=0)
	{
		d_plot->removeMarker(legendMarkerID);
		legendMarkerID=-1;
	}
}

void Graph::updateImageMarker(int x, int y, int w, int h)
{
	ImageMarker* mrk=(ImageMarker*) d_plot->marker(selectedMarker);
	mrk->setRect(x, y, w, h);
	d_plot->replot();
	emit modifiedGraph();
}

void Graph::updateTextMarker(const QString& text,int angle, int bkg,const QFont& fnt,
		const QColor& textColor, const QColor& backgroundColor)
{
	LegendMarker* mrkL=(LegendMarker*) d_plot->marker(selectedMarker);
	mrkL->setText(text);
	mrkL->setAngle(angle);
	mrkL->setTextColor(textColor);
	mrkL->setBackgroundColor(backgroundColor);
	mrkL->setFont(fnt);
	mrkL->setBackground(bkg);

	d_plot->replot();
	emit modifiedGraph();
}


LegendMarker* Graph::legend()
{
	if (legendMarkerID >=0 )
		return (LegendMarker*) d_plot->marker(legendMarkerID);
	else
		return 0;
}

QString Graph::legendText()
{
	QString text="";
	for (int i=0; i<n_curves; i++)
	{
		const QwtPlotCurve *c = curve(i);
		if (c &&  c_type[i] != ErrorBars)
		{
			text+="\\c{";
			text+=QString::number(i+1);
			text+="}";
			text+=c->title().text();
			text+="\n";
		}
	}
	return text;
}

QString Graph::pieLegendText()
{
	QString text="";
	Q3ValueList<int> keys= d_plot->curveKeys();	
	const QwtPlotCurve *curve=d_plot->curve(keys[0]);
	if (curve)
	{
		for (int i=0;i<int(curve->dataSize());i++)
		{
			text+="\\p{";
			text+=QString::number(i+1);
			text+="} ";
			text+=QString::number(i+1);
			text+="\n";
		}
	}
	return text;
}

void Graph::moveBy(int dx, int dy)
{
	if ( dx == 0 && dy == 0 )
		return;

	const QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	const int x = d_plot->transform(curve->xAxis(),
			curve->x(selectedPoint)) + dx;
	const int y = d_plot->transform(curve->yAxis(),
			curve->y(selectedPoint)) + dy;

	move(QPoint(x, y));
}

void Graph::removePoint()
{
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	Q3ValueList<int> keys = d_plot->curveKeys();
	int id = keys.findIndex(selectedCurve);
	QString name=associations[id];
	if (name.contains("=")>0)
	{
		QMessageBox::critical(0, tr("QtiPlot - Remove point error"),
				tr("This function is not available for function curves!"));
		return;	
	}

	if (name.contains("(yErr)") || name.contains("(xErr)"))	
	{
		int pos1=name.find(",",0);
		pos1=name.find(",",pos1+1);
		int pos2=name.find("(",pos1);
		name=name.mid(pos1+1,pos2-pos1-1);
		QwtErrorPlotCurve *err= (QwtErrorPlotCurve *) d_plot->curve(selectedCurve);
		double val=err->errors()[selectedPoint];
		emit clearCell(name,val);
	}
	else
	{
		int pos1=name.find(",",0);
		int pos2=name.find("(",pos1);
		name=name.mid(pos1+1,pos2-pos1-1);
		emit clearCell(name,curve->y(selectedPoint));
	}		

	this->setFocus();
	selectedPoint=-1;
	selectedCurve=-1;
}

// Move the selected point
void Graph::move(const QPoint &pos)
{
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	Q3ValueList<int> keys= d_plot->curveKeys();
	int id=keys.findIndex(selectedCurve);
	if (associations[id].contains("=")>0)
	{
		QMessageBox::critical(0, tr("QtiPlot - Move point error"),
				tr("This function is not available for function curves!"));
		return;	
	}

	QwtArray<double> xData(curve->dataSize());
	QwtArray<double> yData(curve->dataSize());

	for ( int i = 0; i < curve->dataSize(); i++ )
	{
		if ( i == selectedPoint )
		{
			xData[i] = d_plot->invTransform(curve->xAxis(), pos.x());
			yData[i] = d_plot->invTransform(curve->yAxis(), pos.y());
		}
		else
		{
			xData[i] = curve->x(i);
			yData[i] = curve->y(i);
		}
	}
	curve->setData(xData, yData);
	d_plot->replot();

	QString text=QString::number(xData[selectedPoint])+"\t"+QString::number(yData[selectedPoint]);
	emit updateTable(associations[id],selectedPoint,text);

	QString info;
	info.sprintf("x=%g; y=%g",
			xData[selectedPoint],
			yData[selectedPoint]);
	emit cursorInfo(info);
}

// Hightlight the selected point
void Graph::highlightPoint(bool showIt)
{
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	QPainter painter(d_plot->canvas());

	painter.setClipping(TRUE);
	painter.setClipRect(d_plot->canvas()->contentsRect());

	// FIXME: next line
	//    if ( showIt )
	//        painter.setRasterOp(Qt::NotROP);

	curve->draw(&painter,
			d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
			selectedPoint,selectedPoint);
}

void Graph::showCursor(bool showIt)
{
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	d_plot->replot();

	if ( showIt )
	{
		QPainter paint(d_plot->canvas());
		// FIXME: next line
		//paint.setRasterOp(Qt::NotXorROP);
		QwtPlotMarker mrkCross;
		mrkCross.setValue(curve->x(selectedPoint), curve->y(selectedPoint));
		mrkCross.setLinePen (QPen(Qt::red,1));
		mrkCross.setLineStyle(QwtPlotMarker::Cross);
		mrkCross.setSymbol(QwtSymbol(QwtSymbol::Rect,
					QBrush(Qt::NoBrush), QPen(Qt::black,1), QSize(25,25)));
		mrkCross.draw(&paint, d_plot->canvasMap(QwtPlot::xBottom), 
				d_plot->canvasMap(QwtPlot::yLeft), 
				d_plot->canvas()->contentsRect());				
	}
}

// Select the next/previous neighbour of the selected point
void Graph::shiftPointCursor(bool up)
{
	const QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	int index = selectedPoint + (up ? 1 : -1);
	index = (index + curve->dataSize()) % curve->dataSize();

	QString info;
	info=curve->title().text(); 
	info+="[";
	info+=QString::number(index+1);
	info+="]: x=";
	info+=QString::number(curve->x(index), 'G', 15);
	info+="; y=";
	info+=QString::number(curve->y(index), 'G', 15);

	emit cursorInfo(info);

	if ( index != selectedPoint )
	{
		showCursor(FALSE);
		selectedPoint = index;
		showCursor(TRUE);
	}
}

void Graph::shiftCurveCursor(bool up)
{// Select the next/previous curve 
	int index = 0;
	if ( selectedCurve >= 0 )
	{
		for ( uint i = 0; i < c_keys.count() - 1; i++ )
		{
			if ( selectedCurve == c_keys[(int)i] )
			{
				index = i + (up ? 1 : -1);
				break;
			}
		}
	}
	index = (c_keys.count() + index) % c_keys.count();

	if ( selectedCurve != c_keys[index] )
	{
		showCursor(FALSE);
		selectedCurve = c_keys[index];
		showCursor(TRUE);
	}

	const QwtPlotCurve *c = d_plot->curve(selectedCurve);
	if ( !c )
		return;

	QString info;
	info=c->title().text();
	info+="[";
	info+=QString::number(selectedPoint+1);
	info+="]: x=";
	info+=QString::number(c->x(selectedPoint), 'G', 15);
	info+="; y=";
	info+=QString::number(c->y(selectedPoint), 'G', 15);
	emit cursorInfo(info);
}

void Graph::clearPlot()
{
	d_plot->clear();
	d_plot->replot();
}

void Graph::changePlotAssociation(Table* t, int curve, const QString& text)
{
	if (associations[curve] == text)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);
	if (text.contains("(yErr)") || text.contains("(xErr)"))
	{
		QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(curve);
		if (!er)
			return;
		int type = QwtErrorPlotCurve::Vertical;
		if (text.contains("(xErr)"))
			type = QwtErrorPlotCurve::Horizontal;
		if (er->direction() != type)
		{
			er->setDirection(type);
			d_plot->replot();		
		}

		QString s = text;
		if (associations[curve].remove("(xErr)").remove("(yErr)") != 
				s.remove("(xErr)").remove("(yErr)"))
		{
			associations[curve] = text;
			updateData(t, curve);
			updateErrorBarsData(t, curve);
		}
		else
			associations[curve] = text;
	}
	else if ((text.contains("(X)") == 2 && text.contains("(Y)") == 2) || 
			(text.contains("(A)") && text.contains("(M)")))
	{//vectors curve
		QStringList ls = QStringList::split (",", text, FALSE );
		QwtPlotCurve *c = d_plot->curve(c_keys[curve]);
		if (c)
			c->setTitle(ls[1].remove("(Y)"));//update curve name
		associations[curve] = text;
		updateVectorsData(t, curve);	
	}
	else
	{// change error bars associations depending on this curve
		QString old_as = associations[curve];
		for (int i=0; i<n_curves; i++)
		{
			QString as = associations[i];
			if (as.contains(old_as) && (as.contains("(xErr)") || as.contains("(yErr)")))
			{
				QStringList ls = QStringList::split(",", as, false);
				as = text + "," + ls[2];
				associations[i] = as;
			}
		}

		//update curve name
		QStringList ls = QStringList::split (",", text, FALSE );
		QwtPlotCurve *c = d_plot->curve(c_keys[curve]);
		if (c)
			c->setTitle(ls[1].remove("(Y)"));

		associations[curve] = text;
		updateData(t, curve);
	}
	emit modifiedGraph();
	emit modifiedPlotAssociation();
	QApplication::restoreOverrideCursor();
}

void Graph::updateCurveData(Table* w, const QString& yColName, int curve)
{
	if (piePlot)
		updatePieCurveData(w, yColName, curve);
	else
	{
		int plotType = c_type[curve];
		if ( plotType == ErrorBars)
			updateErrorBarsData(w, curve);
		else if (plotType == Histogram)
			updateHistogram(w, yColName, curve);
		else if (plotType == VectXYXY || plotType == VectXYAM)
			updateVectorsData(w, curve);
		else if (plotType == Box)
			updateBoxData(w, yColName, curve);
		else
			updateData(w, curve);
	}
}

void Graph::updateData(Table* w, int curve)
{
	long curveID = c_keys[curve];
	QStringList cols = QStringList::split(",", associations[curve], false);
	QString xcName = cols[0].remove("(X)");
	QString ycName = cols[1].remove("(Y)");

	int xcol=w->colIndex(xcName);
	int ycol=w->colIndex(ycName);
	if (xcol < 0 || ycol < 0)
	{
		removeCurve(curve);
		return;
	}

	Q3MemArray<double> X,Y;
	int i, it=0;
	int r = w->tableRows();

	int xColType = w->columnType(xcol);
	int yColType = w->columnType(ycol);

	QStringList xLabels, yLabels;// store text labels

	QTime time0;
	QDate date;
	if (xColType == Table::Time)
	{
		for (i = 0; i<r; i++ )
		{
			QString xval=w->text(i,xcol);
			if (!xval.isEmpty())
			{
				time0 = QTime::fromString (xval, Qt::TextDate);
				if (time0.isValid())
					break;
			}
		}
	}
	else if (xColType == Table::Date)
	{
		for (i = 0; i<r; i++ )
		{
			QString xval=w->text(i,xcol);
			if (!xval.isEmpty())
			{
				date = QDate::fromString (xval, Qt::ISODate);
				if (date.isValid())
					break;
			}
		}
	}

	for (i = 0; i<r; i++ )
	{
		QString xval=w->text(i,xcol);
		QString yval=w->text(i,ycol);
		if (!xval.isEmpty() && !yval.isEmpty())
		{
			it++;
			X.resize(it);
			Y.resize(it);
			if (xColType == Table::Text)
			{
				xLabels<<xval;
				X[it-1]=(double)it;
			}
			else if (xColType == Table::Time)
			{
				QTime time = QTime::fromString (xval, Qt::TextDate);
				if (time.isValid())
					X[it-1]= time0.msecsTo (time);
			}
			else if (xColType == Table::Date)
			{
				QDate d = QDate::fromString (xval, Qt::ISODate);
				if (d.isValid())
					X[it-1]= (double) date.daysTo (d);
			}
			else
				X[it-1]=xval.toDouble();

			if (yColType == Table::Text)
			{
				yLabels<<yval;
				Y[it-1]=(double)it;
			}
			else
				Y[it-1]=yval.toDouble();
		}
	}

	if (!it)
	{
		removeCurve(curve);
		return;
	}
	else
	{//update curve data
		QwtPlotCurve *c = d_plot->curve(curveID);
		if (c_type[curve] == HorizontalBars)
			c->setData(Y, X, it);
		else
			c->setData(X, Y, it);

		if (xColType == Table::Text)
		{
			if (c_type[curve] == HorizontalBars)
			{
				axisType[QwtPlot::yLeft] = Txt;
				axesFormatInfo[QwtPlot::yLeft] = xcName;
				d_plot->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(xLabels));
			}
			else
			{
				axisType[QwtPlot::xBottom] = Txt;
				axesFormatInfo[QwtPlot::xBottom] = xcName;
				d_plot->setAxisScaleDraw (QwtPlot::xBottom, new QwtTextScaleDraw(xLabels));
			}
		}
		else if (xColType == Table::Time )
		{
			if (c_type[curve] == HorizontalBars)
			{
				QStringList lst= QStringList::split(";", axesFormatInfo[QwtPlot::yLeft], true);
				QString fmtInfo = time0.toString(Qt::TextDate) + ";" + lst[1];
				setLabelsDateTimeFormat(QwtPlot::yLeft, Time, fmtInfo);
			}
			else
			{
				QStringList lst= QStringList::split(";", axesFormatInfo[QwtPlot::xBottom], true);
				QString fmtInfo = time0.toString(Qt::TextDate) + ";" + lst[1];
				setLabelsDateTimeFormat(QwtPlot::xBottom, Time, fmtInfo);
			}
		}
		else if (xColType == Table::Date )
		{
			if (c_type[curve] == HorizontalBars)
			{
				QStringList lst= QStringList::split(";", axesFormatInfo[QwtPlot::yLeft], true);
				QString fmtInfo = date.toString(Qt::ISODate) + ";" + lst[1];
				setLabelsDateTimeFormat(QwtPlot::yLeft, Date, fmtInfo);
			}
			else
			{
				QStringList lst= QStringList::split(";", axesFormatInfo[QwtPlot::xBottom], true);
				QString fmtInfo = date.toString(Qt::ISODate) + ";" + lst[1];
				setLabelsDateTimeFormat(QwtPlot::xBottom, Date, fmtInfo);
			}
		}

		if (yColType == Table::Text)
		{
			axisType[QwtPlot::yLeft] = Txt;
			axesFormatInfo[QwtPlot::yLeft] = ycName;
			d_plot->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(yLabels));
		}
	}

	// move error bars
	for (i=0; i<n_curves; i++)
	{
		if (c_type[i] == ErrorBars)
		{
			QStringList lst = QStringList::split(",", associations[i], false);
			if (lst[0].remove("(X)") == cols[0] && lst[1].remove("(Y)") == cols[1])
			{
				if (!it)
					removeCurve(i);
				else
				{
					QwtPlotCurve *c = d_plot->curve(c_keys[i]);
					if (c)
						c->setData(X, Y, it);
				}
			}
		}
	}
}

void Graph::updateErrorBarsData(Table* w, int curve)
{
	QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(curve);
	if (!er)
		return;

	QStringList asl = QStringList::split(",",associations[curve], FALSE);
	QString errColName = asl[2].remove("(yErr)").remove("(xErr)");
	int errcol = w->colIndex(errColName);
	if (errcol < 0)
	{
		removeCurve(curve);
		return;
	}

	int size = er->dataSize();
	Q3MemArray<double> err(size);
	for (int i = 0; i<size; i++ )
	{
		QString errval = w->text(i,errcol);
		err[i]=errval.toDouble();
	}	

	Q3MemArray<double> err_old = er->errors();
	if (err_old == err)
		return;

	er->setErrors(err);	
	updatePlot();
}

void Graph::updatePieCurveData(Table* w, const QString& yColName, int curve)
{
	Q3MemArray<double> X;
	int it=0;
	int ycol=w->colIndex(yColName);
	for (int i = 0; i<w->tableRows(); i++ )
	{
		QString xval=w->text(i, ycol);
		if (!xval.isEmpty())
		{
			X.resize(++it);
			X[it-1]=xval.toDouble();
		}
	}
	QwtPlotCurve *c = d_plot->curve(c_keys[curve]);
	if (c)
	{
		c->setData(X, X, it);
		d_plot->replot();
	}
}

void Graph::updateVectorsData(Table* w, int curve)
{
	long curveID = c_keys[curve];
	VectorCurve *vect=(VectorCurve *)d_plot->curve(curveID);
	if (!vect)
		return;

	QStringList cols=QStringList::split(",", associations[curve], false);
	int xcol=w->colIndex(cols[0].remove("(X)"));
	int ycol=w->colIndex(cols[1].remove("(Y)"));
	int endXCol=w->colIndex(cols[2].remove("(X)").remove("(A)"));
	int endYCol=w->colIndex(cols[3].remove("(Y)").remove("(M)"));

	Q3MemArray<double> X(1), Y(1), endX(1), endY(1);

	int i, it=0;
	for (i = 0; i< w->tableRows(); i++ )
	{
		QString xval=w->text(i, xcol);
		QString yval=w->text(i, ycol);
		QString xend=w->text(i, endXCol);
		QString yend=w->text(i, endYCol);
		if (!xval.isEmpty() && !yval.isEmpty() && !xend.isEmpty() && !yend.isEmpty())
		{
			it++;
			X.resize(it); Y.resize(it); endX.resize(it); endY.resize(it);
			X[it-1]=xval.toDouble();
			Y[it-1]=yval.toDouble();
			endX[it-1]=xend.toDouble();
			endY[it-1]=yend.toDouble();
		}
	}

	if (!it)
		removeCurve(curve);

	QwtPlotCurve *c = d_plot->curve(c_keys[curve]);
	if (c)
		c->setData(X, X, it);

	vect->setVectorEnd(endX, endY);
	updatePlot();
}

QString Graph::saveEnabledAxes()
{
	QString list="EnabledAxes\t";
	for (int i = 0;i<QwtPlot::axisCnt;i++)
		list+=QString::number(d_plot->axisEnabled (i))+"\t";

	list+="\n";
	return list;
}

bool Graph::framed()
{
	bool frameOn=FALSE;

	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();  
	if (canvas->lineWidth()>0)
		frameOn=TRUE;

	return frameOn;
}

QColor Graph::canvasFrameColor()
{
	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();  
	QPalette pal =canvas->palette();
	return pal.color(QPalette::Active, QColorGroup::Foreground);
}

int Graph::canvasFrameWidth()
{
	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();  
	return canvas->lineWidth();
}

void Graph::drawCanvasFrame(const QStringList& frame)
{
	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();	
	canvas->setLineWidth((frame[1]).toInt());

	QPalette pal =canvas->palette();
	pal.setColor(QColorGroup::Foreground,QColor(frame[2]));
	canvas->setPalette(pal); 
}

void Graph::drawCanvasFrame(bool frameOn, int width, const QColor& color)
{
	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();  
	QPalette pal = canvas->palette();

	if (frameOn && canvas->lineWidth() == width && 
			pal.color(QPalette::Active, QColorGroup::Foreground) == color)
		return;

	if (frameOn)
	{
		canvas->setLineWidth(width);
		pal.setColor(QColorGroup::Foreground,color);
		canvas->setPalette(pal);
	}
	else
	{
		canvas->setLineWidth(0);
		pal.setColor(QColorGroup::Foreground,QColor(Qt::black));
		canvas->setPalette(pal);
	}		
	emit modifiedGraph();	
}

void Graph::drawCanvasFrame(bool frameOn, int width)
{
	if (frameOn)
	{
		QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();	
		canvas->setLineWidth(width);
	}
}

void Graph::drawAxesBackbones(bool yes)
{
	if (drawAxesBackbone == yes)
		return;

	drawAxesBackbone = yes;

	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		QwtScaleWidget *scale=(QwtScaleWidget*) d_plot->axisWidget(i);
		if (scale)
		{
			ScaleDraw *sclDraw= (ScaleDraw *)d_plot->axisScaleDraw (i);
			sclDraw->enableComponent (QwtAbstractScaleDraw::Backbone, yes);
			scale->repaint();
		}
	}

	d_plot->replot();
	emit modifiedGraph();
}

void Graph::loadAxesOptions(const QString& s)
{
	if (s == "1")
		return;

	drawAxesBackbone = false;

	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		QwtScaleWidget *scale=(QwtScaleWidget*) d_plot->axisWidget(i);
		if (scale)
		{
			ScaleDraw *sclDraw= (ScaleDraw *)d_plot->axisScaleDraw (i);
			sclDraw->enableComponent (QwtAbstractScaleDraw::Backbone, false);			
			scale->repaint();
		}
	}
}

void Graph::setAxesLinewidth(int width)
{
	if (d_plot->axesLinewidth() == width)
		return;

	d_plot->setAxesLinewidth(width);

	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		QwtScaleWidget *scale=(QwtScaleWidget*) d_plot->axisWidget(i);
		if (scale)
		{
			scale->setPenWidth(width);
			scale->repaint();
		}
	}

	d_plot->replot();
	emit modifiedGraph();
}

void Graph::loadAxesLinewidth(int width)
{
	d_plot->setAxesLinewidth(width);
}

QString Graph::saveCanvas()
{
	QString s="";
	int w = d_plot->canvas()->lineWidth();
	if (w>0)
	{
		s += "CanvasFrame\t" + QString::number(w)+"\t";
		s += canvasFrameColor().name()+"\n";
	}
	s += "CanvasBackground\t" + d_plot->canvasBackground().name()+"\n"; 
	return s;
}

QString Graph::saveFonts()
{
	int i;
	QString s;
	QStringList list,axesList;
	QFont f;
	list<<"TitleFont";
	f=d_plot->title().font();
	list<<f.family();
	list<<QString::number(f.pointSize());
	list<<QString::number(f.weight());
	list<<QString::number(f.italic());
	list<<QString::number(f.underline());
	list<<QString::number(f.strikeOut());
	s=list.join ("\t")+"\n";

	for (i=0;i<d_plot->axisCnt;i++)
	{
		f=d_plot->axisTitle(i).font();
		list[0]="ScaleFont"+QString::number(i);
		list[1]=f.family();
		list[2]=QString::number(f.pointSize());
		list[3]=QString::number(f.weight());
		list[4]=QString::number(f.italic());
		list[5]=QString::number(f.underline());
		list[6]=QString::number(f.strikeOut());
		s+=list.join ("\t")+"\n";
	}

	for (i=0;i<d_plot->axisCnt;i++)
	{
		f=d_plot->axisFont(i);
		list[0]="AxisFont"+QString::number(i);
		list[1]=f.family();
		list[2]=QString::number(f.pointSize());
		list[3]=QString::number(f.weight());
		list[4]=QString::number(f.italic());
		list[5]=QString::number(f.underline());
		list[6]=QString::number(f.strikeOut());
		s+=list.join ("\t")+"\n";
	}
	return s;
}

QString Graph::saveAxesFormulas()
{
	QString s;
	for (int i=0; i<4; i++)
		if (!axesFormulas[i].isEmpty())
		{
			s += "<AxisFormula pos=\""+QString::number(i)+"\">\n";
			s += axesFormulas[i];
			s += "\n</AxisFormula>\n";
		}
	return s;
}

QString Graph::saveScale()
{
	QString s;
	for (int i=0; i < QwtPlot::axisCnt; i++)
	{
		s += "scale\t" + QString::number(i)+"\t";

		const QwtScaleDiv *scDiv=d_plot->axisScaleDiv(i);
		QwtValueList lst = scDiv->ticks (QwtScaleDiv::MajorTick);

		s += QString::number(QMIN(scDiv->lBound(), scDiv->hBound()), 'g', 15)+"\t";
		s += QString::number(QMAX(scDiv->lBound(), scDiv->hBound()), 'g', 15)+"\t";

		double step = 0.0;
		if (d_user_step[i])
			step = fabs(lst[1]-lst[0]);

		s += QString::number(step, 'g', 15)+"\t";
		s += QString::number(lst.count())+"\t";
		s += QString::number(d_plot->axisMaxMinor(i))+"\t";

		const QwtScaleEngine *sc_eng = d_plot->axisScaleEngine(i);
		QwtScaleTransformation *tr = sc_eng->transformation();
		s += QString::number((int)tr->type())+"\t";
		s += QString::number(sc_eng->testAttribute(QwtScaleEngine::Inverted))+"\n";
	}
	return s;
}

QString Graph::saveErrorBars()
{
	Q3ValueList<int> keys = d_plot->curveKeys();
	QString all;
	for (int i=0; i<n_curves; i++)
	{
		QString s="";
		if (c_type[i] == ErrorBars)
		{
			long curveID = keys[i];
			QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)d_plot->curve(curveID);
			if (er)
			{
				s+="ErrorBars\t";
				s+=QString::number(er->direction())+"\t";
				QStringList cvs=QStringList::split(",",associations[i],FALSE);

				s+=cvs[0].remove("(X)",true)+"\t";
				s+=cvs[1].remove("(Y)",true)+"\t";
				if (!er->direction())
					s+=cvs[2].remove("(xErr)",true)+"\t";
				else
					s+=cvs[2].remove("(yErr)",true)+"\t";

				s+=QString::number(er->width())+"\t";
				s+=QString::number(er->capLength())+"\t";
				s+=er->color().name()+"\t";
				s+=QString::number(er->throughSymbol())+"\t";
				s+=QString::number(er->plusSide())+"\t";
				s+=QString::number(er->minusSide())+"\t";
				s+=QString::number(er->xDataOffset())+"\t";
				s+=QString::number(er->yDataOffset())+"\n";
				all+=s;
			}				
		}
	}
	return all;
}

void Graph::setXAxisTitleColor(const QColor& c)
{
	QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(QwtPlot::xBottom);
	if (scale)
	{
		QwtText t = scale->title();
		t.setColor (c);
		scale->setTitle (t);
		emit modifiedGraph();
	}
}

void Graph::setYAxisTitleColor(const QColor& c)
{
	QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(QwtPlot::yLeft);
	if (scale)
	{
		QwtText t = scale->title();
		t.setColor (c);
		scale->setTitle (t);
		emit modifiedGraph();
	}
}

void Graph::setRightAxisTitleColor(const QColor& c)
{
	QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(QwtPlot::yRight);
	if (scale)
	{
		QwtText t = scale->title();
		t.setColor (c);
		scale->setTitle (t);
		emit modifiedGraph();
	}
}

void Graph::setTopAxisTitleColor(const QColor& c)
{
	QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(QwtPlot::xTop);
	if (scale)
	{
		QwtText t = scale->title();
		t.setColor (c);
		scale->setTitle (t);
		emit modifiedGraph();
	}
}

void Graph::setAxesTitleColor(QStringList l)
{
	for (int i=0;i<int(l.count()-1);i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
			scale->title().setColor(QColor(l[i+1]));
	}
}

QString Graph::saveAxesTitleColors()
{
	QString s="AxesTitleColors\t";
	for (int i=0;i<4;i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		QColor c;
		if (scale)
			c=scale->title().color();
		else
			c=QColor(Qt::black);

		s+=c.name()+"\t";	
	}	
	return s+"\n";
}

QString Graph::saveTitle()
{
	QString s="PlotTitle\t";
	s += d_plot->title().text().replace("\n", "<br>")+"\t";
	s += d_plot->title().color().name()+"\t";
	s += QString::number(d_plot->title().renderFlags())+"\n";
	return s;
}

QString Graph::saveScaleTitles()
{
	int a;
	QString s="";
	for (int i=0;i<4;i++)
	{
		switch (i)
		{
			case 0:
				a=2;
				break;

			case 1:
				a=0;
				break;

			case 2:
				a=3;
				break;

			case 3:
				a=1;
				break;
		}
		s+=(d_plot->axisTitle(a)).text().replace("\n", "<br>")+"\t";
	}
	return s+"\n";
}

QString Graph::saveAxesTitleAlignement()
{
	QString s="AxesTitleAlignment\t";
	QStringList axes;
	int i;
	for (i=0;i<4;i++)
		axes<<QString::number(Qt::AlignHCenter);

	for (i=0;i<4;i++)
	{

		if (d_plot->axisEnabled(i))
			axes[i]=QString::number(d_plot->axisTitle(i).renderFlags());
	}

	s+=axes.join("\t")+"\n";
	return s;
}

QString Graph::savePieCurveLayout()
{
	QString s="PieCurve\t";

	QwtPieCurve *pieCurve=(QwtPieCurve*)curve(0);
	s+= pieCurve->title().text()+"\t";
	QPen pen=pieCurve->pen();

	s+=QString::number(pen.width())+"\t";
	s+=pen.color().name()+"\t";
	s+=penStyleName(pen.style()) + "\t";

	Qt::BrushStyle pattern=pieCurve->pattern();
	int index;
	if (pattern == Qt::SolidPattern)
		index=0;
	else if (pattern == Qt::HorPattern)
		index=1;
	else if (pattern == Qt::VerPattern)
		index=2;
	else if (pattern == Qt::CrossPattern)
		index=3;
	else if (pattern == Qt::BDiagPattern)
		index=4;
	else if (pattern == Qt::FDiagPattern)
		index=5;
	else if (pattern == Qt::DiagCrossPattern)
		index=6;
	else if (pattern == Qt::Dense1Pattern)
		index=7;
	else if (pattern == Qt::Dense2Pattern)
		index=8;
	else if (pattern == Qt::Dense3Pattern)
		index=9;
	else if (pattern == Qt::Dense4Pattern)
		index=10;
	else if (pattern == Qt::Dense5Pattern)
		index=11;
	else if (pattern == Qt::Dense6Pattern)
		index=12;
	else if (pattern == Qt::Dense7Pattern)
		index=13;

	s+=QString::number(index)+"\t";
	s+=QString::number(pieRay)+"\t";
	s+=QString::number(pieCurve->first())+"\n";

	return s;
}

QString Graph::saveCurveLayout(int index)
{
	QString s = QString::null;
	int style = c_type[index];
	QwtPlotCurve *c = (QwtPlotCurve*)curve(index);
	if (c)
	{
		s+=QString::number(style)+"\t";
		if (style == Spline)
			s+="5\t";
		else if (style == VerticalSteps)
			s+="6\t";
		else
			s+=QString::number(c->style())+"\t";
		s+=QString::number(ColorBox::colorIndex(c->pen().color()))+"\t";
		s+=QString::number(c->pen().style()-1)+"\t";
		s+=QString::number(c->pen().width())+"\t";

		const QwtSymbol symbol = c->symbol();
		s+=QString::number(symbol.size().width())+"\t";
		s+=QString::number(SymbolBox::symbolIndex(symbol.style()))+"\t";
		s+=QString::number(ColorBox::colorIndex(symbol.pen().color()))+"\t";
		if (symbol.brush().style() != Qt::NoBrush)
			s+=QString::number(ColorBox::colorIndex(symbol.brush().color()))+"\t";
		else
			s+=QString::number(-1)+"\t";

		bool filled = c->brush().style() == Qt::NoBrush ? false : true;
		s+=QString::number(filled)+"\t";

		s+=QString::number(ColorBox::colorIndex(c->brush().color()))+"\t";
		s+=QString::number(PatternBox::patternIndex(c->brush().style()))+"\t";
		if (style <= LineSymbols || style == Box)
			s+=QString::number(symbol.pen().width())+"\t";
	}

	if(style == VerticalBars||style == HorizontalBars||style == Histogram)
	{
		QwtBarCurve *b = (QwtBarCurve*)c;
		s+=QString::number(b->gap())+"\t";
		s+=QString::number(b->offset())+"\t";
	}

	if(style == Histogram)
	{
		QwtHistogram *h = (QwtHistogram*)c;
		s+=QString::number(h->autoBinning())+"\t";
		s+=QString::number(h->binSize())+"\t";
		s+=QString::number(h->begin())+"\t";
		s+=QString::number(h->end())+"\t";
	}
	else if(style == VectXYXY || style == VectXYAM)
	{
		VectorCurve *v = (VectorCurve*)c;
		s+=v->color().name()+"\t";
		s+=QString::number(v->width())+"\t";
		s+=QString::number(v->headLength())+"\t";
		s+=QString::number(v->headAngle())+"\t";
		s+=QString::number(v->filledArrowHead())+"\t";

		QStringList colsList=QStringList::split(",", associations[index], false);
		s+=colsList[2].remove("(X)").remove("(A)")+"\t";
		s+=colsList[3].remove("(Y)").remove("(M)");
		if (style == VectXYAM)
			s+="\t"+QString::number(v->position());
		s+="\t";
	}
	else if(style == Box)
	{
		BoxCurve *b = (BoxCurve*)c;
		s+=QString::number(SymbolBox::symbolIndex(b->maxStyle()))+"\t";
		s+=QString::number(SymbolBox::symbolIndex(b->p99Style()))+"\t";
		s+=QString::number(SymbolBox::symbolIndex(b->meanStyle()))+"\t";
		s+=QString::number(SymbolBox::symbolIndex(b->p1Style()))+"\t";
		s+=QString::number(SymbolBox::symbolIndex(b->minStyle()))+"\t";
		s+=QString::number(b->boxStyle())+"\t";
		s+=QString::number(b->boxWidth())+"\t";
		s+=QString::number(b->boxRangeType())+"\t";
		s+=QString::number(b->boxRange())+"\t";
		s+=QString::number(b->whiskersRangeType())+"\t";
		s+=QString::number(b->whiskersRange())+"\t";
	}
	else
		s+="\n";

	return s;
}

QString Graph::saveCurves()
{
	QString s;
	if (piePlot)
		s+=savePieCurveLayout();
	else
	{	
		for (int j=0;j<n_curves;j++)
		{
			if (c_type[j] != ErrorBars)
			{
				QwtPlotCurve *c = this->curve(j);
				if (c->rtti() == FunctionCurve::RTTI)
					s += ((FunctionCurve *)c)->saveToString();					
				else if (c_type[j] == Box)
					s += "curve\t" + QString::number(c->x(0)) + "\t" + c->title().text() + "\t";
				else
				{
					QStringList as=QStringList::split(",", associations[j],FALSE);
					s += "curve\t" + as[0].remove("(X)",true) + "\t";
					s += as[1].remove("(Y)",true) + "\t";
				}			
				s += saveCurveLayout(j);
				s += QString::number(c->xAxis())+"\t"+QString::number(c->yAxis())+"\n";
			}
		}
	}
	return s;
}

QString Graph::saveGridOptions()
{
	QString s="grid\t";
	s+=QString::number(grid.majorOnX)+"\t";
	s+=QString::number(grid.minorOnX)+"\t";
	s+=QString::number(grid.majorOnY)+"\t";
	s+=QString::number(grid.minorOnY)+"\t";
	s+=QString::number(grid.majorCol)+"\t";
	s+=QString::number(grid.majorStyle)+"\t";
	s+=QString::number(grid.majorWidth)+"\t";
	s+=QString::number(grid.minorCol)+"\t";
	s+=QString::number(grid.minorStyle)+"\t";
	s+=QString::number(grid.minorWidth)+"\t";
	s+=QString::number(grid.xZeroOn)+"\t";
	s+=QString::number(grid.yZeroOn)+"\t";
	s+=QString::number(grid.xAxis)+"\t";
	s+=QString::number(grid.yAxis)+"\n";
	return s;
}

void Graph::newLegend()
{
	LegendMarker* mrk = new LegendMarker(d_plot);
	mrk->setOrigin(QPoint(10, 10));
	if (piePlot)
		mrk->setText(pieLegendText());
	else
		mrk->setText(legendText());

	mrk->setBackground(defaultMarkerFrame);
	mrk->setFont(defaultMarkerFont);
	mrk->setTextColor(defaultTextMarkerColor);
	mrk->setBackgroundColor(defaultTextMarkerBackground);

	legendMarkerID=d_plot->insertMarker(mrk);

	emit modifiedGraph();
	d_plot->replot();	
}

void Graph::addTimeStamp()
{
	LegendMarker* mrk= new LegendMarker(d_plot);
	mrk->setOrigin(QPoint(d_plot->canvas()->width()/2, 10));

	QDateTime dt = QDateTime::currentDateTime ();
	mrk->setText(dt.toString(Qt::LocalDate));

	mrk->setBackground(defaultMarkerFrame);
	mrk->setFont(defaultMarkerFont);
	mrk->setTextColor(defaultTextMarkerColor);
	mrk->setBackgroundColor(defaultTextMarkerBackground);

	legendMarkerID=d_plot->insertMarker(mrk);

	emit modifiedGraph();
	d_plot->replot();	
}

LegendMarker* Graph::newLegend(const QString& text)
{
	LegendMarker* mrk= new LegendMarker(d_plot);
	selectedMarker = d_plot->insertMarker(mrk);

	mrk->setOrigin(QPoint(5,5));
	mrk->setText(text);
	mrk->setBackground(defaultMarkerFrame);
	mrk->setFont(defaultMarkerFont);
	mrk->setTextColor(defaultTextMarkerColor);
	mrk->setBackgroundColor(defaultTextMarkerBackground);
	return mrk;
}

void Graph::insertLegend(const QStringList& lst, int fileVersion)
{
	legendMarkerID = insertTextMarker(lst, fileVersion);
}

long Graph::insertTextMarker(const QStringList& list, int fileVersion)
{
	QStringList fList=list;
	LegendMarker* mrk = new LegendMarker(d_plot);
	long key = d_plot->insertMarker(mrk);

	if (fileVersion < 86)
		mrk->setOrigin(QPoint(fList[1].toInt(),fList[2].toInt()));
	else
		mrk->setOriginCoord(fList[1].toDouble(), fList[2].toDouble());

	QFont fnt=QFont (fList[3],fList[4].toInt(),fList[5].toInt(),fList[6].toInt());
	fnt.setUnderline(fList[7].toInt());
	fnt.setStrikeOut(fList[8].toInt());
	mrk->setFont(fnt);

	mrk->setAngle(fList[11].toInt());

	if (fileVersion < 71)
	{
		int bkg=fList[10].toInt();
		if (bkg <= 2)
			mrk->setBackground(bkg);
		else if (bkg == 3)
		{
			mrk->setBackground(0);
			mrk->setBackgroundColor(QColor(255, 255, 255));
		}
		else if (bkg == 4)
		{
			mrk->setBackground(0);
			mrk->setBackgroundColor(QColor(Qt::black));
		}

		QString text;
		int n=(int)fList.count();
		for (int i=0;i<n-12;i++)
			text+=fList[12+i]+"\n";
		mrk->setText(text);
	}
	else
	{
		mrk->setTextColor(QColor(fList[9]));
		mrk->setBackground(fList[10].toInt());
		mrk->setBackgroundColor(QColor(fList[12]));

		QString text;
		int n=(int)fList.count();
		for (int i=0;i<n-13;i++)
			text+=fList[13+i]+"\n";
		mrk->setText(text);
	}
	return key;
}

void Graph::insertLineMarker(QStringList list, int fileVersion)
{
	LineMarker* mrk= new LineMarker();
	long mrkID=d_plot->insertMarker(mrk);
	d_lines.resize(++linesOnPlot);
	d_lines[linesOnPlot-1]=mrkID;

	if (fileVersion < 86)
	{
		mrk->setStartPoint(QPoint(list[1].toInt(), list[2].toInt()));
		mrk->setEndPoint(QPoint(list[3].toInt(), list[4].toInt()));
	}
	else
	{
		mrk->setStartPoint(list[1].toDouble(), list[2].toDouble());
		mrk->setEndPoint(list[3].toDouble(), list[4].toDouble());
	}

	mrk->setWidth(list[5].toInt());
	mrk->setColor(QColor(list[6]));
	mrk->setStyle(getPenStyle(list[7]));
	mrk->drawEndArrow(list[8]=="1");
	mrk->drawStartArrow(list[9]=="1");
	if (list.count()>10)
	{
		mrk->setHeadLength(list[10].toInt());		
		mrk->setHeadAngle(list[11].toInt());		
		mrk->fillArrowHead(list[12]=="1");
	}
}

void Graph::insertLineMarker(LineMarker* mrk)
{
	LineMarker* aux= new LineMarker();
	d_lines.resize(++linesOnPlot);
	d_lines[linesOnPlot-1] = d_plot->insertMarker(aux);	

	aux->setStartPoint(mrk->startPointCoord().x(), mrk->startPointCoord().y());	
	aux->setEndPoint(mrk->endPointCoord().x(), mrk->endPointCoord().y());
	aux->setEndPoint(mrk->endPoint());
	aux->setWidth(mrk->width());
	aux->setColor(mrk->color());
	aux->setStyle(mrk->style());
	aux->drawEndArrow(mrk->hasEndArrow());
	aux->drawStartArrow(mrk->hasStartArrow());
	aux->setHeadLength(mrk->headLength());
	aux->setHeadAngle(mrk->headAngle());
	aux->fillArrowHead(mrk->filledArrowHead());	
}

LineMarker* Graph::lineMarker(long id)
{
	return (LineMarker*)d_plot->marker(id);	
}

ImageMarker* Graph::imageMarker(long id)
{
	return (ImageMarker*)d_plot->marker(id);	
}

LegendMarker* Graph::textMarker(long id)
{
	Q3ValueList<int> txtMrkKeys=textMarkerKeys();
	return (LegendMarker*)d_plot->marker(id);	
}

long Graph::insertTextMarker(LegendMarker* mrk)
{
	LegendMarker* aux = new LegendMarker(d_plot);
	selectedMarker = d_plot->insertMarker(aux);

	aux->setTextColor(mrk->getTextColor());
	aux->setBackgroundColor(mrk->backgroundColor());
	aux->setOrigin(mrk->rect().topLeft ());
	aux->setFont(mrk->getFont());
	aux->setBackground(mrk->getBkgType());
	aux->setAngle(mrk->getAngle());
	aux->setText(mrk->getText());
	return selectedMarker;
}

Q3ValueList<int> Graph::textMarkerKeys()
{
	Q3ValueList<int> mrkKeys=d_plot->markerKeys();
	Q3ValueList<int> txtMrkKeys;
	for (int i=0;i<(int)mrkKeys.size();i++)
	{
		if (mrkKeys[i]!=mrkX && mrkKeys[i]!=mrkY &&
				mrkKeys[i]!=startID && mrkKeys[i]!=endID && 
				d_lines.contains(mrkKeys[i])<=0 &&
				d_images.contains(mrkKeys[i])<=0)
		{
			txtMrkKeys.append(mrkKeys[i]);
		}
	}
	return txtMrkKeys;
}

QString Graph::saveMarkers()
{
	QString s;
	Q3ValueList<int> texts=textMarkerKeys();
	int i,t=texts.size(),l=d_lines.size(),im=d_images.size();
	for (i=0;i<im;i++)
	{	
		ImageMarker* mrkI=(ImageMarker*) d_plot->marker(d_images[i]);
		s+="ImageMarker\t";
		s+=mrkI->getFileName()+"\t";

		QwtDoubleRect rect = mrkI->boundingRect();
		s += QString::number(rect.left(), 'g', 15)+"\t";
		s += QString::number(rect.top(), 'g', 15)+"\t";	
		s += QString::number(rect.width(), 'g', 15)+"\t";
		s += QString::number(rect.height(), 'g', 15)+"\n";
	}

	for (i=0;i<l;i++)
	{	
		LineMarker* mrkL=(LineMarker*) d_plot->marker(d_lines[i]);
		s+="lineMarker\t";

		QwtDoublePoint sp = mrkL->startPointCoord();
		s+=(QString::number(sp.x(), 'g', 15))+"\t";
		s+=(QString::number(sp.y(), 'g', 15))+"\t";

		QwtDoublePoint ep = mrkL->endPointCoord();
		s+=(QString::number(ep.x(), 'g', 15))+"\t";
		s+=(QString::number(ep.y(), 'g', 15))+"\t";

		s+=QString::number(mrkL->width())+"\t";
		s+=mrkL->color().name()+"\t";
		s+=penStyleName(mrkL->style())+"\t";
		s+=QString::number(mrkL->hasEndArrow())+"\t";
		s+=QString::number(mrkL->hasStartArrow())+"\t";
		s+=QString::number(mrkL->headLength())+"\t";
		s+=QString::number(mrkL->headAngle())+"\t";
		s+=QString::number(mrkL->filledArrowHead())+"\n";
	}

	for (i=0;i<t;i++)
	{	
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(texts[i]);
		if (texts[i]!=legendMarkerID)
			s+="textMarker\t";
		else
			s+="Legend\t";

		s+=QString::number(mrk->xValue(), 'g', 15)+"\t";
		s+=QString::number(mrk->yValue(), 'g', 15)+"\t";

		QFont f=mrk->getFont();
		s+=f.family()+"\t";
		s+=QString::number(f.pointSize())+"\t";
		s+=QString::number(f.weight())+"\t";
		s+=QString::number(f.italic())+"\t";
		s+=QString::number(f.underline())+"\t";
		s+=QString::number(f.strikeOut())+"\t";
		s+=mrk->getTextColor().name()+"\t";
		s+=QString::number(mrk->getBkgType())+"\t";
		s+=QString::number(mrk->getAngle())+"\t";
		s+=mrk->backgroundColor().name()+"\t";

		QStringList textList=QStringList::split ("\n",mrk->getText(),FALSE);
		s+=textList.join ("\t");
		s+="\n";
	}
	return s;
}

double Graph::selectedXStartValue()
{
	QwtPlotCurve *cv = (QwtPlotCurve *)d_plot->curve(selectedCurve);
	return cv->x(startPoint);
}

double Graph::selectedXEndValue()
{
	QwtPlotCurve *cv = (QwtPlotCurve *)d_plot->curve(selectedCurve);
	return cv->x(endPoint);
}

int Graph::curveIndex(long key)
{
	return c_keys.find (key, 0 );	
}

long Graph::curveKey(int curve)
{	
	return c_keys[curve];
}

QwtPlotCurve *Graph::curve(int index)
{
	if (!n_curves || index >= n_curves)
		return 0;

	return (QwtPlotCurve *)d_plot->curve(c_keys[index]);
}

int Graph::selectedPoints(long curveKey)
{
	int points;
	QwtPlotCurve *cv = (QwtPlotCurve *)d_plot->curve(curveKey);

	if (endPoint>0)
		points=abs(endPoint-startPoint)+1;
	else
		points=cv->dataSize();
	return points;
}

CurveLayout Graph::initCurveLayout()
{
	CurveLayout cl;
	cl.connectType=1;
	cl.lStyle=0;
	cl.lWidth=1;
	cl.sSize=7;
	cl.sType=0;
	cl.filledArea=0;
	cl.aCol=0;
	cl.aStyle=0;
	cl.lCol=0;
	cl.penWidth = 1;
	cl.symCol=0;
	cl.fillCol=0;	
	return cl;
}

CurveLayout Graph::initCurveLayout(int i, int curves, int style)
{
	CurveLayout cl = initCurveLayout();
	cl.symCol=i%16;//16 is the number of predefined colors in Qt
	cl.fillCol=i%16;
	cl.sType=(i+1)%9; //9 is the number of predefined symbols in Qwt

	if (style == Graph::Line)
		cl.sType = 0;
	else if (style == Graph::VerticalDropLines)
		cl.connectType=2; 	        
	else if (style == Graph::HorizontalSteps || style == Graph::VerticalSteps)
	{
		cl.connectType=3; 	      
		cl.sType = 0;
	}
	else if (style == Graph::Spline)
		cl.connectType=5;

	if (style == Graph::VerticalBars || style == Graph::HorizontalBars)
	{
		cl.filledArea=1;
		cl.lCol=0;//black color pen
		cl.aCol=i+1;
		cl.sType = 0;
		QwtBarCurve *b = (QwtBarCurve*)curve(i);
		if (b)
		{
			b->setGap(qRound(100*(1-1.0/(double)curves)));
			b->setOffset(-50*(curves-1) + i*100);
		}
	}
	else if (style == Graph::Histogram)
	{
		cl.filledArea=1;
		cl.lCol=i+1;//start with red color pen
		cl.aCol=i+1; //start with red fill color
		cl.aStyle=4;
		cl.sType = 0;
	}
	else
		cl.lCol=i%16;

	if (i%16 == 13) 
	{//avoid white invisible curves
		cl.lCol = 0; cl.symCol = 0;
	}

	if (style== Graph::Area)
	{
		cl.filledArea=1;
		cl.aCol=i%16;
		cl.sType = 0;
	}
	return cl;
}

void Graph::setCurveType(int curve, int style)
{
	c_type[curve] = style;
}

void Graph::updateCurveLayout(int index, const CurveLayout *cL)
{
	QwtPlotCurve *c = this->curve(index);
	if (!c || c_type.isEmpty() || (int)c_type.size() < index)
		return;

	QPen pen = QPen(color(cL->symCol),cL->penWidth, Qt::SolidLine);
	if (cL->fillCol != -1)
		c->setSymbol(QwtSymbol(SymbolBox::style(cL->sType), QBrush(color(cL->fillCol)), pen, QSize(cL->sSize,cL->sSize)));
	else
		c->setSymbol(QwtSymbol(SymbolBox::style(cL->sType), QBrush(), pen, QSize(cL->sSize,cL->sSize)));

	c->setPen(QPen(color(cL->lCol),cL->lWidth,getPenStyle(cL->lStyle)));

	int style = c_type[index];
	if (style == Scatter)
		c->setStyle(QwtPlotCurve::NoCurve);
	else if (style == Spline)
	{
		c->setStyle(QwtPlotCurve::Lines);
		c->setCurveAttribute(QwtPlotCurve::Fitted, true);
	}
	else if (style == VerticalSteps)
	{
		c->setStyle(QwtPlotCurve::Steps);
		c->setCurveAttribute(QwtPlotCurve::Inverted, true);
	}
	else
		c->setStyle((QwtPlotCurve::CurveStyle)cL->connectType); 

	QBrush brush = QBrush(color(cL->aCol), Qt::NoBrush);
	if (cL->filledArea)
		brush.setStyle(getBrushStyle(cL->aStyle));
	c->setBrush(brush);

	QString yColName=c->title().text();
	for (int i=0;i<n_curves;i++)
	{
		if (associations[i].contains(yColName) && i!=index && c_type[i] == ErrorBars)
		{
			QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(i);
			if (er)
				er->setSymbolSize(QSize(cL->sSize,cL->sSize));		
		}
	}
}

void Graph::updateErrorBars(int curve,bool xErr,int width,int cap,const QColor& c,
		bool plus,bool minus,bool through)
{
	QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(curve);
	if (!er)
		return;

	if (er->width() == width &&
			er->capLength() == cap && 
			er->color() == c &&
			er->plusSide() == plus &&
			er->minusSide() == minus &&
			er->throughSymbol() == through && 
			er->xErrors() == xErr)
		return;

	er->setWidth(width);
	er->setCapLength(cap);
	er->setColor(c);
	er->setXErrors(xErr);
	er->drawThroughSymbol(through);
	er->drawPlusSide(plus);
	er->drawMinusSide(minus);

	if (xErr) 
		associations[curve]=associations[curve].replace("(yErr)","(xErr)",TRUE); 
	else
		associations[curve]=associations[curve].replace("(xErr)","(yErr)",TRUE); 	
}

void Graph::addErrorBars(Table *w, const QString& yColName, 
		Table *errTable, const QString& errColName,
		int type, int width, int cap, const QColor& color,
		bool through, bool minus,bool plus)
{
	Q3ValueList<int> keys = d_plot->curveKeys();
	for (int i = 0; i<n_curves; i++ )
	{
		QwtPlotCurve *c = d_plot->curve(keys[i]);
		if (c && c->title().text() == yColName && c_type[i] != ErrorBars)
		{
			QStringList lst = QStringList::split(",", associations[i], false);
			addErrorBars(w, lst[0].remove("(X)"), yColName, errTable, errColName,
					type, width, cap, color, through, minus, plus);

			return;
		}
	}
}

void Graph::addErrorBars(Table *w, const QString& xColName, const QString& yColName, 
		Table *errTable, const QString& errColName,
		int type, int width, int cap, const QColor& color,
		bool through, bool minus,bool plus, double xOffset, double yOffset)
{
	int xcol=w->colIndex(xColName);
	int ycol=w->colIndex(yColName);
	int errcol=errTable->colIndex(errColName);
	if (xcol<0 || ycol<0 || errcol<0)
		return;

	int i, it=0;
	int xColType = w->columnType(xcol);
	int yColType = w->columnType(ycol);
	QStringList xLabels, yLabels;// store text labels
	Q3MemArray<double> X, Y, err;
	for (i = 0; i<w->tableRows(); i++ )
	{
		QString xval=w->text(i,xcol);
		QString yval=w->text(i,ycol);
		QString errval=errTable->text(i,errcol);
		if (!xval.isEmpty() && !yval.isEmpty() && !errval.isEmpty())
		{
			X.resize(++it);
			Y.resize(it);
			err.resize(it);
			if (xColType == Table::Text)
			{
				xLabels<<xval;
				X[it-1]=(double)it;
			}
			else
				X[it-1]=xval.toDouble();

			if (yColType == Table::Text)
			{
				yLabels<<yval;
				Y[it-1]=(double)it;
			}
			else
				Y[it-1]=yval.toDouble();
			err[it-1]=errval.toDouble();
		}
	}	

	if (!it)
		return;

	QSize size;
	for (i=0;i<n_curves;i++)
	{
		if (associations[i].contains(yColName) && c_type[i] != ErrorBars)
		{
			long curveID = c_keys[i];
			QwtPlotCurve *c = d_plot->curve(curveID);
			size=c->symbol().size();
		}			
	}

	QwtErrorPlotCurve *er=new QwtErrorPlotCurve(type, d_plot, 0);
	er->setData(X,Y,it);
	er->setErrors(err);
	er->setCapLength(cap);
	er->setSymbolSize(size);
	er->setColor(color);
	er->setWidth(width);
	er->drawPlusSide(plus);
	er->drawMinusSide(minus);
	er->drawThroughSymbol(through);
	er->setXDataOffset(xOffset);
	er->setYDataOffset(yOffset);

	long errorsID=d_plot->insertCurve(er);
	er->setTitle(errColName);	

	c_type.resize(++n_curves);
	c_type[n_curves-1]=ErrorBars;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = errorsID;

	QString errLabel=xColName+"(X),"+yColName+"(Y),"+errColName;
	if (type == QwtErrorPlotCurve::Horizontal)
		errLabel+="(xErr)";
	else
		errLabel+="(yErr)";

	associations << errLabel;
	updatePlot();
}

int Graph::pieSize()
{
	return pieRay;
}

Qt::BrushStyle Graph::pieBrushStyle()
{
	Qt::BrushStyle style=Qt::SolidPattern;
	if (piePlot)
	{
		QwtPieCurve *pieCurve = (QwtPieCurve *)curve(0);
		if (pieCurve->pattern() != style)
			style=pieCurve->pattern();
	}	
	return 	style;
}

int Graph::pieFirstColor()
{
	int first=0;
	if (piePlot)
	{
		QwtPieCurve *pieCurve = (QwtPieCurve *)curve(0);
		if (pieCurve->first() != first)
			first=pieCurve->first();
	}	
	return 	first;
}

QPen Graph::pieCurvePen()
{
	QPen pen=QPen(QColor(Qt::black),1,Qt::SolidLine);
	if (piePlot)
	{
		QwtPieCurve *pieCurve = (QwtPieCurve *)curve(0);
		if (pieCurve->pen() != pen)
			pen=pieCurve->pen();
	}	
	return 	pen;
}

void Graph::updatePie(const QPen& pen, const Qt::BrushStyle &brushStyle, int size, int firstColor)
{
	if (curves()>0)
	{
		QwtPieCurve *pieCurve=(QwtPieCurve*)curve(0);
		if (pieCurve)
		{
			pieCurve->setPen(pen);
			pieCurve->setRay(size);
			pieCurve->setBrushStyle(brushStyle);
			pieCurve->setFirstColor(firstColor);
			d_plot->replot();

			pieRay=size;
			emit modifiedGraph();
		}
	}
}

void Graph::plotPie(QwtPieCurve* curve)
{
	int n=curve->dataSize();
	double *dat = new double[n];
	for (int i=0; i<n; i++)
		dat[i]=curve->y(i);

	QwtPieCurve *pieCurve=new QwtPieCurve(d_plot,0);
	pieCurve->setData(dat, dat, n);

	long curveID = d_plot->insertCurve(pieCurve);
	delete[] dat;

	c_keys.resize(++n_curves);
	c_keys[n_curves-1] = curveID;

	c_type.resize(n_curves);
	c_type[n_curves-1] = Pie;

	pieCurve->setPen(curve->pen());
	pieCurve->setTitle(curve->title());
	pieRay=curve->ray();
	pieCurve->setRay(pieRay);
	pieCurve->setBrushStyle(curve->pattern());
	pieCurve->setFirstColor(curve->first());
	piePlot=TRUE;

	associations<<curve->title().text();
}


void Graph::plotPie(Table* w, const QString& name,const QPen& pen, int brush, int size, int firstColor)
{
	associations<<name;
	int ycol = w->colIndex(name);
	Q3MemArray<double> Y(1);
	int it=0;
	for (int i = 0; i<w->tableRows(); i++ )
	{
		QString yval=w->text(i,ycol);
		if (!yval.isEmpty())
		{
			it++;
			Y.resize(it);
			Y[it-1]=yval.toDouble();
		}
	}
	QwtPieCurve *pieCurve=new QwtPieCurve(d_plot,0);
	pieCurve->setData(Y, Y, it);

	long curveID = d_plot->insertCurve(pieCurve);
	c_keys.resize(++n_curves);
	c_keys[n_curves-1] = curveID;

	c_type.resize(n_curves);
	c_type[n_curves-1] = Pie;

	pieCurve->setTitle(name);
	pieCurve->setPen(pen);
	pieCurve->setRay(size);
	pieCurve->setFirstColor(firstColor);	
	pieCurve->setBrushStyle(getBrushStyle(brush));
	piePlot=TRUE;
	pieRay=size;
}

void Graph::plotPie(Table* w, const QString& name)
{
	int ycol = w->colIndex(name);
	int i, it=0;
	double sum=0.0;
	Q3MemArray<double> Y(1);	
	for (i = 0; i<w->tableRows(); i++ )
	{
		QString yval=w->text(i,ycol);
		if (!yval.isEmpty())
		{
			it++;
			Y.resize(it);
			Y[it-1]=yval.toDouble();
			sum+=Y[it-1];
		}
	}
	if (!it)
		return;

	associations<<name;

	QwtPlotLayout *pLayout=d_plot->plotLayout();
	pLayout->activate(d_plot, d_plot->rect(), 0);
	const QRect rect=pLayout->canvasRect();

	QwtPieCurve *pieCurve = new QwtPieCurve(d_plot,0);
	pieCurve->setData(Y, Y, it);
	long curveID = d_plot->insertCurve(pieCurve);
	pieCurve->setTitle(name);

	c_keys.resize(++n_curves);
	c_keys[n_curves-1] = curveID;

	c_type.resize(n_curves);
	c_type[n_curves-1] = Pie;

	const int ray = 125;
	int xc=int(rect.width()*0.5+10);
	int yc=int(rect.y()+rect.height()*0.5);

	double PI=4*atan(1.0);
	double angle = 90;	

	for (i = 0;i<it;i++ )
	{
		const double value = Y[i]/sum*360;
		double alabel= (angle-value*0.5)*PI/180.0;	

		const int x=int(xc+ray*cos(alabel));
		const int y=int(yc-ray*sin(alabel));

		LegendMarker* aux= new LegendMarker(d_plot);
		aux->setOrigin(QPoint(x,y));
		aux->setBackground(0);
		aux->setText(QString::number(Y[i]/sum*100,'g',2)+"%");	
		d_plot->insertMarker(aux);

		angle -= value;
	}

	piePlot=TRUE;

	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
		{
			QString text="";
			for (i=0;i<it;i++)
			{
				text+="\\p{";
				text+=QString::number(i+1);
				text+="} ";
				text+=QString::number(i+1);
				text+="\n";
			}
			mrk->setText(text);
		}		
	}

	for (i=0;i<QwtPlot::axisCnt;i++)
		d_plot->enableAxis(i,FALSE);

	d_plot->setTitle(QString::null);

	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();  
	canvas->setLineWidth(1);

	scalePicker->refresh();
	d_plot->replot();
	updateScale();
}

void Graph::insertCurve(QwtPlotCurve *c, const QString& plotAssociation)
{
	int curveID = d_plot->insertCurve(c);

	c_type.resize(++n_curves);
	c_type[n_curves-1]=Line;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = curveID;

	addLegendItem(c->title().text());	
	associations << plotAssociation;
}

bool Graph::insertCurvesList(Table* w, const QStringList& names, int style, int lWidth, int sSize)
{
	if (style==Graph::Pie)
		plotPie(w,names[0]);
	else if (style == Box)
		plotBoxDiagram(w, names);
	else if (style==Graph::VectXYXY || style==Graph::VectXYAM)
		plotVectorCurve(w, names, style);
	else
	{		
		int curves = (int)names.count();
		for (int i=0; i<curves; i++)
		{
			if (insertCurve(w, names[i],style))
			{
				CurveLayout cl = initCurveLayout(i, curves, style);
				cl.sSize = sSize;
				cl.lWidth = lWidth;
				updateCurveLayout(i, &cl);
			}
			else
				return false;
		}
	}
	updatePlot();
	return true;
}

bool Graph::insertCurve(Table* w, const QString& name, int style)
{//provided for convenience
	int ycol = w->colIndex(name);
	int xcol = w->colX(ycol);

	bool succes = insertCurve(w, w->colName(xcol), name, style);
	if (succes)
		emit modifiedGraph();
	return succes;
}

bool Graph::insertCurve(Table* w, int xcol, const QString& name, int style)
{//provided for convenience
	return insertCurve(w, w->colName(xcol), name, style);
}

bool Graph::insertCurve(Table* w, const QString& xColName, const QString& yColName, int style)
{
	int xcol=w->colIndex(xColName);
	int ycol=w->colIndex(yColName);
	if (xcol < 0 || ycol < 0)
		return false;

	int colPlotDesignation = w->colPlotDesignation(ycol);
	if (colPlotDesignation == Table::xErr || colPlotDesignation == Table::yErr)
	{//add error bars
		ycol = w->colY(ycol);
		if (colPlotDesignation == Table::xErr)
			addErrorBars(w, xColName, w->colName(ycol), w, yColName, (int)QwtErrorPlotCurve::Horizontal);
		else
			addErrorBars(w, xColName, w->colName(ycol), w, yColName);
		return true;
	}

	int xColType = w->columnType(xcol);
	int yColType = w->columnType(ycol);

	Q3MemArray<double> X(1),Y(1);
	int i, it=0;
	QStringList xLabels, yLabels;// store text labels
	QTime time0;
	QDate date;
	int r=w->tableRows();
	if (xColType == Table::Time)
	{
		for (i = 0; i<r; i++ )
		{
			QString xval=w->text(i,xcol);
			if (!xval.isEmpty())
			{
				time0 = QTime::fromString (xval, Qt::TextDate);
				if (time0.isValid())
					break;
			}
		}
	}
	else if (xColType == Table::Date)
	{
		for (i = 0; i<r; i++ )
		{
			QString xval=w->text(i,xcol);
			if (!xval.isEmpty())
			{
				date = QDate::fromString (xval, Qt::ISODate);
				if (date.isValid())
					break;
			}
		}
	}

	for (i = 0; i<r; i++ )
	{
		QString xval=w->text(i,xcol);
		QString yval=w->text(i,ycol);
		if (!xval.isEmpty() && !yval.isEmpty())
		{
			it++;
			X.resize(it);
			Y.resize(it);
			if (xColType == Table::Text)
			{
				if (xLabels.contains(xval) == 0)
					xLabels<<xval;
				X[it-1]=(double)(xLabels.findIndex(xval)+1);
			}
			else if (xColType == Table::Time)
			{
				QTime time = QTime::fromString (xval, Qt::TextDate);
				if (time.isValid())
					X[it-1]= time0.msecsTo (time);
				else
					X[it-1]= 0;
			}
			else if (xColType == Table::Date)
			{
				QDate d = QDate::fromString (xval, Qt::ISODate);
				if (d.isValid())
					X[it-1]= (double) date.daysTo (d);
			}
			else
				X[it-1]=xval.toDouble();

			if (yColType == Table::Text)
			{
				yLabels<<yval;
				Y[it-1]=(double)it;
			}
			else
				Y[it-1]=yval.toDouble();
		}
	}

	if (!it)
		return false;

	associations << xColName+"(X),"+yColName+"(Y)";

	c_type.resize(++n_curves);
	c_type[n_curves-1] = style;

	long curveID=-1;
	QwtPlotCurve *c = 0;
	if (style == VerticalBars)
	{
		c = new QwtBarCurve(QwtBarCurve::Vertical, d_plot,0);
		curveID = d_plot->insertCurve(c);
		c->setStyle(QwtPlotCurve::UserCurve);
		c->setTitle(yColName);
	}
	else if (style == HorizontalBars)
	{
		c = new QwtBarCurve(QwtBarCurve::Horizontal, d_plot,0);
		curveID = d_plot->insertCurve(c);
		c->setStyle(QwtPlotCurve::UserCurve);
		c->setTitle(yColName);
	}
	else if (style == Histogram)
	{
		c = new QwtHistogram(d_plot,0);
		curveID = d_plot->insertCurve(c);
		c->setStyle(QwtPlotCurve::UserCurve);
		c->setTitle(yColName);
	}
	else
	{
		c = new QwtPlotCurve(yColName);
		curveID = d_plot->insertCurve(c);
	}

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = curveID;

	c->setPen(QPen(Qt::black,widthLine)); 

	if (style == Histogram)
		initHistogram(curveID, Y, it);
	else if (style == HorizontalBars)
		c->setData(Y, X, it);
	else
		c->setData(X, Y, it);

	if (xColType == Table::Text )
	{
		if (style == HorizontalBars)
		{
			axesFormatInfo[QwtPlot::yLeft] = xColName;
			axesFormatInfo[QwtPlot::yRight] = xColName;
			axisType[QwtPlot::yLeft] = Txt;
			d_plot->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(xLabels));
		}
		else
		{
			axesFormatInfo[QwtPlot::xBottom] = xColName;
			axesFormatInfo[QwtPlot::xTop] = xColName;
			axisType[QwtPlot::xBottom] = Txt;
			d_plot->setAxisScaleDraw (QwtPlot::xBottom, new QwtTextScaleDraw(xLabels));
		}
	}
	else if (xColType == Table::Time )
	{
		QString fmtInfo = time0.toString(Qt::TextDate) + ";" + w->columnFormat(xcol); 
		if (style == HorizontalBars)
			setLabelsDateTimeFormat(QwtPlot::yLeft, Time, fmtInfo);
		else
			setLabelsDateTimeFormat(QwtPlot::xBottom, Time, fmtInfo);
	}
	else if (xColType == Table::Date )
	{
		QString fmtInfo = date.toString(Qt::ISODate ) + ";yyyy-MM-dd";
		if (style == HorizontalBars)
			setLabelsDateTimeFormat(QwtPlot::yLeft, Date, fmtInfo);
		else
			setLabelsDateTimeFormat(QwtPlot::xBottom, Date, fmtInfo);
	}

	if (yColType == Table::Text)
	{
		axesFormatInfo[QwtPlot::yLeft] = yColName;
		axesFormatInfo[QwtPlot::yRight] = yColName;
		axisType[QwtPlot::yLeft] = Txt;
		d_plot->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(yLabels));
	}

	addLegendItem(yColName);
	updatePlot();
	return true;
}

void Graph::plotVectorCurve(Table* w, const QStringList& colList, int style)
{
	int xcol=w->colIndex(colList[0]);
	int ycol=w->colIndex(colList[1]);
	int endXCol=w->colIndex(colList[2]);
	int endYCol=w->colIndex(colList[3]);	
	Q3MemArray<double> X(1), Y(1), X2(1), Y2(1);
	int it=0;
	for (int i = 0;i<w->tableRows();i++ )
	{
		QString xval=w->text(i,xcol);
		QString yval=w->text(i,ycol);
		QString xend=w->text(i,endXCol);
		QString yend=w->text(i,endYCol);
		if (!xval.isEmpty() && !yval.isEmpty() && !xend.isEmpty() && !yend.isEmpty())
		{
			it++;
			X.resize(it); Y.resize(it); X2.resize(it); Y2.resize(it);
			Y[it-1]=yval.toDouble();
			X[it-1]=xval.toDouble();
			Y2[it-1]=yend.toDouble();
			X2[it-1]=xend.toDouble();
		}
	}

	if (!it)
		return;

	VectorCurve *v = 0;
	if (style == VectXYAM)
		v = new VectorCurve(VectorCurve::XYAM, d_plot, 0);
	else
		v = new VectorCurve(VectorCurve::XYXY, d_plot, 0);

	if (!v)
		return;

	v->setVectorEnd(X2, Y2);

	QString label=colList[1];
	if (style == VectXYAM)
		associations<<colList[0]+"(X),"+colList[1]+"(Y),"+
			colList[2]+"(A),"+colList[3]+"(M)";
	else
		associations<<colList[0]+"(X),"+colList[1]+"(Y),"+
			colList[2]+"(X),"+colList[3]+"(Y)";

	c_type.resize(++n_curves);
	c_type[n_curves-1]=style;

	long curveID = d_plot->insertCurve(v);
	c_keys.resize(n_curves);
	c_keys[n_curves-1] = curveID;

	v->setStyle(QwtPlotCurve::NoCurve); 
	v->setTitle(label);
	v->setData(X, Y, it);

	addLegendItem(label);	
	updatePlot();
}

void Graph::updateVectorsLayout(Table *w, int curve, int colorIndex, int width, 
		int arrowLength, int arrowAngle, bool filled, int position,
		const QString& xEndColName, const QString& yEndColName)
{
	VectorCurve *vect=(VectorCurve *)this->curve(curve);
	if (!vect)
		return;

	QColor c = color(colorIndex);
	QStringList cols=QStringList::split(",", associations[curve], false);
	if (vect->width() == width && vect->color() == c &&
			vect->headLength() == arrowLength && vect->headAngle() == arrowAngle &&
			vect->position() == position &&
			vect->filledArrowHead() == filled && cols[2] == xEndColName && cols[3] == yEndColName)
		return;

	vect->setColor(c);
	vect->setWidth(width);
	vect->setHeadLength(arrowLength);
	vect->setHeadAngle(arrowAngle);
	vect->fillArrowHead(filled);
	vect->setPosition(position);

	if (cols[2] != xEndColName || cols[3] != yEndColName)
	{
		int r=w->tableRows();
		QString aux = cols[0];
		int xcol=w->colIndex(aux.remove("(X)"));
		aux = cols[1];
		int ycol=w->colIndex(aux.remove("(Y)"));
		int endXCol=w->colIndex(xEndColName);
		int endYCol=w->colIndex(yEndColName);
		int size=vect->dataSize();
		Q3MemArray<double> X(size), Y(size);
		int i,it=0;
		for (i = 0;i<r;i++ )
		{
			QString xval=w->text(i,xcol);
			QString yval=w->text(i,ycol);
			QString xend=w->text(i,endXCol);
			QString yend=w->text(i,endYCol);
			if (!xval.isEmpty() && !yval.isEmpty() && !xend.isEmpty() && !yend.isEmpty())
			{
				it++;
				X[it-1]=xend.toDouble();
				Y[it-1]=yend.toDouble();
			}
		}
		vect->setVectorEnd(X,Y);
		if (c_type[curve] == VectXYXY)
		{
			cols[2]=xEndColName+"(X)";
			cols[3]=yEndColName+"(Y)";
		}
		else
		{
			cols[2]=xEndColName+"(A)";
			cols[3]=yEndColName+"(M)";
		}
		associations[curve]=cols.join(",");
	}

	d_plot->replot();
	emit modifiedGraph(); 
}

void Graph::setVectorsLook(int curve, const QColor& c, int width, int arrowLength,
		int arrowAngle, bool filled, int position)
{
	VectorCurve *vect=(VectorCurve *)this->curve(curve);
	if (!vect)
		return;

	vect->setColor(c);
	vect->setWidth(width);
	vect->setHeadLength(arrowLength);
	vect->setHeadAngle(arrowAngle);
	vect->fillArrowHead(filled);
	vect->setPosition(position);
}

void Graph::updatePlot()
{
	if (autoscale && !removePointsEnabled && !movePointsEnabled && 
			!zoomOn() && !rangeSelectorsEnabled)
	{
		for (int i = 0; i < QwtPlot::axisCnt; i++)
			d_plot->setAxisAutoScale(i);
	}

	d_plot->replot();
	d_zoomer[0]->setZoomBase();
	d_zoomer[1]->setZoomBase();	
}

void Graph::updateScale()
{
	const QwtScaleDiv *scDiv=d_plot->axisScaleDiv(QwtPlot::xBottom);
	QwtValueList lst = scDiv->ticks (QwtScaleDiv::MajorTick);

	double step = fabs(lst[1]-lst[0]);

	if (!autoscale)
		d_plot->setAxisScale (QwtPlot::xBottom, scDiv->lBound(), scDiv->hBound(), step);

	scDiv=d_plot->axisScaleDiv(QwtPlot::yLeft);
	lst = scDiv->ticks (QwtScaleDiv::MajorTick);

	step = fabs(lst[1]-lst[0]);

	if (!autoscale)
		d_plot->setAxisScale (QwtPlot::yLeft, scDiv->lBound(), scDiv->hBound(), step);

	d_plot->replot();

	updateMarkersBoundingRect();

	updateSecondaryAxis(QwtPlot::xTop);
	updateSecondaryAxis(QwtPlot::yRight);

	d_plot->replot();//TO DO: avoid 2nd replot!
}

void Graph::setBarsGap(int curve, int gapPercent, int offset)
{
	QwtBarCurve *bars=(QwtBarCurve *)this->curve(curve);
	if (!bars || (bars->gap() == gapPercent && bars->offset() == offset))
		return;

	bars->setGap(gapPercent);
	bars->setOffset(offset);

	QString yColName = bars->title().text();
	for (int i=0; i<n_curves; i++)
	{
		if (associations[i].contains(yColName) && i != curve && c_type[i] == ErrorBars)
		{
			QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(i);
			if (!er)
				continue;

			if (bars->orientation() == QwtBarCurve::Vertical)
				er->setXDataOffset(bars->dataOffset());	
			else
				er->setYDataOffset(bars->dataOffset());	
		}
	}
}

void Graph::removePie()
{	
	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
			mrk->setText(QString::null);
	}

	d_plot->removeCurve(c_keys[0]);
	d_plot->replot();

	c_keys.resize(0);
	c_type.resize(0);
	n_curves=0;
	associations.clear();
	piePlot=FALSE;		
	emit modifiedGraph();
}

/*
 *provided for convenience
 */
void Graph::removeCurve(const QString& s)
{	
	int index = associations.findIndex(s);
	removeCurve(index);
}

void Graph::removeCurve(int index)
{		
	resetErrorBarsOffset(index);

	associations.removeAt(index);

	removeLegendItem(index);

	d_plot->removeCurve(c_keys[index]);
	n_curves--;

	if (piePlot)
	{
		piePlot=FALSE;	
		c_keys.resize(n_curves);
	}
	else
	{
		if (rangeSelectorsEnabled && c_keys[index] == selectedCurve)
		{
			if (n_curves>0)
				shiftCurveSelector(TRUE);
			else
			{
				d_plot->removeMarker(startID);
				d_plot->removeMarker(endID);
				startID=-1;
				endID=-1;
				endPoint=-1;
				selectedCurve=-1;
				selectedPoint=-1;
				selectedCursor=-1;	
			}			
		}

		for (int i=index; i<n_curves; i++)
		{
			c_type[i] = c_type[i+1];
			c_keys[i] = c_keys[i+1];
		}

		c_type.resize(n_curves);
		c_keys.resize(n_curves);
	}
	updatePlot();
	emit modifiedGraph();	
}

void Graph::resetErrorBarsOffset(int index)
{
	int curveType = c_type[index];
	if (curveType != VerticalBars && curveType != Histogram && curveType != HorizontalBars)
		return;

	QwtBarCurve *bars=(QwtBarCurve *)this->curve(index);
	if (!bars || !bars->offset())
		return;

	QString yColName = bars->title().text();
	for (int i=0; i<n_curves; i++)
	{
		if (associations[i].contains(yColName) && i != index && c_type[i] == ErrorBars)
		{
			QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(i);
			if (!er)
				continue;

			if (bars->orientation() == QwtBarCurve::Vertical)
				er->setXDataOffset(0);	
			else
				er->setYDataOffset(0);	
		}
	}
}

void Graph::removeLegendItem(int index)
{
	if (legendMarkerID<0 || c_type[index] == ErrorBars)
		return;

	LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
	if (!mrk)
		return;

	if (piePlot)
	{
		mrk->setText(QString::null);
		return;
	}

	QString text=mrk->getText();
	QStringList items=QStringList::split( "\n", text, FALSE);

	if (index >= (int) items.count())
		return;

	QStringList l = items.grep( "\\c{" + QString::number(index+1) + "}" );
	items.remove(l[0]);//remove the corresponding legend string

	int cv=0;
	for (int i=0; i< (int)items.count(); i++)
	{//set new curves indexes in legend text
		QString item = (items[i]).stripWhiteSpace();
		if (item.startsWith("\\c{", true))
		{
			item.remove(0, item.find("}", 0));
			item.prepend("\\c{"+QString::number(++cv));
		}
		items[i]=item;
	}
	text=items.join ( "\n" ) + "\n";
	mrk->setText(text);
}

void Graph::addLegendItem(const QString& colName)
{
	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
		{
			QString text=mrk->getText();
			if (text.endsWith ( "\n", true ) )
				text.append("\\c{"+QString::number(curves())+"}"+colName+"\n");
			else
				text.append("\n\\c{"+QString::number(curves())+"}"+colName+"\n");

			mrk->setText(text);
		}
	}
}

void Graph::contextMenuEvent(QContextMenuEvent *e)
{
	if (selectedMarker>=0)
		return;

	QPoint pos = d_plot->canvas()->mapFrom(d_plot, e->pos());
	int dist, point;
	const long curve = d_plot->closestCurve(pos.x(), pos.y(), dist, point);
	const QwtPlotCurve *c = d_plot->curve(curve);

	if (c && dist < 10)//10 pixels tolerance
		emit showCurveContextMenu(curve);
	else
		emit showContextMenu();

	e->accept();
}

void Graph::closeEvent(QCloseEvent *e)
{
	emit closedGraph();
	e->accept();

}

bool Graph::zoomOn()
{
	return (d_zoomer[0]->isEnabled() || d_zoomer[1]->isEnabled());
}

void Graph::zoomed (const QwtDoubleRect &rect)
{	
	emit modifiedGraph();
}

void Graph::zoom(bool on)
{
	d_zoomer[0]->setEnabled(on);
	d_zoomer[1]->setEnabled(on);

	QCursor cursor=QCursor (QPixmap(lens_xpm),-1,-1);
	if (on)
		d_plot->canvas()->setCursor(cursor);
	else 
		d_plot->canvas()->setCursor(Qt::arrowCursor);
}

void Graph::zoomOut()
{
	d_zoomer[0]->zoom(-1);
	d_zoomer[1]->zoom(-1);
}

void Graph::drawText(bool on)
{
	QCursor c=QCursor(Qt::IBeamCursor);
	if (on)
		d_plot->canvas()->setCursor(c);
	else
		d_plot->canvas()->setCursor(Qt::arrowCursor);

	drawTextOn=on;
}

void Graph::insertImageMarker(ImageMarker* mrk)
{
	QPixmap photo = mrk->image();
	ImageMarker* mrk2= new ImageMarker(photo);

	int imagesOnPlot=d_images.size();
	d_images.resize(++imagesOnPlot);
	d_images[imagesOnPlot-1]=d_plot->insertMarker(mrk2);

	mrk2->setFileName(mrk->getFileName());
	mrk2->setBoundingRect(mrk->boundingRect());
}

void Graph::insertImageMarker(const QPixmap& photo, const QString& fileName)
{
	ImageMarker* mrk= new ImageMarker(photo);
	long mrkID=d_plot->insertMarker(mrk);

	int imagesOnPlot=d_images.size();
	d_images.resize(++imagesOnPlot);
	d_images[imagesOnPlot-1]=mrkID;

	mrk->setFileName(fileName);
	mrk->setOrigin(QPoint(0,0));

	QSize picSize=photo.size();
	int w = d_plot->canvas()->width();
	if (picSize.width()>w)
		picSize.setWidth(w);
	int h=d_plot->canvas()->height();
	if (picSize.height()>h)
		picSize.setHeight(h);

	mrk->setSize(picSize);
	d_plot->replot();

	emit modifiedGraph();
}

void Graph::insertImageMarker(const QStringList& lst, int fileVersion)
{
	QString fn=lst[1];
	QFile f(fn);
	if (!f.exists())
	{ 
		QMessageBox::warning(0, tr("QtiPlot - File open error"), 
				tr("Image file: <p><b> %1 </b><p>does not exist anymore!").arg(fn));
	}
	else
	{
		QPixmap photo;		
		if (fn.contains(".jpg", false))
			photo.load(fn,"JPEG",QPixmap::Auto);
		else
		{
			QFileInfo fi(fn);
			QString baseName = fi.fileName();
			int pos=baseName.find(".",0);
			QString type=baseName.right(baseName.length()-pos-1);
			photo.load(fn,type.upper(),QPixmap::Auto);
		}

		ImageMarker* mrk = new ImageMarker(photo);
		long mrkID=d_plot->insertMarker(mrk);
		mrk->setFileName(fn);

		if (fileVersion < 86)
		{
			mrk->setOrigin(QPoint(lst[2].toInt(),lst[3].toInt()));
			mrk->setSize(QSize(lst[4].toInt(),lst[5].toInt()));
		}
		else
		{
			mrk->setBoundingRect(QwtDoubleRect(lst[2].toDouble(),lst[3].toDouble(),
						lst[4].toDouble(),lst[5].toDouble()));
		}

		int imagesOnPlot=d_images.size();
		d_images.resize(++imagesOnPlot);
		d_images[imagesOnPlot-1]=mrkID;
	}
}

void Graph::drawLine(bool on, bool arrow)
{
	drawLineOn=on;
	drawArrowOn=arrow;
	if (!on)
		emit drawLineEnded(true);
}

void Graph::modifyFunctionCurve(int curve, int type, const QStringList &formulas,
		const QString& var,QList<double> &ranges, int points)
{
	FunctionCurve *c = (FunctionCurve *)this->curve(curve);
	if (!c)
		return;

	if (c->functionType() == type &&
			c->variable() == var &&
			c->formulas() == formulas &&
			c->startRange() == ranges[0] &&
			c->endRange() == ranges[1] &&
			c->dataSize() == points)
		return;

	bool error=FALSE;
	Q3MemArray<double> X(points),Y(points);
	double step=(ranges[1]-ranges[0])/(double) (points-1);
	QString oldLegend = c->legend();
	if (type == FunctionCurve::Normal)
	{
		MyParser parser;
		double x;
		try
		{	
			parser.DefineVar(var.ascii(), &x);	
			parser.SetExpr(formulas[0].ascii());
			X[0]=ranges[0];x=ranges[0];Y[0]=parser.Eval();
			for (int i = 1; i<points; i++ )
			{
				x+=step;
				X[i]=x;
				Y[i]=parser.Eval();
			}
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(this,tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
			error=TRUE;	
		}
	}	
	else if (type == FunctionCurve::Parametric || type == FunctionCurve::Polar)
	{
		QStringList aux = formulas;
		if (type == FunctionCurve::Polar)
		{
			QString swap=aux[0];
			aux[0]="("+swap+")*cos("+aux[1]+")";
			aux[1]="("+swap+")*sin("+aux[1]+")";
		}

		MyParser xparser;
		MyParser yparser;
		double par;
		try
		{
			xparser.DefineVar(var.ascii(), &par);	
			yparser.DefineVar(var.ascii(), &par);	
			xparser.SetExpr(aux[0].ascii());
			yparser.SetExpr(aux[1].ascii());
			par=ranges[0];
			for (int i = 0; i<points; i++ )
			{
				X[i]=xparser.Eval();
				Y[i]=yparser.Eval();
				par+=step;
			}
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,tr("QtiPlot - Input function error"),QString::fromStdString(e.GetMsg()));
			error=TRUE;	
		}
	}		

	if (error)
		return;

	c->setData(X, Y, points);
	c->setFunctionType((FunctionCurve::FunctionType)type);
	c->setRange(ranges[0], ranges[1]);
	c->setFormulas(formulas);
	c->setVariable(var);

	if (legendMarkerID >= 0)
	{//update the legend marker	
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
		{
			QString text = (mrk->getText()).replace(oldLegend, c->legend());
			mrk->setText(text);
		}
	}
	updatePlot();
	emit modifiedGraph();
	emit modifiedFunction();
}

void Graph::addFunctionCurve(int type, const QStringList &formulas, const QString &var,
		QList<double> &ranges, int points, const QString& title)
{
	bool error=FALSE;
	QVarLengthArray<double> X(points), Y(points);
	QString name;
	++d_functions;
	if (!title.isEmpty())
		name = title;
	else
		name = "F" + QString::number(d_functions);

	double step=(ranges[1]-ranges[0])/(double) (points-1);

	if (type == FunctionCurve::Normal)
	{
		MyParser parser;
		double x;
		try
		{	
			parser.DefineVar(var.ascii(), &x);	
			parser.SetExpr(formulas[0].ascii());

			X[0]=ranges[0];x=ranges[0];Y[0]=parser.Eval();
			for (int i = 1; i<points; i++ )
			{
				x+=step;
				X[i]=x;
				Y[i]=parser.Eval();
			}
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(this, tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
			error=TRUE;	
		}
	}	
	else if (type == FunctionCurve::Parametric || type == FunctionCurve::Polar)
	{
		QStringList aux = formulas;
		MyParser xparser;
		MyParser yparser;
		double par;
		if (type == FunctionCurve::Polar)
		{
			QString swap=aux[0];
			aux[0]="("+swap+")*cos("+aux[1]+")";
			aux[1]="("+swap+")*sin("+aux[1]+")";
		}
		try
		{
			xparser.DefineVar(var.ascii(), &par);	
			yparser.DefineVar(var.ascii(), &par);	
			xparser.SetExpr(aux[0].ascii());
			yparser.SetExpr(aux[1].ascii());
			par=ranges[0];
			for (int i = 0; i<points; i++ )
			{
				X[i]=xparser.Eval();
				Y[i]=yparser.Eval();
				par+=step;
			}
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(this, tr("QtiPlot - Input function error"), QString::fromStdString(e.GetMsg()));
			error=TRUE;	
		}
	}		

	if (error)
		return;

	associations << name;

	FunctionCurve *c = new FunctionCurve((const FunctionCurve::FunctionType)type, name);
	c->setPen(QPen(QColor(Qt::black), widthLine)); 
	c->setData(X.data(), Y.data(), points);
	c->setRange(ranges[0], ranges[1]);
	c->setFormulas(formulas);
	c->setVariable(var);

	int curveID = d_plot->insertCurve(c);

	c_type.resize(++n_curves);
	c_type[n_curves-1]=Line;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = curveID;

	addLegendItem(c->legend());
	updatePlot();
	emit modifiedGraph();
}

void Graph::insertFunctionCurve(const QString& formula, int points, int fileVersion)
{
	int type;
	QStringList formulas;
	QString var, name = QString::null;	
	QList<double> ranges;

	QStringList curve = QStringList::split (",",formula,TRUE);
	if (fileVersion < 87)
	{
		if (curve[0][0]=='f')
		{
			type = FunctionCurve::Normal;
			formulas += curve[0].section('=',1,1);
			var = curve[1];
			ranges += curve[2].toDouble();
			ranges += curve[3].toDouble();
		}
		else if (curve[0][0]=='X')
		{
			type = FunctionCurve::Parametric;
			formulas += curve[0].section('=',1,1);
			formulas += curve[1].section('=',1,1);	
			var = curve[2];
			ranges += curve[3].toDouble();
			ranges += curve[4].toDouble();
		}
		else if (curve[0][0]=='R')
		{
			type = FunctionCurve::Polar;
			formulas += curve[0].section('=',1,1);
			formulas += curve[1].section('=',1,1);	
			var = curve[2];
			ranges += curve[3].toDouble();
			ranges += curve[4].toDouble();
		}
	}
	else
	{
		type = curve[0].toInt();
		name = curve[1];

		if (type == FunctionCurve::Normal)
		{
			formulas << curve[2];
			var = curve[3];
			ranges += curve[4].toDouble();
			ranges += curve[5].toDouble();
		}
		else if (type == FunctionCurve::Polar || type == FunctionCurve::Parametric)
		{
			formulas << curve[2];
			formulas << curve[3];
			var = curve[4];
			ranges += curve[5].toDouble();
			ranges += curve[6].toDouble();
		}	
	}
	addFunctionCurve(type, formulas, var, ranges, points, name);	 
}

void Graph::createWorksheet(const QString& name)
{
	Q3ValueList<int> keys = d_plot->curveKeys();
	for (int i=0; i<n_curves; i++)	
	{
		QwtPlotCurve *cv = (QwtPlotCurve *)d_plot->curve(keys[i]);
		if (cv->title().text() == name)
		{
			int size= cv->dataSize();
			QString text="1\t2\n";
			for (int j=0;j<size;j++)	
			{
				text+=QString::number(cv->x(j))+"\t";
				text+=QString::number(cv->y(j))+"\n";
			}
			emit createTable("table1",size,2, text);
			return;
		}
	}
}

bool Graph::lineProfile()
{
	return lineProfileOn;	
}

QString Graph::saveToString()
{
	QString s="<graph>\n";			
	s+="ggeometry\t";
	QPoint p=this->pos();
	s+=QString::number(p.x())+"\t";
	s+=QString::number(p.y())+"\t";
	s+=QString::number(this->frameGeometry().width())+"\t";
	s+=QString::number(this->frameGeometry().height())+"\n";
	s+=saveTitle();
	s+="Background\t"+ d_plot->paletteBackgroundColor().name()+"\n";
	s+="Margin\t"+QString::number(d_plot->margin())+"\n";
	s+="Border\t"+QString::number(d_plot->lineWidth())+"\t"+d_plot->frameColor().name()+"\n";
	s+=saveGridOptions();
	s+=saveEnabledAxes();
	s+="AxesTitles\t"+saveScaleTitles();
	s+=saveAxesTitleColors();
	s+=saveAxesTitleAlignement();
	s+=saveFonts();
	s+=saveEnabledTickLabels();
	s+=saveAxesColors();
	s+=saveAxesBaseline();
	s+=saveCanvas();
	s+=saveLabelsRotation();
	s+=saveCurves();			
	s+=saveErrorBars();
	s+=saveScale();
	s+=saveAxesFormulas();
	s+=saveLabelsFormat();
	s+=saveAxesLabelsType();
	s+=saveTicksType();
	s+="TicksLength\t"+QString::number(minorTickLength())+"\t"+QString::number(majorTickLength())+"\n";
	s+="DrawAxesBackbone\t"+QString::number(drawAxesBackbone)+"\n";
	s+="AxesLineWidth\t"+QString::number(d_plot->axesLinewidth())+"\n";
	s+=saveMarkers();
	s+="</graph>\n";
	return s;
}

QString Graph::saveAsTemplate() 
{
	QString s="<graph>\n";			
	s+="ggeometry\t";
	QPoint p=this->pos();
	s+=QString::number(p.x())+"\t";
	s+=QString::number(p.y())+"\t";
	s+=QString::number(this->frameGeometry().width())+"\t";
	s+=QString::number(this->frameGeometry().height())+"\n";
	s+=saveTitle();
	s+="Background\t"+ d_plot->paletteBackgroundColor().name()+"\n";
	s+="Margin\t"+QString::number(d_plot->margin())+"\n";
	s+="Border\t"+QString::number(d_plot->lineWidth())+"\t"+d_plot->frameColor().name()+"\n";
	s+=saveGridOptions();
	s+=saveEnabledAxes();
	s+="AxesTitles\t"+saveScaleTitles();
	s+=saveAxesTitleColors();
	s+=saveAxesTitleAlignement();
	s+=saveFonts();
	s+=saveEnabledTickLabels();
	s+=saveAxesColors();
	s+=saveAxesBaseline();
	s+=saveCanvas();
	s+=saveLabelsRotation();
	s+=saveScale();
	s+=saveAxesFormulas();
	s+=saveLabelsFormat();
	s+=saveAxesLabelsType();
	s+=saveTicksType();
	s+="TicksLength\t"+QString::number(minorTickLength())+"\t"+QString::number(majorTickLength())+"\n";
	s+="DrawAxesBackbone\t"+QString::number(drawAxesBackbone)+"\n";
	s+="AxesLineWidth\t"+QString::number(d_plot->axesLinewidth())+"\n";
	s+=saveMarkers();
	s+="</graph>\n";
	return s;
}

void Graph::showIntensityTable()
{	
	ImageMarker* mrk=(ImageMarker*) d_plot->marker(selectedMarker);
	if (!mrk)
		return;

	QPixmap pic=mrk->image();
	emit createIntensityTable(pic);
}

void Graph::updateMarkersBoundingRect()
{
	Q3ValueList<int> texts=textMarkerKeys();

	int i=0;
	for (i=0;i<(int)d_lines.size();i++)
	{			
		LineMarker* mrkL=(LineMarker*)d_plot->marker(d_lines[i]);
		if (mrkL)
			mrkL->updateBoundingRect();
	}

	for (i=0;i<(int)texts.size();i++)
	{
		LegendMarker* mrkT = (LegendMarker*) d_plot->marker(texts[i]);
		if (mrkT)
			mrkT->updateOrigin();	
	}

	for (i=0;i<(int)d_images.size();i++)
	{
		ImageMarker* mrk = (ImageMarker*) d_plot->marker(d_images[i]);
		if (mrk)
			mrk->updateBoundingRect();
	}
}

void Graph::resizeEvent ( QResizeEvent *e )
{
	if (ignoreResize || !this->isVisible())
		return;

	if (autoScaleFonts)
	{
		QSize oldSize=e->oldSize();
		QSize size=e->size();

		double ratio=(double)size.height()/(double)oldSize.height();
		scaleFonts(ratio);
	}
}

void Graph::scaleFonts(double factor)
{
	QFont font;
	Q3ValueList<int> texts=textMarkerKeys();
	int i;
	for (i=0;i<(int)texts.size();i++)
	{
		LegendMarker* mrk = (LegendMarker*) d_plot->marker(texts[i]);
		font = mrk->getFont();
		font.setPointSizeFloat(factor*font.pointSizeFloat());
		mrk->setFont(font);	
	}

	for (i = 0; i<QwtPlot::axisCnt; i++)
	{
		font = axisFont(i);
		font.setPointSizeFloat(factor*font.pointSizeFloat());
		d_plot->setAxisFont (i, font);

		font = axisTitleFont(i);
		font.setPointSizeFloat(factor*font.pointSizeFloat());
		d_plot->axisTitle(i).setFont (font);
	}

	font = d_plot->title().font();
	font.setPointSizeFloat(factor*font.pointSizeFloat());
	d_plot->title().setFont(font);

	d_plot->replot();
}

void Graph::changeMargin (int d)
{
	if (d_plot->margin() == d)
		return;

	d_plot->setMargin(d);
	emit modifiedGraph();
}

void Graph::drawBorder (int width, const QColor& color)
{
	if (d_plot->frameColor() == color && width == d_plot->lineWidth())
		return;

	QPalette pal = d_plot->palette();
	pal.setColor(QColorGroup::Foreground, color);

	d_plot->setPalette(pal);
	d_plot->setLineWidth(width);

	int margin = d_plot->margin();
	d_plot->setMargin(margin + 5);
	d_plot->setMargin(margin);

	emit modifiedGraph();
}

void Graph::setBorder (int width, const QColor& color)
{
	if (d_plot->frameColor() == color && width == d_plot->lineWidth())
		return;

	QPalette pal = d_plot->palette();
	pal.setColor(QColorGroup::Foreground, color);

	d_plot->setPalette(pal);
	d_plot->setLineWidth(width);
}

void Graph::setBackgroundColor(const QColor& color)
{
	d_plot->setPaletteBackgroundColor(color);

	QwtTextLabel *title=d_plot->titleLabel ();
	title->setPaletteBackgroundColor(color);

	for (int i=0;i<QwtPlot::axisCnt;i++)
	{
		QwtScaleWidget *scale=(QwtScaleWidget *) d_plot->axisWidget(i);
		if (scale)
			scale->setPaletteBackgroundColor(color);
	}

	emit modifiedGraph();
}

void Graph::setCanvasBackground(const QColor& color)
{
	Q3ValueList<int> texts=textMarkerKeys();
	for (int i=0; i<(int)texts.size(); i++)
	{
		LegendMarker* mrk = (LegendMarker*) d_plot->marker(texts[i]);
		if (d_plot->canvasBackground() == mrk->backgroundColor())
			mrk->setBackgroundColor(color);	
	}
	d_plot->setCanvasBackground(color);
	emit modifiedGraph();
}

Qt::BrushStyle Graph::getBrushStyle(int style)
{
	Qt::BrushStyle brushStyle;
	switch (style)
	{
		case 0:
			brushStyle=Qt::SolidPattern;
			break;
		case 1:
			brushStyle=Qt::HorPattern;
			break;
		case 2:
			brushStyle=Qt::VerPattern;
			break;
		case 3:
			brushStyle=Qt::CrossPattern;
			break;
		case 4:
			brushStyle=Qt::BDiagPattern;
			break;
		case 5:
			brushStyle=Qt::FDiagPattern;
			break;
		case 6:
			brushStyle=Qt::DiagCrossPattern;
			break;
		case 7:
			brushStyle=Qt::Dense1Pattern;
			break;
		case 8:
			brushStyle=Qt::Dense2Pattern;
			break;
		case 9:
			brushStyle=Qt::Dense3Pattern;
			break;
		case 10:
			brushStyle=Qt::Dense4Pattern;
			break;
		case 11:
			brushStyle=Qt::Dense5Pattern;
			break;
		case 12:
			brushStyle=Qt::Dense6Pattern;
			break;
		case 13:
			brushStyle=Qt::Dense7Pattern;
			break;
	}
	return brushStyle;
}

QString Graph::penStyleName(Qt::PenStyle style)
{
	if (style==Qt::SolidLine)
		return "SolidLine";
	else if (style==Qt::DashLine)
		return "DashLine";
	else if (style==Qt::DotLine)
		return "DotLine";	
	else if (style==Qt::DashDotLine)
		return "DashDotLine";		
	else if (style==Qt::DashDotDotLine)
		return "DashDotDotLine";
	else
		return "SolidLine";
}

Qt::PenStyle Graph::getPenStyle(int style)
{
	Qt::PenStyle linePen;
	switch (style)
	{
		case 0:
			linePen=Qt::SolidLine;
			break;
		case 1:
			linePen=Qt::DashLine;
			break;
		case 2:
			linePen=Qt::DotLine;
			break;
		case 3:
			linePen=Qt::DashDotLine;
			break;
		case 4:
			linePen=Qt::DashDotDotLine;
			break;
	}
	return linePen;
}

Qt::PenStyle Graph::getPenStyle(const QString& s)
{
	Qt::PenStyle style;
	if (s=="SolidLine")
		style=Qt::SolidLine;
	else if (s=="DashLine")
		style=Qt::DashLine;
	else if (s=="DotLine")
		style=Qt::DotLine;
	else if (s=="DashDotLine")
		style=Qt::DashDotLine;
	else if (s=="DashDotDotLine")
		style=Qt::DashDotDotLine;
	return style;
}

int Graph::obsoleteSymbolStyle(int type)
{
	if (type <= 4)
		return type+1;
	else
		return type+2;
}

int Graph::curveType(int curveIndex)
{
	if (curveIndex < (int)c_type.size() && curveIndex >= 0)
		return c_type[curveIndex];
	else 
		return -1;
}

QColor Graph::color(int item)
{
	return ColorBox::color(item);
}

void Graph::showPlotErrorMessage(QWidget *parent, const QStringList& emptyColumns)
{
	QApplication::restoreOverrideCursor();

	int n = (int)emptyColumns.count();
	if (n > 1)
	{
		QString columns;
		for (int i = 0; i < n; i++)
			columns+= "<p><b>" + emptyColumns[i] + "</b></p>";

		QMessageBox::warning(parent, tr("QtiPlot - Warning"), 
				tr("The columns")+": "+columns+tr("are empty and will not be added to the plot!"));
	}
	else if (n == 1)
		QMessageBox::warning(parent, tr("QtiPlot - Warning"), 
				tr("The column")+": <p><b>" + emptyColumns[0] + "</b></p>" + tr("is empty and will not be added to the plot!"));
}

void Graph::moveMarkerBy(int dx, int dy)
{
	Q3ValueList<int> texts = textMarkerKeys();		
	bool line = false, image = false;
	if (d_lines.contains(selectedMarker))
	{
		LineMarker* mrk=(LineMarker*)d_plot->marker(selectedMarker);

		QPoint point=mrk->startPoint();			
		mrk->setStartPoint(QPoint(point.x() + dx, point.y() + dy));

		point = mrk->endPoint();
		mrk->setEndPoint(QPoint(point.x() + dx, point.y() + dy));	

		line = true;
	}
	else if (d_images.contains(selectedMarker))
	{
		ImageMarker* mrk=(ImageMarker*)d_plot->marker(selectedMarker);
		QPoint point = mrk->getOrigin();
		mrk->setOrigin(QPoint(point.x() + dx, point.y() + dy));	

		image = true;
	}
	else if (texts.contains(selectedMarker))
	{
		LegendMarker* mrk=(LegendMarker*)d_plot->marker(selectedMarker);				
		QPoint point = mrk->rect().topLeft();
		mrk->setOrigin(QPoint(point.x() + dx, point.y() + dy));	
	}

	d_plot->replot();

	if (line)
		highlightLineMarker(selectedMarker);
	else if (image)
		highlightImageMarker(selectedMarker);
	else
		highlightTextMarker(selectedMarker);

	emit modifiedGraph();	
}

void Graph::showTitleContextMenu()
{
	Q3PopupMenu titleMenu(this);
	titleMenu.insertItem(QPixmap(cut_xpm), tr("&Cut"),this, SLOT(cutTitle()));
	titleMenu.insertItem(QPixmap(copy_xpm), tr("&Copy"),this, SLOT(copyTitle()));
	titleMenu.insertItem(tr("&Delete"),this, SLOT(removeTitle()));
	titleMenu.insertSeparator();
	titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(viewTitleDialog()));
	titleMenu.exec(QCursor::pos());
}

void Graph::cutTitle()
{
	QApplication::clipboard()->setData(new Q3TextDrag(d_plot->title().text(), d_plot->titleLabel(), 0));
	removeTitle();
}

void Graph::copyTitle()
{
	QApplication::clipboard()->setData(new Q3TextDrag(d_plot->title().text(), d_plot->titleLabel(), 0));
}

void Graph::removeAxisTitle()
{
	d_plot->setAxisTitle(selectedAxis, QString::null);
	d_plot->replot();
	emit modifiedGraph();
}

void Graph::cutAxisTitle()
{
	copyAxisTitle();
	removeAxisTitle();
}

void Graph::copyAxisTitle()
{
	QApplication::clipboard()->setData(new Q3TextDrag(d_plot->axisTitle(selectedAxis).text(), 
				(QWidget *)d_plot->axisWidget(selectedAxis), 0));
}

void Graph::showAxisTitleMenu(int axis)
{
	selectedAxis = axis;

	Q3PopupMenu titleMenu(this);
	titleMenu.insertItem(QPixmap(cut_xpm), tr("&Cut"), this, SLOT(cutAxisTitle()));
	titleMenu.insertItem(QPixmap(copy_xpm), tr("&Copy"), this, SLOT(copyAxisTitle()));
	titleMenu.insertItem(tr("&Delete"),this, SLOT(removeAxisTitle()));
	titleMenu.insertSeparator();
	switch (axis)
	{
		case QwtScaleDraw::BottomScale:
			titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(xAxisTitleDblClicked()));
			break;

		case QwtScaleDraw::LeftScale:
			titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(yAxisTitleDblClicked()));
			break;

		case QwtScaleDraw::TopScale:
			titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(topAxisTitleDblClicked()));
			break;

		case QwtScaleDraw::RightScale:
			titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(rightAxisTitleDblClicked()));
			break;
	}

	titleMenu.exec(QCursor::pos());
}

void Graph::showAxisContextMenu(int axis)
{
	selectedAxis = axis;

	Q3PopupMenu menu(this);
	menu.setCheckable(true);
	menu.insertItem(QPixmap(unzoom_xpm), tr("&Rescale to show all"), this, SLOT(setAutoScale()), tr("Ctrl+Shift+R"));
	menu.insertSeparator();
	menu.insertItem(tr("&Hide axis"), this, SLOT(hideSelectedAxis()));

	int gridsID = menu.insertItem(tr("&Show grids"), this, SLOT(showGrids()));
	if (axis == QwtScaleDraw::LeftScale || axis == QwtScaleDraw::RightScale)
	{
		if (grid.majorOnY) 
			menu. setItemChecked(gridsID, true);
	}
	else 
	{
		if (grid.majorOnX) 
			menu. setItemChecked(gridsID, true);
	}

	menu.insertSeparator();
	menu.insertItem(tr("&Scale..."), this, SLOT(showScaleDialog()));
	menu.insertItem(tr("&Properties..."), this, SLOT(showAxisDialog()));
	menu.exec(QCursor::pos());
}

void Graph::showAxisDialog()
{
	emit showAxisDialog(selectedAxis);
}

void Graph::showScaleDialog()
{
	emit axisDblClicked(selectedAxis);
}

void Graph::hideSelectedAxis()
{
	int axis = -1;
	if (selectedAxis == QwtScaleDraw::LeftScale || selectedAxis == QwtScaleDraw::RightScale)
		axis = selectedAxis - 2;
	else
		axis = selectedAxis + 2;

	d_plot->enableAxis(axis, false);
	scalePicker->refresh();
	emit modifiedGraph();
}

void Graph::showGrids()
{
	if (selectedAxis == QwtScaleDraw::LeftScale || selectedAxis == QwtScaleDraw::RightScale)
	{
		grid.majorOnY = 1 - grid.majorOnY;
		d_plot->grid()->enableY(grid.majorOnY);

		grid.minorOnY = 1 - grid.minorOnY;
		d_plot->grid()->enableYMin(grid.minorOnY);
	}
	else 
	{
		grid.majorOnX = 1 - grid.majorOnX;
		d_plot->grid()->enableX(grid.majorOnX);

		grid.minorOnX = 1 - grid.minorOnX;
		d_plot->grid()->enableXMin(grid.minorOnX);
	}
	d_plot->replot();
	emit modifiedGraph();
}

void Graph::copy(Graph* g)
{
	int i;
	Plot *plot = g->plotWidget();
	d_plot->setMargin(plot->margin());
	setBackgroundColor(plot->paletteBackgroundColor());
	setBorder(plot->lineWidth(), plot->frameColor());
	setCanvasBackground(plot->canvasBackground());

	enableAxes(g->enabledAxes());
	setAxesColors(g->axesColors());
	setAxesBaseline(g->axesBaseline());

	setGridOptions(g->grid);

	d_plot->setTitle (g->plotWidget()->title());

	drawCanvasFrame(g->framed(),g->canvasFrameWidth(), g->canvasFrameColor());

	QStringList lst = g->scalesTitles();
	for (i=0;i<(int)lst.count();i++)
		setAxisTitle(i, lst[i]);

	for (i=0;i<4;i++)
		setAxisFont(i,g->axisFont(i));

	setXAxisTitleColor(g->axisTitleColor(2));
	setXAxisTitleFont(g->axisTitleFont(2));
	setXAxisTitleAlignment(g->axisTitleAlignment(2));

	setYAxisTitleColor(g->axisTitleColor(0));
	setYAxisTitleFont(g->axisTitleFont(0));
	setYAxisTitleAlignment(g->axisTitleAlignment(0));

	setTopAxisTitleColor(g->axisTitleColor(3));
	setTopAxisTitleFont(g->axisTitleFont(3));
	setTopAxisTitleAlignment(g->axisTitleAlignment(3));

	setRightAxisTitleColor(g->axisTitleColor(1));
	setRightAxisTitleFont(g->axisTitleFont(1));
	setRightAxisTitleAlignment(g->axisTitleAlignment(1));

	setAxesLinewidth(plot->axesLinewidth());
	removeLegend();

	associations = g->associations;
	if (g->isPiePlot())
		plotPie((QwtPieCurve*)g->curve(0));
	else
	{
		for (i=0; i<g->curves(); i++)
		{
			QwtPlotCurve *cv = (QwtPlotCurve *)g->curve(i);
			int n = cv->dataSize();
			int style = g->c_type[i];
			QVector<double> x(n);
			QVector<double> y(n);
			for (int j=0; j<n; j++)
			{
				x[j]=cv->x(j);
				y[j]=cv->y(j);
			}

			QwtPlotCurve *c = 0;
			if (cv->rtti() == FunctionCurve::RTTI)
			{
				c = new FunctionCurve(cv->title().text());
				((FunctionCurve*)c)->copy((FunctionCurve*)cv);
			}
			else if (style == VerticalBars || style == HorizontalBars)
			{
				c = new QwtBarCurve(((QwtBarCurve*)cv)->orientation(), d_plot,0);
				((QwtBarCurve*)c)->copy((const QwtBarCurve*)cv);
			}
			else if (style == ErrorBars)
			{
				c = new QwtErrorPlotCurve(d_plot,0);
				((QwtErrorPlotCurve*)c)->copy((const QwtErrorPlotCurve*)cv);
			}
			else if (style == Histogram)
			{
				c = new QwtHistogram(d_plot,0);
				((QwtHistogram *)c)->copy((const QwtHistogram*)cv);
			}
			else if (style == VectXYXY || style == VectXYAM)
			{
				VectorCurve::VectorStyle vs = VectorCurve::XYXY;
				if (style == VectXYAM)
					vs = VectorCurve::XYAM;
				c = new VectorCurve(vs, d_plot, 0);
				((VectorCurve *)c)->copy((const VectorCurve *)cv);
			}
			else if (style == Box)
			{
				c = new BoxCurve(d_plot, 0);
				((BoxCurve*)c)->copy((const BoxCurve *)cv);
				QwtSingleArrayData dat(x[0], y, n);
				c->setData(dat);
			}
			else
				c = new QwtPlotCurve(cv->title());

			int curveID = d_plot->insertCurve(c);

			c_keys.resize(++n_curves);
			c_keys[i] = curveID;

			c_type.resize(n_curves);
			c_type[i] = g->curveType(i);

			if (c_type[i] != Box)
				c->setData(x.data(), y.data(), n);

			c->setPen(cv->pen()); 
			c->setBrush(cv->brush());
			c->setStyle(cv->style());
			c->setSymbol(cv->symbol());

			if (cv->testCurveAttribute (QwtPlotCurve::Fitted))
				c->setCurveAttribute(QwtPlotCurve::Fitted, true);
			else if (cv->testCurveAttribute (QwtPlotCurve::Inverted))
				c->setCurveAttribute(QwtPlotCurve::Inverted, true);

			c->setAxis(cv->xAxis(), cv->yAxis());
		}
	}
	axesFormulas = g->axesFormulas;
	axisType = g->axisType;
	axesFormatInfo = g->axesFormatInfo;
	axisType = g->axisType;

	setAxisLabelRotation(QwtPlot::xBottom, g->labelsRotation(QwtPlot::xBottom));
	setAxisLabelRotation(QwtPlot::xTop, g->labelsRotation(QwtPlot::xTop));

	for (i=0; i<QwtPlot::axisCnt; i++)
	{
		QwtScaleWidget *sc = g->plotWidget()->axisWidget(i);
		if (!sc)
			continue;

		QwtScaleDraw *sd = g->plotWidget()->axisScaleDraw (i);
		if (sd->hasComponent(QwtAbstractScaleDraw::Labels))
		{
			if (axisType[i] == Graph::Numeric)
				setLabelsNumericFormat(i, plot->axisLabelFormat(i), plot->axisLabelPrecision(i), axesFormulas[i]);
			else if (axisType[i] == Graph::Day)
				setLabelsDayFormat(i, axesFormatInfo[i].toInt());
			else if (axisType[i] == Graph::Month)
				setLabelsMonthFormat(i, axesFormatInfo[i].toInt());
			else if (axisType[i] == Graph::Time || axisType[i] == Graph::Date)
				setLabelsDateTimeFormat(i, axisType[i], axesFormatInfo[i]);
			else
			{
				QwtTextScaleDraw *sd = (QwtTextScaleDraw *)plot->axisScaleDraw (i);
				d_plot->setAxisScaleDraw(i, new QwtTextScaleDraw(sd->labelsList()));
			}
		}
		else
		{
			sd = d_plot->axisScaleDraw (i);
			sd->enableComponent (QwtAbstractScaleDraw::Labels, false);
		}
	}
	for (i=0; i<QwtPlot::axisCnt; i++)
	{//set same scales
		const QwtScaleEngine *se = plot->axisScaleEngine(i);
		if (!se)
			continue;

		QwtScaleEngine *sc_engine = 0;
		if (se->transformation()->type() == QwtScaleTransformation::Log10)
			sc_engine = new QwtLog10ScaleEngine();
		else if (se->transformation()->type() == QwtScaleTransformation::Linear)
			sc_engine = new QwtLinearScaleEngine();

		const QwtScaleDiv *sd = plot->axisScaleDiv(i);
		QwtValueList lst = sd->ticks (QwtScaleDiv::MajorTick);
		double step = 0.0;
		d_user_step[i] = g->userDefinedStep(i);
		if (d_user_step[i])
			step = fabs(lst[1]-lst[0]);

		int majorTicks = (int)lst.count();
		int minorTicks = plot->axisMaxMinor(i);
		QwtScaleDiv div = sc_engine->divideScale (QMIN(sd->lBound(), sd->hBound()),
				QMAX(sd->lBound(), sd->hBound()),
				majorTicks, minorTicks, step);
		d_plot->setAxisMaxMajor (i, majorTicks);
		d_plot->setAxisMaxMinor (i, minorTicks);

		if (se->testAttribute(QwtScaleEngine::Inverted))
		{
			sc_engine->setAttribute(QwtScaleEngine::Inverted);
			div.invert();
		}

		d_plot->setAxisScaleEngine (i, sc_engine);
		d_plot->setAxisScaleDiv (i, div);
	}

	drawAxesBackbones(g->drawAxesBackbone);
	setMajorTicksType(g->plotWidget()->getMajorTicksType());
	setMinorTicksType(g->plotWidget()->getMinorTicksType());
	setTicksLength(g->minorTickLength(), g->majorTickLength());

	QwtArray<long> imag = g->imageMarkerKeys();
	for (i=0;i<(int)imag.size();i++)
	{
		ImageMarker* imrk=(ImageMarker*)g->imageMarker(imag[i]);
		if (imrk)
			insertImageMarker(imrk);
	}

	Q3ValueList<int> txtMrkKeys=g->textMarkerKeys();
	for (i=0;i<(int)txtMrkKeys.size();i++)
	{
		LegendMarker* mrk=(LegendMarker*)g->textMarker(txtMrkKeys[i]);
		if (!mrk)
			continue;

		if (txtMrkKeys[i] == g->legendMarkerID)
			legendMarkerID = insertTextMarker(mrk);
		else
			insertTextMarker(mrk);
	}

	QwtArray<long> l = g->lineMarkerKeys();
	for (i=0;i<(int)l.size();i++)
	{
		LineMarker* lmrk=(LineMarker*)g->lineMarker(l[i]);
		if (lmrk)
			insertLineMarker(lmrk);
	}
	d_plot->replot();	
}

void Graph::plotBoxDiagram(Table *w, const QStringList& names)
{
	for (int j = 0; j <(int)names.count(); j++)
	{
		const QString name = names[j];
		associations<<name;
		int ycol = w->colIndex(name);
		int i, size=0;
		for (i = 0; i<w->tableRows(); i++ )
		{
			if (!w->text(i,ycol).isEmpty())
				size++;
		}
		if (size>0)
		{
			QVector<double> y(size);
			int it=0;
			for (i = 0; i<w->tableRows(); i++ )
			{
				QString s = w->text(i,ycol);
				if (!s.isEmpty())
					y[it++] = s.toDouble();
			}
			gsl_sort (y.data(), 1, size);

			QwtSingleArrayData data(double(j+1), y, size);
			BoxCurve *c = new BoxCurve(d_plot,0);
			c->setData(data);

			long curveID = d_plot->insertCurve(c);
			c_keys.resize(++n_curves);
			c_keys[n_curves-1] = curveID;
			c_type.resize(n_curves);
			c_type[n_curves-1] = Box;

			c->setTitle(name);
			c->setPen(QPen(color(j), 1));
			c->setSymbol(QwtSymbol(QwtSymbol::None, QBrush(), QPen(color(j), 1), QSize(7,7)));
		}
	}

	LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
	if (mrk)
		mrk->setText(legendText());	

	axisType[QwtPlot::xBottom] = ColHeader;
	d_plot->setAxisScaleDraw (QwtPlot::xBottom, new QwtTextScaleDraw(w->selectedYLabels()));
	d_plot->setAxisMaxMajor(QwtPlot::xBottom, names.count()+1);
	d_plot->setAxisMaxMinor(QwtPlot::xBottom, 0);

	axisType[QwtPlot::xTop] = ColHeader;
	d_plot->setAxisScaleDraw (QwtPlot::xTop, new QwtTextScaleDraw(w->selectedYLabels()));
	d_plot->setAxisMaxMajor(QwtPlot::xTop, names.count()+1);
	d_plot->setAxisMaxMinor(QwtPlot::xTop, 0);

	axesFormatInfo[QwtPlot::xBottom] = w->name();
	axesFormatInfo[QwtPlot::xTop] = w->name();
}

void Graph::updateBoxData(Table* w, const QString& yColName, int curve)
{
	int ycol=w->colIndex(yColName);
	int i, size=0;
	for (i = 0; i<w->tableRows(); i++)
	{
		if (!w->text(i,ycol).isEmpty())
			size++;
	}

	if (size>0)
	{
		QVector<double> y(size);
		int it=0;
		for (i = 0; i<w->tableRows(); i++)
		{
			QString s = w->text(i,ycol);
			if (!s.isEmpty())
				y[it++] = s.toDouble();
		}
		gsl_sort (y.data(), 1, size);
		BoxCurve *c = (BoxCurve *)this->curve(curve);
		if (c)
		{
			QwtSingleArrayData dat(c->x(0), y, size);
			c->setData(dat);
		}
	}
	else
		removeCurve(curve);		
	d_plot->replot();	
}

void Graph::setCurveStyle(int index, int s)
{
	QwtPlotCurve *c = curve(index);
	if (!c)
		return;

	if (s == 5)//ancient spline style in Qwt 4.2.0
	{
		s = QwtPlotCurve::Lines;
		c->setCurveAttribute(QwtPlotCurve::Fitted, true);
		c_type[index] = Spline;
	}
	else if (s == QwtPlotCurve::Lines)
		c->setCurveAttribute(QwtPlotCurve::Fitted, false);
	else if (s == 6)// Vertical Steps
	{
		s = QwtPlotCurve::Steps;
		c->setCurveAttribute(QwtPlotCurve::Inverted, true);
		c_type[index] = VerticalSteps;
	}
	else if (s == QwtPlotCurve::Steps)// Horizontal Steps
	{
		c->setCurveAttribute(QwtPlotCurve::Inverted, false);
		c_type[index] = HorizontalSteps;
	}
	c->setStyle((QwtPlotCurve::CurveStyle)s); 
}

void Graph::setCurveSymbol(int index, const QwtSymbol& s)
{
	QwtPlotCurve *c = curve(index);
	if (!c || c->symbol() == s)
		return;

	c->setSymbol(s);

	QString yColName=c->title().text();
	for (int i=0;i<n_curves;i++)
	{
		if (associations[i].contains(yColName) && i!=index && c_type[i] == ErrorBars)
		{
			QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(i);
			if (er)
				er->setSymbolSize(s.size());		
		}
	}
}

void Graph::setCurvePen(int index, const QPen& p)
{
	QwtPlotCurve *c = curve(index);
	if (!c || c->pen() == p)
		return;

	c->setPen(p);
}

void Graph::setCurveBrush(int index, const QBrush& b)
{
	QwtPlotCurve *c = curve(index);
	if (!c || c->brush() == b)
		return;

	c->setBrush(b);
}

void Graph::openBoxDiagram(Table *w, const QStringList& l)
{
	associations<<l[2];
	int ycol = w->colIndex(l[2]);
	int i, size=0;
	for (i = 0; i<w->tableRows(); i++ )
	{
		if (!w->text(i,ycol).isEmpty())
			size++;
	}
	if (!size)
		return;

	QVector<double> y(size);
	int it=0;
	for (i = 0; i<w->tableRows(); i++ )
	{
		QString s = w->text(i,ycol);
		if (!s.isEmpty())
			y[it++] = s.toDouble();
	}

	gsl_sort (y.data(), 1, size);//the data must be sorted first!

	BoxCurve *c = new BoxCurve(d_plot,0);
	QwtSingleArrayData dat(l[1].toDouble(), y, size);
	c->setData(dat);

	long curveID = d_plot->insertCurve(c);

	c_keys.resize(++n_curves);
	c_keys[n_curves-1] = curveID;
	c_type.resize(n_curves);
	c_type[n_curves-1] = Box;

	c->setTitle(l[2]);
	c->setMaxStyle(SymbolBox::style(l[16].toInt()));
	c->setP99Style(SymbolBox::style(l[17].toInt()));
	c->setMeanStyle(SymbolBox::style(l[18].toInt()));
	c->setP1Style(SymbolBox::style(l[19].toInt()));
	c->setMinStyle(SymbolBox::style(l[20].toInt()));

	c->setBoxStyle(l[21].toInt());
	c->setBoxWidth(l[22].toInt());
	c->setBoxRange(l[23].toInt(), l[24].toDouble());
	c->setWhiskersRange(l[25].toInt(), l[26].toDouble());
}

QString Graph::curveXColName(const QString& curveTitle)
{
	QStringList cl = curvesList();
	int index=cl.findIndex(curveTitle);
	if (index < 0)
		return QString::null;

	cl=QStringList::split(",", associations[index],FALSE);
	return cl[0].remove("(X)",true);
}

void Graph::disableTools()
{
	if (selectorsEnabled())
	{
		disableRangeSelectors();
		return;
	}
	else if (enabledCursor())
	{
		enableCursor(false);
		replot();
		return;
	}	
	else if (pickerActivated())
	{
		showPlotPicker(false);
		return;
	}
	else if (movePointsActivated())
	{
		movePoints(false);
		return;
	}
	else if (removePointActivated())
	{
		removePoints(false);
		return;
	}
	else if (zoomOn())
	{
		zoom(false);
		return;
	}
	else if (drawLineActive())
	{
		drawLine(false);
		return;
	}
}

void Graph::setTextMarkerDefaults(int f, const QFont& font, 
		const QColor& textCol, const QColor& backgroundCol)
{
	defaultMarkerFont = font;
	defaultMarkerFrame = f; 
	defaultTextMarkerColor = textCol;
	defaultTextMarkerBackground = backgroundCol;
}

void Graph::setArrowDefaults(int lineWidth,  const QColor& c, Qt::PenStyle style,
		int headLength, int headAngle, bool fillHead)
{
	defaultArrowLineWidth = lineWidth; 
	defaultArrowColor = c;
	defaultArrowLineStyle = style;
	defaultArrowHeadLength = headLength;
	defaultArrowHeadAngle = headAngle;
	defaultArrowHeadFill = fillHead;
}

QString Graph::parentPlotName()
{
	QWidget *w = (QWidget *)parent()->parent();
	return QString(w->name());
}

Graph::~Graph()
{
	delete titlePicker;
	delete scalePicker;	
	delete cp;
	delete d_plot;
}
