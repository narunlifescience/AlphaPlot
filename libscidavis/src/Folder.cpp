/***************************************************************************
    File                 : Folder.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Benkert,
					  Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Folder for the project explorer

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
#include "Folder.h"

#include <qobject.h>
#include <qdatetime.h>
#include <qpixmap.h>

#include <qevent.h>
#include <qpoint.h>
#include <qmessagebox.h>
#include <q3dragobject.h>
#include <qmime.h>
#include <q3strlist.h>
#include <qstringlist.h>
#include <q3header.h>
#include <qapplication.h>
#include <q3dragobject.h>
#include <q3iconview.h>
#include <qcursor.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <QKeyEvent>
#include <QDropEvent>
#include <QMouseEvent>

Folder::Folder( Folder *parent, const QString &name )
	: QObject(parent), d_active_window(0)
{
	QObject::setObjectName(name);
	birthdate = QDateTime::currentDateTime ().toString(Qt::LocalDate);

	// FIXME: This doesn't work anymore in Qt4, need alternative method
	// lstWindows.setAutoDelete( true );
}

QList<Folder*> Folder::folders() const
{
	QList<Folder*> lst;
	foreach(QObject *f, children())
		lst.append((Folder*) f);
	return lst;
}

QStringList Folder::subfolders()
{
	QStringList list = QStringList();
	QObjectList folderList = children();
	if (!folderList.isEmpty()){
		QObject * f;
		foreach(f,folderList)
			list << static_cast<Folder *>(f)->name();
	}
	return list;
}

QString Folder::path()
{
    QString s = "/" + QString(name()) + "/";
    Folder *parentFolder = (Folder *)parent();
    while (parentFolder){
        s.prepend("/" + QString(parentFolder->name()));
        parentFolder = (Folder *)parentFolder->parent();
	}
    return s;
}

Folder* Folder::findSubfolder(const QString& s, bool caseSensitive, bool partialMatch)
{
	QObjectList folderList = children();
	if (!folderList.isEmpty()){
		QObject * f;

		foreach(f,folderList){
			QString name = static_cast<Folder *>(f)->name();
			if (partialMatch){
				if (caseSensitive && name.startsWith(s,Qt::CaseSensitive))
					return static_cast<Folder *>(f);
				else if (!caseSensitive && name.startsWith(s,Qt::CaseInsensitive))
					return static_cast<Folder *>(f);
			}
			else // partialMatch == false
			{
				if (caseSensitive && name == s)
					return static_cast<Folder *>(f);
				else if ( !caseSensitive && (name.toLower() == s.toLower()) )
					return static_cast<Folder *>(f);
			}
		}
	}
	return 0;
}

MyWidget* Folder::findWindow(const QString& s, bool windowNames, bool labels,
							 bool caseSensitive, bool partialMatch)
{
	MyWidget* w;
	foreach(w,lstWindows)
	{
		if (windowNames)
		{
			QString name = w->name();
			if (partialMatch && name.startsWith(s, caseSensitive))
				return w;
			else if (caseSensitive && name == s)
				return w;
			else
			{
				QString text = s;
				if (name == text.toLower())
					return w;
			}
		}

		if (labels)
		{
			QString label = w->windowLabel();
			if (partialMatch && label.startsWith(s, caseSensitive))
				return w;
			else if (caseSensitive && label == s)
				return w;
			else
			{
				QString text = s;
				if (label == text.toLower())
					return w;
			}
		}
	}
	return 0;
}

MyWidget *Folder::window(const QString &name, const char *cls, bool recursive)
{
	foreach (MyWidget *w, lstWindows)
		if (w->inherits(cls) && name == w->name())
			return w;
	if (!recursive) return NULL;
	foreach (QObject *f, children())
	{
		MyWidget *w = ((Folder*)f)->window(name, cls, true);
		if (w) return w;
	}
	return NULL;
}

Folder* Folder::rootFolder()
{
	Folder *i = this;
	while(i->parent())
		i = (Folder*)i->parent();
	return i;
}

/*****************************************************************************
 *
 * Class FolderListItem
 *
 *****************************************************************************/

FolderListItem::FolderListItem( Q3ListView *parent, Folder *f )
    : Q3ListViewItem( parent )
{
    myFolder = f;

    setText( 0, f->name() );
	setOpen( true );
	setActive( true );
	setDragEnabled ( true );
	setDropEnabled ( true );
}

FolderListItem::FolderListItem( FolderListItem *parent, Folder *f )
    : Q3ListViewItem( parent )
{
    myFolder = f;

    setText( 0, f->name() );
	setOpen( true );
	setActive( true );
}

