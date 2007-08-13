/***************************************************************************
    File                 : AbstractAspect.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Base class for all persistent objects in a Project.

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
#include "AbstractAspect.h"
#include "AspectModel.h"
#include "aspectcommands.h"

#include <QIcon>
#include <QMenu>

AbstractAspect::AbstractAspect(const QString &name)
	: d_model(new AspectModel(name))
{
}

AbstractAspect::~AbstractAspect()
{
	delete d_model;
}

void AbstractAspect::setParentPrivate(AbstractAspect *new_parent)
{
	if (parent()) removeAspectObserver(parent());
	QObject::setParent(new_parent);
	if (new_parent)
		addAspectObserver(new_parent);
}

void AbstractAspect::addChild(AbstractAspect *child)
{
	exec(new AspectChildAddCmd(this, child, d_model->childCount()));
}

void AbstractAspect::removeChild(AbstractAspect *child)
{
	exec(new AspectChildRemoveCmd(this, child));
}

AbstractAspect *AbstractAspect::child(int index) const
{
	return d_model->child(index);
}

int AbstractAspect::childCount() const
{
	return d_model->childCount();
}

int AbstractAspect::indexOfChild(const AbstractAspect *child) const
{
	return d_model->indexOfChild(child);
}

void AbstractAspect::exec(QUndoCommand *cmd)
{
	QUndoStack *stack = undoStack();
	if (stack)
		stack->push(cmd);
	else {
		cmd->redo();
		delete cmd;
	}
}

QString AbstractAspect::name() const
{
	return d_model->name();
}

void AbstractAspect::setName(const QString &value)
{
	if (value == d_model->name()) return;
	exec(new AspectNameChangeCmd(this, value));
}

QString AbstractAspect::comment() const
{
	return d_model->comment();
}

void AbstractAspect::setComment(const QString &value)
{
	if (value == d_model->comment()) return;
	exec(new AspectCommentChangeCmd(this, value));
}

QString AbstractAspect::captionSpec() const
{
	return d_model->captionSpec();
}

void AbstractAspect::setCaptionSpec(const QString &value)
{
	if (value == d_model->captionSpec()) return;
	exec(new AspectCaptionSpecChangeCmd(this, value));
}

QDateTime AbstractAspect::creationTime() const
{
	return d_model->creationTime();
}

QString AbstractAspect::caption() const
{
	return d_model->caption();
}

QIcon AbstractAspect::icon() const
{
	return QIcon();
}

QMenu *AbstractAspect::createContextMenu()
{
	QMenu *menu = new QMenu();

	menu->addAction(QPixmap(":/close.xpm"), tr("&Remove"), this, SLOT(remove()), tr("Ctrl+W"));

	return menu;
}
