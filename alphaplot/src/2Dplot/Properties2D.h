#ifndef PROPERTY_H
#define PROPERTY_H

#include <QColor>
#include <QIcon>
#include <QPair>
#include <QString>
#include <QObject>

namespace Property2D {

// Type of data
enum DataType {
  Boolean,
  Intiger,
  UnsignedIntiger,
  Double,
  Color,
  CustomEnum,
  StringList,
  PenStyle,
};

// types of widget
enum WidgetType {
  Combobox,
  LineEdit,
  CheckBox,
  NormalSpinBox,
  DoubleSpinBox,
  Label,
  PushButton,
  FontWidget,
  ColorWidget,
};

// editable or not
enum Editable { ReadAndWrite, ReadOnly, Immutable };

enum Group {
  GridGroup,
  AxisGroup,
};

enum SubGroup {
  MajorGridSubGroup,
  MinorGridSubGroup,
  ZeroLineSubGroup,
};

// Basic property unit structure
template <class T>
struct Data {
  QString name;
  T value;
  T valueLowerBound;
  T valueUpperBound;
  Editable editable;
  DataType dataType;
  WidgetType widgetType;
  SubGroup subGroup;
};

// Basic Property Unit Grid (excluding Zero Line)
struct GridUnit {
  Data<bool> visible;
  Data<QColor> strokeColor;
  Data<Qt::PenStyle> style;
  Data<int> strokeThickness;
  Data<QStringList> axes;
};

// Basic Property Unit Zeroline
struct ZeroLine {
  Data<bool> visible;
  Data<QColor> strokeColor;
  Data<Qt::PenStyle> style;
  Data<int> strokeThickness;
};

struct Grid {
  Data<Group> group;
  GridUnit majorGrid;
  GridUnit minorGrid;
  ZeroLine zeroLine;

