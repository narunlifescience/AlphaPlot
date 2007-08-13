/***************************************************************************
    File                 : aspectcommands.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
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
#include "AspectModel.h"

#include <QUndoCommand>

class AspectNameChangeCmd : public QUndoCommand
{
	public:
		AspectNameChangeCmd(AbstractAspect *target, const QString &new_name)
			: d_target(target), d_other_name(new_name) {
				setText(QObject::tr("rename %1 to %2").arg(d_target->name()).arg(new_name));
			}
		virtual void redo() {
			AspectModel *model = d_target->d_model;
			QString tmp = model->name();
			model->setName(d_other_name);
			d_other_name = tmp;
			d_target->aspectDescriptionChanged();
		}
		virtual void undo() { redo(); }

	private:
		AbstractAspect *d_target;
		QString d_other_name;
};

class AspectCommentChangeCmd : public QUndoCommand
{
	public:
		AspectCommentChangeCmd(AbstractAspect *target, const QString &new_comment)
			: d_target(target), d_other_comment(new_comment) {
				setText(QObject::tr("change comment of %1").arg(d_target->name()));
			}
		virtual void redo() {
			AspectModel *model = d_target->d_model;
			QString tmp = model->comment();
			model->setComment(d_other_comment);
			d_other_comment = tmp;
			d_target->aspectDescriptionChanged();
		}
		virtual void undo() { redo(); }

	private:
		AbstractAspect *d_target;
		QString d_other_comment;
};

class AspectCaptionSpecChangeCmd : public QUndoCommand
{
	public:
		AspectCaptionSpecChangeCmd(AbstractAspect *target, const QString &new_caption_spec)
			: d_target(target), d_other_caption_spec(new_caption_spec) {
				setText(QObject::tr("change caption of %1").arg(d_target->name()));
			}
		virtual void redo() {
			AspectModel *model = d_target->d_model;
			QString tmp = model->captionSpec();
			model->setCaptionSpec(d_other_caption_spec);
			d_other_caption_spec = tmp;
			d_target->aspectDescriptionChanged();
		}
		virtual void undo() { redo(); }

	private:
		AbstractAspect *d_target;
		QString d_other_caption_spec;
};

class AspectChildRemoveCmd : public QUndoCommand
{
	public:
		AspectChildRemoveCmd(AbstractAspect *target, AbstractAspect *child)
			: d_target(target), d_child(child), d_owns_child(false) {
				setText(QObject::tr("remove %1").arg(d_child->name()));
			}
		virtual ~AspectChildRemoveCmd() {
			if (d_owns_child) delete d_child;
		}
		virtual void redo() {
			d_child->aspectAboutToBeRemoved(d_child);
			AspectModel *model = d_target->d_model;
			d_index = model->indexOfChild(d_child);
			model->removeChild(d_child);
			d_child->setParentPrivate(0);
			d_target->aspectRemoved(d_target, d_index);
			d_owns_child = true;
		}
		virtual void undo() {
			d_target->aspectAboutToBeAdded(d_target, d_index);
			AspectModel *model = d_target->d_model;
			model->insertChild(d_index, d_child);
			d_child->setParentPrivate(d_target);
			d_child->aspectAdded(d_child);
			d_owns_child = false;
		}

	protected:
		AbstractAspect *d_target, *d_child;
		int d_index;
		//! Whether this command holds the ownership of the child.
		/**
		 * Just checking for parent() == 0 doesn't suffice if an undone
		 * AspectChildRemoveCmd is replaced with a new one - redo() is
		 * called on the new command before the old one is deleted.
		 */
		bool d_owns_child;
};

class AspectChildAddCmd : public AspectChildRemoveCmd
{
	public:
		AspectChildAddCmd(AbstractAspect *target, AbstractAspect *child, int index)
			: AspectChildRemoveCmd(target, child) {
				setText(QObject::tr("add %1").arg(d_child->name()));
				d_index = index;
			}
		virtual void redo() { AspectChildRemoveCmd::undo(); }
		virtual void undo() { AspectChildRemoveCmd::redo(); }
};
