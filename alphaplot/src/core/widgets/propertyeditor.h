/* This file is part of AlphaPlot.
   Copyright 2016 - 2020, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : PropertyEditor */

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QDockWidget>

#include "2Dplot/Axis2D.h"
#include "2Dplot/TextItem2D.h"
#include "ApplicationWindow.h"
#include "MyTreeWidget.h"

class Ui_PropertyEditor;
class QSplitter;
class MyWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QtAbstractPropertyBrowser;
class Layout2D;
class AxisRect2D;
class Grid2D;
class LineSpecial2D;
class Curve2D;
class Vector2D;
class Bar2D;
// 3D plot
class Surface3D;
class Bar3D;
class Scatter3D;
class DataBlockSurface3D;
class DataBlockBar3D;
class DataBlockScatter3D;
// Property
class QtProperty;
class QtBoolPropertyManager;
class QtIntPropertyManager;
class QtDoublePropertyManager;
class QtStringPropertyManager;
class QtSizePropertyManager;
class QtRectPropertyManager;
class QtEnumPropertyManager;
class QtGroupPropertyManager;
class QtColorPropertyManager;
class QtFontPropertyManager;
class QtCheckBoxFactory;
class QtSpinBoxFactory;
class QtDoubleSpinBoxFactory;
class QtLineEditFactory;
class QtEnumEditorFactory;
class QtColorEditorFactory;
class QtFontEditorFactory;

namespace QtDataVisualization {
class QAbstract3DGraph;
class Q3DTheme;
class QValue3DAxis;
class QValue3DAxis;
class QCategory3DAxis;
}  // namespace QtDataVisualization

class PropertyEditor : public QDockWidget {
  Q_OBJECT
 public:
  PropertyEditor(QWidget *parent = nullptr, ApplicationWindow *app = nullptr);
  ~PropertyEditor();
  MyTreeWidget *getObjectBrowser();
  void myWidgetConnections(MyWidget *widget);
  void tableConnections(Table *table);
  void matrixConnections(Matrix *matrix);
  void axisrectConnections(AxisRect2D *axisrect);

 private slots:
  void valueChange(QtProperty *prop, const bool value);
  void valueChange(QtProperty *prop, const QColor &color);
  void valueChange(QtProperty *prop, const QRect &rect);
  void valueChange(QtProperty *prop, const double &value);
  void valueChange(QtProperty *prop, const QString &value);
  void valueChange(QtProperty *prop, const int value);
  void enumValueChange(QtProperty *prop, const int value);
  void valueChange(QtProperty *prop, const QFont &font);
  void valueChange(QtProperty *prop, const QSize &size);

  void selectObjectItem(QTreeWidgetItem *item);
  void axisRectCreated(AxisRect2D *axisrect, MyWidget *widget);
  void objectschanged();

  // properties block handler
  void WindowPropertyBlock(MyWidget *widget);
  void Plot2DPropertyBlock(Layout2D *layout2d, Plot2D *plotcanvas);
  void Layout2DPropertyBlock(Layout2D *layout2d, AxisRect2D *axisrect);
  void Axis2DPropertyBlock(Axis2D *axis);
  void Grid2DPropertyBlock(AxisRect2D *axisrect);
  void Legend2DPropertyBlock(Legend2D *legend);
  void TextItem2DPropertyBlock(TextItem2D *textitem);
  void LineItem2DPropertyBlock(LineItem2D *lineitem);
  void ImageItem2DPropertyBlock(ImageItem2D *imageitem);
  void LineSpecial2DPropertyBlock(LineSpecial2D *lsgraph, AxisRect2D *axisrect);
  void LineSpecialChannel2DPropertyBlock(LineSpecial2D *lsgraph1,
                                         LineSpecial2D *lsgraph2,
                                         AxisRect2D *axisrect);
  void Curve2DPropertyBlock(Curve2D *curve, AxisRect2D *axisrect);
  void Bar2DPropertyBlock(Bar2D *bargraph, AxisRect2D *axisrect);
  void StatBox2DPropertyBlock(StatBox2D *statbox, AxisRect2D *axisrect);
  void Vector2DPropertyBlock(Vector2D *vectorgraph, AxisRect2D *axisrect);
  void Pie2DPropertyBlock(Pie2D *piegraph, AxisRect2D *axisrect);
  void ColorMap2DPropertyBlock(ColorMap2D *colormap, AxisRect2D *axisrect);
  void ErrorBar2DPropertyBlock(ErrorBar2D *errorbar, AxisRect2D *axisrect);
  // 3dplot
  void Canvas3DPropertyBlock(QtDataVisualization::QAbstract3DGraph *graph);
  void Theme3DPropertyBlock(QtDataVisualization::Q3DTheme *theme);
  void Axis3DValuePropertyBlock(QtDataVisualization::QValue3DAxis *axis);
  void Axis3DCatagoryPropertyBlock(QtDataVisualization::QCategory3DAxis *axis);
  void Surface3DPropertyBlock(Surface3D *surface);
  void Bar3DPropertyBlock(Bar3D *bar);
  void Scatter3DPropertyBlock(Scatter3D *scatter);
  void Surface3DSeriesPropertyBlock(DataBlockSurface3D *block);
  void Bar3DSeriesPropertyBlock(DataBlockBar3D *block);
  void Scatter3DSeriesPropertyBlock(DataBlockScatter3D *block);
  // Table
  void TablePropertyBlock(Table *table);
  // Table
  void MatrixPropertyBlock(Matrix *matrix);

