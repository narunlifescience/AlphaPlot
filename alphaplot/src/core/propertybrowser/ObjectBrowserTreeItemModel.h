/***************************************************************************
    File                 : ObjectBrowserTreeItemModel.h
    Project              : AlphaPlot
    Description          : Object item model class
    --------------------------------------------------------------------
    Copyright            : (C) 2022 Arun Narayanankutty
                               <n.arun.lifescience@gmail.com>

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

#ifndef OBJECTBROWSERTREEITEMMODEL_H
#define OBJECTBROWSERTREEITEMMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class ObjectBrowserTreeItem;
class QItemSelectionModel;
class Table;
class Matrix;
class Note;
class Layout2D;
class Layout3D;

class ObjectBrowserTreeItemModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  ObjectBrowserTreeItemModel(QObject *parent);
  ~ObjectBrowserTreeItemModel();

  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  void resetModel() { emit endResetModel(); }
  void updateProperty(ObjectBrowserTreeItem *item);
  void updateChildren(ObjectBrowserTreeItem *item, int column,
                      const QModelIndex &parent);
  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role = Qt::EditRole) override;
  ObjectBrowserTreeItem *rootItem() { return rootItem_; }

  void buildUpNone();
  void buildUpTable(Table *table);
  void buildUpMatrix(Matrix *matrix);
  void buildUpNote(Note *note);
  void buildUpGraph2D(Layout2D *layout);
  void buildUpGraph3D(Layout3D *layout);

 signals:
  void modelResetComplete();

 private:
  ObjectBrowserTreeItem *rootItem_;
};

#endif  // OBJECTTREEITEMMODEL_H
