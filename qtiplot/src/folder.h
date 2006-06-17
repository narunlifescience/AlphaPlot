/***************************************************************************
    File                 : folder.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#ifndef FOLDER_H
#define FOLDER_H

#include <qobject.h>
#include <q3listview.h>
#include <q3iconview.h>
//Added by qt3to4:
#include <QDragEnterEvent>
#include <QMouseEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QDropEvent>
#include <Q3PtrList>

#include "widget.h"

class FolderListItem;

class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class Q3DragObject;

//! Folder for the project explorer
class Folder : public QObject
{
    Q_OBJECT

public:
    Folder( Folder *parent, const QString &name );
    ~Folder();

	QList<MyWidget *> windowsList(){return lstWindows;};

    void addWindow( MyWidget *w ){ lstWindows.append( w );};
	void removeWindow( MyWidget *w ){ lstWindows.takeAt( lstWindows.indexOf(w) );};

	void setFolderName(const QString& s){fName = s;};
    QString folderName() { return fName;};

	//! The list of subfolder names, including first generation children only
	QStringList subfolders();

	//! Pointer to the subfolder called s
	Folder* findSubfolder(const QString& s, bool caseSensitive = true, bool partialMatch = false);

	//! Pointer to the first window matching the search criteria
	MyWidget* findWindow(const QString& s, bool windowNames, bool labels, 
							 bool caseSensitive, bool partialMatch);

	//! The complete path of the folder in the project tree
	QString path();

	//! Size of the folder as a string
	QString sizeToString();

	QString birthDate(){return birthdate;};
	void setBirthDate(const QString& s){birthdate = s;};

	QString modificationDate(){return modifDate;};
	void setModificationDate(const QString& s){modifDate = s;};

	//! Pointer to the corresponding QListViewItem in the main application
	FolderListItem * folderListItem(){return myFolderListItem;};
	void setFolderListItem(FolderListItem *it){myFolderListItem = it;};

protected:
    QString fName, birthdate, modifDate;
    QList<MyWidget *> lstWindows;
	FolderListItem *myFolderListItem;
};

/*****************************************************************************
 *
 * Class WindowListItem
 *
 *****************************************************************************/
//! Windows list item class
class WindowListItem : public Q3ListViewItem
{
public:
    WindowListItem( Q3ListView *parent, MyWidget *w );

    MyWidget *window() { return myWindow; };
	void cancelRename(int){return;};

protected:
    MyWidget *myWindow;
};

/*****************************************************************************
 *
 * Class FolderListItem
 *
 *****************************************************************************/
//! Folders list item class
class FolderListItem : public Q3ListViewItem
{
public:
    FolderListItem( Q3ListView *parent, Folder *f );
    FolderListItem( FolderListItem *parent, Folder *f );

	enum {ListItemType = 1001};

	void setActive( bool o );
	void cancelRename(int){return;};

	virtual int rtti() const {return (int)ListItemType;};

    Folder *folder() { return myFolder; };

	//! This slot checks weather the folder item is a grandchild of the source folder 
	/**
	 * \param src source folder item
	 */
	bool isChildOf(FolderListItem *src);

protected:
    Folder *myFolder;
};

/*****************************************************************************
 *
 * Class FolderListView
 *
 *****************************************************************************/
//! Folder list view class
class FolderListView : public Q3ListView
{
    Q_OBJECT

public:
    FolderListView( QWidget *parent = 0, const char *name = 0 );

public slots:
	void adjustColumns();

protected:
	void startDrag();

    void contentsDropEvent( QDropEvent *e );
    void contentsMouseMoveEvent( QMouseEvent *e );
    void contentsMousePressEvent( QMouseEvent *e );
	void contentsMouseDoubleClickEvent( QMouseEvent* e );
	void keyPressEvent ( QKeyEvent * e );
    void contentsMouseReleaseEvent( QMouseEvent *){mousePressed = false;};
	void enterEvent(QEvent *){mousePressed = false;};

signals:
	void dragItems(QList<Q3ListViewItem *> items);
	void dropItems(Q3ListViewItem *dest);
	void renameItem(Q3ListViewItem *item);
	void addFolderItem();
	void deleteSelection();

private:
	bool mousePressed;
	QPoint presspos;
};

#endif
