/***************************************************************************
    File                 : AbstractAspect.h
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
#ifndef ABSTRACT_ASPECT_H
#define ABSTRACT_ASPECT_H

#include <QObject>

class AbstractAspect;
class AspectModel;
class Project;
class QUndoStack;
class QString;
class QWidget;
class QDateTime;
class QUndoCommand;
class QIcon;
class QMenu;

typedef QList<AbstractAspect*> AspectList;

//! Interface for receiving messages about changes in the Aspect hierarchy.
class AbstractAspectObserver
{
	public:
		virtual ~AbstractAspectObserver() {}
		virtual void aspectDescriptionChanged(AbstractAspect *aspect) {
			Q_UNUSED(aspect);
		}
		virtual void aspectAboutToBeAdded(AbstractAspect *parent, int index) {
			Q_UNUSED(parent); Q_UNUSED(index);
		}
		virtual void aspectAdded(AbstractAspect *aspect) {
			Q_UNUSED(aspect);
		}
		virtual void aspectAboutToBeRemoved(AbstractAspect *aspect) {
			Q_UNUSED(aspect);
		}
		virtual void aspectRemoved(AbstractAspect *parent, int index) {
			Q_UNUSED(parent); Q_UNUSED(index);
		}
};

//! Base class of all persistent objects in a Project.
/**
 * Aspects organize themselves into trees, where a parent takes ownership of its children. Usually,
 * though not necessarily, a Project instance will sit at the root of the tree (without a Project
 * ancestor, project() will return 0 and undo does not work). Children are organized using
 * addChild(), removeChild(), child(), indexOfChild() and childCount() on the parent's side as well
 * as the equivalent convenience methods index() and remove() on the child's side.
 * 
 * AbstractAspect manages for every Aspect the properties #name, #comment, #caption_spec and
 * #creation_time. All of these translate into the caption() as described in the documentation
 * of setCaptionSpec().
 *
 * If an undoStack() can be found (usually it is managed by Project), changes to the properties
 * as well as adding/removing childs support multi-level undo/redo. In order to support undo/redo
 * for problem-specific data in derived classes, make sure that all changes to your data are done
 * by handing appropriate commands to exec().
 *
 * The only thing that's mandatory when deriving from AbstractAspect is to implement view().
 * This allows views to automatically create views on new childs of their Aspect. Optionally,
 * you can supply an icon() to be used by different views (including the ProjectExplorer)
 * and/or reimplement createContextMenu() for a custom context menu of views.
 */
class AbstractAspect : public QObject, public AbstractAspectObserver
{
	Q_OBJECT

	public:
		AbstractAspect(const QString &name);
		virtual ~AbstractAspect();

		//! Return my parent Aspect or 0 if I currently don't have one.
		virtual AbstractAspect *parentAspect() const = 0; //{ return static_cast<AbstractAspect*>(QObject::parent()); }
		//! Add the given Aspect to my list of children.
		void addChild(AbstractAspect *child);
		//! Remove the given Aspect from my list of children.
		void removeChild(AbstractAspect *child);
		//! Get a child by its position in my list of children.
		AbstractAspect *child(int index) const;
		//! Return the number of child Aspects.
		int childCount() const;
		//! Return the position of child in my list of children.
		int indexOfChild(const AbstractAspect *child) const;
		//! Return my position in my parent's list of children.
		int index() const { return parentAspect() ? parentAspect()->indexOfChild(this) : 0; }

		//! See QMetaObject::className().
		const char* className() const { return metaObject()->className(); }
		//! See QObject::inherits().
		bool inherits(const char *class_name) const { return QObject::inherits(class_name); }

		//! Return the Project this Aspect belongs to, or 0 if it is currently not part of one.
		virtual Project *project() const = 0; //{ return parent() ? parent()->project() : 0; }
		//! Execute the given command, pushing it on the undoStack() if available.
		void exec(QUndoCommand *command);
		//! Return the path that leads from the top-most Aspect (usually a Project) to me.
		virtual QString path() const = 0; //{ return parent() ? "" : parent()->path() + "/" + name(); }

