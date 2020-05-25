#ifndef LAYOUTBUTTON2D_H
#define LAYOUTBUTTON2D_H

#include <QPushButton>

class LayoutButton2D : public QPushButton {
  Q_OBJECT

 public:
  LayoutButton2D(const QString &text = QString(),
                 QWidget *parent = nullptr);
  ~LayoutButton2D();

  static int btnSize();
  void setActive(bool status);
  bool isActive();

 protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *);
  void resizeEvent(QResizeEvent *);
  void paintEvent(QPaintEvent *);

 signals:
  void showCurvesDialog();
  void clicked(LayoutButton2D *);
  void showContextMenu();

 private:
  bool active_;
  static const int layoutButtonSize_;
  static QRect highLightRect_;
  QString buttonText_;
};

#endif  // LAYOUTBUTTON2D_H
