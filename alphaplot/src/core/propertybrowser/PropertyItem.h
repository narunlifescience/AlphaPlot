#ifndef PROPERTYITEM_H
#define PROPERTYITEM_H

#include <QDebug>
#include <QIcon>
#include <QObject>
#include <QVariant>

#include "ObjectBrowserTreeItem.h"

class MyWidget;

class PropertyItem : public QObject {
  Q_OBJECT
 public:
  enum class Property {
    None,
    // MyWidget
    BaseWindow_Base_Separator,
    BaseWindow_Name,
    BaseWindow_Label,
    BaseWindow_Geometry,
    BaseWindow_Geometry_X,
    BaseWindow_Geometry_Y,
    BaseWindow_Geometry_Width,
    BaseWindow_Geometry_Height,
    // Table
    TableWindow_Base_Separator,
    TableWindow_Row_Count,
    TableWindow_Column_Count,
    // Matrix
    MatrixWindow_Base_Separator,
    MatrixWindow_Row_Count,
    MatrixWindow_Column_Count,
    // Plot2DCanvas
    Plot2DCanvas_Base_Separator,
    Plot2DCanvas_Dimension,
    Plot2DCanvas_Dimension_Width,
    Plot2DCanvas_Dimension_Height,
    Plot2DCanvas_Background,
    Plot2DCanvas_DPR,
    Plot2DCanvas_OpenGL,
    Plot2DCanvas_Row_Spacing,
    Plot2DCanvas_Column_Spacing,
    // Layout 2D
    Plot2DLayout_Base_Separator,
    Plot2DLayout_Background,
    Plot2DLayout_FillStyle,
    Plot2DLayout_Row_Stretch_Factor,
    Plot2DLayout_Column_Stretch_Factor,
    Plot2DLayout_Outer_Rect,
    Plot2DLayout_Outer_Rect_X,
    Plot2DLayout_Outer_Rect_Y,
    Plot2DLayout_Outer_Rect_Width,
    Plot2DLayout_Outer_Rect_Height,
    Plot2DLayout_Margin,
    Plot2DLayout_Margin_Left,
    Plot2DLayout_Margin_Right,
    Plot2DLayout_Margin_Top,
    Plot2DLayout_Margin_Bottom,
    // Legend 2D
    Plot2DLegend_Base_Separator,
    Plot2DLegend_Visible,
    Plot2DLegend_X,
    Plot2DLegend_Y,
    Plot2DLegend_Direction,
    Plot2DLegend_Margin,
    Plot2DLegend_Margin_Left,
    Plot2DLegend_Margin_Bottom,
    Plot2DLegend_Margin_Right,
    Plot2DLegend_Margin_Top,
    Plot2DLegend_Text_Separator,
    Plot2DLegend_Font,
    Plot2DLegend_Text_Color,
    Plot2DLegend_Icon_Separator,
    Plot2DLegend_Icon_Width,
    Plot2DLegend_Icon_Height,
    Plot2DLegend_Icon_Padding,
    Plot2DLegend_Stroke_Fill_Separator,
    Plot2DLegend_Stroke_Color,
    Plot2DLegend_Stroke_Thickness,
    Plot2DLegend_Stroke_Style,
    Plot2DLegend_Background,
    Plot2DLegend_FillStyle,
    Plot2DLegend_Title_Separator,
    Plot2DLegend_Title_Visible,
    Plot2DLegend_Title_Text,
    Plot2DLegend_Title_Font,
    Plot2DLegend_Title_Color,
    // Axis 2D
    Plot2DAxis_Base_Separator,
    Plot2DAxis_Visible,
    Plot2DAxis_Offset,
    Plot2DAxis_From_Double,
    Plot2DAxis_To_Double,
    Plot2DAxis_From_Text,
    Plot2DAxis_To_Text,
    Plot2DAxis_From_DateTime,
    Plot2DAxis_To_DateTime,
    Plot2DAxis_Ending_Separator,
    Plot2DAxis_Upper_Ending,
    Plot2DAxis_Lower_Ending,
    Plot2DAxis_Type,
    Plot2DAxis_Inverted,
    Plot2DAxis_Antialiased,
    Plot2DAxis_Stroke_Color,
    Plot2DAxis_Stroke_Thickness,
    Plot2DAxis_Stroke_Style,
    Plot2DAxis_Label_Separator,
    Plot2DAxis_Label_Text,
    Plot2DAxis_Label_Font,
    Plot2DAxis_Label_Color,
    Plot2DAxis_Label_Padding,
    Plot2DAxis_Ticks_Separator,
    Plot2DAxis_Ticks_Visible,
    Plot2DAxis_Ticks_Count,
    Plot2DAxis_Ticks_Origin,
    Plot2DAxis_Ticks_LengthIn,
    Plot2DAxis_Ticks_LengthOut,
    Plot2DAxis_Ticks_Stroke_Color,
    Plot2DAxis_Ticks_Stroke_Thickness,
    Plot2DAxis_Ticks_Stroke_Style,
    Plot2DAxis_SubTicks_Separator,
    Plot2DAxis_SubTicks_Visible,
    Plot2DAxis_SubTicks_LengthIn,
    Plot2DAxis_SubTicks_LengthOut,
    Plot2DAxis_SubTicks_Stroke_Color,
    Plot2DAxis_SubTicks_Stroke_Thickness,
    Plot2DAxis_SubTicks_Stroke_Style,
    Plot2DAxis_TickLabel_Separator,
    Plot2DAxis_TickLabel_Visible,
    Plot2DAxis_TickLabel_StepStrategy,
    Plot2DAxis_TickLabel_Font,
    Plot2DAxis_TickLabel_Color,
    Plot2DAxis_TickLabel_Padding,
    Plot2DAxis_TickLabel_Rotation,
    Plot2DAxis_TickLabel_Side,
    Plot2DAxis_TickLabel_LogBase_Log,
    Plot2DAxis_TickLabel_Symbol_Pi,
    Plot2DAxis_TickLabel_Symbol_Value_Pi,
    Plot2DAxis_TickLabel_FractionStyle_Pi,
    Plot2DAxis_TickLabel_Format_Double,
    Plot2DAxis_TickLabel_Format_DateTime,
    Plot2DAxis_TickLabel_Precision_Double,
    // Grid2D
    Plot2DGrid_Horizontal_Separator,
    Plot2DGrid_Horizontal,
    Plot2DGrid_Horizontal_Major_Separator,
    Plot2DGrid_Horizontal_Major,
    Plot2DGrid_Horizontal_Major_Stroke_Color,
    Plot2DGrid_Horizontal_Major_Stroke_Thickness,
    Plot2DGrid_Horizontal_Major_Stroke_Style,
    Plot2DGrid_Horizontal_Zero_Separator,
    Plot2DGrid_Horizontal_Zero_Line,
    Plot2DGrid_Horizontal_Zero_Stroke_Color,
    Plot2DGrid_Horizontal_Zero_Stroke_Thickness,
    Plot2DGrid_Horizontal_Zero_Stroke_Style,
    Plot2DGrid_Horizontal_Minor_Separator,
    Plot2DGrid_Horizontal_Minor,
    Plot2DGrid_Horizontal_Minor_Stroke_Color,
    Plot2DGrid_Horizontal_Minor_Stroke_Thickness,
    Plot2DGrid_Horizontal_Minor_Stroke_Style,
    Plot2DGrid_Vertical_Separator,
    Plot2DGrid_Vertical,
    Plot2DGrid_Vertical_Major_Separator,
    Plot2DGrid_Vertical_Major,
    Plot2DGrid_Vertical_Major_Stroke_Color,
    Plot2DGrid_Vertical_Major_Stroke_Thickness,
    Plot2DGrid_Vertical_Major_Stroke_Style,
    Plot2DGrid_Vertical_Zero_Separator,
    Plot2DGrid_Vertical_Zero_Line,
    Plot2DGrid_Vertical_Zero_Stroke_Color,
    Plot2DGrid_Vertical_Zero_Stroke_Thickness,
    Plot2DGrid_Vertical_Zero_Stroke_Style,
    Plot2DGrid_Vertical_Minor_Separator,
    Plot2DGrid_Vertical_Minor,
    Plot2DGrid_Vertical_Minor_Stroke_Color,
    Plot2DGrid_Vertical_Minor_Stroke_Thickness,
    Plot2DGrid_Vertical_Minor_Stroke_Style,
    // TextItem2D
    Plot2DTextItem_Base_Separator,
    Plot2DTextItem_Position_X,
    Plot2DTextItem_Position_Y,
    Plot2DTextItem_Margin,
    Plot2DTextItem_Margin_Left,
    Plot2DTextItem_Margin_Right,
    Plot2DTextItem_Margin_Top,
    Plot2DTextItem_Margin_Bottom,
    Plot2DTextItem_Antialiased,
    Plot2DTextItem_Text_Separator,
    Plot2DTextItem_Text_Alignment,
    Plot2DTextItem_Text,
    Plot2DTextItem_Text_Rotation,
    Plot2DTextItem_Text_Font,
    Plot2DTextItem_Text_Color,
    Plot2DTextItem_Stroke_Color,
    Plot2DTextItem_Stroke_Thickness,
    Plot2DTextItem_Stroke_Style,
    Plot2DTextItem_Background_Color,
    Plot2DTextItem_Fill_Style,
    // LineItem2D
    Plot2DLineItem_Base_Separator,
    Plot2DLineItem_Position_X1,
    Plot2DLineItem_Position_Y1,
    Plot2DLineItem_Position_X2,
    Plot2DLineItem_Position_Y2,
    Plot2DLineItem_Antialiased,
    Plot2DLineItem_Stroke_Color,
    Plot2DLineItem_Stroke_Thickness,
    Plot2DLineItem_Stroke_Style,
    Plot2DLineItem_End_Separator,
    Plot2DLineItem_Starting_Style,
    Plot2DLineItem_Starting_Width,
    Plot2DLineItem_Starting_Height,
    Plot2DLineItem_Ending_Style,
    Plot2DLineItem_Ending_Width,
    Plot2DLineItem_Ending_Height,
    // ImageItem
    Plot2DImageItem_Base_Separator,
    Plot2DImageItem_Position_X,
    Plot2DImageItem_Position_Y,
    Plot2DImageItem_Source,
    Plot2DImageItem_Rotation,
    Plot2DImageItem_Stroke_Color,
    Plot2DImageItem_Stroke_Thickness,
    Plot2DImageItem_Stroke_Style,
    // Curve2D
    Plot2DCurve_Axis_Separator,
    Plot2DCurve_Axis_X,
    Plot2DCurve_Axis_Y,
    Plot2DCurve_Line_Separator,
    Plot2DCurve_Line_Antialiased,
    Plot2DCurve_Line_Style,
    Plot2DCurve_Line_Stroke_Color,
    Plot2DCurve_Line_Stroke_Thickness,
    Plot2DCurve_Line_Stroke_Style,
    Plot2DCurve_Areafill_Separator,
    Plot2DCurve_Fill_Area,
    Plot2DCurve_Fill_Color,
    Plot2DCurve_Fill_Style,
    Plot2DCurve_Scatter_Separator,
    Plot2DCurve_Scatter_Antialiased,
    Plot2DCurve_Scatter_Style,
    Plot2DCurve_Scatter_Size,
    Plot2DCurve_Scatter_Outline_Color,
    Plot2DCurve_Scatter_Outline_Thickness,
    Plot2DCurve_Scatter_Outline_Style,
    Plot2DCurve_Scatter_Fill_Color,
    Plot2DCurve_Scatter_Fill_Style,
    Plot2DCurve_Legend_Separator,
    Plot2DCurve_Legend_Status,
    Plot2DCurve_Legend_Text,
    // Graph2D
    Plot2DLS_Axis_Separator,
    Plot2DLS_Axis_X,
    Plot2DLS_Axis_Y,
    Plot2DLS_Line_Separator,
    Plot2DLS_Line_Antialiased,
    Plot2DLS_Line_Style,
    Plot2DLS_Line_Stroke_Color,
    Plot2DLS_Line_Stroke_Thickness,
    Plot2DLS_Line_Stroke_Style,
    Plot2DLS_Areafill_Separator,
    Plot2DLS_Fill_Area,
    Plot2DLS_Fill_Color,
    Plot2DLS_Fill_Style,
    Plot2DLS_Scatter_Separator,
    Plot2DLS_Scatter_Antialiased,
    Plot2DLS_Scatter_Style,
    Plot2DLS_Scatter_Size,
    Plot2DLS_Scatter_Outline_Color,
    Plot2DLS_Scatter_Outline_Thickness,
    Plot2DLS_Scatter_Outline_Style,
    Plot2DLS_Scatter_Fill_Color,
    Plot2DLS_Scatter_Fill_Style,
    Plot2DLS_Legend_Separator,
    Plot2DLS_Legend_Status,
    Plot2DLS_Legend_Text,
    // Channel2D
    Plot2DChannel_Axis_Separator,
    Plot2DChannel_Axis_X,
    Plot2DChannel_Axis_Y,
    Plot2DChannel_Line1_Separator,
    Plot2DChannel_Line1_Antialiased,
    Plot2DChannel_Line1_Style,
    Plot2DChannel_Line1_Stroke_Color,
    Plot2DChannel_Line1_Stroke_Thickness,
    Plot2DChannel_Line1_Stroke_Style,
    Plot2DChannel_Scatter1_Separator,
    Plot2DChannel_Scatter1_Antialiased,
    Plot2DChannel_Scatter1_Style,
    Plot2DChannel_Scatter1_Size,
    Plot2DChannel_Scatter1_Outline_Color,
    Plot2DChannel_Scatter1_Outline_Thickness,
    Plot2DChannel_Scatter1_Outline_Style,
    Plot2DChannel_Scatter1_Fill_Color,
    Plot2DChannel_Scatter1_Fill_Style,
    Plot2DChannel_Areafill_Separator,
    Plot2DChannel_Fill_Area,
    Plot2DChannel_Fill_Color,
    Plot2DChannel_Fill_Style,
    Plot2DChannel_Line2_Separator,
    Plot2DChannel_Line2_Antialiased,
    Plot2DChannel_Line2_Style,
    Plot2DChannel_Line2_Stroke_Color,
    Plot2DChannel_Line2_Stroke_Thickness,
    Plot2DChannel_Line2_Stroke_Style,
    Plot2DChannel_Scatter2_Separator,
    Plot2DChannel_Scatter2_Antialiased,
    Plot2DChannel_Scatter2_Style,
    Plot2DChannel_Scatter2_Size,
    Plot2DChannel_Scatter2_Outline_Color,
    Plot2DChannel_Scatter2_Outline_Thickness,
    Plot2DChannel_Scatter2_Outline_Style,
    Plot2DChannel_Scatter2_Fill_Color,
    Plot2DChannel_Scatter2_Fill_Style,
    Plot2DChannel_Legend_Separator,
    Plot2DChannel_Legend_Status,
    Plot2DChannel_Legend_Text,
    // StatBox2D
    Plot2DStatBox_Axis_Separator,
    Plot2DStatBox_Axis_X,
    Plot2DStatBox_Axis_Y,
    Plot2DStatBox_Box_Separator,
    Plot2DStatBox_Box_Antialiased,
    Plot2DStatBox_Box_Width,
    Plot2DStatBox_Box_Style,
    Plot2DStatBox_Box_Stroke_Color,
    Plot2DStatBox_Box_Stroke_Thickness,
    Plot2DStatBox_Box_Stroke_Style,
    Plot2DStatBox_Box_Fill_Antialiased,
    Plot2DStatBox_Box_Fill_Color,
    Plot2DStatBox_Box_Fill_Style,
    Plot2DStatBox_Whisker_Separator,
    Plot2DStatBox_Whisker_Antialiased,
    Plot2DStatBox_Whisker_Width,
    Plot2DStatBox_Whisker_Style,
    Plot2DStatBox_Whisker_Stroke_Color,
    Plot2DStatBox_Whisker_Stroke_Thickness,
    Plot2DStatBox_Whisker_Stroke_Style,
    Plot2DStatBox_WhiskerBar_Separator,
    Plot2DStatBox_WhiskerBar_Stroke_Color,
    Plot2DStatBox_WhiskerBar_Stroke_Thickness,
    Plot2DStatBox_WhiskerBar_Stroke_Style,
    Plot2DStatBox_Median_Separator,
    Plot2DStatBox_Median_Stroke_Color,
    Plot2DStatBox_Median_Stroke_Thickness,
    Plot2DStatBox_Median_Stroke_Style,
    Plot2DStatBox_Scatter_Separator,
    Plot2DStatBox_Scatter_Antialiased,
    Plot2DStatBox_Scatter_Show,
    Plot2DStatBox_Scatter_Style,
    Plot2DStatBox_Scatter_Size,
    Plot2DStatBox_Scatter_Outline_Color,
    Plot2DStatBox_Scatter_Outline_Thickness,
    Plot2DStatBox_Scatter_Outline_Style,
    Plot2DStatBox_Scatter_Fill_Color,
    Plot2DStatBox_Scatter_Fill_Style,
    Plot2DStatBox_Ticktext_Separator,
    Plot2DStatBox_Tick_Text,
    // Vector2D
    Plot2DVector_Axis_Separator,
    Plot2DVector_Axis_X,
    Plot2DVector_Axis_Y,
    Plot2DVector_Line_Separator,
    Plot2DVector_Line_Antialiased,
    Plot2DVector_Line_Stroke_Color,
    Plot2DVector_Line_Stroke_Thickness,
    Plot2DVector_Line_Stroke_Style,
    Plot2DVector_Ending_Separator,
    Plot2DVector_Ending_Style,
    Plot2DVector_Ending_Width,
    Plot2DVector_Ending_Height,
    Plot2DVector_Legend_Separator,
    Plot2DVector_Legend_Status,
    Plot2DVector_Legend_Text,
    // Bar2D
    Plot2DBar_Axis_Separator,
    Plot2DBar_Axis_X,
    Plot2DBar_Axis_Y,
    Plot2DBar_Bar_Separator,
    Plot2DBar_Width,
    Plot2DBar_Gap,
    Plot2DBar_Line_Antialiased,
    Plot2DBar_Line_Stroke_Color,
    Plot2DBar_Line_Stroke_Thickness,
    Plot2DBar_Line_Stroke_Style,
    Plot2DBar_Fill_Antialiased,
    Plot2DBar_Fill_Color,
    Plot2DBar_Fill_Style,
    Plot2DBar_Histogram_Separator,
    Plot2DBar_Histogram_AutoBin,
    Plot2DBar_Histogram_BinSize,
    Plot2DBar_Histogram_Begin,
    Plot2DBar_Histogram_End,
    Plot2DBar_Legend_Separator,
    Plot2DBar_Legend_Status,
    Plot2DBar_Legend_Text,
    // Pie2D
    Plot2DPie_Basic_Separator,
    Plot2DPie_Style,
    Plot2DPie_Stroke_Color,
    Plot2DPie_Stroke_Thickness,
    Plot2DPie_Stroke_Style,
    Plot2DPie_Margin_Percent,
    // ColorMap2D
    Plot2DCM_Basic_Separator,
    Plot2DCM_Interpolate,
    Plot2DCM_Tight_Boundary,
    Plot2DCM_Level_Count,
    Plot2DCM_Gradient,
    Plot2DCM_Gradient_Invert,
    Plot2DCM_Periodic,
    Plot2DCM_Scale_Separator,
    Plot2DCM_Scale_Visible,
    Plot2DCM_Scale_Width,
    Plot2DCM_Axis_Separator,
    Plot2DCM_Axis_Visible,
    Plot2DCM_Axis_Offset,
    Plot2DCM_Axis_From_Double,
    Plot2DCM_Axis_To_Double,
    Plot2DCM_Axis_Type,
    Plot2DCM_Axis_Inverted,
    Plot2DCM_Axis_Antialiased,
    Plot2DCM_Axis_Stroke_Color,
    Plot2DCM_Axis_Stroke_Thickness,
    Plot2DCM_Axis_Stroke_Style,
    Plot2DCM_Axis_Label_Separator,
    Plot2DCM_Axis_Label_Text,
    Plot2DCM_Axis_Label_Font,
    Plot2DCM_Axis_Label_Color,
    Plot2DCM_Axis_Label_Padding,
    Plot2DCM_Axis_Ticks_Separator,
    Plot2DCM_Axis_Ticks_Visible,
    Plot2DCM_Axis_Ticks_LengthIn,
    Plot2DCM_Axis_Ticks_LengthOut,
    Plot2DCM_Axis_Ticks_Stroke_Color,
    Plot2DCM_Axis_Ticks_Stroke_Thickness,
    Plot2DCM_Axis_Ticks_Stroke_Style,
    Plot2DCM_Axis_SubTicks_Separator,
    Plot2DCM_Axis_SubTicks_Visible,
    Plot2DCM_Axis_SubTicks_LengthIn,
    Plot2DCM_Axis_SubTicks_LengthOut,
    Plot2DCM_Axis_SubTicks_Stroke_Color,
    Plot2DCM_Axis_SubTicks_Stroke_Thickness,
    Plot2DCM_Axis_SubTicks_Stroke_Style,
    Plot2DCM_Axis_TickLabel_Separator,
    Plot2DCM_Axis_TickLabel_Visible,
    Plot2DCM_Axis_TickLabel_Font,
    Plot2DCM_Axis_TickLabel_Color,
    Plot2DCM_Axis_TickLabel_Padding,
    Plot2DCM_Axis_TickLabel_Rotation,
    Plot2DCM_Axis_TickLabel_Side,
    Plot2DCM_Axis_TickLabel_Format_Double,
    Plot2DCM_Axis_TickLabel_Precision_Double,
    // ErrorBar2D
    Plot2DErrBar_Basic_Separator,
    Plot2DErrBar_Whisker_Width,
    Plot2DErrBar_Symbol_Gap,
    Plot2DErrBar_Antialiased,
    Plot2DErrBar_Stroke_Color,
    Plot2DErrBar_Stroke_Thickness,
    Plot2DErrBar_Stroke_Style,
    Plot2DErrBar_Fill_Color,
    Plot2DErrBar_Fill_Style,
    // Plot3DCanvas
    Plot3DCanvas_Base_Separator,
    Plot3DCanvas_Theme,
    Plot3DCanvas_Dimension,
    Plot3DCanvas_Dimension_Width,
    Plot3DCanvas_Dimension_Height,
  };

