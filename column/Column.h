/***************************************************************************
    File                 : Column.h
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


#ifndef COLUMN_H
#define COLUMN_H

#include "AbstractColumn.h"
#include "core/AbstractAspect.h"
class ColumnPrivate;
class ColumnSetModeCmd;

//! Table column class
/**
  This class represents a column in a table. It has a public reading and
  (undo aware) writing interface defined in AbstractColumn and a private
  interface, which is only to be used by commands, defined
  by ColumnPrivate and accessed by the d pointer. All private data and
  functions members except utility functions are defined in ColumnPrivate.
  All commands working on a Column must be declared as friend classes.
 */
class Column : public AbstractColumn, public AbstractAspect
{
	Q_OBJECT

// TODO: integrate input/output filters

	public:
		//! Ctor
		Column();
		//! Ctor
		/**
		 * \param data initial data vector
		 * \param validity a list of invalid intervals (optional)
		 */
		Column(QVector<double> data, IntervalAttribute<bool> validity = IntervalAttribute<bool>());
		//! Ctor
		/**
		 * \param data initial data vector
		 * \param validity a list of invalid intervals (optional)
		 */
		Column(QStringList data, IntervalAttribute<bool> validity = IntervalAttribute<bool>()); 
		//! Ctor
		/**
		 * \param data initial data vector
		 * \param validity a list of invalid intervals (optional)
		 */
		Column(QList<QDateTime> data, IntervalAttribute<bool> validity = IntervalAttribute<bool>());
		//! Dtor
		virtual ~Column();

