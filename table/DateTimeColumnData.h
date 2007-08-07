/***************************************************************************
    File                 : DateTimeColumnData.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Data source that stores a list of QDateTimes (implementation)

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

#ifndef DATECOLUMNDATA_H
#define DATECOLUMNDATA_H

#include "AbstractColumnData.h"
#include "core/datatypes/AbstractDateTimeDataSource.h"
#include <QList>
#include <QDateTime>
#include <QDate>
#include <QTime>

//! Data source that stores a list of QDateTimes (implementation)
/**
  * This class stores a list of QDateTimes. It implements the
  * interfaces defined in AbstractColumnData, AbstractDataSource,
  * and AbstractDateTimeDataSource as well as the data type specific
  * writing functions. The stored data can also be accessed by
  * the functions of QList\<QDateTime\>.
  *
  * There is only minor flaw (IMHO) in QDateTime:
  * QDateTime(QDate(), QTime(11,10,0,0)).toString("hh:mm") will give and emtpy string.
  * QDateTime::fromString("11:10","hh:mm") gives QDateTime(QDate(1900,1,1), QTime(11,10,0,0)) 
  * though, so whenever the date is invalid, it is replaced with QDate(1900,1,1).
  * 
  * \sa AbstractColumnData
  * \sa AbstractDataSource
  * \sa AbstractDateTimeDataSource
  */
class DateTimeColumnData : public AbstractDateTimeDataSource, public AbstractColumnData, public QList<QDateTime>
{
	Q_OBJECT

public:
	//! Dtor
	virtual ~DateTimeColumnData(){};
	//! Ctor
	DateTimeColumnData();
	//! Ctor
	DateTimeColumnData(const QList<QDateTime>& list);
	virtual AbstractDataSource *asDataSource() { return this; }
	virtual QObject *asQObject() { return this; }

	//! \name Data writing functions
	//@{
	//! Copy another datetime data source
	/**
	 * This function will return false if the data type
	 * of 'other' is not the same as the type of 'this'.
	 * The validity information for the rows is also copied.
	 * Use a filter to convert a data source.
	 */
	virtual bool copy(const AbstractDataSource * other);
	//! Copies part of another data source of the same type
	/**
	 * This function will return false if the data type
	 * of 'other' is not the same as the type of 'this'.
	 * The validity information for the rows is also copied.
	 * \param other pointer to the data source to copy
	 * \param src_start first row to copy in the data source to copy
	 * \param dest_start first row to copy in
	 * \param num_rows the number of rows to copy
	 */ 
	virtual bool copy(const AbstractDataSource * other, int source_start, int dest_start, int num_rows);
	//! Expand the vector by the specified number of rows
	/**
	 * Since selecting and masking rows higher than the
	 * real internal number of rows is supported, this
	 * does not change the interval attributes. Also
	 * no signal is emitted. If the new rows are filled
	 * with values AbstractDataSource::dataChanged()
	 * must be emitted.
	 * \sa AbstractDataSource::dataChanged()
	 */
	virtual void expand(int new_rows);
	//! Set the column label
	virtual void setLabel(const QString& label);
	//! Set the column comment
	virtual void setComment(const QString& comment);
	//! Set the column plot designation
	virtual void setPlotDesignation(AbstractDataSource::PlotDesignation pd);
	//! Insert some empty rows
	virtual void insertEmptyRows(int before, int count);
	//! Remove 'count' rows starting from row 'first'
	virtual void removeRows(int first, int count);
	//! This must be called before the column is replaced by another
	virtual void notifyReplacement(AbstractDataSource * replacement);
	//! Clear the whole column
	virtual void clear();
	//@}
	
	//! \name Data reading functions
	//@{
	//! Return the column label
	virtual QString label() const;
	//! Return the column comment
	virtual QString comment() const;
	//! Return the column plot designation
	virtual AbstractDataSource::PlotDesignation plotDesignation() const;
	//! Return the list size
	virtual int rowCount() const;
	//! Return the date part of row 'row'
	virtual QDate dateAt(int row) const;
	//! Return the time part of row 'row'
	virtual QTime timeAt(int row) const;
	//! Return the QDateTime in row 'row'
	virtual QDateTime dateTimeAt(int row) const;
	//@}