  enum class PropertyType {
    Separator,
    String,
    Int,
    Double,
    ScientificDouble,
    Bool,
    Enum,
    Color,
    Font,
    Size,
    Rect,
    Margin,
    DateTime,
  };

  struct Pair {
    QString name;
    QVariant value;
    Pair(QString name, QVariant value) : name(name), value(value) {}
  };

  static PropertyItem* create(
      PropertyItem* parent, ObjectBrowserTreeItem* item,
      const Property& property,
      QList<QPair<QIcon, QString>> enumList = QList<QPair<QIcon, QString>>());
  static void init();
  ~PropertyItem();

  template <class T>
  T PropertyData() {
    return data(1, Qt::ItemDataRole::UserRole).value<T>();
  }

  QWidget* createEditor(QWidget* parent) const;
  void setEditorData(QWidget* editor) const;
  QVariant editorData(QWidget* editor) const;

  void appendChild(PropertyItem* child);
  void insertChild(int index, PropertyItem* child);
  void removeChildren(int from, int to);
  void setParent(PropertyItem* parent);
  PropertyItem* parent() const;
  PropertyItem* child(int row);
  int childCount() const;
  int columnCount() const;
  Qt::ItemFlags flags(int column) const;
  int row() const;
  QVariant data(int column, int role) const;

