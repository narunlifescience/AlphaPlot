#ifndef CUSTOMWIDGETS_H
#define CUSTOMWIDGETS_H

#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class QLabel;
class QToolButton;

class StringWidget : public QLineEdit {
  Q_OBJECT
 public:
  StringWidget(QWidget *parent);
  ~StringWidget();

 signals:
  void valueChanged(const QString &value);
};

class IntWidget : public QSpinBox {
  Q_OBJECT
 public:
  IntWidget(QWidget *parent);
  ~IntWidget();

 signals:
  void intValueChanged(const int value);
};

class DoubleWidget : public QDoubleSpinBox {
  Q_OBJECT
 public:
  DoubleWidget(QWidget *parent);
  ~DoubleWidget();

 signals:
  void doubleValueChanged(const double value);
};

class FontWidget : public QWidget {
  Q_OBJECT

 public:
  FontWidget(QWidget *parent);
  ~FontWidget();

  bool eventFilter(QObject *obj, QEvent *ev);
  QFont getFont() const { return m_font_; }

 public slots:
  void setValue(const QFont &value);

 signals:
  void valueChanged(const QFont &value);

 protected:
  void paintEvent(QPaintEvent *);

 private slots:
  void buttonClicked();

 private:
  QPixmap fontValuePixmap(const QFont &font);
  QString fontValueText(const QFont &font);

 private:
  QFont m_font_;
  QLabel *m_pixmapLabel_;
  QLabel *m_label_;
  QToolButton *m_button_;
};

class ColorWidget : public QWidget {
  Q_OBJECT

 public:
  ColorWidget(QWidget *parent);
  bool eventFilter(QObject *obj, QEvent *ev);
  QColor color() const { return m_color; }

 private:
  QPixmap brushValuePixmap(const QBrush &b);
  QString colorValueText(const QColor &c);

 public slots:
  void setValue(const QColor &value);

 signals:
  void valueChanged(const QColor &value);

 protected:
  void paintEvent(QPaintEvent *);

 private slots:
  void buttonClicked();

 private:
  QColor m_color;
  QLabel *m_pixmapLabel;
  QLabel *m_label;
  QToolButton *m_button;
};

class DateTimeWidget : public QDateTimeEdit {
  Q_OBJECT
 public:
  DateTimeWidget(QWidget *parent);
  ~DateTimeWidget();

 signals:
  void datetimeValueChanged(const QDateTime &value);
};

#endif  // CUSTOMWIDGETS_H
