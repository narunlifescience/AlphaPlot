#include "Layout3D.h"

#include <ApplicationWindow.h>
#include <QtCore/qmath.h>
#include <QtDataVisualization/qutils.h>

#include <Q3DInputHandler>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QSurfaceFormat>
#include <QtDataVisualization/Q3DBars>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataProxy>

#include "Bar3D.h"
#include "Custom3DInteractions.h"
#include "DataManager3D.h"
#include "Matrix.h"
#include "MyWidget.h"
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
      custominter_(new Custom3DInteractions) {
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface: {
      graph3dsurface_ = new Q3DSurface();
      main_widget_ = QWidget::createWindowContainer(graph3dsurface_);
      surfacemodifier_ = new Surface3D(graph3dsurface_);
      graph_ = static_cast<QAbstract3DGraph *>(graph3dsurface_);
      connect(surfacemodifier_, &Surface3D::dataAdded, this,
              [&]() { emit dataAdded(this); });
    } break;
    case Graph3DCommon::Plot3DType::Bar: {
      graph3dbars_ = new Q3DBars();
      main_widget_ = QWidget::createWindowContainer(graph3dbars_);
      barmodifier_ = new Bar3D(graph3dbars_);
      graph_ = static_cast<QAbstract3DGraph *>(graph3dbars_);
      connect(barmodifier_, &Bar3D::dataAdded, this,
              [&]() { emit dataAdded(this); });
    } break;
    case Graph3DCommon::Plot3DType::Scatter: {
      graph3dscatter_ = new Q3DScatter();
      main_widget_ = QWidget::createWindowContainer(graph3dscatter_);
      scattermodifier_ = new Scatter3D(graph3dscatter_);
      graph_ = static_cast<QAbstract3DGraph *>(graph3dscatter_);
      connect(scattermodifier_, &Scatter3D::dataAdded, this,
              [&]() { emit dataAdded(this); });
    } break;
  }

  main_widget_->setContentsMargins(0, 0, 0, 0);
  QWidget *widget = new QWidget(this);
  widget->setContentsMargins(0, 0, 0, 0);
  QHBoxLayout *hLayout = new QHBoxLayout(widget);
  hLayout->setMargin(0);
  hLayout->addWidget(main_widget_, 1);
  hLayout->setAlignment(Qt::AlignTop);
  widget->setLayout(hLayout);
  setWidget(widget);
  main_widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  main_widget_->setFocusPolicy(Qt::StrongFocus);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  widget->setFocusPolicy(Qt::StrongFocus);

  if (name.isEmpty()) setObjectName("layout3d");
  QDateTime birthday = QDateTime::currentDateTime();
  setBirthDate(birthday.toString(Qt::LocalDate));
  setFocusPolicy(Qt::TabFocus);

  setGeometry(QRect(0, 0, defaultlayout2dwidth_, defaultlayout2dheight_));
  setMinimumSize(QSize(minimumlayout2dwidth_, minimumlayout2dheight_));
  setFocusPolicy(Qt::StrongFocus);

  // setting general graph properties;
  graph_->setActiveInputHandler(custominter_);
  graph_->activeTheme()->setType(Q3DTheme::ThemeDigia);
  graph_->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
  graph_->scene()->activeCamera()->setCameraPreset(
      Q3DCamera::CameraPresetFront);
  graph_->setAspectRatio(1.0);
  graph_->setHorizontalAspectRatio(1.0);
  // set animations
  m_animationCameraX_ =
      new QPropertyAnimation(graph_->scene()->activeCamera(), "xRotation");
  m_animationCameraX_->setDuration(20000);
  m_animationCameraX_->setStartValue(QVariant::fromValue(-180.0f));
  m_animationCameraX_->setEndValue(QVariant::fromValue(180.0f));
  m_animationCameraX_->setLoopCount(-1);
  upAnimation_ =
      new QPropertyAnimation(graph_->scene()->activeCamera(), "yRotation");
  upAnimation_->setDuration(10000);
  upAnimation_->setStartValue(QVariant::fromValue(0.0f));
  upAnimation_->setEndValue(QVariant::fromValue(90.0f));
  downAnimation_ =
      new QPropertyAnimation(graph_->scene()->activeCamera(), "yRotation");
  downAnimation_->setDuration(10000);
  downAnimation_->setStartValue(QVariant::fromValue(90.0f));
  downAnimation_->setEndValue(QVariant::fromValue(0.0f));
  m_animationCameraY_ = new QSequentialAnimationGroup();
  m_animationCameraY_->setLoopCount(-1);
  m_animationCameraY_->addAnimation(upAnimation_);
  m_animationCameraY_->addAnimation(downAnimation_);

  graph_->scene()->activeCamera()->setCameraPosition(45, 30, 130);
  connect(custominter_, &Custom3DInteractions::showContextMenu, this,
          &Layout3D::showContextMenu);
  // QWindow doesnt pass mousepressevent to the container widget
  // so do it here manually
  connect(custominter_, &Custom3DInteractions::activateParentWindow, this,
          [=]() {
            emit mousepressevent(this);
            unsetCursor();
          });
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

