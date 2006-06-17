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
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3MemArray>
#include <QTextStream>
#include <QContextMenuEvent>
#include <QList>
#include <QCloseEvent>
#include <QLabel>
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
#include "scaleDraws.h"
#include "plot.h"
#include "parser.h"
#include "fileDialogs.h"
#include "colorBox.h"
#include "patternBox.h"
#include "symbolBox.h"

#include <qapplication.h>
#include <qbitmap.h>
#include <qclipboard.h>
#include <qcursor.h>
#include <q3dragobject.h>
#include <q3filedialog.h> 
#include <qimage.h>
#include <qmessagebox.h>
#include <qpixmap.h> 
#include <qpixmapcache.h>
#include <q3popupmenu.h>
#include <q3ptrlist.h>

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_zoomer.h>
// FIXME: Is there a replacement for this in Qwt5?
// #include <qwt_autoscl.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_marker.h>
#include <qwt_text.h>

#include <gsl/gsl_sort.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

//FIXME: All functionality disabled for now (needs port to Qwt5)

	Graph::Graph(QWidget* parent, const char* name, Qt::WFlags f)
: QWidget(parent,name,f)
{
#if false
	if ( !name )
		setName( "graph" );

	n_curves=0;
	linesOnPlot=0;
	widthLine=1;mrkX=-1;mrkY=-1;fitID=0;
	selectedCol=0;selectedPoint=-1;
	selectedCurve =-1;selectedMarker=-1;selectedCursor=-1;
	startPoint=0;endPoint=-1;
	startID=-1; endID=-1;
	pieRadius=100;
	axesLineWidth = 1;	
	lineProfileOn=false;
	drawTextOn=false;
	drawLineOn=false;
	cursorEnabled=false;
	movePointsEnabled=false;
	removePointsEnabled=false;
	pickerEnabled=false;
	rangeSelectorsEnabled=false;
	piePlot=false;
	isTitleSelected=false;
	functions=0;
	ignoreResize= false;
	drawAxesBackbone = true;
	autoscale = true;
	autoScaleFonts = true;
	translateOn = false;

	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		tickLabelsOn<<"1";
		axisType << Numeric;
		axesFormatInfo<<QString();
		lblFormat<<Automatic;
		axesFormulas<<QString();
	}

	d_plot = new Plot(this,"QwtPlot");		
	cp = new CanvasPicker(this);

	LegendMarker *mrk = new LegendMarker(d_plot);
	mrk->setOrigin(QPoint(10, 10));
	legendMarkerID=d_plot->insertMarker(mrk);

	titlePicker = new TitlePicker(d_plot);
	scalePicker = new ScalePicker(d_plot);

	d_zoomer= new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft,
			QwtPicker::DragSelection, QwtPicker::AlwaysOff, d_plot->canvas());
	d_zoomer->setRubberBandPen(Qt::black);
	zoom(false);

	setGeometry( QRect(0, 0, 520, 420 ) );
	setFocusPolicy(Qt::StrongFocus);
	setFocusProxy(d_plot);
	setMouseTracking(true );

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

	initScales();

	connect (d_plot,SIGNAL(selectPlot()),this,SLOT(activateGraph()));
	connect (d_plot,SIGNAL(selectPlot()),this,SLOT(highlightGraph()));
	connect (d_plot,SIGNAL(moveGraph(const QPoint&)),this,SLOT(moveGraph(const QPoint&)));
	connect (d_plot,SIGNAL(releasedGraph()),this,SLOT(releaseGraph()));

	connect (cp,SIGNAL(moveGraph(const QPoint&)),this,SLOT(moveGraph(const QPoint&)));
	connect (cp,SIGNAL(releasedGraph()),this,SLOT(releaseGraph()));
	connect (cp,SIGNAL(highlightGraph()),this,SLOT(highlightGraph()));
	connect (cp,SIGNAL(selectPlot()),this,SLOT(activateGraph()));
	connect (cp,SIGNAL(drawTextOff()),this,SIGNAL(drawTextOff()));
	connect (cp,SIGNAL(viewImageDialog()),this,SIGNAL(viewImageDialog()));
	connect (cp,SIGNAL(viewTextDialog()),this,SIGNAL(viewTextDialog()));
	connect (cp,SIGNAL(viewLineDialog()),this,SIGNAL(viewLineDialog()));
	connect (cp,SIGNAL(showPlotDialog(long)),this,SIGNAL(showPlotDialog(long)));
	connect (cp,SIGNAL(showPieDialog()),this,SIGNAL(showPieDialog()));
	connect (cp,SIGNAL(showMarkerPopupMenu()),this,SIGNAL(showMarkerPopupMenu()));
	connect (cp,SIGNAL(modified()), this, SLOT(modified()));
	connect (cp,SIGNAL(modified()), this, SIGNAL(modifiedGraph()));
	connect (cp,SIGNAL(calculateProfile(const QPoint&, const QPoint&)),
			this,SLOT(calculateLineProfile(const QPoint&, const QPoint&)));

	connect (titlePicker,SIGNAL(highlightGraph()),this,SLOT(highlightGraph()));
	connect (titlePicker,SIGNAL(showTitleMenu()),this,SLOT(showTitleContextMenu()));
	connect (titlePicker,SIGNAL(doubleClicked()),this,SIGNAL(viewTitleDialog()));
	connect (titlePicker,SIGNAL(removeTitle()),this,SLOT(removeTitle()));
	connect (titlePicker,SIGNAL(clicked()), this,SLOT(selectTitle()));
	connect (titlePicker,SIGNAL(moveGraph(const QPoint&)),this,SLOT(moveGraph(const QPoint&)));
	connect (titlePicker,SIGNAL(releasedGraph()),this,SLOT(releaseGraph()));

	connect (scalePicker,SIGNAL(highlightGraph()),this,SLOT(highlightGraph()));
	connect (scalePicker,SIGNAL(clicked()),this,SLOT(activateGraph()));
	connect (scalePicker,SIGNAL(clicked()),this,SLOT(deselectMarker()));
	connect (scalePicker,SIGNAL(clicked()),this,SLOT(deselectTitle()));
	connect (scalePicker,SIGNAL(axisDblClicked(int)),this,SIGNAL(axisDblClicked(int)));
	connect (scalePicker,SIGNAL(axisTitleRightClicked(int)),this,SLOT(showAxisTitleMenu(int)));
	connect (scalePicker,SIGNAL(axisRightClicked(int)),this,SLOT(showAxisContextMenu(int)));
	connect (scalePicker,SIGNAL(xAxisTitleDblClicked()),this,SIGNAL(xAxisTitleDblClicked()));		
	connect (scalePicker,SIGNAL(yAxisTitleDblClicked()),this,SIGNAL(yAxisTitleDblClicked()));
	connect (scalePicker,SIGNAL(rightAxisTitleDblClicked()),this,SIGNAL(rightAxisTitleDblClicked()));
	connect (scalePicker,SIGNAL(topAxisTitleDblClicked()),this,SIGNAL(topAxisTitleDblClicked()));
	connect (scalePicker,SIGNAL(moveGraph(const QPoint&)),this,SLOT(moveGraph(const QPoint&)));
	connect (scalePicker,SIGNAL(releasedGraph()),this, SLOT(releaseGraph()));

	connect (d_zoomer,SIGNAL(zoomed (const QwtDoubleRect &)),this,SLOT(zoomed (const QwtDoubleRect &)));
#endif
}

void Graph::emitModified()
{
#if false
	emit modifiedGraph();
#endif
}

void Graph::modified()
{
#if false
	emit modifiedGraph(this);
#endif
}

void Graph::moveGraph(const QPoint& pos)
{
#if false
	emit moveGraph(this, pos);
#endif
}

void Graph::releaseGraph()
{
#if false
	emit releaseGraph(this);
#endif
}

void Graph::activateGraph()
{
#if false
	emit selectedGraph(this);
#endif
}

void Graph::deselectMarker()
{
#if false
	selectedMarker = -1;
	d_plot->replot();
#endif
}

void Graph::deselectTitle()
{
#if false
	isTitleSelected=false;
	d_plot->titleLabel()->repaint();
#endif
}

long Graph::selectedMarkerKey()
{
#if false
	return selectedMarker;
#endif
}

QwtPlotMarker* Graph::selectedMarkerPtr()
{
#if false
	return d_plot->marker(selectedMarker);
#endif
}

void Graph::setSelectedMarker(long mrk)
{
#if false
	selectedMarker=mrk;
#endif
}

void Graph::highlightLineMarker(long markerID)
{
#if false
	LineMarker* mrkL=(LineMarker*) d_plot->marker(markerID);	
	if (!mrkL)
		return;

	selectedMarker=markerID;		
	QwtPlotCanvas *canvas=d_plot->canvas ();

	// draw highlight rectangle 
	QPainter painter(canvas);
	painter.save();
	QPoint aux=mrkL->startPoint();
	painter.translate(aux.x(),aux.y());
	double t=mrkL->teta();
	painter.rotate(-t);
	painter.setRasterOp(Qt::NotXorROP);
	painter.setPen(QPen(QColor(Qt::red),1,Qt::SolidLine));
	int w=mrkL->width();
	int d=w+(int)floor(mrkL->headLength()*tan(M_PI*mrkL->headAngle()/180.0)+0.5);
	painter.drawRect(-d, -d-w, int(mrkL->length()+2*d), 2*(d+w));				
	painter.restore();	
#endif
}

void Graph::highlightTextMarker(long markerID)
{
#if false
	LegendMarker* mrk=(LegendMarker*)d_plot->marker(markerID);
	if (!mrk)
		return;

	selectedMarker=markerID;		

	QwtPlotCanvas *canvas=d_plot->canvas ();
	QPainter painter(canvas);
	painter.setPen(QPen(Qt::red,2,Qt::SolidLine));
	painter.setRasterOp(Qt::NotXorROP);
	painter.drawRect (mrk->rect());	
#endif
}

void Graph::highlightImageMarker(long markerID)
{
#if false
	ImageMarker* mrkI=(ImageMarker*)d_plot->marker(markerID);
	if (!mrkI)
		return;	

	selectedMarker=markerID;					
	QwtPlotCanvas *canvas=d_plot->canvas ();
	QPainter painter(canvas);
	painter.setPen(QPen(Qt::red,2,Qt::SolidLine));
	painter.setRasterOp(Qt::NotXorROP);
	painter.drawRect (mrkI->rect());	
#endif
}

void Graph::selectNextMarker()
{
#if false
	QwtArray<long> mrkKeys=d_plot->markerKeys();
	int n=mrkKeys.size();
	if (n==0)
		return;

	if (isTitleSelected)
		deselectTitle();

	long min_key=mrkKeys[0], max_key=mrkKeys[0];
	for (int i = 0; i<n; i++ )
	{
		if (mrkKeys[i] >= max_key)
			max_key=mrkKeys[i];
		if (mrkKeys[i] <= min_key)
			min_key=mrkKeys[i];
	}

	d_plot->replot();
	QwtArray<long> texts=textMarkerKeys();
	if (selectedMarker >= 0)
	{
		long key = selectedMarker+1;
		if ( key > max_key )
			key=min_key;
		while(key <= max_key)
		{
			if (texts.contains(key))
			{
				highlightTextMarker(key);
				break;
			}
			else if (lines.contains(key))
			{
				highlightLineMarker(key);
				break;
			}
			else  if (images.contains(key))
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
		long key=min_key;
		while(selectedMarker < 0)
		{
			if (texts.contains(key))
				highlightTextMarker(key);
			else if (lines.contains(key))
				highlightLineMarker(key);
			else  if (images.contains(key))
				highlightImageMarker(key);
			key++;
		}
	}
#endif
}

void Graph::replot()
{
#if false
	d_plot->replot();
#endif
}

void Graph::movedPicker(const QPoint &pos, bool mark)
{
#if false
	QString info;
	info.sprintf("x=%g; y=%g",
			d_plot->invTransform(QwtPlot::xBottom, pos.x()),
			d_plot->invTransform(QwtPlot::yLeft, pos.y()));
	emit cursorInfo(info);

	if (mark)
	{
		QwtMarker mrk;
		mrk.setSymbol(QwtSymbol(QwtSymbol::Cross,
					QBrush(Qt::NoBrush), QPen(Qt::red,1), QSize(15,15)));

		QPainter painter(d_plot->canvas());
		painter.setClipping(true);
		painter.setClipRect(d_plot->canvas()->contentsRect());
		painter.setRasterOp(Qt::NotXorROP);

		if (translateOn)
		{
			const QwtPlotCurve *c = d_plot->curve(selectedCurve);
			if (!c)
				return;

			if (translationDirection)
				mrk.draw(&painter,
						d_plot->transform(QwtPlot::xBottom,c->x(selectedPoint)), pos.y(),QRect());
			else
				mrk.draw(&painter,pos.x(), 
						d_plot->transform(QwtPlot::yLeft,c->y(selectedPoint)),QRect());
		}
		else
			mrk.draw(&painter,pos.x(), pos.y(),QRect());
	}
#endif
}

bool Graph::pickerActivated()
{
#if false
	return pickerEnabled;
#endif
}

void Graph::initFonts(const QFont &scaleTitleFnt, const QFont &numbersFnt, 
		const QFont &textMarkerFnt)
{
#if false
	defaultMarkerFont = textMarkerFnt;
	for (int i = 0;i<QwtPlot::axisCnt;i++)
	{
		d_plot->setAxisFont (i,numbersFnt);
		d_plot->setAxisTitleFont (i,scaleTitleFnt);
	}
#endif
}

void Graph::setAxisFont(int axis,const QFont &fnt)
{
#if false
	d_plot->setAxisFont (axis,fnt);
	if (lblFormat[axis] == Superscripts)
	{	
		QwtSupersciptsScaleDraw *sd= (QwtSupersciptsScaleDraw *)d_plot->axisScaleDraw(axis);
		sd->setFont(fnt);
	}
	d_plot->replot();
	emit modifiedGraph();
#endif
}

QFont Graph::axisFont(int axis)
{
#if false
	return d_plot->axisFont (axis);
#endif
}

void Graph::enableAxes(const QStringList& list)
{
#if false
	int i;
	for (i = 0;i<QwtPlot::axisCnt;i++)
	{
		bool ok=list[i+1].toInt();
		d_plot->enableAxis(i,ok);
	}

	for (i = 0;i<QwtPlot::axisCnt;i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)
			scale->setBaselineDist(0);
	}
	scalePicker->refresh();
#endif
}

void Graph::enableAxes(Q3MemArray<bool> axesOn)
{
#if false
	int i;
	for (i = 0; i<QwtPlot::axisCnt; i++)
		d_plot->enableAxis(i,axesOn[i]);

	for (i = 0;i<QwtPlot::axisCnt;i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)
			scale->setBaselineDist(0);
	}


	scalePicker->refresh();
#endif
}

Q3MemArray<bool> Graph::enabledAxes()
{
#if false
	Q3MemArray<bool> axesOn(4);
	for (int i = 0; i<QwtPlot::axisCnt; i++)
		axesOn[i]=d_plot->axisEnabled (i);
	return axesOn;
#endif
}

QList<int> Graph::axesBaseline()
{
#if false
	QList<int> baselineDist;
	for (int i = 0; i<QwtPlot::axisCnt; i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)
			baselineDist << scale->baseLineDist();
		else
			baselineDist << 0;
	}
	return baselineDist;
#endif
}

void Graph::setAxesBaseline(const QList<int> &lst)
{
#if false
	for (int i = 0; i<QwtPlot::axisCnt; i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)
			scale->setBaselineDist(lst[i]);
	}
#endif
}

void Graph::setAxesBaseline(QStringList &lst)
{
#if false
	lst.remove(lst.first());
	for (int i = 0; i<QwtPlot::axisCnt; i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)
			scale->setBaselineDist((lst[i]).toInt());
	}
#endif
}

QList<int> Graph::axesType()
{
#if false
	return axisType;
#endif
}

QList<int> Graph::ticksType()
{
#if false
	return d_plot->getTicksType();
#endif
}

QStringList Graph::labelsNumericFormat()
{
#if false
	QStringList format;
	for (int i= 0; i<8; i++)
		format<<"0";

	int prec, fieldwidth;
	char f;
	for (int j= 0; j<4; j++)
	{
		if (d_plot->axisEnabled (j))
		{
			d_plot->axisLabelFormat (j, f, prec, fieldwidth);		

			format[2*j]=QString::number(lblFormat[j]);
			format[2*j+1]=QString::number(prec);
		}
		else
		{
			format[2*j]="0";
			format[2*j+1]="4";
		}
	}
	return format;
#endif
}

void Graph::setLabelsNumericFormat(int axis, int format, int prec, const QString& formula)
{	
#if false
	lblFormat[axis] = format;
	axisType[axis] = Numeric;
	axesFormulas[axis] = formula;

	ScaleDraw *sd_old= (ScaleDraw *)d_plot->axisScaleDraw (axis);
	const QwtScaleDiv div=sd_old->scaleDiv ();

	if (format == Superscripts)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(axis);
		QColor color = QColor();
		if (scale)	
		{
			QPalette pal=scale->palette();
			color =pal.color(QPalette::Active, QColorGroup::Foreground);
		}

		QwtSupersciptsScaleDraw* sd= new QwtSupersciptsScaleDraw(axisFont(axis), color);

		sd->setLineWidth(axesLineWidth);
		d_plot->setAxisLabelFormat (axis, 'e', prec, 0);
		sd->setScale(div);
		sd->setFormulaString(formula.ascii());
		d_plot->setAxisScaleDraw (axis,sd);	
	}
	else
	{				
		ScaleDraw *sd= new ScaleDraw(axesLineWidth);

		if (format == Automatic)
			d_plot->setAxisLabelFormat (axis, 'g', prec, 0);
		else if (format == Scientific )
			d_plot->setAxisLabelFormat (axis, 'e', prec, 0);
		else if (format == Decimal)
			d_plot->setAxisLabelFormat (axis, 'f', prec, 0);

		sd->setScale(div);
		sd->setFormulaString(formula.ascii());
		d_plot->setAxisScaleDraw (axis,sd);	
	}
#endif
}

void Graph::setLabelsNumericFormat(int axis, const QStringList& l)
{
#if false
	if (tickLabelsOn[axis].toInt() == 0 || //labels are invisible
			axisType[axis] != Numeric)	return;

	int format=l[2*axis].toInt();
	int prec=l[2*axis+1].toInt();
	setLabelsNumericFormat(axis, format, prec, axesFormulas[axis]);
#endif
}

void Graph::setLabelsNumericFormat(const QStringList& l)
{
#if false
	for (int axis = 0; axis<4; axis++)
		setLabelsNumericFormat (axis, l);
#endif
}

void Graph::setAxesType(const QList<int> tl)
{
#if false
	axisType = tl;	
#endif
}

QString Graph::saveAxesLabelsType()
{
#if false
	QString s="AxisType\t";
	for (int i=0; i<4; i++)
	{
		int type = axisType[i];
		s+=QString::number(type);
		if (type == Time || type == Date || type == Txt || type == ColHeader)
			s+=";"+ axesFormatInfo[i];
		s+="\t";
	};

	return s+"\n";
#endif
}

QString Graph::saveTicksType()
{
#if false
	QList<int> ticksTypeList=d_plot->getTicksType();
	QString s="EnabledTicks\t";
	for (int i=0;i<4;i++)
		s+=QString::number(ticksTypeList[i])+"\t";
	return s+"\n";
#endif
}

QStringList Graph::enabledTickLabels()
{
#if false
	return tickLabelsOn;
#endif
}

