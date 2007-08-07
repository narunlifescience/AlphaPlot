/***************************************************************************
    File                 : StatisticsFilter.cpp
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
#include "StatisticsFilter.h"
#include <math.h>

StatisticsFilter::StatisticsFilter()
{
	for (int i=0; i<2; i++)
		d_strings[i] = new StringStatisticsColumn(this, (StatItem)i);
	for (int i=2; i<11; i++)
		d_doubles[i-2] = new DoubleStatisticsColumn(this, (StatItem)i);
}

StatisticsFilter::~StatisticsFilter()
{
	for (int i=0; i<2; i++)
		delete d_strings[i];
	for (int i=0; i<9; i++)
		delete d_doubles[i];
}

AbstractDataSource *StatisticsFilter::output(int port) const
{
	if (port < 0 || port >= numOutputs()) return 0;
	if (port <= 1) return d_strings[port];
	return d_doubles[port-2];
}

int StatisticsFilter::rowCount() const
{
	int result = 0;
	foreach(AbstractDataSource* i, d_inputs)
		if (i) result++;
	return result;
}

void StatisticsFilter::inputDataAboutToChange(AbstractDataSource*)
{
	// relay signal to all outputs but the first (which only holds the data source labels)
	d_strings[1]->dataAboutToChange(d_strings[1]);
	for (int i=0; i<9; i++)
		d_doubles[i]->dataAboutToChange(d_doubles[i]);
}

void StatisticsFilter::inputAboutToBeDisconnected(AbstractDataSource *source)
{
	if (d_inputs.indexOf(source)+1 == d_s.size())
		d_s.resize(d_s.size()-1);
}

void StatisticsFilter::inputDataChanged(int port)
{
	if (port >= d_s.size()) d_s.resize(port+1);
	Statistics *s = &d_s[port];
	QList< Interval<int> > valid_intervals = Interval<int>(0,d_inputs.at(port)->rowCount()-1) - d_inputs.at(port)->invalidIntervals();
	if (valid_intervals.isEmpty()) {
		s->first_valid_row = -1;
		return;
	}

	// initialize some entries for the following iteration
	s->sum = 0; s->N = 0;
	s->min_index = s->max_index = s->first_valid_row = s->last_valid_row = valid_intervals.at(0).start();
	s->min = s->max = static_cast<AbstractDoubleDataSource*>(d_inputs.at(port))->valueAt(s->first_valid_row);

	// iterate over all valid rows, determining first_valid_row, last_valid_row, N, min, max and sum
	foreach(Interval<int> i, valid_intervals) {
		if (i.start() < s->first_valid_row)
			s->first_valid_row = i.start();
		if (i.end() > s->last_valid_row)
			s->last_valid_row = i.end();
		for (int row = i.start(); row <= i.end(); row++) {
			s->N++;
			double val = static_cast<AbstractDoubleDataSource*>(d_inputs.at(port))->valueAt(row);
			if (val < s->min) {
				s->min = val;
				s->min_index = row;
			} else if (val > s->max) {
				s->max = val;
				s->max_index = row;
			}
			s->sum += val;
		}
	}

	// iterate a second time, using the now-known mean to compute the variance
	double mean = s->sum / double(s->N);
	s->variance = 0;
	foreach(Interval<int> i, valid_intervals)
		for (int row = i.start(); row <= i.end(); row++)
			s->variance += pow(static_cast<AbstractDoubleDataSource*>(d_inputs.at(port))->valueAt(row) - mean, 2);
	s->variance /= double(s->N);

	// emit signals on all output ports that might have changed
	d_strings[1]->dataChanged(d_strings[1]);
	for (int i=0; i<9; i++)
		d_doubles[i]->dataChanged(d_doubles[i]);
}

double StatisticsFilter::DoubleStatisticsColumn::valueAt(int row) const
{
	if (row<0 || row>=d_parent->rowCount()) return 0;
	const Statistics *s = &(d_parent->d_s.at(row));
	switch(d_item) {
		case Mean: return s->sum / double(s->N);
		case Sigma: return sqrt(s->variance);
		case Variance: return s->variance;
		case Sum: return s->sum;
		case iMax: return s->max_index + 1;
		case Max: return s->max;
		case iMin: return s->min_index + 1;
		case Min: return s->min;
		case N: return s->N;
		default: return 0;
	}
}

QString StatisticsFilter::DoubleStatisticsColumn::label() const
{ 
	switch(d_item) {
		case Mean: return tr("Mean");
		case Sigma: return tr("StandardDev");
		case Variance: return tr("Variance");
		case Sum: return tr("Sum");
		case iMax: return tr("iMax");
		case Max: return tr("Max");
		case iMin: return tr("iMin");
		case Min: return tr("Min");
		case N: return tr("N");
		default: return QString();
	}
}

QString StatisticsFilter::StringStatisticsColumn::textAt(int row) const
{
	const Statistics *s = &(d_parent->d_s.at(row));
	switch(d_item) {
		case Label:
			return d_parent->d_inputs.value(row) ?
				d_parent->d_inputs[row]->label() :
				QString();
		case Rows: return QString("[%1,%2]").arg(s->first_valid_row + 1).arg(s->last_valid_row+1);
		default: return QString();
	}
}

QString StatisticsFilter::StringStatisticsColumn::label() const
{
	switch(d_item) {
		case Label: return tr("Name");
		case Rows: return tr("Rows");
		default: return QString();
	}
}