QAbstract3DGraph *Layout3D::getAbstractGraph() const { return graph_; }

void Layout3D::setCustomInteractions(QAbstract3DGraph *graph, bool status) {
  std::unique_ptr<Q3DSurface> gg(new Q3DSurface);
  (status) ? graph->setActiveInputHandler(custominter_)
           : graph->setActiveInputHandler(gg->activeInputHandler());
}

void Layout3D::setAnimation(bool status) {
  if (status) {
    m_animationCameraX_->start();
    m_animationCameraY_->start();
  } else {
    m_animationCameraX_->pause();
    m_animationCameraY_->pause();
  }
}

void Layout3D::exportGraph() {
  std::unique_ptr<ImageExportDialog> ied(new ImageExportDialog(this, false));
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
                    QList<Matrix *> mats, ApplicationWindow *app,
                    bool setname) {
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
    if (setname) {
      if (ok) {
        setName(name);
      } else
        xmlreader->raiseWarning(tr("Layout3D name missing or empty"));
    }

    // read label
    QString label = xmlreader->readAttributeString("label", &ok);
    if (ok) {
      setWindowLabel(label);
    } else
      xmlreader->raiseWarning(tr("Layout3D label missing or empty"));

    loadCanvas(xmlreader);
    loadTheme(xmlreader);
    switch (plottype_) {
      case Graph3DCommon::Plot3DType::Surface:
        loadValueAxis(xmlreader);
        loadValueAxis(xmlreader);
        loadValueAxis(xmlreader);
        surfacemodifier_->load(xmlreader, tabs, mats, app);
        break;
      case Graph3DCommon::Plot3DType::Bar:
        loadCategoryAxis(xmlreader);
        loadCategoryAxis(xmlreader);
        loadValueAxis(xmlreader);
        barmodifier_->load(xmlreader, tabs, mats);
        break;
      case Graph3DCommon::Plot3DType::Scatter:
        loadValueAxis(xmlreader);
        loadValueAxis(xmlreader);
        loadValueAxis(xmlreader);
        scattermodifier_->load(xmlreader, tabs, mats);
        break;
    }
  }
}

void Layout3D::loadCanvas(XmlStreamReader *xmlreader) {
  while (!xmlreader->atEnd()) {
    xmlreader->readNext();
    if (xmlreader->isEndElement() && xmlreader->name() == "canvas") break;
    if (xmlreader->isStartElement() && xmlreader->name() == "canvas") {
      bool ok = false;
      Q3DTheme::Theme theme = Q3DTheme::Theme::ThemeQt;
      QString themename = xmlreader->readAttributeString("themename", &ok);
      if (!ok) xmlreader->raiseWarning("Plot3D theme unknown!");

      // theme
      if (themename == "themeqt")
        theme = Q3DTheme::Theme::ThemeQt;
      else if (themename == "themedigia")
        theme = Q3DTheme::Theme::ThemeDigia;
      else if (themename == "themeebony")
        theme = Q3DTheme::Theme::ThemeEbony;
      else if (themename == "themeretro")
        theme = Q3DTheme::Theme::ThemeRetro;
      else if (themename == "themearmyblue")
        theme = Q3DTheme::Theme::ThemeArmyBlue;
      else if (themename == "themeisabelle")
        theme = Q3DTheme::Theme::ThemeIsabelle;
      else if (themename == "themestonemoss")
        theme = Q3DTheme::Theme::ThemeStoneMoss;
      else if (themename == "themeuserdefined")
        theme = Q3DTheme::Theme::ThemeUserDefined;
      else if (themename == "themeprimarycolors")
        theme = Q3DTheme::Theme::ThemePrimaryColors;
      else
        xmlreader->raiseWarning("Plot3D theme setting unknown!");
      graph_->activeTheme()->setType(theme);
    }
  }
}

