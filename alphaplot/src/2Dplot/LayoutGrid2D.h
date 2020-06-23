#ifndef LAYOUTGRID2D_H
#define LAYOUTGRID2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"

class LayoutGrid2D : public QCPLayoutGrid {
  Q_OBJECT
 public:
  enum class AddElement : int { Left = 0, Top = 1, Right = 2, Bottom = 3 };

  LayoutGrid2D();
  ~LayoutGrid2D();
};

#endif  // LAYOUTGRID2D_H
