#include "CustomWidgets.h"

#include <QApplication>
#include <QColorDialog>
#include <QEvent>
#include <QFontDialog>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPointer>
#include <QStyleOption>
#include <QToolButton>
#include <QtDebug>

StringWidget::StringWidget(QWidget *parent) : QLineEdit(parent) {
  connect(this, &StringWidget::destroyed,
          [&]() { emit this->valueChanged(this->text()); });
}

StringWidget::~StringWidget() {}

FontWidget::FontWidget(QWidget *parent)
    : QWidget(parent),
      m_pixmapLabel_(new QLabel),
      m_label_(new QLabel),
      m_button_(new QToolButton) {
  QHBoxLayout *lt = new QHBoxLayout(this);
  // setupTreeViewEditorMargin(lt);
  lt->setContentsMargins(6, 0, 0, 0);
  lt->setSpacing(0);
  lt->addWidget(m_pixmapLabel_);
  lt->addWidget(m_label_);
  lt->addItem(
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));

  m_button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
  m_button_->setFixedWidth(20);
  setFocusProxy(m_button_);
  setFocusPolicy(m_button_->focusPolicy());
  m_button_->setText(tr("..."));
  m_button_->installEventFilter(this);
  connect(m_button_, &QToolButton::clicked, this, &FontWidget::buttonClicked);
  lt->addWidget(m_button_);
  m_pixmapLabel_->setPixmap(fontValuePixmap(m_font_));
  m_label_->setText(fontValueText(m_font_));
}

FontWidget::~FontWidget() {}

bool FontWidget::eventFilter(QObject *obj, QEvent *ev) {
  if (obj == m_button_) {
    switch (ev->type()) {
      case QEvent::KeyPress:
      case QEvent::KeyRelease: {  // Prevent the QToolButton from handling
                                  // Enter/Escape meant control the delegate
        switch (static_cast<const QKeyEvent *>(ev)->key()) {
          case Qt::Key_Escape:
          case Qt::Key_Enter:
          case Qt::Key_Return:
            ev->ignore();
            return true;
          default:
            break;
        }
      } break;
      default:
        break;
    }
  }
  return QWidget::eventFilter(obj, ev);
}

void FontWidget::setValue(const QFont &value) {
  if (m_font_ != value) {
    m_font_ = value;
    m_pixmapLabel_->setPixmap(fontValuePixmap(value));
    m_label_->setText(fontValueText(value));
  }
}

void FontWidget::paintEvent(QPaintEvent *) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FontWidget::buttonClicked() {
  bool ok = false;
  QFont newFont =
      QFontDialog::getFont(&ok, m_font_, nullptr, tr("Select Font"));
  if (ok && newFont != m_font_) {
    QFont f = m_font_;
    // prevent mask for unchanged attributes, don't change other attributes
    // (like kerning, etc...)
    if (m_font_.family() != newFont.family()) f.setFamily(newFont.family());
    if (m_font_.pointSize() != newFont.pointSize())
      f.setPointSize(newFont.pointSize());
    if (m_font_.bold() != newFont.bold()) f.setBold(newFont.bold());
    if (m_font_.italic() != newFont.italic()) f.setItalic(newFont.italic());
    if (m_font_.underline() != newFont.underline())
      f.setUnderline(newFont.underline());
    if (m_font_.strikeOut() != newFont.strikeOut())
      f.setStrikeOut(newFont.strikeOut());
    setValue(f);
    emit valueChanged(m_font_);
  }
}

QPixmap FontWidget::fontValuePixmap(const QFont &font) {
  QFont f = font;
  QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
  img.fill(0);
  QPainter p(&img);
  p.setRenderHint(QPainter::TextAntialiasing, true);
  p.setRenderHint(QPainter::Antialiasing, true);
  f.setPointSize(13);
  p.setFont(f);
  QTextOption t;
  t.setAlignment(Qt::AlignCenter);
  p.drawText(QRect(0, 0, 16, 16), QString(QLatin1Char('A')), t);
  return QPixmap::fromImage(img);
}

