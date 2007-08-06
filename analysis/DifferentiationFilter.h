/***************************************************************************
    File                 : DifferentiationFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Calculates numerical derivatives.

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
#ifndef DIFFERENTIATION_FILTER_H
#define DIFFERENTIATION_FILTER_H

#include "AbstractFilter.h"
#include "TruncationFilter.h"

/**
 * \brief Calculates numerical derivatives.
 *
 * The data provided to the second input port is considered as a function of
 * the data provided to the first input port. The numerical derivative is
 * computed and returned on the second output port. The first output port
 * yields the first input port truncated to the range for which the numerical
 * derivative is defined (the minimum of the row numbers of both input ports
 * minus the two end points).
 */
class DifferentiationFilter : public AbstractDoubleDataSource, public AbstractFilter {
	Q_OBJECT

// Filter interface
	public:
		virtual int numInputs() const { return 2; }
		virtual int numOutputs() const { return 2; }
		virtual AbstractDataSource* output(int port) const {
			switch(port) {
				case 0: return const_cast<TruncationFilter<double>*>(&d_x_truncator);
				case 1: return const_cast<DifferentiationFilter*>(this);
				default: return 0;
			}
		}
		virtual QString inputLabel(int port) const {
			return port == 0 ? "X" : "Y";
		}
	protected:
		virtual bool inputAcceptable(int, AbstractDataSource *source) {
			return source->inherits("AbstractDoubleDataSource");
		}
		virtual void inputDescriptionAboutToChange(AbstractDataSource*) { emit descriptionAboutToChange(this); }
		virtual void inputDescriptionChanged(AbstractDataSource*) { emit descriptionChanged(this); }
		virtual void inputPlotDesignationAboutToChange(AbstractDataSource*) { emit plotDesignationAboutToChange(this); }
		virtual void inputPlotDesignationChanged(AbstractDataSource*) { emit plotDesignationChanged(this); }
		virtual void inputDataAboutToChange(AbstractDataSource*) { emit dataAboutToChange(this); }
		virtual void inputDataChanged(AbstractDataSource* source) {
			emit dataChanged(this);
			d_x_truncator.setNumRows(rowCount());
			if (d_inputs.indexOf(source) == 1) return;
			d_x_truncator.input(0, source);
			d_x_truncator.setStartSkip(1);
		}

// DataSource interface
	public:
		virtual int rowCount() const {
			if (!d_inputs.value(0) || !d_inputs.value(1)) return 0;
			return qMin(d_inputs[0]->rowCount(), d_inputs[1]->rowCount()) - 2;
		}
		virtual double valueAt(int row) const {
			if (row<0 || row>=rowCount()) return 0;
			AbstractDoubleDataSource *x = static_cast<AbstractDoubleDataSource*>(d_inputs[0]);
			AbstractDoubleDataSource *y = static_cast<AbstractDoubleDataSource*>(d_inputs[1]);
			double d1 = (y->valueAt(row+1)-y->valueAt(row))   / (x->valueAt(row+1)-x->valueAt(row));
			double d2 = (y->valueAt(row+2)-y->valueAt(row+1)) / (x->valueAt(row+2)-x->valueAt(row+1));
			return 0.5*(d1 + d2);
		}
		virtual QString label() const {
			return d_inputs.value(0) && d_inputs.value(1) ?
				QString("d(%1)/d(%2)").arg(d_inputs[1]->label()).arg(d_inputs[0]->label()) :
				QString();
		}
		virtual QString comment() const {
			return d_inputs.value(1) ? tr("Derivative of %1").arg(d_inputs[1]->label()) :
				QString(); }
		virtual PlotDesignation plotDesignation() const { return Y; }

// specific to this class
	private:
		TruncationFilter<double> d_x_truncator;
};

#endif // ifndef DIFFERENTIATION_FILTER_H