QString Graph::saveEnabledTickLabels()
{
#if false
	QString s="EnabledTickLabels\t";
	for (int i=0;i<4;i++)
		s+=tickLabelsOn[i]+"\t";
	return s+"\n";
#endif
}

QString Graph::saveLabelsFormat()
{
#if false
	QString s="LabelsFormat\t";
	QStringList format=labelsNumericFormat();
	s+=format.join("\t");
	return s+"\n";
#endif
}

QString Graph::saveAxesBaseline()
{
#if false
	QString s="AxesBaseline\t";
	for (int i = 0; i<QwtPlot::axisCnt; i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)
			s+= QString::number(scale->baseLineDist()) + "\t";
		else
			s+= "0\t";
	}
	return s+"\n";
#endif
}

QString Graph::saveLabelsRotation()
{
#if false
	QString s="LabelsRotation\t";
	s+=QString::number(labelsRotation(QwtPlot::xBottom))+"\t";
	s+=QString::number(labelsRotation(QwtPlot::xTop))+"\n";
	return s;
#endif
}

void Graph::setEnabledTickLabels(const QStringList& labelsOn)
{
#if false
	if (tickLabelsOn == labelsOn)
		return;

	for (int axis=0; axis<QwtPlot::axisCnt; axis++)
	{
		if (labelsOn[axis]=="0")
			d_plot->setAxisScaleDraw (axis, new QwtNoLabelsScaleDraw(axesLineWidth));
	}
	tickLabelsOn=labelsOn;
#endif
}

void Graph::setTicksType(const QList<int>& list)
{
#if false
	if (d_plot->getTicksType() == list)
		return;

	for (int i=0;i<4;i++)
	{
		ScaleDraw *sd= (ScaleDraw *)d_plot->axisScaleDraw (i);
		if (list[i]==Plot::None || list[i]==Plot::In)
			sd->setTickLength(0,0,0);
		else if (list[i]==Plot::Out)
			sd->setTickLength(4,6,8);
		else if (list[i] == Plot::Both)
			sd->setTickLength (0,0,8); 

		d_plot->setTicksType(i,list[i]);
	}
#endif
}

void Graph::setTicksType(const QStringList& list)
{
#if false
	for (int i=1;i<5;i++)
	{
		int type=list[i].toInt();
		d_plot->setTicksType(i-1,type);
	}
#endif
}

int Graph::minorTickLength()
{
#if false
	return d_plot->minorTickLength();
#endif
}

int Graph::majorTickLength()
{
#if false
	return d_plot->majorTickLength();
#endif
}

void Graph::setAxisTicksLength(int axis, int ticksType, int minLength, int majLength)
{
#if false
	QwtScale *scale = (QwtScale *)d_plot->axis(axis);
	if (!scale)
		return;

	ScaleDraw *sd= (ScaleDraw *)d_plot->axisScaleDraw (axis);
	if (ticksType == Plot::Out)
		sd->setTickLength(minLength, minLength, majLength);
	else if (ticksType == Plot::Both)
		sd->setTickLength (0, 0, majLength);
	else if (ticksType == Plot::In || ticksType == Plot::None)
		sd->setTickLength (0, 0, 0);
#endif
}

void Graph::setTicksLength(int minLength, int majLength)
{
#if false
	QList<int> ticksType = d_plot->getTicksType();
	for (int i=0; i<4; i++)
	{
		int type = ticksType[i];
		setAxisTicksLength (i, type, minLength, majLength);
	}

	d_plot->setTickLength (minLength, majLength);
#endif
}

void Graph::changeTicksLength(int minLength, int majLength)
{
#if false
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
#endif
}

void Graph::showAxis(int axis, int type, const QString& formatInfo, Table *table,
		bool axisOn, int ticksType, bool labelsOn, const QColor& c, 
		int format, int prec, int rotation, int baselineDist,
		const QString& formula)
{		
#if false
	QList<int> ticksTypeList = d_plot->getTicksType();

	char f;
	int pr,fw;
	d_plot->axisLabelFormat (axis, f, pr, fw);

	QwtScale *scale = (QwtScale *)d_plot->axis(axis);
	int oldBaseline = 0;
	if (scale)
		oldBaseline = scale->baseLineDist();

	if (d_plot->axisEnabled (axis) == axisOn &&
			tickLabelsOn[axis] == QString::number(labelsOn) &&
			ticksTypeList[axis] == ticksType &&
			axesColors()[axis] == c.name() &&
			prec == pr && format == lblFormat[axis] &&
			labelsRotation(axis) == rotation &&
			axisType[axis] == type &&
			axesFormatInfo[axis] == formatInfo &&
			axesFormulas[axis] == formula &&
			oldBaseline == baselineDist)
		return;

	d_plot->enableAxis (axis, axisOn);
	scale = (QwtScale *)d_plot->axis(axis);	
	if (scale)
	{
		scale->setBaselineDist(baselineDist);	

		QPalette pal = scale->palette();
		if (pal.color(QPalette::Active, QColorGroup::Foreground) != c)
		{
			pal.setColor(QColorGroup::Foreground,c);
			scale->setPalette(pal);
		}		
	}

	tickLabelsOn[axis]=QString::number(labelsOn);
	if (!labelsOn)
		d_plot->setAxisScaleDraw (axis, new QwtNoLabelsScaleDraw(axesLineWidth));
	else
	{
		setAxisLabelRotation(axis, rotation);

		if (type == Numeric)
			setLabelsNumericFormat(axis, format, prec, formula);
		else if (type == Time || type == Date)
			setLabelsDateTimeFormat (axis, type, formatInfo);
		else	
			setLabelsTextFormat(axis, type, formatInfo, table);
	}

	d_plot->setTicksType(axis, ticksType);	
	setAxisTicksLength(axis, ticksType, d_plot->minorTickLength(), d_plot->majorTickLength());

	ScaleDraw *sclDraw= (ScaleDraw *)d_plot->axisScaleDraw (axis);	
	sclDraw->setOptions(drawAxesBackbone);

	if (axisOn && (axis == QwtPlot::xTop || axis == QwtPlot::yRight))
		updateSecondaryAxis(axis);//synchronize scale divisions

	scalePicker->refresh();		
	d_plot->replot();	
	emit modifiedGraph();
#endif
}

void Graph::setLabelsTextFormat(int axis, int type, const QString& labelsColName, Table *table)
{
#if false
	if (type == Numeric || type == Time || type == Date || tickLabelsOn[axis].toInt() == 0)
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
	else if (type == Day)
	{
		const QwtScaleDiv *sd = d_plot->axisScale (axis);
		int maj = sd->majCnt();
		int min = sd->minCnt();
		int k=0;
		QString day;
		for (int i=0; i<maj; i++)
		{
			while(k < min)
			{
				if (sd->minMark(k) < sd->majMark(i))
				{
					day = QDate::shortDayName (int(sd->minMark(k))%8);
					if (list.isEmpty () || list.last() != day)
						list<<day;
					k++;
				}
				else
					break;
			}

			day = QDate::shortDayName (int(sd->majMark (i))%8);
			if (list.isEmpty () || list.last() != day)
				list<<day;
		}

		while(k < min)
		{
			day = QDate::shortDayName (int(sd->minMark(k))%8);
			if (list.last() != day)
				list<<day;
			k++;
		}
	}
	else if (type == Month)
	{
		const QwtScaleDiv *sd = d_plot->axisScale (axis);
		int maj = sd->majCnt();
		int min = sd->minCnt();
		int k=0;
		QString day;
		for (int i=0; i<maj; i++)
		{
			while(k < min)
			{
				if (sd->minMark(k) < sd->majMark(i))
				{
					day = QDate::shortMonthName (int(sd->minMark(k))%13);
					if (list.isEmpty () || list.last() != day)
						list<<day;
					k++;
				}
				else
					break;
			}

			day = QDate::shortMonthName (int(sd->majMark (i))%13);
			if (list.isEmpty () || list.last() != day)
				list<<day;
		}

		while(k < min)
		{
			day = QDate::shortMonthName (int(sd->minMark(k))%13);
			if (list.last() != day)
				list<<day;
			k++;
		}
	}

	d_plot->setAxisScaleDraw (axis, new QwtTextScaleDraw(list, axesLineWidth));
	axisType[axis] = type;
#endif
}

void Graph::setLabelsDateTimeFormat(int axis, int type, const QString& formatInfo)
{
#if false
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
		TimeScaleDraw *sd = new TimeScaleDraw (t, list[1], axesLineWidth);
		sd->setOptions (drawAxesBackbone);	
		d_plot->setAxisScaleDraw (axis, sd);	
	}
	else if (type == Date)
	{
		QDate d = QDate::fromString (list[0], Qt::ISODate);
		DateScaleDraw *sd = new DateScaleDraw (d, list[1], axesLineWidth);
		sd->setOptions (drawAxesBackbone);	
		d_plot->setAxisScaleDraw (axis, sd);			
	}	

	axisType[axis] = type;
	axesFormatInfo[axis] = formatInfo;
#endif
}

void Graph::setAxisLabelRotation(int axis, int rotation)
{
#if false
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
#endif
}

int Graph::labelsRotation(int axis)
{
#if false
	ScaleDraw *sclDraw= (ScaleDraw *)d_plot->axisScaleDraw (axis);
	return (int)sclDraw->labelRotation();	
#endif
}

