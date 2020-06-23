#ifndef LAYOUTBUTTON2D_H
#define LAYOUTBUTTON2D_H

#include <QToolButton>

class ToolButton : public QToolButton {
  Q_OBJECT

 public:
  ToolButton(QWidget *parent = nullptr);
  ~ToolButton();
  static int height() { return layoutButtonSize_; }

 private:
  static const int layoutButtonSize_;
};

class LayoutButton2D : public ToolButton {
  Q_OBJECT

 public:
  LayoutButton2D(const QPair<int, int> rowcol, QWidget *parent = nullptr);
  ~LayoutButton2D();

  static int btnSize();
  QPair<int, int> getRowCol() const;
  void resetRowCol(const QPair<int, int> rowcol);
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
  static QRect highLightRect_;
  const static QColor highlightColor_;
  QPair<int, int> rowcol_;
  QString buttonText_;
};

#endif  // LAYOUTBUTTON2D_H
