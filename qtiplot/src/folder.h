#ifndef FOLDER_H
#define FOLDER_H

#include <qobject.h>
#include <qlistview.h>
#include <qiconview.h>

#include "widget.h"

class FolderListItem;

class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QDragObject;

class Folder : public QObject
{
    Q_OBJECT

public:
    Folder( Folder *parent, const QString &name );
    ~Folder();

	QPtrList<myWidget> windowsList(){return lstWindows;};

    void addWindow( myWidget *w ){ lstWindows.append( w );};
	void removeWindow( myWidget *w ){ lstWindows.take( lstWindows.find(w) );};

	void setFolderName(const QString& s){fName = s;};
    QString folderName() { return fName;};

	//! The list of subfolder names, including first generation children only
	QStringList subfolders();

	//! Pointer to the subfolder called s
	Folder* findSubfolder(const QString& s, bool caseSensitive = TRUE, bool partialMatch = FALSE);

	//! Pointer to the first window matching the search criteria
	myWidget* findWindow(const QString& s, bool windowNames, bool labels, 
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
    QPtrList<myWidget> lstWindows;
	FolderListItem *myFolderListItem;
};

/*****************************************************************************
 *
 * Class WindowListItem
 *
 *****************************************************************************/

class WindowListItem : public QListViewItem
{
public:
    WindowListItem( QListView *parent, myWidget *w );

    myWidget *window() { return myWindow; };
	void cancelRename(int){return;};

protected:
    myWidget *myWindow;
};

/*****************************************************************************
 *
 * Class FolderListItem
 *
 *****************************************************************************/

class FolderListItem : public QListViewItem
{
public:
    FolderListItem( QListView *parent, Folder *f );
    FolderListItem( FolderListItem *parent, Folder *f );

	enum {ListItemType = 1001};

	void setActive( bool o );
	void cancelRename(int){return;};

	virtual int rtti() const {return (int)ListItemType;};

    Folder *folder() { return myFolder; };

	//! Slot: Checks weather the folder item is a grandchild of the source folder 
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

class FolderListView : public QListView
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
    void contentsMouseReleaseEvent( QMouseEvent *e );
	void contentsMouseDoubleClickEvent( QMouseEvent* e );

	void keyPressEvent ( QKeyEvent * e );

signals:
	void dragItems(QPtrList<QListViewItem> items);
	void dropItems(QListViewItem *dest);
	void renameItem(QListViewItem *item);
	void addFolderItem();
	void deleteSelection();

private:
	bool mousePressed;
	QPoint presspos;
};

#endif
