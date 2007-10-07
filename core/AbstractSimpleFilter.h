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

// TODO: revise description
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
 * 05		virtual bool inputAcceptable(int, AbstractColumn *source) {
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
 * 05		virtual bool inputAcceptable(int, AbstractColumn *source) {
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
class AbstractSimpleFilter : public QObject, public AbstractAspect, public AbstractFilter, public AbstractColumn 
{
	Q_OBJECT

	public:
		//! Ctor
		AbstractSimpleFilter() : AbstractAspect(QString("SimpleFilter")) {}
		//! Ctor
		AbstractSimpleFilter(const QString& label) : AbstractAspect(label) {}
		//! Default to one input port.
		virtual int inputCount() const { return 1; }
		//! We manage only one output port (don't override unless you really know what you are doing).
		virtual int outputCount() const { return 1; }
		//! Return a pointer to myself on port 0 (don't override unless you really know what you are doing).
		virtual AbstractColumn* output(int port) const {
			return port == 0 ? const_cast<AbstractSimpleFilter*>(this) : 0;
		}
		//! Copy label of input port 0.
		virtual QString columnLabel() const {
			return d_inputs.value(0) ? d_inputs.at(0)->columnLabel() : QString();
		}
		//! Copy plot designation of input port 0.
		virtual SciDAVis::PlotDesignation plotDesignation() const {
			return d_inputs.value(0) ?
				d_inputs.at(0)->plotDesignation() :
				SciDAVis::noDesignation;
		}
		//! Return the data type of the column
		virtual SciDAVis::ColumnDataType dataType() const
		{
			Q_ASSERT(d_inputs.value(0) != 0); // calling this function while d_input is empty is a sign of very bad code
			return d_inputs.at(0)->dataType();
		}
		//! Return the column mode
		/**
		 * This function is most used by tables but can also be used
		 * by plots. The column mode specifies how to interpret 
		 * the values in the column additional to the data type.
		 */ 
		virtual SciDAVis::ColumnMode columnMode() const
		{
			Q_ASSERT(d_inputs.value(0) != 0); // calling this function while d_input is empty is a sign of very bad code
			return d_inputs.at(0)->columnMode();
		}
		//! Does nothing (no comment support for filters so far)
		virtual QString columnComment() const { return QString(); }
		//! Set the column label (not supported)
		virtual void setColumnLabel(const QString& label) { Q_UNUSED(label); }
		//! Set the column comment (not supported)
		virtual void setColumnComment(const QString& comment) { Q_UNUSED(comment); }
		//! Return the content of row 'row'.
		/**
		 * Use this only when dataType() is QString
		 */
		virtual QString textAt(int row) const
		{
			Q_ASSERT(d_inputs.value(0) != 0); // calling this function while d_input is empty is a sign of very bad code
			return d_inputs.at(0)->textAt(row);
		}
		//! Return the date part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QDate dateAt(int row) const
		{
			Q_ASSERT(d_inputs.value(0) != 0); // calling this function while d_input is empty is a sign of very bad code
			return d_inputs.at(0)->dateAt(row);
		}
		//! Return the time part of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QTime timeAt(int row) const
		{
			Q_ASSERT(d_inputs.value(0) != 0); // calling this function while d_input is empty is a sign of very bad code
			return d_inputs.at(0)->timeAt(row);
		}
		//! Set the content of row 'row'
		/**
		 * Use this only when dataType() is QDateTime
		 */
		virtual QDateTime dateTimeAt(int row) const
		{
			Q_ASSERT(d_inputs.value(0) != 0); // calling this function while d_input is empty is a sign of very bad code
			return d_inputs.at(0)->dateTimeAt(row);
		}
		//! Return the double value in row 'row'
		/**
		 * Use this only when dataType() is double
		 */
		virtual double valueAt(int row) const
		{
			Q_ASSERT(d_inputs.value(0) != 0); // calling this function while d_input is empty is a sign of very bad code
			return d_inputs.at(0)->valueAt(row);
		}
		// TODO: Implement these functions in a decent way when integrating it into the aspect framework
		//! \name aspect related functions
		//@{
		//! Return my parent Aspect or 0 if I currently don't have one.
		virtual AbstractAspect *parentAspect() const { return 0; }
		//! Return the Project this Aspect belongs to, or 0 if it is currently not part of one.
		virtual Project *project() const { return 0; }
		//! Return the path that leads from the top-most Aspect (usually a Project) to me.
		virtual QString path() const { return QString(); }
		//! Construct a standard view on me.
		/**
		 * If a parent is specified, the view is added to it as a child widget and the parent takes over
		 * ownership. If no parent is given, the caller receives ownership of the view.
		 * 
		 * This method may be called multiple times during the life time of an Aspect, or it might not get
		 * called at all. Aspects must not depend on the existence of a view for their operation.
		 */
		virtual QWidget *view(QWidget *parent = 0) { Q_UNUSED(parent) return 0; }
		//! Remove me from my parent's list of children.
		virtual void remove() {}
		//! Return the undo stack of the Project, or 0 if this Aspect is not part of a Project.
		virtual QUndoStack *undoStack() const { return 0; }
		//@}

		//!\name assuming a 1:1 correspondence between input and output rows
		//@{
		virtual int rowCount() const {
			return d_inputs.value(0) ? d_inputs.at(0)->rowCount() : 0;
		}
		virtual QList< Interval<int> > dependentRows(Interval<int> input_range) const { return QList< Interval<int> >() << input_range; }
		//@}

		// TODO: Implement commands
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
			emit d_sender->maskingAboutToChange(this);	
			d_masking.clear();
			emit d_sender->maskingChanged(this);	
		}
		//! Set an interval masked
		/**
		 * \param i the interval
		 * \param mask true: mask, false: unmask
		 */ 
		virtual void setMasked(Interval<int> i, bool mask = true)
		{
			emit d_sender->maskingAboutToChange(this);	
			d_masking.setValue(i, mask);
			emit d_sender->maskingChanged(this);	
		}
		//! Overloaded function for convenience
		virtual void setMasked(int row, bool mask = true) { setMasked(Interval<int>(row,row), mask); }
		//@}

		// TODO: Implement this in the derived classes
		//! See QMetaObject::className().
		virtual const char* className() const { return "AbstractSimpleFilter"; }
		// TODO: Fix this
		//! See QObject::inherits().
		virtual bool inherits(const char *class_name) const { return false; }

	protected:
		IntervalAttribute<bool> d_masking;

		//!\name signal handlers
		//@{
		// TODO: determine how to handle description changes
		virtual void inputDescriptionAboutToChange(AbstractColumn*) { /*emit descriptionAboutToChange(this);*/ }
		virtual void inputDescriptionChanged(AbstractColumn*) { /*emit descriptionChanged(this);*/ }
		virtual void inputPlotDesignationAboutToChange(AbstractColumn*) { emit d_sender->plotDesignationAboutToChange(this); }
		virtual void inputPlotDesignationChanged(AbstractColumn*) { emit d_sender->plotDesignationChanged(this); }
		virtual void inputDataAboutToChange(AbstractColumn*) { emit d_sender->dataAboutToChange(this); }
		virtual void inputDataChanged(AbstractColumn*) { emit d_sender->dataChanged(this); }

		virtual void inputRowsAboutToBeInserted(AbstractColumn*, Interval<int> range) {
			foreach(Interval<int> output_range, dependentRows(range))
				emit d_sender->rowsAboutToBeInserted(this, output_range.start(), output_range.size());
		}
		virtual void inputRowsInserted(AbstractColumn*, Interval<int> range) {
			foreach(Interval<int> output_range, dependentRows(range))
				emit d_sender->rowsInserted(this, output_range.start(), output_range.size());
		}
		virtual void inputRowsAboutToBeDeleted(AbstractColumn*, Interval<int> range) {
			foreach(Interval<int> output_range, dependentRows(range))
				emit d_sender->rowsAboutToBeDeleted(this, output_range.start(), output_range.size());
		}
		virtual void inputRowsDeleted(AbstractColumn*, Interval<int> range) {
			foreach(Interval<int> output_range, dependentRows(range))
				emit d_sender->rowsDeleted(this, output_range.start(), output_range.size());
		}
		//@}
};

#endif // ifndef ABSTRACT_SIMPLE_FILTER

