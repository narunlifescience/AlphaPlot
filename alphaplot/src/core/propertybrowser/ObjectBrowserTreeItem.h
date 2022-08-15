/***************************************************************************
    File                 : ObjectBrowserTreeItemModel.h
    Project              : AlphaPlot
    Description          : Object item base class
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

#ifndef OBJECTBROWSERTREEITEM_H
#define OBJECTBROWSERTREEITEM_H

#include <QDebug>
#include <QIcon>
#include <QVariant>

class ObjectBrowserTreeItem {
 public:
  enum class ObjectType : int {
    None = 0,
    BaseWindow = 1,
    TableWindow = 2,
    TableDimension = 3,
    MatrixWindow = 4,
    MatrixDimension = 5,
    NoteWindow = 6,
    Plot2DWindow = 7,
    Plot2DCanvas = 8,
    Plot2DLayout = 9,
    Plot2DLegend = 10,
    Plot2DAxis = 11,
    Plot2DGrid = 12,
    Plot2DTextItem = 13,
    Plot2DLineItem = 14,
    Plot2DImageItem = 15,
    Plot2DLSGraph = 16,
    Plot2DChannelGraph = 17,
    Plot2DCurve = 18,
    Plot2DStatBox = 19,
    Plot2DVector = 20,
    Plot2DBarGraph = 21,
    Plot2DPieGraph = 22,
    Plot2DColorMap = 23,
    Plot2DErrorBar = 24,
    Plot3DWindow = 25,
    Plot3DCanvas = 26,
    Plot3DTheme = 27,
    Plot3DAxisValueX = 28,
    Plot3DAxisValueY = 29,
    Plot3DAxisValueZ = 30,
    Plot3DAxisCatagoryX = 31,
    Plot3DAxisCatagoryY = 32,
    Plot3DSurface = 33,
    Plot3DBar = 34,
    Plot3DScatter = 35,
    Plot3DSurfaceDataBlock = 36,
    Plot3DBarDataBlock = 37,
    Plot3DScatterDataBlock = 38,
  };

  struct RoleData {
    QString name;
    QString tooltip;
    QIcon icon;
    RoleData() : name(QString()), tooltip(QString()), icon(QIcon()) {}
  };

  static ObjectBrowserTreeItem *create(QVariant data, const ObjectType &type,
                                       ObjectBrowserTreeItem *parentItem);
  ~ObjectBrowserTreeItem();

  template <class T>
  T *getObjectTreeItem(bool *value) const {
    T *object = itemData_.value<T *>();
    if (!object) {
      *value = false;
      qDebug() << "Null object variant: " << typeid(object).name();
    } else
      *value = true;
    return object;
  }

  bool removeChild(ObjectBrowserTreeItem *item);
  ObjectBrowserTreeItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(const Qt::ItemDataRole &role);
  ObjectType dataType() const;
  int row() const;
  ObjectBrowserTreeItem *parentItem();

 private:
  ObjectBrowserTreeItem(QVariant data, const ObjectType &type,
                        ObjectBrowserTreeItem *parentItem = nullptr);
  RoleData value() const;

 private:
  QVariant itemData_;
  ObjectType itemDataType_;
  ObjectBrowserTreeItem *parentItem_;
  QVector<ObjectBrowserTreeItem *> childItems_;
};

#endif  // OBJECTBROWSERTREEITEM_H
