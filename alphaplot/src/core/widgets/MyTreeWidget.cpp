#include "MyTreeWidget.h"

#include <memory>

#include "2Dplot/Axis2D.h"
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Bar2D.h"
#include "2Dplot/ColorMap2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/DataManager2D.h"
#include "2Dplot/ErrorBar2D.h"
#include "2Dplot/ImageItem2D.h"
#include "2Dplot/Layout2D.h"
#include "2Dplot/LineItem2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "2Dplot/Pie2D.h"
#include "2Dplot/TextItem2D.h"
#include "2Dplot/widgets/AddPlot2DDialog.h"
#include "2Dplot/widgets/Function2DDialog.h"
#include "3Dplot/DataManager3D.h"
#include "Matrix.h"
#include "MyWidget.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "future/core/column/Column.h"

MyTreeWidget::MyTreeWidget(QWidget *parent)
    : QTreeWidget(parent),
      widget_(parent),
      showfunctiondetailscurve2d_(
          new QAction("Show function details...", this)),
      showfunctiondetailssurface3d_(
          new QAction("Show function details...", this)),
      selectdatacolumnslsgraph2d_(new QAction("Go To Data Columns...", this)),
      selectdatacolumnschannelgraph2d_(
          new QAction("Go To Data Columns...", this)),
      selectdatacolumnscurvegraph2d_(
          new QAction("Go To Data Columns...", this)),
      selectdatacolumnsbargraph2d_(new QAction("Go To Data Columns...", this)),
      selectdatacolumnsvectorgraph2d_(
          new QAction("Go To Data Columns...", this)),
      selectdatacolumnsstatboxgraph2d_(
          new QAction("Go To Data Columns...", this)),
      selectdatacolumnserrorgraph2d_(
          new QAction("Go To Data Columns...", this)),
      selectdatacolumnspiegraph2d_(new QAction("Go To Data Columns...", this)),
      selectdatacolumnscolormapgraph2d_(
          new QAction("Go To Data Columns...", this)),
      removeaxis_(new QAction("Remove", this)),
      removels_(new QAction("Remove", this)),
      removechannel_(new QAction("Remove", this)),
      removecurve_(new QAction("Remove", this)),
      removebar_(new QAction("Remove", this)),
      removevector_(new QAction("Remove", this)),
      removestatbox_(new QAction("Remove", this)),
      removeerrorbar_(new QAction("Remove", this)),
      removetextitem_(new QAction("Remove", this)),
      removelineitem_(new QAction("Remove", this)),
      removeimageitem_(new QAction("Remove", this)),
      clonetotopaxis_(new QAction(tr("Clone To new Top Axis"), this)),
      clonetobottomaxis_(new QAction(tr("Clone To new Bottom Axis"), this)),
      clonetoleftaxis_(new QAction(tr("Clone To new Left Axis"), this)),
      clonetorightaxis_(new QAction(tr("Clone To new Right Axis"), this)),
      moveupls_(new QAction("Layer Up", this)),
      moveupchannel_(new QAction("Layer Up", this)),
      moveupcurve_(new QAction("Layer Up", this)),
      moveupbar_(new QAction("Layer Up", this)),
      moveupvector_(new QAction("Layer Up", this)),
      moveupstatbox_(new QAction("Layer Up", this)),
      moveuppie_(new QAction("Layer Up", this)),
      moveupcolormap_(new QAction("Layer Up", this)),
      moveuptextitem_(new QAction("Layer Up", this)),
      moveuplineitem_(new QAction("Layer Up", this)),
      moveupimageitem_(new QAction("Layer Up", this)),
      movedownls_(new QAction("Layer Down", this)),
      movedownchannel_(new QAction("Layer Down", this)),
      movedowncurve_(new QAction("Layer Down", this)),
      movedownbar_(new QAction("Layer Down", this)),
      movedownvector_(new QAction("Layer Down", this)),
      movedownstatbox_(new QAction("Layer Down", this)),
      movedownpie_(new QAction("Layer Down", this)),
      movedowncolormap_(new QAction("Layer Down", this)),
      movedowntextitem_(new QAction("Layer Down", this)),
      movedownlineitem_(new QAction("Layer Down", this)),
      movedownimageitem_(new QAction("Layer Down", this)) {
  setContextMenuPolicy(Qt::CustomContextMenu);
  // Icons
  showfunctiondetailscurve2d_->setIcon(
      IconLoader::load("math-fofx", IconLoader::LightDark));
  showfunctiondetailssurface3d_->setIcon(
      IconLoader::load("math-fofx", IconLoader::LightDark));
  removeaxis_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removels_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removechannel_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removecurve_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removebar_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removevector_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removestatbox_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removeerrorbar_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removetextitem_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removelineitem_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removeimageitem_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  clonetotopaxis_->setIcon(
      IconLoader::load("graph2d-axis-top", IconLoader::LightDark));
  clonetobottomaxis_->setIcon(
      IconLoader::load("graph2d-axis-bottom", IconLoader::LightDark));
  clonetoleftaxis_->setIcon(
      IconLoader::load("graph2d-axis-left", IconLoader::LightDark));
  clonetorightaxis_->setIcon(
      IconLoader::load("graph2d-axis-right", IconLoader::LightDark));
  moveupls_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveupchannel_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveupcurve_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveupbar_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveupvector_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveupstatbox_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveuppie_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveupcolormap_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveuptextitem_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveuplineitem_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  moveupimageitem_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  movedownls_->setIcon(IconLoader::load("edit-down", IconLoader::LightDark));
  movedownchannel_->setIcon(
      IconLoader::load("edit-down", IconLoader::LightDark));
  movedowncurve_->setIcon(IconLoader::load("edit-down", IconLoader::LightDark));
  movedownbar_->setIcon(IconLoader::load("edit-down", IconLoader::LightDark));
  movedownvector_->setIcon(
      IconLoader::load("edit-down", IconLoader::LightDark));
  movedownstatbox_->setIcon(
      IconLoader::load("edit-down", IconLoader::LightDark));
  movedownpie_->setIcon(IconLoader::load("edit-down", IconLoader::LightDark));
  movedowncolormap_->setIcon(
      IconLoader::load("edit-down", IconLoader::LightDark));
  movedowntextitem_->setIcon(
      IconLoader::load("edit-down", IconLoader::LightDark));
  movedownlineitem_->setIcon(
      IconLoader::load("edit-down", IconLoader::LightDark));
  movedownimageitem_->setIcon(
      IconLoader::load("edit-down", IconLoader::LightDark));
  // connections;
  connect(this, &MyTreeWidget::itemClicked, this,
          &MyTreeWidget::CurrentItemChanged);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          SLOT(showContextMenu(const QPoint &)));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          SLOT(showContextMenu(const QPoint &)));
  connect(showfunctiondetailscurve2d_, &QAction::triggered, this,
          &MyTreeWidget::showFunction2dDetails);
  connect(showfunctiondetailssurface3d_, &QAction::triggered, this,
          &MyTreeWidget::showFunction3dDetails);
  connect(clonetotopaxis_, &QAction::triggered, this,
          &MyTreeWidget::cloneAxis2D);
  connect(clonetobottomaxis_, &QAction::triggered, this,
          &MyTreeWidget::cloneAxis2D);
  connect(clonetoleftaxis_, &QAction::triggered, this,
          &MyTreeWidget::cloneAxis2D);
  connect(clonetorightaxis_, &QAction::triggered, this,
          &MyTreeWidget::cloneAxis2D);
  connect(removeaxis_, SIGNAL(triggered(bool)), this, SLOT(removeAxis2D()));
  connect(removels_, SIGNAL(triggered(bool)), this,
          SLOT(removeLineSpecial2D()));
  connect(removechannel_, SIGNAL(triggered(bool)), this,
          SLOT(removeLineSpecialChannel2D()));
  connect(removecurve_, SIGNAL(triggered(bool)), this, SLOT(removeCurve2D()));
  connect(removebar_, SIGNAL(triggered(bool)), this, SLOT(removeBar2D()));
  connect(removevector_, SIGNAL(triggered(bool)), this, SLOT(removeVector2D()));
  connect(removestatbox_, SIGNAL(triggered(bool)), this,
          SLOT(removeStatBox2D()));
  connect(removeerrorbar_, SIGNAL(triggered(bool)), this,
          SLOT(removeErrorBar2D()));
  connect(removetextitem_, SIGNAL(triggered(bool)), this,
          SLOT(removeTextItem2D()));
  connect(removelineitem_, SIGNAL(triggered(bool)), this,
          SLOT(removeLineItem2D()));
  connect(removeimageitem_, SIGNAL(triggered(bool)), this,
          SLOT(removeImageItem2D()));
  // select data
  connect(selectdatacolumnslsgraph2d_, &QAction::triggered, this,
          [=]() { selectData(qobject_cast<QAction *>(sender())); });
  connect(selectdatacolumnschannelgraph2d_, &QAction::triggered, this,
          [=]() { selectData(qobject_cast<QAction *>(sender())); });
  connect(selectdatacolumnscurvegraph2d_, &QAction::triggered, this,
          [=]() { selectData(qobject_cast<QAction *>(sender())); });
  connect(selectdatacolumnsbargraph2d_, &QAction::triggered, this,
          [=]() { selectData(qobject_cast<QAction *>(sender())); });
  connect(selectdatacolumnsvectorgraph2d_, &QAction::triggered, this,
          [=]() { selectData(qobject_cast<QAction *>(sender())); });
  connect(selectdatacolumnsstatboxgraph2d_, &QAction::triggered, this,
          [=]() { selectData(qobject_cast<QAction *>(sender())); });
  connect(selectdatacolumnspiegraph2d_, &QAction::triggered, this,
          [=]() { selectData(qobject_cast<QAction *>(sender())); });
  connect(selectdatacolumnserrorgraph2d_, &QAction::triggered, this,
          [=]() { selectData(qobject_cast<QAction *>(sender())); });
  connect(selectdatacolumnscolormapgraph2d_, &QAction::triggered, this,
          [=]() { selectData(qobject_cast<QAction *>(sender())); });
  // move up layer
  connect(moveupls_, &QAction::triggered, this, [=]() {
    moveplottablelayer<LineSpecial2D>(qobject_cast<QAction *>(sender()),
                                      QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveupchannel_, &QAction::triggered, this, [=]() {
    moveplottablelayer<LineSpecial2D>(qobject_cast<QAction *>(sender()),
                                      QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveupcurve_, &QAction::triggered, this, [=]() {
    moveplottablelayer<Curve2D>(qobject_cast<QAction *>(sender()),
                                QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveupbar_, &QAction::triggered, this, [=]() {
    moveplottablelayer<Bar2D>(qobject_cast<QAction *>(sender()),
                              QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveupvector_, &QAction::triggered, this, [=]() {
    moveplottablelayer<Vector2D>(qobject_cast<QAction *>(sender()),
                                 QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveupstatbox_, &QAction::triggered, this, [=]() {
    moveplottablelayer<StatBox2D>(qobject_cast<QAction *>(sender()),
                                  QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveupcolormap_, &QAction::triggered, this, [=]() {
    moveplottablelayer<ColorMap2D>(qobject_cast<QAction *>(sender()),
                                   QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveuppie_, &QAction::triggered, this, [=]() {
    moveitemlayer<Pie2D>(qobject_cast<QAction *>(sender()),
                         QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveuptextitem_, &QAction::triggered, this, [=]() {
    moveitemlayer<TextItem2D>(qobject_cast<QAction *>(sender()),
                              QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveuplineitem_, &QAction::triggered, this, [=]() {
    moveitemlayer<LineItem2D>(qobject_cast<QAction *>(sender()),
                              QCustomPlot::LayerInsertMode::limAbove);
  });
  connect(moveupimageitem_, &QAction::triggered, this, [=]() {
    moveitemlayer<ImageItem2D>(qobject_cast<QAction *>(sender()),
                               QCustomPlot::LayerInsertMode::limAbove);
  });

  // move down layer
  connect(movedownls_, &QAction::triggered, this, [=]() {
    moveplottablelayer<LineSpecial2D>(qobject_cast<QAction *>(sender()),
                                      QCustomPlot::LayerInsertMode::limBelow);
  });
  connect(movedownchannel_, &QAction::triggered, this, [=]() {});
  connect(movedowncurve_, &QAction::triggered, this, [=]() {
    moveplottablelayer<Curve2D>(qobject_cast<QAction *>(sender()),
                                QCustomPlot::LayerInsertMode::limBelow);
  });
  connect(movedownbar_, &QAction::triggered, this, [=]() {
    moveplottablelayer<Bar2D>(qobject_cast<QAction *>(sender()),
                              QCustomPlot::LayerInsertMode::limBelow);
  });
  connect(movedownvector_, &QAction::triggered, this, [=]() {
    moveplottablelayer<Vector2D>(qobject_cast<QAction *>(sender()),
                                 QCustomPlot::LayerInsertMode::limBelow);
  });
  connect(movedownstatbox_, &QAction::triggered, this, [=]() {
    moveplottablelayer<StatBox2D>(qobject_cast<QAction *>(sender()),
                                  QCustomPlot::LayerInsertMode::limBelow);
  });
  connect(movedowncolormap_, &QAction::triggered, this, [=]() {
    moveplottablelayer<ColorMap2D>(qobject_cast<QAction *>(sender()),
                                   QCustomPlot::LayerInsertMode::limBelow);
  });
  connect(movedownpie_, &QAction::triggered, this, [=]() {
    moveitemlayer<Pie2D>(qobject_cast<QAction *>(sender()),
                         QCustomPlot::LayerInsertMode::limBelow);
  });
  connect(movedowntextitem_, &QAction::triggered, this, [=]() {
    moveitemlayer<TextItem2D>(qobject_cast<QAction *>(sender()),
                              QCustomPlot::LayerInsertMode::limBelow);
  });
  connect(movedownlineitem_, &QAction::triggered, this, [=]() {
    moveitemlayer<LineItem2D>(qobject_cast<QAction *>(sender()),
                              QCustomPlot::LayerInsertMode::limBelow);
  });
  connect(movedownimageitem_, &QAction::triggered, this, [=]() {
    moveitemlayer<ImageItem2D>(qobject_cast<QAction *>(sender()),
                               QCustomPlot::LayerInsertMode::limBelow);
  });
}

MyTreeWidget::~MyTreeWidget() {}

void MyTreeWidget::CurrentItemChanged(QTreeWidgetItem *current) {
  if (!current) return;
  AxisRect2D *currentaxisrect = nullptr;

  switch (static_cast<MyTreeWidget::PropertyItemType>(
      current->data(0, Qt::UserRole).value<int>())) {
    case MyTreeWidget::PropertyItemType::Plot2DLayout: {
      void *ptr = current->data(0, Qt::UserRole + 1).value<void *>();
      currentaxisrect = static_cast<AxisRect2D *>(ptr);
    } break;
    case MyTreeWidget::PropertyItemType::Plot2DGrid:
    case MyTreeWidget::PropertyItemType::Plot2DAxis:
    case MyTreeWidget::PropertyItemType::Plot2DLegend:
    case MyTreeWidget::PropertyItemType::Plot2DTextItem:
    case MyTreeWidget::PropertyItemType::Plot2DLineItem:
    case MyTreeWidget::PropertyItemType::Plot2DImageItem:
    case MyTreeWidget::PropertyItemType::Plot2DLSGraph:
    case MyTreeWidget::PropertyItemType::Plot2DChannelGraph:
    case MyTreeWidget::PropertyItemType::Plot2DCurve:
    case MyTreeWidget::PropertyItemType::Plot2DBarGraph:
    case MyTreeWidget::PropertyItemType::Plot2DStatBox:
    case MyTreeWidget::PropertyItemType::Plot2DVector:
    case MyTreeWidget::PropertyItemType::Plot2DPieGraph:
    case MyTreeWidget::PropertyItemType::Plot2DColorMap: {
      void *ptr = current->parent()->data(0, Qt::UserRole + 1).value<void *>();
      currentaxisrect = static_cast<AxisRect2D *>(ptr);
    } break;
    case MyTreeWidget::PropertyItemType::Plot2DErrorBar: {
      void *ptr = current->parent()
                      ->parent()
                      ->data(0, Qt::UserRole + 1)
                      .value<void *>();
      currentaxisrect = static_cast<AxisRect2D *>(ptr);
      // void *errorptr = current->data(0, Qt::UserRole + 1)
      //    .value<void *>();
      // ErrorBar2D *errorbar = static_cast<ErrorBar2D *>(errorptr);
      // errorbar->setSelectable(QCP::SelectionType::stWhole);
    } break;
    default:
      break;
  }

  if (currentaxisrect) currentaxisrect->selectAxisRect();
}

void MyTreeWidget::showContextMenu(const QPoint &pos) {
  QTreeWidgetItem *item = itemAt(pos);
  if (!item) return;

  QPoint globalPos = viewport()->mapToGlobal(pos);
  QMenu menu;
  QMenu cloneaxismenu;

  switch (static_cast<PropertyItemType>(item->data(0, Qt::UserRole).toInt())) {
    case PropertyItemType::MyWidgetWindow:
      emit itemRootContextMenuRequested();
      return;
      break;
    case PropertyItemType::Plot2DCanvas:
      break;
    case PropertyItemType::Plot2DLayout: {
      void *ptr1 = item->data(0, Qt::UserRole + 1).value<void *>();
      AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr1);
      void *ptr2 = item->data(0, Qt::UserRole + 2).value<void *>();
      Layout2D *layout = static_cast<Layout2D *>(ptr2);
      emit itemContextMenuRequested(layout, axisrect);
      return;
    } break;
    case PropertyItemType::Plot2DAxis: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      Axis2D *ax = static_cast<Axis2D *>(ptr);
      cloneaxismenu.setTitle(tr("Clone Axis..."));
      menu.addMenu(&cloneaxismenu);
      if (ax->getorientation_axis() == Axis2D::AxisOreantation::Top ||
          ax->getorientation_axis() == Axis2D::AxisOreantation::Bottom) {
        cloneaxismenu.addAction(clonetotopaxis_);
        cloneaxismenu.addAction(clonetobottomaxis_);
        clonetotopaxis_->setData(item->data(0, Qt::UserRole + 1));
        clonetobottomaxis_->setData(item->data(0, Qt::UserRole + 1));
      } else {
        cloneaxismenu.addAction(clonetoleftaxis_);
        cloneaxismenu.addAction(clonetorightaxis_);
        clonetoleftaxis_->setData(item->data(0, Qt::UserRole + 1));
        clonetorightaxis_->setData(item->data(0, Qt::UserRole + 1));
      }
      menu.addAction(removeaxis_);
      removeaxis_->setData(item->data(0, Qt::UserRole + 1));
    } break;
    case PropertyItemType::Plot2DLSGraph:
      menu.addAction(selectdatacolumnslsgraph2d_);
      menu.addAction(moveupls_);
      menu.addAction(movedownls_);
      menu.addAction(removels_);
      removels_->setData(item->data(0, Qt::UserRole + 1));
      moveupls_->setData(item->data(0, Qt::UserRole + 1));
      movedownls_->setData(item->data(0, Qt::UserRole + 1));
      selectdatacolumnslsgraph2d_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DChannelGraph:
      menu.addAction(selectdatacolumnschannelgraph2d_);
      menu.addAction(moveupchannel_);
      menu.addAction(movedownchannel_);
      menu.addAction(removechannel_);
      removechannel_->setData(item->data(0, Qt::UserRole + 1));
      moveupchannel_->setData(item->data(0, Qt::UserRole + 1));
      movedownchannel_->setData(item->data(0, Qt::UserRole + 1));
      selectdatacolumnschannelgraph2d_->setData(
          item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DCurve: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      Curve2D *curve = static_cast<Curve2D *>(ptr);
      if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Function) {
        menu.addAction(showfunctiondetailscurve2d_);
        showfunctiondetailscurve2d_->setData(item->data(0, Qt::UserRole + 1));
      }
      menu.addAction(selectdatacolumnscurvegraph2d_);
      menu.addAction(moveupcurve_);
      menu.addAction(movedowncurve_);
      menu.addAction(removecurve_);
      removecurve_->setData(item->data(0, Qt::UserRole + 1));
      moveupcurve_->setData(item->data(0, Qt::UserRole + 1));
      movedowncurve_->setData(item->data(0, Qt::UserRole + 1));
      selectdatacolumnscurvegraph2d_->setData(item->data(0, Qt::UserRole + 1));
    } break;
    case PropertyItemType::Plot2DBarGraph:
      menu.addAction(selectdatacolumnsbargraph2d_);
      menu.addAction(moveupbar_);
      menu.addAction(movedownbar_);
      menu.addAction(removebar_);
      removebar_->setData(item->data(0, Qt::UserRole + 1));
      moveupbar_->setData(item->data(0, Qt::UserRole + 1));
      movedownbar_->setData(item->data(0, Qt::UserRole + 1));
      selectdatacolumnsbargraph2d_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DVector:
      menu.addAction(selectdatacolumnsvectorgraph2d_);
      menu.addAction(moveupvector_);
      menu.addAction(movedownvector_);
      menu.addAction(removevector_);
      removevector_->setData(item->data(0, Qt::UserRole + 1));
      moveupvector_->setData(item->data(0, Qt::UserRole + 1));
      movedownvector_->setData(item->data(0, Qt::UserRole + 1));
      selectdatacolumnsvectorgraph2d_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DStatBox:
      menu.addAction(selectdatacolumnsstatboxgraph2d_);
      menu.addAction(moveupstatbox_);
      menu.addAction(movedownstatbox_);
      menu.addAction(removestatbox_);
      removestatbox_->setData(item->data(0, Qt::UserRole + 1));
      moveupstatbox_->setData(item->data(0, Qt::UserRole + 1));
      movedownstatbox_->setData(item->data(0, Qt::UserRole + 1));
      selectdatacolumnsstatboxgraph2d_->setData(
          item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DErrorBar:
      menu.addAction(selectdatacolumnserrorgraph2d_);
      menu.addAction(removeerrorbar_);
      removeerrorbar_->setData(item->data(0, Qt::UserRole + 1));
      selectdatacolumnserrorgraph2d_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DTextItem:
      menu.addAction(moveuptextitem_);
      menu.addAction(movedowntextitem_);
      menu.addAction(removetextitem_);
      removetextitem_->setData(item->data(0, Qt::UserRole + 1));
      moveuptextitem_->setData(item->data(0, Qt::UserRole + 1));
      movedowntextitem_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DLineItem:
      menu.addAction(moveuplineitem_);
      menu.addAction(movedownlineitem_);
      menu.addAction(removelineitem_);
      removelineitem_->setData(item->data(0, Qt::UserRole + 1));
      moveuplineitem_->setData(item->data(0, Qt::UserRole + 1));
      movedownlineitem_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DImageItem:
      menu.addAction(moveupimageitem_);
      menu.addAction(movedownimageitem_);
      menu.addAction(removeimageitem_);
      removeimageitem_->setData(item->data(0, Qt::UserRole + 1));
      moveupimageitem_->setData(item->data(0, Qt::UserRole + 1));
      movedownimageitem_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DPieGraph:
      menu.addAction(selectdatacolumnspiegraph2d_);
      menu.addAction(moveuppie_);
      menu.addAction(movedownpie_);
      moveuppie_->setData(item->data(0, Qt::UserRole + 1));
      movedownpie_->setData(item->data(0, Qt::UserRole + 1));
      selectdatacolumnspiegraph2d_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot2DColorMap:
      menu.addAction(selectdatacolumnscolormapgraph2d_);
      menu.addAction(moveupcolormap_);
      menu.addAction(movedowncolormap_);
      moveupcolormap_->setData(item->data(0, Qt::UserRole + 1));
      movedowncolormap_->setData(item->data(0, Qt::UserRole + 1));
      selectdatacolumnscolormapgraph2d_->setData(
          item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Plot3DSurfaceDataBlock: {
      void *ptr = item->data(0, Qt::UserRole + 1).value<void *>();
      DataBlockSurface3D *block = static_cast<DataBlockSurface3D *>(ptr);
      if (!block->ismatrix()) {
        menu.addAction(showfunctiondetailssurface3d_);
        showfunctiondetailssurface3d_->setData(item->data(0, Qt::UserRole + 1));
      }
    } break;
    default:
      break;
  }
  menu.exec(globalPos);
}

void MyTreeWidget::cloneAxis2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Axis2D *axis = static_cast<Axis2D *>(ptr);
  AxisRect2D *axisrect = axis->getaxisrect_axis();
  Axis2D *newaxis = nullptr;
  if (action == clonetotopaxis_)
    newaxis = axisrect->addAxis2D(Axis2D::AxisOreantation::Top,
                                  axis->gettickertype_axis());
  else if (action == clonetobottomaxis_)
    newaxis = axisrect->addAxis2D(Axis2D::AxisOreantation::Bottom,
                                  axis->gettickertype_axis());
  else if (action == clonetoleftaxis_)
    newaxis = axisrect->addAxis2D(Axis2D::AxisOreantation::Left,
                                  axis->gettickertype_axis());
  else if (action == clonetorightaxis_)
    newaxis = axisrect->addAxis2D(Axis2D::AxisOreantation::Right,
                                  axis->gettickertype_axis());

  axis->clone(newaxis);
  emit axisrect->Axis2DCloned(axis);
  axisrect->parentPlot()->replot();
}

void MyTreeWidget::removeAxis2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Axis2D *axis = static_cast<Axis2D *>(ptr);
  AxisRect2D *axisrect = axis->getaxisrect_axis();
  bool result = axisrect->removeAxis2D(axis);
  if (!result) {
    qDebug() << "unable to remove line special 2d plot";
    return;
  }
}

void MyTreeWidget::removeLineSpecial2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  LineSpecial2D *ls = static_cast<LineSpecial2D *>(ptr);
  bool result = ls->getxaxis()->getaxisrect_axis()->removeLineSpecial2D(ls);
  if (!result) {
    qDebug() << "unable to remove line special 2d plot";
    return;
  }
}

void MyTreeWidget::removeLineSpecialChannel2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  LineSpecial2D *ls = static_cast<LineSpecial2D *>(ptr);
  AxisRect2D *axisrect = ls->getxaxis()->getaxisrect_axis();
  auto channellist = axisrect->getChannelVec();
  int position = -1;
  for (int i = 0; i < channellist.count(); i++) {
    auto channel = channellist.at(i);
    if (channel.first == ls || channel.second == ls) {
      position = i;
      break;
    }
  }
  if (position != -1) axisrect->removeChannel2D(channellist.at(position));
}

void MyTreeWidget::removeCurve2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Curve2D *curve = static_cast<Curve2D *>(ptr);
  bool result = curve->getxaxis()->getaxisrect_axis()->removeCurve2D(curve);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
    return;
  }
}

void MyTreeWidget::removeBar2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Bar2D *bar = static_cast<Bar2D *>(ptr);
  bool result = bar->getxaxis()->getaxisrect_axis()->removeBar2D(bar);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
    return;
  }
}

void MyTreeWidget::removeVector2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Vector2D *vector = static_cast<Vector2D *>(ptr);
  bool result = vector->getxaxis()->getaxisrect_axis()->removeVector2D(vector);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
    return;
  }
}

void MyTreeWidget::removeStatBox2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  StatBox2D *statbox = static_cast<StatBox2D *>(ptr);
  bool result =
      statbox->getxaxis()->getaxisrect_axis()->removeStatBox2D(statbox);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
    return;
  }
}

void MyTreeWidget::removeErrorBar2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  ErrorBar2D *error = static_cast<ErrorBar2D *>(ptr);
  LineSpecial2D *ls = error->getlinespecial2d_errorbar();
  Curve2D *curve = error->getcurve2d_errorbar();
  Bar2D *bar = error->getbar2d_errorbar();
  if (ls) {
    (error->geterrortype_errorbar() == QCPErrorBars::ErrorType::etKeyError)
        ? ls->removeXerrorBar()
        : ls->removeYerrorBar();
  } else if (curve) {
    (error->geterrortype_errorbar() == QCPErrorBars::ErrorType::etKeyError)
        ? curve->removeXerrorBar()
        : curve->removeYerrorBar();
  } else if (bar) {
    (error->geterrortype_errorbar() == QCPErrorBars::ErrorType::etKeyError)
        ? bar->removeXerrorBar()
        : bar->removeYerrorBar();
  }
}

void MyTreeWidget::removeTextItem2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  TextItem2D *textitem = static_cast<TextItem2D *>(ptr);
  bool result = textitem->getaxisrect()->removeTextItem2D(textitem);
  if (!result) {
    qDebug() << "unable to remove line special 2d plot";
    return;
  }
}

void MyTreeWidget::removeLineItem2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  LineItem2D *lineitem = static_cast<LineItem2D *>(ptr);
  bool result = lineitem->getaxisrect()->removeLineItem2D(lineitem);
  if (!result) {
    qDebug() << "unable to remove line special 2d plot";
    return;
  }
}

void MyTreeWidget::removeImageItem2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  ImageItem2D *imageitem = static_cast<ImageItem2D *>(ptr);
  bool result = imageitem->getaxisrect()->removeImageItem2D(imageitem);
  if (!result) {
    qDebug() << "unable to remove line special 2d plot";
    return;
  }
}

