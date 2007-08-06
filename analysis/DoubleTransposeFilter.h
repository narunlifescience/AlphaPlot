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
		DoubleTransposeFilter(bool greedy) : d_greedy(greedy) {}
		virtual ~DoubleTransposeFilter() {
			foreach(OutputColumn *o, d_output_columns)
				delete o;
		}
		virtual int numInputs() const { return -1; }
		virtual int numOutputs() const {
			if (!d_inputs.value(0)) return 0;
			int result = -1;
			foreach(AbstractDataSource *i, d_inputs) {
				if (!i) continue;
				int n;
				if (d_greedy) {
					if ((n = i->rowCount()) > result)
						result = n;
				} else {
					if ((n = i->rowCount()) < result || result == -1)
						result = n;
				}
			}
			return result;
		}
		virtual AbstractDataSource *output(int port) const { return d_output_columns.value(port); }

	protected:
		virtual bool inputAcceptable(int, AbstractDataSource *source) {
			return source->inherits("AbstractDoubleDataSource");
		}
		virtual void inputDataAboutToChange(AbstractDataSource*) {
			foreach(OutputColumn *o, d_output_columns)
				o->dataAboutToChange(o);
		}
		virtual void inputDataChanged(AbstractDataSource*) {
			int old_num_columns = d_output_columns.size();
			int new_num_columns = numOutputs();
			if (new_num_columns > old_num_columns) {
				d_output_columns.resize(new_num_columns);
				for (int i = old_num_columns; i < new_num_columns; i++)
					d_output_columns[i] = new OutputColumn(this, i);
			} else if (new_num_columns < old_num_columns) {
				for (int i = new_num_columns; i < old_num_columns; i++)
					delete d_output_columns.at(i);
				d_output_columns.resize(new_num_columns);
			}
			foreach(OutputColumn *o, d_output_columns)
				o->dataChanged(o);
		}

	private:
		bool d_greedy;
		class OutputColumn : public AbstractDoubleDataSource {
			public:
				OutputColumn(DoubleTransposeFilter *parent, int row) : d_parent(parent), d_row(row) {}
				virtual int rowCount() const { return d_parent->d_inputs.size(); }
				virtual double valueAt(int col) const {
					return isRowValid(col) ?
						static_cast<AbstractDoubleDataSource*>(d_parent->d_inputs.at(col))->valueAt(d_row) :
						0;
				}
				virtual bool isRowValid(int col) const {
					return d_parent->d_inputs.value(col) && d_row < d_parent->d_inputs.at(col)->rowCount();
				}
				virtual QString label() const { return QString::number(d_row + 1); }
				virtual PlotDesignation plotDesignation() const { return noDesignation; }
			private:
				friend class DoubleTransposeFilter;
				DoubleTransposeFilter *d_parent;
				int d_row;
		};
		QVector<OutputColumn*> d_output_columns;
};

#endif // ifndef DOUBLE_TRANSPOSE_FILTER_H

