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

#include "core/AbstractAspect.h"
#include "core/AbstractSimpleFilter.h"
#include "lib/IntervalAttribute.h"
#include "column/ColumnPrivate.h"
#include "column/columncommands.h"
class QString;

//! Table column class
/**
  This class represents a column in a table. It has a public reading and
  (undo aware) writing interface defined in AbstractColumn and a private
  interface, which is only to be used by commands, defined
  by ColumnPrivate and accessed by the d pointer. All private data and
  functions members except utility functions are defined in ColumnPrivate.
  All commands working on a Column must be declared as friend classes.
 */
class Column : public QObject, public AbstractAspect, public AbstractColumn
{
	Q_OBJECT

	public:
		//! Ctor
		/**
		 * \param label the column label (= aspect name)
		 * \param mode initial column mode
		 */
		Column(const QString& label, SciDAVis::ColumnMode mode);
		//! Ctor
		/**
		 * \param label the column label (= aspect name)
		 * \param data initial data vector
		 * \param validity a list of invalid intervals (optional)
		 */
		Column(const QString& label, QVector<double> data, IntervalAttribute<bool> validity = IntervalAttribute<bool>());
		//! Ctor
		/**
		 * \param label the column label (= aspect name)
		 * \param data initial data vector
		 * \param validity a list of invalid intervals (optional)
		 */
		Column(const QString& label, QStringList data, IntervalAttribute<bool> validity = IntervalAttribute<bool>()); 
		//! Ctor
		/**
		 * \param label the column label (= aspect name)
		 * \param data initial data vector
		 * \param validity a list of invalid intervals (optional)
		 */
		Column(const QString& label, QList<QDateTime> data, IntervalAttribute<bool> validity = IntervalAttribute<bool>());
		//! Dtor
		~Column();

		//! \name aspect related functions
		//@{
		//! Return the QObject that is responsible for emitting signals
		virtual const QObject *signalEmitter() const { return this; }
		//! Return the QObject that is responsible for receiving signals
		virtual const QObject *signalReceiver() const { return this; }
		//! See QMetaObject::className().
		virtual const char* className() const { return metaObject()->className(); }
		//! See QObject::inherits().
		virtual bool inherits(const char *class_name) const { return QObject::inherits(class_name); }
		//! This will always return zero as columns don't have a view
		virtual QWidget *view(QWidget *parent = 0) { Q_UNUSED(parent) return 0; }
		//@}

