/***************************************************************************
    File                 : AbstractGraphTool.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Interface for tools operating on a Layer

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
#ifndef ABSTRACT_GRAPH_TOOL_H
#define ABSTRACT_GRAPH_TOOL_H

class Layer;

/*! Interface for tools operating on a Layer.
 *
 * The basic idea is to have one AbstractGraphTool subclass per user-visible tool operating on a Layer,
 * although there may be cases where exceptions to this rule are appropriate.
 * %Note that tools can be presented to the user via icons (like DataPickerTool) or via menu entries
 * (like TranslateCurveTool); the basic concept is quite similar.
 * The main point in managing plot tools as subclasses of this class (as opposed to using void pointers)
 * is the virtual destructor that allows tools to clean up after themselves.
 * Additionally, a pointer to the parent Layer (#m_layer) is managed.
 * In the future, this class may provide other generic tool functionality.
 *
 * %Note that zooming and range selection are somewhat special in that they can be active in addition
 * to other tools. These are handled as special cases, while all other tools are rendered mutually exclusive
 * by having Layer manage a pointer to the currently active tool (Layer::m_active_tool).
 *
 * It would be nice for some of the plot tools (like TranslateCurveTool or MultiPeakFitTool) to send a signal
 * when they are finished and to generalize the statusText signal provided by most tools, but having
 * AbstractGraphTool inherit from QObject would make it impossible for
 * plot tools to also inherit from other QObject decendants (such as QwtPlotPicker).
 * As a workaround, plot tools can call Layer::setActiveTool(), carefully noting that they are deleted
 * during this call.
 *
 * Currently, plot tools are instantiated by ApplicationWindow and handed to the Layer in question;
 * this scheme will have to be revised for dynamically adding new tools via plugins.
 */
class AbstractGraphTool
{
	public:
		AbstractGraphTool(Layer *layer) { m_layer = layer; }
		virtual ~AbstractGraphTool() {};
	protected:
		Layer *m_layer;
};

#endif // ifndef ABSTRACT_GRAPH_TOOL_H