void Graph::setYAxisTitleFont(const QFont &fnt)
{
#if false
	d_plot->setAxisTitleFont (QwtPlot::yLeft,fnt);
	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::setXAxisTitleFont(const QFont &fnt)
{
#if false
	d_plot->setAxisTitleFont (QwtPlot::xBottom,fnt);
	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::setRightAxisTitleFont(const QFont &fnt)
{
#if false
	d_plot->setAxisTitleFont (QwtPlot::yRight,fnt);
	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::setTopAxisTitleFont(const QFont &fnt)
{
#if false
	d_plot->setAxisTitleFont (QwtPlot::xTop,fnt);
	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::setAxisTitleFont(int axis,const QFont &fnt)
{
#if false
	d_plot->setAxisTitleFont (axis,fnt);
	d_plot->replot();
	emit modifiedGraph(); 
#endif
}

QFont Graph::axisTitleFont(int axis)
{
#if false
	return d_plot->axisTitleFont (axis); 
#endif
}

QColor Graph::axisTitleColor(int axis)
{
#if false
	QColor c;
	QwtScale *scale = (QwtScale *)d_plot->axis(axis);
	if (scale)
		c=scale->titleColor();
	return c; 
#endif
}

void Graph::setAxesColors(const QStringList& colors)
{
#if false
	for (int i=0;i<4;i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)
		{
			QPalette pal =scale->palette();
			pal.setColor(QColorGroup::Foreground,QColor(colors[i]));
			scale->setPalette(pal);
		}
	}
#endif
}

QString Graph::saveAxesColors()
{
#if false
	QString s="AxesColors\t";
	QStringList colors;
	QPalette pal;
	int i;
	for (i=0;i<4;i++)
		colors<<(Qt::black).name();

	for (i=0;i<4;i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)	
		{
			pal=scale->palette();
			colors[i]=pal.color(QPalette::Active, QColorGroup::Foreground).name();
		}
	}
	s+=colors.join ("\t")+"\n";
	return s;
#endif
}

QStringList Graph::axesColors()
{
#if false
	QStringList colors;
	QPalette pal;
	int i;
	for (i=0;i<4;i++)
		colors<<(Qt::black).name();

	for (i=0;i<4;i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)	
		{
			pal=scale->palette();
			colors[i]=pal.color(QPalette::Active, QColorGroup::Foreground).name();
		}
	}
	return colors;
#endif
}

QFont Graph::titleFont()
{
#if false
	return d_plot->titleFont();
#endif
}

QColor Graph::titleColor()
{
#if false
	QLabel *title=d_plot->titleLabel();
	QPalette pal =title->palette();
	return pal.color(QPalette::Active, QColorGroup::Foreground);
#endif
}

void Graph::setTitleColor(const QColor & c)
{
#if false
	QLabel *title=d_plot->titleLabel();
	QPalette pal =title->palette();
	pal.setColor(QColorGroup::Foreground,c);
	if (title)
		title->setPalette(pal);
	d_plot->replot();
	emit modifiedGraph(); 
#endif
}

void Graph::setTitleAlignment(int align)
{
#if false
	QLabel *title=d_plot->titleLabel();
	title->setAlignment(align);
	d_plot->replot();
	emit modifiedGraph(); 
#endif
}

void Graph::setTitleFont(const QFont &fnt)
{
#if false
	d_plot->setTitleFont(fnt);
	d_plot->replot();
	emit modifiedGraph(); 
#endif
}

QString Graph::title()
{
#if false
	return d_plot->title();
#endif
}

int Graph::titleAlignment()
{
#if false
	QLabel *title=d_plot->titleLabel();
	return title->alignment();
#endif
}

void Graph::setAutoScale()
{
#if false
	int scaleType=scales[6].toInt();

	bool inverted = false;
	if (scales[7] == "1")
		inverted = true;

	int a;
	for (a = QwtPlot::xBottom;	a<= QwtPlot::xTop; a++)
	{
		if (scaleType == 1)
		{
			if (inverted)
				d_plot->setAxisOptions(a, QwtAutoScale::Logarithmic | QwtAutoScale::Inverted);
			else
				d_plot->setAxisOptions(a, QwtAutoScale::Logarithmic);
		}
		else if (scaleType == 0)
		{
			if (inverted)
				d_plot->setAxisOptions(a, QwtAutoScale::None | QwtAutoScale::Inverted);
			else
				d_plot->setAxisOptions(a, QwtAutoScale::None);
		}
	}

	scaleType=scales[14].toInt();

	if (scales[15] == "1")
		inverted = true;
	else
		inverted = false;	

	for (a = QwtPlot::yLeft; a<= QwtPlot::yRight; a++)
	{
		if (scaleType == 1)
		{
			if (inverted)
				d_plot->setAxisOptions(a, QwtAutoScale::Logarithmic | QwtAutoScale::Inverted);
			else
				d_plot->setAxisOptions(a, QwtAutoScale::Logarithmic);
		}
		else if (scaleType == 0)
		{
			if (inverted)
				d_plot->setAxisOptions(a, QwtAutoScale::None | QwtAutoScale::Inverted);
			else
				d_plot->setAxisOptions(a, QwtAutoScale::None);
		}
	}

	d_plot->setAxisAutoScale(QwtPlot::xBottom);
	d_plot->setAxisAutoScale(QwtPlot::yLeft);
	d_plot->replot();
	d_zoomer->setZoomBase();
	updateScale();
	emit modifiedGraph();
#endif
}

void Graph::setYAxisTitle(const QString& text)
{
#if false
	d_plot->setAxisTitle(0,text);
	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::setXAxisTitle(const QString& text)
{
#if false
	d_plot->setAxisTitle(2,text);
	d_plot->replot(); 
	emit modifiedGraph();
#endif
}

void Graph::setRightAxisTitle(const QString& text)
{
#if false
	d_plot->setAxisTitle(1,text);
	d_plot->replot(); 
	emit modifiedGraph();
#endif
}

void Graph::setTopAxisTitle(const QString& text)
{
#if false
	d_plot->setAxisTitle(3,text);
	d_plot->replot(); 
	emit modifiedGraph();
#endif
}

int Graph::axisTitleAlignment (int axis)
{
#if false
	return d_plot->axisTitleAlignment (axis);
#endif
}

void Graph::setAxesTitlesAlignment(const QStringList& align) 
{
#if false
	for (int i=0;i<4;i++)	
		d_plot->setAxisTitleAlignment(i,align[i+1].toInt());	
#endif
}

void Graph::setXAxisTitleAlignment(int align) 
{
#if false
	d_plot->setAxisTitleAlignment(QwtPlot::xBottom,align);
	d_plot->replot(); 
	emit modifiedGraph();	
#endif
}

void Graph::setYAxisTitleAlignment(int align) 
{
#if false
	d_plot->setAxisTitleAlignment(QwtPlot::yLeft,align);
	d_plot->replot(); 
	emit modifiedGraph();	
#endif
}

void Graph::setTopAxisTitleAlignment(int align) 
{
#if false
	d_plot->setAxisTitleAlignment(QwtPlot::xTop,align);
	d_plot->replot(); 
	emit modifiedGraph();	
#endif
}

void Graph::setRightAxisTitleAlignment(int align) 
{
#if false
	d_plot->setAxisTitleAlignment(QwtPlot::yRight,align);
	d_plot->replot(); 
	emit modifiedGraph();	
#endif
}

void Graph::setAxisTitle(int axis, const QString& text)
{
#if false
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
#endif
}

void Graph::setGridOptions(const GridOptions& options)
{
#if false
	grid=options;

	QColor minColor = color(grid.minorCol);
	QColor majColor = color(grid.majorCol);

	Qt::PenStyle majStyle;
	Qt::PenStyle minStyle;

	switch (grid.majorStyle)
	{
		case 0:
			majStyle=Qt::SolidLine;
			break;
		case 1:
			majStyle=Qt::DashLine;
			break;
		case 2:
			majStyle=Qt::DotLine;
			break;
		case 3:
			majStyle=Qt::DashDotLine;
			break;
		case 4:
			majStyle=Qt::DashDotDotLine;
			break;
	}

	switch (grid.minorStyle)
	{
		case 0:
			minStyle=Qt::SolidLine;
			break;
		case 1:
			minStyle=Qt::DashLine;
			break;
		case 2:
			minStyle=Qt::DotLine;
			break;
		case 3:
			minStyle=Qt::DashDotLine;
			break;
		case 4:
			minStyle=Qt::DashDotDotLine;
			break;
	}

	QPen majPen=QPen (majColor,grid.majorWidth,majStyle);
	d_plot->setGridMajPen (majPen);

	QPen minPen=QPen (minColor,grid.minorWidth,minStyle);
	d_plot->setGridMinPen(minPen);

	if (grid.majorOnX) d_plot->enableGridX (true);
	else if (grid.majorOnX==0) d_plot->enableGridX (false);

	if (grid.minorOnX) d_plot->enableGridXMin (true);
	else if (grid.minorOnX==0) d_plot->enableGridXMin (false);

	if (grid.majorOnY) d_plot->enableGridY (true);
	else if (grid.majorOnY==0) d_plot->enableGridY (false);

	if (grid.minorOnY) d_plot->enableGridYMin (true);
	else d_plot->enableGridYMin (false);

	if (mrkX<0 && grid.xZeroOn)
	{
		mrkX=d_plot->insertMarker();
		d_plot->setMarkerLineStyle(mrkX, QwtMarker::VLine);
		d_plot->setMarkerPos(mrkX, 0.0,0.0);
		d_plot->setMarkerPen(mrkX, QPen(Qt::black, 2,Qt::SolidLine));
	}
	else if (mrkX>=0 && !grid.xZeroOn)
	{
		d_plot->removeMarker(mrkX);
		mrkX=-1;
	}

	if (mrkY<0 && grid.yZeroOn)
	{
		d_plot->removeMarker(mrkY);
		mrkY=d_plot->insertMarker();
		d_plot->setMarkerLineStyle(mrkY, QwtMarker::HLine);
		d_plot->setMarkerPos(mrkY, 0.0,0.0);
		d_plot->setMarkerPen(mrkY, QPen(Qt::black, 2,Qt::SolidLine));
	}
	else if (mrkY>=0 && !grid.yZeroOn)
	{
		d_plot->removeMarker(mrkY);
		mrkY=-1;		
	}	
	emit modifiedGraph();
#endif
}

GridOptions Graph::getGridOptions()
{
#if false
	return grid;
#endif
}

QStringList Graph::scalesTitles()
{
#if false
	QStringList scaleTitles;
	int i,axis;
	for (i=0;i<QwtPlot::axisCnt;i++)
	{
		switch (i)
		{
			case 0:
				axis=2;
				scaleTitles<<d_plot->axisTitle(axis);
				break;

			case 1:
				axis=0;
				scaleTitles<<d_plot->axisTitle(axis);
				break;

			case 2:
				axis=3;
				scaleTitles<<d_plot->axisTitle(axis);
				break;

			case 3:
				axis=1;
				scaleTitles<<d_plot->axisTitle(axis);
				break;
		}
	}
	return scaleTitles;
#endif
}

QStringList Graph::plotLimits()
{
#if false
	return scales;
#endif
}

void Graph::updateSecondaryAxis(int axis)
{
#if false
	int a = QwtPlot::xBottom;
	if (axis == QwtPlot::yRight) 
		a = QwtPlot::yLeft;

	if (d_plot->axisEnabled(a))
	{
		const QwtScaleDiv *sc = d_plot->axisScale(a);
		d_plot->setAxisScale(axis, sc->lBound(), sc->hBound(), sc->majStep());
	}
#endif
}

void Graph::setAxisScale(int axis,const QStringList& s)
{
#if false
	scales=s;

	double start=scales[8*axis+0].toDouble();
	double end=scales[8*axis+1].toDouble();
	double step=scales[8*axis+2].toDouble();
	int majTicks=scales[8*axis+3].toInt();
	int minTicks=scales[8*axis+4].toInt();
	int stepOn=scales[8*axis+5].toInt();
	int scaleType=scales[8*axis+6].toInt();

	int a=QwtPlot::xBottom;
	if (axis) 
		a=QwtPlot::yLeft;

	for (int i = a; i <= a+1; i++)
	{
		if (scaleType == 1)
		{
			if (scales[8*axis+7] == "1")
				d_plot->setAxisOptions(i, QwtAutoScale::Logarithmic | QwtAutoScale::Inverted);
			else
				d_plot->setAxisOptions(i, QwtAutoScale::Logarithmic);

			if (stepOn)	
				d_plot->setAxisScale(i,start,end,step);
			else
			{
				d_plot->setAxisScale(i, start, end, 0.0);
				d_plot->setAxisMaxMajor(i, majTicks);
				d_plot->setAxisMaxMinor(i, minTicks);
			}
		}
		else if (scaleType == 0)
		{
			if (scales[8*axis+7] == "1")
				d_plot->setAxisOptions(i, QwtAutoScale::None | QwtAutoScale::Inverted);
			else
				d_plot->setAxisOptions(i, QwtAutoScale::None);

			if (stepOn)	
				d_plot->setAxisScale(i, start, end, step);
			else
			{
				d_plot->setAxisMaxMajor(i, majTicks);
				d_plot->setAxisMaxMinor(i, minTicks);
				d_plot->setAxisScale(i, start, end, 0.0);
			}
		}
	}
#endif
}

void Graph::setScaleDiv(int axis,const QStringList& s)
{
#if false
	setAxisScale(axis, s);
	emit modifiedGraph();
#endif
}

void Graph::setScales(const QStringList& s)
{
#if false
	for (int axis=0;axis<2;axis++)
		setAxisScale(axis, s);
#endif
}


void Graph::copyCanvas(bool on)
{
#if false
	int w=d_plot->canvas()->width();
	int h=d_plot->canvas()->height();
	if (on)
	{
		QPixmap pix(w,h,-1);
		pix=pix.grabWidget(d_plot->canvas(),0,0,-1,-1 );
		QPixmapCache::insert ("field",pix);
	}
#endif
}

void Graph::shiftCurveSelector(bool up)
{
#if false
	const QwtArray<long> keys = d_plot->curveKeys();

	int index = 0;
	if ( selectedCurve >= 0 )
	{
		for ( uint i = 0; i < keys.count() - 1; i++ )
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

	d_plot->setMarkerPos (startID,curve->x(startPoint),curve->y(startPoint));
	d_plot->setMarkerPos (endID,curve->x(endPoint),curve->y(endPoint));
	d_plot->replot();

	QString info;		
	if ( selectedCursor == startID )
	{
		info="L <=> ";
		selectedPoint=startPoint;
	}
	else if ( selectedCursor == endID )
	{
		info="R <=> ";
		selectedPoint=endPoint;
	}

	info+=curve->title(); 
	info+="[";
	info+=QString::number(selectedPoint);
	info+="]: x=";
	info+=QString::number(curve->x(selectedPoint), 'G', 15);
	info+="; y=";
	info+=QString::number(curve->y(selectedPoint), 'G', 15);

	emit cursorInfo(info);
	emit dataRangeChanged();

	QPainter painter(d_plot->canvas());
	painter.setClipping(true);
	painter.setClipRect(d_plot->canvas()->contentsRect());
	painter.setRasterOp(Qt::NotROP);

	curve->draw(&painter,
			d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
			selectedPoint,selectedPoint);
#endif
}

void Graph::shiftRangeSelector(bool shift)
{		
#if false
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve || curve->dataSize()<=0)
		return;

	QString info;	
	if (shift)
	{
		QwtSymbol symbol=d_plot->markerSymbol (selectedCursor);
		if ( selectedCursor == startID )
		{
			symbol.setPen(QPen(Qt::black,2,Qt::SolidLine));
			d_plot->setMarkerSymbol (selectedCursor,symbol);
			d_plot->setMarkerLinePen(selectedCursor, QPen(Qt::black,1,Qt::DashLine));

			selectedCursor = endID;
			symbol=d_plot->markerSymbol (selectedCursor);
			symbol.setPen(QPen(Qt::red,2,Qt::SolidLine));
			d_plot->setMarkerSymbol (selectedCursor,symbol);
			d_plot->setMarkerLinePen(selectedCursor, QPen(Qt::red,1,Qt::DashLine));

			selectedPoint = endPoint;			
			info="R <=> ";
		}
		else if ( selectedCursor == endID )
		{
			symbol.setPen(QPen(Qt::black,2,Qt::SolidLine));
			d_plot->setMarkerSymbol (selectedCursor,symbol);
			d_plot->setMarkerLinePen(selectedCursor, QPen(Qt::black,1,Qt::DashLine));
			selectedCursor = startID;

			symbol=d_plot->markerSymbol (selectedCursor);
			symbol.setPen(QPen(Qt::red,2,Qt::SolidLine));
			d_plot->setMarkerSymbol (selectedCursor,symbol);
			d_plot->setMarkerLinePen(selectedCursor, QPen(Qt::red,1,Qt::DashLine));

			selectedPoint = startPoint;			
			info="L <=> ";
		}
		d_plot->replot();

		info+=curve->title(); 
		info+="[";
		info+=QString::number(selectedPoint+1);
		info+="]: x=";
		info+=QString::number(curve->x(selectedPoint), 'G', 15);
		info+="; y=";
		info+=QString::number(curve->y(selectedPoint), 'G', 15);

		emit cursorInfo(info);

		QPainter painter(d_plot->canvas());
		painter.setClipping(true);
		painter.setClipRect(d_plot->canvas()->contentsRect());
		painter.setRasterOp(Qt::NotROP);

		curve->draw(&painter,
				d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
				selectedPoint,selectedPoint);
	}
#endif
}

void Graph::moveRangeSelector(bool up)
{	
#if false
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

	d_plot->setMarkerPos (selectedCursor,curve->x(selectedPoint),curve->y(selectedPoint));
	d_plot->replot();

	if ( selectedCursor == startID )
	{
		startPoint = selectedPoint;
		info="L <=> ";
	}
	else if ( selectedCursor == endID )
	{
		endPoint = selectedPoint;
		info="R <=> ";
	}

	info+=curve->title();
	info+="[";
	info+=QString::number(index+1);
	info+="]: x=";
	info+=QString::number(curve->x(index), 'G', 15);
	info+="; y=";
	info+=QString::number(curve->y(index), 'G', 15);

	emit cursorInfo(info);
	emit dataRangeChanged();

	QPainter painter(d_plot->canvas());
	painter.setClipping(true);
	painter.setClipRect(d_plot->canvas()->contentsRect());
	painter.setRasterOp(Qt::NotROP);

	curve->draw(&painter,
			d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
			selectedPoint,selectedPoint);	
#endif
}

//places the active range selector at the selected point
void Graph::moveRangeSelector()
{	
#if false
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

	d_plot->setMarkerPos (selectedCursor,curve->x(selectedPoint),curve->y(selectedPoint));
	d_plot->replot();

	if ( selectedCursor == startID )
	{
		startPoint = selectedPoint;
		info="L <=> ";
	}
	else if ( selectedCursor == endID )
	{
		endPoint = selectedPoint;
		info="R <=> ";
	}

	info+=curve->title(); 
	info+="[";
	info+=QString::number(index+1);
	info+="]: x=";
	info+=QString::number(curve->x(index), 'G', 15);
	info+="; y=";
	info+=QString::number(curve->y(index), 'G', 15);

	emit cursorInfo(info);
	emit dataRangeChanged();

	QPainter painter(d_plot->canvas());
	painter.setClipping(true);
	painter.setClipRect(d_plot->canvas()->contentsRect());
	painter.setRasterOp(Qt::NotROP);

	curve->draw(&painter,
			d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
			selectedPoint,selectedPoint);	
#endif
}

bool Graph::enableRangeSelectors(bool on)
{
#if false
	rangeSelectorsEnabled=on;

	long curveID;
	QwtPlotCurve *curve=0;
	bool success=false;	
	for (int i=n_curves-1;i>=0;i--)
	{
		curveID= c_keys[i]; 
		curve=d_plot->curve(curveID);
		if (curve && curve->dataSize()>0)
		{
			success=true;
			break;
		}			
	}

	if (!success)
	{
		QMessageBox::critical(this,"QtiPlot - Warning",
				"All the curves on this plot are empty!");

		disableRangeSelectors();
		return false;
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
		QwtSymbol symbol=d_plot->curveSymbol(selectedCurve);
		if (symbol.style() != QwtSymbol::None)
		{
			QSize sz=symbol.size();
			d+=QMAX(sz.width(),sz.height());
		}

		startID=d_plot->insertLineMarker (QString(),QwtPlot::xBottom);
		d_plot->setMarkerSymbol(startID,QwtSymbol(QwtSymbol::Cross,
					QBrush(Qt::NoBrush), QPen(Qt::red,2), QSize(d,d)));
		d_plot->setMarkerLineStyle(startID, QwtMarker::VLine);
		d_plot->setMarkerLinePen(startID, QPen(Qt::red,1,Qt::DashLine));
		d_plot->setMarkerPos (startID,curve->x(0),curve->y(0));
		selectedCursor=startID;

		endID=d_plot->insertLineMarker (QString(),QwtPlot::xBottom);
		d_plot->setMarkerLineStyle(endID, QwtMarker::VLine);
		d_plot->setMarkerLinePen(endID, QPen(Qt::black,1,Qt::DashLine));
		d_plot->setMarkerSymbol(endID,QwtSymbol(QwtSymbol::Cross,
					QBrush(Qt::NoBrush), QPen(Qt::black,2), QSize(d,d)));	
		d_plot->setMarkerPos (endID,curve->x(endPoint),curve->y(endPoint));
		d_plot->replot();

		QPainter painter(d_plot->canvas());

		painter.setClipping(true);
		painter.setClipRect(d_plot->canvas()->contentsRect());
		painter.setRasterOp(Qt::NotROP);

		curve->draw(&painter,
				d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),0,0);

		emit dataRangeChanged();
	}
	else
		disableRangeSelectors();	

	return true;
#endif
}

void  Graph::disableRangeSelectors()
{
#if false
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
	rangeSelectorsEnabled=false;	
	d_plot->canvas()->setCursor(Qt::arrowCursor);
#endif
}

void Graph::showPlotPicker(bool on)
{
#if false
	pickerEnabled=on;
	QCursor cursor=QCursor (QPixmap(cursor_xpm),-1,-1);
	if (on)
		d_plot->canvas()->setCursor(cursor);
	else 
		d_plot->canvas()->setCursor(Qt::arrowCursor);
#endif
}

void Graph::startCurveTranslation()
{
#if false
	showPlotPicker(true);
	emit cursorInfo(tr("Curve selected! Move cursor and click to choose a point and double-click/press 'Enter' to finish!"));
#endif
}

void Graph::insertPlottedList(const QStringList& names)
{
#if false
	QwtArray<long> keys= d_plot->curveKeys();
	for (int i=0; i<n_curves; i++)
		d_plot->setCurveTitle(keys[i], names[i]);
#endif
}

QStringList Graph::curvesList()
{	
#if false
	QStringList cList;
	QwtArray<long> keys= d_plot->curveKeys();
	for (int i=0; i<n_curves; i++)
		cList<<d_plot->curveTitle(keys[i]);

	return cList;
#endif
}

QStringList Graph::plotAssociations()
{
#if false
	return associations;
#endif
}

void Graph::setPlotAssociations(const QStringList& newList)
{
#if false
	associations=newList;
#endif
}

void Graph::copyImage()
{
#if false
	QPixmap pic = graphPixmap();
	QImage image = pic.convertToImage(); 	
	QApplication::clipboard()->setData( new Q3ImageDrag (image,d_plot,0) );	
#endif
}

QPixmap Graph::graphPixmap()
{
#if false
	int lw = d_plot->lineWidth();
	int clw = 2*d_plot->canvas()->lineWidth();

	QPixmap pic(d_plot->width() + 2*lw + clw, d_plot->height() + 2*lw + clw, -1, QPixmap::BestOptim);
	pic.fill (QColor(255, 255, 255));
	QPainter paint;
	paint.begin(&pic);

	QRect rect = QRect(lw, lw, d_plot->width() - 2*lw, d_plot->height() - 2*lw);

	QwtPlotLayout *layout= d_plot->plotLayout ();
	layout->activate(d_plot, rect, 0);

	QwtPlotPrintFilter  filter; 
	filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle |
			QwtPlotPrintFilter::PrintCanvasBackground);

	if (d_plot->paletteBackgroundColor() != QColor(255, 255, 255))
		paint.fillRect(rect, d_plot->paletteBackgroundColor());

	d_plot->drawPixmap(&paint, rect);
	d_plot->printFrame(&paint, QRect(lw/2,lw/2, 
				d_plot->width()-lw, d_plot->height()-lw));//draw plot frame

	paint.end();

	//the initial layout is invalidated during the print operation and must be recalculated	
	layout->activate(d_plot, d_plot->rect(), 0);

	return pic;
#endif
}

void Graph::exportImage(const QString& fileName, const QString& fileType,
		int quality, bool transparent)
{
#if false
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
#endif
}

void Graph::exportToEPS(const QString& fname)
{	
#if false
	QPrinter printer;
	printer.setResolution(84);
	printer.setPageSize (QPrinter::A4);
	printer.setColorMode (QPrinter::Color);
	printer.setFullPage(true);
	printer.setOutputToFile(true);
	printer.setOutputFileName(fname);

	PrintFilter  filter(d_plot); 
	filter.setOptions(QwtPlotPrintFilter::PrintAll |QwtPlotPrintFilter::PrintTitle |
			QwtPlotPrintFilter::PrintCanvasBackground);

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
	QRect body = QRect(rect);	
	QPainter paint(&printer);

	int lw = d_plot->lineWidth();
	if (lw > 0)
	{
		body.moveBy (-lw, -lw);
		body.setWidth(body.width() + 2*lw);
		body.setHeight(body.height() + 2*lw);
	}	

	if (d_plot->paletteBackgroundColor() != QColor(255, 255, 255))
		paint.fillRect(body, d_plot->paletteBackgroundColor());

	d_plot->printFrame(&paint, body);
	d_plot->print(&paint, rect, filter);
#endif
}

void Graph::exportToEPS(const QString& fname, int res, QPrinter::Orientation o, 
		QPrinter::PageSize size, QPrinter::ColorMode col)
{	
#if false
	QPrinter printer;
	printer.setResolution(res);
	printer.setPageSize (size);
	printer.setColorMode (col);
	printer.setOrientation(o);
	printer.setFullPage(true);
	printer.setOutputToFile(true);
	printer.setOutputFileName(fname);

	PrintFilter  filter(d_plot); 
	filter.setOptions(QwtPlotPrintFilter::PrintAll |QwtPlotPrintFilter::PrintTitle |
			QwtPlotPrintFilter::PrintCanvasBackground);

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
	QRect body = QRect(rect);	
	QPainter paint(&printer);

	int lw = d_plot->lineWidth();
	if (lw > 0)
	{
		body.moveBy (-lw, -lw);
		body.setWidth(body.width() + 2*lw);
		body.setHeight(body.height() + 2*lw);
	}	

	if (d_plot->paletteBackgroundColor() != QColor(255, 255, 255))
		paint.fillRect(body, d_plot->paletteBackgroundColor());

	d_plot->printFrame(&paint, body);
	d_plot->print(&paint, rect, filter);
#endif
}

void Graph::addBoundingBox(const QString& fname, const QRect& rect)
{
	#if false//
	//adding a bounding box info to EPS files
	QFile f(fname);
	QString szBoxInfo;
	szBoxInfo.sprintf("%%%%BoundingBox: %d %d %d %d\n", 
			rect.x(), rect.y(), rect.width(), rect.height());

	f.open(QIODevice::ReadOnly);

	QTextStream in( &f );
	in.setEncoding( QTextStream::Latin1 );

	QString s=in.readLine()+"\n";
	s+=szBoxInfo;

	in.readLine();
	while (! in.atEnd() ) {s+=in.readLine()+"\n";}

	f.close();
	f.open(QIODevice::WriteOnly);	 
	QTextStream out( &f);
	out.setEncoding( QTextStream::Latin1 );
	out<<s;
	f.close();
#endif
}

void Graph::print()
{
#if false
	QPrinter printer;
	printer.setResolution(84); //empirical value, it works...
	printer.setColorMode (QPrinter::Color);
	printer.setFullPage(true);

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
		QRect body = QRect(rect);	
		QPainter paint(&printer);

		int lw = d_plot->lineWidth();
		if ( lw > 0)
		{
			body.moveBy ( -lw, -lw);
			body.setWidth(body.width() + 2*lw);
			body.setHeight(body.height() + 2*lw);
		}	

		if (d_plot->paletteBackgroundColor() != QColor(255, 255, 255))
			paint.fillRect(body, d_plot->paletteBackgroundColor());

		d_plot->printFrame(&paint, body);

		PrintFilter filter(d_plot); 	
		filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle |
				QwtPlotPrintFilter::PrintCanvasBackground);
		d_plot->print(&paint, rect, filter);
	}
#endif
}

void Graph::exportToWmf(const QString& fname) 
{	//not working
#if false

	/*PrintFilter filter(d_plot); 	
	  filter.setOptions(QwtPlotPrintFilter::PrintAll | QwtPlotPrintFilter::PrintTitle |
	  QwtPlotPrintFilter::PrintCanvasBackground);

	  QWMF m_wmf ;
	  m_wmf.setOutputFileName(fname);

	  QPainter painter( &m_wmf );	
	  d_plot->print(&painter , d_plot->rect(), filter);
	  painter.end(); */
#endif
}

void Graph::movePoints(bool enabled)
{
#if false
	movePointsEnabled=enabled;
	if (enabled)
		d_plot->canvas()->setCursor(Qt::pointingHandCursor);
	else 
		d_plot->canvas()->setCursor(Qt::arrowCursor);
#endif
}

