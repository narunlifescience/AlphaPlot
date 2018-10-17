#ifndef SPLINE_H
#define SPLINE_H

#include <QPolygon>
#include <QVector>

class Spline {
 public:
  //! Spline type
  enum SplineType { Natural, Periodic };

  Spline();
  Spline(const Spline &);

  ~Spline();

  Spline &operator=(const Spline &);

  void setSplineType(SplineType);
  SplineType splineType() const;

  bool setPoints(const QPolygonF &points);
  QPolygonF points() const;

  void reset();

  bool isValid() const;
  double value(double x) const;

  const QVector<double> &coefficientsA() const;
  const QVector<double> &coefficientsB() const;
  const QVector<double> &coefficientsC() const;

 protected:
  bool buildNaturalSpline(const QPolygonF &);
  bool buildPeriodicSpline(const QPolygonF &);

 private:
  inline double sqr(double x) { return x * x; }
  class PrivateData;
  PrivateData *d_data;
};

#endif  // SPLINE_H
