/***************************************************************************
    File                 : AbstractDataSource.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Reading interface for column based data

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

#ifndef ABSTRACTDATASOURCE_H
#define ABSTRACTDATASOURCE_H

#include <QObject>
#include <QtAlgorithms>
#include <QList>
class QString;
#include "lib/Interval.h"
#include "core/globals.h"

//! Reading interface for column-based data
/**
  This is the abstract base class for column-based data, 
  i.e. mathematically a vector or technically a 1D array or list.
  It only defines the reading interface and has no data members itself. 
  The writing interface is defined in AbstractColumnData and
  classes derived from it.

  An object of a class derived from this is used as a column 
  in a table or as a data source for plots, filters and fit functions.
  This class defines all non-specific read functions and signals that
  indicate a data change. Classes derived from this will either store a 
  vector with entries of one certain data type, e.g. double, QString, 
  QDateTime, or generate such values on demand. To determine the data
  type of a class derived from this, use qobject_cast or QObject::inherits().

  Any class whose output values are subject to change over time must emit
  the according signals. These signals notify any object working with the
  data source before and after a change of the data source.
  In some cases it will be necessary for a class using 
  data sources to connect QObject::destroyed() also, to react 
  to a column's deletion, e.g. a filter's reaction to a 
  table deletion.
  */
class AbstractDataSource : public QObject
{
	Q_OBJECT

public:
	//! Ctor
	AbstractDataSource() {}
	//! Dtor
	virtual ~AbstractDataSource() { emit aboutToBeDestroyed(this); }

	//! Return the data vector size
	/**
	 * \sa AbstractColumnData::expand()
	 */ 
	virtual int rowCount() const = 0;
	//! Return the column label
	virtual QString label() const = 0;
	//! Return the column comment
	virtual QString comment() const { return QString(); }
	//! Return the column plot designation
	virtual SciDAVis::PlotDesignation plotDesignation() const = 0;
	
	//! \name IntervalAttribute related functions
	//@{
	//! Return whether a certain row contains an invalid value 	 
	virtual bool isInvalid(int row) const { return !Interval<int>(0, rowCount()-1).contains(row); } 	 
	//! Return whether a certain interval of rows contains only invalid values 	 
	virtual bool isInvalid(Interval<int> i) const { return !Interval<int>(0, rowCount()-1).contains(i); } 	 
	//! Return all intervals of invalid rows
	virtual QList< Interval<int> > invalidIntervals() const { return QList< Interval<int> >(); } 	 
	//! Return whether a certain row is selected 	 
	virtual bool isSelected(int row) const { Q_UNUSED(row); return false; } 	 
	//! Return whether a certain interval of rows is fully selected
	virtual bool isSelected(Interval<int> i) const { Q_UNUSED(i); return false; } 	 
	//! Return all selected intervals 	 
	virtual QList< Interval<int> > selectedIntervals() const { return QList< Interval<int> >(); } 	 
	//! Return whether a certain row is masked 	 
	virtual bool isMasked(int row) const { Q_UNUSED(row); return false; } 	 
	//! Return whether a certain interval of rows rows is fully masked 	 
	virtual bool isMasked(Interval<int> i) const { Q_UNUSED(i); return false; }
	//! Return all intervals of masked rows
	virtual QList< Interval<int> > maskedIntervals() const { return QList< Interval<int> >(); } 	 
	//@}

