#ifndef TEXTITEM2D_H
#define TEXTITEM2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "AxisRect2D.h"
#include "Plot2D.h"

class TextItem2D : public QCPItemText
{
public:
  TextItem2D(AxisRect2D *axisrect, Plot2D *plot);
  ~TextItem2D();

private:
  AxisRect2D *axisrect_;
};

#endif // TEXTITEM2D_H
