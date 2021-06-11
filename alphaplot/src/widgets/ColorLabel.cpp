#include "ColorLabel.h"

#include <QApplication>

const int ColorLabel::lbl_line_width = 1;
const int ColorLabel::btn_size = 24;

ColorLabel::ColorLabel(QWidget *parent) : QLabel("", parent), color_(Qt::gray) {
  setFixedSize(btn_size, btn_size);
  (height() > width()) ? setFixedWidth(height()) : setFixedHeight(width());
}

ColorLabel::~ColorLabel() {}

void ColorLabel::setColor(const QColor &color) {
  color_ = color;
  QString stylesheetstring =
      "QLabel:!disabled{background : rgba(%1,%2, %3, %4); border: %5px solid "
      "rgba(%6, %7, %8, %9);}";
  QColor bordercolor = qApp->palette().windowText().color();
  setStyleSheet(stylesheetstring.arg(color.red())
                    .arg(color_.green())
                    .arg(color_.blue())
                    .arg(color_.alpha())
                    .arg(lbl_line_width)
                    .arg(bordercolor.red())
                    .arg(bordercolor.green())
                    .arg(bordercolor.blue())
                    .arg(bordercolor.alpha()));
}

QColor ColorLabel::getColor() const { return color_; }
