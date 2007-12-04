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
void AbstractFilterWrapper::inputModeAboutToChange(AbstractColumn * source)
{
	d_parent->inputModeAboutToChange(source);
}
void AbstractFilterWrapper::inputModeChanged(AbstractColumn * source)
{
	d_parent->inputModeChanged(source);
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
void AbstractFilterWrapper::inputRowsAboutToBeRemoved(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsAboutToBeRemoved(source, before, count);
}
void AbstractFilterWrapper::inputRowsRemoved(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsRemoved(source, before, count);
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
	if (old_input) 
	{
		old_input->abstractColumnSignalEmitter()->disconnect(d_abstract_filter_wrapper); // disconnect all signals
		// replace input, notifying the filter implementation of the changes
		inputDescriptionAboutToChange(old_input.get());
		inputPlotDesignationAboutToChange(old_input.get());
		inputMaskingAboutToChange(old_input.get());
		inputDataAboutToChange(old_input.get());
		if(source && source->columnMode() != old_input->columnMode())
			inputModeAboutToChange(old_input.get());
	}
	if (!source)
		inputAboutToBeDisconnected(old_input.get());
	d_inputs[port] = source;
	if (source) { // we have a new source
		if(old_input && source->columnMode() != old_input->columnMode())
			inputModeAboutToChange(source.get());
		inputDataChanged(source.get());
		inputMaskingChanged(source.get());
		inputPlotDesignationChanged(source.get());
		inputDescriptionChanged(source.get());
		// connect the source's signals
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(descriptionAboutToChange(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputDescriptionAboutToChange(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(descriptionChanged(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputDescriptionChanged(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(plotDesignationAboutToChange(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputPlotDesignationAboutToChange(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(plotDesignationChanged(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputPlotDesignationChanged(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(modeAboutToChange(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputModeAboutToChange(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(modeChanged(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputModeChanged(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(dataAboutToChange(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputDataAboutToChange(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(dataChanged(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputDataChanged(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(aboutToBeReplaced(AbstractColumn *,shared_ptr<AbstractColumn>)),
				d_abstract_filter_wrapper, SLOT(inputAboutToBeReplaced(AbstractColumn *,shared_ptr<AbstractColumn>)));
		QObject::connect(source->abstractColumnSignalEmitter(), 
			SIGNAL(rowsAboutToBeInserted(AbstractColumn *,int,int)),
			d_abstract_filter_wrapper, SLOT(inputRowsAboutToBeInserted(AbstractColumn *,int,int)));
		QObject::connect(source->abstractColumnSignalEmitter(), 
			SIGNAL(rowsInserted(AbstractColumn *,int,int)),
			d_abstract_filter_wrapper, SLOT(inputRowsInserted(AbstractColumn *,int,int)));
		QObject::connect(source->abstractColumnSignalEmitter(), 
			SIGNAL(rowsAboutToBeRemoved(AbstractColumn *,int,int)),
			d_abstract_filter_wrapper, SLOT(inputRowsAboutToBeRemoved(AbstractColumn *,int,int)));
		QObject::connect(source->abstractColumnSignalEmitter(), 
			SIGNAL(rowsRemoved(AbstractColumn *, int, int)),
			d_abstract_filter_wrapper, SLOT(inputRowsRemoved(AbstractColumn *,int,int)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(maskingAboutToChange(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputMaskingAboutToChange(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(maskingChanged(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputMaskingChanged(AbstractColumn *)));
		QObject::connect(source->abstractColumnSignalEmitter(), SIGNAL(aboutToBeDestroyed(AbstractColumn *)),
				d_abstract_filter_wrapper, SLOT(inputAboutToBeDestroyed(AbstractColumn *)));
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

void AbstractFilter::inputAboutToBeReplaced(AbstractColumn * source, shared_ptr<AbstractColumn> replacement)
{
	input(portIndexOf(source), replacement);
}

