#ifndef TEXTITEM2D_H
#define TEXTITEM2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "AxisRect2D.h"
#include "Plot2D.h"

class TextItem2D : public QCPItemText {
 public:
  TextItem2D(AxisRect2D *axisrect, Plot2D *plot);
  ~TextItem2D();

  enum class TextAlignment : int {
    TopLeft = 0,
    TopCenter = 1,
    TopRight = 2,
    CenterLeft = 3,
    CenterCenter = 4,
    CenterRight = 5,
    BottomLeft = 6,
    BottomCenter = 7,
    BottomRight = 8
  };

  QColor getstrokecolor_textitem() const;
  double getstrokethickness_textitem() const;
  Qt::PenStyle getstrokestyle_textitem() const;
  TextAlignment gettextalignment_textitem() const;

  void setstrokecolor_textitem(const QColor &color);
  void setstrokethickness_textitem(const double value);
  void setstrokestyle_textitem(const Qt::PenStyle &style);
  void settextalignment_textitem(const TextAlignment &value);

 protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);
  void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
  void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos);

 private:
  AxisRect2D *axisrect_;
  bool draggingtextitem_;
  QPointF dragtextitemorigin_;
  TextAlignment textalignment_;
};

#endif  // TEXTITEM2D_H