		//! Return the data type of the column
		virtual SciDAVis::ColumnDataType dataType() const { return d->dataType(); }
		//! Return whether the object is read-only
		virtual bool isReadOnly() const { return false; };
		//! Return the column mode
		/*
		 * This function is most used by tables but can also be used
		 * by plots. The column mode specifies how to interpret 
		 * the values in the column additional to the data type.
		 */ 
		virtual SciDAVis::ColumnMode columnMode() const { return d->columnMode(); }
		//! Set the column mode
		/*
		 * This sets the column mode and, if
		 * necessary, converts it to another datatype.
		 */
		virtual void setColumnMode(SciDAVis::ColumnMode mode);
		//! Copy another column of the same type
		/**
		 * This function will return false if the data type
		 * of 'other' is not the same as the type of 'this'.
		 * The validity information for the rows is also copied.
		 * Use a filter to convert a column to another type.
		 */
		virtual bool copy(const AbstractColumn * other);
		//! Copies part of another column of the same type
		/**
		 * This function will return false if the data type
		 * of 'other' is not the same as the type of 'this'.
		 * The validity information for the rows is also copied.
		 * \param other pointer to the column to copy
		 * \param src_start first row to copy in the column to copy
		 * \param dest_start first row to copy in
		 * \param num_rows the number of rows to copy
		 */ 
		virtual bool copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows);

		//! Return the data vector size
		/**
		 * \sa AbstractColumn::expand()
		 */ 
		virtual int rowCount() const { return d->rowCount(); }
		//! Expand the vector by the specified number of rows
		/**
		 * Since selecting and masking rows higher than the
		 * real internal number of rows is supported, this
		 * does not change the interval attributes. Also
		 * no signal is emitted. If the new rows are filled
		 * with values AbstractColumn::dataChanged()
		 * must be emitted.
		 * \sa AbstractColumn::dataChanged()
		 */
		virtual void expand(int new_rows);
		//! Insert some empty (or initialized with zero) rows
		virtual void insertEmptyRows(int before, int count);
		//! Remove 'count' rows starting from row 'first'
		virtual void removeRows(int first, int count);
		//! Return the column label
		virtual QString columnLabel() const { return name(); }
		//! Return the column comment
		virtual QString columnComment() const { return comment(); }
		//! Set the column label
		virtual void setColumnLabel(const QString& label);
		//! Set the column comment
		virtual void setColumnComment(const QString& comment);
		//! Return the column plot designation
		virtual SciDAVis::PlotDesignation plotDesignation() const { return d->plotDesignation(); }
		//! Set the column plot designation
		virtual void setPlotDesignation(SciDAVis::PlotDesignation pd);
		//! Clear the whole column
		virtual void clear();
		//! This must be called before the column is replaced by another
		virtual void notifyReplacement(AbstractColumn * replacement);

		//! \name IntervalAttribute related functions
		//@{
		//! Return whether a certain row contains an invalid value 	 
		virtual bool isInvalid(int row) const { return d->isInvalid(row); }
		//! Return whether a certain interval of rows contains only invalid values 	 
		virtual bool isInvalid(Interval<int> i) const { return d->isInvalid(i); }
		//! Return all intervals of invalid rows
		virtual QList< Interval<int> > invalidIntervals() const { return d->invalidIntervals(); }
		//! Return whether a certain row is masked 	 
		virtual bool isMasked(int row) const { return d->isMasked(row); }
		//! Return whether a certain interval of rows rows is fully masked 	 
		virtual bool isMasked(Interval<int> i) const { return d->isMasked(i); }
		//! Return all intervals of masked rows
		virtual QList< Interval<int> > maskedIntervals() const { return d->maskedIntervals(); }
		//! Clear all validity information
		virtual void clearValidity();
		//! Clear all masking information
		virtual void clearMasks();
		//! Set an interval invalid or valid
		/**
		 * \param i the interval
		 * \param invalid true: set invalid, false: set valid
		 */ 
		virtual void setInvalid(Interval<int> i, bool invalid = true);
		//! Overloaded function for convenience
		virtual void setInvalid(int row, bool invalid = true);
		//! Set an interval masked
		/**
		 * \param i the interval
		 * \param mask true: mask, false: unmask
		 */ 
		virtual void setMasked(Interval<int> i, bool mask = true);
		//! Overloaded function for convenience
		virtual void setMasked(int row, bool mask = true);
		//@}

		//! \name Formula related functions
		//@{
		//! Return the formula associated with row 'row' 	 
		virtual QString formula(int row) const { return d->formula(row); }
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
		virtual QList< Interval<int> > formulaIntervals() const { return d->formulaIntervals(); }
		//! Set a formula string for an interval of rows
		virtual void setFormula(Interval<int> i, QString formula);
		//! Overloaded function for convenience
		virtual void setFormula(int row, QString formula);
		//! Clear all formulas
		virtual void clearFormulas();
		//@}
		
		//! \name type specific functions
		//@{
		//! Return the content of row 'row'.
		/**
		 * Use this only when dataType() is QString
		 */
		virtual QString textAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QString
		 */
		virtual void setTextAt(int row, QString new_value);
		//! Replace a range of values 
		/**
		 * Use this only when dataType() is QString
		 */
		virtual void replaceTexts(int first, QStringList new_values);
		//! Return the date part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QDate dateAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual void setDateAt(int row, QDate new_value);
		//! Return the time part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QTime timeAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual void setTimeAt(int row, QTime new_value);
		//! Return the QDateTime in row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QDateTime dateTimeAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual void setDateTimeAt(int row, QDateTime new_value);
		//! Replace a range of values 
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual void replaceDateTimes(int first, QList<QDateTime> new_values);
		//! Return the double value in row 'row'
		virtual double valueAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is double
		 */
		virtual void setValueAt(int row, double new_value);
		//! Replace a range of values 
		/**
		 * Use this only when dataType() is double
		 */
		virtual void replaceValues(int first, int num_rows, const double * new_values);
		//@}

	friend class ColumnSetModeCmd;	
	// TODO: add all column commands
	
	private:
		//! Internal utility function
		void createPrivateObject();
		//! Pointer to the private interface and all private data
		ColumnPrivate * d;
};

#endif
