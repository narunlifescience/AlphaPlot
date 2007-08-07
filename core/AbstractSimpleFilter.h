/***************************************************************************
    File                 : AbstractSimpleFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Simplified filter interface for filters with
                           only one output port.

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
#ifndef ABSTRACT_SIMPLE_FILTER
#define ABSTRACT_SIMPLE_FILTER

#include "AbstractFilter.h"
#include "datatypes/AbstractDoubleDataSource.h"
#include "datatypes/AbstractStringDataSource.h"
#include "datatypes/AbstractDateTimeDataSource.h"
#include "lib/IntervalAttribute.h"

template<class T> class AbstractDataSourceTemplate {};
template<> class AbstractDataSourceTemplate<double> : public AbstractDoubleDataSource {};
template<> class AbstractDataSourceTemplate<QString> : public AbstractStringDataSource {};
template<> class AbstractDataSourceTemplate<QDateTime> : public AbstractDateTimeDataSource {};

/**
 * \brief Simplified filter interface for filters with only one output port.
 *
 * This class is only meant to simplify implementation of a restricted subtype of filter.
 * It should not be used as type for variables, which should always use either
 * AbstractFilter or (if necessary) an actual (non-abstract) implementation.
 *
 * The trick here is that, in a sense, the filter is its own output port. This means you
 * can implement a complete filter in only one class and don't have to coordinate data
 * transfer between a filter class and a data source class.
 * Additionaly, AbstractSimpleFilter offers some useful convenience methods which make writing
 * filters as painless as possible.
 *
 * For the template argument T (the data type of the output port), only double, QString and
 * QDateTime are supported.
 *
 * \section tutorial1 Tutorial, Step 1
 * The simplest filter you can write assumes there's also only one input port and rows on the
 * input correspond 1:1 to rows in the output. All you need to specify is what data type you
 * want to have on the input port and how to compute the output values:
 *
 * \code
 * 01 #include "AbstractSimpleFilter.h"
 * 02 class TutorialFilter1 : public AbstractSimpleFilter<double>
 * 03 {
 * 04	protected:
 * 05		virtual bool inputAcceptable(int, AbstractDataSource *source) {
 * 06			return source->inherits("AbstractDoubleDataSource");
 * 07		}
 * 08	public:
 * 09		virtual double valueAt(int row) const {
 * 10			if (!doubleInput()) return 0;
 * 11			double input_value = doubleInput()->valueAt(row);
 * 12			return input_value * input_value;
 * 13		}
 * 14 };
 * \endcode
 *
 * This filter reads an input value (line 11) and returns its square (line 12).
 * Before you call doubleInput(), don't forget to make sure that the data source really is of type
 * double (lines 5 to 7). Before you actually use doubleInput(), make sure that the input port has
 * been connected to a data source (line 10).
 * Otherwise line 11 would result in a crash. That's it, we've already written a
 * fully-functional filter!
 *
 * Equivalently, you can write 1:1-filters for QString or QDateTime inputs by checking for
 * "AbstractStringDataSource" or "AbstractDateTimeDataSource" in line 6. You would then use
 * stringInput()->textAt(row) or dateTimeInput()->dateTimeAt(row) to access the input data.
 * In order to provide QString output, substitute "QString" for "double" in line 2 and
 * implement QString textAt(int row) const instead of valueAt() in line 9.
 * For QDateTime output, you have to implement three methods (in addition to changing
 * line 2 accordingly):
 * \code
 * virtual QDateTime dateTimeAt(int row) const;
 * virtual QDate dateAt(int row) const;
 * virtual QTime timeAt(int row) const;
 * \endcode
 *
 * \section tutorial2 Tutorial, Step 2
 * Now for something slightly more interesting: a filter that uses only every second row of its
 * input. We no longer have a 1:1 correspondence between input and output rows, so we'll have
 * to do a bit more work in order to have everything come out as expected.
 * We'll use double-typed input and output again:
 * \code
 * 01 #include "AbstractSimpleFilter.h"
 * 02 class TutorialFilter2 : public AbstractSimpleFilter<double>
 * 03 {
 * 04	protected:
 * 05		virtual bool inputAcceptable(int, AbstractDataSource *source) {
 * 06			return source->inherits("AbstractDoubleDataSource");
 * 07		}
 * \endcode
 * Even rows (including row 0) get dropped, odd rows are renumbered:
 * \code
 * 08	public:
 * 09 	virtual double valueAt(int row) const {
 * 10 		if (!doubleInput()) return 0;
 * 11 		return doubleInput()->valueAt(2*row + 1);
 * 12 	}
 * \endcode
 */
