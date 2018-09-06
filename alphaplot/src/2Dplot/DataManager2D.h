#ifndef DATAMANAGER2D_H
#define DATAMANAGER2D_H

#include <QObject>
#include <QPair>

class Table;
class Column;

class DataManager2D : public QObject {
  Q_OBJECT
 public:
  explicit DataManager2D(QObject *parent = nullptr);
  QPair<QList<bool>, QList<bool>> datatemplateupdate(Table *table, Column *xData,
                                                     Column *yData, int from,
                                                     int to);

 signals:

 public slots:
};

#endif  // DATAMANAGER2D_H
