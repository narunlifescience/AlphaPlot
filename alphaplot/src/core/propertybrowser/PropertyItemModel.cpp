#include "PropertyItemModel.h"

#include <math.h>

#include <QFont>
#include <QSettings>

#include "../../3rdparty/qcustomplot/qcustomplot.h"
#include "2Dplot/Axis2D.h"
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Channel2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/DataManager2D.h"
#include "2Dplot/GridPair2D.h"
#include "2Dplot/ImageItem2D.h"
#include "2Dplot/Layout2D.h"
#include "2Dplot/Legend2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "2Dplot/TextItem2D.h"
#include "3Dplot/Layout3D.h"
#include "Matrix.h"
#include "MyWidget.h"
#include "ObjectBrowserTreeItem.h"
#include "PropertyItem.h"
#include "QDebug"
#include "Table.h"
#include "core/IconLoader.h"

PropertyItemModel::PropertyItemModel(QObject *parent)
    : QAbstractItemModel(parent) {
  rootItem_ = static_cast<PropertyItem *>(
      PropertyItem::create(nullptr, nullptr, PropertyItem::Property::None));
}

PropertyItemModel::~PropertyItemModel() { delete rootItem_; }

QModelIndex PropertyItemModel::buddy(const QModelIndex &index) const {
  if (index.column() == 1) return index;
  return index.sibling(index.row(), 1);
}

int PropertyItemModel::columnCount(const QModelIndex &parent) const {
  // <property, value>, hence always 2
  if (parent.isValid())
    return static_cast<PropertyItem *>(parent.internalPointer())->columnCount();
  else
    return rootItem_->columnCount();
}

QVariant PropertyItemModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();

  auto item = static_cast<PropertyItem *>(index.internalPointer());
  return item->data(index.column(), role);
}

bool PropertyItemModel::setData(const QModelIndex &index, const QVariant &value,
                                int role) {
  if (!index.isValid()) return false;

  // we check whether the data has really changed, otherwise we ignore it
  if (role == Qt::EditRole) {
    auto item = static_cast<PropertyItem *>(index.internalPointer());
    QVariant data = item->data(index.column(), role);
    if (data.type() == QVariant::Double && value.type() == QVariant::Double) {
      // since we store some properties as floats we get some round-off
      // errors here. Thus, we use an epsilon here.
      // NOTE: Since 0.14 PropertyFloat uses double precision, so this is maybe
      // unnecessary now?
      /*double d = data.toDouble();
      double v = value.toDouble();
      if (std::fabs(d - v) > std::numeric_limits<double>::epsilon())
        return item->setD;*/
    } /*else if (data != value)
      return item->setData(value);*/
  }

  return true;
}

Qt::ItemFlags PropertyItemModel::flags(const QModelIndex &index) const {
  auto item = static_cast<PropertyItem *>(index.internalPointer());
  return item->flags(index.column());
}

