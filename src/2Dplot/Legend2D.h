#ifndef LEGEND2D_H
#define LEGEND2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "AxisRect2D.h"

class LineSpecial2D;

class Legend2D : public QCPLegend {
  Q_OBJECT
 public:
  Legend2D(AxisRect2D *axisrect);
  ~Legend2D();

  bool gethidden_legend() const;
  QColor getborderstrokecolor_legend() const;
  double getborderstrokethickness_legend() const;
  Qt::PenStyle getborderstrokestyle_legend() const;
  QPointF getposition_legend() const;
  AxisRect2D *getaxisrect_legend() const;

  void sethidden_legend(const bool status);
  void setborderstrokecolor_legend(const QColor &color);
  void setborderstrokethickness_legend(const double value);
  void setborderstrokestyle_legend(const Qt::PenStyle &style);
  void setposition_legend(QPointF origin);

  void save(XmlStreamWriter *xmlwriter);
  bool load(XmlStreamReader *xmlreader);

 signals:
  void legendClicked();
  void legendMoved();

 protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);
  void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
  void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos);

 private:
  AxisRect2D *axisrect_;
  bool draggingLegend_;
  QPointF dragLegendOrigin_;
  Qt::CursorShape cursorshape_;
};

class LegendItem2D : public QCPPlottableLegendItem {
  Q_OBJECT
 public:
  LegendItem2D(Legend2D *legend, QCPAbstractPlottable *plottable);
  ~LegendItem2D();

 signals:
  void legendItemClicked();

 protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);
};

class SplineLegendItem2D : public LegendItem2D {
  Q_OBJECT
 public:
  SplineLegendItem2D(Legend2D *legend, Curve2D *plottable);
  ~SplineLegendItem2D();

 protected:
  void draw(QCPPainter *painter);

 private:
  bool isspline_;
  Curve2D *curve_;
  Legend2D *legend_;
};

class VectorLegendItem2D : public LegendItem2D {
  Q_OBJECT
 public:
  VectorLegendItem2D(Legend2D *legend, Vector2D *plottable);
  ~VectorLegendItem2D();

 protected:
  void draw(QCPPainter *painter);

 private:
  void setLineEndings();

 private:
  Vector2D *vector_;
  Legend2D *legend_;
  QCPItemLine *lineitem_;
  QCPLineEnding *start_;
  QCPLineEnding *stop_;
};

class PieLegendItem2D : public QCPAbstractLegendItem {
  Q_OBJECT
 public:
  PieLegendItem2D(Legend2D *parent, QColor color, const QString &string);

 protected:
  QColor color_;
  QString string_;
  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
  virtual QSize minimumOuterSizeHint() const Q_DECL_OVERRIDE;

  // non-virtual methods:
  QPen getIconBorderPen() const;
  QColor getTextColor() const;
  QFont getFont() const;
};

#endif  // LEGEND2D_H
