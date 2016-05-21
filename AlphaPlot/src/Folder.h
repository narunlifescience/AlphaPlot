/* This file is part of AlphaPlot.
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>
   Copyright 2006, Ion Vasilief <ion_vasilief@yahoo.fr>
   Copyright 2006, Knut Franke <knut.franke@gmx.de>
   Copyright 2006, Tilman Benkert <thzs@gmx.net>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : Folder, items & widgets for the project explorer */

#ifndef FOLDER_H
#define FOLDER_H

#include "MyWidget.h"

#include "Matrix.h"
#include "MultiLayer.h"
#include "Note.h"
#include "Table.h"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>
#include <QTreeWidget>

class FolderListItem;
class FolderTreeWidgetItem;

class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;

// Folder for the project explorer
class Folder : public QObject {
  Q_OBJECT

 public:
  Folder(Folder *parent, const QString &name);

  QList<MyWidget *> windowsList() { return lstWindows; }

  void addWindow(MyWidget *window) {
    window->setFolder(this);
    lstWindows.append(window);
  }
  void removeWindow(MyWidget *window) {
    window->setFolder(0);
    lstWindows.removeAll(window);
    if (window == d_active_window) d_active_window = 0;
  }

  int windowCount(bool recursive = false) const {
    int result = lstWindows.size();
    if (recursive) foreach (Folder *folder, folders())
        result += folder->windowCount(true);
    return result;
  }

  // The list of subfolder names, including first generation children only
  QStringList subfolders();

  // The list of subfolders
  QList<Folder *> folders() const;

  // Pointer to the subfolder called s
  Folder *findSubfolder(const QString &subFolderName, bool caseSensitive = true,
                        bool partialMatch = false);

  // Pointer to the first window matching the search criteria
  MyWidget *findWindow(const QString &name, bool windowNames, bool labels,
                       bool caseSensitive, bool partialMatch);

  // get a window by name
  /* Returns the first window with given name that inherits class cls; NULL on
   * failure. If recursive is true, do a depth-first recursive search. */
  MyWidget *window(const QString &name, const char *cls = "myWidget",
                   bool recursive = false);
  // Return table named name or NULL
  Table *table(const QString &name, bool recursive = false) {
    return qobject_cast<Table *>(window(name, "Table", recursive));
  }
  // Return matrix named name or NULL
  Matrix *matrix(const QString &name, bool recursive = false) {
    return qobject_cast<Matrix *>(window(name, "Matrix", recursive));
  }
  // Return graph named name or NULL
  MultiLayer *graph(const QString &name, bool recursive = false) {
    return qobject_cast<MultiLayer *>(window(name, "MultiLayer", recursive));
  }
  // Return note named name or NULL
  Note *note(const QString &name, bool recursive = false) {
    return qobject_cast<Note *>(window(name, "Note", recursive));
  }

  // The complete path of the folder in the project tree
  QString path();

  // The root of the hierarchy this folder belongs to.
  Folder *rootFolder();

  QString birthDate() { return birthdate; }
  void setBirthDate(const QString &date) { birthdate = date; }

  QString modificationDate() { return modifDate; }
  void setModificationDate(const QString &date) { modifDate = date; }

  FolderTreeWidgetItem *folderTreeWidgetItem() {
    return myFolderTreeWidgetItem;
  }

  void setFolderTreeWidgetItem(FolderTreeWidgetItem *item) {
    myFolderTreeWidgetItem = item;
  }

  MyWidget *activeWindow() { return d_active_window; }
  void setActiveWindow(MyWidget *window) { d_active_window = window; }

  // TODO: move to Aspect
  QString name() { return objectName(); }
  void setName(const QString &name) { setObjectName(name); }

 protected:
  QString birthdate, modifDate;
  QList<MyWidget *> lstWindows;
  FolderTreeWidgetItem *myFolderTreeWidgetItem;

  // Pointer to the active window in the folder
  MyWidget *d_active_window;
};

//--------------------------class WindowTreeWidgetItem-------------------------

class WindowTableWidgetItem : public QTreeWidgetItem {
 public:
  WindowTableWidgetItem(QTreeWidget *parent, MyWidget *w);
  MyWidget *window() { return myWindow; }

 protected:
  MyWidget *myWindow;
};

//--------------------------class FolderTreeWidgetItem-------------------------

class FolderTreeWidgetItem : public QTreeWidgetItem {
 public:
  FolderTreeWidgetItem(QTreeWidget *parent, Folder *dir);
  FolderTreeWidgetItem(FolderTreeWidgetItem *parent, Folder *dir);

  void setActive(bool status);

  Folder *folder() { return myFolder; }

  // Checks weather the folder item is a grandchild of the source folder
  bool isChildOf(FolderTreeWidgetItem *src);

 protected:
  Folder *myFolder;
};

//--------------------------class FolderTreeWidget-----------------------------

// Folder list view class
/*class FolderListView : public Q3ListView {
  Q_OBJECT

 public:
  FolderListView(QWidget *parent = 0, const QString name = QString());

 public slots:
  void adjustColumns();

 protected:
  void startDrag();

  void contentsDropEvent(QDropEvent *e);
  void contentsMouseMoveEvent(QMouseEvent *e);
  void contentsMousePressEvent(QMouseEvent *e);
  void contentsMouseDoubleClickEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void contentsMouseReleaseEvent(QMouseEvent *) { mousePressed = false; }
  void enterEvent(QEvent *) { mousePressed = false; }

 signals:
  void dragItems(QList<Q3ListViewItem *> items);
  void dropItems(Q3ListViewItem *dest);
  void renameItem(Q3ListViewItem *item);
  void addFolderItem();
  void deleteSelection();

 private:
  bool mousePressed;
  QPoint presspos;
};*/

class FolderTreeWidget : public QTreeWidget {
  Q_OBJECT

 public:
  FolderTreeWidget(QWidget *parent = 0, const QString name = QString());

  // Item types
  enum ItemType {
    Folders = 1001,
    Windows = 1002,
  };

 public slots:
  void adjustColumns();

 protected:
  void startDrag(Qt::DropActions);
  void dropEvent(QDropEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);

 signals:
  void dragItems(QList<QTreeWidgetItem *> items);
  void dropItems(QTreeWidgetItem *dest);
  void renameItem(QTreeWidgetItem *item);
  void addFolderItem();
  void deleteSelection();

 private:
  bool mousePressed;
  QPoint presspos;
};

#endif  // FOLDER_H