void Layout3D::loadTheme(XmlStreamReader *xmlreader) {
  while (!xmlreader->atEnd()) {
    xmlreader->readNext();
    if (xmlreader->isEndElement() && xmlreader->name() == "theme") break;
    if (xmlreader->isStartElement() && xmlreader->name() == "theme") {
      bool ok = false;
      Q3DTheme *theme = graph_->activeTheme();

      // canvas color
      QString canvascolor = xmlreader->readAttributeString("canvascolor", &ok);
      if (ok)
        theme->setWindowColor(canvascolor);
      else
        xmlreader->raiseWarning("Plot3D canvas color property setting error");

      // background
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "background")
          break;
        if (xmlreader->isStartElement() && xmlreader->name() == "background") {
          bool ok = false;

          // background visible
          bool visible = xmlreader->readAttributeBool("visible", &ok);
          if (ok)
            theme->setBackgroundEnabled(visible);
          else
            xmlreader->raiseWarning(
                "Plot3D background visible property setting error");

          // background color
          QString bkcolor = xmlreader->readAttributeString("color", &ok);
          if (ok)
            theme->setBackgroundColor(bkcolor);
          else
            xmlreader->raiseWarning(
                "Plot3D background color property setting error");
        }
      }

      // light
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "light") break;
        if (xmlreader->isStartElement() && xmlreader->name() == "light") {
          bool ok = false;

          // light color
          QString lightcolor = xmlreader->readAttributeString("color", &ok);
          if (ok)
            theme->setLightColor(lightcolor);
          else
            xmlreader->raiseWarning(
                "Plot3D light color property setting error");

          // light strength
          int strength = xmlreader->readAttributeInt("strength", &ok);
          if (ok)
            theme->setLightStrength(strength);
          else
            xmlreader->raiseWarning(
                "Plot3D light strength property setting error");

          // ambient light strength
          int ambstrength = xmlreader->readAttributeInt("ambientstrength", &ok);
          if (ok)
            theme->setAmbientLightStrength(ambstrength);
          else
            xmlreader->raiseWarning(
                "Plot3D ambient light strength property setting error");
        }
      }

      // grid
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "grid") break;
        if (xmlreader->isStartElement() && xmlreader->name() == "grid") {
          bool ok = false;

          // grid visible
          bool visible = xmlreader->readAttributeBool("visible", &ok);
          if (ok)
            theme->setGridEnabled(visible);
          else
            xmlreader->raiseWarning(
                "Plot3D grid visible property setting error");

          // grid line color
          QString gdcolor = xmlreader->readAttributeString("color", &ok);
          if (ok)
            theme->setGridLineColor(gdcolor);
          else
            xmlreader->raiseWarning(
                "Plot3D grid line color property setting error");
        }
      }

      // label
      while (!xmlreader->atEnd()) {
        xmlreader->readNext();
        if (xmlreader->isEndElement() && xmlreader->name() == "label") break;
        if (xmlreader->isStartElement() && xmlreader->name() == "label") {
          bool ok = false;

          // label background visible
          bool visible = xmlreader->readAttributeBool("backgroundvisible", &ok);
          if (ok)
            theme->setLabelBackgroundEnabled(visible);
          else
            xmlreader->raiseWarning(
                "Plot3D label background visible property setting error");

          // label background color
          QString bkcolor =
              xmlreader->readAttributeString("backgroundcolor", &ok);
          if (ok)
            theme->setLabelBackgroundColor(bkcolor);
          else
            xmlreader->raiseWarning(
                "Plot3D label background color property setting error");

          // label border visible
          bool lbbovisible = xmlreader->readAttributeBool("bordervisible", &ok);
          if (ok)
            theme->setLabelBorderEnabled(lbbovisible);
          else
            xmlreader->raiseWarning(
                "Plot3D label border visible property setting error");

          // label font
          while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "font") break;
            if (xmlreader->isStartElement() && xmlreader->name() == "font") {
              QPair<QFont, QColor> fontpair = xmlreader->readFont(&ok);
              if (ok) {
                theme->setFont(fontpair.first);
                theme->setLabelTextColor(fontpair.second);
              } else
                xmlreader->raiseWarning(
                    tr("Plot3D label font & color property setting error"));
            }
          }
        }
      }
    }
  }
}

