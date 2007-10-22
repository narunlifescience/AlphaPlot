/***************************************************************************
    File                 : AbstractAspect.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
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
#include "AspectPrivate.h"
#include "aspectcommands.h"

#include <QIcon>
#include <QMenu>
#include <QtDebug>

AbstractAspect::AbstractAspect(const QString &name)
	: d(new AspectPrivate(name, this)), d_parent(0), d_wrapper(new AbstractAspectWrapper(this))
{
}

AbstractAspect::~AbstractAspect()
{
	for(int i=0; i<childCount(); i++)
		d->child(i)->setParent(0);
	delete d_wrapper;
	delete d;
}

void AbstractAspect::setParent(AbstractAspect * new_parent)
{
	if(d_parent)
		QObject::disconnect(d_wrapper, 0, d_parent->abstractAspectSignalEmitter(), 0);
	d_parent = new_parent;	
	if(d_parent)
	{
		QObject::connect(d_wrapper, SIGNAL(aspectDescriptionChanged(AbstractAspect *)), 
				d_parent->abstractAspectSignalEmitter(), SIGNAL(aspectDescriptionChanged(AbstractAspect *)));
		QObject::connect(d_wrapper, SIGNAL(aspectAboutToBeAdded(AbstractAspect *, int)), 
				d_parent->abstractAspectSignalEmitter(), SIGNAL(aspectAboutToBeAdded(AbstractAspect *, int)));
		QObject::connect(d_wrapper, SIGNAL(aspectAboutToBeRemoved(AbstractAspect *, int)), 
				d_parent->abstractAspectSignalEmitter(), SIGNAL(aspectAboutToBeRemoved(AbstractAspect *, int)));
		QObject::connect(d_wrapper, SIGNAL(aspectAdded(AbstractAspect *, int)), 
				d_parent->abstractAspectSignalEmitter(), SIGNAL(aspectAdded(AbstractAspect *, int)));
		QObject::connect(d_wrapper, SIGNAL(aspectRemoved(AbstractAspect *, int)), 
				d_parent->abstractAspectSignalEmitter(), SIGNAL(aspectRemoved(AbstractAspect *, int)));
		QObject::connect(d_wrapper, SIGNAL(aspectAboutToBeRemoved(AbstractAspect *)), 
				d_parent->abstractAspectSignalEmitter(), SIGNAL(aspectAboutToBeRemoved(AbstractAspect *)));
		QObject::connect(d_wrapper, SIGNAL(aspectAdded(AbstractAspect *)), 
				d_parent->abstractAspectSignalEmitter(), SIGNAL(aspectAdded(AbstractAspect *)));
	}
}

void AbstractAspect::addChild(shared_ptr<AbstractAspect> child)
{
	exec(new AspectChildAddCmd(d, child, d->childCount()));
}

void AbstractAspect::removeChild(shared_ptr<AbstractAspect> child)
{
	Q_ASSERT(indexOfChild(child) != -1);
	exec(new AspectChildRemoveCmd(d, child));
}

void AbstractAspect::removeChild(int index)
{
	Q_ASSERT(index >= 0 && index <= childCount());
	exec(new AspectChildRemoveCmd(d, d->child(index)));
}

shared_ptr<AbstractAspect> AbstractAspect::child(int index) const
{
	Q_ASSERT(index >= 0 && index <= childCount());
	return d->child(index);
}

int AbstractAspect::childCount() const
{
	return d->childCount();
}

int AbstractAspect::indexOfChild(const AbstractAspect *child) const
{
	return d->indexOfChild(child);
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
	return d->name();
}

void AbstractAspect::setName(const QString &value)
{
	if (value == d->name()) return;
	exec(new AspectNameChangeCmd(d, value));
}

QString AbstractAspect::comment() const
{
	return d->comment();
}

void AbstractAspect::setComment(const QString &value)
{
	if (value == d->comment()) return;
	exec(new AspectCommentChangeCmd(d, value));
}

QString AbstractAspect::captionSpec() const
{
	return d->captionSpec();
}

void AbstractAspect::setCaptionSpec(const QString &value)
{
	if (value == d->captionSpec()) return;
	exec(new AspectCaptionSpecChangeCmd(d, value));
}

QDateTime AbstractAspect::creationTime() const
{
	return d->creationTime();
}

QString AbstractAspect::caption() const
{
	return d->caption();
}

QIcon AbstractAspect::icon() const
{
	return QIcon();
}

QMenu *AbstractAspect::createContextMenu()
{
	QMenu *menu = new QMenu();

	menu->addAction(QPixmap(":/close.xpm"), QObject::tr("&Remove"), d_wrapper, SLOT(remove()), QObject::tr("Ctrl+W"));

	return menu;
}
		
void AbstractAspectWrapper::setName(const QString &value) 
{ 
	d_aspect->setName(value); 
}

void AbstractAspectWrapper::setComment(const QString &value) 
{ 
	d_aspect->setComment(value); 
}

void AbstractAspectWrapper::setCaptionSpec(const QString &value) 
{ 
	d_aspect->setCaptionSpec(value); 
}

void AbstractAspectWrapper::remove() 
{
	d_aspect->remove(); 
}

