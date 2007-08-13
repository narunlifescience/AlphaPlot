/***************************************************************************
    File                 : MdiSubWindow.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
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
#include "MdiSubWindow.h"

#include <QCloseEvent>
#include <QIcon>
#include <QMenu>

MdiSubWindow::MdiSubWindow(AbstractAspect *aspect, QWidget *view)
	: d_aspect(aspect), d_closing(false)
{
	setWidget(view);
	aspectDescriptionChanged(d_aspect);
	setWindowIcon(d_aspect->icon());
	// TODO: doesn't work... bug in Qt?
	//setSystemMenu(d_aspect->createContextMenu());
	d_aspect->addAspectObserver(this);
}

void MdiSubWindow::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = d_aspect->createContextMenu();
	Q_CHECK_PTR(menu);
	menu->exec(event->globalPos());
	delete menu;
}

MdiSubWindow::~MdiSubWindow()
{
	d_aspect->removeAspectObserver(this);
}

void MdiSubWindow::aspectDescriptionChanged(AbstractAspect *aspect)
{
	if (aspect != d_aspect) return;
	setWindowTitle(d_aspect->caption());
	update();
}

void MdiSubWindow::aspectAboutToBeRemoved(AbstractAspect *aspect)
{
	if (aspect != d_aspect || d_closing) return;
	d_closing = true;
	close();
}

void MdiSubWindow::closeEvent(QCloseEvent *event)
{
	if (!d_closing) {
		d_closing = true;
		if (d_aspect->parent())
			d_aspect->parent()->removeChild(d_aspect);
	}
	event->accept();
}
