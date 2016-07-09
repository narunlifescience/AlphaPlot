#include "Legend2D.h"
#include "LineScatter2D.h"

Legend2D::Legend2D() : QCPLegend() {}

Legend2D::~Legend2D() {}

void Legend2D::mousePressEvent(QMouseEvent *) { emit legendClicked(); }

LegendItem2D::LegendItem2D(Legend2D *legend, QCPAbstractPlottable *plottable)
    : QCPPlottableLegendItem(legend, plottable) {}

LegendItem2D::~LegendItem2D() {}

void LegendItem2D::mousePressEvent(QMouseEvent *) { emit legendItemClicked(); }
