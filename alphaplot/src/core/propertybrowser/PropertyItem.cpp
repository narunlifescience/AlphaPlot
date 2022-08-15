#include "PropertyItem.h"

#include <QApplication>
#include <QBrush>
#include <QCheckBox>
#include <QComboBox>
#include <QFontDatabase>
#include <QLineEdit>
#include <QObject>
#include <QtDebug>
#include <limits>

#include "2Dplot/Channel2D.h"
#include "2Dplot/ColorMap2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/DataManager2D.h"
#include "2Dplot/ErrorBar2D.h"
#include "2Dplot/Grid2D.h"
#include "2Dplot/GridPair2D.h"
#include "2Dplot/ImageItem2D.h"
#include "2Dplot/Layout2D.h"
#include "2Dplot/LayoutGrid2D.h"
#include "2Dplot/Legend2D.h"
#include "2Dplot/LineItem2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "2Dplot/Pie2D.h"
#include "2Dplot/Plot2D.h"
#include "2Dplot/TextItem2D.h"
#include "3Dplot/Bar3D.h"
#include "3Dplot/Layout3D.h"
#include "3Dplot/Scatter3D.h"
#include "3Dplot/Surface3D.h"
#include "ApplicationWindow.h"
#include "CustomWidgets.h"
#include "Matrix.h"
#include "ScientificDoubleSpinBox.h"
#include "Table.h"
#include "core/Utilities.h"

PropertyItem::PropertyItem(PropertyItem *parent, ObjectBrowserTreeItem *item,
                           const PropertyItem::Property &property)
    : item_(item),
      property_(property),
      parentItem_(parent),
      readonly_(false),
      lowerboundint_(-std::numeric_limits<int>::max()),
      upperboundint_(std::numeric_limits<int>::max()),
      lowerbounddouble_(-std::numeric_limits<double>::max()),
      upperbounddouble_(std::numeric_limits<double>::max()),
      singlestepint_(1),
      singlestepdouble_(0.1),
      prefix_(QString()),
      suffix_(QString()),
      precision_(16),
      datetimeformat_("dd.MM.yyyy") {
  if (parentItem_) parentItem_->appendChild(this);
  switch (property_) {
    case PropertyItem::Property::BaseWindow_Base_Separator:
    case PropertyItem::Property::TableWindow_Base_Separator:
    case PropertyItem::Property::MatrixWindow_Base_Separator:
    case PropertyItem::Property::Plot2DCanvas_Base_Separator:
    case PropertyItem::Property::Plot2DLayout_Base_Separator:
    case PropertyItem::Property::Plot2DLegend_Base_Separator:
    case PropertyItem::Property::Plot2DLegend_Stroke_Fill_Separator:
    case PropertyItem::Property::Plot2DLegend_Text_Separator:
    case PropertyItem::Property::Plot2DLegend_Icon_Separator:
    case PropertyItem::Property::Plot2DLegend_Title_Separator:
    case PropertyItem::Property::Plot2DAxis_Base_Separator:
    case PropertyItem::Property::Plot2DAxis_Ending_Separator:
    case PropertyItem::Property::Plot2DAxis_Label_Separator:
    case PropertyItem::Property::Plot2DAxis_Ticks_Separator:
    case PropertyItem::Property::Plot2DAxis_SubTicks_Separator:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Separator:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Separator:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Separator:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Separator:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Separator:
    case PropertyItem::Property::Plot2DGrid_Vertical_Separator:
    case PropertyItem::Property::Plot2DGrid_Vertical_Major_Separator:
    case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Separator:
    case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Separator:
    case PropertyItem::Property::Plot2DTextItem_Base_Separator:
    case PropertyItem::Property::Plot2DTextItem_Text_Separator:
    case PropertyItem::Property::Plot2DLineItem_Base_Separator:
    case PropertyItem::Property::Plot2DLineItem_End_Separator:
    case PropertyItem::Property::Plot2DImageItem_Base_Separator:
    case PropertyItem::Property::Plot2DCurve_Axis_Separator:
    case PropertyItem::Property::Plot2DCurve_Line_Separator:
    case PropertyItem::Property::Plot2DCurve_Areafill_Separator:
    case PropertyItem::Property::Plot2DCurve_Scatter_Separator:
    case PropertyItem::Property::Plot2DCurve_Legend_Separator:
    case PropertyItem::Property::Plot2DLS_Axis_Separator:
    case PropertyItem::Property::Plot2DLS_Line_Separator:
    case PropertyItem::Property::Plot2DLS_Areafill_Separator:
    case PropertyItem::Property::Plot2DLS_Scatter_Separator:
    case PropertyItem::Property::Plot2DLS_Legend_Separator:
    case PropertyItem::Property::Plot2DChannel_Axis_Separator:
    case PropertyItem::Property::Plot2DChannel_Line1_Separator:
    case PropertyItem::Property::Plot2DChannel_Scatter1_Separator:
    case PropertyItem::Property::Plot2DChannel_Areafill_Separator:
    case PropertyItem::Property::Plot2DChannel_Line2_Separator:
    case PropertyItem::Property::Plot2DChannel_Scatter2_Separator:
    case PropertyItem::Property::Plot2DChannel_Legend_Separator:
    case PropertyItem::Property::Plot2DStatBox_Axis_Separator:
    case PropertyItem::Property::Plot2DStatBox_Box_Separator:
    case PropertyItem::Property::Plot2DStatBox_Whisker_Separator:
    case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Separator:
    case PropertyItem::Property::Plot2DStatBox_Median_Separator:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Separator:
    case PropertyItem::Property::Plot2DStatBox_Ticktext_Separator:
    case PropertyItem::Property::Plot2DVector_Axis_Separator:
    case PropertyItem::Property::Plot2DVector_Line_Separator:
    case PropertyItem::Property::Plot2DVector_Ending_Separator:
    case PropertyItem::Property::Plot2DVector_Legend_Separator:
    case PropertyItem::Property::Plot2DBar_Axis_Separator:
    case PropertyItem::Property::Plot2DBar_Bar_Separator:
    case PropertyItem::Property::Plot2DBar_Histogram_Separator:
    case PropertyItem::Property::Plot2DBar_Legend_Separator:
    case PropertyItem::Property::Plot2DPie_Basic_Separator:
    case PropertyItem::Property::Plot2DCM_Basic_Separator:
    case PropertyItem::Property::Plot2DCM_Scale_Separator:
    case PropertyItem::Property::Plot2DCM_Axis_Separator:
    case PropertyItem::Property::Plot2DCM_Axis_Label_Separator:
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_Separator:
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Separator:
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Separator:
    case PropertyItem::Property::Plot2DErrBar_Basic_Separator:
    case PropertyItem::Property::Plot3DCanvas_Base_Separator:
      propertytype_ = PropertyItem::PropertyType::Separator;
      break;
    case PropertyItem::Property::None:
    case PropertyItem::Property::BaseWindow_Name:
    case PropertyItem::Property::BaseWindow_Label:
    case PropertyItem::Property::Plot2DLegend_Title_Text:
    case PropertyItem::Property::Plot2DAxis_Label_Text:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Symbol_Pi:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Format_DateTime:
    case PropertyItem::Property::Plot2DTextItem_Text:
    case PropertyItem::Property::Plot2DImageItem_Source:
    case PropertyItem::Property::Plot2DCurve_Legend_Text:
    case PropertyItem::Property::Plot2DLS_Legend_Text:
    case PropertyItem::Property::Plot2DChannel_Legend_Text:
    case PropertyItem::Property::Plot2DStatBox_Tick_Text:
    case PropertyItem::Property::Plot2DVector_Legend_Text:
    case PropertyItem::Property::Plot2DBar_Legend_Text:
    case PropertyItem::Property::Plot2DCM_Axis_Label_Text:
      propertytype_ = PropertyItem::PropertyType::String;
      break;
    case PropertyItem::Property::BaseWindow_Geometry_X:
    case PropertyItem::Property::BaseWindow_Geometry_Y:
    case PropertyItem::Property::BaseWindow_Geometry_Width:
    case PropertyItem::Property::BaseWindow_Geometry_Height:
    case PropertyItem::Property::TableWindow_Row_Count:
    case PropertyItem::Property::TableWindow_Column_Count:
    case PropertyItem::Property::MatrixWindow_Row_Count:
    case PropertyItem::Property::MatrixWindow_Column_Count:
    case PropertyItem::Property::Plot2DCanvas_Dimension_Width:
    case PropertyItem::Property::Plot2DCanvas_Dimension_Height:
    case PropertyItem::Property::Plot2DCanvas_Row_Spacing:
    case PropertyItem::Property::Plot2DCanvas_Column_Spacing:
    case PropertyItem::Property::Plot2DLayout_Outer_Rect_X:
    case PropertyItem::Property::Plot2DLayout_Outer_Rect_Y:
    case PropertyItem::Property::Plot2DLayout_Outer_Rect_Width:
    case PropertyItem::Property::Plot2DLayout_Outer_Rect_Height:
    case PropertyItem::Property::Plot2DLayout_Margin_Left:
    case PropertyItem::Property::Plot2DLayout_Margin_Right:
    case PropertyItem::Property::Plot2DLayout_Margin_Top:
    case PropertyItem::Property::Plot2DLayout_Margin_Bottom:
    case PropertyItem::Property::Plot2DLegend_Margin_Left:
    case PropertyItem::Property::Plot2DLegend_Margin_Bottom:
    case PropertyItem::Property::Plot2DLegend_Margin_Right:
    case PropertyItem::Property::Plot2DLegend_Margin_Top:
    case PropertyItem::Property::Plot2DLegend_Icon_Width:
    case PropertyItem::Property::Plot2DLegend_Icon_Height:
    case PropertyItem::Property::Plot2DLegend_Icon_Padding:
    case PropertyItem::Property::Plot2DAxis_Offset:
    case PropertyItem::Property::Plot2DAxis_Label_Padding:
    case PropertyItem::Property::Plot2DAxis_Ticks_Count:
    case PropertyItem::Property::Plot2DAxis_Ticks_LengthIn:
    case PropertyItem::Property::Plot2DAxis_Ticks_LengthOut:
    case PropertyItem::Property::Plot2DAxis_SubTicks_LengthIn:
    case PropertyItem::Property::Plot2DAxis_SubTicks_LengthOut:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Padding:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Precision_Double:
    case PropertyItem::Property::Plot2DTextItem_Margin_Left:
    case PropertyItem::Property::Plot2DTextItem_Margin_Right:
    case PropertyItem::Property::Plot2DTextItem_Margin_Top:
    case PropertyItem::Property::Plot2DTextItem_Margin_Bottom:
    case PropertyItem::Property::Plot2DPie_Margin_Percent:
    case PropertyItem::Property::Plot2DCM_Level_Count:
    case PropertyItem::Property::Plot2DCM_Scale_Width:
    case PropertyItem::Property::Plot2DCM_Axis_Offset:
    case PropertyItem::Property::Plot2DCM_Axis_Label_Padding:
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_LengthIn:
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_LengthOut:
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_LengthIn:
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_LengthOut:
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Padding:
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Precision_Double:
    case PropertyItem::Property::Plot3DCanvas_Dimension_Width:
    case PropertyItem::Property::Plot3DCanvas_Dimension_Height:
      propertytype_ = PropertyItem::PropertyType::Int;
      break;
    case PropertyItem::Property::Plot2DCanvas_DPR:
    case PropertyItem::Property::Plot2DLayout_Row_Stretch_Factor:
    case PropertyItem::Property::Plot2DLayout_Column_Stretch_Factor:
    case PropertyItem::Property::Plot2DLegend_X:
    case PropertyItem::Property::Plot2DLegend_Y:
    case PropertyItem::Property::Plot2DLegend_Stroke_Thickness:
    case PropertyItem::Property::Plot2DAxis_From_Text:
    case PropertyItem::Property::Plot2DAxis_To_Text:
    case PropertyItem::Property::Plot2DAxis_Stroke_Thickness:
    case PropertyItem::Property::Plot2DAxis_Ticks_Origin:
    case PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Thickness:
    case PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Thickness:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Rotation:
    case PropertyItem::Property::Plot2DAxis_TickLabel_LogBase_Log:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Symbol_Value_Pi:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Thickness:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Thickness:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Thickness:
    case PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Thickness:
    case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Thickness:
    case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Thickness:
    case PropertyItem::Property::Plot2DTextItem_Position_X:
    case PropertyItem::Property::Plot2DTextItem_Position_Y:
    case PropertyItem::Property::Plot2DTextItem_Text_Rotation:
    case PropertyItem::Property::Plot2DTextItem_Stroke_Thickness:
    case PropertyItem::Property::Plot2DLineItem_Position_X1:
    case PropertyItem::Property::Plot2DLineItem_Position_Y1:
    case PropertyItem::Property::Plot2DLineItem_Position_X2:
    case PropertyItem::Property::Plot2DLineItem_Position_Y2:
    case PropertyItem::Property::Plot2DLineItem_Stroke_Thickness:
    case PropertyItem::Property::Plot2DLineItem_Starting_Width:
    case PropertyItem::Property::Plot2DLineItem_Starting_Height:
    case PropertyItem::Property::Plot2DLineItem_Ending_Width:
    case PropertyItem::Property::Plot2DLineItem_Ending_Height:
    case PropertyItem::Property::Plot2DImageItem_Position_X:
    case PropertyItem::Property::Plot2DImageItem_Position_Y:
    case PropertyItem::Property::Plot2DImageItem_Rotation:
    case PropertyItem::Property::Plot2DImageItem_Stroke_Thickness:
    case PropertyItem::Property::Plot2DCurve_Line_Stroke_Thickness:
    case PropertyItem::Property::Plot2DCurve_Scatter_Size:
    case PropertyItem::Property::Plot2DCurve_Scatter_Outline_Thickness:
    case PropertyItem::Property::Plot2DLS_Line_Stroke_Thickness:
    case PropertyItem::Property::Plot2DLS_Scatter_Size:
    case PropertyItem::Property::Plot2DLS_Scatter_Outline_Thickness:
    case PropertyItem::Property::Plot2DChannel_Line1_Stroke_Thickness:
    case PropertyItem::Property::Plot2DChannel_Scatter1_Size:
    case PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Thickness:
    case PropertyItem::Property::Plot2DChannel_Line2_Stroke_Thickness:
    case PropertyItem::Property::Plot2DChannel_Scatter2_Size:
    case PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Thickness:
    case PropertyItem::Property::Plot2DStatBox_Box_Width:
    case PropertyItem::Property::Plot2DStatBox_Box_Stroke_Thickness:
    case PropertyItem::Property::Plot2DStatBox_Whisker_Width:
    case PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Thickness:
    case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Thickness:
    case PropertyItem::Property::Plot2DStatBox_Median_Stroke_Thickness:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Size:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Thickness:
    case PropertyItem::Property::Plot2DVector_Line_Stroke_Thickness:
    case PropertyItem::Property::Plot2DVector_Ending_Width:
    case PropertyItem::Property::Plot2DVector_Ending_Height:
    case PropertyItem::Property::Plot2DBar_Width:
    case PropertyItem::Property::Plot2DBar_Gap:
    case PropertyItem::Property::Plot2DBar_Histogram_BinSize:
    case PropertyItem::Property::Plot2DBar_Histogram_Begin:
    case PropertyItem::Property::Plot2DBar_Histogram_End:
    case PropertyItem::Property::Plot2DBar_Line_Stroke_Thickness:
    case PropertyItem::Property::Plot2DPie_Stroke_Thickness:
    case PropertyItem::Property::Plot2DCM_Axis_Stroke_Thickness:
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Thickness:
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Thickness:
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Rotation:
    case PropertyItem::Property::Plot2DErrBar_Whisker_Width:
    case PropertyItem::Property::Plot2DErrBar_Symbol_Gap:
    case PropertyItem::Property::Plot2DErrBar_Stroke_Thickness:
      propertytype_ = PropertyItem::PropertyType::Double;
      break;
    case PropertyItem::Property::Plot2DAxis_From_Double:
    case PropertyItem::Property::Plot2DAxis_To_Double:
    case PropertyItem::Property::Plot2DCM_Axis_From_Double:
    case PropertyItem::Property::Plot2DCM_Axis_To_Double:
      propertytype_ = PropertyItem::PropertyType::ScientificDouble;
      break;
    case PropertyItem::Property::Plot2DCanvas_OpenGL:
    case PropertyItem::Property::Plot2DLayout_Margin:
    case PropertyItem::Property::Plot2DLegend_Visible:
    case PropertyItem::Property::Plot2DLegend_Title_Visible:
    case PropertyItem::Property::Plot2DAxis_Visible:
    case PropertyItem::Property::Plot2DAxis_Inverted:
    case PropertyItem::Property::Plot2DAxis_Antialiased:
    case PropertyItem::Property::Plot2DAxis_Ticks_Visible:
    case PropertyItem::Property::Plot2DAxis_SubTicks_Visible:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Visible:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Major:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Line:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Minor:
    case PropertyItem::Property::Plot2DGrid_Vertical_Major:
    case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Line:
    case PropertyItem::Property::Plot2DGrid_Vertical_Minor:
    case PropertyItem::Property::Plot2DTextItem_Antialiased:
    case PropertyItem::Property::Plot2DLineItem_Antialiased:
    case PropertyItem::Property::Plot2DCurve_Line_Antialiased:
    case PropertyItem::Property::Plot2DCurve_Fill_Area:
    case PropertyItem::Property::Plot2DCurve_Scatter_Antialiased:
    case PropertyItem::Property::Plot2DCurve_Legend_Status:
    case PropertyItem::Property::Plot2DLS_Line_Antialiased:
    case PropertyItem::Property::Plot2DLS_Fill_Area:
    case PropertyItem::Property::Plot2DLS_Scatter_Antialiased:
    case PropertyItem::Property::Plot2DLS_Legend_Status:
    case PropertyItem::Property::Plot2DChannel_Line1_Antialiased:
    case PropertyItem::Property::Plot2DChannel_Scatter1_Antialiased:
    case PropertyItem::Property::Plot2DChannel_Fill_Area:
    case PropertyItem::Property::Plot2DChannel_Line2_Antialiased:
    case PropertyItem::Property::Plot2DChannel_Scatter2_Antialiased:
    case PropertyItem::Property::Plot2DChannel_Legend_Status:
    case PropertyItem::Property::Plot2DStatBox_Box_Antialiased:
    case PropertyItem::Property::Plot2DStatBox_Box_Fill_Antialiased:
    case PropertyItem::Property::Plot2DStatBox_Whisker_Antialiased:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Antialiased:
    case PropertyItem::Property::Plot2DVector_Line_Antialiased:
    case PropertyItem::Property::Plot2DVector_Legend_Status:
    case PropertyItem::Property::Plot2DBar_Line_Antialiased:
    case PropertyItem::Property::Plot2DBar_Fill_Antialiased:
    case PropertyItem::Property::Plot2DBar_Histogram_AutoBin:
    case PropertyItem::Property::Plot2DBar_Legend_Status:
    case PropertyItem::Property::Plot2DCM_Interpolate:
    case PropertyItem::Property::Plot2DCM_Tight_Boundary:
    case PropertyItem::Property::Plot2DCM_Gradient_Invert:
    case PropertyItem::Property::Plot2DCM_Periodic:
    case PropertyItem::Property::Plot2DCM_Scale_Visible:
    case PropertyItem::Property::Plot2DCM_Axis_Visible:
    case PropertyItem::Property::Plot2DCM_Axis_Inverted:
    case PropertyItem::Property::Plot2DCM_Axis_Antialiased:
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_Visible:
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Visible:
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Visible:
    case PropertyItem::Property::Plot2DErrBar_Antialiased:
      propertytype_ = PropertyItem::PropertyType::Bool;
      break;
    case PropertyItem::Property::Plot2DLayout_FillStyle:
    case PropertyItem::Property::Plot2DLegend_Direction:
    case PropertyItem::Property::Plot2DLegend_Stroke_Style:
    case PropertyItem::Property::Plot2DLegend_FillStyle:
    case PropertyItem::Property::Plot2DAxis_Upper_Ending:
    case PropertyItem::Property::Plot2DAxis_Lower_Ending:
    case PropertyItem::Property::Plot2DAxis_Type:
    case PropertyItem::Property::Plot2DAxis_Stroke_Style:
    case PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Style:
    case PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Style:
    case PropertyItem::Property::Plot2DAxis_TickLabel_StepStrategy:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Side:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Format_Double:
    case PropertyItem::Property::Plot2DAxis_TickLabel_FractionStyle_Pi:
    case PropertyItem::Property::Plot2DGrid_Horizontal:
    case PropertyItem::Property::Plot2DGrid_Vertical:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Style:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Style:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Style:
    case PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Style:
    case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Style:
    case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Style:
    case PropertyItem::Property::Plot2DTextItem_Text_Alignment:
    case PropertyItem::Property::Plot2DTextItem_Stroke_Style:
    case PropertyItem::Property::Plot2DTextItem_Fill_Style:
    case PropertyItem::Property::Plot2DLineItem_Stroke_Style:
    case PropertyItem::Property::Plot2DLineItem_Starting_Style:
    case PropertyItem::Property::Plot2DLineItem_Ending_Style:
    case PropertyItem::Property::Plot2DImageItem_Stroke_Style:
    case PropertyItem::Property::Plot2DCurve_Axis_X:
    case PropertyItem::Property::Plot2DCurve_Axis_Y:
    case PropertyItem::Property::Plot2DCurve_Line_Style:
    case PropertyItem::Property::Plot2DCurve_Line_Stroke_Style:
    case PropertyItem::Property::Plot2DCurve_Fill_Style:
    case PropertyItem::Property::Plot2DCurve_Scatter_Style:
    case PropertyItem::Property::Plot2DCurve_Scatter_Outline_Style:
    case PropertyItem::Property::Plot2DCurve_Scatter_Fill_Style:
    case PropertyItem::Property::Plot2DLS_Axis_X:
    case PropertyItem::Property::Plot2DLS_Axis_Y:
    case PropertyItem::Property::Plot2DLS_Line_Style:
    case PropertyItem::Property::Plot2DLS_Line_Stroke_Style:
    case PropertyItem::Property::Plot2DLS_Fill_Style:
    case PropertyItem::Property::Plot2DLS_Scatter_Style:
    case PropertyItem::Property::Plot2DLS_Scatter_Outline_Style:
    case PropertyItem::Property::Plot2DLS_Scatter_Fill_Style:
    case PropertyItem::Property::Plot2DChannel_Axis_X:
    case PropertyItem::Property::Plot2DChannel_Axis_Y:
    case PropertyItem::Property::Plot2DChannel_Line1_Style:
    case PropertyItem::Property::Plot2DChannel_Line1_Stroke_Style:
    case PropertyItem::Property::Plot2DChannel_Scatter1_Style:
    case PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Style:
    case PropertyItem::Property::Plot2DChannel_Scatter1_Fill_Style:
    case PropertyItem::Property::Plot2DChannel_Fill_Style:
    case PropertyItem::Property::Plot2DChannel_Line2_Style:
    case PropertyItem::Property::Plot2DChannel_Line2_Stroke_Style:
    case PropertyItem::Property::Plot2DChannel_Scatter2_Style:
    case PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Style:
    case PropertyItem::Property::Plot2DChannel_Scatter2_Fill_Style:
    case PropertyItem::Property::Plot2DStatBox_Axis_X:
    case PropertyItem::Property::Plot2DStatBox_Axis_Y:
    case PropertyItem::Property::Plot2DStatBox_Box_Style:
    case PropertyItem::Property::Plot2DStatBox_Box_Stroke_Style:
    case PropertyItem::Property::Plot2DStatBox_Box_Fill_Style:
    case PropertyItem::Property::Plot2DStatBox_Whisker_Style:
    case PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Style:
    case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Style:
    case PropertyItem::Property::Plot2DStatBox_Median_Stroke_Style:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Show:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Style:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Style:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Fill_Style:
    case PropertyItem::Property::Plot2DVector_Axis_X:
    case PropertyItem::Property::Plot2DVector_Axis_Y:
    case PropertyItem::Property::Plot2DVector_Line_Stroke_Style:
    case PropertyItem::Property::Plot2DVector_Ending_Style:
    case PropertyItem::Property::Plot2DBar_Axis_X:
    case PropertyItem::Property::Plot2DBar_Axis_Y:
    case PropertyItem::Property::Plot2DBar_Line_Stroke_Style:
    case PropertyItem::Property::Plot2DBar_Fill_Style:
    case PropertyItem::Property::Plot2DPie_Style:
    case PropertyItem::Property::Plot2DPie_Stroke_Style:
    case PropertyItem::Property::Plot2DCM_Gradient:
    case PropertyItem::Property::Plot2DCM_Axis_Type:
    case PropertyItem::Property::Plot2DCM_Axis_Stroke_Style:
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Style:
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Style:
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Side:
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Format_Double:
    case PropertyItem::Property::Plot2DErrBar_Stroke_Style:
    case PropertyItem::Property::Plot2DErrBar_Fill_Style:
    case PropertyItem::Property::Plot3DCanvas_Theme:
      propertytype_ = PropertyItem::PropertyType::Enum;
      break;
    case PropertyItem::Property::Plot2DCanvas_Background:
    case PropertyItem::Property::Plot2DLayout_Background:
    case PropertyItem::Property::Plot2DLegend_Text_Color:
    case PropertyItem::Property::Plot2DLegend_Stroke_Color:
    case PropertyItem::Property::Plot2DLegend_Background:
    case PropertyItem::Property::Plot2DLegend_Title_Color:
    case PropertyItem::Property::Plot2DAxis_Stroke_Color:
    case PropertyItem::Property::Plot2DAxis_Label_Color:
    case PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Color:
    case PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Color:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Color:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Color:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Color:
    case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Color:
    case PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Color:
    case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Color:
    case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Color:
    case PropertyItem::Property::Plot2DTextItem_Text_Color:
    case PropertyItem::Property::Plot2DTextItem_Stroke_Color:
    case PropertyItem::Property::Plot2DTextItem_Background_Color:
    case PropertyItem::Property::Plot2DLineItem_Stroke_Color:
    case PropertyItem::Property::Plot2DImageItem_Stroke_Color:
    case PropertyItem::Property::Plot2DCurve_Line_Stroke_Color:
    case PropertyItem::Property::Plot2DCurve_Fill_Color:
    case PropertyItem::Property::Plot2DCurve_Scatter_Outline_Color:
    case PropertyItem::Property::Plot2DCurve_Scatter_Fill_Color:
    case PropertyItem::Property::Plot2DLS_Line_Stroke_Color:
    case PropertyItem::Property::Plot2DLS_Fill_Color:
    case PropertyItem::Property::Plot2DLS_Scatter_Outline_Color:
    case PropertyItem::Property::Plot2DLS_Scatter_Fill_Color:
    case PropertyItem::Property::Plot2DChannel_Line1_Stroke_Color:
    case PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Color:
    case PropertyItem::Property::Plot2DChannel_Scatter1_Fill_Color:
    case PropertyItem::Property::Plot2DChannel_Fill_Color:
    case PropertyItem::Property::Plot2DChannel_Line2_Stroke_Color:
    case PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Color:
    case PropertyItem::Property::Plot2DChannel_Scatter2_Fill_Color:
    case PropertyItem::Property::Plot2DStatBox_Box_Stroke_Color:
    case PropertyItem::Property::Plot2DStatBox_Box_Fill_Color:
    case PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Color:
    case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Color:
    case PropertyItem::Property::Plot2DStatBox_Median_Stroke_Color:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Color:
    case PropertyItem::Property::Plot2DStatBox_Scatter_Fill_Color:
    case PropertyItem::Property::Plot2DVector_Line_Stroke_Color:
    case PropertyItem::Property::Plot2DBar_Line_Stroke_Color:
    case PropertyItem::Property::Plot2DBar_Fill_Color:
    case PropertyItem::Property::Plot2DPie_Stroke_Color:
    case PropertyItem::Property::Plot2DCM_Axis_Stroke_Color:
    case PropertyItem::Property::Plot2DCM_Axis_Label_Color:
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Color:
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Color:
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Color:
    case PropertyItem::Property::Plot2DErrBar_Stroke_Color:
    case PropertyItem::Property::Plot2DErrBar_Fill_Color:
      propertytype_ = PropertyItem::PropertyType::Color;
      break;
    case PropertyItem::Property::Plot2DLegend_Font:
    case PropertyItem::Property::Plot2DLegend_Title_Font:
    case PropertyItem::Property::Plot2DAxis_Label_Font:
    case PropertyItem::Property::Plot2DAxis_TickLabel_Font:
    case PropertyItem::Property::Plot2DTextItem_Text_Font:
    case PropertyItem::Property::Plot2DCM_Axis_Label_Font:
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Font:
      propertytype_ = PropertyItem::PropertyType::Font;
      break;
    case PropertyItem::Property::Plot2DCanvas_Dimension:
    case PropertyItem::Property::Plot3DCanvas_Dimension:
      propertytype_ = PropertyItem::PropertyType::Size;
      break;
    case PropertyItem::Property::BaseWindow_Geometry:
    case PropertyItem::Property::Plot2DLayout_Outer_Rect:
      propertytype_ = PropertyItem::PropertyType::Rect;
      break;
    case PropertyItem::Property::Plot2DLegend_Margin:
    case PropertyItem::Property::Plot2DTextItem_Margin:
      propertytype_ = PropertyItem::PropertyType::Margin;
      break;
    case PropertyItem::Property::Plot2DAxis_From_DateTime:
    case PropertyItem::Property::Plot2DAxis_To_DateTime:
      propertytype_ = PropertyItem::PropertyType::DateTime;
      break;
  }
}