bool Graph::movePointsActivated()
{
#if false
	return movePointsEnabled;
#endif
}

void Graph::removePoints(bool enabled)
{
#if false
	removePointsEnabled=enabled;
	QCursor cursor=QCursor (QPixmap(vizor_xpm),-1,-1);

	if (enabled)
		d_plot->canvas()->setCursor(cursor);
	else 
		d_plot->canvas()->setCursor(Qt::arrowCursor);
#endif
}

bool Graph::removePointActivated()
{
#if false
	return removePointsEnabled;
#endif
}

void Graph::multiPeakFit(int fitType, int peaks)
{
#if false
	showPlotPicker(true);
	n_peaks = peaks;
	selected_peaks = 0;
	peaks_array = new double[2*peaks];
	fit_type = fitType;
	d_plot->canvas()->grabMouse();
#endif
}

bool Graph::selectPeaksOn()
{
#if false
	if (n_peaks && selected_peaks<n_peaks)
		return true;
	else
		return false;
#endif
}

void Graph::translateCurve(int direction)
{
#if false
	translateOn=true;
	translationDirection = direction;
	d_plot->canvas()->setCursor(QCursor (QPixmap(vizor_xpm),-1,-1));
#endif
}

void Graph::translateCurveTo(const QPoint& p)
{
#if false
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
#endif
}

QString Graph::selectedCurveTitle()
{
#if false
	const QwtPlotCurve *c = d_plot->curve(selectedCurve);
	if (c)
		return c->title();
	else
		return QString();
#endif
}

void Graph::selectPeak(const QPoint &)
{
#if false
	const QwtPlotCurve *c = d_plot->curve(selectedCurve);
	if (!c)
		return;

	peaks_array[2*selected_peaks]=c->y(selectedPoint);
	peaks_array[2*selected_peaks+1]=c->x(selectedPoint);

	QwtPlotMarker *mrk = new QwtPlotMarker(d_plot);
	mrk->setLineStyle(QwtMarker::VLine);
	mrk->setLinePen(QPen(Qt::green, 2, Qt::DashLine));
	mrk->setXValue(c->x(selectedPoint));
	d_plot->insertMarker(mrk);
	d_plot->replot();
	selected_peaks++;
	if (selected_peaks == n_peaks)
	{
		showPlotPicker(false);
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		fitMultiPeak(fit_type, c->title());
		QApplication::restoreOverrideCursor();
		//remove peak line markers
		QwtArray<long>mrks = d_plot->markerKeys();
		int n=(int)mrks.count();
		for (int i=0; i<n_peaks; i++)
			d_plot->removeMarker(mrks[n-i-1]);
		n_peaks = 0;
		d_plot->canvas()->releaseMouse();
		return;
	}
	emit cursorInfo(tr("Peak %1 selected! Click to select a point and double-click/press 'Enter' to set the position of the next peak!").arg(QString::number(selected_peaks)));
#endif
}

bool Graph::selectPoint(const QPoint &pos)
{
#if false
	int dist, point;
	double xVal, yVal;

	const int curve = d_plot->closestCurve(pos.x(), pos.y(), dist, xVal, yVal, point);
	if (curve >= 0 && dist < 10)//10 pixels tolerance
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

			d_plot->setMarkerPos (startID,c->x(startPoint),c->y(startPoint));
			d_plot->setMarkerPos (endID,c->x(endPoint),c->y(endPoint));
		}

		selectedCurve = curve;
		selectedPoint = point;
		highlightPoint(true);

		QString info;
		info=c->title();
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
		highlightPoint(false);
		return false;
	}
	return false;
#endif
}

void Graph::selectCurve(const QPoint &pos)
{
#if false
	int dist, point;
	double xVal, yVal;
	const int curve = d_plot->closestCurve(pos.x(), pos.y(), dist, xVal, yVal, point);
	if ( curve >= 0 && dist < 10 ) // 10 pixels tolerance
	{
		selectedCurve = curve;
		selectedPoint = point;
		showCursor(true);

		const QwtPlotCurve *c = d_plot->curve(selectedCurve);
		QString info;
		info=c->title();
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
		showCursor(false);
		selectedCurve = -1;
		selectedPoint = -1;
	}
#endif
}

bool Graph::markerSelected()
{
#if false
	bool selected=false;
	if (selectedMarker>=0)
		selected=true;
	return  selected;
#endif
}

void Graph::removeMarker()
{
#if false
	if (selectedMarker>=0)
	{
		d_plot->removeMarker(selectedMarker);
		d_plot->replot();
		emit modifiedGraph();

		if (selectedMarker==legendMarkerID)
			legendMarkerID=-1;

		if (lines.contains(selectedMarker)>0)
		{
			int i,index=lines.find(selectedMarker,0);
			for (i=index;i<linesOnPlot;i++)
				lines[i]=lines[i+1];
			linesOnPlot--;
			lines.resize(linesOnPlot);
		}

		else if (images.contains(selectedMarker)>0)
		{
			int i,index=images.find(selectedMarker,0);
			int imagesOnPlot=images.size();

			for (i=index;i<imagesOnPlot;i++)
				images[i]=images[i+1];
			imagesOnPlot--;
			images.resize(imagesOnPlot);
		}

		selectedMarker=-1;
	}
#endif
}

void Graph::cutMarker()
{
#if false
	copyMarker();
	removeMarker();
#endif
}

bool Graph::arrowMarkerSelected()
{
#if false
	bool arrow=false;
	if (lines.contains(selectedMarker)>0)
		arrow=true;
	return arrow;
#endif
}

bool Graph::imageMarkerSelected()
{
#if false
	bool image=false;
	if (images.contains(selectedMarker)>0)
		image=true;
	return image;
#endif
}

void Graph::copyMarker()
{
#if false
	if (selectedMarker<0)
	{
		selectedMarkerType=None;
		return ;
	}

	if (lines.contains(selectedMarker))
	{
		LineMarker* mrkL=(LineMarker*) d_plot->marker(selectedMarker);
		auxMrkStart=mrkL->startPoint();
		auxMrkEnd=mrkL->endPoint();
		selectedMarkerType=Arrow;
	}
	else if (images.contains(selectedMarker))
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
#endif
}

void Graph::pasteMarker()
{
#if false
	if (selectedMarkerType==Arrow)
	{
		LineMarker* mrkL=new LineMarker(d_plot);
		mrkL->setColor(auxMrkColor);
		mrkL->setWidth(auxMrkWidth);
		mrkL->setStyle(auxMrkStyle);
		mrkL->setStartPoint(QPoint(auxMrkStart.x()+10,auxMrkStart.y()+10));
		mrkL->setEndPoint(QPoint(auxMrkEnd.x()+10,auxMrkEnd.y()+10));
		mrkL->setStartArrow(startArrowOn);
		mrkL->setEndArrow(endArrowOn);
		mrkL->setHeadLength(auxArrowHeadLength);
		mrkL->setHeadAngle(auxArrowHeadAngle);
		mrkL->fillArrowHead(auxFilledArrowHead);
		long mrkID=d_plot->insertMarker(mrkL);
		linesOnPlot++;
		lines.resize(linesOnPlot);
		lines[linesOnPlot-1]=mrkID;
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

		ImageMarker* mrk= new ImageMarker(photo, d_plot);
		mrk->setFileName(fn);

		QPoint o=d_plot->canvas()->mapFromGlobal(QCursor::pos());
		if (!d_plot->canvas()->contentsRect().contains(o))
			o=QPoint(auxMrkStart.x()+20,auxMrkStart.y()+20);

		mrk->setOrigin(o);
		mrk->setSize(rect.size());
		long mrkID=d_plot->insertMarker(mrk);
		d_plot->replot();

		int imagesOnPlot=images.size();
		imagesOnPlot++;
		images.resize(imagesOnPlot);
		images[imagesOnPlot-1]=mrkID;

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
#endif
}

void Graph::setCopiedMarkerEnds(const QPoint& start, const QPoint& end)
{
#if false
	auxMrkStart=start;
	auxMrkEnd=end;
#endif
}

void Graph::setCopiedTextOptions(int bkg, const QString& text, const QFont& font, 
		const QColor& color, const QColor& bkgColor)
{
#if false
	auxMrkBkg=bkg;
	auxMrkText=text;
	auxMrkFont=font;
	auxMrkColor=color;
	auxMrkBkgColor = bkgColor;
#endif
}

void Graph::setCopiedArrowOptions(int width, Qt::PenStyle style, const QColor& color,
		bool start, bool end, int headLength, 
		int headAngle, bool filledHead)
{
#if false
	auxMrkWidth=width;	
	auxMrkStyle=style;
	auxMrkColor=color;
	startArrowOn=start;
	endArrowOn=end;
	auxArrowHeadLength=headLength;
	auxArrowHeadAngle=headAngle;
	auxFilledArrowHead=filledHead;
#endif
}

bool Graph::titleSelected()
{
#if false
	return isTitleSelected;
#endif
}

void Graph::setTitleSelected(bool on)
{
#if false
	isTitleSelected=on;
#endif
}

void Graph::selectTitle()
{
#if false
	if (!isTitleSelected)
		emit selectedGraph(this);

	d_plot->replot();
	isTitleSelected=true;
	selectedMarker = -1;

	QLabel *title=d_plot->titleLabel();
	QPainter paint (title);
	paint.setPen(QPen(Qt::black,1,Qt::DotLine));
	paint.setRasterOp(Qt::NotROP);	
	paint.drawRect (title->rect());
	paint.end();
#endif
}

void Graph::setTitle(const QString& t)
{
#if false
	QRect rect=d_plot->rect();
	QString oldTitle=d_plot->title();
	d_plot->setTitle (t);	
	if (oldTitle.isEmpty())
	{
		d_plot->resize(rect.width(),rect.height()+1);
		d_plot->resize(rect.width(),rect.height()-1);
	}
	emit modifiedGraph(); 
#endif
}

void Graph::removeTitle()
{
#if false
	if (isTitleSelected)
	{
		QwtPlotLayout *plotLayout=d_plot->plotLayout();
		QRect tRect=plotLayout->titleRect ();
		int height=d_plot->height();
		int width=d_plot->width();	
		d_plot->setTitle("");

		int h=height-tRect.height();
		d_plot->resize(width,h);
		resize(width,h);

		emit modifiedGraph(); 
	}
#endif
}

void Graph::initTitle( bool on)
{
#if false
	if (on)
		d_plot->setTitle("Title");
#endif
}

void Graph::initTitleFont( const QFont& fnt)
{
#if false
	d_plot->setTitleFont(fnt);
#endif
}

void Graph::removeLegend()
{
#if false
	if (legendMarkerID>=0)
	{
		d_plot->removeMarker(legendMarkerID);
		legendMarkerID=-1;
	}
#endif
}

void Graph::updateImageMarker(int x, int y, int width, int height)
{
#if false
	ImageMarker* mrk=(ImageMarker*) d_plot->marker(selectedMarker);
	mrk->setOrigin(QPoint(x,y));
	mrk->setSize(QSize(width,height));
	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::updateLineMarker(const QColor& c,int w,Qt::PenStyle style,bool endArrow, bool startArrow)
{
#if false
	LineMarker* mrkL=(LineMarker*) d_plot->marker(selectedMarker);
	mrkL->setColor(c);
	mrkL->setWidth(w);
	mrkL->setStyle(style);
	mrkL->setEndArrow(endArrow);
	mrkL->setStartArrow(startArrow);

	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::setArrowHeadGeometry(int length, int angle, bool filled)
{
#if false
	LineMarker* mrkL=(LineMarker*) d_plot->marker(selectedMarker);
	if (!mrkL)
		return;

	if (mrkL->headLength() != length)
		mrkL->setHeadLength( length );

	if (mrkL->headAngle() != angle)
		mrkL->setHeadAngle( angle );

	if (mrkL->filledArrowHead() != filled)
		mrkL->fillArrowHead( filled );

	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::updateLineMarkerGeometry(const QPoint& sp,const QPoint& ep)
{
#if false
	LineMarker* mrkL=(LineMarker*) d_plot->marker(selectedMarker);
	if (!mrkL)
		return;

	QPoint startPoint=mrkL->startPoint();
	QPoint endPoint=mrkL->endPoint();

	if (startPoint != sp)
		mrkL->setStartPoint(sp);

	if (endPoint != ep)
		mrkL->setEndPoint(ep);

	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::updateTextMarker(const QString& text,int angle, int bkg,const QFont& fnt,
		const QColor& textColor, const QColor& backgroundColor)
{
#if false
	LegendMarker* mrkL=(LegendMarker*) d_plot->marker(selectedMarker);
	mrkL->setText(text);
	mrkL->setAngle(angle);
	mrkL->setTextColor(textColor);
	mrkL->setBackgroundColor(backgroundColor);
	mrkL->setFont(fnt);
	mrkL->setBackground(bkg);

	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::setLegendText(const QString& text)
{
#if false
	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
			mrk->setText(text);	

		d_plot->replot();
	}
#endif
}

QString Graph::getLegendText()
{
#if false
	QString legend;
	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
			legend=mrk->getText();	
	}
	return legend;
#endif
}

QString Graph::legendText()
{
#if false
	QString text="";
	for (int i=0; i<n_curves; i++)
	{
		const QwtPlotCurve *c = curve(i);
		if (c &&  c_type[i] != ErrorBars)
		{
			text+="\\c{";
			text+=QString::number(i+1);
			text+="}";
			text+=c->title();
			text+="\n";
		}
	}
	return text;
#endif
}

QString Graph::pieLegendText()
{
#if false
	QString text="";
	QwtArray<long> keys= d_plot->curveKeys();	
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
#endif
}

void Graph::moveBy(int dx, int dy)
{
#if false
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
#endif
}

void Graph::removePoint()
{
#if false
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	QwtArray<long> keys= d_plot->curveKeys();
	int id=keys.find(selectedCurve,0);
	QString name=associations[id];
	if (name.contains("=")>0)
	{
		QMessageBox::critical(0,"QtiPlot - Remove point error",
				"This function is not available for function curves!");
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
#endif
}

// Move the selected point
void Graph::move(const QPoint &pos)
{
#if false
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	QwtArray<long> keys= d_plot->curveKeys();
	int id=keys.find(selectedCurve,0);
	if (associations[id].contains("=")>0)
	{
		QMessageBox::critical(0,"QtiPlot - Move point error",
				"This function is not available for function curves!");
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
#endif
}

// Hightlight the selected point
void Graph::highlightPoint(bool showIt)
{
#if false
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	QPainter painter(d_plot->canvas());

	painter.setClipping(true);
	painter.setClipRect(d_plot->canvas()->contentsRect());

	if ( showIt )
		painter.setRasterOp(Qt::NotROP);

	curve->draw(&painter,
			d_plot->canvasMap(curve->xAxis()), d_plot->canvasMap(curve->yAxis()),
			selectedPoint,selectedPoint);
#endif
}

void Graph::showCursor(bool showIt)
{
#if false
	QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	QPainter painter(d_plot->canvas());

	int x=d_plot->transform(QwtPlot::xBottom,curve->x(selectedPoint));
	int y=d_plot->transform(QwtPlot::yLeft,curve->y(selectedPoint));

	d_plot->replot();
	if ( showIt )
	{
		painter.setRasterOp(Qt::NotXorROP);
		QwtMarker mrkCross;
		mrkCross.setLinePen (QPen(Qt::red,1));
		mrkCross.setLineStyle(QwtMarker::Cross);
		mrkCross.setSymbol(QwtSymbol(QwtSymbol::Rect,
					QBrush(Qt::NoBrush), QPen(Qt::black,1), QSize(25,25)));
		mrkCross.draw(&painter,x,y,d_plot->canvas()->contentsRect());				
	}
#endif
}

// Select the next/previous neighbour of the selected point
void Graph::shiftPointCursor(bool up)
{
#if false
	const QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	int index = selectedPoint + (up ? 1 : -1);
	index = (index + curve->dataSize()) % curve->dataSize();

	QString info;
	info=curve->title(); 
	info+="[";
	info+=QString::number(index+1);
	info+="]: x=";
	info+=QString::number(curve->x(index), 'G', 15);
	info+="; y=";
	info+=QString::number(curve->y(index), 'G', 15);

	emit cursorInfo(info);

	if ( index != selectedPoint )
	{
		showCursor(false);
		selectedPoint = index;
		showCursor(true);
	}
#endif
}

void Graph::shiftCurveCursor(bool up)
{// Select the next/previous curve 
#if false
	const QwtArray<long> keys = d_plot->curveKeys();

	int index = 0;
	if ( selectedCurve >= 0 )
	{
		for ( uint i = 0; i < keys.count() - 1; i++ )
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
	{
		showCursor(false);
		selectedCurve = keys[index];
		showCursor(true);
	}

	const QwtPlotCurve *curve = d_plot->curve(selectedCurve);
	if ( !curve )
		return;

	QString info;
	info=curve->title();
	info+="[";
	info+=QString::number(selectedPoint+1);
	info+="]: x=";
	info+=QString::number(curve->x(selectedPoint), 'G', 15);
	info+="; y=";
	info+=QString::number(curve->y(selectedPoint), 'G', 15);
	emit cursorInfo(info);
#endif
}

void Graph::clearPlot()
{
#if false
	d_plot->clear();
	d_plot->replot();
#endif
}

void Graph::changePlotAssociation(Table* t, int curve, const QString& text)
{
#if false
	if (associations[curve] == text)
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
		QStringList ls = QStringList::split (",", text, false );
		d_plot->setCurveTitle(c_keys[curve], ls[1].remove("(Y)"));//update curve name
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
		QStringList ls = QStringList::split (",", text, false );
		d_plot->setCurveTitle(c_keys[curve], ls[1].remove("(Y)"));

		associations[curve] = text;
		updateData(t, curve);
	}
	emit modifiedGraph();
	emit modifiedPlotAssociation();
	QApplication::restoreOverrideCursor();
#endif
}

void Graph::updateCurveData(Table* w, const QString& yColName, int curve)
{
#if false
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
#endif
}

void Graph::updateData(Table* w, int curve)
{
#if false
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
		if (c_type[curve] == HorizontalBars)
			d_plot->setCurveData(curveID, Y, X, it);
		else
			d_plot->setCurveData(curveID, X, Y, it);

		if (xColType == Table::Text)
		{
			if (c_type[curve] == HorizontalBars)
			{
				axisType[QwtPlot::yLeft] = Txt;
				axesFormatInfo[QwtPlot::yLeft] = xcName;
				d_plot->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(xLabels, axesLineWidth));
			}
			else
			{
				axisType[QwtPlot::xBottom] = Txt;
				axesFormatInfo[QwtPlot::xBottom] = xcName;
				d_plot->setAxisScaleDraw (QwtPlot::xBottom, new QwtTextScaleDraw(xLabels, axesLineWidth));
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
			d_plot->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(yLabels, axesLineWidth));
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
					d_plot->setCurveData(c_keys[i], X, Y, it);
			}
		}
	}
#endif
}

void Graph::updateErrorBarsData(Table* w, int curve)
{
#if false
	QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(curve);
	if (!er)
		return;

	QStringList asl = QStringList::split(",",associations[curve], false);
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
#endif
}

void Graph::updatePieCurveData(Table* w, const QString& yColName, int curve)
{
#if false
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
	d_plot->setCurveData(c_keys[curve], X, X, it);
	d_plot->replot();	
#endif
}

void Graph::updateVectorsData(Table* w, int curve)
{
#if false
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

	d_plot->setCurveData(curveID, X, Y, it);
	vect->setVectorEnd(endX, endY);
	updatePlot();
#endif
}

QString Graph::saveEnabledAxes()
{
#if false
	QString list="EnabledAxes\t";
	for (int i = 0;i<QwtPlot::axisCnt;i++)
		list+=QString::number(d_plot->axisEnabled (i))+"\t";

	list+="\n";
	return list;
#endif
}

bool Graph::framed()
{
#if false
	bool frameOn=false;

	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();  
	if (canvas->lineWidth()>0)
		frameOn=true;

	return frameOn;
#endif
}

QColor Graph::canvasFrameColor()
{
#if false
	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();  
	QPalette pal =canvas->palette();
	return pal.color(QPalette::Active, QColorGroup::Foreground);
#endif
}

int Graph::canvasFrameWidth()
{
#if false
	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();  
	return canvas->lineWidth();
#endif
}

void Graph::drawCanvasFrame(const QStringList& frame)
{
#if false
	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();	
	canvas->setLineWidth((frame[1]).toInt());

	QPalette pal =canvas->palette();
	pal.setColor(QColorGroup::Foreground,QColor(frame[2]));
	canvas->setPalette(pal); 
#endif
}

void Graph::drawCanvasFrame(bool frameOn, int width, const QColor& color)
{
#if false
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
#endif
}

void Graph::drawCanvasFrame(bool frameOn, int width)
{
#if false
	if (frameOn)
	{
		QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();	
		canvas->setLineWidth(width);
	}
#endif
}

void Graph::drawAxesBackbones(bool yes)
{
#if false
	if (drawAxesBackbone == yes)
		return;

	drawAxesBackbone = yes;

	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		QwtScale *scale=(QwtScale*) d_plot->axis(i);
		if (scale)
		{
			ScaleDraw *sclDraw= (ScaleDraw *)d_plot->axisScaleDraw (i);
			if (yes)
				sclDraw->setOptions(1);
			else
				sclDraw->setOptions(0);

			scale->repaint();
		}
	}

	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::loadAxesOptions(const QString& s)
{
#if false
	if (s == "1")
		return;

	drawAxesBackbone = false;

	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		QwtScale *scale=(QwtScale*) d_plot->axis(i);
		if (scale)
		{
			ScaleDraw *sclDraw= (ScaleDraw *)d_plot->axisScaleDraw (i);
			sclDraw->setOptions(0);			
			scale->repaint();
		}
	}
#endif
}

int Graph::axesLinewidth()
{
#if false
	return axesLineWidth;
#endif
}

void Graph::setAxesLinewidth(int width)
{
#if false
	if (axesLineWidth == width)
		return;

	axesLineWidth = width;
	d_plot->setTicksLineWidth(axesLineWidth);

	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		QwtScale *scale=(QwtScale*) d_plot->axis(i);
		if (scale)
		{
			ScaleDraw *sclDraw= (ScaleDraw *)d_plot->axisScaleDraw (i);
			sclDraw->setLineWidth(width);
			scale->repaint();
		}
	}
	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::loadAxesLinewidth(int width)
{
#if false
	axesLineWidth = width;
	d_plot->setTicksLineWidth(axesLineWidth);

	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		QwtScale *scale=(QwtScale*) d_plot->axis(i);
		if (scale)
		{
			ScaleDraw *sclDraw= (ScaleDraw *)d_plot->axisScaleDraw (i);
			sclDraw->setLineWidth(width);
			scale->repaint();
		}
	}
#endif
}

QString Graph::saveCanvasFrame()
{
#if false
	QString s="CanvasFrame\t";
	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();
	int w=canvas->lineWidth();
	if (w>0)
	{
		s+=QString::number(w)+"\t";
		s+=canvasFrameColor().name()+"\n";
	}
	else s="";
	return s;
#endif
}

QString Graph::saveFonts()
{
#if false
	int i;
	QString s;
	QStringList list,axesList;
	QFont f;
	list<<"TitleFont";
	f=d_plot->titleFont();
	list<<f.family();
	list<<QString::number(f.pointSize());
	list<<QString::number(f.weight());
	list<<QString::number(f.italic());
	list<<QString::number(f.underline());
	list<<QString::number(f.strikeOut());
	s=list.join ("\t")+"\n";

	for (i=0;i<d_plot->axisCnt;i++)
	{
		f=d_plot->axisTitleFont(i);
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
#endif
}

QString Graph::saveAxesFormulas()
{
#if false
	QString s="AxesFormulas\t";
	s+=axesFormulas.join ("\t");
	return s+"\n";
#endif
}

QString Graph::saveScale()
{
#if false
	QString s="scale\t";
	s+=scales.join ("\t");
	return s+"\n";
#endif
}

QString Graph::saveErrorBars()
{
#if false
	QwtArray<long> keys = d_plot->curveKeys();
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
				QStringList cvs=QStringList::split(",",associations[i],false);

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
				s+=QString::number(er->minusSide())+"\n";
				all+=s;
			}				
		}
	}
	return all;
