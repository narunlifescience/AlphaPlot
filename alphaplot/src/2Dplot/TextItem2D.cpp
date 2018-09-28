#include "TextItem2D.h"
#include "QColor"
#include "QPen"

TextItem2D::TextItem2D(AxisRect2D *axisrect, Plot2D *plot)
    : QCPItemText(plot), axisrect_(axisrect) {
  setClipAxisRect(axisrect_);
  setAntialiased(false);
}

TextItem2D::~TextItem2D() {}