void Layout3D::loadValueAxis(XmlStreamReader *xmlreader) {
  while (!xmlreader->atEnd()) {
    xmlreader->readNext();
    if (xmlreader->isEndElement() && xmlreader->name() == "axis") break;
    if (xmlreader->isStartElement() && xmlreader->name() == "axis") {
      bool ok = false;

      // axis type
      QString axistype = xmlreader->readAttributeString("type", &ok);
      if (!ok) xmlreader->raiseError("Plot3D axis type unknown error!");
      if (axistype != "value")
        xmlreader->raiseError("Plot3D axis type unexpected error!");

      // axis orientation
      QString axisori = xmlreader->readAttributeString("orientation", &ok);
      if (!ok) xmlreader->raiseError("Plot3D axis orientation unknown error!");
      QValue3DAxis *axis = nullptr;
      switch (plottype_) {
        case Graph3DCommon::Plot3DType::Surface:
          if (axisori == "x")
            axis = surfacemodifier_->getGraph()->axisX();
          else if (axisori == "y")
            axis = surfacemodifier_->getGraph()->axisY();
          else if (axisori == "z")
            axis = surfacemodifier_->getGraph()->axisZ();
          break;
        case Graph3DCommon::Plot3DType::Bar:
          if (axisori == "z") axis = barmodifier_->getGraph()->valueAxis();
          break;
        case Graph3DCommon::Plot3DType::Scatter:
          if (axisori == "x")
            axis = scattermodifier_->getGraph()->axisX();
          else if (axisori == "y")
            axis = scattermodifier_->getGraph()->axisY();
          else if (axisori == "z")
            axis = scattermodifier_->getGraph()->axisZ();
          break;
      }
      if (!axis)
        xmlreader->raiseError("Plot3D value axis unable to initialize error!");

      // Axis autorange
      bool autorange = xmlreader->readAttributeBool("autorange", &ok);
      (ok) ? axis->setAutoAdjustRange(autorange)
           : xmlreader->raiseWarning(
                 "Plot3D value axis autorange setting error!");

      // Axis from
      double from = xmlreader->readAttributeDouble("from", &ok);
      (ok) ? axis->setMin(from)
           : xmlreader->raiseWarning("Plot3D value axis from setting error!");

      // Axis to
      double to = xmlreader->readAttributeDouble("to", &ok);
      (ok) ? axis->setMax(to)
           : xmlreader->raiseWarning("Plot3D value axis to setting error!");

      // Axis range reverse
      bool reverse = xmlreader->readAttributeBool("reverse", &ok);
      (ok)
          ? axis->setReversed(reverse)
          : xmlreader->raiseWarning("Plot3D value axis reverse setting error!");

      // Axis tick count
      int tickcount = xmlreader->readAttributeInt("tickcount", &ok);
      (ok) ? axis->setSegmentCount(tickcount)
           : xmlreader->raiseWarning(
                 "Plot3D value axis tick count setting error!");

      // Axis subtick count
      int subtickcount = xmlreader->readAttributeInt("subtickcount", &ok);
      (ok) ? axis->setSubSegmentCount(subtickcount)
           : xmlreader->raiseWarning(
                 "Plot3D value axis subtick count setting error!");

      // Axis ticklabel format
      QString lblformat =
          xmlreader->readAttributeString("ticklabelformat", &ok);
      (ok) ? axis->setLabelFormat(lblformat)
           : xmlreader->raiseWarning(
                 "Plot3D value axis tick label format setting error!");

      // Axis tick label rotation
      double tklblrotation =
          xmlreader->readAttributeDouble("ticklabelrotation", &ok);
      (ok) ? axis->setLabelAutoRotation(tklblrotation)
           : xmlreader->raiseWarning(
                 "Plot3D value axis tick label rotation setting error!");

      // Axis label visible
      bool lblvisible = xmlreader->readAttributeBool("labelvisible", &ok);
      (ok) ? axis->setTitleVisible(lblvisible)
           : xmlreader->raiseWarning(
                 "Plot3D value axis title visible setting error!");

      // Axis label fixed
      bool lblfixed = xmlreader->readAttributeBool("labelfixed", &ok);
      (ok) ? axis->setTitleFixed(lblfixed)
           : xmlreader->raiseWarning(
                 "Plot3D value axis title fixed setting error!");

      // Axis label text
      QString label = xmlreader->readAttributeString("label", &ok);
      (ok) ? axis->setTitle(label)
           : xmlreader->raiseWarning("Plot3D value axis label setting error!");
    }
  }
}

