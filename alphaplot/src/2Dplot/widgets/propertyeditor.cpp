#include "propertyeditor.h"

#include <QDebug>
#include <QSplitter>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "../3rdparty/propertybrowser/qteditorfactory.h"
#include "../3rdparty/propertybrowser/qtpropertymanager.h"
#include "../3rdparty/propertybrowser/qttreepropertybrowser.h"
#include "2Dplot/ColorMap2D.h"
#include "2Dplot/DataManager2D.h"
#include "2Dplot/ErrorBar2D.h"
#include "2Dplot/ImageItem2D.h"
#include "2Dplot/Layout2D.h"
#include "2Dplot/Legend2D.h"
#include "2Dplot/LineItem2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "3Dplot/Graph3D.h"
#include "Matrix.h"
#include "MyWidget.h"
#include "Note.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/Utilities.h"
#include "ui_propertyeditor.h"

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

  // Plot Canvas properties
  canvaspropertycoloritem_ = colorManager_->addProperty("Background Color");
  canvaspropertybufferdevicepixelratioitem_ =
      doubleManager_->addProperty("Device Pixel Ratio");
  canvaspropertyopenglitem_ = boolManager_->addProperty("OpenGL");
  canvaspropertyrectitem_ = rectManager_->addProperty(tr("Plot Dimension"));

  // Layout Properties
  layoutpropertygroupitem_ = groupManager_->addProperty(tr("Layout"));
  layoutpropertyrectitem_ = rectManager_->addProperty(tr("Outer Rect"));
  layoutpropertygroupitem_->addSubProperty(layoutpropertyrectitem_);
  layoutpropertycoloritem_ = colorManager_->addProperty(tr("Background Color"));
  layoutpropertygroupitem_->addSubProperty(layoutpropertycoloritem_);
  layoutpropertyrectitem_->setEnabled(false);
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
  axispropertytickcountitem_ = intManager_->addProperty("Count");
  axispropertytickvisibilityitem_->addSubProperty(axispropertytickcountitem_);
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
  itempropertylegendoriginxitem_ = doubleManager_->addProperty("Position X");
  itempropertylegendoriginyitem_ = doubleManager_->addProperty("Position Y");
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
  doubleManager_->setRange(itempropertylegendoriginxitem_, 0, 1);
  doubleManager_->setRange(itempropertylegendoriginyitem_, 0, 1);
  doubleManager_->setSingleStep(itempropertylegendoriginxitem_, 0.01);
  doubleManager_->setSingleStep(itempropertylegendoriginyitem_, 0.01);
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
  itempropertylinepixelpositionx1item_ =
      doubleManager_->addProperty("Pixel Position X1");
  itempropertylinepixelpositiony1item_ =
      doubleManager_->addProperty("Pixel Position Y1");
  itempropertylinepixelpositionx2item_ =
      doubleManager_->addProperty("Pixel Position X2");
  itempropertylinepixelpositiony2item_ =
      doubleManager_->addProperty("Pixel Position Y2");
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
  itempropertyimagepixelpositionxitem_ =
      doubleManager_->addProperty("Pixel Position X");
  itempropertyimagepixelpositionyitem_ =
      doubleManager_->addProperty("Pixel Position Y");
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

  // LineSpecial Properties block
  QStringList lstylelist;
  lstylelist << tr("StepLeft") << tr("StepRight") << tr("StepCenter")
             << tr("Impulse") << tr("Line");
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
  // LineSpecialChannel Properties block
  channelplotpropertyxaxisitem_ = enumManager_->addProperty("X Axis");
  channelplotpropertyyaxisitem_ = enumManager_->addProperty("Y Axis");
  channelplotpropertylegendtextitem_ =
      stringManager_->addProperty("Plot Legrad");
  // channel 1st graph
  channel1plotpropertygroupitem_ =
      groupManager_->addProperty("Channel Border 1");
  channel1plotpropertylinestyleitem_ = enumManager_->addProperty("Line Style");
  enumManager_->setEnumNames(channel1plotpropertylinestyleitem_, lstylelist);
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertylinestyleitem_);
  channel1plotpropertylinestrokecoloritem_ =
      colorManager_->addProperty("Line Stroke Color");
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertylinestrokecoloritem_);
  channel1plotpropertylinestrokethicknessitem_ =
      doubleManager_->addProperty("Line Stroke Thickness");
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertylinestrokethicknessitem_);
  channel1plotpropertylinestroketypeitem_ =
      enumManager_->addProperty("Line Stroke Type");
  enumManager_->setEnumNames(channel1plotpropertylinestroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(channel1plotpropertylinestroketypeitem_,
                             stroketypeiconslist);
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertylinestroketypeitem_);
  channel1plotpropertylinefillcoloritem_ =
      colorManager_->addProperty("Area Fill Color");
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertylinefillcoloritem_);
  channel1plotpropertylineantialiaseditem_ =
      boolManager_->addProperty("Line Antialiased");
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertylineantialiaseditem_);
  channel1plotpropertyscatterstyleitem_ =
      enumManager_->addProperty("Scatter Style");
  enumManager_->setEnumNames(channel1plotpropertyscatterstyleitem_, sstylelist);
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertyscatterstyleitem_);
  channel1plotpropertyscatterthicknessitem_ =
      doubleManager_->addProperty("Scatter Size");
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertyscatterthicknessitem_);
  channel1plotpropertyscatterfillcoloritem_ =
      colorManager_->addProperty("Scatter Fill Color");
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertyscatterfillcoloritem_);
  channel1plotpropertyscatterstrokecoloritem_ =
      colorManager_->addProperty("Scatter Outline Color");
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertyscatterstrokecoloritem_);
  channel1plotpropertyscatterstrokethicknessitem_ =
      doubleManager_->addProperty("Scatter Outline Thickness");
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertyscatterstrokethicknessitem_);
  channel1plotpropertyscatterstrokestyleitem_ =
      enumManager_->addProperty("Scatter Outline Type");
  enumManager_->setEnumNames(channel1plotpropertyscatterstrokestyleitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(channel1plotpropertyscatterstrokestyleitem_,
                             stroketypeiconslist);
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertyscatterstrokestyleitem_);
  channel1plotpropertyscatterantialiaseditem_ =
      boolManager_->addProperty("Scatter Antialiased");
  channel1plotpropertygroupitem_->addSubProperty(
      channel1plotpropertyscatterantialiaseditem_);
  // channe2 1st graph
  channel2plotpropertygroupitem_ =
      groupManager_->addProperty("Channel Border 2");
  channel2plotpropertylinestyleitem_ = enumManager_->addProperty("Line Style");
  enumManager_->setEnumNames(channel2plotpropertylinestyleitem_, lstylelist);
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertylinestyleitem_);
  channel2plotpropertylinestrokecoloritem_ =
      colorManager_->addProperty("Line Stroke Color");
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertylinestrokecoloritem_);
  channel2plotpropertylinestrokethicknessitem_ =
      doubleManager_->addProperty("Line Stroke Thickness");
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertylinestrokethicknessitem_);
  channel2plotpropertylinestroketypeitem_ =
      enumManager_->addProperty("Line Stroke Type");
  enumManager_->setEnumNames(channel2plotpropertylinestroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(channel2plotpropertylinestroketypeitem_,
                             stroketypeiconslist);
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertylinestroketypeitem_);
  channel2plotpropertylineantialiaseditem_ =
      boolManager_->addProperty("Line Antialiased");
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertylineantialiaseditem_);
  channel2plotpropertyscatterstyleitem_ =
      enumManager_->addProperty("Scatter Style");
  enumManager_->setEnumNames(channel2plotpropertyscatterstyleitem_, sstylelist);
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertyscatterstyleitem_);
  channel2plotpropertyscatterthicknessitem_ =
      doubleManager_->addProperty("Scatter Size");
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertyscatterthicknessitem_);
  channel2plotpropertyscatterfillcoloritem_ =
      colorManager_->addProperty("Scatter Fill Color");
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertyscatterfillcoloritem_);
  channel2plotpropertyscatterstrokecoloritem_ =
      colorManager_->addProperty("Scatter Outline Color");
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertyscatterstrokecoloritem_);
  channel2plotpropertyscatterstrokethicknessitem_ =
      doubleManager_->addProperty("Scatter Outline Thickness");
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertyscatterstrokethicknessitem_);
  channel2plotpropertyscatterstrokestyleitem_ =
      enumManager_->addProperty("Scatter Outline Type");
  enumManager_->setEnumNames(channel2plotpropertyscatterstrokestyleitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(channel2plotpropertyscatterstrokestyleitem_,
                             stroketypeiconslist);
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertyscatterstrokestyleitem_);
  channel2plotpropertyscatterantialiaseditem_ =
      boolManager_->addProperty("Scatter Antialiased");
  channel2plotpropertygroupitem_->addSubProperty(
      channel2plotpropertyscatterantialiaseditem_);

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
  barplotpropertystackgapitem_ = doubleManager_->addProperty("Stack Gap");
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
  doubleManager_->setSingleStep(barplotpropertywidthitem_, 0.01);

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
  statboxplotpropertyboxoutlinecoloritem_ =
      colorManager_->addProperty("Box Stroke Color");
  statboxplotpropertyboxoutlinethicknessitem_ =
      doubleManager_->addProperty("Box Stroke Thickness");
  statboxplotpropertyboxoutlinestyleitem_ =
      enumManager_->addProperty("Box Stroke Style");
  enumManager_->setEnumNames(statboxplotpropertyboxoutlinestyleitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(statboxplotpropertyboxoutlinestyleitem_,
                             stroketypeiconslist);
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
  QStringList piestyle;
  piestyle << "Pie"
           << "Half Pie";
  pieplotpropertystyleitem_ = enumManager_->addProperty("Style");
  enumManager_->setEnumNames(pieplotpropertystyleitem_, piestyle);
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

  // Colormap Properties Block
  QStringList datascaletypelist;
  datascaletypelist << "Linear"
                    << "Logarithmic";
  QStringList gradientlist;
  gradientlist << "Grayscale"
               << "Hot"
               << "Cold"
               << "Night"
               << "candy"
               << "Geography"
               << "Ion"
               << "Thermal"
               << "Polar"
               << "Spectrum"
               << "Jet"
               << "Hues";
  colormappropertyinterpolateitem_ = boolManager_->addProperty("Interpolate");
  colormappropertytightboundaryitem_ =
      boolManager_->addProperty("Tight Boundary");
  colormappropertylevelcountitem_ = intManager_->addProperty(tr("Level Count"));
  colormappropertygradientinvertitem_ =
      boolManager_->addProperty(tr("Gradient Invert"));
  colormappropertygradientperiodicitem_ =
      boolManager_->addProperty(tr("Gradient Periodic"));
  colormappropertygradientitem_ = enumManager_->addProperty("Gradient");
  enumManager_->setEnumNames(colormappropertygradientitem_, gradientlist);
  colormappropertyscalevisibleitem_ =
      boolManager_->addProperty("Colormap Scale");
  colormappropertyscalewidthitem_ = intManager_->addProperty("Scale Width");
  colormappropertyscalevisibleitem_->addSubProperty(
      colormappropertyscalewidthitem_);
  // Colormap Axis general
  colormappropertyscaleaxisvisibleitem_ =
      boolManager_->addProperty("Scale Visible");
  colormappropertyscaleaxisoffsetitem_ = intManager_->addProperty(tr("Offset"));
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxisoffsetitem_);
  colormappropertyscaleaxisfromitem_ = doubleManager_->addProperty(tr("From"));
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxisfromitem_);
  colormappropertyscaleaxistoitem_ = doubleManager_->addProperty(tr("To"));
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxistoitem_);
  colormappropertyscaleaxislinlogitem_ = enumManager_->addProperty(tr("Type"));
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxislinlogitem_);
  enumManager_->setEnumNames(colormappropertyscaleaxislinlogitem_,
                             axistypelist);
  colormappropertyscaleaxisinvertitem_ =
      boolManager_->addProperty(tr("Inverted"));
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxisinvertitem_);
  colormappropertyscaleaxisstrokecoloritem_ =
      colorManager_->addProperty(tr("Stroke Color"));
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxisstrokecoloritem_);
  colormappropertyscaleaxisstrokethicknessitem_ =
      doubleManager_->addProperty(tr("Stroke Thickness"));
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxisstrokethicknessitem_);
  colormappropertyscaleaxisstroketypeitem_ =
      enumManager_->addProperty(tr("Stroke Type"));
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxisstroketypeitem_);
  enumManager_->setEnumNames(colormappropertyscaleaxisstroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(colormappropertyscaleaxisstroketypeitem_,
                             stroketypeiconslist);
  colormappropertyscaleaxisantialiaseditem_ =
      boolManager_->addProperty("Antialiased");
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxisantialiaseditem_);
  // Colormap Axis Properties Label sub block
  colormappropertyscaleaxislabeltextitem_ =
      stringManager_->addProperty(tr("Label Text"));
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxislabeltextitem_);
  colormappropertyscaleaxislabelfontitem_ =
      fontManager_->addProperty("Label Font");
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxislabelfontitem_);
  colormappropertyscaleaxislabelcoloritem_ =
      colorManager_->addProperty("Label Color");
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxislabelcoloritem_);
  colormappropertyscaleaxislabelpaddingitem_ =
      intManager_->addProperty("Label Padding");
  colormappropertyscaleaxisvisibleitem_->addSubProperty(
      colormappropertyscaleaxislabelpaddingitem_);
  // Colormap Axis Properties Ticks sub block
  colormappropertyscaleaxistickvisibilityitem_ =
      boolManager_->addProperty("Scale Axis Ticks");
  colormappropertyscaleaxisticklengthinitem_ =
      intManager_->addProperty("Length In");
  colormappropertyscaleaxistickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxisticklengthinitem_);
  colormappropertyscaleaxisticklengthoutitem_ =
      intManager_->addProperty("Length Out");
  colormappropertyscaleaxistickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxisticklengthoutitem_);
  colormappropertyscaleaxistickstrokecoloritem_ =
      colorManager_->addProperty("Stroke Color");
  colormappropertyscaleaxistickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxistickstrokecoloritem_);
  colormappropertyscaleaxistickstrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  colormappropertyscaleaxistickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxistickstrokethicknessitem_);
  colormappropertyscaleaxistickstroketypeitem_ =
      enumManager_->addProperty("Stroke Type");
  colormappropertyscaleaxistickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxistickstroketypeitem_);
  enumManager_->setEnumNames(colormappropertyscaleaxistickstroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(colormappropertyscaleaxistickstroketypeitem_,
                             stroketypeiconslist);
  // Colormap Axis Properties Sub-ticks sub block
  colormappropertyscaleaxissubtickvisibilityitem_ =
      boolManager_->addProperty("Scale Axis Sub-Ticks");
  colormappropertyscaleaxissubticklengthinitem_ =
      intManager_->addProperty("Length In");
  colormappropertyscaleaxissubtickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxissubticklengthinitem_);
  colormappropertyscaleaxissubticklengthoutitem_ =
      intManager_->addProperty("Length Out");
  colormappropertyscaleaxissubtickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxissubticklengthoutitem_);
  colormappropertyscaleaxissubtickstrokecoloritem_ =
      colorManager_->addProperty("Stroke Color");
  colormappropertyscaleaxissubtickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxissubtickstrokecoloritem_);
  colormappropertyscaleaxissubtickstrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  colormappropertyscaleaxissubtickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxissubtickstrokethicknessitem_);
  colormappropertyscaleaxissubtickstroketypeitem_ =
      enumManager_->addProperty("Stroke Type");
  colormappropertyscaleaxissubtickvisibilityitem_->addSubProperty(
      colormappropertyscaleaxissubtickstroketypeitem_);
  enumManager_->setEnumNames(colormappropertyscaleaxissubtickstroketypeitem_,
                             stroketypelist);
  enumManager_->setEnumIcons(colormappropertyscaleaxissubtickstroketypeitem_,
                             stroketypeiconslist);
  // Colormap Axis Properties Ticks Label sub block
  colormappropertyscaleaxisticklabelvisibilityitem_ =
      boolManager_->addProperty("Scale Tick Label");
  colormappropertyscaleaxisticklabelfontitem_ =
      fontManager_->addProperty("Font");
  colormappropertyscaleaxisticklabelvisibilityitem_->addSubProperty(
      colormappropertyscaleaxisticklabelfontitem_);
  colormappropertyscaleaxisticklabelcoloritem_ =
      colorManager_->addProperty("Color");
  colormappropertyscaleaxisticklabelvisibilityitem_->addSubProperty(
      colormappropertyscaleaxisticklabelcoloritem_);
  colormappropertyscaleaxisticklabelpaddingitem_ =
      intManager_->addProperty("Padding");
  colormappropertyscaleaxisticklabelvisibilityitem_->addSubProperty(
      colormappropertyscaleaxisticklabelpaddingitem_);
  colormappropertyscaleaxisticklabelrotationitem_ =
      doubleManager_->addProperty("Rotation");
  colormappropertyscaleaxisticklabelvisibilityitem_->addSubProperty(
      colormappropertyscaleaxisticklabelrotationitem_);
  colormappropertyscaleaxisticklabelsideitem_ =
      enumManager_->addProperty("Side");
  colormappropertyscaleaxisticklabelvisibilityitem_->addSubProperty(
      colormappropertyscaleaxisticklabelsideitem_);
  enumManager_->setEnumNames(colormappropertyscaleaxisticklabelsideitem_,
                             axislabelsidelist);
  colormappropertyscaleaxisticklabelformatitem_ =
      enumManager_->addProperty("Format");
  colormappropertyscaleaxisticklabelvisibilityitem_->addSubProperty(
      colormappropertyscaleaxisticklabelformatitem_);
  enumManager_->setEnumNames(colormappropertyscaleaxisticklabelformatitem_,
                             formatlist);
  colormappropertyscaleaxisticklabelprecisionitem_ =
      intManager_->addProperty("Precision");
  colormappropertyscaleaxisticklabelvisibilityitem_->addSubProperty(
      colormappropertyscaleaxisticklabelprecisionitem_);

  errorbarpropertywhiskerwidthitem_ =
      doubleManager_->addProperty("Whisker Width");
  errorbarpropertysymbolgaptem_ = doubleManager_->addProperty("Symbol Gap");
  errorbarpropertystrokecoloritem_ = colorManager_->addProperty("Stroke Color");
  errorbarpropertystrokethicknessitem_ =
      doubleManager_->addProperty("Stroke Thickness");
  errorbarpropertystroketypeitem_ = enumManager_->addProperty("Stroke Type");
  enumManager_->setEnumNames(errorbarpropertystroketypeitem_, stroketypelist);
  enumManager_->setEnumIcons(errorbarpropertystroketypeitem_,
                             stroketypeiconslist);
  errorbarpropertyfillstatusitem_ = boolManager_->addProperty("Fill");
  errorbarpropertyfillcoloritem_ = colorManager_->addProperty("Fill Color");
  errorbarpropertyantialiaseditem_ = boolManager_->addProperty("Antialiased");

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
  connect(objectbrowser_, &MyTreeWidget::adderrorbar, this,
          &PropertyEditor::adderrorbar);
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
  connect(this, &PropertyEditor::refreshCanvasRect, [=]() {
    QTreeWidgetItem *item = objectbrowser_->currentItem();
    if (item && static_cast<MyTreeWidget::PropertyItemType>(
                    item->data(0, Qt::UserRole).value<int>()) ==
                    MyTreeWidget::PropertyItemType::PlotCanvas) {
      Plot2D *plotcanvas =
          getgraph2dobject<Plot2D>(objectbrowser_->currentItem());
      rectManager_->setValue(canvaspropertyrectitem_, plotcanvas->geometry());
    }
    if (item && static_cast<MyTreeWidget::PropertyItemType>(
                    item->data(0, Qt::UserRole).value<int>()) ==
                    MyTreeWidget::PropertyItemType::Layout) {
      AxisRect2D *axisrect =
          getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
      rectManager_->setValue(layoutpropertyrectitem_, axisrect->outerRect());
    }
  });
}