	//! \name Formula related functions
	//@{
	//! Return the formula associated with row 'row' 	 
	virtual QString formula(int row) const { Q_UNUSED(row); return QString(); }
	//! Return the intervals that have associated formulas
	/**
	 * This can be used to make a list of formulas with their intervals.
	 * Here is some example code:
	 *
	 * \code
	 * QStringList list;
	 * QList< Interval<int> > intervals = my_column.formulaIntervals();
	 * foreach(Interval<int> interval, intervals)
	 * 	list << QString(interval.toString() + ": " + my_column.formula(interval.start()));
	 * \endcode
	 */
	virtual QList< Interval<int> > formulaIntervals() const { return QList< Interval<int> >(); }
	//@}
signals: 
	//! Column label and/or comment will be changed
	/**
	 * 'source' is always the this pointer of the column that
	 * emitted this signal. This way it's easier to use
	 * one handler for lots of columns.
	 */
	void descriptionAboutToChange(AbstractDataSource * source); 
	//! Column label and/or comment changed
	/**
	 * 'source' is always the this pointer of the column that
	 * emitted this signal. This way it's easier to use
	 * one handler for lots of columns.
	 */
	void descriptionChanged(AbstractDataSource * source); 
	//! Column plot designation will be changed
	/**
	 * 'source' is always the this pointer of the column that
	 * emitted this signal. This way it's easier to use
	 * one handler for lots of columns.
	 */
	void plotDesignationAboutToChange(AbstractDataSource * source); 
	//! Column plot designation changed
	/**
	 * 'source' is always the this pointer of the column that
	 * emitted this signal. This way it's easier to use
	 * one handler for lots of columns.
	 */
	void plotDesignationChanged(AbstractDataSource * source); 
	//! Data (including validity) of the column will be changed
	/**
	 * 'source' is always the this pointer of the column that
	 * emitted this signal. This way it's easier to use
	 * one handler for lots of columns.
	 */
	void dataAboutToChange(AbstractDataSource * source); 
	//! Data (including validity) of the column has changed
	/**
	 * Important: When data has changed also the number
	 * of rows in the column may have increased without
	 * any other signal emission.
	 * 'source' is always the this pointer of the column that
	 * emitted this signal. This way it's easier to use
	 * one handler for lots of columns.
	 * \sa AbstractColumnData::expand()
	 */
	void dataChanged(AbstractDataSource * source); 
	//! The column will be replaced
	/**
	 * This is used then a column is replaced by another
	 * column, possibly of another type. This is
	 * necessary because changing a column's type
	 * cannot be done without changing the pointer
	 * to the data source.
	 *
	 * \param new_col Pointer to the column this one is to be replaced with.
	 *
	 * \param source is always a pointer to the column that
	 * emitted this signal. This way it's easier to use
	 * one handler for lots of columns.
	 */
	void aboutToBeReplaced(AbstractDataSource * source, AbstractDataSource * new_col); 
	//! Rows will be inserted
	/**
	 *	\param source the column that emitted the signal
	 *	\param before the row to insert before
	 *	\param count the number of rows to be inserted
	 */
	void rowsAboutToBeInserted(AbstractDataSource * source, int before, int count); 
	//! Rows have been inserted
	/**
	 *	\param source the column that emitted the signal
	 *	\param before the row to insert before
	 *	\param count the number of rows to be inserted
	 */
	void rowsInserted(AbstractDataSource * source, int before, int count); 
	//! Rows will be deleted
	/**
	 *	\param source the column that emitted the signal
	 *	\param first the first row to be deleted
	 *	\param count the number of rows to be deleted
	 */
	void rowsAboutToBeDeleted(AbstractDataSource * source, int first, int count); 
	//! Rows have been deleted
	/**
	 *	\param source the column that emitted the signal
	 *	\param first the first row that was deleted
	 *	\param count the number of deleted rows
	 */
	void rowsDeleted(AbstractDataSource * source, int first, int count); 
	//! IntervalAttribute related signal
	void selectionAboutToChange(AbstractDataSource * source); 
	//! IntervalAttribute related signal
	void selectionChanged(AbstractDataSource * source); 
	//! IntervalAttribute related signal
	void maskingAboutToChange(AbstractDataSource * source); 
	//! IntervalAttribute related signal
	void maskingChanged(AbstractDataSource * source); 
	//! Emitted shortly before this data source is deleted.
	/**
	 * \param source the object emitting this signal
	 *
	 * This is need by AbstractFilter. QObject::destroyed() does not work there because its argument
	 * can't be cast to AbstractDataSource properly (qobject_cast and lookup of the pointer in
	 * AbstractFilter::d_inputs fail).
	 */
	void aboutToBeDestroyed(AbstractDataSource * source);

};

#endif
