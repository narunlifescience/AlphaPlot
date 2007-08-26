/***************************************************************************
    File                 : AbstractColumn.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Interface definition for data with column logic

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

#ifndef ABSTRACTCOLUMN_H
#define ABSTRACTCOLUMN_H

#include <QObject>
#include <QtAlgorithms>
#include <QList>
class QString;
#include "lib/Interval.h"
#include "core/globals.h"

//! Interface definition for data with column logic
/**
  This is an abstract base class for column-based data, 
  i.e. mathematically a vector or technically a 1D array or list.
  It only defines the interface but has no data members itself. 

  Classes derived from this are typically table columns or filters
  that can be chained between table columns and plots. From the
  point of view of the plot functions there will be no difference 
  between a table column and a filter since both use this interface.

  Classes derived from this will either store a 
  vector with entries of one certain data type, e.g. double, QString, 
  QDateTime, or generate such values on demand. To determine the data
  type of a class derived from this, use the dataType() function. 
  AbstractColumn defines all access functions for all supported data 
  types but only those corresponding to the return value of dataType() 
  will return a meaningful value. Calling functions not belonging to 
  the data type of the column is save, but will do nothing (writing
  function) or return some default value (reading functions).

  This class also defines all signals which indicate a data change. 
  Any class whose output values are subject to change over time must emit
  the according signals. These signals notify any object working with the
  column before and after a change of the column.
  In some cases it will be necessary for a class using 
  the column to connect QObject::destroyed() also, to react 
  to a column's deletion, e.g. a filter's reaction to a 
  table deletion.

  All writing functions have a "do nothing" standard implementation to
  make deriving a read-only class very easy without bothering about the
  writing interface. Changing name and comment should be implemented even
  for read only columns, usually using the AbstractAspect interface.
  */
class AbstractColumn : public QObject
{
	Q_OBJECT

	public:
		//! Ctor
		AbstractColumn() {}
		//! Dtor
		virtual ~AbstractColumn() { emit aboutToBeDestroyed(this); }

