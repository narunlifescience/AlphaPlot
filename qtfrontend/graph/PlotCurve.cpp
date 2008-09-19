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
#include "Layer.h"
#include "table/Table.h"

#include <QDateTime>
#include <QMessageBox>
#include <QVarLengthArray>
#include <qwt_symbol.h>

DataCurve::DataCurve(Table *t, const QString& xColName, const char *name, int startRow, int endRow):
    PlotCurve(name),
	m_table(t),
	m_x_column(xColName),
	m_start_row(startRow),
	m_end_row(endRow)
{
	if (t && m_end_row < 0)
		m_end_row = t->rowCount() - 1;
}

void DataCurve::setRowRange(int startRow, int endRow)
{
	if (m_start_row == startRow && m_end_row == endRow)
		return;

	m_start_row = startRow;
	m_end_row = endRow;

	loadData();

	foreach(DataCurve *c, m_error_bars)
		c->loadData();
}

void DataCurve::setFullRange()
{
	m_start_row = 0;
	m_end_row = m_table->rowCount() - 1;

	loadData();

	foreach(DataCurve *c, m_error_bars)
		c->loadData();
}

bool DataCurve::isFullRange()
{
	if (m_start_row != 0 || m_end_row != m_table->rowCount() - 1)
		return false;
	else
		return true;
}

QString DataCurve::plotAssociation()
{
    if (!m_x_column.isEmpty())
        return m_x_column + "(X)," + title().text() + "(Y)";
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

        lst = m_x_column.split("_", QString::SkipEmptyParts);
        if (lst[0] == oldName)
            m_x_column = newName + "_" + lst[1];
    }
    else
    {
        if (title().text() == oldName)
            setTitle(newName);
        if (m_x_column == oldName)
            m_x_column = newName;
    }
}

bool DataCurve::updateData(Table *t, const QString& colName)
{
	if (m_table != t || (colName != title().text() && m_x_column != colName))
		return false;

	loadData();
	return true;
}

void DataCurve::loadData()
{
	Layer *g = (Layer *)plot()->parent();
	if (!g)
		return;

	int xcol = m_table->colIndex(m_x_column);
	int ycol = m_table->colIndex(title().text());

	if (xcol < 0 || ycol < 0){
		remove();
		return;
	}

	int r = abs(m_end_row - m_start_row) + 1;
    QVarLengthArray<double> X(r), Y(r);
	int xColType = m_table->columnType(xcol);
	int yColType = m_table->columnType(ycol);

	QStringList xLabels, yLabels;// store text labels

	QTime time0;
	QDate date0;
	QString date_time_fmt = m_table->columnFormat(xcol);
	if (xColType == SciDAVis::Time){
		for (int i = m_start_row; i <= m_end_row; i++ ){
			QString xval=m_table->text(i,xcol);
			if (!xval.isEmpty()){
				time0 = QTime::fromString (xval, date_time_fmt);
				if (time0.isValid())
					break;
			}
		}
	} else if (xColType == SciDAVis::Date){
		for (int i = m_start_row; i <= m_end_row; i++ ){
			QString xval=m_table->text(i,xcol);
			if (!xval.isEmpty()){
				date0 = QDate::fromString (xval, date_time_fmt);
				if (date0.isValid())
					break;
			}
		}
	}

	int size = 0;
	for (int i = m_start_row; i <= m_end_row; i++ ){
		QString xval = m_table->text(i,xcol);
		QString yval = m_table->text(i,ycol);
		if (!xval.isEmpty() && !yval.isEmpty()){
		    bool valid_data = true;
			if (xColType == SciDAVis::Text){
				xLabels << xval;
				X[size] = (double)(size + 1);
			} else if (xColType == SciDAVis::Time){
				QTime time = QTime::fromString (xval, date_time_fmt);
				if (time.isValid())
					X[size]= time0.msecsTo (time);
			} else if (xColType == SciDAVis::Date){
				QDate d = QDate::fromString (xval, date_time_fmt);
				if (d.isValid())
					X[size] = (double) date0.daysTo(d);
			} else
				X[size] = QLocale().toDouble(xval, &valid_data);

			if (yColType == SciDAVis::Text){
				yLabels << yval;
				Y[size] = (double)(size + 1);
			} else
				Y[size] = QLocale().toDouble(yval, &valid_data);

            if (valid_data)
                size++;
		}
	}

    X.resize(size);
    Y.resize(size);

	if (!size){
		remove();
		return;
	} else {
		if (m_type == Layer::HorizontalBars){
			setData(Y.data(), X.data(), size);
			foreach(DataCurve *c, m_error_bars)
                c->setData(Y.data(), X.data(), size);
		} else {
			setData(X.data(), Y.data(), size);
			foreach(DataCurve *c, m_error_bars)
                c->setData(X.data(), Y.data(), size);
		}

		if (xColType == SciDAVis::Text){
			if (m_type == Layer::HorizontalBars)
				g->setLabelsTextFormat(QwtPlot::yLeft, Layer::Txt, m_x_column, xLabels);
			else
                g->setLabelsTextFormat(QwtPlot::xBottom, Layer::Txt, m_x_column, xLabels);
		} else if (xColType == SciDAVis::Time ){
			if (m_type == Layer::HorizontalBars){
				QStringList lst = g->axisFormatInfo(QwtPlot::yLeft).split(";");
				QString fmtInfo = time0.toString() + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::yLeft, Layer::Time, fmtInfo);
			} else {
				QStringList lst = g->axisFormatInfo(QwtPlot::xBottom).split(";");
				QString fmtInfo = time0.toString() + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::xBottom, Layer::Time, fmtInfo);
			}
		} else if (xColType == SciDAVis::Date ) {
			if (m_type == Layer::HorizontalBars){
				QStringList lst = g->axisFormatInfo(QwtPlot::yLeft).split(";");
				QString fmtInfo = date0.toString(Qt::ISODate) + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::yLeft, Layer::Date, fmtInfo);
			} else {
				QStringList lst = g->axisFormatInfo(QwtPlot::xBottom).split(";");
				QString fmtInfo = date0.toString(Qt::ISODate) + ";" + lst[1];
				g->setLabelsDateTimeFormat(QwtPlot::xBottom, Layer::Date, fmtInfo);
			}
		}

		if (yColType == SciDAVis::Text)
            g->setLabelsTextFormat(QwtPlot::yLeft, Layer::Txt, title().text(), yLabels);
	}
}

