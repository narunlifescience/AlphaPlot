#include "MyTreeWidget.h"

#include <memory>

#include "2Dplot/Axis2D.h"
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/ImageItem2D.h"
#include "2Dplot/LineItem2D.h"
#include "2Dplot/TextItem2D.h"
#include "AddAxisWidget.h"
#include "AddPlot2DDialog.h"
#include "Function2DDialog.h"
#include "core/IconLoader.h"

MyTreeWidget::MyTreeWidget(QWidget *parent)
    : QTreeWidget(parent),
      widget_(parent),
      addgraph_(new QAction("Add", this)),
      addfunctionplot_(new QAction("Add", this)),
      leftvalueaxis_(new QAction("Value", this)),
      leftlogaxis_(new QAction("Log", this)),
      leftpiaxis_(new QAction("Pi", this)),
      lefttextaxis_(new QAction("Text", this)),
      lefttimeaxis_(new QAction("Time", this)),
      leftdatetimeaxis_(new QAction("DateTime", this)),
      bottomvalueaxis_(new QAction("Value", this)),
      bottomlogaxis_(new QAction("Log", this)),
      bottompiaxis_(new QAction("Pi", this)),
      bottomtextaxis_(new QAction("Text", this)),
      bottomtimeaxis_(new QAction("Time", this)),
      bottomdatetimeaxis_(new QAction("DateTime", this)),
      rightvalueaxis_(new QAction("Value", this)),
      rightlogaxis_(new QAction("Log", this)),
      rightpiaxis_(new QAction("Pi", this)),
      righttextaxis_(new QAction("Text", this)),
      righttimeaxis_(new QAction("Time", this)),
      rightdatetimeaxis_(new QAction("DateTime", this)),
      topvalueaxis_(new QAction("Value", this)),
      toplogaxis_(new QAction("Log", this)),
      toppiaxis_(new QAction("Pi", this)),
      toptextaxis_(new QAction("Text", this)),
      toptimeaxis_(new QAction("Time", this)),
      topdatetimeaxis_(new QAction("DateTime", this)),
      removeaxis_(new QAction("Remove", this)),
      removels_(new QAction("Remove", this)),
      removecurve_(new QAction("Remove", this)),
      removebar_(new QAction("Remove", this)),
      removevector_(new QAction("Remove", this)),
      removestatbox_(new QAction("Remove", this)),
      removetextitem_(new QAction("Remove", this)),
      removelineitem_(new QAction("Remove", this)),
      removeimageitem_(new QAction("Remove", this)) {
  setContextMenuPolicy(Qt::CustomContextMenu);

  addgraph_->setText("Add Plot...");
  addfunctionplot_->setText("Add Function Plot...");
  leftvalueaxis_->setText("Value Axis");
  leftlogaxis_->setText("Log Axis");
  leftpiaxis_->setText("Pi Axis");
  lefttextaxis_->setText("Text Axis");
  lefttimeaxis_->setText("Time Axis");
  leftdatetimeaxis_->setText("DateTime Axis");
  bottomvalueaxis_->setText("Value Axis");
  bottomlogaxis_->setText("Log Axis");
  bottompiaxis_->setText("Pi Axis");
  bottomtextaxis_->setText("Text Axis");
  bottomtimeaxis_->setText("Time Axis");
  bottomdatetimeaxis_->setText("DateTime Axis");
  rightvalueaxis_->setText("Value Axis");
  rightlogaxis_->setText("Log Axis");
  rightpiaxis_->setText("Pi Axis");
  righttextaxis_->setText("Text Axis");
  righttimeaxis_->setText("Time Axis");
  rightdatetimeaxis_->setText("DateTime Axis");
  topvalueaxis_->setText("Value Axis");
  toplogaxis_->setText("Log Axis");
  toppiaxis_->setText("Pi Axis");
  toptextaxis_->setText("Text Axis");
  toptimeaxis_->setText("Time Axis");
  topdatetimeaxis_->setText("DateTime Axis");
  removeaxis_->setText("Remove");
  removels_->setText("Remove");
  removecurve_->setText("Remove");
  removebar_->setText("Remove");
  removevector_->setText("Remove");
  removestatbox_->setText("remove");
  removetextitem_->setText("Remove");
  removelineitem_->setText("Remove");
  removeimageitem_->setText("Remove");

  addgraph_->setIcon(IconLoader::load("edit-add-graph", IconLoader::LightDark));
  addfunctionplot_->setIcon(
      IconLoader::load("math-fofx", IconLoader::LightDark));
  removeaxis_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removels_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removecurve_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removebar_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removevector_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removestatbox_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removetextitem_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removelineitem_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removeimageitem_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));

  connect(this,
          SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
          this, SLOT(CurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          SLOT(showContextMenu(const QPoint &)));
  connect(addfunctionplot_, SIGNAL(triggered(bool)), this,
          SLOT(addfunctionplot()));
  connect(addgraph_, SIGNAL(triggered(bool)), this, SLOT(addplot()));
  connect(leftvalueaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(leftlogaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(leftpiaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(lefttextaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(lefttimeaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(leftdatetimeaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(bottomvalueaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(bottomlogaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(bottompiaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(bottomtextaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(bottomtimeaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(bottomdatetimeaxis_, SIGNAL(triggered(bool)), this,
          SLOT(addAxis2D()));
  connect(rightvalueaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(rightlogaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(rightpiaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(righttextaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(righttimeaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(rightdatetimeaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(topvalueaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(toplogaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(toppiaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(toptextaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(toptimeaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(topdatetimeaxis_, SIGNAL(triggered(bool)), this, SLOT(addAxis2D()));
  connect(removeaxis_, SIGNAL(triggered(bool)), this, SLOT(removeAxis2D()));
  connect(removels_, SIGNAL(triggered(bool)), this,
          SLOT(removeLineSpecial2D()));
  connect(removecurve_, SIGNAL(triggered(bool)), this, SLOT(removeCurve2D()));
  connect(removebar_, SIGNAL(triggered(bool)), this, SLOT(removeBar2D()));
  connect(removevector_, SIGNAL(triggered(bool)), this, SLOT(removeVector2D()));
  connect(removestatbox_, SIGNAL(triggered(bool)), this,
          SLOT(removeStatBox2D()));
  connect(removetextitem_, SIGNAL(triggered(bool)), this,
          SLOT(removeTextItem2D()));
  connect(removelineitem_, SIGNAL(triggered(bool)), this,
          SLOT(removeLineItem2D()));
  connect(removeimageitem_, SIGNAL(triggered(bool)), this,
          SLOT(removeImageItem2D()));
}

MyTreeWidget::~MyTreeWidget() {}

void MyTreeWidget::CurrentItemChanged(QTreeWidgetItem *current,
                                      QTreeWidgetItem *previous) {
  Q_UNUSED(previous);
  AxisRect2D *currentaxisrect = nullptr;
  if (current) {
    switch (static_cast<MyTreeWidget::PropertyItemType>(
        current->data(0, Qt::UserRole).value<int>())) {
      case MyTreeWidget::PropertyItemType::Layout: {
        void *ptr = current->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::Grid: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::Axis: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::Legend: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::TextItem: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::LineItem: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::ImageItem: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::LSGraph: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::Curve: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::BarGraph: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::StatBox: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::Vector: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::PieGraph: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
      case MyTreeWidget::PropertyItemType::ColorMap: {
        void *ptr =
            current->parent()->data(0, Qt::UserRole + 1).value<void *>();
        currentaxisrect = static_cast<AxisRect2D *>(ptr);
      } break;
    }
  }

  if (currentaxisrect) currentaxisrect->selectAxisRect();
}

void MyTreeWidget::showContextMenu(const QPoint &pos) {
  QTreeWidgetItem *item = itemAt(pos);
  if (!item) return;

  QPoint globalPos = viewport()->mapToGlobal(pos);
  QMenu menu;
  QMenu addleftaxismenu;
  QMenu addbottomaxismenu;
  QMenu addrightaxismenu;
  QMenu addtopaxismenu;

  switch (static_cast<PropertyItemType>(item->data(0, Qt::UserRole).toInt())) {
    case PropertyItemType::Layout: {
      menu.addAction(addgraph_);
      addgraph_->setData(item->data(0, Qt::UserRole + 1));
      menu.addAction(addfunctionplot_);
      addfunctionplot_->setData(item->data(0, Qt::UserRole + 1));
      menu.addMenu(&addleftaxismenu);
      addleftaxismenu.setTitle("Add Left Axis");
      addleftaxismenu.setIcon(
          IconLoader::load("graph2d-axis-left", IconLoader::LightDark));
      addleftaxismenu.addAction(leftvalueaxis_);
      addleftaxismenu.addAction(leftlogaxis_);
      addleftaxismenu.addAction(leftpiaxis_);
      addleftaxismenu.addAction(lefttextaxis_);
      addleftaxismenu.addAction(lefttimeaxis_);
      addleftaxismenu.addAction(leftdatetimeaxis_);
      menu.addMenu(&addbottomaxismenu);
      addbottomaxismenu.setTitle("Add Bottom Axis");
      addbottomaxismenu.setIcon(
          IconLoader::load("graph2d-axis-bottom", IconLoader::LightDark));
      addbottomaxismenu.addAction(bottomvalueaxis_);
      addbottomaxismenu.addAction(bottomlogaxis_);
      addbottomaxismenu.addAction(bottompiaxis_);
      addbottomaxismenu.addAction(bottomtextaxis_);
      addbottomaxismenu.addAction(bottomtimeaxis_);
      addbottomaxismenu.addAction(bottomdatetimeaxis_);
      menu.addMenu(&addrightaxismenu);
      addrightaxismenu.setTitle("Add Right Axis");
      addrightaxismenu.setIcon(
          IconLoader::load("graph2d-axis-right", IconLoader::LightDark));
      addrightaxismenu.addAction(rightvalueaxis_);
      addrightaxismenu.addAction(rightlogaxis_);
      addrightaxismenu.addAction(rightpiaxis_);
      addrightaxismenu.addAction(righttextaxis_);
      addrightaxismenu.addAction(righttimeaxis_);
      addrightaxismenu.addAction(rightdatetimeaxis_);
      menu.addMenu(&addtopaxismenu);
      addtopaxismenu.setTitle("Add Top Axis");
      addtopaxismenu.setIcon(
          IconLoader::load("graph2d-axis-top", IconLoader::LightDark));
      addtopaxismenu.addAction(topvalueaxis_);
      addtopaxismenu.addAction(toplogaxis_);
      addtopaxismenu.addAction(toppiaxis_);
      addtopaxismenu.addAction(toptextaxis_);
      addtopaxismenu.addAction(toptimeaxis_);
      addtopaxismenu.addAction(topdatetimeaxis_);
      leftvalueaxis_->setData(item->data(0, Qt::UserRole + 1));
      leftlogaxis_->setData(item->data(0, Qt::UserRole + 1));
      leftpiaxis_->setData(item->data(0, Qt::UserRole + 1));
      lefttextaxis_->setData(item->data(0, Qt::UserRole + 1));
      lefttimeaxis_->setData(item->data(0, Qt::UserRole + 1));
      leftdatetimeaxis_->setData(item->data(0, Qt::UserRole + 1));
      bottomvalueaxis_->setData(item->data(0, Qt::UserRole + 1));
      bottomlogaxis_->setData(item->data(0, Qt::UserRole + 1));
      bottompiaxis_->setData(item->data(0, Qt::UserRole + 1));
      bottomtextaxis_->setData(item->data(0, Qt::UserRole + 1));
      bottomtimeaxis_->setData(item->data(0, Qt::UserRole + 1));
      bottomdatetimeaxis_->setData(item->data(0, Qt::UserRole + 1));
      rightvalueaxis_->setData(item->data(0, Qt::UserRole + 1));
      rightlogaxis_->setData(item->data(0, Qt::UserRole + 1));
      rightpiaxis_->setData(item->data(0, Qt::UserRole + 1));
      righttextaxis_->setData(item->data(0, Qt::UserRole + 1));
      righttimeaxis_->setData(item->data(0, Qt::UserRole + 1));
      rightdatetimeaxis_->setData(item->data(0, Qt::UserRole + 1));
      topvalueaxis_->setData(item->data(0, Qt::UserRole + 1));
      toplogaxis_->setData(item->data(0, Qt::UserRole + 1));
      toppiaxis_->setData(item->data(0, Qt::UserRole + 1));
      toptextaxis_->setData(item->data(0, Qt::UserRole + 1));
      toptimeaxis_->setData(item->data(0, Qt::UserRole + 1));
      topdatetimeaxis_->setData(item->data(0, Qt::UserRole + 1));
    } break;
    case PropertyItemType::Axis:
      menu.addAction(removeaxis_);
      removeaxis_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::LSGraph:
      menu.addAction("Edit Data");
      menu.addAction("Analyze");
      menu.addAction(removels_);
      removels_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Curve:
      menu.addAction("Edit Data");
      menu.addAction("Analyze");
      menu.addAction(removecurve_);
      removecurve_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::BarGraph:
      menu.addAction("Edit Data");
      menu.addAction("Analyze");
      menu.addAction(removebar_);
      removebar_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Vector:
      menu.addAction("Edit Data");
      menu.addAction("Analyze");
      menu.addAction(removevector_);
      removevector_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::StatBox:
      menu.addAction(removestatbox_);
      removestatbox_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::TextItem:
      menu.addAction(removetextitem_);
      removetextitem_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::LineItem:
      menu.addAction(removelineitem_);
      removelineitem_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::ImageItem:
      menu.addAction(removeimageitem_);
      removeimageitem_->setData(item->data(0, Qt::UserRole + 1));
      break;
    case PropertyItemType::Grid:
    case PropertyItemType::Legend:
    case PropertyItemType::PieGraph:
    case PropertyItemType::ColorMap:
      break;
  }
  menu.exec(globalPos);
}

void MyTreeWidget::addfunctionplot() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr);
  std::unique_ptr<Function2DDialog> fd(new Function2DDialog(widget_));
  fd->setLayout2DToModify(axisrect, -1);
  fd->setModal(true);
  fd->exec();
}

void MyTreeWidget::addplot() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr);
  std::unique_ptr<AddPlot2DDialog> addplot2d(
      new AddPlot2DDialog(widget_, axisrect));
  addplot2d->setModal(true);
  addplot2d->exec();
}

/*void MyTreeWidget::addAxis2D() {
  AddAxisWidget *ad = new AddAxisWidget(nullptr);
  ad->setAttribute(Qt::WA_DeleteOnClose);
  ad->setModal(true);
  ad->show();

  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr);
  connect(ad, SIGNAL(addleftaxisclicked()), axisrect, SLOT(addLeftAxis2D()));
  connect(ad, SIGNAL(addbottomaxisclicked()), axisrect,
          SLOT(addBottomAxis2D()));
  connect(ad, SIGNAL(addrightaxisclicked()), axisrect, SLOT(addRightAxis2D()));
  connect(ad, SIGNAL(addtopaxisclicked()), axisrect, SLOT(addTopAxis2D()));
  // axisrect->parentPlot()->replot();
}*/

void MyTreeWidget::addAxis2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  AxisRect2D *axisrect = static_cast<AxisRect2D *>(ptr);
  Axis2D *axis = nullptr;

  if (action == leftvalueaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Left,
                               Axis2D::TickerType::Value);
  else if (action == leftlogaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Left,
                               Axis2D::TickerType::Log);
  else if (action == leftpiaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Left,
                               Axis2D::TickerType::Pi);
  else if (action == lefttextaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Left,
                               Axis2D::TickerType::Text);
  else if (action == lefttimeaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Left,
                               Axis2D::TickerType::Time);
  else if (action == leftdatetimeaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Left,
                               Axis2D::TickerType::DateTime);
  else if (action == bottomvalueaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Bottom,
                               Axis2D::TickerType::Value);
  else if (action == bottomlogaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Bottom,
                               Axis2D::TickerType::Log);
  else if (action == bottompiaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Left,
                               Axis2D::TickerType::Pi);
  else if (action == bottomtextaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Bottom,
                               Axis2D::TickerType::Text);
  else if (action == bottomtimeaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Bottom,
                               Axis2D::TickerType::Time);
  else if (action == bottomdatetimeaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Bottom,
                               Axis2D::TickerType::DateTime);
  else if (action == rightvalueaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Right,
                               Axis2D::TickerType::Value);
  else if (action == rightlogaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Right,
                               Axis2D::TickerType::Log);
  else if (action == rightpiaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Right,
                               Axis2D::TickerType::Pi);
  else if (action == righttextaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Right,
                               Axis2D::TickerType::Text);
  else if (action == righttimeaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Right,
                               Axis2D::TickerType::Time);
  else if (action == rightdatetimeaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Right,
                               Axis2D::TickerType::DateTime);
  else if (action == topvalueaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Top,
                               Axis2D::TickerType::Value);
  else if (action == toplogaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Top,
                               Axis2D::TickerType::Log);
  else if (action == toppiaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Top,
                               Axis2D::TickerType::Pi);
  else if (action == toptextaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Top,
                               Axis2D::TickerType::Text);
  else if (action == toptimeaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Top,
                               Axis2D::TickerType::Time);
  else if (action == topdatetimeaxis_)
    axis = axisrect->addAxis2D(Axis2D::AxisOreantation::Top,
                               Axis2D::TickerType::DateTime);
  if (axis)
    axisrect->parentPlot()->replot(
        QCustomPlot::RefreshPriority::rpQueuedRefresh);
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
  axisrect->parentPlot()->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}

void MyTreeWidget::removeLineSpecial2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  LineSpecial2D *ls = static_cast<LineSpecial2D *>(ptr);
  QCustomPlot *customplot = ls->parentPlot();
  bool result =
      ls->getxaxis_lsplot()->getaxisrect_axis()->removeLineSpecial2D(ls);
  if (!result) {
    qDebug() << "unable to remove line special 2d plot";
    return;
  }
  customplot->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}

void MyTreeWidget::removeCurve2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Curve2D *curve = static_cast<Curve2D *>(ptr);
  QCustomPlot *customplot = curve->parentPlot();
  bool result =
      curve->getxaxis_cplot()->getaxisrect_axis()->removeCurve2D(curve);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
    return;
  }
  customplot->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}

void MyTreeWidget::removeBar2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Bar2D *bar = static_cast<Bar2D *>(ptr);
  QCustomPlot *customplot = bar->parentPlot();
  bool result = bar->getxaxis_barplot()->getaxisrect_axis()->removeBar2D(bar);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
    return;
  }
  customplot->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}

void MyTreeWidget::removeVector2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Vector2D *vector = static_cast<Vector2D *>(ptr);
  QCustomPlot *customplot = vector->parentPlot();
  bool result =
      vector->getxaxis_vecplot()->getaxisrect_axis()->removeVector2D(vector);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
    return;
  }
  customplot->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}

void MyTreeWidget::removeStatBox2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  StatBox2D *statbox = static_cast<StatBox2D *>(ptr);
  QCustomPlot *customplot = statbox->parentPlot();
  bool result =
      statbox->getxaxis_statbox()->getaxisrect_axis()->removeStatBox2D(statbox);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
    return;
  }
  customplot->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}

void MyTreeWidget::removeTextItem2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  TextItem2D *textitem = static_cast<TextItem2D *>(ptr);
  QCustomPlot *customplot = textitem->parentPlot();
  bool result = textitem->getaxisrect_textitem()->removeTextItem2D(textitem);
  if (!result) {
    qDebug() << "unable to remove line special 2d plot";
    return;
  }
  customplot->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}

void MyTreeWidget::removeLineItem2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  LineItem2D *lineitem = static_cast<LineItem2D *>(ptr);
  QCustomPlot *customplot = lineitem->parentPlot();
  bool result = lineitem->getaxisrect_lineitem()->removeLineItem2D(lineitem);
  if (!result) {
    qDebug() << "unable to remove line special 2d plot";
    return;
  }
  customplot->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}

void MyTreeWidget::removeImageItem2D() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  ImageItem2D *imageitem = static_cast<ImageItem2D *>(ptr);
  QCustomPlot *customplot = imageitem->parentPlot();
  bool result =
      imageitem->getaxisrect_imageitem()->removeImageItem2D(imageitem);
  if (!result) {
    qDebug() << "unable to remove line special 2d plot";
    return;
  }
  customplot->replot(QCustomPlot::RefreshPriority::rpQueuedRefresh);
}
