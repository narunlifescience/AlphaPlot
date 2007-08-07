/***************************************************************************
    File                 : StatisticsFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Computes standard statistics on any number of inputs.

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
#ifndef STATISTICS_FILTER_H
#define STATISTICS_FILTER_H

#include "core/AbstractFilter.h"
#include "core/datatypes/AbstractDoubleDataSource.h"
#include "core/datatypes/AbstractStringDataSource.h"

/**
 * \brief Computes standard statistics on any number of inputs.
 *
 * This filter is functionally equivalent to TableStatistics (except that row statics
 * are not implemented yet). It takes any number of inputs, computes statistics on
 * each and returns them on its outputs. Each output port corresponds to one
 * statistical item and provides as many rows as input ports are connected.
 */
class StatisticsFilter : public AbstractFilter {
	public:
		//! Standard constructor.
		StatisticsFilter();
		//! Destructor.
		virtual ~StatisticsFilter();
		//! Accept any number of inputs.
		virtual int numInputs() const { return -1; }
		//! Currently, 11 statistics items are computed (see the private StatItem enum).
		virtual int numOutputs() const { return 11; }
		virtual AbstractDataSource *output(int port) const;
		//! Number of rows = number of inputs that have been provided.
		int rowCount() const;
	protected:
		virtual bool inputAcceptable(int, AbstractDataSource *source) {
			return source->inherits("AbstractDoubleDataSource");
		}
		virtual void inputDescriptionAboutToChange(AbstractDataSource*) { d_strings[0]->dataAboutToChange(d_doubles[0]); }
		virtual void inputDescriptionChanged(AbstractDataSource*) { d_strings[0]->dataChanged(d_doubles[0]); }
		virtual void inputDataAboutToChange(AbstractDataSource*);
		virtual void inputAboutToBeDisconnected(AbstractDataSource*);
		//! This is where the magic happens: data changes on an input port cause the corresponding entry in #d_s to be recomputed.
		virtual void inputDataChanged(int port);
	private:
		//! The values being cached for each input column provided.
		struct Statistics {
			int first_valid_row, last_valid_row, min_index, max_index, N;
			double min, max, sum, variance;
		};
		enum StatItem { Label, Rows, Mean, Sigma, Variance, Sum, iMax, Max, iMin, Min, N };

		//! Implements the double-typed output ports.
		class DoubleStatisticsColumn : public AbstractDoubleDataSource {
			public:
				DoubleStatisticsColumn(const StatisticsFilter *parent, StatItem item) : d_parent(parent), d_item(item) {}
				virtual int rowCount() const { return d_parent->rowCount(); }
				virtual double valueAt(int row) const;
				virtual QString label() const; 
				virtual PlotDesignation plotDesignation() const { return noDesignation; }
			private:
				friend class StatisticsFilter;
				const StatisticsFilter *d_parent;
				StatItem d_item;
		};

		//! Implements the string-typed output ports.
		class StringStatisticsColumn : public AbstractStringDataSource {
			public:
				StringStatisticsColumn(const StatisticsFilter *parent, StatItem item) : d_parent(parent), d_item(item) {}
				virtual int rowCount() const { return d_parent->rowCount(); }
				virtual QString textAt(int row) const;
				virtual QString label() const;
				virtual PlotDesignation plotDesignation() const { return noDesignation; }
			private:
				friend class StatisticsFilter;
				const StatisticsFilter *d_parent;
				StatItem d_item;
		};

		QVector<Statistics> d_s;
		StringStatisticsColumn* d_strings[2];
		DoubleStatisticsColumn* d_doubles[9];
};

#endif // ifndef STATISTICS_FILTER_H