PropertyItem *PropertyItem::create(PropertyItem *parent,
                                   ObjectBrowserTreeItem *item,
                                   const PropertyItem::Property &property,
                                   QList<QPair<QIcon, QString> > enumList) {
  auto pitem = new PropertyItem(parent, item, property);
  switch (pitem->propertytype_) {
    case PropertyItem::PropertyType::Enum:
      Q_ASSERT(!enumList.isEmpty());
      pitem->setEnumNames(enumList);
      break;
    case PropertyItem::PropertyType::Size:
    case PropertyItem::PropertyType::Rect:
    case PropertyItem::PropertyType::Margin:
      pitem->setReadonly(true);
      // fallthrough intended
    default:
      Q_ASSERT(enumList.isEmpty());
      break;
  }
  if (pitem->propertytype_ == PropertyItem::PropertyType::Enum) {
  }
  return pitem;
}

void PropertyItem::init() {}

PropertyItem::~PropertyItem() { qDeleteAll(childItems_); }

QWidget *PropertyItem::createEditor(QWidget *parent) const {
  switch (propertytype_) {
    case PropertyItem::PropertyType::Separator:
      return nullptr;
      break;
    case PropertyItem::PropertyType::String:
    case PropertyItem::PropertyType::Size:
    case PropertyItem::PropertyType::Rect:
    case PropertyItem::PropertyType::Margin: {
      StringWidget *le = new StringWidget(parent);
      le->setReadOnly(isreadonly());
      QObject::connect(le, &StringWidget::valueChanged, this,
                       &PropertyItem::setStringValue);
      return le;
    } break;
    case PropertyItem::PropertyType::Int: {
      IntWidget *sb = new IntWidget(parent);
      sb->setDisabled(isreadonly());
      QObject::connect(sb, &IntWidget::intValueChanged, this,
                       &PropertyItem::setIntValue);
      sb->setRange(lowerboundint_, upperboundint_);
      sb->setSingleStep(singlestepint_);
      sb->setPrefix(prefix_);
      sb->setSuffix(suffix_);
      return sb;
    } break;
    case PropertyItem::PropertyType::Double: {
      DoubleWidget *sb = new DoubleWidget(parent);
      sb->setDisabled(isreadonly());
      QObject::connect(sb, &DoubleWidget::doubleValueChanged, this,
                       &PropertyItem::setDoubleValue);
      sb->setRange(lowerbounddouble_, upperbounddouble_);
      sb->setSingleStep(singlestepdouble_);
      sb->setPrefix(prefix_);
      sb->setSuffix(suffix_);
      sb->setDecimals(precision_);
      return sb;
    } break;
    case PropertyItem::PropertyType::ScientificDouble: {
      ScientificDoubleSpinBox *sb = new ScientificDoubleSpinBox(parent);
      sb->setDisabled(isreadonly());
      QObject::connect(sb,
                       &ScientificDoubleSpinBox::scientificdoubleValueChanged,
                       this, &PropertyItem::setScientificDoubleValue);
      sb->setRange(lowerbounddouble_, upperbounddouble_);
      sb->setPrefix(prefix_);
      sb->setSuffix(suffix_);
      sb->setDecimals(precision_);
      return sb;
    } break;
    case PropertyItem::PropertyType::Bool: {
      QCheckBox *cb = new QCheckBox(parent);
      cb->setDisabled(isreadonly());
      QObject::connect(cb, &QCheckBox::clicked, this,
                       &PropertyItem::setBoolValue);
      return cb;
    } break;
    case PropertyItem::PropertyType::Enum: {
      QComboBox *cb = new QComboBox(parent);
      cb->setDisabled(isreadonly());
      foreach (auto ename, enumList_) {
        cb->addItem(ename.first, ename.second);
      }
      QObject::connect(cb, qOverload<int>(&QComboBox::currentIndexChanged),
                       this, &PropertyItem::setEnumValue);
      return cb;
    } break;
    case PropertyItem::PropertyType::Color: {
      ColorWidget *cb = new ColorWidget(parent);
      cb->setDisabled(isreadonly());
      QObject::connect(cb, &ColorWidget::valueChanged, this,
                       &PropertyItem::setColorValue);
      return cb;
    } break;
    case PropertyItem::PropertyType::Font: {
      FontWidget *cb = new FontWidget(parent);
      cb->setDisabled(isreadonly());
      QObject::connect(cb, &FontWidget::valueChanged, this,
                       &PropertyItem::setFontValue);
      return cb;
    } break;
    case PropertyItem::PropertyType::DateTime: {
      DateTimeWidget *dt = new DateTimeWidget(parent);
      dt->setDisabled(isreadonly());
      QObject::connect(dt, &DateTimeWidget::datetimeValueChanged, this,
                       &PropertyItem::setDateTimeValue);
      dt->setDisplayFormat(datetimeformat_);
      return dt;
    } break;
  }

  return nullptr;
}

void PropertyItem::setEditorData(QWidget *editor) const {
  QVariant dat = value().value;
  switch (propertytype_) {
    case PropertyItem::PropertyType::Separator:
      break;
    case PropertyItem::PropertyType::String:
    case PropertyItem::PropertyType::Size:
    case PropertyItem::PropertyType::Rect:
    case PropertyItem::PropertyType::Margin: {
      StringWidget *le = qobject_cast<StringWidget *>(editor);
      if (le) le->setText(dat.toString());
    } break;
    case PropertyItem::PropertyType::Int: {
      IntWidget *sb = qobject_cast<IntWidget *>(editor);
      int sd = dat.toInt();
      sb->setValue(sd);
    } break;
    case PropertyItem::PropertyType::Double: {
      DoubleWidget *sb = qobject_cast<DoubleWidget *>(editor);
      double sd = dat.toDouble();
      sb->setValue(sd);
    } break;
    case PropertyItem::PropertyType::ScientificDouble: {
      ScientificDoubleSpinBox *sb =
          qobject_cast<ScientificDoubleSpinBox *>(editor);
      double sd = dat.toDouble();
      sb->setValue(sd);
    } break;
    case PropertyItem::PropertyType::Bool: {
      QCheckBox *cb = qobject_cast<QCheckBox *>(editor);
      bool stus = dat.toBool();
      (stus) ? cb->setText(tr("True")) : cb->setText(tr("False"));
      connect(cb, &QCheckBox::toggled, [=](const bool status) {
        (status) ? cb->setText(tr("True")) : cb->setText(tr("False"));
      });
      cb->setChecked(stus);
    } break;
    case PropertyItem::PropertyType::Enum: {
      QComboBox *cb = qobject_cast<QComboBox *>(editor);
      int val = dat.toInt();
      cb->setCurrentIndex(val);
    } break;
    case PropertyItem::PropertyType::Color: {
      ColorWidget *cb = qobject_cast<ColorWidget *>(editor);
      QColor color = dat.value<QColor>();
      cb->setValue(color);
    } break;
    case PropertyItem::PropertyType::Font: {
      FontWidget *cb = qobject_cast<FontWidget *>(editor);
      QFont f = dat.value<QFont>();
      cb->setValue(f);
    } break;
    case PropertyItem::PropertyType::DateTime: {
      DateTimeWidget *dt = qobject_cast<DateTimeWidget *>(editor);
      QDateTime v = dat.value<QDateTime>();
      dt->setDateTime(v);
    } break;
  }
}

QVariant PropertyItem::editorData(QWidget *editor) const {
  switch (propertytype_) {
    case PropertyItem::PropertyType::Separator:
      return QVariant();
      break;
    case PropertyItem::PropertyType::String:
    case PropertyItem::PropertyType::Size:
    case PropertyItem::PropertyType::Rect:
    case PropertyItem::PropertyType::Margin: {
      StringWidget *le = qobject_cast<StringWidget *>(editor);
      if (le) return QVariant(le->text());
    } break;
    case PropertyItem::PropertyType::Int: {
      IntWidget *sb = qobject_cast<IntWidget *>(editor);
      if (sb) return QVariant(sb->value());
    } break;
    case PropertyItem::PropertyType::Double: {
      DoubleWidget *sb = qobject_cast<DoubleWidget *>(editor);
      if (sb) return QVariant(sb->value());
    } break;
    case PropertyItem::PropertyType::ScientificDouble: {
      ScientificDoubleSpinBox *sb =
          qobject_cast<ScientificDoubleSpinBox *>(editor);
      if (sb) return QVariant(sb->value());
    } break;
    case PropertyItem::PropertyType::Bool: {
      QCheckBox *cb = qobject_cast<QCheckBox *>(editor);
      return QVariant(cb->isChecked());
    } break;
    case PropertyItem::PropertyType::Enum: {
      QComboBox *cb = qobject_cast<QComboBox *>(editor);
      return QVariant(cb->currentIndex());
    } break;
    case PropertyItem::PropertyType::Color: {
      ColorWidget *cb = qobject_cast<ColorWidget *>(editor);
      if (cb) return QVariant(cb->color());
    } break;
    case PropertyItem::PropertyType::Font: {
      FontWidget *cb = qobject_cast<FontWidget *>(editor);
      if (cb) return QVariant(cb->font());
    } break;
    case PropertyItem::PropertyType::DateTime: {
      DateTimeWidget *dt = qobject_cast<DateTimeWidget *>(editor);
      if (dt) return QVariant(dt->dateTime());
    } break;
  }
  return QVariant();
}

void PropertyItem::appendChild(PropertyItem *child) {
  childItems_.append(child);
}

void PropertyItem::insertChild(int index, PropertyItem *child) {
  childItems_.insert(index, child);
}

void PropertyItem::removeChildren(int from, int to) {
  int count = to - from + 1;
  for (int i = 0; i < count; i++) {
    PropertyItem *child = childItems_.takeAt(from);
    delete child;
  }
}

void PropertyItem::setParent(PropertyItem *parent) { parentItem_ = parent; }

PropertyItem *PropertyItem::parent() const { return parentItem_; }

PropertyItem *PropertyItem::child(int row) { return childItems_.value(row); }

int PropertyItem::childCount() const { return childItems_.count(); }

int PropertyItem::columnCount() const {
  if (PropertyItem::propertytype_ == PropertyItem::PropertyType::Separator)
    return 1;
  else
    return 2;
}

Qt::ItemFlags PropertyItem::flags(int column) const {
  Qt::ItemFlags basicFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if (column == 1 && !isreadonly())
    return basicFlags | Qt::ItemIsEditable;
  else
    return basicFlags;
}

int PropertyItem::row() const {
  if (parentItem_)
    return parentItem_->childItems_.indexOf(const_cast<PropertyItem *>(this));

  return 0;
}

QString PropertyItem::toString() const {
  QVariant variant = value().value;
  if (!variant.isValid() || variant.isNull()) return QString();

  switch (propertytype_) {
    case PropertyItem::PropertyType::Separator:
      return QString();
      break;
    case PropertyItem::PropertyType::String: {
      return variant.value<QString>();
    } break;
    case PropertyItem::PropertyType::Int: {
      if (variant.canConvert(QVariant::Int))
        return QString("%1%2%3").arg(prefix_, variant.toString(), suffix_);
    } break;
    case PropertyItem::PropertyType::Double: {
      if (variant.canConvert(QVariant::Double))
        return QString("%1%2%3").arg(
            prefix_, QString::number(variant.toDouble(), 'f', precision_),
            suffix_);
    } break;
    case PropertyItem::PropertyType::ScientificDouble: {
      if (variant.canConvert(QVariant::Double))
        return QString("%1%2%3").arg(
            prefix_, QString::number(variant.toDouble(), 'g', precision_),
            suffix_);
    } break;
    case PropertyItem::PropertyType::Bool: {
      if (variant.canConvert(QVariant::Bool)) {
        bool stus = variant.toBool();
        if (stus)
          return QObject::tr("True");
        else
          return QObject::tr("False");
      }
    } break;
    case PropertyItem::PropertyType::Enum: {
      if (variant.canConvert(QVariant::Int))
        return enumList_.at(variant.toInt()).second;
    } break;
    case PropertyItem::PropertyType::Color: {
      if (variant.canConvert(QVariant::Color)) {
        QColor col = variant.value<QColor>();
        return QString("[%1, %2, %3] (%4)")
            .arg(QString::number(col.red()), QString::number(col.green()),
                 QString::number(col.blue()), QString::number(col.alpha()));
      }
    } break;
    case PropertyItem::PropertyType::Font: {
      if (variant.canConvert(QVariant::Font)) {
        QFont font = variant.value<QFont>();
        return QString("[%1, %2]")
            .arg(font.family(), QString::number(font.pointSize()));
      }
    } break;
    case PropertyItem::PropertyType::Size: {
      if (variant.canConvert(QVariant::Size)) {
        QSize size = variant.value<QSize>();
        return QString("%1 X %2").arg(QString::number(size.width()),
                                      QString::number(size.height()));
      }
    } break;
    case PropertyItem::PropertyType::Rect: {
      if (variant.canConvert(QVariant::Rect)) {
        QRect rect = variant.value<QRect>();
        return QString("[(%1, %2), %3 X %4]")
            .arg(QString::number(rect.x()), QString::number(rect.y()),
                 QString::number(rect.width()), QString::number(rect.height()));
      }
    } break;
    case PropertyItem::PropertyType::Margin: {
      if (variant.canConvert(QVariant::String)) return variant.value<QString>();
    } break;
    case PropertyItem::PropertyType::DateTime: {
      if (variant.canConvert(QVariant::DateTime)) {
        QDateTime dt = variant.value<QDateTime>();
        return dt.toString(datetimeformat_);
      }
    } break;
  }
  return QString();
}

