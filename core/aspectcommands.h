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
#include "AspectModel.h"

#ifndef _NO_TR1_
#include "tr1/memory"
using std::tr1::shared_ptr;
#else // if your compiler does not have TR1 support, you can use boost instead:
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
#endif

#include <QUndoCommand>

// Remark: Normally, following the model-view-presenter paradigm,
// the commands should be between presenter and model and the 
// the commands should only be aware of the model.
// Due to the tree structure of the aspects, this is not possible here since the
// parent aspect needs to be known. Therefore, target of a command is
// a pointer to the parent aspect instead of its model.

class AspectNameChangeCmd : public QUndoCommand
{
	public:
		AspectNameChangeCmd(AbstractAspect *target, const QString &new_name)
			: d_target(target), d_other_name(new_name) {
				setText(QObject::tr("rename %1 to %2").arg(d_target->name()).arg(new_name));
			}

		virtual void redo() {
			shared_ptr<AspectModel> model = d_target->d_model;
			QString tmp = model->name();
			model->setName(d_other_name);
			d_other_name = tmp;
			emit d_target->abstractAspectSignalEmitter()->aspectDescriptionChanged(d_target);
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
			shared_ptr<AspectModel> model = d_target->d_model;
			QString tmp = model->comment();
			model->setComment(d_other_comment);
			d_other_comment = tmp;
			emit d_target->abstractAspectSignalEmitter()->aspectDescriptionChanged(d_target);
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
			shared_ptr<AspectModel> model = d_target->d_model;
			QString tmp = model->captionSpec();
			model->setCaptionSpec(d_other_caption_spec);
			d_other_caption_spec = tmp;
			emit d_target->abstractAspectSignalEmitter()->aspectDescriptionChanged(d_target);
		}

		virtual void undo() { redo(); }

	private:
		AbstractAspect *d_target;
		QString d_other_caption_spec;
};


class AspectChildRemoveCmd : public QUndoCommand
{
	public:
		AspectChildRemoveCmd(AbstractAspect * target, shared_ptr<AbstractAspect> child)
			: d_target(target), d_child(child) {
				setText(QObject::tr("remove %1").arg(d_child->name()));
			}

		virtual void redo() {
			shared_ptr<AspectModel> model = d_target->d_model;
			d_index = model->indexOfChild(d_child);
			Q_ASSERT(d_index != -1); // d_child must be a child of d_target
			emit d_target->abstractAspectSignalEmitter()->aspectAboutToBeRemoved(d_target, d_index);
			emit d_child->abstractAspectSignalEmitter()->aspectAboutToBeRemoved(d_child.get());
			model->removeChild(d_child);
			d_child->setParent(0);
			emit d_target->abstractAspectSignalEmitter()->aspectRemoved(d_target, d_index);
		}

		virtual void undo() {
			shared_ptr<AspectModel> model = d_target->d_model;
			Q_ASSERT(d_index != -1); // d_child must be a child of d_target
			emit d_target->abstractAspectSignalEmitter()->aspectAboutToBeAdded(d_target, d_index);
			model->insertChild(d_index, d_child);
			d_child->setParent(d_target);
			emit d_target->abstractAspectSignalEmitter()->aspectAdded(d_target, d_index);
			emit d_child->abstractAspectSignalEmitter()->aspectAdded(d_child.get());
		}

	protected:
		AbstractAspect * d_target;
		shared_ptr<AbstractAspect> d_child;
		int d_index;
};

class AspectChildAddCmd : public AspectChildRemoveCmd
{
	public:
		AspectChildAddCmd(AbstractAspect * target, shared_ptr<AbstractAspect> child, int index)
			: AspectChildRemoveCmd(target, child) {
				setText(QObject::tr("add %1").arg(d_child->name()));
				d_index = index;
			}

		virtual void redo() { AspectChildRemoveCmd::undo(); }

		virtual void undo() { AspectChildRemoveCmd::redo(); }
};
