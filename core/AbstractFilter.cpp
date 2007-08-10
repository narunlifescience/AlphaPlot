#include "AbstractFilter.h"

void AbstractFilterSlotMachine::inputDescriptionAboutToChange(AbstractDataSource* source)
{
	d_parent->inputDescriptionAboutToChange(source);
}
void AbstractFilterSlotMachine::inputDescriptionChanged(AbstractDataSource* source)
{
	d_parent->inputDescriptionChanged(source);
}
void AbstractFilterSlotMachine::inputPlotDesignationAboutToChange(AbstractDataSource* source)
{
	d_parent->inputPlotDesignationAboutToChange(source);
}
void AbstractFilterSlotMachine::inputPlotDesignationChanged(AbstractDataSource* source)
{
	d_parent->inputPlotDesignationChanged(source);
}
void AbstractFilterSlotMachine::inputDataAboutToChange(AbstractDataSource* source)
{
	d_parent->inputDataAboutToChange(source);
}
void AbstractFilterSlotMachine::inputDataChanged(AbstractDataSource* source)
{
	d_parent->inputDataChanged(source);
}
void AbstractFilterSlotMachine::inputAboutToBeReplaced(AbstractDataSource* source, AbstractDataSource* replacement)
{
	d_parent->inputAboutToBeReplaced(source, replacement);
}
void AbstractFilterSlotMachine::inputRowsAboutToBeInserted(AbstractDataSource *source, int before, int count)
{
	d_parent->inputRowsAboutToBeInserted(source, before, count);
}
void AbstractFilterSlotMachine::inputRowsInserted(AbstractDataSource *source, int before, int count)
{
	d_parent->inputRowsInserted(source, before, count);
}
void AbstractFilterSlotMachine::inputRowsAboutToBeDeleted(AbstractDataSource *source, int before, int count)
{
	d_parent->inputRowsAboutToBeDeleted(source, before, count);
}
void AbstractFilterSlotMachine::inputRowsDeleted(AbstractDataSource *source, int before, int count)
{
	d_parent->inputRowsDeleted(source, before, count);
}
void AbstractFilterSlotMachine::inputValidityAboutToChange(AbstractDataSource *source)
{
	d_parent->inputValidityAboutToChange(source);
}
void AbstractFilterSlotMachine::inputValidityChanged(AbstractDataSource *source)
{
	d_parent->inputValidityChanged(source);
}
void AbstractFilterSlotMachine::inputMaskingAboutToChange(AbstractDataSource *source)
{
	d_parent->inputMaskingAboutToChange(source);
}
void AbstractFilterSlotMachine::inputMaskingChanged(AbstractDataSource *source)
{
	d_parent->inputMaskingChanged(source);
}
void AbstractFilterSlotMachine::inputAboutToBeDestroyed(AbstractDataSource *source)
{
	d_parent->inputAboutToBeDestroyed(source);
}

bool AbstractFilter::input(int port, AbstractDataSource *source)
{
	if (port<0 || (inputCount()>=0 && port>=inputCount())) return false;
	if (source && !inputAcceptable(port, source)) return false;
	if (d_inputs.size() <= port) d_inputs.resize(port+1);
	AbstractDataSource *old_input = d_inputs.value(port);
	if (old_input)  // disconnect the old input's signals
		old_input->disconnect(&d_slot_machine); // disconnect all signals
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
		QObject::connect(source, SIGNAL(descriptionAboutToChange(AbstractDataSource*)),
				&d_slot_machine, SLOT(inputDescriptionAboutToChange(AbstractDataSource*)));
		QObject::connect(source, SIGNAL(descriptionChanged(AbstractDataSource*)),
				&d_slot_machine, SLOT(inputDescriptionChanged(AbstractDataSource*)));
		QObject::connect(source, SIGNAL(plotDesignationAboutToChange(AbstractDataSource*)),
				&d_slot_machine, SLOT(inputPlotDesignationAboutToChange(AbstractDataSource*)));
		QObject::connect(source, SIGNAL(plotDesignationChanged(AbstractDataSource*)),
				&d_slot_machine, SLOT(inputPlotDesignationChanged(AbstractDataSource*)));
		QObject::connect(source, SIGNAL(dataAboutToChange(AbstractDataSource*)),
				&d_slot_machine, SLOT(inputDataAboutToChange(AbstractDataSource*)));
		QObject::connect(source, SIGNAL(dataChanged(AbstractDataSource*)),
				&d_slot_machine, SLOT(inputDataChanged(AbstractDataSource*)));
		QObject::connect(source, SIGNAL(aboutToBeReplaced(AbstractDataSource*,AbstractDataSource*)),
				&d_slot_machine, SLOT(inputAboutToBeReplaced(AbstractDataSource*,AbstractDataSource*)));
		QObject::connect(source, SIGNAL(aboutToBeDestroyed(AbstractDataSource*)),
				&d_slot_machine, SLOT(inputAboutToBeDestroyed(AbstractDataSource*)));
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

void AbstractFilter::inputDescriptionAboutToChange(AbstractDataSource* source)
{
	inputDescriptionAboutToChange(d_inputs.indexOf(source));
}
void AbstractFilter::inputDescriptionChanged(AbstractDataSource* source)
{
	inputDescriptionChanged(d_inputs.indexOf(source));
}
void AbstractFilter::inputPlotDesignationAboutToChange(AbstractDataSource* source)
{
	inputPlotDesignationAboutToChange(d_inputs.indexOf(source));
}
void AbstractFilter::inputPlotDesignationChanged(AbstractDataSource* source)
{
	inputPlotDesignationChanged(d_inputs.indexOf(source));
}
void AbstractFilter::inputDataAboutToChange(AbstractDataSource* source)
{
	inputDataAboutToChange(d_inputs.indexOf(source));
}
void AbstractFilter::inputDataChanged(AbstractDataSource* source)
{
	inputDataChanged(d_inputs.indexOf(source));
}
void AbstractFilter::inputAboutToBeReplaced(AbstractDataSource* source, AbstractDataSource* replacement)
{
	input(d_inputs.indexOf(source), replacement);
}

