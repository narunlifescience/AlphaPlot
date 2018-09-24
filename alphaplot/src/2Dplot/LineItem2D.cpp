#include "LineItem2D.h"

LineItem2D::LineItem2D(AxisRect2D *axisrect, Plot2D *plot)
    : QCPItemLine(plot), axisrect_(axisrect) {
  setClipAxisRect(axisrect_);
  setClipToAxisRect(true);
}