 signals:
  void refreshCanvasRect();

 public slots:
  void populateObjectBrowser(MyWidget *widget);

 private:
  void setObjectPropertyId();
  template <class T>
  T *getgraph2dobject(QTreeWidgetItem *item) {
    void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
    T *object = static_cast<T *>(ptr);
    return object;
  }
  void blockManagers(bool value);

 private:
  ApplicationWindow *app_;
  Ui_PropertyEditor *ui_;
  QSplitter *splitter_;
  MyTreeWidget *objectbrowser_;
  QtAbstractPropertyBrowser *propertybrowser_;
  QList<QTreeWidgetItem *> objectitems_;
  QWidget *previouswidget_;
  // Property
  QtGroupPropertyManager *groupManager_;
  QtBoolPropertyManager *boolManager_;
  QtIntPropertyManager *intManager_;
  QtDoublePropertyManager *doubleManager_;
  QtStringPropertyManager *stringManager_;
  QtSizePropertyManager *sizeManager_;
  QtRectPropertyManager *rectManager_;
  QtEnumPropertyManager *enumManager_;
  QtColorPropertyManager *colorManager_;
  QtFontPropertyManager *fontManager_;
  // Property Widget Factory
  QtCheckBoxFactory *checkBoxFactory_;
  QtSpinBoxFactory *spinBoxFactory_;
  QtDoubleSpinBoxFactory *doubleSpinBoxFactory_;
  QtLineEditFactory *lineEditFactory_;
  QtEnumEditorFactory *comboBoxFactory_;
  QtColorEditorFactory *colorFactory_;
  QtFontEditorFactory *fontFactory_;
  // window properties
  QtProperty *mywidgetwindowrectitem_;
  QtProperty *mywidgetwindownameitem_;
  QtProperty *mywidgetwindowlabelitem_;
  // Plot Canvas properties
  QtProperty *canvaspropertycoloritem_;
  QtProperty *canvaspropertybufferdevicepixelratioitem_;
  QtProperty *canvaspropertyopenglitem_;
  QtProperty *canvaspropertysizeitem_;
  QtProperty *canvaspropertyrowsapcingitem_;
  QtProperty *canvaspropertycolumnsapcingitem_;
  // Layout properties
  QtProperty *layoutpropertymargingroupitem_;
  QtProperty *layoutpropertyrectitem_;
  QtProperty *layoutpropertycoloritem_;
  QtProperty *layoutpropertyfillstyleitem_;
  QtProperty *layoutpropertyautomarginstatusitem_;
  QtProperty *layoutpropertyleftmarginitem_;
  QtProperty *layoutpropertyrightmarginitem_;
  QtProperty *layoutpropertytopmarginitem_;
  QtProperty *layoutpropertybottommarginitem_;
  QtProperty *layoutpropertyrowstreachfactoritem_;
  QtProperty *layoutpropertycolumnstreachfactoritem_;

