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

const int Layout3D::defaultlayout2dwidth_ = 500;
const int Layout3D::defaultlayout2dheight_ = 400;
const int Layout3D::minimumlayout2dwidth_ = 100;
const int Layout3D::minimumlayout2dheight_ = 100;

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
      graph3dsurface_->setFlags(Qt::FramelessWindowHint);
      graph3dsurface_->setFlag(Qt::WindowType::SubWindow, true);
      main_widget_ = createWindowContainer(graph3dsurface_);
      surfacemodifier_ = new Surface3D(graph3dsurface_);
    } break;
    case Graph3DCommon::Plot3DType::Bar: {
      graph3dbars_ = new Q3DBars();
      graph3dbars_->setFlags(Qt::FramelessWindowHint);
      graph3dbars_->setFlag(Qt::WindowType::SubWindow, true);
      main_widget_ = createWindowContainer(graph3dbars_);
      barmodifier_ = new Bar3D(graph3dbars_);
    } break;
    case Graph3DCommon::Plot3DType::Scatter: {
      graph3dscatter_ = new Q3DScatter();
      graph3dscatter_->setFlags(Qt::FramelessWindowHint);
      graph3dscatter_->setFlag(Qt::WindowType::SubWindow, true);
      main_widget_ = createWindowContainer(graph3dscatter_);
      scattermodifier_ = new Scatter3D(graph3dscatter_);
    } break;
  }
  main_widget_->setContentsMargins(0, 0, 0, 0);
  if (name.isEmpty()) setObjectName("layout3d");
  QDateTime birthday = QDateTime::currentDateTime();
  setBirthDate(birthday.toString(Qt::LocalDate));
  setFocusPolicy(Qt::TabFocus);

  setWidget(main_widget_);
  setGeometry(QRect(0, 0, defaultlayout2dwidth_, defaultlayout2dheight_));
  setMinimumSize(QSize(minimumlayout2dwidth_, minimumlayout2dheight_));
  setFocusPolicy(Qt::StrongFocus);
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

  QString file_name = ied->selectedFiles().at(0);
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

  // int vector_width = ied->vector_width();
  // int vector_height = ied->vector_height();

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

QSize Layout3D::getContainerSize() const { return main_widget_->size(); }

Graph3DCommon::Plot3DType Layout3D::getPlotType() const { return plottype_; }

void Layout3D::load(XmlStreamReader *xmlreader, QList<Table *> tabs,
                    QList<Matrix *> mats) {
  if (xmlreader->isStartElement() && xmlreader->name() == "plot3d") {
    bool ok = false;

    // read caption spec
    int x = xmlreader->readAttributeInt("x", &ok);
    if (ok) {
      int y = xmlreader->readAttributeInt("y", &ok);
      if (ok) {
        int width = xmlreader->readAttributeInt("width", &ok);
        if (ok) {
          int height = xmlreader->readAttributeInt("height", &ok);
          if (ok) {
            setGeometry(x, y, width, height);
          }
        }
      }
    } else
      xmlreader->raiseWarning(tr("Plot3D geometry setting error."));

    // read creation time
    QString time = xmlreader->readAttributeString("creation_time", &ok);
    QDateTime creation_time =
        QDateTime::fromString(time, "yyyy-dd-MM hh:mm:ss:zzz");
    if (!time.isEmpty() && creation_time.isValid() && ok) {
      setBirthDate(creation_time.toString(Qt::LocalDate));
    } else {
      xmlreader->raiseWarning(
          tr("Invalid creation time. Using current time insted."));
      setBirthDate(QDateTime::currentDateTime().toString(Qt::LocalDate));
    }
    // read caption spec
    int captionspec = xmlreader->readAttributeInt("caption_spec", &ok);
    if (ok)
      setCaptionPolicy(static_cast<MyWidget::CaptionPolicy>(captionspec));
    else
      xmlreader->raiseWarning(tr("Invalid caption policy or read error."));
    // read name
    QString name = xmlreader->readAttributeString("name", &ok);
    if (ok) {
      setName(name);
    } else
      xmlreader->raiseWarning(tr("Layout3D name missing or empty"));

    // read label
    QString label = xmlreader->readAttributeString("label", &ok);
    if (ok) {
      setWindowLabel(name);
    } else
      xmlreader->raiseWarning(tr("Layout3D label missing or empty"));
  }
}

void Layout3D::save(QXmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("plot3d");
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface:
      xmlwriter->writeAttribute("type", "surface");
      break;
    case Graph3DCommon::Plot3DType::Bar:
      xmlwriter->writeAttribute("type", "bar");
      break;
    case Graph3DCommon::Plot3DType::Scatter:
      xmlwriter->writeAttribute("type", "scatter");
      break;
  }
  xmlwriter->writeAttribute("x", QString::number(pos().x()));
  xmlwriter->writeAttribute("y", QString::number(pos().y()));
  xmlwriter->writeAttribute("width", QString::number(width()));
  xmlwriter->writeAttribute("height", QString::number(height()));
  xmlwriter->writeAttribute("creation_time", birthDate());
  xmlwriter->writeAttribute("caption_spec", QString::number(captionPolicy()));
  xmlwriter->writeAttribute("name", name());
  xmlwriter->writeAttribute("label", windowLabel());
  xmlwriter->writeEndElement();
}
