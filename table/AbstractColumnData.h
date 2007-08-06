/***************************************************************************
    File                 : AbstractColumnData.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Writing interface for column based data

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

#ifndef ABSTRACTCOLUMNDATA_H
#define ABSTRACTCOLUMNDATA_H

#include "AbstractDataSource.h"
#include "IntervalAttribute.h"
class QString;
#include <QObject>

//! Writing interface for column-based data
/**
  This is an abstract base class for column-based data, 
  i.e. mathematically a vector or technically a 1D array or list.
  It only defines the writing interface and has no data members itself. 
  The reading interface is defined in AbstractDataSource and
  classes derived from it.

  An instance of a subclass of this class represents a column
  in a TableDataModel or as a data source for plots and filters.
  In the latter case the filter/fit function must display the 
  data in a table that is read-only to the user or at least 
  does not support undo/redo. If instances of a subclass of AbstractColumnData
  are managed by something that supports undo, the write access
  provided by AbstractColumnData must only be available to the managing
  class itself and to its commands (QUndoCommand subclasses).
  This is for example the case for DoubleColumnData and Table.

  AbstractColumnData defines write-access methods which do not require
  knowledge of the type of data being handled. Classes derived from 
  this one will store a vector with entries of one certain data type, 
  e.g. double, QString, or QDateTime. To determine the data type of a 
  class derived from this, use qobject_cast or QObject::inherits().

  This class also implements functions to assign formulas to
  intervals of rows.
  */
class AbstractColumnData
{

public:
	//! Dtor
	virtual ~AbstractColumnData(){};

	//! Cast me to a data source (for read-access).
	/**
	 * Implementations of AbstractColumnData (the generic write interface) also implement
	 * AbstractDataSource (the generic read interface). Because of limitations of the C++
	 * language, you cannot safely cross-cast AbstractColumnData to AbstractDataSource, though.
	 * This has to be done on a per-implementation basis; thus this virtual casting method.
	 */
	virtual AbstractDataSource *asDataSource() = 0;
	//! Cast me to a qobject (for qobject_cast).
	/**
	 * Implementations of AbstractColumnData (the generic write interface) also implement
	 * AbstractDataSource (the generic read interface) which inherits from QObject. 
	 * Because of limitations of the C++ language, you cannot safely cross-cast 
	 * AbstractColumnData to QObject, though.
	 * This has to be done on a per-implementation basis; thus this virtual casting method.
	 */
	virtual QObject *asQObject() = 0;
	//! Copy another data source of the same type
	/**
	 * This function will return false if the data type
	 * of 'other' is not the same as the type of 'this'.
	 * The validity information for the rows is also copied.
	 * Use a filter to convert a data source.
	 */
	virtual bool copy(const AbstractDataSource * other) = 0;
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
	virtual bool copy(const AbstractDataSource * source, int source_start, int dest_start, int num_rows) = 0;
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
	virtual void expand(int new_rows) = 0;
	//! Set the column label
	virtual void setLabel(const QString& label) = 0; 
	//! Set the column comment
	virtual void setComment(const QString& comment) = 0;
	//! Set the column plot designation
	virtual void setPlotDesignation(AbstractDataSource::PlotDesignation pd) = 0;
	//! Insert some empty (or initialized with zero) rows
	virtual void insertEmptyRows(int before, int count) = 0;
	//! Remove 'count' rows starting from row 'first'
	virtual void removeRows(int first, int count) = 0;
	//! This must be called before the column is replaced by another
	virtual void notifyReplacement(AbstractDataSource * replacement) = 0;
	//! Clear the whole column
	virtual void clear() = 0;

	//! \name IntervalAttribute related functions
	//@{
	//! Clear all validity information
	virtual void clearValidity() = 0;
	//! Clear all selection information
	virtual void clearSelections() = 0;
	//! Clear all masking information
	virtual void clearMasks() = 0;
	//! Set an interval invalid or valid
	/**
	 * \param i the interval
	 * \param invalid true: set invalid, false: set valid
	 */ 
	virtual void setInvalid(Interval<int> i, bool invalid = true) = 0;
	//! Overloaded function for convenience
	virtual void setInvalid(int row, bool invalid = true) = 0;
	//! Select of deselect a certain interval
	/**
	 * \param i the interval
	 * \param select true: select, false: deselect
	 */ 
	virtual void setSelected(Interval<int> i, bool select = true) = 0;
	//! Overloaded function for convenience
	virtual void setSelected(int row, bool select = true) = 0;
	//! Set an interval masked
	/**
	 * \param i the interval
	 * \param mask true: mask, false: unmask
	 */ 
	virtual void setMasked(Interval<int> i, bool mask = true) = 0;
	//! Overloaded function for convenience
	virtual void setMasked(int row, bool mask = true) = 0;
	//@}
	
	//! \name Formula related functions
	//@{
	//! Return the formula associated with row 'row' 	 
	QString formula(int row) const { return d_formulas.value(row); }
	//! Set a formula string for an interval of rows
	void setFormula(Interval<int> i, QString formula) { d_formulas.setValue(i, formula); }
	//! Overloaded function for convenience
	void setFormula(int row, QString formula) { setFormula(Interval<int>(row,row), formula); }
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
	QList< Interval<int> > formulaIntervals() const { return d_formulas.intervals(); }
	//! Clear all formulas
	void clearFormulas();
	//@}

protected:
	IntervalAttribute<QString> d_formulas;
};

#endif
