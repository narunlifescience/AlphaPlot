/***************************************************************************
    File                 : Column.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Table column class

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

#include "Column.h"
#include "ColumnPrivate.h"
#include "columncommands.h"

Column::Column()
{
	createPrivateObject();
}

Column::Column(QVector<double> data, IntervalAttribute<bool> validity)
{
	createPrivateObject();
	d->d_data_type = SciDAVis::TypeDouble;
	d->d_column_mode = SciDAVis::Numeric;
	d->d_data = new QVector<double>();
	*(static_cast< QVector<double>* >(d->d_data)) = data;
	d->d_validity = validity;
}

Column::Column(QStringList data, IntervalAttribute<bool> validity)
{
	createPrivateObject();
	d->d_data_type = SciDAVis::TypeQString;
	d->d_column_mode = SciDAVis::Text;
	d->d_data = new StringList();
	*(static_cast< QStringList* >(d->d_data)) = data;
	d->d_validity = validity;
}

Column::Column(QList<QDateTime> data, IntervalAttribute<bool> validity)
{
	createPrivateObject();
	d->d_data_type = SciDAVis::TypeQDateTime;
	d->d_column_mode = SciDAVis::DateTime;
	d->d_data = new QList<QDateTime>();
	*(static_cast< QList<QDateTime>* >(d->d_data)) = data;
	d->d_validity = validity;
}

Column::createPrivateObject()
{
	d = new ColumnPrivate();
	// map all signals 1:1
	connect(d, SIGNAL(plotDesignationAboutToChange(AbstractColumn *)), this, SIGNAL(plotDesignationAboutToChange(AbstractColumn *)) ); 
	connect(d, SIGNAL(plotDesignationChanged(AbstractColumn *)), this, SIGNAL(plotDesignationChanged(AbstractColumn *)) ); 
	connect(d, SIGNAL(modeAboutToChange(AbstractColumn *)), this, SIGNAL(modeAboutToChange(AbstractColumn *)) ); 
	connect(d, SIGNAL(modeChanged(AbstractColumn *)), this, SIGNAL(modeChanged(AbstractColumn *)) ); 
	connect(d, SIGNAL(dataAboutToChange(AbstractColumn *)), this, SIGNAL(dataAboutToChange(AbstractColumn *)) ); 
	connect(d, SIGNAL(dataChanged(AbstractColumn *)), this, SIGNAL(dataChanged(AbstractColumn *)) ); 
	connect(d, SIGNAL(aboutToBeReplaced(AbstractColumn *, AbstractColumn *)), this, SIGNAL(aboutToBeReplaced(AbstractColumn *, AbstractColumn *)) ); 
	connect(d, SIGNAL(rowsAboutToBeInserted(AbstractColumn *, int, int)), this, SIGNAL(rowsAboutToBeInserted(AbstractColumn *, int, int)) ); 
	connect(d, SIGNAL(rowsInserted(AbstractColumn *, int, int)), this, SIGNAL(rowsInserted(AbstractColumn *, int, int)) ); 
	connect(d, SIGNAL(rowsAboutToBeDeleted(AbstractColumn *, int, int)), this, SIGNAL(rowsAboutToBeDeleted(AbstractColumn *, int, int)) ); 
	connect(d, SIGNAL(rowsDeleted(AbstractColumn *, int, int)), this, SIGNAL(rowsDeleted(AbstractColumn *, int, int)) ); 
	connect(d, SIGNAL(maskingAboutToChange(AbstractColumn *)), this, SIGNAL(maskingAboutToChange(AbstractColumn *)) ); 
	connect(d, SIGNAL(maskingChanged(AbstractColumn *)), this, SIGNAL(maskingChanged(AbstractColumn *)) ); 
}

Column::~Column()
{
	delete d;
}

void Column::setColumnMode(SciDAVis::ColumnMode mode)
{
	exec(new ColumnSetModeCmd(this, mode));
}


bool Column::copy(const AbstractColumn * other)
{
 // TODO
}

bool Column::copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows)
{
 // TODO
}

void Column::expand(int new_rows)
{
 // TODO
}

void Column::insertEmptyRows(int before, int count)
{
 // TODO
}

void Column::removeRows(int first, int count)
{
 // TODO
}

void Column::setPlotDesignation(SciDAVis::PlotDesignation pd)
{
 // TODO
}

void Column::clear()
{
 // TODO
}

void Column::notifyReplacement(AbstractColumn * replacement)
{
	emit aboutToBeReplaced(this, replacement); 
}

void Column::clearValidity()
{
 // TODO
}

void Column::clearMasks()
{
 // TODO
}

void Column::setInvalid(Interval<int> i, bool invalid = true)
{
 // TODO
}

void Column::setInvalid(int row, bool invalid = true)
{
 // TODO
}

void Column::setMasked(Interval<int> i, bool mask = true)
{
 // TODO
}

void Column::setMasked(int row, bool mask = true)
{
 // TODO
}

void Column::setFormula(Interval<int> i, QString formula)
{
 // TODO
}

void Column::setFormula(int row, QString formula)
{
 // TODO
}

void Column::clearFormulas()
{
 // TODO
}

void Column::setTextAt(int row, QString new_value)
{
 // TODO
}

void Column::replaceTexts(int first, QStringList new_values)
{
 // TODO
}

void Column::setDateAt(int row, QDate new_value)
{
 // TODO
}

void Column::setTimeAt(int row, QTime new_value)
{
 // TODO
}

void Column::setDateTimeAt(int row, QDateTime new_value)
{
 // TODO
}

void Column::replaceDateTimes(int first, QList<QDateTime> new_values)
{
 // TODO
}

void Column::setValueAt(int row, double new_value)
{
 // TODO
}

void Column::replaceValues(int first, int num_rows, const double * new_values)
{
 // TODO
}

QString Column::textAt(int row) const
{
	if(data_type != SciDAVis::TypeQString) return QString();
	return static_cast< QStringList* >(D_PTR->data)->value(row);
}

QDate Column::dateAt(int row) const
{
	return dateTimeAt(row).date();
}

QTime Column::timeAt(int row) const
{
	return dateTimeAt(row).time();
}

QDateTime Column::dateTimeAt(int row) const
{
	if(data_type != SciDAVis::TypeQDateTime) return QDateTime();
	return static_cast< QList<QDateTime>* >(D_PTR->data)->value(row);
}

double Column::valueAt(int row) const
{
	if(data_type != SciDAVis::TypeDouble) return 0.0;
	return static_cast< QVector<double>* >(D_PTR->data)->value(row);
}

