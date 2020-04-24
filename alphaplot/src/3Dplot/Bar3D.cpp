#include "Bar3D.h"

#include <qmath.h>

#include "Custom3DInteractions.h"
#include "Matrix.h"

Bar3D::Bar3D(Q3DBars *bar, const Graph3DCommon::Plot3DType &type)
    : graph_(bar),
      matrix_(nullptr),
      plotType_(type),
      custominter_(new Custom3DInteractions),
      functionDataProxy_(new QBarDataProxy()),
      dataSeries_(new QBar3DSeries),
      modelDataProxy_(new QItemModelBarDataProxy) {
  graph_->activeTheme()->setType(Q3DTheme::ThemeDigia);
  graph_->setActiveInputHandler(custominter_);
  graph_->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
  graph_->scene()->activeCamera()->setCameraPreset(
      Q3DCamera::CameraPresetFront);

  graph_->setColumnAxis(new QCategory3DAxis);
  graph_->setRowAxis(new QCategory3DAxis);
  graph_->setValueAxis(new QValue3DAxis);
  graph_->columnAxis()->setTitle("X");
  graph_->rowAxis()->setTitle("Y");
  graph_->valueAxis()->setTitle("Z");
}

Bar3D::~Bar3D() {}

void Bar3D::setmatrixdatamodel(Matrix *matrix) {
  matrix_ = matrix;
  modelDataProxy_.get()->setItemModel(matrix->getmodel());
  modelDataProxy_.get()->setUseModelCategories(true);
  dataSeries_.get()->setDataProxy(modelDataProxy_.get());
  dataSeries_->setMesh(QAbstract3DSeries::Mesh::MeshBar);
  graph_->addSeries(dataSeries_.get());
  setGradient();
  graph_->setBarThickness(1.0f);
  graph_->setBarSpacing(QSizeF(0.2, 0.2));

  // Set selection mode to slice row
  graph_->setSelectionMode(QAbstract3DGraph::SelectionItemAndRow |
                           QAbstract3DGraph::SelectionSlice);
}

Matrix *Bar3D::getMatrix() { return matrix_; }

void Bar3D::setGradient() {
  QLinearGradient gr;
  gr.setColorAt(0.2, Qt::black);
  gr.setColorAt(0.33, Qt::blue);
  gr.setColorAt(0.67, Qt::red);
  gr.setColorAt(1.0, Qt::yellow);

  dataSeries_->setBaseGradient(gr);
  dataSeries_->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}