		//! Return the data type of the column
		virtual SciDAVis::ColumnDataType dataType() const { return SciDAVis::UndefinedType; };
		//! Return whether the object is read-only
		virtual bool isReadOnly() const { return true; };
		//! Return the column mode
		/*
		 * This function is most used by tables but can also be used
		 * by plots. The column mode specifies how to interpret 
		 * the values in the column additional to the data type.
		 */ 
		virtual SciDAVis::ColumnMode columnMode() const = 0;
		//! Set the column mode
		/*
		 * This only sets the saved column mode and, if
		 * necessary, converts it to another datatype.
		 */
		virtual void setColumnMode(SciDAVis::ColumnMode mode) {};
		//! Copy another column of the same type
		/**
		 * This function will return false if the data type
		 * of 'other' is not the same as the type of 'this'.
		 * The validity information for the rows is also copied.
		 * Use a filter to convert a column to another type.
		 */
		virtual bool copy(const AbstractColumn * other) { return false; };
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
		virtual bool copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows) { return false; };

		//! Return the data vector size
		/**
		 * \sa AbstractColumn::expand()
		 */ 
		virtual int rowCount() const = 0;
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
		virtual void expand(int new_rows) {};
		//! Insert some empty (or initialized with zero) rows
		virtual void insertEmptyRows(int before, int count) {};
		//! Remove 'count' rows starting from row 'first'
		virtual void removeRows(int first, int count) {};
		//! Return the column label
		virtual QString columnLabel() const = 0;
		//! Return the column comment
		virtual QString columnComment() const = 0;
		//! Set the column label
		virtual void setColumnLabel(const QString& label) = 0;
		//! Set the column comment
		virtual void setColumnComment(const QString& comment) = 0;
		//! Return the column plot designation
		virtual SciDAVis::PlotDesignation plotDesignation() const = 0;
		//! Set the column plot designation
		virtual void setPlotDesignation(SciDAVis::PlotDesignation pd) {};
		//! Clear the whole column
		virtual void clear() {};
		//! This must be called before the column is replaced by another
		virtual void notifyReplacement(AbstractColumn * replacement) { emit aboutToBeReplaced(this, replacement); }

		//! \name IntervalAttribute related functions
		//@{
		//! Return whether a certain row contains an invalid value 	 
		virtual bool isInvalid(int row) const { return !Interval<int>(0, rowCount()-1).contains(row); } 	 
		//! Return whether a certain interval of rows contains only invalid values 	 
		virtual bool isInvalid(Interval<int> i) const { return !Interval<int>(0, rowCount()-1).contains(i); } 	 
		//! Return all intervals of invalid rows
		virtual QList< Interval<int> > invalidIntervals() const { return QList< Interval<int> >(); } 	 
		//! Return whether a certain row is masked 	 
		virtual bool isMasked(int row) const { Q_UNUSED(row); return false; } 	 
		//! Return whether a certain interval of rows rows is fully masked 	 
		virtual bool isMasked(Interval<int> i) const { Q_UNUSED(i); return false; }
		//! Return all intervals of masked rows
		virtual QList< Interval<int> > maskedIntervals() const { return QList< Interval<int> >(); } 	 
		//! Clear all validity information
		virtual void clearValidity() {};
		//! Clear all masking information
		virtual void clearMasks() {};
		//! Set an interval invalid or valid
		/**
		 * \param i the interval
		 * \param invalid true: set invalid, false: set valid
		 */ 
		virtual void setInvalid(Interval<int> i, bool invalid = true) {};
		//! Overloaded function for convenience
		virtual void setInvalid(int row, bool invalid = true) {};
		//! Set an interval masked
		/**
		 * \param i the interval
		 * \param mask true: mask, false: unmask
		 */ 
		virtual void setMasked(Interval<int> i, bool mask = true) {};
		//! Overloaded function for convenience
		virtual void setMasked(int row, bool mask = true) {};
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
		//! Set a formula string for an interval of rows
		virtual void setFormula(Interval<int> i, QString formula) {};
		//! Overloaded function for convenience
		virtual void setFormula(int row, QString formula) {};
		//! Clear all formulas
		virtual void clearFormulas() {};
		//@}
		
		//! \name type specific functions
		//@{
		//! Return the content of row 'row'.
		/**
		 * Use this only when dataType() is QString
		 */
		virtual QString textAt(int row) const = 0;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QString
		 */
		virtual void setTextAt(int row, QString new_value) {};
		//! Replace a range of values 
		/**
		 * Use this only when dataType() is QString
		 */
		virtual void replaceTexts(int first, QStringList new_values) {};
		//! Return the date part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QDate dateAt(int row) const = 0;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual void setDateAt(int row, QDate new_value) {};
		//! Return the time part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QTime timeAt(int row) const = 0;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual void setTimeAt(int row, QTime new_value) {};
		//! Return the QDateTime in row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QDateTime dateTimeAt(int row) const = 0;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual void setDateTimeAt(int row, QDateTime new_value) {};
		//! Replace a range of values 
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual void replaceDateTimes(int first, QList<QDateTime> new_values) {};
		//! Return the double value in row 'row'
		virtual double valueAt(int row) const = 0;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is double
		 */
		virtual void setValueAt(int row, double new_value) {};
		//! Replace a range of values 
		/**
		 * Use this only when dataType() is double
		 */
		virtual void replaceValues(int first, int num_rows, const double * new_values) {};
		//@}

	signals: 
		//! Column plot designation will be changed
		/**
		 * 'source' is always the this pointer of the column that
		 * emitted this signal. This way it's easier to use
		 * one handler for lots of columns.
		 */
		void plotDesignationAboutToChange(AbstractColumn * source); 
		//! Column plot designation changed
		/**
		 * 'source' is always the this pointer of the column that
		 * emitted this signal. This way it's easier to use
		 * one handler for lots of columns.
		 */
		void plotDesignationChanged(AbstractColumn * source); 
		//! Column mode (possibly also the data type) will be changed
		/**
		 * 'source' is always the this pointer of the column that
		 * emitted this signal. This way it's easier to use
		 * one handler for lots of columns.
		 */
		void modeAboutToChange(AbstractColumn * source); 
		//! Column mode (possibly also the data type) changed
		/**
		 * 'source' is always the this pointer of the column that
		 * emitted this signal. This way it's easier to use
		 * one handler for lots of columns.
		 */
		void modeChanged(AbstractColumn * source); 
		//! Data (including validity) of the column will be changed
		/**
		 * 'source' is always the this pointer of the column that
		 * emitted this signal. This way it's easier to use
		 * one handler for lots of columns.
		 */
		void dataAboutToChange(AbstractColumn * source); 
		//! Data (including validity) of the column has changed
		/**
		 * Important: When data has changed also the number
		 * of rows in the column may have increased without
		 * any other signal emission.
		 * 'source' is always the this pointer of the column that
		 * emitted this signal. This way it's easier to use
		 * one handler for lots of columns.
		 * \sa AbstractColumn::expand()
		 */
		void dataChanged(AbstractColumn * source); 
		//! The column will be replaced
		/**
		 * This is used then a column is replaced by another
		 * column, possibly of another type. 
		 *
		 * \param new_col Pointer to the column this one is to be replaced with.
		 *
		 * \param source is always a pointer to the column that
		 * emitted this signal. This way it's easier to use
		 * one handler for lots of columns.
		 */
		void aboutToBeReplaced(AbstractColumn * source, AbstractColumn * new_col); 
		//! Rows will be inserted
		/**
		 *	\param source the column that emitted the signal
		 *	\param before the row to insert before
		 *	\param count the number of rows to be inserted
		 */
		void rowsAboutToBeInserted(AbstractColumn * source, int before, int count); 
		//! Rows have been inserted
		/**
		 *	\param source the column that emitted the signal
		 *	\param before the row to insert before
		 *	\param count the number of rows to be inserted
		 */
		void rowsInserted(AbstractColumn * source, int before, int count); 
		//! Rows will be deleted
		/**
		 *	\param source the column that emitted the signal
		 *	\param first the first row to be deleted
		 *	\param count the number of rows to be deleted
		 */
		void rowsAboutToBeDeleted(AbstractColumn * source, int first, int count); 
		//! Rows have been deleted
		/**
		 *	\param source the column that emitted the signal
		 *	\param first the first row that was deleted
		 *	\param count the number of deleted rows
		 */
		void rowsDeleted(AbstractColumn * source, int first, int count); 
		//! IntervalAttribute related signal
		void maskingAboutToChange(AbstractColumn * source); 
		//! IntervalAttribute related signal
		void maskingChanged(AbstractColumn * source); 
		//! Emitted shortly before this data source is deleted.
		/**
		 * \param source the object emitting this signal
		 *
		 * This is need by AbstractFilter. QObject::destroyed() does not work there because its argument
		 * can't be cast to AbstractColumn properly (qobject_cast and lookup of the pointer in
		 * AbstractFilter::d_inputs fail).
		 */
		void aboutToBeDestroyed(AbstractColumn * source);

};

#endif