void MyTreeWidget::showFunction3dDetails() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action || action != showfunctiondetailssurface3d_) return;
  void *ptr = action->data().value<void *>();
  DataBlockSurface3D *block = static_cast<DataBlockSurface3D *>(ptr);
  QString funcsurface = QString(
      "<tr> <td align=\"right\">Function :</td><td>%1</td></tr>"
      "<tr> <td align=\"right\">xl :</td><td>%2</td></tr>"
      "<tr> <td align=\"right\">xu :</td><td>%3</td></tr>"
      "<tr> <td align=\"right\">yl :</td><td>%4</td></tr>"
      "<tr> <td align=\"right\">yu :</td><td>%5</td></tr>"
      "<tr> <td align=\"right\">zl :</td><td>%6</td></tr>"
      "<tr> <td align=\"right\">zu :</td><td>%7</td></tr>"
      "<tr> <td align=\"right\">Points :</td><td>%8</td></tr>");
  QMessageBox::information(this, tr("Function Surface3D"),
                           QString(funcsurface)
                               .arg(block->getfunction())
                               .arg(QString::number(block->getxlower()))
                               .arg(QString::number(block->getxupper()))
                               .arg(QString::number(block->getylower()))
                               .arg(QString::number(block->getyupper()))
                               .arg(QString::number(block->getzlower()))
                               .arg(QString::number(block->getzupper()))
                               .arg(QString::number(block->getxpoints())));
}

