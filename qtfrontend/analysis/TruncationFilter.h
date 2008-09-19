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

#include "core/AbstractSimpleFilter.h"
#include <QDateTime>

template<class T> class TruncationFilterBase : public AbstractSimpleFilter<T>
{
// specific to this class
	public:
		TruncationFilterBase() : m_start(0), m_num_rows(0) {}
		bool setStartSkip(int n) {
			if (!AbstractSimpleFilter<T>::m_inputs.value(0) || n<0 || n>AbstractSimpleFilter<T>::m_inputs[0]->rowCount()) return false;
			m_start = n;
			return true;
		}
		bool setNumRows(int n) {
			if (!AbstractSimpleFilter<T>::m_inputs.value(0) || n<0 || m_start+n>=AbstractSimpleFilter<T>::m_inputs[0]->rowCount()) return false;
			m_num_rows = n;
			return true;
		}
	protected:
		int m_start, m_num_rows;

// simplified filter interface
	public:
		virtual QString label() const {
			return AbstractSimpleFilter<T>::m_inputs.value(0) ?
				QString("%1 [%2,%3]").arg(AbstractSimpleFilter<T>::m_inputs.at(0)->label()).arg(m_start+1).arg(m_start+m_num_rows) :
				QString();
		}
		virtual int rowCount() const { return m_num_rows; }
		virtual void inputDataChanged(AbstractDataSource* source)  {
			if (!AbstractSimpleFilter<T>::m_inputs.value(0) || m_start+m_num_rows>=AbstractSimpleFilter<T>::m_inputs[0]->rowCount())
				m_num_rows = 0;
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
			if (row<0 || row>=m_num_rows) return 0;
			return static_cast<AbstractDoubleDataSource*>(TruncationFilterBase<double>::m_inputs[0])->valueAt(m_start + row);
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
			if (row<0 || row>=m_num_rows) return 0;
			return static_cast<AbstractStringDataSource*>(TruncationFilterBase<QString>::m_inputs[0])->textAt(m_start + row);
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
			if (row<0 || row>=m_num_rows) return QDateTime();
			return static_cast<AbstractDateTimeDataSource*>(TruncationFilterBase<QDateTime>::m_inputs[0])->dateTimeAt(m_start + row);
		}
		virtual QDate dateAt(int row) const {
			if (row<0 || row>=m_num_rows) return QDate();
			return static_cast<AbstractDateTimeDataSource*>(TruncationFilterBase<QDateTime>::m_inputs[0])->dateAt(m_start + row);
		}
		virtual QTime timeAt(int row) const {
			if (row<0 || row>=m_num_rows) return QTime();
			return static_cast<AbstractDateTimeDataSource*>(TruncationFilterBase<QDateTime>::m_inputs[0])->timeAt(m_start + row);
		}
};

#endif // ifndef TRUNCATION_FILTER