		//! Return an icon to be used for decorating my views.
		virtual QIcon icon() const;
		//! Return a new context menu for my views.
		/**
		 * Caller takes ownership of the menu.
		 */
		virtual QMenu *createContextMenu();
		//! Construct a standard view on me.
		/**
		 * If a parent is specified, the view is added to it as a child widget and the parent takes over
		 * ownership. If no parent is given, the caller receives ownership of the view.
		 * 
		 * This method may be called multiple times during the life time of an Aspect, or it might not get
		 * called at all. Aspects must not depend on the existence of a view for their operation.
		 */
		virtual QWidget *view(QWidget *parent = 0) = 0;

		Q_PROPERTY(QString name READ name WRITE setName);
		Q_PROPERTY(QString comment READ comment WRITE setComment);
		Q_PROPERTY(QString caption_spec READ captionSpec WRITE setCaptionSpec);
		// requires #include<QDateTime> and most classes including AbstractAspect.h
		// won't need it... hmm.. TODO: decide what to do
		//Q_PROPERTY(QDateTime creation_time READ creationTime);
		Q_PROPERTY(QString caption READ caption);

		QString name() const;
		QString comment() const;
		//! Return the specification string used for constructing the caption().
		/**
		 * See setCaptionSpec() for format.
		 */
		QString captionSpec() const;
		QDateTime creationTime() const;
		QString caption() const;

		void addAspectObserver(AbstractAspectObserver *observer) {
			d_observers << observer;
		}
		void removeAspectObserver(AbstractAspectObserver *observer) {
			d_observers.removeAll(observer);
		}
		virtual void aspectDescriptionChanged(AbstractAspect *aspect = 0) {
			if (!aspect) aspect = this;
			foreach(AbstractAspectObserver *observer, d_observers)
				observer->aspectDescriptionChanged(aspect);
		}
		virtual void aspectAboutToBeAdded(AbstractAspect *parent, int index) {
			foreach(AbstractAspectObserver *observer, d_observers)
				observer->aspectAboutToBeAdded(parent, index);
		}
		virtual void aspectAdded(AbstractAspect *aspect) {
			foreach(AbstractAspectObserver *observer, d_observers)
				observer->aspectAdded(aspect);
		}
		virtual void aspectAboutToBeRemoved(AbstractAspect *aspect) {
			foreach(AbstractAspectObserver *observer, d_observers)
				observer->aspectAboutToBeRemoved(aspect);
		}
		virtual void aspectRemoved(AbstractAspect *parent, int index) {
			foreach(AbstractAspectObserver *observer, d_observers)
				observer->aspectRemoved(parent, index);
		}

	public slots:
		void setName(const QString &value);
		void setComment(const QString &value);
		//! Set the specification string used for constructing the caption().
		/**
		 * The caption is constructed by substituting some magic tokens in the specification:
		 *
		 * * %n - name()
		 * * %c - comment()
		 * * %t - creationTime()
		 *
		 * Additionally, you can use the construct %C{<text>}. <text> is only shown in the caption
		 * if comment() is not empty (name and creation time should never be empty).
		 *
		 * The default caption specification is "%n%C{ - }%c".
		 */
		void setCaptionSpec(const QString &value);
		//! Remove me from my parent's list of children.
		virtual void remove() = 0; // { if(parent()) parent()->removeChild(this); }
		//! Return the undo stack of the Project, or 0 if this Aspect is not part of a Project.
		virtual QUndoStack *undoStack() const = 0; // { return parent() ? parent()->undoStack() : 0; }

	private:
		AspectModel *d_model;
		QList<AbstractAspectObserver*> d_observers;

		// Undo commands need direct access to the model.
		friend class AspectNameChangeCmd;
		friend class AspectCommentChangeCmd;
		friend class AspectCaptionSpecChangeCmd;
		friend class AspectChildRemoveCmd;
		friend class AspectChildAddCmd;

		//! Wrapper around QObject::setParent() which also updates the list of observers.
		void setParentPrivate(AbstractAspect *new_parent);
};

#endif // ifndef ABSTRACT_ASPECT_H
