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

#ifndef _NO_TR1_
#include "tr1/memory"
using std::tr1::shared_ptr;
#else // if your compiler does not have TR1 support, you can use boost instead:
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
#endif

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
			emit owner->abstractAspectSignalEmitter()->aspectDescriptionAboutToChange(owner);
			QString tmp = d_target->name();
			d_target->setName(d_other_name);
			d_other_name = tmp;
			emit owner->abstractAspectSignalEmitter()->aspectDescriptionChanged(owner);
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
			emit owner->abstractAspectSignalEmitter()->aspectDescriptionAboutToChange(owner);
			QString tmp = d_target->comment();
			d_target->setComment(d_other_comment);
			d_other_comment = tmp;
			emit owner->abstractAspectSignalEmitter()->aspectDescriptionChanged(owner);
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
			emit owner->abstractAspectSignalEmitter()->aspectDescriptionAboutToChange(owner);
			QString tmp = d_target->captionSpec();
			d_target->setCaptionSpec(d_other_caption_spec);
			d_other_caption_spec = tmp;
			emit owner->abstractAspectSignalEmitter()->aspectDescriptionChanged(owner);
		}

		virtual void undo() { redo(); }

	private:
		AspectPrivate *d_target;
		QString d_other_caption_spec;
};


class AspectChildRemoveCmd : public QUndoCommand
{
	public:
		AspectChildRemoveCmd(AspectPrivate * target, shared_ptr<AbstractAspect> child)
			: d_target(target), d_child(child) {
				setText(QObject::tr("%1: remove %2").arg(d_target->name()).arg(d_child->name()));
			}

		virtual void redo() {
			d_index = d_target->indexOfChild(d_child);
			Q_ASSERT(d_index != -1); // d_child must be a child of d_target->owner()
			AbstractAspect * owner = d_target->owner();
			emit owner->abstractAspectSignalEmitter()->aspectAboutToBeRemoved(owner, d_index);
			emit d_child->abstractAspectSignalEmitter()->aspectAboutToBeRemoved(d_child.get());
			d_target->removeChild(d_child);
			d_child->setParentAspect(0);
			emit owner->abstractAspectSignalEmitter()->aspectRemoved(owner, d_index);
		}

		virtual void undo() {
			Q_ASSERT(d_index != -1); // d_child must be a child of d_target->owner()
			AbstractAspect * owner = d_target->owner();
			emit owner->abstractAspectSignalEmitter()->aspectAboutToBeAdded(owner, d_index);
			d_target->insertChild(d_index, d_child);
			d_child->setParentAspect(owner);
			emit owner->abstractAspectSignalEmitter()->aspectAdded(owner, d_index);
			emit d_child->abstractAspectSignalEmitter()->aspectAdded(d_child.get());
		}

	protected:
		AspectPrivate * d_target;
		shared_ptr<AbstractAspect> d_child;
		int d_index;
};

class AspectChildAddCmd : public AspectChildRemoveCmd
{
	public:
		AspectChildAddCmd(AspectPrivate * target, shared_ptr<AbstractAspect> child, int index)
			: AspectChildRemoveCmd(target, child) {
				setText(QObject::tr("%1: add %2").arg(d_target->name()).arg(d_child->name()));
				d_index = index;
			}

		virtual void redo() { AspectChildRemoveCmd::undo(); }

		virtual void undo() { AspectChildRemoveCmd::redo(); }
};
