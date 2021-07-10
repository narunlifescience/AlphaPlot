#ifndef IMAGEITEM2D_H
#define IMAGEITEM2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"

class AxisRect2D;
class Plot2D;
class XmlStreamWriter;
class XmlStreamReader;

class ImageItem2D : public QCPItemPixmap {
 public:
  ImageItem2D(AxisRect2D *axisrect, Plot2D *plot, const QString &filename);
  ~ImageItem2D();

  AxisRect2D *getaxisrect() const;
  QString getsource_imageitem() const;
  int getrotation_imageitem() const;
  QColor getstrokecolor_imageitem() const;
  double getstrokethickness_imageitem() const;
  Qt::PenStyle getstrokestyle_imageitem() const;
  QPointF getposition_imageitem() const;

  void setstrokecolor_imageitem(const QColor &color);
  void setstrokethickness_imageitem(const double value);
  void setstrokestyle_imageitem(const Qt::PenStyle &style);
  void setposition_imageitem(const QPointF origin);

  void setpixmap_imageitem();
  void setrotation_imageitem(int degree);

  void save(XmlStreamWriter *xmlwriter);
  bool load(XmlStreamReader *xmlreader);

 protected:
  void draw(QCPPainter *painter);
  void mousePressEvent(QMouseEvent *event, const QVariant &details);
  void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
  void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos);

 private:
  const static int selectionpixelsize_;
  AxisRect2D *axisrect_;
  QString layername_;
  bool imageitemclicked_ ;
  QCPItemAnchor *dragginganchorimageitem_ ;
  bool draggingimageitem_;
  QPointF dragimageitemorigintopleft_;
  QPointF dragimageitemoriginbottomright_;
  QString imagefilename_;
  QPixmap *pixmap_;
  int rotation_;
  QCursor cursorshape_;
};

#endif  // IMAGEITEM2D_H