QModelIndex PropertyItemModel::index(int row, int column,
                                     const QModelIndex &parent) const {
  PropertyItem *parentItem;

  if (!parent.isValid())
    parentItem = rootItem_;
  else
    parentItem = static_cast<PropertyItem *>(parent.internalPointer());

  PropertyItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex PropertyItemModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();

  PropertyItem *childItem =
      static_cast<PropertyItem *>(index.internalPointer());
  PropertyItem *parentItem = childItem->parent();

  if (parentItem == rootItem_) return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

int PropertyItemModel::rowCount(const QModelIndex &parent) const {
  PropertyItem *parentItem;

  if (!parent.isValid())
    parentItem = rootItem_;
  else
    parentItem = static_cast<PropertyItem *>(parent.internalPointer());

  return parentItem->childCount();
}

QVariant PropertyItemModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const {
  if (orientation == Qt::Horizontal) {
    if (role != Qt::DisplayRole) return QVariant();
    if (section == 0) return tr("Property");
    if (section == 1) return tr("Value");
  }

  return QVariant();
}

bool PropertyItemModel::setHeaderData(int, Qt::Orientation, const QVariant &,
                                      int) {
  return false;
}

void PropertyItemModel::updateProperty(PropertyItem *item) {
  int column = 1;
  int numChild = rootItem_->childCount();
  for (int row = 0; row < numChild; row++) {
    PropertyItem *child = rootItem_->child(row);
    if (child == item) {
      // child->updateData();
      QModelIndex data = this->index(row, column, QModelIndex());
      if (data.isValid()) {
        // child->assignProperty(&prop);
        emit dataChanged(data, data);
        updateChildren(child, column, data);
      }
      break;
    }
  }
}

void PropertyItemModel::buildUp(ObjectBrowserTreeItem *item) {
  QSettings settings;
  settings.beginGroup("General");
  int precision = settings.value("DecimalDigits", 6).toInt();
  settings.endGroup();
  QList<QPair<QIcon, QString>> penstyles;
  penstyles << QPair<QIcon, QString>(
                   IconLoader::load("clear-loginfo", IconLoader::General),
                   tr("None"))
            << QPair<QIcon, QString>(
                   QIcon(":/icons/common/16/edit-style-solidline.png"),
                   tr("Solid Line"))
            << QPair<QIcon, QString>(
                   QIcon(":/icons/common/16/edit-style-dashline.png"),
                   tr("Dash Line"))
            << QPair<QIcon, QString>(
                   QIcon(":/icons/common/16/edit-style-dotline.png"),
                   tr("Dot Line"))
            << QPair<QIcon, QString>(
                   QIcon(":/icons/common/16/edit-style-dashdotline.png"),
                   tr("Dash Dot Line"))
            << QPair<QIcon, QString>(
                   QIcon(":/icons/common/16/edit-style-dashdotdotline.png"),
                   tr("Dash Dot Dot Line"));
  QList<QPair<QIcon, QString>> brushfillstyles;
  brushfillstyles
      << QPair<QIcon, QString>(
             IconLoader::load("clear-loginfo", IconLoader::General), tr("None"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-solid-pattern.png"),
             tr("Solid Color"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-dense1-pattern.png"),
             tr("Dense Point 1"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-dense2-pattern.png"),
             tr("Dense Point 2"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-dense3-pattern.png"),
             tr("Dense Point 3"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-dense4-pattern.png"),
             tr("Dense Point 4"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-dense5-pattern.png"),
             tr("Dense Point 5"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-dense6-pattern.png"),
             tr("Dense Point 6"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-dense7-pattern.png"),
             tr("Dense Point 7"))
      << QPair<QIcon, QString>(QIcon(":/icons/common/16/edit-hor-pattern.png"),
                               tr("Hor Line"))
      << QPair<QIcon, QString>(QIcon(":/icons/common/16/edit-ver-pattern.png"),
                               tr("Ver Line"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-cross-pattern.png"),
             tr("Cross Line"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-bdiag-pattern.png"),
             tr("B-Diag Line"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-fdiag-pattern.png"),
             tr("F-Diag Line"))
      << QPair<QIcon, QString>(
             QIcon(":/icons/common/16/edit-diagcross-pattern.png"),
             tr("Diag Cross Line"));
  QList<QPair<QIcon, QString>> legenddirection;
  legenddirection << QPair<QIcon, QString>(QIcon(), tr("Rows"))
                  << QPair<QIcon, QString>(QIcon(), tr("Columns"));
  QList<QPair<QIcon, QString>> endings;
  endings << QPair<QIcon, QString>(
                 IconLoader::load("clear-loginfo", IconLoader::General),
                 tr("None"))
          << QPair<QIcon, QString>(QIcon(), tr("Flat Arrow"))
          << QPair<QIcon, QString>(QIcon(), tr("Spike Arrow"))
          << QPair<QIcon, QString>(QIcon(), tr("Line Arrow"))
          << QPair<QIcon, QString>(QIcon(), tr("Disc"))
          << QPair<QIcon, QString>(QIcon(), tr("Square"))
          << QPair<QIcon, QString>(QIcon(), tr("Diamond"))
          << QPair<QIcon, QString>(QIcon(), tr("Bar"))
          << QPair<QIcon, QString>(QIcon(), tr("Half Bar"))
          << QPair<QIcon, QString>(QIcon(), tr("Skewed Bar"));
  QList<QPair<QIcon, QString>> axisscaletype;
  axisscaletype << QPair<QIcon, QString>(QIcon(), tr("Linear"))
                << QPair<QIcon, QString>(QIcon(), tr("Logarithmic"));
  QList<QPair<QIcon, QString>> axisticklabelstepstrategy;
  axisticklabelstepstrategy
      << QPair<QIcon, QString>(QIcon(), tr("Readability"))
      << QPair<QIcon, QString>(QIcon(), tr("Meet Tick Count"));
  QList<QPair<QIcon, QString>> axisticklabelside;
  axisticklabelside << QPair<QIcon, QString>(QIcon(), tr("In"))
                    << QPair<QIcon, QString>(QIcon(), tr("Out"));
  QList<QPair<QIcon, QString>> axisformatdouble;
  axisformatdouble
      << QPair<QIcon, QString>(QIcon(), tr("Scientific: 1.250e+01"))
      << QPair<QIcon, QString>(QIcon(), tr("Scientific: 1.250.10^-1"))
      << QPair<QIcon, QString>(QIcon(), tr("Scientific: 1.250x10^-1"))
      << QPair<QIcon, QString>(QIcon(), tr("Scientific: 1.250E+01"))
      << QPair<QIcon, QString>(QIcon(), tr("Decimal: 12.500"))
      << QPair<QIcon, QString>(QIcon(), tr("Automatic(g)"))
      << QPair<QIcon, QString>(QIcon(), tr("Automatic(gb)"))
      << QPair<QIcon, QString>(QIcon(), tr("Automatic(gbc)"))
      << QPair<QIcon, QString>(QIcon(), tr("Automatic(G)"));
  QList<QPair<QIcon, QString>> axissymbolfractionstyle;
  axissymbolfractionstyle
      << QPair<QIcon, QString>(QIcon(), tr("Floating Point"))
      << QPair<QIcon, QString>(QIcon(), tr("ASCII Fractions"))
      << QPair<QIcon, QString>(QIcon(), tr("Unicode Fractions"));
  QList<QPair<QIcon, QString>> textalignmentstyles;
  textalignmentstyles << QPair<QIcon, QString>(QIcon(), tr("Top Left"))
                      << QPair<QIcon, QString>(QIcon(), tr("Top Center"))
                      << QPair<QIcon, QString>(QIcon(), tr("Top Right"))
                      << QPair<QIcon, QString>(QIcon(), tr("Center Left"))
                      << QPair<QIcon, QString>(QIcon(), tr("Center Center"))
                      << QPair<QIcon, QString>(QIcon(), tr("Center Right"))
                      << QPair<QIcon, QString>(QIcon(), tr("Bottom Left"))
                      << QPair<QIcon, QString>(QIcon(), tr("Bottom Center"))
                      << QPair<QIcon, QString>(QIcon(), tr("Bottom Right"));
  QList<QPair<QIcon, QString>> linestylescurve;
  linestylescurve << QPair<QIcon, QString>(
                         IconLoader::load("clear-loginfo", IconLoader::General),
                         tr("None"))
                  << QPair<QIcon, QString>(
                         QIcon(":/icons/common/16/edit-style-solidline.png"),
                         tr("Line"));
  QList<QPair<QIcon, QString>> linestylesls;
  linestylesls << QPair<QIcon, QString>(QIcon(), tr("Step Left"))
               << QPair<QIcon, QString>(QIcon(), tr("Step Right"))
               << QPair<QIcon, QString>(QIcon(), tr("Step Center"))
               << QPair<QIcon, QString>(QIcon(), tr("Impulse"))
               << QPair<QIcon, QString>(
                      QIcon(":/icons/common/16/edit-style-solidline.png"),
                      tr("Line"));
  QList<QPair<QIcon, QString>> scatterstyles;
  scatterstyles << QPair<QIcon, QString>(
                       IconLoader::load("clear-loginfo", IconLoader::General),
                       tr("None"))
                << QPair<QIcon, QString>(QIcon(), tr("Dot"))
                << QPair<QIcon, QString>(QIcon(), tr("Cross"))
                << QPair<QIcon, QString>(QIcon(), tr("Plus"))
                << QPair<QIcon, QString>(QIcon(), tr("Circle"))
                << QPair<QIcon, QString>(QIcon(), tr("Disc"))
                << QPair<QIcon, QString>(QIcon(), tr("Square"))
                << QPair<QIcon, QString>(QIcon(), tr("Diamond"))
                << QPair<QIcon, QString>(QIcon(), tr("Star"))
                << QPair<QIcon, QString>(QIcon(), tr("Triangle"))
                << QPair<QIcon, QString>(QIcon(), tr("Triangle Inverted"))
                << QPair<QIcon, QString>(QIcon(), tr("Cross Square"))
                << QPair<QIcon, QString>(QIcon(), tr("Plus Square"))
                << QPair<QIcon, QString>(QIcon(), tr("Cross Circle"))
                << QPair<QIcon, QString>(QIcon(), tr("Plus Circle"))
                << QPair<QIcon, QString>(QIcon(), tr("Peace"));
  beginResetModel();
  delete rootItem_;
  rootItem_ = static_cast<PropertyItem *>(
      PropertyItem::create(nullptr, nullptr, PropertyItem::Property::None));
  // fill up the listview with the properties
  switch (item->dataType()) {
    // MyWidget
    case ObjectBrowserTreeItem::ObjectType::None:
    case ObjectBrowserTreeItem::ObjectType::TableWindow:
    case ObjectBrowserTreeItem::ObjectType::MatrixWindow:
    case ObjectBrowserTreeItem::ObjectType::NoteWindow:
    case ObjectBrowserTreeItem::ObjectType::Plot2DWindow:
    case ObjectBrowserTreeItem::ObjectType::Plot3DWindow:
      break;
    case ObjectBrowserTreeItem::ObjectType::BaseWindow: {
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::BaseWindow_Base_Separator);
      auto name = PropertyItem::create(rootItem_, item,
                                       PropertyItem::Property::BaseWindow_Name);
      auto label = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::BaseWindow_Label);
      auto geom = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::BaseWindow_Geometry);
      PropertyItem::create(geom, item,
                           PropertyItem::Property::BaseWindow_Geometry_X);
      PropertyItem::create(geom, item,
                           PropertyItem::Property::BaseWindow_Geometry_Y);
      auto bw = PropertyItem::create(
          geom, item, PropertyItem::Property::BaseWindow_Geometry_Width);
      bw->setLowerLimitInt(0);
      auto bh = PropertyItem::create(
          geom, item, PropertyItem::Property::BaseWindow_Geometry_Height);
      bh->setLowerLimitInt(0);
      // instant update
      bool status = false;
      MyWidget *widget = item->getObjectTreeItem<MyWidget>(&status);
      if (status) {
        connect(widget, &MyWidget::geometrychange, this,
                [=]() { updateProperty(geom); });
        connect(name, &PropertyItem::namechange, this,
                &PropertyItemModel::namechange);
        connect(label, &PropertyItem::labelchange, this,
                &PropertyItemModel::labelchange);
      }
    } break;
    // Table
    case ObjectBrowserTreeItem::ObjectType::TableDimension: {
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::TableWindow_Base_Separator);
      auto row = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::TableWindow_Row_Count);
      auto column = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::TableWindow_Column_Count);
      bool status = false;
      Table *table = item->getObjectTreeItem<Table>(&status);
      if (status) {
        connect(table, &Table::rowcountchange, this,
                [=]() { updateProperty(row); });
        connect(table, &Table::columncountchange, this,
                [=]() { updateProperty(column); });
      }
    } break;
    // Matrix
    case ObjectBrowserTreeItem::ObjectType::MatrixDimension: {
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::MatrixWindow_Base_Separator);
      auto row = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::MatrixWindow_Row_Count);
      auto column = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::MatrixWindow_Column_Count);
      bool status = false;
      Matrix *matrix = item->getObjectTreeItem<Matrix>(&status);
      if (status) {
        connect(matrix, &Matrix::rowcountchange, this,
                [=]() { updateProperty(row); });
        connect(matrix, &Matrix::columncountchange, this,
                [=]() { updateProperty(column); });
      }
    } break;
    // Plot2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DCanvas: {
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCanvas_Base_Separator);
      auto dim = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DCanvas_Dimension);
      auto pcw = PropertyItem::create(
          dim, item, PropertyItem::Property::Plot2DCanvas_Dimension_Width);
      pcw->setLowerLimitInt(0);
      auto pch = PropertyItem::create(
          dim, item, PropertyItem::Property::Plot2DCanvas_Dimension_Height);
      pch->setLowerLimitInt(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCanvas_Background);
      auto dpr = PropertyItem::create(rootItem_, item,
                                      PropertyItem::Property::Plot2DCanvas_DPR);
      dpr->setPrecision(4);
      dpr->setSingleStepDouble(0.1);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCanvas_OpenGL);
      auto pcrs = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DCanvas_Row_Spacing);
      pcrs->setLowerLimitInt(0);
      auto pccs = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DCanvas_Column_Spacing);
      pccs->setLowerLimitInt(0);
      // instant update
      bool status = false;
      Layout2D *plot = item->parentItem()->getObjectTreeItem<Layout2D>(&status);
      if (status)
        connect(plot, &Layout2D::geometrychange, this,
                [=]() { updateProperty(dim); });
    } break;
    // AxisRect2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DLayout: {
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLayout_Base_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLayout_Background);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLayout_FillStyle,
                           brushfillstyles);
      auto rsf = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLayout_Row_Stretch_Factor);
      rsf->setPrecision(4);
      rsf->setSingleStepDouble(0.01);
      auto csf = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLayout_Column_Stretch_Factor);
      csf->setPrecision(4);
      csf->setSingleStepDouble(0.01);
      auto rect = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLayout_Outer_Rect);
      auto rectx = PropertyItem::create(
          rect, item, PropertyItem::Property::Plot2DLayout_Outer_Rect_X);
      rectx->setReadonly(true);
      auto recty = PropertyItem::create(
          rect, item, PropertyItem::Property::Plot2DLayout_Outer_Rect_Y);
      recty->setReadonly(true);
      auto rectw = PropertyItem::create(
          rect, item, PropertyItem::Property::Plot2DLayout_Outer_Rect_Width);
      rectw->setReadonly(true);
      auto recth = PropertyItem::create(
          rect, item, PropertyItem::Property::Plot2DLayout_Outer_Rect_Height);
      recth->setReadonly(true);
      auto mar = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLayout_Margin);
      auto marl = PropertyItem::create(
          mar, item, PropertyItem::Property::Plot2DLayout_Margin_Left);
      auto marb = PropertyItem::create(
          mar, item, PropertyItem::Property::Plot2DLayout_Margin_Bottom);
      auto marr = PropertyItem::create(
          mar, item, PropertyItem::Property::Plot2DLayout_Margin_Right);
      auto mart = PropertyItem::create(
          mar, item, PropertyItem::Property::Plot2DLayout_Margin_Top);
      // instant update
      bool status = false;
      AxisRect2D *axisrect = item->getObjectTreeItem<AxisRect2D>(&status);
      if (status) {
        marl->setReadonly(axisrect->getAutoMarginsBool());
        marr->setReadonly(axisrect->getAutoMarginsBool());
        mart->setReadonly(axisrect->getAutoMarginsBool());
        marb->setReadonly(axisrect->getAutoMarginsBool());
        connect(marl, &PropertyItem::readonlyChanged, this,
                &PropertyItemModel::repaintViewer);
        connect(marr, &PropertyItem::readonlyChanged, this,
                &PropertyItemModel::repaintViewer);
        connect(mart, &PropertyItem::readonlyChanged, this,
                &PropertyItemModel::repaintViewer);
        connect(marb, &PropertyItem::readonlyChanged, this,
                &PropertyItemModel::repaintViewer);
      }
      Layout2D *plot = item->parentItem()->getObjectTreeItem<Layout2D>(&status);
      if (status)
        connect(plot, &Layout2D::geometrychange, this,
                [=]() { updateProperty(rect); });
    } break;
    // Legend 2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DLegend: {
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Base_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Visible);
      auto posx = PropertyItem::create(rootItem_, item,
                                       PropertyItem::Property::Plot2DLegend_X);
      posx->setPrecision(4);
      posx->setSingleStepDouble(0.01);
      auto posy = PropertyItem::create(rootItem_, item,
                                       PropertyItem::Property::Plot2DLegend_Y);
      posy->setPrecision(4);
      posy->setSingleStepDouble(0.01);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Direction,
                           legenddirection);
      auto margin = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLegend_Margin);
      PropertyItem::create(margin, item,
                           PropertyItem::Property::Plot2DLegend_Margin_Left);
      PropertyItem::create(margin, item,
                           PropertyItem::Property::Plot2DLegend_Margin_Bottom);
      PropertyItem::create(margin, item,
                           PropertyItem::Property::Plot2DLegend_Margin_Right);
      PropertyItem::create(margin, item,
                           PropertyItem::Property::Plot2DLegend_Margin_Top);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLegend_Stroke_Fill_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Stroke_Color);
      auto sthickness = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLegend_Stroke_Thickness);
      sthickness->setPrecision(4);
      sthickness->setSingleStepDouble(0.1);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Stroke_Style,
                           penstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Background);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_FillStyle,
                           brushfillstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Text_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Font);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Text_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Icon_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Icon_Width);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Icon_Height);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLegend_Icon_Padding);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLegend_Title_Separator);
      auto title = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLegend_Title_Visible);
      auto titletext = PropertyItem::create(
          title, item, PropertyItem::Property::Plot2DLegend_Title_Text);
      auto titlefont = PropertyItem::create(
          title, item, PropertyItem::Property::Plot2DLegend_Title_Font);
      auto titlecolor = PropertyItem::create(
          title, item, PropertyItem::Property::Plot2DLegend_Title_Color);
      // instant update
      bool status = false;
      Legend2D *legend = item->getObjectTreeItem<Legend2D>(&status);
      if (status)
        connect(legend, &Legend2D::legendMoved, this, [=]() {
          updateProperty(posx);
          updateProperty(posy);
        });
      titletext->setReadonly(!legend->istitle_legend());
      titlefont->setReadonly(!legend->istitle_legend());
      titlecolor->setReadonly(!legend->istitle_legend());
      connect(titletext, &PropertyItem::readonlyChanged, this,
              &PropertyItemModel::repaintViewer);
      connect(titlefont, &PropertyItem::readonlyChanged, this,
              &PropertyItemModel::repaintViewer);
      connect(titlecolor, &PropertyItem::readonlyChanged, this,
              &PropertyItemModel::repaintViewer);
    } break;
      // Axis 2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DAxis: {
      bool status = false;
      Axis2D *axis = item->getObjectTreeItem<Axis2D>(&status);
      if (!status) {
        qDebug() << "unable to getTreeObjectItem Axis2D pointer";
        return;
      }
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Base_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Visible);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Offset);
      if (axis->gettickertype_axis() == Axis2D::TickerType::Value ||
          axis->gettickertype_axis() == Axis2D::TickerType::Log ||
          axis->gettickertype_axis() == Axis2D::TickerType::Pi) {
        auto from = PropertyItem::create(
            rootItem_, item, PropertyItem::Property::Plot2DAxis_From_Double);
        from->setPrecision(precision);
        auto to = PropertyItem::create(
            rootItem_, item, PropertyItem::Property::Plot2DAxis_To_Double);
        to->setPrecision(precision);
        from->setLowerLimitDouble(-QCPRange::maxRange);
        from->setUpperLimitDouble(QCPRange::maxRange);
        to->setLowerLimitDouble(-QCPRange::maxRange);
        to->setUpperLimitDouble(QCPRange::maxRange);
        // instant update
        connect(axis, &Axis2D::rescaleAxis2D, this, [=]() {
          updateProperty(from);
          updateProperty(to);
        });
      } else if (axis->gettickertype_axis() == Axis2D::TickerType::DateTime) {
        QString dtformat =
            static_cast<QCPAxisTickerDateTime *>(axis->getticker_axis().get())
                ->dateTimeFormat();
        auto from = PropertyItem::create(
            rootItem_, item, PropertyItem::Property::Plot2DAxis_From_DateTime);
        from->setDateTimeFormat(dtformat);
        auto to = PropertyItem::create(
            rootItem_, item, PropertyItem::Property::Plot2DAxis_To_DateTime);
        to->setDateTimeFormat(dtformat);
        connect(axis, &Axis2D::rescaleAxis2D, this, [=]() {
          updateProperty(from);
          updateProperty(to);
        });
      } else if (axis->gettickertype_axis() == Axis2D::TickerType::Text) {
        auto from = PropertyItem::create(
            rootItem_, item, PropertyItem::Property::Plot2DAxis_From_Text);
        from->setPrecision(2);
        auto to = PropertyItem::create(
            rootItem_, item, PropertyItem::Property::Plot2DAxis_To_Text);
        to->setPrecision(2);
        connect(axis, &Axis2D::rescaleAxis2D, this, [=]() {
          updateProperty(from);
          updateProperty(to);
        });
      }
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Type,
                           axisscaletype);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Inverted);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Stroke_Color);
      auto strokethick = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DAxis_Stroke_Thickness);
      strokethick->setPrecision(4);
      strokethick->setLowerLimitDouble(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Stroke_Style,
                           penstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Ending_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Upper_Ending,
                           endings);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Lower_Ending,
                           endings);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Label_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Label_Text);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Label_Font);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Label_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Label_Padding);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Ticks_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Ticks_Visible);
      auto tickcount = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DAxis_Ticks_Count);
      tickcount->setLowerLimitInt(1);
      auto tickorigin = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DAxis_Ticks_Origin);
      tickorigin->setPrecision(4);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Ticks_LengthIn);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_Ticks_LengthOut);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Color);
      auto tickstrokethickness = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Thickness);
      tickstrokethickness->setPrecision(4);
      tickstrokethickness->setLowerLimitDouble(0);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_Ticks_Stroke_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_SubTicks_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_SubTicks_Visible);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_SubTicks_LengthIn);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_SubTicks_LengthOut);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Color);
      auto subtickstrokethickness = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Thickness);
      subtickstrokethickness->setPrecision(4);
      subtickstrokethickness->setLowerLimitDouble(0);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_SubTicks_Stroke_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_TickLabel_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_TickLabel_Visible);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_TickLabel_StepStrategy,
          axisticklabelstepstrategy);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_TickLabel_Font);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_TickLabel_Color);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_TickLabel_Padding);
      auto ticklabelrot = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DAxis_TickLabel_Rotation);
      ticklabelrot->setPrecision(2);
      ticklabelrot->setLowerLimitDouble(0);
      ticklabelrot->setUpperLimitDouble(360);
      ticklabelrot->setSuffix(" °");
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DAxis_TickLabel_Side,
                           axisticklabelside);
      if (axis->gettickertype_axis() == Axis2D::TickerType::Log) {
        auto logbase = PropertyItem::create(
            rootItem_, item,
            PropertyItem::Property::Plot2DAxis_TickLabel_LogBase_Log);
        logbase->setPrecision(4);
        logbase->setLowerLimitDouble(0);
      }
      if (axis->gettickertype_axis() == Axis2D::TickerType::Pi) {
        PropertyItem::create(
            rootItem_, item,
            PropertyItem::Property::Plot2DAxis_TickLabel_Symbol_Pi);
        auto symbolvalue = PropertyItem::create(
            rootItem_, item,
            PropertyItem::Property::Plot2DAxis_TickLabel_Symbol_Value_Pi);
        symbolvalue->setPrecision(4);
        PropertyItem::create(
            rootItem_, item,
            PropertyItem::Property::Plot2DAxis_TickLabel_FractionStyle_Pi,
            axissymbolfractionstyle);
      }
      if (axis->gettickertype_axis() == Axis2D::TickerType::Value ||
          axis->gettickertype_axis() == Axis2D::TickerType::Log ||
          axis->gettickertype_axis() == Axis2D::TickerType::Pi) {
        PropertyItem::create(
            rootItem_, item,
            PropertyItem::Property::Plot2DAxis_TickLabel_Format_Double,
            axisformatdouble);
        PropertyItem::create(
            rootItem_, item,
            PropertyItem::Property::Plot2DAxis_TickLabel_Precision_Double);
      } else if (axis->gettickertype_axis() == Axis2D::TickerType::DateTime) {
        PropertyItem::create(
            rootItem_, item,
            PropertyItem::Property::Plot2DAxis_TickLabel_Format_DateTime);
      }
    } break;
    // Grid2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DGrid: {
      bool status = false;
      GridPair2D *gpair = item->getObjectTreeItem<GridPair2D>(&status);
      if (!status) {
        qDebug() << "unable to getTreeObjectItem GridPair2D pointer";
        return;
      }
      if (!gpair->getXgridAxis() || !gpair->getXgridAxis()) {
        qDebug() << "grid X or Y axis is null";
        return;
      }
      QList<QPair<QIcon, QString>> xaxislist =
          getXaxislist(gpair->getXgridAxis()->getaxisrect_axis());
      QList<QPair<QIcon, QString>> yaxislist =
          getYaxislist(gpair->getYgridAxis()->getaxisrect_axis());
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DGrid_Horizontal,
                           xaxislist);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Major_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DGrid_Horizontal_Major);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Color);
      auto hormajthick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Thickness);
      hormajthick->setLowerLimitDouble(0);
      hormajthick->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Major_Stroke_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Line);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Color);
      auto horzerothick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Thickness);
      horzerothick->setLowerLimitDouble(0);
      horzerothick->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Zero_Stroke_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DGrid_Horizontal_Minor);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Color);
      auto horminthick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Thickness);
      horminthick->setLowerLimitDouble(0);
      horminthick->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Horizontal_Minor_Stroke_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DGrid_Vertical,
                           yaxislist);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Major_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DGrid_Vertical_Major);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Color);
      auto vermajthick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Thickness);
      vermajthick->setLowerLimitDouble(0);
      vermajthick->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Major_Stroke_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Zero_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Zero_Line);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Color);
      auto verzerothick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Thickness);
      verzerothick->setLowerLimitDouble(0);
      verzerothick->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Zero_Stroke_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Minor_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DGrid_Vertical_Minor);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Color);
      auto verminthick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Thickness);
      verminthick->setLowerLimitDouble(0);
      verminthick->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DGrid_Vertical_Minor_Stroke_Style,
          penstyles);
    } break;
    // TextItem2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DTextItem: {
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DTextItem_Base_Separator);
      auto x = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DTextItem_Position_X);
      x->setPrecision(4);
      auto y = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DTextItem_Position_Y);
      y->setPrecision(4);
      auto margin = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DTextItem_Margin);
      PropertyItem::create(margin, item,
                           PropertyItem::Property::Plot2DTextItem_Margin_Left);
      PropertyItem::create(
          margin, item, PropertyItem::Property::Plot2DTextItem_Margin_Bottom);
      PropertyItem::create(margin, item,
                           PropertyItem::Property::Plot2DTextItem_Margin_Right);
      PropertyItem::create(margin, item,
                           PropertyItem::Property::Plot2DTextItem_Margin_Top);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DTextItem_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DTextItem_Stroke_Color);
      auto strokethickness = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DTextItem_Stroke_Thickness);
      strokethickness->setPrecision(4);
      strokethickness->setLowerLimitDouble(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DTextItem_Stroke_Style,
                           penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DTextItem_Background_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DTextItem_Fill_Style,
                           brushfillstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DTextItem_Text_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DTextItem_Text_Alignment,
          textalignmentstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DTextItem_Text);
      auto textrotation = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DTextItem_Text_Rotation);
      textrotation->setPrecision(2);
      textrotation->setSuffix(" °");
      textrotation->setLowerLimitDouble(-360);
      textrotation->setUpperLimitDouble(360);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DTextItem_Text_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DTextItem_Text_Font);
      // instant update
      bool status = false;
      TextItem2D *textitem = item->getObjectTreeItem<TextItem2D>(&status);
      if (status) {
        connect(textitem, &TextItem2D::positionChanged, this, [=]() {
          updateProperty(x);
          updateProperty(y);
        });
      }
    } break;
    // LineItem2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DLineItem: {
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLineItem_Base_Separator);
      auto x1 = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLineItem_Position_X1);
      x1->setPrecision(4);
      auto y1 = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLineItem_Position_Y1);
      y1->setPrecision(4);
      auto x2 = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLineItem_Position_X2);
      x2->setPrecision(4);
      auto y2 = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLineItem_Position_Y2);
      y2->setPrecision(4);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLineItem_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLineItem_Stroke_Color);
      auto strokethickness = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLineItem_Stroke_Thickness);
      strokethickness->setPrecision(4);
      strokethickness->setLowerLimitDouble(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLineItem_Stroke_Style,
                           penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLineItem_End_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLineItem_Starting_Style, endings);
      auto startingwidth = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLineItem_Starting_Width);
      startingwidth->setPrecision(4);
      startingwidth->setLowerLimitDouble(0);
      auto startingheight = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLineItem_Starting_Height);
      startingheight->setPrecision(4);
      startingheight->setLowerLimitDouble(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLineItem_Ending_Style,
                           endings);
      auto endingwidth = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLineItem_Ending_Width);
      endingwidth->setPrecision(4);
      endingwidth->setLowerLimitDouble(0);
      auto endingheight = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLineItem_Ending_Height);
      endingheight->setPrecision(4);
      endingheight->setLowerLimitDouble(0);
    } break;
    // ImageItem2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DImageItem: {
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DImageItem_Base_Separator);
      auto x = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DImageItem_Position_X);
      x->setPrecision(4);
      auto y = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DImageItem_Position_Y);
      y->setPrecision(4);
      auto source = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DImageItem_Source);
      source->setReadonly(true);
      auto rotation = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DImageItem_Rotation);
      rotation->setPrecision(2);
      rotation->setLowerLimitDouble(-360);
      rotation->setUpperLimitDouble(360);
      rotation->setSuffix(" °");
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DImageItem_Stroke_Color);
      auto strokethickness = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DImageItem_Stroke_Thickness);
      strokethickness->setPrecision(4);
      strokethickness->setLowerLimitDouble(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DImageItem_Stroke_Style,
                           penstyles);
      // instant update
      bool status = false;
      ImageItem2D *imageitem = item->getObjectTreeItem<ImageItem2D>(&status);
      if (status) {
        connect(imageitem, &ImageItem2D::positionChanged, this, [=]() {
          updateProperty(x);
          updateProperty(y);
        });
      }
    } break;
    // Curve2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DCurve: {
      bool status = false;
      Curve2D *curve = item->getObjectTreeItem<Curve2D>(&status);
      if (!status) {
        qDebug() << "unable to getTreeObjectItem Curve2D pointer";
        return;
      }
      QList<QPair<QIcon, QString>> xaxislist =
          getXaxislist(curve->getxaxis()->getaxisrect_axis());
      QList<QPair<QIcon, QString>> yaxislist =
          getYaxislist(curve->getxaxis()->getaxisrect_axis());
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Axis_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Axis_X,
                           xaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Axis_Y,
                           yaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Line_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Line_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Line_Style,
                           linestylescurve);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Line_Stroke_Color);
      auto lstroket = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Line_Stroke_Thickness);
      lstroket->setLowerLimitDouble(0);
      lstroket->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Line_Stroke_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Areafill_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Fill_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Fill_Style,
                           brushfillstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Scatter_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Scatter_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Scatter_Style,
                           scatterstyles);
      auto scattersize = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DCurve_Scatter_Size);
      scattersize->setLowerLimitDouble(0);
      scattersize->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Scatter_Outline_Color);
      auto scatterstrokethick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Scatter_Outline_Thickness);
      scatterstrokethick->setLowerLimitDouble(0);
      scatterstrokethick->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Scatter_Outline_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Scatter_Fill_Color);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Scatter_Fill_Style,
          brushfillstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCurve_Legend_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Legend_Status);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCurve_Legend_Text);
    } break;
    // Graph2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DLSGraph: {
      bool status = false;
      LineSpecial2D *ls = item->getObjectTreeItem<LineSpecial2D>(&status);
      if (!status) {
        qDebug() << "unable to getTreeObjectItem LineSpecial2D pointer";
        return;
      }
      QList<QPair<QIcon, QString>> xaxislist =
          getXaxislist(ls->getxaxis()->getaxisrect_axis());
      QList<QPair<QIcon, QString>> yaxislist =
          getYaxislist(ls->getxaxis()->getaxisrect_axis());
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Axis_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Axis_X, xaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Axis_Y, yaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Line_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Line_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Line_Style,
                           linestylesls);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Line_Stroke_Color);
      auto lstroket = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLS_Line_Stroke_Thickness);
      lstroket->setLowerLimitDouble(0);
      lstroket->setPrecision(4);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Line_Stroke_Style,
                           penstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Areafill_Separator);
      // PropertyItem::create(rootItem_, item,
      //                      PropertyItem::Property::Plot2DLS_Fill_Area);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Fill_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Fill_Style,
                           brushfillstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Scatter_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLS_Scatter_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Scatter_Style,
                           scatterstyles);
      auto scattersize = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DLS_Scatter_Size);
      scattersize->setLowerLimitDouble(0);
      scattersize->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLS_Scatter_Outline_Color);
      auto scatterstrokethick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLS_Scatter_Outline_Thickness);
      scatterstrokethick->setLowerLimitDouble(0);
      scatterstrokethick->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DLS_Scatter_Outline_Style, penstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Scatter_Fill_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Scatter_Fill_Style,
                           brushfillstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Legend_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Legend_Status);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DLS_Legend_Text);
    } break;
    // Channel2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DChannelGraph: {
      bool status = false;
      Channel2D *channel = item->getObjectTreeItem<Channel2D>(&status);
      if (!status) {
        qDebug() << "unable to getTreeObjectItem Channel2D pointer";
        return;
      }
      QList<QPair<QIcon, QString>> xaxislist = getXaxislist(
          channel->getChannelFirst()->getxaxis()->getaxisrect_axis());
      QList<QPair<QIcon, QString>> yaxislist = getYaxislist(
          channel->getChannelFirst()->getxaxis()->getaxisrect_axis());
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Axis_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Axis_X,
                           xaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Axis_Y,
                           yaxislist);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line1_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line1_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Line1_Style,
                           linestylesls);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line1_Stroke_Color);
      auto lstroket1 = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line1_Stroke_Thickness);
      lstroket1->setLowerLimitDouble(0);
      lstroket1->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line1_Stroke_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter1_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter1_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Scatter1_Style,
                           scatterstyles);
      auto scattersize1 = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DChannel_Scatter1_Size);
      scattersize1->setLowerLimitDouble(0);
      scattersize1->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Color);
      auto scatterstrokethick1 = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Thickness);
      scatterstrokethick1->setLowerLimitDouble(0);
      scatterstrokethick1->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter1_Outline_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter1_Fill_Color);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter1_Fill_Style,
          brushfillstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Areafill_Separator);
      // PropertyItem::create(rootItem_, item,
      //                      PropertyItem::Property::Plot2DChannel_Fill_Area);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Fill_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Fill_Style,
                           brushfillstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line2_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line2_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Line2_Style,
                           linestylesls);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line2_Stroke_Color);
      auto lstroket2 = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line2_Stroke_Thickness);
      lstroket2->setLowerLimitDouble(0);
      lstroket2->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Line2_Stroke_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter2_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter2_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Scatter2_Style,
                           scatterstyles);
      auto scattersize2 = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DChannel_Scatter2_Size);
      scattersize2->setLowerLimitDouble(0);
      scattersize2->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Color);
      auto scatterstrokethick2 = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Thickness);
      scatterstrokethick2->setLowerLimitDouble(0);
      scatterstrokethick2->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter2_Outline_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter2_Fill_Color);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Scatter2_Fill_Style,
          brushfillstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DChannel_Legend_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Legend_Status);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DChannel_Legend_Text);
    } break;
    // StatBox2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DStatBox: {
      bool status = false;
      StatBox2D *sb = item->getObjectTreeItem<StatBox2D>(&status);
      if (!status) {
        qDebug() << "unable to getTreeObjectItem StatBox2D pointer";
        return;
      }
      QList<QPair<QIcon, QString>> xaxislist =
          getXaxislist(sb->getxaxis()->getaxisrect_axis());
      QList<QPair<QIcon, QString>> yaxislist =
          getYaxislist(sb->getxaxis()->getaxisrect_axis());
      QList<QPair<QIcon, QString>> boxwhiskerstyles;
      boxwhiskerstyles << QPair<QIcon, QString>(QIcon(), tr("SD"))
                       << QPair<QIcon, QString>(QIcon(), tr("SE"))
                       << QPair<QIcon, QString>(QIcon(), tr("Perc_25_75"))
                       << QPair<QIcon, QString>(QIcon(), tr("Perc_10_90"))
                       << QPair<QIcon, QString>(QIcon(), tr("Perc_5_95"))
                       << QPair<QIcon, QString>(QIcon(), tr("Perc_1_99"))
                       << QPair<QIcon, QString>(QIcon(), tr("Min Max"))
                       << QPair<QIcon, QString>(QIcon(), tr("Constant"))
                       << QPair<QIcon, QString>(QIcon(), tr("IQR_1_5_auto"));
      QList<QPair<QIcon, QString>> outliers;
      outliers << QPair<QIcon, QString>(QIcon(), tr("Outlier"))
               << QPair<QIcon, QString>(QIcon(), tr("All"))
               << QPair<QIcon, QString>(QIcon(), tr("Min Max"))
               << QPair<QIcon, QString>(QIcon(), tr("None"));
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Axis_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DStatBox_Axis_X,
                           xaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DStatBox_Axis_Y,
                           yaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DStatBox_Box_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Box_Antialiased);
      auto boxwidth = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DStatBox_Box_Width);
      boxwidth->setLowerLimitDouble(0);
      boxwidth->setPrecision(4);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DStatBox_Box_Style,
                           boxwhiskerstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Box_Stroke_Color);
      auto boxst = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Box_Stroke_Thickness);
      boxst->setLowerLimitDouble(0);
      boxst->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Box_Stroke_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Box_Fill_Antialiased);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Box_Fill_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DStatBox_Box_Fill_Style,
                           brushfillstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Whisker_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Whisker_Antialiased);
      auto ww = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DStatBox_Whisker_Width);
      ww->setLowerLimitDouble(0);
      ww->setPrecision(4);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DStatBox_Whisker_Style,
                           boxwhiskerstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Color);
      auto whiskerst = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Thickness);
      whiskerst->setLowerLimitDouble(0);
      whiskerst->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Whisker_Stroke_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_WhiskerBar_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Color);
      auto whiskerbarst = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Thickness);
      whiskerbarst->setLowerLimitDouble(0);
      whiskerbarst->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_WhiskerBar_Stroke_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Median_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Median_Stroke_Color);
      auto medianst = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Median_Stroke_Thickness);
      medianst->setLowerLimitDouble(0);
      medianst->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Median_Stroke_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Scatter_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Scatter_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DStatBox_Scatter_Show,
                           outliers);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DStatBox_Scatter_Style,
                           scatterstyles);
      auto scattersize = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DStatBox_Scatter_Size);
      scattersize->setLowerLimitDouble(0);
      scattersize->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Color);
      auto scatterstrokethick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Thickness);
      scatterstrokethick->setLowerLimitDouble(0);
      scatterstrokethick->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Scatter_Outline_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Scatter_Fill_Color);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Scatter_Fill_Style,
          brushfillstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DStatBox_Ticktext_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DStatBox_Tick_Text);
    } break;
    // Vector2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DVector: {
      bool status = false;
      Vector2D *vec = item->getObjectTreeItem<Vector2D>(&status);
      if (!status) {
        qDebug() << "unable to getTreeObjectItem Vector2D pointer";
        return;
      }
      QList<QPair<QIcon, QString>> xaxislist =
          getXaxislist(vec->getxaxis()->getaxisrect_axis());
      QList<QPair<QIcon, QString>> yaxislist =
          getYaxislist(vec->getxaxis()->getaxisrect_axis());
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DVector_Axis_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DVector_Axis_X,
                           xaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DVector_Axis_Y,
                           yaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DVector_Line_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DVector_Line_Antialiased);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DVector_Line_Stroke_Color);
      auto lstroket = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DVector_Line_Stroke_Thickness);
      lstroket->setLowerLimitDouble(0);
      lstroket->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DVector_Line_Stroke_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DVector_Ending_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DVector_Ending_Style,
                           endings);
      auto width = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DVector_Ending_Width);
      width->setLowerLimitDouble(0);
      width->setPrecision(4);
      auto height = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DVector_Ending_Height);
      height->setLowerLimitDouble(0);
      height->setPrecision(4);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DVector_Legend_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DVector_Legend_Status);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DVector_Legend_Text);
    } break;
    // Bar2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DBarGraph: {
      bool status = false;
      Bar2D *bar = item->getObjectTreeItem<Bar2D>(&status);
      if (!status) {
        qDebug() << "unable to getTreeObjectItem Bar2D pointer";
        return;
      }
      QList<QPair<QIcon, QString>> xaxislist =
          getXaxislist(bar->getxaxis()->getaxisrect_axis());
      QList<QPair<QIcon, QString>> yaxislist =
          getYaxislist(bar->getxaxis()->getaxisrect_axis());
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Axis_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Axis_X, xaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Axis_Y, yaxislist);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Bar_Separator);
      auto bwidth = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DBar_Width);
      bwidth->setLowerLimitDouble(0);
      bwidth->setPrecision(4);
      if (bar->getBarStyle() != Bar2D::BarStyle::Individual) {
        auto gap = PropertyItem::create(rootItem_, item,
                                        PropertyItem::Property::Plot2DBar_Gap);
        gap->setLowerLimitDouble(0);
        gap->setPrecision(4);
      }
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Line_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Line_Stroke_Color);
      auto linethickness = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DBar_Line_Stroke_Thickness);
      linethickness->setLowerLimitDouble(0);
      linethickness->setPrecision(4);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Line_Stroke_Style,
                           penstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Fill_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Fill_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Fill_Style,
                           brushfillstyles);
      if (bar->ishistogram_barplot()) {
        PropertyItem::create(
            rootItem_, item,
            PropertyItem::Property::Plot2DBar_Histogram_Separator);
        auto bin = PropertyItem::create(
            rootItem_, item,
            PropertyItem::Property::Plot2DBar_Histogram_AutoBin);
        auto binsize = PropertyItem::create(
            bin, item, PropertyItem::Property::Plot2DBar_Histogram_BinSize);
        binsize->setLowerLimitDouble(0);
        binsize->setPrecision(12);
        auto begin = PropertyItem::create(
            bin, item, PropertyItem::Property::Plot2DBar_Histogram_Begin);
        begin->setPrecision(12);
        auto end = PropertyItem::create(
            bin, item, PropertyItem::Property::Plot2DBar_Histogram_End);
        end->setPrecision(12);
        // instant update
        binsize->setReadonly(bar->getdatablock_histplot()->getautobin());
        begin->setReadonly(bar->getdatablock_histplot()->getautobin());
        end->setReadonly(bar->getdatablock_histplot()->getautobin());
        connect(binsize, &PropertyItem::readonlyChanged, this,
                &PropertyItemModel::repaintViewer);
        connect(begin, &PropertyItem::readonlyChanged, this,
                &PropertyItemModel::repaintViewer);
        connect(end, &PropertyItem::readonlyChanged, this,
                &PropertyItemModel::repaintViewer);
      }
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Legend_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Legend_Status);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DBar_Legend_Text);
    } break;
    // Pie2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DPieGraph: {
      QList<QPair<QIcon, QString>> piestyles;
      piestyles << QPair<QIcon, QString>(QIcon(), tr("Pie"))
                << QPair<QIcon, QString>(QIcon(), tr("Half Pie"));
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DPie_Basic_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DPie_Style, piestyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DPie_Stroke_Color);
      auto linethickness = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DPie_Stroke_Thickness);
      linethickness->setLowerLimitDouble(0);
      linethickness->setPrecision(4);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DPie_Stroke_Style,
                           penstyles);
      auto margin = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DPie_Margin_Percent);
      margin->setLowerLimitInt(0);
      margin->setUpperLimitInt(100);
      margin->setSuffix(" %");
    } break;
    // ColorMap2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DColorMap: {
      QList<QPair<QIcon, QString>> grad;
      grad << QPair<QIcon, QString>(QIcon(), tr("Gray Scale"))
           << QPair<QIcon, QString>(QIcon(), tr("Hot"))
           << QPair<QIcon, QString>(QIcon(), tr("Cold"))
           << QPair<QIcon, QString>(QIcon(), tr("Night"))
           << QPair<QIcon, QString>(QIcon(), tr("Candy"))
           << QPair<QIcon, QString>(QIcon(), tr("Geography"))
           << QPair<QIcon, QString>(QIcon(), tr("Ion"))
           << QPair<QIcon, QString>(QIcon(), tr("Thermal"))
           << QPair<QIcon, QString>(QIcon(), tr("Polar"))
           << QPair<QIcon, QString>(QIcon(), tr("Spectrum"))
           << QPair<QIcon, QString>(QIcon(), tr("Jet"))
           << QPair<QIcon, QString>(QIcon(), tr("Hues"));
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Basic_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Interpolate);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Tight_Boundary);
      auto lcount = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DCM_Level_Count);
      lcount->setLowerLimitInt(0);

      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Gradient, grad);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Gradient_Invert);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Periodic);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Scale_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Scale_Visible);
      auto swidth = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DCM_Scale_Width);
      swidth->setLowerLimitInt(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Separator);

      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Visible);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Offset);
      auto from = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DCM_Axis_From_Double);
      from->setPrecision(precision);
      auto to = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DCM_Axis_To_Double);
      to->setPrecision(precision);
      from->setLowerLimitDouble(-QCPRange::maxRange);
      from->setUpperLimitDouble(QCPRange::maxRange);
      to->setLowerLimitDouble(-QCPRange::maxRange);
      to->setUpperLimitDouble(QCPRange::maxRange);
      // instant update
      /*connect(axis, &Axis2D::rescaleAxis2D, this, [=]() {
        updateProperty(from);
        updateProperty(to);
      });*/
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Type,
                           axisscaletype);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Inverted);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Stroke_Color);
      auto strokethick = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_Stroke_Thickness);
      strokethick->setPrecision(4);
      strokethick->setLowerLimitDouble(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Stroke_Style,
                           penstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Antialiased);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_Label_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Label_Text);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Label_Font);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Label_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Label_Padding);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_Ticks_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_Ticks_Visible);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_Ticks_LengthIn);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_Ticks_LengthOut);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Color);
      auto tickstrokethickness = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Thickness);
      tickstrokethickness->setPrecision(4);
      tickstrokethickness->setLowerLimitDouble(0);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_Ticks_Stroke_Style, penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_SubTicks_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_SubTicks_Visible);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_SubTicks_LengthIn);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_SubTicks_LengthOut);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Color);
      auto subtickstrokethickness = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Thickness);
      subtickstrokethickness->setPrecision(4);
      subtickstrokethickness->setLowerLimitDouble(0);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_SubTicks_Stroke_Style,
          penstyles);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_TickLabel_Separator);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_TickLabel_Visible);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_TickLabel_Font);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_TickLabel_Color);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_TickLabel_Padding);
      auto ticklabelrot = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_TickLabel_Rotation);
      ticklabelrot->setPrecision(2);
      ticklabelrot->setLowerLimitDouble(0);
      ticklabelrot->setUpperLimitDouble(360);
      ticklabelrot->setSuffix(" °");
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DCM_Axis_TickLabel_Side,
                           axisticklabelside);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_TickLabel_Format_Double,
          axisformatdouble);
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DCM_Axis_TickLabel_Precision_Double);
    } break;
    // ErrorBar2D
    case ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar: {
      PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DErrBar_Basic_Separator);
      auto ww = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DErrBar_Whisker_Width);
      ww->setPrecision(4);
      ww->setLowerLimitDouble(0);
      auto sg = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot2DErrBar_Symbol_Gap);
      sg->setPrecision(4);
      sg->setLowerLimitDouble(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DErrBar_Antialiased);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DErrBar_Stroke_Color);
      auto stroket = PropertyItem::create(
          rootItem_, item,
          PropertyItem::Property::Plot2DErrBar_Stroke_Thickness);
      stroket->setPrecision(4);
      stroket->setLowerLimitDouble(0);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DErrBar_Stroke_Style,
                           penstyles);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DErrBar_Fill_Color);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot2DErrBar_Fill_Style,
                           brushfillstyles);
    } break;
    // Plot3D Canvas
    case ObjectBrowserTreeItem::ObjectType::Plot3DCanvas: {
      QList<QPair<QIcon, QString>> themes;
      themes << QPair<QIcon, QString>(QIcon(), tr("Theme QT"))
             << QPair<QIcon, QString>(QIcon(), tr("Theme Primary Colors"))
             << QPair<QIcon, QString>(QIcon(), tr("Theme Digia"))
             << QPair<QIcon, QString>(QIcon(), tr("Stone Moss"))
             << QPair<QIcon, QString>(QIcon(), tr("Theme Army Blue"))
             << QPair<QIcon, QString>(QIcon(), tr("Theme Retro"))
             << QPair<QIcon, QString>(QIcon(), tr("Theme Ebony"))
             << QPair<QIcon, QString>(QIcon(), tr("Theme Isabelle"))
             << QPair<QIcon, QString>(QIcon(), tr("Theme User Defined"));
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot3DCanvas_Base_Separator);
      PropertyItem::create(rootItem_, item,
                           PropertyItem::Property::Plot3DCanvas_Theme, themes);
      auto dim = PropertyItem::create(
          rootItem_, item, PropertyItem::Property::Plot3DCanvas_Dimension);
      auto pcw = PropertyItem::create(
          dim, item, PropertyItem::Property::Plot3DCanvas_Dimension_Width);
      pcw->setLowerLimitInt(0);
      auto pch = PropertyItem::create(
          dim, item, PropertyItem::Property::Plot3DCanvas_Dimension_Height);
      pch->setLowerLimitInt(0);
      // instant update
      bool status = false;
      Layout3D *plot = item->parentItem()->getObjectTreeItem<Layout3D>(&status);
      if (status)
        connect(plot, &Layout3D::geometrychange, this,
                [=]() { updateProperty(dim); });
    } break;
  }

  endResetModel();
  emit modelResetComplete();
}

