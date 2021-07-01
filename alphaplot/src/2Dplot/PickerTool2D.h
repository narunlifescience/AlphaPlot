#ifndef PICKERTOOL2D_H
#define PICKERTOOL2D_H

#include "Layout2D.h"

class PickerTool2D : public QObject {
  Q_OBJECT
 public:
  explicit PickerTool2D(Layout2D *parent);
  ~PickerTool2D();

  // setters
  void setPicker(const Graph2DCommon::Picker &picker);
  void setRangePickerCurve(Curve2D *curve);

  // getters
  Graph2DCommon::Picker getPicker() const;
  Curve2D *getRangePickerCurve();
  QPair<double, double> getRangePickerLower() const;
  QPair<double, double> getRangePickerUpper() const;

 public slots:
  void showtooltip(const QPointF position, const double xval, const double yval,
                   Axis2D *xaxis, Axis2D *yaxis);
  // drag range
  void datarangemousepress(Curve2D *curve, const double xval,
                           const double yval);
  void datarangelinedrag(const double xval, const double yval);
  void datarangemouserelease(const QPointF position);

 private:
  struct DatarangePicker {
    Curve2D *curve;
    QCPItemStraightLine *line;
    double lowerx;
    double lowery;
    double upperx;
    double uppery;
    bool active;
    DatarangePicker()
        : curve(nullptr),
          line(nullptr),
          lowerx(0.0),
          lowery(0.0),
          upperx(0.0),
          uppery(0.0),
          active(false) {}
  };
  void setupRangepicker();
  void removePickerLinesAndEllipses();
  void resetDataRangePicker();
  void moveLineEllipseItenTo(double xval, double yval, bool active);

  Layout2D *layout_;
  Curve2D *curve_;
  Graph2DCommon::Picker picker_;
  QCPItemStraightLine *xpickerline_;
  QCPItemStraightLine *ypickerline_;
  QCPItemEllipse *xpickerellipse_;
  QCPItemEllipse *ypickerellipse_;
  DatarangePicker rangepicker_;
  static const int ellipseradius_;
};

#endif  // PICKERTOOL2D_H
