#include "Axis2DPropertiesDialog.h"
#include "ui_Axis2DPropertiesDialog.h"

#include <QDebug>

Axis2DPropertiesDialog::Axis2DPropertiesDialog(QWidget *parent, QCPAxis *axis,
                                               AxisRect2D *axisRect)
    : QDialog(parent),
      ui_(new Ui_Axis2DPropertiesDialog),
      selectedAxis_(nullptr),
      selectedAxisRect_(axisRect) {
  ui_->setupUi(this);

  leftAxisRootItem_ = new QTreeWidgetItem(ui_->axisTreeWidget);
  bottomAxisRootItem_ = new QTreeWidgetItem(ui_->axisTreeWidget);
  rightAxisRootItem_ = new QTreeWidgetItem(ui_->axisTreeWidget);
  topAxisRootItem_ = new QTreeWidgetItem(ui_->axisTreeWidget);
  populateAxisTreeWidget();
  ui_->axisTreeWidget->setColumnCount(1);

  connect(ui_->axisTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
          SLOT(axisItemActivated(QTreeWidgetItem *)));

  connect(ui_->applyPushButton, SIGNAL(clicked()), SLOT(apply()));
  connect(ui_->okPushButton, SIGNAL(clicked()), SLOT(applyAndClose()));
  connect(ui_->cancelPushButton, SIGNAL(clicked()), SLOT(close()));
}

Axis2DPropertiesDialog::~Axis2DPropertiesDialog() { delete ui_; }

void Axis2DPropertiesDialog::apply() {
  if (!selectedAxis_) return;

  selectedAxis_->setRange(QCPRange(ui_->axisFromDoubleSpinBox->value(),
                                   ui_->axisToDoubleSpinBox->value()));

  if (ui_->axisTypeComboBox->currentIndex() == 0) {
    selectedAxis_->setScaleType(QCPAxis::stLinear);
  } else {
    selectedAxis_->setScaleType(QCPAxis::stLogarithmic);
  }

  selectedAxis_->setRangeReversed(ui_->axisInvertedCheckBox->isChecked());
  emit areplot();
}

void Axis2DPropertiesDialog::applyAndClose() {
  apply();
  close();
}

void Axis2DPropertiesDialog::axisItemActivated(QTreeWidgetItem *item) {
  if (item->isSelected()) {
    if (item->parent() == leftAxisRootItem_) {
      selectedAxis_ = selectedAxisRect_->getAxesOrientedTo(Axis2D::Left)
                          .at(item->parent()->indexOfChild(item));
    } else if (item->parent() == bottomAxisRootItem_) {
      selectedAxis_ = selectedAxisRect_->getAxesOrientedTo(Axis2D::Bottom)
                          .at(item->parent()->indexOfChild(item));
    } else if (item->parent() == rightAxisRootItem_) {
      selectedAxis_ = selectedAxisRect_->getAxesOrientedTo(Axis2D::Right)
                          .at(item->parent()->indexOfChild(item));
    } else if (item->parent() == topAxisRootItem_) {
      selectedAxis_ = selectedAxisRect_->getAxesOrientedTo(Axis2D::Top)
                          .at(item->parent()->indexOfChild(item));
    }
    populateAxisValue();
  }
}

void Axis2DPropertiesDialog::populateAxisTreeWidget() {
  // Left Axes
  int count = 0;
  leftAxisRootItem_->setText(0, tr("Left Axes"));
  leftAxisRootItem_->setIcon(0, QPixmap(":/image5.xpm"));
  count = selectedAxisRect_->getAxesOrientedTo(Axis2D::Left).count();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *leftAxisChildItem = new QTreeWidgetItem();
    leftAxisRootItem_->addChild(leftAxisChildItem);
    leftAxisChildItem->setText(0, QString::number(i) + " - Y:Axis");
  }

  // Bottom Axes
  count = 0;
  bottomAxisRootItem_->setText(0, tr("Bottom Axes"));
  bottomAxisRootItem_->setIcon(0, QPixmap(":/image4.xpm"));
  count = selectedAxisRect_->getAxesOrientedTo(Axis2D::Bottom).size();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *bottomAxisChildItem = new QTreeWidgetItem();
    bottomAxisRootItem_->addChild(bottomAxisChildItem);
    bottomAxisChildItem->setText(0, QString::number(i) + " - X:Axis");
  }

  // Right Axes
  count = 0;
  rightAxisRootItem_->setText(0, tr("Right Axes"));
  rightAxisRootItem_->setIcon(0, QPixmap(":/image7.xpm"));
  count = selectedAxisRect_->getAxesOrientedTo(Axis2D::Right).count();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *rightAxisChildItem = new QTreeWidgetItem();
    rightAxisRootItem_->addChild(rightAxisChildItem);
    rightAxisChildItem->setText(0, QString::number(i) + " - Y:Axis");
  }

  // Top Axes
  count = 0;
  topAxisRootItem_->setText(0, tr("Top Axes"));
  topAxisRootItem_->setIcon(0, QPixmap(":/image6.xpm"));
  count = selectedAxisRect_->getAxesOrientedTo(Axis2D::Top).count();
  for (int i = 0; i < count; i++) {
    QTreeWidgetItem *topAxisChildItem = new QTreeWidgetItem();
    topAxisRootItem_->addChild(topAxisChildItem);
    topAxisChildItem->setText(0, QString::number(i) + " - X:Axis");
  }

  // add all top level axis items
  ui_->axisTreeWidget->addTopLevelItem(leftAxisRootItem_);
  ui_->axisTreeWidget->addTopLevelItem(bottomAxisRootItem_);
  ui_->axisTreeWidget->addTopLevelItem(rightAxisRootItem_);
  ui_->axisTreeWidget->addTopLevelItem(topAxisRootItem_);
  leftAxisRootItem_->setFlags(Qt::ItemIsEnabled);
  bottomAxisRootItem_->setFlags(Qt::ItemIsEnabled);
  rightAxisRootItem_->setFlags(Qt::ItemIsEnabled);
  topAxisRootItem_->setFlags(Qt::ItemIsEnabled);
  leftAxisRootItem_->setExpanded(true);
  bottomAxisRootItem_->setExpanded(true);
  rightAxisRootItem_->setExpanded(true);
  topAxisRootItem_->setExpanded(true);
}

void Axis2DPropertiesDialog::populateAxisValue() {
  ui_->axisFromDoubleSpinBox->setValue(selectedAxis_->range().lower);
  ui_->axisToDoubleSpinBox->setValue(selectedAxis_->range().upper);

  switch (selectedAxis_->scaleType()) {
    case QCPAxis::stLinear:
      ui_->axisTypeComboBox->setCurrentIndex(0);
      break;
    case QCPAxis::stLogarithmic:
      ui_->axisTypeComboBox->setCurrentIndex(1);
      break;
  }

  ui_->axisInvertedCheckBox->setChecked(selectedAxis_->rangeReversed());
  ui_->majorTicksSpinBox->setValue(selectedAxis_->subTickCount());
  ui_->minorTicksSpinBox->setValue(selectedAxis_->subTickCount());
}