void DataCurve::removeErrorBars(DataCurve *c)
{
	if (!c || m_error_bars.isEmpty())
		return;

	int index = m_error_bars.indexOf(c);
	if (index >= 0 && index < m_error_bars.size())
		m_error_bars.removeAt(index);
}

void DataCurve::clearErrorBars()
{
	if (m_error_bars.isEmpty())
		return;

	foreach(DataCurve *c, m_error_bars)
		c->remove();
}

void DataCurve::remove()
{
	Layer *g = (Layer *)plot()->parent();
	if (!g)
		return;

	g->removeCurve(title().text());
}

void DataCurve::setVisible(bool on)
{
	QwtPlotCurve::setVisible(on);
	foreach(DataCurve *c, m_error_bars)
		c->setVisible(on);
}

int DataCurve::tableRow(int point)
{
    if (!m_table)
        return -1;

	int xcol = m_table->colIndex(m_x_column);
	int ycol = m_table->colIndex(title().text());

	if (xcol < 0 || ycol < 0)
		return -1;

    int xColType = m_table->columnType(xcol);
    if (xColType == SciDAVis::Date){
        QString format = m_table->columnFormat(xcol);
        QDate date0 = QDate::fromString (m_table->text(m_start_row, xcol), format);
        for (int i = m_start_row; i <= m_end_row; i++ ){
            QDate d = QDate::fromString (m_table->text(i, xcol), format);
            if (d.isValid()){
                if (m_type == Layer::HorizontalBars && date0.daysTo(d) == y(point) && m_table->cell(i, ycol) == x(point))
                    return i;
                else if (date0.daysTo(d) == x(point) && m_table->cell(i, ycol) == y(point))
                    return i;
            }
        }
    } else if (xColType == SciDAVis::Time){
        QString format = m_table->columnFormat(xcol);
        QTime t0 = QTime::fromString (m_table->text(m_start_row, xcol), format);
        for (int i = m_start_row; i <= m_end_row; i++ ){
            QTime t = QTime::fromString (m_table->text(i, xcol), format);
            if (t.isValid()){
                if (m_type == Layer::HorizontalBars && t0.msecsTo(t) == y(point) && m_table->cell(i, ycol) == x(point))
                    return i;
                if (t0.msecsTo(t) == x(point) && m_table->cell(i, ycol) == y(point))
                    return i;
            }
        }
    }

	double x_val = x(point);
	double y_val = y(point);
	for (int i = m_start_row; i <= m_end_row; i++ ){
		if (m_table->cell(i, xcol) == x_val && m_table->cell(i, ycol) == y_val)
			return i;
	}
	return -1;
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
    double m_x_right = xMap.invTransform(x_right + margin);
    int x_left = xMap.transform(r.left());
    double m_x_left = xMap.invTransform(x_left - margin);

    int y_top = yMap.transform(r.top());
    double m_y_top = yMap.invTransform(y_top + margin);
    int y_bottom = yMap.transform(r.bottom());
    double m_y_bottom = yMap.invTransform(y_bottom - margin);

    return QwtDoubleRect(m_x_left, m_y_top, qAbs(m_x_right - m_x_left), qAbs(m_y_bottom - m_y_top));
}