void MyTreeWidget::showFunction2dDetails() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action || action != showfunctiondetailscurve2d_) return;
  void *ptr = action->data().value<void *>();
  Curve2D *curve = static_cast<Curve2D *>(ptr);
  const PlotData::FunctionData funcdata = curve->getfuncdata_cplot();
  switch (funcdata.type) {
    case 0: {
      QString functype, func;
      QString funcxy = QString(
          "<tr> <td align=\"right\">Type :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Function :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">Points :</td><td>%5</td></tr>");
      if (funcdata.functions.size() == 1) {
        functype = QString(tr("Normal XY"));
        func = funcdata.functions.at(0);
      } else {
        functype = QString(tr("Unknown"));
        func = QString(tr("unknown"));
      }
      QMessageBox::information(this, tr("Function: %1").arg(functype),
                               QString(funcxy)
                                   .arg(functype)
                                   .arg(func)
                                   .arg(funcdata.from)
                                   .arg(funcdata.to)
                                   .arg(funcdata.points));
    } break;
    case 1: {
      QString functype, func1, func2;
      QString funcparam = QString(
          "<tr> <td align=\"right\">Type :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Function X :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">Function Y :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">Parameter :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%5</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%6</td></tr>"
          "<tr> <td align=\"right\">Points :</td><td>%7</td></tr>");
      if (funcdata.functions.size() == 2) {
        functype = QString(tr("Parametric"));
        func1 = funcdata.functions.at(0);
        func2 = funcdata.functions.at(1);
      } else {
        functype = QString(tr("Unknown"));
        func1 = QString(tr("unknown"));
        func2 = QString(tr("unknown"));
      }
      QMessageBox::information(this, tr("Function: %1").arg(functype),
                               QString(funcparam)
                                   .arg(functype)
                                   .arg(func1)
                                   .arg(func2)
                                   .arg(funcdata.parameter)
                                   .arg(funcdata.from)
                                   .arg(funcdata.to)
                                   .arg(funcdata.points));
    } break;
    case 2: {
      QString functype, func1, func2;
      QString funcpolar = QString(
          "<tr> <td align=\"right\">Type :</td><td>%1</td></tr>"
          "<tr> <td align=\"right\">Function R :</td><td>%2</td></tr>"
          "<tr> <td align=\"right\">Function Theta :</td><td>%3</td></tr>"
          "<tr> <td align=\"right\">Parameter :</td><td>%4</td></tr>"
          "<tr> <td align=\"right\">From :</td><td>%5</td></tr>"
          "<tr> <td align=\"right\">To :</td><td>%6</td></tr>"
          "<tr> <td align=\"right\">Points :</td><td>%7</td></tr>");
      if (funcdata.functions.size() == 2) {
        functype = QString(tr("Polar"));
        func1 = funcdata.functions.at(0);
        func2 = funcdata.functions.at(1);
      } else {
        functype = QString(tr("Unknown"));
        func1 = QString(tr("unknown"));
        func2 = QString(tr("unknown"));
      }
      QMessageBox::information(this, tr("Function: %1").arg(functype),
                               QString(funcpolar)
                                   .arg(functype)
                                   .arg(func1)
                                   .arg(func2)
                                   .arg(funcdata.parameter)
                                   .arg(funcdata.from)
                                   .arg(funcdata.to)
                                   .arg(funcdata.points));
    } break;
  }
}

