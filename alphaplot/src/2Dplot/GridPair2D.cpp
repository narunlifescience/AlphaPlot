#include "GridPair2D.h"

#include "Axis2D.h"
#include "AxisRect2D.h"
#include "Grid2D.h"
#include "core/IconLoader.h"

GridPair2D::GridPair2D(QObject *parent, QPair<Grid2D *, Axis2D *> xgrid,
                       QPair<Grid2D *, Axis2D *> ygrid)
    : QObject(parent), xgrid_(xgrid), ygrid_(ygrid) {}

GridPair2D::~GridPair2D() {}

QString GridPair2D::getItemName() { return tr("Grid"); }

QIcon GridPair2D::getItemIcon() {
  return IconLoader::load("graph3d-cross", IconLoader::LightDark);
}

QString GridPair2D::getItemTooltip() { return getItemName(); }
