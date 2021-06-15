#ifndef LAYOUT3D_H
#define LAYOUT3D_H

#include <QtDataVisualization>

#include "3Dplot/Graph3DCommon.h"
#include "MyWidget.h"

class Matrix;
class Table;
class Surface3D;
class Bar3D;
class Scatter3D;
class XmlStreamReader;
class XmlStreamWriter;
class Custom3DInteractions;
class ApplicationWindow;
using namespace QtDataVisualization;

class Layout3D : public MyWidget {
  Q_OBJECT
 public:
  Layout3D(const Graph3DCommon::Plot3DType &plottype, const QString &label,
           QWidget *parent = nullptr, const QString name = QString(),
           Qt::WindowFlags flag = Qt::SubWindow);
  ~Layout3D();

  Surface3D *getSurface3DModifier() const;
  Bar3D *getBar3DModifier() const;
  Scatter3D *getScatter3DModifier() const;
  QAbstract3DGraph *getAbstractGraph() const;
  void setCustomInteractions(QAbstract3DGraph *graph, bool status);
  void setAnimation(bool status);

  void exportGraph();
  void saveRastered(const QString &filename, const char *format,
                    const int quality, const int maa, int dpi,
                    const QSize &size);
  void exportGraphwithoutdialog(const QString &name,
                                const QString &selected_filter,
                                const QSize &size);
  void generateSurfacePlot3D(QList<QPair<QPair<double, double>, double>> *data,
                             const Graph3DCommon::Function3DData &funcdata);

  QSize getContainerSize() const;
  Graph3DCommon::Plot3DType getPlotType() const;
  void load(XmlStreamReader *xmlreader, QList<Table *> tabs,
            QList<Matrix *> mats, ApplicationWindow *app, bool setname = true);
  void loadCanvas(XmlStreamReader *xmlreader);
  void loadTheme(XmlStreamReader *xmlreader);
  void loadValueAxis(XmlStreamReader *xmlreader);
  void loadCategoryAxis(XmlStreamReader *xmlreader);
  void save(XmlStreamWriter *xmlwriter, const bool saveastemplate = false);
  void saveCanvas(XmlStreamWriter *xmlwriter);
  void saveTheme(XmlStreamWriter *xmlwriter);
  void saveValueAxis(XmlStreamWriter *xmlwriter, QValue3DAxis *axis);
  void saveCategoryAxis(XmlStreamWriter *xmlwriter, QCategory3DAxis *axis);
  QList<MyWidget *> dependentTableMatrix();
  void copy(Layout3D *layout, QList<Table *> tables, QList<Matrix *> matrixs,
            ApplicationWindow *app);
  void print();
  void copyToClipbord();

 signals:
  void dataAdded(MyWidget *mywidget);
  void showContextMenu();

 private:
  const Graph3DCommon::Plot3DType plottype_;
  QWidget *main_widget_;
  Q3DSurface *graph3dsurface_;
  Q3DBars *graph3dbars_;
  Q3DScatter *graph3dscatter_;
  Surface3D *surfacemodifier_;
  Bar3D *barmodifier_;
  Scatter3D *scattermodifier_;
  QAbstract3DGraph *graph_;
  Custom3DInteractions *custominter_;
  QPropertyAnimation *m_animationCameraX_;
  QSequentialAnimationGroup *m_animationCameraY_;
  QPropertyAnimation *upAnimation_;
  QPropertyAnimation *downAnimation_;
  static const int defaultlayout2dwidth_;
  static const int defaultlayout2dheight_;
  static const int minimumlayout2dwidth_;
  static const int minimumlayout2dheight_;
};
#endif  // LAYOUT3D_H
