/***************************************************************************
    File                 : AbstractSimpleFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
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

#include <QtGlobal>
#include "core/AbstractFilter.h"
#include "core/AbstractColumn.h"
#include "core/AbstractAspect.h"
#include "lib/IntervalAttribute.h"

#ifndef _NO_TR1_
#include "tr1/memory" 
using std::tr1::enable_shared_from_this;
using std::tr1::static_pointer_cast;
using std::tr1::dynamic_pointer_cast;
#else // if your compiler does not have TR1 support, you can use boost instead:
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
using boost::enable_shared_from_this;
using boost::static_pointer_cast;
using boost::dynamic_pointer_cast;
#endif

/**
 * \brief Simplified filter interface for filters with only one output port.
 *
 * This class is only meant to simplify implementation of a restricted subtype of filter.
 * It should not be instantiated directly. You should always either derive from
 * AbstractFilter or (if necessary) provide an actual (non-abstract) implementation.
 *
 * The trick here is that, in a sense, the filter is its own output port. This means you
 * can implement a complete filter in only one class and don't have to coordinate data
 * transfer between a filter class and a data source class.
 * Additionaly, AbstractSimpleFilter offers some useful convenience methods which make writing
 * filters as painless as possible.
 *
 * For the data type of the output, all types supported by AbstractColumn (currently double, QString and
 * QDateTime) are supported.
 *
 * \section tutorial1 Tutorial, Step 1
 * The simplest filter you can write assumes there's also only one input port and rows on the
 * input correspond 1:1 to rows in the output. All you need to specify is what data type you
 * want to have (in this example double) on the input port and how to compute the output values:
 *
 * \code
 * 01 #include "AbstractSimpleFilter.h"
 * 02 class TutorialFilter1 : public AbstractSimpleFilter
 * 03 {
 * 04	protected:
 * 05		virtual bool inputAcceptable(int, AbstractColumn *source) {
 * 06			return (source->dataType() == SciDAVis::TypeDouble);
 * 07		}
 * 08	public:
 * 09		virtual SciDAVis::ColumnDataType dataType() const { return SciDAVis::TypeDouble; }
 * 10
 * 11		virtual double valueAt(int row) const {
 * 12			if (!d_inputs.value(0)) return 0.0;
 * 13			double input_value = d_inputs.value(0)->valueAt(row);
 * 14			return input_value * input_value;
 * 15		}
 * 16 };
 * \endcode
 *
 * This filter reads an input value (line 13) and returns its square (line 14).
 * Reimplementing inputAcceptable() makes sure that the data source really is of type
 * double (lines 5 to 7). Otherwise, the source will be rejected by AbstractFilter::input().
 * The output type is repoted by reimplementing dataType() (line 09).
 * Before you actually use d_inputs.value(0), make sure that the input port has
 * been connected to a data source (line 12).
 * Otherwise line 13 would result in a crash. That's it, we've already written a
 * fully-functional filter!
 *
 * Equivalently, you can write 1:1-filters for QString or QDateTime inputs by checking for
 * SciDAVis::TypeQString or SciDAVis::TypeQDateTime in line 6. You would then use
 * AbstractColumn::textAt(row) or AbstractColumn::dateTimeAt(row) in line 13 to access the input data.
 * For QString output, you need to implement AbstractColumn::textAt(row). 
 * For QDateTime output, you have to implement three methods:
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
 * 02 class TutorialFilter2 : public AbstractSimpleFilter
 * 03 {
 * 04	protected:
 * 05		virtual bool inputAcceptable(int, AbstractColumn *source) {
 * 06			return (source->dataType() == SciDAVis::TypeDouble);
 * 07		}
 * 08	public:
 * 09		virtual SciDAVis::ColumnDataType dataType() const { return SciDAVis::TypeDouble; }
 * \endcode
 * Even rows (including row 0) get dropped, odd rows are renumbered:
 * \code
 * 10	public:
 * 11 	virtual double valueAt(int row) const {
 * 12		if (!d_inputs.value(0)) return 0.0;
 * 13		return d_inputs.value(0)->valueAt(2*row + 1);
 * 14 	}
 * \endcode
 */
class AbstractSimpleFilter : public QObject, public AbstractFilter, public AbstractColumn, public enable_shared_from_this<AbstractSimpleFilter>
{
	Q_OBJECT

