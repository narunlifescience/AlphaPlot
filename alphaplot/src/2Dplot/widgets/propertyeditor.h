#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QDockWidget>
#include <QTreeWidget>
#include "../Axis2D.h"

class Ui_PropertyEditor;
class QSplitter;
class MyWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QtAbstractPropertyBrowser;
class Layout2D;
class AxisRect2D;
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
  enum class PropertyItemType : int {
    Layout = 0,
    Grid = 1,
    Axis = 2,
  };

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
  void axisRectCreated(AxisRect2D *axisrect, MyWidget *widget);

 public slots:
  void populateObjectBrowser(MyWidget *widget);

 private:
  void setObjectPropertyId();
  QString generateAxisTreeitemText(const Axis2D::AxisOreantation &orient);
  template <class T>
  T* getgraph2dobject(QTreeWidgetItem *item) {
    void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
    T* object = static_cast<T *>(ptr);
    return object;
  }

 private:
  Ui_PropertyEditor *ui_;
  QSplitter *splitter_;
  QTreeWidget *objectbrowser_;
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

  // Axis Properties Major Grid Sub Block
  QtProperty *axisXgridpropertygroupitem_;
  QtProperty *axisXmajorgridpropertyvisibleitem_;
  QtProperty *axisXmajorgridpropertystrokecoloritem;
  QtProperty *axisXmajorgridpropertystrokethicknessitem;
  QtProperty *axisXmajorgridpropertystroketypeitem;
  QtProperty *axisXmajorgridpropertyzerolinevisibleitem;
  // Axis Properties Minor Grid Sub Block
  QtProperty *axisXminorgridpropertyvisibleitem_;
  QtProperty *axisXminorgridpropertystrokecoloritem;
  QtProperty *axisXminorgridpropertystrokethicknessitem;
  QtProperty *axisXminorgridpropertystroketypeitem;
  QtProperty *axisXminorgridpropertyzerolinevisibleitem;
};

#endif  // PROPERTYEDITOR_H
