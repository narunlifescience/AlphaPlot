#include "ControlWidget.h"

#include <QResizeEvent>

ControlWidget::ControlWidget(QWidget *parent) : QWidget(parent) {}

void ControlWidget::resizeEvent(QResizeEvent *event) {
  if (event->oldSize().width() != event->size().width()) emit widthChanged();
  QWidget::resizeEvent(event);
}
