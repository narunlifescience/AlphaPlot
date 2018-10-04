#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QDockWidget>
#include "../Axis2D.h"
#include "2Dplot/TextItem2D.h"
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
class LineScatter2D;
class Curve2D;
class Spline2D;
class Vector2D;
class Bar2D;
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

class PropertyEditor : public QDockWidget {
  Q_OBJECT
 public:
  explicit PropertyEditor(QWidget *parent = nullptr);
  ~PropertyEditor();
  MyTreeWidget *getObjectBrowser();

 private slots:
  void valueChange(QtProperty *prop, const bool value);
  void valueChange(QtProperty *prop, const QColor &color);
  void valueChange(QtProperty *prop, const QRect &rect);
  void valueChange(QtProperty *prop, const double &value);
  void valueChange(QtProperty *prop, const QString &value);
  void valueChange(QtProperty *prop, const int value);
  void enumValueChange(QtProperty *prop, const int value);
  void valueChange(QtProperty *prop, const QFont &font);

  void selectObjectItem(QTreeWidgetItem *item);
  void LayoutPropertyBlock(AxisRect2D *axisrect);
  void AxisPropertyBlock(Axis2D *axis);
  void GridPropertyBlock(AxisRect2D *axisrect);
  void LegendPropertyBlock(Legend2D *legend);
  void TextItemPropertyBlock(TextItem2D *textitem);
  void LSPropertyBlock(LineScatter2D *lsgraph, AxisRect2D *axisrect);
  void CurvePropertyBlock(Curve2D *curve, AxisRect2D *axisrect);
  void SplinePropertyBlock(Spline2D *splinegraph, AxisRect2D *axisrect);
  void StatBoxPropertyBlock(StatBox2D *statbox, AxisRect2D *axisrect);
  void VectorPropertyBlock(Vector2D *vectorgraph, AxisRect2D *axisrect);
  void axisRectCreated(AxisRect2D *axisrect, MyWidget *widget);
  void axisCreated(Axis2D *axis);
  void axisRemoved(AxisRect2D *axisrect);
  void textItem2DCreated(TextItem2D *textitem);
  void textItem2DRemoved(AxisRect2D *axisrect);
  void lineScatterCreated(LineScatter2D *ls);
  void lineScatterRemoved(AxisRect2D *axisrect);
  void curveCreated(Curve2D *curve);
  void splineCreated(Spline2D *spline);
  void statBox2DCreated(StatBox2D *statbox);
  void statBox2DRemoved(AxisRect2D *axisrect);
  void vectorCreated(Vector2D *vector);
  void barCreated(Bar2D *bar);

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
  template <class T>
  void objectschanged(T *element) {
    MyWidget *widget =
        static_cast<MyWidget *>(element->parentPlot()->parentWidget());
    if (widget) {
      populateObjectBrowser(widget);
    }
  }

  typedef QPair<QPair<Grid2D *, Axis2D *>, QPair<Grid2D *, Axis2D *>> GridPair;
  typedef QVector<LineScatter2D *> LsVec;
  typedef QVector<Curve2D *> CurveVec;
  typedef QVector<Spline2D *> SplineVec;
  typedef QVector<Vector2D *> VectorVec;
  typedef QVector<Bar2D *> BarVec;

 private:
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
  // Layout properties
  QtProperty *layoutpropertygroupitem_;
  QtProperty *layoutpropertyrectitem_;
  QtProperty *layoutpropertycoloritem_;
  // Axis Properties General Block
  QtProperty *axispropertyvisibleitem_;
  QtProperty *axispropertyoffsetitem_;
  QtProperty *axispropertyfromitem_;
  QtProperty *axispropertytoitem_;
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
  QtProperty *axispropertyticklabelprecisionitem_;

  // Legend Properties
  QtProperty *itempropertylegendfontitem_;
  QtProperty *itempropertylegendtextcoloritem_;
  QtProperty *itempropertylegendiconwidthitem_;
  QtProperty *itempropertylegendiconheightitem_;
  QtProperty *itempropertylegendicontextpaddingitem_;
  QtProperty *itempropertylegendborderstrokecoloritem_;
  QtProperty *itempropertylegendborderstrokethicknessitem_;
  QtProperty *itempropertylegendborderstroketypeitem_;
  QtProperty *itempropertylegendbackgroundcoloritem_;

