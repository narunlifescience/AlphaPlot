#ifndef GRIDPAIR2D_H
#define GRIDPAIR2D_H

#include <QObject>
#include <QPair>

class Grid2D;
class Axis2D;

class GridPair2D : public QObject {
  Q_OBJECT
 public:
  GridPair2D(QObject *parent, QPair<Grid2D *, Axis2D *> xgrid,
             QPair<Grid2D *, Axis2D *> ygrid);
  ~GridPair2D();

  QString getItemName();
  QIcon getItemIcon();
  QString getItemTooltip();

  QPair<Grid2D *, Axis2D *> getXgridPair() { return xgrid_; }
  QPair<Grid2D *, Axis2D *> getYgridPair() { return ygrid_; }
  Axis2D *getXgridAxis() { return xgrid_.second; }
  Axis2D *getYgridAxis() { return ygrid_.second; }
  Grid2D *getXgrid() { return xgrid_.first; }
  Grid2D *getYgrid() { return ygrid_.first; }

  void setXgridAxis(Axis2D *axis) { xgrid_.second = axis; }
  void setYgridAxis(Axis2D *axis) { ygrid_.second = axis; }
  void setXgrid(Grid2D *grid) { xgrid_.first = grid; }
  void setYgrid(Grid2D *grid) { ygrid_.first = grid; }

 private:
  QPair<Grid2D *, Axis2D *> xgrid_;
  QPair<Grid2D *, Axis2D *> ygrid_;
};

Q_DECLARE_METATYPE(GridPair2D *);
#endif  // GRIDPAIR2D_H
