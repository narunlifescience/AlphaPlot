#include "Layout3D.h"

#include <ApplicationWindow.h>
#include <QtCore/qmath.h>

#include <Q3DInputHandler>
#include <QDateTime>
#include <QtDataVisualization/Q3DBars>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataProxy>

#include "Bar3D.h"
#include "Scatter3D.h"
#include "Surface3D.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"
#include "plotcommon/widgets/ImageExportDialog.h"

Layout3D::Layout3D(const Graph3DCommon::Plot3DType &plottype,
                   const QString &label, QWidget *parent, const QString name,
                   Qt::WindowFlags flag)
    : MyWidget(label, parent, name, flag),
      plottype_(plottype),
      graph3dsurface_(nullptr),
      graph3dbars_(nullptr),
      graph3dscatter_(nullptr),
      surfacemodifier_(nullptr),
      barmodifier_(nullptr),
      scattermodifier_(nullptr),
      matrix_(nullptr) {
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface: {
      graph3dsurface_ = new Q3DSurface();
      main_widget_ = createWindowContainer(graph3dsurface_);
      surfacemodifier_ = new Surface3D(graph3dsurface_);
    } break;
    case Graph3DCommon::Plot3DType::Bar: {
      graph3dbars_ = new Q3DBars();
      main_widget_ = createWindowContainer(graph3dbars_);
      barmodifier_ = new Bar3D(graph3dbars_);
    } break;
    case Graph3DCommon::Plot3DType::Scatter: {
      graph3dscatter_ = new Q3DScatter();
      main_widget_ = createWindowContainer(graph3dscatter_);
      scattermodifier_ = new Scatter3D(graph3dscatter_);
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

Surface3D *Layout3D::getSurface3DModifier() const {
  if (plottype_ == Graph3DCommon::Plot3DType::Surface)
    return surfacemodifier_;
  else {
    qDebug() << "getSurface3DModifier() is null: this is not Surface3D";
    return nullptr;
  }
}

Bar3D *Layout3D::getBar3DModifier() const {
  if (plottype_ == Graph3DCommon::Plot3DType::Bar)
    return barmodifier_;
  else {
    qDebug() << "getBar3DModifier() is null: this is not Bar3D";
    return nullptr;
  }
}

Scatter3D *Layout3D::getScatter3DModifier() const {
  if (plottype_ == Graph3DCommon::Plot3DType::Scatter)
    return scattermodifier_;
  else {
    qDebug() << "getScatter3DModifier() is null: this is not Scatter3D";
    return nullptr;
  }
}

void Layout3D::exportGraph() {
  std::unique_ptr<ImageExportDialog> ied(new ImageExportDialog(this));
  ied->enableraster_scale(false);
  ied->setraster_scale(1);
  ied->setraster_antialias(16);
  ied->setraster_height(main_widget_->height());
  ied->setraster_width(main_widget_->width());
  ied->setvector_height(main_widget_->height());
  ied->setraster_width(main_widget_->width());
  if (ied->exec() != QDialog::Accepted) return;
  if (ied->selectedFiles().isEmpty()) return;
  QString selected_filter = ied->selectedNameFilter();

  QString file_name = ied->selectedFiles()[0];
  QFileInfo file_info(file_name);
  if (!file_info.fileName().contains("."))
    file_name.append(selected_filter.remove("*"));

  QFile file(file_name);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(
        this, tr("Export Error"),
        tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that "
           "you have the right to write to this location!")
            .arg(file_name));
    return;
  }

  int raster_resolution = ied->raster_resolution();
  int raster_width = ied->raster_width();
  int raster_height = ied->raster_height();
  int raster_quality = ied->raster_quality();
  int raster_antialias = ied->raster_antialias();

  int vector_width = ied->vector_width();
  int vector_height = ied->vector_height();

  if (selected_filter.contains(".pdf")) {
    // plot2dCanvas_->savePdf(file_name, vector_width, vector_height);
  } else if (selected_filter.contains(".svg")) {
    // plot2dCanvas_->saveSvg(file_name, vector_width, vector_height);
  } else if (selected_filter.contains(".ps")) {
    // plot2dCanvas_->savePs(file_name, vector_width, vector_height);
  } else {
    QByteArray ba = selected_filter.toLatin1();
    ba = ba.trimmed();
    ba.remove(0, 1);
    const char *c_char = ba.data();
    saveRastered(file_name, c_char, raster_quality, raster_antialias,
                 raster_resolution, QSize(raster_width, raster_height));
  }
}

void Layout3D::saveRastered(const QString &filename, const char *format,
                            const int quality, const int maa, int dpi,
                            const QSize &size) {
  QImage image = QImage();
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface:
      image = graph3dsurface_->renderToImage(maa, size);
      break;
    case Graph3DCommon::Plot3DType::Bar:
      image = graph3dbars_->renderToImage(maa, size);
      break;
    case Graph3DCommon::Plot3DType::Scatter:
      image = graph3dscatter_->renderToImage(maa, size);
      break;
  }
  int dpm = dpi / 0.0254;
  image.setDotsPerMeterX(dpm);
  image.setDotsPerMeterY(dpm);
  image.save(filename, format, quality);
}

void Layout3D::exportGraphwithoutdialog(const QString &name,
                                        const QString &selected_filter,
                                        const QSize &size) {
  // set default values
  int raster_quality = 100;
  int raster_dpi = 300;
  int raster_antialias = 8;

  if (selected_filter.contains(".pdf")) {
    // plot2dCanvas_->savePdf(name, vector_width, vector_height);
  } else if (selected_filter.contains(".svg")) {
    // plot2dCanvas_->saveSvg(name, vector_width, vector_height);
  } else if (selected_filter.contains(".ps")) {
    // plot2dCanvas_->savePs(name, vector_width, vector_height);
  } else {
    QByteArray ba = selected_filter.toLatin1();
    ba = ba.trimmed();
    ba.remove(0, 1);
    const char *c_char = ba.data();
    saveRastered(name, c_char, raster_quality, raster_antialias, raster_dpi,
                 size);
  }
}

void Layout3D::generateSurfacePlot3D(
    QList<QPair<QPair<double, double>, double> > *data,
    const Graph3DCommon::Function3DData &funcdata) {
  surfacemodifier_->setfunctiondata(data, funcdata);
}

void Layout3D::setMatrixDataModel(Matrix *matrix) {
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface:
      surfacemodifier_->setmatrixdatamodel(matrix);
      break;
    case Graph3DCommon::Plot3DType::Bar:
      barmodifier_->setmatrixdatamodel(matrix);
      break;
    case Graph3DCommon::Plot3DType::Scatter:
      scattermodifier_->setmatrixdatamodel(matrix);
      break;
  }
}

Matrix *Layout3D::getMatrix() const {
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface:
      return surfacemodifier_->getMatrix();
    case Graph3DCommon::Plot3DType::Bar:
      return barmodifier_->getMatrix();
    case Graph3DCommon::Plot3DType::Scatter:
      return scattermodifier_->getMatrix();
    default:
      return nullptr;
  }
}

QSize Layout3D::getContainerSize() const { return main_widget_->size(); }

Graph3DCommon::Plot3DType Layout3D::getPlotType() const { return plottype_; }

void Layout3D::load(XmlStreamReader *reader) {}

void Layout3D::save(QXmlStreamWriter *writer) {}
