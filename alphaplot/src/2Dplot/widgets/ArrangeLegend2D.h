#ifndef ARRANGELEGEND2D_H
#define ARRANGELEGEND2D_H

#include <QDialog>

class QListWidget;
class QPushButton;
class Legend2D;
class Curve2D;
class LineSpecial2D;
class Vector2D;
class Bar2D;
class StatBox2D;
class Pie2D;
class ColorMap2D;
class Channel2D;
class QCPAbstractLegendItem;

//! Add/remove curves dialog
class ArrangeLegend2D : public QDialog {
  Q_OBJECT

 public:
  ArrangeLegend2D(QWidget *parent, Legend2D *legend,
                  Qt::WindowFlags fl = Qt::Widget);
  ~ArrangeLegend2D();

 private:
  void rearrangeLegend2D();
  void remakeLegendItem2D(QCPAbstractLegendItem *item);
  QSize sizeHint() const;

  Legend2D *legend_;
  QPushButton *btnOK_;
  QPushButton *btnCancel_;
  QPushButton *btnUp_;
  QPushButton *btnDown_;
  QListWidget *plotlegendlist_;
  // plottables
  QVector<Curve2D *> cvec_;
  QVector<LineSpecial2D *> lsvec_;
  QVector<Channel2D *> lspecialvec_;
  QVector<Vector2D *> vvec_;
  QVector<Bar2D *> bvec_;
  QVector<StatBox2D *> svec_;
  QVector<Pie2D *> pvec_;
  QVector<ColorMap2D *> covec_;
  QVector<QPair<QCPAbstractLegendItem *, QString>> abstactvec_;
};

#endif  // ARRANGELEGEND2D_H
