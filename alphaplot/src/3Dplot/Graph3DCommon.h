#ifndef GRAPH3DCOMMON_H
#define GRAPH3DCOMMON_H

#include <QString>

namespace Graph3DCommon {
enum class Plot3DType : int {
  Surface = 0,
  Bar = 1,
  Scatter = 2,
};

struct Function3DData {
  QString function;
  double xl;
  double xu;
  double yl;
  double yu;
  double zl;
  double zu;
  int xpoints;
  int ypoints;
};
}  // namespace Graph3DCommon
#endif  // GRAPH3DCOMMON_H
