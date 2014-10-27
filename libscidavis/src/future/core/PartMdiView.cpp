/***************************************************************************
    File                 : PartMdiView.cpp
    Project              : SciDAVis
    Description          : MDI sub window to be wrapped around views of
                           AbstractPart.
    --------------------------------------------------------------------
    Copyright            : (C) 2007-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2007-2009 Knut Franke (knut.franke*gmx.de)
                           (replace * with @ in the email addresses) 

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
#include "PartMdiView.h"
#include "AbstractPart.h"

#include <QCloseEvent>
#include <QMenu>
#include <QMdiArea>

PartMdiView::PartMdiView(AbstractPart *part, QWidget * embedded_view)
	: QMdiSubWindow(0), d_part(part), d_closing(false), d_status(Closed)
{
	setWindowIcon(d_part->icon());
	handleAspectDescriptionChanged(d_part);
	connect(d_part, SIGNAL(aspectDescriptionChanged(const AbstractAspect *)), 
		this, SLOT(handleAspectDescriptionChanged(const AbstractAspect *)));
	connect(d_part, SIGNAL(aspectAboutToBeRemoved(const AbstractAspect*)),
			this, SLOT(handleAspectAboutToBeRemoved(const AbstractAspect*)));
	setWidget(embedded_view);
}

void PartMdiView::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = d_part->createContextMenu();
	menu->exec(event->globalPos());
	delete menu;
}

PartMdiView::~PartMdiView()
{
}

void PartMdiView::handleAspectDescriptionChanged(const AbstractAspect *aspect)
{
	if (aspect != d_part) return;
	setWindowTitle(d_part->caption());
	update();
}

void PartMdiView::handleAspectAboutToBeRemoved(const AbstractAspect *aspect)
{
	if (aspect != d_part) return;
	d_closing = true;
	close();
}

void PartMdiView::closeEvent(QCloseEvent *event)
{
	if (!d_closing) {
		d_closing = true;
		d_part->remove();
		event->accept();
	}
	d_closing = false;
	
	SubWindowStatus old_status = d_status;
	d_status = Closed;
	emit statusChanged(this, old_status, d_status);
}

void PartMdiView::hideEvent(QHideEvent *event)
{
	SubWindowStatus old_status = d_status;
	d_status = Hidden;
	emit statusChanged(this, old_status, d_status);
	event->accept();
}

void PartMdiView::showEvent(QShowEvent *event)
{
	SubWindowStatus old_status = d_status;
	d_status = Visible;
	emit statusChanged(this, old_status, d_status);
	event->accept();
}