  // Axis Properties General Block
  QtProperty *axispropertyvisibleitem_;
  QtProperty *axispropertyoffsetitem_;
  QtProperty *axispropertyfromitem_;
  QtProperty *axispropertytoitem_;
  QtProperty *axispropertyupperendingstyleitem_;
  QtProperty *axispropertylowerendingstyleitem_;
  QtProperty *axispropertylinlogitem_;
  QtProperty *axispropertyinvertitem_;
  QtProperty *axispropertystrokecoloritem_;
  QtProperty *axispropertystrokethicknessitem_;
  QtProperty *axispropertystroketypeitem_;
  QtProperty *axispropertyantialiaseditem_;
  // Axis Properties Label sub block
  QtProperty *axispropertylabeltextitem_;
  QtProperty *axispropertylabelfontitem_;
  QtProperty *axispropertylabelcoloritem_;
  QtProperty *axispropertylabelpaddingitem_;
  // Axis Properties Ticks sub block
  QtProperty *axispropertytickvisibilityitem_;
  QtProperty *axispropertytickcountitem_;
  QtProperty *axispropertytickoriginitem_;
  QtProperty *axispropertyticklengthinitem_;
  QtProperty *axispropertyticklengthoutitem_;
  QtProperty *axispropertytickstrokecoloritem_;
  QtProperty *axispropertytickstrokethicknessitem_;
  QtProperty *axispropertytickstroketypeitem_;
  // Axis Properties Sub-ticks sub block
  QtProperty *axispropertysubtickvisibilityitem_;
  QtProperty *axispropertysubticklengthinitem_;
  QtProperty *axispropertysubticklengthoutitem_;
  QtProperty *axispropertysubtickstrokecoloritem_;
  QtProperty *axispropertysubtickstrokethicknessitem_;
  QtProperty *axispropertysubtickstroketypeitem_;
  // Axis Properties Ticks Label sub block
  QtProperty *axispropertyticklabelvisibilityitem_;
  QtProperty *axispropertyticklabelfontitem_;
  QtProperty *axispropertyticklabelcoloritem_;
  QtProperty *axispropertyticklabelpaddingitem_;
  QtProperty *axispropertyticklabelrotationitem_;
  QtProperty *axispropertyticklabelsideitem_;
  QtProperty *axispropertyticklabelformatitem_;
  QtProperty *axispropertyticklabelprecisionitem_;

  // Legend Properties
  QtProperty *itempropertylegendoriginxitem_;
  QtProperty *itempropertylegendoriginyitem_;
  QtProperty *itempropertylegendvisibleitem_;
  QtProperty *itempropertylegenddirectionitem_;
  QtProperty *itempropertylegendmarginitem_;
  QtProperty *itempropertylegendfontitem_;
  QtProperty *itempropertylegendtextcoloritem_;
  QtProperty *itempropertylegendiconwidthitem_;
  QtProperty *itempropertylegendiconheightitem_;
  QtProperty *itempropertylegendicontextpaddingitem_;
  QtProperty *itempropertylegendborderstrokecoloritem_;
  QtProperty *itempropertylegendborderstrokethicknessitem_;
  QtProperty *itempropertylegendborderstroketypeitem_;
  QtProperty *itempropertylegendbackgroundcoloritem_;
  QtProperty *itempropertylegendbackgroundfillstyleitem_;
  QtProperty *itempropertylegendtitlevisibleitem_;
  QtProperty *itempropertylegendtitletextitem_;
  QtProperty *itempropertylegendtitlefontitem_;
  QtProperty *itempropertylegendtitlecoloritem_;

  // Text Item Properties
  QtProperty *itempropertytextpixelpositionxitem_;
  QtProperty *itempropertytextpixelpositionyitem_;
  QtProperty *itempropertytextmarginitem_;
  QtProperty *itempropertytexttextitem_;
  QtProperty *itempropertytextfontitem_;
  QtProperty *itempropertytextcoloritem_;
  QtProperty *itempropertytextantialiaseditem_;
  QtProperty *itempropertytextstrokecoloritem_;
  QtProperty *itempropertytextstrokethicknessitem_;
  QtProperty *itempropertytextstroketypeitem_;
  QtProperty *itempropertytextbackgroundcoloritem_;
  QtProperty *itempropertytextbackgroundfillstyleitem_;
  QtProperty *itempropertytextrotationitem_;
  QtProperty *itempropertytextpositionalignmentitem_;
  QtProperty *itempropertytexttextalignmentitem_;

