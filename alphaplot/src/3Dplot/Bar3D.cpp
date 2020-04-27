#include "Bar3D.h"

#include <qmath.h>

#include "Custom3DInteractions.h"
#include "DataManager3D.h"
#include "Matrix.h"
#include "Table.h"
#include "future/core/column/Column.h"

Bar3D::Bar3D(Q3DBars *bar)
    : graph_(bar), custominter_(new Custom3DInteractions), data_(nullptr) {
  graph_->activeTheme()->setType(Q3DTheme::ThemeDigia);
  graph_->setActiveInputHandler(custominter_);
  graph_->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
  graph_->scene()->activeCamera()->setCameraPreset(
      Q3DCamera::CameraPresetFront);

  graph_->setColumnAxis(new QCategory3DAxis);
  graph_->setRowAxis(new QCategory3DAxis);
  graph_->setValueAxis(new QValue3DAxis);
}

Bar3D::~Bar3D() {}

void Bar3D::settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                         QList<Column *> zcolumns) {
  if (data_ != nullptr) return;
  data_ = new DataBlockBar3D(table, xcolumn, ycolumn, zcolumns);
  graph_->addSeries(data_->getdataseries());
  setGradient();
}

void Bar3D::settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                         Column *zcolumn) {
  if (data_ != nullptr) return;
  data_ = new DataBlockBar3D(table, xcolumn, ycolumn, zcolumn);
  graph_->addSeries(data_->getdataseries());
  setGradient();
}

void Bar3D::setmatrixdatamodel(Matrix *matrix) {
  if (data_ != nullptr) return;
  data_ = new DataBlockBar3D(matrix);
  graph_->addSeries(data_->getdataseries());
  setGradient();
}

Q3DBars *Bar3D::getGraph() const { return graph_; }

DataBlockBar3D *Bar3D::getData() const { return data_; }

void Bar3D::setGradient() {
  QLinearGradient gr;
  gr.setColorAt(0.2, Qt::black);
  gr.setColorAt(0.33, Qt::blue);
  gr.setColorAt(0.67, Qt::red);
  gr.setColorAt(1.0, Qt::yellow);

  graph_->seriesList().at(0)->setBaseGradient(gr);
  graph_->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
  graph_->setSelectionMode(QAbstract3DGraph::SelectionItemAndRow |
                           QAbstract3DGraph::SelectionSlice);
}