PropertyEditor::~PropertyEditor() { delete ui_; }

MyTreeWidget *PropertyEditor::getObjectBrowser() { return objectbrowser_; }

void PropertyEditor::valueChange(QtProperty *prop, const bool value) {
  if (prop->compare(canvaspropertyopenglitem_)) {
    Plot2D *plot = getgraph2dobject<Plot2D>(objectbrowser_->currentItem());
    plot->setOpenGl(value);
    plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
  } else if (prop->compare(axispropertyvisibleitem_)) {
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
  } else if (prop->compare(channel1plotpropertylineantialiaseditem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlineantialiased_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(channel1plotpropertyscatterantialiaseditem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterantialiased_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(channel2plotpropertylineantialiaseditem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlineantialiased_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(channel2plotpropertyscatterantialiaseditem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterantialiased_lsplot(value);
    lsgraph->layer()->replot();
  } else if (prop->compare(cplotpropertylinefillstatusitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinefillstatus_cplot(value);
    curve->layer()->replot();
    curve->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
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
  } else if (prop->compare(colormappropertyinterpolateitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->setInterpolate(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertytightboundaryitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->setTightBoundary(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertygradientinvertitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->setgradientinverted_colormap(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertygradientperiodicitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->setgradientperiodic_colormap(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscalevisibleitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->setVisible(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisvisibleitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setVisible(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisinvertitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setRangeReversed(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisantialiaseditem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setAntialiased(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxistickvisibilityitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setTicks(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxissubtickvisibilityitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setSubTicks(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklabelvisibilityitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setTickLabels(value);
    colormap->layer()->replot();
  } else if (prop->compare(errorbarpropertyfillstatusitem_)) {
    ErrorBar2D *errorbar =
        getgraph2dobject<ErrorBar2D>(objectbrowser_->currentItem());
    errorbar->setfillstatus_errorbar(value);
    errorbar->layer()->replot();
  } else if (prop->compare(errorbarpropertyantialiaseditem_)) {
    ErrorBar2D *errorbar =
        getgraph2dobject<ErrorBar2D>(objectbrowser_->currentItem());
    errorbar->setAntialiasedFill(value);
    errorbar->setAntialiasedScatters(value);
    errorbar->setAntialiased(value);
    errorbar->layer()->replot();
  } else {
    qDebug() << "unknown bool property item";
  }
}

void PropertyEditor::valueChange(QtProperty *prop, const QColor &color) {
  disconnect(colorManager_, SIGNAL(valueChanged(QtProperty *, QColor)), this,
             SLOT(valueChange(QtProperty *, const QColor &)));
  if (prop->compare(canvaspropertycoloritem_)) {
    Plot2D *plotcanvas =
        getgraph2dobject<Plot2D>(objectbrowser_->currentItem());
    plotcanvas->setBackgroundColor(color);
    plotcanvas->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
  } else if (prop->compare(layoutpropertycoloritem_)) {
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
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(lsplotpropertylinefillcoloritem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlinefillcolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterfillcoloritem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscatterfillcolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterstrokecoloritem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscatterstrokecolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertylinestrokecoloritem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlinestrokecolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertylinefillcoloritem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlinefillcolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertyscatterfillcoloritem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterfillcolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertyscatterstrokecoloritem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterstrokecolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertylinestrokecoloritem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlinestrokecolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertyscatterfillcoloritem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterfillcolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertyscatterstrokecoloritem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterstrokecolor_lsplot(color);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(cplotpropertylinestrokecoloritem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinestrokecolor_cplot(color);
    curve->layer()->replot();
    curve->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(cplotpropertylinefillcoloritem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinefillcolor_cplot(color);
    curve->layer()->replot();
    curve->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterfillcoloritem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscatterfillcolor_cplot(color);
    curve->layer()->replot();
    curve->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterstrokecoloritem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscatterstrokecolor_cplot(color);
    curve->layer()->replot();
    curve->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(barplotpropertyfillcoloritem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setfillcolor_barplot(color);
    bar->layer()->replot();
    bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(barplotpropertystrokecoloritem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setstrokecolor_barplot(color);
    bar->layer()->replot();
    bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyfillcoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setfillcolor_statbox(color);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyboxoutlinecoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    QPen p = statbox->pen();
    p.setColor(color);
    statbox->setPen(p);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerstrokecoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerstrokecolor_statbox(color);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerbarstrokecoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerbarstrokecolor_statbox(color);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertymedianstrokecoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setmedianstrokecolor_statbox(color);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatterfillcoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscatterfillcolor_statbox(color);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatteroutlinecoloritem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscatterstrokecolor_statbox(color);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(vectorpropertylinestrokecoloritem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setlinestrokecolor_vecplot(color);
    vector->layer()->replot();
    vector->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(pieplotpropertylinestrokecoloritem_)) {
    Pie2D *pie = getgraph2dobject<Pie2D>(objectbrowser_->currentItem());
    pie->setstrokecolor_pieplot(color);
    pie->layer()->replot();
    pie->getaxisrect()->getLegend()->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisstrokecoloritem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    QPen p = colormap->getcolormapscale_colormap()->axis()->basePen();
    p.setColor(color);
    colormap->getcolormapscale_colormap()->axis()->setBasePen(p);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxislabelcoloritem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setLabelColor(color);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxistickstrokecoloritem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    QPen p = colormap->getcolormapscale_colormap()->axis()->tickPen();
    p.setColor(color);
    colormap->getcolormapscale_colormap()->axis()->setTickPen(p);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxissubtickstrokecoloritem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    QPen p = colormap->getcolormapscale_colormap()->axis()->subTickPen();
    p.setColor(color);
    colormap->getcolormapscale_colormap()->axis()->setSubTickPen(p);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklabelcoloritem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setTickLabelColor(color);
    colormap->layer()->replot();
  } else if (prop->compare(errorbarpropertystrokecoloritem_)) {
    ErrorBar2D *errorbar =
        getgraph2dobject<ErrorBar2D>(objectbrowser_->currentItem());
    QPen p = errorbar->pen();
    p.setColor(color);
    errorbar->setPen(p);
    errorbar->layer()->replot();
  } else if (prop->compare(errorbarpropertyfillcoloritem_)) {
    ErrorBar2D *errorbar =
        getgraph2dobject<ErrorBar2D>(objectbrowser_->currentItem());
    QBrush b = errorbar->brush();
    b.setColor(color);
    errorbar->setBrush(b);
    errorbar->layer()->replot();
  }
  connect(colorManager_, SIGNAL(valueChanged(QtProperty *, QColor)), this,
          SLOT(valueChange(QtProperty *, const QColor &)));
}

void PropertyEditor::valueChange(QtProperty *prop, const QRect &rect) {
  if (prop->compare(canvaspropertyrectitem_)) {
    Plot2D *plot = getgraph2dobject<Plot2D>(objectbrowser_->currentItem());
    QRect oldrect = plot->geometry();
    plot->setGeometry(rect);
    MyWidget *widget = qobject_cast<MyWidget *>(plot->parentWidget()->parent());
    if (widget) {
      QRect wrect = widget->geometry();
      widget->resize(wrect.width() + (rect.width() - oldrect.width()),
                     wrect.height() + (rect.height() - oldrect.height()));
    }
    plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
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
  } else
    qDebug() << "unknown QRect Item";
}

void PropertyEditor::valueChange(QtProperty *prop, const double &value) {
  if (prop->compare(canvaspropertybufferdevicepixelratioitem_)) {
    Plot2D *plot = getgraph2dobject<Plot2D>(objectbrowser_->currentItem());
    plot->setBufferDevicePixelRatio(value);
    plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
  } else if (prop->compare(hmajgridpropertystrokethicknessitem_)) {
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
  } else if (prop->compare(itempropertylegendoriginxitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    QPointF origin = legend->getposition_legend();
    origin.setX(value);
    if (origin != legend->getposition_legend()) {
      legend->setposition_legend(origin);
      legend->layer()->replot();
    }
  } else if (prop->compare(itempropertylegendoriginyitem_)) {
    Legend2D *legend =
        getgraph2dobject<Legend2D>(objectbrowser_->currentItem());
    QPointF origin = legend->getposition_legend();
    origin.setY(value);
    if (origin != legend->getposition_legend()) {
      legend->setposition_legend(origin);
      legend->layer()->replot();
    }
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
    textitem->setpixelposition_textitem(point);
    textitem->layer()->replot();
  } else if (prop->compare(itempropertytextpixelpositionyitem_)) {
    TextItem2D *textitem =
        getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
    QPointF point = textitem->position->pixelPosition();
    point.setY(value);
    textitem->setpixelposition_textitem(point);
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
  } else if (prop->compare(itempropertylinepixelpositionx1item_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    QPointF point = lineitem->position("start")->pixelPosition();
    point.setX(value);
    lineitem->position("start")->setPixelPosition(point);
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertylinepixelpositiony1item_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    QPointF point = lineitem->position("start")->pixelPosition();
    point.setY(value);
    lineitem->position("start")->setPixelPosition(point);
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertylinepixelpositionx2item_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    QPointF point = lineitem->position("end")->pixelPosition();
    point.setX(value);
    lineitem->position("end")->setPixelPosition(point);
    lineitem->layer()->replot();
  } else if (prop->compare(itempropertylinepixelpositiony2item_)) {
    LineItem2D *lineitem =
        getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
    QPointF point = lineitem->position("end")->pixelPosition();
    point.setY(value);
    lineitem->position("end")->setPixelPosition(point);
    lineitem->layer()->replot();
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
  } else if (prop->compare(itempropertyimagepixelpositionxitem_)) {
    ImageItem2D *imageitem =
        getgraph2dobject<ImageItem2D>(objectbrowser_->currentItem());
    QPointF point = imageitem->position("topLeft")->pixelPosition();
    point.setX(value);
    imageitem->position("topLeft")->setPixelPosition(point);
    imageitem->layer()->replot();
  } else if (prop->compare(itempropertyimagepixelpositionyitem_)) {
    ImageItem2D *imageitem =
        getgraph2dobject<ImageItem2D>(objectbrowser_->currentItem());
    QPointF point = imageitem->position("topLeft")->pixelPosition();
    point.setY(value);
    imageitem->position("topLeft")->setPixelPosition(point);
    imageitem->layer()->replot();
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
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterthicknessitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscattersize_lsplot(value);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterstrokethicknessitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscatterstrokethickness_lsplot(value);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertylinestrokethicknessitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlinestrokethickness_lsplot(value);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertyscatterthicknessitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscattersize_lsplot(value);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertyscatterstrokethicknessitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterstrokethickness_lsplot(value);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertylinestrokethicknessitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlinestrokethickness_lsplot(value);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertyscatterthicknessitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscattersize_lsplot(value);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertyscatterstrokethicknessitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterstrokethickness_lsplot(value);
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(cplotpropertylinestrokethicknessitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setlinestrokethickness_cplot(value);
    curve->layer()->replot();
    curve->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterthicknessitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscattersize_cplot(value);
    curve->layer()->replot();
    curve->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(cplotpropertyscatterstrokethicknessitem_)) {
    Curve2D *curve = getgraph2dobject<Curve2D>(objectbrowser_->currentItem());
    curve->setscatterstrokethickness_cplot(value);
    curve->layer()->replot();
    curve->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(barplotpropertywidthitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setWidth(value);
    bar->layer()->replot();
    bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(barplotpropertystackgapitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setStackingGap(value);
    bar->layer()->replot();
    bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(barplotpropertystrokethicknessitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setstrokethickness_barplot(value);
    bar->layer()->replot();
    bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertywidthitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setWidth(value);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerwidthitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setWhiskerWidth(value);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyboxoutlinethicknessitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    QPen p = statbox->pen();
    p.setWidthF(value);
    statbox->setPen(p);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerstrokethicknessitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerstrokethickness_statbox(value);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerbarstrokethicknessitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerbarstrokethickness_statbox(value);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertymideanstrokethicknessitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setmedianstrokethickness_statbox(value);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscattersizeitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscattersize_statbox(value);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatteroutlinethicknessitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscatterstrokethickness_statbox(value);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(vectorpropertylinestrokethicknessitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setlinestrokethickness_vecplot(value);
    vector->layer()->replot();
    vector->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(vectorpropertylineendingheightitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setendheight_vecplot(value, Vector2D::LineEndLocation::Stop);
    vector->layer()->replot();
    vector->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(vectorpropertylineendingwidthitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setendwidth_vecplot(value, Vector2D::LineEndLocation::Stop);
    vector->layer()->replot();
    vector->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(pieplotpropertylinestrokethicknessitem_)) {
    Pie2D *pie = getgraph2dobject<Pie2D>(objectbrowser_->currentItem());
    pie->setstrokethickness_pieplot(value);
    pie->layer()->replot();
    pie->getaxisrect()->getLegend()->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisfromitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setRangeLower(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxistoitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setRangeUpper(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisstrokethicknessitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    QPen p = colormap->getcolormapscale_colormap()->axis()->basePen();
    p.setWidthF(value);
    colormap->getcolormapscale_colormap()->axis()->setBasePen(p);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxistickstrokethicknessitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    QPen p = colormap->getcolormapscale_colormap()->axis()->tickPen();
    p.setWidthF(value);
    colormap->getcolormapscale_colormap()->axis()->setTickPen(p);
    colormap->layer()->replot();
  } else if (prop->compare(
                 colormappropertyscaleaxissubtickstrokethicknessitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    QPen p = colormap->getcolormapscale_colormap()->axis()->subTickPen();
    p.setWidthF(value);
    colormap->getcolormapscale_colormap()->axis()->setSubTickPen(p);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklabelrotationitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setTickLabelRotation(value);
    colormap->layer()->replot();
  } else if (prop->compare(errorbarpropertywhiskerwidthitem_)) {
    ErrorBar2D *errorbar =
        getgraph2dobject<ErrorBar2D>(objectbrowser_->currentItem());
    errorbar->setWhiskerWidth(value);
    errorbar->layer()->replot();
  } else if (prop->compare(errorbarpropertysymbolgaptem_)) {
    ErrorBar2D *errorbar =
        getgraph2dobject<ErrorBar2D>(objectbrowser_->currentItem());
    errorbar->setSymbolGap(value);
    errorbar->layer()->replot();
  } else if (prop->compare(errorbarpropertystrokethicknessitem_)) {
    ErrorBar2D *errorbar =
        getgraph2dobject<ErrorBar2D>(objectbrowser_->currentItem());
    QPen p = errorbar->pen();
    p.setWidthF(value);
    errorbar->setPen(p);
    errorbar->layer()->replot();
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
  } else if (prop->compare(channelplotpropertylegendtextitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
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
    statbox->setlegendtext_statbox(Utilities::splitstring(value));
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
  } else if (prop->compare(colormappropertyscaleaxislabeltextitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->setLabel(value);
    colormap->setname_colormap(Utilities::splitstring(value));
    colormap->layer()->replot();
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
  } else if (prop->compare(axispropertytickcountitem_)) {
    Axis2D *axis = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
    axis->settickscount_axis(value);
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
  } else if (prop->compare(colormappropertyscalewidthitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->setcolormapscalewidth_colormap(value);
    colormap->parentPlot()->replot();
  } else if (prop->compare(colormappropertyscaleaxisoffsetitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setOffset(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxislabelpaddingitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setLabelPadding(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklengthinitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setTickLengthIn(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklengthoutitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setTickLengthOut(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxissubticklengthinitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setSubTickLengthIn(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxissubticklengthoutitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setSubTickLengthOut(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklabelpaddingitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setTickLabelPadding(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklabelpaddingitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setNumberPrecision(value);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertylevelcountitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->setlevelcount_colormap(value);
    colormap->layer()->replot();
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
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(lsplotpropertylinestroketypeitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setlinestrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterstyleitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscattershape_lsplot(
        static_cast<Graph2DCommon::ScatterStyle>(value));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(lsplotpropertyscatterstrokestyleitem_)) {
    LineSpecial2D *lsgraph =
        getgraph2dobject<LineSpecial2D>(objectbrowser_->currentItem());
    lsgraph->setscatterstrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channelplotpropertyxaxisitem_)) {
    void *ptr1 = objectbrowser_->currentItem()
                     ->data(0, Qt::UserRole + 1)
                     .value<void *>();
    LineSpecial2D *lsgraph1 = static_cast<LineSpecial2D *>(ptr1);
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getXAxis(value);
    if (!axis) return;
    lsgraph1->setxaxis_lsplot(axis);
    void *ptr2 = objectbrowser_->currentItem()
                     ->data(0, Qt::UserRole + 3)
                     .value<void *>();
    LineSpecial2D *lsgraph2 = static_cast<LineSpecial2D *>(ptr2);
    if (!axis) return;
    lsgraph2->setxaxis_lsplot(axis);
    lsgraph1->layer()->replot();
    lsgraph2->layer()->replot();
  } else if (prop->compare(channelplotpropertyyaxisitem_)) {
    void *ptr1 = objectbrowser_->currentItem()
                     ->data(0, Qt::UserRole + 1)
                     .value<void *>();
    LineSpecial2D *lsgraph1 = static_cast<LineSpecial2D *>(ptr1);
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = axisrect->getYAxis(value);
    if (!axis) return;
    lsgraph1->setyaxis_lsplot(axis);
    void *ptr2 = objectbrowser_->currentItem()
                     ->data(0, Qt::UserRole + 3)
                     .value<void *>();
    LineSpecial2D *lsgraph2 = static_cast<LineSpecial2D *>(ptr2);
    if (!axis) return;
    lsgraph2->setyaxis_lsplot(axis);
    lsgraph1->layer()->replot();
    lsgraph2->layer()->replot();
  } else if (prop->compare(channel1plotpropertylinestyleitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlinetype_lsplot(
        static_cast<Graph2DCommon::LineStyleType>(value));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertylinestroketypeitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlinestrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertyscatterstyleitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscattershape_lsplot(
        static_cast<Graph2DCommon::ScatterStyle>(value));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel1plotpropertyscatterstrokestyleitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 1)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterstrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertylinestyleitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlinetype_lsplot(
        static_cast<Graph2DCommon::LineStyleType>(value));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertylinestroketypeitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setlinestrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertyscatterstyleitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscattershape_lsplot(
        static_cast<Graph2DCommon::ScatterStyle>(value));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(channel2plotpropertyscatterstrokestyleitem_)) {
    void *ptr = objectbrowser_->currentItem()
                    ->data(0, Qt::UserRole + 3)
                    .value<void *>();
    LineSpecial2D *lsgraph = static_cast<LineSpecial2D *>(ptr);
    lsgraph->setscatterstrokestyle_lsplot(static_cast<Qt::PenStyle>(value + 1));
    lsgraph->layer()->replot();
    lsgraph->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
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
    Axis2D *axis = nullptr;
    (bar->getxaxis()->getorientation_axis() == Axis2D::AxisOreantation::Top ||
     bar->getxaxis()->getorientation_axis() == Axis2D::AxisOreantation::Bottom)
        ? axis = axisrect->getXAxis(value)
        : axis = axisrect->getYAxis(value);
    if (!axis) return;
    bar->setxaxis_barplot(axis, true);
    bar->layer()->replot();
  } else if (prop->compare(barplotpropertyyaxisitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    AxisRect2D *axisrect =
        getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem()->parent());
    Axis2D *axis = nullptr;
    (bar->getxaxis()->getorientation_axis() == Axis2D::AxisOreantation::Top ||
     bar->getxaxis()->getorientation_axis() == Axis2D::AxisOreantation::Bottom)
        ? axis = axisrect->getYAxis(value)
        : axis = axisrect->getXAxis(value);
    if (!axis) return;
    bar->setyaxis_barplot(axis, true);
    bar->layer()->replot();
  } else if (prop->compare(barplotpropertystrokestyleitem_)) {
    Bar2D *bar = getgraph2dobject<Bar2D>(objectbrowser_->currentItem());
    bar->setstrokestyle_barplot(static_cast<Qt::PenStyle>(value + 1));
    bar->layer()->replot();
    bar->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
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
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerstyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerstyle_statbox(
        static_cast<StatBox2D::BoxWhiskerStyle>(value));
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyboxoutlinestyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    QPen p = statbox->pen();
    p.setStyle(static_cast<Qt::PenStyle>(value + 1));
    statbox->setPen(p);
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerstrokestyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerstrokestyle_statbox(
        static_cast<Qt::PenStyle>(value + 1));
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertywhiskerbarstrokestyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setwhiskerbarstrokestyle_statbox(
        static_cast<Qt::PenStyle>(value + 1));
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertymideanstrokestyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setmedianstrokestyle_statbox(static_cast<Qt::PenStyle>(value + 1));
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatterstyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscattershape_statbox(
        static_cast<Graph2DCommon::ScatterStyle>(value));
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(statboxplotpropertyscatteroutlinestyleitem_)) {
    StatBox2D *statbox =
        getgraph2dobject<StatBox2D>(objectbrowser_->currentItem());
    statbox->setscatterstrokestyle_statbox(
        static_cast<Qt::PenStyle>(value + 1));
    statbox->layer()->replot();
    statbox->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
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
    vector->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(vectorpropertylineendingtypeitem_)) {
    Vector2D *vector =
        getgraph2dobject<Vector2D>(objectbrowser_->currentItem());
    vector->setendstyle_vecplot(static_cast<Vector2D::LineEnd>(value),
                                Vector2D::LineEndLocation::Stop);
    vector->layer()->replot();
    vector->getxaxis()->getaxisrect_axis()->getLegend()->layer()->replot();
  } else if (prop->compare(pieplotpropertystyleitem_)) {
    Pie2D *pie = getgraph2dobject<Pie2D>(objectbrowser_->currentItem());
    pie->setstyle_pieplot(static_cast<Pie2D::Style>(value));
    pie->layer()->replot();
    pie->getaxisrect()->replotBareBones();
    pie->getaxisrect()->getLegend()->layer()->replot();
  } else if (prop->compare(pieplotpropertylinestroketypeitem_)) {
    Pie2D *pie = getgraph2dobject<Pie2D>(objectbrowser_->currentItem());
    pie->setstrokestyle_pieplot(static_cast<Qt::PenStyle>(value + 1));
    pie->layer()->replot();
    pie->getaxisrect()->getLegend()->layer()->replot();
  } else if (prop->compare(colormappropertygradientitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->setgradient_colormap(static_cast<ColorMap2D::Gradient>(value));
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxislinlogitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setScaleType(
        static_cast<QCPAxis::ScaleType>(value));
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisstroketypeitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    QPen p = colormap->getcolormapscale_colormap()->axis()->basePen();
    p.setStyle(static_cast<Qt::PenStyle>(value + 1));
    colormap->getcolormapscale_colormap()->axis()->setBasePen(p);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxistickstroketypeitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    QPen p = colormap->getcolormapscale_colormap()->axis()->tickPen();
    p.setStyle(static_cast<Qt::PenStyle>(value + 1));
    colormap->getcolormapscale_colormap()->axis()->setTickPen(p);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxissubtickstroketypeitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    QPen p = colormap->getcolormapscale_colormap()->axis()->subTickPen();
    p.setStyle(static_cast<Qt::PenStyle>(value + 1));
    colormap->getcolormapscale_colormap()->axis()->setSubTickPen(p);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklabelsideitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setTickLabelSide(
        static_cast<QCPAxis::LabelSide>(value));
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklabelformatitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->setcolormapscaleticklabelformat_axis(
        static_cast<Axis2D::AxisLabelFormat>(value));
    colormap->parentPlot()->replot();
  } else if (prop->compare(errorbarpropertystroketypeitem_)) {
    ErrorBar2D *errorbar =
        getgraph2dobject<ErrorBar2D>(objectbrowser_->currentItem());
    QPen p = errorbar->pen();
    p.setStyle(static_cast<Qt::PenStyle>(value + 1));
    errorbar->setPen(p);
    errorbar->layer()->replot();
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
  } else if (prop->compare(colormappropertyscaleaxislabelfontitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setLabelFont(font);
    colormap->layer()->replot();
  } else if (prop->compare(colormappropertyscaleaxisticklabelfontitem_)) {
    ColorMap2D *colormap =
        getgraph2dobject<ColorMap2D>(objectbrowser_->currentItem());
    colormap->getcolormapscale_colormap()->axis()->setTickLabelFont(font);
    colormap->layer()->replot();
  }
}

void PropertyEditor::selectObjectItem(QTreeWidgetItem *item) {
  switch (static_cast<MyTreeWidget::PropertyItemType>(
      item->data(0, Qt::UserRole).value<int>())) {
    case MyTreeWidget::PropertyItemType::PlotCanvas: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      Plot2D *plotcanvas = static_cast<Plot2D *>(ptr);
      Plot2DPropertyBlock(plotcanvas);
    } break;
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
      LineSpecial2DPropertyBlock(lsgraph, axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::ChannelGraph: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      LineSpecial2D *lsgraph1 = static_cast<LineSpecial2D *>(ptr1);
      void *ptr2 = item->data(0, Qt::UserRole + 2).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      void *ptr3 = item->data(0, Qt::UserRole + 3).value<void *>();
      LineSpecial2D *lsgraph2 = static_cast<LineSpecial2D *>(ptr3);
      LineSpecialChannel2DPropertyBlock(lsgraph1, lsgraph2, axisrect);
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
    case MyTreeWidget::PropertyItemType::ColorMap: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      ColorMap2D *colormap = static_cast<ColorMap2D *>(ptr1);
      void *ptr2 = item->parent()->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      ColorMap2DPropertyBlock(colormap, axisrect);
    } break;
    case MyTreeWidget::PropertyItemType::ErrorBar: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      ErrorBar2D *errorbar = static_cast<ErrorBar2D *>(ptr1);
      void *ptr2 = item->parent()->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr2);
      ErrorBar2DPropertyBlock(errorbar, axisrect);
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
  intManager_->setValue(axispropertytickcountitem_, axis->gettickscount_axis());
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
  propertybrowser_->addProperty(itempropertylegendoriginxitem_);
  propertybrowser_->addProperty(itempropertylegendoriginyitem_);
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

  doubleManager_->setValue(itempropertylegendoriginxitem_,
                           legend->getposition_legend().x());
  doubleManager_->setValue(itempropertylegendoriginyitem_,
                           legend->getposition_legend().y());
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

  propertybrowser_->addProperty(itempropertylinepixelpositionx1item_);
  propertybrowser_->addProperty(itempropertylinepixelpositiony1item_);
  propertybrowser_->addProperty(itempropertylinepixelpositionx2item_);
  propertybrowser_->addProperty(itempropertylinepixelpositiony2item_);
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

  doubleManager_->setValue(itempropertylinepixelpositionx1item_,
                           lineitem->position("start")->pixelPosition().x());
  doubleManager_->setValue(itempropertylinepixelpositiony1item_,
                           lineitem->position("start")->pixelPosition().y());
  doubleManager_->setValue(itempropertylinepixelpositionx2item_,
                           lineitem->position("end")->pixelPosition().x());
  doubleManager_->setValue(itempropertylinepixelpositiony2item_,
                           lineitem->position("end")->pixelPosition().y());
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
  propertybrowser_->addProperty(itempropertyimagepixelpositionxitem_);
  propertybrowser_->addProperty(itempropertyimagepixelpositionyitem_);
  propertybrowser_->addProperty(itempropertyimagestrokecoloritem_);
  propertybrowser_->addProperty(itempropertyimagestrokethicknessitem_);
  propertybrowser_->addProperty(itempropertyimagestroketypeitem_);

  doubleManager_->setValue(itempropertyimagepixelpositionxitem_,
                           imageitem->position("topLeft")->pixelPosition().x());
  doubleManager_->setValue(itempropertyimagepixelpositionyitem_,
                           imageitem->position("topLeft")->pixelPosition().y());
  stringManager_->setValue(itempropertyimagesourceitem_,
                           imageitem->getsource_imageitem());
  colorManager_->setValue(itempropertyimagestrokecoloritem_,
                          imageitem->getstrokecolor_imageitem());
  doubleManager_->setValue(itempropertyimagestrokethicknessitem_,
                           imageitem->getstrokethickness_imageitem());
  enumManager_->setValue(itempropertyimagestroketypeitem_,
                         imageitem->getstrokestyle_imageitem() - 1);
}

void PropertyEditor::LineSpecial2DPropertyBlock(LineSpecial2D *lsgraph,
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
      if (yaxes.at(i) == lsgraph->getyaxis()) {
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
      if (xaxes.at(i) == lsgraph->getxaxis()) {
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

void PropertyEditor::LineSpecialChannel2DPropertyBlock(LineSpecial2D *lsgraph1,
                                                       LineSpecial2D *lsgraph2,
                                                       AxisRect2D *axisrect) {
  propertybrowser_->clear();
  propertybrowser_->addProperty(channelplotpropertyxaxisitem_);
  propertybrowser_->addProperty(channelplotpropertyyaxisitem_);
  propertybrowser_->addProperty(channelplotpropertylegendtextitem_);
  propertybrowser_->addProperty(channel1plotpropertygroupitem_);
  propertybrowser_->addProperty(channel2plotpropertygroupitem_);

  {
    QStringList lsyaxislist;
    int currentyaxis = 0;
    int ycount = 0;
    QList<Axis2D *> yaxes = axisrect->getYAxes2D();

    for (int i = 0; i < yaxes.size(); i++) {
      lsyaxislist << QString("Y Axis %1").arg(i + 1);
      if (yaxes.at(i) == lsgraph1->getyaxis()) {
        currentyaxis = ycount;
      }
      ycount++;
    }
    enumManager_->setEnumNames(channelplotpropertyyaxisitem_, lsyaxislist);
    enumManager_->setValue(channelplotpropertyyaxisitem_, currentyaxis);
  }

  {
    QStringList lsxaxislist;
    int currentxaxis = 0;
    int xcount = 0;
    QList<Axis2D *> xaxes = axisrect->getXAxes2D();
    for (int i = 0; i < xaxes.size(); i++) {
      lsxaxislist << QString("X Axis %1").arg(i + 1);
      if (xaxes.at(i) == lsgraph1->getxaxis()) {
        currentxaxis = xcount;
      }
      xcount++;
    }

    enumManager_->setEnumNames(channelplotpropertyxaxisitem_, lsxaxislist);
    enumManager_->setValue(channelplotpropertyxaxisitem_, currentxaxis);
  }

  stringManager_->setValue(
      channelplotpropertylegendtextitem_,
      Utilities::joinstring(lsgraph1->getlegendtext_lsplot()));
  enumManager_->setValue(channel1plotpropertylinestyleitem_,
                         static_cast<int>(lsgraph1->getlinetype_lsplot()));
  colorManager_->setValue(channel1plotpropertylinestrokecoloritem_,
                          lsgraph1->getlinestrokecolor_lsplot());
  doubleManager_->setValue(channel1plotpropertylinestrokethicknessitem_,
                           lsgraph1->getlinestrokethickness_lsplot());
  enumManager_->setValue(
      channel1plotpropertylinestroketypeitem_,
      static_cast<int>(lsgraph1->getlinestrokestyle_lsplot() - 1));
  colorManager_->setValue(channel1plotpropertylinefillcoloritem_,
                          lsgraph1->getlinefillcolor_lsplot());
  boolManager_->setValue(channel1plotpropertylineantialiaseditem_,
                         lsgraph1->getlineantialiased_lsplot());
  enumManager_->setValue(channel1plotpropertyscatterstyleitem_,
                         static_cast<int>(lsgraph1->getscattershape_lsplot()));
  doubleManager_->setValue(channel1plotpropertyscatterthicknessitem_,
                           lsgraph1->getscattersize_lsplot());
  colorManager_->setValue(channel1plotpropertyscatterfillcoloritem_,
                          lsgraph1->getscatterfillcolor_lsplot());
  colorManager_->setValue(channel1plotpropertyscatterstrokecoloritem_,
                          lsgraph1->getscatterstrokecolor_lsplot());
  enumManager_->setValue(
      channel1plotpropertyscatterstrokestyleitem_,
      static_cast<int>(lsgraph1->getscatterstrokestyle_lsplot() - 1));
  doubleManager_->setValue(channel1plotpropertyscatterstrokethicknessitem_,
                           lsgraph1->getscatterstrokethickness_lsplot());
  boolManager_->setValue(channel1plotpropertyscatterantialiaseditem_,
                         lsgraph1->getscatterantialiased_lsplot());
  enumManager_->setValue(channel2plotpropertylinestyleitem_,
                         static_cast<int>(lsgraph2->getlinetype_lsplot()));
  colorManager_->setValue(channel2plotpropertylinestrokecoloritem_,
                          lsgraph2->getlinestrokecolor_lsplot());
  doubleManager_->setValue(channel2plotpropertylinestrokethicknessitem_,
                           lsgraph2->getlinestrokethickness_lsplot());
  enumManager_->setValue(
      channel2plotpropertylinestroketypeitem_,
      static_cast<int>(lsgraph2->getlinestrokestyle_lsplot() - 1));
  boolManager_->setValue(channel2plotpropertylineantialiaseditem_,
                         lsgraph2->getlineantialiased_lsplot());
  enumManager_->setValue(channel2plotpropertyscatterstyleitem_,
                         static_cast<int>(lsgraph2->getscattershape_lsplot()));
  doubleManager_->setValue(channel2plotpropertyscatterthicknessitem_,
                           lsgraph2->getscattersize_lsplot());
  colorManager_->setValue(channel2plotpropertyscatterfillcoloritem_,
                          lsgraph2->getscatterfillcolor_lsplot());
  colorManager_->setValue(channel2plotpropertyscatterstrokecoloritem_,
                          lsgraph2->getscatterstrokecolor_lsplot());
  enumManager_->setValue(
      channel2plotpropertyscatterstrokestyleitem_,
      static_cast<int>(lsgraph2->getscatterstrokestyle_lsplot() - 1));
  doubleManager_->setValue(channel2plotpropertyscatterstrokethicknessitem_,
                           lsgraph2->getscatterstrokethickness_lsplot());
  boolManager_->setValue(channel2plotpropertyscatterantialiaseditem_,
                         lsgraph2->getscatterantialiased_lsplot());
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
      if (yaxes.at(i) == curve->getyaxis()) {
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
      if (xaxes.at(i) == curve->getxaxis()) {
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
  propertybrowser_->addProperty(barplotpropertystackgapitem_);
  propertybrowser_->addProperty(barplotpropertyfillantialiaseditem_);
  propertybrowser_->addProperty(barplotpropertyfillcoloritem_);
  propertybrowser_->addProperty(barplotpropertyantialiaseditem_);
  propertybrowser_->addProperty(barplotpropertystrokecoloritem_);
  propertybrowser_->addProperty(barplotpropertystrokethicknessitem_);
  propertybrowser_->addProperty(barplotpropertystrokestyleitem_);
  propertybrowser_->addProperty(barplotpropertylegendtextitem_);
  QStringList baryaxislist;
  int currentyaxis = 0;
  int ycount = 0;
  if (bargraph->getxaxis()->getorientation_axis() ==
          Axis2D::AxisOreantation::Top ||
      bargraph->getxaxis()->getorientation_axis() ==
          Axis2D::AxisOreantation::Bottom) {
    QList<Axis2D *> yaxes = axisrect->getYAxes2D();
    for (int i = 0; i < yaxes.size(); i++) {
      baryaxislist << QString("Y Axis %1").arg(i + 1);
      if (yaxes.at(i) == bargraph->getyaxis()) {
        currentyaxis = ycount;
      }
      ycount++;
    }
  } else {
    QList<Axis2D *> yaxes = axisrect->getXAxes2D();
    for (int i = 0; i < yaxes.size(); i++) {
      baryaxislist << QString("X Axis %1").arg(i + 1);
      if (yaxes.at(i) == bargraph->getyaxis()) {
        currentyaxis = ycount;
      }
      ycount++;
    }
  }
  enumManager_->setEnumNames(barplotpropertyyaxisitem_, baryaxislist);
  enumManager_->setValue(barplotpropertyyaxisitem_, currentyaxis);

  QStringList barxaxislist;
  int currentxaxis = 0;
  int xcount = 0;
  if (bargraph->getxaxis()->getorientation_axis() ==
          Axis2D::AxisOreantation::Top ||
      bargraph->getxaxis()->getorientation_axis() ==
          Axis2D::AxisOreantation::Bottom) {
    QList<Axis2D *> xaxes = axisrect->getXAxes2D();
    for (int i = 0; i < xaxes.size(); i++) {
      barxaxislist << QString("X Axis %1").arg(i + 1);
      if (xaxes.at(i) == bargraph->getxaxis()) {
        currentxaxis = xcount;
      }
      xcount++;
    }
  } else {
    QList<Axis2D *> xaxes = axisrect->getYAxes2D();
    for (int i = 0; i < xaxes.size(); i++) {
      barxaxislist << QString("Y Axis %1").arg(i + 1);
      if (xaxes.at(i) == bargraph->getxaxis()) {
        currentxaxis = xcount;
      }
      xcount++;
    }
  }
  enumManager_->setEnumNames(barplotpropertyxaxisitem_, barxaxislist);
  enumManager_->setValue(barplotpropertyxaxisitem_, currentxaxis);

  doubleManager_->setValue(barplotpropertywidthitem_, bargraph->width());
  doubleManager_->setValue(barplotpropertystackgapitem_,
                           bargraph->stackingGap());
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
  propertybrowser_->addProperty(statboxplotpropertyboxstyleitem_);
  propertybrowser_->addProperty(statboxplotpropertyantialiaseditem_);
  propertybrowser_->addProperty(statboxplotpropertyboxoutlinecoloritem_);
  propertybrowser_->addProperty(statboxplotpropertyboxoutlinethicknessitem_);
  propertybrowser_->addProperty(statboxplotpropertyboxoutlinestyleitem_);
  propertybrowser_->addProperty(statboxplotpropertyfillantialiaseditem_);
  propertybrowser_->addProperty(statboxplotpropertyfillstatusitem_);
  propertybrowser_->addProperty(statboxplotpropertyfillcoloritem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerwidthitem_);
  propertybrowser_->addProperty(statboxplotpropertywhiskerstyleitem_);
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
      if (yaxes.at(i) == statbox->getyaxis()) {
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
      if (xaxes.at(i) == statbox->getxaxis()) {
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

  colorManager_->setValue(statboxplotpropertyboxoutlinecoloritem_,
                          statbox->pen().color());
  doubleManager_->setValue(statboxplotpropertyboxoutlinethicknessitem_,
                           statbox->pen().widthF());
  enumManager_->setValue(statboxplotpropertyboxoutlinestyleitem_,
                         statbox->pen().style() - 1);
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
      if (yaxes.at(i) == vectorgraph->getyaxis()) {
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
      if (xaxes.at(i) == vectorgraph->getxaxis()) {
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
                             Vector2D::LineEndLocation::Stop)));
  doubleManager_->setValue(
      vectorpropertylineendingheightitem_,
      vectorgraph->getendheight_vecplot(Vector2D::LineEndLocation::Stop));
  doubleManager_->setValue(
      vectorpropertylineendingwidthitem_,
      vectorgraph->getendwidth_vecplot(Vector2D::LineEndLocation::Stop));
  boolManager_->setValue(vectorpropertylineantialiaseditem_,
                         vectorgraph->getlineantialiased_vecplot());
  stringManager_->setValue(
      vectorpropertylegendtextitem_,
      Utilities::joinstring(vectorgraph->getlegendtext_vecplot()));
}

void PropertyEditor::Pie2DPropertyBlock(Pie2D *piegraph, AxisRect2D *axisrect) {
  Q_UNUSED(axisrect)
  propertybrowser_->clear();

  // Pie Properties Block
  propertybrowser_->addProperty(pieplotpropertystyleitem_);
  propertybrowser_->addProperty(pieplotpropertylinestrokecoloritem_);
  propertybrowser_->addProperty(pieplotpropertylinestrokethicknessitem_);
  propertybrowser_->addProperty(pieplotpropertylinestroketypeitem_);
  propertybrowser_->addProperty(pieplotpropertymarginpercentitem_);

  enumManager_->setValue(pieplotpropertystyleitem_,
                         static_cast<int>(piegraph->getStyle_pieplot()));
  colorManager_->setValue(pieplotpropertylinestrokecoloritem_,
                          piegraph->getstrokecolor_pieplot());
  doubleManager_->setValue(pieplotpropertylinestrokethicknessitem_,
                           piegraph->getstrokethickness_pieplot());
  enumManager_->setValue(pieplotpropertylinestroketypeitem_,
                         piegraph->getstrokestyle_pieplot() - 1);
  intManager_->setValue(pieplotpropertymarginpercentitem_,
                        piegraph->getmarginpercent_pieplot());
}

void PropertyEditor::ColorMap2DPropertyBlock(ColorMap2D *colormap,
                                             AxisRect2D *axisrect) {
  Q_UNUSED(axisrect)
  propertybrowser_->clear();

  // Colormap Properties Block
  propertybrowser_->addProperty(colormappropertyinterpolateitem_);
  propertybrowser_->addProperty(colormappropertytightboundaryitem_);
  propertybrowser_->addProperty(colormappropertylevelcountitem_);
  propertybrowser_->addProperty(colormappropertygradientitem_);
  propertybrowser_->addProperty(colormappropertygradientinvertitem_);
  propertybrowser_->addProperty(colormappropertygradientperiodicitem_);

  propertybrowser_->addProperty(colormappropertyscalevisibleitem_);
  propertybrowser_->addProperty(colormappropertyscaleaxisvisibleitem_);
  propertybrowser_->addProperty(colormappropertyscaleaxistickvisibilityitem_);
  propertybrowser_->addProperty(
      colormappropertyscaleaxissubtickvisibilityitem_);
  propertybrowser_->addProperty(
      colormappropertyscaleaxisticklabelvisibilityitem_);

  boolManager_->setValue(colormappropertyinterpolateitem_,
                         colormap->interpolate());
  boolManager_->setValue(colormappropertytightboundaryitem_,
                         colormap->tightBoundary());
  intManager_->setValue(colormappropertylevelcountitem_,
                        colormap->getlevelcount_colormap());
  enumManager_->setValue(colormappropertygradientitem_,
                         static_cast<int>(colormap->getgradient_colormap()));
  boolManager_->setValue(colormappropertygradientinvertitem_,
                         colormap->getgradientinverted_colormap());
  boolManager_->setValue(colormappropertygradientperiodicitem_,
                         colormap->getgradientperiodic_colormap());
  boolManager_->setValue(colormappropertyscalevisibleitem_,
                         colormap->getcolormapscale_colormap()->visible());
  intManager_->setValue(colormappropertyscalewidthitem_,
                        colormap->getcolormapscalewidth_colormap());
  boolManager_->setValue(
      colormappropertyscaleaxisvisibleitem_,
      colormap->getcolormapscale_colormap()->axis()->visible());
  intManager_->setValue(
      colormappropertyscaleaxisoffsetitem_,
      colormap->getcolormapscale_colormap()->axis()->offset());
  doubleManager_->setValue(
      colormappropertyscaleaxisfromitem_,
      colormap->getcolormapscale_colormap()->axis()->range().lower);
  doubleManager_->setValue(
      colormappropertyscaleaxistoitem_,
      colormap->getcolormapscale_colormap()->axis()->range().upper);
  enumManager_->setValue(colormappropertyscaleaxislinlogitem_,
                         static_cast<int>(colormap->dataScaleType()));
  boolManager_->setValue(
      colormappropertyscaleaxisinvertitem_,
      colormap->getcolormapscale_colormap()->axis()->rangeReversed());
  colorManager_->setValue(
      colormappropertyscaleaxisstrokecoloritem_,
      colormap->getcolormapscale_colormap()->axis()->basePen().color());
  doubleManager_->setValue(
      colormappropertyscaleaxisstrokethicknessitem_,
      colormap->getcolormapscale_colormap()->axis()->basePen().widthF());
  enumManager_->setValue(
      colormappropertyscaleaxisstroketypeitem_,
      colormap->getcolormapscale_colormap()->axis()->basePen().style() - 1);
  boolManager_->setValue(
      colormappropertyscaleaxisantialiaseditem_,
      colormap->getcolormapscale_colormap()->axis()->antialiased());
  stringManager_->setValue(colormappropertyscaleaxislabeltextitem_,
                           colormap->getname_colormap());
  fontManager_->setValue(
      colormappropertyscaleaxislabelfontitem_,
      colormap->getcolormapscale_colormap()->axis()->labelFont());
  colorManager_->setValue(
      colormappropertyscaleaxislabelcoloritem_,
      colormap->getcolormapscale_colormap()->axis()->labelColor());
  intManager_->setValue(
      colormappropertyscaleaxislabelpaddingitem_,
      colormap->getcolormapscale_colormap()->axis()->labelPadding());
  boolManager_->setValue(
      colormappropertyscaleaxistickvisibilityitem_,
      colormap->getcolormapscale_colormap()->axis()->ticks());
  intManager_->setValue(
      colormappropertyscaleaxisticklengthinitem_,
      colormap->getcolormapscale_colormap()->axis()->tickLengthIn());
  intManager_->setValue(
      colormappropertyscaleaxisticklengthoutitem_,
      colormap->getcolormapscale_colormap()->axis()->tickLengthOut());
  colorManager_->setValue(
      colormappropertyscaleaxistickstrokecoloritem_,
      colormap->getcolormapscale_colormap()->axis()->tickPen().color());
  doubleManager_->setValue(
      colormappropertyscaleaxistickstrokethicknessitem_,
      colormap->getcolormapscale_colormap()->axis()->tickPen().widthF());
  enumManager_->setValue(
      colormappropertyscaleaxistickstroketypeitem_,
      colormap->getcolormapscale_colormap()->axis()->tickPen().style() - 1);
  boolManager_->setValue(
      colormappropertyscaleaxissubtickvisibilityitem_,
      colormap->getcolormapscale_colormap()->axis()->subTicks());
  intManager_->setValue(
      colormappropertyscaleaxissubticklengthinitem_,
      colormap->getcolormapscale_colormap()->axis()->subTickLengthIn());
  intManager_->setValue(
      colormappropertyscaleaxissubticklengthoutitem_,
      colormap->getcolormapscale_colormap()->axis()->subTickLengthOut());
  colorManager_->setValue(
      colormappropertyscaleaxissubtickstrokecoloritem_,
      colormap->getcolormapscale_colormap()->axis()->subTickPen().color());
  doubleManager_->setValue(
      colormappropertyscaleaxissubtickstrokethicknessitem_,
      colormap->getcolormapscale_colormap()->axis()->subTickPen().widthF());
  enumManager_->setValue(
      colormappropertyscaleaxissubtickstroketypeitem_,
      colormap->getcolormapscale_colormap()->axis()->subTickPen().style() - 1);
  boolManager_->setValue(
      colormappropertyscaleaxisticklabelvisibilityitem_,
      colormap->getcolormapscale_colormap()->axis()->tickLabels());
  fontManager_->setValue(
      colormappropertyscaleaxisticklabelfontitem_,
      colormap->getcolormapscale_colormap()->axis()->tickLabelFont());
  colorManager_->setValue(
      colormappropertyscaleaxisticklabelcoloritem_,
      colormap->getcolormapscale_colormap()->axis()->tickLabelColor());
  intManager_->setValue(
      colormappropertyscaleaxisticklabelpaddingitem_,
      colormap->getcolormapscale_colormap()->axis()->tickLabelPadding());
  doubleManager_->setValue(
      colormappropertyscaleaxisticklabelrotationitem_,
      colormap->getcolormapscale_colormap()->axis()->tickLabelRotation());
  enumManager_->setValue(
      colormappropertyscaleaxisticklabelsideitem_,
      colormap->getcolormapscale_colormap()->axis()->tickLabelSide());
  enumManager_->setValue(
      colormappropertyscaleaxisticklabelformatitem_,
      static_cast<int>(colormap->getcolormapscaleticklabelformat_axis()));
  intManager_->setValue(
      colormappropertyscaleaxisticklabelprecisionitem_,
      colormap->getcolormapscale_colormap()->axis()->numberPrecision());
}

void PropertyEditor::ErrorBar2DPropertyBlock(ErrorBar2D *errorbar,
                                             AxisRect2D *axisrect) {
  Q_UNUSED(axisrect)
  propertybrowser_->clear();
  propertybrowser_->addProperty(errorbarpropertywhiskerwidthitem_);
  propertybrowser_->addProperty(errorbarpropertysymbolgaptem_);
  propertybrowser_->addProperty(errorbarpropertyantialiaseditem_);
  propertybrowser_->addProperty(errorbarpropertystrokecoloritem_);
  propertybrowser_->addProperty(errorbarpropertystrokethicknessitem_);
  propertybrowser_->addProperty(errorbarpropertystroketypeitem_);
  propertybrowser_->addProperty(errorbarpropertyfillstatusitem_);
  propertybrowser_->addProperty(errorbarpropertyfillcoloritem_);
  doubleManager_->setValue(errorbarpropertywhiskerwidthitem_,
                           errorbar->whiskerWidth());
  doubleManager_->setValue(errorbarpropertysymbolgaptem_,
                           errorbar->symbolGap());
  colorManager_->setValue(errorbarpropertystrokecoloritem_,
                          errorbar->pen().color());
  doubleManager_->setValue(errorbarpropertystrokethicknessitem_,
                           errorbar->pen().widthF());
  enumManager_->setValue(errorbarpropertystroketypeitem_,
                         errorbar->pen().style() - 1);
  boolManager_->setValue(errorbarpropertyfillstatusitem_,
                         errorbar->getfillstatus_errorbar());
  colorManager_->setValue(errorbarpropertyfillcoloritem_,
                          errorbar->brush().color());
  boolManager_->setValue(errorbarpropertyantialiaseditem_,
                         errorbar->antialiased());
}

void PropertyEditor::axisRectCreated(AxisRect2D *axisrect, MyWidget *widget) {
  populateObjectBrowser(widget);
  axisrectConnections(axisrect);
}

void PropertyEditor::objectschanged() {
  ApplicationWindow *app_ = qobject_cast<ApplicationWindow *>(parent());
  if (app_) {
    MyWidget *mywidget = app_->getactiveMyWidget();
    populateObjectBrowser(mywidget);
  }
}

void PropertyEditor::Plot2DPropertyBlock(Plot2D *plotcanvas) {
  propertybrowser_->clear();
  propertybrowser_->addProperty(canvaspropertycoloritem_);
  propertybrowser_->addProperty(canvaspropertybufferdevicepixelratioitem_);
  propertybrowser_->addProperty(canvaspropertyopenglitem_);
  propertybrowser_->addProperty(canvaspropertyrectitem_);
  colorManager_->setValue(canvaspropertycoloritem_,
                          plotcanvas->getBackgroundColor());
  doubleManager_->setValue(canvaspropertybufferdevicepixelratioitem_,
                           plotcanvas->bufferDevicePixelRatio());
  boolManager_->setValue(canvaspropertyopenglitem_, plotcanvas->openGl());
  rectManager_->setValue(canvaspropertyrectitem_, plotcanvas->geometry());
}

void PropertyEditor::populateObjectBrowser(MyWidget *widget) {
  // delete all TreeWidgetItems
  while (objectbrowser_->topLevelItemCount()) {
    QTreeWidgetItemIterator itr(objectbrowser_,
                                QTreeWidgetItemIterator::NoChildren);
    while (*itr) {
      delete (*itr);
      ++itr;
    }
  }
  objectbrowser_->clear();
  objectitems_.clear();
  propertybrowser_->clear();

  QString tooltiptextx = QString(
      "<tr> <td align=\"right\">Table :</td><td>%1</td></tr>"
      "<tr> <td align=\"right\">Column :</td><td>%2</td></tr>"
      "<tr> <td align=\"right\">From :</td><td>%4</td></tr>"
      "<tr> <td align=\"right\">To :</td><td>%5</td></tr>");
  QString tooltiptextxy = QString(
      "<tr> <td align=\"right\">Table :</td><td>%1</td></tr>"
      "<tr> <td align=\"right\">Column X :</td><td>%2</td></tr>"
      "<tr> <td align=\"right\">Column Y :</td><td>%3</td></tr>"
      "<tr> <td align=\"right\">From :</td><td>%4</td></tr>"
      "<tr> <td align=\"right\">To :</td><td>%5</td></tr>");
  QString tooltiptextxyyy = QString(
      "<tr> <td align=\"right\">Table :</td><td>%1</td></tr>"
      "<tr> <td align=\"right\">Column :</td><td>%2</td></tr>"
      "<tr> <td align=\"right\">Column :</td><td>%3</td></tr>"
      "<tr> <td align=\"right\">Column :</td><td>%4</td></tr>"
      "<tr> <td align=\"right\">Column :</td><td>%5</td></tr>"
      "<tr> <td align=\"right\">From :</td><td>%6</td></tr>"
      "<tr> <td align=\"right\">To :</td><td>%7</td></tr>");
  QString tooltiptextxyy = QString(
      "<tr> <td align=\"right\">Table :</td><td>%1</td></tr>"
      "<tr> <td align=\"right\">Column X :</td><td>%2</td></tr>"
      "<tr> <td align=\"right\">Column Y1 :</td><td>%3</td></tr>"
      "<tr> <td align=\"right\">Column Y2 :</td><td>%4</td></tr>"
      "<tr> <td align=\"right\">From :</td><td>%6</td></tr>"
      "<tr> <td align=\"right\">To :</td><td>%7</td></tr>");
  QString tooltiptextmatrix = QString(
      "<tr> <td align=\"right\">Matrix :</td><td>%1</td></tr>"
      "<tr> <td align=\"right\">Rows :</td><td>%2</td></tr>"
      "<tr> <td align=\"right\">Columns :</td><td>%4</td></tr>");

  if (qobject_cast<Layout2D *>(widget)) {
    Layout2D *gd = qobject_cast<Layout2D *>(widget);
    objectbrowser_->setHeaderLabel(qobject_cast<Layout2D *>(widget)->name());
    objectbrowser_->headerItem()->setIcon(
        0, IconLoader::load("edit-graph", IconLoader::LightDark));
    QList<AxisRect2D *> elementslist = gd->getAxisRectList();

    // canvas
    QString canvasitemtext = QString("Canvas");
    QTreeWidgetItem *canvasitem = new QTreeWidgetItem(
        static_cast<QTreeWidget *>(nullptr), QStringList(canvasitemtext));
    canvasitem->setToolTip(0, canvasitemtext);
    canvasitem->setIcon(0,
                        IconLoader::load("view-image", IconLoader::LightDark));
    canvasitem->setData(
        0, Qt::UserRole,
        static_cast<int>(MyTreeWidget::PropertyItemType::PlotCanvas));
    canvasitem->setData(0, Qt::UserRole + 1,
                        QVariant::fromValue<void *>(gd->getPlotCanwas()));
    objectitems_.append(canvasitem);

    // Layout items
    for (int i = 0; i < elementslist.size(); ++i) {
      AxisRect2D *element = elementslist.at(i);
      QString itemtext =
          QString("Layout: %1 (%2x%3)").arg(i + 1).arg(1).arg(i + 1);
      QTreeWidgetItem *item = new QTreeWidgetItem(
          static_cast<QTreeWidget *>(nullptr), QStringList(itemtext));
      item->setToolTip(0, itemtext);
      item->setIcon(0,
                    IconLoader::load("graph2d-layout", IconLoader::LightDark));
      item->setData(0, Qt::UserRole,
                    static_cast<int>(MyTreeWidget::PropertyItemType::Layout));
      item->setData(0, Qt::UserRole + 1, QVariant::fromValue<void *>(element));

      // Legend
      QString legendtext = tr("Legend");
      QTreeWidgetItem *legenditem = new QTreeWidgetItem(
          static_cast<QTreeWidget *>(nullptr), QStringList(legendtext));
      legenditem->setToolTip(0, legendtext);
      legenditem->setIcon(
          0, IconLoader::load("edit-legend", IconLoader::LightDark));
      legenditem->setData(
          0, Qt::UserRole,
          static_cast<int>(MyTreeWidget::PropertyItemType::Legend));
      Legend2D *legend = element->getLegend();
      legenditem->setData(0, Qt::UserRole + 1,
                          QVariant::fromValue<void *>(legend));
      item->addChild(legenditem);

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
        axisitem->setToolTip(0, axistext);
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
        axisitem->setToolTip(0, axistext);
        axisitem->setData(
            0, Qt::UserRole,
            static_cast<int>(MyTreeWidget::PropertyItemType::Axis));
        axisitem->setData(0, Qt::UserRole + 1,
                          QVariant::fromValue<void *>(axis));
        item->addChild(axisitem);
      }

      // plottables & items vector of element
      auto textitems = element->getTextItemVec();
      auto lineitems = element->getLineItemVec();
      auto imageitems = element->getImageItemVec();
      auto graphvec = element->getLsVec();
      auto curvevec = element->getCurveVec();
      auto statboxvec = element->getStatBoxVec();
      auto vectorvec = element->getVectorVec();
      auto channelvec = element->getChannelVec();
      auto barvec = element->getBarVec();
      auto pievec = element->getPieVec();
      auto colormapvec = element->getColorMapVec();
      auto layervec = element->getLayerVec();
      // reverse layer list order
      for (int k = 0, s = layervec.size(), max = (s / 2); k < max; k++)
        layervec.swapItemsAt(k, s - (1 + k));

      foreach (QCPLayer *layer, layervec) {
        bool layerfound = false;
        // Text items
        for (int j = 0; j < textitems.size(); j++) {
          if (layer == textitems.at(j)->layer()) {
            QTreeWidgetItem *textitem =
                new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr));
            QString text = QString("Text Item: " + QString::number(j + 1));
            textitem->setIcon(
                0, IconLoader::load("draw-text", IconLoader::LightDark));
            textitem->setText(0, text);
            textitem->setToolTip(0, text);
            textitem->setData(
                0, Qt::UserRole,
                static_cast<int>(MyTreeWidget::PropertyItemType::TextItem));
            textitem->setData(0, Qt::UserRole + 1,
                              QVariant::fromValue<void *>(textitems.at(j)));
            item->addChild(textitem);
            textitems.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // Line items
        for (int j = 0; j < lineitems.size(); j++) {
          if (layer == lineitems.at(j)->layer()) {
            QTreeWidgetItem *lineitem =
                new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr));
            QString text = QString("Line Item: " + QString::number(j + 1));
            lineitem->setIcon(
                0, IconLoader::load("draw-line", IconLoader::LightDark));
            lineitem->setText(0, text);
            lineitem->setToolTip(0, text);
            lineitem->setData(
                0, Qt::UserRole,
                static_cast<int>(MyTreeWidget::PropertyItemType::LineItem));
            lineitem->setData(0, Qt::UserRole + 1,
                              QVariant::fromValue<void *>(lineitems.at(j)));
            item->addChild(lineitem);
            lineitems.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // Image items
        for (int j = 0; j < imageitems.size(); j++) {
          if (layer == imageitems.at(j)->layer()) {
            QTreeWidgetItem *imageitem =
                new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr));
            QString text = QString("Image Item: " + QString::number(j + 1));
            imageitem->setIcon(
                0, IconLoader::load("view-image", IconLoader::LightDark));
            imageitem->setText(0, text);
            imageitem->setToolTip(0, text);
            imageitem->setData(
                0, Qt::UserRole,
                static_cast<int>(MyTreeWidget::PropertyItemType::ImageItem));
            imageitem->setData(0, Qt::UserRole + 1,
                               QVariant::fromValue<void *>(imageitems.at(j)));
            item->addChild(imageitem);
            imageitems.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // LineSpecial plot Items
        for (int j = 0; j < graphvec.size(); j++) {
          if (layer == graphvec.at(j)->layer()) {
            LineSpecial2D *lsgraph = graphvec.at(j);
            DataBlockGraph *data = lsgraph->getdatablock_lsplot();
            QString lsgraphtext = data->gettable()->name() + "_" +
                                  data->getxcolumn()->name() + "_" +
                                  data->getycolumn()->name() + "[" +
                                  QString::number(data->getfrom() + 1) + ":" +
                                  QString::number(data->getto() + 1) + "]";
            QTreeWidgetItem *lsgraphitem = new QTreeWidgetItem(
                static_cast<QTreeWidget *>(nullptr), QStringList(lsgraphtext));
            QString tooltiptext = tooltiptextxy.arg(data->gettable()->name())
                                      .arg(data->getxcolumn()->name())
                                      .arg(data->getycolumn()->name())
                                      .arg(QString::number(data->getfrom() + 1))
                                      .arg(QString::number(data->getto() + 1));
            lsgraphitem->setToolTip(0, tooltiptext);
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
            // x error
            ErrorBar2D *xerror = lsgraph->getxerrorbar_lsplot();
            if (xerror) {
              DataBlockError *data = xerror->getdatablock_error();
              QString xerrortext = data->gettable()->name() + "_" +
                                   data->geterrorcolumn()->name() + "[" +
                                   QString::number(data->getfrom() + 1) + ":" +
                                   QString::number(data->getto() + 1) + "]";
              QTreeWidgetItem *xerroritem = new QTreeWidgetItem(
                  static_cast<QTreeWidget *>(nullptr), QStringList(xerrortext));
              QString tooltiperror =
                  tooltiptextx.arg(data->gettable()->name())
                      .arg(data->geterrorcolumn()->name())
                      .arg(QString::number(data->getfrom() + 1))
                      .arg(QString::number(data->getto() + 1));
              xerroritem->setToolTip(0, tooltiperror);
              xerroritem->setIcon(
                  0, IconLoader::load("graph-x-error", IconLoader::LightDark));
              xerroritem->setData(
                  0, Qt::UserRole,
                  static_cast<int>(MyTreeWidget::PropertyItemType::ErrorBar));
              xerroritem->setData(0, Qt::UserRole + 1,
                                  QVariant::fromValue<void *>(xerror));
              lsgraphitem->addChild(xerroritem);
            }
            // y error
            ErrorBar2D *yerror = lsgraph->getyerrorbar_lsplot();
            if (yerror) {
              DataBlockError *data = yerror->getdatablock_error();
              QString yerrortext = data->gettable()->name() + "_" +
                                   data->geterrorcolumn()->name() + "[" +
                                   QString::number(data->getfrom() + 1) + ":" +
                                   QString::number(data->getto() + 1) + "]";
              QTreeWidgetItem *yerroritem = new QTreeWidgetItem(
                  static_cast<QTreeWidget *>(nullptr), QStringList(yerrortext));
              QString tooltiperror =
                  tooltiptextx.arg(data->gettable()->name())
                      .arg(data->geterrorcolumn()->name())
                      .arg(QString::number(data->getfrom() + 1))
                      .arg(QString::number(data->getto() + 1));
              yerroritem->setToolTip(0, tooltiperror);
              yerroritem->setIcon(
                  0, IconLoader::load("graph-y-error", IconLoader::LightDark));
              yerroritem->setData(
                  0, Qt::UserRole,
                  static_cast<int>(MyTreeWidget::PropertyItemType::ErrorBar));
              yerroritem->setData(0, Qt::UserRole + 1,
                                  QVariant::fromValue<void *>(yerror));
              lsgraphitem->addChild(yerroritem);
            }
            graphvec.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // Curve plot Items
        int function = 1;
        for (int j = 0; j < curvevec.size(); j++) {
          if (layer == curvevec.at(j)->layer()) {
            Curve2D *curvegraph = curvevec.at(j);
            QString curvegraphtext = "curve";
            QTreeWidgetItem *curvegraphitem =
                new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                                    QStringList(curvegraphtext));
            switch (curvegraph->getplottype_cplot()) {
              case Graph2DCommon::PlotType::Associated: {
                DataBlockCurve *data = curvegraph->getdatablock_cplot();
                curvegraphtext = data->gettable()->name() + "_" +
                                 data->getxcolumn()->name() + "_" +
                                 data->getycolumn()->name() + "[" +
                                 QString::number(data->getfrom() + 1) + ":" +
                                 QString::number(data->getto() + 1) + "]";
                curvegraphitem->setText(0, curvegraphtext);
                QString tooltiptext =
                    tooltiptextxy.arg(data->gettable()->name())
                        .arg(data->getxcolumn()->name())
                        .arg(data->getycolumn()->name())
                        .arg(QString::number(data->getfrom() + 1))
                        .arg(QString::number(data->getto() + 1));
                curvegraphitem->setToolTip(0, tooltiptext);
                if (curvegraph->getcurvetype_cplot() ==
                    Curve2D::Curve2DType::Curve) {
                  curvegraphitem->setIcon(
                      0,
                      IconLoader::load("graph2d-curve", IconLoader::LightDark));
                } else
                  curvegraphitem->setIcon(
                      0, IconLoader::load("graph2d-spline",
                                          IconLoader::LightDark));
              } break;
              case Graph2DCommon::PlotType::Function:
                curvegraphtext = QString("Function %1").arg(function++);
                curvegraphitem->setIcon(
                    0, IconLoader::load("graph2d-function-xy",
                                        IconLoader::LightDark));
                curvegraphitem->setText(0, curvegraphtext);
                curvegraphitem->setToolTip(0, curvegraphtext);
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
            // x error
            ErrorBar2D *xerror = curvegraph->getxerrorbar_curveplot();
            if (xerror) {
              DataBlockError *data = xerror->getdatablock_error();
              QString xerrortext = data->gettable()->name() + "_" +
                                   data->geterrorcolumn()->name() + "[" +
                                   QString::number(data->getfrom() + 1) + ":" +
                                   QString::number(data->getto() + 1) + "]";
              QTreeWidgetItem *xerroritem = new QTreeWidgetItem(
                  static_cast<QTreeWidget *>(nullptr), QStringList(xerrortext));
              QString tooltiperror =
                  tooltiptextx.arg(data->gettable()->name())
                      .arg(data->geterrorcolumn()->name())
                      .arg(QString::number(data->getfrom() + 1))
                      .arg(QString::number(data->getto() + 1));
              xerroritem->setToolTip(0, tooltiperror);
              xerroritem->setIcon(
                  0, IconLoader::load("graph-x-error", IconLoader::LightDark));
              xerroritem->setData(
                  0, Qt::UserRole,
                  static_cast<int>(MyTreeWidget::PropertyItemType::ErrorBar));
              xerroritem->setData(0, Qt::UserRole + 1,
                                  QVariant::fromValue<void *>(xerror));
              curvegraphitem->addChild(xerroritem);
            }
            // y error
            ErrorBar2D *yerror = curvegraph->getyerrorbar_curveplot();
            if (yerror) {
              DataBlockError *data = yerror->getdatablock_error();
              QString yerrortext = data->gettable()->name() + "_" +
                                   data->geterrorcolumn()->name() + "[" +
                                   QString::number(data->getfrom() + 1) + ":" +
                                   QString::number(data->getto() + 1) + "]";
              QTreeWidgetItem *yerroritem = new QTreeWidgetItem(
                  static_cast<QTreeWidget *>(nullptr), QStringList(yerrortext));
              QString tooltiperror =
                  tooltiptextx.arg(data->gettable()->name())
                      .arg(data->geterrorcolumn()->name())
                      .arg(QString::number(data->getfrom() + 1))
                      .arg(QString::number(data->getto() + 1));
              yerroritem->setToolTip(0, tooltiperror);
              yerroritem->setIcon(
                  0, IconLoader::load("graph-y-error", IconLoader::LightDark));
              yerroritem->setData(
                  0, Qt::UserRole,
                  static_cast<int>(MyTreeWidget::PropertyItemType::ErrorBar));
              yerroritem->setData(0, Qt::UserRole + 1,
                                  QVariant::fromValue<void *>(yerror));
              curvegraphitem->addChild(yerroritem);
            }
            curvevec.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // Statbox plot Items
        for (int j = 0; j < statboxvec.size(); j++) {
          if (layer == statboxvec.at(j)->layer()) {
            StatBox2D *statbox = statboxvec.at(j);
            QString statboxtext =
                statbox->gettable_statbox()->name() + "_" +
                statbox->getcolumn_statbox()->name() + "[" +
                QString::number(statbox->getfrom_statbox() + 1) + ":" +
                QString::number(statbox->getto_statbox() + 1) + "]";
            QTreeWidgetItem *statboxitem = new QTreeWidgetItem(
                static_cast<QTreeWidget *>(nullptr), QStringList(statboxtext));
            QString tooltip =
                tooltiptextx.arg(statbox->gettable_statbox()->name())
                    .arg(statbox->getcolumn_statbox()->name())
                    .arg(QString::number(statbox->getfrom_statbox() + 1))
                    .arg(QString::number(statbox->getto_statbox() + 1));
            statboxitem->setToolTip(0, tooltip);
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
            statboxvec.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // Vector plot Items
        for (int j = 0; j < vectorvec.size(); j++) {
          if (layer == vectorvec.at(j)->layer()) {
            Vector2D *vector = vectorvec.at(j);
            QString vectortext =
                vector->gettable_vecplot()->name() + "_" +
                vector->getfirstcol_vecplot()->name() + "_" +
                vector->getsecondcol_vecplot()->name() + "_" +
                vector->getthirdcol_vecplot()->name() + "_" +
                vector->getfourthcol_vecplot()->name() + "[" +
                QString::number(vector->getfrom_vecplot() + 1) + ":" +
                QString::number(vector->getto_vecplot() + 1) + "]";
            QTreeWidgetItem *vectoritem = new QTreeWidgetItem(
                static_cast<QTreeWidget *>(nullptr), QStringList(vectortext));
            QString tooltiptext =
                tooltiptextxyyy.arg(vector->gettable_vecplot()->name())
                    .arg(vector->getfirstcol_vecplot()->name())
                    .arg(vector->getsecondcol_vecplot()->name())
                    .arg(vector->getthirdcol_vecplot()->name())
                    .arg(vector->getfourthcol_vecplot()->name())
                    .arg(QString::number(vector->getfrom_vecplot() + 1))
                    .arg(QString::number(vector->getto_vecplot() + 1));
            vectoritem->setToolTip(0, tooltiptext);
            vectoritem->setIcon(0, IconLoader::load("graph2d-vector-xy",
                                                    IconLoader::LightDark));
            vectoritem->setData(
                0, Qt::UserRole,
                static_cast<int>(MyTreeWidget::PropertyItemType::Vector));
            vectoritem->setData(0, Qt::UserRole + 1,
                                QVariant::fromValue<void *>(vector));
            vectoritem->setData(0, Qt::UserRole + 2,
                                QVariant::fromValue<void *>(element));
            item->addChild(vectoritem);
            vectorvec.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // LineSpecialchannel plot Items
        for (int j = 0; j < channelvec.size(); j++) {
          if (layer == channelvec.at(j).first->layer()) {
            LineSpecial2D *lsgraph1 = channelvec.at(j).first;
            LineSpecial2D *lsgraph2 = channelvec.at(j).second;
            DataBlockGraph *data1 = lsgraph1->getdatablock_lsplot();
            DataBlockGraph *data2 = lsgraph2->getdatablock_lsplot();
            QString lsgraph1text = data1->gettable()->name() + "_" +
                                   data1->getxcolumn()->name() + "_" +
                                   data1->getycolumn()->name() + "_" +
                                   data2->getycolumn()->name() + "[" +
                                   QString::number(data1->getfrom() + 1) + ":" +
                                   QString::number(data1->getto() + 1) + "]";
            QTreeWidgetItem *channelitem = new QTreeWidgetItem(
                static_cast<QTreeWidget *>(nullptr), QStringList(lsgraph1text));
            QString tooltiptext =
                tooltiptextxyy.arg(data1->gettable()->name())
                    .arg(data1->getxcolumn()->name())
                    .arg(data1->getycolumn()->name())
                    .arg(data2->getycolumn()->name())
                    .arg(QString::number(data1->getfrom() + 1))
                    .arg(QString::number(data1->getto() + 1));
            channelitem->setToolTip(0, tooltiptext);
            channelitem->setIcon(
                0, IconLoader::load("graph2d-channel", IconLoader::LightDark));
            channelitem->setData(
                0, Qt::UserRole,
                static_cast<int>(MyTreeWidget::PropertyItemType::ChannelGraph));
            channelitem->setData(0, Qt::UserRole + 1,
                                 QVariant::fromValue<void *>(lsgraph1));
            channelitem->setData(0, Qt::UserRole + 2,
                                 QVariant::fromValue<void *>(element));
            channelitem->setData(0, Qt::UserRole + 3,
                                 QVariant::fromValue<void *>(lsgraph2));
            item->addChild(channelitem);
            channelvec.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // Bar Plot items
        for (int j = 0; j < barvec.size(); j++) {
          if (layer == barvec.at(j)->layer()) {
            Bar2D *bar = barvec.at(j);
            QString bartext = QString("Histogram");
            QTreeWidgetItem *baritem = new QTreeWidgetItem(
                static_cast<QTreeWidget *>(nullptr), QStringList(bartext));
            if (bar->ishistogram_barplot()) {
              bartext = bar->gettable_histogram()->name() + "_" +
                        bar->getcolumn_histogram()->name() + "[" +
                        QString::number(bar->getfrom_histogram() + 1) + ":" +
                        QString::number(bar->getto_histogram() + 1) + "]";
              QString tooltiptext =
                  tooltiptextx.arg(bar->gettable_histogram()->name())
                      .arg(bar->getcolumn_histogram()->name())
                      .arg(QString::number(bar->getfrom_histogram() + 1))
                      .arg(QString::number(bar->getto_histogram() + 1));
              baritem->setToolTip(0, tooltiptext);
              baritem->setText(0, bartext);
              baritem->setIcon(0, IconLoader::load("graph2d-histogram",
                                                   IconLoader::LightDark));
            } else {
              DataBlockBar *data = bar->getdatablock_barplot();
              bartext = data->gettable()->name() + "_" +
                        data->getxcolumn()->name() + "_" +
                        data->getycolumn()->name() + "[" +
                        QString::number(data->getfrom() + 1) + ":" +
                        QString::number(data->getto() + 1) + "]";
              baritem->setText(0, bartext);
              if ((bar->getxaxis()->getorientation_axis() ==
                       Axis2D::AxisOreantation::Top ||
                   bar->getxaxis()->getorientation_axis() ==
                       Axis2D::AxisOreantation::Bottom) &&
                  bar->getstackposition_barplot() == -1) {
                baritem->setIcon(0, IconLoader::load("graph2d-vertical-bar",
                                                     IconLoader::LightDark));
              } else if ((bar->getxaxis()->getorientation_axis() ==
                              Axis2D::AxisOreantation::Top ||
                          bar->getxaxis()->getorientation_axis() ==
                              Axis2D::AxisOreantation::Bottom) &&
                         bar->getstackposition_barplot() != -1) {
                baritem->setIcon(0,
                                 IconLoader::load("graph2d-vertical-stack-bar",
                                                  IconLoader::LightDark));
              } else if ((bar->getxaxis()->getorientation_axis() !=
                              Axis2D::AxisOreantation::Top &&
                          bar->getxaxis()->getorientation_axis() !=
                              Axis2D::AxisOreantation::Bottom) &&
                         bar->getstackposition_barplot() == -1) {
                baritem->setIcon(0, IconLoader::load("graph2d-horizontal-bar",
                                                     IconLoader::LightDark));
              } else
                baritem->setIcon(
                    0, IconLoader::load("graph2d-horizontal-stack-bar",
                                        IconLoader::LightDark));
              QString tooltiptext =
                  tooltiptextxy.arg(data->gettable()->name())
                      .arg(data->getxcolumn()->name())
                      .arg(data->getycolumn()->name())
                      .arg(QString::number(data->getfrom() + 1))
                      .arg(QString::number(data->getto() + 1));
              baritem->setToolTip(0, tooltiptext);
            }
            baritem->setData(
                0, Qt::UserRole,
                static_cast<int>(MyTreeWidget::PropertyItemType::BarGraph));
            baritem->setData(0, Qt::UserRole + 1,
                             QVariant::fromValue<void *>(bar));
            baritem->setData(0, Qt::UserRole + 2,
                             QVariant::fromValue<void *>(element));
            item->addChild(baritem);
            // x error
            ErrorBar2D *xerror = bar->getxerrorbar_barplot();
            if (xerror) {
              DataBlockError *data = xerror->getdatablock_error();
              QString xerrortext = data->gettable()->name() + "_" +
                                   data->geterrorcolumn()->name() + "[" +
                                   QString::number(data->getfrom() + 1) + ":" +
                                   QString::number(data->getto() + 1) + "]";
              QTreeWidgetItem *xerroritem = new QTreeWidgetItem(
                  static_cast<QTreeWidget *>(nullptr), QStringList(xerrortext));
              QString tooltiperror =
                  tooltiptextx.arg(data->gettable()->name())
                      .arg(data->geterrorcolumn()->name())
                      .arg(QString::number(data->getfrom() + 1))
                      .arg(QString::number(data->getto() + 1));
              xerroritem->setToolTip(0, tooltiperror);
              xerroritem->setIcon(
                  0, IconLoader::load("graph-x-error", IconLoader::LightDark));
              xerroritem->setData(
                  0, Qt::UserRole,
                  static_cast<int>(MyTreeWidget::PropertyItemType::ErrorBar));
              xerroritem->setData(0, Qt::UserRole + 1,
                                  QVariant::fromValue<void *>(xerror));
              baritem->addChild(xerroritem);
            }
            // y error
            ErrorBar2D *yerror = bar->getyerrorbar_barplot();
            if (yerror) {
              DataBlockError *data = yerror->getdatablock_error();
              QString yerrortext = data->gettable()->name() + "_" +
                                   data->geterrorcolumn()->name() + "[" +
                                   QString::number(data->getfrom() + 1) + ":" +
                                   QString::number(data->getto() + 1) + "]";
              QTreeWidgetItem *yerroritem = new QTreeWidgetItem(
                  static_cast<QTreeWidget *>(nullptr), QStringList(yerrortext));
              QString tooltiperror =
                  tooltiptextx.arg(data->gettable()->name())
                      .arg(data->geterrorcolumn()->name())
                      .arg(QString::number(data->getfrom() + 1))
                      .arg(QString::number(data->getto() + 1));
              yerroritem->setToolTip(0, tooltiperror);
              yerroritem->setIcon(
                  0, IconLoader::load("graph-y-error", IconLoader::LightDark));
              yerroritem->setData(
                  0, Qt::UserRole,
                  static_cast<int>(MyTreeWidget::PropertyItemType::ErrorBar));
              yerroritem->setData(0, Qt::UserRole + 1,
                                  QVariant::fromValue<void *>(yerror));
              baritem->addChild(yerroritem);
            }
            barvec.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // Pie Plot Items
        for (int j = 0; j < pievec.size(); j++) {
          if (layer == pievec.at(j)->layer()) {
            Pie2D *pie = pievec.at(j);
            QString pietext = pie->gettable_pieplot()->name() + "_" +
                              pie->getxcolumn_pieplot()->name() + "[" +
                              QString::number(pie->getfrom_pieplot() + 1) +
                              ":" + QString::number(pie->getto_pieplot() + 1) +
                              "]";
            QTreeWidgetItem *pieitem = new QTreeWidgetItem(
                static_cast<QTreeWidget *>(nullptr), QStringList(pietext));
            QString tooltip =
                tooltiptextx.arg(pie->gettable_pieplot()->name())
                    .arg(pie->getxcolumn_pieplot()->name())
                    .arg(QString::number(pie->getfrom_pieplot() + 1))
                    .arg(QString::number(pie->getto_pieplot() + 1));
            pieitem->setToolTip(0, tooltip);
            pieitem->setIcon(
                0, IconLoader::load("graph2d-pie", IconLoader::LightDark));
            pieitem->setData(
                0, Qt::UserRole,
                static_cast<int>(MyTreeWidget::PropertyItemType::PieGraph));
            pieitem->setData(0, Qt::UserRole + 1,
                             QVariant::fromValue<void *>(pie));
            pieitem->setData(0, Qt::UserRole + 2,
                             QVariant::fromValue<void *>(element));
            item->addChild(pieitem);
            pievec.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;

        // ColorMap Plot Items
        for (int j = 0; j < colormapvec.size(); j++) {
          if (layer == colormapvec.at(j)->layer()) {
            ColorMap2D *colormap = colormapvec.at(j);

            QString colormaptext =
                colormap->getmatrix_colormap()->name() + "[" +
                QString::number(colormap->getrows_colormap()) + "x" +
                QString::number(colormap->getcolumns_colormap()) + "]";
            QTreeWidgetItem *colormapitem = new QTreeWidgetItem(
                static_cast<QTreeWidget *>(nullptr), QStringList(colormaptext));
            QString tooltip =
                tooltiptextmatrix.arg(colormap->getmatrix_colormap()->name())
                    .arg(QString::number(colormap->getrows_colormap()))
                    .arg(QString::number(colormap->getcolumns_colormap()));
            colormapitem->setToolTip(0, tooltip);
            colormapitem->setIcon(
                0, IconLoader::load("edit-colormap3d", IconLoader::General));
            colormapitem->setData(
                0, Qt::UserRole,
                static_cast<int>(MyTreeWidget::PropertyItemType::ColorMap));
            colormapitem->setData(0, Qt::UserRole + 1,
                                  QVariant::fromValue<void *>(colormap));
            colormapitem->setData(0, Qt::UserRole + 2,
                                  QVariant::fromValue<void *>(element));
            item->addChild(colormapitem);
            colormapvec.removeAt(j);
            layerfound = true;
            break;
          }
        }
        if (layerfound) continue;
      }

      // Grids
      QString gridtext = "Axis Grids";
      QTreeWidgetItem *griditem =
          new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                              QStringList(QString("Axis Grids")));
      griditem->setToolTip(0, gridtext);
      griditem->setIcon(
          0, IconLoader::load("graph3d-cross", IconLoader::LightDark));
      griditem->setData(0, Qt::UserRole,
                        static_cast<int>(MyTreeWidget::PropertyItemType::Grid));
      griditem->setData(0, Qt::UserRole + 1,
                        QVariant::fromValue<void *>(element));
      item->addChild(griditem);

      objectitems_.append(item);
    }
    if (previouswidget_ != gd)
      connect(gd, SIGNAL(AxisRectCreated(AxisRect2D *, MyWidget *)), this,
              SLOT(axisRectCreated(AxisRect2D *, MyWidget *)));
    objectbrowser_->addTopLevelItems(objectitems_);
    previouswidget_ = gd;
    objectbrowser_->insertTopLevelItems(0, objectitems_);
  } else if (qobject_cast<Graph3D *>(widget)) {
    objectbrowser_->setHeaderLabel(qobject_cast<Graph3D *>(widget)->name());
    objectbrowser_->headerItem()->setIcon(
        0, IconLoader::load("edit-graph3d", IconLoader::LightDark));
  } else if (qobject_cast<Table *>(widget)) {
    objectbrowser_->setHeaderLabel(qobject_cast<Table *>(widget)->name());
    objectbrowser_->headerItem()->setIcon(
        0, IconLoader::load("table", IconLoader::LightDark));
  } else if (qobject_cast<Note *>(widget)) {
    objectbrowser_->setHeaderLabel(qobject_cast<Note *>(widget)->name());
    objectbrowser_->headerItem()->setIcon(
        0, IconLoader::load("edit-note", IconLoader::LightDark));
  } else if (qobject_cast<Matrix *>(widget)) {
    objectbrowser_->setHeaderLabel(qobject_cast<Matrix *>(widget)->name());
    objectbrowser_->headerItem()->setIcon(
        0, IconLoader::load("matrix", IconLoader::LightDark));
  } else {
    objectbrowser_->setHeaderLabel("(none)");
    objectbrowser_->headerItem()->setIcon(
        0, IconLoader::load("clear-loginfo", IconLoader::General));
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
  connect(axisrect, &AxisRect2D::Axis2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect->getLegend(), &Legend2D::legendMoved, [=]() {
    if (objectbrowser_->currentItem() &&
        static_cast<MyTreeWidget::PropertyItemType>(
            objectbrowser_->currentItem()
                ->data(0, Qt::UserRole)
                .value<int>()) == MyTreeWidget::PropertyItemType::Legend) {
      QPointF origin = axisrect->getLegend()->getposition_legend();
      doubleManager_->setValue(itempropertylegendoriginxitem_, origin.x());
      doubleManager_->setValue(itempropertylegendoriginyitem_, origin.y());
      axisrect->parentPlot()->replot(
          QCustomPlot::RefreshPriority::rpQueuedRefresh);
    }
  });
  connect(axisrect, &AxisRect2D::TextItem2DMoved, [=]() {
    if (objectbrowser_->currentItem() &&
        static_cast<MyTreeWidget::PropertyItemType>(
            objectbrowser_->currentItem()
                ->data(0, Qt::UserRole)
                .value<int>()) == MyTreeWidget::PropertyItemType::TextItem) {
      TextItem2D *textitem =
          getgraph2dobject<TextItem2D>(objectbrowser_->currentItem());
      QPointF origin = textitem->position->pixelPosition();
      doubleManager_->setValue(itempropertytextpixelpositionxitem_, origin.x());
      doubleManager_->setValue(itempropertytextpixelpositionyitem_, origin.y());
    }
  });
  connect(axisrect, &AxisRect2D::LineItem2DMoved, [=]() {
    if (objectbrowser_->currentItem() &&
        static_cast<MyTreeWidget::PropertyItemType>(
            objectbrowser_->currentItem()
                ->data(0, Qt::UserRole)
                .value<int>()) == MyTreeWidget::PropertyItemType::LineItem) {
      LineItem2D *lineitem =
          getgraph2dobject<LineItem2D>(objectbrowser_->currentItem());
      QPointF origin1 = lineitem->position("start")->pixelPosition();
      QPointF origin2 = lineitem->position("end")->pixelPosition();
      doubleManager_->setValue(itempropertylinepixelpositionx1item_,
                               origin1.x());
      doubleManager_->setValue(itempropertylinepixelpositiony1item_,
                               origin1.y());
      doubleManager_->setValue(itempropertylinepixelpositionx2item_,
                               origin2.x());
      doubleManager_->setValue(itempropertylinepixelpositiony2item_,
                               origin2.y());
    }
  });
  connect(axisrect, &AxisRect2D::ImageItem2DMoved, [=]() {
    if (objectbrowser_->currentItem() &&
        static_cast<MyTreeWidget::PropertyItemType>(
            objectbrowser_->currentItem()
                ->data(0, Qt::UserRole)
                .value<int>()) == MyTreeWidget::PropertyItemType::ImageItem) {
      ImageItem2D *imageitem =
          getgraph2dobject<ImageItem2D>(objectbrowser_->currentItem());
      QPointF origin = imageitem->position("topLeft")->pixelPosition();
      doubleManager_->setValue(itempropertyimagepixelpositionxitem_,
                               origin.x());
      doubleManager_->setValue(itempropertyimagepixelpositionyitem_,
                               origin.y());
    }
  });
  connect(axisrect, &AxisRect2D::TextItem2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::LineItem2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::ImageItem2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::LineSpecial2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::LineSpecialChannel2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });

  connect(axisrect, &AxisRect2D::Curve2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::StatBox2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::Vector2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::Bar2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::Pie2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::ColorMap2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::ErrorBar2DCreated, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });

  // Removed
  connect(axisrect, &AxisRect2D::Axis2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::TextItem2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::LineItem2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::ImageItem2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::LineSpecial2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::LineSpecialChannel2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::Curve2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::StatBox2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::Vector2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::Bar2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::Pie2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::ColorMap2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
  connect(axisrect, &AxisRect2D::ErrorBar2DRemoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });

  // Layer moved
  connect(axisrect, &AxisRect2D::LayerMoved, [=]() {
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
    objectschanged();
  });
}

void PropertyEditor::setObjectPropertyId() {
  // Plot Canvas properties
  canvaspropertycoloritem_->setPropertyId("canvaspropertycoloritem_");
  canvaspropertybufferdevicepixelratioitem_->setPropertyId(
      "canvaspropertybufferdevicepixelratioitem_");
  canvaspropertyopenglitem_->setPropertyId("canvaspropertyopenglitem_");
  canvaspropertyrectitem_->setPropertyId("canvaspropertyrectitem_");
  // Layout properties
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
  axispropertytickcountitem_->setPropertyId("axispropertytickcountitem_");
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
  itempropertylegendoriginxitem_->setPropertyId(
      "itempropertylegendoriginxitem_");
  itempropertylegendoriginyitem_->setPropertyId(
      "itempropertylegendoriginyitem_");
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
  itempropertylinepixelpositionx1item_->setPropertyId(
      "itempropertylinepixelpositionx1item_");
  itempropertylinepixelpositiony1item_->setPropertyId(
      "itempropertylinepixelpositiony1item_");
  itempropertylinepixelpositionx2item_->setPropertyId(
      "itempropertylinepixelpositionx2item_");
  itempropertylinepixelpositiony2item_->setPropertyId(
      "itempropertylinepixelpositiony2item_");
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
  itempropertyimagepixelpositionxitem_->setPropertyId(
      "itempropertyimagepixelpositionxitem_");
  itempropertyimagepixelpositionyitem_->setPropertyId(
      "itempropertyimagepixelpositionyitem_");
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
  // LineSpecialChannel Properties block
  channelplotpropertyxaxisitem_->setPropertyId("channelplotpropertyxaxisitem_");
  channelplotpropertyyaxisitem_->setPropertyId("channelplotpropertyyaxisitem_");
  channelplotpropertylegendtextitem_->setPropertyId(
      "channelplotpropertylegendtextitem_");
  channel1plotpropertygroupitem_->setPropertyId(
      "channel1plotpropertygroupitem_");
  channel1plotpropertylinestyleitem_->setPropertyId(
      "channel1plotpropertylinestyleitem_");
  channel1plotpropertylinestrokecoloritem_->setPropertyId(
      "channel1plotpropertylinestrokecoloritem_");
  channel1plotpropertylinestrokethicknessitem_->setPropertyId(
      "channel1plotpropertylinestrokethicknessitem_");
  channel1plotpropertylinestroketypeitem_->setPropertyId(
      "channel1plotpropertylinestroketypeitem_");
  channel1plotpropertylinefillcoloritem_->setPropertyId(
      "channel1plotpropertylinefillcoloritem_");
  channel1plotpropertylineantialiaseditem_->setPropertyId(
      "channel1plotpropertylineantialiaseditem_");
  channel1plotpropertyscatterstyleitem_->setPropertyId(
      "channel1plotpropertyscatterstyleitem_");
  channel1plotpropertyscatterthicknessitem_->setPropertyId(
      "channel1plotpropertyscatterthicknessitem_");
  channel1plotpropertyscatterfillcoloritem_->setPropertyId(
      "channel1plotpropertyscatterfillcoloritem_");
  channel1plotpropertyscatterstrokecoloritem_->setPropertyId(
      "channel1plotpropertyscatterstrokecoloritem_");
  channel1plotpropertyscatterstrokethicknessitem_->setPropertyId(
      "channel1plotpropertyscatterstrokethicknessitem_");
  channel1plotpropertyscatterstrokestyleitem_->setPropertyId(
      "channel1plotpropertyscatterstrokestyleitem_");
  channel1plotpropertyscatterantialiaseditem_->setPropertyId(
      "channel1plotpropertyscatterantialiaseditem_");
  channel2plotpropertygroupitem_->setPropertyId(
      "channel2plotpropertygroupitem_");
  channel2plotpropertylinestyleitem_->setPropertyId(
      "channel2plotpropertylinestyleitem_");
  channel2plotpropertylinestrokecoloritem_->setPropertyId(
      "channel2plotpropertylinestrokecoloritem_");
  channel2plotpropertylinestrokethicknessitem_->setPropertyId(
      "channel2plotpropertylinestrokethicknessitem_");
  channel2plotpropertylinestroketypeitem_->setPropertyId(
      "channel2plotpropertylinestroketypeitem_");
  channel2plotpropertylineantialiaseditem_->setPropertyId(
      "channel2plotpropertylineantialiaseditem_");
  channel2plotpropertyscatterstyleitem_->setPropertyId(
      "channel2plotpropertyscatterstyleitem_");
  channel2plotpropertyscatterthicknessitem_->setPropertyId(
      "channel2plotpropertyscatterthicknessitem_");
  channel2plotpropertyscatterfillcoloritem_->setPropertyId(
      "channel2plotpropertyscatterfillcoloritem_");
  channel2plotpropertyscatterstrokecoloritem_->setPropertyId(
      "channel2plotpropertyscatterstrokecoloritem_");
  channel2plotpropertyscatterstrokethicknessitem_->setPropertyId(
      "channel2plotpropertyscatterstrokethicknessitem_");
  channel2plotpropertyscatterstrokestyleitem_->setPropertyId(
      "channel2plotpropertyscatterstrokestyleitem_");
  channel2plotpropertyscatterantialiaseditem_->setPropertyId(
      "channel2plotpropertyscatterantialiaseditem_");
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
  barplotpropertystackgapitem_->setPropertyId("barplotpropertystackgapitem_");
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
  statboxplotpropertyboxoutlinecoloritem_->setPropertyId(
      "statboxplotpropertyboxoutlinecoloritem_");
  statboxplotpropertyboxoutlinethicknessitem_->setPropertyId(
      "statboxplotpropertyboxoutlinethicknessitem_");
  statboxplotpropertyboxoutlinestyleitem_->setPropertyId(
      "statboxplotpropertyboxoutlinestyleitem_");
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
  pieplotpropertystyleitem_->setPropertyId("pieplotpropertystyleitem_");
  pieplotpropertylinestrokecoloritem_->setPropertyId(
      "pieplotpropertylinestrokecoloritem_");
  pieplotpropertylinestrokethicknessitem_->setPropertyId(
      "pieplotpropertylinestrokethicknessitem_");
  pieplotpropertylinestroketypeitem_->setPropertyId(
      "pieplotpropertylinestroketypeitem_");
  pieplotpropertymarginpercentitem_->setPropertyId(
      "pieplotpropertymarginpercentitem_");

  // Colormap Properties Block
  colormappropertyinterpolateitem_->setPropertyId(
      "colormappropertyinterpolateitem_");
  colormappropertytightboundaryitem_->setPropertyId(
      "colormappropertytightboundaryitem_");
  colormappropertylevelcountitem_->setPropertyId(
      "colormappropertylevelcountitem_");
  colormappropertygradientitem_->setPropertyId("colormappropertygradientitem_");
  colormappropertygradientinvertitem_->setPropertyId(
      "colormappropertygradientinvertitem_");
  colormappropertygradientperiodicitem_->setPropertyId(
      "colormappropertygradientperiodicitem_");
  colormappropertyscalevisibleitem_->setPropertyId(
      "colormappropertyscalevisibleitem_");
  colormappropertyscalewidthitem_->setPropertyId(
      "colormappropertyscalewidthitem_");
  // Colormap Axis general
  colormappropertyscaleaxisvisibleitem_->setPropertyId(
      "colormappropertyscaleaxisvisibleitem_");
  colormappropertyscaleaxisoffsetitem_->setPropertyId(
      "colormappropertyscaleaxisoffsetitem_");
  colormappropertyscaleaxisfromitem_->setPropertyId(
      "colormappropertyscaleaxisfromitem_");
  colormappropertyscaleaxistoitem_->setPropertyId(
      "colormappropertyscaleaxistoitem_");
  colormappropertyscaleaxislinlogitem_->setPropertyId(
      "colormappropertyscaleaxislinlogitem_");
  colormappropertyscaleaxisinvertitem_->setPropertyId(
      "colormappropertyscaleaxisinvertitem_");
  colormappropertyscaleaxisstrokecoloritem_->setPropertyId(
      "colormappropertyscaleaxisstrokecoloritem_");
  colormappropertyscaleaxisstrokethicknessitem_->setPropertyId(
      "colormappropertyscaleaxisstrokethicknessitem_");
  colormappropertyscaleaxisstroketypeitem_->setPropertyId(
      "colormappropertyscaleaxisstroketypeitem_");
  colormappropertyscaleaxisantialiaseditem_->setPropertyId(
      "colormappropertyscaleaxisantialiaseditem_");
  // Colormap Axis Properties Label sub block
  colormappropertyscaleaxislabeltextitem_->setPropertyId(
      "colormappropertyscaleaxislabeltextitem_");
  colormappropertyscaleaxislabelfontitem_->setPropertyId(
      "colormappropertyscaleaxislabelfontitem_");
  colormappropertyscaleaxislabelcoloritem_->setPropertyId(
      "colormappropertyscaleaxislabelcoloritem_");
  colormappropertyscaleaxislabelpaddingitem_->setPropertyId(
      "colormappropertyscaleaxislabelpaddingitem_");
  // Colormap Axis Properties Ticks sub block
  colormappropertyscaleaxistickvisibilityitem_->setPropertyId(
      "colormappropertyscaleaxistickvisibilityitem_");
  colormappropertyscaleaxisticklengthinitem_->setPropertyId(
      "colormappropertyscaleaxisticklengthinitem_");
  colormappropertyscaleaxisticklengthoutitem_->setPropertyId(
      "colormappropertyscaleaxisticklengthoutitem_");
  colormappropertyscaleaxistickstrokecoloritem_->setPropertyId(
      "colormappropertyscaleaxistickstrokecoloritem_");
  colormappropertyscaleaxistickstrokethicknessitem_->setPropertyId(
      "colormappropertyscaleaxistickstrokethicknessitem_");
  colormappropertyscaleaxistickstroketypeitem_->setPropertyId(
      "colormappropertyscaleaxistickstroketypeitem_");
  // Colormap Axis Properties Sub-ticks sub block
  colormappropertyscaleaxissubtickvisibilityitem_->setPropertyId(
      "colormappropertyscaleaxissubtickvisibilityitem_");
  colormappropertyscaleaxissubticklengthinitem_->setPropertyId(
      "colormappropertyscaleaxissubticklengthinitem_");
  colormappropertyscaleaxissubticklengthoutitem_->setPropertyId(
      "colormappropertyscaleaxissubticklengthoutitem_");
  colormappropertyscaleaxissubtickstrokecoloritem_->setPropertyId(
      "colormappropertyscaleaxissubtickstrokecoloritem_");
  colormappropertyscaleaxissubtickstrokethicknessitem_->setPropertyId(
      "colormappropertyscaleaxissubtickstrokethicknessitem_");
  colormappropertyscaleaxissubtickstroketypeitem_->setPropertyId(
      "colormappropertyscaleaxissubtickstroketypeitem_");
  // Colormap Axis Properties Ticks Label sub block
  colormappropertyscaleaxisticklabelvisibilityitem_->setPropertyId(
      "colormappropertyscaleaxisticklabelvisibilityitem_");
  colormappropertyscaleaxisticklabelfontitem_->setPropertyId(
      "colormappropertyscaleaxisticklabelfontitem_");
  colormappropertyscaleaxisticklabelcoloritem_->setPropertyId(
      "colormappropertyscaleaxisticklabelcoloritem_");
  colormappropertyscaleaxisticklabelpaddingitem_->setPropertyId(
      "colormappropertyscaleaxisticklabelpaddingitem_");
  colormappropertyscaleaxisticklabelrotationitem_->setPropertyId(
      "colormappropertyscaleaxisticklabelrotationitem_");
  colormappropertyscaleaxisticklabelsideitem_->setPropertyId(
      "colormappropertyscaleaxisticklabelsideitem_");
  colormappropertyscaleaxisticklabelformatitem_->setPropertyId(
      "colormappropertyscaleaxisticklabelformatitem_");
  colormappropertyscaleaxisticklabelprecisionitem_->setPropertyId(
      "colormappropertyscaleaxisticklabelprecisionitem_");

  // ErrorBar2D
  errorbarpropertywhiskerwidthitem_->setPropertyId(
      "errorbarpropertywhiskerwidthitem_");
  errorbarpropertysymbolgaptem_->setPropertyId("errorbarpropertysymbolgaptem_");
  errorbarpropertystrokecoloritem_->setPropertyId(
      "errorbarpropertystrokecoloritem_");
  errorbarpropertystrokethicknessitem_->setPropertyId(
      "errorbarpropertystrokethicknessitem_");
  errorbarpropertystroketypeitem_->setPropertyId(
      "errorbarpropertystroketypeitem_");
  errorbarpropertyantialiaseditem_->setPropertyId(
      "errorbarpropertyantialiaseditem_");
  errorbarpropertyfillstatusitem_->setPropertyId(
      "errorbarpropertyfillstatusitem_");
  errorbarpropertyfillcoloritem_->setPropertyId(
      "errorbarpropertyfillcoloritem_");

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