  // Line Item Properties
  QtProperty *itempropertylinepixelpositionx1item_;
  QtProperty *itempropertylinepixelpositiony1item_;
  QtProperty *itempropertylinepixelpositionx2item_;
  QtProperty *itempropertylinepixelpositiony2item_;
  QtProperty *itempropertylineantialiaseditem_;
  QtProperty *itempropertylinestrokecoloritem_;
  QtProperty *itempropertylinestrokethicknessitem_;
  QtProperty *itempropertylinestroketypeitem_;
  QtProperty *itempropertylineendingstyleitem_;
  QtProperty *itempropertylineendingwidthitem_;
  QtProperty *itempropertylineendinglengthitem_;
  QtProperty *itempropertylinestartingstyleitem_;
  QtProperty *itempropertylinestartingwidthitem_;
  QtProperty *itempropertylinestartinglengthitem_;

  // Image Item Properties
  QtProperty *itempropertyimagepixelpositionxitem_;
  QtProperty *itempropertyimagepixelpositionyitem_;
  QtProperty *itempropertyimagerotationitem_;
  QtProperty *itempropertyimagesourceitem_;
  QtProperty *itempropertyimagestrokecoloritem_;
  QtProperty *itempropertyimagestrokethicknessitem_;
  QtProperty *itempropertyimagestroketypeitem_;

  // LineSpecial Properties block
  QtProperty *lsplotpropertyxaxisitem_;
  QtProperty *lsplotpropertyyaxisitem_;
  QtProperty *lsplotpropertylinestyleitem_;
  QtProperty *lsplotpropertylinestrokecoloritem_;
  QtProperty *lsplotpropertylinestrokethicknessitem_;
  QtProperty *lsplotpropertylinestroketypeitem_;
  QtProperty *lsplotpropertylinefillstatusitem_;
  QtProperty *lsplotpropertylinefillcoloritem_;
  QtProperty *lsplotpropertylinefillstyleitem_;
  QtProperty *lsplotpropertylineantialiaseditem_;
  QtProperty *lsplotpropertyscatterstyleitem_;
  QtProperty *lsplotpropertyscatterthicknessitem_;
  QtProperty *lsplotpropertyscatterfillcoloritem_;
  QtProperty *lsplotpropertyscatterstrokecoloritem_;
  QtProperty *lsplotpropertyscatterstrokethicknessitem_;
  QtProperty *lsplotpropertyscatterstrokestyleitem_;
  QtProperty *lsplotpropertyscatterantialiaseditem_;
  QtProperty *lsplotpropertylegendvisibleitem_;
  QtProperty *lsplotpropertylegendtextitem_;

  // LineSpecialChannel Properties block
  QtProperty *channelplotpropertyxaxisitem_;
  QtProperty *channelplotpropertyyaxisitem_;
  QtProperty *channelplotpropertylegendvisibleitem_;
  QtProperty *channelplotpropertylegendtextitem_;
  QtProperty *channel1plotpropertygroupitem_;
  QtProperty *channel1plotpropertylinestyleitem_;
  QtProperty *channel1plotpropertylinestrokecoloritem_;
  QtProperty *channel1plotpropertylinestrokethicknessitem_;
  QtProperty *channel1plotpropertylinestroketypeitem_;
  QtProperty *channel1plotpropertylinefillcoloritem_;
  QtProperty *channel1plotpropertylinefillstylritem_;
  QtProperty *channel1plotpropertylineantialiaseditem_;
  QtProperty *channel1plotpropertyscatterstyleitem_;
  QtProperty *channel1plotpropertyscatterthicknessitem_;
  QtProperty *channel1plotpropertyscatterfillcoloritem_;
  QtProperty *channel1plotpropertyscatterstrokecoloritem_;
  QtProperty *channel1plotpropertyscatterstrokethicknessitem_;
  QtProperty *channel1plotpropertyscatterstrokestyleitem_;
  QtProperty *channel1plotpropertyscatterantialiaseditem_;
  QtProperty *channel2plotpropertygroupitem_;
  QtProperty *channel2plotpropertylinestyleitem_;
  QtProperty *channel2plotpropertylinestrokecoloritem_;
  QtProperty *channel2plotpropertylinestrokethicknessitem_;
  QtProperty *channel2plotpropertylinestroketypeitem_;
  QtProperty *channel2plotpropertylineantialiaseditem_;
  QtProperty *channel2plotpropertyscatterstyleitem_;
  QtProperty *channel2plotpropertyscatterthicknessitem_;
  QtProperty *channel2plotpropertyscatterfillcoloritem_;
  QtProperty *channel2plotpropertyscatterstrokecoloritem_;
  QtProperty *channel2plotpropertyscatterstrokethicknessitem_;
  QtProperty *channel2plotpropertyscatterstrokestyleitem_;
  QtProperty *channel2plotpropertyscatterantialiaseditem_;