		//! Return the data type of the column
		SciDAVis::ColumnDataType dataType() const { return d->dataType(); }
		//! Return whether the object is read-only
		bool isReadOnly() const { return false; };
		//! Return the column mode
		/**
		 * This function is most used by tables but can also be used
		 * by plots. The column mode specifies how to interpret 
		 * the values in the column additional to the data type.
		 */ 
		SciDAVis::ColumnMode columnMode() const { return d->columnMode(); }
		//! Set the column mode
		/**
		 * This sets the column mode and, if
		 * necessary, converts it to another datatype.
		 * Remark: setting the mode back to undefined (the 
		 * initial value) is not supported.
		 */
		void setColumnMode(SciDAVis::ColumnMode mode);
		//! Copy another column of the same type
		/**
		 * This function will return false if the data type
		 * of 'other' is not the same as the type of 'this'.
		 * The validity information for the rows is also copied.
		 * Use a filter to convert a column to another type.
		 */
		bool copy(const AbstractColumn * other);
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
		bool copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows);
		//! Return the data vector size
		/**
		 * This returns the number of rows that actually contain data. 
		 * Rows beyond this can be masked etc. but should be ignored by filters,
		 * plots etc.
		 */
		int rowCount() const { return d->rowCount(); }
		//! Insert some empty (or initialized with zero) rows
		void insertEmptyRows(int before, int count);
		//! Remove 'count' rows starting from row 'first'
		void removeRows(int first, int count);
		//! Return the column label
		QString columnLabel() const { return name(); }
		//! Return the column comment
		QString columnComment() const { return comment(); }
		//! Set the column label
		void setColumnLabel(const QString& label) { setName(label); }
		//! Set the column comment
		void setColumnComment(const QString& comment) { setComment(comment); }
		//! Return the column plot designation
		SciDAVis::PlotDesignation plotDesignation() const { return d->plotDesignation(); }
		//! Set the column plot designation
		void setPlotDesignation(SciDAVis::PlotDesignation pd);
		//! Clear the whole column
		void clear();
		//! This must be called before the column is replaced by another
		void notifyReplacement(AbstractColumn * replacement);
		//! Return the input filter (for string -> data type conversion)
		/**
		 * This method is mainly used to get a filter that can convert
		 * user input (strings) to the column's data type.
		 */
		AbstractSimpleFilter * inputFilter() const { return d->inputFilter(); }
		//! Return the output filter (for data type -> string  conversion)
		/**
		 * This method is mainly used to get a filter that can convert
		 * the column's data type to strings (usualy to display in a view).
		 */
		AbstractSimpleFilter * outputFilter() const { return d->outputFilter(); }

		//! \name IntervalAttribute related functions
		//@{
		//! Return whether a certain row contains an invalid value 	 
		bool isInvalid(int row) const { return d->isInvalid(row); }
		//! Return whether a certain interval of rows contains only invalid values 	 
		bool isInvalid(Interval<int> i) const { return d->isInvalid(i); }
		//! Return all intervals of invalid rows
		QList< Interval<int> > invalidIntervals() const { return d->invalidIntervals(); }
		//! Return whether a certain row is masked 	 
		bool isMasked(int row) const { return d->isMasked(row); }
		//! Return whether a certain interval of rows rows is fully masked 	 
		bool isMasked(Interval<int> i) const { return d->isMasked(i); }
		//! Return all intervals of masked rows
		QList< Interval<int> > maskedIntervals() const { return d->maskedIntervals(); }
		//! Clear all validity information
		void clearValidity();
		//! Clear all masking information
		void clearMasks();
		//! Set an interval invalid or valid
		/**
		 * \param i the interval
		 * \param invalid true: set invalid, false: set valid
		 */ 
		void setInvalid(Interval<int> i, bool invalid = true);
		//! Overloaded function for convenience
		void setInvalid(int row, bool invalid = true);
		//! Set an interval masked
		/**
		 * \param i the interval
		 * \param mask true: mask, false: unmask
		 */ 
		void setMasked(Interval<int> i, bool mask = true);
		//! Overloaded function for convenience
		void setMasked(int row, bool mask = true);
		//@}

		//! \name Formula related functions
		//@{
		//! Return the formula associated with row 'row' 	 
		QString formula(int row) const { return d->formula(row); }
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
		QList< Interval<int> > formulaIntervals() const { return d->formulaIntervals(); }
		//! Set a formula string for an interval of rows
		void setFormula(Interval<int> i, QString formula);
		//! Overloaded function for convenience
		void setFormula(int row, QString formula);
		//! Clear all formulas
		void clearFormulas();
		//@}
		
		//! \name type specific functions
		//@{
		//! Return the content of row 'row'.
		/**
		 * Use this only when dataType() is QString
		 */
		QString textAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QString
		 */
		void setTextAt(int row, QString new_value);
		//! Replace a range of values 
		/**
		 * Use this only when dataType() is QString
		 */
		void replaceTexts(int first, const QStringList& new_values);
		//! Return the date part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		QDate dateAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		void setDateAt(int row, QDate new_value);
		//! Return the time part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		QTime timeAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		void setTimeAt(int row, QTime new_value);
		//! Return the QDateTime in row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		QDateTime dateTimeAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		void setDateTimeAt(int row, QDateTime new_value);
		//! Replace a range of values 
		/**
		 * Use this only when dataType() is QDateTime
		 */
		void replaceDateTimes(int first, const QList<QDateTime>& new_values);
		//! Return the double value in row 'row'
		double valueAt(int row) const;
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is double
		 */
		void setValueAt(int row, double new_value);
		//! Replace a range of values 
		/**
		 * Use this only when dataType() is double
		 */
		virtual void replaceValues(int first, const QVector<double>& new_values);
		//@}

		friend class ColumnSetModeCmd;	
		friend class ColumnFullCopyCmd;
		friend class ColumnPartialCopyCmd;
		friend class ColumnInsertEmptyRowsCmd;
		friend class ColumnRemoveRowsCmd;
		friend class ColumnSetPlotDesignationCmd;
		friend class ColumnClearCmd;
		friend class ColumnClearValidityCmd;
		friend class ColumnClearMasksCmd;
		friend class ColumnSetInvalidCmd;
		friend class ColumnSetMaskedCmd;
		friend class ColumnSetFormulaCmd;
		friend class ColumnClearFormulasCmd;
		friend class ColumnSetTextCmd;
		friend class ColumnSetValueCmd;
		friend class ColumnSetDateTimeCmd;
		friend class ColumnReplaceTextsCmd;
		friend class ColumnReplaceValuesCmd;
		friend class ColumnReplaceDateTimesCmd;

	private:
		//! Pointer to the private interface and all private data
		ColumnPrivate * d;
};

#endif
