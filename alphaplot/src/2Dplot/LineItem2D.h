#ifndef LINEITEM2D_H
#define LINEITEM2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "AxisRect2D.h"
#include "Plot2D.h"

class LineItem2D : QCPItemLine
{
public:
  LineItem2D(AxisRect2D *axisrect, Plot2D *plot);

private:
  AxisRect2D *axisrect_;
};

#endif // LINEITEM2D_H
