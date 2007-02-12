/***************************************************************************
	File                 : Spectrogram.cpp
	Project              : QtiPlot
--------------------------------------------------------------------
	Copyright            : (C) 2006 by Ion Vasilief
	Email                : ion_vasilief@yahoo.fr
	Description          : QtiPlot's Spectrogram Class
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

#include "Spectrogram.h"
#include <math.h>
#include <qpen.h>
#include <qmessagebox.h>

#include <qwt_scale_widget.h>

Spectrogram::Spectrogram():
	QwtPlotSpectrogram(),
	d_matrix(0),
	color_axis(QwtPlot::yRight),
	color_map_policy(Default),
	color_map(QwtLinearColorMap())
{
}

Spectrogram::Spectrogram(Matrix *m):
	QwtPlotSpectrogram(QString(m->name())),
	d_matrix(m),
	color_axis(QwtPlot::yRight),
	color_map_policy(Default),
	color_map(QwtLinearColorMap())
{
setData(MatrixData(m));
double step = fabs(data().range().maxValue() - data().range().minValue())/5.0;

QwtValueList contourLevels;
for ( double level = data().range().minValue() + step; 
	level < data().range().maxValue(); level += step )
    contourLevels += level;

setContourLevels(contourLevels);
}

void Spectrogram::updateData(Matrix *m)
{
if (!m)
	return;

QwtPlot *plot = this->plot();
if (!plot)
	return;

setData(MatrixData(m));
setLevelsNumber(levels());

QwtScaleWidget *colorAxis = plot->axisWidget(color_axis);
if (colorAxis)
	colorAxis->setColorMap(data().range(), colorMap());

plot->setAxisScale(color_axis, data().range().minValue(), data().range().maxValue());
plot->replot();
}

void Spectrogram::setLevelsNumber(int levels)
{
double step = fabs(data().range().maxValue() - data().range().minValue())/(double)levels;

QwtValueList contourLevels;
for ( double level = data().range().minValue() + step; 
	level < data().range().maxValue(); level += step )
    contourLevels += level;

setContourLevels(contourLevels);	
}

bool Spectrogram::hasColorScale()
{
QwtPlot *plot = this->plot();
if (!plot)
	return false;

QwtScaleWidget *colorAxis = plot->axisWidget(color_axis);
return colorAxis->isColorBarEnabled();
}

void Spectrogram::showColorScale(int axis, bool on)
{
if (hasColorScale() == on && color_axis == axis)
	return;

QwtPlot *plot = this->plot();
if (!plot)
	return;

QwtScaleWidget *colorAxis = plot->axisWidget(color_axis);
colorAxis->setColorBarEnabled(false);

color_axis = axis;

// We must switch main and the color scale axes and their respective scales
int xAxis = this->xAxis();
int yAxis = this->yAxis();
int oldMainAxis;
if (axis == QwtPlot::xBottom || axis == QwtPlot::xTop)
	{
	oldMainAxis = xAxis;
	xAxis = 5 - color_axis;
	}
else if (axis == QwtPlot::yLeft || axis == QwtPlot::yRight)
	{
	oldMainAxis = yAxis;
	yAxis = 1 - color_axis;
	}

// First we switch axes
setAxis(xAxis, yAxis);

// Next we switch axes scales
QwtScaleDiv *scDiv = plot->axisScaleDiv(oldMainAxis);
if (axis == QwtPlot::xBottom || axis == QwtPlot::xTop)
	plot->setAxisScale(xAxis, scDiv->lBound(), scDiv->hBound());
else if (axis == QwtPlot::yLeft || color_axis == QwtPlot::yRight)
	plot->setAxisScale(yAxis, scDiv->lBound(), scDiv->hBound());

colorAxis = plot->axisWidget(color_axis);
plot->setAxisScale(color_axis, data().range().minValue(), data().range().maxValue());
colorAxis->setColorBarEnabled(on);
colorAxis->setColorMap(data().range(), colorMap());
if (!plot->axisEnabled(color_axis))
	plot->enableAxis(color_axis);
colorAxis->show();
plot->updateLayout();
}

int Spectrogram::colorBarWidth()
{
QwtPlot *plot = this->plot();
if (!plot)
	return 0;

QwtScaleWidget *colorAxis = plot->axisWidget(color_axis);
return colorAxis->colorBarWidth();
}

void Spectrogram::setColorBarWidth(int width)
{
QwtPlot *plot = this->plot();
if (!plot)
	return;

QwtScaleWidget *colorAxis = plot->axisWidget(color_axis);
colorAxis->setColorBarWidth(width);
}

Spectrogram* Spectrogram::copy()
{
Spectrogram *new_s = new Spectrogram(matrix());
new_s->setDisplayMode(QwtPlotSpectrogram::ImageMode, testDisplayMode(QwtPlotSpectrogram::ImageMode));
new_s->setDisplayMode(QwtPlotSpectrogram::ContourMode, testDisplayMode(QwtPlotSpectrogram::ContourMode));
new_s->setColorMap (colorMap());
new_s->setAxis(xAxis(), yAxis());
new_s->setDefaultContourPen(defaultContourPen());
new_s->setLevelsNumber(levels());
new_s->color_map_policy = color_map_policy;
return new_s;
}

void Spectrogram::setGrayScale()
{
color_map = QwtLinearColorMap(Qt::black, Qt::white);
setColorMap(color_map);
color_map_policy = GrayScale;

QwtPlot *plot = this->plot();
if (!plot)
	return;

QwtScaleWidget *colorAxis = plot->axisWidget(color_axis);
if (colorAxis)
	colorAxis->setColorMap(data().range(), colorMap());
}

void Spectrogram::setDefaultColorMap()
{
color_map = defaultColorMap();
setColorMap(color_map);
color_map_policy = Default;

QwtPlot *plot = this->plot();
if (!plot)
	return;

QwtScaleWidget *colorAxis = plot->axisWidget(color_axis);
if (colorAxis)
	colorAxis->setColorMap(this->data().range(), this->colorMap());
}

void Spectrogram::setCustomColorMap(const QwtLinearColorMap& map)
{
setColorMap(map);
color_map = map;
color_map_policy = Custom;

QwtPlot *plot = this->plot();
if (!plot)
	return;

QwtScaleWidget *colorAxis = plot->axisWidget(color_axis);
if (colorAxis)
	colorAxis->setColorMap(this->data().range(), this->colorMap());
}

QwtLinearColorMap Spectrogram::defaultColorMap()
{
QwtLinearColorMap colorMap(Qt::blue, Qt::red);
colorMap.addColorStop(0.25, Qt::cyan);
colorMap.addColorStop(0.5, Qt::green);
colorMap.addColorStop(0.75, Qt::yellow);
return colorMap;
}

QString Spectrogram::saveToString()
{
QString s = "<spectrogram>\n";
s += "\t<matrix>" + QString(d_matrix->name()) + "</matrix>\n";

if (color_map_policy != Custom)
	s += "\t<ColorPolicy>" + QString::number(color_map_policy) + "</ColorPolicy>\n";
else
	{
	s += "\t<ColorMap>\n";
	s += "\t\t<Mode>" + QString::number(color_map.mode()) + "</Mode>\n";
	s += "\t\t<MinColor>" + color_map.color1().name() + "</MinColor>\n";
	s += "\t\t<MaxColor>" + color_map.color2().name() + "</MaxColor>\n";
	QwtArray <double> colors = color_map.colorStops();
	int stops = (int)colors.size();
	s += "\t\t<ColorStops>" + QString::number(stops - 2) + "</ColorStops>\n";
	for (int i = 1; i < stops - 1; i++)
		{
		s += "\t\t<Stop>" + QString::number(colors[i]) + "\t";
		s += QColor(color_map.rgb(QwtDoubleInterval(0,1), colors[i])).name();
		s += "</Stop>\n";
		}
	s += "\t</ColorMap>\n";
	}
s += "\t<Image>"+QString::number(testDisplayMode(QwtPlotSpectrogram::ImageMode))+"</Image>\n";

bool contourLines = testDisplayMode(QwtPlotSpectrogram::ContourMode);
s += "\t<ContourLines>"+QString::number(contourLines)+"</ContourLines>\n";
if (contourLines)
	{
	s += "\t\t<Levels>"+QString::number(levels())+"</Levels>\n";
	bool defaultPen = defaultContourPen().style() != Qt::NoPen;
	s += "\t\t<DefaultPen>"+QString::number(defaultPen)+"</DefaultPen>\n";
	if (defaultPen)
		{
		s += "\t\t\t<PenColor>"+defaultContourPen().color().name()+"</PenColor>\n";
		s += "\t\t\t<PenWidth>"+QString::number(defaultContourPen().width())+"</PenWidth>\n";
		s += "\t\t\t<PenStyle>"+QString::number(defaultContourPen().style() - 1)+"</PenStyle>\n";
		}
	}
QwtScaleWidget *colorAxis = plot()->axisWidget(color_axis);
if (colorAxis && colorAxis->isColorBarEnabled())
	{
	s += "\t<ColorBar>\n\t\t<axis>" + QString::number(color_axis) + "</axis>\n";
	s += "\t\t<width>" + QString::number(colorAxis->colorBarWidth()) + "</width>\n";
	s += "\t</ColorBar>\n";
	}

return s+"</spectrogram>\n";
}

double MatrixData::value(double x, double y) const
{       
int i = abs((int)floor((y - y_start)/dy - 1));
int j = abs((int)floor((x - x_start)/dx));

if (d_m && i < n_rows && j < n_cols)
	return d_m[i][j];
else 
	return 0.0;
}


