#include "propertyeditor.h"
#include "ui_propertyeditor.h"

#include <QSplitter>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtDebug>

#include "../3rdparty/propertybrowser/qteditorfactory.h"
#include "../3rdparty/propertybrowser/qtpropertymanager.h"
#include "../3rdparty/propertybrowser/qttreepropertybrowser.h"
#include "2Dplot/ImageItem2D.h"
#include "2Dplot/Layout2D.h"
#include "2Dplot/Legend2D.h"
#include "2Dplot/LineItem2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "Matrix.h"
#include "MyWidget.h"
#include "Note.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/Utilities.h"

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
  QStringList formatlist;
  formatlist << "Scientific: 1.250e+01"
             << "Scientific: 1.250.10^-1"
             << "Scientific: 1.250x10^-1"
             << "Scientific: 1.250E+01"
             << "Decimal: 12.500"
             << "Automatic(g)"
             << "Automatic(gb)"
             << "Automatic(gbc)"
             << "Automatic(G)";

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
  axispropertyticklabelformatitem_ = enumManager_->addProperty("Format");
  axispropertyticklabelvisibilityitem_->addSubProperty(
      axispropertyticklabelformatitem_);
  enumManager_->setEnumNames(axispropertyticklabelformatitem_, formatlist);
  axispropertyticklabelprecisionitem_ = intManager_->addProperty("Precision");
  axispropertyticklabelvisibilityitem_->addSubProperty(
      axispropertyticklabelprecisionitem_);
  // Legend Properties
  itempropertylegendvisibleitem_ = boolManager_->addProperty("Visible");
  itempropertylegendfontitem_ = fontManager_->addProperty("Font");
  itempropertylegendtextcoloritem_ = colorManager_->addProperty("Text color");
  itempropertylegendiconwidthitem_ = intManager_->addProperty("Icon width");
  itempropertylegendiconheightitem_ = intManager_->addProperty("Icon Height");
  itempropertylegendicontextpaddingitem_ =
      intManager_->addProperty("Icon padding");
  itempropertylegendborderstrokecoloritem_ =
      colorManager_->addProperty("Stroke color");
  itempropertylegendborderstrokethicknessitem_ =
      doubleManager_->addProperty("Stroke thickness");
  itempropertylegendborderstroketypeitem_ =
      enumManager_->addProperty("Stroke style");
  enumManager_->setEnumNames(itempropertylegendborderstroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(itempropertylegendborderstroketypeitem_,
                             stroketypeiconslist);
  itempropertylegendbackgroundcoloritem_ =
      colorManager_->addProperty("Background color");
  // Text Item Properties
  QStringList alignlist;
  alignlist << tr("Top - Left") << tr("Top - Center") << tr("Top - Right")
            << tr("Center - Left") << tr("Center - Center")
            << tr("Center - Right") << tr("Bottom - Left")
            << tr("Bottom - Center") << tr("Bottom - Right");
  itempropertytextpixelpositionxitem_ =
      doubleManager_->addProperty("Pixel Position X");
  itempropertytextpixelpositionyitem_ =
      doubleManager_->addProperty("Pixel Position Y");
  itempropertytextmarginitem_ = rectManager_->addProperty("Margin");
  itempropertytexttextitem_ = stringManager_->addProperty("Text");
  itempropertytextfontitem_ = fontManager_->addProperty("Font");
  itempropertytextcoloritem_ = colorManager_->addProperty("Text color");
  itempropertytextantialiaseditem_ = boolManager_->addProperty("Antialiased");
  itempropertytextstrokecoloritem_ = colorManager_->addProperty("Stroke Color");
  itempropertytextstrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  itempropertytextstroketypeitem_ = enumManager_->addProperty("Stroke Type");
  enumManager_->setEnumNames(itempropertytextstroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(itempropertytextstroketypeitem_,
                             stroketypeiconslist);
  itempropertytextbackgroundcoloritem_ =
      colorManager_->addProperty("Background color");
  itempropertytextrotationitem_ = doubleManager_->addProperty("Text Rotation");
  itempropertytextpositionalignmentitem_ =
      enumManager_->addProperty("Positional alignment");
  enumManager_->setEnumNames(itempropertytextpositionalignmentitem_, alignlist);
  itempropertytexttextalignmentitem_ =
      enumManager_->addProperty("Text alignment");
  enumManager_->setEnumNames(itempropertytexttextalignmentitem_, alignlist);

  // Line Item Properties
  QStringList endingstylelist;
  endingstylelist << tr("None") << tr("Flat Arrow") << tr("Spike Arrow")
                  << tr("Line Arrow") << tr("Disc") << tr("Square")
                  << tr("Diamond") << tr("Bar") << tr("Half Bar")
                  << tr("Skewed Bar");
  itempropertylineantialiaseditem_ = boolManager_->addProperty("Antialiased");
  itempropertylinestrokecoloritem_ = colorManager_->addProperty("Stroke Color");
  itempropertylinestrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  itempropertylinestroketypeitem_ = enumManager_->addProperty("Stroke Style");
  enumManager_->setEnumNames(itempropertylinestroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(itempropertylinestroketypeitem_,
                             stroketypeiconslist);
  itempropertylineendingstyleitem_ = enumManager_->addProperty("Ending Style");
  enumManager_->setEnumNames(itempropertylineendingstyleitem_, endingstylelist);
  itempropertylineendingwidthitem_ =
      doubleManager_->addProperty("Ending Width");
  itempropertylineendinglengthitem_ =
      doubleManager_->addProperty("Ending Length");
  itempropertylinestartingstyleitem_ =
      enumManager_->addProperty("Starting Style");
  enumManager_->setEnumNames(itempropertylinestartingstyleitem_,
                             endingstylelist);
  itempropertylinestartingwidthitem_ =
      doubleManager_->addProperty("Starting Width");
  itempropertylinestartinglengthitem_ =
      doubleManager_->addProperty("Starting Length");

  // Image Item Properties
  itempropertyimagesourceitem_ = stringManager_->addProperty("Source");
  itempropertyimagesourceitem_->setEnabled(false);
  itempropertyimagestrokecoloritem_ =
      colorManager_->addProperty("Stroke Color");
  itempropertyimagestrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  itempropertyimagestroketypeitem_ = enumManager_->addProperty("Stroke Style");
  enumManager_->setEnumNames(itempropertyimagestroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(itempropertyimagestroketypeitem_,
                             stroketypeiconslist);

  // LineScatter Properties block
  QStringList lstylelist;
  lstylelist << tr("StepLeft") << tr("StepRight") << tr("StepCenter")
             << tr("Impulse");
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

  // Curve property block
  QStringList clstylelist;
  clstylelist << tr("None") << tr("Line");
  cplotpropertyxaxisitem_ = enumManager_->addProperty("X Axis");
  cplotpropertyyaxisitem_ = enumManager_->addProperty("Y Axis");
  cplotpropertylinestyleitem_ = enumManager_->addProperty("Line Style");
  enumManager_->setEnumNames(cplotpropertylinestyleitem_, clstylelist);
  cplotpropertylinestrokecoloritem_ =
      colorManager_->addProperty("Line Stroke Color");
  cplotpropertylinestrokethicknessitem_ =
      doubleManager_->addProperty("Line Stroke Thickness");
  cplotpropertylinestroketypeitem_ =
      enumManager_->addProperty("Line Stroke Type");
  enumManager_->setEnumNames(cplotpropertylinestroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(cplotpropertylinestroketypeitem_,
                             stroketypeiconslist);
  cplotpropertylinefillstatusitem_ =
      boolManager_->addProperty("Fill Under Area");
  cplotpropertylinefillcoloritem_ =
      colorManager_->addProperty("Area Fill Color");
  cplotpropertylineantialiaseditem_ =
      boolManager_->addProperty("Line Antialiased");
  cplotpropertyscatterstyleitem_ = enumManager_->addProperty("Scatter Style");
  enumManager_->setEnumNames(cplotpropertyscatterstyleitem_, sstylelist);
  cplotpropertyscatterthicknessitem_ =
      doubleManager_->addProperty("Scatter Size");
  cplotpropertyscatterfillcoloritem_ =
      colorManager_->addProperty("Scatter Fill Color");
  cplotpropertyscatterstrokecoloritem_ =
      colorManager_->addProperty("Scatter Outline Color");
  cplotpropertyscatterstrokethicknessitem_ =
      doubleManager_->addProperty("Scatter Outline Thickness");
  cplotpropertyscatterstrokestyleitem_ =
      enumManager_->addProperty("Scatter Outline Type");
  enumManager_->setEnumNames(cplotpropertyscatterstrokestyleitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(cplotpropertyscatterstrokestyleitem_,
                             stroketypeiconslist);
  cplotpropertyscatterantialiaseditem_ =
      boolManager_->addProperty("Scatter Antialiased");
  cplotpropertylegendtextitem_ = stringManager_->addProperty("Plot Legrad");

  // Box Properties block
  barplotpropertyxaxisitem_ = enumManager_->addProperty("X Axis");
  barplotpropertyyaxisitem_ = enumManager_->addProperty("Y Axis");
  barplotpropertywidthitem_ = doubleManager_->addProperty("Width");
  barplotpropertyfillantialiaseditem_ =
      boolManager_->addProperty("Fill Antialiased");
  barplotpropertyfillcoloritem_ = colorManager_->addProperty("Fill Color");
  barplotpropertyantialiaseditem_ =
      boolManager_->addProperty("Stroke Antialiased");
  barplotpropertystrokecoloritem_ = colorManager_->addProperty("Stroke Color");
  barplotpropertystrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  barplotpropertystrokestyleitem_ = enumManager_->addProperty("Stroke Style");
  enumManager_->setEnumNames(barplotpropertystrokestyleitem_, stroketypelist);
  enumManager_->setEnumIcons(barplotpropertystrokestyleitem_,
                             stroketypeiconslist);
  barplotpropertylegendtextitem_ = stringManager_->addProperty("Legend Text");

  // StatBox Properties block
  QStringList boxwhiskerstylelist;
  boxwhiskerstylelist << "SD"
                      << "SE"
                      << "Perc_25_75"
                      << "Perc_10_90"
                      << "Perc_5_95"
                      << "Perc_1_99"
                      << "MinMax"
                      << "Constant";
  statboxplotpropertyxaxisitem_ = enumManager_->addProperty("X Axis");
  statboxplotpropertyyaxisitem_ = enumManager_->addProperty("Y Axis");
  statboxplotpropertywidthitem_ = doubleManager_->addProperty("Width");
  statboxplotpropertywhiskerwidthitem_ =
      doubleManager_->addProperty("Whisker Width");
  statboxplotpropertyboxstyleitem_ = enumManager_->addProperty("Box Style");
  enumManager_->setEnumNames(statboxplotpropertyboxstyleitem_,
                             boxwhiskerstylelist);
  statboxplotpropertywhiskerstyleitem_ =
      enumManager_->addProperty("Whisker Style");
  enumManager_->setEnumNames(statboxplotpropertywhiskerstyleitem_,
                             boxwhiskerstylelist);
  statboxplotpropertyantialiaseditem_ =
      boolManager_->addProperty("Antialiased");
  statboxplotpropertyfillantialiaseditem_ =
      boolManager_->addProperty("Fill Antialiased");
  statboxplotpropertyfillstatusitem_ = boolManager_->addProperty("Fill Status");
  statboxplotpropertyfillcoloritem_ = colorManager_->addProperty("Fill Color");
  statboxplotpropertywhiskerantialiaseditem_ =
      boolManager_->addProperty("Whisker Antialiased");
  statboxplotpropertywhiskerstrokecoloritem_ =
      colorManager_->addProperty("Whisker Stroke Color");
  statboxplotpropertywhiskerstrokethicknessitem_ =
      doubleManager_->addProperty("Whisker Stroke Thickness");
  statboxplotpropertywhiskerstrokestyleitem_ =
      enumManager_->addProperty("Whisker Stroke Style");
  enumManager_->setEnumNames(statboxplotpropertywhiskerstrokestyleitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(statboxplotpropertywhiskerstrokestyleitem_,
                             stroketypeiconslist);
  statboxplotpropertywhiskerbarstrokecoloritem_ =
      colorManager_->addProperty("Whiskerbar Stroke Color");
  statboxplotpropertywhiskerbarstrokethicknessitem_ =
      doubleManager_->addProperty("Whiskerbar Stroke Thickness");
  statboxplotpropertywhiskerbarstrokestyleitem_ =
      enumManager_->addProperty("Whiskerbar Stroke Style");
  enumManager_->setEnumNames(statboxplotpropertywhiskerbarstrokestyleitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(statboxplotpropertywhiskerbarstrokestyleitem_,
                             stroketypeiconslist);
  statboxplotpropertymedianstrokecoloritem_ =
      colorManager_->addProperty("Median Stroke Color");
  statboxplotpropertymideanstrokethicknessitem_ =
      doubleManager_->addProperty("Median Stroke Thickness");
  statboxplotpropertymideanstrokestyleitem_ =
      enumManager_->addProperty("Median Stroke Style");
  enumManager_->setEnumNames(statboxplotpropertymideanstrokestyleitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(statboxplotpropertymideanstrokestyleitem_,
                             stroketypeiconslist);
  statboxplotpropertyscatterantialiaseditem_ =
      boolManager_->addProperty("Scatter Antialiased");
  statboxplotpropertyscatterstyleitem_ =
      enumManager_->addProperty("Scatter Style");
  enumManager_->setEnumNames(statboxplotpropertyscatterstyleitem_, sstylelist);
  statboxplotpropertyscattersizeitem_ =
      doubleManager_->addProperty("Scatter Size");
  statboxplotpropertyscatterfillcoloritem_ =
      colorManager_->addProperty("Scatter Fill Color");
  statboxplotpropertyscatteroutlinecoloritem_ =
      colorManager_->addProperty("Scatter Outline Color");
  statboxplotpropertyscatteroutlinethicknessitem_ =
      doubleManager_->addProperty("Scatter Outline Thickness");
  statboxplotpropertyscatteroutlinestyleitem_ =
      enumManager_->addProperty("Scatter Outline Style");
  enumManager_->setEnumNames(statboxplotpropertyscatteroutlinestyleitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(statboxplotpropertyscatteroutlinestyleitem_,
                             stroketypeiconslist);
  statboxplotpropertylegendtextitem_ =
      stringManager_->addProperty("Legend text");

  // Vector Properties block
  vectorpropertyxaxisitem_ = enumManager_->addProperty("X Axis");
  vectorpropertyyaxisitem_ = enumManager_->addProperty("Y Axis");
  vectorpropertylinestrokecoloritem_ =
      colorManager_->addProperty("Line Stroke Color");
  vectorpropertylinestrokethicknessitem_ =
      doubleManager_->addProperty("Line Stroke Thickness");
  vectorpropertylinestroketypeitem_ =
      enumManager_->addProperty("Line Stroke Type");
  enumManager_->setEnumNames(vectorpropertylinestroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(vectorpropertylinestroketypeitem_,
                             stroketypeiconslist);
  vectorpropertylineendingtypeitem_ =
      enumManager_->addProperty("Line Ending Type");
  enumManager_->setEnumNames(vectorpropertylineendingtypeitem_,
                             endingstylelist);
  vectorpropertylineendingheightitem_ =
      doubleManager_->addProperty("Line Ending Height");
  vectorpropertylineendingwidthitem_ =
      doubleManager_->addProperty("Line Ending Width");
  vectorpropertylineantialiaseditem_ =
      boolManager_->addProperty("Line Antialiased");
  vectorpropertylegendtextitem_ = stringManager_->addProperty("Plot Legrad");

  // Pie Properties Block
  pieplotpropertylinestrokecoloritem_ =
      colorManager_->addProperty("Stroke Color");
  pieplotpropertylinestrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  pieplotpropertylinestroketypeitem_ =
      enumManager_->addProperty("Stroke Style");
  enumManager_->setEnumNames(pieplotpropertylinestroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(pieplotpropertylinestroketypeitem_,
                             stroketypeiconslist);
  pieplotpropertymarginpercentitem_ =
      intManager_->addProperty("Margin Percent");
  intManager_->setRange(pieplotpropertymarginpercentitem_, 0, 100);

  // Axis Properties Major Grid Sub Block
  hgridaxispropertycomboitem_ = enumManager_->addProperty("Horizontal Axis");
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
  hmajgridpropertyzerolinestrokecoloritem_ =
      colorManager_->addProperty(tr("Zero Color"));
  hmajgridpropertyvisibleitem_->addSubProperty(
      hmajgridpropertyzerolinestrokecoloritem_);
  hmajgridpropertyzerolinestrokethicknessitem_ =
      doubleManager_->addProperty(tr("Zero Thickness"));
  hmajgridpropertyvisibleitem_->addSubProperty(
      hmajgridpropertyzerolinestrokethicknessitem_);
  hmajgridpropertyzerolinestroketypeitem_ =
      enumManager_->addProperty(tr("Zero Type"));
  hmajgridpropertyvisibleitem_->addSubProperty(
      hmajgridpropertyzerolinestroketypeitem_);
  enumManager_->setEnumNames(hmajgridpropertyzerolinestroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(hmajgridpropertyzerolinestroketypeitem_,
                             stroketypeiconslist);

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

  // Axis Properties Major Grid Sub Block
  vgridaxispropertycomboitem_ = enumManager_->addProperty("Vertical Axis");
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
  vmajgridpropertyzerolinestrokecoloritem_ =
      colorManager_->addProperty(tr("Zero Color"));
  vmajgridpropertyvisibleitem_->addSubProperty(
      vmajgridpropertyzerolinestrokecoloritem_);
  vmajgridpropertyzerolinestrokethicknessitem_ =
      doubleManager_->addProperty(tr("Zero Thickness"));
  vmajgridpropertyvisibleitem_->addSubProperty(
      vmajgridpropertyzerolinestrokethicknessitem_);
  vmajgridpropertyzerolinestroketypeitem_ =
      enumManager_->addProperty(tr("Zero Type"));
  vmajgridpropertyvisibleitem_->addSubProperty(
      vmajgridpropertyzerolinestroketypeitem_);
  enumManager_->setEnumNames(vmajgridpropertyzerolinestroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(vmajgridpropertyzerolinestroketypeitem_,
                             stroketypeiconslist);
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

MyTreeWidget *PropertyEditor::getObjectBrowser() { return objectbrowser_; }

void PropertyEditor::valueChange(QtProperty *prop, const bool value) {
  if (prop->compare(axispropertyvisibleitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setshowhide_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(hmajgridpropertyvisibleitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setMajorGridVisible(value);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(hmingridpropertyvisibleitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setMinorGridVisible(value);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(hmajgridpropertyzerolinevisibleitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setZerothLineVisible(value);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(vmajgridpropertyvisibleitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setMajorGridVisible(value);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(vmingridpropertyvisibleitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setMinorGridVisible(value);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(vmajgridpropertyzerolinevisibleitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setZerothLineVisible(value);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(axispropertyinvertitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setinverted_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertyantialiaseditem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setantialiased_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertytickvisibilityitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->settickvisibility_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertysubtickvisibilityitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubtickvisibility_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklabelvisibilityitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelvisibility_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(itempropertylegendvisibleitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    legend->sethidden_legend(value);
    legend->layer()->replot();
  } else if (prop->compare(itempropertytextantialiaseditem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    textitem->setAntialiased(value);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertylineantialiaseditem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setAntialiased(value);
    lineitem->layer()->replot();
  } else if (prop->compare(lsplotpropertylinefillstatusitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlinefillstatus_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertylineantialiaseditem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlineantialiased_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterantialiaseditem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscatterantialiased_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(cplotpropertylinefillstatusitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinefillstatus_cplot(value);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertylineantialiaseditem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlineantialiased_cplot(value);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterantialiaseditem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscatterantialiased_cplot(value);
    curve->layer()->replot();
  } else if (prop->compare(barplotpropertyfillantialiaseditem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setAntialiasedFill(value);
    bar->layer()->replot();
  } else if (prop->compare(barplotpropertyantialiaseditem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setAntialiased(value);
    bar->layer()->replot();
  } else if (prop->compare(statboxplotpropertyantialiaseditem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setAntialiased(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyfillantialiaseditem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setAntialiasedFill(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyfillstatusitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setfillstatus_statbox(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerantialiaseditem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setWhiskerAntialiased(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatterantialiaseditem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setAntialiasedScatters(value);
    statbox->layer()->replot();
  } else if (prop->compare(vectorpropertylineantialiaseditem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setlineantialiased_vecplot(value);
    vector->layer()->replot();
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
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedReplot);
  } else if (prop->compare(hmajgridpropertystrokecoloritem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setMajorGridColor(color);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(hmajgridpropertyzerolinestrokecoloritem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setZerothLineColor(color);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(hmingridpropertystrokecoloritem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setMinorGridColor(color);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(vmajgridpropertystrokecoloritem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setMajorGridColor(color);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(vmajgridpropertyzerolinestrokecoloritem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setZerothLineColor(color);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(vmingridpropertystrokecoloritem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setMinorGridColor(color);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(axispropertystrokecoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setstrokecolor_axis(color);
    axis->layer()->replot();
  } else if (prop->compare(axispropertylabelcoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setlabelcolor_axis(color);
    axis->layer()->replot();
  } else if (prop->compare(axispropertytickstrokecoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->settickstrokecolor_axis(color);
    axis->layer()->replot();
  } else if (prop->compare(axispropertysubtickstrokecoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubtickstrokecolor_axis(color);
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklabelcoloritem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelcolor_axis(color);
    axis->layer()->replot();
  } else if (prop->compare(itempropertylegendtextcoloritem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    legend->setTextColor(color);
    legend->layer()->replot();
  } else if (prop->compare(itempropertylegendborderstrokecoloritem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    legend->setborderstrokecolor_legend(color);
    legend->layer()->replot();
  } else if (prop->compare(itempropertylegendbackgroundcoloritem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    QBrush b = legend->brush();
    b.setColor(color);
    legend->setBrush(b);
    legend->layer()->replot();
  } else if (prop->compare(itempropertytextcoloritem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    textitem->setColor(color);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertytextstrokecoloritem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    textitem->setstrokecolor_textitem(color);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertytextbackgroundcoloritem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    QBrush b = textitem->brush();
    b.setStyle(Qt::SolidPattern);
    b.setColor(color);
    textitem->setBrush(b);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertylinestrokecoloritem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setstrokecolor_lineitem(color);
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertyimagestrokecoloritem_)) {
    ImageItem2D *imageitem =
        getgraph2dobject<ImageItem2D>(objectbrowser_->currentItem());
    imageitem->setstrokecolor_imageitem(color);
    imageitem->layer()->replot();
  } else if (prop->compare(lsplotpropertylinestrokecoloritem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlinestrokecolor_lsplot(color);
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertylinefillcoloritem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlinefillcolor_lsplot(color);
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterfillcoloritem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscatterfillcolor_lsplot(color);
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterstrokecoloritem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscatterstrokecolor_lsplot(color);
    lsgraph->layer()->replot();
  } else if (prop->compare(cplotpropertylinestrokecoloritem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinestrokecolor_cplot(color);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertylinefillcoloritem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinefillcolor_cplot(color);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterfillcoloritem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscatterfillcolor_cplot(color);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterstrokecoloritem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscatterstrokecolor_cplot(color);
    curve->layer()->replot();
  } else if (prop->compare(barplotpropertyfillcoloritem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setfillcolor_barplot(color);
    bar->layer()->replot();
  } else if (prop->compare(barplotpropertystrokecoloritem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setstrokecolor_barplot(color);
    bar->layer()->replot();
  } else if (prop->compare(statboxplotpropertyfillcoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setfillcolor_statbox(color);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerstrokecoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerstrokecolor_statbox(color);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerbarstrokecoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerbarstrokecolor_statbox(color);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertymedianstrokecoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setmedianstrokecolor_statbox(color);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatterfillcoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscatterfillcolor_statbox(color);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatteroutlinecoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscatterstrokecolor_statbox(color);
    statbox->layer()->replot();
  } else if (prop->compare(vectorpropertylinestrokecoloritem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setlinestrokecolor_vecplot(color);
    vector->layer()->replot();
  } else if (prop->compare(pieplotpropertylinestrokecoloritem_)) {
    Pie2D *pie = getgraph2dobject<Pie2D>(objectbrowser_->currentItem());
    pie->setstrokecolor_pieplot(color);
    pie->layer()->replot();
  }
  connect(colorManager_, SIGNAL(valueChanged(QtProperty *, QColor)), this,
          SLOT(valueChange(QtProperty *, const QColor &)));
}

void PropertyEditor::valueChange(QtProperty *prop, const QRect &rect) {
  if (prop->compare(layoutpropertyrectitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    // axisrect->setOuterRect(rect);
  } else if (prop->compare(itempropertytextmarginitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    QMargins margin;
    margin.setLeft(rect.left());
    margin.setTop(rect.top());
    margin.setRight(rect.right());
    margin.setBottom(rect.bottom());
    textitem->setPadding(margin);
    textitem->layer()->replot();
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const double &value) {
  if (prop->compare(hmajgridpropertystrokethicknessitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setMajorGridThickness(value);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(hmajgridpropertyzerolinestrokethicknessitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setZerothLineThickness(value);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(hmingridpropertystrokethicknessitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setMinorGridThickness(value);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(vmajgridpropertystrokethicknessitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setMajorGridThickness(value);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(vmajgridpropertyzerolinestrokethicknessitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setZerothLineThickness(value);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(vmingridpropertystrokethicknessitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setMinorGridThickness(value);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(axispropertyfromitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setRangeLower(value);
    axis->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
  } else if (prop->compare(axispropertytoitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setRangeUpper(value);
    axis->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
  } else if (prop->compare(axispropertystrokethicknessitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setstrokethickness_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertytickstrokethicknessitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->settickstrokethickness_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertysubtickstrokethicknessitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubtickstrokethickness_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklabelrotationitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelrotation_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(itempropertylegendborderstrokethicknessitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    legend->setborderstrokethickness_legend(value);
    legend->layer()->replot();
  } else if (prop->compare(itempropertytextpixelpositionxitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    QPointF point = textitem->position->pixelPosition();
    point.setX(value);
    textitem->position->setPixelPosition(point);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertytextpixelpositionyitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    QPointF point = textitem->position->pixelPosition();
    point.setY(value);
    textitem->position->setPixelPosition(point);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertytextstrokethicknessitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    textitem->setstrokethickness_textitem(value);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertylinestrokethicknessitem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setstrokethickness_lineitem(value);
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertytextrotationitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    textitem->setRotation(value);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertylineendingwidthitem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setendwidth_lineitem(value, LineItem2D::LineEndLocation::Stop);
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertylineendinglengthitem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setendlength_lineitem(value, LineItem2D::LineEndLocation::Stop);
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertylinestartingwidthitem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setendwidth_lineitem(value, LineItem2D::LineEndLocation::Start);
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertylinestartinglengthitem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setendlength_lineitem(value, LineItem2D::LineEndLocation::Start);
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertyimagestrokethicknessitem_)) {
    ImageItem2D *imageitem =
        getgraph2dobject<ImageItem2D>(objectbrowser_->currentItem());
    imageitem->setstrokethickness_imageitem(value);
    imageitem->layer()->replot();
  } else if (prop->compare(lsplotpropertylinestrokethicknessitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlinestrokethickness_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterthicknessitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscattersize_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterstrokethicknessitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscatterstrokethickness_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(cplotpropertylinestrokethicknessitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinestrokethickness_cplot(value);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterthicknessitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscattersize_cplot(value);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterstrokethicknessitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscatterstrokethickness_cplot(value);
    curve->layer()->replot();
  } else if (prop->compare(barplotpropertywidthitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setWidth(value);
    bar->layer()->replot();
  } else if (prop->compare(barplotpropertystrokethicknessitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setstrokethickness_barplot(value);
    bar->layer()->replot();
  } else if (prop->compare(statboxplotpropertywidthitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setWidth(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerwidthitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setWhiskerWidth(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerstrokethicknessitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerstrokethickness_statbox(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerbarstrokethicknessitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerbarstrokethickness_statbox(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertymideanstrokethicknessitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setmedianstrokethickness_statbox(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscattersizeitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscattersize_statbox(value);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatteroutlinethicknessitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscatterstrokethickness_statbox(value);
    statbox->layer()->replot();
  } else if (prop->compare(vectorpropertylinestrokethicknessitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setlinestrokethickness_vecplot(value);
    vector->layer()->replot();
  } else if (prop->compare(vectorpropertylineendingheightitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setendheight_vecplot(value, Vector2D::LineEndLocation::Start);
    vector->layer()->replot();
  } else if (prop->compare(vectorpropertylineendingwidthitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setendwidth_vecplot(value, Vector2D::LineEndLocation::Start);
    vector->layer()->replot();
  } else if (prop->compare(pieplotpropertylinestrokethicknessitem_)) {
    Pie2D *pie = getgraph2dobject<Pie2D>(objectbrowser_->currentItem());
    pie->setstrokethickness_pieplot(value);
    pie->layer()->replot();
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const QString &value) {
  if (prop->compare(axispropertylabeltextitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setLabel(Utilities::splitstring(value));
    axis->layer()->replot();
  } else if (prop->compare(itempropertytexttextitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    textitem->setText(Utilities::splitstring(value));
    textitem->layer()->replot();
  } else if (prop->compare(lsplotpropertylegendtextitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlegendtext_lsplot(Utilities::splitstring(value));
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    axisrect->getLegend()->layer()->replot();
  } else if (prop->compare(cplotpropertylegendtextitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlegendtext_cplot(Utilities::splitstring(value));
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    axisrect->getLegend()->layer()->replot();
  } else if (prop->compare(barplotpropertylegendtextitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setName(Utilities::splitstring(value));
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    axisrect->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertylegendtextitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setName(Utilities::splitstring(value));
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    axisrect->getLegend()->layer()->replot();
  } else if (prop->compare(vectorpropertylegendtextitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setlegendtext_vecplot(Utilities::splitstring(value));
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    axisrect->getLegend()->layer()->replot();
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const int value) {
  if (prop->compare(axispropertyoffsetitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setoffset_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertylabelpaddingitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setlabelpadding_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklengthinitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklengthin_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklengthoutitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklengthout_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertysubticklengthinitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubticklengthin_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertysubticklengthoutitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubticklengthout_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklabelpaddingitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelpadding_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklabelprecisionitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelprecision_axis(value);
    axis->layer()->replot();
  } else if (prop->compare(itempropertylegendiconwidthitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    QSize size = legend->iconSize();
    size.setWidth(value);
    legend->setIconSize(size);
    legend->layer()->replot();
  } else if (prop->compare(itempropertylegendiconheightitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    QSize size = legend->iconSize();
    size.setHeight(value);
    legend->setIconSize(size);
    legend->layer()->replot();
  } else if (prop->compare(itempropertylegendicontextpaddingitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    legend->setIconTextPadding(value);
    legend->layer()->replot();
  } else if (prop->compare(pieplotpropertymarginpercentitem_)) {
    Pie2D *pie = getgraph2dobject<Pie2D>(objectbrowser_->currentItem());
    pie->setmarginpercent_pieplot(value);
    pie->layer()->replot();
  }
}

void PropertyEditor::enumValueChange(QtProperty *prop, const int value) {
  if (prop->compare(hgridaxispropertycomboitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    Axis2D *axis = axisrect->getXAxes2D().at(value);
    axisrect->bindGridTo(axis);
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(hmajgridpropertystroketypeitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setMajorGridStyle(
        static_cast<Qt::PenStyle>(value + 1));
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(hmajgridpropertyzerolinestroketypeitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setZerothLineStyle(
        static_cast<Qt::PenStyle>(value + 1));
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(hmingridpropertystroketypeitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().first.first->setMinorGridStyle(
        static_cast<Qt::PenStyle>(value + 1));
    axisrect->getGridPair().first.first->layer()->replot();
  } else if (prop->compare(vgridaxispropertycomboitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    Axis2D *axis = axisrect->getYAxes2D().at(value);
    axisrect->bindGridTo(axis);
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(vmajgridpropertystroketypeitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setMajorGridStyle(
        static_cast<Qt::PenStyle>(value + 1));
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(vmajgridpropertyzerolinestroketypeitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setZerothLineStyle(
        static_cast<Qt::PenStyle>(value + 1));
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(vmingridpropertystroketypeitem_)) {
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
    axisrect->getGridPair().second.first->setMinorGridStyle(
        static_cast<Qt::PenStyle>(value + 1));
    axisrect->getGridPair().second.first->layer()->replot();
  } else if (prop->compare(axispropertylinlogitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setscaletype_axis(static_cast<Axis2D::AxisScaleType>(value));
    axis->layer()->replot();
  } else if (prop->compare(axispropertystroketypeitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setstroketype_axis(static_cast<Qt::PenStyle>(value + 1));
    axis->layer()->replot();
  } else if (prop->compare(axispropertytickstroketypeitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->settickstrokestyle_axis(static_cast<Qt::PenStyle>(value + 1));
    axis->layer()->replot();
  } else if (prop->compare(axispropertysubtickstroketypeitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setsubtickstrokestyle_axis(static_cast<Qt::PenStyle>(value + 1));
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklabelsideitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelside_axis(static_cast<Axis2D::AxisLabelSide>(value));
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklabelformatitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelformat_axis(static_cast<Axis2D::AxisLabelFormat>(value));
    axis->parentPlot()->replot();
  } else if (prop->compare(itempropertylegendborderstroketypeitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    legend->setborderstrokestyle_legend(static_cast<Qt::PenStyle>(value + 1));
    legend->layer()->replot();
  } else if (prop->compare(itempropertytexttextalignmentitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    textitem->settextalignment_textitem(
        static_cast<TextItem2D::TextAlignment>(value));
    textitem->layer()->replot();
  } else if (prop->compare(itempropertytextstroketypeitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    textitem->setstrokestyle_textitem(static_cast<Qt::PenStyle>(value + 1));
    textitem->layer()->replot();
  } else if (prop->compare(itempropertylinestroketypeitem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setstrokestyle_lineitem(static_cast<Qt::PenStyle>(value + 1));
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertylineendingstyleitem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setendstyle_lineitem(
        LineItem2D::LineEndLocation::Stop,
        static_cast<QCPLineEnding::EndingStyle>(value));
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertylinestartingstyleitem_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    lineitem->setendstyle_lineitem(
        LineItem2D::LineEndLocation::Start,
        static_cast<QCPLineEnding::EndingStyle>(value));
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertylegendborderstroketypeitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    legend->setborderstrokestyle_legend(static_cast<Qt::PenStyle>(value + 1));
    legend->layer()->replot();
  } else if (prop->compare(itempropertyimagestroketypeitem_)) {
    ImageItem2D *imageitem =
        getgraph2dobject<ImageItem2D>(objectbrowser_->currentItem());
    imageitem->setstrokestyle_imageitem(static_cast<Qt::PenStyle>(value + 1));
    imageitem->layer()->replot();
  } else if (prop->compare(lsplotpropertyxaxisitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getXAxis(value);
    if (!axis) return;
    lsgraph->setxaxis_lsplot(axis);
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertyyaxisitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getYAxis(value);
    if (!axis) return;
    lsgraph->setyaxis_lsplot(axis);
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertylinestyleitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlinetype_lsplot(
        static_cast<Graph2DCommon::LineStyleType>(value));
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertylinestroketypeitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlinestrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterstyleitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscattershape_lsplot(
        static_cast<Graph2DCommon::ScatterStyle>(value));
    lsgraph->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterstrokestyleitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscatterstrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->layer()->replot();
  } else if (prop->compare(cplotpropertyxaxisitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getXAxis(value);
    if (!axis) return;
    curve->setxaxis_cplot(axis);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertyyaxisitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getYAxis(value);
    if (!axis) return;
    curve->setyaxis_cplot(axis);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertylinestyleitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinetype_cplot(value);
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertylinestroketypeitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinestrokestyle_cplot(static_cast<Qt::PenStyle>(value + 1));
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterstyleitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscattershape_cplot(
        static_cast<Graph2DCommon::ScatterStyle>(value));
    curve->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterstrokestyleitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscatterstrokestyle_cplot(static_cast<Qt::PenStyle>(value + 1));
    curve->layer()->replot();
  } else if (prop->compare(barplotpropertyxaxisitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getXAxis(value);
    if (!axis) return;
    bar->setxaxis_barplot(axis);
    bar->layer()->replot();
  } else if (prop->compare(barplotpropertyyaxisitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getYAxis(value);
    if (!axis) return;
    bar->setyaxis_barplot(axis);
    bar->layer()->replot();
  } else if (prop->compare(barplotpropertystrokestyleitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setstrokestyle_barplot(static_cast<Qt::PenStyle>(value + 1));
    bar->layer()->replot();
  } else if (prop->compare(statboxplotpropertyxaxisitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getXAxis(value);
    if (!axis) return;
    statbox->setxaxis_statbox(axis);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyyaxisitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getYAxis(value);
    if (!axis) return;
    statbox->setyaxis_statbox(axis);
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyboxstyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setboxstyle_statbox(
        static_cast<StatBox2D::BoxWhiskerStyle>(value));
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerstyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerstyle_statbox(
        static_cast<StatBox2D::BoxWhiskerStyle>(value));
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerstrokestyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerstrokestyle_statbox(
        static_cast<Qt::PenStyle>(value + 1));
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerbarstrokestyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerbarstrokestyle_statbox(
        static_cast<Qt::PenStyle>(value + 1));
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertymideanstrokestyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setmedianstrokestyle_statbox(static_cast<Qt::PenStyle>(value + 1));
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatterstyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscattershape_statbox(
        static_cast<Graph2DCommon::ScatterStyle>(value));
    statbox->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatteroutlinestyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscatterstrokestyle_statbox(
        static_cast<Qt::PenStyle>(value + 1));
    statbox->layer()->replot();
  } else if (prop->compare(vectorpropertyxaxisitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getXAxis(value);
    if (!axis) return;
    vector->setxaxis_vecplot(axis);
    vector->layer()->replot();
  } else if (prop->compare(vectorpropertyyaxisitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getYAxis(value);
    if (!axis) return;
    vector->setyaxis_vecplot(axis);
    vector->layer()->replot();
  } else if (prop->compare(vectorpropertylinestroketypeitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setlinestrokestyle_vecplot(static_cast<Qt::PenStyle>(value + 1));
    vector->layer()->replot();
  } else if (prop->compare(vectorpropertylineendingtypeitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setendstyle_vecplot(static_cast<Vector2D::LineEnd>(value),
                                Vector2D::LineEndLocation::Start);
    vector->layer()->replot();
  } else if (prop->compare(pieplotpropertylinestroketypeitem_)) {
    Pie2D *pie = getgraph2dobject<Pie2D>(objectbrowser_->currentItem());
    pie->setstrokestyle_pieplot(static_cast<Qt::PenStyle>(value + 1));
    pie->layer()->replot();
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const QFont &font) {
  if (prop->compare(axispropertylabelfontitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setlabelfont_axis(font);
    axis->layer()->replot();
  } else if (prop->compare(axispropertyticklabelfontitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->setticklabelfont_axis(font);
    axis->layer()->replot();
  } else if (prop->compare(itempropertytextfontitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    textitem->setFont(font);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertylegendfontitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    legend->setFont(font);
    legend->layer()->replot();
  }
}

void PropertyEditor::selectObjectItem(QTreeWidgetItem *item) {
  switch (static_cast<MyTreeWidget::PropertyItemType>(
      item->data(0, Qt::UserRole).value<int>())) {
    case MyTreeWidget::PropertyItemType::Layout: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr);
      Layout2DPropertyBlock(axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::Grid: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr);
      Grid2DPropertyBlock(axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::Axis: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      Axis2D *axis = static_cast<Axis2D *>(ptr);
      Axis2DPropertyBlock(axis);
    } break;
    case MyTreeWidget::PropertyItemType::Legend: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      Legend2D *legend = static_cast<Legend2D *>(ptr1);
      Legend2DPropertyBlock(legend);
    } break;
    case MyTreeWidget::PropertyItemType::TextItem: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      TextItem2D *textitem = static_cast<TextItem2D *>(ptr1);
      TextItem2DPropertyBlock(textitem);
    } break;
    case MyTreeWidget::PropertyItemType::LineItem: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      LineItem2D *lineitem = static_cast<LineItem2D *>(ptr1);
      LineItem2DPropertyBlock(lineitem);
    } break;
    case MyTreeWidget::PropertyItemType::ImageItem: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      ImageItem2D *imageitem = static_cast<ImageItem2D *>(ptr1);
      ImageItem2DPropertyBlock(imageitem);
    } break;
    case MyTreeWidget::PropertyItemType::LSGraph: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr1);
      void *ptr2 = item->data(0, Qt::UserRole + 2).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      LineScatter2DPropertyBlock(lsgraph, axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::Curve: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      Curve2D *curve = static_cast<Curve2D *>(ptr1);
      void *ptr2 = item->data(0, Qt::UserRole + 2).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      Curve2DPropertyBlock(curve, axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::BarGraph: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      Bar2D *bar = static_cast<Bar2D *>(ptr1);
      void *ptr2 = item->parent()->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      Bar2DPropertyBlock(bar, axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::StatBox: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      StatBox2D *statbox = static_cast<StatBox2D *>(ptr1);
      void *ptr2 = item->parent()->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      StatBox2DPropertyBlock(statbox, axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::Vector: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      Vector2D *vector = static_cast<Vector2D *>(ptr1);
      void *ptr2 = item->parent()->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      Vector2DPropertyBlock(vector, axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::PieGraph: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      Pie2D *pie = static_cast<Pie2D *>(ptr1);
      void *ptr2 = item->parent()->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      Pie2DPropertyBlock(pie, axisrect);
    } break;
  }
}

void PropertyEditor::Layout2DPropertyBlock(AxisRect2D *axisrect) {
  propertybrowser_->clear();
  colorManager_->blockSignals(true);

  layoutpropertygroupitem_->setPropertyName(QString("Layout 1x%1").arg(1));
  rectManager_->setValue(layoutpropertyrectitem_, axisrect->outerRect());
  colorManager_->setValue(layoutpropertycoloritem_,
                          axisrect->backgroundBrush().color());
  propertybrowser_->addProperty(layoutpropertygroupitem_);
  colorManager_->blockSignals(false);
}

void PropertyEditor::Axis2DPropertyBlock(Axis2D *axis) {
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
                           Utilities::joinstring(axis->getlabeltext_axis()));
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
  enumManager_->setValue(axispropertyticklabelformatitem_,
                         static_cast<int>(axis->getticklabelformat_axis()));
  intManager_->setValue(axispropertyticklabelprecisionitem_,
                        axis->getticklabelprecision_axis());
}

void PropertyEditor::Grid2DPropertyBlock(AxisRect2D *axisrect) {
  propertybrowser_->clear();
  QPair<QPair<Grid2D *, Axis2D *>, QPair<Grid2D *, Axis2D *>> gridpair =
      axisrect->getGridPair();
  if (!gridpair.first.first || !gridpair.second.first) {
    qDebug() << "invalid grid pointers";
    return;
  }
  propertybrowser_->addProperty(hgridaxispropertycomboitem_);
  propertybrowser_->addProperty(vgridaxispropertycomboitem_);

  {
    QStringList lsxaxislist;
    int currentxaxis = 0;
    int xcount = 0;
    QList<Axis2D *> xaxes = axisrect->getXAxes2D();
    for (int i = 0; i < xaxes.size(); i++) {
      lsxaxislist << QString("X Axis %1").arg(i + 1);
      if (xaxes.at(i) == gridpair.first.second) {
        currentxaxis = xcount;
      }
      xcount++;
    }

    enumManager_->setEnumNames(hgridaxispropertycomboitem_, lsxaxislist);
    enumManager_->setValue(hgridaxispropertycomboitem_, currentxaxis);
  }

  Grid2D *hgrid = axisrect->getGridPair().first.first;

  boolManager_->setValue(hmajgridpropertyvisibleitem_,
                         hgrid->getMajorGridVisible());
  colorManager_->setValue(hmajgridpropertystrokecoloritem_,
                          hgrid->getMajorGridColor());
  doubleManager_->setValue(hmajgridpropertystrokethicknessitem_,
                           hgrid->getMajorGridwidth());
  enumManager_->setValue(hmajgridpropertystroketypeitem_,
                         hgrid->getMajorGridStyle() - 1);
  boolManager_->setValue(hmajgridpropertyzerolinevisibleitem_,
                         hgrid->getZerothLineVisible());
  colorManager_->setValue(hmajgridpropertyzerolinestrokecoloritem_,
                          hgrid->getZerothLineColor());
  doubleManager_->setValue(hmajgridpropertyzerolinestrokethicknessitem_,
                           hgrid->getZeroLinewidth());
  enumManager_->setValue(hmajgridpropertyzerolinestroketypeitem_,
                         hgrid->getZeroLineStyle() - 1);
  boolManager_->setValue(hmingridpropertyvisibleitem_,
                         hgrid->getMinorGridVisible());
  colorManager_->setValue(hmingridpropertystrokecoloritem_,
                          hgrid->getMinorGridColor());
  doubleManager_->setValue(hmingridpropertystrokethicknessitem_,
                           hgrid->getMinorGridwidth());
  enumManager_->setValue(hmingridpropertystroketypeitem_,
                         hgrid->getMinorGridStyle() - 1);

  {
    QStringList lsyaxislist;
    int currentyaxis = 0;
    int ycount = 0;
    QList<Axis2D *> yaxes = axisrect->getYAxes2D();

    for (int i = 0; i < yaxes.size(); i++) {
      lsyaxislist << QString("Y Axis %1").arg(i + 1);
      if (yaxes.at(i) == gridpair.second.second) {
        currentyaxis = ycount;
      }
      ycount++;
    }
    enumManager_->setEnumNames(vgridaxispropertycomboitem_, lsyaxislist);
    enumManager_->setValue(vgridaxispropertycomboitem_, currentyaxis);
  }
  Grid2D *vgrid = axisrect->getGridPair().second.first;

  boolManager_->setValue(vmajgridpropertyvisibleitem_,
                         vgrid->getMajorGridVisible());
  colorManager_->setValue(vmajgridpropertystrokecoloritem_,
                          vgrid->getMajorGridColor());
  doubleManager_->setValue(vmajgridpropertystrokethicknessitem_,
                           vgrid->getMajorGridwidth());
  enumManager_->setValue(vmajgridpropertystroketypeitem_,
                         vgrid->getMajorGridStyle() - 1);
  boolManager_->setValue(vmajgridpropertyzerolinevisibleitem_,
                         vgrid->getZerothLineVisible());
  colorManager_->setValue(vmajgridpropertyzerolinestrokecoloritem_,
                          vgrid->getZerothLineColor());
  doubleManager_->setValue(vmajgridpropertyzerolinestrokethicknessitem_,
                           vgrid->getZeroLinewidth());
  enumManager_->setValue(vmajgridpropertyzerolinestroketypeitem_,
                         vgrid->getZeroLineStyle() - 1);
  boolManager_->setValue(vmingridpropertyvisibleitem_,
                         vgrid->getMinorGridVisible());
  colorManager_->setValue(vmingridpropertystrokecoloritem_,
                          vgrid->getMinorGridColor());
  doubleManager_->setValue(vmingridpropertystrokethicknessitem_,
                           vgrid->getMinorGridwidth());
  enumManager_->setValue(vmingridpropertystroketypeitem_,
                         vgrid->getMinorGridStyle() - 1);
}

void PropertyEditor::Legend2DPropertyBlock(Legend2D *legend) {
  propertybrowser_->clear();

  // Legend Properties
  propertybrowser_->addProperty(itempropertylegendvisibleitem_);
  propertybrowser_->addProperty(itempropertylegendfontitem_);
  propertybrowser_->addProperty(itempropertylegendtextcoloritem_);
  propertybrowser_->addProperty(itempropertylegendiconwidthitem_);
  propertybrowser_->addProperty(itempropertylegendiconheightitem_);
  propertybrowser_->addProperty(itempropertylegendicontextpaddingitem_);
  propertybrowser_->addProperty(itempropertylegendborderstrokecoloritem_);
  propertybrowser_->addProperty(itempropertylegendborderstrokethicknessitem_);
  propertybrowser_->addProperty(itempropertylegendborderstroketypeitem_);
  propertybrowser_->addProperty(itempropertylegendbackgroundcoloritem_);

  boolManager_->setValue(itempropertylegendvisibleitem_,
                         legend->gethidden_legend());
  fontManager_->setValue(itempropertylegendfontitem_, legend->font());
  colorManager_->setValue(itempropertylegendtextcoloritem_,
                          legend->textColor());
  intManager_->setValue(itempropertylegendiconwidthitem_,
                        legend->iconSize().rwidth());
  intManager_->setValue(itempropertylegendiconheightitem_,
                        legend->iconSize().rheight());
  intManager_->setValue(itempropertylegendicontextpaddingitem_,
                        legend->iconTextPadding());
  colorManager_->setValue(itempropertylegendborderstrokecoloritem_,
                          legend->getborderstrokecolor_legend());
  doubleManager_->setValue(itempropertylegendborderstrokethicknessitem_,
                           legend->getborderstrokethickness_legend());
  enumManager_->setValue(itempropertylegendborderstroketypeitem_,
                         legend->getborderstrokestyle_legend() - 1);
  colorManager_->setValue(itempropertylegendbackgroundcoloritem_,
                          legend->brush().color());
}

void PropertyEditor::TextItem2DPropertyBlock(TextItem2D *textitem) {
  propertybrowser_->clear();

  propertybrowser_->addProperty(itempropertytextpixelpositionxitem_);
  propertybrowser_->addProperty(itempropertytextpixelpositionyitem_);
  propertybrowser_->addProperty(itempropertytextmarginitem_);
  propertybrowser_->addProperty(itempropertytexttextalignmentitem_);
  propertybrowser_->addProperty(itempropertytexttextitem_);
  propertybrowser_->addProperty(itempropertytextfontitem_);
  propertybrowser_->addProperty(itempropertytextcoloritem_);
  propertybrowser_->addProperty(itempropertytextantialiaseditem_);
  propertybrowser_->addProperty(itempropertytextstrokecoloritem_);
  propertybrowser_->addProperty(itempropertytextstrokethicknessitem_);
  propertybrowser_->addProperty(itempropertytextstroketypeitem_);
  propertybrowser_->addProperty(itempropertytextbackgroundcoloritem_);
  propertybrowser_->addProperty(itempropertytextrotationitem_);

  doubleManager_->setValue(itempropertytextpixelpositionxitem_,
                           textitem->position->pixelPosition().x());
  doubleManager_->setValue(itempropertytextpixelpositionyitem_,
                           textitem->position->pixelPosition().y());
  QRect rect;
  rect.setLeft(textitem->padding().left());
  rect.setTop(textitem->padding().top());
  rect.setRight(textitem->padding().right());
  rect.setBottom(textitem->padding().bottom());
  rectManager_->setValue(itempropertytextmarginitem_, rect);
  stringManager_->setValue(itempropertytexttextitem_,
                           Utilities::joinstring(textitem->text()));
  fontManager_->setValue(itempropertytextfontitem_, textitem->font());
  colorManager_->setValue(itempropertytextcoloritem_, textitem->color());
  boolManager_->setValue(itempropertytextantialiaseditem_,
                         textitem->antialiased());
  colorManager_->setValue(itempropertytextstrokecoloritem_,
                          textitem->getstrokecolor_textitem());
  doubleManager_->setValue(itempropertytextstrokethicknessitem_,
                           textitem->getstrokethickness_textitem());
  enumManager_->setValue(itempropertytextstroketypeitem_,
                         textitem->getstrokestyle_textitem() - 1);
  colorManager_->setValue(itempropertytextbackgroundcoloritem_,
                          textitem->brush().color());
  doubleManager_->setValue(itempropertytextrotationitem_, textitem->rotation());
  enumManager_->setValue(
      itempropertytexttextalignmentitem_,
      static_cast<int>(textitem->gettextalignment_textitem()));
}

void PropertyEditor::LineItem2DPropertyBlock(LineItem2D *lineitem) {
  propertybrowser_->clear();

  propertybrowser_->addProperty(itempropertylineantialiaseditem_);
  propertybrowser_->addProperty(itempropertylinestrokecoloritem_);
  propertybrowser_->addProperty(itempropertylinestrokethicknessitem_);
  propertybrowser_->addProperty(itempropertylinestroketypeitem_);
  propertybrowser_->addProperty(itempropertylineendingstyleitem_);
  propertybrowser_->addProperty(itempropertylineendingwidthitem_);
  propertybrowser_->addProperty(itempropertylineendinglengthitem_);
  propertybrowser_->addProperty(itempropertylinestartingstyleitem_);
  propertybrowser_->addProperty(itempropertylinestartingwidthitem_);
  propertybrowser_->addProperty(itempropertylinestartinglengthitem_);

  boolManager_->setValue(itempropertylineantialiaseditem_,
                         lineitem->antialiased());
  colorManager_->setValue(itempropertylinestrokecoloritem_,
                          lineitem->getstrokecolor_lineitem());
  doubleManager_->setValue(itempropertylinestrokethicknessitem_,
                           lineitem->getstrokethickness_lineitem());
  enumManager_->setValue(itempropertylinestroketypeitem_,
                         lineitem->getstrokestyle_lineitem() - 1);
  enumManager_->setValue(
      itempropertylineendingstyleitem_,
      lineitem->getendstyle_lineitem(LineItem2D::LineEndLocation::Stop));
  doubleManager_->setValue(
      itempropertylineendingwidthitem_,
      lineitem->getendwidth_lineitem(LineItem2D::LineEndLocation::Stop));
  doubleManager_->setValue(
      itempropertylineendinglengthitem_,
      lineitem->getendlength_lineitem(LineItem2D::LineEndLocation::Stop));
  enumManager_->setValue(
      itempropertylinestartingstyleitem_,
      lineitem->getendstyle_lineitem(LineItem2D::LineEndLocation::Start));
  doubleManager_->setValue(
      itempropertylinestartingwidthitem_,
      lineitem->getendwidth_lineitem(LineItem2D::LineEndLocation::Start));
  doubleManager_->setValue(
      itempropertylinestartinglengthitem_,
      lineitem->getendlength_lineitem(LineItem2D::LineEndLocation::Start));
}

void PropertyEditor::ImageItem2DPropertyBlock(ImageItem2D *imageitem) {
  propertybrowser_->clear();

  propertybrowser_->addProperty(itempropertyimagesourceitem_);
  propertybrowser_->addProperty(itempropertyimagestrokecoloritem_);
  propertybrowser_->addProperty(itempropertyimagestrokethicknessitem_);
  propertybrowser_->addProperty(itempropertyimagestroketypeitem_);

  stringManager_->setValue(itempropertyimagesourceitem_,
                           imageitem->getsource_imageitem());
  colorManager_->setValue(itempropertyimagestrokecoloritem_,
                          imageitem->getstrokecolor_imageitem());
  doubleManager_->setValue(itempropertyimagestrokethicknessitem_,
                           imageitem->getstrokethickness_imageitem());
  enumManager_->setValue(itempropertyimagestroketypeitem_,
                         imageitem->getstrokestyle_imageitem() - 1);
}

void PropertyEditor::LineScatter2DPropertyBlock(LineSpecial2D *lsgraph,
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
  stringManager_->setValue(
      lsplotpropertylegendtextitem_,
      Utilities::joinstring(lsgraph->getlegendtext_lsplot()));
}

void PropertyEditor::Curve2DPropertyBlock(Curve2D *curve,
                                          AxisRect2D *axisrect) {
  propertybrowser_->clear();

  propertybrowser_->addProperty(cplotpropertyxaxisitem_);
  propertybrowser_->addProperty(cplotpropertyyaxisitem_);
  propertybrowser_->addProperty(cplotpropertylinestyleitem_);
  propertybrowser_->addProperty(cplotpropertylinestrokecoloritem_);
  propertybrowser_->addProperty(cplotpropertylinestrokethicknessitem_);
  propertybrowser_->addProperty(cplotpropertylinestroketypeitem_);
  propertybrowser_->addProperty(cplotpropertylinefillstatusitem_);
  propertybrowser_->addProperty(cplotpropertylinefillcoloritem_);
  propertybrowser_->addProperty(cplotpropertylineantialiaseditem_);
  propertybrowser_->addProperty(cplotpropertyscatterstyleitem_);
  propertybrowser_->addProperty(cplotpropertyscatterthicknessitem_);
  propertybrowser_->addProperty(cplotpropertyscatterfillcoloritem_);
  propertybrowser_->addProperty(cplotpropertyscatterstrokecoloritem_);
  propertybrowser_->addProperty(cplotpropertyscatterstrokestyleitem_);
  propertybrowser_->addProperty(cplotpropertyscatterstrokethicknessitem_);
  propertybrowser_->addProperty(cplotpropertyscatterantialiaseditem_);
  propertybrowser_->addProperty(cplotpropertylegendtextitem_);
  {
    QStringList cyaxislist;
    int currentyaxis = 0;
    int ycount = 0;
    QList<Axis2D *> yaxes = axisrect->getYAxes2D();

    for (int i = 0; i < yaxes.size(); i++) {
      cyaxislist << QString("Y Axis %1").arg(i + 1);
      if (yaxes.at(i) == curve->getyaxis_cplot()) {
        currentyaxis = ycount;
      }
      ycount++;
    }
    enumManager_->setEnumNames(cplotpropertyyaxisitem_, cyaxislist);
    enumManager_->setValue(cplotpropertyyaxisitem_, currentyaxis);
  }

  {
    QStringList cxaxislist;
    int currentxaxis = 0;
    int xcount = 0;
    QList<Axis2D *> xaxes = axisrect->getXAxes2D();
    for (int i = 0; i < xaxes.size(); i++) {
      cxaxislist << QString("X Axis %1").arg(i + 1);
      if (xaxes.at(i) == curve->getxaxis_cplot()) {
        currentxaxis = xcount;
      }
      xcount++;
    }

    enumManager_->setEnumNames(cplotpropertyxaxisitem_, cxaxislist);
    enumManager_->setValue(cplotpropertyxaxisitem_, currentxaxis);
  }

  enumManager_->setValue(cplotpropertylinestyleitem_,
                         static_cast<int>(curve->getlinetype_cplot()));
  colorManager_->setValue(cplotpropertylinestrokecoloritem_,
                          curve->getlinestrokecolor_cplot());
  doubleManager_->setValue(cplotpropertylinestrokethicknessitem_,
                           curve->getlinestrokethickness_cplot());
  enumManager_->setValue(
      cplotpropertylinestroketypeitem_,
      static_cast<int>(curve->getlinestrokestyle_cplot() - 1));
  boolManager_->setValue(cplotpropertylinefillstatusitem_,
                         curve->getlinefillstatus_cplot());
  colorManager_->setValue(cplotpropertylinefillcoloritem_,
                          curve->getlinefillcolor_cplot());
  boolManager_->setValue(cplotpropertylineantialiaseditem_,
                         curve->getlineantialiased_cplot());
  enumManager_->setValue(cplotpropertyscatterstyleitem_,
                         static_cast<int>(curve->getscattershape_cplot()));
  doubleManager_->setValue(cplotpropertyscatterthicknessitem_,
                           curve->getscattersize_cplot());
  colorManager_->setValue(cplotpropertyscatterfillcoloritem_,
                          curve->getscatterfillcolor_cplot());
  colorManager_->setValue(cplotpropertyscatterstrokecoloritem_,
                          curve->getscatterstrokecolor_cplot());
  enumManager_->setValue(
      cplotpropertyscatterstrokestyleitem_,
      static_cast<int>(curve->getscatterstrokestyle_cplot() - 1));
  doubleManager_->setValue(cplotpropertyscatterstrokethicknessitem_,
                           curve->getscatterstrokethickness_cplot());
  boolManager_->setValue(cplotpropertyscatterantialiaseditem_,
                         curve->getscatterantialiased_cplot());
  stringManager_->setValue(cplotpropertylegendtextitem_,
                           Utilities::joinstring(curve->getlegendtext_cplot()));
}

void PropertyEditor::Bar2DPropertyBlock(Bar2D *bargraph, AxisRect2D *axisrect) {
  propertybrowser_->clear();

  propertybrowser_->addProperty(barplotpropertyxaxisitem_);
  propertybrowser_->addProperty(barplotpropertyyaxisitem_);
  propertybrowser_->addProperty(barplotpropertywidthitem_);
  propertybrowser_->addProperty(barplotpropertyfillantialiaseditem_);
  propertybrowser_->addProperty(barplotpropertyfillcoloritem_);
  propertybrowser_->addProperty(barplotpropertyantialiaseditem_);
  propertybrowser_->addProperty(barplotpropertystrokecoloritem_);
  propertybrowser_->addProperty(barplotpropertystrokethicknessitem_);
  propertybrowser_->addProperty(barplotpropertystrokestyleitem_);
  propertybrowser_->addProperty(barplotpropertylegendtextitem_);
  {
    QStringList baryaxislist;
    int currentyaxis = 0;
    int ycount = 0;
    QList<Axis2D *> yaxes = axisrect->getYAxes2D();

    for (int i = 0; i < yaxes.size(); i++) {
      baryaxislist << QString("Y Axis %1").arg(i + 1);
      if (yaxes.at(i) == bargraph->getyaxis_barplot()) {
        currentyaxis = ycount;
      }
      ycount++;
    }
    enumManager_->setEnumNames(barplotpropertyyaxisitem_, baryaxislist);
    enumManager_->setValue(barplotpropertyyaxisitem_, currentyaxis);
  }

  {
    QStringList barxaxislist;
    int currentxaxis = 0;
    int xcount = 0;
    QList<Axis2D *> xaxes = axisrect->getXAxes2D();
    for (int i = 0; i < xaxes.size(); i++) {
      barxaxislist << QString("X Axis %1").arg(i + 1);
      if (xaxes.at(i) == bargraph->getxaxis_barplot()) {
        currentxaxis = xcount;
      }
      xcount++;
    }

    enumManager_->setEnumNames(barplotpropertyxaxisitem_, barxaxislist);
    enumManager_->setValue(barplotpropertyxaxisitem_, currentxaxis);
  }

  doubleManager_->setValue(barplotpropertywidthitem_, bargraph->width());
  boolManager_->setValue(barplotpropertyfillantialiaseditem_,
                         bargraph->antialiasedFill());
  colorManager_->setValue(barplotpropertyfillcoloritem_,
                          bargraph->getfillcolor_barplot());
  boolManager_->setValue(barplotpropertyantialiaseditem_,
                         bargraph->antialiased());
  colorManager_->setValue(barplotpropertystrokecoloritem_,
                          bargraph->getstrokecolor_barplot());
  doubleManager_->setValue(barplotpropertystrokethicknessitem_,
                           bargraph->getstrokethickness_barplot());
  enumManager_->setValue(barplotpropertystrokestyleitem_,
                         bargraph->getstrokestyle_barplot() - 1);
  stringManager_->setValue(barplotpropertylegendtextitem_,
                           Utilities::joinstring(bargraph->name()));
}

void PropertyEditor::StatBox2DPropertyBlock(StatBox2D *statbox,
                                            AxisRect2D *axisrect) {
  propertybrowser_->clear();

  propertybrowser_->addProperty(statboxplotpropertyxaxisitem_);
  propertybrowser_->addProperty(statboxplotpropertyyaxisitem_);
  propertybrowser_->addProperty(statboxplotpropertywidthitem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerwidthitem_);
  propertybrowser_->addProperty(statboxplotpropertyboxstyleitem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerstyleitem_);
  propertybrowser_->addProperty(statboxplotpropertyantialiaseditem_);
  propertybrowser_->addProperty(statboxplotpropertyfillantialiaseditem_);
  propertybrowser_->addProperty(statboxplotpropertyfillstatusitem_);
  propertybrowser_->addProperty(statboxplotpropertyfillcoloritem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerantialiaseditem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerstrokecoloritem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerstrokethicknessitem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerstrokestyleitem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerbarstrokecoloritem_);
  propertybrowser_->addProperty(
      statboxplotpropertywhiskerbarstrokethicknessitem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerbarstrokestyleitem_);
  propertybrowser_->addProperty(statboxplotpropertymedianstrokecoloritem_);
  propertybrowser_->addProperty(statboxplotpropertymideanstrokethicknessitem_);
  propertybrowser_->addProperty(statboxplotpropertymideanstrokestyleitem_);
  propertybrowser_->addProperty(statboxplotpropertyscatterantialiaseditem_);
  propertybrowser_->addProperty(statboxplotpropertyscatterstyleitem_);
  propertybrowser_->addProperty(statboxplotpropertyscattersizeitem_);
  propertybrowser_->addProperty(statboxplotpropertyscatterfillcoloritem_);
  propertybrowser_->addProperty(statboxplotpropertyscatteroutlinecoloritem_);
  propertybrowser_->addProperty(
      statboxplotpropertyscatteroutlinethicknessitem_);
  propertybrowser_->addProperty(statboxplotpropertyscatteroutlinestyleitem_);
  propertybrowser_->addProperty(statboxplotpropertylegendtextitem_);

  {
    QStringList statboxyaxislist;
    int currentyaxis = 0;
    int ycount = 0;
    QList<Axis2D *> yaxes = axisrect->getYAxes2D();

    for (int i = 0; i < yaxes.size(); i++) {
      statboxyaxislist << QString("Y Axis %1").arg(i + 1);
      if (yaxes.at(i) == statbox->getyaxis_statbox()) {
        currentyaxis = ycount;
      }
      ycount++;
    }
    enumManager_->setEnumNames(statboxplotpropertyyaxisitem_, statboxyaxislist);
    enumManager_->setValue(statboxplotpropertyyaxisitem_, currentyaxis);
  }

  {
    QStringList statboxxaxislist;
    int currentxaxis = 0;
    int xcount = 0;
    QList<Axis2D *> xaxes = axisrect->getXAxes2D();
    for (int i = 0; i < xaxes.size(); i++) {
      statboxxaxislist << QString("X Axis %1").arg(i + 1);
      if (xaxes.at(i) == statbox->getxaxis_statbox()) {
        currentxaxis = xcount;
      }
      xcount++;
    }

    enumManager_->setEnumNames(statboxplotpropertyxaxisitem_, statboxxaxislist);
    enumManager_->setValue(statboxplotpropertyxaxisitem_, currentxaxis);
  }

  doubleManager_->setValue(statboxplotpropertywidthitem_, statbox->width());
  doubleManager_->setValue(statboxplotpropertywhiskerwidthitem_,
                           statbox->whiskerWidth());
  enumManager_->setValue(statboxplotpropertyboxstyleitem_,
                         statbox->getboxstyle_statbox());
  enumManager_->setValue(statboxplotpropertywhiskerstyleitem_,
                         statbox->getwhiskerstyle_statbox());
  boolManager_->setValue(statboxplotpropertyantialiaseditem_,
                         statbox->antialiased());
  boolManager_->setValue(statboxplotpropertyfillantialiaseditem_,
                         statbox->antialiasedFill());
  boolManager_->setValue(statboxplotpropertyfillstatusitem_,
                         statbox->getfillstatus_statbox());
  colorManager_->setValue(statboxplotpropertyfillcoloritem_,
                          statbox->getfillcolor_statbox());
  boolManager_->setValue(statboxplotpropertywhiskerantialiaseditem_,
                         statbox->whiskerAntialiased());
  colorManager_->setValue(statboxplotpropertywhiskerstrokecoloritem_,
                          statbox->getwhiskerstrokecolor_statbox());
  doubleManager_->setValue(statboxplotpropertywhiskerstrokethicknessitem_,
                           statbox->getwhiskerstrokethickness_statbox());
  enumManager_->setValue(statboxplotpropertywhiskerstrokestyleitem_,
                         statbox->getwhiskerstrokestyle_statbox() - 1);
  colorManager_->setValue(statboxplotpropertywhiskerbarstrokecoloritem_,
                          statbox->getwhiskerbarstrokecolor_statbox());
  doubleManager_->setValue(statboxplotpropertywhiskerbarstrokethicknessitem_,
                           statbox->getwhiskerbarstrokethickness_statbox());
  enumManager_->setValue(statboxplotpropertywhiskerbarstrokestyleitem_,
                         statbox->getwhiskerbarstrokestyle_statbox() - 1);
  colorManager_->setValue(statboxplotpropertymedianstrokecoloritem_,
                          statbox->getmedianstrokecolor_statbox());
  doubleManager_->setValue(statboxplotpropertymideanstrokethicknessitem_,
                           statbox->getmedianstrokethickness_statbox());
  enumManager_->setValue(statboxplotpropertymideanstrokestyleitem_,
                         statbox->getmedianstrokestyle_statbox() - 1);
  boolManager_->setValue(statboxplotpropertyscatterantialiaseditem_,
                         statbox->antialiasedScatters());
  enumManager_->setValue(statboxplotpropertyscatterstyleitem_,
                         static_cast<int>(statbox->getscattershape_statbox()));
  doubleManager_->setValue(statboxplotpropertyscattersizeitem_,
                           statbox->getscattersize_statbox());
  colorManager_->setValue(statboxplotpropertyscatterfillcoloritem_,
                          statbox->getscatterfillcolor_statbox());
  colorManager_->setValue(statboxplotpropertyscatteroutlinecoloritem_,
                          statbox->getscatterstrokecolor_statbox());
  doubleManager_->setValue(statboxplotpropertyscatteroutlinethicknessitem_,
                           statbox->getscatterstrokethickness_statbox());
  enumManager_->setValue(statboxplotpropertyscatteroutlinestyleitem_,
                         statbox->getscatterstrokestyle_statbox() - 1);
  stringManager_->setValue(statboxplotpropertylegendtextitem_,
                           Utilities::joinstring(statbox->name()));
}

void PropertyEditor::Vector2DPropertyBlock(Vector2D *vectorgraph,
                                           AxisRect2D *axisrect) {
  propertybrowser_->clear();

  propertybrowser_->addProperty(vectorpropertyxaxisitem_);
  propertybrowser_->addProperty(vectorpropertyyaxisitem_);
  propertybrowser_->addProperty(vectorpropertylinestrokecoloritem_);
  propertybrowser_->addProperty(vectorpropertylinestrokethicknessitem_);
  propertybrowser_->addProperty(vectorpropertylinestroketypeitem_);
  propertybrowser_->addProperty(vectorpropertylineendingtypeitem_);
  propertybrowser_->addProperty(vectorpropertylineendingheightitem_);
  propertybrowser_->addProperty(vectorpropertylineendingwidthitem_);
  propertybrowser_->addProperty(vectorpropertylineantialiaseditem_);
  propertybrowser_->addProperty(vectorpropertylegendtextitem_);
  {
    QStringList vectoryaxislist;
    int currentyaxis = 0;
    int ycount = 0;
    QList<Axis2D *> yaxes = axisrect->getYAxes2D();

    for (int i = 0; i < yaxes.size(); i++) {
      vectoryaxislist << QString("Y Axis %1").arg(i + 1);
      if (yaxes.at(i) == vectorgraph->getyaxis_vecplot()) {
        currentyaxis = ycount;
      }
      ycount++;
    }
    enumManager_->setEnumNames(vectorpropertyyaxisitem_, vectoryaxislist);
    enumManager_->setValue(vectorpropertyyaxisitem_, currentyaxis);
  }

  {
    QStringList vectorxaxislist;
    int currentxaxis = 0;
    int xcount = 0;
    QList<Axis2D *> xaxes = axisrect->getXAxes2D();
    for (int i = 0; i < xaxes.size(); i++) {
      vectorxaxislist << QString("X Axis %1").arg(i + 1);
      if (xaxes.at(i) == vectorgraph->getxaxis_vecplot()) {
        currentxaxis = xcount;
      }
      xcount++;
    }

    enumManager_->setEnumNames(vectorpropertyxaxisitem_, vectorxaxislist);
    enumManager_->setValue(vectorpropertyxaxisitem_, currentxaxis);
  }
  colorManager_->setValue(vectorpropertylinestrokecoloritem_,
                          vectorgraph->getlinestrokecolor_vecplot());
  doubleManager_->setValue(vectorpropertylinestrokethicknessitem_,
                           vectorgraph->getlinestrokethickness_vecplot());
  enumManager_->setValue(
      vectorpropertylinestroketypeitem_,
      static_cast<int>(vectorgraph->getlinestrokestyle_vecplot() - 1));
  enumManager_->setValue(vectorpropertylineendingtypeitem_,
                         static_cast<int>(vectorgraph->getendstyle_vecplot(
                             Vector2D::LineEndLocation::Start)));
  doubleManager_->setValue(
      vectorpropertylineendingheightitem_,
      vectorgraph->getendheight_vecplot(Vector2D::LineEndLocation::Start));
  doubleManager_->setValue(
      vectorpropertylineendingwidthitem_,
      vectorgraph->getendwidth_vecplot(Vector2D::LineEndLocation::Start));
  boolManager_->setValue(vectorpropertylineantialiaseditem_,
                         vectorgraph->getlineantialiased_vecplot());
  stringManager_->setValue(
      vectorpropertylegendtextitem_,
      Utilities::joinstring(vectorgraph->getlegendtext_vecplot()));
}

void PropertyEditor::Pie2DPropertyBlock(Pie2D *piegraph, AxisRect2D *axisrect) {
  propertybrowser_->clear();

  // Pie Properties Block
  propertybrowser_->addProperty(pieplotpropertylinestrokecoloritem_);
  propertybrowser_->addProperty(pieplotpropertylinestrokethicknessitem_);
  propertybrowser_->addProperty(pieplotpropertylinestroketypeitem_);
  propertybrowser_->addProperty(pieplotpropertymarginpercentitem_);

  colorManager_->setValue(pieplotpropertylinestrokecoloritem_,
                          piegraph->getstrokecolor_pieplot());
  doubleManager_->setValue(pieplotpropertylinestrokethicknessitem_,
                           piegraph->getstrokethickness_pieplot());
  enumManager_->setValue(pieplotpropertylinestroketypeitem_,
                         piegraph->getstrokestyle_pieplot() - 1);
  intManager_->setValue(pieplotpropertymarginpercentitem_,
                        piegraph->getmarginpercent_pieplot());
}

void PropertyEditor::axisRectCreated(AxisRect2D *axisrect, MyWidget *widget) {
  Q_UNUSED(axisrect);
  populateObjectBrowser(widget);
}

void PropertyEditor::objectschanged() {
  ApplicationWindow *app_ = qobject_cast<ApplicationWindow *>(parent());
  if (app_) {
    MyWidget *mywidget = app_->getactiveMyWidget();
    populateObjectBrowser(mywidget);
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
      griditem->setData(0, Qt::UserRole + 1,
                        QVariant::fromValue<void *>(element));
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

      // Legend
      QTreeWidgetItem *legenditem = new QTreeWidgetItem(
          static_cast<QTreeWidget *>(nullptr), QStringList(QString("Legend")));
      legenditem->setIcon(
          0, IconLoader::load("edit-legend", IconLoader::LightDark));
      legenditem->setData(
          0, Qt::UserRole,
          static_cast<int>(MyTreeWidget::PropertyItemType::Legend));
      Legend2D *legend = element->getLegend();
      legenditem->setData(0, Qt::UserRole + 1,
                          QVariant::fromValue<void *>(legend));
      item->addChild(legenditem);

      // Text items
      QVector<TextItem2D *> textitems = element->getTextItemVec();
      for (int j = 0; j < textitems.size(); j++) {
        QTreeWidgetItem *textitem =
            new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr));
        QString text = QString("Text Item: " + QString::number(j + 1));
        textitem->setIcon(0,
                          IconLoader::load("draw-text", IconLoader::LightDark));
        textitem->setText(0, text);
        textitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::TextItem));
        textitem->setData(0, Qt::UserRole + 1,
                          QVariant::fromValue<void *>(textitems.at(j)));
        item->addChild(textitem);
      }

      // Line items
      QVector<LineItem2D *> lineitems = element->getLineItemVec();
      for (int j = 0; j < lineitems.size(); j++) {
        QTreeWidgetItem *lineitem =
            new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr));
        QString text = QString("Line Item: " + QString::number(j + 1));
        lineitem->setIcon(0,
                          IconLoader::load("draw-line", IconLoader::LightDark));
        lineitem->setText(0, text);
        lineitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::LineItem));
        lineitem->setData(0, Qt::UserRole + 1,
                          QVariant::fromValue<void *>(lineitems.at(j)));
        item->addChild(lineitem);
      }

      // Image items
      QVector<ImageItem2D *> imageitems = element->getImageItemVec();
      for (int j = 0; j < imageitems.size(); j++) {
        QTreeWidgetItem *imageitem =
            new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr));
        QString text = QString("Image Item: " + QString::number(j + 1));
        imageitem->setIcon(
            0, IconLoader::load("view-image", IconLoader::LightDark));
        imageitem->setText(0, text);
        imageitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::ImageItem));
        imageitem->setData(0, Qt::UserRole + 1,
                           QVariant::fromValue<void *>(imageitems.at(j)));
        item->addChild(imageitem);
      }

      // LineSpecial plot Items
      LsVec graphvec = element->getLsVec();
      for (int j = 0; j < graphvec.size(); j++) {
        LineSpecial2D *lsgraph = graphvec.at(j);
        QString lsgraphtext = QString("Line Special %1").arg(j + 1);
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

      // Curve plot Items
      CurveVec curvevec = element->getCurveVec();
      for (int j = 0; j < curvevec.size(); j++) {
        Curve2D *curvegraph = curvevec.at(j);
        QString curvegraphtext = QString("Line Scatter %1").arg(j + 1);
        QTreeWidgetItem *curvegraphitem = new QTreeWidgetItem(
            static_cast<QTreeWidget *>(nullptr), QStringList(curvegraphtext));
        switch (curvegraph->getplottype_cplot()) {
          case Graph2DCommon::PlotType::Associated:
            if (curvegraph->getcurvetype_cplot() == Curve2D::Curve2DType::Curve)
              curvegraphitem->setIcon(
                  0, IconLoader::load("graph2d-line", IconLoader::LightDark));
            else
              curvegraphitem->setIcon(
                  0, IconLoader::load("graph2d-spline", IconLoader::LightDark));
            break;
          case Graph2DCommon::PlotType::Function:
            curvegraphitem->setIcon(0, IconLoader::load("graph2d-function-xy",
                                                        IconLoader::LightDark));
            break;
        }
        curvegraphitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::Curve));
        curvegraphitem->setData(0, Qt::UserRole + 1,
                                QVariant::fromValue<void *>(curvegraph));
        curvegraphitem->setData(0, Qt::UserRole + 2,
                                QVariant::fromValue<void *>(element));
        item->addChild(curvegraphitem);
      }

      // Statbox plot Items
      QVector<StatBox2D *> statboxvec = element->getStatBoxVec();
      for (int j = 0; j < statboxvec.size(); j++) {
        StatBox2D *statbox = statboxvec.at(j);
        QString statboxtext = QString("Statbox %1").arg(j + 1);
        QTreeWidgetItem *statboxitem = new QTreeWidgetItem(
            static_cast<QTreeWidget *>(nullptr), QStringList(statboxtext));
        statboxitem->setIcon(
            0, IconLoader::load("graph2d-box", IconLoader::LightDark));
        statboxitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::StatBox));
        statboxitem->setData(0, Qt::UserRole + 1,
                             QVariant::fromValue<void *>(statbox));
        statboxitem->setData(0, Qt::UserRole + 2,
                             QVariant::fromValue<void *>(element));
        item->addChild(statboxitem);
      }

      // Vector plot Items
      VectorVec vectorvec = element->getVectorVec();
      for (int j = 0; j < vectorvec.size(); j++) {
        Vector2D *vector = vectorvec.at(j);
        QString vectortext = QString("Vector %1").arg(j + 1);
        QTreeWidgetItem *vectoritem = new QTreeWidgetItem(
            static_cast<QTreeWidget *>(nullptr), QStringList(vectortext));
        vectoritem->setIcon(
            0, IconLoader::load("graph2d-vector-xy", IconLoader::LightDark));
        vectoritem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::Vector));
        vectoritem->setData(0, Qt::UserRole + 1,
                            QVariant::fromValue<void *>(vector));
        vectoritem->setData(0, Qt::UserRole + 2,
                            QVariant::fromValue<void *>(element));
        item->addChild(vectoritem);
      }

      // Bar Plot items
      BarVec barvec = element->getBarVec();
      for (int j = 0; j < barvec.size(); j++) {
        Bar2D *bar = barvec.at(j);
        QString bartext = QString("Bar %1").arg(j + 1);
        QTreeWidgetItem *baritem = new QTreeWidgetItem(
            static_cast<QTreeWidget *>(nullptr), QStringList(bartext));
        if (bar->ishistogram_barplot()) {
          baritem->setIcon(
              0, IconLoader::load("graph2d-histogram", IconLoader::LightDark));
        } else {
          baritem->setIcon(0, IconLoader::load("graph2d-vertical-bar",
                                               IconLoader::LightDark));
        }
        baritem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::BarGraph));
        baritem->setData(0, Qt::UserRole + 1, QVariant::fromValue<void *>(bar));
        baritem->setData(0, Qt::UserRole + 2,
                         QVariant::fromValue<void *>(element));
        item->addChild(baritem);
      }

      // Pie Plot Items
      QVector<Pie2D *> pievec = element->getPieVec();
      for (int j = 0; j < pievec.size(); j++) {
        Pie2D *pie = pievec.at(j);
        QString pietext = QString("Pie %1").arg(j + 1);
        QTreeWidgetItem *pieitem = new QTreeWidgetItem(
            static_cast<QTreeWidget *>(nullptr), QStringList(pietext));
        pieitem->setIcon(
            0, IconLoader::load("graph2d-pie", IconLoader::LightDark));
        pieitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::PieGraph));
        pieitem->setData(0, Qt::UserRole + 1, QVariant::fromValue<void *>(pie));
        pieitem->setData(0, Qt::UserRole + 2,
                         QVariant::fromValue<void *>(element));
        item->addChild(pieitem);
      }

      objectitems_.append(item);
      // if (!elementslist.isEmpty()) {
      // elementslist.at(0)->parentPlot()->replot();
      //}
      axisrectConnections(elementslist.at(i));
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

void PropertyEditor::axisrectConnections(AxisRect2D *axisrect) {
  // created
  connect(axisrect, SIGNAL(Axis2DCreated(Axis2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(TextItem2DCreated(TextItem2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(LineItem2DCreated(LineItem2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(ImageItem2DCreated(ImageItem2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(LineScatter2DCreated(LineSpecial2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(Curve2DCreated(Curve2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(StatBox2DCreated(StatBox2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(Vector2DCreated(Vector2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(Bar2DCreated(Bar2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(Pie2DCreated(Pie2D *)), this,
          SLOT(objectschanged()));

  // Removed
  connect(axisrect, SIGNAL(Axis2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(TextItem2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(LineItem2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(ImageItem2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(LineScatter2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(Curve2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(StatBox2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(Vector2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(Bar2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
  connect(axisrect, SIGNAL(Pie2DRemoved(AxisRect2D *)), this,
          SLOT(objectschanged()));
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
  axispropertyticklabelformatitem_->setPropertyId(
      "axispropertyticklabelformatitem_");
  axispropertyticklabelprecisionitem_->setPropertyId(
      "axispropertyticklabelprecisionitem_");
  // Legend Properties
  itempropertylegendvisibleitem_->setPropertyId(
      "itempropertylegendvisibleitem_");
  itempropertylegendfontitem_->setPropertyId("itempropertylegendfontitem_");
  itempropertylegendtextcoloritem_->setPropertyId(
      "itempropertylegendtextcoloritem_");
  itempropertylegendiconwidthitem_->setPropertyId(
      "itempropertylegendiconwidthitem_");
  itempropertylegendiconheightitem_->setPropertyId(
      "itempropertylegendiconheightitem_");
  itempropertylegendicontextpaddingitem_->setPropertyId(
      "itempropertylegendicontextpaddingitem_");
  itempropertylegendborderstrokecoloritem_->setPropertyId(
      "itempropertylegendborderstrokecoloritem_");
  itempropertylegendborderstrokethicknessitem_->setPropertyId(
      "itempropertylegendborderstrokethicknessitem_");
  itempropertylegendborderstroketypeitem_->setPropertyId(
      "itempropertylegendborderstroketypeitem_");
  itempropertylegendbackgroundcoloritem_->setPropertyId(
      "itempropertylegendbackgroundcoloritem_");
  // Text Item Properties block
  itempropertytextpixelpositionxitem_->setPropertyId(
      "itempropertytextpixelpositionxitem_");
  itempropertytextpixelpositionyitem_->setPropertyId(
      "itempropertytextpixelpositionyitem_");
  itempropertytextmarginitem_->setPropertyId("itempropertytextmarginitem_");
  itempropertytexttextitem_->setPropertyId("itempropertytexttextitem_");
  itempropertytextfontitem_->setPropertyId("itempropertytextfontitem_");
  itempropertytextcoloritem_->setPropertyId("itempropertytextcoloritem_");
  itempropertytextantialiaseditem_->setPropertyId(
      "itempropertytextantialiaseditem_");
  itempropertytextstrokecoloritem_->setPropertyId(
      "itempropertytextstrokecoloritem_");
  itempropertytextstrokethicknessitem_->setPropertyId(
      "itempropertytextstrokethicknessitem_");
  itempropertytextstroketypeitem_->setPropertyId(
      "itempropertytextstroketypeitem_");
  itempropertytextbackgroundcoloritem_->setPropertyId(
      "itempropertytextbackgroundcoloritem_");
  itempropertytextrotationitem_->setPropertyId("itempropertytextrotationitem_");
  itempropertytextpositionalignmentitem_->setPropertyId(
      "itempropertytextpositionalignmentitem_");
  itempropertytexttextalignmentitem_->setPropertyId(
      "itempropertytexttextalignmentitem_");
  // Line Item Properties
  itempropertylineantialiaseditem_->setPropertyId(
      "itempropertylineantialiaseditem_");
  itempropertylinestrokecoloritem_->setPropertyId(
      "itempropertylinestrokecoloritem_");
  itempropertylinestrokethicknessitem_->setPropertyId(
      "itempropertylinestrokethicknessitem_");
  itempropertylinestroketypeitem_->setPropertyId(
      "itempropertylinestroketypeitem_");
  itempropertylineendingstyleitem_->setPropertyId(
      "itempropertylineendingstyleitem_");
  itempropertylineendingwidthitem_->setPropertyId(
      "itempropertylineendingwidthitem_");
  itempropertylineendinglengthitem_->setPropertyId(
      "itempropertylineendinglengthitem_");
  itempropertylinestartingstyleitem_->setPropertyId(
      "itempropertylinestartingstyleitem_");
  itempropertylinestartingwidthitem_->setPropertyId(
      "itempropertylinestartingwidthitem_");
  itempropertylinestartinglengthitem_->setPropertyId(
      "itempropertylinestartinglengthitem_");
  // Image Item Properties
  itempropertyimagesourceitem_->setPropertyId("itempropertyimagesourceitem_");
  itempropertyimagestrokecoloritem_->setPropertyId(
      "itempropertyimagestrokecoloritem_");
  itempropertyimagestrokethicknessitem_->setPropertyId(
      "itempropertyimagestrokethicknessitem_");
  itempropertyimagestroketypeitem_->setPropertyId(
      "itempropertyimagestroketypeitem_");
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
  // Curve Property Block
  cplotpropertyxaxisitem_->setPropertyId("cplotpropertyxaxisitem_");
  cplotpropertyyaxisitem_->setPropertyId("cplotpropertyyaxisitem_");
  cplotpropertylinestyleitem_->setPropertyId("cplotpropertylinestyleitem_");
  cplotpropertylinestrokecoloritem_->setPropertyId(
      "cplotpropertylinestrokecoloritem_");
  cplotpropertylinestrokethicknessitem_->setPropertyId(
      "cplotpropertylinestrokethicknesitem_");
  cplotpropertylinestroketypeitem_->setPropertyId(
      "cplotpropertylinestroketypeitem_");
  cplotpropertylinefillstatusitem_->setPropertyId(
      "cplotpropertylinefillstatusitem_");
  cplotpropertylinefillcoloritem_->setPropertyId(
      "cplotpropertylinefillcoloritem_");
  cplotpropertylineantialiaseditem_->setPropertyId(
      "cplotpropertylineantialiaseditem_");
  cplotpropertyscatterstyleitem_->setPropertyId(
      "cplotpropertyscatterstyleitem_");
  cplotpropertyscatterthicknessitem_->setPropertyId(
      "cplotpropertyscatterthicknessitem_");
  cplotpropertyscatterfillcoloritem_->setPropertyId(
      "cplotpropertyscatterfillcoloritem_");
  cplotpropertyscatterstrokecoloritem_->setPropertyId(
      "cplotpropertyscatterstrokecoloritem_");
  cplotpropertyscatterstrokethicknessitem_->setPropertyId(
      "cplotpropertyscatterstrokethicknessitem_");
  cplotpropertyscatterstrokestyleitem_->setPropertyId(
      "cplotpropertyscatterstrokestyleitem_");
  cplotpropertyscatterantialiaseditem_->setPropertyId(
      "cplotpropertyscatterantialiaseditem_");
  cplotpropertylegendtextitem_->setPropertyId("cplotpropertylelegendtextitem_");

  // Box Properties block
  barplotpropertyxaxisitem_->setPropertyId("barplotpropertyxaxisitem_");
  barplotpropertyyaxisitem_->setPropertyId("barplotpropertyyaxisitem_");
  barplotpropertywidthitem_->setPropertyId("barplotpropertywidthitem_");
  barplotpropertyfillantialiaseditem_->setPropertyId(
      "barplotpropertyfillantialiaseditem_");
  barplotpropertyfillcoloritem_->setPropertyId("barplotpropertyfillcoloritem_");
  barplotpropertyantialiaseditem_->setPropertyId(
      "barplotpropertyantialiaseditem_");
  barplotpropertystrokecoloritem_->setPropertyId(
      "barplotpropertystrokecoloritem_");
  barplotpropertystrokethicknessitem_->setPropertyId(
      "barplotpropertystrokethicknessitem_");
  barplotpropertystrokestyleitem_->setPropertyId(
      "barplotpropertystrokestyleitem_");
  barplotpropertylegendtextitem_->setPropertyId(
      "barplotpropertylegendtextitem_");

  // StatBox Properties block
  statboxplotpropertyxaxisitem_->setPropertyId("statboxplotpropertyxaxisitem_");
  statboxplotpropertyyaxisitem_->setPropertyId("statboxplotpropertyyaxisitem_");
  statboxplotpropertywidthitem_->setPropertyId("statboxplotpropertywidthitem_");
  statboxplotpropertywhiskerwidthitem_->setPropertyId(
      "statboxplotpropertywhiskerwidthitem_");
  statboxplotpropertyboxstyleitem_->setPropertyId(
      "statboxplotpropertyboxstyleitem_");
  statboxplotpropertywhiskerstyleitem_->setPropertyId(
      "statboxplotpropertywhiskerstyleitem_");
  statboxplotpropertyantialiaseditem_->setPropertyId(
      "statboxplotpropertyantialiaseditem_");
  statboxplotpropertyfillantialiaseditem_->setPropertyId(
      "statboxplotpropertyfillantialiaseditem_");
  statboxplotpropertyfillstatusitem_->setPropertyId(
      "statboxplotpropertyfillstatusitem_");
  statboxplotpropertyfillcoloritem_->setPropertyId(
      "statboxplotpropertyfillcoloritem_");
  statboxplotpropertywhiskerantialiaseditem_->setPropertyId(
      "statboxplotpropertywhiskerantialiaseditem_");
  statboxplotpropertywhiskerstrokecoloritem_->setPropertyId(
      "statboxplotpropertywhiskerstrokecoloritem_");
  statboxplotpropertywhiskerstrokethicknessitem_->setPropertyId(
      "statboxplotpropertywhiskerstrokethicknessitem_");
  statboxplotpropertywhiskerstrokestyleitem_->setPropertyId(
      "statboxplotpropertywhiskerstrokestyleitem_");
  statboxplotpropertywhiskerbarstrokecoloritem_->setPropertyId(
      "statboxplotpropertywhiskerbarstrokecoloritem_");
  statboxplotpropertywhiskerbarstrokethicknessitem_->setPropertyId(
      "statboxplotpropertywhiskerbarstrokethicknessitem_");
  statboxplotpropertywhiskerbarstrokestyleitem_->setPropertyId(
      "statboxplotpropertywhiskerbarstrokestyleitem_");
  statboxplotpropertymedianstrokecoloritem_->setPropertyId(
      "statboxplotpropertymedianstrokecoloritem_");
  statboxplotpropertymideanstrokethicknessitem_->setPropertyId(
      "statboxplotpropertymideanstrokethicknessitem_");
  statboxplotpropertymideanstrokestyleitem_->setPropertyId(
      "statboxplotpropertymideanstrokestyleitem_");
  statboxplotpropertyscatterantialiaseditem_->setPropertyId(
      "statboxplotpropertyscatterantialiaseditem_");
  statboxplotpropertyscatterstyleitem_->setPropertyId(
      "statboxplotpropertyscatterstyleitem_");
  statboxplotpropertyscattersizeitem_->setPropertyId(
      "statboxplotpropertyscattersizeitem_");
  statboxplotpropertyscatterfillcoloritem_->setPropertyId(
      "statboxplotpropertyscatterfillcoloritem_");
  statboxplotpropertyscatteroutlinecoloritem_->setPropertyId(
      "statboxplotpropertyscatteroutlinecoloritem_");
  statboxplotpropertyscatteroutlinethicknessitem_->setPropertyId(
      "statboxplotpropertyscatteroutlinethicknessitem_");
  statboxplotpropertyscatteroutlinestyleitem_->setPropertyId(
      "statboxplotpropertyscatteroutlinestyleitem_");
  statboxplotpropertylegendtextitem_->setPropertyId(
      "statboxplotpropertylegendtextitem_");

  // Vector property block
  vectorpropertyxaxisitem_->setPropertyId("vectorpropertyxaxisitem_");
  vectorpropertyyaxisitem_->setPropertyId("vectorpropertyyaxisitem_");
  vectorpropertylinestrokecoloritem_->setPropertyId(
      "vectorpropertylinestrokecoloritem_");
  vectorpropertylinestrokethicknessitem_->setPropertyId(
      "vectorpropertylinestrokethicknessitem_");
  vectorpropertylinestroketypeitem_->setPropertyId(
      "vectorpropertylinestroketypeitem_");
  vectorpropertylineendingtypeitem_->setPropertyId(
      "vectorpropertylineendingtypeitem_");
  vectorpropertylineendingheightitem_->setPropertyId(
      "vectorpropertylineendingheightitem_");
  vectorpropertylineendingwidthitem_->setPropertyId(
      "vectorpropertylineendingwidthitem_");
  vectorpropertylineantialiaseditem_->setPropertyId(
      "vectorpropertylineantialiaseditem_");
  vectorpropertylegendtextitem_->setPropertyId("vectorpropertylegendtextitem_");

  // Pie Properties Block
  pieplotpropertylinestrokecoloritem_->setPropertyId(
      "pieplotpropertylinestrokecoloritem_");
  pieplotpropertylinestrokethicknessitem_->setPropertyId(
      "pieplotpropertylinestrokethicknessitem_");
  pieplotpropertylinestroketypeitem_->setPropertyId(
      "pieplotpropertylinestroketypeitem_");
  pieplotpropertymarginpercentitem_->setPropertyId(
      "pieplotpropertymarginpercentitem_");

  // Grid Block
  hgridaxispropertycomboitem_->setPropertyId("hgridaxispropertycomboitem_");
  hmajgridpropertyvisibleitem_->setPropertyId("hmajgridpropertyvisibleitem_");
  hmajgridpropertystrokecoloritem_->setPropertyId(
      "hmajgridpropertystrokecoloritem_");
  hmajgridpropertystrokethicknessitem_->setPropertyId(
      "hmajgridpropertystrokethicknessitem_");
  hmajgridpropertystroketypeitem_->setPropertyId(
      "hmajgridpropertystroketypeitem_");
  hmajgridpropertyzerolinevisibleitem_->setPropertyId(
      "hmajgridpropertyzerolinevisibleitem_");
  hmajgridpropertyzerolinestrokecoloritem_->setPropertyId(
      "hmajgridpropertyzerolinestrokecoloritem_");
  hmajgridpropertyzerolinestrokethicknessitem_->setPropertyId(
      "hmajgridpropertyzerolinestrokethicknessitem_");
  hmajgridpropertyzerolinestroketypeitem_->setPropertyId(
      "hmajgridpropertyzerolinestroketypeitem_");
  hmingridpropertyvisibleitem_->setPropertyId("hmingridpropertyvisibleitem_");
  hmingridpropertystrokecoloritem_->setPropertyId(
      "hmingridpropertystrokecoloritem_");
  hmingridpropertystrokethicknessitem_->setPropertyId(
      "hmingridpropertystrokethicknessitem_");
  hmingridpropertystroketypeitem_->setPropertyId(
      "hmingridpropertystroketypeitem_");
  vgridaxispropertycomboitem_->setPropertyId("vgridaxispropertycomboitem_");
  vmajgridpropertyvisibleitem_->setPropertyId("vmajgridpropertyvisibleitem_");
  vmajgridpropertystrokecoloritem_->setPropertyId(
      "vmajgridpropertystrokecoloritem_");
  vmajgridpropertystrokethicknessitem_->setPropertyId(
      "vmajgridpropertystrokethicknessitem_");
  vmajgridpropertystroketypeitem_->setPropertyId(
      "vmajgridpropertystroketypeitem_");
  vmajgridpropertyzerolinevisibleitem_->setPropertyId(
      "vmajgridpropertyzerolinevisibleitem_");
  vmajgridpropertyzerolinestrokecoloritem_->setPropertyId(
      "vmajgridpropertyzerolinestrokecoloritem_");
  vmajgridpropertyzerolinestrokethicknessitem_->setPropertyId(
      "vmajgridpropertyzerolinestrokethicknessitem_");
  vmajgridpropertyzerolinestroketypeitem_->setPropertyId(
      "vmajgridpropertyzerolinestroketypeitem_");
  vmingridpropertyvisibleitem_->setPropertyId("vmingridpropertyvisibleitem_");
  vmingridpropertystrokecoloritem_->setPropertyId(
      "vmingridpropertystrokecoloritem_");
  vmingridpropertystrokethicknessitem_->setPropertyId(
      "vmingridpropertystrokethicknessitem_");
  vmingridpropertystroketypeitem_->setPropertyId(
      "vmingridpropertystroketypeitem_");
}
