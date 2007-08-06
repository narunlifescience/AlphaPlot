/***************************************************************************
    File                 : ReadOnlyTableModel.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Model to display the output of a filter

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

#include "ReadOnlyTableModel.h"
#include "AbstractStringDataSource.h"
#include "CopyThroughFilter.h"
#include "Double2StringFilter.h"
#include "DateTime2StringFilter.h"

ReadOnlyTableModel::~ReadOnlyTableModel()
{
	foreach(AbstractFilter *i, d_output_filters)
		if (i) delete i;
}

Qt::ItemFlags ReadOnlyTableModel::flags(const QModelIndex & index ) const
{
	if (index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	else
		return Qt::ItemIsEnabled;
}


QVariant ReadOnlyTableModel::data(const QModelIndex &index, int role) const
{
	if( !index.isValid() ||
			((role != Qt::DisplayRole) && (role != Qt::EditRole) && (role != Qt::ToolTipRole) ) ||
			!d_output_filters.value(index.column()) )
		return QVariant();

	return QVariant(static_cast<AbstractStringDataSource*>(d_output_filters.at(index.column())->output(0))->textAt(index.row()));
}




QVariant ReadOnlyTableModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
	if ((role != Qt::DisplayRole) && (role != Qt::EditRole) && (role != Qt::ToolTipRole) )
		return QVariant();
	switch(orientation) 
	{
		case Qt::Horizontal: 
			return d_inputs.value(section) ? d_inputs.at(section)->label() : QVariant();
		case Qt::Vertical: 
			return QVariant(QString::number(section+1));
	}
}

int ReadOnlyTableModel::rowCount(const QModelIndex &) const 
{
	int rows, result = 0;
	foreach(AbstractDataSource *i, d_inputs) 
	{
		if (!i) continue;
		if ((rows = i->rowCount()) > result)
			result = rows;
	}
	return result;
}

int ReadOnlyTableModel::columnCount(const QModelIndex &) const 
{ 
	return d_inputs.size(); 
}

QModelIndex ReadOnlyTableModel::index(int row, int column, const QModelIndex &parent) const 
{
	Q_UNUSED(parent)
	return createIndex(row, column);
}

QModelIndex ReadOnlyTableModel::parent(const QModelIndex &) const
{ 
	return QModelIndex(); 
}

int ReadOnlyTableModel::numInputs() const 
{ 	
	return -1; 
}

bool ReadOnlyTableModel::inputAcceptable(int, AbstractDataSource *source)
{
	return source->inherits("AbstractStringDataSource") ||
		source->inherits("AbstractDoubleDataSource") ||
		source->inherits("AbstractDateTimeDataSource");
}

int ReadOnlyTableModel::numOutputs() const 
{ 
	return 0; 
}

AbstractDataSource* ReadOnlyTableModel::output(int) const 
{ 
	return 0; 
}

void ReadOnlyTableModel::inputDataChanged(int port) 
{
	if (port >= d_output_filters.size())
		d_output_filters.insert(port, 0);
	AbstractFilter *old_filter = d_output_filters.at(port);
	if (d_inputs.at(port)) {
		if (old_filter) {
			// input is connected and there's already a filter for it
			if (!old_filter->input(0, d_inputs.at(port))) {
				// can't connect => type of input changed
				delete old_filter;
				d_output_filters[port] = newOutputFilterFor(d_inputs.at(port));
			}
		} else // just create a new filter for the input
			d_output_filters[port] = newOutputFilterFor(d_inputs.at(port));
	} else {
		// input disconnected, therefore we delete its filter
		if (old_filter)
			delete old_filter;
		// shrink d_output_filters to size of d_inputs
		for (int i=d_output_filters.size(); i>d_inputs.size(); i--)
			d_output_filters.removeLast();
	}
	emit dataChanged(createIndex(0,port), createIndex(d_inputs[port]->rowCount()-1,port));
}

AbstractFilter *ReadOnlyTableModel::newOutputFilterFor(AbstractDataSource *source)
{
	AbstractFilter *result = 0;
	if (source->inherits("AbstractStringDataSource"))
		result = new CopyThroughFilter();
	else if (source->inherits("AbstractDoubleDataSource"))
		result = new Double2StringFilter();
	else if (source->inherits("AbstractDateTimeDataSource"))
		result = new DateTime2StringFilter();
	else
		return 0;
	result->input(0, source);
	return result;
}

void ReadOnlyTableModel::inputDescriptionChanged(int port) 
{
	emit headerDataChanged(Qt::Horizontal, port, port);
}


