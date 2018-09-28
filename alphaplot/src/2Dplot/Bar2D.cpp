#include "Bar2D.h"
#include "../future/core/column/Column.h"
#include "DataManager2D.h"
#include "Table.h"

Bar2D::Bar2D(Table *table, Column *xcol, Column *ycol, int from, int to,
             Axis2D *xAxis, Axis2D *yAxis)
    : QCPBars(xAxis, yAxis),
      barwidth_(1),
      xaxis_(xAxis),
      yaxis_(yAxis),
      bardata_(new DataBlockBar(table, xcol, ycol, from, to)) {
  setData(bardata_->data());
}

Bar2D::~Bar2D() { delete bardata_; }

Axis2D *Bar2D::getxaxis_barplot() const { return xaxis_; }

Axis2D *Bar2D::getyaxis_barplot() const { return yaxis_; }

DataBlockBar *Bar2D::getdatablock_barplot() const { return bardata_; }

void Bar2D::setBarData(Table *table, Column *xcol, Column *ycol, int from,
                       int to) {
  bardata_->regenerateDataBlock(table, xcol, ycol, from, to);
  setData(bardata_->data());
}

void Bar2D::setBarWidth(double barwidth) {
  setWidth(barwidth / static_cast<double>(data().data()->size()));
}

double Bar2D::getBarWidth() { return barwidth_; }