void FolderListItem::setActive( bool o )
{
    if ( o )
		setPixmap(0, QPixmap(":/folder_open.xpm") );
    else
		setPixmap(0, QPixmap(":/folder_closed.xpm") );

	setSelected(o);
}

bool FolderListItem::isChildOf(FolderListItem *src)
{
FolderListItem *parent = (FolderListItem *)this->parent();
while (parent)
	{
	if (parent == src)
		return true;

	parent = (FolderListItem *)parent->parent();
	}
return false;
}

/*****************************************************************************
 *
 * Class FolderListView
 *
 *****************************************************************************/

FolderListView::FolderListView( QWidget *parent, const QString name )
	: Q3ListView( parent, name.toLocal8Bit().constData() ), mousePressed( false )
{
    setAcceptDrops( true );
    viewport()->setAcceptDrops( true );
}

void FolderListView::startDrag()
{
Q3ListViewItem *item = currentItem();
if (!item)
	return;

if (item == firstChild() && item->listView()->rootIsDecorated())
	return;//it's the project folder so we don't want the user to move it

// QPoint orig = viewportToContents( viewport()->mapFromGlobal( QCursor::pos() ) );

QPixmap pix;
if (item->rtti() == FolderListItem::RTTI)
	pix = QPixmap(":/folder_closed.xpm");
else
	pix = *item->pixmap (0);

Q3IconDrag *drag = new Q3IconDrag(viewport());
drag->setPixmap(pix, QPoint(pix.width()/2, pix.height()/2 ) );

QList<Q3ListViewItem *> lst;
for (item = firstChild(); item; item = item->itemBelow())
	{
	if (item->isSelected())
		lst.append(item);
	}

emit dragItems(lst);
drag->drag();
}

void FolderListView::contentsDropEvent( QDropEvent *e )
{
Q3ListViewItem *dest = itemAt( contentsToViewport(e->pos()) );
if ( dest && dest->rtti() == FolderListItem::RTTI)
	{
	emit dropItems(dest);
	e->accept();
    }
else
	e->ignore();
}

void FolderListView::keyPressEvent ( QKeyEvent * e )
{
if (isRenaming())
	{
	e->ignore();
	return;
	}
Q3ListViewItem *item = currentItem();
if (!item) {
	Q3ListView::keyPressEvent ( e );
	return;
}

if (item->rtti() == FolderListItem::RTTI &&
	(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return))
	{
	emit doubleClicked(item);
	e->accept();
	}
else if (e->key() == Qt::Key_F2)
	{
	if (item)
		emit renameItem(item);
	e->accept();
	}
else if(e->key() == Qt::Key_A && e->state() == Qt::ControlModifier)
	{
	selectAll(true);
	e->accept();
	}
else if(e->key() == Qt::Key_F7)
	{
	emit addFolderItem();
	e->accept();
	}
else if(e->key() == Qt::Key_F8)
	{
	emit deleteSelection();
	e->accept();
	}
else
	Q3ListView::keyPressEvent ( e );
}

void FolderListView::contentsMouseDoubleClickEvent( QMouseEvent* e )
{
	if (isRenaming())
		{
		e->ignore();
		return;
		}

	Q3ListView::contentsMouseDoubleClickEvent( e );
}

void FolderListView::contentsMousePressEvent( QMouseEvent* e )
{
Q3ListView::contentsMousePressEvent(e);
if (e->button() != Qt::LeftButton) return;
QPoint p( contentsToViewport( e->pos() ) );
Q3ListViewItem *i = itemAt( p );

if ( i )
		{// if the user clicked into the root decoration of the item, don't try to start a drag!
		if ( p.x() > header()->cellPos( header()->mapToActual( 0 ) ) +
			treeStepSize() * ( i->depth() + ( rootIsDecorated() ? 1 : 0) ) + itemMargin() ||
			p.x() < header()->cellPos( header()->mapToActual( 0 ) ) )
			{
			presspos = e->pos();
	    	mousePressed = true;
			}
    	}
}

void FolderListView::contentsMouseMoveEvent( QMouseEvent* e )
{
if ( mousePressed && ( presspos - e->pos() ).manhattanLength() > QApplication::startDragDistance() )
	{
	mousePressed = false;
	Q3ListViewItem *item = itemAt( contentsToViewport(presspos) );
	if ( item )
		startDrag();
    }
}

void FolderListView::adjustColumns()
{
for (int i=0; i < columns (); i++)
	adjustColumn(i);
}

/*****************************************************************************
 *
 * Class WindowListItem
 *
 *****************************************************************************/

WindowListItem::WindowListItem( Q3ListView *parent, MyWidget *w )
    : Q3ListViewItem( parent )
{
    myWindow = w;

	setDragEnabled ( true );
}
