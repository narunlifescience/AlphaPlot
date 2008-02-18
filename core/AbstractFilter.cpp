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

bool AbstractFilter::input(int port, AbstractColumn* source)
{
	if (port<0 || (inputCount()>=0 && port>=inputCount())) return false;
	if (source && !inputAcceptable(port, source)) return false;
	if (port >= d_inputs.size()) d_inputs.resize(port+1);
	AbstractColumn* old_input = d_inputs.value(port);
	if (source == old_input) return true;
	if (old_input) 
	{
		old_input->disconnect(this); // disconnect all signals
		// replace input, notifying the filter implementation of the changes
		inputDescriptionAboutToChange(old_input);
		inputPlotDesignationAboutToChange(old_input);
		inputMaskingAboutToChange(old_input);
		inputDataAboutToChange(old_input);
		if(source && source->columnMode() != old_input->columnMode())
			inputModeAboutToChange(old_input);
	}
	if (!source)
		inputAboutToBeDisconnected(old_input);
	d_inputs[port] = source;
	if (source) { // we have a new source
		if(old_input && source->columnMode() != old_input->columnMode())
			inputModeAboutToChange(source);
		inputDataChanged(source);
		inputMaskingChanged(source);
		inputPlotDesignationChanged(source);
		inputDescriptionChanged(source);
		// connect the source's signals
		QObject::connect(source, SIGNAL(aspectDescriptionAboutToChange(AbstractAspect *)),
				this, SLOT(inputDescriptionAboutToChange(AbstractAspect *)));
		QObject::connect(source, SIGNAL(aspectDescriptionChanged(AbstractAspect *)),
				this, SLOT(inputDescriptionChanged(AbstractAspect *)));
		QObject::connect(source, SIGNAL(plotDesignationAboutToChange(AbstractColumn *)),
				this, SLOT(inputPlotDesignationAboutToChange(AbstractColumn *)));
		QObject::connect(source, SIGNAL(plotDesignationChanged(AbstractColumn *)),
				this, SLOT(inputPlotDesignationChanged(AbstractColumn *)));
		QObject::connect(source, SIGNAL(modeAboutToChange(AbstractColumn *)),
				this, SLOT(inputModeAboutToChange(AbstractColumn *)));
		QObject::connect(source, SIGNAL(modeChanged(AbstractColumn *)),
				this, SLOT(inputModeChanged(AbstractColumn *)));
		QObject::connect(source, SIGNAL(dataAboutToChange(AbstractColumn *)),
				this, SLOT(inputDataAboutToChange(AbstractColumn *)));
		QObject::connect(source, SIGNAL(dataChanged(AbstractColumn *)),
				this, SLOT(inputDataChanged(AbstractColumn *)));
		QObject::connect(source, SIGNAL(aboutToBeReplaced(AbstractColumn *,AbstractColumn*)),
				this, SLOT(inputAboutToBeReplaced(AbstractColumn *,AbstractColumn*)));
		QObject::connect(source, 
			SIGNAL(rowsAboutToBeInserted(AbstractColumn *,int,int)),
			this, SLOT(inputRowsAboutToBeInserted(AbstractColumn *,int,int)));
		QObject::connect(source, 
			SIGNAL(rowsInserted(AbstractColumn *,int,int)),
			this, SLOT(inputRowsInserted(AbstractColumn *,int,int)));
		QObject::connect(source, 
			SIGNAL(rowsAboutToBeRemoved(AbstractColumn *,int,int)),
			this, SLOT(inputRowsAboutToBeRemoved(AbstractColumn *,int,int)));
		QObject::connect(source, 
			SIGNAL(rowsRemoved(AbstractColumn *, int, int)),
			this, SLOT(inputRowsRemoved(AbstractColumn *,int,int)));
		QObject::connect(source, SIGNAL(maskingAboutToChange(AbstractColumn *)),
				this, SLOT(inputMaskingAboutToChange(AbstractColumn *)));
		QObject::connect(source, SIGNAL(maskingChanged(AbstractColumn *)),
				this, SLOT(inputMaskingChanged(AbstractColumn *)));
		QObject::connect(source, SIGNAL(aboutToBeDestroyed(AbstractColumn *)),
				this, SLOT(inputAboutToBeDestroyed(AbstractColumn *)));
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

bool AbstractFilter::input(AbstractFilter* sources)
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

void AbstractFilter::inputAboutToBeReplaced(AbstractColumn * source, AbstractColumn* replacement)
{
	input(portIndexOf(source), replacement);
}

