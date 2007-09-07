#include "AbstractFilter.h"

void AbstractFilterSlotMachine::inputDescriptionAboutToChange(AbstractColumn* source)
{
	d_parent->inputDescriptionAboutToChange(source);
}
void AbstractFilterSlotMachine::inputDescriptionChanged(AbstractColumn* source)
{
	d_parent->inputDescriptionChanged(source);
}
void AbstractFilterSlotMachine::inputPlotDesignationAboutToChange(AbstractColumn* source)
{
	d_parent->inputPlotDesignationAboutToChange(source);
}
void AbstractFilterSlotMachine::inputPlotDesignationChanged(AbstractColumn* source)
{
	d_parent->inputPlotDesignationChanged(source);
}
void AbstractFilterSlotMachine::inputDataAboutToChange(AbstractColumn* source)
{
	d_parent->inputDataAboutToChange(source);
}
void AbstractFilterSlotMachine::inputDataChanged(AbstractColumn* source)
{
	d_parent->inputDataChanged(source);
}
void AbstractFilterSlotMachine::inputAboutToBeReplaced(AbstractColumn* source, AbstractColumn* replacement)
{
	d_parent->inputAboutToBeReplaced(source, replacement);
}
void AbstractFilterSlotMachine::inputRowsAboutToBeInserted(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsAboutToBeInserted(source, before, count);
}
void AbstractFilterSlotMachine::inputRowsInserted(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsInserted(source, before, count);
}
void AbstractFilterSlotMachine::inputRowsAboutToBeDeleted(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsAboutToBeDeleted(source, before, count);
}
void AbstractFilterSlotMachine::inputRowsDeleted(AbstractColumn *source, int before, int count)
{
	d_parent->inputRowsDeleted(source, before, count);
}
void AbstractFilterSlotMachine::inputValidityAboutToChange(AbstractColumn *source)
{
	d_parent->inputValidityAboutToChange(source);
}
void AbstractFilterSlotMachine::inputValidityChanged(AbstractColumn *source)
{
	d_parent->inputValidityChanged(source);
}
void AbstractFilterSlotMachine::inputMaskingAboutToChange(AbstractColumn *source)
{
	d_parent->inputMaskingAboutToChange(source);
}
void AbstractFilterSlotMachine::inputMaskingChanged(AbstractColumn *source)
{
	d_parent->inputMaskingChanged(source);
}
void AbstractFilterSlotMachine::inputAboutToBeDestroyed(AbstractColumn *source)
{
	d_parent->inputAboutToBeDestroyed(source);
}

bool AbstractFilter::input(int port, AbstractColumn *source)
{
	if (port<0 || (inputCount()>=0 && port>=inputCount())) return false;
	if (source && !inputAcceptable(port, source)) return false;
	if (d_inputs.size() <= port) d_inputs.resize(port+1);
	AbstractColumn *old_input = d_inputs.value(port);
	if (old_input)  // disconnect the old input's signals
		old_input->signalSender()->disconnect(&d_slot_machine); // disconnect all signals
	// replace input, notifying the filter implementation of the changes
	inputDescriptionAboutToChange(old_input);
	inputPlotDesignationAboutToChange(old_input);
	inputDataAboutToChange(old_input);
	if (!source)
		inputAboutToBeDisconnected(old_input);
	d_inputs[port] = source;
	if (source) { // we have a new source
		inputDescriptionChanged(source);
		inputPlotDesignationChanged(source);
		inputDataChanged(source);
		// connect the source's signals
		/*QObject::connect(source->signalSender(), SIGNAL(descriptionAboutToChange(AbstractColumn*)),
				&d_slot_machine, SLOT(inputDescriptionAboutToChange(AbstractColumn*)));
		QObject::connect(source->signalSender(), SIGNAL(descriptionChanged(AbstractColumn*)),
				&d_slot_machine, SLOT(inputDescriptionChanged(AbstractColumn*)));*/
		QObject::connect(source->signalSender(), SIGNAL(plotDesignationAboutToChange(AbstractColumn*)),
				&d_slot_machine, SLOT(inputPlotDesignationAboutToChange(AbstractColumn*)));
		QObject::connect(source->signalSender(), SIGNAL(plotDesignationChanged(AbstractColumn*)),
				&d_slot_machine, SLOT(inputPlotDesignationChanged(AbstractColumn*)));
		QObject::connect(source->signalSender(), SIGNAL(dataAboutToChange(AbstractColumn*)),
				&d_slot_machine, SLOT(inputDataAboutToChange(AbstractColumn*)));
		QObject::connect(source->signalSender(), SIGNAL(dataChanged(AbstractColumn*)),
				&d_slot_machine, SLOT(inputDataChanged(AbstractColumn*)));
		QObject::connect(source->signalSender(), SIGNAL(aboutToBeReplaced(AbstractColumn*,AbstractColumn*)),
				&d_slot_machine, SLOT(inputAboutToBeReplaced(AbstractColumn*,AbstractColumn*)));
		QObject::connect(source->signalSender(), SIGNAL(aboutToBeDestroyed(AbstractColumn*)),
				&d_slot_machine, SLOT(inputAboutToBeDestroyed(AbstractColumn*)));
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

void AbstractFilter::inputDescriptionAboutToChange(AbstractColumn* source)
{
	inputDescriptionAboutToChange(d_inputs.indexOf(source));
}
void AbstractFilter::inputDescriptionChanged(AbstractColumn* source)
{
	inputDescriptionChanged(d_inputs.indexOf(source));
}
void AbstractFilter::inputPlotDesignationAboutToChange(AbstractColumn* source)
{
	inputPlotDesignationAboutToChange(d_inputs.indexOf(source));
}
void AbstractFilter::inputPlotDesignationChanged(AbstractColumn* source)
{
	inputPlotDesignationChanged(d_inputs.indexOf(source));
}
void AbstractFilter::inputDataAboutToChange(AbstractColumn* source)
{
	inputDataAboutToChange(d_inputs.indexOf(source));
}
void AbstractFilter::inputDataChanged(AbstractColumn* source)
{
	inputDataChanged(d_inputs.indexOf(source));
}
void AbstractFilter::inputAboutToBeReplaced(AbstractColumn* source, AbstractColumn* replacement)
{
	input(d_inputs.indexOf(source), replacement);
}