template<class T> class AbstractSimpleFilter : public AbstractDataSourceTemplate<T>, public AbstractFilter
{
	public:
		//! Default to one input port.
		virtual int numInputs() const { return 1; }
		//! We manage only one output port (don't override unless you really know what you are doing).
		virtual int numOutputs() const { return 1; }
		//! Return a pointer to myself on port 0 (don't override unless you really know what you are doing).
		virtual AbstractDataSource* output(int port) const {
			return port == 0 ? const_cast<AbstractSimpleFilter<T>*>(this) : 0;
		}
		//! Copy label of input port 0.
		virtual QString label() const {
			return d_inputs.value(0) ? d_inputs.at(0)->label() : QString();
		}
		//! Copy plot designation of input port 0.
		virtual AbstractDataSource::PlotDesignation plotDesignation() const {
			return d_inputs.value(0) ?
				d_inputs.at(0)->plotDesignation() :
				AbstractDataSource::noDesignation;
		}
		//!\name assuming a 1:1 correspondence between input and output rows
		//@{
		virtual int rowCount() const {
			return d_inputs.value(0) ? d_inputs.at(0)->rowCount() : 0;
		}
		virtual QList< Interval<int> > dependentRows(Interval<int> input_range) const { return QList< Interval<int> >() << input_range; }
		//@}

		//!\name Masking
		//@{
		//! Return whether a certain row is masked
		virtual bool isMasked(int row) const { return d_masking.isSet(row); }
		//! Return whether a certain interval of rows rows is fully masked
		virtual bool isMasked(Interval<int> i) const { return d_masking.isSet(i); }
		//! Return all intervals of masked rows
		virtual QList< Interval<int> > maskedIntervals() const { return d_masking.intervals(); }
		//! Clear all masking information
		virtual void clearMasks()
		{
			emit maskingAboutToChange(this);	
			d_masking.clear();
			emit maskingChanged(this);	
		}
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
		IntervalAttribute<bool> d_masking;

		/**
		 * \brief Only use this if you are sure that an AbstractDoubleDataSource is connected to the given port.
		 *
		 * The standard way of ensuring this is reimplementing AbstractFilter::inputAcceptable():
		 *
		 * \code
		 * virtual bool inputAcceptable(int, AbstractDataSource *source) {
		 * 	return source->inherits("AbstractDoubleDataSource");
		 * }
		 * \endcode
		 */
		AbstractDoubleDataSource *doubleInput(int port=0) const {
			return static_cast<AbstractDoubleDataSource*>(d_inputs.value(port));
		}
		/**
		 * \brief Only use this if you are sure that an AbstractStringDataSource is connected to the given port.
		 *
		 * The standard way of ensuring this is reimplementing AbstractFilter::inputAcceptable():
		 *
		 * \code
		 * virtual bool inputAcceptable(int, AbstractDataSource *source) {
		 * 	return source->inherits("AbstractStringDataSource");
		 * }
		 * \endcode
		 */
		AbstractStringDataSource *stringInput(int port=0) const {
			return static_cast<AbstractStringDataSource*>(d_inputs.value(port));
		}
		/**
		 * \brief Only use this if you are sure that an AbstractDateTimeDataSource is connected to the given port.
		 *
		 * The standard way of ensuring this is reimplementing AbstractFilter::inputAcceptable():
		 *
		 * \code
		 * virtual bool inputAcceptable(int, AbstractDataSource *source) {
		 * 	return source->inherits("AbstractDateTimeDataSource");
		 * }
		 * \endcode
		 */
		AbstractDateTimeDataSource *dateTimeInput(int port=0) const {
			return static_cast<AbstractDateTimeDataSource*>(d_inputs.value(port));
		}

		//!\name signal handlers
		//@{
		virtual void inputDescriptionAboutToChange(AbstractDataSource*) { emit descriptionAboutToChange(this); }
		virtual void inputDescriptionChanged(AbstractDataSource*) { emit descriptionChanged(this); }
		virtual void inputPlotDesignationAboutToChange(AbstractDataSource*) { emit plotDesignationAboutToChange(this); }
		virtual void inputPlotDesignationChanged(AbstractDataSource*) { emit plotDesignationChanged(this); }
		virtual void inputDataAboutToChange(AbstractDataSource*) { emit dataAboutToChange(this); }
		virtual void inputDataChanged(AbstractDataSource*) { emit dataChanged(this); }

		virtual void inputRowsAboutToBeInserted(AbstractDataSource*, Interval<int> range) {
			foreach(Interval<int> output_range, dependentRows(range))
				emit rowsAboutToBeInserted(this, output_range.start(), output_range.size());
		}
		virtual void inputRowsInserted(AbstractDataSource*, Interval<int> range) {
			foreach(Interval<int> output_range, dependentRows(range))
				emit rowsInserted(this, output_range.start(), output_range.size());
		}
		virtual void inputRowsAboutToBeDeleted(AbstractDataSource*, Interval<int> range) {
			foreach(Interval<int> output_range, dependentRows(range))
				emit rowsAboutToBeDeleted(this, output_range.start(), output_range.size());
		}
		virtual void inputRowsDeleted(AbstractDataSource*, Interval<int> range) {
			foreach(Interval<int> output_range, dependentRows(range))
				emit rowsDeleted(this, output_range.start(), output_range.size());
		}
		//@}
};

#endif // ifndef ABSTRACT_SIMPLE_FILTER