  // Curve Properties block
  QtProperty *cplotpropertyxaxisitem_;
  QtProperty *cplotpropertyyaxisitem_;
  QtProperty *cplotpropertylinestyleitem_;
  QtProperty *cplotpropertylinestrokecoloritem_;
  QtProperty *cplotpropertylinestrokethicknessitem_;
  QtProperty *cplotpropertylinestroketypeitem_;
  QtProperty *cplotpropertylinefillstatusitem_;
  QtProperty *cplotpropertylinefillcoloritem_;
  QtProperty *cplotpropertylinefillstyleitem_;
  QtProperty *cplotpropertylineantialiaseditem_;
  QtProperty *cplotpropertyscatterstyleitem_;
  QtProperty *cplotpropertyscatterthicknessitem_;
  QtProperty *cplotpropertyscatterfillcoloritem_;
  QtProperty *cplotpropertyscatterstrokecoloritem_;
  QtProperty *cplotpropertyscatterstrokethicknessitem_;
  QtProperty *cplotpropertyscatterstrokestyleitem_;
  QtProperty *cplotpropertyscatterantialiaseditem_;
  QtProperty *cplotpropertylegendvisibleitem_;
  QtProperty *cplotpropertylegendtextitem_;

  // Box Properties block
  QtProperty *barplotpropertyxaxisitem_;
  QtProperty *barplotpropertyyaxisitem_;
  QtProperty *barplotpropertywidthitem_;
  QtProperty *barplotpropertystackgapitem_;
  QtProperty *barplotpropertyfillantialiaseditem_;
  QtProperty *barplotpropertyfillcoloritem_;
  QtProperty *barplotpropertyfillstyleitem_;
  QtProperty *barplotpropertyantialiaseditem_;
  QtProperty *barplotpropertystrokecoloritem_;
  QtProperty *barplotpropertystrokethicknessitem_;
  QtProperty *barplotpropertystrokestyleitem_;
  QtProperty *barplotpropertylegendtextitem_;
  QtProperty *barplotpropertyhistautobinstatusitem_;
  QtProperty *barplotpropertyhistbinitem_;
  QtProperty *barplotpropertyhistbeginitem_;
  QtProperty *barplotpropertyhistenditem_;

  // StatBox Properties block
  QtProperty *statboxplotpropertyxaxisitem_;
  QtProperty *statboxplotpropertyyaxisitem_;
  QtProperty *statboxplotpropertywidthitem_;
  QtProperty *statboxplotpropertywhiskerwidthitem_;
  QtProperty *statboxplotpropertyboxstyleitem_;
  QtProperty *statboxplotpropertyboxoutlinecoloritem_;
  QtProperty *statboxplotpropertyboxoutlinethicknessitem_;
  QtProperty *statboxplotpropertyboxoutlinestyleitem_;
  QtProperty *statboxplotpropertywhiskerstyleitem_;
  QtProperty *statboxplotpropertyantialiaseditem_;
  QtProperty *statboxplotpropertyfillantialiaseditem_;
  QtProperty *statboxplotpropertyfillstatusitem_;
  QtProperty *statboxplotpropertyfillcoloritem_;
  QtProperty *statboxplotpropertyfillstyleitem_;
  QtProperty *statboxplotpropertywhiskerantialiaseditem_;
  QtProperty *statboxplotpropertywhiskerstrokecoloritem_;
  QtProperty *statboxplotpropertywhiskerstrokethicknessitem_;
  QtProperty *statboxplotpropertywhiskerstrokestyleitem_;
  QtProperty *statboxplotpropertywhiskerbarstrokecoloritem_;
  QtProperty *statboxplotpropertywhiskerbarstrokethicknessitem_;
  QtProperty *statboxplotpropertywhiskerbarstrokestyleitem_;
  QtProperty *statboxplotpropertymedianstrokecoloritem_;
  QtProperty *statboxplotpropertymideanstrokethicknessitem_;
  QtProperty *statboxplotpropertymideanstrokestyleitem_;
  QtProperty *statboxplotpropertyscatterantialiaseditem_;
  QtProperty *statboxplotpropertyscatterstyleitem_;
  QtProperty *statboxplotpropertyscattersizeitem_;
  QtProperty *statboxplotpropertyscatterfillcoloritem_;
  QtProperty *statboxplotpropertyscatteroutlinecoloritem_;
  QtProperty *statboxplotpropertyscatteroutlinethicknessitem_;
  QtProperty *statboxplotpropertyscatteroutlinestyleitem_;
  QtProperty *statboxplotpropertylegendtextitem_;