void Layout3D::loadCategoryAxis(XmlStreamReader *xmlreader) {
  while (!xmlreader->atEnd()) {
    xmlreader->readNext();
    if (xmlreader->isEndElement() && xmlreader->name() == "axis") break;
    if (xmlreader->isStartElement() && xmlreader->name() == "axis") {
      bool ok = false;

      // axis type
      QString axistype = xmlreader->readAttributeString("type", &ok);
      if (!ok) xmlreader->raiseError("Plot3D axis type unknown error!");
      if (axistype != "category")
        xmlreader->raiseError("Plot3D axis type unexpected error!");

      // axis orientation
      QString axisori = xmlreader->readAttributeString("orientation", &ok);
      if (!ok) xmlreader->raiseError("Plot3D axis orientation unknown error!");
      QCategory3DAxis *axis = nullptr;
      switch (plottype_) {
        case Graph3DCommon::Plot3DType::Surface:
          break;
        case Graph3DCommon::Plot3DType::Bar:
          if (axisori == "x")
            axis = barmodifier_->getGraph()->rowAxis();
          else if (axisori == "y")
            axis = barmodifier_->getGraph()->columnAxis();
          break;
        case Graph3DCommon::Plot3DType::Scatter:
          break;
      }
      if (!axis)
        xmlreader->raiseError(
            "Plot3D category axis unable to initialize error!");

      // Axis autorange
      bool autorange = xmlreader->readAttributeBool("autorange", &ok);
      (ok) ? axis->setAutoAdjustRange(autorange)
           : xmlreader->raiseWarning(
                 "Plot3D category axis autorange setting error!");

      // Axis from
      double from = xmlreader->readAttributeDouble("from", &ok);
      (ok)
          ? axis->setMin(from)
          : xmlreader->raiseWarning("Plot3D category axis from setting error!");

      // Axis to
      double to = xmlreader->readAttributeDouble("to", &ok);
      (ok) ? axis->setMax(to)
           : xmlreader->raiseWarning("Plot3D category axis to setting error!");

      // Axis tick label rotation
      double tklblrotation =
          xmlreader->readAttributeDouble("ticklabelrotation", &ok);
      (ok) ? axis->setLabelAutoRotation(tklblrotation)
           : xmlreader->raiseWarning(
                 "Plot3D category axis tick label rotation setting error!");

      // Axis label visible
      bool lblvisible = xmlreader->readAttributeBool("labelvisible", &ok);
      (ok) ? axis->setTitleVisible(lblvisible)
           : xmlreader->raiseWarning(
                 "Plot3D category axis title visible setting error!");

      // Axis label fixed
      bool lblfixed = xmlreader->readAttributeBool("labelfixed", &ok);
      (ok) ? axis->setTitleFixed(lblfixed)
           : xmlreader->raiseWarning(
                 "Plot3D category axis title fixed setting error!");

      // Axis label text
      QString label = xmlreader->readAttributeString("label", &ok);
      (ok) ? axis->setTitle(label)
           : xmlreader->raiseWarning(
                 "Plot3D category axis label setting error!");
    }
  }
}

void Layout3D::save(XmlStreamWriter *xmlwriter, const bool saveastemplate) {
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
  QDateTime datetime = QDateTime::fromString(birthDate(), Qt::LocalDate);
  xmlwriter->writeAttribute("creation_time",
                            datetime.toString("yyyy-dd-MM hh:mm:ss:zzz"));
  xmlwriter->writeAttribute("caption_spec", QString::number(captionPolicy()));
  xmlwriter->writeAttribute("name", name());
  xmlwriter->writeAttribute("label", windowLabel());
  saveCanvas(xmlwriter);
  saveTheme(xmlwriter);
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface:
      saveValueAxis(xmlwriter, surfacemodifier_->getGraph()->axisX());
      saveValueAxis(xmlwriter, surfacemodifier_->getGraph()->axisY());
      saveValueAxis(xmlwriter, surfacemodifier_->getGraph()->axisZ());
      surfacemodifier_->save(xmlwriter, saveastemplate);
      break;
    case Graph3DCommon::Plot3DType::Bar:
      saveCategoryAxis(xmlwriter, barmodifier_->getGraph()->columnAxis());
      saveCategoryAxis(xmlwriter, barmodifier_->getGraph()->rowAxis());
      saveValueAxis(xmlwriter, barmodifier_->getGraph()->valueAxis());
      barmodifier_->save(xmlwriter, saveastemplate);
      break;
    case Graph3DCommon::Plot3DType::Scatter:
      saveValueAxis(xmlwriter, scattermodifier_->getGraph()->axisX());
      saveValueAxis(xmlwriter, scattermodifier_->getGraph()->axisY());
      saveValueAxis(xmlwriter, scattermodifier_->getGraph()->axisZ());
      scattermodifier_->save(xmlwriter, saveastemplate);
      break;
  }
  xmlwriter->writeEndElement();
}

