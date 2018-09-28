#include "MyTreeWidget.h"

#include <memory>

#include "../Axis2D.h"
#include "../AxisRect2D.h"
#include "AddAxisWidget.h"
#include "AddPlot2DDialog.h"
#include "Function2DDialog.h"
#include "core/IconLoader.h"

MyTreeWidget::MyTreeWidget(QWidget *parent)
    : QTreeWidget(parent),
      widget_(parent),
      addgraph_(new QAction("Add", this)),
      addfunctiongraph_(new QAction("Add", this)),
      addaxis_(new QAction("Add", this)),
      removeaxis_(new QAction("Remove", this)),
      removels_(new QAction("Remove", this)),
      removespline_(new QAction("Remove", this)),
      removecurve_(new QAction("Remove", this)),
      removebar_(new QAction("Remove", this)),
      removevector_(new QAction("Remove", this)),
      removepie_(new QAction("Remove", this)) {
  setContextMenuPolicy(Qt::CustomContextMenu);

  addgraph_->setText("Add Plot...");
  addfunctiongraph_->setText("Add Function Plot...");
  addaxis_->setText("Add Axis");
  removeaxis_->setText("Remove");
  removels_->setText("Remove");
  removespline_->setText("Remove");
  removecurve_->setText("Remove");
  removebar_->setText("Remove");
  removevector_->setText("Remove");
  removepie_->setText("Remove");

  addgraph_->setIcon(IconLoader::load("edit-add-graph", IconLoader::LightDark));
  addfunctiongraph_->setIcon(
      IconLoader::load("math-fofx", IconLoader::LightDark));
  addaxis_->setIcon(
      IconLoader::load("graph2d-axis-left", IconLoader::LightDark));
  removeaxis_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removels_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removespline_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removecurve_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removebar_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));
  removevector_->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  removepie_->setIcon(IconLoader::load("clear-loginfo", IconLoader::General));

  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          SLOT(showContextMenu(const QPoint &)));
  connect(addfunctiongraph_, SIGNAL(triggered(bool)), this,
          SLOT(addfunctiongraph()));
  connect(addgraph_, SIGNAL(triggered(bool)), this, SLOT(addplot()));
  connect(addaxis_, SIGNAL(triggered(bool)), this, SLOT(addaxis()));
  connect(removeaxis_, SIGNAL(triggered(bool)), this, SLOT(removeaxis()));
  connect(removels_, SIGNAL(triggered(bool)), this, SLOT(removels()));
  connect(removespline_, SIGNAL(triggered(bool)), this, SLOT(removespline()));
  connect(removecurve_, SIGNAL(triggered(bool)), this, SLOT(removecurve()));
  connect(removebar_, SIGNAL(triggered(bool)), this, SLOT(removebar()));
  // connect(removevector_, SIGNAL(triggered(bool)), this,
  // SLOT(removevector())); connect(removepie_, SIGNAL(triggered(bool)), this,
  // SLOT(removepie()));
}

void MyTreeWidget::showContextMenu(const QPoint &pos) {
  QTreeWidgetItem *item = itemAt(pos);
  if (!item) return;

  QPoint globalPos = viewport()->mapToGlobal(pos);
  QMenu menu;

  switch (static_cast<PropertyItemType>(item->data(0, Qt::UserRole).toInt())) {
    case PropertyItemType::Layout:
      menu.addAction(addgraph_);
      addgraph_->setData(item->data(0, Qt::UserRole + 1));
      menu.addAction(addfunctiongraph_);
      addfunctiongraph_->setData(item->data(0, Qt::UserRole + 1));
      menu.addAction(addaxis_);
      addaxis_->setData(item->data(0, Qt::UserRole + 1));
      // const char *l = "cool";
      // QAction addaxis = QAction("cool", this);
      // menu.addAction("Add Axis", test(item), this);
      // menu.addAction("", )
      break;

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
    case PropertyItemType::Spline:
      menu.addAction("Edit Data");
      menu.addAction("Analyze");
      menu.addAction(removespline_);
      removespline_->setData(item->data(0, Qt::UserRole + 1));
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
      menu.addAction(removebar_);
      break;

    case PropertyItemType::Grid:
      break;
  }

  menu.exec(globalPos);
}

void MyTreeWidget::addfunctiongraph() {
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

void MyTreeWidget::addaxis() {
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
}

void MyTreeWidget::removeaxis() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Axis2D *axis = static_cast<Axis2D *>(ptr);
  AxisRect2D *axisrect = axis->getaxisrect_axis();
  axisrect->removeAxis2D(axis);
}

void MyTreeWidget::removels() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  LineScatter2D *ls = static_cast<LineScatter2D *>(ptr);
  bool result =
      ls->getxaxis_lsplot()->getaxisrect_axis()->removeLineScatter2D(ls);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
  }
}

void MyTreeWidget::removecurve() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Curve2D *curve = static_cast<Curve2D *>(ptr);
  bool result =
      curve->getxaxis_cplot()->getaxisrect_axis()->removeCurve2D(curve);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
  }
}

void MyTreeWidget::removespline() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Spline2D *spline = static_cast<Spline2D *>(ptr);
  bool result =
      spline->getxaxis_splot()->getaxisrect_axis()->removeSpline2D(spline);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
  }
}

void MyTreeWidget::removebar() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (!action) return;
  void *ptr = action->data().value<void *>();
  Bar2D *bar = static_cast<Bar2D *>(ptr);
  bool result = bar->getxaxis_barplot()->getaxisrect_axis()->removeBar2D(bar);
  if (!result) {
    qDebug() << "unable to remove line scatter 2d plot";
  }
}
