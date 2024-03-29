/***************************************************************************
    File                 : MatrixModel.cpp
    Project              : AlphaPlot
    Description          : Model for the access to a Matrix
    --------------------------------------------------------------------
    Copyright            : (C) 2008-2009 Tilman Benkert (thzs*gmx.net)
                           (replace * with @ in the email addresses)

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

#include "matrix/MatrixModel.h"

#include <QBrush>
#include <QColor>
#include <QString>

#include "Matrix.h"
#include "matrix/future_Matrix.h"

MatrixModel::MatrixModel(future::Matrix *matrix)
    : QAbstractItemModel(0), d_matrix(matrix) {
  connect(d_matrix, &future::Matrix::columnsAboutToBeInserted, this,
          &MatrixModel::handleColumnsAboutToBeInserted);
  connect(d_matrix, &future::Matrix::columnsInserted, this,
          &MatrixModel::handleColumnsInserted);
  connect(d_matrix, &future::Matrix::columnsAboutToBeRemoved, this,
          &MatrixModel::handleColumnsAboutToBeRemoved);
  connect(d_matrix, &future::Matrix::columnsRemoved, this,
          &MatrixModel::handleColumnsRemoved);
  connect(d_matrix, &future::Matrix::rowsAboutToBeInserted, this,
          &MatrixModel::handleRowsAboutToBeInserted);
  connect(d_matrix, &future::Matrix::rowsInserted, this,
          &MatrixModel::handleRowsInserted);
  connect(d_matrix, &future::Matrix::rowsAboutToBeRemoved, this,
          &MatrixModel::handleRowsAboutToBeRemoved);
  connect(d_matrix, &future::Matrix::rowsRemoved, this,
          &MatrixModel::handleRowsRemoved);
  connect(d_matrix, &future::Matrix::dataChanged, this,
          &MatrixModel::handleDataChanged);
  connect(d_matrix, &future::Matrix::coordinatesChanged, this,
          &MatrixModel::handleCoordinatesChanged);
  connect(d_matrix, &future::Matrix::formatChanged, this,
          &MatrixModel::handleFormatChanged);
}

MatrixModel::~MatrixModel() {}

Qt::ItemFlags MatrixModel::flags(const QModelIndex &index) const {
  if (index.isValid())
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
  else
    return Qt::ItemIsEnabled;
}

QVariant MatrixModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();

  int row = index.row();
  int col = index.column();

  switch (role) {
    case Qt::ToolTipRole:
    case Qt::EditRole:
    case Qt::DisplayRole:
      return QVariant(d_matrix->text(row, col));
    case Qt::BackgroundRole:
      // yellow color to distinguish a matrix from a table
      return QVariant(QBrush(QColor(0xff, 0xff, 0x77)));
    case Qt::ForegroundRole:
      // The text color should always be black regardless of theme
      return QVariant(QBrush(QColor(Qt::black)));
  }

  return QVariant();
}

QVariant MatrixModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const {
  QString result;
  switch (orientation) {
    case Qt::Horizontal:
      switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
          if (!d_matrix->getShowOnlyValues())
            result += QString::number(section + 1) + QString(" (");
          double diff = d_matrix->xEnd() - d_matrix->xStart();
          double step = 0.0;
          if (d_matrix->columnCount() > 1)
            step = diff / double(d_matrix->columnCount() - 1);
          result += QLocale().toString(
              d_matrix->xStart() + double(section) * step,
              d_matrix->numericFormat(), d_matrix->displayedDigits());

          if (!d_matrix->getShowOnlyValues()) result += QString(")");
          return QVariant(result);
      }
    case Qt::Vertical:
      switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
          if (!d_matrix->getShowOnlyValues())
            result += QString::number(section + 1) + QString(" (");
          double diff = d_matrix->yEnd() - d_matrix->yStart();
          double step = 0.0;
          if (d_matrix->rowCount() > 1)
            step = diff / double(d_matrix->rowCount() - 1);
          // TODO: implement decent double == 0 check
          if (diff < 1e-10)
            result += QLocale().toString(d_matrix->yStart(),
                                         d_matrix->numericFormat(),
                                         d_matrix->displayedDigits());
          else
            result += QLocale().toString(
                d_matrix->yStart() + double(section) * step,
                d_matrix->numericFormat(), d_matrix->displayedDigits());

          if (!d_matrix->getShowOnlyValues()) result += QString(")");
          return QVariant(result);
      }
  }
  return QVariant();
}

int MatrixModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return d_matrix->rowCount();
}

int MatrixModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return d_matrix->columnCount();
}

bool MatrixModel::setData(const QModelIndex &index, const QVariant &value,
                          int role) {
  if (!index.isValid()) return false;

  int row = index.row();
  int column = index.column();

  if (role == Qt::EditRole) {
    static_cast< ::Matrix *>(d_matrix->view())
        ->setText(row, column, value.toString());
    return true;
  }
  return false;
}

QModelIndex MatrixModel::index(int row, int column,
                               const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return createIndex(row, column);
}

QModelIndex MatrixModel::parent(const QModelIndex &child) const {
  Q_UNUSED(child)
  return QModelIndex();
}

void MatrixModel::handleColumnsAboutToBeInserted(int before, int count) {
  beginInsertColumns(QModelIndex(), before, before + count - 1);
}

void MatrixModel::handleColumnsInserted(int first, int count) {
  Q_UNUSED(first)
  Q_UNUSED(count)
  endInsertColumns();
}

void MatrixModel::handleColumnsAboutToBeRemoved(int first, int count) {
  beginRemoveColumns(QModelIndex(), first, first + count - 1);
}

void MatrixModel::handleColumnsRemoved(int first, int count) {
  Q_UNUSED(first)
  Q_UNUSED(count)
  endRemoveColumns();
}

void MatrixModel::handleRowsAboutToBeInserted(int before, int count) {
  beginInsertRows(QModelIndex(), before, before + count - 1);
}

void MatrixModel::handleRowsInserted(int first, int count) {
  Q_UNUSED(first)
  Q_UNUSED(count)
  endInsertRows();
}

void MatrixModel::handleRowsAboutToBeRemoved(int first, int count) {
  beginRemoveRows(QModelIndex(), first, first + count - 1);
}

void MatrixModel::handleRowsRemoved(int first, int count) {
  Q_UNUSED(first)
  Q_UNUSED(count)
  endRemoveRows();
}

void MatrixModel::handleDataChanged(int top, int left, int bottom, int right) {
  emit dataChanged(index(top, left), index(bottom, right));
}

void MatrixModel::handleCoordinatesChanged() {
  emit headerDataChanged(Qt::Horizontal, 0, columnCount() - 1);
  emit headerDataChanged(Qt::Vertical, 0, rowCount() - 1);
}

void MatrixModel::handleFormatChanged() {
  handleCoordinatesChanged();
  handleDataChanged(0, 0, rowCount() - 1, columnCount() - 1);
}
