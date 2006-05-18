#include "folder.h"

#include <qobjectlist.h>
#include <qdatetime.h>
#include <qpixmap.h>

#include <qevent.h>
#include <qpoint.h>
#include <qmessagebox.h>
#include <qdragobject.h>
#include <qmime.h>
#include <qstrlist.h>
#include <qstringlist.h>
#include <qheader.h>
#include <qapplication.h>
#include <qdragobject.h>
#include <qiconview.h>
#include <qcursor.h>

static const char* folder_closed_xpm[]={
    "16 16 9 1",
    "g c #808080",
    "b c #c0c000",
    "e c #c0c0c0",
    "# c #000000",
    "c c #ffff00",
    ". c None",
    "a c #585858",
    "f c #a0a0a4",
    "d c #ffffff",
    "..###...........",
    ".#abc##.........",
    ".#daabc#####....",
    ".#ddeaabbccc#...",
    ".#dedeeabbbba...",
    ".#edeeeeaaaab#..",
    ".#deeeeeeefe#ba.",
    ".#eeeeeeefef#ba.",
    ".#eeeeeefeff#ba.",
    ".#eeeeefefff#ba.",
    ".##geefeffff#ba.",
    "...##gefffff#ba.",
    ".....##fffff#ba.",
    ".......##fff#b##",
    ".........##f#b##",
    "...........####."};

static const char* folder_open_xpm[]={
    "16 16 11 1",
    "# c #000000",
    "g c #c0c0c0",
    "e c #303030",
    "a c #ffa858",
    "b c #808080",
    "d c #a0a0a4",
    "f c #585858",
    "c c #ffdca8",
    "h c #dcdcdc",
    "i c #ffffff",
    ". c None",
    "....###.........",
    "....#ab##.......",
    "....#acab####...",
    "###.#acccccca#..",
    "#ddefaaaccccca#.",
    "#bdddbaaaacccab#",
    ".eddddbbaaaacab#",
    ".#bddggdbbaaaab#",
    "..edgdggggbbaab#",
    "..#bgggghghdaab#",
    "...ebhggghicfab#",
    "....#edhhiiidab#",
    "......#egiiicfb#",
    "........#egiibb#",
    "..........#egib#",
    "............#ee#"};

Folder::Folder( Folder *parent, const QString &name )
    : QObject( parent, name ), fName( name )
{
    QDateTime dt = QDateTime::currentDateTime ();
	birthdate = dt.toString(Qt::LocalDate);

	lstWindows.setAutoDelete( true );
}

QStringList Folder::subfolders()
{
QStringList lst = QStringList();
QObjectList* folderLst = (QObjectList*)children();
if (!folderLst)
	return lst;

if (folderLst)
	{
	Folder *f;
	for (f = (Folder*)folderLst->first(); f; f = (Folder*)folderLst->next())
		lst << f->folderName();
	}
return lst;
}

QString Folder::path()
{
QString s = "/" + fName + "/";
Folder *parentFolder = (Folder *)parent();
while (parentFolder)
	{
	s.prepend("/"+parentFolder->folderName());
	parentFolder = (Folder *)parentFolder->parent();
	}
return s;
}

Folder* Folder::findSubfolder(const QString& s, bool caseSensitive, bool partialMatch)
{
QObjectList* folderLst = (QObjectList*)children();
if (folderLst)
	{
	Folder *f;
	for (f = (Folder*)folderLst->first(); f; f = (Folder*)folderLst->next())
		{
		QString name = f->folderName();
		if (partialMatch && name.startsWith(s, caseSensitive))
			return f;
		else if (caseSensitive && name == s)
			return f;
		else 
			{
			QString text = s;
			if (name == text.lower())
				return f;
			}
		}
	}
return 0;
}

myWidget* Folder::findWindow(const QString& s, bool windowNames, bool labels, 
							 bool caseSensitive, bool partialMatch)
{
myWidget* w = 0;
for (w = lstWindows.first(); w; w = lstWindows.next())
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
			if (name == text.lower())
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
			if (label == text.lower())
				return w;
			}
		}
	}
return w;
}