bool PropertyItemModel::removeRows(int row, int count,
                                   const QModelIndex &parent) {
  PropertyItem *item;
  if (!parent.isValid())
    item = rootItem_;
  else
    item = static_cast<PropertyItem *>(parent.internalPointer());

  int start = row;
  int end = row + count - 1;
  beginRemoveRows(parent, start, end);
  item->removeChildren(start, end);
  endRemoveRows();
  return true;
}

void PropertyItemModel::removeAll() {
  beginResetModel();
  delete rootItem_;
  rootItem_ = static_cast<PropertyItem *>(
      PropertyItem::create(nullptr, nullptr, PropertyItem::Property::None));
  endResetModel();
}

void PropertyItemModel::updateChildren(PropertyItem *item, int column,
                                       const QModelIndex &parent) {
  int numChild = item->childCount();
  if (numChild > 0) {
    QModelIndex topLeft = this->index(0, column, parent);
    QModelIndex bottomRight = this->index(numChild, column, parent);
    emit dataChanged(topLeft, bottomRight);
  }
}

QList<QPair<QIcon, QString>> PropertyItemModel::getXaxislist(
    AxisRect2D *axisrect) {
  QList<QPair<QIcon, QString>> xaxislist;
  QList<Axis2D *> xaxes = axisrect->getXAxes2D();
  for (int i = 0; i < xaxes.size(); i++) {
    xaxislist << QPair<QIcon, QString>(
        xaxes.at(i)->getItemIcon(),
        xaxes.at(i)->getname_axis() +
            QString::number(xaxes.at(i)->getnumber_axis()));
  }
  return xaxislist;
}

QList<QPair<QIcon, QString>> PropertyItemModel::getYaxislist(
    AxisRect2D *axisrect) {
  QList<QPair<QIcon, QString>> yaxislist;
  QList<Axis2D *> yaxes = axisrect->getYAxes2D();
  for (int i = 0; i < yaxes.size(); i++) {
    yaxislist << QPair<QIcon, QString>(
        yaxes.at(i)->getItemIcon(),
        yaxes.at(i)->getname_axis() +
            QString::number(yaxes.at(i)->getnumber_axis()));
  }
  return yaxislist;
}
