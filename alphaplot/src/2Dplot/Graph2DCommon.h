#ifndef GRAPH2DCOMMON_H
#define GRAPH2DCOMMON_H

#include <QStringList>
class Column;
class Table;

namespace Graph2DCommon {
enum class AddLayoutElement : int { Left = 0, Top = 1, Right = 2, Bottom = 3 };

enum class PieStyle : int {
  Pie = 0,
  HalfPie = 1,
};

enum class LineStyleType : int {
  StepLeft = 0,
  StepRight = 1,
  StepCenter = 2,
  Impulse = 3,
  Line = 4,
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

enum class Picker : int {
  None = 0,
  DataPoint = 1,
  DataGraph = 2,
  DataMove = 3,
  DataRemove = 4,
  DragRange = 5,
  ZoomRange = 6,
  DataRange = 7,
};

enum class PlotType { Associated, Function };
}  // namespace Graph2DCommon

namespace PlotData {
struct FunctionData {
  int type;
  QStringList functions;
  QString parameter;
  double from;
  double to;
  int points;
  FunctionData()
      : type(0),
        functions(QStringList()),
        parameter(QString()),
        from(0.0),
        to(0.0),
        points(0) {}
};

struct AssociatedData {
  Table *table;
  Column *xcol;
  Column *ycol;
  int from;
  int to;
  AssociatedData() : table(nullptr), xcol(nullptr), ycol(nullptr) {}
};

struct HistData {
  Table *table;
  Column *col;
  int from;
  int to;
  bool autobin;
  double binsize;
  double begin;
  double end;
  HistData()
      : table(nullptr),
        col(nullptr),
        autobin(true),
        binsize(0),
        begin(0),
        end(0) {}
};
}  // namespace PlotData

#endif  // GRAPH2DCOMMON_H
