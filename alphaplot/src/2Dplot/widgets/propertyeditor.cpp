#include "propertyeditor.h"
#include "ui_propertyeditor.h"

#include <QSplitter>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtDebug>

#include "../../MyWidget.h"
#include "../3rdparty/propertybrowser/qteditorfactory.h"
#include "../3rdparty/propertybrowser/qtpropertymanager.h"
#include "../3rdparty/propertybrowser/qttreepropertybrowser.h"
#include "../Layout2D.h"
#include "../LineScatter2D.h"
#include "Matrix.h"
#include "MultiLayer.h"
#include "Note.h"
#include "Table.h"
#include "core/IconLoader.h"

PropertyEditor::PropertyEditor(QWidget *parent)
    : QDockWidget(parent),
      ui_(new Ui_PropertyEditor),
      splitter_(new QSplitter(Qt::Vertical)),
      objectbrowser_(new MyTreeWidget(this)),
      propertybrowser_(new QtTreePropertyBrowser(this)),
      objectitems_(QList<QTreeWidgetItem *>()),
      previouswidget_(nullptr),
      // property browser Items
      groupManager_(new QtGroupPropertyManager(propertybrowser_)),
      boolManager_(new QtBoolPropertyManager(propertybrowser_)),
      intManager_(new QtIntPropertyManager(propertybrowser_)),
      doubleManager_(new QtDoublePropertyManager(propertybrowser_)),
      stringManager_(new QtStringPropertyManager(propertybrowser_)),
      sizeManager_(new QtSizePropertyManager(propertybrowser_)),
      rectManager_(new QtRectPropertyManager(propertybrowser_)),
      enumManager_(new QtEnumPropertyManager(propertybrowser_)),
      colorManager_(new QtColorPropertyManager(propertybrowser_)),
      fontManager_(new QtFontPropertyManager(propertybrowser_)),
      // Property Widget factory
      checkBoxFactory_(new QtCheckBoxFactory(propertybrowser_)),
      spinBoxFactory_(new QtSpinBoxFactory(propertybrowser_)),
      doubleSpinBoxFactory_(new QtDoubleSpinBoxFactory(propertybrowser_)),
      lineEditFactory_(new QtLineEditFactory(propertybrowser_)),
      comboBoxFactory_(new QtEnumEditorFactory(propertybrowser_)),
      colorFactory_(new QtColorEditorFactory(propertybrowser_)),
      fontFactory_(new QtFontEditorFactory(propertybrowser_)) {
  ui_->setupUi(this);
  setWindowTitle(tr("Property Editor"));
  setWindowIcon(QIcon());
  objectbrowser_->setColumnCount(1);

  ui_->dockWidgetContents->layout()->setContentsMargins(0, 0, 0, 0);
  objectbrowser_->setFrameShape(QFrame::NoFrame);
  objectbrowser_->setSelectionMode(QAbstractItemView::SingleSelection);
  splitter_->addWidget(objectbrowser_);
  splitter_->addWidget(propertybrowser_);
  setWidget(splitter_);
  propertybrowser_->setFocusPolicy(Qt::NoFocus);

  // Pen stle enum
  QStringList axistypelist;
  axistypelist << tr("Linear") << tr("Logarithmic");
  QStringList stroketypelist;
  stroketypelist << tr("Solid Line") << tr("Dash Line") << tr("Dot Line")
                 << tr("Dash Dot Line") << tr("Dash Dot Dot Line");
  QStringList axislabelsidelist;
  axislabelsidelist << tr("Down") << tr("Up");
  QMap<int, QIcon> stroketypeiconslist;
  stroketypeiconslist[0] = QIcon(":/icons/common/16/edit-style-solidline.png");
  stroketypeiconslist[1] = QIcon(":/icons/common/16/edit-style-dashline.png");
  stroketypeiconslist[2] = QIcon(":/icons/common/16/edit-style-dotline.png");
  stroketypeiconslist[3] =
      QIcon(":/icons/common/16/edit-style-dashdotline.png");
  stroketypeiconslist[4] =
      QIcon(":/icons/common/16/edit-style-dashdotdotline.png");

  // Property browser set up appropriate widget factory
  propertybrowser_->setFactoryForManager(boolManager_, checkBoxFactory_);
  propertybrowser_->setFactoryForManager(intManager_, spinBoxFactory_);
  propertybrowser_->setFactoryForManager(doubleManager_, doubleSpinBoxFactory_);
  propertybrowser_->setFactoryForManager(stringManager_, lineEditFactory_);
  propertybrowser_->setFactoryForManager(sizeManager_->subIntPropertyManager(),
                                         spinBoxFactory_);
  propertybrowser_->setFactoryForManager(rectManager_->subIntPropertyManager(),
                                         spinBoxFactory_);
  propertybrowser_->setFactoryForManager(enumManager_, comboBoxFactory_);
  propertybrowser_->setFactoryForManager(colorManager_, colorFactory_);
  propertybrowser_->setFactoryForManager(fontManager_, fontFactory_);

  // Layout Properties
  layoutpropertygroupitem_ = groupManager_->addProperty(tr("Layout"));
  layoutpropertyrectitem_ = rectManager_->addProperty(tr("Outer Rect"));
  layoutpropertygroupitem_->addSubProperty(layoutpropertyrectitem_);
  layoutpropertycoloritem_ = colorManager_->addProperty(tr("Background Color"));
  layoutpropertygroupitem_->addSubProperty(layoutpropertycoloritem_);
  // Axis Properties
  axispropertyvisibleitem_ = boolManager_->addProperty(tr("Visible"));
  axispropertyoffsetitem_ = intManager_->addProperty(tr("Offset"));
  axispropertyfromitem_ = doubleManager_->addProperty(tr("From"));
  axispropertytoitem_ = doubleManager_->addProperty(tr("To"));
  axispropertylinlogitem_ = enumManager_->addProperty(tr("Type"));
  enumManager_->setEnumNames(axispropertylinlogitem_, axistypelist);
  axispropertyinvertitem_ = boolManager_->addProperty(tr("Inverted"));
  axispropertystrokecoloritem_ = colorManager_->addProperty(tr("Stroke Color"));
  axispropertystrokethicknessitem_ =
      doubleManager_->addProperty(tr("Stroke Thickness"));
  axispropertystroketypeitem_ = enumManager_->addProperty(tr("Stroke Type"));
  enumManager_->setEnumNames(axispropertystroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(axispropertystroketypeitem_, stroketypeiconslist);
  axispropertyantialiaseditem_ = boolManager_->addProperty("Antialiased");
  axispropertylabeltextitem_ = stringManager_->addProperty(tr("Label Text"));
  axispropertylabelfontitem_ = fontManager_->addProperty("Label Font");
  axispropertylabelcoloritem_ = colorManager_->addProperty("Label Color");
  axispropertylabelpaddingitem_ = intManager_->addProperty("Label Padding");
  // Axis Properties Ticks sub block
  axispropertytickvisibilityitem_ = boolManager_->addProperty("Axis Ticks");
  axispropertyticklengthinitem_ = intManager_->addProperty("Length In");
  axispropertytickvisibilityitem_->addSubProperty(
      axispropertyticklengthinitem_);
  axispropertyticklengthoutitem_ = intManager_->addProperty("Length Out");
  axispropertytickvisibilityitem_->addSubProperty(
      axispropertyticklengthoutitem_);
  axispropertytickstrokecoloritem_ = colorManager_->addProperty("Stroke Color");
  axispropertytickvisibilityitem_->addSubProperty(
      axispropertytickstrokecoloritem_);
  axispropertytickstrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  axispropertytickvisibilityitem_->addSubProperty(
      axispropertytickstrokethicknessitem_);
  axispropertytickstroketypeitem_ = enumManager_->addProperty("Stroke Type");
  axispropertytickvisibilityitem_->addSubProperty(
      axispropertytickstroketypeitem_);
  enumManager_->setEnumNames(axispropertytickstroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(axispropertytickstroketypeitem_,
                             stroketypeiconslist);
  // Axis Properties Sub-ticks sub block
  axispropertysubtickvisibilityitem_ =
      boolManager_->addProperty("Axis Sub-Ticks");
  axispropertysubticklengthinitem_ = intManager_->addProperty("Length In");
  axispropertysubtickvisibilityitem_->addSubProperty(
      axispropertysubticklengthinitem_);
  axispropertysubticklengthoutitem_ = intManager_->addProperty("Length Out");
  axispropertysubtickvisibilityitem_->addSubProperty(
      axispropertysubticklengthoutitem_);
  axispropertysubtickstrokecoloritem_ =
      colorManager_->addProperty("Stroke Color");
  axispropertysubtickvisibilityitem_->addSubProperty(
      axispropertysubtickstrokecoloritem_);
  axispropertysubtickstrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  axispropertysubtickvisibilityitem_->addSubProperty(
      axispropertysubtickstrokethicknessitem_);
  axispropertysubtickstroketypeitem_ = enumManager_->addProperty("Stroke Type");
  axispropertysubtickvisibilityitem_->addSubProperty(
      axispropertysubtickstroketypeitem_);
  enumManager_->setEnumNames(axispropertysubtickstroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(axispropertysubtickstroketypeitem_,
                             stroketypeiconslist);
  // Axis Properties Ticks Label sub block
  axispropertyticklabelvisibilityitem_ =
      boolManager_->addProperty("Tick Label");
  axispropertyticklabelfontitem_ = fontManager_->addProperty("Font");
  axispropertyticklabelvisibilityitem_->addSubProperty(
      axispropertyticklabelfontitem_);
  axispropertyticklabelcoloritem_ = colorManager_->addProperty("Color");
  axispropertyticklabelvisibilityitem_->addSubProperty(
      axispropertyticklabelcoloritem_);
  axispropertyticklabelpaddingitem_ = intManager_->addProperty("Padding");
  axispropertyticklabelvisibilityitem_->addSubProperty(
      axispropertyticklabelpaddingitem_);
  axispropertyticklabelrotationitem_ = doubleManager_->addProperty("Rotation");
  axispropertyticklabelvisibilityitem_->addSubProperty(
      axispropertyticklabelrotationitem_);
  axispropertyticklabelsideitem_ = enumManager_->addProperty("Side");
  axispropertyticklabelvisibilityitem_->addSubProperty(
      axispropertyticklabelsideitem_);
  enumManager_->setEnumNames(axispropertyticklabelsideitem_, axislabelsidelist);
  axispropertyticklabelprecisionitem_ = intManager_->addProperty("Precision");
  axispropertyticklabelvisibilityitem_->addSubProperty(
      axispropertyticklabelprecisionitem_);
  // LineScatter Properties block
  QStringList lstylelist;
  lstylelist << tr("None") << tr("Line") << tr("StepLeft") << tr("StepRight")
             << tr("StepCenter") << tr("Impulse");
  QStringList sstylelist;
  sstylelist << tr("None") << tr("Dot") << tr("Cross") << tr("Plus")
             << tr("Circle") << tr("Disc") << tr("Square") << tr("Diamond")
             << tr("Star") << tr("Triangle") << tr("TriangleInverted")
             << tr("CrossSquare") << tr("PlusSquare") << tr("CrossCircle")
             << tr("PlusCircle") << tr("Peace");
  lsplotpropertyxaxisitem_ = enumManager_->addProperty("X Axis");
  lsplotpropertyyaxisitem_ = enumManager_->addProperty("Y Axis");
  lsplotpropertylinestyleitem_ = enumManager_->addProperty("Line Style");
  enumManager_->setEnumNames(lsplotpropertylinestyleitem_, lstylelist);
  lsplotpropertylinestrokecoloritem_ =
      colorManager_->addProperty("Line Stroke Color");
  lsplotpropertylinestrokethicknessitem_ =
      doubleManager_->addProperty("Line Stroke Thickness");
  lsplotpropertylinestroketypeitem_ =
      enumManager_->addProperty("Line Stroke Type");
  enumManager_->setEnumNames(lsplotpropertylinestroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(lsplotpropertylinestroketypeitem_,
                             stroketypeiconslist);
  lsplotpropertylinefillstatusitem_ =
      boolManager_->addProperty("Fill Under Area");
  lsplotpropertylinefillcoloritem_ =
      colorManager_->addProperty("Area Fill Color");
  lsplotpropertylineantialiaseditem_ =
      boolManager_->addProperty("Line Antialiased");
  lsplotpropertyscatterstyleitem_ = enumManager_->addProperty("Scatter Style");
  enumManager_->setEnumNames(lsplotpropertyscatterstyleitem_, sstylelist);
  lsplotpropertyscatterthicknessitem_ =
      doubleManager_->addProperty("Scatter Size");
  lsplotpropertyscatterfillcoloritem_ =
      colorManager_->addProperty("Scatter Fill Color");
  lsplotpropertyscatterstrokecoloritem_ =
      colorManager_->addProperty("Scatter Outline Color");
  lsplotpropertyscatterstrokethicknessitem_ =
      doubleManager_->addProperty("Scatter Outline Thickness");
  lsplotpropertyscatterstrokestyleitem_ =
      enumManager_->addProperty("Scatter Outline Type");
  enumManager_->setEnumNames(lsplotpropertyscatterstrokestyleitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(lsplotpropertyscatterstrokestyleitem_,
                             stroketypeiconslist);
  lsplotpropertyscatterantialiaseditem_ =
      boolManager_->addProperty("Scatter Antialiased");
  lsplotpropertylegendtextitem_ = stringManager_->addProperty("Plot Legrad");

  // Axis Properties Major Grid Sub Block
  hgridaxispropertycomboitem_ =
      groupManager_->addProperty("Horizontal Axis Grids");
  hmajgridpropertyvisibleitem_ = boolManager_->addProperty(tr("Major Grid"));
  hgridaxispropertycomboitem_->addSubProperty(hmajgridpropertyvisibleitem_);
  hmajgridpropertystrokecoloritem_ =
      colorManager_->addProperty(tr("Stroke Color"));
  hmajgridpropertyvisibleitem_->addSubProperty(
      hmajgridpropertystrokecoloritem_);
  hmajgridpropertystrokethicknessitem_ =
      doubleManager_->addProperty(tr("Stroke Thickness"));
  hmajgridpropertyvisibleitem_->addSubProperty(
      hmajgridpropertystrokethicknessitem_);
  hmajgridpropertystroketypeitem_ =
      enumManager_->addProperty(tr("Stroke Type"));
  hmajgridpropertyvisibleitem_->addSubProperty(hmajgridpropertystroketypeitem_);
  enumManager_->setEnumNames(hmajgridpropertystroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(hmajgridpropertystroketypeitem_,
                             stroketypeiconslist);
  hmajgridpropertyzerolinevisibleitem_ =
      boolManager_->addProperty(tr("Zero Line"));
  hmajgridpropertyvisibleitem_->addSubProperty(
      hmajgridpropertyzerolinevisibleitem_);
  // Axis Properties Minor Grid Sub Block
  hmingridpropertyvisibleitem_ = boolManager_->addProperty(tr("Minor Grid"));
  hgridaxispropertycomboitem_->addSubProperty(hmingridpropertyvisibleitem_);
  hmingridpropertystrokecoloritem_ =
      colorManager_->addProperty(tr("Stroke Color"));
  hmingridpropertyvisibleitem_->addSubProperty(
      hmingridpropertystrokecoloritem_);
  hmingridpropertystrokethicknessitem_ =
      doubleManager_->addProperty(tr("Stroke Thickness"));
  hmingridpropertyvisibleitem_->addSubProperty(
      hmingridpropertystrokethicknessitem_);
  hmingridpropertystroketypeitem_ =
      enumManager_->addProperty(tr("Stroke Type"));
  hmingridpropertyvisibleitem_->addSubProperty(hmingridpropertystroketypeitem_);
  enumManager_->setEnumNames(hmingridpropertystroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(hmingridpropertystroketypeitem_,
                             stroketypeiconslist);
  hmingridpropertyzerolinevisibleitem_ =
      boolManager_->addProperty(tr("Zero Line"));
  hmingridpropertyvisibleitem_->addSubProperty(
      hmingridpropertyzerolinevisibleitem_);

  // Axis Properties Major Grid Sub Block
  vgridaxispropertycomboitem_ =
      groupManager_->addProperty("Vertical Axis Grids");
  vmajgridpropertyvisibleitem_ = boolManager_->addProperty(tr("Major Grid"));
  vgridaxispropertycomboitem_->addSubProperty(vmajgridpropertyvisibleitem_);
  vmajgridpropertystrokecoloritem_ =
      colorManager_->addProperty(tr("Stroke Color"));
  vmajgridpropertyvisibleitem_->addSubProperty(
      vmajgridpropertystrokecoloritem_);
  vmajgridpropertystrokethicknessitem_ =
      doubleManager_->addProperty(tr("Stroke Thickness"));
  vmajgridpropertyvisibleitem_->addSubProperty(
      vmajgridpropertystrokethicknessitem_);
  vmajgridpropertystroketypeitem_ =
      enumManager_->addProperty(tr("Stroke Type"));
  vmajgridpropertyvisibleitem_->addSubProperty(vmajgridpropertystroketypeitem_);
  enumManager_->setEnumNames(vmajgridpropertystroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(vmajgridpropertystroketypeitem_,
                             stroketypeiconslist);
  vmajgridpropertyzerolinevisibleitem_ =
      boolManager_->addProperty(tr("Zero Line"));
  vmajgridpropertyvisibleitem_->addSubProperty(
      vmajgridpropertyzerolinevisibleitem_);
  // Axis Properties Minor Grid Sub Block
  vmingridpropertyvisibleitem_ = boolManager_->addProperty(tr("Minor Grid"));
  vgridaxispropertycomboitem_->addSubProperty(vmingridpropertyvisibleitem_);
  vmingridpropertystrokecoloritem_ =
      colorManager_->addProperty(tr("Stroke Color"));
  vmingridpropertyvisibleitem_->addSubProperty(
      vmingridpropertystrokecoloritem_);
  vmingridpropertystrokethicknessitem_ =
      doubleManager_->addProperty(tr("Stroke Thickness"));
  vmingridpropertyvisibleitem_->addSubProperty(
      vmingridpropertystrokethicknessitem_);
  vmingridpropertystroketypeitem_ =
      enumManager_->addProperty(tr("Stroke Type"));
  vmingridpropertyvisibleitem_->addSubProperty(vmingridpropertystroketypeitem_);
  enumManager_->setEnumNames(vmingridpropertystroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(vmingridpropertystroketypeitem_,
                             stroketypeiconslist);
  vmingridpropertyzerolinevisibleitem_ =
      boolManager_->addProperty(tr("Zero Line"));
  vmingridpropertyvisibleitem_->addSubProperty(
      vmingridpropertyzerolinevisibleitem_);

  // initiate property ID required for compare()
  setObjectPropertyId();

  connect(objectbrowser_, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
          SLOT(selectObjectItem(QTreeWidgetItem *)));
  connect(boolManager_, SIGNAL(valueChanged(QtProperty *, bool)), this,
          SLOT(valueChange(QtProperty *, const bool &)));
  connect(colorManager_, SIGNAL(valueChanged(QtProperty *, QColor)), this,
          SLOT(valueChange(QtProperty *, const QColor &)));
  connect(rectManager_, SIGNAL(valueChanged(QtProperty *, QRect)), this,
          SLOT(valueChange(QtProperty *, const QRect &)));
  connect(doubleManager_, SIGNAL(valueChanged(QtProperty *, double)), this,
          SLOT(valueChange(QtProperty *, const double &)));
  connect(stringManager_, SIGNAL(valueChanged(QtProperty *, QString)), this,
          SLOT(valueChange(QtProperty *, const QString &)));
  connect(intManager_, SIGNAL(valueChanged(QtProperty *, int)), this,
          SLOT(valueChange(QtProperty *, const int)));
  connect(enumManager_, SIGNAL(valueChanged(QtProperty *, int)), this,
          SLOT(enumValueChange(QtProperty *, const int)));
  connect(fontManager_, SIGNAL(valueChanged(QtProperty *, QFont)), this,
          SLOT(valueChange(QtProperty *, const QFont &)));
}

PropertyEditor::~PropertyEditor() { delete ui_; }

void PropertyEditor::valueChange(QtProperty *prop, const bool value) {
  if (prop->compare(axispropertyvisibleitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setshowhide_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyinvertitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setinverted_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyantialiaseditem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setantialiased_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertytickvisibilityitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->settickvisibility_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertysubtickvisibilityitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubtickvisibility_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyticklabelvisibilityitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelvisibility_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertylinefillstatusitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setlinefillstatus_lsplot(value);
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertylineantialiaseditem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setlineantialiased_lsplot(value);
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertyscatterantialiaseditem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setscatterantialiased_lsplot(value);
    lsgraph->parentPlot()->replot();
  } else {
    qDebug() << "unknown bool property item";
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const QColor &color) {
  disconnect(colorManager_, SIGNAL(valueChanged(QtProperty *, QColor)), this,
             SLOT(valueChange(QtProperty *, const QColor &)));
  if (prop->compare(layoutpropertycoloritem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    QBrush brush = axisrect->backgroundBrush();
    brush.setColor(color);
    axisrect->setBackground(brush);
    axisrect->parentPlot()->replot();
  } else if (prop->compare(axispropertystrokecoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setstrokecolor_axis(color);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertylabelcoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setlabelcolor_axis(color);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertytickstrokecoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->settickstrokecolor_axis(color);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertysubtickstrokecoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubtickstrokecolor_axis(color);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyticklabelcoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelcolor_axis(color);
    axis->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertylinestrokecoloritem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setlinestrokecolor_lsplot(color);
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertylinefillcoloritem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setlinefillcolor_lsplot(color);
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertyscatterfillcoloritem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setscatterfillcolor_lsplot(color);
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertyscatterstrokecoloritem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setscatterstrokecolor_lsplot(color);
    lsgraph->parentPlot()->replot();
  }
  connect(colorManager_, SIGNAL(valueChanged(QtProperty *, QColor)), this,
          SLOT(valueChange(QtProperty *, const QColor &)));
}

void PropertyEditor::valueChange(QtProperty *prop, const QRect &rect) {
  if (prop->compare(layoutpropertyrectitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    // axisrect->setOuterRect(rect);
    // axisrect->parentPlot()->replot();
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const double &value) {
  if (prop->compare(axispropertyfromitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setRangeLower(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertytoitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setRangeUpper(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertystrokethicknessitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setstrokethickness_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertytickstrokethicknessitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->settickstrokethickness_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertysubtickstrokethicknessitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubtickstrokethickness_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyticklabelrotationitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelrotation_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertylinestrokethicknessitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setlinestrokethickness_lsplot(value);
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertyscatterthicknessitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setscattersize_lsplot(value);
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertyscatterstrokethicknessitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setscatterstrokethickness_lsplot(value);
    lsgraph->parentPlot()->replot();
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const QString &value) {
  if (prop->compare(axispropertylabeltextitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setLabel(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertylegendtextitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setlegendtext_lsplot(value);
    lsgraph->parentPlot()->replot();
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const int value) {
  if (prop->compare(axispropertyoffsetitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setoffset_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertylabelpaddingitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setlabelpadding_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyticklengthinitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklengthin_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyticklengthoutitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklengthout_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertysubticklengthinitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubticklengthin_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertysubticklengthoutitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubticklengthout_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyticklabelpaddingitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelpadding_axis(value);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyticklabelprecisionitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelprecision_axis(value);
    axis->parentPlot()->replot();
  }
}

void PropertyEditor::enumValueChange(QtProperty *prop, const int value) {
  if (prop->compare(axispropertylinlogitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setscaletype_axis(static_cast<Axis2D::AxisScaleType>(value));
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertystroketypeitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setstroketype_axis(static_cast<Qt::PenStyle>(value + 1));
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertytickstroketypeitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->settickstrokestyle_axis(static_cast<Qt::PenStyle>(value + 1));
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertysubtickstroketypeitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubtickstrokestyle_axis(static_cast<Qt::PenStyle>(value + 1));
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyticklabelsideitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelside_axis(static_cast<Axis2D::AxisLabelSide>(value));
    axis->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertyxaxisitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getXAxis(value);
    if(!axis)
      return;
    lsgraph->setxaxis_lsplot(axis);
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertyyaxisitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getYAxis(value);
    if(!axis)
      return;
    lsgraph->setyaxis_lsplot(axis);
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertylinestyleitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setlinetype_lsplot(
        static_cast<LineScatter2D::LineStyleType>(value));
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertylinestroketypeitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setlinestrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertyscatterstyleitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setscattershape_lsplot(
        static_cast<LineScatter2D::ScatterStyle>(value));
    lsgraph->parentPlot()->replot();
  } else if (prop->compare(lsplotpropertyscatterstrokestyleitem_)) {
    LineScatter2D *lsgraph =
        getgraph2dobject<LineScatter2D>(objectbrowser_->currentItem());
    lsgraph->setscatterstrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->parentPlot()->replot();
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const QFont &font) {
  if (prop->compare(axispropertylabelfontitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setlabelfont_axis(font);
    axis->parentPlot()->replot();
  } else if (prop->compare(axispropertyticklabelfontitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelfont_axis(font);
    axis->parentPlot()->replot();
  }
}

void PropertyEditor::selectObjectItem(QTreeWidgetItem *item) {
  switch (static_cast<MyTreeWidget::PropertyItemType>(
      item->data(0, Qt::UserRole).value<int>())) {
    case MyTreeWidget::PropertyItemType::Layout: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr);
      LayoutPropertyBlock(axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::Grid: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr);
      GridPropertyBlock(axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::Axis: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      Axis2D *axis = static_cast<Axis2D *>(ptr);
      AxisPropertyBlock(axis);
    } break;
    case MyTreeWidget::PropertyItemType::LSGraph: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      LineScatter2D *lsgraph = static_cast<LineScatter2D *>(ptr1);
      void *ptr2 = item->data(0, Qt::UserRole + 2).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      LSPropertyBlock(lsgraph, axisrect);
    } break;
  }
}

void PropertyEditor::LayoutPropertyBlock(AxisRect2D *axisrect) {
  propertybrowser_->clear();
  colorManager_->blockSignals(true);

  layoutpropertygroupitem_->setPropertyName(QString("Layout 1x%1").arg(1));
  rectManager_->setValue(layoutpropertyrectitem_, axisrect->outerRect());
  colorManager_->setValue(layoutpropertycoloritem_,
                          axisrect->backgroundBrush().color());
  propertybrowser_->addProperty(layoutpropertygroupitem_);
  colorManager_->blockSignals(false);
}

void PropertyEditor::AxisPropertyBlock(Axis2D *axis) {
  propertybrowser_->clear();

  propertybrowser_->addProperty(axispropertyvisibleitem_);
  propertybrowser_->addProperty(axispropertyoffsetitem_);
  propertybrowser_->addProperty(axispropertyfromitem_);
  propertybrowser_->addProperty(axispropertytoitem_);
  propertybrowser_->addProperty(axispropertylinlogitem_);
  propertybrowser_->addProperty(axispropertyinvertitem_);
  propertybrowser_->addProperty(axispropertystrokecoloritem_);
  propertybrowser_->addProperty(axispropertystrokethicknessitem_);
  propertybrowser_->addProperty(axispropertystroketypeitem_);
  propertybrowser_->addProperty(axispropertyantialiaseditem_);
  propertybrowser_->addProperty(axispropertylabeltextitem_);
  propertybrowser_->addProperty(axispropertylabelfontitem_);
  propertybrowser_->addProperty(axispropertylabelcoloritem_);
  propertybrowser_->addProperty(axispropertylabelpaddingitem_);
  propertybrowser_->addProperty(axispropertytickvisibilityitem_);
  propertybrowser_->addProperty(axispropertysubtickvisibilityitem_);
  propertybrowser_->addProperty(axispropertyticklabelvisibilityitem_);

  boolManager_->setValue(axispropertyvisibleitem_, axis->getshowhide_axis());
  intManager_->setValue(axispropertyoffsetitem_, axis->getoffset_axis());
  doubleManager_->setValue(axispropertyfromitem_, axis->getfrom_axis());
  doubleManager_->setValue(axispropertytoitem_, axis->getto_axis());
  enumManager_->setValue(axispropertylinlogitem_,
                         static_cast<int>(axis->getscaletype_axis()));
  boolManager_->setValue(axispropertyinvertitem_, axis->getinverted_axis());
  colorManager_->setValue(axispropertystrokecoloritem_,
                          axis->getstrokecolor_axis());
  doubleManager_->setValue(axispropertystrokethicknessitem_,
                           axis->getstrokethickness_axis());
  enumManager_->setValue(axispropertystroketypeitem_,
                         axis->getstroketype_axis() - 1);
  boolManager_->setValue(axispropertyantialiaseditem_,
                         axis->getantialiased_axis());
  stringManager_->setValue(axispropertylabeltextitem_,
                           axis->getlabeltext_axis());
  fontManager_->setValue(axispropertylabelfontitem_, axis->getlabelfont_axis());
  colorManager_->setValue(axispropertylabelcoloritem_,
                          axis->getlabelcolor_axis());
  intManager_->setValue(axispropertylabelpaddingitem_,
                        axis->getlabelpadding_axis());
  boolManager_->setValue(axispropertytickvisibilityitem_,
                         axis->gettickvisibility_axis());
  intManager_->setValue(axispropertyticklengthinitem_,
                        axis->getticklengthin_axis());
  intManager_->setValue(axispropertyticklengthoutitem_,
                        axis->getticklengthout_axis());
  colorManager_->setValue(axispropertytickstrokecoloritem_,
                          axis->gettickstrokecolor_axis());
  doubleManager_->setValue(axispropertytickstrokethicknessitem_,
                           axis->gettickstrokethickness_axis());
  enumManager_->setValue(axispropertytickstroketypeitem_,
                         axis->gettickstrokestyle_axis() - 1);
  boolManager_->setValue(axispropertysubtickvisibilityitem_,
                         axis->getsubtickvisibility_axis());
  intManager_->setValue(axispropertysubticklengthinitem_,
                        axis->getsubticklengthin_axis());
  intManager_->setValue(axispropertysubticklengthoutitem_,
                        axis->getsubticklengthout_axis());
  colorManager_->setValue(axispropertysubtickstrokecoloritem_,
                          axis->getsubtickstrokecolor_axis());
  doubleManager_->setValue(axispropertysubtickstrokethicknessitem_,
                           axis->getsubtickstrokethickness_axis());
  enumManager_->setValue(axispropertysubtickstroketypeitem_,
                         axis->getsubtickstrokestyle_axis() - 1);
  boolManager_->setValue(axispropertyticklabelvisibilityitem_,
                         axis->getticklabelvisibility_axis());
  fontManager_->setValue(axispropertyticklabelfontitem_,
                         axis->getticklabelfont_axis());
  colorManager_->setValue(axispropertyticklabelcoloritem_,
                          axis->getticklabelcolor_axis());
  intManager_->setValue(axispropertyticklabelpaddingitem_,
                        axis->getticklabelpadding_axis());
  doubleManager_->setValue(axispropertyticklabelrotationitem_,
                           axis->getticklabelrotation_axis());
  enumManager_->setValue(axispropertyticklabelsideitem_,
                         static_cast<int>(axis->getticklabelside_axis()));
  intManager_->setValue(axispropertyticklabelprecisionitem_,
                        axis->getticklabelprecision_axis());
}

void PropertyEditor::GridPropertyBlock(AxisRect2D *axisrect) {
  propertybrowser_->clear();
  propertybrowser_->addProperty(hgridaxispropertycomboitem_);
  propertybrowser_->addProperty(vgridaxispropertycomboitem_);
}

void PropertyEditor::LSPropertyBlock(LineScatter2D *lsgraph,
                                     AxisRect2D *axisrect) {
  propertybrowser_->clear();

  propertybrowser_->addProperty(lsplotpropertyxaxisitem_);
  propertybrowser_->addProperty(lsplotpropertyyaxisitem_);
  propertybrowser_->addProperty(lsplotpropertylinestyleitem_);
  propertybrowser_->addProperty(lsplotpropertylinestrokecoloritem_);
  propertybrowser_->addProperty(lsplotpropertylinestrokethicknessitem_);
  propertybrowser_->addProperty(lsplotpropertylinestroketypeitem_);
  propertybrowser_->addProperty(lsplotpropertylinefillstatusitem_);
  propertybrowser_->addProperty(lsplotpropertylinefillcoloritem_);
  propertybrowser_->addProperty(lsplotpropertylineantialiaseditem_);
  propertybrowser_->addProperty(lsplotpropertyscatterstyleitem_);
  propertybrowser_->addProperty(lsplotpropertyscatterthicknessitem_);
  propertybrowser_->addProperty(lsplotpropertyscatterfillcoloritem_);
  propertybrowser_->addProperty(lsplotpropertyscatterstrokecoloritem_);
  propertybrowser_->addProperty(lsplotpropertyscatterstrokestyleitem_);
  propertybrowser_->addProperty(lsplotpropertyscatterstrokethicknessitem_);
  propertybrowser_->addProperty(lsplotpropertyscatterantialiaseditem_);
  propertybrowser_->addProperty(lsplotpropertylegendtextitem_);
  {
    QStringList lsyaxislist;
    int currentyaxis = 0;
    int ycount = 0;
    QList<Axis2D *> yaxes = axisrect->getYAxes2D();

    for (int i = 0; i < yaxes.size(); i++) {
      lsyaxislist << QString("Y Axis %1").arg(i + 1);
      if (yaxes.at(i) == lsgraph->getyaxis_lsplot()) {
        currentyaxis = ycount;
      }
      ycount++;
    }
    enumManager_->setEnumNames(lsplotpropertyyaxisitem_, lsyaxislist);
    enumManager_->setValue(lsplotpropertyyaxisitem_, currentyaxis);
  }

  {
    QStringList lsxaxislist;
    int currentxaxis = 0;
    int xcount = 0;
    QList<Axis2D *> xaxes = axisrect->getXAxes2D();
    for (int i = 0; i < xaxes.size(); i++) {
      lsxaxislist << QString("X Axis %1").arg(i + 1);
      if (xaxes.at(i) == lsgraph->getxaxis_lsplot()) {
        currentxaxis = xcount;
      }
      xcount++;
    }

    enumManager_->setEnumNames(lsplotpropertyxaxisitem_, lsxaxislist);
    enumManager_->setValue(lsplotpropertyxaxisitem_, currentxaxis);
  }

  // lsgraph->getxaxis_lsplot();
  // lsgraph->getyaxis_lsplot()

  enumManager_->setValue(lsplotpropertylinestyleitem_,
                         static_cast<int>(lsgraph->getlinetype_lsplot()));
  colorManager_->setValue(lsplotpropertylinestrokecoloritem_,
                          lsgraph->getlinestrokecolor_lsplot());
  doubleManager_->setValue(lsplotpropertylinestrokethicknessitem_,
                           lsgraph->getlinestrokethickness_lsplot());
  enumManager_->setValue(
      lsplotpropertylinestroketypeitem_,
      static_cast<int>(lsgraph->getlinestrokestyle_lsplot() - 1));
  boolManager_->setValue(lsplotpropertylinefillstatusitem_,
                         lsgraph->getlinefillstatus_lsplot());
  colorManager_->setValue(lsplotpropertylinefillcoloritem_,
                          lsgraph->getlinefillcolor_lsplot());
  boolManager_->setValue(lsplotpropertylineantialiaseditem_,
                         lsgraph->getlineantialiased_lsplot());
  enumManager_->setValue(lsplotpropertyscatterstyleitem_,
                         static_cast<int>(lsgraph->getscattershape_lsplot()));
  doubleManager_->setValue(lsplotpropertyscatterthicknessitem_,
                           lsgraph->getscattersize_lsplot());
  colorManager_->setValue(lsplotpropertyscatterfillcoloritem_,
                          lsgraph->getscatterfillcolor_lsplot());
  colorManager_->setValue(lsplotpropertyscatterstrokecoloritem_,
                          lsgraph->getscatterstrokecolor_lsplot());
  enumManager_->setValue(
      lsplotpropertyscatterstrokestyleitem_,
      static_cast<int>(lsgraph->getscatterstrokestyle_lsplot() - 1));
  doubleManager_->setValue(lsplotpropertyscatterstrokethicknessitem_,
                           lsgraph->getscatterstrokethickness_lsplot());
  boolManager_->setValue(lsplotpropertyscatterantialiaseditem_,
                         lsgraph->getscatterantialiased_lsplot());
  stringManager_->setValue(lsplotpropertylegendtextitem_,
                           lsgraph->getlegendtext_lsplot());
}

void PropertyEditor::axisRectCreated(AxisRect2D *axisrect, MyWidget *widget) {
  connect(axisrect, SIGNAL(AxisCreated(Axis2D *)), this,
          SLOT(axisCreated(Axis2D *)));
  connect(axisrect, SIGNAL(AxisRemoved(AxisRect2D *)), this,
          SLOT(axisRemoved(AxisRect2D *)));

  connect(axisrect, SIGNAL(LineScatterCreated(LineScatter2D *)), this,
          SLOT(lineScatterCreated(LineScatter2D *)));
  connect(axisrect, SIGNAL(BarCreated(Bar2D *)), this,
          SLOT(barCreated(Bar2D *)));
  populateObjectBrowser(widget);
}

void PropertyEditor::axisCreated(Axis2D *axis) {
  MyWidget *widget = qobject_cast<MyWidget *>(axis->parentPlot()->parent());
  if (widget) {
    populateObjectBrowser(widget);
  }
}

void PropertyEditor::axisRemoved(AxisRect2D *axisrect) {
  MyWidget *widget = qobject_cast<MyWidget *>(axisrect->parentPlot()->parent());
  if (widget) {
    populateObjectBrowser(widget);
  }
}

void PropertyEditor::lineScatterCreated(LineScatter2D *ls) {
  MyWidget *widget = qobject_cast<MyWidget *>(ls->parentPlot()->parent());
  if (widget) {
    populateObjectBrowser(widget);
  }
}

void PropertyEditor::barCreated(Bar2D *bar) {
  MyWidget *widget = qobject_cast<MyWidget *>(bar->parentPlot()->parent());
  if (widget) {
    populateObjectBrowser(widget);
  }
}

void PropertyEditor::populateObjectBrowser(MyWidget *widget) {
  objectbrowser_->clear();
  objectitems_.clear();
  propertybrowser_->clear();

  if (qobject_cast<Layout2D *>(widget)) {
    Layout2D *gd = qobject_cast<Layout2D *>(widget);
    objectbrowser_->setHeaderLabel("Graph2D");
    QList<AxisRect2D *> elementslist = gd->getAxisRectList();

    // Layout items
    for (int i = 0; i < elementslist.size(); ++i) {
      AxisRect2D *element = elementslist.at(i);
      QTreeWidgetItem *item =
          new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                              QStringList(QString("Layout: %1").arg(i + 1)));
      item->setIcon(0,
                    IconLoader::load("graph2d-layout", IconLoader::LightDark));
      item->setData(0, Qt::UserRole,
                    static_cast<int>(MyTreeWidget::PropertyItemType::Layout));
      item->setData(0, Qt::UserRole + 1, QVariant::fromValue<void *>(element));

      // Grids
      QTreeWidgetItem *griditem =
          new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                              QStringList(QString("Axis Grids")));
      griditem->setIcon(
          0, IconLoader::load("graph3d-cross", IconLoader::LightDark));
      griditem->setData(0, Qt::UserRole,
                        static_cast<int>(MyTreeWidget::PropertyItemType::Grid));
      item->setData(0, Qt::UserRole + 1, QVariant::fromValue<void *>(element));
      item->addChild(griditem);

      // Axis items
      QList<Axis2D *> xaxes = element->getXAxes2D();
      QList<Axis2D *> yaxes = element->getYAxes2D();

      for (int j = 0; j < xaxes.size(); j++) {
        Axis2D *axis = xaxes.at(j);
        QString axistext;
        QTreeWidgetItem *axisitem =
            new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr));

        switch (axis->getorientation_axis()) {
          case Axis2D::AxisOreantation::Bottom:
            axistext = QString("X Axis: " + QString::number(j + 1));
            axisitem->setIcon(0, IconLoader::load("graph2d-axis-bottom",
                                                  IconLoader::LightDark));
            break;
          case Axis2D::AxisOreantation::Top:
            axistext = QString("X Axis: " + QString::number(j + 1));
            axisitem->setIcon(
                0, IconLoader::load("graph2d-axis-top", IconLoader::LightDark));
            break;
          case Axis2D::AxisOreantation::Left:
          case Axis2D::AxisOreantation::Right:
            qDebug() << "no left & right oriented X axis";
            break;
        }
        axisitem->setText(0, axistext);
        axisitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::Axis));
        axisitem->setData(0, Qt::UserRole + 1,
                          QVariant::fromValue<void *>(axis));
        item->addChild(axisitem);
      }

      for (int j = 0; j < yaxes.size(); j++) {
        Axis2D *axis = yaxes.at(j);
        QString axistext;
        QTreeWidgetItem *axisitem =
            new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr));

        switch (axis->getorientation_axis()) {
          case Axis2D::AxisOreantation::Left:
            axistext = QString("Y Axis: " + QString::number(j + 1));
            axisitem->setIcon(0, IconLoader::load("graph2d-axis-left",
                                                  IconLoader::LightDark));
            break;
          case Axis2D::AxisOreantation::Right:
            axistext = QString("Y Axis: " + QString::number(j + 1));
            axisitem->setIcon(0, IconLoader::load("graph2d-axis-right",
                                                  IconLoader::LightDark));
            break;
          case Axis2D::AxisOreantation::Top:
          case Axis2D::AxisOreantation::Bottom:
            qDebug() << "no top & bottom oriented Y axis";
            break;
        }
        axisitem->setText(0, axistext);
        axisitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::Axis));
        axisitem->setData(0, Qt::UserRole + 1,
                          QVariant::fromValue<void *>(axis));
        item->addChild(axisitem);
      }

      // LineScatter plot Items
      LsVec graphvec = element->getGraphVec();
      for (int j = 0; j < graphvec.size(); j++) {
        LineScatter2D *lsgraph = graphvec.at(j);
        QString lsgraphtext = QString("Line Scatter %1").arg(j + 1);
        QTreeWidgetItem *lsgraphitem = new QTreeWidgetItem(
            static_cast<QTreeWidget *>(nullptr), QStringList(lsgraphtext));
        lsgraphitem->setIcon(
            0, IconLoader::load("graph2d-line", IconLoader::LightDark));
        lsgraphitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::LSGraph));
        lsgraphitem->setData(0, Qt::UserRole + 1,
                             QVariant::fromValue<void *>(lsgraph));
        lsgraphitem->setData(0, Qt::UserRole + 2,
                             QVariant::fromValue<void *>(element));
        item->addChild(lsgraphitem);
      }

      // Bar Plot items
      BarVec barvec = element->getBarVec();
      for (int j = 0; j < barvec.size(); j++) {
        Bar2D *bar = barvec.at(j);
        QString bartext = QString("Bar %1").arg(j + 1);
        QTreeWidgetItem *baritem = new QTreeWidgetItem(
            static_cast<QTreeWidget *>(nullptr), QStringList(bartext));
        baritem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::BarGraph));
        baritem->setData(0, Qt::UserRole + 1, QVariant::fromValue<void *>(bar));
        baritem->setData(0, Qt::UserRole + 2,
                         QVariant::fromValue<void *>(element));
        item->addChild(baritem);
      }
      objectitems_.append(item);
      // if (!elementslist.isEmpty()) {
      // elementslist.at(0)->parentPlot()->replot();
      //}
    }
    connect(gd, SIGNAL(AxisRectCreated(AxisRect2D *, MyWidget *)), this,
            SLOT(axisRectCreated(AxisRect2D *, MyWidget *)));
    objectbrowser_->addTopLevelItems(objectitems_);
    previouswidget_ = gd;
    objectbrowser_->insertTopLevelItems(0, objectitems_);
  } else if (qobject_cast<Table *>(widget)) {
    objectbrowser_->setHeaderLabel("Table");
  } else if (qobject_cast<Note *>(widget)) {
    objectbrowser_->setHeaderLabel("Note");
  } else if (qobject_cast<Matrix *>(widget)) {
    objectbrowser_->setHeaderLabel("Matrix");
  } else if (qobject_cast<MultiLayer *>(widget)) {
    objectbrowser_->setHeaderLabel("Graph");
  } else {
    qDebug() << "unknown Mywidget";
  }
  objectbrowser_->expandAll();
  // always select toplevel item 0
  /*if (objectbrowser_->selectedItems().size() == 0 &&
      objectbrowser_->topLevelItemCount() > 0) {
    objectbrowser_->topLevelItem(0)->setSelected(true);
    selectObjectItem(objectbrowser_->topLevelItem(0));
  }*/
}

void PropertyEditor::setObjectPropertyId() {
  layoutpropertygroupitem_->setPropertyId("layoutpropertygroupitem_");
  layoutpropertyrectitem_->setPropertyId("layoutpropertyrectitem_");
  layoutpropertycoloritem_->setPropertyId("layoutpropertycoloritem_");
  // Axis Properties General Block
  axispropertyvisibleitem_->setPropertyId("axispropertyvisibleitem_");
  axispropertyoffsetitem_->setPropertyId("axispropertyoffsetitem_");
  axispropertyfromitem_->setPropertyId("axispropertyfromitem_");
  axispropertytoitem_->setPropertyId("axispropertytoitem_");
  axispropertylinlogitem_->setPropertyId("axispropertylinlogitem_");
  axispropertyinvertitem_->setPropertyId("axispropertyinvertitem_");
  axispropertystrokecoloritem_->setPropertyId("axispropertystrokecoloritem_");
  axispropertystrokethicknessitem_->setPropertyId(
      "axispropertystrokethicknessitem_");
  axispropertystroketypeitem_->setPropertyId("axispropertystroketypeitem_");
  axispropertyantialiaseditem_->setPropertyId("axispropertyantialiaseditem_");
  // Axis Properties Label sub block
  axispropertylabeltextitem_->setPropertyId("axispropertylabeltextitem_");
  axispropertylabelfontitem_->setPropertyId("axispropertylabelfontitem_");
  axispropertylabelcoloritem_->setPropertyId("axispropertylabelcoloritem_");
  axispropertylabelpaddingitem_->setPropertyId("axispropertylabelpaddingitem_");
  // Axis Properties Ticks sub block
  axispropertytickvisibilityitem_->setPropertyId(
      "axispropertytickvisibilityitem_");
  axispropertyticklengthinitem_->setPropertyId("axispropertyticklengthinitem_");
  axispropertyticklengthoutitem_->setPropertyId(
      "axispropertyticklengthoutitem_");
  axispropertytickstrokecoloritem_->setPropertyId(
      "axispropertytickstrokecoloritem_");
  axispropertytickstrokethicknessitem_->setPropertyId(
      "axispropertytickstrokethicknessitem_");
  axispropertytickstroketypeitem_->setPropertyId(
      "axispropertytickstroketypeitem_");
  // Axis Properties Sub-ticks sub block
  axispropertysubtickvisibilityitem_->setPropertyId(
      "axispropertysubtickvisibilityitem_");
  axispropertysubticklengthinitem_->setPropertyId(
      "axispropertysubticklengthinitem_");
  axispropertysubticklengthoutitem_->setPropertyId(
      "axispropertysubticklengthoutitem_");
  axispropertysubtickstrokecoloritem_->setPropertyId(
      "axispropertysubtickstrokecoloritem_");
  axispropertysubtickstrokethicknessitem_->setPropertyId(
      "axispropertysubtickstrokethicknessitem_");
  axispropertysubtickstroketypeitem_->setPropertyId(
      "axispropertysubtickstroketypeitem_");
  // Axis Properties Ticks Label sub block
  axispropertyticklabelvisibilityitem_->setPropertyId(
      "axispropertyticklabelvisibilityitem_");
  axispropertyticklabelfontitem_->setPropertyId(
      "axispropertyticklabelfontitem_");
  axispropertyticklabelcoloritem_->setPropertyId(
      "axispropertyticklabelcoloritem_");
  axispropertyticklabelpaddingitem_->setPropertyId(
      "axispropertyticklabelpaddingitem_");
  axispropertyticklabelrotationitem_->setPropertyId(
      "axispropertyticklabelrotationitem_");
  axispropertyticklabelsideitem_->setPropertyId(
      "axispropertyticklabelsideitem_");
  axispropertyticklabelprecisionitem_->setPropertyId(
      "axispropertyticklabelprecisionitem_");
  // Line Scatter Property Block
  lsplotpropertyxaxisitem_->setPropertyId("lsplotpropertyxaxisitem_");
  lsplotpropertyyaxisitem_->setPropertyId("lsplotpropertyyaxisitem_");
  lsplotpropertylinestyleitem_->setPropertyId("lsplotpropertylinestyleitem_");
  lsplotpropertylinestrokecoloritem_->setPropertyId(
      "lsplotpropertylinestrokecoloritem_");
  lsplotpropertylinestrokethicknessitem_->setPropertyId(
      "lsplotpropertylinestrokethicknesitem_");
  lsplotpropertylinestroketypeitem_->setPropertyId(
      "lsplotpropertylinestroketypeitem_");
  lsplotpropertylinefillstatusitem_->setPropertyId(
      "lsplotpropertylinefillstatusitem_");
  lsplotpropertylinefillcoloritem_->setPropertyId(
      "lsplotpropertylinefillcoloritem_");
  lsplotpropertylineantialiaseditem_->setPropertyId(
      "lsplotpropertylineantialiaseditem_");
  lsplotpropertyscatterstyleitem_->setPropertyId(
      "lsplotpropertyscatterstyleitem_");
  lsplotpropertyscatterthicknessitem_->setPropertyId(
      "lsplotpropertyscatterthicknessitem_");
  lsplotpropertyscatterfillcoloritem_->setPropertyId(
      "lsplotpropertyscatterfillcoloritem_");
  lsplotpropertyscatterstrokecoloritem_->setPropertyId(
      "lsplotpropertyscatterstrokecoloritem_");
  lsplotpropertyscatterstrokethicknessitem_->setPropertyId(
      "lsplotpropertyscatterstrokethicknessitem_");
  lsplotpropertyscatterstrokestyleitem_->setPropertyId(
      "lsplotpropertyscatterstrokestyleitem_");
  lsplotpropertyscatterantialiaseditem_->setPropertyId(
      "lsplotpropertyscatterantialiaseditem_");
  lsplotpropertylegendtextitem_->setPropertyId(
      "lsplotpropertylelegendtextitem_");
}
