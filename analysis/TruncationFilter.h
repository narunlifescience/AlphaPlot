/***************************************************************************
    File                 : TruncationFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : A simple filter which truncates its (single) input.

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
#ifndef TRUNCATION_FILTER
#define TRUNCATION_FILTER

#include "AbstractSimpleFilter.h"
#include <QDateTime>

template<class T> class TruncationFilterBase : public AbstractSimpleFilter<T>
{
// specific to this class
	public:
		TruncationFilterBase() : d_start(0), d_num_rows(0) {}
		bool setStartSkip(int n) {
			if (!AbstractSimpleFilter<T>::d_inputs.value(0) || n<0 || n>AbstractSimpleFilter<T>::d_inputs[0]->rowCount()) return false;
			d_start = n;
			return true;
		}
		bool setNumRows(int n) {
			if (!AbstractSimpleFilter<T>::d_inputs.value(0) || n<0 || d_start+n>=AbstractSimpleFilter<T>::d_inputs[0]->rowCount()) return false;
			d_num_rows = n;
			return true;
		}
	protected:
		int d_start, d_num_rows;

// simplified filter interface
	public:
		virtual QString label() const {
			return AbstractSimpleFilter<T>::d_inputs.value(0) ?
				QString("%1 [%2,%3]").arg(AbstractSimpleFilter<T>::d_inputs.at(0)->label()).arg(d_start+1).arg(d_start+d_num_rows) :
				QString();
		}
		virtual int rowCount() const { return d_num_rows; }
		virtual void inputDataChanged(AbstractDataSource* source)  {
			if (!AbstractSimpleFilter<T>::d_inputs.value(0) || d_start+d_num_rows>=AbstractSimpleFilter<T>::d_inputs[0]->rowCount())
				d_num_rows = 0;
			AbstractSimpleFilter<T>::inputDataChanged(source);
		}
};

/**
 * \brief A simple filter which truncates its (single) input.
 *
 * After connecting a data source to its input port, you have to tell TruncationFilter
 * which (continuous) subset of the input to return by calling setStartSkip() and setNumRows().
 *
 * TODO: we don't need to use templates here if we don't use the SimpleFilter interface
 */
template<class T> class TruncationFilter {};

template<> class TruncationFilter<double> : public TruncationFilterBase<double>
{
	protected:
		virtual bool inputAcceptable(int, AbstractDataSource *source) {
			return source->inherits("AbstractDoubleDataSource");
		}
	public:
		virtual double valueAt(int row) const {
			if (row<0 || row>=d_num_rows) return 0;
			return static_cast<AbstractDoubleDataSource*>(TruncationFilterBase<double>::d_inputs[0])->valueAt(d_start + row);
		}
};

template<> class TruncationFilter<QString> : public TruncationFilterBase<QString>
{
	protected:
		virtual bool inputAcceptable(int, AbstractDataSource *source) {
			return source->inherits("AbstractStringDataSource");
		}
	public:
		virtual QString textAt(int row) const {
			if (row<0 || row>=d_num_rows) return 0;
			return static_cast<AbstractStringDataSource*>(TruncationFilterBase<QString>::d_inputs[0])->textAt(d_start + row);
		}
};

template<> class TruncationFilter<QDateTime> : public TruncationFilterBase<QDateTime>
{
	protected:
		virtual bool inputAcceptable(int, AbstractDataSource *source) {
			return source->inherits("AbstractDateTimeDataSource");
		}
	public:
		virtual QDateTime dateTimeAt(int row) const {
			if (row<0 || row>=d_num_rows) return QDateTime();
			return static_cast<AbstractDateTimeDataSource*>(TruncationFilterBase<QDateTime>::d_inputs[0])->dateTimeAt(d_start + row);
		}
		virtual QDate dateAt(int row) const {
			if (row<0 || row>=d_num_rows) return QDate();
			return static_cast<AbstractDateTimeDataSource*>(TruncationFilterBase<QDateTime>::d_inputs[0])->dateAt(d_start + row);
		}
		virtual QTime timeAt(int row) const {
			if (row<0 || row>=d_num_rows) return QTime();
			return static_cast<AbstractDateTimeDataSource*>(TruncationFilterBase<QDateTime>::d_inputs[0])->timeAt(d_start + row);
		}
};

#endif // ifndef TRUNCATION_FILTER
