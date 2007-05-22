/***************************************************************************
    File                 : Graph.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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

#include "Graph.h"
#include "CanvasPicker.h"
#include "QwtErrorPlotCurve.h"
#include "LegendMarker.h"
#include "LineMarker.h"
#include "cursors.h"
#include "ScalePicker.h"
#include "TitlePicker.h"
#include "QwtPieCurve.h"
#include "ImageMarker.h"
#include "QwtBarCurve.h"
#include "BoxCurve.h"
#include "QwtHistogram.h"
#include "VectorCurve.h"
#include "ScaleDraw.h"
#include "ColorBox.h"
#include "PatternBox.h"
#include "SymbolBox.h"
#include "FunctionCurve.h"
#include "Spectrogram.h"
#include "SelectionMoveResizer.h"
#include "RangeSelectorTool.h"
#include "PlotCurve.h"
#include "ApplicationWindow.h"

#include <QApplication>
#include <QBitmap>
#include <QClipboard>
#include <QCursor>
#include <QImage>
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <QMenu>
#include <QTextStream>
#include <QLocale>
#include <QPrintDialog>
#include <QImageWriter>
#include <QFileInfo>

#if QT_VERSION >= 0x040300
	#include <QSvgGenerator>
#endif

#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_text.h>
#include <qwt_text_label.h>
#include <qwt_color_map.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

Graph::Graph(QWidget* parent, const char* name, Qt::WFlags f)
: QWidget(parent,name,f)
{
	if ( !name )
		setName( "graph" );

	n_curves=0;
	d_active_tool = NULL;
	widthLine=1;mrkX=-1;mrkY=-1;;
	selectedMarker=-1;
	drawTextOn=false;
	drawLineOn=false;
	drawArrowOn=false;
	ignoreResize = true;
	drawAxesBackbone = true;
	autoscale = true;
	autoScaleFonts = false;
	d_antialiasing = true;
	d_scale_on_print = true;
	d_print_cropmarks = false;

	defaultArrowLineWidth = 1;
	defaultArrowColor = QColor(Qt::black);
	defaultArrowLineStyle = Qt::SolidLine;
	defaultArrowHeadLength = 4;
	defaultArrowHeadAngle = 45;
	defaultArrowHeadFill = true;

	defaultMarkerFont = QFont();
	defaultMarkerFrame = 1;
	defaultTextMarkerColor = QColor(Qt::black);
	defaultTextMarkerBackground = QColor(Qt::white);

	d_user_step = QVector<double>(QwtPlot::axisCnt);
	for (int i=0; i<QwtPlot::axisCnt; i++)
	{
		axisType << Numeric;
		axesFormatInfo << QString::null;
		axesFormulas << QString::null;
		d_user_step[i] = 0.0;
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
	zoom(false);

	setGeometry(0, 0, 500, 400);
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
	grid.minorCol=18;
	grid.minorStyle=2;
	grid.minorWidth=1;
	grid.xZeroOn=0;
	grid.yZeroOn=0;
	setGridOptions(grid);
	grid.xAxis = QwtPlot::xBottom;
	grid.yAxis = QwtPlot::yLeft;

	legendMarkerID = -1; // no legend for an empty graph
	d_texts = QVector<int>();
	c_type = QVector<int>();
	c_keys = QVector<int>();

	connect (cp,SIGNAL(selectPlot()),this,SLOT(activateGraph()));
	connect (cp,SIGNAL(drawTextOff()),this,SIGNAL(drawTextOff()));
	connect (cp,SIGNAL(viewImageDialog()),this,SIGNAL(viewImageDialog()));
	connect (cp,SIGNAL(viewTextDialog()),this,SIGNAL(viewTextDialog()));
	connect (cp,SIGNAL(viewLineDialog()),this,SIGNAL(viewLineDialog()));
	connect (cp,SIGNAL(showPlotDialog(int)),this,SIGNAL(showPlotDialog(int)));
	connect (cp,SIGNAL(showMarkerPopupMenu()),this,SIGNAL(showMarkerPopupMenu()));
	connect (cp,SIGNAL(modified()), this, SIGNAL(modifiedGraph()));

	connect (titlePicker,SIGNAL(showTitleMenu()),this,SLOT(showTitleContextMenu()));
	connect (titlePicker,SIGNAL(doubleClicked()),this,SIGNAL(viewTitleDialog()));
	connect (titlePicker,SIGNAL(removeTitle()),this,SLOT(removeTitle()));
	connect (titlePicker,SIGNAL(clicked()), this,SLOT(selectTitle()));

	connect (scalePicker,SIGNAL(clicked()),this,SLOT(activateGraph()));
	connect (scalePicker,SIGNAL(clicked()),this,SLOT(deselectMarker()));
	connect (scalePicker,SIGNAL(axisDblClicked(int)),this,SIGNAL(axisDblClicked(int)));
	connect (scalePicker,SIGNAL(axisTitleRightClicked(int)),this,SLOT(showAxisTitleMenu(int)));
	connect (scalePicker,SIGNAL(axisRightClicked(int)),this,SLOT(showAxisContextMenu(int)));
	connect (scalePicker,SIGNAL(xAxisTitleDblClicked()),this,SIGNAL(xAxisTitleDblClicked()));
	connect (scalePicker,SIGNAL(yAxisTitleDblClicked()),this,SIGNAL(yAxisTitleDblClicked()));
	connect (scalePicker,SIGNAL(rightAxisTitleDblClicked()),this,SIGNAL(rightAxisTitleDblClicked()));
	connect (scalePicker,SIGNAL(topAxisTitleDblClicked()),this,SIGNAL(topAxisTitleDblClicked()));

	connect (d_zoomer[0],SIGNAL(zoomed (const QwtDoubleRect &)),this,SLOT(zoomed (const QwtDoubleRect &)));
}

void Graph::notifyChanges()
{
	emit modifiedGraph();
}

void Graph::activateGraph()
{
	emit selectedGraph(this);
	setFocus();
}

void Graph::deselectMarker()
{
	selectedMarker = -1;
	if (d_markers_selector)
		delete d_markers_selector;
}

long Graph::selectedMarkerKey()
{
	return selectedMarker;
}

QwtPlotMarker* Graph::selectedMarkerPtr()
{
	return d_plot->marker(selectedMarker);
}

void Graph::setSelectedMarker(long mrk, bool add)
{
	selectedMarker = mrk;
	if (add) {
		if (d_markers_selector) {
			if (d_texts.contains(mrk))
				d_markers_selector->add((LegendMarker*)d_plot->marker(mrk));
			else if (d_lines.contains(mrk))
				d_markers_selector->add((LineMarker*)d_plot->marker(mrk));
			else if (d_images.contains(mrk))
				d_markers_selector->add((ImageMarker*)d_plot->marker(mrk));
		} else {
			if (d_texts.contains(mrk))
				d_markers_selector = new SelectionMoveResizer((LegendMarker*)d_plot->marker(mrk));
			else if (d_lines.contains(mrk))
				d_markers_selector = new SelectionMoveResizer((LineMarker*)d_plot->marker(mrk));
			else if (d_images.contains(mrk))
				d_markers_selector = new SelectionMoveResizer((ImageMarker*)d_plot->marker(mrk));
			else
				return;
			connect(d_markers_selector, SIGNAL(targetsChanged()), this, SIGNAL(modifiedGraph()));
		}
	} else {
		if (d_texts.contains(mrk)) {
			if (d_markers_selector) {
				if (d_markers_selector->contains((LegendMarker*)d_plot->marker(mrk)))
					return;
				delete d_markers_selector;
			}
			d_markers_selector = new SelectionMoveResizer((LegendMarker*)d_plot->marker(mrk));
		} else if (d_lines.contains(mrk)) {
			if (d_markers_selector) {
				if (d_markers_selector->contains((LineMarker*)d_plot->marker(mrk)))
					return;
				delete d_markers_selector;
			}
			d_markers_selector = new SelectionMoveResizer((LineMarker*)d_plot->marker(mrk));
		} else if (d_images.contains(mrk)) {
			if (d_markers_selector) {
				if (d_markers_selector->contains((ImageMarker*)d_plot->marker(mrk)))
					return;
				delete d_markers_selector;
			}
			d_markers_selector = new SelectionMoveResizer((ImageMarker*)d_plot->marker(mrk));
		} else
			return;
		connect(d_markers_selector, SIGNAL(targetsChanged()), this, SIGNAL(modifiedGraph()));
	}
}

void Graph::replot()
{
	d_plot->replot();
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

void Graph::enableAxes(QVector<bool> axesOn)
{
	for (int i = 0; i<QwtPlot::axisCnt; i++)
		d_plot->enableAxis(i, axesOn[i]);

	for (int i = 0;i<QwtPlot::axisCnt;i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
			scale->setMargin(0);
	}
	scalePicker->refresh();
}

QVector<bool> Graph::enabledAxes()
{
	QVector<bool> axesOn(4);
	for (int i = 0; i<QwtPlot::axisCnt; i++)
		axesOn[i]=d_plot->axisEnabled (i);
	return axesOn;
}

QList<int> Graph::axesBaseline()
{
	QList<int> baselineDist;
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

void Graph::setAxesBaseline(const QList<int> &lst)
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

QList<int> Graph::axesType()
{
	return axisType;
}

void Graph::setLabelsNumericFormat(int axis, int format, int prec, const QString& formula)
{
	axisType[axis] = Numeric;
	axesFormulas[axis] = formula;

	ScaleDraw *sd_old = (ScaleDraw *)d_plot->axisScaleDraw (axis);
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
		ScaleDraw *sd = new ScaleDraw(formula.ascii());
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

void Graph::setAxesType(const QList<int> tl)
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
	QList<int> ticksTypeList=d_plot->getMajorTicksType();
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

void Graph::setMajorTicksType(const QList<int>& lst)
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

void Graph::setMinorTicksType(const QList<int>& lst)
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

	ScaleDraw *sd = (ScaleDraw *)d_plot->axisScaleDraw (axis);
	sd->setMajorTicksStyle((ScaleDraw::TicksStyle)majTicksType);
	sd->setMinorTicksStyle((ScaleDraw::TicksStyle)minTicksType);

	if (majTicksType == ScaleDraw::None && minTicksType == ScaleDraw::None)
		sd->enableComponent (QwtAbstractScaleDraw::Ticks, false);
	else
		sd->enableComponent (QwtAbstractScaleDraw::Ticks);

	if (majTicksType == ScaleDraw::None || majTicksType == ScaleDraw::In)
		majLength = minLength;
	if (minTicksType == ScaleDraw::None || minTicksType == ScaleDraw::In)
		minLength = 0;

	sd->setTickLength (QwtScaleDiv::MinorTick, minLength);
	sd->setTickLength (QwtScaleDiv::MediumTick, minLength);
	sd->setTickLength (QwtScaleDiv::MajorTick, majLength);
}

void Graph::setTicksLength(int minLength, int majLength)
{
	QList<int> majTicksType = d_plot->getMajorTicksType();
	QList<int> minTicksType = d_plot->getMinorTicksType();

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
		const QString& formula, const QColor& labelsColor)
{
	d_plot->enableAxis(axis, axisOn);
	if (!axisOn)
		return;

	QList<int> majTicksTypeList = d_plot->getMajorTicksType();
	QList<int> minTicksTypeList = d_plot->getMinorTicksType();

	QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(axis);
	ScaleDraw *sclDraw = (ScaleDraw *)d_plot->axisScaleDraw (axis);

	if (d_plot->axisEnabled (axis) == axisOn &&
			majTicksTypeList[axis] == majTicksType &&
			minTicksTypeList[axis] == minTicksType &&
			axesColors()[axis] == c.name() &&
            axesNumColors()[axis] == labelsColor.name() &&
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
		pal.setColor(QColorGroup::Foreground, c);
    if (pal.color(QPalette::Active, QColorGroup::Text) != labelsColor)
		pal.setColor(QColorGroup::Text, labelsColor);
    scale->setPalette(pal);

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
	sclDraw->enableComponent(QwtAbstractScaleDraw::Backbone, drawAxesBackbone);

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

	ScaleDraw *sd_old = (ScaleDraw *)d_plot->axisScaleDraw (axis);
	const QwtScaleDiv div = sd_old->scaleDiv ();

	WeekDayScaleDraw *sd = new WeekDayScaleDraw((WeekDayScaleDraw::NameFormat)format);
	sd->setScaleDiv(div);
	d_plot->setAxisScaleDraw (axis, sd);
}

void Graph::setLabelsMonthFormat(int axis, int format)
{
	axisType[axis] = Month;
	axesFormatInfo[axis] = QString::number(format);

	ScaleDraw *sd_old = (ScaleDraw *)d_plot->axisScaleDraw (axis);
	const QwtScaleDiv div = sd_old->scaleDiv ();

	MonthScaleDraw *sd = new MonthScaleDraw((MonthScaleDraw::NameFormat)format);
	sd->setScaleDiv(div);
	d_plot->setAxisScaleDraw (axis, sd);
}

void Graph::setLabelsTextFormat(int axis, int type, const QString& name, const QStringList& lst)
{
	if (type != Txt && type != ColHeader)
		return;

    axesFormatInfo[axis] = name;
	d_plot->setAxisScaleDraw (axis, new QwtTextScaleDraw(lst));
	axisType[axis] = type;
}

void Graph::setLabelsTextFormat(int axis, int type, const QString& labelsColName, Table *table)
{
	if (type != Txt && type != ColHeader)
		return;

	QStringList list;
	if (type == Txt)
	{
		if (!table)
			return;

		axesFormatInfo[axis] = labelsColName;
		int r = table->numRows();
		int col = table->colIndex(labelsColName);

		for (int i=0; i < r; i++)
			list<<table->text(i, col);
	}
	else if (type == ColHeader)
	{
		if (!table)
			return;

		axesFormatInfo[axis] = table->name();
		for (int i=0; i<table->numCols(); i++)
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

	QStringList list = formatInfo.split(";", QString::KeepEmptyParts);
	if ((int)list.count() < 2)
	{
        QMessageBox::critical(this, tr("QtiPlot - Error"), "Couldn't change the axis type to the requested format!");
        return;
    }
    if (list[0].isEmpty() || list[1].isEmpty())
    {
        QMessageBox::critical(this, tr("QtiPlot - Error"), "Couldn't change the axis type to the requested format!");
        return;
    }

	if (type == Time)
	{
		TimeScaleDraw *sd = new TimeScaleDraw (QTime::fromString (list[0]), list[1]);
		sd->enableComponent (QwtAbstractScaleDraw::Backbone, drawAxesBackbone);
		d_plot->setAxisScaleDraw (axis, sd);
	}
	else if (type == Date)
	{
		DateScaleDraw *sd = new DateScaleDraw (QDate::fromString (list[0], Qt::ISODate), list[1]);
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
	d_plot->setAxisLabelRotation (axis, (double)rotation);
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

void Graph::setAxesNumColors(const QStringList& colors)
{
  	for (int i=0;i<4;i++)
  	{
  	     QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
  	     if (scale)
  	     {
  	         QPalette pal = scale->palette();
  	         pal.setColor(QColorGroup::Text, QColor(colors[i]));
  	         scale->setPalette(pal);
  	     }
  	}
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
	QStringList colors, numColors;
	QPalette pal;
	int i;
	for (i=0;i<4;i++)
    {
		colors<<QColor(Qt::black).name();
        numColors<<QColor(Qt::black).name();
    }

	for (i=0;i<4;i++)
	{
		QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
		if (scale)
		{
			pal=scale->palette();
			colors[i]=pal.color(QPalette::Active, QColorGroup::Foreground).name();
            numColors[i]=pal.color(QPalette::Active, QColorGroup::Text).name();
		}
	}
	s+=colors.join ("\t")+"\n";
    s+="AxesNumberColors\t"+numColors.join ("\t")+"\n";
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

QColor Graph::axisColor(int axis)
{
    QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(axis);
    if (scale)
  	     return scale->palette().color(QPalette::Active, QColorGroup::Foreground);
  	else
  	     return QColor(Qt::black);
}

QColor Graph::axisNumbersColor(int axis)
{
    QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(axis);
 	if (scale)
  	     return scale->palette().color(QPalette::Active, QColorGroup::Text);
  	else
  	     return QColor(Qt::black);
}

QStringList Graph::axesNumColors()
{
  	QStringList colors;
  	QPalette pal;
  	int i;
  	for (i=0;i<4;i++)
  	     colors << QColor(Qt::black).name();

  	for (i=0;i<4;i++)
  	{
  	     QwtScaleWidget *scale = (QwtScaleWidget *)d_plot->axisWidget(i);
  	     if (scale)
  	     {
  	         pal=scale->palette();
  	         colors[i]=pal.color(QPalette::Active, QColorGroup::Text).name();
  	     }
  	}
  	return colors;
}

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
	d_plot->setAxisTitle(a, text);
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

	QColor minColor = ColorBox::color(grid.minorCol);
	QColor majColor = ColorBox::color(grid.majorCol);

	Qt::PenStyle majStyle = getPenStyle(grid.majorStyle);
	Qt::PenStyle minStyle = getPenStyle(grid.minorStyle);

	QPen majPen=QPen (majColor,grid.majorWidth,majStyle);
	d_plot->grid()->setMajPen (majPen);

	QPen minPen=QPen (minColor,grid.minorWidth,minStyle);
	d_plot->grid()->setMinPen(minPen);

	if (grid.majorOnX) d_plot->grid()->enableX (true);
	else if (grid.majorOnX==0) d_plot->grid()->enableX (false);

	if (grid.minorOnX) d_plot->grid()->enableXMin (true);
	else if (grid.minorOnX==0) d_plot->grid()->enableXMin (false);

	if (grid.majorOnY) d_plot->grid()->enableY (true);
	else if (grid.majorOnY==0) d_plot->grid()->enableY (false);

	if (grid.minorOnY) d_plot->grid()->enableYMin (true);
	else d_plot->grid()->enableYMin (false);

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
		QwtPlotItem *it = plotItem(i);
		if (!it)
			continue;

		if (it->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
  	         {
  	         Spectrogram *sp = (Spectrogram *)it;
  	         if (sp->colorScaleAxis() == axis)
  	              return;
  	         }

		if ((axis == QwtPlot::yRight && it->yAxis() == QwtPlot::yRight) ||
            (axis == QwtPlot::xTop && it->xAxis () == QwtPlot::xTop))
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

void Graph::setScale(int axis, double start, double end, double step, int majorTicks, int minorTicks, int type, bool inverted)
{
	QwtScaleEngine *sc_engine = 0;
	if (type)
		sc_engine = new QwtLog10ScaleEngine();
	else
		sc_engine = new QwtLinearScaleEngine();

	int max_min_intervals = minorTicks;
	if (minorTicks == 1)
		max_min_intervals = 3;
	if (minorTicks > 1)
		max_min_intervals = minorTicks + 1;

	QwtScaleDiv div = sc_engine->divideScale (QMIN(start, end), QMAX(start, end), majorTicks, max_min_intervals, step);
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

	d_user_step[axis] = step;

	if (axis == QwtPlot::xBottom || axis == QwtPlot::yLeft)
  	{
  		updateSecondaryAxis(QwtPlot::xTop);
  	    updateSecondaryAxis(QwtPlot::yRight);
  	}

	d_plot->replot();
	//keep markers on canvas area
	updateMarkersBoundingRect();
	d_plot->replot();
}

QStringList Graph::analysableCurvesList()
{
	QStringList cList;
	QList<int> keys = d_plot->curveKeys();
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotCurve *c = d_plot->curve(keys[i]);
  	    if (c && c_type[i] != ErrorBars)
  	        cList << c->title().text();
  	 }
return cList;
}

QStringList Graph::curvesList()
{
	QStringList cList;
	QList<int> keys = d_plot->curveKeys();
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotCurve *c = d_plot->curve(keys[i]);
  	    if (c)
  	        cList << c->title().text();
  	 }
return cList;
}

QStringList Graph::plotItemsList()
{
  	QStringList cList;
  	QList<int> keys = d_plot->curveKeys();
  	for (int i=0; i<(int)keys.count(); i++)
  	{
  		QwtPlotItem *it = d_plot->plotItem(keys[i]);
		if (it)
			cList << it->title().text();
	}
	return cList;
}

void Graph::copyImage()
{
	QApplication::clipboard()->setPixmap(graphPixmap(), QClipboard::Clipboard);
}

QPixmap Graph::graphPixmap()
{
	return QPixmap::grabWidget(this);
}

void Graph::exportToFile(const QString& fileName)
{
	if ( fileName.isEmpty() )
	{
		QMessageBox::critical(this, tr("QtiPlot - Error"), tr("Please provide a valid file name!"));
        return;
	}

	if (fileName.contains(".eps") || fileName.contains(".pdf") || fileName.contains(".ps"))
	{
		exportVector(fileName);
		return;
	}
	else
	{
		QList<QByteArray> list = QImageWriter::supportedImageFormats();
    	for(int i=0 ; i<list.count() ; i++)
		{
			if (fileName.contains( "." + list[i].toLower()))
			{
				exportImage(fileName);
				return;
			}
		}
    	QMessageBox::critical(this, tr("QtiPlot - Error"), tr("File format not handled, operation aborted!"));
	}
}

void Graph::exportImage(const QString& fileName, int quality, bool transparent)
{
	QPixmap pic = graphPixmap();

	if (transparent)
	{
		QBitmap mask(pic.size());
		mask.fill(Qt::color1);
		QPainter p;
		p.begin(&mask);
		p.setPen(Qt::color0);

        QColor background = QColor (Qt::white);
		QRgb backgroundPixel = background.rgb ();
		QImage image = pic.convertToImage();
		for (int y=0; y<image.height(); y++)
		{
			for ( int x=0; x<image.width(); x++ )
			{
				QRgb rgb = image.pixel(x, y);
				if (rgb == backgroundPixel) // we want the frame transparent
					p.drawPoint(x, y);
			}
		}
		p.end();
		pic.setMask(mask);
	}

	pic.save(fileName, 0, quality);
}

void Graph::exportVector(const QString& fileName, int res, bool color)
{
	if ( fileName.isEmpty() )
	{
		QMessageBox::critical(this, tr("QtiPlot - Error"), tr("Please provide a valid file name!"));
        return;
	}

	QPrinter printer;
    printer.setCreator("QtiPlot");
	printer.setFullPage(true);
	if (res)
		printer.setResolution(res);
	
    printer.setOutputFileName(fileName);
    if (fileName.contains(".eps"))
    	printer.setOutputFormat(QPrinter::PostScriptFormat);

    QRect plotRect = d_plot->rect();
    printer.setPageSize(minPageSize(printer, plotRect));

	// export should preserve plot aspect ratio, if possible
	double aspect = double(d_plot->frameGeometry().width())/double(d_plot->frameGeometry().height());
	if (aspect < 1)
		printer.setOrientation(QPrinter::Portrait);
	else
		printer.setOrientation(QPrinter::Landscape);

	if (color)
		printer.setColorMode(QPrinter::Color);
	else
		printer.setColorMode(QPrinter::GrayScale);

    QRect paperRect = printer.paperRect();
    int x_margin = (paperRect.width() - plotRect.width())/2;
    int y_margin = (paperRect.height() - plotRect.height())/2;
    plotRect.moveTo(x_margin, y_margin);

    QPainter paint(&printer);
	d_plot->print(&paint, plotRect);
}

void Graph::print()
{
	QPrinter printer;
	printer.setColorMode (QPrinter::Color);
	printer.setFullPage(true);

	//printing should preserve plot aspect ratio, if possible
	double aspect = double(d_plot->width())/double(d_plot->height());
	if (aspect < 1)
		printer.setOrientation(QPrinter::Portrait);
	else
		printer.setOrientation(QPrinter::Landscape);

	QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted)
	{
		QRect plotRect = d_plot->rect();
		QRect paperRect = printer.paperRect();
		if (d_scale_on_print)
		{
			int dpiy = printer.logicalDpiY();
			int margin = (int) ((2/2.54)*dpiy ); // 2 cm margins

			int width = qRound(aspect*printer.height()) - 2*margin;
			int x=qRound(abs(printer.width()- width)*0.5);

			plotRect = QRect(x, margin, width, printer.height() - 2*margin);
			if (x < margin)
			{
				plotRect.setLeft(margin);
				plotRect.setWidth(printer.width() - 2*margin);
			}
		}
		else
		{
    		int x_margin = (paperRect.width() - plotRect.width())/2;
    		int y_margin = (paperRect.height() - plotRect.height())/2;
    		plotRect.moveTo(x_margin, y_margin);
		}

        QPainter paint(&printer);
        if (d_print_cropmarks)
        {
			QRect cr = plotRect; // cropmarks rectangle
			cr.addCoords(-1, -1, 2, 2);
            paint.save();
            paint.setPen(QPen(QColor(Qt::black), 0.5, Qt::DashLine));
            paint.drawLine(paperRect.left(), cr.top(), paperRect.right(), cr.top());
            paint.drawLine(paperRect.left(), cr.bottom(), paperRect.right(), cr.bottom());
            paint.drawLine(cr.left(), paperRect.top(), cr.left(), paperRect.bottom());
            paint.drawLine(cr.right(), paperRect.top(), cr.right(), paperRect.bottom());
            paint.restore();
        }

		d_plot->print(&paint, plotRect);
	}
}

void Graph::exportSVG(const QString& fname)
{
	#if QT_VERSION >= 0x040300
		QSvgGenerator svg;
        svg.setFileName(fname);
        svg.setSize(d_plot->size());
	
		QPainter p(&svg);
		d_plot->print(&p, d_plot->rect());
		p.end();
	#endif
}

int Graph::selectedCurveID()
{
	if (d_range_selector)
		return curveKey(curveIndex(d_range_selector->selectedCurve()));
	else
		return -1;
}

QString Graph::selectedCurveTitle()
{
	if (d_range_selector)
		return d_range_selector->selectedCurve()->title().text();
	else
		return QString::null;
}

bool Graph::markerSelected()
{
	return (selectedMarker>=0);
}

void Graph::removeMarker()
{
	if (selectedMarker>=0)
	{
		if (d_markers_selector) {
			if (d_texts.contains(selectedMarker))
				d_markers_selector->removeAll((LegendMarker*)d_plot->marker(selectedMarker));
			else if (d_lines.contains(selectedMarker))
				d_markers_selector->removeAll((LineMarker*)d_plot->marker(selectedMarker));
			else if (d_images.contains(selectedMarker))
				d_markers_selector->removeAll((ImageMarker*)d_plot->marker(selectedMarker));
		}
		d_plot->removeMarker(selectedMarker);
		d_plot->replot();
		emit modifiedGraph();

		if (selectedMarker==legendMarkerID)
			legendMarkerID=-1;

		if (d_lines.contains(selectedMarker)>0)
		{
			int index = d_lines.indexOf(selectedMarker);
            int last_line_marker = (int)d_lines.size() - 1;
			for (int i=index; i < last_line_marker; i++)
				d_lines[i]=d_lines[i+1];
			d_lines.resize(last_line_marker);
		}
		else if(d_texts.contains(selectedMarker)>0)
		{
			int index=d_texts.indexOf(selectedMarker);
			int last_text_marker = d_texts.size() - 1;
			for (int i=index; i < last_text_marker; i++)
				d_texts[i]=d_texts[i+1];
			d_texts.resize(last_text_marker);
		}
		else if(d_images.contains(selectedMarker)>0)
		{
			int index=d_images.indexOf(selectedMarker);
			int last_image_marker = d_images.size() - 1;
			for (int i=index; i < last_image_marker; i++)
				d_images[i]=d_images[i+1];
			d_images.resize(last_image_marker);
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
	return (d_lines.contains(selectedMarker));
}

bool Graph::imageMarkerSelected()
{
	return (d_images.contains(selectedMarker));
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
		auxMrkStart=mrkI->origin();
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
        int linesOnPlot = (int)d_lines.size();
  	    d_lines.resize(++linesOnPlot);
  	    d_lines[linesOnPlot-1] = d_plot->insertMarker(mrkL);

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

		d_plot->replot();

		deselectMarker();
	}
	else if (selectedMarkerType==Image)
	{
		QString fn=auxMrkFileName;
		QRect rect=QRect(auxMrkStart,auxMrkEnd);

		QFileInfo fi(fn);
		QString baseName = fi.fileName();
		int pos=baseName.find(".",0);
		QString type=baseName.right(baseName.length()-pos-1);

        QPixmap photo;
		if (type.upper()=="JPG")
			photo.load(fn,"JPEG",QPixmap::Auto);
		else
			photo.load(fn,type.upper(),QPixmap::Auto);

		ImageMarker* mrk= new ImageMarker(photo);
		int imagesOnPlot=d_images.size();
  	    d_images.resize(++imagesOnPlot);
  	    d_images[imagesOnPlot-1] = d_plot->insertMarker(mrk);
		mrk->setFileName(fn);

		QPoint o=d_plot->canvas()->mapFromGlobal(QCursor::pos());
		if (!d_plot->canvas()->contentsRect().contains(o))
			o=QPoint(auxMrkStart.x()+20,auxMrkStart.y()+20);

		mrk->setOrigin(o);
		mrk->setSize(rect.size());
		d_plot->replot();

		deselectMarker();
	}
	else
	{
		LegendMarker* mrk=new LegendMarker(d_plot);
        int texts = d_texts.size();
  	    d_texts.resize(++texts);
  	    d_texts[texts-1] = d_plot->insertMarker(mrk);

		QPoint o=d_plot->canvas()->mapFromGlobal(QCursor::pos());
		if (!d_plot->canvas()->contentsRect().contains(o))
			o=QPoint(auxMrkStart.x()+20,auxMrkStart.y()+20);

		mrk->setOrigin(o);
		mrk->setAngle(auxMrkAngle);
		mrk->setFrameStyle(auxMrkBkg);
		mrk->setFont(auxMrkFont);
		mrk->setText(auxMrkText);
		mrk->setTextColor(auxMrkColor);
		mrk->setBackgroundColor(auxMrkBkgColor);

		d_plot->replot();
		deselectMarker();
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

	deselectMarker();
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
		d_plot->setTitle(" ");
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
	if (legendMarkerID >= 0)
	{
		int index = d_texts.indexOf(legendMarkerID);
		int texts = d_texts.size();
		for (int i=index; i<texts; i++)
			d_texts[i]=d_texts[i+1];
		d_texts.resize(--texts);

		d_plot->removeMarker(legendMarkerID);
		legendMarkerID=-1;
	}
}

void Graph::updateImageMarker(int x, int y, int w, int h)
{
	ImageMarker* mrk =(ImageMarker*) d_plot->marker(selectedMarker);
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
	mrkL->setFrameStyle(bkg);

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
		if (c && c->rtti() != QwtPlotItem::Rtti_PlotSpectrogram && c_type[i] != ErrorBars )
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
	QList<int> keys= d_plot->curveKeys();
	const QwtPlotCurve *curve = (QwtPlotCurve *)d_plot->curve(keys[0]);
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

void Graph::updateCurvesData(Table* w, const QString& yColName)
{
    QList<int> keys = d_plot->curveKeys();
    int updated_curves = 0;
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotItem *it = d_plot->plotItem(keys[i]);
		if (!it)
            continue;
        if (it->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
            continue;
        if (((PlotCurve *)it)->type() == Function)
            continue;

        if(((DataCurve *)it)->updateData(w, yColName))
            updated_curves++;
	}
    if (updated_curves)
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
	bool frameOn=false;

	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();
	if (canvas->lineWidth()>0)
		frameOn=true;

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
			ScaleDraw *sclDraw = (ScaleDraw *)d_plot->axisScaleDraw (i);
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
			ScaleDraw *sclDraw = (ScaleDraw *)d_plot->axisScaleDraw (i);
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
	s += "CanvasBackground\t" + d_plot->canvasBackground().name()+"\t";
	s += QString::number(d_plot->canvasBackground().alpha())+"\n";
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
		s += QString::number(d_user_step[i], 'g', 15)+"\t";
		s += QString::number(d_plot->axisMaxMajor(i))+"\t";
		s += QString::number(d_plot->axisMaxMinor(i))+"\t";

		const QwtScaleEngine *sc_eng = d_plot->axisScaleEngine(i);
		QwtScaleTransformation *tr = sc_eng->transformation();
		s += QString::number((int)tr->type())+"\t";
		s += QString::number(sc_eng->testAttribute(QwtScaleEngine::Inverted))+"\n";
	}
	return s;
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
		{
  	        QwtText title = scale->title();
  	        title.setColor(QColor(l[i+1]));
  	        scale->setTitle(title);
  	   }
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
	for (int i=0; i<4; i++)
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
		QString title = d_plot->axisTitle(a).text();
		if (!title.isEmpty())
            s += title.replace("\n", "<br>")+"\t";
        else
            s += "\t";
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
	s+=QString::number(pieCurve->ray())+"\t";
	s+=QString::number(pieCurve->firstColor())+"\t";
	s+=QString::number(pieCurve->startRow())+"\t"+QString::number(pieCurve->endRow())+"\t";
	s+=QString::number(pieCurve->isVisible())+"\n";
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

		QStringList colsList = v->plotAssociation().split(",", QString::SkipEmptyParts);
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

	return s;
}

QString Graph::saveCurves()
{
	QString s;
	if (isPiePlot())
		s+=savePieCurveLayout();
	else
	{
		for (int i=0; i<n_curves; i++)
		{
			QwtPlotItem *it = plotItem(i);
			if (!it)
  	        	continue;

            if (it->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
            {
                s += ((Spectrogram *)it)->saveToString();
                continue;
            }

            DataCurve *c = (DataCurve *)it;
			if (c->type() != ErrorBars)
			{
				if (c->type() == Function)
					s += ((FunctionCurve *)c)->saveToString();
				else if (c->type() == Box)
					s += "curve\t" + QString::number(c->x(0)) + "\t" + c->title().text() + "\t";
				else
					s += "curve\t" + c->xColumnName() + "\t" + c->title().text() + "\t";

				s += saveCurveLayout(i);
				s += QString::number(c->xAxis())+"\t"+QString::number(c->yAxis())+"\t";
				s += QString::number(c->startRow())+"\t"+QString::number(c->endRow())+"\t";
				s += QString::number(c->isVisible())+"\n";
			}
		    else if (c->type() == ErrorBars)
  	        {
  	        	QwtErrorPlotCurve *er = (QwtErrorPlotCurve *)it;
  	            s += "ErrorBars\t";
  	            s += QString::number(er->direction())+"\t";
  	            s += er->masterCurve()->xColumnName() + "\t";
  	            s += er->masterCurve()->title().text() + "\t";
  	            s += er->title().text() + "\t";
  	            s += QString::number(er->width())+"\t";
  	            s += QString::number(er->capLength())+"\t";
  	            s += er->color().name()+"\t";
  	            s += QString::number(er->throughSymbol())+"\t";
  	            s += QString::number(er->plusSide())+"\t";
  	            s += QString::number(er->minusSide())+"\n";
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

	if (isPiePlot())
		mrk->setText(pieLegendText());
	else
		mrk->setText(legendText());

	mrk->setFrameStyle(defaultMarkerFrame);
	mrk->setFont(defaultMarkerFont);
	mrk->setTextColor(defaultTextMarkerColor);
	mrk->setBackgroundColor(defaultTextMarkerBackground);

	legendMarkerID = d_plot->insertMarker(mrk);
	int texts = d_texts.size();
	d_texts.resize(++texts);
	d_texts[texts-1] = legendMarkerID;

	emit modifiedGraph();
	d_plot->replot();
}

void Graph::addTimeStamp()
{
	LegendMarker* mrk= newLegend(QDateTime::currentDateTime().toString(Qt::LocalDate));
	mrk->setOrigin(QPoint(d_plot->canvas()->width()/2, 10));
	emit modifiedGraph();
	d_plot->replot();
}

LegendMarker* Graph::newLegend(const QString& text)
{
	LegendMarker* mrk = new LegendMarker(d_plot);
	selectedMarker = d_plot->insertMarker(mrk);
	if(d_markers_selector)
		delete d_markers_selector;

	int texts = d_texts.size();
	d_texts.resize(++texts);
	d_texts[texts-1] = selectedMarker;

	mrk->setOrigin(QPoint(5,5));
	mrk->setText(text);
	mrk->setFrameStyle(defaultMarkerFrame);
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

	int texts = d_texts.size();
	d_texts.resize(++texts);
	d_texts[texts-1] = key;

	if (fileVersion < 86)
		mrk->setOrigin(QPoint(fList[1].toInt(),fList[2].toInt()));
	else
		mrk->setOriginCoord(fList[1].toDouble(), fList[2].toDouble());

	QFont fnt=QFont (fList[3],fList[4].toInt(),fList[5].toInt(),fList[6].toInt());
	fnt.setUnderline(fList[7].toInt());
	fnt.setStrikeOut(fList[8].toInt());
	mrk->setFont(fnt);

	mrk->setAngle(fList[11].toInt());

    QString text = QString();
	if (fileVersion < 71)
	{
		int bkg=fList[10].toInt();
		if (bkg <= 2)
			mrk->setFrameStyle(bkg);
		else if (bkg == 3)
		{
			mrk->setFrameStyle(0);
			mrk->setBackgroundColor(QColor(255, 255, 255));
		}
		else if (bkg == 4)
		{
			mrk->setFrameStyle(0);
			mrk->setBackgroundColor(QColor(Qt::black));
		}

		int n =(int)fList.count();
		for (int i=0;i<n-12;i++)
			text += fList[12+i]+"\n";
	}
	else if (fileVersion < 90)
	{
		mrk->setTextColor(QColor(fList[9]));
		mrk->setFrameStyle(fList[10].toInt());
		mrk->setBackgroundColor(QColor(fList[12]));

		int n=(int)fList.count();
		for (int i=0;i<n-13;i++)
			text += fList[13+i]+"\n";
	}
	else
	{
		mrk->setTextColor(QColor(fList[9]));
		mrk->setFrameStyle(fList[10].toInt());
		QColor c = QColor(fList[12]);
		c.setAlpha(fList[13].toInt());
		mrk->setBackgroundColor(c);

		int n = (int)fList.count();
		for (int i=0; i<n-14; i++)
			text += fList[14+i]+"\n";
	}
    mrk->setText(text.trimmed());
	return key;
}

void Graph::insertLineMarker(QStringList list, int fileVersion)
{
	LineMarker* mrk= new LineMarker();
	long mrkID=d_plot->insertMarker(mrk);
    int linesOnPlot = (int)d_lines.size();
	d_lines.resize(++linesOnPlot);
	d_lines[linesOnPlot-1]=mrkID;

	if (fileVersion < 86)
	{
		mrk->setStartPoint(QPoint(list[1].toInt(), list[2].toInt()));
		mrk->setEndPoint(QPoint(list[3].toInt(), list[4].toInt()));
	}
	else
		mrk->setBoundingRect(list[1].toDouble(), list[2].toDouble(),
							list[3].toDouble(), list[4].toDouble());

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
    int linesOnPlot = (int)d_lines.size();
	d_lines.resize(++linesOnPlot);
	d_lines[linesOnPlot-1] = d_plot->insertMarker(aux);

	aux->setBoundingRect(mrk->startPointCoord().x(), mrk->startPointCoord().y(),
						 mrk->endPointCoord().x(), mrk->endPointCoord().y());
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
	return (LegendMarker*)d_plot->marker(id);
}

long Graph::insertTextMarker(LegendMarker* mrk)
{
	LegendMarker* aux = new LegendMarker(d_plot);
	selectedMarker = d_plot->insertMarker(aux);
	if(d_markers_selector)
		delete d_markers_selector;

	int texts = d_texts.size();
	d_texts.resize(++texts);
	d_texts[texts-1] = selectedMarker;

	aux->setTextColor(mrk->textColor());
	aux->setBackgroundColor(mrk->backgroundColor());
	aux->setOriginCoord(mrk->xValue(), mrk->yValue());
	aux->setFont(mrk->font());
	aux->setFrameStyle(mrk->frameStyle());
	aux->setAngle(mrk->angle());
	aux->setText(mrk->text());
	return selectedMarker;
}

QString Graph::saveMarkers()
{
	QString s;
	int t = d_texts.size(), l = d_lines.size(), im = d_images.size();
	for (int i=0; i<im; i++)
	{
		ImageMarker* mrkI=(ImageMarker*) d_plot->marker(d_images[i]);
		s += "<image>\t";
		s += mrkI->getFileName()+"\t";
		s += QString::number(mrkI->xValue(), 'g', 15)+"\t";
		s += QString::number(mrkI->yValue(), 'g', 15)+"\t";
		s += QString::number(mrkI->right(), 'g', 15)+"\t";
		s += QString::number(mrkI->bottom(), 'g', 15)+"</image>\n";
	}

	for (int i=0; i<l; i++)
	{
		LineMarker* mrkL=(LineMarker*) d_plot->marker(d_lines[i]);
		s+="<line>\t";

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
		s+=QString::number(mrkL->filledArrowHead())+"</line>\n";
	}

	for (int i=0; i<t; i++)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(d_texts[i]);
		if (d_texts[i] != legendMarkerID)
			s+="<text>\t";
		else
			s+="<legend>\t";

		s+=QString::number(mrk->xValue(), 'g', 15)+"\t";
		s+=QString::number(mrk->yValue(), 'g', 15)+"\t";

		QFont f=mrk->font();
		s+=f.family()+"\t";
		s+=QString::number(f.pointSize())+"\t";
		s+=QString::number(f.weight())+"\t";
		s+=QString::number(f.italic())+"\t";
		s+=QString::number(f.underline())+"\t";
		s+=QString::number(f.strikeOut())+"\t";
		s+=mrk->textColor().name()+"\t";
		s+=QString::number(mrk->frameStyle())+"\t";
		s+=QString::number(mrk->angle())+"\t";
		s+=mrk->backgroundColor().name()+"\t";
		s+=QString::number(mrk->backgroundColor().alpha())+"\t";

		QStringList textList=mrk->text().split("\n", QString::KeepEmptyParts);
		s+=textList.join ("\t");
		if (d_texts[i]!=legendMarkerID)
  	        s+="</text>\n";
  	    else
  	        s+="</legend>\n";
	}
	return s;
}

double Graph::selectedXStartValue()
{
	if (d_range_selector)
		return d_range_selector->minXValue();
	else
		return 0;
}

double Graph::selectedXEndValue()
{
	if (d_range_selector)
		return d_range_selector->maxXValue();
	else
		return 0;
}

QwtPlotItem* Graph::plotItem(int index)
{
    if (!n_curves || index >= n_curves || index < 0)
		return 0;

	return d_plot->plotItem(c_keys[index]);
}

int Graph::plotItemIndex(QwtPlotItem *it) const
{
	for (int i = 0; i < n_curves; i++)
	{
		if (d_plot->plotItem(c_keys[i]) == it)
			return i;
	}
	return -1;
}

QwtPlotCurve *Graph::curve(int index)
{
	if (!n_curves || index >= n_curves || index < 0)
		return 0;

	return d_plot->curve(c_keys[index]);
}

int Graph::curveIndex(QwtPlotCurve *c) const
{
	return plotItemIndex(c);
}

int Graph::range(int index, double *start, double *end)
{
	if (d_range_selector && d_range_selector->selectedCurve() == curve(index)) {
		*start = d_range_selector->minXValue();
		*end = d_range_selector->maxXValue();
		return d_range_selector->dataSize();
	} else {
		QwtPlotCurve *c = curve(index);
		if (!c)
			return 0;

		*start = c->x(0);
		*end = c->x(c->dataSize() - 1);
		return c->dataSize();
	}
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

CurveLayout Graph::initCurveLayout(int style, int curves)
{
    int i = n_curves - 1;

	CurveLayout cl = initCurveLayout();
	int color;
	guessUniqueCurveLayout(color, cl.sType);

  	cl.lCol = color;
  	cl.symCol = color;
  	cl.fillCol = color;

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
	else if (curves && (style == Graph::VerticalBars || style == Graph::HorizontalBars))
	{
		cl.filledArea=1;
		cl.lCol=0;//black color pen
		cl.aCol=i+1;
		cl.sType = 0;
		if (c_type[i] == Graph::VerticalBars || style == Graph::HorizontalBars)
		{
			QwtBarCurve *b = (QwtBarCurve*)curve(i);
			if (b)
			{
				b->setGap(qRound(100*(1-1.0/(double)curves)));
				b->setOffset(-50*(curves-1) + i*100);
			}
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
	else if (style== Graph::Area)
	{
		cl.filledArea=1;
		cl.aCol= color;
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
	DataCurve *c = (DataCurve *)this->curve(index);
	if (!c)
		return;
    if (c_type.isEmpty() || (int)c_type.size() < index)
        return;

	QPen pen = QPen(ColorBox::color(cL->symCol),cL->penWidth, Qt::SolidLine);
	if (cL->fillCol != -1)
		c->setSymbol(QwtSymbol(SymbolBox::style(cL->sType), QBrush(ColorBox::color(cL->fillCol)), pen, QSize(cL->sSize,cL->sSize)));
	else
		c->setSymbol(QwtSymbol(SymbolBox::style(cL->sType), QBrush(), pen, QSize(cL->sSize,cL->sSize)));

	c->setPen(QPen(ColorBox::color(cL->lCol), cL->lWidth, getPenStyle(cL->lStyle)));

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

	QBrush brush = QBrush(ColorBox::color(cL->aCol));
	if (cL->filledArea)
		brush.setStyle(getBrushStyle(cL->aStyle));
	else
		brush.setStyle(Qt::NoBrush);
	c->setBrush(brush);
}

void Graph::updateErrorBars(QwtErrorPlotCurve *er, bool xErr, int width, int cap, const QColor& c,
		bool plus, bool minus, bool through)
{
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

	d_plot->replot();
	emit modifiedGraph();
}

bool Graph::addErrorBars(const QString& yColName, Table *errTable, const QString& errColName,
		int type, int width, int cap, const QColor& color, bool through, bool minus, bool plus)
{
	QList<int> keys = d_plot->curveKeys();
	for (int i = 0; i<n_curves; i++ )
	{
		DataCurve *c = (DataCurve *)d_plot->curve(keys[i]);
		if (c && c->title().text() == yColName && c_type[i] != ErrorBars)
		{
			return addErrorBars(c->xColumnName(), yColName, errTable, errColName,
					type, width, cap, color, through, minus, plus);
		}
	}
	return false;
}

bool Graph::addErrorBars(const QString& xColName, const QString& yColName,
		Table *errTable, const QString& errColName, int type, int width, int cap,
		const QColor& color, bool through, bool minus, bool plus)
{
	DataCurve *master_curve = masterCurve(xColName, yColName);
	if (!master_curve)
		return false;

	QwtErrorPlotCurve *er = new QwtErrorPlotCurve(type, errTable, errColName);
	er->setMasterCurve(master_curve);
	er->setCapLength(cap);
	er->setColor(color);
	er->setWidth(width);
	er->drawPlusSide(plus);
	er->drawMinusSide(minus);
	er->drawThroughSymbol(through);

	c_type.resize(++n_curves);
	c_type[n_curves-1] = ErrorBars;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = d_plot->insertCurve(er);

	updatePlot();
	return true;
}

void Graph::plotPie(Table* w, const QString& name, const QPen& pen, int brush,
					int size, int firstColor, int startRow, int endRow, bool visible)
{
	if (endRow < 0)
		endRow = w->numRows() - 1;

	QwtPieCurve *pieCurve = new QwtPieCurve(w, name, startRow, endRow);
	pieCurve->loadData();
	pieCurve->setPen(pen);
	pieCurve->setRay(size);
	pieCurve->setFirstColor(firstColor);
	pieCurve->setBrushStyle(getBrushStyle(brush));
	pieCurve->setVisible(visible);

	c_keys.resize(++n_curves);
	c_keys[n_curves-1] = d_plot->insertCurve(pieCurve);

	c_type.resize(n_curves);
	c_type[n_curves-1] = Pie;
}

void Graph::plotPie(Table* w, const QString& name, int startRow, int endRow)
{
	int ycol = w->colIndex(name);
	int size = 0;
	double sum = 0.0;

	if (endRow < 0)
		endRow = w->numRows() - 1;

	QVarLengthArray<double> Y(abs(endRow - startRow) + 1);
	for (int i = startRow; i<= endRow; i++)
	{
		QString yval = w->text(i,ycol);
		if (!yval.isEmpty())
		{
			Y[size]=yval.toDouble();
			sum += Y[size];
			size++;
		}
	}
	if (!size)
		return;
    Y.resize(size);

	QwtPlotLayout *pLayout = d_plot->plotLayout();
	pLayout->activate(d_plot, d_plot->rect(), 0);
	const QRect rect = pLayout->canvasRect();

	QwtPieCurve *pieCurve = new QwtPieCurve(w, name, startRow, endRow);
	pieCurve->setData(Y.data(), Y.data(), size);

	c_keys.resize(++n_curves);
	c_keys[n_curves-1] = d_plot->insertCurve(pieCurve);

	c_type.resize(n_curves);
	c_type[n_curves-1] = Pie;

	const int ray = 125;
	int xc = int(rect.width()/2 + 10);
	int yc = int(rect.y() + rect.height()/2 + pLayout->titleRect().height() + 5);

	double PI = 4*atan(1.0);
	double angle = 90;

	for (int i = 0; i<size; i++ )
	{
		const double value = Y[i]/sum*360;
		double alabel = (angle - value*0.5)*PI/180.0;

		const int x = int(xc + ray*cos(alabel));
		const int y = int(yc - ray*sin(alabel));

		LegendMarker* aux = new LegendMarker(d_plot);
		aux->setOrigin(QPoint(x,y));
		aux->setFrameStyle(0);
		aux->setText(QString::number(Y[i]/sum*100,'g',2)+"%");

		int texts = d_texts.size();
		d_texts.resize(++texts);
		d_texts[texts-1] = d_plot->insertMarker(aux);

		angle -= value;
	}

	if (legendMarkerID>=0)
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
		{
			QString text="";
			for (int i=0;i<size;i++)
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

	for (int i=0; i<QwtPlot::axisCnt; i++)
		d_plot->enableAxis(i, false);

	d_plot->setTitle(QString::null);

	QwtPlotCanvas* canvas=(QwtPlotCanvas*) d_plot->canvas();
	canvas->setLineWidth(1);

	scalePicker->refresh();
	d_plot->replot();
	updateScale();
}

void Graph::insertPlotItem(QwtPlotItem *i, int type)
{
	c_type.resize(++n_curves);
	c_type[n_curves-1] = type;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = d_plot->insertCurve(i);

	if (i->rtti() != QwtPlotItem::Rtti_PlotSpectrogram)
  		addLegendItem(i->title().text());
}

bool Graph::insertCurvesList(Table* w, const QStringList& names, int style, int lWidth,
							int sSize, int startRow, int endRow)
{
	if (style==Graph::Pie)
		plotPie(w, names[0], startRow, endRow);
	else if (style == Box)
		plotBoxDiagram(w, names, startRow, endRow);
	else if (style==Graph::VectXYXY || style==Graph::VectXYAM)
		plotVectorCurve(w, names, style, startRow, endRow);
	else
	{
		int curves = (int)names.count();
        int errCurves = 0;
		QStringList lst = QStringList();
        for (int i=0; i<curves; i++)
        {//We rearrange the list so that the error bars are placed at the end
        	int j = w->colIndex(names[i]);
  	        if (w->colPlotDesignation(j) == Table::xErr || w->colPlotDesignation(j) == Table::yErr)
			{
				errCurves++;
				lst << names[i];
			}
			else
				lst.prepend(names[i]);
        }

		for (int i=0; i<curves; i++)
		{
            int j = w->colIndex(names[i]);
            bool ok = false;
            if (w->colPlotDesignation(j) == Table::xErr || w->colPlotDesignation(j) == Table::yErr)
			{
				int ycol = w->colY(w->colIndex(names[i]));
                if (ycol < 0)
                    return false;

                if (w->colPlotDesignation(j) == Table::xErr)
                    ok = addErrorBars(w->colName(ycol), w, names[i], (int)QwtErrorPlotCurve::Horizontal);
                else
                    ok = addErrorBars(w->colName(ycol), w, names[i]);
			}
			else
                ok = insertCurve(w, names[i], style, startRow, endRow);

            if (ok)
			{
				CurveLayout cl = initCurveLayout(style, curves - errCurves);
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

bool Graph::insertCurve(Table* w, const QString& name, int style, int startRow, int endRow)
{//provided for convenience
	int ycol = w->colIndex(name);
	int xcol = w->colX(ycol);

	bool succes = insertCurve(w, w->colName(xcol), w->colName(ycol), style, startRow, endRow);
	if (succes)
		emit modifiedGraph();
	return succes;
}

bool Graph::insertCurve(Table* w, int xcol, const QString& name, int style)
{
	return insertCurve(w, w->colName(xcol), w->colName(w->colIndex(name)), style);
}

bool Graph::insertCurve(Table* w, const QString& xColName, const QString& yColName, int style, int startRow, int endRow)
{
	int xcol=w->colIndex(xColName);
	int ycol=w->colIndex(yColName);
	if (xcol < 0 || ycol < 0)
		return false;

	int xColType = w->columnType(xcol);
	int yColType = w->columnType(ycol);
	int i, size=0;
	QString date_time_fmt = w->columnFormat(xcol);
	QStringList xLabels, yLabels;// store text labels
	QTime time0;
	QDate date0;
    QLocale locale;

	if (endRow < 0)
		endRow = w->numRows() - 1;

	int r = abs(endRow - startRow) + 1;
    QVector<double> X(r), Y(r);
	if (xColType == Table::Time)
	{
		for (i = startRow; i<=endRow; i++ )
		{
			QString xval=w->text(i,xcol);
			if (!xval.isEmpty())
			{
				time0 = QTime::fromString (xval, date_time_fmt);
				if (time0.isValid())
					break;
			}
		}
	}
	else if (xColType == Table::Date)
	{
		for (i = startRow; i<=endRow; i++ )
		{
			QString xval=w->text(i,xcol);
			if (!xval.isEmpty())
			{
				date0 = QDate::fromString(xval, date_time_fmt);
				if (date0.isValid())
					break;
			}
		}
	}

	for (i = startRow; i<=endRow; i++ )
	{
		QString xval=w->text(i,xcol);
		QString yval=w->text(i,ycol);
		if (!xval.isEmpty() && !yval.isEmpty())
		{
			if (xColType == Table::Text)
			{
				if (xLabels.contains(xval) == 0)
					xLabels << xval;
				X[size] = (double)(xLabels.findIndex(xval)+1);
			}
			else if (xColType == Table::Time)
			{
				QTime time = QTime::fromString (xval, date_time_fmt);
				if (time.isValid())
					X[size] = time0.msecsTo (time);
				else
					X[size] = 0;
			}
			else if (xColType == Table::Date)
			{
				QDate d = QDate::fromString (xval, date_time_fmt);
				if (d.isValid())
					X[size] = (double) date0.daysTo(d);
			}
			else
                X[size] = Table::stringToDouble(xval);

			if (yColType == Table::Text)
			{
				yLabels << yval;
				Y[size] = (double) (size + 1);
			}
			else
                Y[size] = Table::stringToDouble(yval);

            size++;
		}
	}

	if (!size)
		return false;

	X.resize(size);
	Y.resize(size);

	DataCurve *c = 0;
	if (style == VerticalBars)
	{
		c = new QwtBarCurve(QwtBarCurve::Vertical, w, xColName, yColName, startRow, endRow);
		c->setStyle(QwtPlotCurve::UserCurve);
	}
	else if (style == HorizontalBars)
	{
		c = new QwtBarCurve(QwtBarCurve::Horizontal, w, xColName, yColName, startRow, endRow);
		c->setStyle(QwtPlotCurve::UserCurve);
	}
	else if (style == Histogram)
	{
		c = new QwtHistogram(w, xColName, yColName, startRow, endRow);
		((QwtHistogram *)c)->initData(Y, size);
		c->setStyle(QwtPlotCurve::UserCurve);
	}
	else
		c = new DataCurve(w, xColName, yColName, startRow, endRow);

	c_type.resize(++n_curves);
	c_type[n_curves-1] = style;
	c_keys.resize(n_curves);
	c_keys[n_curves-1] = d_plot->insertCurve(c);

	c->setPen(QPen(Qt::black,widthLine));

	if (style == HorizontalBars)
		c->setData(Y.data(), X.data(), size);
	else if (style != Histogram)
		c->setData(X.data(), Y.data(), size);

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
	else if (xColType == Table::Time)
	{
		QString fmtInfo = time0.toString() + ";" + date_time_fmt;
		if (style == HorizontalBars)
			setLabelsDateTimeFormat(QwtPlot::yLeft, Time, fmtInfo);
		else
			setLabelsDateTimeFormat(QwtPlot::xBottom, Time, fmtInfo);
	}
	else if (xColType == Table::Date )
	{
		QString fmtInfo = date0.toString(Qt::ISODate) + ";" + date_time_fmt;
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

void Graph::plotVectorCurve(Table* w, const QStringList& colList, int style, int startRow, int endRow)
{
	if (colList.count() != 4)
		return;

	if (endRow < 0)
		endRow = w->numRows() - 1;

	VectorCurve *v = 0;
	if (style == VectXYAM)
		v = new VectorCurve(VectorCurve::XYAM, w, colList[0], colList[1], colList[2], colList[3], startRow, endRow);
	else
		v = new VectorCurve(VectorCurve::XYXY, w, colList[0], colList[1], colList[2], colList[3], startRow, endRow);

	if (!v)
		return;

	v->loadData();
	v->setStyle(QwtPlotCurve::NoCurve);

	c_type.resize(++n_curves);
	c_type[n_curves-1] = style;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = d_plot->insertCurve(v);

	addLegendItem(colList[1]);
	updatePlot();
}

void Graph::updateVectorsLayout(int curve, const QColor& color, int width,
		int arrowLength, int arrowAngle, bool filled, int position,
		const QString& xEndColName, const QString& yEndColName)
{
	VectorCurve *vect = (VectorCurve *)this->curve(curve);
	if (!vect)
		return;

	vect->setColor(color);
	vect->setWidth(width);
	vect->setHeadLength(arrowLength);
	vect->setHeadAngle(arrowAngle);
	vect->fillArrowHead(filled);
	vect->setPosition(position);

	if (!xEndColName.isEmpty() && !yEndColName.isEmpty())
		vect->setVectorEnd(xEndColName, yEndColName);

	d_plot->replot();
	emit modifiedGraph();
}

void Graph::updatePlot()
{
	if (autoscale && !zoomOn() && d_active_tool==NULL)	{
		for (int i = 0; i < QwtPlot::axisCnt; i++)
			d_plot->setAxisAutoScale(i);
	}

	d_plot->replot();
    updateMarkersBoundingRect();
    if (isPiePlot())
    {
        QwtPieCurve *c = (QwtPieCurve *)curve(0);
        c->updateBoundingRect();
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

	d_plot->replot();//TODO: avoid 2nd replot!
}

void Graph::setBarsGap(int curve, int gapPercent, int offset)
{
	QwtBarCurve *bars = (QwtBarCurve *)this->curve(curve);
	if (!bars)
		return;

    if (bars->gap() == gapPercent && bars->offset() == offset)
        return;

	bars->setGap(gapPercent);
	bars->setOffset(offset);
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
	emit modifiedGraph();
}

void Graph::removeCurves(const QString& s)
{
    QList<int> keys = d_plot->curveKeys();
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotItem *it = d_plot->plotItem(keys[i]);
		if (!it)
            continue;

        if (it->title().text() == s)
        {
            removeCurve(i);
            continue;
        }

        if (it->rtti() != QwtPlotItem::Rtti_PlotCurve)
            continue;
        if (((PlotCurve *)it)->type() == Function)
            continue;

        if(((DataCurve *)it)->plotAssociation().contains(s))
            removeCurve(i);
	}
	d_plot->replot();
}

void Graph::removeCurve(const QString& s)
{
	removeCurve(plotItemsList().findIndex(s));
}

void Graph::removeCurve(int index)
{
	if (index < 0 || index >= n_curves)
		return;

	QwtPlotItem *it = plotItem(index);
	if (!it)
		return;

	removeLegendItem(index);

	if (it->rtti() != QwtPlotItem::Rtti_PlotSpectrogram)
	{
        if (((PlotCurve *)it)->type() == ErrorBars)
            ((QwtErrorPlotCurve *)it)->detachFromMasterCurve();
		else if (((PlotCurve *)it)->type() != Function)
			((DataCurve *)it)->clearErrorBars();

		if (d_fit_curves.contains((QwtPlotCurve *)it))
		{
			int i = d_fit_curves.indexOf((QwtPlotCurve *)it);
			if (i >= 0 && i < d_fit_curves.size())
				d_fit_curves.removeAt(i);
		}
	}

    if (d_range_selector && curve(index) == d_range_selector->selectedCurve())
    {
        if (n_curves > 1 && (index - 1) >= 0)
            d_range_selector->setSelectedCurve(curve(index - 1));
        else if (n_curves > 1 && index + 1 < n_curves)
            d_range_selector->setSelectedCurve(curve(index + 1));
        else
            delete d_range_selector;
    }

	d_plot->removeCurve(c_keys[index]);
	n_curves--;

	for (int i=index; i<n_curves; i++)
    {
        c_type[i] = c_type[i+1];
        c_keys[i] = c_keys[i+1];
    }
    c_type.resize(n_curves);
    c_keys.resize(n_curves);
	emit modifiedGraph();
}

void Graph::removeLegendItem(int index)
{
	if (legendMarkerID<0 || c_type[index] == ErrorBars)
		return;

	LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
	if (!mrk)
		return;

	if (isPiePlot())
	{
		mrk->setText(QString::null);
		return;
	}

	QString text=mrk->text();
	QStringList items=text.split( "\n", QString::SkipEmptyParts);

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
	if (legendMarkerID >= 0 )
	{
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
		{
			QString text = mrk->text();
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
	if (selectedMarker>=0) {
		emit showMarkerPopupMenu();
		return;
	}

	QPoint pos = d_plot->canvas()->mapFrom(d_plot, e->pos());
	int dist, point;
	const long curve = d_plot->closestCurve(pos.x(), pos.y(), dist, point);
	const QwtPlotCurve *c = (QwtPlotCurve *)d_plot->curve(curve);

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

void Graph::zoomed (const QwtDoubleRect &)
{
	emit modifiedGraph();
}

void Graph::zoom(bool on)
{
	d_zoomer[0]->setEnabled(on);
	d_zoomer[1]->setEnabled(on);
	for (int i=0; i<n_curves; i++)
  	{
  		Spectrogram *sp = (Spectrogram *)this->curve(i);
  	    if (sp && sp->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
  	    {
  	     	if (sp->colorScaleAxis() == QwtPlot::xBottom || sp->colorScaleAxis() == QwtPlot::yLeft)
  	        	d_zoomer[0]->setEnabled(false);
  	        else
  	        	d_zoomer[1]->setEnabled(false);
  	    }
  	}

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

	updateSecondaryAxis(QwtPlot::xTop);
  	updateSecondaryAxis(QwtPlot::yRight);
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
	QPixmap photo = mrk->pixmap();
	ImageMarker* mrk2= new ImageMarker(photo);

	int imagesOnPlot=d_images.size();
	d_images.resize(++imagesOnPlot);
	d_images[imagesOnPlot-1]=d_plot->insertMarker(mrk2);

	mrk2->setFileName(mrk->getFileName());
	mrk2->setBoundingRect(mrk->xValue(), mrk->yValue(), mrk->right(), mrk->bottom());
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
	QString fn = lst[1];
	if (!QFile::exists(fn))
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

        int imagesOnPlot = d_images.size();
		d_images.resize(++imagesOnPlot);

		ImageMarker* mrk = new ImageMarker(photo);
		mrk->setFileName(fn);
		d_images[imagesOnPlot-1] = d_plot->insertMarker(mrk);

		if (fileVersion < 86)
		{
			mrk->setOrigin(QPoint(lst[2].toInt(), lst[3].toInt()));
			mrk->setSize(QSize(lst[4].toInt(), lst[5].toInt()));
		}
		else if (fileVersion < 90)
		{
		    double left = lst[2].toDouble();
		    double right = left + lst[4].toDouble();
		    double top = lst[3].toDouble();
		    double bottom = top - lst[5].toDouble();
			mrk->setBoundingRect(left, top, right, bottom);
		}
		else
			mrk->setBoundingRect(lst[2].toDouble(), lst[3].toDouble(), lst[4].toDouble(), lst[5].toDouble());
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

	QString oldLegend = c->legend();

	c->setFunctionType((FunctionCurve::FunctionType)type);
	c->setRange(ranges[0], ranges[1]);
	c->setFormulas(formulas);
	c->setVariable(var);
	c->loadData(points);

	if (legendMarkerID >= 0)
	{//update the legend marker
		LegendMarker* mrk=(LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
		{
			QString text = (mrk->text()).replace(oldLegend, c->legend());
			mrk->setText(text);
		}
	}
	updatePlot();
	emit modifiedGraph();
}

QString Graph::generateFunctionName(const QString& name)
{
    int index = 1;
  	QString newName = name + QString::number(index);

  	QStringList lst;
  	for (int i=0; i<n_curves; i++)
  	{
  		PlotCurve *c = (PlotCurve*)this->curve(i);
  		if (!c)
            continue;

  	    if (c->type() == Function)
  	    	lst << c->title().text();
	}

  	while(lst.contains(newName)){
  	        newName = name + QString::number(++index);}
  	return newName;
}

void Graph::addFunctionCurve(int type, const QStringList &formulas, const QString &var,
		QList<double> &ranges, int points, const QString& title)
{
	QString name;
	if (!title.isEmpty())
		name = title;
	else
		name = generateFunctionName();

	FunctionCurve *c = new FunctionCurve((const FunctionCurve::FunctionType)type, name);
	c->setPen(QPen(QColor(Qt::black), widthLine));
	c->setRange(ranges[0], ranges[1]);
	c->setFormulas(formulas);
	c->setVariable(var);
	c->loadData(points);

	c_type.resize(++n_curves);
	c_type[n_curves-1] = Line;

	c_keys.resize(n_curves);
	c_keys[n_curves-1] = d_plot->insertCurve(c);

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

	QStringList curve = formula.split(",");
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

void Graph::createTable(const QString& curveName)
{
    if (curveName.isEmpty())
        return;

    const QwtPlotCurve* cv = curve(curveName);
    if (!cv)
        return;

    createTable(cv);
}

void Graph::createTable(const QwtPlotCurve* curve)
{
    if (!curve)
        return;

    int size = curve->dataSize();
    QString text = "1\t2\n";
    for (int i=0; i<size; i++)
	{
        text += QString::number(curve->x(i))+"\t";
        text += QString::number(curve->y(i))+"\n";
    }
    QString legend = tr("Data set generated from curve") + ": " + curve->title().text();
    emit createTable(tr("Table") + "1" + "\t" + legend, size, 2, text);
}

QString Graph::saveToString(bool saveAsTemplate)
{
	QString s="<graph>\n";
	s+="ggeometry\t";
	s+=QString::number(this->pos().x())+"\t";
	s+=QString::number(this->pos().y())+"\t";
	s+=QString::number(this->frameGeometry().width())+"\t";
	s+=QString::number(this->frameGeometry().height())+"\n";
	s+=saveTitle();
	s+="<Antialiasing>" + QString::number(d_antialiasing) + "</Antialiasing>\n";
	s+="Background\t" + d_plot->paletteBackgroundColor().name() + "\t";
	s+=QString::number(d_plot->paletteBackgroundColor().alpha()) + "\n";
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

    if (!saveAsTemplate)
	   s+=saveCurves();

	s+=saveScale();
	s+=saveAxesFormulas();
	s+=saveLabelsFormat();
	s+=saveAxesLabelsType();
	s+=saveTicksType();
	s+="TicksLength\t"+QString::number(minorTickLength())+"\t"+QString::number(majorTickLength())+"\n";
	s+="DrawAxesBackbone\t"+QString::number(drawAxesBackbone)+"\n";
	s+="AxesLineWidth\t"+QString::number(d_plot->axesLinewidth())+"\n";
	s+=saveLabelsRotation();
	s+=saveMarkers();
	s+="</graph>\n";
	return s;
}

void Graph::showIntensityTable()
{
	ImageMarker* mrk=(ImageMarker*) d_plot->marker(selectedMarker);
	if (!mrk)
		return;

	emit createIntensityTable(mrk->getFileName());
}

void Graph::updateMarkersBoundingRect()
{
	for (int i=0;i<(int)d_lines.size();i++)
	{
		LineMarker* mrkL = (LineMarker*)d_plot->marker(d_lines[i]);
		if (mrkL)
			mrkL->updateBoundingRect();
	}
	for (int i=0; i<(int)d_texts.size(); i++)
	{
		LegendMarker* mrkT = (LegendMarker*) d_plot->marker(d_texts[i]);
		if (mrkT)
			mrkT->updateOrigin();
	}

	for (int i=0;i<(int)d_images.size();i++)
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

	d_plot->resize(e->size());
}

void Graph::scaleFonts(double factor)
{
	for (int i=0;i<(int)d_texts.size();i++)
	{
		LegendMarker* mrk = (LegendMarker*) d_plot->marker(d_texts[i]);
		QFont font = mrk->font();
		font.setPointSizeFloat(factor*font.pointSizeFloat());
		mrk->setFont(font);
	}
	for (int i = 0; i<QwtPlot::axisCnt; i++)
	{
		QFont font = axisFont(i);
		font.setPointSizeFloat(factor*font.pointSizeFloat());
		d_plot->setAxisFont(i, font);

		QwtText title = d_plot->axisTitle(i);
		font = title.font();
		font.setPointSizeFloat(factor*font.pointSizeFloat());
		title.setFont(font);
		d_plot->setAxisTitle(i, title);
	}

	QwtText title = d_plot->title();
	QFont font = title.font();
	font.setPointSizeFloat(factor*font.pointSizeFloat());
	title.setFont(font);
	d_plot->setTitle(title);

	d_plot->replot();
}

void Graph::changeMargin (int d)
{
	if (d_plot->margin() == d)
		return;

	d_plot->setMargin(d);
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
    QColorGroup cg;
	QPalette p = d_plot->palette();
	p.setColor(QColorGroup::Window, color);
    d_plot->setPalette(p);

    d_plot->setAutoFillBackground(true);
	emit modifiedGraph();
}

void Graph::setCanvasBackground(const QColor& color)
{
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

void Graph::showPlotErrorMessage(QWidget *parent, const QStringList& emptyColumns)
{
	QApplication::restoreOverrideCursor();

	int n = (int)emptyColumns.count();
	if (n > 1)
	{
		QString columns;
		for (int i = 0; i < n; i++)
			columns += "<p><b>" + emptyColumns[i] + "</b></p>";

		QMessageBox::warning(parent, tr("QtiPlot - Warning"),
				tr("The columns") + ": " + columns + tr("are empty and will not be added to the plot!"));
	}
	else if (n == 1)
		QMessageBox::warning(parent, tr("QtiPlot - Warning"),
				tr("The column") + " <b>" + emptyColumns[0] + "</b> " + tr("is empty and will not be added to the plot!"));
}

void Graph::showTitleContextMenu()
{
	QMenu titleMenu(this);
	titleMenu.insertItem(QPixmap(cut_xpm), tr("&Cut"),this, SLOT(cutTitle()));
	titleMenu.insertItem(QPixmap(copy_xpm), tr("&Copy"),this, SLOT(copyTitle()));
	titleMenu.insertItem(tr("&Delete"),this, SLOT(removeTitle()));
	titleMenu.insertSeparator();
	titleMenu.insertItem(tr("&Properties..."), this, SIGNAL(viewTitleDialog()));
	titleMenu.exec(QCursor::pos());
}

void Graph::cutTitle()
{
	QApplication::clipboard()->setText(d_plot->title().text(), QClipboard::Clipboard);
	removeTitle();
}

void Graph::copyTitle()
{
	QApplication::clipboard()->setText(d_plot->title().text(), QClipboard::Clipboard);
}

void Graph::removeAxisTitle()
{
	int axis = (selectedAxis + 2)%4;//unconsistent notation in Qwt enumerations between
  	//QwtScaleDraw::alignement and QwtPlot::Axis
  	d_plot->setAxisTitle(axis, QString::null);
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
	int axis = (selectedAxis + 2)%4;//unconsistent notation in Qwt enumerations between
  	//QwtScaleDraw::alignement and QwtPlot::Axis
  	QApplication::clipboard()->setText(d_plot->axisTitle(axis).text(), QClipboard::Clipboard);
	}

void Graph::showAxisTitleMenu(int axis)
{
	selectedAxis = axis;

	QMenu titleMenu(this);
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

	QMenu menu(this);
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

	setAntialiasing(g->antialiasing());

	setBackgroundColor(plot->paletteBackgroundColor());
	setBorder(plot->lineWidth(), plot->frameColor());
	setCanvasBackground(plot->canvasBackground());

	enableAxes(g->enabledAxes());
	setAxesColors(g->axesColors());
    setAxesNumColors(g->axesNumColors());
	setAxesBaseline(g->axesBaseline());

	setGridOptions(g->gridOptions());

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

    for (i=0; i<g->curves(); i++)
		{
			QwtPlotItem *it = (QwtPlotItem *)g->plotItem(i);
			if (it->rtti() == QwtPlotItem::Rtti_PlotCurve)
  	        {
  	        DataCurve *cv = (DataCurve *)it;
			int n = cv->dataSize();
			int style = ((PlotCurve *)it)->type();
			QVector<double> x(n);
			QVector<double> y(n);
			for (int j=0; j<n; j++)
			{
				x[j]=cv->x(j);
				y[j]=cv->y(j);
			}

			PlotCurve *c = 0;
			if (style == Pie)
			{
				c = new QwtPieCurve(cv->table(), cv->title().text(), cv->startRow(), cv->endRow());
				((QwtPieCurve*)c)->setRay(((QwtPieCurve*)cv)->ray());
                ((QwtPieCurve*)c)->setFirstColor(((QwtPieCurve*)cv)->firstColor());
			}
			else if (style == Function)
			{
				c = new FunctionCurve(cv->title().text());
				((FunctionCurve*)c)->copy((FunctionCurve*)cv);
			}
			else if (style == VerticalBars || style == HorizontalBars)
			{
				c = new QwtBarCurve(((QwtBarCurve*)cv)->orientation(), cv->table(), cv->xColumnName(),
									cv->title().text(), cv->startRow(), cv->endRow());
				((QwtBarCurve*)c)->copy((const QwtBarCurve*)cv);
			}
			else if (style == ErrorBars)
			{
				QwtErrorPlotCurve *er = (QwtErrorPlotCurve*)cv;
				DataCurve *master_curve = masterCurve(er);
				if (master_curve)
				{
					c = new QwtErrorPlotCurve(cv->table(), cv->title().text());
					((QwtErrorPlotCurve*)c)->copy(er);
					((QwtErrorPlotCurve*)c)->setMasterCurve(master_curve);
				}
			}
			else if (style == Histogram)
			{
				c = new QwtHistogram(cv->table(), cv->xColumnName(), cv->title().text(), cv->startRow(), cv->endRow());
				((QwtHistogram *)c)->copy((const QwtHistogram*)cv);
			}
			else if (style == VectXYXY || style == VectXYAM)
			{
				VectorCurve::VectorStyle vs = VectorCurve::XYXY;
				if (style == VectXYAM)
					vs = VectorCurve::XYAM;
				c = new VectorCurve(vs, cv->table(), cv->xColumnName(), cv->title().text(),
									((VectorCurve *)cv)->vectorEndXAColName(),
									((VectorCurve *)cv)->vectorEndYMColName(),
									cv->startRow(), cv->endRow());
				((VectorCurve *)c)->copy((const VectorCurve *)cv);
			}
			else if (style == Box)
			{
				c = new BoxCurve(cv->table(), cv->title().text(), cv->startRow(), cv->endRow());
				((BoxCurve*)c)->copy((const BoxCurve *)cv);
				QwtSingleArrayData dat(x[0], y, n);
				c->setData(dat);
			}
			else
				c = new DataCurve(cv->table(), cv->xColumnName(), cv->title().text(), cv->startRow(), cv->endRow());

			c_keys.resize(++n_curves);
			c_keys[i] = d_plot->insertCurve(c);

			c_type.resize(n_curves);
			c_type[i] = g->curveType(i);

			if (c_type[i] != Box && c_type[i] != ErrorBars)
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
			c->setVisible(cv->isVisible());

			QList<QwtPlotCurve *>lst = g->fitCurvesList();
			if (lst.contains((QwtPlotCurve *)it))
				d_fit_curves << c;
		}
		else if (it->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
  	    	{
  	     	Spectrogram *sp = ((Spectrogram *)it)->copy();
  	        c_keys.resize(++n_curves);
  	        c_keys[i] = d_plot->insertCurve(sp);

  	        sp->showColorScale(((Spectrogram *)it)->colorScaleAxis(), ((Spectrogram *)it)->hasColorScale());
  	        sp->setColorBarWidth(((Spectrogram *)it)->colorBarWidth());
			sp->setVisible(it->isVisible());

  	        c_type.resize(n_curves);
  	        c_type[i] = g->curveType(i);
  	        }
  	    }

	axesFormulas = g->axesFormulas;
	axisType = g->axisType;
	axesFormatInfo = g->axesFormatInfo;
	axisType = g->axisType;

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

		int majorTicks = plot->axisMaxMajor(i);
  	    int minorTicks = plot->axisMaxMinor(i);
  	    d_plot->setAxisMaxMajor (i, majorTicks);
  	    d_plot->setAxisMaxMinor (i, minorTicks);

		const QwtScaleDiv *sd = plot->axisScaleDiv(i);
		QwtValueList lst = sd->ticks (QwtScaleDiv::MajorTick);

		d_user_step[i] = g->axisStep(i);

		QwtScaleDiv div = sc_engine->divideScale (QMIN(sd->lBound(), sd->hBound()),
				QMAX(sd->lBound(), sd->hBound()), majorTicks, minorTicks, d_user_step[i]);

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

	setAxisLabelRotation(QwtPlot::xBottom, g->labelsRotation(QwtPlot::xBottom));
  	setAxisLabelRotation(QwtPlot::xTop, g->labelsRotation(QwtPlot::xTop));

	QVector<int> imag = g->imageMarkerKeys();
	for (i=0; i<(int)imag.size(); i++)
	{
		ImageMarker* imrk = (ImageMarker*)g->imageMarker(imag[i]);
		if (imrk)
			insertImageMarker(imrk);
	}

	QVector<int> txtMrkKeys=g->textMarkerKeys();
	for (i=0; i<(int)txtMrkKeys.size(); i++)
	{
		LegendMarker* mrk = (LegendMarker*)g->textMarker(txtMrkKeys[i]);
		if (!mrk)
			continue;

		if (txtMrkKeys[i] == g->legendMarkerID)
			legendMarkerID = insertTextMarker(mrk);
		else
			insertTextMarker(mrk);
	}

	QVector<int> l = g->lineMarkerKeys();
	for (i=0; i<(int)l.size(); i++)
	{
		LineMarker* lmrk=(LineMarker*)g->lineMarker(l[i]);
		if (lmrk)
			insertLineMarker(lmrk);
	}
	d_plot->replot();

    if (isPiePlot())
    {
        QwtPieCurve *c = (QwtPieCurve *)curve(0);
        c->updateBoundingRect();
    }
}

void Graph::plotBoxDiagram(Table *w, const QStringList& names, int startRow, int endRow)
{
	if (endRow < 0)
		endRow = w->numRows() - 1;

	for (int j = 0; j <(int)names.count(); j++)
	{
        BoxCurve *c = new BoxCurve(w, names[j], startRow, endRow);
        c->setData(QwtSingleArrayData(double(j+1), QwtArray<double>(), 0));
        c->loadData();

        c_keys.resize(++n_curves);
        c_keys[n_curves-1] = d_plot->insertCurve(c);
        c_type.resize(n_curves);
        c_type[n_curves-1] = Box;

        c->setPen(QPen(ColorBox::color(j), 1));
        c->setSymbol(QwtSymbol(QwtSymbol::NoSymbol, QBrush(), QPen(ColorBox::color(j), 1), QSize(7,7)));
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
	if (!c)
		return;

	c->setSymbol(s);
}

void Graph::setCurvePen(int index, const QPen& p)
{
	QwtPlotCurve *c = curve(index);
	if (!c)
		return;

	c->setPen(p);
}

void Graph::setCurveBrush(int index, const QBrush& b)
{
	QwtPlotCurve *c = curve(index);
	if (!c)
		return;

	c->setBrush(b);
}

void Graph::openBoxDiagram(Table *w, const QStringList& l, int fileVersion)
{
    if (!w)
        return;

    int startRow = 0;
    int endRow = w->numRows()-1;
    if (fileVersion >= 90)
    {
        startRow = l[l.count()-3].toInt();
        endRow = l[l.count()-2].toInt();
    }

	BoxCurve *c = new BoxCurve(w, l[2], startRow, endRow);
	c->setData(QwtSingleArrayData(l[1].toDouble(), QwtArray<double>(), 0));
	c->setData(QwtSingleArrayData(l[1].toDouble(), QwtArray<double>(), 0));
	c->loadData();

	c_keys.resize(++n_curves);
	c_keys[n_curves-1] = d_plot->insertCurve(c);
	c_type.resize(n_curves);
	c_type[n_curves-1] = Box;

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

void Graph::disableTools()
{
	if (zoomOn())
		zoom(false);
	if (drawLineActive())
		drawLine(false);
	setActiveTool(NULL);
	if (d_range_selector)
		delete d_range_selector;
}

bool Graph::enableRangeSelectors(const QObject *status_target, const char *status_slot)
{
	if (d_range_selector)
		delete d_range_selector;
	d_range_selector = new RangeSelectorTool(this, status_target, status_slot);
	connect(d_range_selector, SIGNAL(changed()), this, SIGNAL(dataRangeChanged()));
	return true;
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

void Graph::guessUniqueCurveLayout(int& colorIndex, int& symbolIndex)
{
	colorIndex = 0;
	symbolIndex = 0;

	int curve_index = n_curves - 1;
	if (curve_index >= 0 && c_type[curve_index] == ErrorBars)
	{// find out the pen color of the master curve
		QwtErrorPlotCurve *er = (QwtErrorPlotCurve *)d_plot->curve(c_keys[curve_index]);
		DataCurve *master_curve = er->masterCurve();
		if (master_curve)
		{
			colorIndex = ColorBox::colorIndex(master_curve->pen().color());
			return;
		}
	}

	for (int i=0; i<n_curves; i++)
	{
		const QwtPlotCurve *c = curve(i);
		if (c && c->rtti() == QwtPlotItem::Rtti_PlotCurve)
		{
			int index = ColorBox::colorIndex(c->pen().color());
			if (index > colorIndex)
				colorIndex = index;

			QwtSymbol symb = c->symbol();
			index = SymbolBox::symbolIndex(symb.style());
			if (index > symbolIndex)
				symbolIndex = index;
		}
	}
	if (n_curves > 1)
		colorIndex = (++colorIndex)%16;
	if (colorIndex == 13) //avoid white invisible curves
		colorIndex = 0;

	symbolIndex = (++symbolIndex)%15;
	if (!symbolIndex)
		symbolIndex = 1;
}

void Graph::addFitCurve(QwtPlotCurve *c)
{
	if (c)
		d_fit_curves << c;
}

void Graph::deleteFitCurves()
{
	QList<int> keys = d_plot->curveKeys();
	foreach(QwtPlotCurve *c, d_fit_curves)
		removeCurve(curveIndex(c));

	d_plot->replot();
}

void Graph::plotSpectrogram(Matrix *m, CurveType type)
{
	if (type != GrayMap && type != ColorMap && type != ContourMap)
  		return;

  	Spectrogram *d_spectrogram = new Spectrogram(m);
  	if (type == GrayMap)
  		d_spectrogram->setGrayScale();
  	else if (type == ContourMap)
  		{
  	    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, false);
  	    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, true);
  	    }
  	else if (type == ColorMap)
  	    {
  	    d_spectrogram->setDefaultColorMap();
  	    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, true);
  	    }

  	c_keys.resize(++n_curves);
  	c_keys[n_curves-1] = d_plot->insertCurve(d_spectrogram);

  	c_type.resize(n_curves);
  	c_type[n_curves-1] = type;

  	QwtScaleWidget *rightAxis = d_plot->axisWidget(QwtPlot::yRight);
  	rightAxis->setColorBarEnabled(type != ContourMap);
  	rightAxis->setColorMap(d_spectrogram->data().range(), d_spectrogram->colorMap());

  	d_plot->setAxisScale(QwtPlot::xBottom, m->xStart(), m->xEnd());
  	d_plot->setAxisScale(QwtPlot::yLeft, m->yStart(), m->yEnd());

  	d_plot->setAxisScale(QwtPlot::yRight,
  	d_spectrogram->data().range().minValue(),
  	d_spectrogram->data().range().maxValue());
  	d_plot->enableAxis(QwtPlot::yRight, type != ContourMap);

  	d_plot->replot();
}

void Graph::restoreSpectrogram(ApplicationWindow *app, const QStringList& lst)
{
  	QStringList::const_iterator line = lst.begin();
  	QString s = (*line).stripWhiteSpace();
  	QString matrixName = s.remove("<matrix>").remove("</matrix>");
  	Matrix *m = app->matrix(matrixName);
  	if (!m)
        return;

  	Spectrogram *sp = new Spectrogram(m);

	c_type.resize(++n_curves);
	c_type[n_curves-1] = Graph::ColorMap;
	c_keys.resize(n_curves);
	c_keys[n_curves-1] = d_plot->insertCurve(sp);

  	for (line++; line != lst.end(); line++)
    {
        QString s = *line;
        if (s.contains("<ColorPolicy>"))
        {
            int color_policy = s.remove("<ColorPolicy>").remove("</ColorPolicy>").stripWhiteSpace().toInt();
            if (color_policy == Spectrogram::GrayScale)
                sp->setGrayScale();
            else if (color_policy == Spectrogram::Default)
                sp->setDefaultColorMap();
        }
        else if (s.contains("<ColorMap>"))
        {
            s = *(++line);
            int mode = s.remove("<Mode>").remove("</Mode>").stripWhiteSpace().toInt();
            s = *(++line);
            QColor color1 = QColor(s.remove("<MinColor>").remove("</MinColor>").stripWhiteSpace());
            s = *(++line);
            QColor color2 = QColor(s.remove("<MaxColor>").remove("</MaxColor>").stripWhiteSpace());

            QwtLinearColorMap colorMap = QwtLinearColorMap(color1, color2);
            colorMap.setMode((QwtLinearColorMap::Mode)mode);

            s = *(++line);
            int stops = s.remove("<ColorStops>").remove("</ColorStops>").stripWhiteSpace().toInt();
            for (int i = 0; i < stops; i++)
            {
                s = (*(++line)).stripWhiteSpace();
                QStringList l = QStringList::split("\t", s.remove("<Stop>").remove("</Stop>"));
                colorMap.addColorStop(l[0].toDouble(), QColor(l[1]));
            }
            sp->setCustomColorMap(colorMap);
            line++;
        }
        else if (s.contains("<Image>"))
        {
            int mode = s.remove("<Image>").remove("</Image>").stripWhiteSpace().toInt();
            sp->setDisplayMode(QwtPlotSpectrogram::ImageMode, mode);
        }
        else if (s.contains("<ContourLines>"))
        {
            int contours = s.remove("<ContourLines>").remove("</ContourLines>").stripWhiteSpace().toInt();
            sp->setDisplayMode(QwtPlotSpectrogram::ContourMode, contours);
            if (contours)
            {
                s = (*(++line)).stripWhiteSpace();
                int levels = s.remove("<Levels>").remove("</Levels>").toInt();
                sp->setLevelsNumber(levels);

                s = (*(++line)).stripWhiteSpace();
                int defaultPen = s.remove("<DefaultPen>").remove("</DefaultPen>").toInt();
                if (!defaultPen)
                    sp->setDefaultContourPen(Qt::NoPen);
                else
                {
                    s = (*(++line)).stripWhiteSpace();
                    QColor c = QColor(s.remove("<PenColor>").remove("</PenColor>"));
                    s = (*(++line)).stripWhiteSpace();
                    int width = s.remove("<PenWidth>").remove("</PenWidth>").toInt();
                    s = (*(++line)).stripWhiteSpace();
                    int style = s.remove("<PenStyle>").remove("</PenStyle>").toInt();
                    sp->setDefaultContourPen(QPen(c, width, Graph::getPenStyle(style)));
                }
            }
        }
        else if (s.contains("<ColorBar>"))
        {
            s = *(++line);
            int color_axis = s.remove("<axis>").remove("</axis>").stripWhiteSpace().toInt();
            s = *(++line);
            int width = s.remove("<width>").remove("</width>").stripWhiteSpace().toInt();

            QwtScaleWidget *colorAxis = d_plot->axisWidget(color_axis);
            if (colorAxis)
            {
                colorAxis->setColorBarWidth(width);
                colorAxis->setColorBarEnabled(true);
            }
            line++;
        }
		else if (s.contains("<Visible>"))
        {
            int on = s.remove("<Visible>").remove("</Visible>").stripWhiteSpace().toInt();
            sp->setVisible(on);
        }
    }
}

bool Graph::validCurvesDataSize()
{
	if (!n_curves)
	{
		QMessageBox::warning(this, tr("QtiPlot - Warning"), tr("There are no curves available on this plot!"));
		return false;
	}
	else
	{
		for (int i=0; i < n_curves; i++)
		{
			 QwtPlotItem *item = curve(i);
  	         if(item && item->rtti() != QwtPlotItem::Rtti_PlotSpectrogram)
  	         {
  	             QwtPlotCurve *c = (QwtPlotCurve *)item;
  	             if (c->dataSize() >= 2)
                    return true;
  	         }
  	    }
		QMessageBox::warning(this, tr("QtiPlot - Error"),
		tr("There are no curves with more than two points on this plot. Operation aborted!"));
		return false;
	}
}

Graph::~Graph()
{
	setActiveTool(NULL);
	if (d_range_selector)
		delete d_range_selector;
	delete titlePicker;
	delete scalePicker;
	delete cp;
	delete d_plot;
}

void Graph::setAntialiasing(bool on, bool update)
{
	if (d_antialiasing == on)
		return;

	d_antialiasing = on;

	if (update)
	{
		QList<int> curve_keys = d_plot->curveKeys();
  		for (int i=0; i<(int)curve_keys.count(); i++)
  		{
  			QwtPlotItem *c = d_plot->curve(curve_keys[i]);
			if (c)
				c->setRenderHint(QwtPlotItem::RenderAntialiased, d_antialiasing);
		}
		QList<int> marker_keys = d_plot->markerKeys();
  		for (int i=0; i<(int)marker_keys.count(); i++)
  		{
  			QwtPlotMarker *m = d_plot->marker(marker_keys[i]);
			if (m)
				m->setRenderHint(QwtPlotItem::RenderAntialiased, d_antialiasing);
		}
		d_plot->replot();
	}
}

bool Graph::focusNextPrevChild ( bool )
{
	QList<int> mrkKeys = d_plot->markerKeys();
	int n = mrkKeys.size();
	if (n < 2)
		return false;

	int min_key = mrkKeys[0], max_key = mrkKeys[0];
	for (int i = 0; i<n; i++ )
	{
		if (mrkKeys[i] >= max_key)
			max_key = mrkKeys[i];
		if (mrkKeys[i] <= min_key)
			min_key = mrkKeys[i];
	}

	int key = selectedMarker;
	if (key >= 0)
	{
		key++;
		if ( key > max_key )
			key = min_key;
	} else
		key = min_key;

	cp->disableEditing();

	setSelectedMarker(key);
	return true;
}

QString Graph::axisFormatInfo(int axis)
{
	if (axis < 0 || axis > QwtPlot::axisCnt)
		return QString();
	else
		return axesFormatInfo[axis];
}

void Graph::updateCurveNames(const QString& oldName, const QString& newName, bool updateTableName)
{
    //update plotted curves list
	QList<int> keys = d_plot->curveKeys();
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotItem *it = d_plot->plotItem(keys[i]);
		if (!it)
            continue;
        if (it->rtti() != QwtPlotItem::Rtti_PlotCurve)
            continue;

        DataCurve *c = (DataCurve *)it;
        if (c->plotAssociation().contains(oldName))
            c->updateColumnNames(oldName, newName, updateTableName);
	}

    if (legendMarkerID >= 0 )
	{//update legend
		LegendMarker * mrk = (LegendMarker*) d_plot->marker(legendMarkerID);
		if (mrk)
		{
            QStringList lst = mrk->text().split("\n", QString::SkipEmptyParts);
            lst.replaceInStrings(oldName, newName);
			mrk->setText(lst.join("\n"));
			d_plot->replot();
		}
	}
}

void Graph::setCurveFullRange(int curveIndex)
{
	DataCurve *c = (DataCurve *)curve(curveIndex);
	if (c)
	{
		c->setFullRange();
		updatePlot();
		emit modifiedGraph();
	}
}

DataCurve* Graph::masterCurve(QwtErrorPlotCurve *er)
{
	QList<int> keys = d_plot->curveKeys();
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotItem *it = d_plot->plotItem(keys[i]);
		if (!it)
            continue;
        if (it->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
            continue;
        if (((PlotCurve *)it)->type() == Function)
            continue;

        if (((DataCurve *)it)->plotAssociation() == er->masterCurve()->plotAssociation())
			return (DataCurve *)it;
	}
	return 0;
}

DataCurve* Graph::masterCurve(const QString& xColName, const QString& yColName)
{
	QString master_curve = xColName + "(X)," + yColName + "(Y)";
	QList<int> keys = d_plot->curveKeys();
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotItem *it = d_plot->plotItem(keys[i]);
		if (!it)
            continue;
        if (it->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
            continue;
        if (((PlotCurve *)it)->type() == Function)
            continue;

        if (((DataCurve *)it)->plotAssociation() == master_curve)
			return (DataCurve *)it;
	}
	return 0;
}

void Graph::showCurve(int index, bool visible)
{
	QwtPlotItem *it = plotItem(index);
	if (it)
		it->setVisible(visible);

	emit modifiedGraph();
}

int Graph::visibleCurves()
{
    int c = 0;
	QList<int> keys = d_plot->curveKeys();
	for (int i=0; i<(int)keys.count(); i++)
	{
		QwtPlotItem *it = d_plot->plotItem(keys[i]);
		if (it && it->isVisible())
            c++;
	}
	return c;
}

QPrinter::PageSize Graph::minPageSize(const QPrinter& printer, const QRect& r)
{
	double x_margin = 0.2/2.54*printer.logicalDpiX(); // 2 mm margins
	double y_margin = 0.2/2.54*printer.logicalDpiY(); 
    double w_mm = 2*x_margin + (double)(r.width())/(double)printer.logicalDpiX() * 25.4;
    double h_mm = 2*y_margin + (double)(r.height())/(double)printer.logicalDpiY() * 25.4;

    int w, h;
    if (w_mm/h_mm > 1)
    {
        w = (int)ceil(w_mm);
        h = (int)ceil(h_mm);
    }
    else
    {
        h = (int)ceil(w_mm);
        w = (int)ceil(h_mm);
    }
	
	QPrinter::PageSize size = QPrinter::A5;
	if (w < 45 && h < 32)
        size =  QPrinter::B10;
	else if (w < 52 && h < 37)
        size =  QPrinter::A9;
    else if (w < 64 && h < 45)
        size =  QPrinter::B9;
    else if (w < 74 && h < 52)
        size =  QPrinter::A8;
    else if (w < 91 && h < 64)
        size =  QPrinter::B8;
    else if (w < 105 && h < 74)
        size =  QPrinter::A7;
    else if (w < 128 && h < 91)
        size =  QPrinter::B7;
    else if (w < 148 && h < 105)
        size =  QPrinter::A6;
    else if (w < 182 && h < 128)
        size =  QPrinter::B6;
    else if (w < 210 && h < 148)
        size =  QPrinter::A5;
	else if (w < 220 && h < 110)
        size =  QPrinter::DLE;
	else if (w < 229 && h < 163)
        size =  QPrinter::C5E;
	else if (w < 241 && h < 105)
        size =  QPrinter::Comm10E;
    else if (w < 257 && h < 182)
        size =  QPrinter::B5;
	else if (w < 279 && h < 216)
        size =  QPrinter::Letter;
    else if (w < 297 && h < 210)
        size =  QPrinter::A4;
	else if (w < 330 && h < 210)
        size =  QPrinter::Folio;
	else if (w < 356 && h < 216)
        size =  QPrinter::Legal;
    else if (w < 364 && h < 257)
        size =  QPrinter::B4;
    else if (w < 420 && h < 297)
        size =  QPrinter::A3;
    else if (w < 515 && h < 364)
        size =  QPrinter::B3;
    else if (w < 594 && h < 420)
        size =  QPrinter::A2;
    else if (w < 728 && h < 515)
        size =  QPrinter::B2;
	else if (w < 841 && h < 594)
        size =  QPrinter::A1;
    else if (w < 1030 && h < 728)
        size =  QPrinter::B1;
	else if (w < 1189 && h < 841)
        size =  QPrinter::A0;
    else if (w < 1456 && h < 1030)
        size =  QPrinter::B0;
	
	return size;
}
