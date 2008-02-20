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
#include "Folder.h"

#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>
#include <QApplication>

AbstractAspect::AbstractAspect(const QString &name)
	: d_aspect_private(new Private(this, name))
{
}

AbstractAspect::~AbstractAspect()
{
	delete d_aspect_private;
}

AbstractAspect * AbstractAspect::parentAspect() const
{
	return d_aspect_private->parent();
}

void AbstractAspect::addChild(AbstractAspect* child)
{
	Q_CHECK_PTR(child);
	beginMacro(tr("%1: add %2.").arg(name()).arg(child->name()));
	exec(new AspectChildAddCmd(d_aspect_private, child, d_aspect_private->childCount()));
	aspectAddedOuter(child);
	endMacro();
}

void AbstractAspect::insertChild(AbstractAspect* child, int index)
{
	Q_CHECK_PTR(child);
	beginMacro(tr("%1: insert %2 at position %3.").arg(name()).arg(child->name()).arg(index+1));
	exec(new AspectChildAddCmd(d_aspect_private, child, index));
	aspectAddedOuter(child);
	endMacro();
}

void AbstractAspect::removeChild(AbstractAspect* child)
{
	Q_ASSERT(indexOfChild(child) != -1);
	beginMacro(tr("%1: remove %2.").arg(name()).arg(child->name()));
	aspectAboutToBeRemovedOuter(child);
	exec(new AspectChildRemoveCmd(d_aspect_private, child));
	endMacro();
}

void AbstractAspect::removeChild(int index)
{
	Q_ASSERT(index >= 0 && index <= childCount());
	removeChild(d_aspect_private->child(index));
}

AbstractAspect* AbstractAspect::child(int index) const
{
	Q_ASSERT(index >= 0 && index <= childCount());
	return d_aspect_private->child(index);
}

int AbstractAspect::childCount() const
{
	return d_aspect_private->childCount();
}

int AbstractAspect::indexOfChild(const AbstractAspect *child) const
{
	return d_aspect_private->indexOfChild(child);
}

void AbstractAspect::moveChild(int from, int to)
{
	Q_ASSERT(0 <= from < d_aspect_private->childCount());
	Q_ASSERT(0 <= to   < d_aspect_private->childCount());
	exec(new AspectChildMoveCmd(d_aspect_private, from, to));
}

void AbstractAspect::exec(QUndoCommand *cmd)
{
	Q_CHECK_PTR(cmd);
	QUndoStack *stack = undoStack();
	if (stack)
		stack->push(cmd);
	else {
		cmd->redo();
		delete cmd;
	}
}

void AbstractAspect::beginMacro(const QString& text)
{
	QUndoStack *stack = undoStack();
	if (stack)
		stack->beginMacro(text);
}

void AbstractAspect::endMacro()
{
	QUndoStack *stack = undoStack();
	if (stack)
		stack->endMacro();
}

QString AbstractAspect::name() const
{
	return d_aspect_private->name();
}

void AbstractAspect::setName(const QString &value)
{
	if (value == d_aspect_private->name()) return;
	exec(new AspectNameChangeCmd(d_aspect_private, value));
}

QString AbstractAspect::comment() const
{
	return d_aspect_private->comment();
}

void AbstractAspect::setComment(const QString &value)
{
	if (value == d_aspect_private->comment()) return;
	exec(new AspectCommentChangeCmd(d_aspect_private, value));
}

QString AbstractAspect::captionSpec() const
{
	return d_aspect_private->captionSpec();
}

void AbstractAspect::setCaptionSpec(const QString &value)
{
	if (value == d_aspect_private->captionSpec()) return;
	exec(new AspectCaptionSpecChangeCmd(d_aspect_private, value));
}

void AbstractAspect::setCreationTime(const QDateTime& time)
{
	if (time == d_aspect_private->creationTime()) return;
	exec(new AspectCreationTimeChangeCmd(d_aspect_private, time));
}

QDateTime AbstractAspect::creationTime() const
{
	return d_aspect_private->creationTime();
}

QString AbstractAspect::caption() const
{
	return d_aspect_private->caption();
}

QIcon AbstractAspect::icon() const
{
	return QIcon();
}

QAction *AbstractAspect::undoAction(QObject *parent) const
{
	if (!undoStack()) return 0;
	QAction *result = undoStack()->createUndoAction(parent);
	result->setIcon(QIcon(QPixmap(":/undo.xpm")));
	// TODO: make this customizable via preferences dialog.
	// Maybe use a singleton settings manager for stuff that applies to the application rather than
	// a specific project.
	result->setShortcut(tr("Ctrl+Z"));
	return result;
}

QAction *AbstractAspect::redoAction(QObject *parent) const
{
	if (!undoStack()) return 0;
	QAction *result = undoStack()->createRedoAction(parent);
	result->setIcon(QIcon(QPixmap(":/redo.xpm")));
	result->setShortcut(tr("Ctrl+Y"));
	return result;
}

QMenu *AbstractAspect::createContextMenu() const
{
	QMenu * menu = new QMenu();
    
	const QStyle *widget_style = qApp->style();
	QAction *action_temp;

	menu->addAction(undoAction(menu));
	menu->addAction(redoAction(menu));

	menu->addSeparator();

	action_temp = menu->addAction(QObject::tr("&Remove"), this, SLOT(remove()));
	//menu->addAction(QPixmap(":/close.xpm"), QObject::tr("&Remove"), this, SLOT(remove()), QObject::tr("Ctrl+W"));
	action_temp->setIcon(widget_style->standardIcon(QStyle::SP_TrashIcon));

	//TODO: Is there any need for this? It just displays the same info as the project explorer.
	//menu->addAction(QPixmap(), QObject::tr("&Properties"), this, SLOT(showProperties()) );

	return menu;
}

void AbstractAspect::showProperties()
{
	QString message;
	message += QObject::tr("Name: ") + name() + "\n\n";
	message += QObject::tr("Comment: ") + comment() + "\n\n";
	message += QObject::tr("Type: ") + QString(metaObject()->className()) + "\n\n";
	message += QObject::tr("Path: ") + path() + "\n\n";
	message += QObject::tr("Created: ") + creationTime().toString(QString("yyyy-MM-dd hh:mm:ss")) + "\n\n";

	QMessageBox * mbox = new QMessageBox( QMessageBox::Information, QObject::tr("Properties"), message, QMessageBox::Ok);
	mbox->setIconPixmap(icon().pixmap(32, QIcon::Normal, QIcon::On));
	mbox->setAttribute(Qt::WA_DeleteOnClose);
	mbox->show();
}

Folder * AbstractAspect::folder()
{
	if(inherits("Folder")) return static_cast<Folder *>(this);
	AbstractAspect * parent_aspect = parentAspect();
	while(parent_aspect && !parent_aspect->inherits("Folder")) 
		parent_aspect = parent_aspect->parentAspect();
	return static_cast<Folder *>(parent_aspect);	
}

bool AbstractAspect::isDescendantOf(AbstractAspect *other)
{
	if(other == this) return true;
	AbstractAspect * parent_aspect = parentAspect();
	while(parent_aspect)
	{
		if(parent_aspect == other) return true;
		parent_aspect = parent_aspect->parentAspect();
	}
	return false;
}

