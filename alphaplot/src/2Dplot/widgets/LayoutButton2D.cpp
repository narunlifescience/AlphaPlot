#include "LayoutButton2D.h"

#include <QMouseEvent>
#include <QPainter>

const int LayoutButton2D::layoutButtonSize_ = 22;
QRect LayoutButton2D::highLightRect_;

LayoutButton2D::LayoutButton2D(const QString &text, QWidget *parent)
    : QPushButton(parent), active_(false), buttonText_(text) {
  setToggleButton(true);
  setMaximumWidth(LayoutButton2D::btnSize());
  setMaximumHeight(LayoutButton2D::btnSize());
  setFixedSize(QSize(layoutButtonSize_, layoutButtonSize_));
}

LayoutButton2D::~LayoutButton2D() {}

int LayoutButton2D::btnSize() { return layoutButtonSize_; }

void LayoutButton2D::setActive(bool status) {
  active_ = status;
  repaint();
}

bool LayoutButton2D::isActive() { return active_; }

void LayoutButton2D::mousePressEvent(QMouseEvent *event) {
  emit clicked(this);
  if (!active_) {
    emit clicked(this);
  }
  if (event->button() == Qt::RightButton) {
    emit showContextMenu();
  }
}

void LayoutButton2D::mouseDoubleClickEvent(QMouseEvent *) {
  emit showCurvesDialog();
}

void LayoutButton2D::resizeEvent(QResizeEvent *) {
  highLightRect_ = QRect(2, 2, size().rwidth() - 5, size().rheight() - 5);
}

void LayoutButton2D::paintEvent(QPaintEvent *event) {
  QPushButton::paintEvent(event);
  if (active_) {
    QPainter painter(this);
    painter.setPen(QPen(QColor(255,59,59, 255)));
    painter.drawRect(highLightRect_);
    painter.drawText(highLightRect_, Qt::AlignCenter, buttonText_);
  } else {
    QPainter painter(this);
    painter.drawText(highLightRect_, Qt::AlignCenter, buttonText_);
  }
}