#endif
}

void Graph::setXAxisTitleColor(const QColor& c)
{
#if false
	QwtScale *scale = (QwtScale *)d_plot->axis(QwtPlot::xBottom);
	if (scale)
	{
		scale->setTitleColor (c);
		emit modifiedGraph();
	}
#endif
}

void Graph::setYAxisTitleColor(const QColor& c)
{
#if false
	QwtScale *scale = (QwtScale *)d_plot->axis(QwtPlot::yLeft);
	if (scale)
	{
		scale->setTitleColor (c);
		emit modifiedGraph();
	}
#endif
}

void Graph::setRightAxisTitleColor(const QColor& c)
{
#if false
	QwtScale *scale = (QwtScale *)d_plot->axis(QwtPlot::yRight);
	if (scale)
	{
		scale->setTitleColor (c);
		emit modifiedGraph();
	}
#endif
}

void Graph::setTopAxisTitleColor(const QColor& c)
{
#if false
	QwtScale *scale = (QwtScale *)d_plot->axis(QwtPlot::xTop);
	if (scale)
	{
		scale->setTitleColor (c);
		emit modifiedGraph();
	}
#endif
}

void Graph::setAxesTitleColor(QStringList l)
{
#if false
	for (int i=0;i<int(l.count()-1);i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		if (scale)
			scale->setTitleColor (QColor(l[i+1]));
	}
#endif
}

QString Graph::saveAxesTitleColors()
{
#if false
	QString s="AxesTitleColors\t";
	for (int i=0;i<4;i++)
	{
		QwtScale *scale = (QwtScale *)d_plot->axis(i);
		QColor c;
		if (scale)
			c=scale->titleColor();
		else
			c=QColor(Qt::black);

		s+=c.name()+"\t";	
	}	
	return s+"\n";
#endif
}

QString Graph::saveTitle()
{
#if false
	QString s="PlotTitle\t";
	s+=title().replace("\n", "<br>")+"\t";

	QLabel *title=d_plot->titleLabel();
	QPalette pal =title->palette();
	QColor color=pal.color(QPalette::Active, QColorGroup::Foreground);
	s+=color.name()+"\t";
	s+=QString::number(title->alignment())+"\n";
	return s;
#endif
}

QString Graph::saveScaleTitles()
{
#if false
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
		s+=(d_plot->axisTitle(a)).replace("\n", "<br>")+"\t";
	}
	return s+"\n";
#endif
}

QString Graph::saveAxesTitleAlignement()
{
#if false
	QString s="AxesTitleAlignment\t";
	QStringList axes;
	int i;
	for (i=0;i<4;i++)
		axes<<QString::number(Qt::AlignHCenter);

	for (i=0;i<4;i++)
	{

		if (d_plot->axisEnabled(i))
			axes[i]=QString::number(d_plot->axisTitleAlignment (i));
	}

	s+=axes.join("\t")+"\n";
	return s;
#endif
}

QString Graph::savePieCurveLayout()
{
#if false
	QString s="PieCurve\t";

	QwtPieCurve *pieCurve=(QwtPieCurve*)curve(0);
	s+=pieCurve->title()+"\t";
	QPen pen=pieCurve->pen();

	s+=QString::number(pen.width())+"\t";
	s+=pen.color().name()+"\t";

	Qt::PenStyle style=pen.style();
	if (style==Qt::SolidLine)
		s+="SolidLine\t";
	else if (style==Qt::DashLine)
		s+="DashLine\t";
	else if (style==Qt::DotLine)
		s+="DotLine\t";	
	else if (style==Qt::DashDotLine)
		s+="DashDotLine\t";		
	else if (style==Qt::DashDotDotLine)
		s+="DashDotDotLine\t";

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
	s+=QString::number(pieRadius)+"\t";
	s+=QString::number(pieCurve->first())+"\n";

	return s;
#endif
}

QString Graph::saveCurveLayout(int index)
{
#if false
	QString s = QString();
	int style = c_type[index];
	QwtPlotCurve *c = (QwtPlotCurve*)curve(index);
	if (c)
	{
		s+=QString::number(style)+"\t";
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
		s+=QString::number(h->end())+"\n";
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
		s+="\n";
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
		s+=QString::number(b->whiskersRange())+"\n";
	}
	else
		s+="\n";

	return s;
#endif
}

QString Graph::saveCurves()
{
#if false
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
				QString name = c->title();				
				if (!name.contains("_") && name.contains("="))
				{
					s+="FunctionCurve\t"+name+"\t";
					s+=QString::number(c->dataSize())+"\t";
					s+=QString::number(c->minXValue())+"\t";
					s+=QString::number(c->x(1)-c->x(0))+"\t";					
				}
				else if (c_type[j] == Box)
					s+="curve\t" + QString::number(c->x(0)) + "\t" + c->title() + "\t";
				else
				{
					QStringList as=QStringList::split(",", associations[j],false);
					s+= "curve\t" + as[0].remove("(X)",true) + "\t";
					s+= as[1].remove("(Y)",true) + "\t";
				}			
				s+=saveCurveLayout(j);
			}
		}
	}
	return s;
#endif
}

QString Graph::saveGridOptions()
{
#if false
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
	s+=QString::number(grid.yZeroOn)+"\n";

	return s;
#endif
}

bool Graph::legendOn()
{
#if false
	bool on=false;
	if (legendMarkerID>=0)	
		on=true;
	return on;
#endif
}

LegendMarker* Graph::legend()
{
#if false
	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		return mrk;	
	}
	else 
		return 0;
#endif
}

void Graph::newLegend(const QFont& fnt, int frameStyle)
{
#if false
	LegendMarker* mrk = new LegendMarker(d_plot);
	mrk->setOrigin(QPoint(10, 10));
	if (piePlot)
		mrk->setText(pieLegendText());
	else
		mrk->setText(legendText());

	mrk->setFont(fnt);
	mrk->setBackground(frameStyle);
	legendMarkerID=d_plot->insertMarker(mrk);

	emit modifiedGraph();
	d_plot->replot();	
#endif
}

void Graph::addTimeStamp(const QFont& fnt, int frameStyle)
{
#if false
	LegendMarker* mrk= new LegendMarker(d_plot);
	mrk->setOrigin(QPoint(d_plot->canvas()->width()/2, 10));

	QDateTime dt = QDateTime::currentDateTime ();
	mrk->setText(dt.toString(Qt::LocalDate));

	mrk->setFont(fnt);
	mrk->setBackground(frameStyle);
	legendMarkerID=d_plot->insertMarker(mrk);

	emit modifiedGraph();
	d_plot->replot();	
#endif
}

QSize Graph::newLegend(const QString& text)
{
#if false
	LegendMarker* mrk= new LegendMarker(d_plot);
	mrk->setOrigin(QPoint(5,5));
	mrk->setText(text);
	mrk->setBackground(LegendMarker::None);
	selectedMarker=d_plot->insertMarker(mrk);
	d_plot->replot();
	QRect rect=mrk->rect();
	return rect.size();
#endif
}

void Graph::insertLegend(const QStringList& lst)
{
#if false
	legendMarkerID = insertTextMarker(lst);
#endif
}

void Graph::insertLegend_obsolete(const QStringList& lst)
{
#if false
	legendMarkerID = insertTextMarker_obsolete(lst);
#endif
}

long Graph::insertTextMarker(const QStringList& list)
{
#if false
	QStringList fList=list;
	LegendMarker* mrk= new LegendMarker(d_plot);
	mrk->setTextColor(QColor(fList[9]));
	mrk->setOrigin(QPoint(fList[1].toInt(),fList[2].toInt()));

	QFont fnt=QFont (fList[3],fList[4].toInt(),fList[5].toInt(),fList[6].toInt());
	fnt.setUnderline(fList[7].toInt());
	fnt.setStrikeOut(fList[8].toInt());
	mrk->setFont(fnt);

	mrk->setBackground(fList[10].toInt());
	mrk->setAngle(fList[11].toInt());
	mrk->setBackgroundColor(QColor(fList[12]));

	QString text;
	int n=(int)fList.count();
	for (int i=0;i<n-13;i++)
		text+=fList[13+i]+"\n";
	mrk->setText(text);

	return d_plot->insertMarker(mrk);	
#endif
}

long Graph::insertTextMarker_obsolete(const QStringList& list)
{
#if false
	QStringList fList=list;
	LegendMarker* mrk= new LegendMarker(d_plot);
	mrk->setTextColor(QColor(fList[9]));
	mrk->setOrigin(QPoint(fList[1].toInt(),fList[2].toInt()));

	QFont fnt=QFont (fList[3],fList[4].toInt(),fList[5].toInt(),fList[6].toInt());
	fnt.setUnderline(fList[7].toInt());
	fnt.setStrikeOut(fList[8].toInt());
	mrk->setFont(fnt);

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
	mrk->setAngle(fList[11].toInt());

	QString text;
	int n=(int)fList.count();
	for (int i=0;i<n-12;i++)
		text+=fList[12+i]+"\n";
	mrk->setText(text);

	return d_plot->insertMarker(mrk);
#endif
}

void Graph::insertLineMarker(QStringList list)
{
#if false
	LineMarker* mrk= new LineMarker(d_plot);
	mrk->setStartPoint(QPoint(list[1].toInt(), list[2].toInt()));
	mrk->setEndPoint(QPoint(list[3].toInt(), list[4].toInt()));
	mrk->setWidth(list[5].toInt());
	mrk->setColor(QColor(list[6]));

	Qt::PenStyle style;
	if (list[7]=="SolidLine")
		style=Qt::SolidLine;
	else if (list[7]=="DashLine")
		style=Qt::DashLine;
	else if (list[7]=="DotLine")
		style=Qt::DotLine;	
	else if (list[7]=="DashDotLine")
		style=Qt::DashDotLine;		
	else if (list[7]=="DashDotDotLine")
		style=Qt::DashDotDotLine;
	mrk->setStyle(style);

	mrk->setEndArrow(list[8]=="1");
	mrk->setStartArrow(list[9]=="1");
	if (list.count()>10)
	{
		mrk->setHeadLength(list[10].toInt());		
		mrk->setHeadAngle(list[11].toInt());		
		mrk->fillArrowHead(list[12]=="1");
	}

	long mrkID=d_plot->insertMarker(mrk);
	lines.resize(++linesOnPlot);
	lines[linesOnPlot-1]=mrkID;
#endif
}

void Graph::insertLineMarker(LineMarker* mrk)
{
#if false
	LineMarker* aux= new LineMarker(d_plot);
	aux->setStartPoint(mrk->startPoint());	
	aux->setEndPoint(mrk->endPoint());
	aux->setWidth(mrk->width());
	aux->setColor(mrk->color());
	aux->setStyle(mrk->style());
	aux->setEndArrow(mrk->getEndArrow());
	aux->setStartArrow(mrk->getStartArrow());
	aux->setHeadLength(mrk->headLength());
	aux->setHeadAngle(mrk->headAngle());
	aux->fillArrowHead(mrk->filledArrowHead());

	long mrkID=d_plot->insertMarker(aux);
	lines.resize(++linesOnPlot);
	lines[linesOnPlot-1]=mrkID;	
#endif
}

LineMarker* Graph::lineMarker(long id)
{
#if false
	return (LineMarker*)d_plot->marker(id);	
#endif
}

ImageMarker* Graph::imageMarker(long id)
{
#if false
	return (ImageMarker*)d_plot->marker(id);	
#endif
}

QwtArray<long> Graph::imageMarkerKeys()
{
#if false
	return images;
#endif
}

QwtArray<long> Graph::lineMarkerKeys()
{
#if false
	return lines;
#endif
}

LegendMarker* Graph::textMarker(long id)
{
#if false
	QwtArray<long> txtMrkKeys=textMarkerKeys();
	return (LegendMarker*)d_plot->marker(id);	
#endif
}

void Graph::insertTextMarker(LegendMarker* mrk)
{
#if false
	LegendMarker* aux= new LegendMarker(d_plot);
	aux->setTextColor(mrk->getTextColor());
	aux->setBackgroundColor(mrk->backgroundColor());
	aux->setOrigin(mrk->rect().topLeft ());
	aux->setFont(mrk->getFont());
	aux->setBackground(mrk->getBkgType());
	aux->setAngle(mrk->getAngle());
	aux->setText(mrk->getText());	
	selectedMarker=d_plot->insertMarker(aux);
#endif
}

QwtArray<long> Graph::textMarkerKeys()
{
#if false
	QwtArray<long> mrkKeys=d_plot->markerKeys();
	QwtArray<long> txtMrkKeys(0);
	int k=0;
	for (int i=0;i<(int)mrkKeys.size();i++)
	{
		if (mrkKeys[i]!=mrkX && mrkKeys[i]!=mrkY &&
				mrkKeys[i]!=startID && mrkKeys[i]!=endID && lines.contains(mrkKeys[i])<=0
				&& images.contains(mrkKeys[i])<=0)
		{
			txtMrkKeys.resize(++k);
			txtMrkKeys[k-1]=mrkKeys[i];
		}
	}
	return txtMrkKeys;
#endif
}

QString Graph::saveMarkers()
{
#if false
	QString s;
	QwtArray<long> texts=textMarkerKeys();
	int i,t=texts.size(),l=lines.size(),im=images.size();
	for (i=0;i<im;i++)
	{	
		ImageMarker* mrkI=(ImageMarker*) d_plot->marker(images[i]);
		s+="ImageMarker\t";
		s+=mrkI->getFileName()+"\t";

		QPoint aux=mrkI->getOrigin();
		s+=QString::number(aux.x())+"\t";
		s+=QString::number(aux.y())+"\t";

		QSize size=mrkI->size();
		s+=QString::number(size.width())+"\t";
		s+=QString::number(size.height())+"\n";
	}

	for (i=0;i<l;i++)
	{	
		LineMarker* mrkL=(LineMarker*) d_plot->marker(lines[i]);
		s+="lineMarker\t";
		QPoint aux=mrkL->startPoint();
		s+=QString::number(aux.x())+"\t";
		s+=QString::number(aux.y())+"\t";
		aux=mrkL->endPoint();
		s+=QString::number(aux.x())+"\t";
		s+=QString::number(aux.y())+"\t";	
		s+=QString::number(mrkL->width())+"\t";
		s+=mrkL->color().name()+"\t";

		Qt::PenStyle style=mrkL->style();
		if (style==Qt::SolidLine)
			s+="SolidLine\t";
		else if (style==Qt::DashLine)
			s+="DashLine\t";
		else if (style==Qt::DotLine)
			s+="DotLine\t";	
		else if (style==Qt::DashDotLine)
			s+="DashDotLine\t";		
		else if (style==Qt::DashDotDotLine)
			s+="DashDotDotLine\t";

		s+=QString::number(mrkL->getEndArrow())+"\t";
		s+=QString::number(mrkL->getStartArrow())+"\t";
		s+=QString::number(mrkL->headLength())+"\t";
		s+=QString::number(mrkL->headAngle())+"\t";
		s+=QString::number(mrkL->filledArrowHead())+"\n";
	}

	for (i=0;i<t;i++)
	{	
		if(texts[i]!=legendMarkerID)
		{
			LegendMarker* mrk=(LegendMarker*) d_plot->marker(texts[i]);
			s+="textMarker\t";
			QRect mRect=mrk->rect();
			s+=QString::number(mRect.x())+"\t";
			s+=QString::number(mRect.y())+"\t";

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

			QStringList textList=QStringList::split ("\n",mrk->getText(),false);
			s+=textList.join ("\t");
			s+="\n";
		}
	}
	return s;
#endif
}

