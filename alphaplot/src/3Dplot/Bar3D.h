#ifndef BAR3D_H
#define BAR3D_H

#include <QtDataVisualization/Q3DBars>
#include <QtDataVisualization/QItemModelBarDataProxy>
#include <memory>

#include "3Dplot/Graph3DCommon.h"

class Custom3DInteractions;
class Matrix;
using namespace QtDataVisualization;

class Bar3D : public QObject {
  Q_OBJECT
 public:
  Bar3D(Q3DBars *bar, const Graph3DCommon::Plot3DType &type);
  ~Bar3D();

  void setmatrixdatamodel(Matrix *matrix);
  Matrix *getMatrix();

 private:
  void setGradient();

 private:
  Q3DBars *graph_;
  Matrix *matrix_;
  Graph3DCommon::Plot3DType plotType_;
  Custom3DInteractions *custominter_;
  std::unique_ptr<QBarDataProxy> functionDataProxy_;
  std::unique_ptr<QBar3DSeries> dataSeries_;
  std::unique_ptr<QItemModelBarDataProxy> modelDataProxy_;
};

#endif  // BAR3D_H