QString FontWidget::fontValueText(const QFont &font) {
  return QApplication::translate("PropertyUtils", " [%1, %2]", 0)
      .arg(font.family())
      .arg(font.pointSize());
}

ColorWidget::ColorWidget(QWidget *parent)
    : QWidget(parent),
      m_pixmapLabel(new QLabel),
      m_label(new QLabel),
      m_button(new QToolButton) {
  QHBoxLayout *lt = new QHBoxLayout(this);
  lt->setContentsMargins(6, 0, 0, 0);
  lt->setSpacing(0);
  lt->addWidget(m_pixmapLabel);
  lt->addWidget(m_label);
  lt->addItem(
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored));

  m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
  m_button->setFixedWidth(20);
  setFocusProxy(m_button);
  setFocusPolicy(m_button->focusPolicy());
  m_button->setText(tr("..."));
  m_button->installEventFilter(this);
  connect(m_button, &QToolButton::clicked, this, &ColorWidget::buttonClicked);
  lt->addWidget(m_button);
  m_pixmapLabel->setPixmap(brushValuePixmap(QBrush(m_color)));
  m_label->setText(colorValueText(m_color));
}

void ColorWidget::setValue(const QColor &c) {
  if (m_color != c) {
    m_color = c;
    m_pixmapLabel->setPixmap(brushValuePixmap(QBrush(c)));
    m_label->setText(colorValueText(c));
  }
}

void ColorWidget::buttonClicked() {
  QColor newColor = QColorDialog::getColor(
      m_color, nullptr, tr("Select Color"),
      QColorDialog::ColorDialogOptions(QColorDialog::ShowAlphaChannel));
  if (m_color != newColor && newColor.isValid()) {
    setValue(newColor);
    emit valueChanged(m_color);
  }
}

bool ColorWidget::eventFilter(QObject *obj, QEvent *ev) {
  if (obj == m_button) {
    switch (ev->type()) {
      case QEvent::KeyPress:
      case QEvent::KeyRelease: {  // Prevent the QToolButton from handling
                                  // Enter/Escape meant control the delegate
        switch (static_cast<const QKeyEvent *>(ev)->key()) {
          case Qt::Key_Escape:
          case Qt::Key_Enter:
          case Qt::Key_Return:
            ev->ignore();
            return true;
          default:
            break;
        }
      } break;
      default:
        break;
    }
  }
  return QWidget::eventFilter(obj, ev);
}

QPixmap ColorWidget::brushValuePixmap(const QBrush &b) {
  QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
  img.fill(0);

  QPainter painter(&img);
  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.fillRect(0, 0, img.width(), img.height(), b);
  QColor color = b.color();
  if (color.alpha() != 255) {  // indicate alpha by an inset
    QBrush opaqueBrush = b;
    color.setAlpha(255);
    opaqueBrush.setColor(color);
    painter.fillRect(img.width() / 4, img.height() / 4, img.width() / 2,
                     img.height() / 2, opaqueBrush);
  }
  painter.end();
  return QPixmap::fromImage(img);
}

QString ColorWidget::colorValueText(const QColor &c) {
  return QString(" [%1, %2, %3] (%4)")
      .arg(QString::number(c.red()), QString::number(c.green()),
           QString::number(c.blue()), QString::number(c.alpha()));
}

void ColorWidget::paintEvent(QPaintEvent *) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

DateTimeWidget::DateTimeWidget(QWidget *parent) : QDateTimeEdit(parent) {
  connect(this, &DateTimeWidget::destroyed,
          [&]() { emit this->datetimeValueChanged(this->dateTime()); });
}

DateTimeWidget::~DateTimeWidget() {}

DoubleWidget::DoubleWidget(QWidget *parent) : QDoubleSpinBox(parent) {
  connect(this, &DoubleWidget::destroyed,
          [&]() { emit this->doubleValueChanged(this->value()); });
}

DoubleWidget::~DoubleWidget() {}

IntWidget::IntWidget(QWidget *parent) : QSpinBox(parent) {
  connect(this, &IntWidget::destroyed,
          [&]() { emit this->intValueChanged(this->value()); });
}

IntWidget::~IntWidget() {}
