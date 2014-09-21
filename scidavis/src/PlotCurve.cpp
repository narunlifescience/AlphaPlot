/***************************************************************************
    File                 : DataCurve.cpp
    Project              : SciDAVis
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
#include "core/column/Column.h"
#include "core/datatypes/DateTime2StringFilter.h"
#include <QDateTime>
#include <QMessageBox>
#include <qwt_symbol.h>

DataCurve::DataCurve(Table *t, const QString& xColName, const QString &name, int startRow, int endRow):
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
        if (lst.size()>0 && lst[0] == oldName)
            setTitle(newName + "_" + lst[1]);

        lst = d_x_column.split("_", QString::SkipEmptyParts);
        if (lst.size()>0 && lst[0] == oldName)
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

QList< QVector<double> > DataCurve::convertData(const QList<Column*> &cols, const QList<int> &axes) const {
	Graph *g = 0;
	if (plot())
		g = static_cast<Graph*>(plot()->parent());

	int end_row = d_end_row;
	// make sure end_row is a valid index for all columns
	foreach(Column *col, cols)
		if (end_row >= col->rowCount())
			end_row = col->rowCount() - 1;

	// determine rows for which all columns have valid content
	QList<int> valid_rows;
	for (int row = d_start_row; row <= end_row; row++) {
		bool all_valid = true;
		foreach(Column *col, cols)
			if (col->isInvalid(row)) {
				all_valid = false;
				break;
			}
		if (all_valid)
			valid_rows.push_back(row);
	}

	// initialize result list
	QList< QVector<double> > result;
	for (int i=0; i<cols.size(); i++)
		result.push_back(QVector<double>(valid_rows.size()));

	// For date and time (but not DateTime) values, numbers are relative to the first valid row's
	// content. We can't change this easily without breaking backwards compatibility (think of
	// scale ranges, placement of labels and arrows etc).
	QList<QDate> reference_dates;
	QList<QTime> reference_times;
	for(int i=0; i<cols.size(); i++) {
		Column *col = cols[i];

		switch (col->columnMode()) {
			case Table::Time:
				{
					QTime time;
					QString format;
					if (g && g->axesType()[axes[i]] == Table::Time) {
						QStringList lst = g->axisFormatInfo(axes[i]).split(";");
						time = QTime::fromString(lst[0]);
						if (lst.size() >= 2) format = lst[1];
					}
					if (!time.isValid()) {
						foreach (int row, valid_rows) {
							time = col->timeAt(row);
							if (time.isValid()) break;
						}
					}

					if (format.isEmpty())
						format = static_cast<DateTime2StringFilter *>(col->outputFilter())->format();

					reference_dates.push_back(QDate());
					reference_times.push_back(time);
					if (g)
						g->setLabelsDateTimeFormat(axes[i], Graph::Time, time.toString() + ";" + format);
					break;
				}
			case Table::Date:
				{
					QDate date;
					QString format;

					if (g && g->axesType()[axes[i]] == Table::Time) {
						QStringList lst = g->axisFormatInfo(axes[i]).split(";");
						date = QDate::fromString(lst[0], "YYYY-MM-DD");
						if (lst.size() >= 2) format = lst[1];
					}

					if (!date.isValid()) {
						foreach (int row, valid_rows) {
							date = col->dateAt(row);
							if (date.isValid()) break;
						}
					}

					if (format.isEmpty())
						format = static_cast<DateTime2StringFilter *>(col->outputFilter())->format();

					reference_dates.push_back(date);
					reference_times.push_back(QTime());
					if (g)
						g->setLabelsDateTimeFormat(axes[i], Graph::Date, date.toString("YYYY-MM-DD") + ";" + format);
					break;
				}
			case Table::DateTime:
				{
					QDateTime datetime;
					QString format;

					if (g && g->axesType()[axes[i]] == Table::DateTime) {
						QStringList lst = g->axisFormatInfo(axes[i]).split(";");
						datetime = QDateTime::fromString(lst[0], "YYYY-MM-DDTHH:MM:SS");
						if (lst.size() >= 2) format = lst[1];
					}

					if (!datetime.isValid()) {
						foreach (int row, valid_rows) {
							datetime = col->dateTimeAt(row);
							if (datetime.isValid()) break;
						}
					}

					if (format.isEmpty())
						format = static_cast<DateTime2StringFilter *>(col->outputFilter())->format();

					reference_dates.push_back(QDate());
					reference_times.push_back(QTime());
					if (g)
						g->setLabelsDateTimeFormat(axes[i], Graph::DateTime, datetime.toString("YYYY-MM-DDTHH:MM:SS") + ";" + format);
					break;
				}
			case Table::Text:
				if (g)
					g->setLabelsTextFormat(axes[i], col, d_start_row, end_row);
				reference_dates.push_back(QDate());
				reference_times.push_back(QTime());
				break;
			default:
				reference_dates.push_back(QDate());
				reference_times.push_back(QTime());
				break;
		};
	}

	// convert data to numeric representation used for plotting
	for (int i=0; i<valid_rows.size(); i++)
		for (int j=0; j<cols.size(); j++)
			switch (cols[j]->columnMode()) {
				case Table::Text:
					result[j][i] = static_cast<double>(valid_rows[i] + 1);
					break;
				case Table::Time:
					result[j][i] = reference_times[j].msecsTo(cols[j]->timeAt(valid_rows[i]));
					break;
				case Table::Date:
					result[j][i] = reference_dates[j].daysTo(cols[j]->dateAt(valid_rows[i]));
					break;
				case Table::DateTime:
					{
						QDateTime dt = cols[j]->dateTimeAt(valid_rows[i]);
						result[j][i] = double(dt.date().toJulianDay()) +
							double( -dt.time().msecsTo(QTime(12,0,0,0)) ) / 86400000.0;
						break;
					}
				default:
					result[j][i] = cols[j]->valueAt(valid_rows[i]);
					break;
			};

	d_index_to_row = QVector<int>::fromList(valid_rows);
	return result;
}

bool DataCurve::loadData()
{
	Column *x_col_ptr = d_table->column(d_x_column);
	Column *y_col_ptr = d_table->column(title().text());
	if (!x_col_ptr || !y_col_ptr) {
		remove();
		return false;
	}
	
	QList< QVector<double> > points = convertData(
			d_type == Graph::HorizontalBars ? (QList<Column*>() << y_col_ptr << x_col_ptr) : (QList<Column*>() << x_col_ptr << y_col_ptr),
			QList<int>() << xAxis() << yAxis());

	if (points.isEmpty() || points[0].size() == 0) {
		remove();
		return false;
	}

	setData(points[0].data(), points[1].data(), points[0].size());
	foreach(DataCurve *c, d_error_bars)
		c->setData(points[0].data(), points[1].data(), points[0].size());

	return true;
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
	if (!plot()) return;
	Graph *g = (Graph *)plot()->parent();
	if (!g) return;

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
	return d_index_to_row.value(point, -1);
}

QwtDoubleRect PlotCurve::boundingRect() const
{
    QwtDoubleRect r = QwtPlotCurve::boundingRect();
    if (symbol().style() == QwtSymbol::NoSymbol)
        return r;

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

