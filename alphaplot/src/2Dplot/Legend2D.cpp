#include "Legend2D.h"
#include "LineSpecial2D.h"

Legend2D::Legend2D(AxisRect2D *axisrect)
    : QCPLegend(), axisrect_(axisrect), draggingLegend_(false) {}

Legend2D::~Legend2D() {}

bool Legend2D::gethidden_legend() const { return layer()->visible(); }

QColor Legend2D::getborderstrokecolor_legend() const {
  return borderPen().color();
}

double Legend2D::getborderstrokethickness_legend() const {
  return borderPen().widthF();
}

Qt::PenStyle Legend2D::getborderstrokestyle_legend() const {
  return borderPen().style();
}

void Legend2D::sethidden_legend(const bool status) { setVisible(status); }

void Legend2D::setborderstrokecolor_legend(const QColor &color) {
  QPen p = borderPen();
  p.setColor(color);
  setBorderPen(p);
}

void Legend2D::setborderstrokethickness_legend(const double value) {
  QPen p = borderPen();
  p.setWidthF(value);
  setBorderPen(p);
}

void Legend2D::setborderstrokestyle_legend(const Qt::PenStyle &style) {
  QPen p = borderPen();
  p.setStyle(style);
  setBorderPen(p);
}

void Legend2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  Q_UNUSED(details);
  emit legendClicked();
  if (event->button() == Qt::LeftButton) {
    if (selectTest(event->pos(), false) > 0) {
      draggingLegend_ = true;
      // since insetRect is in axisRect coordinates (0..1), we transform the
      // mouse position:
      QPointF mousePoint((event->pos().x() - axisrect_->left()) /
                             static_cast<double>(axisrect_->width()),
                         (event->pos().y() - axisrect_->top()) /
                             static_cast<double>(axisrect_->height()));
      dragLegendOrigin_ =
          mousePoint - axisrect_->insetLayout()->insetRect(0).topLeft();
      parentPlot()->setCursor(Qt::ClosedHandCursor);
    }
  }
}

void Legend2D::mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) {
  Q_UNUSED(startPos);
  // dragging legend
  if (draggingLegend_) {
    QRectF rect = axisrect_->insetLayout()->insetRect(0);
    // QRect r = rect();

    // set bounding rect for drag event
    QPoint eventpos = event->pos();
    if (event->pos().x() < axisrect_->left()) {
      eventpos.setX(axisrect_->left());
    }
    if (event->pos().x() > axisrect_->right()) {
      eventpos.setX(axisrect_->right());
    }
    if (event->pos().y() < axisrect_->top()) {
      eventpos.setY(axisrect_->top());
    }
    if (event->pos().y() > axisrect_->bottom()) {
      eventpos.setY(axisrect_->bottom());
    }

    // since insetRect is in axisRect coordinates (0..1), we transform the mouse
    // position:
    QPointF mousePoint((eventpos.x() - axisrect_->left()) /
                           static_cast<double>(axisrect_->width()),
                       (eventpos.y() - axisrect_->top()) /
                           static_cast<double>(axisrect_->height()));

    rect.moveTopLeft(mousePoint - dragLegendOrigin_);
    axisrect_->insetLayout()->setInsetRect(0, rect);
    layer()->replot();
    event->accept();
  }
}

void Legend2D::mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos) {
  Q_UNUSED(startPos);
  if (event->button() == Qt::LeftButton) {
    if (draggingLegend_) {
      draggingLegend_ = false;
      parentPlot()->unsetCursor();
    }
  }
}

LegendItem2D::LegendItem2D(Legend2D *legend, QCPAbstractPlottable *plottable)
    : QCPPlottableLegendItem(legend, plottable) {}

LegendItem2D::~LegendItem2D() {}

void LegendItem2D::mousePressEvent(QMouseEvent *event,
                                   const QVariant &details) {
  emit legendItemClicked();
  QCPPlottableLegendItem::mousePressEvent(event, details);
}
