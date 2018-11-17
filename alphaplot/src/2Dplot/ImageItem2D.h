#ifndef IMAGEITEM2D_H
#define IMAGEITEM2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "AxisRect2D.h"

class ImageItem2D : public QCPItemPixmap {
 public:
  ImageItem2D(AxisRect2D *axisrect, Plot2D *plot, const QString &filename);
  ~ImageItem2D();

  AxisRect2D *getaxisrect_imageitem() const;
  QString getsource_imageitem() const;
  QColor getstrokecolor_imageitem() const;
  double getstrokethickness_imageitem() const;
  Qt::PenStyle getstrokestyle_imageitem() const;

  void setstrokecolor_imageitem(const QColor &color);
  void setstrokethickness_imageitem(const double value);
  void setstrokestyle_imageitem(const Qt::PenStyle &style);

  void setpixmap_imageitem();

 protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);
  void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
  void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos);

 private:
  const static int selectionpixelsize_;
  AxisRect2D *axisrect_;
  bool draggingimageitem_;
  QPointF dragimageitemorigin_;
  QString imagefilename_;
  QPixmap *pixmap_;
};

#endif  // IMAGEITEM2D_H
