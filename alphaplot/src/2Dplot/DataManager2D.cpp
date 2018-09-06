#include "DataManager2D.h"
#include "../future/core/column/Column.h"
#include "Table.h"

DataManager2D::DataManager2D(QObject *parent) : QObject(parent) {}

QPair<QList<bool>, QList<bool> > DataManager2D::datatemplateupdate(
    Table *table, Column *xData, Column *yData, int from, int to) {
  double xdata = 0, ydata = 0;
  qDebug() << "data manager2d";

  // strip unused end rows
  int end_row = to;
  if (end_row >= xData->rowCount()) end_row = xData->rowCount() - 1;
  if (end_row >= yData->rowCount()) end_row = yData->rowCount() - 1;

  // determine rows for which all columns have valid content
  QList<int> valid_rows;
  for (int row = from; row <= end_row; row++) {
    bool all_valid = true;

    if (xData->isInvalid(row) || yData->isInvalid(row)) {
      all_valid = false;
    }

    if (all_valid) valid_rows.push_back(row);
  }
}