QString Graph::saveLegend()
{
#if false
	QString s="";
	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		s+="Legend\t";
		QRect mRect=mrk->rect();
		s+=QString::number(mRect.x())+"\t";
		s+=QString::number(mRect.y())+"\t";

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

		QStringList textList=QStringList::split ("\n",mrk->getText(),false);
		s+=textList.join ("\t");
		s+="\n";
	}
	return s;		
#endif
}

double Graph::selectedXStartValue()
{
#if false
	QwtPlotCurve *cv = (QwtPlotCurve *)d_plot->curve(selectedCurve);
	return cv->x(startPoint);
#endif
}

double Graph::selectedXEndValue()
{
#if false
	QwtPlotCurve *cv = (QwtPlotCurve *)d_plot->curve(selectedCurve);
	return cv->x(endPoint);
#endif
}

int Graph::curveIndex(long key)
{
#if false
	return c_keys.find (key, 0 );	
#endif
}

long Graph::curveKey(int curve)
{	
#if false
	return c_keys[curve];
#endif
}

int Graph::curveDataSize(int curve)
{
#if false
	QwtPlotCurve *c = this->curve(curve);
	return c->dataSize();
#endif
}

QwtPlotCurve *Graph::curve(int index)
{
#if false
	return (QwtPlotCurve *)d_plot->curve(c_keys[index]);
#endif
}

int Graph::selectedPoints(long curveKey)
{
#if false
	int points;
	QwtPlotCurve *cv = (QwtPlotCurve *)d_plot->curve(curveKey);

	if (endPoint>0)
		points=abs(endPoint-startPoint)+1;
	else
		points=cv->dataSize();
	return points;
#endif
}

CurveLayout Graph::initCurveLayout()
{
#if false
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
#endif
}

CurveLayout Graph::initCurveLayout(int i, int curves, int style)
{
#if false
	CurveLayout cl = initCurveLayout();
	cl.symCol=i%16;//16 is the number of predefined colors in Qt
	cl.fillCol=i%16;
	cl.sType=(i+1)%9; //9 is the number of predefined symbols in Qwt

	if (style == Graph::VerticalDropLines)
		cl.connectType=2;
	else if (style == Graph::Steps)
		cl.connectType=3;
	else if (style == Graph::Spline)
		cl.connectType=5;

	if (style == Graph::VerticalBars || style == Graph::HorizontalBars)
	{
		cl.filledArea=1;
		cl.lCol=0;//black color pen
		cl.aCol=i+1;
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
	}
	return cl;
#endif
}

void Graph::setCurveType(int curve, int style)
{
#if false
	c_type[curve] = style;
#endif
}

void Graph::updateCurveLayout(int index, const CurveLayout *cL)
{
#if false
	QwtPlotCurve *c = this->curve(index);
	if (!c || c_type.isEmpty() || (int)c_type.size() < index)
		return;

	int style = c_type[index];
	if (style == Scatter || style == LineSymbols ||	style == Spline || 
			style == VerticalDropLines || style == Box)
	{
		QPen pen = QPen(color(cL->symCol),cL->penWidth, Qt::SolidLine);
		/*if (!cL->penWidth)
		  pen.setStyle(Qt::NoPen);*/

		if (cL->fillCol != -1)
			c->setSymbol(QwtSymbol(SymbolBox::style(cL->sType),
						QBrush(color(cL->fillCol)), pen, QSize(cL->sSize,cL->sSize)));
		else
			c->setSymbol(QwtSymbol(SymbolBox::style(cL->sType), QBrush(), 
						pen, QSize(cL->sSize,cL->sSize)));
	}
	else
		c->setSymbol(QwtSymbol(QwtSymbol::None, QBrush(), QPen(), QSize(cL->sSize,cL->sSize)));

	c->setPen(QPen(color(cL->lCol),cL->lWidth,getPenStyle(cL->lStyle)));

	if (style == Scatter)
		c->setStyle(0,0); 
	else
		c->setStyle(cL->connectType,0); 

	QBrush brush = QBrush(color(cL->aCol), Qt::NoBrush);
	if (cL->filledArea)
		brush.setStyle(getBrushStyle(cL->aStyle));
	c->setBrush(brush);

	QString yColName=c->title();
	for (int i=0;i<n_curves;i++)
	{
		if (associations[i].contains(yColName) && i!=index && c_type[i] == ErrorBars)
		{
			QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(i);
			if (er)
				er->setSymbolSize(QSize(cL->sSize,cL->sSize));		
		}
	}
#endif
}

void Graph::updateErrorBars(int curve,bool xErr,int width,int cap,const QColor& c,
		bool plus,bool minus,bool through)
{
#if false
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
		associations[curve]=associations[curve].replace("(yErr)","(xErr)",true); 
	else
		associations[curve]=associations[curve].replace("(xErr)","(yErr)",true); 	
#endif
}

void Graph::addErrorBars(Table *w, const QString& yColName, 
		Table *errTable, const QString& errColName,
		int type, int width, int cap, const QColor& color,
		bool through, bool minus,bool plus)
{
#if false
	QwtArray<long> keys = d_plot->curveKeys();
	for (int i = 0; i<n_curves; i++ )
	{
		if (d_plot->curveTitle(keys[i]) == yColName && c_type[i] != ErrorBars)
		{
			QStringList lst = QStringList::split(",", associations[i], false);
			addErrorBars(w, lst[0].remove("(X)"), yColName, errTable, errColName,
					type, width, cap, color, through, minus, plus);

			return;
		}
	}
#endif
}

void Graph::addErrorBars(Table *w, const QString& xColName, const QString& yColName, 
		Table *errTable, const QString& errColName,
		int type, int width, int cap, const QColor& color,
		bool through, bool minus,bool plus)
{
#if false
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
			QwtSymbol symb=d_plot->curveSymbol(curveID);
			size=symb.size();
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

	long errorsID=d_plot->insertCurve(er);
	d_plot->setCurveTitle(errorsID, errColName);	

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
#endif
}

int Graph::pieSize()
{
#if false
	return pieRadius;
#endif
}

Qt::BrushStyle Graph::pieBrushStyle()
{
#if false
	Qt::BrushStyle style=Qt::SolidPattern;
	if (piePlot)
	{
		QwtPieCurve *pieCurve = (QwtPieCurve *)curve(0);
		if (pieCurve->pattern() != style)
			style=pieCurve->pattern();
	}	
	return 	style;
#endif
}

int Graph::pieFirstColor()
{
#if false
	int first=0;
	if (piePlot)
	{
		QwtPieCurve *pieCurve = (QwtPieCurve *)curve(0);
		if (pieCurve->first() != first)
			first=pieCurve->first();
	}	
	return 	first;
#endif
}

QPen Graph::pieCurvePen()
{
#if false
	QPen pen=QPen(QColor(Qt::black),1,Qt::SolidLine);
	if (piePlot)
	{
		QwtPieCurve *pieCurve = (QwtPieCurve *)curve(0);
		if (pieCurve->pen() != pen)
			pen=pieCurve->pen();
	}	
	return 	pen;
#endif
}

void Graph::updatePie(const QPen& pen, const Qt::BrushStyle &brushStyle, int size, int firstColor)
{
#if false
	if (curves()>0)
	{
		QwtPieCurve *pieCurve=(QwtPieCurve*)curve(0);
		if (pieCurve)
		{
			pieCurve->setPen(pen);
			pieCurve->setRadius(size);
			pieCurve->setBrushStyle(brushStyle);
			pieCurve->setFirstColor(firstColor);
			d_plot->replot();

			pieRadius=size;
			emit modifiedGraph();
		}
	}
#endif
}

void Graph::plotPie(QwtPieCurve* curve)
{
#if false
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
	pieRadius=curve->radius();
	pieCurve->setRadius(pieRadius);
	pieCurve->setBrushStyle(curve->pattern());
	pieCurve->setFirstColor(curve->first());
	piePlot=true;

	associations<<curve->title();
#endif
}


void Graph::plotPie(Table* w, const QString& name,const QPen& pen, int brush, int size, int firstColor)
{
#if false
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
	pieCurve->setRadius(size);
	pieCurve->setFirstColor(firstColor);	
	pieCurve->setBrushStyle(getBrushStyle(brush));
	piePlot=true;
	pieRadius=size;
#endif
}

void Graph::plotPie(Table* w, const QString& name)
{
#if false
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

	const int radius = 125;
	int xc=int(rect.width()*0.5+10);
	int yc=int(rect.y()+rect.height()*0.5);

	double PI=4*atan(1.0);
	double angle = 90;	

	for (i = 0;i<it;i++ )
	{
		const double value = Y[i]/sum*360;
		double alabel= (angle-value*0.5)*PI/180.0;	

		const int x=int(xc+radius*cos(alabel));
		const int y=int(yc-radius*sin(alabel));

		LegendMarker* aux= new LegendMarker(d_plot);
		aux->setOrigin(QPoint(x,y));
		aux->setBackground(0);
		aux->setText(QString::number(Y[i]/sum*100,'g',2)+"%");	
		d_plot->insertMarker(aux);

		angle -= value;
	}

	piePlot=true;

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
		d_plot->enableAxis(i,false);

	d_plot->setTitle(QString());

	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();  
	canvas->setLineWidth(1);

	scalePicker->refresh();
	d_plot->replot();
	updateScale();
#endif
}

bool Graph::insertCurvesList(Table* w, const QStringList& names, int style, int lWidth, int sSize)
{
#if false
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
#endif
}

bool Graph::insertCurve(Table* w, const QString& name, int style)
{//provided for convenience
#if false
	int ycol = w->colIndex(name);
	int xcol = w->colX(ycol);

	bool succes = insertCurve(w, w->colName(xcol), name, style);
	if (succes)
		emit modifiedGraph();
	return succes;
#endif
}

bool Graph::insertCurve(Table* w, int xcol, const QString& name, int style)
{//provided for convenience
#if false
	return insertCurve(w, w->colName(xcol), name, style);
#endif
}

bool Graph::insertCurve(Table* w, const QString& xColName, const QString& yColName, int style)
{
#if false
	int xcol=w->colIndex(xColName);
	int ycol=w->colIndex(yColName);
	if (xcol < 0 || ycol < 0)
		return false;

	Q3MemArray<double> X(1),Y(1);
	int i, it=0;

	int xColType = w->columnType(xcol);
	int yColType = w->columnType(ycol);
	QString timeFormat = (w->columnFormat(xcol)); 

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
	if (style == VerticalBars)
	{
		QwtBarCurve *bars=new QwtBarCurve(QwtBarCurve::Vertical, d_plot,0);
		curveID = d_plot->insertCurve(bars);
		d_plot->setCurveStyle(curveID, QwtCurve::UserCurve);
		d_plot->setCurveTitle(curveID, yColName);
	}
	else if (style == HorizontalBars)
	{
		QwtBarCurve *bars=new QwtBarCurve(QwtBarCurve::Horizontal, d_plot,0);
		curveID = d_plot->insertCurve(bars);
		d_plot->setCurveStyle(curveID, QwtCurve::UserCurve);
		d_plot->setCurveTitle(curveID, yColName);
	}
	else if (style == Histogram)
	{
		QwtHistogram *histogram=new QwtHistogram(d_plot,0);
		curveID = d_plot->insertCurve(histogram);
		d_plot->setCurveStyle(curveID, QwtCurve::UserCurve);
		d_plot->setCurveTitle(curveID, yColName);
	}
	else
		curveID = d_plot->insertCurve(yColName);

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = curveID;

	d_plot->setCurvePen(curveID, QPen(Qt::black,widthLine)); 

	if (style == Histogram)
		initHistogram(curveID, Y, it);
	else if (style == HorizontalBars)
		d_plot->setCurveData(curveID, Y, X, it);
	else
		d_plot->setCurveData(curveID, X, Y, it);

	if (xColType == Table::Text )
	{
		if (style == HorizontalBars)
		{
			axesFormatInfo[QwtPlot::yLeft] = xColName;
			axesFormatInfo[QwtPlot::yRight] = xColName;
			axisType[QwtPlot::yLeft] = Txt;
			d_plot->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(xLabels, axesLineWidth));
		}
		else
		{
			axesFormatInfo[QwtPlot::xBottom] = xColName;
			axesFormatInfo[QwtPlot::xTop] = xColName;
			axisType[QwtPlot::xBottom] = Txt;
			d_plot->setAxisScaleDraw (QwtPlot::xBottom, new QwtTextScaleDraw(xLabels, axesLineWidth));
		}
	}
	else if (xColType == Table::Time )
	{
		QString fmtInfo = time0.toString(Qt::TextDate) + ";" + timeFormat;
		if (style == HorizontalBars)
			setLabelsDateTimeFormat(QwtPlot::yLeft, Time, fmtInfo);
		else
			setLabelsDateTimeFormat(QwtPlot::xBottom, Time, fmtInfo);
	}
	else if (xColType == Table::Date )
	{
		QString fmtInfo = date.toString(Qt::ISODate ) + ";" + timeFormat;
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
		d_plot->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(yLabels, axesLineWidth));
	}

	addLegendItem(yColName);
	return true;
#endif
}

void Graph::plotVectorCurve(Table* w, const QStringList& colList, int style)
{
#if false
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

	d_plot->setCurveStyle(curveID, 0,0); 
	d_plot->setCurveTitle(curveID, label);
	d_plot->setCurveData(curveID, X, Y, it);

	addLegendItem(label);	
	updatePlot();
#endif
}

void Graph::updateVectorsLayout(Table *w, int curve, int colorIndex, int width, 
		int arrowLength, int arrowAngle, bool filled, int position,
		const QString& xEndColName, const QString& yEndColName)
{
#if false
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
#endif
}

void Graph::setVectorsLook(int curve, const QColor& c, int width, int arrowLength,
		int arrowAngle, bool filled, int position)
{
#if false
	VectorCurve *vect=(VectorCurve *)this->curve(curve);
	if (!vect)
		return;

	vect->setColor(c);
	vect->setWidth(width);
	vect->setHeadLength(arrowLength);
	vect->setHeadAngle(arrowAngle);
	vect->fillArrowHead(filled);
	vect->setPosition(position);
#endif
}

void Graph::updatePlot()
{
#if false
	if (autoscale && !removePointsEnabled && !movePointsEnabled && 
			!zoomOn() && !rangeSelectorsEnabled)
	{
		d_plot->setAxisAutoScale(QwtPlot::xBottom);
		d_plot->setAxisAutoScale(QwtPlot::yLeft);
	}

	d_plot->replot();
	updateScale();
	d_zoomer->setZoomBase();	
#endif
}

void Graph::initScales()
{
#if false
	for (int i = 0; i<16; i++)
		scales << "";

	const QwtScaleDiv *scDiv=d_plot->axisScale(QwtPlot::xBottom);
	double step=scDiv->majStep();
	scales[0]=QString::number(scDiv->lBound());
	scales[1]=QString::number(scDiv->hBound());
	scales[2]=QString::number(step);
	scales[3]=QString::number(d_plot->axisMaxMajor(QwtPlot::xBottom));
	scales[4]=QString::number(d_plot->axisMaxMinor(QwtPlot::xBottom));

	scDiv=d_plot->axisScale(QwtPlot::yLeft);
	step=scDiv->majStep();
	scales[8]=QString::number(scDiv->lBound());
	scales[9]=QString::number(scDiv->hBound());
	scales[10]=QString::number(step);
	scales[11]=QString::number(d_plot->axisMaxMajor(QwtPlot::yLeft));
	scales[12]=QString::number(d_plot->axisMaxMinor(QwtPlot::yLeft));

	//updating scale maps
	xCanvasMap = d_plot->canvasMap (QwtPlot::xBottom);
	yCanvasMap = d_plot->canvasMap (QwtPlot::yLeft);	
#endif
}

void Graph::updateScale()
{
#if false
	const QwtScaleDiv *scDiv=d_plot->axisScale(QwtPlot::xBottom);
	double step=scDiv->majStep();
	scales[0]=QString::number(scDiv->lBound());
	scales[1]=QString::number(scDiv->hBound());
	scales[2]=QString::number(step);
	scales[3]=QString::number(d_plot->axisMaxMajor(2));
	scales[4]=QString::number(d_plot->axisMaxMinor(2));

	if (!autoscale)
		d_plot->setAxisScale (QwtPlot::xBottom, scDiv->lBound(), scDiv->hBound(), step);

	scDiv=d_plot->axisScale(QwtPlot::yLeft);
	step=scDiv->majStep();
	scales[8]=QString::number(scDiv->lBound());
	scales[9]=QString::number(scDiv->hBound());
	scales[10]=QString::number(step);
	scales[11]=QString::number(d_plot->axisMaxMajor(0));
	scales[12]=QString::number(d_plot->axisMaxMinor(0));

	if (!autoscale)
		d_plot->setAxisScale (QwtPlot::yLeft, scDiv->lBound(), scDiv->hBound(), step);

	//updating scale maps
	xCanvasMap=d_plot->canvasMap (QwtPlot::xBottom);
	yCanvasMap=d_plot->canvasMap (QwtPlot::yLeft);

	resizeMarkers (1, 1);
#endif
}

void Graph::setBarsGap(int curve, int gapPercent, int offset)
{
#if false
	QwtBarCurve *bars=(QwtBarCurve *)this->curve(curve);
	if (!bars || (bars->gap() == gapPercent && bars->offset() == offset))
		return;

	bars->setGap(gapPercent);
	bars->setOffset(offset);
#endif
}

void Graph::removePie()
{	
#if false
	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
			mrk->setText(QString());
	}

	d_plot->removeCurve(c_keys[0]);
	d_plot->replot();

	c_keys.resize(0);
	c_type.resize(0);
	n_curves=0;
	associations.clear();
	piePlot=false;		
	emit modifiedGraph();
#endif
}

/*
 *provided for convenience
 */
void Graph::removeCurve(const QString& s)
{	
#if false
	int index = associations.findIndex(s);
	removeCurve(index);
#endif
}

