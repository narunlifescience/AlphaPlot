/***************************************************************************
    File                 : DataCurve.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : AbstractPlotCurve and DataCurve classes

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
#include <qwt_symbol.h>

DataCurve::DataCurve(Table *t, const QString& xColName, const char *name, int startRow, int endRow):
    PlotCurve(name),
	d_table(t),
	d_x_column(xColName),
	d_start_row(startRow),
	d_end_row(endRow)
{
	if (t && d_end_row < 0)
		d_end_row = t->numRows() - 1;
}

void DataCurve::setRowRange(int startRow, int endRow)
{
	if (d_start_row == startRow && d_end_row == endRow)
		return;

	d_start_row = startRow;
	d_end_row = endRow;

	loadData();

	foreach(DataCurve *c, d_error_bars)
		c->loadData();
}

void DataCurve::setFullRange()
{
	d_start_row = 0;
	d_end_row = d_table->numRows() - 1;

	loadData();

	foreach(DataCurve *c, d_error_bars)
		c->loadData();
}

bool DataCurve::isFullRange()
{
	if (d_start_row != 0 || d_end_row != d_table->numRows() - 1)
		return false;
	else
		return true;
}

QString DataCurve::plotAssociation()
{
    if (!d_x_column.isEmpty())
        return d_x_column + "(X)," + title().text() + "(Y)";
    else
        return title().text();
}

void DataCurve::updateColumnNames(const QString& oldName, const QString& newName, bool updateTableName)
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

bool DataCurve::updateData(Table *t, const QString& colName)
{
	if (d_table != t || (colName != title().text() && d_x_column != colName))
		return false;

	loadData();
	return true;
}

void DataCurve::loadData()
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
	QDate date0;
	QString date_time_fmt = d_table->columnFormat(xcol);
	if (xColType == Table::Time)
	{
		for (int i = d_start_row; i <= d_end_row; i++ )
		{
			QString xval=d_table->text(i,xcol);
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
		for (int i = d_start_row; i <= d_end_row; i++ )
		{
			QString xval=d_table->text(i,xcol);
			if (!xval.isEmpty())
			{
				date0 = QDate::fromString (xval, date_time_fmt);
				if (date0.isValid())
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
				QTime time = QTime::fromString (xval, date_time_fmt);
				if (time.isValid())
					X[size]= time0.msecsTo (time);
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
				Y[size] = (double)(size + 1);
			}
			else
				Y[size] = Table::stringToDouble(yval);

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
	{
		if (d_type == Graph::HorizontalBars)
		{
			setData(Y.data(), X.data(), size);
			foreach(DataCurve *c, d_error_bars)
                c->setData(Y.data(), X.data(), size);
		}
		else
		{
			setData(X.data(), Y.data(), size);
			foreach(DataCurve *c, d_error_bars)
                c->setData(X.data(), Y.data(), size);
		}

		if (xColType == Table::Text)
		{
			if (d_type == Graph::HorizontalBars)
				g->setLabelsTextFormat(QwtPlot::yLeft, Graph::Txt, d_x_column, xLabels);
			else
                g->setLabelsTextFormat(QwtPlot::xBottom, Graph::Txt, d_x_column, xLabels);
		}
		else if (xColType == Table::Time )
		{
			if (d_type == Graph::HorizontalBars)
			{
				QStringList lst = g->axisFormatInfo(QwtPlot::yLeft).split(";");
				QString fmtInfo = time0.toString() + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::yLeft, Graph::Time, fmtInfo);
			}
			else
			{
				QStringList lst = g->axisFormatInfo(QwtPlot::xBottom).split(";");
				QString fmtInfo = time0.toString() + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::xBottom, Graph::Time, fmtInfo);
			}
		}
		else if (xColType == Table::Date )
		{
			if (d_type == Graph::HorizontalBars)
			{
				QStringList lst = g->axisFormatInfo(QwtPlot::yLeft).split(";");
				QString fmtInfo = date0.toString(Qt::ISODate) + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::yLeft, Graph::Date, fmtInfo);
			}
			else
			{
				QStringList lst = g->axisFormatInfo(QwtPlot::xBottom).split(";");
				QString fmtInfo = date0.toString(Qt::ISODate) + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::xBottom, Graph::Date, fmtInfo);
			}
		}

		if (yColType == Table::Text)
            g->setLabelsTextFormat(QwtPlot::yLeft, Graph::Txt, title().text(), yLabels);
	}
}

void DataCurve::removeErrorBars(DataCurve *c)
{
	if (!c || d_error_bars.isEmpty())
		return;

	int index = d_error_bars.indexOf(c);
	if (index >= 0 && index < d_error_bars.size())
		d_error_bars.removeAt(index);
}

void DataCurve::clearErrorBars()
{
	if (d_error_bars.isEmpty())
		return;

	foreach(DataCurve *c, d_error_bars)
		c->remove();
}

void DataCurve::remove()
{
	Graph *g = (Graph *)plot()->parent();
	if (!g)
		return;

	g->removeCurve(title().text());
}

void DataCurve::setVisible(bool on)
{
	QwtPlotCurve::setVisible(on);
	foreach(DataCurve *c, d_error_bars)
		c->setVisible(on);
}

int DataCurve::tableRow(int point)
{
	int xcol = d_table->colIndex(d_x_column);
	int ycol = d_table->colIndex(title().text());

	if (xcol < 0 || ycol < 0)
		return -1;

	for (int i = d_start_row; i <= d_end_row; i++ )
	{
		if (d_table->cell(i, xcol) == x(point) && d_table->cell(i, ycol) == y(point))
			return i;
	}
	return -1;
}

QwtDoubleRect PlotCurve::boundingRect() const
{
    QwtDoubleRect r = QwtPlotCurve::boundingRect();
    int margin = 1;
    if (symbol().style() != QwtSymbol::NoSymbol)
        margin += symbol().size().width();

    const QwtScaleMap &xMap = plot()->canvasMap(xAxis());
	const QwtScaleMap &yMap = plot()->canvasMap(yAxis());

    int x_right = xMap.transform(r.right());
    double d_x_right = xMap.invTransform(x_right + margin);
    int x_left = xMap.transform(r.left());
    double d_x_left = xMap.invTransform(x_left - margin);

    int y_top = yMap.transform(r.top());
    double d_y_top = yMap.invTransform(y_top + margin);
    int y_bottom = yMap.transform(r.bottom());
    double d_y_bottom = yMap.invTransform(y_bottom - margin);

    return QwtDoubleRect(d_x_left, d_y_top, qAbs(d_x_right - d_x_left), qAbs(d_y_bottom - d_y_top));
}
