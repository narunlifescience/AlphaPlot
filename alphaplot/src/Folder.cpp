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

#include "Folder.h"
#include "core/IconLoader.h"

#include <QCursor>
#include <QDateTime>
#include <QDropEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMessageBox>
#include <QMouseEvent>
#include <QObject>
#include <QPixmap>
#include <QPoint>
#include <QStringList>
#include <QTime>

QStringList Folder::currentFolderNames = {};

Folder::Folder(Folder *parent, const QString &name)
    : QObject(parent), d_active_window(0) {
  QObject::setObjectName(name);
  birthdate = QDateTime::currentDateTime().toString(Qt::LocalDate);
}

QList<Folder *> Folder::folders() const {
  QList<Folder *> folderList;
  foreach (QObject *folder, children())
    folderList.append(static_cast<Folder *>(folder));
  return folderList;
}

QStringList Folder::subfolders() {
  QStringList subFolderList = QStringList();
  QObjectList folderList = children();
  if (!folderList.isEmpty()) {
    QObject *folder;
    foreach (folder, folderList)
      subFolderList << static_cast<Folder *>(folder)->name();
  }
  return subFolderList;
}

QString Folder::path() {
  QString folderPath = "/" + QString(name()) + "/";
  Folder *parentFolder = static_cast<Folder *>(parent());
  while (parentFolder) {
    folderPath.prepend("/" + QString(parentFolder->name()));
    parentFolder = static_cast<Folder *>(parentFolder->parent());
  }
  return folderPath;
}

Folder *Folder::findSubfolder(const QString &subFolderName, bool caseSensitive,
                              bool partialMatch) {
  QObjectList folderList = children();

  if (folderList.isEmpty()) return nullptr;

  foreach (QObject *folder, folderList) {
    QString name = static_cast<Folder *>(folder)->name();
    if (partialMatch) {
      if (caseSensitive && name.startsWith(subFolderName, Qt::CaseSensitive))
        return static_cast<Folder *>(folder);
      else if (!caseSensitive &&
               name.startsWith(subFolderName, Qt::CaseInsensitive))
        return static_cast<Folder *>(folder);
    } else  // partialMatch == false
    {
      if (caseSensitive && name == subFolderName)
        return static_cast<Folder *>(folder);
      else if (!caseSensitive && (name.toLower() == subFolderName.toLower()))
        return static_cast<Folder *>(folder);
    }
  }
  // will never reach
  return nullptr;
}

MyWidget *Folder::findWindow(const QString &name, bool windowNames, bool labels,
                             bool caseSensitive, bool partialMatch) {
  if (lstWindows.isEmpty()) return nullptr;

  foreach (MyWidget *window, lstWindows) {
    if (windowNames) {
      QString windowName = window->name();
      if (partialMatch && windowName.startsWith(name, caseSensitive))
        return window;
      else if (caseSensitive && windowName == name)
        return window;
      else {
        QString text = windowName;
        if (windowName == text.toLower()) return window;
      }
    }

    if (labels) {
      QString label = window->windowLabel();
      if (partialMatch && label.startsWith(name, caseSensitive))
        return window;
      else if (caseSensitive && label == name)
        return window;
      else {
        QString text = name;
        if (label == text.toLower()) return window;
      }
    }
  }
  // will never reach
  return nullptr;
}

MyWidget *Folder::window(const QString &name, const char *cls, bool recursive) {
  foreach (MyWidget *window, lstWindows) {
    if (window->inherits(cls) && name == window->name()) return window;
  }

  if (!recursive) return nullptr;

  foreach (QObject *folder, children()) {
    MyWidget *window = (static_cast<Folder *>(folder))->window(name, cls, true);
    if (window) return window;
  }

  return nullptr;
}

Folder *Folder::rootFolder() {
  Folder *rootFolder = this;
  while (rootFolder->parent()) {
    rootFolder = static_cast<Folder *>(rootFolder->parent());
  }

  return rootFolder;
}

//--------------------------class WindowTreeWidgetItem-------------------------

