/***************************************************************************
    File                 : AbstractAspect.h
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
#ifndef ABSTRACT_ASPECT_H
#define ABSTRACT_ASPECT_H

#include <QObject>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

#ifndef _NO_TR1_
#include "tr1/memory"
using std::tr1::shared_ptr;
#else // if your compiler does not have TR1 support, you can use boost instead:
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
#endif

class AbstractAspect;
class AspectPrivate;
class Project;
class QUndoStack;
class QString;
class QWidget;
class QDateTime;
class QUndoCommand;
class QIcon;
class QMenu;
class Folder;
class AspectView;

//! Wrapper class for AbstractAspect (receives and sends signals for it)
/**
 * See: http://doc.trolltech.com/qq/qq15-academic.html
 */
class AbstractAspectWrapper : public QObject
{
	Q_OBJECT

	public:
		AbstractAspectWrapper(AbstractAspect * aspect) 
	         : QObject(0), d_aspect(aspect) {}
		virtual ~AbstractAspectWrapper() {}

	public slots:
		void setName(const QString &value);
		void setComment(const QString &value);
		void setCaptionSpec(const QString &value);
		void remove();
		void showProperties();

	signals:
		//! Emit this before the name, comment or caption spec is changed
		void aspectDescriptionAboutToChange(AbstractAspect *aspect);
		//! Emit this when the name, comment or caption spec changed
		void aspectDescriptionChanged(AbstractAspect *aspect);
		//! Emit this when a parent aspect is about to get a new child inserted
		void aspectAboutToBeAdded(AbstractAspect *parent, int index);
		//! Emit this from a newly added aspect
		void aspectAdded(AbstractAspect *aspect);
		//! Emit this from a parent after adding a new child to it
		void aspectAdded(AbstractAspect *parent, int index);
		//! Emit this from an aspect about to be removed from its parent's children
		void aspectAboutToBeRemoved(AbstractAspect *aspect);
		//! Emit this from a parent before removing its child
		void aspectAboutToBeRemoved(AbstractAspect *parent, int index);
		//! Emit this from the parent after removing a child
		void aspectRemoved(AbstractAspect *parent, int index);

	private:
		AbstractAspect * d_aspect;
		
		friend class AbstractSimpleFilter;
		// Undo commands need access to the signals
		friend class AspectNameChangeCmd;
		friend class AspectCommentChangeCmd;
		friend class AspectCaptionSpecChangeCmd;
		friend class AspectChildRemoveCmd;
		friend class AspectChildAddCmd;
		friend class Table;
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
 *
 * AbstractAspect also defines signals and slots in a wrapper class AbstractAspectWrapper. This
 * can be queried by abstractAspectSignalEmitter() and abstractAspectSignalReceiver() which
 * both return the same pointer but should be used depending on the situation to make
 * the purpose of the returned pointer very clear. The reason why AbstractAspect does not 
 * inherit from QObject is to avoid multiple inheritance from QObject in classes derived
 * from AbstractAspect. The functions inherits() and className() are provided nonetheless.
 *
 * The private data of AbstractAspect is contained in a separate class AbstractAspectPrivate. 
 * The write access to AbstractAspectPrivate should always be done using aspect commands
 * to allow undo/redo.
 *
 * The children of an aspect are addressed by smart pointers (shared_ptr) which take
 * care of deleting the children when necessary. 
 */
class AbstractAspect 
{
	public:
		AbstractAspect(const QString &name);
		virtual ~AbstractAspect();

		//! Return my parent Aspect or 0 if I currently don't have one.
		virtual AbstractAspect * parentAspect() const { return d_parent_aspect; }
		//! Return the folder the Aspect is contained in or 0 if not.
		/**
		 * The returned folder may be the aspect itself if it inherits Folder.
		*/
		virtual Folder * folder();
		//! Return whether the there is a path upwards to the given aspect
		/**
		 * This also returns true if other==this.
		 */
		virtual bool isDescendantOf(AbstractAspect *other);


		// TODO: add unique name checking
		//! Add the given Aspect to my list of children.
		void addChild(shared_ptr<AbstractAspect> child);
		//! Remove the given Aspect from my list of children.
		void removeChild(shared_ptr<AbstractAspect> child);
		//! Remove the Aspect at the given index from my list of children.
		void removeChild(int index);
		//! Get a child by its position in my list of children.
		shared_ptr<AbstractAspect> child(int index) const;
		//! Return the number of child Aspects.
		int childCount() const;
		//! Return the position of child in my list of children.
		int indexOfChild(const AbstractAspect * child) const;
		//! Return the position of child in my list of children.
		int indexOfChild(shared_ptr<AbstractAspect> child) const { return indexOfChild(child.get()); }
		//! Return my position in my parent's list of children.
		int index() const { return parentAspect() ? parentAspect()->indexOfChild(this) : 0; }

