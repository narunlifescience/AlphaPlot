#include "Legend2D.h"
#include "LineSpecial2D.h"

Legend2D::Legend2D() : QCPLegend() {}

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

void Legend2D::sethidden_legend(const bool status) {
  layer()->setVisible(status);
}

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

void Legend2D::mousePressEvent(QMouseEvent *) { emit legendClicked(); }

LegendItem2D::LegendItem2D(Legend2D *legend, QCPAbstractPlottable *plottable)
    : QCPPlottableLegendItem(legend, plottable) {}

LegendItem2D::~LegendItem2D() {}

void LegendItem2D::mousePressEvent(QMouseEvent *) { emit legendItemClicked(); }
