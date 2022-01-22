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
  setStyleSheet(stylesheetstring.arg(QString::number(color_.red()),
                    QString::number(color_.green()),
                    QString::number(color_.blue()),
                    QString::number(color_.alpha()),
                    QString::number(lbl_line_width),
                    QString::number(bordercolor.red()),
                    QString::number(bordercolor.green()),
                    QString::number(bordercolor.blue()),
                    QString::number(bordercolor.alpha())));
}

QColor ColorLabel::getColor() const { return color_; }