  // Text Item Properties
  QtProperty *itempropertytextpixelpositionxitem_;
  QtProperty *itempropertytextpixelpositionyitem_;
  QtProperty *itempropertytextmarginitem_;
  QtProperty *itempropertytexttextitem_;
  QtProperty *itempropertytextfontitem_;
  QtProperty *itempropertytextcoloritem_;
  QtProperty *itempropertyantialiaseditem_;
  QtProperty *itempropertytextstrokecoloritem_;
  QtProperty *itempropertytextstrokethicknessitem_;
  QtProperty *itempropertytextstroketypeitem_;
  QtProperty *itempropertytextbackgroundcoloritem_;
  QtProperty *itempropertytextrotationitem_;
  QtProperty *itempropertytextpositionalignmentitem_;
  QtProperty *itempropertytexttextalignmentitem_;

  // LineScatter Properties block
  QtProperty *lsplotpropertyxaxisitem_;
  QtProperty *lsplotpropertyyaxisitem_;
  QtProperty *lsplotpropertylinestyleitem_;
  QtProperty *lsplotpropertylinestrokecoloritem_;
  QtProperty *lsplotpropertylinestrokethicknessitem_;
  QtProperty *lsplotpropertylinestroketypeitem_;
  QtProperty *lsplotpropertylinefillstatusitem_;
  QtProperty *lsplotpropertylinefillcoloritem_;
  QtProperty *lsplotpropertylineantialiaseditem_;
  QtProperty *lsplotpropertyscatterstyleitem_;
  QtProperty *lsplotpropertyscatterthicknessitem_;
  QtProperty *lsplotpropertyscatterfillcoloritem_;
  QtProperty *lsplotpropertyscatterstrokecoloritem_;
  QtProperty *lsplotpropertyscatterstrokethicknessitem_;
  QtProperty *lsplotpropertyscatterstrokestyleitem_;
  QtProperty *lsplotpropertyscatterantialiaseditem_;
  QtProperty *lsplotpropertylegendtextitem_;

  // Curve Properties block
  QtProperty *cplotpropertyxaxisitem_;
  QtProperty *cplotpropertyyaxisitem_;
  QtProperty *cplotpropertylinestyleitem_;
  QtProperty *cplotpropertylinestrokecoloritem_;
  QtProperty *cplotpropertylinestrokethicknessitem_;
  QtProperty *cplotpropertylinestroketypeitem_;
  QtProperty *cplotpropertylinefillstatusitem_;
  QtProperty *cplotpropertylinefillcoloritem_;
  QtProperty *cplotpropertylineantialiaseditem_;
  QtProperty *cplotpropertyscatterstyleitem_;
  QtProperty *cplotpropertyscatterthicknessitem_;
  QtProperty *cplotpropertyscatterfillcoloritem_;
  QtProperty *cplotpropertyscatterstrokecoloritem_;
  QtProperty *cplotpropertyscatterstrokethicknessitem_;
  QtProperty *cplotpropertyscatterstrokestyleitem_;
  QtProperty *cplotpropertyscatterantialiaseditem_;
  QtProperty *cplotpropertylegendtextitem_;

  // Spline Properties block
  QtProperty *splinepropertyxaxisitem_;
  QtProperty *splinepropertyyaxisitem_;
  QtProperty *splinepropertylinestrokecoloritem_;
  QtProperty *splinepropertylinestrokethicknessitem_;
  QtProperty *splinepropertylinestroketypeitem_;
  QtProperty *splinepropertylinefillstatusitem_;
  QtProperty *splinepropertylinefillcoloritem_;
  QtProperty *splinepropertylineantialiaseditem_;
  QtProperty *splinepropertylegendtextitem_;

  // StatBox Properties block
  QtProperty *statboxplotpropertyxaxisitem_;
  QtProperty *statboxplotpropertyyaxisitem_;
  QtProperty *statboxplotpropertywidthitem_;
  QtProperty *statboxplotpropertywhiskerwidthitem_;
  QtProperty *statboxplotpropertyboxstyleitem_;
  QtProperty *statboxplotpropertywhiskerstyleitem_;
  QtProperty *statboxplotpropertyantialiaseditem_;
  QtProperty *statboxplotpropertyfillantialiaseditem_;
  QtProperty *statboxplotpropertyfillstatusitem_;
  QtProperty *statboxplotpropertyfillcoloritem_;
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
  QtProperty *vectorpropertylegendtextitem_;

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
};

#endif  // PROPERTYEDITOR_H
