/***************************************************************************
    File                 : AspectView.cpp
    Project              : SciDAVis
    Description          : MDI sub window that implements functions common
                           to all aspect views
    --------------------------------------------------------------------
    Copyright            : (C) 2007 Tilman Hoener zu Siederdissen (thzs*gmx.net)
    Copyright            : (C) 2007 Knut Franke (knut.franke*gmx.de)
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
#include "AspectView.h"

#include <QCloseEvent>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QWidget>
#include <QStyle>
#include <QApplication>
#include <QPushButton>
#include <QtDebug>

AspectView::AspectView(AbstractAspect *aspect)
	: QMdiSubWindow(0), d_aspect(aspect), d_closing(false)
{
	setWindowIcon(d_aspect->icon());
	handleAspectDescriptionChanged(d_aspect);
	connect(d_aspect, SIGNAL(aspectDescriptionChanged(AbstractAspect *)), 
		this, SLOT(handleAspectDescriptionChanged(AbstractAspect *)));
}

void AspectView::contextMenuEvent(QContextMenuEvent *event)
{
	qDebug() << "AspectView::contextMenuEvent()";
	QMenu *menu = new QMenu();
	createContextMenu(menu);
    menu->addSeparator();
	d_aspect->createContextMenu(menu);
	menu->exec(event->globalPos());
	delete menu;
}

QMenu *AspectView::createContextMenu(QMenu *append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();
    const QStyle *widget_style = style();
   	
	QAction *action_temp;
	if(windowState() & (Qt::WindowMinimized | Qt::WindowMaximized))
	{
		action_temp = menu->addAction(tr("&Restore"), this, SLOT(showNormal()));
		action_temp->setIcon(widget_style->standardIcon(QStyle::SP_TitleBarNormalButton));
	}
	if(!(windowState() & Qt::WindowMinimized))
	{
		action_temp = menu->addAction(tr("Mi&nimize"), this, SLOT(showMinimized()));
		action_temp->setIcon(widget_style->standardIcon(QStyle::SP_TitleBarMinButton));
	}
	if(!(windowState() & Qt::WindowMaximized))
	{
		action_temp = menu->addAction(tr("Ma&ximize"), this, SLOT(showMaximized()));
		action_temp->setIcon(widget_style->standardIcon(QStyle::SP_TitleBarMaxButton));
	}
	action_temp = menu->addAction(tr("&Close"), this, SLOT(close()));
	action_temp->setIcon(widget_style->standardIcon(QStyle::SP_TitleBarCloseButton));

	return menu;
}

AspectView::~AspectView()
{
}

void AspectView::handleAspectDescriptionChanged(AbstractAspect *aspect)
{
	if (aspect != d_aspect) return;
	setWindowTitle(d_aspect->caption());
	update();
}

void AspectView::closeEvent(QCloseEvent *event)
{
	if (!d_closing) 
	{
		d_closing = true;

		const QStyle *app_style = qApp->style();
		QMessageBox msg_box;
		msg_box.setIcon(QMessageBox::Question);
		msg_box.setWindowTitle(tr("Hide or remove?"));
		msg_box.setText(tr("Hide the window or remove %1 from the project?").arg(d_aspect->name()));
		QPushButton *hide_button = msg_box.addButton(tr("&Hide"), QMessageBox::YesRole);
		hide_button->setIcon(app_style->standardIcon(QStyle::SP_TitleBarCloseButton));
		QPushButton *remove_button = msg_box.addButton(tr("&Remove"), QMessageBox::YesRole);
		remove_button->setIcon(app_style->standardIcon(QStyle::SP_TrashIcon));
		QPushButton *cancel_button = msg_box.addButton(tr("&Cancel"), QMessageBox::RejectRole);
		cancel_button->setIcon(app_style->standardIcon(QStyle::SP_DialogCancelButton));
		msg_box.exec();

		if(msg_box.clickedButton() == hide_button) 
		{
			hide();
			event->accept();
		}
		else if(msg_box.clickedButton() == remove_button) 
		{
			d_aspect->remove();
			event->accept();
		}
		else event->ignore();
		d_closing = false;
	}
}
