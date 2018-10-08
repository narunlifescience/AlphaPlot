#ifndef LINESCATTERCOMMON_H
#define LINESCATTERCOMMON_H

class Column;
class Table;

namespace LSCommon {
enum class LineStyleType : int {
  StepLeft = 0,
  StepRight = 1,
  StepCenter = 2,
  Impulse = 3,
};

enum class ScatterStyle : int {
  None = 0,
  Dot = 1,
  Cross = 2,
  Plus = 3,
  Circle = 4,
  Disc = 5,
  Square = 6,
  Diamond = 7,
  Star = 8,
  Triangle = 9,
  TriangleInverted = 10,
  CrossSquare = 11,
  PlusSquare = 12,
  CrossCircle = 13,
  PlusCircle = 14,
  Peace = 15,
};

enum class PlotType { Associated, Function };
}  // namespace LSCommon

namespace PlotData {
struct AssociatedData {
  Column *xcol;
  Column *ycol;
  Table *table;
};
}  // namespace PlotData

#endif  // LINESCATTERCOMMON_H
