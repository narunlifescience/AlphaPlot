#include "Layout3D.h"
/*#include "Surface3D.h"

#include <QtCore/qmath.h>
#include <QDateTime>
#include <QtDataVisualization/Q3DBars>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/Q3DSurface>

#include <ApplicationWindow.h>
#include <Q3DInputHandler>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataProxy>

Layout3D::Layout3D(const Layout3D::Plot3DType &plottype, const QString &label,
                   QWidget *parent, const QString name, Qt::WindowFlags flag)
    : MyWidget(label, parent, name, flag),
      plottype_(plottype),
      graph3dsurface_(nullptr),
      graph3dbars_(nullptr),
      graph3dscatter_(nullptr) {
  switch (plottype_) {
    case Plot3DType::Surface: {
      graph3dsurface_ = new Q3DSurface();
      main_widget_ = createWindowContainer(graph3dsurface_);
      modifier_ = new Surface3D(graph3dsurface_);
    } break;
    case Plot3DType::Bar: {
      graph3dbars_ = new Q3DBars();
      main_widget_ = createWindowContainer(graph3dbars_);
    } break;
    case Plot3DType::Scatter: {
      graph3dscatter_ = new Q3DScatter();
      main_widget_ = createWindowContainer(graph3dscatter_);
    } break;
  }
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

void Layout3D::generateSurfacePlot3D(
    QList<QPair<QPair<double, double>, double> > *data) {
  modifier_->fillfunctiondata(data);

}

void Layout3D::generateBarPlot3D() {}

void Layout3D::generateScatterPlot3D() {}*/
