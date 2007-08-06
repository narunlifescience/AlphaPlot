/***************************************************************************
    File                 : QwtDataFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Wraps two input ports into a QwtData object.

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

#ifndef QWT_DATA_FILTER_H
#define QWT_DATA_FILTER_H

#include "AbstractFilter.h"
#include <qwt_data.h>

/**
 * \brief Wraps two input ports into a QwtData object.
 *
 * Just a few lines of glue code between SciDAVis's data source model and Qwt's (less generic) data model.
 */
class QwtDataFilter : public QwtData, public AbstractFilter
{
	public:
		QwtDataFilter() { d_data[0] = d_data[1] = 0; }
		virtual ~QwtDataFilter() {
			if(d_data[0]) delete d_data[0];
			if(d_data[1]) delete d_data[1];
		}

		//! \name Reimplemented from AbstractFilter
		//@{
		virtual int numInputs() const { return 2; }
		virtual int numOutputs() const { return 0; }
		virtual AbstractDataSource* const output(int) { return 0; }
		virtual QString inputLabel(int port) const {
			return port == 0 ? "X" : "Y";
		}
		//@}

		//! \name Reimplemented from QwtData
		//@{
		virtual friend QwtData *copy() const {
			if(!d_inputs.value(0) || !d_inputs.value(1)) return 0;
			QwtDataFilter *result = new QwtDataFilter();

			result->d_data[0] = new DoubleColumnData(d_inputs.at(0)->rowCount());
			result->d_data[0]->copy(d_inputs.at(0));
			result->input(0, result->d_data[0]);

			result->d_data[1] = new DoubleColumnData(d_inputs.at(1)->rowCount());
			result->d_data[1]->copy(d_inputs.at(1));
			result->input(1, result->d_data[1]);

			return result;
		}
		virtual size_t size() const {
			return (d_inputs.value(0) && d_inputs.value(1)) ?
				qMin(d_inputs.at(0)->rowCount(), d_inputs.at(1)->rowCount()) :
				0;
		}
		virtual double x(size_t i) const {
			return d_inputs.value(0) ? d_inputs.at(0)->valueAt(i) : 0;
		}
		virtual double y(size_t i) const {
			return d_inputs.value(1) ? d_inputs.at(1)->valueAt(i) : 0;
		}
		//@}

	private:
		//! Only used as a result of calling copy().
		DoubleColumnData *d_data[2];
};

#endif // ifndef QWT_DATA_FILTER_H

