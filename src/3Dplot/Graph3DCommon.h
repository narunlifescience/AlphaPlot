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

enum class Gradient : int {
  Grayscale = 0,
  Hot = 1,
  Cold = 2,
  Night = 3,
  Candy = 4,
  Geography = 5,
  Ion = 6,
  Thermal = 7,
  Polar = 8,
  Spectrum = 9,
  Jet = 10,
  Hues = 11,
  BBRY = 12,
  GYRD = 13,
};
}  // namespace Graph3DCommon
#endif  // GRAPH3DCOMMON_H