  // Vector properties block
  QtProperty *vectorpropertyxaxisitem_;
  QtProperty *vectorpropertyyaxisitem_;
  QtProperty *vectorpropertylinestrokecoloritem_;
  QtProperty *vectorpropertylinestrokethicknessitem_;
  QtProperty *vectorpropertylinestroketypeitem_;
  QtProperty *vectorpropertylineendingtypeitem_;
  QtProperty *vectorpropertylineendingheightitem_;
  QtProperty *vectorpropertylineendingwidthitem_;
  QtProperty *vectorpropertylineantialiaseditem_;
  QtProperty *vectorpropertylegendvisibleitem_;
  QtProperty *vectorpropertylegendtextitem_;

  // Pie Properties Block
  QtProperty *pieplotpropertystyleitem_;
  QtProperty *pieplotpropertylinestrokecoloritem_;
  QtProperty *pieplotpropertylinestrokethicknessitem_;
  QtProperty *pieplotpropertylinestroketypeitem_;
  QtProperty *pieplotpropertymarginpercentitem_;

  // Colormap Properties Block
  QtProperty *colormappropertyinterpolateitem_;
  QtProperty *colormappropertytightboundaryitem_;
  QtProperty *colormappropertylevelcountitem_;
  QtProperty *colormappropertygradientinvertitem_;
  QtProperty *colormappropertygradientperiodicitem_;
  QtProperty *colormappropertygradientitem_;
  // colormap scale
  QtProperty *colormappropertyscalevisibleitem_;
  QtProperty *colormappropertyscalewidthitem_;
  // Colormap Axis general
  QtProperty *colormappropertyscaleaxisvisibleitem_;
  QtProperty *colormappropertyscaleaxisoffsetitem_;
  QtProperty *colormappropertyscaleaxisfromitem_;
  QtProperty *colormappropertyscaleaxistoitem_;
  QtProperty *colormappropertyscaleaxislinlogitem_;
  QtProperty *colormappropertyscaleaxisinvertitem_;
  QtProperty *colormappropertyscaleaxisstrokecoloritem_;
  QtProperty *colormappropertyscaleaxisstrokethicknessitem_;
  QtProperty *colormappropertyscaleaxisstroketypeitem_;
  QtProperty *colormappropertyscaleaxisantialiaseditem_;
  // Colormap Axis Properties Label sub block
  QtProperty *colormappropertyscaleaxislabeltextitem_;
  QtProperty *colormappropertyscaleaxislabelfontitem_;
  QtProperty *colormappropertyscaleaxislabelcoloritem_;
  QtProperty *colormappropertyscaleaxislabelpaddingitem_;
  // Colormap Axis Properties Ticks sub block
  QtProperty *colormappropertyscaleaxistickvisibilityitem_;
  QtProperty *colormappropertyscaleaxisticklengthinitem_;
  QtProperty *colormappropertyscaleaxisticklengthoutitem_;
  QtProperty *colormappropertyscaleaxistickstrokecoloritem_;
  QtProperty *colormappropertyscaleaxistickstrokethicknessitem_;
  QtProperty *colormappropertyscaleaxistickstroketypeitem_;
  // Colormap Axis Properties Sub-ticks sub block
  QtProperty *colormappropertyscaleaxissubtickvisibilityitem_;
  QtProperty *colormappropertyscaleaxissubticklengthinitem_;
  QtProperty *colormappropertyscaleaxissubticklengthoutitem_;
  QtProperty *colormappropertyscaleaxissubtickstrokecoloritem_;
  QtProperty *colormappropertyscaleaxissubtickstrokethicknessitem_;
  QtProperty *colormappropertyscaleaxissubtickstroketypeitem_;
  // Colormap Axis Properties Ticks Label sub block
  QtProperty *colormappropertyscaleaxisticklabelvisibilityitem_;
  QtProperty *colormappropertyscaleaxisticklabelfontitem_;
  QtProperty *colormappropertyscaleaxisticklabelcoloritem_;
  QtProperty *colormappropertyscaleaxisticklabelpaddingitem_;
  QtProperty *colormappropertyscaleaxisticklabelrotationitem_;
  QtProperty *colormappropertyscaleaxisticklabelsideitem_;
  QtProperty *colormappropertyscaleaxisticklabelformatitem_;
  QtProperty *colormappropertyscaleaxisticklabelprecisionitem_;