void Layout3D::saveCanvas(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("canvas");
  Q3DTheme *theme = graph_->activeTheme();
  switch (theme->type()) {
    case Q3DTheme::Theme::ThemeQt:
      xmlwriter->writeAttribute("themename", "themeqt");
      break;
    case Q3DTheme::Theme::ThemeDigia:
      xmlwriter->writeAttribute("themename", "themedigia");
      break;
    case Q3DTheme::Theme::ThemeEbony:
      xmlwriter->writeAttribute("themename", "themeebony");
      break;
    case Q3DTheme::Theme::ThemeRetro:
      xmlwriter->writeAttribute("themename", "themeretro");
      break;
    case Q3DTheme::Theme::ThemeArmyBlue:
      xmlwriter->writeAttribute("themename", "themearmyblue");
      break;
    case Q3DTheme::Theme::ThemeIsabelle:
      xmlwriter->writeAttribute("themename", "themeisabelle");
      break;
    case Q3DTheme::Theme::ThemeStoneMoss:
      xmlwriter->writeAttribute("themename", "themestonemoss");
      break;
    case Q3DTheme::Theme::ThemeUserDefined:
      xmlwriter->writeAttribute("themename", "themeuserdefined");
      break;
    case Q3DTheme::Theme::ThemePrimaryColors:
      xmlwriter->writeAttribute("themename", "themeprimarycolors");
      break;
  }
  xmlwriter->writeEndElement();
}

void Layout3D::saveTheme(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("theme");
  Q3DTheme *theme = graph_->activeTheme();
  xmlwriter->writeAttribute("canvascolor",
                            theme->windowColor().name(QColor::HexArgb));
  xmlwriter->writeStartElement("background");
  (theme->isBackgroundEnabled())
      ? xmlwriter->writeAttribute("visible", "true")
      : xmlwriter->writeAttribute("visible", "false");
  xmlwriter->writeAttribute("color",
                            theme->backgroundColor().name(QColor::HexArgb));
  xmlwriter->writeEndElement();
  xmlwriter->writeStartElement("light");
  xmlwriter->writeAttribute("color", theme->lightColor().name(QColor::HexArgb));
  xmlwriter->writeAttribute("strength",
                            QString::number(theme->lightStrength()));
  xmlwriter->writeAttribute("ambientstrength",
                            QString::number(theme->ambientLightStrength()));
  xmlwriter->writeEndElement();
  xmlwriter->writeStartElement("grid");
  (theme->isGridEnabled()) ? xmlwriter->writeAttribute("visible", "true")
                           : xmlwriter->writeAttribute("visible", "false");
  xmlwriter->writeAttribute("color",
                            theme->gridLineColor().name(QColor::HexArgb));
  xmlwriter->writeEndElement();
  xmlwriter->writeStartElement("label");
  (theme->isLabelBackgroundEnabled())
      ? xmlwriter->writeAttribute("backgroundvisible", "true")
      : xmlwriter->writeAttribute("backgroundvisible", "false");
  xmlwriter->writeAttribute(
      "backgroundcolor", theme->labelBackgroundColor().name(QColor::HexArgb));
  (theme->isLabelBorderEnabled())
      ? xmlwriter->writeAttribute("bordervisible", "true")
      : xmlwriter->writeAttribute("bordervisible", "false");
  xmlwriter->writeFont(theme->font(), theme->labelTextColor());
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
}

