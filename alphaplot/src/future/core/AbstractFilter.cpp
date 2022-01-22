/***************************************************************************
    File                 : AbstractFilter.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Benkert
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

bool AbstractFilter::input(int port, const AbstractColumn *source) {
  if (port < 0 || (inputCount() >= 0 && port >= inputCount())) return false;
  if (source && !inputAcceptable(port, source)) return false;
  if (port >= d_inputs.size()) d_inputs.resize(port + 1);
  const AbstractColumn *old_input = d_inputs.value(port);
  if (source == old_input) return true;
  if (old_input) {
    disconnect(old_input, nullptr, this, nullptr);
    // replace input, notifying the filter implementation of the changes
    inputDescriptionAboutToChange(old_input);
    inputPlotDesignationAboutToChange(old_input);
    inputMaskingAboutToChange(old_input);
    inputDataAboutToChange(old_input);
    if (source && source->columnMode() != old_input->columnMode())
      inputModeAboutToChange(old_input);
  }
  if (!source) inputAboutToBeDisconnected(old_input);
  d_inputs[port] = source;
  if (source) {  // we have a new source
    if (old_input && source->columnMode() != old_input->columnMode())
      inputModeAboutToChange(source);
    inputDataChanged(source);
    inputMaskingChanged(source);
    inputPlotDesignationChanged(source);
    inputDescriptionChanged(source);
    // connect the source's signals
    QObject::connect(source, &AbstractColumn::aspectDescriptionAboutToChange,
                     this,
                     qOverload<const AbstractAspect *>(
                         &AbstractFilter::inputDescriptionAboutToChange));
    QObject::connect(source, &AbstractColumn::aspectDescriptionChanged, this,
                     qOverload<const AbstractAspect *>(
                         &AbstractFilter::inputDescriptionChanged));
    QObject::connect(source, &AbstractColumn::plotDesignationAboutToChange,
                     this, &AbstractFilter::inputPlotDesignationAboutToChange);
    QObject::connect(source, &AbstractColumn::plotDesignationChanged, this,
                     &AbstractFilter::inputPlotDesignationChanged);
    QObject::connect(source, &AbstractColumn::modeAboutToChange, this,
                     &AbstractFilter::inputModeAboutToChange);
    QObject::connect(source, &AbstractColumn::modeChanged, this,
                     &AbstractFilter::inputModeChanged);
    QObject::connect(source, &AbstractColumn::dataAboutToChange, this,
                     &AbstractFilter::inputDataAboutToChange);
    QObject::connect(source, &AbstractColumn::dataChanged, this,
                     &AbstractFilter::inputDataChanged);
    QObject::connect(source, &AbstractColumn::aboutToBeReplaced, this,
                     &AbstractFilter::inputAboutToBeReplaced);
    QObject::connect(source, &AbstractColumn::rowsAboutToBeInserted, this,
                     &AbstractFilter::inputRowsAboutToBeInserted);
    QObject::connect(source, &AbstractColumn::rowsInserted, this,
                     &AbstractFilter::inputRowsInserted);
    QObject::connect(source, &AbstractColumn::rowsAboutToBeRemoved, this,
                     &AbstractFilter::inputRowsAboutToBeRemoved);
    QObject::connect(source, &AbstractColumn::rowsRemoved, this,
                     &AbstractFilter::inputRowsRemoved);
    QObject::connect(source, &AbstractColumn::maskingAboutToChange, this,
                     &AbstractFilter::inputMaskingAboutToChange);
    QObject::connect(source, &AbstractColumn::maskingChanged, this,
                     &AbstractFilter::inputMaskingChanged);
    QObject::connect(source, &AbstractColumn::aboutToBeDestroyed, this,
                     &AbstractFilter::inputAboutToBeDestroyed);
  } else {  // source==0, that is, the input port has been disconnected
    // try to shrink d_inputs
    int num_connected_inputs = d_inputs.size();
    while (d_inputs.at(num_connected_inputs - 1) == 0) {
      num_connected_inputs--;
      if (!num_connected_inputs) break;
    }
    d_inputs.resize(num_connected_inputs);
  }
  return true;
}

bool AbstractFilter::input(const AbstractFilter *sources) {
  if (!sources) return false;
  bool result = true;
  for (int i = 0; i < sources->outputCount(); i++)
    if (!input(i, sources->output(i))) result = false;
  return result;
}

QString AbstractFilter::inputLabel(int port) const {
  return QObject::tr("In%1").arg(port + 1);
}

void AbstractFilter::inputAboutToBeReplaced(const AbstractColumn *source,
                                            const AbstractColumn *replacement) {
  input(portIndexOf(source), replacement);
}