		//! See QMetaObject::className().
		virtual const char* className() const { return "AbstractAspect"; }
		//! See QObject::inherits().
		virtual bool inherits(const char *class_name) const { return (QString(class_name) == QString("AbstractAspect")); }

		//! Return the Project this Aspect belongs to, or 0 if it is currently not part of one.
		virtual Project *project() const { return parentAspect() ? parentAspect()->project() : 0; }
		//! Execute the given command, pushing it on the undoStack() if available.
		void exec(QUndoCommand *command);
		//! Begin an undo stack macro (series of commands)
		void beginMacro(const QString& text);
		//! End the undo stack macro
		void endMacro();
		//! Return the path that leads from the top-most Aspect (usually a Project) to me.
		virtual QString path() const { return parentAspect() ? parentAspect()->path() + "/" + name() : "";  }

		//! Return an icon to be used for decorating my views.
		virtual QIcon icon() const;
		//! Return a new context menu
		/**
		 * \param append_to if a pointer to a QMenu is passed
		 * to the function, the actions are appended to
		 * it instead of the creation of a new menu.
		 * Otherwise the caller takes ownership of the menu.
		 */
		virtual QMenu *createContextMenu(QMenu * append_to = 0);
		//! Construct a standard view on me.
		/**
		 * If a parent is specified, the view is added to it as a child widget and the parent takes over
		 * ownership. If no parent is given, the caller receives ownership of the view.
		 * 
		 * This method may be called multiple times during the life time of an Aspect, or it might not get
		 * called at all. Aspects must not depend on the existence of a view for their operation.
		 */
		virtual AspectView *view() = 0;

		QString name() const;
		QString comment() const;
		//! Return the specification string used for constructing the caption().
		/**
		 * See setCaptionSpec() for format.
		 */
		QString captionSpec() const;
		QDateTime creationTime() const;
		QString caption() const;

		//! Return the QObject that is responsible for emitting signals
		/**
		 * Using this mechanism avoids the need to have QObject as a base class and
		 * thus avoids multiple inheritance problems with classes derived from
		 * AbstractAspect
		 */
		AbstractAspectWrapper *abstractAspectSignalEmitter() const { return d_aspect_wrapper; }
		//! Return the QObject that is responsible for receiving signals
		/**
		 * Using this mechanism avoids the need to have QObject as a base class and
		 * thus avoids multiple inheritance problems with classes derived from
		 * AbstractAspect
		 */
		AbstractAspectWrapper *abstractAspectSignalReceiver() const { return d_aspect_wrapper; }

		//! Return the undo stack of the Project, or 0 if this Aspect is not part of a Project.
		virtual QUndoStack *undoStack() const { return parentAspect() ? parentAspect()->undoStack() : 0; }

		//! Set the parent aspect 
		void setParentAspect(AbstractAspect * new_parent);

	// wrapped slots 
	public: 
		void setName(const QString &value);
		void setComment(const QString &value);
		//! Set the specification string used for constructing the caption().
		/**
		 * The caption is constructed by substituting some magic tokens in the specification:
		 *
		 * - \%n - name()
		 * - \%c - comment()
		 * - \%t - creationTime()
		 *
		 * Additionally, you can use the construct %C{<text>}. <text> is only shown in the caption
		 * if comment() is not empty (name and creation time should never be empty).
		 *
		 * The default caption specification is "%n%C{ - }%c".
		 */
		void setCaptionSpec(const QString &value);
		//! Set the creation time
		/**
		 * The creation time will automatically be set when the aspect object
		 * is created. This function is usually only needed when the aspect
		 * is loaded from a file.
		 */
		void setCreationTime(const QDateTime& time);
		//! Remove me from my parent's list of children.
		virtual void remove() { if(parentAspect()) parentAspect()->removeChild(parentAspect()->indexOfChild(this)); }
		//! Show info about the aspect
		void showProperties();

	protected:
		AspectPrivate * d_aspect_private;
		AbstractAspect * d_parent_aspect; // making this a shared_ptr would lead to circular references
		AbstractAspectWrapper *d_aspect_wrapper;

		// Undo commands need direct access to the model.
		friend class AspectNameChangeCmd;
		friend class AspectCommentChangeCmd;
		friend class AspectCaptionSpecChangeCmd;
		friend class AspectChildRemoveCmd;
		friend class AspectChildAddCmd;
};

#endif // ifndef ABSTRACT_ASPECT_H