void Graph::removeCurve(int index)
{		
#if false
	QStringList::Iterator it=associations.at(index);
	associations.remove(it);

	removeLegendItem(index);
	d_plot->removeCurve(c_keys[index]);

	if (piePlot)
	{
		piePlot=false;	
		c_keys.resize(--n_curves);
	}
	else
	{			
		for (int i=index; i<n_curves; i++)
		{
			c_type[i]=c_type[i+1];
			c_keys[i] = c_keys[i+1];
		}

		c_type.resize(--n_curves);
		c_keys.resize(n_curves);

		if (rangeSelectorsEnabled)
		{
			if (n_curves>0)
				shiftCurveSelector(true);
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
	}
	updatePlot();
	emit modifiedGraph();	
#endif
}

void Graph::removeLegendItem(int index)
{
#if false
	if (legendMarkerID<0 || c_type[index] == ErrorBars)
		return;

	LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
	if (!mrk)
		return;

	if (piePlot)
	{
		mrk->setText(QString());
		return;
	}

	QString text=mrk->getText();
	QStringList items=QStringList::split( "\n", text, false);

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
#endif
}

void Graph::addLegendItem(const QString& colName)
{
#if false
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
#endif
}

void Graph::contextMenuEvent(QContextMenuEvent *e)
{
#if false
	if (selectedMarker>=0)
		return;

	emit showContextMenu();
	e->accept();
#endif
}

void Graph::closeEvent(QCloseEvent *e)
{
#if false
	emit closedGraph();
	e->accept();

#endif
}

bool Graph::zoomOn()
{
#if false
	return d_zoomer->isEnabled();
#endif
}

void Graph::zoomed (const QwtDoubleRect &rect)
{
#if false
	d_zoomer->setZoomBase (rect);

	const QwtScaleDiv *scDiv=d_plot->axisScale(QwtPlot::xBottom);
	double step=scDiv->majStep();
	scales[0]=QString::number(scDiv->lBound(),'e',3);
	scales[1]=QString::number(scDiv->hBound(),'e',3);
	scales[2]=QString::number(step);
	scales[3]=QString::number(d_plot->axisMaxMajor(2));
	scales[4]=QString::number(d_plot->axisMaxMinor(2));

	d_plot->setAxisScale (QwtPlot::xTop, scDiv->lBound(), scDiv->hBound(), step);

	scDiv=d_plot->axisScale(QwtPlot::yLeft);
	step=scDiv->majStep();
	scales[8]=QString::number(scDiv->lBound(),'e',3);
	scales[9]=QString::number(scDiv->hBound(),'e',3);
	scales[10]=QString::number(step);
	scales[11]=QString::number(d_plot->axisMaxMajor(0));
	scales[12]=QString::number(d_plot->axisMaxMinor(0));

	d_plot->setAxisScale (QwtPlot::yRight, scDiv->lBound(), scDiv->hBound(), step);
	d_plot->replot();	
	emit modifiedGraph();
#endif
}

void Graph::zoom(bool on)
{
#if false
	d_zoomer->setEnabled(on);

	int prec, fw;
	char f;
	for (int j= 0; j<QwtPlot::axisCnt; j++)
	{
		d_plot->axisLabelFormat(j,f,prec,fw);
		if (prec<4)
			d_plot->setAxisLabelFormat (j,f,4,fw);			
	}

	QCursor cursor=QCursor (QPixmap(lens_xpm),-1,-1);
	if (on)
		d_plot->canvas()->setCursor(cursor);
	else 
		d_plot->canvas()->setCursor(Qt::arrowCursor);
#endif
}

void Graph::drawText(bool on)
{
#if false
	QCursor c=QCursor(Qt::IBeamCursor);
	if (on)
		d_plot->canvas()->setCursor(c);
	else
		d_plot->canvas()->setCursor(Qt::arrowCursor);

	drawTextOn=on;
#endif
}

void Graph::insertImageMarker(ImageMarker* mrk)
{
#if false
	QPixmap photo=mrk->image();
	ImageMarker* mrk2= new ImageMarker(photo,d_plot);
	mrk2->setFileName(mrk->getFileName());
	mrk2->setOrigin(mrk->getOrigin());	
	mrk2->setSize(mrk->size());	
	long mrk2ID=d_plot->insertMarker(mrk2);
	int imagesOnPlot=images.size();
	images.resize(++imagesOnPlot);
	images[imagesOnPlot-1]=mrk2ID;
#endif
}

void Graph::insertImageMarker(const QPixmap& photo, const QString& fileName)
{
#if false
	ImageMarker* mrk= new ImageMarker(photo, d_plot);
	mrk->setFileName(fileName);
	mrk->setOrigin(QPoint(0,0));

	QSize picSize=photo.size();
	int w=d_plot->canvas()->width();
	if (picSize.width()>w)
		picSize.setWidth(w);
	int h=d_plot->canvas()->height();
	if (picSize.height()>h)
		picSize.setHeight(h);

	mrk->setSize(picSize);
	long mrkID=d_plot->insertMarker(mrk);
	d_plot->replot();

	int imagesOnPlot=images.size();
	imagesOnPlot++;
	images.resize(imagesOnPlot);
	images[imagesOnPlot-1]=mrkID;
	emit modifiedGraph();
#endif
}

void Graph::insertImageMarker(const QStringList& options)
{
#if false
	QString fn=options[1];
	QFile f(fn);
	if (!f.exists())
	{ 
		QMessageBox::warning(0,tr("QtiPlot - File open error"), 
				tr("Image file:" 
					"<p><b>"+fn+"</b>" 
					"<p>does not exist anymore!"));
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
		ImageMarker* mrk= new ImageMarker(photo,d_plot);
		mrk->setFileName(fn);
		mrk->setOrigin(QPoint(options[2].toInt(),options[3].toInt()));
		mrk->setSize(QSize(options[4].toInt(),options[5].toInt()));
		long mrkID=d_plot->insertMarker(mrk);
		int imagesOnPlot=images.size();
		images.resize(++imagesOnPlot);
		images[imagesOnPlot-1]=mrkID;
	}
#endif
}

bool Graph::drawTextActive()
{
#if false
	return drawTextOn;
#endif
}

void Graph::drawLine(bool on)
{
#if false
	drawLineOn=on;
#endif
}

bool Graph::drawLineActive()
{
#if false
	return drawLineOn;
#endif
}

void Graph::setFitID(int id)
{
#if false
	fitID=id;
#endif
}

void Graph::insertOldFunctionCurve(const QString& formula, double from, double step, int points)
{
#if false
	functions++;
	associations << formula;

	int i;
	int pos=formula.find("=",0);
	QString function=formula;
	function=function.mid(pos+1,function.length()-pos-1);

	MyParser parser;
	double x;
	parser.DefineVar("x", &x);		
	parser.SetExpr(function.ascii());

	Q3MemArray<double> X(points),Y(points);
	X[0]=from;x=from;Y[0]=parser.Eval();
	for (i = 1;i<points;i++ )
	{
		x+=step;
		X[i]=x;
		Y[i]=parser.Eval();
	}

	long curveID = d_plot->insertCurve(formula);
	d_plot->setCurvePen(curveID, QPen(Qt::black,widthLine)); 
	d_plot->setCurveData(curveID, X, Y, points);

	c_type.resize(++n_curves);
	c_type[n_curves-1]=Line;

	updatePlot();
	scales[5]="1";
	scales[6]="0";
	scales[12]="1";
	scales[13]="0";
#endif
}

void Graph::modifyFunctionCurve(int curve, QString& type,QStringList &formulas,QStringList &vars,QList<double> &ranges,QList<int> &points)
{
#if false
	int i,ndata;
	bool error=false;
	Q3MemArray<double> X(1),Y(1);
	QString label,swap;
	double step;

	QwtPlotCurve *c=this->curve(curve);
	if (!c)
		return;

	QString oldLabel = associations[curve];
	if (type=="Function")
	{
		label="f"+QString::number(functions)+"(x)="+formulas[0]+",x,"+QString::number(ranges[0],'e',6)+","+QString::number(ranges[1],'e',6);
		if (label == oldLabel && c->dataSize() == points[0])
			return;

		ndata=points[0];
		X.resize(points[0]);
		Y.resize(points[0]);
		step=(ranges[1]-ranges[0])/(double) (points[0]-1);
		MyParser parser;
		double x;
		bool error=false;

		try
		{	
			parser.DefineVar(vars[0].ascii(), &x);	
			parser.SetExpr(formulas[0].ascii());

			X[0]=ranges[0];x=ranges[0];Y[0]=parser.Eval();
			for (i = 1;i<ndata;i++ )
			{
				x+=step;
				X[i]=x;
				Y[i]=parser.Eval();
			}
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,"QtiPlot - Input function error", QString::fromStdString(e.GetMsg()));
			error=true;	
		}
	}	

	if (error)
		return;

	if (type=="Parametric plot" || type=="Polar plot")
	{
		if (type=="Parametric plot")
		{
			label="X"+QString::number(functions)+"("+vars[0]+")="+formulas[0]+",Y"+QString::number(functions)+"("+vars[0]+")="+formulas[1]+","+vars[0]+","+QString::number(ranges[0],'e',6)+","+QString::number(ranges[1],'e',6);
		}
		if (type=="Polar plot")
		{
			label="R"+QString::number(functions)+"("+vars[0]+")="+formulas[0]+",Theta"+QString::number(functions)+"("+vars[0]+")="+formulas[1]+","+vars[0]+","+QString::number(ranges[0],'e',6)+","+QString::number(ranges[1],'e',6);
			swap=formulas[0];
			formulas[0]="("+swap+")*cos("+formulas[1]+")";
			formulas[1]="("+swap+")*sin("+formulas[1]+")";
		}

		if (label == oldLabel && c->dataSize() == points[0])
			return;

		X.resize(points[0]);
		Y.resize(points[0]);
		step=(ranges[1]-ranges[0])/(double) (points[0]-1);	
		MyParser xparser;
		MyParser yparser;
		double par;
		ndata=points[0];

		try
		{
			xparser.DefineVar(vars[0].ascii(), &par);	
			yparser.DefineVar(vars[0].ascii(), &par);	
			xparser.SetExpr(formulas[0].ascii());
			yparser.SetExpr(formulas[1].ascii());
			par=ranges[0];
			for (i = 0;i<points[0];i++ )
			{
				X[i]=xparser.Eval();
				Y[i]=yparser.Eval();
				par+=step;
			}
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,"QtiPlot - Input function error",QString::fromStdString(e.GetMsg()));
			error=true;	
		}
	}		

	if (error)
		return;

	associations[curve] = label;
	c->setData(X, Y, ndata);
	c->setTitle(label);	

	//update the legend marker	
	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
		{
			QString text=mrk->getText();
			text.replace(oldLabel, label);
			mrk->setText(text);
		}
	}

	updatePlot();
	emit modifiedGraph();
	emit modifiedFunction();
#endif
}

void Graph::addFunctionCurve(QString& type,QStringList &formulas,QStringList &vars,QList<double> &ranges,QList<int> &points)
{
#if false
	int i,ndata;
	bool error=false;
	Q3MemArray<double> X(1),Y(1);
	QString label,swap;
	double step;

	if (type=="Function")
	{
		ndata=points[0];
		X.resize(points[0]);
		Y.resize(points[0]);
		step=(ranges[1]-ranges[0])/(double) (points[0]-1);
		MyParser parser;
		double x;
		bool error=false;

		try
		{	
			parser.DefineVar(vars[0].ascii(), &x);	
			parser.SetExpr(formulas[0].ascii());

			X[0]=ranges[0];x=ranges[0];Y[0]=parser.Eval();
			for (i = 1;i<ndata;i++ )
			{
				x+=step;
				X[i]=x;
				Y[i]=parser.Eval();
			}
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,"QtiPlot - Input function error", QString::fromStdString(e.GetMsg()));
			error=true;	
		}
		label="f"+QString::number(functions)+"(x)="+formulas[0]+",x,"+QString::number(ranges[0],'e',6)+","+QString::number(ranges[1],'e',6);
	}	

	if (error)
		return;

	if (type=="Parametric plot" || type=="Polar plot")
	{
		X.resize(points[0]);
		Y.resize(points[0]);
		step=(ranges[1]-ranges[0])/(double) (points[0]-1);	
		MyParser xparser;
		MyParser yparser;
		double par;
		ndata=points[0];
		if (type=="Parametric plot")
			label="X"+QString::number(functions)+"("+vars[0]+")="+formulas[0]+",Y"+QString::number(functions)+"("+vars[0]+")="+formulas[1]+","+vars[0]+","+QString::number(ranges[0],'e',6)+","+QString::number(ranges[1],'e',6);
		if (type=="Polar plot")
		{
			label="R"+QString::number(functions)+"("+vars[0]+")="+formulas[0]+",Theta"+QString::number(functions)+"("+vars[0]+")="+formulas[1]+","+vars[0]+","+QString::number(ranges[0],'e',6)+","+QString::number(ranges[1],'e',6);
			swap=formulas[0];
			formulas[0]="("+swap+")*cos("+formulas[1]+")";
			formulas[1]="("+swap+")*sin("+formulas[1]+")";
		}
		try
		{
			xparser.DefineVar(vars[0].ascii(), &par);	
			yparser.DefineVar(vars[0].ascii(), &par);	
			xparser.SetExpr(formulas[0].ascii());
			yparser.SetExpr(formulas[1].ascii());
			par=ranges[0];
			for (i = 0;i<points[0];i++ )
			{
				X[i]=xparser.Eval();
				Y[i]=yparser.Eval();
				par+=step;
			}
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,"QtiPlot - Input function error",QString::fromStdString(e.GetMsg()));
			error=true;	
		}
	}		

	if (error)
		return;

	functions++;
	associations << label;

	QColor c=QColor(Qt::black);
	long curveID = d_plot->insertCurve(label);
	d_plot->setCurvePen(curveID, QPen(Qt::black,widthLine)); 
	d_plot->setCurveData(curveID, X, Y, ndata);

	c_type.resize(++n_curves);
	c_type[n_curves-1]=Line;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = curveID;

	addLegendItem(label);
	updatePlot();
	emit modifiedGraph();
#endif
}

void Graph::insertFunctionCurve(const QString& formula, double from, double step, int points)
{
#if false
	QStringList curve;
	QString type;
	QStringList formulas;
	QStringList variables;	
	QList<double> ranges;
	QList<int> varpoints;

	if (!formula.contains(","))	//version < 0.4.3
		insertOldFunctionCurve(formula,from, step, points);
	else
	{//version >= 0.4.3
		curve=QStringList::split (",",formula,true);
		if (curve[0][0]=='f')
		{
			type="Function";
			formulas+=curve[0].section('=',1,1);
			variables+=curve[1];
			ranges+=curve[2].toDouble();
			ranges+=curve[3].toDouble();
			varpoints[0]=points;
		}
		else if (curve[0][0]=='X')
		{
			type="Parametric plot";
			formulas+=curve[0].section('=',1,1);
			formulas+=curve[1].section('=',1,1);	
			variables+=curve[2];
			ranges+=curve[3].toDouble();
			ranges+=curve[4].toDouble();
			varpoints[0]=points;
		}
		else if (curve[0][0]=='R')
		{
			type="Polar plot";
			formulas+=curve[0].section('=',1,1);
			formulas+=curve[1].section('=',1,1);	
			variables+=curve[2];
			ranges+=curve[3].toDouble();
			ranges+=curve[4].toDouble();
			varpoints[0]=points;
		}
		addFunctionCurve(type,formulas,variables,ranges,varpoints);	 
	}
#endif
}

void Graph::createWorksheet(const QString& name)
{
#if false
	QwtArray<long> keys = d_plot->curveKeys();
	for (int i=0; i<n_curves; i++)	
	{
		QwtPlotCurve *cv = (QwtPlotCurve *)d_plot->curve(keys[i]);
		if (cv->title() == name)
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
#endif
}

bool Graph::lineProfile()
{
#if false
	return lineProfileOn;	
#endif
}

QString Graph::saveToString()
{
#if false
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
	s+=saveTicksType();
	s+="TicksLength\t"+QString::number(minorTickLength())+"\t"+
		QString::number(majorTickLength())+"\n";
	s+=saveEnabledTickLabels();
	s+=saveAxesColors();
	s+=saveAxesBaseline();
	s+=saveCanvasFrame();
	s+=saveLabelsRotation();
	s+=saveCurves();			
	s+=saveErrorBars();
	s+=saveScale();
	s+=saveAxesFormulas();
	s+=saveLabelsFormat();
	s+=saveAxesLabelsType();
	s+="DrawAxesBackbone\t"+QString::number(drawAxesBackbone)+"\n";
	s+="AxesLineWidth\t"+QString::number(axesLineWidth)+"\n";

	if (legendMarkerID>0)
		s+=saveLegend();
	s+=saveMarkers();

	s+="</graph>\n";
	return s;
#endif
}

QString Graph::saveAsTemplate() 
{
#if false
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
	s+=saveTicksType();
	s+="TicksLength\t"+QString::number(minorTickLength())+"\t"+
		QString::number(majorTickLength())+"\n";
	s+=saveEnabledTickLabels();
	s+=saveAxesColors();
	s+=saveAxesBaseline();
	s+=saveCanvasFrame();
	s+=saveLabelsRotation();
	s+=saveScale();
	s+=saveAxesFormulas();
	s+=saveLabelsFormat();
	s+=saveAxesLabelsType();
	s+="DrawAxesBackbone\t"+QString::number(drawAxesBackbone)+"\n";
	s+="AxesLineWidth\t"+QString::number(axesLineWidth)+"\n";
	s+=saveMarkers();
	s+="</graph>\n";
	return s;
#endif
}

void Graph::showIntensityTable()
{	
#if false
	ImageMarker* mrk=(ImageMarker*) d_plot->marker(selectedMarker);
	if (!mrk)
		return;

	QPixmap pic=mrk->image();
	emit createIntensityTable(pic);
#endif
}

void Graph::resizeMarkers (double w_ratio, double h_ratio)
{
#if false
	if (ignoreResize || (w_ratio == 1 && h_ratio == 1))
		return;

	Q3MemArray<long> texts=textMarkerKeys();

	QwtDiMap xmap=d_plot->canvasMap (QwtPlot::xBottom);
	QwtDiMap ymap=d_plot->canvasMap (QwtPlot::yLeft);

	int i=0;
	for (i=0;i<(int)lines.size();i++)
	{			
		LineMarker* mrkL=(LineMarker*)d_plot->marker(lines[i]);
		QPoint start=mrkL->startPoint();
		QPoint end=mrkL->endPoint();

		double r0_x=xCanvasMap.invTransform(start.x());
		double r0_y=yCanvasMap.invTransform(start.y());	
		double r1_x=xCanvasMap.invTransform(end.x());
		double r1_y=yCanvasMap.invTransform(end.y());

		int x0=xmap.transform(r0_x);
		int x1=xmap.transform(r1_x);	
		int y0=ymap.transform(r0_y);
		int y1=ymap.transform(r1_y);

		mrkL->setStartPoint(QPoint(x0,y0));
		mrkL->setEndPoint(QPoint(x1,y1));
	}

	for (i=0;i<(int)texts.size();i++)
	{
		LegendMarker* mrkT = (LegendMarker*) d_plot->marker(texts[i]);
		QPoint o=mrkT->rect().topLeft();
		double d_ox=xCanvasMap.invTransform(o.x());
		double d_oy=yCanvasMap.invTransform(o.y());

		int ox=xmap.transform(d_ox);	
		int oy=ymap.transform(d_oy);
		mrkT->setOrigin(QPoint(ox,oy));	
	}

	for (i=0;i<(int)images.size();i++)
	{
		ImageMarker* mrk = (ImageMarker*) d_plot->marker(images[i]);
		QPoint o=mrk->getOrigin();
		double d_ix=xCanvasMap.invTransform(o.x());
		double d_iy=yCanvasMap.invTransform(o.y());
		int ix=xmap.transform(d_ix);	
		int iy=ymap.transform(d_iy);	
		mrk->setOrigin(QPoint(ix,iy));

		QSize size = mrk->size();
		mrk->setSize(QSize(int(size.width()*w_ratio), int(size.height()*h_ratio)));
	}

	xCanvasMap=xmap;
	yCanvasMap=ymap;

	d_plot->replot();
#endif
}

void Graph::resizeEvent ( QResizeEvent *e )
{
#if false
	if (ignoreResize || !this->isVisible())
		return;

	if (autoScaleFonts)
	{
		QSize oldSize=e->oldSize();
		QSize size=e->size();

		double ratio=(double)size.height()/(double)oldSize.height();
		scaleFonts(ratio);
	}
#endif
}

void Graph::scaleFonts(double factor)
{
#if false
	QFont font;
	Q3MemArray<long> texts=textMarkerKeys();
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
		if (lblFormat[i] == Superscripts)
		{	
			QwtSupersciptsScaleDraw *sd= (QwtSupersciptsScaleDraw *)d_plot->axisScaleDraw (i);
			sd->setFont(font);
		}
		font = axisTitleFont(i);
		font.setPointSizeFloat(factor*font.pointSizeFloat());
		d_plot->setAxisTitleFont (i, font);
	}

	font = d_plot->titleFont();
	font.setPointSizeFloat(factor*font.pointSizeFloat());
	d_plot->setTitleFont(font);

	d_plot->replot();
#endif
}

