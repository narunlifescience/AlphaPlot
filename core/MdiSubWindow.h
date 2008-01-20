/***************************************************************************
    File                 : MdiSubWindow.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
    Description          : An MDI sub window which manages views on an
                           AbstractAspect.

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
#ifndef MDI_SUB_WINDOW_H
#define MDI_SUB_WINDOW_H

#include <QMdiSubWindow>
#include "AbstractAspect.h"

//! An MDI sub window which manages views on an AbstractAspect.
/**
 * Instances of this class are meant to be wrapped around views on an AbstractAspect
 * before they are added to a QMdiArea. In addition to the functionality provided
 * by QMdiSubWindow, MdiSubWindow automatically updates the window title when
 * AbstractAspect::caption() changes, removes the Aspect when the MDI window is closed
 * and vice versa, and provides access to the Aspect's context menu.
 */
class MdiSubWindow : public QMdiSubWindow
{
	Q_OBJECT

	public:
		//! Construct a window managing view on aspect.
		MdiSubWindow(shared_ptr<AbstractAspect> aspect, QWidget *view);
		~MdiSubWindow();

		shared_ptr<AbstractAspect> aspect() const { return d_aspect; }

	public slots:
		//! Keep my window title in sync with AbstractAspect::caption().
		void aspectDescriptionChanged(AbstractAspect *aspect);
		//! Close me before my Aspect is removed.
		void aspectAboutToBeRemoved(AbstractAspect *aspect);

	protected:
		//! When I'm being closed, remove my Aspect from its parent.
		virtual void closeEvent(QCloseEvent *event);
		void contextMenuEvent(QContextMenuEvent *event);

	private:
		//! The aspect who's view I'm managing.
		shared_ptr<AbstractAspect> d_aspect;
		//! Whether I'm just being closed.
		/**
		 * Depending on whether an Aspect is removed programatically or by closing its
		 * default view (held by me), either aspectAboutToBeRemoved() generates a close
		 * event for me or closeEvent() removes #d_aspect from its parent. Before one causes
		 * the other to be called, #d_closing is set to true so as to avoid infinite
		 * recursion.
		 */
		bool d_closing;
};

#endif // ifndef MDI_SUB_WINDOW_H