void MyTreeWidget::selectData(QAction *action) {
  if (!action) return;
  if (action == selectdatacolumnslsgraph2d_) {
    void *ptr = action->data().value<void *>();
    LineSpecial2D *ls = static_cast<LineSpecial2D *>(ptr);
    DataBlockGraph *data = ls->getdatablock_lsplot();
    Table *table = data->gettable();
    Column *xcol = data->getxcolumn();
    Column *ycol = data->getycolumn();
    MyWidget *widget = static_cast<MyWidget *>(table);
    table->deselectAll();
    table->selectColumn(table->colIndex(xcol->name()));
    table->selectColumn(table->colIndex(ycol->name()));
    emit activate(widget);
  } else if (action == selectdatacolumnschannelgraph2d_) {
    void *ptr = action->data().value<void *>();
    LineSpecial2D *ls = static_cast<LineSpecial2D *>(ptr);
    DataBlockGraph *data = ls->getdatablock_lsplot();
    Table *table = data->gettable();
    Column *xcol = data->getxcolumn();
    Column *ycol = data->getycolumn();
    MyWidget *widget = static_cast<MyWidget *>(table);
    table->deselectAll();
    table->selectColumn(table->colIndex(xcol->name()));
    table->selectColumn(table->colIndex(ycol->name()));
    emit activate(widget);
  } else if (action == selectdatacolumnscurvegraph2d_) {
    void *ptr = action->data().value<void *>();
    Curve2D *curve = static_cast<Curve2D *>(ptr);
    DataBlockCurve *data = curve->getdatablock_cplot();
    Table *table = data->gettable();
    Column *xcol = data->getxcolumn();
    Column *ycol = data->getycolumn();
    MyWidget *widget = static_cast<MyWidget *>(table);
    table->deselectAll();
    table->selectColumn(table->colIndex(xcol->name()));
    table->selectColumn(table->colIndex(ycol->name()));
    emit activate(widget);
  } else if (action == selectdatacolumnsbargraph2d_) {
    void *ptr = action->data().value<void *>();
    Bar2D *bar = static_cast<Bar2D *>(ptr);
    if (bar->ishistogram_barplot()) {
      DataBlockHist *data = bar->getdatablock_histplot();
      Table *table = data->gettable();
      Column *col = data->getcolumn();
      MyWidget *widget = static_cast<MyWidget *>(table);
      table->deselectAll();
      table->selectColumn(table->colIndex(col->name()));
      emit activate(widget);
    } else {
      DataBlockBar *data = bar->getdatablock_barplot();
      Table *table = data->gettable();
      Column *xcol = data->getxcolumn();
      Column *ycol = data->getycolumn();
      MyWidget *widget = static_cast<MyWidget *>(table);
      table->deselectAll();
      table->selectColumn(table->colIndex(xcol->name()));
      table->selectColumn(table->colIndex(ycol->name()));
      emit activate(widget);
    }
  } else if (action == selectdatacolumnsvectorgraph2d_) {
    void *ptr = action->data().value<void *>();
    Vector2D *vector = static_cast<Vector2D *>(ptr);
    Table *table = vector->gettable_vecplot();
    Column *xcol1 = vector->getfirstcol_vecplot();
    Column *ycol1 = vector->getsecondcol_vecplot();
    Column *ycol2 = vector->getthirdcol_vecplot();
    Column *ycol3 = vector->getfourthcol_vecplot();
    MyWidget *widget = static_cast<MyWidget *>(table);
    table->deselectAll();
    table->selectColumn(table->colIndex(xcol1->name()));
    table->selectColumn(table->colIndex(ycol1->name()));
    table->selectColumn(table->colIndex(ycol2->name()));
    table->selectColumn(table->colIndex(ycol3->name()));
    emit activate(widget);
  } else if (action == selectdatacolumnsstatboxgraph2d_) {
    void *ptr = action->data().value<void *>();
    StatBox2D *statbox = static_cast<StatBox2D *>(ptr);
    Table *table = statbox->gettable_statbox();
    Column *col = statbox->getcolumn_statbox();
    MyWidget *widget = static_cast<MyWidget *>(table);
    table->deselectAll();
    table->selectColumn(table->colIndex(col->name()));
    emit activate(widget);
  } else if (action == selectdatacolumnspiegraph2d_) {
    void *ptr = action->data().value<void *>();
    Pie2D *pie = static_cast<Pie2D *>(ptr);
    Table *table = pie->gettable_pieplot();
    Column *xcol = pie->getxcolumn_pieplot();
    Column *ycol = pie->getycolumn_pieplot();
    MyWidget *widget = static_cast<MyWidget *>(table);
    table->deselectAll();
    table->selectColumn(table->colIndex(xcol->name()));
    table->selectColumn(table->colIndex(ycol->name()));
    emit activate(widget);
  } else if (action == selectdatacolumnserrorgraph2d_) {
    void *ptr = action->data().value<void *>();
    ErrorBar2D *err = static_cast<ErrorBar2D *>(ptr);
    DataBlockError *data = err->getdatablock_error();
    Table *table = data->gettable();
    Column *col = data->geterrorcolumn();
    MyWidget *widget = static_cast<MyWidget *>(table);
    table->deselectAll();
    table->selectColumn(table->colIndex(col->name()));
    emit activate(widget);
  } else if (action == selectdatacolumnscolormapgraph2d_) {
    void *ptr = action->data().value<void *>();
    ColorMap2D *colormap = static_cast<ColorMap2D *>(ptr);
    Matrix *matrix = colormap->getmatrix_colormap();
    MyWidget *widget = static_cast<MyWidget *>(matrix);
    matrix->selectAll();
    emit activate(widget);
  }
}

template <class T>
void MyTreeWidget::moveplottablelayer(
    QAction *action, const QCustomPlot::LayerInsertMode &mode) {
  if (!action) return;
  void *ptr = action->data().value<void *>();
  T *obj = static_cast<T *>(ptr);
  AxisRect2D *axisrect = obj->getxaxis()->getaxisrect_axis();
  axisrect->moveLayer(obj->layer(), mode);
}

template <class T>
void MyTreeWidget::moveitemlayer(QAction *action,
                                 const QCustomPlot::LayerInsertMode &mode) {
  if (!action) return;
  void *ptr = action->data().value<void *>();
  T *obj = static_cast<T *>(ptr);
  AxisRect2D *axisrect = obj->getaxisrect();
  axisrect->moveLayer(obj->layer(), mode);
}