void Graph::changeMargin (int d)
{
#if false
	if (d_plot->margin() == d)
		return;

	d_plot->setMargin(d);
	emit modifiedGraph();
#endif
}

void Graph::drawBorder (int width, const QColor& color)
{
#if false
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
#endif
}

void Graph::setBorder (int width, const QColor& color)
{
#if false
	if (d_plot->frameColor() == color && width == d_plot->lineWidth())
		return;

	QPalette pal = d_plot->palette();
	pal.setColor(QColorGroup::Foreground, color);

	d_plot->setPalette(pal);
	d_plot->setLineWidth(width);
#endif
}

void Graph::setBackgroundColor(const QColor& color)
{
#if false
	Q3MemArray<long> texts=textMarkerKeys();
	int i;	
	for (i=0; i<(int)texts.size(); i++)
	{
		LegendMarker* mrk = (LegendMarker*) d_plot->marker(texts[i]);
		if (d_plot->paletteBackgroundColor() == mrk->backgroundColor())
			mrk->setBackgroundColor(color);	
	}

	d_plot->setPaletteBackgroundColor(color);

	QLabel *title=d_plot->titleLabel ();
	title->setPaletteBackgroundColor(color);

	for (i=0;i<QwtPlot::axisCnt;i++)
	{
		QwtScale *scale=(QwtScale *) d_plot->axis (i);
		if (scale)
			scale->setPaletteBackgroundColor(color);
	}

	QwtPlotCanvas *plotCanvas = d_plot->canvas();
	plotCanvas->setPaletteBackgroundColor(color);

	d_plot->replot();
	emit modifiedGraph();
#endif
}

Qt::BrushStyle Graph::getBrushStyle(int style)
{
#if false
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
#endif
}

Qt::PenStyle Graph::getPenStyle(int style)
{
#if false
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
#endif
}

Qt::PenStyle Graph::getPenStyle(const QString& s)
{
#if false
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
#endif
}

int Graph::obsoleteSymbolStyle(int type)
{
#if false
	if (type <= 4)
		return type+1;
	else
		return type+2;
#endif
}

int Graph::curveType(int curveIndex)
{
#if false
	if (curveIndex < (int)c_type.size())
		return c_type[curveIndex];
	else 
		return -1;
#endif
}

QColor Graph::color(int item)
{
#if false
	return ColorBox::color(item);
#endif
}

void Graph::showPlotErrorMessage(QWidget *parent, const QStringList& emptyColumns)
{
#if false
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
#endif
}

void Graph::moveMarkerBy(int dx, int dy)
{
#if false
	Q3MemArray<long> texts = textMarkerKeys();		
	bool line = false, image = false;
	if (lines.contains(selectedMarker))
	{
		LineMarker* mrk=(LineMarker*)d_plot->marker(selectedMarker);

		QPoint point=mrk->startPoint();			
		mrk->setStartPoint(QPoint(point.x() + dx, point.y() + dy));

		point = mrk->endPoint();
		mrk->setEndPoint(QPoint(point.x() + dx, point.y() + dy));	

		line = true;
	}
	else if (images.contains(selectedMarker))
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
#endif
}

void Graph::showTitleContextMenu()
{
#if false
	Q3PopupMenu titleMenu(this);
	titleMenu.insertItem(QPixmap(cut_xpm), tr("&Cut"),this, SLOT(cutTitle()));
	titleMenu.insertItem(QPixmap(copy_xpm), tr("&Copy"),this, SLOT(copyTitle()));
	titleMenu.insertItem(tr("&Delete"),this, SLOT(removeTitle()));
	titleMenu.insertSeparator();
	titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(viewTitleDialog()));
	titleMenu.exec(QCursor::pos());
#endif
}

void Graph::cutTitle()
{
#if false
	QApplication::clipboard()->setData(new Q3TextDrag(d_plot->title(), d_plot->titleLabel(), 0));
	removeTitle();
#endif
}

void Graph::copyTitle()
{
#if false
	QApplication::clipboard()->setData(new Q3TextDrag(d_plot->title(), d_plot->titleLabel(), 0));
#endif
}

void Graph::removeAxisTitle()
{
#if false
	d_plot->setAxisTitle(selectedAxis, QString());
	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::cutAxisTitle()
{
#if false
	copyAxisTitle();
	removeAxisTitle();
#endif
}

void Graph::copyAxisTitle()
{
#if false
	QApplication::clipboard()->setData(new Q3TextDrag(d_plot->axisTitle(selectedAxis), 
				(QWidget *)d_plot->axis(selectedAxis), 0));
#endif
}

void Graph::showAxisTitleMenu(int axis)
{
#if false
	selectedAxis = axis;

	Q3PopupMenu titleMenu(this);
	titleMenu.insertItem(QPixmap(cut_xpm), tr("&Cut"), this, SLOT(cutAxisTitle()));
	titleMenu.insertItem(QPixmap(copy_xpm), tr("&Copy"), this, SLOT(copyAxisTitle()));
	titleMenu.insertItem(tr("&Delete"),this, SLOT(removeAxisTitle()));
	titleMenu.insertSeparator();
	switch (axis)
	{
		case QwtScale::Bottom:
			titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(xAxisTitleDblClicked()));
			break;

		case QwtScale::Left:
			titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(yAxisTitleDblClicked()));
			break;

		case QwtScale::Top:
			titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(topAxisTitleDblClicked()));
			break;

		case QwtScale::Right:
			titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(rightAxisTitleDblClicked()));
			break;
	}

	titleMenu.exec(QCursor::pos());
#endif
}

void Graph::showAxisContextMenu(int axis)
{
#if false
	selectedAxis = axis;

	Q3PopupMenu menu(this);
	menu.setCheckable(true);
	menu.insertItem(QPixmap(unzoom_xpm), tr("&Rescale to show all"), this, SLOT(setAutoScale()), tr("Ctrl+Shift+R"));
	menu.insertSeparator();
	menu.insertItem(tr("&Hide axis"), this, SLOT(hideSelectedAxis()));

	int gridsID = menu.insertItem(tr("&Show grids"), this, SLOT(showGrids()));
	if (selectedAxis == QwtScale::Left || selectedAxis == QwtScale::Right)
	{
		if (grid.majorOnY) 
			menu. setItemChecked(gridsID, true);
	}
	else if (selectedAxis == QwtScale::Bottom || selectedAxis == QwtScale::Top)
	{
		if (grid.majorOnX) 
			menu. setItemChecked(gridsID, true);
	}

	menu.insertSeparator();
	menu.insertItem(tr("&Scale..."), this, SLOT(showScaleDialog()));
	menu.insertItem(tr("&Properties..."), this, SLOT(showAxisDialog()));
	menu.exec(QCursor::pos());
#endif
}

void Graph::showAxisDialog()
{
#if false
	if (selectedAxis == QwtScale::Bottom)
		selectedAxis = QwtScale::Top;
	else if (selectedAxis == QwtScale::Top)
		selectedAxis = QwtScale::Bottom;

	emit showAxisDialog(selectedAxis);
#endif
}

void Graph::showScaleDialog()
{
#if false
	if (selectedAxis == QwtScale::Bottom)
		selectedAxis = QwtScale::Top;
	else if (selectedAxis == QwtScale::Top)
		selectedAxis = QwtScale::Bottom;

	emit axisDblClicked(selectedAxis);
#endif
}

void Graph::hideSelectedAxis()
{
#if false
	d_plot->enableAxis(selectedAxis, false);
	scalePicker->refresh();
	emit modifiedGraph();
#endif
}

void Graph::showGrids()
{
#if false
	if (selectedAxis == QwtScale::Left || selectedAxis == QwtScale::Right)
	{
		if (grid.majorOnY) 
		{
			grid.majorOnY = 0;
			d_plot->enableGridY (false);
		}
		else
		{
			grid.majorOnY = 1;
			d_plot->enableGridY (true);
		}

		if (grid.minorOnY) 
		{
			grid.minorOnY = 0;
			d_plot->enableGridYMin (false);
		}
		else 
		{
			grid.minorOnY = 1;
			d_plot->enableGridYMin (true);
		}
	}
	else if (selectedAxis == QwtScale::Bottom || selectedAxis == QwtScale::Top)
	{
		if (grid.majorOnX) 
		{
			grid.majorOnX = 0;
			d_plot->enableGridX (false);
		}
		else 
		{
			grid.majorOnX = 1;
			d_plot->enableGridX (true);
		}

		if (grid.minorOnX) 
		{
			grid.minorOnX = 0;
			d_plot->enableGridXMin (false);
		}
		else 
		{
			grid.minorOnX = 1;
			d_plot->enableGridXMin (true);
		}
	}
	d_plot->replot();
	emit modifiedGraph();
#endif
}

void Graph::copy(Graph* g)
{
#if false
	int i;
	Plot *plot = g->plotWidget();
	d_plot->setMargin(plot->margin());
	setBackgroundColor(plot->paletteBackgroundColor());
	setBorder(plot->lineWidth(), plot->frameColor());

	enableAxes(g->enabledAxes());
	setAxesColors(g->axesColors());
	setTicksType(g->ticksType());
	setAxesBaseline(g->axesBaseline());
	drawAxesBackbones(g->drawAxesBackbone);

	setGridOptions(g->grid);
	setTitle(g->title());
	setTitleFont(g->titleFont());
	setTitleColor(g->titleColor());
	setTitleAlignment(g->titleAlignment());
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

	setAxesLinewidth(g->axesLineWidth);
	setTicksLength(g->minorTickLength(), g->majorTickLength());
	removeLegend();

	associations = g->associations;
	if (g->isPiePlot())
		plotPie((QwtPieCurve*)g->curve(0));
	else
	{
		for (i=0;i<g->curves();i++)
		{
			QwtPlotCurve *cv = (QwtPlotCurve *)g->curve(i);
			int n = cv->dataSize();
			int style = g->c_type[i];
			double *x = new double[n];
			double *y = new double[n];
			for (int j=0; j<n; j++)
			{
				x[j]=cv->x(j);
				y[j]=cv->y(j);
			}
			long curveID;
			if (style == VerticalBars || style == HorizontalBars)
			{
				QwtBarCurve *b2=new QwtBarCurve(((QwtBarCurve*)cv)->orientation(), d_plot,0);
				b2->copy((const QwtBarCurve*)cv);
				curveID = d_plot->insertCurve(b2);
			}
			else if (style == ErrorBars)
			{
				QwtErrorPlotCurve *e2 = new QwtErrorPlotCurve(d_plot,0);
				e2->copy((const QwtErrorPlotCurve*)cv);
				curveID = d_plot->insertCurve(e2);
			}
			else if (style == Histogram)
			{
				QwtHistogram *h2 = new QwtHistogram(d_plot,0);
				h2->copy((const QwtHistogram*)cv);
				curveID = d_plot->insertCurve(h2);
			}
			else if (style == VectXYXY || style == VectXYAM)
			{
				VectorCurve::VectorStyle vs = VectorCurve::XYXY;
				if (style == VectXYAM)
					vs = VectorCurve::XYAM;
				VectorCurve *v2 = new VectorCurve(vs, d_plot, 0);
				v2->copy((const VectorCurve *)cv);
				curveID = d_plot->insertCurve(v2);
			}
			else if (style == Box)
			{
				BoxCurve *b2 = new BoxCurve(d_plot, 0);
				b2->copy((const BoxCurve *)cv);
				curveID = d_plot->insertCurve(b2);
				QwtSingleArrayData dat(x[0], y, n);
				b2->setData(dat);
			}
			else
				curveID = d_plot->insertCurve(cv->title());

			c_keys.resize(++n_curves);
			c_keys[i] = curveID;

			c_type.resize(n_curves);
			c_type[i] = g->curveType(i);

			if (c_type[i] != Box)
				d_plot->setCurveData(curveID, x, y, n);

			delete[] x; 
			delete[] y;
			d_plot->setCurvePen(curveID, cv->pen()); 
			d_plot->setCurveBrush(curveID, cv->brush());
			d_plot->setCurveStyle(curveID, cv->style());
			d_plot->setCurveSymbol(curveID, cv->symbol());
		}
	}
	axesFormulas = g->axesFormulas;
	axisType = g->axisType;
	axesFormatInfo = g->axesFormatInfo;
	axisType = g->axisType;

	setAxisLabelRotation(QwtPlot::xBottom, g->labelsRotation(QwtPlot::xBottom));
	setAxisLabelRotation(QwtPlot::xTop, g->labelsRotation(QwtPlot::xTop));

	tickLabelsOn = g->tickLabelsOn;
	for (i=0; i<4; i++)
	{
		if (tickLabelsOn[i] == "1")
		{
			if (axisType[i] == Graph::Numeric)
				setLabelsNumericFormat(i, g->labelsNumericFormat());
			else if (axisType[i] == Graph::Time || axisType[i] == Graph::Date)
				setLabelsDateTimeFormat(i, axisType[i], axesFormatInfo[i]);
			else
			{
				QwtTextScaleDraw *sd = (QwtTextScaleDraw *)plot->axisScaleDraw (i);
				d_plot->setAxisScaleDraw(i, new QwtTextScaleDraw(sd->labelsList(), axesLineWidth));
			}
		}
		else
			d_plot->setAxisScaleDraw (i, new QwtNoLabelsScaleDraw(axesLineWidth));
	}
	setScales(g->scales);

	QwtArray<long> imag = g->imageMarkerKeys();
	for (i=0;i<(int)imag.size();i++)
	{
		ImageMarker* imrk=(ImageMarker*)g->imageMarker(imag[i]);
		if (imrk)
			insertImageMarker(imrk);
	}

	QwtArray<long> txtMrkKeys=g->textMarkerKeys();
	for (i=0;i<(int)txtMrkKeys.size();i++)
	{
		LegendMarker* mrk=(LegendMarker*)g->textMarker(txtMrkKeys[i]);
		if (mrk)
			insertTextMarker(mrk);
	}
	legendMarkerID = g->legendMarkerID;

	QwtArray<long> l = g->lineMarkerKeys();
	for (i=0;i<(int)l.size();i++)
	{
		LineMarker* lmrk=(LineMarker*)g->lineMarker(l[i]);
		if (lmrk)
			insertLineMarker(lmrk);
	}
	d_plot->replot();	
#endif
}

void Graph::plotBoxDiagram(Table *w, const QStringList& names)
{
#if false
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
			double *y = new double[size];
			int it=0;
			for (i = 0; i<w->tableRows(); i++ )
			{
				QString s = w->text(i,ycol);
				if (!s.isEmpty())
					y[it++] = s.toDouble();
			}
			gsl_sort (y, 1, size);

			QwtSingleArrayData data(double(j+1), y, size);
			BoxCurve *c = new BoxCurve(d_plot,0);
			c->setData(data);
			delete[] y;

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
	d_plot->setAxisScaleDraw (QwtPlot::xBottom, new QwtTextScaleDraw(w->selectedYLabels(), axesLineWidth));
	d_plot->setAxisMaxMajor(QwtPlot::xBottom, names.count()+1);
	d_plot->setAxisMaxMinor(QwtPlot::xBottom, 0);

	axisType[QwtPlot::xTop] = ColHeader;
	d_plot->setAxisScaleDraw (QwtPlot::xTop, new QwtTextScaleDraw(w->selectedYLabels(), axesLineWidth));
	d_plot->setAxisMaxMajor(QwtPlot::xTop, names.count()+1);
	d_plot->setAxisMaxMinor(QwtPlot::xTop, 0);

	axesFormatInfo[QwtPlot::xBottom] = w->name();
	axesFormatInfo[QwtPlot::xTop] = w->name();
#endif
}

void Graph::updateBoxData(Table* w, const QString& yColName, int curve)
{
#if false
	int ycol=w->colIndex(yColName);
	int i, size=0;
	for (i = 0; i<w->tableRows(); i++)
	{
		if (!w->text(i,ycol).isEmpty())
			size++;
	}

	if (size>0)
	{
		double *y = new double[size];
		int it=0;
		for (i = 0; i<w->tableRows(); i++)
		{
			QString s = w->text(i,ycol);
			if (!s.isEmpty())
				y[it++] = s.toDouble();
		}
		gsl_sort (y, 1, size);
		BoxCurve *c = (BoxCurve *)this->curve(curve);
		if (c)
		{
			QwtSingleArrayData dat(c->x(0), y, size);
			c->setData(dat);
		}
		delete[] y;
	}
	else
		removeCurve(curve);		
	d_plot->replot();	
#endif
}

void Graph::setCurveStyle(int index, int s)
{
#if false
	QwtPlotCurve *c = curve(index);
	if (!c)
		return;
	c->setStyle(s,0); 
#endif
}

void Graph::setCurveSymbol(int index, const QwtSymbol& s)
{
#if false
	QwtPlotCurve *c = curve(index);
	if (!c || c->symbol() == s)
		return;

	c->setSymbol(s);

	QString yColName=c->title();
	for (int i=0;i<n_curves;i++)
	{
		if (associations[i].contains(yColName) && i!=index && c_type[i] == ErrorBars)
		{
			QwtErrorPlotCurve *er=(QwtErrorPlotCurve *)this->curve(i);
			if (er)
				er->setSymbolSize(s.size());		
		}
	}
#endif
}

void Graph::setCurvePen(int index, const QPen& p)
{
#if false
	QwtPlotCurve *c = curve(index);
	if (!c || c->pen() == p)
		return;

	c->setPen(p);
#endif
}

void Graph::setCurveBrush(int index, const QBrush& b)
{
#if false
	QwtPlotCurve *c = curve(index);
	if (!c || c->brush() == b)
		return;

	c->setBrush(b);
#endif
}

void Graph::openBoxDiagram(Table *w, const QStringList& l)
{
#if false
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

	double *y = new double[size];
	int it=0;
	for (i = 0; i<w->tableRows(); i++ )
	{
		QString s = w->text(i,ycol);
		if (!s.isEmpty())
			y[it++] = s.toDouble();
	}

	gsl_sort (y, 1, size);//the data must be sorted first!

	BoxCurve *c = new BoxCurve(d_plot,0);
	QwtSingleArrayData dat(l[1].toDouble(), y, size);
	c->setData(dat);
	delete[] y;

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
#endif
}

QString Graph::curveXColName(const QString& curveTitle)
{
#if false
	QStringList cl = curvesList();
	int index=cl.findIndex(curveTitle);
	if (index < 0)
		return QString();

	cl=QStringList::split(",", associations[index],false);
	return cl[0].remove("(X)",true);
#endif
}

Graph::~Graph()
{
#if false
	delete d_zoomer;
	delete titlePicker;
	delete scalePicker;	
	delete cp;
	delete d_plot;
#endif
}