PropertyItem::Pair PropertyItem::value() const {
  bool status = false;
  switch (item_->dataType()) {
    case ObjectBrowserTreeItem::ObjectType::None: {
      switch (property_) {
        case PropertyItem::Property::None:
          return Pair("", QVariant());
          break;
        default:
          break;
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::BaseWindow: {
      MyWidget *widget = item_->getObjectTreeItem<MyWidget>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::BaseWindow_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::BaseWindow_Name:
          return Pair(tr("Name"), widget->name());
          break;
        case PropertyItem::Property::BaseWindow_Label:
          return Pair(tr("Label"), widget->windowLabel());
          break;
        case PropertyItem::Property::BaseWindow_Geometry:
          return Pair(tr("Geometry"), widget->geometry());
          break;
        case PropertyItem::Property::BaseWindow_Geometry_X:
          return Pair(tr("X"), widget->geometry().x());
          break;
        case PropertyItem::Property::BaseWindow_Geometry_Y:
          return Pair(tr("Y"), widget->geometry().y());
          break;
        case PropertyItem::Property::BaseWindow_Geometry_Width:
          return Pair(tr("Width"), widget->geometry().width());
          break;
        case PropertyItem::Property::BaseWindow_Geometry_Height:
          return Pair(tr("Height"), widget->geometry().height());
          break;
        default:
          break;
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::TableDimension: {
      Table *table = item_->getObjectTreeItem<Table>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::TableWindow_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::TableWindow_Row_Count:
          return Pair(tr("Row Count"), table->numRows());
          break;
        case PropertyItem::Property::TableWindow_Column_Count:
          return Pair(tr("Column Count"), table->numCols());
          break;
        default:
          break;
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::MatrixDimension: {
      Matrix *matrix = item_->getObjectTreeItem<Matrix>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::MatrixWindow_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::MatrixWindow_Row_Count:
          return Pair(tr("Row Count"), matrix->numRows());
          break;
        case PropertyItem::Property::MatrixWindow_Column_Count:
          return Pair(tr("Column Count"), matrix->numCols());
          break;
        default:
          break;
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::TableWindow:
    case ObjectBrowserTreeItem::ObjectType::MatrixWindow:
    case ObjectBrowserTreeItem::ObjectType::NoteWindow:
    case ObjectBrowserTreeItem::ObjectType::Plot2DWindow:
    case ObjectBrowserTreeItem::ObjectType::Plot3DWindow:
      break;
    // Plot2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DCanvas: {
      Plot2D *plot = item_->getObjectTreeItem<Plot2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DCanvas_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DCanvas_Dimension:
          return Pair(tr("Dimension"), QSize(plot->width(), plot->height()));
          break;
        case PropertyItem::Property::Plot2DCanvas_Dimension_Width:
          return Pair(tr("Width"), plot->width());
          break;
        case PropertyItem::Property::Plot2DCanvas_Dimension_Height:
          return Pair(tr("Height"), plot->height());
          break;
        case PropertyItem::Property::Plot2DCanvas_Background:
          return Pair(tr("Background Color"), plot->getBackgroundColor());
          break;
        case PropertyItem::Property::Plot2DCanvas_DPR:
          return Pair(tr("Device Pixel Ratio"), plot->bufferDevicePixelRatio());
          break;
        case PropertyItem::Property::Plot2DCanvas_OpenGL:
          return Pair(tr("OpenGL"), plot->openGl());
          break;
        case PropertyItem::Property::Plot2DCanvas_Row_Spacing: {
          Layout2D *layout =
              item_->parentItem()->getObjectTreeItem<Layout2D>(&status);
          if (status)
            return Pair(tr("Row Spacing"),
                        layout->getLayoutGrid()->rowSpacing());
        } break;
        case PropertyItem::Property::Plot2DCanvas_Column_Spacing: {
          Layout2D *layout =
              item_->parentItem()->getObjectTreeItem<Layout2D>(&status);
          if (status)
            return Pair(tr("Column Spacing"),
                        layout->getLayoutGrid()->columnSpacing());
        } break;
        default:
          break;
      }
    } break;
    // AxisRect2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DLayout: {
      AxisRect2D *axisrect = item_->getObjectTreeItem<AxisRect2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DLayout_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DLayout_Background:
          return Pair(tr("Background Color"),
                      axisrect->backgroundBrush().color());
          break;
        case PropertyItem::Property::Plot2DLayout_FillStyle:
          return Pair(tr("Fill Style"),
                      static_cast<int>(axisrect->backgroundBrush().style()));
          break;
        case PropertyItem::Property::Plot2DLayout_Row_Stretch_Factor: {
          Layout2D *layout =
              item_->parentItem()->getObjectTreeItem<Layout2D>(&status);
          if (status) {
            QPair<int, int> rowcol = layout->getAxisRectRowCol(axisrect);
            return Pair(
                tr("Row Stretch Factor"),
                layout->getLayoutGrid()->rowStretchFactors().at(rowcol.first));
          }
        } break;
        case PropertyItem::Property::Plot2DLayout_Column_Stretch_Factor: {
          Layout2D *layout =
              item_->parentItem()->getObjectTreeItem<Layout2D>(&status);
          if (status) {
            QPair<int, int> rowcol = layout->getAxisRectRowCol(axisrect);
            return Pair(tr("Column Stretch Factor"),
                        layout->getLayoutGrid()->columnStretchFactors().at(
                            rowcol.second));
          }
        } break;
        case PropertyItem::Property::Plot2DLayout_Outer_Rect:
          return Pair(tr("Outer Rect"), axisrect->outerRect());
          break;
        case PropertyItem::Property::Plot2DLayout_Outer_Rect_X:
          return Pair(tr("X"), axisrect->outerRect().x());
          break;
        case PropertyItem::Property::Plot2DLayout_Outer_Rect_Y:
          return Pair(tr("Y"), axisrect->outerRect().y());
          break;
        case PropertyItem::Property::Plot2DLayout_Outer_Rect_Width:
          return Pair(tr("Width"), axisrect->outerRect().width());
          break;
        case PropertyItem::Property::Plot2DLayout_Outer_Rect_Height:
          return Pair(tr("Height"), axisrect->outerRect().height());
          break;
        case PropertyItem::Property::Plot2DLayout_Margin:
          return Pair(tr("Auto Margin"), axisrect->getAutoMarginsBool());
          break;
        case PropertyItem::Property::Plot2DLayout_Margin_Left:
          return Pair(tr("Left"), axisrect->margins().left());
          break;
        case PropertyItem::Property::Plot2DLayout_Margin_Right:
          return Pair(tr("Right"), axisrect->margins().right());
          break;
        case PropertyItem::Property::Plot2DLayout_Margin_Top:
          return Pair(tr("Top"), axisrect->margins().top());
          break;
        case PropertyItem::Property::Plot2DLayout_Margin_Bottom:
          return Pair(tr("Bottom"), axisrect->margins().bottom());
          break;
        default:
          break;
      }
    } break;
    // Legend2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DLegend: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DLegend_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DLegend_X:
          return Pair(tr("Position X"), legend->getposition_legend().x());
          break;
        case PropertyItem::Property::Plot2DLegend_Y:
          return Pair(tr("Position Y"), legend->getposition_legend().y());
          break;
        case PropertyItem::Property::Plot2DLegend_Visible:
          return Pair(tr("Visible"), legend->gethidden_legend());
          break;
        case PropertyItem::Property::Plot2DLegend_Direction:
          return Pair(tr("Direction"), legend->getdirection_legend());
          break;
        case PropertyItem::Property::Plot2DLegend_Margin: {
          QString string = QString("%1, %2, %3, %4")
                               .arg(QString::number(legend->margins().left()),
                                    QString::number(legend->margins().bottom()),
                                    QString::number(legend->margins().right()),
                                    QString::number(legend->margins().top()));
          return Pair(tr("Margin"), string);
        } break;
        case PropertyItem::Property::Plot2DLegend_Margin_Left:
          return Pair(tr("Left"), legend->margins().left());
          break;
        case PropertyItem::Property::Plot2DLegend_Margin_Bottom:
          return Pair(tr("Bottom"), legend->margins().bottom());
          break;
        case PropertyItem::Property::Plot2DLegend_Margin_Right:
          return Pair(tr("Right"), legend->margins().right());
          break;
        case PropertyItem::Property::Plot2DLegend_Margin_Top:
          return Pair(tr("Top"), legend->margins().top());
          break;
        case PropertyItem::Property::Plot2DLegend_Text_Separator:
          return Pair(tr("Text"), QString());
          break;
        case PropertyItem::Property::Plot2DLegend_Font:
          return Pair(tr("Font"), legend->font());
          break;
        case PropertyItem::Property::Plot2DLegend_Text_Color:
          return Pair(tr("Text Color"), legend->textColor());
          break;
        case PropertyItem::Property::Plot2DLegend_Icon_Separator:
          return Pair(tr("Icon"), QString());
          break;
        case PropertyItem::Property::Plot2DLegend_Icon_Width:
          return Pair(tr("Icon Width"), legend->iconSize().rwidth());
          break;
        case PropertyItem::Property::Plot2DLegend_Icon_Height:
          return Pair(tr("Icon Height"), legend->iconSize().rheight());
          break;
        case PropertyItem::Property::Plot2DLegend_Icon_Padding:
          return Pair(tr("Icon Padding"), legend->iconTextPadding());
          break;
        case PropertyItem::Property::Plot2DLegend_Stroke_Fill_Separator:
          return Pair(tr("Stroke & Fill"), QString());
          break;
        case PropertyItem::Property::Plot2DLegend_Stroke_Color:
          return Pair(tr("Stroke Color"),
                      legend->getborderstrokecolor_legend());
          break;
        case PropertyItem::Property::Plot2DLegend_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"),
                      legend->getborderstrokethickness_legend());
          break;
        case PropertyItem::Property::Plot2DLegend_Stroke_Style:
          return Pair(tr("Stroke Style"),
                      static_cast<int>(legend->getborderstrokestyle_legend()));
          break;
        case PropertyItem::Property::Plot2DLegend_Background:
          return Pair(tr("Background Color"), legend->brush().color());
          break;
        case PropertyItem::Property::Plot2DLegend_FillStyle:
          return Pair(tr("Fill Style"),
                      static_cast<int>(legend->brush().style()));
          break;
        case PropertyItem::Property::Plot2DLegend_Title_Separator:
          return Pair(tr("Title"), QString());
          break;
        case PropertyItem::Property::Plot2DLegend_Title_Visible:
          return Pair(tr("Title"), legend->istitle_legend());
          break;
        case PropertyItem::Property::Plot2DLegend_Title_Text:
          return Pair(tr("Text"),
                      Utilities::joinstring(legend->titletext_legend()));
          break;
        case PropertyItem::Property::Plot2DLegend_Title_Font:
          return Pair(tr("Font"), legend->titlefont_legend());
          break;
        case PropertyItem::Property::Plot2DLegend_Title_Color:
          return Pair(tr("Color"), legend->titlecolor_legend());
          break;
        default:
          break;
      }
    } break;
    // Axis2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DAxis: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DAxis_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DAxis_Visible:
          return Pair(tr("Visible"), axis->getshowhide_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Offset:
          return Pair(tr("Offset"), axis->getoffset_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_From_Double:
        case PropertyItem::Property::Plot2DAxis_From_Text:
          return Pair(tr("From"), axis->getfrom_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_To_Double:
        case PropertyItem::Property::Plot2DAxis_To_Text:
          return Pair(tr("To"), axis->getto_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_From_DateTime:
          return Pair(tr("From"), QCPAxisTickerDateTime::keyToDateTime(
                                      axis->getfrom_axis()));
          break;
        case PropertyItem::Property::Plot2DAxis_To_DateTime:
          return Pair(tr("To"),
                      QCPAxisTickerDateTime::keyToDateTime(axis->getto_axis()));
          break;
        case PropertyItem::Property::Plot2DAxis_Ending_Separator:
          return Pair(tr("Ending"), QString());
          break;
        case PropertyItem::Property::Plot2DAxis_Upper_Ending:
          return Pair(tr("Upper Ending"),
                      static_cast<int>(axis->upperEnding().style()));
          break;
        case PropertyItem::Property::Plot2DAxis_Lower_Ending:
          return Pair(tr("Lower Ending"),
                      static_cast<int>(axis->lowerEnding().style()));
          break;
        case PropertyItem::Property::Plot2DAxis_Type:
          return Pair(tr("Type"), static_cast<int>(axis->getscaletype_axis()));
          break;
        case PropertyItem::Property::Plot2DAxis_Inverted:
          return Pair(tr("Inverted"), axis->getinverted_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Stroke_Color:
          return Pair(tr("Stroke Color"), axis->getstrokecolor_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"), axis->getstrokethickness_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(axis->getstroketype_axis()));
          break;
        case PropertyItem::Property::Plot2DAxis_Antialiased:
          return Pair(tr("Antialiased"), axis->getantialiased_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Label_Separator:
          return Pair(tr("Label"), QString());
          break;
        case PropertyItem::Property::Plot2DAxis_Label_Text:
          return Pair(tr("Label Text"),
                      Utilities::joinstring(axis->getlabeltext_axis()));
          break;
        case PropertyItem::Property::Plot2DAxis_Label_Font:
          return Pair(tr("Label Font"), axis->getlabelfont_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Label_Color:
          return Pair(tr("Label Color"), axis->getlabelcolor_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Label_Padding:
          return Pair(tr("Label Padding"), axis->getlabelpadding_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Ticks_Separator:
          return Pair(tr("Tick"), QString());
          break;
        case PropertyItem::Property::Plot2DAxis_Ticks_Visible:
          return Pair(tr("Visible"), axis->gettickvisibility_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Ticks_Count:
          return Pair(tr("Count"), axis->gettickscount_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Ticks_Origin:
          return Pair(tr("Origin"), axis->getticksorigin());
          break;
        case PropertyItem::Property::Plot2DAxis_Ticks_LengthIn:
          return Pair(tr("Length In"), axis->getticklengthin_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Ticks_LengthOut:
          return Pair(tr("Length Out"), axis->getticklengthout_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Color:
          return Pair(tr("Stroke Color"), axis->gettickstrokecolor_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"),
                      axis->gettickstrokethickness_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(axis->gettickstrokestyle_axis()));
          break;
        case PropertyItem::Property::Plot2DAxis_SubTicks_Separator:
          return Pair(tr("Sub-Tick"), QString());
          break;
        case PropertyItem::Property::Plot2DAxis_SubTicks_Visible:
          return Pair(tr("Visible"), axis->getsubtickvisibility_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_SubTicks_LengthIn:
          return Pair(tr("Length In"), axis->getsubticklengthin_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_SubTicks_LengthOut:
          return Pair(tr("Length Out"), axis->getsubticklengthout_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Color:
          return Pair(tr("Stroke Color"), axis->getsubtickstrokecolor_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"),
                      axis->getsubtickstrokethickness_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(axis->getsubtickstrokestyle_axis()));
          break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Separator:
          return Pair(tr("Tick Label"), QString());
          break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Visible:
          return Pair(tr("Visible"), axis->getticklabelvisibility_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_StepStrategy:
          return Pair(
              tr("Step Strategy"),
              static_cast<int>(axis->getticker_axis()->tickStepStrategy()));
          break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Font:
          return Pair(tr("Font"), axis->getticklabelfont_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Color:
          return Pair(tr("Color"), axis->getticklabelcolor_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Padding:
          return Pair(tr("Padding"), axis->getticklabelpadding_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Rotation:
          return Pair(tr("Rotation"), axis->getticklabelrotation_axis());
          break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Side:
          return Pair(tr("Side"),
                      static_cast<int>(axis->getticklabelside_axis()));
          break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_LogBase_Log: {
          if (axis->gettickertype_axis() == Axis2D::TickerType::Log) {
            QSharedPointer<QCPAxisTickerLog> logticker =
                qSharedPointerCast<QCPAxisTickerLog>(axis->getticker_axis());
            if (logticker) return Pair(tr("Log Base"), logticker->logBase());
          }
        } break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Symbol_Pi: {
          if (axis->gettickertype_axis() == Axis2D::TickerType::Pi) {
            QSharedPointer<QCPAxisTickerPi> piticker =
                qSharedPointerCast<QCPAxisTickerPi>(axis->getticker_axis());
            if (piticker)
              return Pair(tr("Symbol"),
                          Utilities::joinstring(piticker->piSymbol()));
          }
        } break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Symbol_Value_Pi: {
          if (axis->gettickertype_axis() == Axis2D::TickerType::Pi) {
            QSharedPointer<QCPAxisTickerPi> piticker =
                qSharedPointerCast<QCPAxisTickerPi>(axis->getticker_axis());
            if (piticker) return Pair(tr("Symbol Value"), piticker->piValue());
          }
        } break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_FractionStyle_Pi: {
          if (axis->gettickertype_axis() == Axis2D::TickerType::Pi) {
            QSharedPointer<QCPAxisTickerPi> piticker =
                qSharedPointerCast<QCPAxisTickerPi>(axis->getticker_axis());
            if (piticker)
              return Pair(tr("Fraction Style"),
                          static_cast<int>(piticker->fractionStyle()));
          }
        } break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Format_Double: {
          if ((axis->gettickertype_axis() == Axis2D::TickerType::Value ||
               axis->gettickertype_axis() == Axis2D::TickerType::Log ||
               axis->gettickertype_axis() == Axis2D::TickerType::Pi))
            return Pair(tr("Format"),
                        static_cast<int>(axis->getticklabelformat_axis()));
        } break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Format_DateTime: {
          if (axis->gettickertype_axis() == Axis2D::TickerType::DateTime) {
            QSharedPointer<QCPAxisTickerDateTime> dtticker =
                qSharedPointerCast<QCPAxisTickerDateTime>(
                    axis->getticker_axis());
            if (dtticker)
              return Pair(tr("Format"),
                          Utilities::joinstring(dtticker->dateTimeFormat()));
          }
        } break;
        case PropertyItem::Property::Plot2DAxis_TickLabel_Precision_Double: {
          if ((axis->gettickertype_axis() == Axis2D::TickerType::Value ||
               axis->gettickertype_axis() == Axis2D::TickerType::Log ||
               axis->gettickertype_axis() == Axis2D::TickerType::Pi))
            return Pair(tr("Precision"),
                        static_cast<int>(axis->getticklabelprecision_axis()));
        } break;
        default:
          break;
      }
    } break;
    // GridPair2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DGrid: {
      GridPair2D *gpair = item_->getObjectTreeItem<GridPair2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DGrid_Horizontal_Separator: {
          if (gpair->getXgridAxis())
            return Pair(tr("Horizontal Grid Axis"), QString());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal: {
          if (gpair->getXgridAxis())
            return Pair(tr("Horizontal Axis"),
                        gpair->getXgridAxis()->getnumber_axis() - 1);
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Separator: {
          if (gpair->getXgridAxis())
            return Pair(tr("Horizontal Major Grid"), QString());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Major: {
          if (gpair->getXgridAxis())
            return Pair(tr("Visible"),
                        gpair->getXgrid()->getMajorGridVisible());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Color: {
          if (gpair->getXgridAxis())
            return Pair(tr("Stroke Color"),
                        gpair->getXgrid()->getMajorGridColor());
        } break;
        case PropertyItem::Property::
            Plot2DGrid_Horizontal_Major_Stroke_Thickness: {
          if (gpair->getXgridAxis())
            return Pair(tr("Stroke Thickness"),
                        gpair->getXgrid()->getMajorGridwidth());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Style: {
          if (gpair->getXgridAxis())
            return Pair(
                tr("Stroke Type"),
                static_cast<int>(gpair->getXgrid()->getMajorGridStyle()));
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Separator: {
          if (gpair->getXgridAxis())
            return Pair(tr("Horizontal Zero Line"), QString());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Line: {
          if (gpair->getXgridAxis())
            return Pair(tr("Visible"),
                        gpair->getXgrid()->getZerothLineVisible());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Color: {
          if (gpair->getXgridAxis())
            return Pair(tr("Zero Color"),
                        gpair->getXgrid()->getZerothLineColor());
        } break;
        case PropertyItem::Property::
            Plot2DGrid_Horizontal_Zero_Stroke_Thickness: {
          if (gpair->getXgridAxis())
            return Pair(tr("Zero Thickness"),
                        gpair->getXgrid()->getZeroLinewidth());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Style: {
          if (gpair->getXgridAxis())
            return Pair(
                tr("Zero Style"),
                static_cast<int>(gpair->getXgrid()->getZeroLineStyle()));
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Separator: {
          if (gpair->getXgridAxis())
            return Pair(tr("Horizontal Minor Grid"), QString());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Minor: {
          if (gpair->getXgridAxis())
            return Pair(tr("Visible"),
                        gpair->getXgrid()->getMinorGridVisible());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Color: {
          if (gpair->getXgridAxis())
            return Pair(tr("Stroke Color"),
                        gpair->getXgrid()->getMinorGridColor());
        } break;
        case PropertyItem::Property::
            Plot2DGrid_Horizontal_Minor_Stroke_Thickness: {
          if (gpair->getXgridAxis())
            return Pair(tr("Stroke Thickness"),
                        gpair->getXgrid()->getMinorGridwidth());
        } break;
        case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Style: {
          if (gpair->getXgridAxis())
            return Pair(
                tr("Stroke Type"),
                static_cast<int>(gpair->getXgrid()->getMinorGridStyle()));
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Separator: {
          if (gpair->getYgridAxis())
            return Pair(tr("Vertical Grid Axis"), QString());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical: {
          if (gpair->getYgridAxis())
            return Pair(tr("Vertical Axis"),
                        gpair->getYgridAxis()->getnumber_axis() - 1);
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Major_Separator: {
          if (gpair->getYgridAxis())
            return Pair(tr("Vertical Major Grid"), QString());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Major: {
          if (gpair->getYgridAxis())
            return Pair(tr("Visible"),
                        gpair->getYgrid()->getMajorGridVisible());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Color: {
          if (gpair->getYgridAxis())
            return Pair(tr("Stroke Color"),
                        gpair->getYgrid()->getMajorGridColor());
        } break;
        case PropertyItem::Property::
            Plot2DGrid_Vertical_Major_Stroke_Thickness: {
          if (gpair->getYgridAxis())
            return Pair(tr("Stroke Thickness"),
                        gpair->getYgrid()->getMajorGridwidth());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Style: {
          if (gpair->getYgridAxis())
            return Pair(
                tr("Stroke Type"),
                static_cast<int>(gpair->getYgrid()->getMajorGridStyle()));
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Separator: {
          if (gpair->getYgridAxis())
            return Pair(tr("Vertical Zero Line"), QString());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Line: {
          if (gpair->getYgridAxis())
            return Pair(tr("Visible"),
                        gpair->getYgrid()->getZerothLineVisible());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Color: {
          if (gpair->getYgridAxis())
            return Pair(tr("Zero Color"),
                        gpair->getYgrid()->getZerothLineColor());
        } break;
        case PropertyItem::Property::
            Plot2DGrid_Vertical_Zero_Stroke_Thickness: {
          if (gpair->getYgridAxis())
            return Pair(tr("Zero Thickness"),
                        gpair->getYgrid()->getZeroLinewidth());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Style: {
          if (gpair->getYgridAxis())
            return Pair(
                tr("Zero Style"),
                static_cast<int>(gpair->getYgrid()->getZeroLineStyle()));
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Separator: {
          if (gpair->getYgridAxis())
            return Pair(tr("Vertical Minor Grid"), QString());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Minor: {
          if (gpair->getYgridAxis())
            return Pair(tr("Visible"),
                        gpair->getYgrid()->getMinorGridVisible());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Color: {
          if (gpair->getYgridAxis())
            return Pair(tr("Stroke Color"),
                        gpair->getYgrid()->getMinorGridColor());
        } break;
        case PropertyItem::Property::
            Plot2DGrid_Vertical_Minor_Stroke_Thickness: {
          if (gpair->getYgridAxis())
            return Pair(tr("Stroke Thickness"),
                        gpair->getYgrid()->getMinorGridwidth());
        } break;
        case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Style: {
          if (gpair->getYgridAxis())
            return Pair(
                tr("Stroke Type"),
                static_cast<int>(gpair->getYgrid()->getMinorGridStyle()));
        } break;
        default:
          break;
      }
    } break;
    // TextItem2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DTextItem: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DTextItem_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DTextItem_Position_X:
          return Pair(tr("Pixel Position X"),
                      textitem->position->pixelPosition().x());
          break;
        case PropertyItem::Property::Plot2DTextItem_Position_Y:
          return Pair(tr("Pixel Position Y"),
                      textitem->position->pixelPosition().y());
          break;
        case PropertyItem::Property::Plot2DTextItem_Margin: {
          QString string =
              QString("%1, %2, %3, %4")
                  .arg(QString::number(textitem->padding().left()),
                       QString::number(textitem->padding().bottom()),
                       QString::number(textitem->padding().right()),
                       QString::number(textitem->padding().top()));
          return Pair(tr("Padding"), string);
        } break;
        case PropertyItem::Property::Plot2DTextItem_Margin_Left:
          return Pair(tr("Left"), textitem->padding().left());
          break;
        case PropertyItem::Property::Plot2DTextItem_Margin_Bottom:
          return Pair(tr("Bottom"), textitem->padding().bottom());
          break;
        case PropertyItem::Property::Plot2DTextItem_Margin_Right:
          return Pair(tr("Right"), textitem->padding().right());
          break;
        case PropertyItem::Property::Plot2DTextItem_Margin_Top:
          return Pair(tr("Top"), textitem->padding().top());
          break;
        case PropertyItem::Property::Plot2DTextItem_Antialiased:
          return Pair(tr("Antialiased"), textitem->antialiased());
          break;
        case PropertyItem::Property::Plot2DTextItem_Text_Separator:
          return Pair(tr("Text"), QString());
          break;
        case PropertyItem::Property::Plot2DTextItem_Text_Alignment:
          return Pair(tr("Text Alignment"),
                      static_cast<int>(textitem->gettextalignment_textitem()));
          break;
        case PropertyItem::Property::Plot2DTextItem_Text:
          return Pair(tr("Text"), Utilities::joinstring(textitem->text()));
          break;
        case PropertyItem::Property::Plot2DTextItem_Text_Rotation:
          return Pair(tr("Rotation"), textitem->rotation());
          break;
        case PropertyItem::Property::Plot2DTextItem_Text_Font:
          return Pair(tr("Text Font"), textitem->font());
          break;
        case PropertyItem::Property::Plot2DTextItem_Text_Color:
          return Pair(tr("Text Color"), textitem->color());
          break;
        case PropertyItem::Property::Plot2DTextItem_Stroke_Color:
          return Pair(tr("Stroke Color"), textitem->getstrokecolor_textitem());
          break;
        case PropertyItem::Property::Plot2DTextItem_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"),
                      textitem->getstrokethickness_textitem());
          break;
        case PropertyItem::Property::Plot2DTextItem_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(textitem->getstrokestyle_textitem()));
          break;
        case PropertyItem::Property::Plot2DTextItem_Background_Color:
          return Pair(tr("Background Color"), textitem->brush().color());
          break;
        case PropertyItem::Property::Plot2DTextItem_Fill_Style:
          return Pair(tr("Fill Style"),
                      static_cast<int>(textitem->brush().style()));
          break;
        default:
          break;
      }
    } break;
    // LineItem2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DLineItem: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DLineItem_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DLineItem_Position_X1:
          return Pair(tr("Pixel Position X1"),
                      lineitem->position("start")->pixelPosition().x());
          break;
        case PropertyItem::Property::Plot2DLineItem_Position_Y1:
          return Pair(tr("Pixel Position Y1"),
                      lineitem->position("start")->pixelPosition().y());
          break;
        case PropertyItem::Property::Plot2DLineItem_Position_X2:
          return Pair(tr("Pixel Position X2"),
                      lineitem->position("end")->pixelPosition().x());
          break;
        case PropertyItem::Property::Plot2DLineItem_Position_Y2:
          return Pair(tr("Pixel Position Y2"),
                      lineitem->position("end")->pixelPosition().y());
          break;
        case PropertyItem::Property::Plot2DLineItem_Antialiased:
          return Pair(tr("Antialiased"), lineitem->antialiased());
          break;
        case PropertyItem::Property::Plot2DLineItem_Stroke_Color:
          return Pair(tr("Stroke Color"), lineitem->getstrokecolor_lineitem());
          break;
        case PropertyItem::Property::Plot2DLineItem_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"),
                      lineitem->getstrokethickness_lineitem());
          break;
        case PropertyItem::Property::Plot2DLineItem_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(lineitem->getstrokestyle_lineitem()));
          break;
        case PropertyItem::Property::Plot2DLineItem_End_Separator:
          return Pair(tr("End"), QString());
          break;
        case PropertyItem::Property::Plot2DLineItem_Starting_Style:
          return Pair(tr("Starting Style"),
                      static_cast<int>(lineitem->getendstyle_lineitem(
                          LineItem2D::LineEndLocation::Start)));
          break;
        case PropertyItem::Property::Plot2DLineItem_Starting_Width:
          return Pair(tr("Starting Width"),
                      lineitem->getendwidth_lineitem(
                          LineItem2D::LineEndLocation::Start));
          break;
        case PropertyItem::Property::Plot2DLineItem_Starting_Height:
          return Pair(tr("Starting Length"),
                      lineitem->getendlength_lineitem(
                          LineItem2D::LineEndLocation::Start));
          break;
        case PropertyItem::Property::Plot2DLineItem_Ending_Style:
          return Pair(tr("Ending Style"),
                      static_cast<int>(lineitem->getendstyle_lineitem(
                          LineItem2D::LineEndLocation::Stop)));
          break;
        case PropertyItem::Property::Plot2DLineItem_Ending_Width:
          return Pair(tr("Ending Width"),
                      lineitem->getendwidth_lineitem(
                          LineItem2D::LineEndLocation::Stop));
          break;
        case PropertyItem::Property::Plot2DLineItem_Ending_Height:
          return Pair(tr("Ending Length"),
                      lineitem->getendlength_lineitem(
                          LineItem2D::LineEndLocation::Stop));
          break;
        default:
          break;
      }
    } break;
    // ImageItem2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DImageItem: {
      ImageItem2D *imageitem = item_->getObjectTreeItem<ImageItem2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DImageItem_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DImageItem_Position_X:
          return Pair(tr("Pixel Position X"),
                      imageitem->position("topLeft")->pixelPosition().x());
          break;
        case PropertyItem::Property::Plot2DImageItem_Position_Y:
          if (status)
            return Pair(tr("Pixel Position Y"),
                        imageitem->position("topLeft")->pixelPosition().y());
          break;
        case PropertyItem::Property::Plot2DImageItem_Source:
          return Pair(tr("Source"), imageitem->getsource_imageitem());
          break;
        case PropertyItem::Property::Plot2DImageItem_Rotation:
          return Pair(tr("Rotation"), imageitem->getrotation_imageitem());
          break;
        case PropertyItem::Property::Plot2DImageItem_Stroke_Color:
          if (status)
            return Pair(tr("Stroke Color"),
                        imageitem->getstrokecolor_imageitem());
          break;
        case PropertyItem::Property::Plot2DImageItem_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"),
                      imageitem->getstrokethickness_imageitem());
          break;
        case PropertyItem::Property::Plot2DImageItem_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(imageitem->getstrokestyle_imageitem()));
          break;
        default:
          break;
      }
    } break;
    // Curve2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DCurve: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DCurve_Axis_Separator:
          return Pair(tr("Axes"), QString());
          break;
        case PropertyItem::Property::Plot2DCurve_Axis_X:
          return Pair(
              tr("X Axis"),
              static_cast<int>(curve->getxaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DCurve_Axis_Y:
          return Pair(
              tr("Y Axis"),
              static_cast<int>(curve->getyaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DCurve_Line_Separator:
          return Pair(tr("Line"), QString());
          break;
        case PropertyItem::Property::Plot2DCurve_Line_Antialiased:
          return Pair(tr("Line Antialiased"),
                      curve->getlineantialiased_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Line_Style:
          return Pair(tr("Line Style"),
                      static_cast<int>(curve->getlinetype_cplot()));
          break;
        case PropertyItem::Property::Plot2DCurve_Line_Stroke_Color:
          return Pair(tr("Line Stroke Color"),
                      curve->getlinestrokecolor_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Line_Stroke_Thickness:
          return Pair(tr("Line Stroke Thickness"),
                      curve->getlinestrokethickness_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Line_Stroke_Style:
          return Pair(tr("Line Stroke Type"),
                      static_cast<int>(curve->getlinestrokestyle_cplot()));
          break;
        case PropertyItem::Property::Plot2DCurve_Areafill_Separator:
          return Pair(tr("Area Fill"), QString());
          break;
        case PropertyItem::Property::Plot2DCurve_Fill_Area:
          return Pair(tr("Fill Under Area"), curve->getlinefillstatus_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Fill_Color:
          return Pair(tr("Area Fill Color"), curve->getlinefillcolor_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Fill_Style:
          return Pair(tr("Area Fill Style"),
                      static_cast<int>(curve->getlinefillstyle_cplot()));
          break;
        case PropertyItem::Property::Plot2DCurve_Scatter_Separator:
          return Pair(tr("Scatter"), QString());
          break;
        case PropertyItem::Property::Plot2DCurve_Scatter_Antialiased:
          return Pair(tr("Scatter Antialiased"),
                      curve->getscatterantialiased_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Scatter_Style:
          return Pair(tr("Scatter Style"),
                      static_cast<int>(curve->getscattershape_cplot()));
          break;
        case PropertyItem::Property::Plot2DCurve_Scatter_Size:
          return Pair(tr("Scatter Size"), curve->getscattersize_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Scatter_Outline_Color:
          return Pair(tr("Scatter Stroke Color"),
                      curve->getscatterstrokecolor_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Scatter_Outline_Thickness:
          return Pair(tr("Scatter Stroke Thickness"),
                      curve->getscatterstrokethickness_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Scatter_Outline_Style:
          return Pair(tr("Scatter Stroke Type"),
                      static_cast<int>(curve->getscatterstrokestyle_cplot()));
          break;
        case PropertyItem::Property::Plot2DCurve_Scatter_Fill_Color:
          return Pair(tr("Scatter Fill Color"),
                      curve->getscatterfillcolor_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Scatter_Fill_Style:
          return Pair(tr("Scatter Fill Type"),
                      static_cast<int>(curve->getscatterfillstyle_cplot()));
          break;
        case PropertyItem::Property::Plot2DCurve_Legend_Separator:
          return Pair(tr("Plot Legend"), QString());
          break;
        case PropertyItem::Property::Plot2DCurve_Legend_Status:
          return Pair(tr("Visible"), curve->getlegendvisible_cplot());
          break;
        case PropertyItem::Property::Plot2DCurve_Legend_Text:
          return Pair(tr("Text"),
                      Utilities::joinstring(curve->getlegendtext_cplot()));
          break;
        default:
          break;
      }
    } break;
    // Graph2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DLSGraph: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DLS_Axis_Separator:
          return Pair(tr("Axes"), QString());
          break;
        case PropertyItem::Property::Plot2DLS_Axis_X:
          return Pair(tr("X Axis"),
                      static_cast<int>(ls->getxaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DLS_Axis_Y:
          return Pair(tr("Y Axis"),
                      static_cast<int>(ls->getyaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DLS_Line_Separator:
          return Pair(tr("Line"), QString());
          break;
        case PropertyItem::Property::Plot2DLS_Line_Antialiased:
          return Pair(tr("Line Antialiased"), ls->getlineantialiased_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Line_Style:
          return Pair(tr("Line Style"),
                      static_cast<int>(ls->getlinetype_lsplot()));
          break;
        case PropertyItem::Property::Plot2DLS_Line_Stroke_Color:
          return Pair(tr("Line Stroke Color"), ls->getlinestrokecolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Line_Stroke_Thickness:
          return Pair(tr("Line Stroke Thickness"),
                      ls->getlinestrokethickness_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Line_Stroke_Style:
          return Pair(tr("Line Stroke Type"),
                      static_cast<int>(ls->getlinestrokestyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DLS_Areafill_Separator:
          return Pair(tr("Area Fill"), QString());
          break;
        case PropertyItem::Property::Plot2DLS_Fill_Area:
          return Pair(tr("Fill Under Area"), ls->getlinefillstatus_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Fill_Color:
          return Pair(tr("Area Fill Color"), ls->getlinefillcolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Fill_Style:
          return Pair(tr("Area Fill Style"),
                      static_cast<int>(ls->getlinefillstyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DLS_Scatter_Separator:
          return Pair(tr("Scatter"), QString());
          break;
        case PropertyItem::Property::Plot2DLS_Scatter_Antialiased:
          return Pair(tr("Scatter Antialiased"),
                      ls->getscatterantialiased_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Scatter_Style:
          return Pair(tr("Scatter Style"),
                      static_cast<int>(ls->getscattershape_lsplot()));
          break;
        case PropertyItem::Property::Plot2DLS_Scatter_Size:
          return Pair(tr("Scatter Size"), ls->getscattersize_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Scatter_Outline_Color:
          return Pair(tr("Scatter Stroke Color"),
                      ls->getscatterstrokecolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Scatter_Outline_Thickness:
          return Pair(tr("Scatter Stroke Thickness"),
                      ls->getscatterstrokethickness_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Scatter_Outline_Style:
          return Pair(tr("Scatter Stroke Type"),
                      static_cast<int>(ls->getscatterstrokestyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DLS_Scatter_Fill_Color:
          return Pair(tr("Scatter Fill Color"),
                      ls->getscatterfillcolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Scatter_Fill_Style:
          return Pair(tr("Scatter Fill Type"),
                      static_cast<int>(ls->getscatterfillstyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DLS_Legend_Separator:
          return Pair(tr("Plot Legend"), QString());
          break;
        case PropertyItem::Property::Plot2DLS_Legend_Status:
          return Pair(tr("Visible"), ls->getlegendvisible_lsplot());
          break;
        case PropertyItem::Property::Plot2DLS_Legend_Text:
          return Pair(tr("Text"),
                      Utilities::joinstring(ls->getlegendtext_lsplot()));
          break;
        default:
          break;
      }
    } break;
    // Channel2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DChannelGraph: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (!status) break;
      LineSpecial2D *ls1 = channel->getChannelFirst();
      LineSpecial2D *ls2 = channel->getChannelSecond();
      switch (property_) {
        case PropertyItem::Property::Plot2DChannel_Axis_Separator:
          return Pair(tr("Axes"), QString());
          break;
        case PropertyItem::Property::Plot2DChannel_Axis_X:
          return Pair(tr("X Axis"),
                      static_cast<int>(ls1->getxaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DChannel_Axis_Y:
          return Pair(tr("Y Axis"),
                      static_cast<int>(ls1->getyaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DChannel_Line1_Separator:
          return Pair(tr("Channel Line 1"), QString());
          break;
        case PropertyItem::Property::Plot2DChannel_Line1_Antialiased:
          return Pair(tr("Line Antialiased"), ls1->getlineantialiased_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Line1_Style:
          return Pair(tr("Line Style"),
                      static_cast<int>(ls1->getlinetype_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Line1_Stroke_Color:
          return Pair(tr("Line Stroke Color"),
                      ls1->getlinestrokecolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Line1_Stroke_Thickness:
          return Pair(tr("Line Stroke Thickness"),
                      ls1->getlinestrokethickness_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Line1_Stroke_Style:
          return Pair(tr("Line Stroke Type"),
                      static_cast<int>(ls1->getlinestrokestyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter1_Separator:
          return Pair(tr("Channel Scatter 1"), QString());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter1_Antialiased:
          return Pair(tr("Scatter Antialiased"),
                      ls1->getscatterantialiased_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter1_Style:
          return Pair(tr("Scatter Style"),
                      static_cast<int>(ls1->getscattershape_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter1_Size:
          return Pair(tr("Scatter Size"), ls1->getscattersize_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Color:
          return Pair(tr("Scatter Stroke Color"),
                      ls1->getscatterstrokecolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Thickness:
          return Pair(tr("Scatter Stroke Thickness"),
                      ls1->getscatterstrokethickness_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Style:
          return Pair(tr("Scatter Stroke Type"),
                      static_cast<int>(ls1->getscatterstrokestyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter1_Fill_Color:
          return Pair(tr("Scatter Fill Color"),
                      ls1->getscatterfillcolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter1_Fill_Style:
          return Pair(tr("Scatter Fill Type"),
                      static_cast<int>(ls1->getscatterfillstyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Areafill_Separator:
          return Pair(tr("Area Fill"), QString());
          break;
        case PropertyItem::Property::Plot2DChannel_Fill_Area:
          return Pair(tr("Fill Under Area"), ls1->getlinefillstatus_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Fill_Color:
          return Pair(tr("Area Fill Color"), ls1->getlinefillcolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Fill_Style:
          return Pair(tr("Area Fill Style"),
                      static_cast<int>(ls1->getlinefillstyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Line2_Separator:
          return Pair(tr("Channel Line 2"), QString());
          break;
        case PropertyItem::Property::Plot2DChannel_Line2_Antialiased:
          return Pair(tr("Line Antialiased"), ls2->getlineantialiased_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Line2_Style:
          return Pair(tr("Line Style"),
                      static_cast<int>(ls2->getlinetype_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Line2_Stroke_Color:
          return Pair(tr("Line Stroke Color"),
                      ls2->getlinestrokecolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Line2_Stroke_Thickness:
          return Pair(tr("Line Stroke Thickness"),
                      ls2->getlinestrokethickness_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Line2_Stroke_Style:
          return Pair(tr("Line Stroke Type"),
                      static_cast<int>(ls2->getlinestrokestyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter2_Separator:
          return Pair(tr("Channel Scatter 2"), QString());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter2_Antialiased:
          return Pair(tr("Scatter Antialiased"),
                      ls2->getscatterantialiased_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter2_Style:
          return Pair(tr("Scatter Style"),
                      static_cast<int>(ls2->getscattershape_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter2_Size:
          return Pair(tr("Scatter Size"), ls2->getscattersize_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Color:
          return Pair(tr("Scatter Stroke Color"),
                      ls2->getscatterstrokecolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Thickness:
          return Pair(tr("Scatter Stroke Thickness"),
                      ls2->getscatterstrokethickness_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Style:
          return Pair(tr("Scatter Stroke Type"),
                      static_cast<int>(ls2->getscatterstrokestyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter2_Fill_Color:
          return Pair(tr("Scatter Fill Color"),
                      ls2->getscatterfillcolor_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Scatter2_Fill_Style:
          return Pair(tr("Scatter Fill Type"),
                      static_cast<int>(ls2->getscatterfillstyle_lsplot()));
          break;
        case PropertyItem::Property::Plot2DChannel_Legend_Separator:
          return Pair(tr("Plot Legend"), QString());
          break;
        case PropertyItem::Property::Plot2DChannel_Legend_Status:
          return Pair(tr("Visible"), ls1->getlegendvisible_lsplot());
          break;
        case PropertyItem::Property::Plot2DChannel_Legend_Text:
          return Pair(tr("Text"),
                      Utilities::joinstring(ls1->getlegendtext_lsplot()));
          break;
        default:
          break;
      }
    } break;
    // StatBox2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DStatBox: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DStatBox_Axis_Separator:
          return Pair(tr("Axes"), QString());
          break;
        case PropertyItem::Property::Plot2DStatBox_Axis_X:
          return Pair(tr("X Axis"),
                      static_cast<int>(sb->getxaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DStatBox_Axis_Y:
          return Pair(tr("Y Axis"),
                      static_cast<int>(sb->getyaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Separator:
          return Pair(tr("Box"), QString());
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Antialiased:
          return Pair(tr("Box Antialiased"), sb->antialiased());
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Width:
          return Pair(tr("Box Width"), sb->width());
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Style:
          return Pair(tr("Box Style"),
                      static_cast<int>(sb->getboxstyle_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Stroke_Color:
          return Pair(tr("Box Stroke Color"), sb->pen().color());
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Stroke_Thickness:
          return Pair(tr("Box Stroke Thickness"), sb->pen().widthF());
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Stroke_Style:
          return Pair(tr("Box Stroke Type"),
                      static_cast<int>(sb->pen().style()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Fill_Antialiased:
          return Pair(tr("Box Fill Antialiased"), sb->antialiasedFill());
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Fill_Color:
          return Pair(tr("Box Fill Color"), sb->getfillcolor_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_Box_Fill_Style:
          return Pair(tr("Box Fill Type"),
                      static_cast<int>(sb->getfillstyle_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Whisker_Separator:
          return Pair(tr("Whisker"), QString());
          break;
        case PropertyItem::Property::Plot2DStatBox_Whisker_Antialiased:
          return Pair(tr("Whisker Antialiased"), sb->whiskerAntialiased());
          break;
        case PropertyItem::Property::Plot2DStatBox_Whisker_Width:
          return Pair(tr("Whisker Width"), sb->whiskerWidth());
          break;
        case PropertyItem::Property::Plot2DStatBox_Whisker_Style:
          return Pair(tr("Whisker Style"),
                      static_cast<int>(sb->getwhiskerstyle_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Color:
          return Pair(tr("Whisker Stroke Color"),
                      sb->getwhiskerstrokecolor_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Thickness:
          return Pair(tr("Whisker Stroke Thickness"),
                      sb->getwhiskerstrokethickness_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Style:
          return Pair(tr("Whisker Stroke Type"),
                      static_cast<int>(sb->getwhiskerstrokestyle_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Separator:
          return Pair(tr("Whisker Bar"), QString());
          break;
        case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Color:
          return Pair(tr("Whiskerbar Stroke Color"),
                      sb->getwhiskerbarstrokecolor_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Thickness:
          return Pair(tr("Whiskerbar Stroke Thickness"),
                      sb->getwhiskerbarstrokethickness_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Style:
          return Pair(tr("Whiskerbar Stroke Type"),
                      static_cast<int>(sb->getwhiskerbarstrokestyle_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Median_Separator:
          return Pair(tr("Median"), QString());
          break;
        case PropertyItem::Property::Plot2DStatBox_Median_Stroke_Color:
          return Pair(tr("Median Stroke Color"),
                      sb->getmedianstrokecolor_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_Median_Stroke_Thickness:
          return Pair(tr("Median Stroke Thickness"),
                      sb->getmedianstrokethickness_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_Median_Stroke_Style:
          return Pair(tr("Median Stroke Type"),
                      static_cast<int>(sb->getmedianstrokestyle_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Separator:
          return Pair(tr("Scatter"), QString());
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Antialiased:
          return Pair(tr("Scatter Antialiased"), sb->antialiasedScatters());
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Show:
          return Pair(tr("Scatter Show"),
                      static_cast<int>(sb->getOutlierScatter_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Style:
          return Pair(tr("Scatter Style"),
                      static_cast<int>(sb->getscattershape_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Size:
          return Pair(tr("Scatter Size"), sb->getscattersize_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Color:
          return Pair(tr("Scatter Stroke Color"),
                      sb->getscatterstrokecolor_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Thickness:
          return Pair(tr("Scatter Stroke Thickness"),
                      sb->getscatterstrokethickness_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Style:
          return Pair(tr("Scatter Stroke Type"),
                      static_cast<int>(sb->getscatterstrokestyle_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Fill_Color:
          return Pair(tr("Scatter Fill Color"),
                      sb->getscatterfillcolor_statbox());
          break;
        case PropertyItem::Property::Plot2DStatBox_Scatter_Fill_Style:
          return Pair(tr("Scatter Fill Type"),
                      static_cast<int>(sb->getscatterfillstyle_statbox()));
          break;
        case PropertyItem::Property::Plot2DStatBox_Ticktext_Separator:
          return Pair(tr("Tick/Legend"), QString());
          break;
        case PropertyItem::Property::Plot2DStatBox_Tick_Text:
          return Pair(tr("Text"), Utilities::joinstring(sb->name()));
          break;
        default:
          break;
      }
    } break;
    // Vector2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DVector: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DVector_Axis_Separator:
          return Pair(tr("Axes"), QString());
          break;
        case PropertyItem::Property::Plot2DVector_Axis_X:
          return Pair(tr("X Axis"),
                      static_cast<int>(vec->getxaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DVector_Axis_Y:
          return Pair(tr("Y Axis"),
                      static_cast<int>(vec->getyaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DVector_Line_Separator:
          return Pair(tr("Line"), QString());
          break;
        case PropertyItem::Property::Plot2DVector_Line_Antialiased:
          return Pair(tr("Line Antialiased"),
                      vec->getlineantialiased_vecplot());
          break;
        case PropertyItem::Property::Plot2DVector_Line_Stroke_Color:
          return Pair(tr("Line Stroke Color"),
                      vec->getlinestrokecolor_vecplot());
          break;
        case PropertyItem::Property::Plot2DVector_Line_Stroke_Thickness:
          return Pair(tr("Line Stroke Thickness"),
                      vec->getlinestrokethickness_vecplot());
          break;
        case PropertyItem::Property::Plot2DVector_Line_Stroke_Style:
          return Pair(tr("Line Stroke Type"),
                      static_cast<int>(vec->getlinestrokestyle_vecplot()));
          break;
        case PropertyItem::Property::Plot2DVector_Ending_Separator:
          return Pair(tr("Ending"), QString());
          break;
        case PropertyItem::Property::Plot2DVector_Ending_Style:
          return Pair(tr("Ending Type"),
                      static_cast<int>(vec->getendstyle_vecplot(
                          Vector2D::LineEndLocation::Head)));
          break;
        case PropertyItem::Property::Plot2DVector_Ending_Width:
          return Pair(tr("Ending Width"), vec->getendwidth_vecplot(
                                              Vector2D::LineEndLocation::Head));
          break;
        case PropertyItem::Property::Plot2DVector_Ending_Height:
          return Pair(
              tr("Ending Height"),
              vec->getendheight_vecplot(Vector2D::LineEndLocation::Head));
          break;
        case PropertyItem::Property::Plot2DVector_Legend_Separator:
          return Pair(tr("Legend"), QString());
          break;
        case PropertyItem::Property::Plot2DVector_Legend_Status:
          return Pair(tr("Legend Show"), vec->getlegendvisible_vecplot());
          break;
        case PropertyItem::Property::Plot2DVector_Legend_Text:
          return Pair(tr("Legend Text"),
                      Utilities::joinstring(vec->getlegendtext_vecplot()));
          break;
        default:
          break;
      }
    } break;
    // Bar2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DBarGraph: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DBar_Axis_Separator:
          return Pair(tr("Axes"), QString());
          break;
        case PropertyItem::Property::Plot2DBar_Axis_X:
          return Pair(tr("X Axis"),
                      static_cast<int>(bar->getxaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DBar_Axis_Y:
          return Pair(tr("Y Axis"),
                      static_cast<int>(bar->getyaxis()->getnumber_axis() - 1));
          break;
        case PropertyItem::Property::Plot2DBar_Bar_Separator:
          return Pair(tr("Bar"), QString());
          break;
        case PropertyItem::Property::Plot2DBar_Width:
          return Pair(tr("Width"), bar->width());
          break;
        case PropertyItem::Property::Plot2DBar_Gap: {
          double spacing = 0;
          (bar->getBarStyle() == Bar2D::BarStyle::Grouped)
              ? spacing = bar->getBarGroup()->spacing()
              : spacing = bar->stackingGap();
          if (bar->getBarStyle() != Bar2D::BarStyle::Individual)
            return Pair(tr("Stack/Group Gap"), spacing);
        } break;
        case PropertyItem::Property::Plot2DBar_Line_Antialiased:
          return Pair(tr("Stroke Antialiased"), bar->antialiased());
          break;
        case PropertyItem::Property::Plot2DBar_Line_Stroke_Color:
          return Pair(tr("Stroke Color"), bar->getstrokecolor_barplot());
          break;
        case PropertyItem::Property::Plot2DBar_Line_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"),
                      bar->getstrokethickness_barplot());
          break;
        case PropertyItem::Property::Plot2DBar_Line_Stroke_Style:
          return Pair(tr("Stroke Color"),
                      static_cast<int>(bar->getstrokestyle_barplot()));
          break;
        case PropertyItem::Property::Plot2DBar_Fill_Antialiased:
          return Pair(tr("Fill Antialiased"), bar->antialiasedFill());
          break;
        case PropertyItem::Property::Plot2DBar_Fill_Color:
          return Pair(tr("Fill Color"), bar->getfillcolor_barplot());
          break;
        case PropertyItem::Property::Plot2DBar_Fill_Style:
          return Pair(tr("Fill Style"),
                      static_cast<int>(bar->getfillstyle_barplot()));
          break;
        case PropertyItem::Property::Plot2DBar_Histogram_Separator:
          if (bar->ishistogram_barplot())
            return Pair(tr("Histogram"), QString());
          break;
        case PropertyItem::Property::Plot2DBar_Histogram_AutoBin:
          if (bar->ishistogram_barplot())
            return Pair(tr("Auto Bin"),
                        bar->getdatablock_histplot()->getautobin());
          break;
        case PropertyItem::Property::Plot2DBar_Histogram_BinSize:
          if (bar->ishistogram_barplot())
            return Pair(tr("Bin Size"),
                        bar->getdatablock_histplot()->getbinsize());
          break;
        case PropertyItem::Property::Plot2DBar_Histogram_Begin:
          if (bar->ishistogram_barplot())
            return Pair(tr("Begin"), bar->getdatablock_histplot()->getbegin());
          break;
        case PropertyItem::Property::Plot2DBar_Histogram_End:
          if (bar->ishistogram_barplot())
            return Pair(tr("End"), bar->getdatablock_histplot()->getend());
          break;
        case PropertyItem::Property::Plot2DBar_Legend_Separator:
          return Pair(tr("Legend"), QString());
          break;
        case PropertyItem::Property::Plot2DBar_Legend_Status:
          return Pair(tr("Legend Show"), bar->getlegendvisible_barplot());
          break;
        case PropertyItem::Property::Plot2DBar_Legend_Text:
          return Pair(tr("Legend Text"),
                      Utilities::joinstring(bar->getlegendtext_barplot()));
          break;
        default:
          break;
      }
    } break;
    // Pie2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DPieGraph: {
      Pie2D *pie = item_->getObjectTreeItem<Pie2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DPie_Basic_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DPie_Style:
          return Pair(tr("Style"), static_cast<int>(pie->getStyle_pieplot()));
          break;
        case PropertyItem::Property::Plot2DPie_Stroke_Color:
          return Pair(tr("Stroke Color"), pie->getstrokecolor_pieplot());
          break;
        case PropertyItem::Property::Plot2DPie_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"),
                      pie->getstrokethickness_pieplot());
          break;
        case PropertyItem::Property::Plot2DPie_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(pie->getstrokestyle_pieplot()));
          break;
        case PropertyItem::Property::Plot2DPie_Margin_Percent:
          return Pair(tr("Margin Percent"), pie->getmarginpercent_pieplot());
          break;
        default:
          break;
      }
    } break;
    // ColorMap2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DColorMap: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (!status) break;
      QCPAxis *axis = cm->getcolormapscale_colormap()->axis();
      switch (property_) {
        case PropertyItem::Property::Plot2DCM_Basic_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DCM_Interpolate:
          return Pair(tr("Interpolate"), cm->interpolate());
          break;
        case PropertyItem::Property::Plot2DCM_Tight_Boundary:
          return Pair(tr("Tight Boundary"), cm->tightBoundary());
          break;
        case PropertyItem::Property::Plot2DCM_Level_Count:
          return Pair(tr("Level Count"), cm->getlevelcount_colormap());
          break;
        case PropertyItem::Property::Plot2DCM_Gradient:
          return Pair(tr("Gradient"),
                      static_cast<int>(cm->getgradient_colormap()));
          break;
        case PropertyItem::Property::Plot2DCM_Gradient_Invert:
          return Pair(tr("Gradient Invert"),
                      cm->getgradientinverted_colormap());
          break;
        case PropertyItem::Property::Plot2DCM_Periodic:
          return Pair(tr("Gradient Periodic"),
                      cm->getgradientperiodic_colormap());
          break;
        case PropertyItem::Property::Plot2DCM_Scale_Separator:
          return Pair(tr("Scale"), QString());
          break;
        case PropertyItem::Property::Plot2DCM_Scale_Visible:
          return Pair(tr("Visible"),
                      cm->getcolormapscale_colormap()->visible());
          break;
        case PropertyItem::Property::Plot2DCM_Scale_Width:
          return Pair(tr("Width"), cm->getcolormapscalewidth_colormap());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Separator:
          return Pair(tr("Scale Axis"), QString());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Visible:
          return Pair(tr("Visible"), axis->visible());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Offset:
          return Pair(tr("Offset"), axis->offset());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_From_Double:
          return Pair(tr("From"), axis->range().lower);
          break;
        case PropertyItem::Property::Plot2DCM_Axis_To_Double:
          return Pair(tr("To"), axis->range().upper);
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Type:
          return Pair(tr("Type"), static_cast<int>(cm->dataScaleType()));
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Inverted:
          return Pair(tr("Inverted"), axis->rangeReversed());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Antialiased:
          return Pair(tr("Antialiased"), axis->antialiased());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Stroke_Color:
          return Pair(tr("Stroke Color"), axis->basePen().color());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"), axis->basePen().widthF());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(axis->basePen().style()));
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Label_Separator:
          return Pair(tr("Scale Axis Label"), QString());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Label_Text:
          return Pair(tr("Label Text"),
                      Utilities::joinstring(cm->getname_colormap()));
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Label_Font:
          return Pair(tr("Label Font"), axis->labelFont());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Label_Color:
          return Pair(tr("Label Color"), axis->labelColor());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Label_Padding:
          return Pair(tr("Label Padding"), axis->labelPadding());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Ticks_Separator:
          return Pair(tr("Scale Axis Tick"), QString());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Ticks_Visible:
          return Pair(tr("Visible"), axis->ticks());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Ticks_LengthIn:
          return Pair(tr("Length In"), axis->tickLengthIn());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Ticks_LengthOut:
          return Pair(tr("Length Out"), axis->tickLengthOut());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Color:
          return Pair(tr("Stroke Color"), axis->tickPen().color());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"), axis->tickPen().widthF());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(axis->tickPen().style()));
          break;
        case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Separator:
          return Pair(tr("Scale Axis Sub-Tick"), QString());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Visible:
          return Pair(tr("Visible"), axis->subTicks());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_SubTicks_LengthIn:
          return Pair(tr("Length In"), axis->subTickLengthIn());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_SubTicks_LengthOut:
          return Pair(tr("Length Out"), axis->subTickLengthOut());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Color:
          return Pair(tr("Stroke Color"), axis->subTickPen().color());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"), axis->subTickPen().widthF());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Style:
          return Pair(tr("Stroke Type"),
                      static_cast<int>(axis->subTickPen().style()));
          break;
        case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Separator:
          return Pair(tr("Scale Axis Tick Label"), QString());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Visible:
          return Pair(tr("Visible"), axis->tickLabels());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Font:
          return Pair(tr("Font"), axis->tickLabelFont());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Color:
          return Pair(tr("Color"), axis->tickLabelColor());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Padding:
          return Pair(tr("Padding"), axis->tickLabelPadding());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Rotation:
          return Pair(tr("Rotation"), axis->tickLabelRotation());
          break;
        case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Side:
          return Pair(tr("Side"), static_cast<int>(axis->tickLabelSide()));
          break;
        case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Format_Double: {
          return Pair(
              tr("Format"),
              static_cast<int>(cm->getcolormapscaleticklabelformat_axis()));
        } break;
        case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Precision_Double: {
          return Pair(tr("Precision"),
                      static_cast<int>(axis->numberPrecision()));
        } break;
        default:
          break;
      }
    } break;
    // ErrorBar2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar: {
      ErrorBar2D *err = item_->getObjectTreeItem<ErrorBar2D>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot2DErrBar_Basic_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot2DErrBar_Whisker_Width:
          return Pair(tr("Whisker Width"), err->whiskerWidth());
          break;
        case PropertyItem::Property::Plot2DErrBar_Symbol_Gap:
          return Pair(tr("Symbol Gap"), err->symbolGap());
          break;
        case PropertyItem::Property::Plot2DErrBar_Antialiased:
          return Pair(tr("Antialiased"), err->antialiased());
          break;
        case PropertyItem::Property::Plot2DErrBar_Stroke_Color:
          return Pair(tr("Stroke Color"), err->pen().color());
          break;
        case PropertyItem::Property::Plot2DErrBar_Stroke_Thickness:
          return Pair(tr("Stroke Thickness"), err->pen().widthF());
          break;
        case PropertyItem::Property::Plot2DErrBar_Stroke_Style:
          return Pair(tr("Stroke Type"), static_cast<int>(err->pen().style()));
          break;
        case PropertyItem::Property::Plot2DErrBar_Fill_Color:
          return Pair(tr("Fill Color"), err->brush().color());
          break;
        case PropertyItem::Property::Plot2DErrBar_Fill_Style:
          return Pair(tr("Stroke Color"),
                      static_cast<int>(err->brush().style()));
          break;
        default:
          break;
      }
    } break;
    // Plot3D Canvas
    case ObjectBrowserTreeItem::ObjectType::Plot3DCanvas: {
      QAbstract3DGraph *plot =
          item_->getObjectTreeItem<QAbstract3DGraph>(&status);
      if (!status) break;
      switch (property_) {
        case PropertyItem::Property::Plot3DCanvas_Base_Separator:
          return Pair(tr("Base"), QString());
          break;
        case PropertyItem::Property::Plot3DCanvas_Theme:
          return Pair(tr("Theme"), plot->activeTheme()->type());
          break;
        case PropertyItem::Property::Plot3DCanvas_Dimension:
          return Pair(tr("Dimension"),
                      QSize(plot->size().width(), plot->size().height()));
          break;
        case PropertyItem::Property::Plot3DCanvas_Dimension_Width:
          return Pair(tr("Width"), plot->size().width());
          break;
        case PropertyItem::Property::Plot3DCanvas_Dimension_Height:
          return Pair(tr("Height"), plot->size().height());
          break;
        default:
          break;
      }
    } break;
  }
  return Pair(QString(), QVariant());
}

bool PropertyItem::isreadonly() const { return readonly_; }

QVariant PropertyItem::data(int column, int role) const {
  if (column == 0) switch (role) {
      case Qt::ItemDataRole::DisplayRole:
      case Qt::ItemDataRole::ToolTipRole:
        return value().name;
        break;
      default:
        return QVariant();
    }
  else {
    switch (role) {
      case Qt::UserRole:
        return value().value;
      case Qt::ItemDataRole::DisplayRole:
      case Qt::ItemDataRole::ToolTipRole:
        return toString();
        break;
      case Qt::ItemDataRole::DecorationRole: {
        if (propertytype_ == PropertyItem::PropertyType::Enum)
          return enumList_.at(value().value.toInt()).first;
        else
          return QVariant();
      } break;
      default:
        return QVariant();
    }
  }
}

void PropertyItem::setStringValue(const QString &val) {
  bool status = false;
  switch (property_) {
    // MyWidget
    case PropertyItem::Property::BaseWindow_Name: {
      MyWidget *widget = item_->getObjectTreeItem<MyWidget>(&status);
      if (val != PropertyData<QString>() && status)
        emit namechange(widget, val);
    } break;
    case PropertyItem::Property::BaseWindow_Label: {
      MyWidget *widget = item_->getObjectTreeItem<MyWidget>(&status);
      if (val != PropertyData<QString>() && status)
        emit labelchange(widget, val);
    } break;
    // Legend2D
    case PropertyItem::Property::Plot2DLegend_Title_Text: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<QString>() && status) {
        legend->settitletext_legend(Utilities::splitstring(val));
        legend->layer()->replot();
      }
    } break;
    // Axis2D
    case PropertyItem::Property::Plot2DAxis_Label_Text: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QString>() && status) {
        axis->setlabeltext_axis(Utilities::splitstring(val));
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Symbol_Pi: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QString>() && status) {
        QSharedPointer<QCPAxisTickerPi> piticker =
            qSharedPointerCast<QCPAxisTickerPi>(axis->getticker_axis());
        if (piticker) {
          piticker->setPiSymbol(Utilities::splitstring(val));
          axis->parentPlot()->replot(
              QCustomPlot::RefreshPriority::rpQueuedReplot);
        }
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Format_DateTime: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QString>() && status) {
        QSharedPointer<QCPAxisTickerDateTime> dtticker =
            qSharedPointerCast<QCPAxisTickerDateTime>(axis->getticker_axis());
        if (dtticker) {
          dtticker->setDateTimeFormat(Utilities::splitstring(val));
          axis->parentPlot()->replot(
              QCustomPlot::RefreshPriority::rpQueuedReplot);
        }
      }
    } break;
    // TextItem2D
    case PropertyItem::Property::Plot2DTextItem_Text: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<QString>() && status) {
        textitem->setText(Utilities::splitstring(val));
        textitem->layer()->replot();
      }
    } break;
    // Curve2D
    case PropertyItem::Property::Plot2DCurve_Legend_Text: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<QString>() && status) {
        curve->setlegendtext_cplot(Utilities::splitstring(val));
        curve->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Graph2D
    case PropertyItem::Property::Plot2DLS_Legend_Text: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<QString>() && status) {
        ls->setlegendtext_lsplot(Utilities::splitstring(val));
        ls->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Channel2D
    case PropertyItem::Property::Plot2DChannel_Legend_Text: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<QString>() && status) {
        channel->getChannelFirst()->setlegendtext_lsplot(
            Utilities::splitstring(val));
        channel->getChannelFirst()
            ->getxaxis()
            ->getaxisrect_axis()
            ->getLegend()
            ->layer()
            ->replot();
      }
    } break;
    // StatBox2D
    case PropertyItem::Property::Plot2DStatBox_Tick_Text: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<QString>() && status) {
        sb->setlegendtext_statbox(Utilities::splitstring(val));
        sb->getxaxis()->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Vector2D
    case PropertyItem::Property::Plot2DVector_Legend_Text: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (val != PropertyData<QString>() && status) {
        vec->setlegendtext_vecplot(Utilities::splitstring(val));
        vec->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Bar2D
    case PropertyItem::Property::Plot2DBar_Legend_Text: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<QString>() && status) {
        bar->setlegendtext_barplot(Utilities::splitstring(val));
        bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // ColorMap2D
    case PropertyItem::Property::Plot2DCM_Axis_Label_Text: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<QString>() && status) {
        cm->getcolormapscale_colormap()->setLabel(Utilities::splitstring(val));
        cm->setname_colormap(Utilities::splitstring(val));
        cm->layer()->replot();
        cm->getcolormapscale_colormap()->axis()->layer()->replot();
      }
    } break;
    default:
      break;
  }
}

void PropertyItem::setIntValue(const int &val) {
  bool status = false;
  switch (property_) {
    case PropertyItem::Property::BaseWindow_Geometry_X: {
      MyWidget *widget = item_->getObjectTreeItem<MyWidget>(&status);
      QRect rect = widget->geometry();
      if (val != PropertyData<int>() && status) {
        rect.setX(val);
        widget->setGeometry(rect);
      }
    } break;
    case PropertyItem::Property::BaseWindow_Geometry_Y: {
      MyWidget *widget = item_->getObjectTreeItem<MyWidget>(&status);
      QRect rect = widget->geometry();
      if (val != PropertyData<int>() && status) {
        rect.setY(val);
        widget->setGeometry(rect);
      }
    } break;
    case PropertyItem::Property::BaseWindow_Geometry_Width: {
      MyWidget *widget = item_->getObjectTreeItem<MyWidget>(&status);
      QRect rect = widget->geometry();
      if (val != PropertyData<int>() && status) {
        rect.setWidth(val);
        widget->setGeometry(rect);
      }
    } break;
    case PropertyItem::Property::BaseWindow_Geometry_Height: {
      MyWidget *widget = item_->getObjectTreeItem<MyWidget>(&status);
      QRect rect = widget->geometry();
      if (val != PropertyData<int>() && status) {
        rect.setHeight(val);
        widget->setGeometry(rect);
      }
    } break;
    case PropertyItem::Property::TableWindow_Row_Count: {
      Table *table = item_->getObjectTreeItem<Table>(&status);
      if (val != PropertyData<int>() && status) {
        table->setNumRows(val);
      }
    } break;
    case PropertyItem::Property::TableWindow_Column_Count: {
      Table *table = item_->getObjectTreeItem<Table>(&status);
      if (val != PropertyData<int>() && status) {
        table->setNumCols(val);
      }
    } break;
    case PropertyItem::Property::MatrixWindow_Row_Count: {
      Matrix *matrix = item_->getObjectTreeItem<Matrix>(&status);
      if (val != PropertyData<int>() && status) {
        matrix->setNumRows(val);
      }
    } break;
    case PropertyItem::Property::MatrixWindow_Column_Count: {
      Matrix *matrix = item_->getObjectTreeItem<Matrix>(&status);
      if (val != PropertyData<int>() && status) {
        matrix->setNumCols(val);
      }
    } break;
    case PropertyItem::Property::Plot2DCanvas_Dimension_Width: {
      Plot2D *plot = item_->getObjectTreeItem<Plot2D>(&status);
      QRect oldrect = plot->geometry();
      if (val != PropertyData<int>() && status) {
        QRect rect = oldrect;
        rect.setWidth(val);
        plot->setGeometry(rect);
        MyWidget *widget =
            qobject_cast<MyWidget *>(plot->parentWidget()->parent());
        if (widget) {
          QRect wrect = widget->geometry();
          widget->resize(wrect.width() + (rect.width() - oldrect.width()),
                         wrect.height() + (rect.height() - oldrect.height()));
        }
        plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCanvas_Dimension_Height: {
      Plot2D *plot = item_->getObjectTreeItem<Plot2D>(&status);
      QRect oldrect = plot->geometry();
      if (val != PropertyData<int>() && status) {
        QRect rect = oldrect;
        rect.setHeight(val);
        plot->setGeometry(rect);
        MyWidget *widget =
            qobject_cast<MyWidget *>(plot->parentWidget()->parent());
        if (widget) {
          QRect wrect = widget->geometry();
          widget->resize(wrect.width() + (rect.width() - oldrect.width()),
                         wrect.height() + (rect.height() - oldrect.height()));
        }
        plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCanvas_Row_Spacing: {
      Layout2D *layout =
          item_->parentItem()->getObjectTreeItem<Layout2D>(&status);
      if (val != PropertyData<int>() && status) {
        LayoutGrid2D *grid = layout->getLayoutGrid();
        grid->setRowSpacing(val);
        layout->getPlotCanwas()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCanvas_Column_Spacing: {
      Layout2D *layout =
          item_->parentItem()->getObjectTreeItem<Layout2D>(&status);
      if (val != PropertyData<int>() && status) {
        LayoutGrid2D *grid = layout->getLayoutGrid();
        grid->setColumnSpacing(val);
        layout->getPlotCanwas()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Margin_Left: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        QMargins mar = legend->margins();
        mar.setLeft(val);
        legend->setMargins(mar);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Margin_Right: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        QMargins mar = legend->margins();
        mar.setRight(val);
        legend->setMargins(mar);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Margin_Top: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        QMargins mar = legend->margins();
        mar.setTop(val);
        legend->setMargins(mar);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Margin_Bottom: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        QMargins mar = legend->margins();
        mar.setBottom(val);
        legend->setMargins(mar);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Icon_Width: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        QSize size = legend->iconSize();
        size.setWidth(val);
        legend->setIconSize(size);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Icon_Height: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        QSize size = legend->iconSize();
        size.setHeight(val);
        legend->setIconSize(size);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Icon_Padding: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        legend->setIconTextPadding(val);
        legend->layer()->replot();
      }
    } break;
      // Axis 2D
    case PropertyItem::Property::Plot2DAxis_Offset: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setoffset_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Label_Padding: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setlabelpadding_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Ticks_Count: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->settickscount_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Ticks_LengthIn: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setticklengthin_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Ticks_LengthOut: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setticklengthout_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_SubTicks_LengthIn: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setsubticklengthin_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_SubTicks_LengthOut: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setsubticklengthout_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Padding: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setticklabelpadding_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Precision_Double: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        if (axis->gettickertype_axis() == Axis2D::TickerType::Value ||
            axis->gettickertype_axis() == Axis2D::TickerType::Log ||
            axis->gettickertype_axis() == Axis2D::TickerType::Pi) {
          axis->setticklabelprecision_axis(val);
          axis->parentPlot()->replot(
              QCustomPlot::RefreshPriority::rpQueuedReplot);
        }
      }
    } break;
    // TextItem2D
    case PropertyItem::Property::Plot2DTextItem_Margin_Left: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        QMargins mar = textitem->padding();
        mar.setLeft(val);
        textitem->setPadding(mar);
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Margin_Bottom: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        QMargins mar = textitem->padding();
        mar.setBottom(val);
        textitem->setPadding(mar);
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Margin_Right: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        QMargins mar = textitem->padding();
        mar.setRight(val);
        textitem->setPadding(mar);
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Margin_Top: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        QMargins mar = textitem->padding();
        mar.setTop(val);
        textitem->setPadding(mar);
        textitem->layer()->replot();
      }
    } break;
    // Bar2D
    case PropertyItem::Property::Plot2DPie_Margin_Percent: {
      Pie2D *pie = item_->getObjectTreeItem<Pie2D>(&status);
      if (val != PropertyData<int>() && status) {
        pie->setmarginpercent_pieplot(val);
        pie->layer()->replot();
      }
    } break;
    // ColorMap2D
    case PropertyItem::Property::Plot2DCM_Level_Count: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->setlevelcount_colormap(val);
        cm->parentPlot()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Scale_Width: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->setcolormapscalewidth_colormap(val);
        cm->parentPlot()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Offset: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setOffset(val);
        cm->parentPlot()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Label_Padding: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setLabelPadding(val);
        cm->parentPlot()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_LengthIn: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setTickLengthIn(val);
        cm->parentPlot()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_LengthOut: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setTickLengthOut(val);
        cm->parentPlot()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_LengthIn: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setSubTickLengthIn(val);
        cm->parentPlot()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_LengthOut: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setSubTickLengthOut(val);
        cm->parentPlot()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Padding: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setTickLabelPadding(val);
        cm->parentPlot()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Precision_Double: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setNumberPrecision(val);
        cm->parentPlot()->replot();
      }
    } break;
    // Plot3D Canvas
    case PropertyItem::Property::Plot3DCanvas_Dimension_Width: {
      QAbstract3DGraph *plot =
          item_->getObjectTreeItem<QAbstract3DGraph>(&status);
      if (val != PropertyData<int>() && status) {
        QSize oldsize = plot->size();
        QSize sizediff = QSize(val, plot->size().height()) - oldsize;
        MyWidget *widget =
            item_->parentItem()->getObjectTreeItem<MyWidget>(&status);
        if (status) widget->resize(widget->size() + sizediff);
      }
    } break;
    case PropertyItem::Property::Plot3DCanvas_Dimension_Height: {
      QAbstract3DGraph *plot =
          item_->getObjectTreeItem<QAbstract3DGraph>(&status);
      if (val != PropertyData<int>() && status) {
        QSize oldsize = plot->size();
        QSize sizediff = QSize(plot->size().width(), val) - oldsize;
        MyWidget *widget =
            item_->parentItem()->getObjectTreeItem<MyWidget>(&status);
        if (status) widget->resize(widget->size() + sizediff);
      }
    } break;
    default:
      break;
  }
}

void PropertyItem::setDoubleValue(const double &val) {
  bool status = false;
  switch (property_) {
    // Plot2D
    case PropertyItem::Property::Plot2DCanvas_DPR: {
      Plot2D *plot = item_->getObjectTreeItem<Plot2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        plot->setBufferDevicePixelRatio(val);
        plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    // Layout2D
    case PropertyItem::Property::Plot2DLayout_Row_Stretch_Factor: {
      AxisRect2D *axisrect = item_->getObjectTreeItem<AxisRect2D>(&status);
      if (status) {
        Layout2D *layout =
            item_->parentItem()->getObjectTreeItem<Layout2D>(&status);
        if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
          QPair<int, int> rowcol = layout->getAxisRectRowCol(axisrect);
          layout->getLayoutGrid()->setRowStretchFactor(rowcol.first, val);
          layout->getPlotCanwas()->replot(
              QCustomPlot::RefreshPriority::rpQueuedReplot);
        }
      }
    } break;
    case PropertyItem::Property::Plot2DLayout_Column_Stretch_Factor: {
      AxisRect2D *axisrect = item_->getObjectTreeItem<AxisRect2D>(&status);
      if (status) {
        Layout2D *layout =
            item_->parentItem()->getObjectTreeItem<Layout2D>(&status);
        if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
          QPair<int, int> rowcol = layout->getAxisRectRowCol(axisrect);
          layout->getLayoutGrid()->setColumnStretchFactor(rowcol.second, val);
          layout->getPlotCanwas()->replot(
              QCustomPlot::RefreshPriority::rpQueuedReplot);
        }
      }
    } break;
    // Legend2D
    case PropertyItem::Property::Plot2DLegend_X: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF point = QPointF(legend->getposition_legend());
        point.setX(val);
        legend->setposition_legend(point);
        legend->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Y: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF point = QPointF(legend->getposition_legend());
        point.setY(val);
        legend->setposition_legend(point);
        legend->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Stroke_Thickness: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        legend->setborderstrokethickness_legend(val);
        legend->layer()->replot();
      }
    } break;
    // Axis2D
    case PropertyItem::Property::Plot2DAxis_From_Text: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        if (val < axis->getto_axis()) {
          if (QCPRange::validRange(val, axis->getto_axis())) {
            axis->setfrom_axis(val);
            axis->parentPlot()->replot(
                QCustomPlot::RefreshPriority::rpQueuedReplot);
          } else
            emit outofrange(axis->getfrom_axis(), axis->getto_axis());
        }
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_To_Text: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        if (val > axis->getfrom_axis()) {
          if (QCPRange::validRange(axis->getfrom_axis(), val)) {
            axis->setto_axis(val);
            axis->parentPlot()->replot(
                QCustomPlot::RefreshPriority::rpQueuedReplot);
          } else
            emit outofrange(axis->getfrom_axis(), axis->getto_axis());
        }
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Stroke_Thickness: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        axis->setstrokethickness_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Ticks_Origin: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        axis->setticksorigin(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Thickness: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        axis->settickstrokethickness_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Thickness: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        axis->setsubtickstrokethickness_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Rotation: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        axis->setticklabelrotation_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_LogBase_Log: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status &&
          val != 1) {
        QSharedPointer<QCPAxisTickerLog> logticker =
            qSharedPointerCast<QCPAxisTickerLog>(axis->getticker_axis());
        if (logticker) {
          logticker->setLogBase(val);
          axis->parentPlot()->replot(
              QCustomPlot::RefreshPriority::rpQueuedReplot);
        }
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Symbol_Value_Pi: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QSharedPointer<QCPAxisTickerPi> piticker =
            qSharedPointerCast<QCPAxisTickerPi>(axis->getticker_axis());
        if (piticker) {
          piticker->setPiValue(val);
          axis->parentPlot()->replot(
              QCustomPlot::RefreshPriority::rpQueuedReplot);
        }
      }
    } break;
    // Grid2D
    case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Thickness: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        gp->getXgrid()->setMajorGridThickness(val);
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Thickness: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        gp->getXgrid()->setZerothLineThickness(val);
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Thickness: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        gp->getXgrid()->setMinorGridThickness(val);
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Thickness: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        gp->getYgrid()->setMajorGridThickness(val);
        gp->getYgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Thickness: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        gp->getYgrid()->setZerothLineThickness(val);
        gp->getYgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Thickness: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        gp->getYgrid()->setMinorGridThickness(val);
        gp->getYgrid()->layer()->replot();
      }
    } break;
    // TextItem2D
    case PropertyItem::Property::Plot2DTextItem_Position_X: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF pos = textitem->position->pixelPosition();
        pos.setX(val);
        textitem->setpixelposition_textitem(pos);
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Position_Y: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF pos = textitem->position->pixelPosition();
        pos.setY(val);
        textitem->setpixelposition_textitem(pos);
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Text_Rotation: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        textitem->setRotation(val);
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Stroke_Thickness: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        textitem->setstrokethickness_textitem(val);
        textitem->layer()->replot();
      }
    } break;
    // LineItem2D
    case PropertyItem::Property::Plot2DLineItem_Position_X1: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF point = lineitem->position("start")->pixelPosition();
        point.setX(val);
        lineitem->position("start")->setPixelPosition(point);
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Position_Y1: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF point = lineitem->position("start")->pixelPosition();
        point.setY(val);
        lineitem->position("start")->setPixelPosition(point);
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Position_X2: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF point = lineitem->position("end")->pixelPosition();
        point.setX(val);
        lineitem->position("end")->setPixelPosition(point);
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Position_Y2: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF point = lineitem->position("end")->pixelPosition();
        point.setY(val);
        lineitem->position("end")->setPixelPosition(point);
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Stroke_Thickness: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        lineitem->setstrokethickness_lineitem(val);
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Starting_Width: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        lineitem->setendwidth_lineitem(val, LineItem2D::LineEndLocation::Start);
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Starting_Height: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        lineitem->setendlength_lineitem(val,
                                        LineItem2D::LineEndLocation::Start);
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Ending_Width: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        lineitem->setendwidth_lineitem(val, LineItem2D::LineEndLocation::Stop);
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Ending_Height: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        lineitem->setendlength_lineitem(val, LineItem2D::LineEndLocation::Stop);
        lineitem->layer()->replot();
      }
    } break;
    // ImageItem2D
    case PropertyItem::Property::Plot2DImageItem_Position_X: {
      ImageItem2D *imageitem = item_->getObjectTreeItem<ImageItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF point = imageitem->position("topLeft")->pixelPosition();
        point.setX(val);
        imageitem->setposition_imageitem(point);
        imageitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DImageItem_Position_Y: {
      ImageItem2D *imageitem = item_->getObjectTreeItem<ImageItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPointF point = imageitem->position("topLeft")->pixelPosition();
        point.setY(val);
        imageitem->setposition_imageitem(point);
        imageitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DImageItem_Rotation: {
      ImageItem2D *imageitem = item_->getObjectTreeItem<ImageItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        imageitem->setrotation_imageitem(val);
        imageitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DImageItem_Stroke_Thickness: {
      ImageItem2D *imageitem = item_->getObjectTreeItem<ImageItem2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        imageitem->setstrokethickness_imageitem(val);
        imageitem->layer()->replot();
      }
    } break;
    // Curve2D
    case PropertyItem::Property::Plot2DCurve_Line_Stroke_Thickness: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        curve->setlinestrokethickness_cplot(val);
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Scatter_Size: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        curve->setscattersize_cplot(val);
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Scatter_Outline_Thickness: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        curve->setscatterstrokethickness_cplot(val);
        curve->layer()->replot();
      }
    } break;
    // Graph2D
    case PropertyItem::Property::Plot2DLS_Line_Stroke_Thickness: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        ls->setlinestrokethickness_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Scatter_Size: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        ls->setscattersize_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Scatter_Outline_Thickness: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        ls->setscatterstrokethickness_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    // Channel2D
    case PropertyItem::Property::Plot2DChannel_Line1_Stroke_Thickness: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        channel->getChannelFirst()->setlinestrokethickness_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter1_Size: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        channel->getChannelFirst()->setscattersize_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Thickness: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        channel->getChannelFirst()->setscatterstrokethickness_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Line2_Stroke_Thickness: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        channel->getChannelSecond()->setlinestrokethickness_lsplot(val);
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter2_Size: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        channel->getChannelSecond()->setscattersize_lsplot(val);
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Thickness: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        channel->getChannelSecond()->setscatterstrokethickness_lsplot(val);
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    // StatBox2D
    case PropertyItem::Property::Plot2DStatBox_Box_Width: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        sb->setWidth(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Box_Stroke_Thickness: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPen p = sb->pen();
        p.setWidthF(val);
        sb->setPen(p);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Whisker_Width: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        sb->setWhiskerWidth(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Thickness: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        sb->setwhiskerstrokethickness_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Thickness: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        sb->setwhiskerbarstrokethickness_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Median_Stroke_Thickness: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        sb->setmedianstrokethickness_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Scatter_Size: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        sb->setscattersize_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Thickness: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        sb->setscatterstrokethickness_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Vector2D
    case PropertyItem::Property::Plot2DVector_Line_Stroke_Thickness: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        vec->setlinestrokethickness_vecplot(val);
        vec->layer()->replot();
        vec->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DVector_Ending_Width: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        vec->setendwidth_vecplot(val, Vector2D::LineEndLocation::Head);
        vec->layer()->replot();
        vec->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DVector_Ending_Height: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        vec->setendheight_vecplot(val, Vector2D::LineEndLocation::Head);
        vec->layer()->replot();
        vec->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Bar2D
    case PropertyItem::Property::Plot2DBar_Width: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        bar->setWidth(val);
        bar->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Gap: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (bar->getBarStyle() != Bar2D::BarStyle::Individual) {
        if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
          (bar->getBarStyle() == Bar2D::BarStyle::Grouped)
              ? bar->getBarGroup()->setSpacing(val)
              : bar->setStackingGap(val);
          bar->layer()->replot();
        }
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Line_Stroke_Thickness: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        bar->setstrokethickness_barplot(val);
        bar->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Histogram_BinSize: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status &&
          bar->ishistogram_barplot()) {
        if (!bar->getdatablock_histplot()->getautobin())
          bar->setHistBinSize(val);
        bar->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Histogram_Begin: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status &&
          bar->ishistogram_barplot()) {
        if (!bar->getdatablock_histplot()->getautobin()) bar->setHistBegin(val);
        bar->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Histogram_End: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status &&
          bar->ishistogram_barplot()) {
        if (!bar->getdatablock_histplot()->getautobin()) bar->setHistEnd(val);
        bar->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DPie_Stroke_Thickness: {
      Pie2D *pie = item_->getObjectTreeItem<Pie2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        pie->setstrokethickness_pieplot(val);
        pie->layer()->replot();
      }
    } break;
    // ColorMap2D
    case PropertyItem::Property::Plot2DCM_Axis_Stroke_Thickness: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPen p = cm->getcolormapscale_colormap()->axis()->basePen();
        p.setWidthF(val);
        cm->getcolormapscale_colormap()->axis()->setBasePen(p);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Thickness: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPen p = cm->getcolormapscale_colormap()->axis()->tickPen();
        p.setWidthF(val);
        cm->getcolormapscale_colormap()->axis()->setTickPen(p);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Thickness: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPen p = cm->getcolormapscale_colormap()->axis()->subTickPen();
        p.setWidthF(val);
        cm->getcolormapscale_colormap()->axis()->setSubTickPen(p);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Rotation: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        cm->getcolormapscale_colormap()->axis()->setTickLabelRotation(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    // ErrorBar2D
    case PropertyItem::Property::Plot2DErrBar_Whisker_Width: {
      ErrorBar2D *err = item_->getObjectTreeItem<ErrorBar2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        err->setWhiskerWidth(val);
        err->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DErrBar_Symbol_Gap: {
      ErrorBar2D *err = item_->getObjectTreeItem<ErrorBar2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        err->setSymbolGap(val);
        err->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DErrBar_Stroke_Thickness: {
      ErrorBar2D *err = item_->getObjectTreeItem<ErrorBar2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        QPen p = err->pen();
        p.setWidthF(val);
        err->setPen(p);
        err->layer()->replot();
      }
    } break;
    default:
      break;
  }
}

void PropertyItem::setScientificDoubleValue(const double &val) {
  bool status = false;
  switch (property_) {
    // Axis2D
    case PropertyItem::Property::Plot2DAxis_From_Double: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        if (val < axis->getto_axis()) {
          if (QCPRange::validRange(val, axis->getto_axis())) {
            axis->setfrom_axis(val);
            axis->parentPlot()->replot(
                QCustomPlot::RefreshPriority::rpQueuedReplot);
          } else
            emit outofrange(axis->getfrom_axis(), axis->getto_axis());
        }
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_To_Double: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        if (val > axis->getfrom_axis()) {
          if (QCPRange::validRange(axis->getfrom_axis(), val)) {
            axis->setto_axis(val);
            axis->parentPlot()->replot(
                QCustomPlot::RefreshPriority::rpQueuedReplot);
          } else
            emit outofrange(axis->getfrom_axis(), axis->getto_axis());
        }
      }
    } break;
    // ColorMap2D
    case PropertyItem::Property::Plot2DCM_Axis_From_Double: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        if (val < cm->getcolormapscale_colormap()->axis()->range().upper) {
          if (QCPRange::validRange(
                  val,
                  cm->getcolormapscale_colormap()->axis()->range().upper)) {
            cm->getcolormapscale_colormap()->axis()->setRangeLower(val);
            cm->parentPlot()->replot(
                QCustomPlot::RefreshPriority::rpQueuedReplot);
          } else
            emit outofrange(
                cm->getcolormapscale_colormap()->axis()->range().lower,
                cm->getcolormapscale_colormap()->axis()->range().upper);
        }
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_To_Double: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (!Utilities::isSameDouble(val, PropertyData<double>()) && status) {
        if (val > cm->getcolormapscale_colormap()->axis()->range().lower) {
          if (QCPRange::validRange(
                  cm->getcolormapscale_colormap()->axis()->range().lower,
                  val)) {
            cm->getcolormapscale_colormap()->axis()->setRangeUpper(val);
            cm->parentPlot()->replot(
                QCustomPlot::RefreshPriority::rpQueuedReplot);
          } else
            emit outofrange(
                cm->getcolormapscale_colormap()->axis()->range().lower,
                cm->getcolormapscale_colormap()->axis()->range().upper);
        }
      }
    } break;
    default:
      break;
  }
}

void PropertyItem::setBoolValue(const bool &val) {
  bool status = false;
  switch (property_) {
    // Plot2D
    case PropertyItem::Property::Plot2DCanvas_OpenGL: {
      Plot2D *plot = item_->getObjectTreeItem<Plot2D>(&status);
      if (val != PropertyData<bool>() && status) {
        plot->setOpenGl(val);
        plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    // AxisRect 2D
    case PropertyItem::Property::Plot2DLayout_Margin: {
      AxisRect2D *axisrect = item_->getObjectTreeItem<AxisRect2D>(&status);
      if (val != PropertyData<bool>() && status) {
        axisrect->setAutoMarginsBool(val);
        axisrect->layer()->replot();
        foreach (auto *item, childItems_) { item->setReadonly(val); }
      }
    } break;
    // Legend 2D
    case PropertyItem::Property::Plot2DLegend_Visible: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<bool>() && status) {
        legend->setVisible(val);
        legend->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Title_Visible: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<bool>() && status) {
        (val) ? legend->addtitle_legend() : legend->removetitle_legend();
        legend->layer()->replot();
        foreach (auto *item, childItems_) { item->setReadonly(!val); }
      }
    } break;
    // Axis 2D
    case PropertyItem::Property::Plot2DAxis_Visible: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<bool>() && status) {
        axis->setshowhide_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Inverted: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<bool>() && status) {
        axis->setinverted_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Antialiased: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<bool>() && status) {
        axis->setantialiased_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Ticks_Visible: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<bool>() && status) {
        axis->settickvisibility_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_SubTicks_Visible: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<bool>() && status) {
        axis->setsubtickvisibility_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Visible: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<bool>() && status) {
        axis->setticklabelvisibility_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
      // Grid 2D
    case PropertyItem::Property::Plot2DGrid_Horizontal_Major: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<bool>() && status) {
        gp->getXgrid()->setMajorGridVisible(val);
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Line: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<bool>() && status) {
        gp->getXgrid()->setZerothLineVisible(val);
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Horizontal_Minor: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<bool>() && status) {
        gp->getXgrid()->setMinorGridVisible(val);
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Major: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<bool>() && status) {
        gp->getYgrid()->setMajorGridVisible(val);
        gp->getYgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Line: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<bool>() && status) {
        gp->getYgrid()->setZerothLineVisible(val);
        gp->getYgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Minor: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<bool>() && status) {
        gp->getYgrid()->setMinorGridVisible(val);
        gp->getYgrid()->layer()->replot();
      }
    } break;
    // TextItem2D
    case PropertyItem::Property::Plot2DTextItem_Antialiased: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<bool>() && status) {
        textitem->setAntialiased(val);
        textitem->layer()->replot();
      }
    } break;
    // LineItem2D
    case PropertyItem::Property::Plot2DLineItem_Antialiased: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (val != PropertyData<bool>() && status) {
        lineitem->setAntialiased(val);
        lineitem->layer()->replot();
      }
    } break;
    // Curve2D
    case PropertyItem::Property::Plot2DCurve_Line_Antialiased: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<bool>() && status) {
        curve->setlineantialiased_cplot(val);
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Scatter_Antialiased: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<bool>() && status) {
        curve->setscatterantialiased_cplot(val);
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Legend_Status: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<bool>() && status) {
        curve->setlegendvisible_cplot(val);
        curve->layer()->replot();
      }
    } break;
    // Graph2D
    case PropertyItem::Property::Plot2DLS_Line_Antialiased: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<bool>() && status) {
        ls->setlineantialiased_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Scatter_Antialiased: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<bool>() && status) {
        ls->setscatterantialiased_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Legend_Status: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<bool>() && status) {
        ls->setlegendvisible_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    // Channel2D
    case PropertyItem::Property::Plot2DChannel_Line1_Antialiased: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<bool>() && status) {
        channel->getChannelFirst()->setlineantialiased_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter1_Antialiased: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<bool>() && status) {
        channel->getChannelFirst()->setscatterantialiased_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Line2_Antialiased: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<bool>() && status) {
        channel->getChannelSecond()->setlineantialiased_lsplot(val);
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter2_Antialiased: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<bool>() && status) {
        channel->getChannelSecond()->setscatterantialiased_lsplot(val);
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Legend_Status: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<bool>() && status) {
        channel->getChannelFirst()->setlegendvisible_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    // StatBox2D
    case PropertyItem::Property::Plot2DStatBox_Box_Antialiased: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<bool>() && status) {
        sb->setAntialiased(val);
        sb->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Box_Fill_Antialiased: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<bool>() && status) {
        sb->setAntialiasedFill(val);
        sb->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Whisker_Antialiased: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<bool>() && status) {
        sb->setWhiskerAntialiased(val);
        sb->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Scatter_Antialiased: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<bool>() && status) {
        sb->setAntialiasedScatters(val);
        sb->layer()->replot();
      }
    } break;
    // Vector2D
    case PropertyItem::Property::Plot2DVector_Line_Antialiased: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (val != PropertyData<bool>() && status) {
        vec->setlineantialiased_vecplot(val);
        vec->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DVector_Legend_Status: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (val != PropertyData<bool>() && status) {
        vec->setlegendvisible_vecplot(val);
        vec->layer()->replot();
      }
    } break;
    // Bar2D
    case PropertyItem::Property::Plot2DBar_Line_Antialiased: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<bool>() && status) {
        bar->setAntialiased(val);
        bar->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Fill_Antialiased: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<bool>() && status) {
        bar->setAntialiasedFill(val);
        bar->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Histogram_AutoBin: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<bool>() && status && bar->ishistogram_barplot()) {
        bar->setHistAutoBin(val);
        bar->layer()->replot();
        foreach (auto *item, childItems_) { item->setReadonly(val); }
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Legend_Status: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<bool>() && status) {
        bar->setlegendvisible_barplot(val);
        bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // ColorMap2D
    case PropertyItem::Property::Plot2DCM_Interpolate: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->setInterpolate(val);
        cm->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Tight_Boundary: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->setTightBoundary(val);
        cm->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Gradient_Invert: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->setgradientinverted_colormap(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Periodic: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->setgradientperiodic_colormap(val);
        cm->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Scale_Visible: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->getcolormapscale_colormap()->setVisible(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Visible: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->getcolormapscale_colormap()->axis()->setVisible(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Inverted: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->getcolormapscale_colormap()->axis()->setRangeReversed(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Antialiased: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->getcolormapscale_colormap()->axis()->setAntialiased(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_Visible: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->getcolormapscale_colormap()->axis()->setTicks(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Visible: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->getcolormapscale_colormap()->axis()->setSubTicks(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Visible: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<bool>() && status) {
        cm->getcolormapscale_colormap()->axis()->setTickLabels(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    // ErrorBar2D
    case PropertyItem::Property::Plot2DErrBar_Antialiased: {
      ErrorBar2D *err = item_->getObjectTreeItem<ErrorBar2D>(&status);
      if (val != PropertyData<bool>() && status) {
        err->setAntialiased(val);
        err->layer()->replot();
      }
    } break;
    default:
      break;
  }
}

void PropertyItem::setEnumValue(const int &val) {
  bool status = false;
  switch (property_) {
    // AxisRect2D
    case PropertyItem::Property::Plot2DLayout_FillStyle: {
      AxisRect2D *axisrect = item_->getObjectTreeItem<AxisRect2D>(&status);
      if (val != PropertyData<int>() && status) {
        QBrush br = axisrect->backgroundBrush();
        br.setStyle(static_cast<Qt::BrushStyle>(val));
        axisrect->setBackground(br);
        axisrect->layer()->replot();
      }
    } break;
    // Legend2D
    case PropertyItem::Property::Plot2DLegend_Direction: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        legend->setdirection_legend(val);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_FillStyle: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        QBrush br = legend->brush();
        br.setStyle(static_cast<Qt::BrushStyle>(val));
        legend->setBrush(br);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Stroke_Style: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<int>() && status) {
        legend->setborderstrokestyle_legend(static_cast<Qt::PenStyle>(val));
        legend->layer()->replot();
      }
    } break;
    // Axis2D
    case PropertyItem::Property::Plot2DAxis_Upper_Ending: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setUpperEnding(static_cast<QCPLineEnding::EndingStyle>(val));
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Lower_Ending: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setLowerEnding(static_cast<QCPLineEnding::EndingStyle>(val));
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Type: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setscaletype_axis(static_cast<Axis2D::AxisScaleType>(val));
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Stroke_Style: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setstroketype_axis(static_cast<Qt::PenStyle>(val));
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Style: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->settickstrokestyle_axis(static_cast<Qt::PenStyle>(val));
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Style: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setsubtickstrokestyle_axis(static_cast<Qt::PenStyle>(val));
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_StepStrategy: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->getticker_axis()->setTickStepStrategy(
            static_cast<QCPAxisTicker::TickStepStrategy>(val));
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Side: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        axis->setticklabelside_axis(static_cast<Axis2D::AxisLabelSide>(val));
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Format_Double: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        if (axis->gettickertype_axis() == Axis2D::TickerType::Value ||
            axis->gettickertype_axis() == Axis2D::TickerType::Pi ||
            axis->gettickertype_axis() == Axis2D::TickerType::Log) {
          axis->setticklabelformat_axis(
              static_cast<Axis2D::AxisLabelFormat>(val));
          axis->parentPlot()->replot(
              QCustomPlot::RefreshPriority::rpQueuedReplot);
        }
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_FractionStyle_Pi: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<int>() && status) {
        QSharedPointer<QCPAxisTickerPi> piticker =
            qSharedPointerCast<QCPAxisTickerPi>(axis->getticker_axis());
        if (piticker) {
          piticker->setFractionStyle(
              static_cast<QCPAxisTickerPi::FractionStyle>(val));
          axis->parentPlot()->replot(
              QCustomPlot::RefreshPriority::rpQueuedReplot);
        }
      }
    } break;
    // Grid2D
    case PropertyItem::Property::Plot2DGrid_Horizontal: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> xaxes =
            gp->getXgridAxis()->getaxisrect_axis()->getXAxes2D();
        xaxes.at(val)->getaxisrect_axis()->bindGridTo(xaxes.at(val));
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> yaxes =
            gp->getYgridAxis()->getaxisrect_axis()->getYAxes2D();
        yaxes.at(val)->getaxisrect_axis()->bindGridTo(yaxes.at(val));
        gp->getYgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Style: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<int>() && status) {
        gp->getXgrid()->setMajorGridStyle(static_cast<Qt::PenStyle>(val));
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Style: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<int>() && status) {
        gp->getXgrid()->setZerothLineStyle(static_cast<Qt::PenStyle>(val));
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Style: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<int>() && status) {
        gp->getXgrid()->setMinorGridStyle(static_cast<Qt::PenStyle>(val));
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Style: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<int>() && status) {
        gp->getYgrid()->setMajorGridStyle(static_cast<Qt::PenStyle>(val));
        gp->getYgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Style: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<int>() && status) {
        gp->getYgrid()->setZerothLineStyle(static_cast<Qt::PenStyle>(val));
        gp->getYgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Style: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<int>() && status) {
        gp->getYgrid()->setMinorGridStyle(static_cast<Qt::PenStyle>(val));
        gp->getYgrid()->layer()->replot();
      }
    } break;
    // TextItem2D
    case PropertyItem::Property::Plot2DTextItem_Text_Alignment: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        textitem->settextalignment_textitem(
            static_cast<TextItem2D::TextAlignment>(val));
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Stroke_Style: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        textitem->setstrokestyle_textitem(static_cast<Qt::PenStyle>(val));
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Fill_Style: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        QBrush b = textitem->brush();
        b.setStyle(static_cast<Qt::BrushStyle>(val));
        textitem->setBrush(b);
        textitem->layer()->replot();
      }
    } break;
    // LineItem2D
    case PropertyItem::Property::Plot2DLineItem_Stroke_Style: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        lineitem->setstrokestyle_lineitem(static_cast<Qt::PenStyle>(val));
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Starting_Style: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        lineitem->setendstyle_lineitem(
            LineItem2D::LineEndLocation::Start,
            static_cast<QCPLineEnding::EndingStyle>(val));
        lineitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLineItem_Ending_Style: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        lineitem->setendstyle_lineitem(
            LineItem2D::LineEndLocation::Stop,
            static_cast<QCPLineEnding::EndingStyle>(val));
        lineitem->layer()->replot();
      }
    } break;
    // ImageItem2D
    case PropertyItem::Property::Plot2DImageItem_Stroke_Style: {
      ImageItem2D *imageitem = item_->getObjectTreeItem<ImageItem2D>(&status);
      if (val != PropertyData<int>() && status) {
        imageitem->setstrokestyle_imageitem(static_cast<Qt::PenStyle>(val));
        imageitem->layer()->replot();
      }
    } break;
    // Curve2D
    case PropertyItem::Property::Plot2DCurve_Axis_X: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> xaxes =
            curve->getxaxis()->getaxisrect_axis()->getXAxes2D();
        if (val > xaxes.count() - 1) {
          qDebug() << "x axes index out of bounds " << val;
          return;
        }
        curve->setxaxis_cplot(xaxes.at(val));
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Axis_Y: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> yaxes =
            curve->getxaxis()->getaxisrect_axis()->getYAxes2D();
        if (val > yaxes.count() - 1) {
          qDebug() << "y axes index out of bounds " << val;
          return;
        }
        curve->setyaxis_cplot(yaxes.at(val));
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Line_Style: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<int>() && status) {
        curve->setlinetype_cplot(val);
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Line_Stroke_Style: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<int>() && status) {
        curve->setlinestrokestyle_cplot(static_cast<Qt::PenStyle>(val));
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Fill_Style: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<int>() && status) {
        curve->setlinefillstyle_cplot(static_cast<Qt::BrushStyle>(val));
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Scatter_Style: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<int>() && status) {
        curve->setscattershape_cplot(
            static_cast<Graph2DCommon::ScatterStyle>(val));
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Scatter_Outline_Style: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<int>() && status) {
        curve->setscatterstrokestyle_cplot(static_cast<Qt::PenStyle>(val));
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Scatter_Fill_Style: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<int>() && status) {
        ls->setscatterfillstyle_lsplot(static_cast<Qt::BrushStyle>(val));
        ls->layer()->replot();
      }
    } break;
    // Graph2D
    case PropertyItem::Property::Plot2DLS_Axis_X: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> xaxes =
            ls->getxaxis()->getaxisrect_axis()->getXAxes2D();
        if (val > xaxes.count() - 1) {
          qDebug() << "x axes index out of bounds " << val;
          return;
        }
        ls->setxaxis_lsplot(xaxes.at(val));
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Axis_Y: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> yaxes =
            ls->getxaxis()->getaxisrect_axis()->getYAxes2D();
        if (val > yaxes.count() - 1) {
          qDebug() << "y axes index out of bounds " << val;
          return;
        }
        ls->setyaxis_lsplot(yaxes.at(val));
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Line_Style: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<int>() && status) {
        ls->setlinetype_lsplot(static_cast<Graph2DCommon::LineStyleType>(val));
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Line_Stroke_Style: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<int>() && status) {
        ls->setlinestrokestyle_lsplot(static_cast<Qt::PenStyle>(val));
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Fill_Style: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<int>() && status) {
        ls->setlinefillstyle_lsplot(static_cast<Qt::BrushStyle>(val));
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Scatter_Style: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<int>() && status) {
        ls->setscattershape_lsplot(
            static_cast<Graph2DCommon::ScatterStyle>(val));
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Scatter_Outline_Style: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<int>() && status) {
        ls->setscatterstrokestyle_lsplot(static_cast<Qt::PenStyle>(val));
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Scatter_Fill_Style: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<int>() && status) {
        ls->setscatterfillstyle_lsplot(static_cast<Qt::BrushStyle>(val));
        ls->layer()->replot();
      }
    } break;
    // Channel2D
    case PropertyItem::Property::Plot2DChannel_Axis_X: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        LineSpecial2D *ls1 = channel->getChannelFirst();
        LineSpecial2D *ls2 = channel->getChannelSecond();
        if (!ls1 || !ls2) return;
        QList<Axis2D *> xaxes =
            ls1->getxaxis()->getaxisrect_axis()->getXAxes2D();
        if (val > xaxes.count() - 1) {
          qDebug() << "x axes index out of bounds " << val;
          return;
        }

        ls1->setxaxis_lsplot(xaxes.at(val));
        ls2->setxaxis_lsplot(xaxes.at(val));
        ls1->layer()->replot();
        ls2->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Axis_Y: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        LineSpecial2D *ls1 = channel->getChannelFirst();
        LineSpecial2D *ls2 = channel->getChannelSecond();
        if (!ls1 || !ls2) return;
        QList<Axis2D *> yaxes =
            ls1->getxaxis()->getaxisrect_axis()->getYAxes2D();
        if (val > yaxes.count() - 1) {
          qDebug() << "y axes index out of bounds " << val;
          return;
        }
        ls1->setyaxis_lsplot(yaxes.at(val));
        ls2->setyaxis_lsplot(yaxes.at(val));
        ls1->layer()->replot();
        ls2->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Line1_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelFirst()->setlinetype_lsplot(
            static_cast<Graph2DCommon::LineStyleType>(val));
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Line1_Stroke_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelFirst()->setlinestrokestyle_lsplot(
            static_cast<Qt::PenStyle>(val));
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Fill_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelFirst()->setlinefillstyle_lsplot(
            static_cast<Qt::BrushStyle>(val));
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter1_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelFirst()->setscattershape_lsplot(
            static_cast<Graph2DCommon::ScatterStyle>(val));
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelFirst()->setscatterstrokestyle_lsplot(
            static_cast<Qt::PenStyle>(val));
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter1_Fill_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelFirst()->setscatterfillstyle_lsplot(
            static_cast<Qt::BrushStyle>(val));
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Line2_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelSecond()->setlinetype_lsplot(
            static_cast<Graph2DCommon::LineStyleType>(val));
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Line2_Stroke_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelSecond()->setlinestrokestyle_lsplot(
            static_cast<Qt::PenStyle>(val));
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter2_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelSecond()->setscattershape_lsplot(
            static_cast<Graph2DCommon::ScatterStyle>(val));
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelSecond()->setscatterstrokestyle_lsplot(
            static_cast<Qt::PenStyle>(val));
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter2_Fill_Style: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<int>() && status) {
        channel->getChannelSecond()->setscatterfillstyle_lsplot(
            static_cast<Qt::BrushStyle>(val));
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    // StatBox2D
    case PropertyItem::Property::Plot2DStatBox_Axis_X: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> xaxes =
            sb->getxaxis()->getaxisrect_axis()->getXAxes2D();
        if (val > xaxes.count() - 1) {
          qDebug() << "x axes index out of bounds " << val;
          return;
        }
        sb->setxaxis_statbox(xaxes.at(val));
        sb->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Axis_Y: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> yaxes =
            sb->getxaxis()->getaxisrect_axis()->getYAxes2D();
        if (val > yaxes.count() - 1) {
          qDebug() << "y axes index out of bounds " << val;
          return;
        }
        sb->setyaxis_statbox(yaxes.at(val));
        sb->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Box_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setboxstyle_statbox(static_cast<StatBox2D::BoxWhiskerStyle>(val));
        sb->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Box_Stroke_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        QPen p = sb->pen();
        p.setStyle(static_cast<Qt::PenStyle>(val));
        sb->setPen(p);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Box_Fill_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setfillstyle_statbox(static_cast<Qt::BrushStyle>(val));
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Whisker_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setwhiskerstyle_statbox(
            static_cast<StatBox2D::BoxWhiskerStyle>(val));
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setwhiskerstrokestyle_statbox(static_cast<Qt::PenStyle>(val));
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setwhiskerbarstrokestyle_statbox(static_cast<Qt::PenStyle>(val));
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Median_Stroke_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setmedianstrokestyle_statbox(static_cast<Qt::PenStyle>(val));
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Scatter_Show: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setOutlierScatter(static_cast<StatBox2D::Scatter>(val));
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Scatter_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setscattershape_statbox(
            static_cast<Graph2DCommon::ScatterStyle>(val));
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setscatterstrokestyle_statbox(static_cast<Qt::PenStyle>(val));
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Scatter_Fill_Style: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<int>() && status) {
        sb->setscatterfillstyle_statbox(static_cast<Qt::BrushStyle>(val));
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Vector2D
    case PropertyItem::Property::Plot2DVector_Axis_X: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> xaxes =
            vec->getxaxis()->getaxisrect_axis()->getXAxes2D();
        if (val > xaxes.count() - 1) {
          qDebug() << "x axes index out of bounds " << val;
          return;
        }
        vec->setxaxis_vecplot(xaxes.at(val));
        vec->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DVector_Axis_Y: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> yaxes =
            vec->getxaxis()->getaxisrect_axis()->getYAxes2D();
        if (val > yaxes.count() - 1) {
          qDebug() << "y axes index out of bounds " << val;
          return;
        }
        vec->setyaxis_vecplot(yaxes.at(val));
        vec->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DVector_Line_Stroke_Style: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (val != PropertyData<int>() && status) {
        vec->setlinestrokestyle_vecplot(static_cast<Qt::PenStyle>(val));
        vec->layer()->replot();
        vec->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DVector_Ending_Style: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (val != PropertyData<int>() && status) {
        vec->setendstyle_vecplot(static_cast<Vector2D::LineEnd>(val),
                                 Vector2D::LineEndLocation::Head);
        vec->layer()->replot();
        vec->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Bar2D
    case PropertyItem::Property::Plot2DBar_Axis_X: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> xaxes =
            bar->getxaxis()->getaxisrect_axis()->getXAxes2D();
        if (val > xaxes.count() - 1) {
          qDebug() << "x axes index out of bounds " << val;
          return;
        }
        bar->setxaxis_barplot(xaxes.at(val));
        bar->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Axis_Y: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<int>() && status) {
        QList<Axis2D *> yaxes =
            bar->getxaxis()->getaxisrect_axis()->getYAxes2D();
        if (val > yaxes.count() - 1) {
          qDebug() << "y axes index out of bounds " << val;
          return;
        }
        bar->setyaxis_barplot(yaxes.at(val));
        bar->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Line_Stroke_Style: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<int>() && status) {
        bar->setstrokestyle_barplot(static_cast<Qt::PenStyle>(val));
        bar->layer()->replot();
        bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Fill_Style: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<int>() && status) {
        bar->setfillstyle_barplot(static_cast<Qt::BrushStyle>(val));
        bar->layer()->replot();
        bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Pie2D
    case PropertyItem::Property::Plot2DPie_Style: {
      Pie2D *pie = item_->getObjectTreeItem<Pie2D>(&status);
      if (val != PropertyData<int>() && status) {
        pie->setstyle_pieplot(static_cast<Graph2DCommon::PieStyle>(val));
        pie->layer()->replot();
        pie->getaxisrect()->replotBareBones();
        pie->getaxisrect()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DPie_Stroke_Style: {
      Pie2D *pie = item_->getObjectTreeItem<Pie2D>(&status);
      if (val != PropertyData<int>() && status) {
        pie->setstrokestyle_pieplot(static_cast<Qt::PenStyle>(val));
        pie->layer()->replot();
        pie->getaxisrect()->replotBareBones();
        pie->getaxisrect()->getLegend()->layer()->replot();
      }
    } break;
    // ColorMap2D
    case PropertyItem::Property::Plot2DCM_Gradient: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->setgradient_colormap(static_cast<ColorMap2D::Gradient>(val));
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Type: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setScaleType(
            static_cast<QCPAxis::ScaleType>(val));
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Stroke_Style: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        QPen p = cm->getcolormapscale_colormap()->axis()->basePen();
        p.setStyle(static_cast<Qt::PenStyle>(val));
        cm->getcolormapscale_colormap()->axis()->setBasePen(p);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Style: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        QPen p = cm->getcolormapscale_colormap()->axis()->tickPen();
        p.setStyle(static_cast<Qt::PenStyle>(val));
        cm->getcolormapscale_colormap()->axis()->setTickPen(p);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Style: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        QPen p = cm->getcolormapscale_colormap()->axis()->subTickPen();
        p.setStyle(static_cast<Qt::PenStyle>(val));
        cm->getcolormapscale_colormap()->axis()->setSubTickPen(p);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Side: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->getcolormapscale_colormap()->axis()->setTickLabelSide(
            static_cast<QCPAxis::LabelSide>(val));
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Format_Double: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<int>() && status) {
        cm->setcolormapscaleticklabelformat_axis(
            static_cast<Axis2D::AxisLabelFormat>(val));
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    // ErrorBar2D
    case PropertyItem::Property::Plot2DErrBar_Stroke_Style: {
      ErrorBar2D *err = item_->getObjectTreeItem<ErrorBar2D>(&status);
      if (val != PropertyData<int>() && status) {
        QPen p = err->pen();
        p.setStyle(static_cast<Qt::PenStyle>(val));
        err->setPen(p);
        err->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DErrBar_Fill_Style: {
      ErrorBar2D *err = item_->getObjectTreeItem<ErrorBar2D>(&status);
      if (val != PropertyData<int>() && status) {
        QBrush b = err->brush();
        b.setStyle(static_cast<Qt::BrushStyle>(val));
        err->setBrush(b);
        err->layer()->replot();
      }
    } break;
    // Plot3D Canvas
    case PropertyItem::Property::Plot3DCanvas_Theme: {
      QAbstract3DGraph *plot =
          item_->getObjectTreeItem<QAbstract3DGraph>(&status);
      if (val != PropertyData<int>() && status) {
        plot->activeTheme()->setType(static_cast<Q3DTheme::Theme>(val));
      }
    } break;
    default:
      break;
  }
}

void PropertyItem::setColorValue(const QColor &val) {
  bool status = false;
  switch (property_) {
    // Plot2D
    case PropertyItem::Property::Plot2DCanvas_Background: {
      Plot2D *plot = item_->getObjectTreeItem<Plot2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        plot->setBackgroundColor(val);
        plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    // AxisRect2D
    case PropertyItem::Property::Plot2DLayout_Background: {
      AxisRect2D *axisrect = item_->getObjectTreeItem<AxisRect2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        QBrush br = axisrect->backgroundBrush();
        br.setColor(val);
        axisrect->setBackground(br);
        axisrect->layer()->replot();
      }
    } break;
    // Legend2D
    case PropertyItem::Property::Plot2DLegend_Text_Color: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        legend->setTextColor(val);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Background: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        QBrush b = legend->brush();
        b.setColor(val);
        legend->setBrush(b);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Stroke_Color: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        legend->setborderstrokecolor_legend(val);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Title_Color: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        legend->settitlecolor_legend(val);
        legend->layer()->replot();
      }
    } break;
    // Axis2D
    case PropertyItem::Property::Plot2DAxis_Stroke_Color: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        axis->setstrokecolor_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Label_Color: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        axis->setlabelcolor_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Color: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        axis->settickstrokecolor_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Color: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        axis->setsubtickstrokecolor_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Color: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        axis->setticklabelcolor_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    // Grid2D
    case PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Color: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        gp->getXgrid()->setMajorGridColor(val);
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Color: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        gp->getXgrid()->setZerothLineColor(val);
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Color: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        gp->getXgrid()->setMinorGridColor(val);
        gp->getXgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Color: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        gp->getYgrid()->setMajorGridColor(val);
        gp->getYgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Color: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        gp->getYgrid()->setZerothLineColor(val);
        gp->getYgrid()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Color: {
      GridPair2D *gp = item_->getObjectTreeItem<GridPair2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        gp->getYgrid()->setMinorGridColor(val);
        gp->getYgrid()->layer()->replot();
      }
    } break;
    // TextItem2D
    case PropertyItem::Property::Plot2DTextItem_Text_Color: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        textitem->setColor(val);
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Stroke_Color: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        textitem->setstrokecolor_textitem(val);
        textitem->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DTextItem_Background_Color: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        QBrush b = textitem->brush();
        b.setColor(val);
        textitem->setBrush(b);
        textitem->layer()->replot();
      }
    } break;
    // LineItem2D
    case PropertyItem::Property::Plot2DLineItem_Stroke_Color: {
      LineItem2D *lineitem = item_->getObjectTreeItem<LineItem2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        lineitem->setstrokecolor_lineitem(val);
        lineitem->layer()->replot();
      }
    } break;
    // ImageItem2D
    case PropertyItem::Property::Plot2DImageItem_Stroke_Color: {
      ImageItem2D *imageitem = item_->getObjectTreeItem<ImageItem2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        imageitem->setstrokecolor_imageitem(val);
        imageitem->layer()->replot();
      }
    } break;
    // Curve2D
    case PropertyItem::Property::Plot2DCurve_Line_Stroke_Color: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        curve->setlinestrokecolor_cplot(val);
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Fill_Color: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        curve->setlinefillcolor_cplot(val);
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Scatter_Outline_Color: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        curve->setscatterstrokecolor_cplot(val);
        curve->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DCurve_Scatter_Fill_Color: {
      Curve2D *curve = item_->getObjectTreeItem<Curve2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        curve->setscatterfillcolor_cplot(val);
        curve->layer()->replot();
      }
    } break;
    // Graph2D
    case PropertyItem::Property::Plot2DLS_Line_Stroke_Color: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        ls->setlinestrokecolor_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Fill_Color: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        ls->setlinefillcolor_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Scatter_Outline_Color: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        ls->setscatterstrokecolor_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLS_Scatter_Fill_Color: {
      LineSpecial2D *ls = item_->getObjectTreeItem<LineSpecial2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        ls->setscatterfillcolor_lsplot(val);
        ls->layer()->replot();
      }
    } break;
    // Channel2D
    case PropertyItem::Property::Plot2DChannel_Line1_Stroke_Color: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        channel->getChannelFirst()->setlinestrokecolor_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Fill_Color: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        channel->getChannelFirst()->setlinefillcolor_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Color: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        channel->getChannelFirst()->setscatterstrokecolor_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter1_Fill_Color: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        channel->getChannelFirst()->setscatterfillcolor_lsplot(val);
        channel->getChannelFirst()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Line2_Stroke_Color: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        channel->getChannelSecond()->setlinestrokecolor_lsplot(val);
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Color: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        channel->getChannelSecond()->setscatterstrokecolor_lsplot(val);
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DChannel_Scatter2_Fill_Color: {
      Channel2D *channel = item_->getObjectTreeItem<Channel2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        channel->getChannelSecond()->setscatterfillcolor_lsplot(val);
        channel->getChannelSecond()->layer()->replot();
      }
    } break;
    // StatBox2D
    case PropertyItem::Property::Plot2DStatBox_Box_Stroke_Color: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        QPen p = sb->pen();
        p.setColor(val);
        sb->setPen(p);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Box_Fill_Color: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        sb->setfillcolor_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Color: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        sb->setwhiskerstrokecolor_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Color: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        sb->setwhiskerbarstrokecolor_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Median_Stroke_Color: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        sb->setmedianstrokecolor_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Color: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        sb->setscatterstrokecolor_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DStatBox_Scatter_Fill_Color: {
      StatBox2D *sb = item_->getObjectTreeItem<StatBox2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        sb->setscatterfillcolor_statbox(val);
        sb->layer()->replot();
        sb->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Vector2D
    case PropertyItem::Property::Plot2DVector_Line_Stroke_Color: {
      Vector2D *vec = item_->getObjectTreeItem<Vector2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        vec->setlinestrokecolor_vecplot(val);
        vec->layer()->replot();
        vec->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    // Bar2D
    case PropertyItem::Property::Plot2DBar_Line_Stroke_Color: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        bar->setstrokecolor_barplot(val);
        bar->layer()->replot();
        bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DBar_Fill_Color: {
      Bar2D *bar = item_->getObjectTreeItem<Bar2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        bar->setfillcolor_barplot(val);
        bar->layer()->replot();
        bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DPie_Stroke_Color: {
      Pie2D *pie = item_->getObjectTreeItem<Pie2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        pie->setstrokecolor_pieplot(val);
        pie->layer()->replot();
        pie->getaxisrect()->getLegend()->layer()->replot();
      }
    } break;
      // ColorMap2D
    case PropertyItem::Property::Plot2DCM_Axis_Stroke_Color: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        QPen p = cm->getcolormapscale_colormap()->axis()->basePen();
        p.setColor(val);
        cm->getcolormapscale_colormap()->axis()->setBasePen(p);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Label_Color: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        cm->getcolormapscale_colormap()->axis()->setLabelColor(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Color: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        QPen p = cm->getcolormapscale_colormap()->axis()->tickPen();
        p.setColor(val);
        cm->getcolormapscale_colormap()->axis()->setTickPen(p);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Color: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        QPen p = cm->getcolormapscale_colormap()->axis()->subTickPen();
        p.setColor(val);
        cm->getcolormapscale_colormap()->axis()->setSubTickPen(p);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Color: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        cm->getcolormapscale_colormap()->axis()->setTickLabelColor(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    // ErrorBar2D
    case PropertyItem::Property::Plot2DErrBar_Stroke_Color: {
      ErrorBar2D *err = item_->getObjectTreeItem<ErrorBar2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        QPen p = err->pen();
        p.setColor(val);
        err->setPen(p);
        err->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DErrBar_Fill_Color: {
      ErrorBar2D *err = item_->getObjectTreeItem<ErrorBar2D>(&status);
      if (val != PropertyData<QColor>() && status) {
        QBrush b = err->brush();
        b.setColor(val);
        err->setBrush(b);
        err->layer()->replot();
      }
    } break;
    default:
      break;
  }
}

void PropertyItem::setFontValue(const QFont &val) {
  bool status = false;
  switch (property_) {
    case PropertyItem::Property::Plot2DLegend_Font: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<QFont>() && status) {
        legend->setFont(val);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DLegend_Title_Font: {
      Legend2D *legend = item_->getObjectTreeItem<Legend2D>(&status);
      if (val != PropertyData<QFont>() && status) {
        legend->settitlefont_legend(val);
        legend->layer()->replot();
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_Label_Font: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QFont>() && status) {
        axis->setlabelfont_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_TickLabel_Font: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QFont>() && status) {
        axis->setticklabelfont_axis(val);
        axis->parentPlot()->replot(
            QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    // TextItem2D
    case PropertyItem::Property::Plot2DTextItem_Text_Font: {
      TextItem2D *textitem = item_->getObjectTreeItem<TextItem2D>(&status);
      if (val != PropertyData<QFont>() && status) {
        textitem->setFont(val);
        textitem->layer()->replot();
      }
    } break;
    // ColorMap2D
    case PropertyItem::Property::Plot2DCM_Axis_Label_Font: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<QFont>() && status) {
        cm->getcolormapscale_colormap()->axis()->setLabelFont(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    case PropertyItem::Property::Plot2DCM_Axis_TickLabel_Font: {
      ColorMap2D *cm = item_->getObjectTreeItem<ColorMap2D>(&status);
      if (val != PropertyData<QFont>() && status) {
        cm->getcolormapscale_colormap()->axis()->setTickLabelFont(val);
        cm->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    } break;
    default:
      break;
  }
}

void PropertyItem::setDateTimeValue(const QDateTime &val) {
  bool status = false;
  switch (property_) {
    case PropertyItem::Property::Plot2DAxis_From_DateTime: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QDateTime>() && status) {
        if (QCPAxisTickerDateTime::dateTimeToKey(val) < axis->getto_axis()) {
          if (QCPRange::validRange(QCPAxisTickerDateTime::dateTimeToKey(val),
                                   axis->getto_axis())) {
            axis->setfrom_axis(QCPAxisTickerDateTime::dateTimeToKey(val));
            axis->parentPlot()->replot(
                QCustomPlot::RefreshPriority::rpQueuedReplot);
          } else
            emit outofrange(axis->getfrom_axis(), axis->getto_axis());
        }
      }
    } break;
    case PropertyItem::Property::Plot2DAxis_To_DateTime: {
      Axis2D *axis = item_->getObjectTreeItem<Axis2D>(&status);
      if (val != PropertyData<QDateTime>() && status) {
        if (QCPAxisTickerDateTime::dateTimeToKey(val) > axis->getfrom_axis()) {
          if (QCPRange::validRange(axis->getfrom_axis(),
                                   QCPAxisTickerDateTime::dateTimeToKey(val))) {
            axis->setto_axis(QCPAxisTickerDateTime::dateTimeToKey(val));
            axis->parentPlot()->replot(
                QCustomPlot::RefreshPriority::rpQueuedReplot);
          } else
            emit outofrange(axis->getfrom_axis(), axis->getto_axis());
        }
      }
    } break;
    default:
      break;
  }
}

void PropertyItem::setReadonly(const bool status) {
  readonly_ = status;
  emit readonlyChanged(status);
}
