#include "AddAxisWidget.h"
#include "ui_AddAxisWidget.h"

AddAxisWidget::AddAxisWidget(QWidget *parent)
    : QDialog(parent), ui_(new Ui_AddAxisWidget) {
  ui_->setupUi(this);
  ui_->addleftaxis->setIcon(QPixmap(":/image5.xpm"));
  ui_->addbottomaxis->setIcon(QPixmap(":/image4.xpm"));
  ui_->addrightaxis->setIcon(QPixmap(":/image7.xpm"));
  ui_->addtopaxis->setIcon(QPixmap(":/image6.xpm"));
  connect(ui_->addleftaxis, SIGNAL(clicked()), SLOT(leftaxis()));
  connect(ui_->addbottomaxis, SIGNAL(clicked()), SLOT(bottomaxis()));
  connect(ui_->addrightaxis, SIGNAL(clicked()), SLOT(rightaxis()));
  connect(ui_->addtopaxis, SIGNAL(clicked()), SLOT(topaxis()));
}

AddAxisWidget::~AddAxisWidget() { delete ui_; }

void AddAxisWidget::leftaxis() {
  emit addleftaxisclicked();
  close();
}

void AddAxisWidget::bottomaxis() {
  emit addbottomaxisclicked();
  close();
}

void AddAxisWidget::rightaxis() {
  emit addrightaxisclicked();
  close();
}

void AddAxisWidget::topaxis() {
  emit addtopaxisclicked();
  close();
}
