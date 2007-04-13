/***************************************************************************
    File                 : PlotCurve.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Abstract 2D Plot Curve

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
#include "PlotCurve.h"
#include "ScaleDraw.h"
#include <QDateTime>
#include <QMessageBox>

PlotCurve::PlotCurve(const char *name):
    QwtPlotCurve(name),
	d_table(NULL),
	d_x_column(QString()),
	d_start_row(0),
	d_end_row(-1),
	d_type(0)
{
}

PlotCurve::PlotCurve(Table *t, const QString& xColName, const char *name, int startRow, int endRow):
    QwtPlotCurve(name),
	d_table(NULL),
	d_x_column(QString()),
	d_start_row(0),
	d_end_row(-1),
	d_type(0)
{
	if (t)
	{
		d_table = t;
		d_x_column = xColName;
		d_start_row = startRow;
		d_end_row = endRow;
		if (d_end_row < 0)
			d_end_row = t->tableRows();
	}
}

void PlotCurve::setRowRange(int startRow, int endRow)
{
	if (d_start_row == startRow && d_end_row == endRow)
		return;

	d_start_row = startRow;
	d_end_row = endRow;

	loadData();

	foreach(PlotCurve *c, d_error_bars)
		c->loadData();
}

void PlotCurve::setFullRange()
{
	d_start_row = 0;
	d_end_row = d_table->tableRows() - 1;

	loadData();

	foreach(PlotCurve *c, d_error_bars)
		c->loadData();
}

bool PlotCurve::isFullRange()
{
	if (d_start_row != 0 || d_end_row != d_table->tableRows() - 1)
		return false;
	else
		return true;
}

QString PlotCurve::plotAssociation()
{
    if (!d_x_column.isEmpty())
        return d_x_column + "(X)," + title().text() + "(Y)";
    else
        return title().text();
}

void PlotCurve::updateColumnNames(const QString& oldName, const QString& newName, bool updateTableName)
{
    if (updateTableName)
    {
        QString s = title().text();
        QStringList lst = s.split("_", QString::SkipEmptyParts);
        if (lst[0] == oldName)
            setTitle(newName + "_" + lst[1]);

        lst = d_x_column.split("_", QString::SkipEmptyParts);
        if (lst[0] == oldName)
            d_x_column = newName + "_" + lst[1];
    }
    else
    {
        if (title().text() == oldName)
            setTitle(newName);
        if (d_x_column == oldName)
            d_x_column = newName;
    }
}

void PlotCurve::updateData(Table *t, const QString& colName)
{
	if (d_table != t || (colName != title().text() && d_x_column != colName))
		return;

	loadData();
}

void PlotCurve::loadData()
{
	Graph *g = (Graph *)plot()->parent();
	if (!g)
		return;

	int xcol = d_table->colIndex(d_x_column);
	int ycol = d_table->colIndex(title().text());

	if (xcol < 0 || ycol < 0)
	{
		remove();
		return;
	}

	int r = abs(d_end_row - d_start_row) + 1;
    QVarLengthArray<double> X(r), Y(r);
	int xColType = d_table->columnType(xcol);
	int yColType = d_table->columnType(ycol);

	QStringList xLabels, yLabels;// store text labels

	QTime time0;
	QDate date;
	if (xColType == Table::Time)
	{
		for (int i = d_start_row; i <= d_end_row; i++ )
		{
			QString xval=d_table->text(i,xcol);
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
		for (int i = d_start_row; i <= d_end_row; i++ )
		{
			QString xval=d_table->text(i,xcol);
			if (!xval.isEmpty())
			{
				date = QDate::fromString (xval, Qt::ISODate);
				if (date.isValid())
					break;
			}
		}
	}

	int size = 0;
	for (int i = d_start_row; i <= d_end_row; i++ )
	{
		QString xval = d_table->text(i,xcol);
		QString yval = d_table->text(i,ycol);
		if (!xval.isEmpty() && !yval.isEmpty())
		{
			if (xColType == Table::Text)
			{
				xLabels << xval;
				X[size] = (double)(size + 1);
			}
			else if (xColType == Table::Time)
			{
				QTime time = QTime::fromString (xval, Qt::TextDate);
				if (time.isValid())
					X[size]= time0.msecsTo (time);
			}
			else if (xColType == Table::Date)
			{
				QDate d = QDate::fromString (xval, Qt::ISODate);
				if (d.isValid())
					X[size] = (double) date.daysTo(d);
			}
			else
				X[size] = xval.toDouble();

			if (yColType == Table::Text)
			{
				yLabels << yval;
				Y[size] = (double)(size + 1);
			}
			else
				Y[size] = yval.toDouble();

            size++;
		}
	}

    X.resize(size);
    Y.resize(size);

	if (!size)
	{
		remove();
		return;
	}
	else
	{//update curve data
		int curveType = g->curveType(g->curveIndex(this));
		if (curveType == Graph::HorizontalBars)
			setData(Y.data(), X.data(), size);
		else
			setData(X.data(), Y.data(), size);

		if (xColType == Table::Text)
		{
			if (curveType == Graph::HorizontalBars)
			{
				//axisType[QwtPlot::yLeft] = Txt;
				//axesFormatInfo[QwtPlot::yLeft] = xcName;
				plot()->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(xLabels));
			}
			else
			{
				//axisType[QwtPlot::xBottom] = Txt;
				//axesFormatInfo[QwtPlot::xBottom] = xcName;
				plot()->setAxisScaleDraw (QwtPlot::xBottom, new QwtTextScaleDraw(xLabels));
			}
		}
		else if (xColType == Table::Time )
		{
			if (curveType == Graph::HorizontalBars)
			{
				QStringList lst = g->axisFormatInfo(QwtPlot::yLeft).split(";");
				QString fmtInfo = time0.toString(Qt::TextDate) + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::yLeft, Graph::Time, fmtInfo);
			}
			else
			{
				QStringList lst = g->axisFormatInfo(QwtPlot::xBottom).split(";");
				QString fmtInfo = time0.toString(Qt::TextDate) + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::xBottom, Graph::Time, fmtInfo);
			}
		}
		else if (xColType == Table::Date )
		{
			if (curveType == Graph::HorizontalBars)
			{
				QStringList lst = g->axisFormatInfo(QwtPlot::yLeft).split(";");
				QString fmtInfo = date.toString(Qt::ISODate) + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::yLeft, Graph::Date, fmtInfo);
			}
			else
			{
				QStringList lst = g->axisFormatInfo(QwtPlot::xBottom).split(";");
				QString fmtInfo = date.toString(Qt::ISODate) + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::xBottom, Graph::Date, fmtInfo);
			}
		}

		if (yColType == Table::Text)
		{
			//axisType[QwtPlot::yLeft] = Txt;
			//axesFormatInfo[QwtPlot::yLeft] = ycName;
			plot()->setAxisScaleDraw (QwtPlot::yLeft, new QwtTextScaleDraw(yLabels));
		}
	}
}

void PlotCurve::removeErrorBars(PlotCurve *c)
{
	if (!c || d_error_bars.isEmpty())
		return;

	int index = d_error_bars.indexOf(c);
	if (index >= 0 && index < d_error_bars.size())
		d_error_bars.removeAt(index);
}

void PlotCurve::clearErrorBars()
{
	if (d_error_bars.isEmpty())
		return;

	foreach(PlotCurve *c, d_error_bars)
		c->remove();
}

void PlotCurve::remove()
{
	Graph *g = (Graph *)plot()->parent();
	if (!g)
		return;

	g->removeCurve(title().text());
}
