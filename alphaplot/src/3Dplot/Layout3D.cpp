#include "Layout3D.h"

#include <QtCore/qmath.h>
#include <QDateTime>

Layout3D::Layout3D(const QString &label, QWidget *parent, const QString name,
                   Qt::WindowFlags f)
    : MyWidget(label, parent, name, f), main_widget_(new QWidget(this)) {
  main_widget_->setContentsMargins(0, 0, 0, 0);
  if (name.isEmpty()) setObjectName("layout3d plot");
  QDateTime birthday = QDateTime::currentDateTime();
  setBirthDate(birthday.toString(Qt::LocalDate));

  setWidget(main_widget_);
  setGeometry(QRect(0, 0, 500, 400));
  setMinimumSize(QSize(100, 100));
  setFocusPolicy(Qt::StrongFocus);
  main_widget_->setStyleSheet(
      ".QWidget { background-color:rgba(255,255,255,255);}");
}

Layout3D::~Layout3D() {}

void Layout3D::generateSurfacePlot3D() {}