void Layout3D::saveValueAxis(XmlStreamWriter *xmlwriter, QValue3DAxis *axis) {
  xmlwriter->writeStartElement("axis");
  xmlwriter->writeAttribute("type", "value");
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface:
      if (surfacemodifier_->getGraph()->axisX() == axis)
        xmlwriter->writeAttribute("orientation", "x");
      else if (surfacemodifier_->getGraph()->axisY() == axis)
        xmlwriter->writeAttribute("orientation", "y");
      else if (surfacemodifier_->getGraph()->axisZ() == axis)
        xmlwriter->writeAttribute("orientation", "z");
      break;
    case Graph3DCommon::Plot3DType::Bar:
      if (barmodifier_->getGraph()->valueAxis() == axis)
        xmlwriter->writeAttribute("orientation", "z");
      break;
    case Graph3DCommon::Plot3DType::Scatter:
      if (scattermodifier_->getGraph()->axisX() == axis)
        xmlwriter->writeAttribute("orientation", "x");
      else if (scattermodifier_->getGraph()->axisY() == axis)
        xmlwriter->writeAttribute("orientation", "y");
      else if (scattermodifier_->getGraph()->axisZ() == axis)
        xmlwriter->writeAttribute("orientation", "z");
      break;
  }
  (axis->isAutoAdjustRange()) ? xmlwriter->writeAttribute("autorange", "true")
                              : xmlwriter->writeAttribute("autorange", "false");
  xmlwriter->writeAttribute("from", QString::number(axis->min()));
  xmlwriter->writeAttribute("to", QString::number(axis->max()));
  (axis->reversed()) ? xmlwriter->writeAttribute("reverse", "true")
                     : xmlwriter->writeAttribute("reverse", "false");
  xmlwriter->writeAttribute("tickcount", QString::number(axis->segmentCount()));
  xmlwriter->writeAttribute("subtickcount",
                            QString::number(axis->subSegmentCount()));
  xmlwriter->writeAttribute("ticklabelformat", axis->labelFormat());
  xmlwriter->writeAttribute("ticklabelrotation",
                            QString::number(axis->labelAutoRotation()));
  (axis->isTitleVisible()) ? xmlwriter->writeAttribute("labelvisible", "true")
                           : xmlwriter->writeAttribute("labelvisible", "false");
  (axis->isTitleFixed()) ? xmlwriter->writeAttribute("labelfixed", "true")
                         : xmlwriter->writeAttribute("labelfixed", "false");
  xmlwriter->writeAttribute("label", axis->title());
  xmlwriter->writeEndElement();
}

void Layout3D::saveCategoryAxis(XmlStreamWriter *xmlwriter,
                                QCategory3DAxis *axis) {
  xmlwriter->writeStartElement("axis");
  xmlwriter->writeAttribute("type", "category");
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface:
      break;
    case Graph3DCommon::Plot3DType::Bar:
      if (barmodifier_->getGraph()->rowAxis() == axis)
        xmlwriter->writeAttribute("orientation", "x");
      else if (barmodifier_->getGraph()->columnAxis() == axis)
        xmlwriter->writeAttribute("orientation", "y");
      break;
    case Graph3DCommon::Plot3DType::Scatter:
      break;
  }
  (axis->isAutoAdjustRange()) ? xmlwriter->writeAttribute("autorange", "true")
                              : xmlwriter->writeAttribute("autorange", "false");
  xmlwriter->writeAttribute("from", QString::number(axis->min()));
  xmlwriter->writeAttribute("to", QString::number(axis->max()));
  xmlwriter->writeAttribute("ticklabelrotation",
                            QString::number(axis->labelAutoRotation()));
  (axis->isTitleVisible()) ? xmlwriter->writeAttribute("labelvisible", "true")
                           : xmlwriter->writeAttribute("labelvisible", "false");
  (axis->isTitleFixed()) ? xmlwriter->writeAttribute("labelfixed", "true")
                         : xmlwriter->writeAttribute("labelfixed", "false");
  xmlwriter->writeAttribute("label", axis->title());
  xmlwriter->writeEndElement();
}

