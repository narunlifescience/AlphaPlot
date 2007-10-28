/***************************************************************************
    File                 : AbstractFilter.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
    Description          : Base class for all analysis operations.

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
#include "AbstractFilter.h"

void AbstractFilterWrapper::inputDescriptionAboutToChange(AbstractColumn * source)
{
	d_parent->inputDescriptionAboutToChange(source);
}
void AbstractFilterWrapper::inputDescriptionChanged(AbstractColumn * source)
{
	d_parent->inputDescriptionChanged(source);
}
void AbstractFilterWrapper::inputPlotDesignationAboutToChange(AbstractColumn * source)
{
	d_parent->inputPlotDesignationAboutToChange(source);
}
void AbstractFilterWrapper::inputPlotDesignationChanged(AbstractColumn * source)
{
	d_parent->inputPlotDesignationChanged(source);
}
void AbstractFilterWrapper::inputDataAboutToChange(AbstractColumn * source)
{
	d_parent->inputDataAboutToChange(source);
}
void AbstractFilterWrapper::inputDataChanged(AbstractColumn * source)
{
	d_parent->inputDataChanged(source);
}
void AbstractFilterWrapper::inputAboutToBeReplaced(AbstractColumn * source, shared_ptr<AbstractColumn> replacement)
{
	d_parent->inputAboutToBeReplaced(source, replacement);
}
void AbstractFilterWrapper::inputRowsAboutToBeInserted(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsAboutToBeInserted(source, before, count);
}
void AbstractFilterWrapper::inputRowsInserted(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsInserted(source, before, count);
}
void AbstractFilterWrapper::inputRowsAboutToBeDeleted(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsAboutToBeDeleted(source, before, count);
}
void AbstractFilterWrapper::inputRowsDeleted(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsDeleted(source, before, count);
}
void AbstractFilterWrapper::inputValidityAboutToChange(AbstractColumn *source)
{
	d_parent->inputValidityAboutToChange(source);
}
void AbstractFilterWrapper::inputValidityChanged(AbstractColumn *source)
{
	d_parent->inputValidityChanged(source);
}
void AbstractFilterWrapper::inputMaskingAboutToChange(AbstractColumn *source)
{
	d_parent->inputMaskingAboutToChange(source);
}
void AbstractFilterWrapper::inputMaskingChanged(AbstractColumn *source)
{
	d_parent->inputMaskingChanged(source);
}
void AbstractFilterWrapper::inputAboutToBeDestroyed(AbstractColumn *source)
{
	d_parent->inputAboutToBeDestroyed(source);
}

bool AbstractFilter::input(int port, shared_ptr<AbstractColumn> source)
{
	if (port<0 || (inputCount()>=0 && port>=inputCount())) return false;
	if (source && !inputAcceptable(port, source.get())) return false;
	if (d_inputs.size() <= port) d_inputs.resize(port+1);
	shared_ptr<AbstractColumn> old_input = d_inputs.value(port);
	if (old_input)  // disconnect the old input's signals
		old_input->abstractColumnSignalEmitter()->disconnect(d_wrapper); // disconnect all signals
	// replace input, notifying the filter implementation of the changes
	inputDescriptionAboutToChange(old_input.get());
	inputPlotDesignationAboutToChange(old_input.get());
	inputDataAboutToChange(old_input.get());
	if (!source)
		inputAboutToBeDisconnected(old_input.get());
	d_inputs[port] = source;
	if (source) { // we have a new source
		inputDescriptionChanged(source.get());
		inputPlotDesignationChanged(source.get());
		inputDataChanged(source.get());
		// connect the source's signals
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(descriptionAboutToChange(AbstractColumn *)),
				d_wrapper, SLOT(inputDescriptionAboutToChange(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(descriptionChanged(AbstractColumn *)),
				d_wrapper, SLOT(inputDescriptionChanged(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(plotDesignationAboutToChange(AbstractColumn *)),
				d_wrapper, SLOT(inputPlotDesignationAboutToChange(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(plotDesignationChanged(AbstractColumn *)),
				d_wrapper, SLOT(inputPlotDesignationChanged(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(dataAboutToChange(AbstractColumn *)),
				d_wrapper, SLOT(inputDataAboutToChange(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(dataChanged(AbstractColumn *)),
				d_wrapper, SLOT(inputDataChanged(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(aboutToBeReplaced(AbstractColumn *,shared_ptr<AbstractColumn>)),
				d_wrapper, SLOT(inputAboutToBeReplaced(AbstractColumn *,shared_ptr<AbstractColumn>)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(aboutToBeDestroyed(AbstractColumn *)),
				d_wrapper, SLOT(inputAboutToBeDestroyed(AbstractColumn *)));
	} else { // source==0, that is, the input port has been disconnected
		// try to shrink d_inputs
		int num_connected_inputs = d_inputs.size();
		while ( d_inputs.at(num_connected_inputs-1) == 0 )
		{
			num_connected_inputs--;
			if(!num_connected_inputs) break;
		}
		d_inputs.resize(num_connected_inputs);
	}
	return true;
}

bool AbstractFilter::input(shared_ptr<AbstractFilter> sources)
{
	if (!sources) return false;
	bool result = true;
	for (int i=0; i<sources->outputCount(); i++)
		if (!input(i, sources->output(i)))
			result = false;
	return result;
}

QString AbstractFilter::inputLabel(int port) const
{
	return QObject::tr("In%1").arg(port + 1);
}

void AbstractFilter::inputDescriptionAboutToChange(AbstractColumn * source)
{
	inputDescriptionAboutToChange(indexOf(source));
}
void AbstractFilter::inputDescriptionChanged(AbstractColumn * source)
{
	inputDescriptionChanged(indexOf(source));
}
void AbstractFilter::inputPlotDesignationAboutToChange(AbstractColumn * source)
{
	inputPlotDesignationAboutToChange(indexOf(source));
}
void AbstractFilter::inputPlotDesignationChanged(AbstractColumn * source)
{
	inputPlotDesignationChanged(indexOf(source));
}
void AbstractFilter::inputDataAboutToChange(AbstractColumn * source)
{
	inputDataAboutToChange(indexOf(source));
}
void AbstractFilter::inputDataChanged(AbstractColumn * source)
{
	inputDataChanged(indexOf(source));
}
void AbstractFilter::inputAboutToBeReplaced(AbstractColumn * source, shared_ptr<AbstractColumn> replacement)
{
	input(indexOf(source), replacement);
}

