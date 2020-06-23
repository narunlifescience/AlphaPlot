#include "LayoutButton2D.h"

#include <QMouseEvent>
#include <QPainter>
#include <QSize>

const int ToolButton::layoutButtonSize_ = 22;
QRect LayoutButton2D::highLightRect_;
const QColor LayoutButton2D::highlightColor_ = QColor(255, 59, 59, 255);

LayoutButton2D::LayoutButton2D(const QPair<int, int> rowcol, QWidget *parent)
    : ToolButton(parent),
      active_(false),
      rowcol_(rowcol),
      buttonText_(
          QString("%1,%2").arg(rowcol.first + 1).arg(rowcol.second + 1)) {
  setCheckable(true);
  setFixedWidth(height() * 1.5);
}

LayoutButton2D::~LayoutButton2D() {}

int LayoutButton2D::btnSize() { return height(); }

QPair<int, int> LayoutButton2D::getRowCol() const { return rowcol_; }

void LayoutButton2D::resetRowCol(const QPair<int, int> rowcol) {
  buttonText_ = QString("%1,%2").arg(rowcol.first + 1).arg(rowcol.second + 1);
  repaint();
}

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
  QToolButton::paintEvent(event);
  if (active_) {
    QPainter painter(this);
    painter.setPen(QPen(highlightColor_));
    painter.drawRect(highLightRect_);
    painter.drawText(highLightRect_, Qt::AlignCenter, buttonText_);
  } else {
    QPainter painter(this);
    painter.drawText(highLightRect_, Qt::AlignCenter, buttonText_);
  }
}

ToolButton::ToolButton(QWidget *parent) : QToolButton(parent) {
  setMaximumWidth(LayoutButton2D::btnSize());
  setMaximumHeight(LayoutButton2D::btnSize());
  setFixedSize(QSize(layoutButtonSize_, layoutButtonSize_));
  setFocusPolicy(Qt::NoFocus);
}

ToolButton::~ToolButton() {}