  // ErrorBar2D
  QtProperty *errorbarpropertywhiskerwidthitem_;
  QtProperty *errorbarpropertysymbolgaptem_;
  QtProperty *errorbarpropertystrokecoloritem_;
  QtProperty *errorbarpropertystrokethicknessitem_;
  QtProperty *errorbarpropertystroketypeitem_;
  QtProperty *errorbarpropertyantialiaseditem_;
  QtProperty *errorbarpropertyfillstatusitem_;
  QtProperty *errorbarpropertyfillcoloritem_;

  // Horizontal Major Grid Sub Block
  QtProperty *hgridaxispropertycomboitem_;
  QtProperty *hmajgridpropertyvisibleitem_;
  QtProperty *hmajgridpropertystrokecoloritem_;
  QtProperty *hmajgridpropertystrokethicknessitem_;
  QtProperty *hmajgridpropertystroketypeitem_;
  QtProperty *hmajgridpropertyzerolinevisibleitem_;
  QtProperty *hmajgridpropertyzerolinestrokecoloritem_;
  QtProperty *hmajgridpropertyzerolinestrokethicknessitem_;
  QtProperty *hmajgridpropertyzerolinestroketypeitem_;
  // Horizontal Minor Grid Sub Block
  QtProperty *hmingridpropertyvisibleitem_;
  QtProperty *hmingridpropertystrokecoloritem_;
  QtProperty *hmingridpropertystrokethicknessitem_;
  QtProperty *hmingridpropertystroketypeitem_;
  // Vertical Major Grid Sub Block
  QtProperty *vgridaxispropertycomboitem_;
  QtProperty *vmajgridpropertyvisibleitem_;
  QtProperty *vmajgridpropertystrokecoloritem_;
  QtProperty *vmajgridpropertystrokethicknessitem_;
  QtProperty *vmajgridpropertystroketypeitem_;
  QtProperty *vmajgridpropertyzerolinevisibleitem_;
  QtProperty *vmajgridpropertyzerolinestrokecoloritem_;
  QtProperty *vmajgridpropertyzerolinestrokethicknessitem_;
  QtProperty *vmajgridpropertyzerolinestroketypeitem_;
  // Vertical Minor Grid Sub Block
  QtProperty *vmingridpropertyvisibleitem_;
  QtProperty *vmingridpropertystrokecoloritem_;
  QtProperty *vmingridpropertystrokethicknessitem_;
  QtProperty *vmingridpropertystroketypeitem_;
  // Plot3D Canvas properties
  QtProperty *plot3dcanvasthemeitem_;
  QtProperty *plot3dcanvassizeitem_;
  // Plot3D Theme properties
  QtProperty *plot3dcanvaswindowcoloritem_;
  QtProperty *plot3dcanvasbackgroundvisibleitem_;
  QtProperty *plot3dcanvasbackgroundcoloritem_;
  QtProperty *plot3dcanvasambientlightstrengthitem_;
  QtProperty *plot3dcanvaslightstrengthitem_;
  QtProperty *plot3dcanvaslightcoloritem_;
  QtProperty *plot3dcanvasgridvisibleitem_;
  QtProperty *plot3dcanvasgridcoloritem_;
  QtProperty *plot3dcanvasfontitem_;
  QtProperty *plot3dcanvaslabelbackgroundvisibleitem_;
  QtProperty *plot3dcanvaslabelbackgroundcoloritem_;
  QtProperty *plot3dcanvaslabelbordervisibleitem_;
  QtProperty *plot3dcanvaslabeltextcoloritem_;
  // Plot3D Value Axis
  QtProperty *plot3daxisvalueautoadjustrangeitem_;
  QtProperty *plot3daxisvaluerangeloweritem_;
  QtProperty *plot3daxisvaluerangeupperitem_;
  QtProperty *plot3daxisvalueticklabelrotationitem_;
  QtProperty *plot3daxisvaluetitlevisibleitem_;
  QtProperty *plot3daxisvaluetitlefixeditem_;
  QtProperty *plot3daxisvaluetitletextitem_;
  QtProperty *plot3dvalueaxislabelformatitem_;
  QtProperty *plot3dvalueaxisreverseitem_;
  QtProperty *plot3dvalueaxistickcountitem_;
  QtProperty *plot3dvalueaxissubtickcountitem_;
  // Plot3D Catagory Axis
  QtProperty *plot3daxiscatagoryautoadjustrangeitem_;
  QtProperty *plot3daxiscatagoryrangeloweritem_;
  QtProperty *plot3daxiscatagoryrangeupperitem_;
  QtProperty *plot3daxiscatagoryticklabelrotationitem_;
  QtProperty *plot3daxiscatagorytitlevisibleitem_;
  QtProperty *plot3daxiscatagorytitlefixeditem_;
  QtProperty *plot3daxiscatagorytitletextitem_;
  // Plot3D Surface
  QtProperty *plot3dsurfacefliphorizontalgriditem_;
  QtProperty *plot3dsurfaceaspectratioitem_;
  QtProperty *plot3dsurfacehorizontalaspectratioitem_;
  QtProperty *plot3dsurfaceshadowqualityitem_;
  QtProperty *plot3dsurfaceorthoprojectionstatusitem_;
  QtProperty *plot3dsurfacepolarstatusitem_;
  // plot3D Bar
  QtProperty *plot3dbarspacingxitem_;
  QtProperty *plot3dbarspacingyitem_;
  QtProperty *plot3dbarspacingrelativeitem_;
  QtProperty *plot3dbarthicknessitem_;
  QtProperty *plot3dbaraspectratioitem_;
  QtProperty *plot3dbarhorizontalaspectratioitem_;
  QtProperty *plot3dbarshadowqualityitem_;
  QtProperty *plot3dbarorthoprojectionstatusitem_;
  QtProperty *plot3dbarpolarstatusitem_;
  // Plot3D Scatter
  QtProperty *plot3dscatteraspectratioitem_;
  QtProperty *plot3dscatterhorizontalaspectratioitem_;
  QtProperty *plot3dscattershadowqualityitem_;
  QtProperty *plot3dscatterorthoprojectionstatusitem_;
  QtProperty *plot3dscatterpolarstatusitem_;
  // Plot3D Surface Series
  QtProperty *plot3dsurfaceseriesvisibleitem_;
  QtProperty *plot3dsurfaceseriesflatshadingstatusitem_;
  QtProperty *plot3dsurfaceseriesdrawitem_;
  QtProperty *plot3dsurfaceseriesmeshsmoothitem_;
  QtProperty *plot3dsurfaceseriescolorstyleitem_;
  QtProperty *plot3dsurfaceseriesbasecoloritem_;
  QtProperty *plot3dsurfaceseriesbasegradiantitem_;
  QtProperty *plot3dsurfaceserieshighlightcoloritem_;
  // Plot3D Bar Series
  QtProperty *plot3dbarseriesvisibleitem_;
  QtProperty *plot3dbarseriesmeshitem_;
  QtProperty *plot3dbarseriesmeshsmoothitem_;
  QtProperty *plot3dbarseriescolorstyleitem_;
  QtProperty *plot3dbarseriesbasecoloritem_;
  QtProperty *plot3dbarseriesbasegradiantitem_;
  QtProperty *plot3dbarserieshighlightcoloritem_;
  // Plot3D Scatter Series
  QtProperty *plot3dscatterseriesvisibleitem_;
  QtProperty *plot3dscatterseriessizeitem_;
  QtProperty *plot3dscatterseriesmeshitem_;
  QtProperty *plot3dscatterseriesmeshsmoothitem_;
  QtProperty *plot3dscatterseriescolorstyleitem_;
  QtProperty *plot3dscatterseriesbasecoloritem_;
  QtProperty *plot3dscatterseriesbasegradiantitem_;
  QtProperty *plot3dscatterserieshighlightcoloritem_;
  // Table
  QtProperty *tablewindowrowcountitem_;
  QtProperty *tablewindowcolcountitem_;
  // Matrix
  QtProperty *matrixwindowrowcountitem_;
  QtProperty *matrixwindowcolcountitem_;
};

#endif  // PROPERTYEDITOR_H
