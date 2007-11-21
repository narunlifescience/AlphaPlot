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
#include <QIcon>

Column::Column(const QString& label, SciDAVis::ColumnMode mode)
 : AbstractAspect(label)
{
	d = shared_ptr<ColumnPrivate>(new ColumnPrivate(this, mode));
	init();
}

Column::Column(const QString& label, QVector<double> data, IntervalAttribute<bool> validity)
 : AbstractAspect(label)
{
	d = shared_ptr<ColumnPrivate>(new ColumnPrivate(this, SciDAVis::TypeDouble, 
		SciDAVis::Numeric, new QVector<double>(data), validity));
	init();
}

Column::Column(const QString& label, QStringList data, IntervalAttribute<bool> validity)
 : AbstractAspect(label)
{
	d = shared_ptr<ColumnPrivate>(new ColumnPrivate(this, SciDAVis::TypeQString,
		SciDAVis::Text, new QStringList(data), validity));
	init();
}

Column::Column(const QString& label, QList<QDateTime> data, IntervalAttribute<bool> validity)
 : AbstractAspect(label)
{
	d = shared_ptr<ColumnPrivate>(new ColumnPrivate(this, SciDAVis::TypeQDateTime, 
		SciDAVis::DateTime, new QList<QDateTime>(data), validity));
	init();
}

void Column::init()
{
	connect(abstractAspectSignalEmitter(), SIGNAL(aspectDescriptionAboutToChange(AbstractAspect *)),
		this, SLOT(emitDescriptionAboutToChange(AbstractAspect *)));
	connect(abstractAspectSignalEmitter(), SIGNAL(aspectDescriptionChanged(AbstractAspect *)),
		this, SLOT(emitDescriptionChanged(AbstractAspect *)));
}

void Column::emitDescriptionAboutToChange(AbstractAspect * aspect)
{
	if (aspect != static_cast<AbstractAspect *>(this)) return;
	emit abstractColumnSignalEmitter()->descriptionAboutToChange(this);
}

void Column::emitDescriptionChanged(AbstractAspect * aspect)
{
	if (aspect != static_cast<AbstractAspect *>(this)) return;
	emit abstractColumnSignalEmitter()->descriptionChanged(this);
}

Column::~Column()
{
}

void Column::setColumnMode(SciDAVis::ColumnMode mode)
{
	exec(new ColumnSetModeCmd(d, mode));
}


bool Column::copy(const AbstractColumn * other)
{
	if(other->dataType() != dataType()) return false;
	exec(new ColumnFullCopyCmd(d, other));
	return true;
}

bool Column::copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows)
{
	if(source->dataType() != dataType()) return false;
	exec(new ColumnPartialCopyCmd(d, source, source_start, dest_start, num_rows));
	return true;
}

void Column::insertRows(int before, int count)
{
	exec(new ColumnInsertEmptyRowsCmd(d, before, count));
}

void Column::removeRows(int first, int count)
{
	exec(new ColumnRemoveRowsCmd(d, first, count));
}

void Column::setPlotDesignation(SciDAVis::PlotDesignation pd)
{
	exec(new ColumnSetPlotDesignationCmd(d, pd));
}

void Column::clear()
{
	exec(new ColumnClearCmd(d));
}

void Column::notifyReplacement(shared_ptr<AbstractColumn> replacement)
{
	emit abstractColumnSignalEmitter()->aboutToBeReplaced(this, replacement); 
}

void Column::clearValidity()
{
	exec(new ColumnClearValidityCmd(d));
}

void Column::clearMasks()
{
	exec(new ColumnClearMasksCmd(d));
}

void Column::setInvalid(Interval<int> i, bool invalid)
{
	exec(new ColumnSetInvalidCmd(d, i, invalid));
}

void Column::setInvalid(int row, bool invalid)
{
	setInvalid(Interval<int>(row,row), invalid);
}

void Column::setMasked(Interval<int> i, bool mask)
{
	exec(new ColumnSetMaskedCmd(d, i, mask));
}

void Column::setMasked(int row, bool mask)
{
	setMasked(Interval<int>(row,row), mask);
}

void Column::setFormula(Interval<int> i, QString formula)
{
	exec(new ColumnSetFormulaCmd(d, i, formula));
}

void Column::setFormula(int row, QString formula)
{
	setFormula(Interval<int>(row, row), formula);
}

void Column::clearFormulas()
{
	exec(new ColumnClearFormulasCmd(d));
}

void Column::setTextAt(int row, QString new_value)
{
	exec(new ColumnSetTextCmd(d, row, new_value));
}

void Column::replaceTexts(int first, const QStringList& new_values)
{
	exec(new ColumnReplaceTextsCmd(d, first, new_values));
}

void Column::setDateAt(int row, QDate new_value)
{
	setDateTimeAt(row, QDateTime(new_value, timeAt(row)));
}

void Column::setTimeAt(int row, QTime new_value)
{
	setDateTimeAt(row, QDateTime(dateAt(row), new_value));
}

void Column::setDateTimeAt(int row, QDateTime new_value)
{
	exec(new ColumnSetDateTimeCmd(d, row, new_value));
}

void Column::replaceDateTimes(int first, const QList<QDateTime>& new_values)
{
	exec(new ColumnReplaceDateTimesCmd(d, first, new_values));
}

void Column::setValueAt(int row, double new_value)
{
	exec(new ColumnSetValueCmd(d, row, new_value));
}

void Column::replaceValues(int first, const QVector<double>& new_values)
{
	exec(new ColumnReplaceValuesCmd(d, first, new_values));
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

void Column::setColumnLabel(const QString& label) 
{ 
	setName(label); 
}

void Column::setColumnComment(const QString& comment) 
{ 
	setComment(comment); 
}

QIcon Column::icon() const
{
	switch(dataType())
	{
		case SciDAVis::TypeDouble:
			return QIcon(QPixmap(":/numerictype.png"));
		case SciDAVis::TypeQString:
			return QIcon(QPixmap(":/texttype.png"));
		case SciDAVis::TypeQDateTime:
			return QIcon(QPixmap(":/datetype.png"));
	}
	return QIcon();
}

