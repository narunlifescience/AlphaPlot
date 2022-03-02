#ifndef IMAGEITEM2D_H
#define IMAGEITEM2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "core/propertybrowser/ObjectBrowserTreeItem.h"

class AxisRect2D;
class Plot2D;
class XmlStreamWriter;
class XmlStreamReader;

class ImageItem2D : public QCPItemPixmap, public ObjectBrowserTreeItem {
  Q_OBJECT
 public:
  ImageItem2D(ObjectBrowserTreeItem *parentitem, AxisRect2D *axisrect,
              Plot2D *plot, const QString &filename);
  ~ImageItem2D();

  virtual QString getItemName() override;
  virtual QIcon getItemIcon() override;
  virtual QString getItemTooltip() override;

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
  void draw(QCPPainter *painter) override;
  void mousePressEvent(QMouseEvent *event, const QVariant &details) override;
  void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) override;
  void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos) override;

 private:
  ObjectBrowserTreeItem *parentitem_;
  const static int selectionpixelsize_;
  AxisRect2D *axisrect_;
  QString layername_;
  bool imageitemclicked_;
  QCPItemAnchor *dragginganchorimageitem_;
  bool draggingimageitem_;
  QPointF dragimageitemorigintopleft_;
  QPointF dragimageitemoriginbottomright_;
  QString imagefilename_;
  QPixmap *pixmap_;
  int rotation_;
  QCursor cursorshape_;
};

Q_DECLARE_METATYPE(ImageItem2D *);
#endif  // IMAGEITEM2D_H
