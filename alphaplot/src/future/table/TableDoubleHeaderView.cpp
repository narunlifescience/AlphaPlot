/***************************************************************************
    File                 : TableDoubleHeaderView.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Benkert,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Horizontal header for TableView displaying comments
 in a second header

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

#include "TableDoubleHeaderView.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QLayout>
#include <QPainter>

#include "../../globals.h"
#include "TableCommentsHeaderModel.h"
#include "core/AppearanceManager.h"

TableCommentsHeaderView::TableCommentsHeaderView(QWidget *parent)
    : QHeaderView(Qt::Horizontal, parent) {}

TableCommentsHeaderView::~TableCommentsHeaderView() { delete model(); }

void TableCommentsHeaderView::setModel(QAbstractItemModel *model) {
  Q_ASSERT(model->inherits("TableModel"));
  QAbstractItemModel *old_model = QHeaderView::model();
  TableCommentsHeaderModel *new_model =
      new TableCommentsHeaderModel(static_cast<TableModel *>(model));
  QHeaderView::setModel(new_model);
  QObject::disconnect(
      new_model, SIGNAL(columnsInserted(QModelIndex, int, int)), this,
      SLOT(sectionsInserted(QModelIndex, int, int)));  // We have to make sure
                                                       // sectionsInserted is
                                                       // called in the right
                                                       // order
  delete old_model;
}

QSize TableCommentsHeaderView::sizeHint() const {
  QSize comment_size;
  comment_size = QHeaderView::sizeHint();
  comment_size.setHeight(AppearanceManager::commentHeaderHeight);
  return comment_size;
}

TableDoubleHeaderView::TableDoubleHeaderView(QWidget *parent)
    : QHeaderView(Qt::Horizontal, parent) {
  setDefaultAlignment(Qt::AlignLeft | Qt::AlignTop);
  d_slave = new TableCommentsHeaderView();
  d_slave->setDefaultAlignment(Qt::AlignLeft | Qt::AlignTop);
  d_show_comments = true;
}

TableDoubleHeaderView::~TableDoubleHeaderView() { delete d_slave; }

QSize TableDoubleHeaderView::sizeHint() const {
  QSize master_size, slave_size;
  master_size = QHeaderView::sizeHint();
  master_size.setHeight(AppearanceManager::headerHeight);
  slave_size = d_slave->sizeHint();
  if (d_show_comments) {
    master_size.setHeight(master_size.height() + slave_size.height());
  }

  return master_size;
}

void TableDoubleHeaderView::setModel(QAbstractItemModel *model) {
  Q_ASSERT(model->inherits("TableModel"));
  d_slave->setModel(model);
  QHeaderView::setModel(model);
  connect(model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)), this,
          SLOT(headerDataChanged(Qt::Orientation, int, int)));
}

void TableDoubleHeaderView::paintSection(QPainter *painter, const QRect &rect,
                                         int logicalIndex) const {
  QRect master_rect = rect;
  painter->save();
  if (d_show_comments)
    master_rect = rect.adjusted(0, 0, 0, -d_slave->sizeHint().height());
  QHeaderView::paintSection(painter, master_rect, logicalIndex);
  if (d_show_comments && rect.height() > master_rect.height()) {
    QRect slave_rect = rect.adjusted(0, master_rect.height(), 0, 0);
    d_slave->paintSection(painter, slave_rect, logicalIndex);
  }
  painter->restore();

  // Color code column decorations
  painter->setPen(QPen(
      static_cast<TableModel *>(model())->headerDataColorCode(logicalIndex),
      AppearanceManager::colorCodeThickness, Qt::SolidLine, Qt::RoundCap));
  painter->drawLine(
      rect.bottomLeft().x() + AppearanceManager::colorCodeXPadding,
      rect.bottomLeft().y() - AppearanceManager::colorCodeYPadding,
      rect.bottomRight().x() - AppearanceManager::colorCodeXPadding,
      rect.bottomRight().y() - AppearanceManager::colorCodeYPadding);
}

bool TableDoubleHeaderView::areCommentsShown() const { return d_show_comments; }

// better implimentation required.. so comment out for now
/*void TableDoubleHeaderView::mouseMoveEvent(QMouseEvent *event) {
  if (logicalIndexAt(event->pos().x()) >= 0 &&
      logicalIndexAt(event->pos().x()) < count()) {
    setCursor(Qt::UpArrowCursor);
  } else {
    setCursor(Qt::ArrowCursor);
  }
}*/

void TableDoubleHeaderView::showComments(bool on) {
  d_show_comments = on;
  refresh();
}

void TableDoubleHeaderView::refresh() {
  // adjust geometry and repaint header (still looking for a more elegant
  // solution)
  d_slave->setStretchLastSection(
      true);  // ugly hack (flaw in Qt? Does anyone know a better way?)
  d_slave->updateGeometry();
  d_slave->setStretchLastSection(false);  // ugly hack part 2
  setStretchLastSection(
      true);  // ugly hack (flaw in Qt? Does anyone know a better way?)
  updateGeometry();
  setStretchLastSection(false);  // ugly hack part 2
  update();
}

void TableDoubleHeaderView::headerDataChanged(Qt::Orientation orientation,
                                              int logicalFirst,
                                              int logicalLast) {
  Q_UNUSED(logicalFirst);
  Q_UNUSED(logicalLast);
  if (orientation == Qt::Horizontal) refresh();
}

void TableDoubleHeaderView::sectionsInserted(const QModelIndex &parent,
                                             int logicalFirst,
                                             int logicalLast) {
  d_slave->sectionsInserted(parent, logicalFirst, logicalLast);
  QHeaderView::sectionsInserted(parent, logicalFirst, logicalLast);
  Q_ASSERT(d_slave->count() == QHeaderView::count());
}