  Grid () {
    // Group
    group.name = QObject::tr("Grid");
    group.value = GridGroup;
    group.editable = Immutable;
    group.dataType = CustomEnum;
    group.widgetType = Label;
    // Grid visibility
    majorGrid.visible.name = QObject::tr("Visible");
    majorGrid.visible.value = true;
    majorGrid.visible.editable = ReadAndWrite;
    majorGrid.visible.dataType = Boolean;
    majorGrid.visible.widgetType = Combobox;
    majorGrid.visible.subGroup = MajorGridSubGroup;
    minorGrid.visible.name = QObject::tr("Visible");
    minorGrid.visible.value = false;
    minorGrid.visible.editable = ReadAndWrite;
    minorGrid.visible.dataType = Boolean;
    minorGrid.visible.widgetType = Combobox;
    minorGrid.visible.subGroup = MinorGridSubGroup;
    // Grid Stroke Color
    majorGrid.strokeColor.name = QObject::tr("Line Color");
    majorGrid.strokeColor.value = Qt::darkGray;
    majorGrid.strokeColor.editable = ReadAndWrite;
    majorGrid.strokeColor.dataType = Color;
    majorGrid.strokeColor.widgetType =ColorWidget;
    majorGrid.strokeColor.subGroup = MajorGridSubGroup;
    minorGrid.strokeColor.name = QObject::tr("Line Color");
    minorGrid.strokeColor.value = Qt::lightGray;
    minorGrid.strokeColor.editable = ReadAndWrite;
    minorGrid.strokeColor.dataType = Color;
    minorGrid.strokeColor.widgetType =ColorWidget;
    minorGrid.strokeColor.subGroup = MinorGridSubGroup;
    // Grid Stroke Style
    majorGrid.style.name = QObject::tr("Line Style");
    majorGrid.style.editable = ReadAndWrite;
    majorGrid.style.dataType = PenStyle;
    majorGrid.style.widgetType = Combobox;
    majorGrid.style.subGroup = MajorGridSubGroup;
    minorGrid.style.name = QObject::tr("Line Style");
    minorGrid.style.editable = ReadAndWrite;
    minorGrid.style.dataType = PenStyle;
    minorGrid.style.widgetType = Combobox;
    minorGrid.style.subGroup = MinorGridSubGroup;
    // Grid Stroke Thickness
    majorGrid.strokeThickness.name = QObject::tr("Line Thickness");
    majorGrid.strokeThickness.value = 0;
    majorGrid.strokeThickness.valueLowerBound = 0;
    majorGrid.strokeThickness.valueUpperBound = 1000;
    majorGrid.strokeThickness.editable = ReadAndWrite;
    majorGrid.strokeThickness.dataType = Intiger;
    majorGrid.strokeThickness.widgetType = NormalSpinBox;
    majorGrid.strokeThickness.subGroup = MajorGridSubGroup;
    minorGrid.strokeThickness.name = QObject::tr("Line Thickness");
    minorGrid.strokeThickness.value = 0;
    minorGrid.strokeThickness.valueLowerBound = 0;
    minorGrid.strokeThickness.valueUpperBound = 1000;
    minorGrid.strokeThickness.editable = ReadAndWrite;
    minorGrid.strokeThickness.dataType = Intiger;
    minorGrid.strokeThickness.widgetType = NormalSpinBox;
    minorGrid.strokeThickness.subGroup = MinorGridSubGroup;
    // Grid Axes
    majorGrid.axes.name = QObject::tr("Associated Axis");
    majorGrid.axes.editable = ReadAndWrite;
    majorGrid.axes.dataType = StringList;
    majorGrid.axes.widgetType = Combobox;
    majorGrid.axes.subGroup = MajorGridSubGroup;
    minorGrid.axes.name = QObject::tr("Associated Axis");
    minorGrid.axes.editable = ReadAndWrite;
    minorGrid.axes.dataType = StringList;
    minorGrid.axes.widgetType = Combobox;
    minorGrid.axes.subGroup = MinorGridSubGroup;
    // Zeroline
    zeroLine.visible.name = QObject::tr("Zero Line Visible");
    zeroLine.visible.value = true;
    zeroLine.visible.editable = ReadAndWrite;
    zeroLine.visible.dataType = Boolean;
    zeroLine.visible.widgetType = Combobox;
    zeroLine.visible.subGroup = ZeroLineSubGroup;
    // Zeroline Stroke Color
    zeroLine.strokeColor.name = QObject::tr("Zero Line Color");
    zeroLine.strokeColor.value = Qt::darkGray;
    zeroLine.strokeColor.editable = ReadAndWrite;
    zeroLine.strokeColor.dataType = Color;
    zeroLine.strokeColor.widgetType =ColorWidget;
    zeroLine.strokeColor.subGroup = ZeroLineSubGroup;
    // Zeroline Stroke Style
    zeroLine.style.name = QObject::tr("Zero Line Style");
    zeroLine.style.editable = ReadAndWrite;
    zeroLine.style.dataType = PenStyle;
    zeroLine.style.widgetType = Combobox;
    zeroLine.style.subGroup = ZeroLineSubGroup;
    // Zeroline Stroke Thickness
    zeroLine.strokeThickness.name = QObject::tr("Zero Line Thickness");
    zeroLine.strokeThickness.value = 0;
    zeroLine.strokeThickness.valueLowerBound = 0;
    zeroLine.strokeThickness.valueUpperBound = 1000;
    zeroLine.strokeThickness.editable = ReadAndWrite;
    zeroLine.strokeThickness.dataType = Intiger;
    zeroLine.strokeThickness.widgetType = NormalSpinBox;
    zeroLine.strokeThickness.subGroup = ZeroLineSubGroup;
  }
};

struct Axis {
  enum AxisPropertyGroup {
    General,
    Scale,
    MajorTick,
    MinorTick,
  };
  Data<bool> visible;


};
}

#endif  // Property_H