QString Folder::sizeToString()
{
int size = 0;

QObjectList* folderLst = (QObjectList*)children();
if (folderLst)
	{
	Folder *f;
	for (f = (Folder*)folderLst->first(); f; f = (Folder*)folderLst->next())
		size += sizeof(f);
	}

myWidget *w;
for (w = lstWindows.first(); w ; w = lstWindows.next())
	size += sizeof(w);

return QString::number(8*size/1024.0,'f',1)+" "+tr("kB")+" ("+QString::number(8*size)+" "+tr("bytes")+")";
}

Folder::~Folder()
{
}

/*****************************************************************************
 *
 * Class FolderListItem
 *
 *****************************************************************************/

FolderListItem::FolderListItem( QListView *parent, Folder *f )
    : QListViewItem( parent )
{
    myFolder = f;

    setText( 0, f->folderName() );
	setOpen( true );
	setActive( true );
	setDragEnabled ( true );
	setDropEnabled ( true );
}

FolderListItem::FolderListItem( FolderListItem *parent, Folder *f )
    : QListViewItem( parent )
{
    myFolder = f;

    setText( 0, f->folderName() );
	setOpen( true );
	setActive( true );
}

void FolderListItem::setActive( bool o )
{
    if ( o )
		setPixmap(0, QPixmap( folder_open_xpm ) );
    else
		setPixmap(0, QPixmap( folder_closed_xpm ) );

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

FolderListView::FolderListView( QWidget *parent, const char *name )
    : QListView( parent, name ), mousePressed( FALSE )
{
    setAcceptDrops( TRUE );
    viewport()->setAcceptDrops( TRUE );
}

void FolderListView::startDrag()
{
QListViewItem *item = currentItem();
if (!item)
	return;

if (item == firstChild() && item->listView()->rootIsDecorated())
	return;//it's the project folder so we don't want the user to move it

QPoint orig = viewportToContents( viewport()->mapFromGlobal( QCursor::pos() ) );

QPixmap pix;
if (item->rtti() == FolderListItem::ListItemType)
	pix = QPixmap( folder_closed_xpm );
else
	pix = *item->pixmap (0);

QIconDrag *drag = new QIconDrag(viewport());
drag->setPixmap(pix, QPoint(pix.width()/2, pix.height()/2 ) );

QPtrList<QListViewItem> lst;
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
QListViewItem *dest = itemAt( contentsToViewport(e->pos()) );
if ( dest && dest->rtti() == FolderListItem::ListItemType) 
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

if (currentItem()->rtti() == FolderListItem::ListItemType &&
	(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return))
	{
	emit doubleClicked(currentItem());
	e->accept();
	}
else if (e->key() == Qt::Key_F2)
	{
	if (currentItem())
		emit renameItem(currentItem());
	e->accept();
	}
else if(e->key() == Qt::Key_A && e->state() == Qt::ControlButton)
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
	QListView::keyPressEvent ( e );
}

void FolderListView::contentsMouseDoubleClickEvent( QMouseEvent* e )
{
	if (isRenaming())
		{
		e->ignore();
		return;
		}
	
	QListView::contentsMouseDoubleClickEvent( e );
}

void FolderListView::contentsMousePressEvent( QMouseEvent* e )
{	
QListView::contentsMousePressEvent(e);
QPoint p( contentsToViewport( e->pos() ) );
QListViewItem *i = itemAt( p );

if ( i ) 
		{// if the user clicked into the root decoration of the item, don't try to start a drag!
		if ( p.x() > header()->cellPos( header()->mapToActual( 0 ) ) +
			treeStepSize() * ( i->depth() + ( rootIsDecorated() ? 1 : 0) ) + itemMargin() ||
			p.x() < header()->cellPos( header()->mapToActual( 0 ) ) ) 
			{
			presspos = e->pos();
	    	mousePressed = TRUE;
			}
    	}
}

void FolderListView::contentsMouseMoveEvent( QMouseEvent* e )
{
if ( mousePressed && ( presspos - e->pos() ).manhattanLength() > QApplication::startDragDistance() ) 
	{
	mousePressed = FALSE;
	QListViewItem *item = itemAt( contentsToViewport(presspos) );
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

WindowListItem::WindowListItem( QListView *parent, myWidget *w )
    : QListViewItem( parent )
{
    myWindow = w;

	setDragEnabled ( true );
}
