/***************************************************************************
    File                 : aspectcommands.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
    Description          : Undo commands used by AbstractAspect.
                           Only meant to be used within AbstractAspect.cpp

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


#include <QUndoCommand>

class AspectNameChangeCmd : public QUndoCommand
{
	public:
		AspectNameChangeCmd(AspectPrivate *target, const QString &new_name)
			: d_target(target), d_other_name(new_name) {
				setText(QObject::tr("%1: rename to %2").arg(d_target->name()).arg(new_name));
			}

		virtual void redo() {
			AbstractAspect * owner = d_target->owner();
			emit owner->aspectDescriptionAboutToChange(owner);
			QString tmp = d_target->name();
			d_target->setName(d_other_name);
			d_other_name = tmp;
			emit owner->aspectDescriptionChanged(owner);
		}

		virtual void undo() { redo(); }

	private:
		AspectPrivate *d_target;
		QString d_other_name;
};

class AspectCommentChangeCmd : public QUndoCommand
{
	public:
		AspectCommentChangeCmd(AspectPrivate *target, const QString &new_comment)
			: d_target(target), d_other_comment(new_comment) {
				setText(QObject::tr("%1: change comment").arg(d_target->name()));
			}

		virtual void redo() {
			AbstractAspect * owner = d_target->owner();
			emit owner->aspectDescriptionAboutToChange(owner);
			QString tmp = d_target->comment();
			d_target->setComment(d_other_comment);
			d_other_comment = tmp;
			emit owner->aspectDescriptionChanged(owner);
		}

		virtual void undo() { redo(); }

	private:
		AspectPrivate *d_target;
		QString d_other_comment;
};

class AspectCaptionSpecChangeCmd : public QUndoCommand
{
	public:
		AspectCaptionSpecChangeCmd(AspectPrivate *target, const QString &new_caption_spec)
			: d_target(target), d_other_caption_spec(new_caption_spec) {
				setText(QObject::tr("%1: change caption").arg(d_target->name()));
			}

		virtual void redo() {
			AbstractAspect * owner = d_target->owner();
			emit owner->aspectDescriptionAboutToChange(owner);
			QString tmp = d_target->captionSpec();
			d_target->setCaptionSpec(d_other_caption_spec);
			d_other_caption_spec = tmp;
			emit owner->aspectDescriptionChanged(owner);
		}

		virtual void undo() { redo(); }

	private:
		AspectPrivate *d_target;
		QString d_other_caption_spec;
};


class AspectCreationTimeChangeCmd : public QUndoCommand
{
	public:
		AspectCreationTimeChangeCmd(AspectPrivate *target, const QDateTime &new_creation_time)
			: d_target(target), d_other_creation_time(new_creation_time) {
				setText(QObject::tr("%1: set creation time").arg(d_target->name()));
			}

		virtual void redo() {
			QDateTime tmp = d_target->creationTime();
			d_target->setCreationTime(d_other_creation_time);
			d_other_creation_time = tmp;
		}

		virtual void undo() { redo(); }

	private:
		AspectPrivate *d_target;
		QDateTime d_other_creation_time;
};


class AspectChildRemoveCmd : public QUndoCommand
{
	public:
		AspectChildRemoveCmd(AspectPrivate * target, AbstractAspect* child)
			: d_target(target), d_child(child) {
				setText(QObject::tr("%1: remove %2").arg(d_target->name()).arg(d_child->name()));
			}
		~AspectChildRemoveCmd() {
			if (!d_child->parentAspect())
				delete d_child;
		}

		// calling redo transfers ownership of d_child to the undo command
		virtual void redo() {
			d_index = d_target->indexOfChild(d_child);
			Q_ASSERT(d_index != -1); // d_child must be a child of d_target->owner()
			AbstractAspect * owner = d_target->owner();
			emit owner->aspectAboutToBeRemoved(owner, d_index);
			emit d_child->aspectAboutToBeRemoved(d_child);
			d_target->removeChild(d_child);
			d_child->setParentAspect(0);
			emit owner->aspectRemoved(owner, d_index);
		}

		// calling undo transfers ownership of d_child back to its parent aspect
		virtual void undo() {
			Q_ASSERT(d_index != -1); // d_child must be a child of d_target->owner()
			AbstractAspect * owner = d_target->owner();
			emit owner->aspectAboutToBeAdded(owner, d_index);
			d_target->insertChild(d_index, d_child);
			d_child->setParentAspect(owner);
			emit owner->aspectAdded(owner, d_index);
			emit d_child->aspectAdded(d_child);
		}

	protected:
		AspectPrivate * d_target;
		AbstractAspect* d_child;
		int d_index;
};

class AspectChildAddCmd : public AspectChildRemoveCmd
{
	public:
		AspectChildAddCmd(AspectPrivate * target, AbstractAspect* child, int index)
			: AspectChildRemoveCmd(target, child) {
				setText(QObject::tr("%1: add %2").arg(d_target->name()).arg(d_child->name()));
				d_index = index;
			}

		virtual void redo() { AspectChildRemoveCmd::undo(); }

		virtual void undo() { AspectChildRemoveCmd::redo(); }
};

class AspectChildMoveCmd : public QUndoCommand
{
	public:
		AspectChildMoveCmd(AspectPrivate * target, int from, int to)
			: d_target(target), d_from(from), d_to(to) {
				setText(QObject::tr("%1: move child from position %2 to %3.").arg(d_target->name()).arg(d_from+1).arg(d_to+1));
			}

		virtual void redo() {
			AbstractAspect * owner = d_target->owner();
			AbstractAspect * child = d_target->child(d_from);
			// Moving in one go confuses QTreeView, so we would need another two signals
			// to be mapped to QAbstractItemModel::layoutAboutToBeChanged() and ::layoutChanged().
			emit owner->aspectAboutToBeRemoved(owner, d_from);
			emit child->aspectAboutToBeRemoved(child);
			d_target->removeChild(child);
			emit owner->aspectRemoved(owner, d_from);
			emit owner->aspectAboutToBeAdded(owner, d_to);
			d_target->insertChild(d_to, child);
			emit owner->aspectAdded(owner, d_to);
			emit child->aspectAdded(child);
		}

		virtual void undo() {
			AbstractAspect * owner = d_target->owner();
			AbstractAspect * child = d_target->child(d_to);
			emit owner->aspectAboutToBeRemoved(owner, d_to);
			emit child->aspectAboutToBeRemoved(child);
			d_target->removeChild(child);
			emit owner->aspectRemoved(owner, d_to);
			emit owner->aspectAboutToBeAdded(owner, d_from);
			d_target->insertChild(d_from, child);
			emit owner->aspectAdded(owner, d_from);
			emit child->aspectAdded(child);
		}

	private:
		AspectPrivate *d_target;
		int d_from, d_to;
};
