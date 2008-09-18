/***************************************************************************
    File                 : DoubleTransposeFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Interchange columns (= input ports) and their rows.

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
#ifndef DOUBLE_TRANSPOSE_FILTER_H
#define DOUBLE_TRANSPOSE_FILTER_H

#include "AbstractFilter.h"

/**
 * \brief Interchange columns (= input ports) and their rows.
 *
 * An arbitrary number of inputs is regarded as the columns of a matrix (of double values).
 * The transpose of this matrix is returned column-wise on a number of output ports.
 * Operation can be greedy, meaning that the number of output ports equals the largest
 * number of rows of the inputs (shorter ones being padded with zeroes), or non-greedy,
 * meaning that the number of output ports equals the smallest number of rows of the inputs
 * (loosing some of the data).
 */
class DoubleTransposeFilter : public AbstractFilter {
	public:
		DoubleTransposeFilter(bool greedy) : m_greedy(greedy) {}
		virtual ~DoubleTransposeFilter() {
			foreach(OutputColumn *o, m_output_columns)
				delete o;
		}
		virtual int inputCount() const { return -1; }
		virtual int outputCount() const {
			if (!m_inputs.value(0)) return 0;
			int result = -1;
			foreach(AbstractDataSource *i, m_inputs) {
				if (!i) continue;
				int n;
				if (m_greedy) {
					if ((n = i->rowCount()) > result)
						result = n;
				} else {
					if ((n = i->rowCount()) < result || result == -1)
						result = n;
				}
			}
			return result;
		}
		virtual AbstractDataSource *output(int port) const { return m_output_columns.value(port); }

	protected:
		virtual bool inputAcceptable(int, AbstractDataSource *source) {
			return source->inherits("AbstractDoubleDataSource");
		}
		virtual void inputDataAboutToChange(AbstractDataSource*) {
			foreach(OutputColumn *o, m_output_columns)
				o->dataAboutToChange(o);
		}
		virtual void inputDataChanged(AbstractDataSource*) {
			int old_num_columns = m_output_columns.size();
			int new_num_columns = outputCount();
			if (new_num_columns > old_num_columns) {
				m_output_columns.resize(new_num_columns);
				for (int i = old_num_columns; i < new_num_columns; i++)
					m_output_columns[i] = new OutputColumn(this, i);
			} else if (new_num_columns < old_num_columns) {
				for (int i = new_num_columns; i < old_num_columns; i++)
					delete m_output_columns.at(i);
				m_output_columns.resize(new_num_columns);
			}
			foreach(OutputColumn *o, m_output_columns)
				o->dataChanged(o);
		}

	private:
		bool m_greedy;
		class OutputColumn : public AbstractDoubleDataSource {
			public:
				OutputColumn(DoubleTransposeFilter *parent, int row) : m_parent(parent), m_row(row) {}
				virtual int rowCount() const { return m_parent->m_inputs.size(); }
				virtual double valueAt(int col) const {
					return isRowValid(col) ?
						static_cast<AbstractDoubleDataSource*>(m_parent->m_inputs.at(col))->valueAt(m_row) :
						0;
				}
				virtual bool isRowValid(int col) const {
					return m_parent->m_inputs.value(col) && m_row < m_parent->m_inputs.at(col)->rowCount();
				}
				virtual QString label() const { return QString::number(m_row + 1); }
				virtual PlotDesignation plotDesignation() const { return noDesignation; }
			private:
				friend class DoubleTransposeFilter;
				DoubleTransposeFilter *m_parent;
				int m_row;
		};
		QVector<OutputColumn*> m_output_columns;
};

#endif // ifndef DOUBLE_TRANSPOSE_FILTER_H

