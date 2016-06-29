#ifndef LAYOUTGRID2D_H
#define LAYOUTGRID2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"

class LayoutGrid2D : public QCPLayoutGrid
{
public:
  LayoutGrid2D(QWidget *parent = 0);
  ~LayoutGrid2D();
};

#endif // LAYOUTGRID2D_H