WindowTableWidgetItem::WindowTableWidgetItem(QTreeWidget *parent, MyWidget *w)
    : QTreeWidgetItem(parent, FolderTreeWidget::Windows), myWindow(w) {
  setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

//--------------------------class FolderTreeWidgetItem-------------------------

FolderTreeWidgetItem::FolderTreeWidgetItem(QTreeWidget *parent, Folder *dir)
    : QTreeWidgetItem(parent, FolderTreeWidget::Folders), myFolder(dir) {
  setText(0, dir->name());
  setExpanded(true);
  setActive(true);
  setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled |
           Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
}

FolderTreeWidgetItem::FolderTreeWidgetItem(FolderTreeWidgetItem *parent,
                                           Folder *dir)
    : QTreeWidgetItem(parent, FolderTreeWidget::Folders), myFolder(dir) {
  setText(0, dir->name());
  setExpanded(true);
  setActive(true);
  setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled |
           Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
}

void FolderTreeWidgetItem::setActive(bool status) {
  if (!parent() && treeWidget()->columnCount() == 1 /*folder view*/) {
    setIcon(0, IconLoader::load("alpha-project", IconLoader::LightDark));
    return;
  }

  (status) ? setIcon(0, IconLoader::load("folder-open", IconLoader::General))
           : setIcon(0, IconLoader::load("folder-closed", IconLoader::General));
  setSelected(status);
}

bool FolderTreeWidgetItem::isChildOf(FolderTreeWidgetItem *src) {
  FolderTreeWidgetItem *parent =
      static_cast<FolderTreeWidgetItem *>(this->parent());
  while (parent) {
    if (parent == src) return true;
    parent = static_cast<FolderTreeWidgetItem *>(parent->parent());
  }
  return false;
}

//--------------------------class FolderTreeWidget-----------------------------

FolderTreeWidget::FolderTreeWidget(QWidget *parent, const QString name)
    : QTreeWidget(parent), tableWidgetDeligate(new TableWidgetDelegate()) {
  setAcceptDrops(true);
  viewport()->setAcceptDrops(true);
  setName(name);
  setItemDelegate(tableWidgetDeligate);
  connect(tableWidgetDeligate, SIGNAL(emptyFolderName()), this,
          SLOT(emptyFolderNameMsgBox()));
  connect(tableWidgetDeligate, SIGNAL(invalidFolderName(const QString &)), this,
          SLOT(invalidFolderNameMsgBox(const QString &)));
}

FolderTreeWidget::~FolderTreeWidget() { delete tableWidgetDeligate; }

void FolderTreeWidget::adjustColumns() {
  for (int i = 0; i < columnCount(); i++) resizeColumnToContents(i);
}

void FolderTreeWidget::emptyFolderNameMsgBox() {
  QMessageBox::critical(this, tr("Error"), tr("Please enter a valid name!"));
}

void FolderTreeWidget::invalidFolderNameMsgBox(const QString &name) {
  QMessageBox::critical(this, tr("Error"),
                        "\"" + name + "\" " + tr("name already exists!") +
                            "\n" + tr("Please choose another name!"));
}

void FolderTreeWidget::startDrag(Qt::DropActions) {
  QTreeWidgetItem *item = currentItem();
  if (!item) return;

  if (item == topLevelItem(0) && item->treeWidget()->rootIsDecorated())
    return;  // it's the project folder so we don't want a drag

  //  QList<QTreeWidgetItem *> lst;
  //  for (item = firstChild(); item; item = item->itemBelow()) {
  //    if (item->isSelected()) lst.append(item);
  //  }

  //  emit dragItems(lst);
}

void FolderTreeWidget::dropEvent(QDropEvent *event) {
  QTreeWidgetItem *dest = itemAt(mapToGlobal(event->pos()));
  if (dest && dest->type() == FolderTreeWidget::Folders) {
    emit dropItems(dest);
    event->accept();
  } else {
    event->ignore();
  }
}

void FolderTreeWidget::keyPressEvent(QKeyEvent *event) {
  QTreeWidgetItem *item = currentItem();
  if (!item) {
    QTreeWidget::keyPressEvent(event);
    return;
  }

  if (item->type() == FolderTreeWidget::Folders &&
      (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
    // note: we always sent 0 as the default colum
    emit itemDoubleClicked(item, 0);
    event->accept();
  } else if (event->key() == Qt::Key_F2) {
    if (item) emit renameItem(item);
    event->accept();
  } else if (event->key() == Qt::Key_A &&
             event->state() == Qt::ControlModifier) {
    selectAll();
    event->accept();
  } else if (event->key() == Qt::Key_F7) {
    emit addFolderItem();
    event->accept();
  } else if (event->key() == Qt::Key_F8) {
    emit deleteSelection();
    event->accept();
  } else
    QTreeWidget::keyPressEvent(event);
}

void FolderTreeWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QTreeWidgetItem *it = itemAt(event->pos());
  if (it)
    // note: we always sent 0 as the default colum
    emit itemDoubleClicked(it, 0);
}

//--------------------------class TableWidgetDeligate--------------------------

void TableWidgetDelegate::setModelData(QWidget *editor,
                                       QAbstractItemModel *model,
                                       const QModelIndex &index) const {
  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
  if (!lineEdit->isModified()) {
    return;
  }
  QString text = lineEdit->text().trimmed();

  // If text is empty, do nothing - preserve the old value.
  if (text.isEmpty()) {
    emit emptyFolderName();
    return;
  }

  // If text is used, do nothing - preserve the old value.
  foreach (QString folderName, Folder::currentFolderNames) {
    if (text == folderName) {
      emit invalidFolderName(folderName);
      return;
    }
  }

  // Everything seems OK... lets set data.
  QItemDelegate::setModelData(editor, model, index);
}