	//! \name IntervalAttribute related reading functions
	//@{
	//! Return whether a certain row contains an invalid value 	 
	virtual bool isInvalid(int row) const { return d_validity.isSet(row); } 	 
	//! Return whether a certain interval of rows contains only invalid values 	 
	virtual bool isInvalid(Interval<int> i) const { return d_validity.isSet(i); } 	 
	//! Return all intervals of invalid rows
	virtual QList< Interval<int> > invalidIntervals() const { return d_validity.intervals(); } 	 
	//! Return whether a certain row is selected 	 
	virtual bool isSelected(int row) const { return d_selection.isSet(row); } 	 
	//! Return whether a certain interval of rows is fully selected
	virtual bool isSelected(Interval<int> i) const { return d_selection.isSet(i); } 	 
	//! Return all selected intervals 	 
	virtual QList< Interval<int> > selectedIntervals() const { return d_selection.intervals(); } 	 
	//! Return whether a certain row is masked 	 
	virtual bool isMasked(int row) const { return d_masking.isSet(row); } 	 
	//! Return whether a certain interval of rows rows is fully masked 	 
	virtual bool isMasked(Interval<int> i) const { return d_masking.isSet(i); }
	//! Return all intervals of masked rows
	virtual QList< Interval<int> > maskedIntervals() const { return d_masking.intervals(); } 	 
	//@}
	
	//! \name IntervalAttribute related writing functions
	//@{
	//! Clear all validity information
	virtual void clearValidity()
	{
		emit dataAboutToChange(this);	
		d_validity.clear();
		emit dataChanged(this);	
	}
	//! Clear all selection information
	virtual void clearSelections()
	{
		emit selectionAboutToChange(this);	
		d_selection.clear();
		emit selectionChanged(this);	
	}
	//! Clear all masking information
	virtual void clearMasks()
	{
		emit maskingAboutToChange(this);	
		d_masking.clear();
		emit maskingChanged(this);	
	}
	//! Set an interval invalid or valid
	/**
	 * \param i the interval
	 * \param invalid true: set invalid, false: set valid
	 */ 
	virtual void setInvalid(Interval<int> i, bool invalid = true)
	{
		emit dataAboutToChange(this);	
		d_validity.setValue(i, invalid);
		emit dataChanged(this);	
	}
	//! Overloaded function for convenience
	virtual void setInvalid(int row, bool invalid = true) { setInvalid(Interval<int>(row,row), invalid); }
	//! Select of deselect a certain interval
	/**
	 * \param i the interval
	 * \param select true: select, false: deselect
	 */ 
	virtual void setSelected(Interval<int> i, bool select = true)
	{
		emit selectionAboutToChange(this);	
		d_selection.setValue(i, select);
		emit selectionChanged(this);	
	}
	//! Overloaded function for convenience
	virtual void setSelected(int row, bool select = true) { setSelected(Interval<int>(row,row), select); }
	//! Set an interval masked
	/**
	 * \param i the interval
	 * \param mask true: mask, false: unmask
	 */ 
	virtual void setMasked(Interval<int> i, bool mask = true)
	{
		emit maskingAboutToChange(this);	
		d_masking.setValue(i, mask);
		emit maskingChanged(this);	
	}
	//! Overloaded function for convenience
	virtual void setMasked(int row, bool mask = true) { setMasked(Interval<int>(row,row), mask); }
	//@}
	
protected:
	IntervalAttribute<bool> d_validity;
	IntervalAttribute<bool> d_selection;
	IntervalAttribute<bool> d_masking;
	//! The column label
	QString d_label;
	//! The column comment
	QString d_comment;
	//! The plot designation
	AbstractDataSource::PlotDesignation d_plot_designation;
};

#endif