	public:
		//! Ctor
		AbstractSimpleFilter() {}
		//! Default to one input port.
		virtual int inputCount() const { return 1; }
		//! We manage only one output port (don't override unless you really know what you are doing).
		virtual int outputCount() const { return 1; }
		//! Return a pointer to myself on port 0 (don't override unless you really know what you are doing).
		virtual shared_ptr<AbstractColumn> output(int port) const {
			return port == 0 ? const_cast<AbstractSimpleFilter *>(this)->sharedAbstractColumnPtrFromThis() : shared_ptr<AbstractColumn>();
		}
		//! Copy label of input port 0.
		virtual QString columnLabel() const {
			return d_inputs.value(0) ? d_inputs.at(0)->columnLabel() : QString();
		}
		//! Copy comment of input port 0.
		virtual QString columnComment() const {
			return d_inputs.value(0) ? d_inputs.at(0)->columnComment() : QString();
		}
		//! Copy plot designation of input port 0.
		virtual SciDAVis::PlotDesignation plotDesignation() const {
			return d_inputs.value(0) ?
				d_inputs.at(0)->plotDesignation() :
				SciDAVis::noDesignation;
		}
		//! Return the data type of the input
		virtual SciDAVis::ColumnDataType dataType() const
		{
			// calling this function while d_input is empty is a sign of very bad code
			// nevertheless it will return some rather meaningless value to
			// avoid crashes
			return d_inputs.value(0) ? d_inputs.at(0)->dataType() : SciDAVis::TypeQString;
		}
		//! Return the column mode
		/**
		 * This function is most used by tables but can also be used
		 * by plots. The column mode specifies how to interpret 
		 * the values in the column additional to the data type.
		 */ 
		virtual SciDAVis::ColumnMode columnMode() const
		{
			// calling this function while d_input is empty is a sign of very bad code
			// nevertheless it will return some rather meaningless value to
			// avoid crashes
			return d_inputs.value(0) ? d_inputs.at(0)->columnMode() : SciDAVis::Text;
		}
		//! Set the column label (does nothing in the standard implementation)
		virtual void setColumnLabel(const QString& label) { Q_UNUSED(label); }
		//! Set the column comment (does nothing in the standard implementation)
		virtual void setColumnComment(const QString& comment) { Q_UNUSED(comment); }
		//! Return the content of row 'row'.
		/**
		 * Use this only when dataType() is QString
		 */
		virtual QString textAt(int row) const
		{
			return d_inputs.value(0) ? d_inputs.at(0)->textAt(row) : QString();
		}
		//! Return the date part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QDate dateAt(int row) const
		{
			return d_inputs.value(0) ? d_inputs.at(0)->dateAt(row) : QDate();
		}
		//! Return the time part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QTime timeAt(int row) const
		{
			return d_inputs.value(0) ? d_inputs.at(0)->timeAt(row) : QTime();
		}
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QDateTime dateTimeAt(int row) const
		{
			return d_inputs.value(0) ? d_inputs.at(0)->dateTimeAt(row) : QDateTime();
		}
		//! Return the double value in row 'row'
		/**
		 * Use this only when dataType() is double
		 */
		virtual double valueAt(int row) const
		{
			return d_inputs.value(0) ? d_inputs.at(0)->valueAt(row) : 0.0;
		}
		//! \name aspect related functions
		//@{
		//! Construct a standard view on me.
		/**
		 * This returns 0 if not overloaded.
		 * \sa AbstractAspect::view()
		 */
		virtual QWidget *view(QWidget *parent = 0) { Q_UNUSED(parent) return 0; }
		//! Remove me from my parent's list of children.
		//@}

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
			emit abstractColumnSignalEmitter()->maskingAboutToChange(this);	
			d_masking.clear();
			emit abstractColumnSignalEmitter()->maskingChanged(this);	
		}
		//! Set an interval masked
		/**
		 * \param i the interval
		 * \param mask true: mask, false: unmask
		 */ 
		virtual void setMasked(Interval<int> i, bool mask = true)
		{
			emit abstractColumnSignalEmitter()->maskingAboutToChange(this);	
			d_masking.setValue(i, mask);
			emit abstractColumnSignalEmitter()->maskingChanged(this);	
		}
		//! Overloaded function for convenience
		virtual void setMasked(int row, bool mask = true) { setMasked(Interval<int>(row,row), mask); }
		//@}

		//! See QMetaObject::className().
		virtual const char* className() const { return metaObject()->className(); }
		//! See QObject::inherits().
		virtual bool inherits(const char *class_name) const { return QObject::inherits(class_name); }

		//! Return whether a certain row contains an invalid value 	 
		virtual bool isInvalid(int row) const { return d_inputs.value(0) ? d_inputs.at(0)->isInvalid(row) : false; }
		//! Return whether a certain interval of rows contains only invalid values 	 
		virtual bool isInvalid(Interval<int> i) const { return d_inputs.value(0) ? d_inputs.at(0)->isInvalid(i) : false; }
		//! Return all intervals of invalid rows
		virtual QList< Interval<int> > invalidIntervals() const 
		{
			return d_inputs.value(0) ? d_inputs.at(0)->maskedIntervals() : QList< Interval<int> >(); 
		}

		//! \name XML related functions
		//@{
		//! Save the column as XML
		virtual void save(QXmlStreamWriter * writer) const = 0;
		//! Load the column from XML
		virtual bool load(QXmlStreamReader * reader) = 0;
		//@}

	protected:
		IntervalAttribute<bool> d_masking;

		//!\name signal handlers
		//@{
		virtual void inputDescriptionAboutToChange(AbstractColumn*) 
		{ 
			emit abstractColumnSignalEmitter()->descriptionAboutToChange(this); 
		}
		virtual void inputDescriptionChanged(AbstractColumn*) 
		{ 
			emit abstractColumnSignalEmitter()->descriptionChanged(this); 
		}
		virtual void inputPlotDesignationAboutToChange(AbstractColumn*) 
		{ 
			emit abstractColumnSignalEmitter()->plotDesignationAboutToChange(this); 
		}
		virtual void inputPlotDesignationChanged(AbstractColumn*) 
		{ 
			emit abstractColumnSignalEmitter()->plotDesignationChanged(this); 
		}
		virtual void inputModeAboutToChange(AbstractColumn*) 
		{ 
			emit abstractColumnSignalEmitter()->dataAboutToChange(this); 
		}
		virtual void inputModeChanged(AbstractColumn*) 
		{ 
			emit abstractColumnSignalEmitter()->dataChanged(this); 
		}
		virtual void inputDataAboutToChange(AbstractColumn*) 
		{ 
			emit abstractColumnSignalEmitter()->dataAboutToChange(this); 
		}
		virtual void inputDataChanged(AbstractColumn*) 
		{ 
			emit abstractColumnSignalEmitter()->dataChanged(this); 
		}

		virtual void inputRowsAboutToBeInserted(AbstractColumn * source, int before, int count) {
			Q_UNUSED(source);
			Q_UNUSED(count);
			foreach(Interval<int> output_range, dependentRows(Interval<int>(before, before)))
				emit abstractColumnSignalEmitter()->rowsAboutToBeInserted(this, output_range.start(), output_range.size());
		}
		virtual void inputRowsInserted(AbstractColumn * source, int before, int count) {
			Q_UNUSED(source);
			Q_UNUSED(count);
			foreach(Interval<int> output_range, dependentRows(Interval<int>(before, before)))
				emit abstractColumnSignalEmitter()->rowsInserted(this, output_range.start(), output_range.size());
		}
		virtual void inputRowsAboutToBeRemoved(AbstractColumn * source, int first, int count) {
			Q_UNUSED(source);
			foreach(Interval<int> output_range, dependentRows(Interval<int>(first, first+count-1)))
				emit abstractColumnSignalEmitter()->rowsAboutToBeRemoved(this, output_range.start(), output_range.size());
		}
		virtual void inputRowsRemoved(AbstractColumn * source, int first, int count) {
			Q_UNUSED(source);
			foreach(Interval<int> output_range, dependentRows(Interval<int>(first, first+count-1)))
				emit abstractColumnSignalEmitter()->rowsRemoved(this, output_range.start(), output_range.size());
		}
		//@}

	private:
		//! Helper function
		shared_ptr<AbstractColumn> sharedAbstractColumnPtrFromThis()
		{
			return dynamic_pointer_cast<AbstractColumn>(shared_from_this());
		}

};

#endif // ifndef ABSTRACT_SIMPLE_FILTER

