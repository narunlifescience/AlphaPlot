/***************************************************************************
    File                 : ColumnPrivate.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Private members for Class Column

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

#ifndef COLUMNPRIVATE_H
#define COLUMNPRIVATE_H

#include <QObject>
#include "AbstractColumn.h"
class QString;
class Column;

//! Private interface and members for class Column
class ColumnPrivate : public AbstractColumn
{
	Q_OBJECT

	public:
		//! Ctor
		ColumnPrivate();
		//! Dtor
		~ColumnPrivate();

		//! Return the data type of the column
		virtual SciDAVis::ColumnDataType dataType() const { return d_data_type; };
		//! Return whether the object is read-only
		virtual bool isReadOnly() const { return false; };
		//! Return the column mode
		/*
		 * This function is most used by tables but can also be used
		 * by plots. The column mode specifies how to interpret 
		 * the values in the column additional to the data type.
		 */ 
		virtual SciDAVis::ColumnMode columnMode() const { return d_column_mode; };
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
		virtual int rowCount() const;
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
		//! Don't use this, its just a dummy implementation
		virtual QString columnLabel() const { return QString(); };
		//! Don't use this, its just a dummy implementation
		virtual QString columnComment() const { return QString(); };
		//! Don't use this, its just a dummy implementation
		virtual void setColumnLabel(const QString& label) {};
		//! Don't use this, its just a dummy implementation
		virtual void setColumnComment(const QString& comment) {};
		//! Return the column plot designation
		virtual SciDAVis::PlotDesignation plotDesignation() const { return d_plot_designation; };
		//! Set the column plot designation
		virtual void setPlotDesignation(SciDAVis::PlotDesignation pd);
		//! Clear the whole column
		virtual void clear();
		//! Don't use this, use Column::notifyReplacement
		virtual void notifyReplacement(AbstractColumn * replacement);

		//! \name IntervalAttribute related functions
		//@{
		//! Return whether a certain row contains an invalid value 	 
		virtual bool isInvalid(int row) const { return d_validity.isSet(row); } 	 
		//! Return whether a certain interval of rows contains only invalid values 	 
		virtual bool isInvalid(Interval<int> i) const { return d_validity.isSet(i); } 	 
		//! Return all intervals of invalid rows
		virtual QList< Interval<int> > invalidIntervals() const { return d_validity.intervals(); } 	 
		//! Return whether a certain row is masked 	 
		virtual bool isMasked(int row) const { return d_masking.isSet(row); } 	 
		//! Return whether a certain interval of rows rows is fully masked 	 
		virtual bool isMasked(Interval<int> i) const { return d_masking.isSet(i); }
		//! Return all intervals of masked rows
		virtual QList< Interval<int> > maskedIntervals() const { return d_masking.intervals(); } 	 
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
		virtual QString formula(int row) const { return d_formulas.value(row); }
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
		virtual QList< Interval<int> > formulaIntervals() const { return d_formulas.intervals(); }
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


		//! \name data members
		//@{
		//! Data type string
		/**
		 * double, QString, or QDateTime
		 */ 
		SciDAVis::ColumnDataType d_data_type;
		//! The column mode
		/**
		 * The column mode specifies how to interpret 
		 * the values in the column additional to the data type.
		 */
		SciDAVis::ColumnMode d_column_mode;
		//! Pointer to the data vector
		/**
		 * This will point to a QVector<double>, QStringList or
		 * QList<QDateTime> depending on the stored data type.
		 */
		void * d_data;
		IntervalAttribute<bool> d_validity;
		IntervalAttribute<bool> d_selection;
		IntervalAttribute<bool> d_masking;
		IntervalAttribute<QString> d_formulas;
		//! The plot designation
		SciDAVis::PlotDesignation d_plot_designation;
		//@}
		
};

#endif