  PropertyItem::Property property() const { return property_; }
  PropertyItem::PropertyType propertyType() { return propertytype_; }

  bool isreadonly() const;
  void setReadonly(const bool status);
  QString toString() const;

  void setLowerLimitInt(const int value) { lowerboundint_ = value; }
  void setUpperLimitInt(const int value) { upperboundint_ = value; }
  void setLowerLimitDouble(const double value) { lowerbounddouble_ = value; }
  void setUpperLimitDouble(const double value) { upperbounddouble_ = value; }
  void setSingleStepInt(const int value) { singlestepint_ = value; }
  void setSingleStepDouble(const double value) { singlestepdouble_ = value; }
  void setPrefix(const QString& value) { prefix_ = value; }
  void setSuffix(const QString& value) { suffix_ = value; }
  void setPrecision(const int value) { precision_ = value; }
  void setDateTimeFormat(const QString& format) { datetimeformat_ = format; }

 private:
  void setEnumNames(QList<QPair<QIcon, QString>> enumList) {
    enumList_ = enumList;
  }

 protected:
  PropertyItem(PropertyItem* parent, ObjectBrowserTreeItem* item,
               const PropertyItem::Property& property);

  Pair value() const;

 signals:
  void namechange(MyWidget* widget, const QString& name);
  void labelchange(MyWidget* widget, const QString& label);
  void outofrange(const double min, const double max);
  void readonlyChanged(const bool status);

 private slots:
  void setStringValue(const QString& val);
  void setIntValue(const int& val);
  void setDoubleValue(const double& val);
  void setScientificDoubleValue(const double& val);
  void setBoolValue(const bool& val);
  void setEnumValue(const int& val);
  void setColorValue(const QColor& val);
  void setFontValue(const QFont& val);
  void setDateTimeValue(const QDateTime& val);

 private:
  ObjectBrowserTreeItem* item_;
  Property property_;
  PropertyType propertytype_;
  PropertyItem* parentItem_;
  QList<PropertyItem*> childItems_;
  QList<QPair<QIcon, QString>> enumList_;
  bool readonly_;
  int lowerboundint_;
  int upperboundint_;
  double lowerbounddouble_;
  double upperbounddouble_;
  int singlestepint_;
  double singlestepdouble_;
  QString prefix_;
  QString suffix_;
  int precision_;
  QString datetimeformat_;
};

#endif  // PROPERTYITEM_H
