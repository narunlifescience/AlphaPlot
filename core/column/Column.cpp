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

Column::Column(const QString& label, SciDAVis::ColumnMode mode)
 : AbstractAspect(label)
{
	d = new ColumnPrivate(this, mode);
	connectPrivateObject();
}

Column::Column(const QString& label, QVector<double> data, IntervalAttribute<bool> validity)
 : AbstractAspect(label)
{
	d = new ColumnPrivate(this, SciDAVis::TypeDouble, SciDAVis::Numeric, new QVector<double>(data), validity);
	connectPrivateObject();
}

Column::Column(const QString& label, QStringList data, IntervalAttribute<bool> validity)
 : AbstractAspect(label)
{
	d = new ColumnPrivate(this, SciDAVis::TypeQString, SciDAVis::Text, new QStringList(data), validity);
	connectPrivateObject();
}

Column::Column(const QString& label, QList<QDateTime> data, IntervalAttribute<bool> validity)
 : AbstractAspect(label)
{
	d = new ColumnPrivate(this, SciDAVis::TypeQDateTime, SciDAVis::DateTime, new QList<QDateTime>(data), validity);
	connectPrivateObject();
}

void Column::connectPrivateObject()
{
	// map all signals 1:1
	connect(d, SIGNAL(plotDesignationAboutToChange(AbstractColumn *)), signalSender(), SIGNAL(plotDesignationAboutToChange(AbstractColumn *)) ); 
	connect(d, SIGNAL(plotDesignationChanged(AbstractColumn *)), signalSender(), SIGNAL(plotDesignationChanged(AbstractColumn *)) ); 
	connect(d, SIGNAL(modeAboutToChange(AbstractColumn *)), signalSender(), SIGNAL(modeAboutToChange(AbstractColumn *)) ); 
	connect(d, SIGNAL(modeChanged(AbstractColumn *)), signalSender(), SIGNAL(modeChanged(AbstractColumn *)) ); 
	connect(d, SIGNAL(dataAboutToChange(AbstractColumn *)), signalSender(), SIGNAL(dataAboutToChange(AbstractColumn *)) ); 
	connect(d, SIGNAL(dataChanged(AbstractColumn *)), signalSender(), SIGNAL(dataChanged(AbstractColumn *)) ); 
	connect(d, SIGNAL(aboutToBeReplaced(AbstractColumn *, AbstractColumn *)), signalSender(), SIGNAL(aboutToBeReplaced(AbstractColumn *, AbstractColumn *)) ); 
	connect(d, SIGNAL(rowsAboutToBeInserted(AbstractColumn *, int, int)), signalSender(), SIGNAL(rowsAboutToBeInserted(AbstractColumn *, int, int)) ); 
	connect(d, SIGNAL(rowsInserted(AbstractColumn *, int, int)), signalSender(), SIGNAL(rowsInserted(AbstractColumn *, int, int)) ); 
	connect(d, SIGNAL(rowsAboutToBeDeleted(AbstractColumn *, int, int)), signalSender(), SIGNAL(rowsAboutToBeDeleted(AbstractColumn *, int, int)) ); 
	connect(d, SIGNAL(rowsDeleted(AbstractColumn *, int, int)), signalSender(), SIGNAL(rowsDeleted(AbstractColumn *, int, int)) ); 
	connect(d, SIGNAL(maskingAboutToChange(AbstractColumn *)), signalSender(), SIGNAL(maskingAboutToChange(AbstractColumn *)) ); 
	connect(d, SIGNAL(maskingChanged(AbstractColumn *)), signalSender(), SIGNAL(maskingChanged(AbstractColumn *)) ); 
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
	if(other->dataType() != dataType()) return false;
	exec(new ColumnFullCopyCmd(this, other));
	return true;
}

bool Column::copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows)
{
	if(source->dataType() != dataType()) return false;
	exec(new ColumnPartialCopyCmd(this, source, source_start, dest_start, num_rows));
	return true;
}

void Column::insertEmptyRows(int before, int count)
{
	exec(new ColumnInsertRowsCmd(this, before, count));
}

void Column::removeRows(int first, int count)
{
	exec(new ColumnRemoveRowsCmd(this, first, count));
}

void Column::setPlotDesignation(SciDAVis::PlotDesignation pd)
{
	exec(new ColumnSetPlotDesignationCmd(this, pd));
}

void Column::clear()
{
	exec(new ColumnClearCmd(this));
}

void Column::notifyReplacement(AbstractColumn * replacement)
{
	emit d_sender->aboutToBeReplaced(this, replacement); 
}

void Column::clearValidity()
{
	exec(new ColumnClearValidityCmd(this));
}

void Column::clearMasks()
{
	exec(new ColumnClearMasksCmd(this));
}

void Column::setInvalid(Interval<int> i, bool invalid)
{
	exec(new ColumnSetInvalidCmd(this, i, invalid));
}

void Column::setInvalid(int row, bool invalid)
{
	setInvalid(Interval<int>(row,row), invalid);
}

void Column::setMasked(Interval<int> i, bool mask)
{
	exec(new ColumnSetMaskedCmd(this, i, mask));
}

void Column::setMasked(int row, bool mask)
{
	setMasked(Interval<int>(row,row), mask);
}

void Column::setFormula(Interval<int> i, QString formula)
{
	exec(new ColumnSetFormulaCmd(this, i, formula));
}

void Column::setFormula(int row, QString formula)
{
	setFormula(Interval<int>(row, row), formula);
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
	return d->textAt(row);
}

QDate Column::dateAt(int row) const
{
	return d->dateAt(row);
}

QTime Column::timeAt(int row) const
{
	return d->timeAt(row);
}

QDateTime Column::dateTimeAt(int row) const
{
	return d->dateTimeAt(row);
}

double Column::valueAt(int row) const
{
	return d->valueAt(row);
}