QList<MyWidget *> Layout3D::dependentTableMatrix() {
  QList<MyWidget *> dependeon;
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface: {
      QVector<DataBlockSurface3D *> sdata = surfacemodifier_->getData();
      foreach (DataBlockSurface3D *block, sdata) {
        if (block->ismatrix() && block->getmatrix())
          dependeon << block->getmatrix();
        else if (block->istable() && block->gettable())
          dependeon << block->gettable();
      }
    } break;
    case Graph3DCommon::Plot3DType::Bar: {
      QVector<DataBlockBar3D *> sdata = barmodifier_->getData();
      foreach (DataBlockBar3D *block, sdata) {
        if (block->ismatrix() && block->getmatrix())
          dependeon << block->getmatrix();
        else if (block->gettable())
          dependeon << block->gettable();
      }
    } break;
    case Graph3DCommon::Plot3DType::Scatter: {
      QVector<DataBlockScatter3D *> sdata = scattermodifier_->getData();
      foreach (DataBlockScatter3D *block, sdata) {
        if (block->ismatrix() && block->getmatrix())
          dependeon << block->getmatrix();
        else if (block->gettable())
          dependeon << block->gettable();
      }
    } break;
  }
  return dependeon;
}

void Layout3D::copy(Layout3D *layout, QList<Table *> tables,
                    QList<Matrix *> matrixs, ApplicationWindow *app) {
  std::unique_ptr<QTemporaryFile> file =
      std::unique_ptr<QTemporaryFile>(new QTemporaryFile("temp"));
  if (!file->open()) {
    qDebug() << "failed to open xml file for writing";
    return;
  }
  std::unique_ptr<XmlStreamWriter> xmlwriter =
      std::unique_ptr<XmlStreamWriter>(new XmlStreamWriter(file.get()));
  xmlwriter->setCodec("UTF-8");
  xmlwriter->setAutoFormatting(false);
  layout->save(xmlwriter.get());
  file->close();
  if (!file->open()) {
    qDebug() << "failed to read xml file for writing";
    return;
  }
  std::unique_ptr<XmlStreamReader> xmlreader =
      std::unique_ptr<XmlStreamReader>(new XmlStreamReader(file.get()));

  QXmlStreamReader::TokenType token;
  while (!xmlreader->atEnd()) {
    token = xmlreader->readNext();
    if (token == QXmlStreamReader::StartElement &&
        xmlreader->name() == "plot3d") {
      load(xmlreader.get(), tables, matrixs, app, false);
    }
  }
  file->close();
}

void Layout3D::print() {
  std::unique_ptr<QPrinter> printer = std::unique_ptr<QPrinter>(new QPrinter);
  std::unique_ptr<QPrintPreviewDialog> previewDialog =
      std::unique_ptr<QPrintPreviewDialog>(
          new QPrintPreviewDialog(printer.get(), this));
  connect(previewDialog.get(), &QPrintPreviewDialog::paintRequested,
          [=](QPrinter *printer) {
            printer->setColorMode(QPrinter::Color);
            std::unique_ptr<QPainter> painter =
                std::unique_ptr<QPainter>(new QPainter(printer));

            QImage image = QImage();
            QSize size = QSize(main_widget_->width(), main_widget_->height());
            switch (plottype_) {
              case Graph3DCommon::Plot3DType::Surface:
                image = graph3dsurface_->renderToImage(64, size);
                break;
              case Graph3DCommon::Plot3DType::Bar:
                image = graph3dbars_->renderToImage(64, size);
                break;
              case Graph3DCommon::Plot3DType::Scatter:
                image = graph3dscatter_->renderToImage(64, size);
                break;
            }
            // int dpm = 72 / 0.0254;
            // image.setDotsPerMeterX(dpm);
            // image.setDotsPerMeterY(dpm);

            QPointF point = QPointF((printer->pageLayout()
                                         .paintRectPixels(printer->resolution())
                                         .width() /
                                     2) -
                                        (size.width() / 2),
                                    (printer->pageLayout()
                                         .paintRectPixels(printer->resolution())
                                         .height() /
                                     2) -
                                        (size.height() / 2));
            painter->drawImage(point.x(), point.y(), image);
          });
  previewDialog->exec();
}

void Layout3D::copyToClipbord() {
  QImage image = QImage();
  QSize size = QSize(main_widget_->width(), main_widget_->height());
  switch (plottype_) {
    case Graph3DCommon::Plot3DType::Surface:
      image = graph3dsurface_->renderToImage(64, size);
      break;
    case Graph3DCommon::Plot3DType::Bar:
      image = graph3dbars_->renderToImage(64, size);
      break;
    case Graph3DCommon::Plot3DType::Scatter:
      image = graph3dscatter_->renderToImage(64, size);
      break;
  }
  QGuiApplication::clipboard()->setImage(image, QClipboard::Clipboard);
}
