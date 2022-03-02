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

#include <QVariant>

class ObjectBrowserTreeItem {
 public:
  enum class ObjectType : int {
    None = 0,
    TableWindow = 1,
    MatrixWindow = 2,
    NoteWindow = 3,
    Plot2DWindow = 4,
    Plot3DWindow = 5,
    BaseWindow = 6,
    Plot2DCanvas = 7,
    Plot2DLayout = 8,
    Plot2DLegend = 9,
    Plot2DAxis = 10,
    Plot2DGrid = 11,
    Plot2DTextItem = 12,
    Plot2DLineItem = 13,
    Plot2DImageItem = 14,
    Plot2DLSGraph = 15,
    Plot2DChannelGraph = 16,
    Plot2DCurve = 17,
    Plot2DStatBox = 18,
    Plot2DVector = 19,
    Plot2DBarGraph = 20,
    Plot2DPieGraph = 21,
    Plot2DColorMap = 22,
    Plot2DErrorBar = 23,
    Plot3DCanvas = 24,
    Plot3DTheme = 25,
    Plot3DAxisValue = 26,
    Plot3DAxisCatagory = 27,
    Plot3DSurface = 28,
    Plot3DBar = 29,
    Plot3DScatter = 30,
    Plot3DSurfaceDataBlock = 31,
    Plot3DBarDataBlock = 32,
    Plot3DScatterDataBlock = 33,
  };
  ObjectBrowserTreeItem(QVariant data, const ObjectType &type,
                        ObjectBrowserTreeItem *parentItem = nullptr);
  virtual ~ObjectBrowserTreeItem();

  virtual QString getItemName() = 0;
  virtual QIcon getItemIcon() = 0;
  virtual QString getItemTooltip() = 0;

  void addChildAppropriately();
  bool removeChild(ObjectBrowserTreeItem *item);
  ObjectBrowserTreeItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(const Qt::ItemDataRole &role);
  ObjectType dataType() const;
  int row() const;
  ObjectBrowserTreeItem *parentItem();

 private:
  QVariant itemData_;
  ObjectType itemDataType_;
  ObjectBrowserTreeItem *parentItem_;
  QVector<ObjectBrowserTreeItem *> childItems_;
};

#endif  // OBJECTBROWSERTREEITEM_H
