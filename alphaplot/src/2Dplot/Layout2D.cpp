#include "Layout2D.h"
#include <QDateTime>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QStyleOption>
#include <QVBoxLayout>

#include "../core/IconLoader.h"
#include "../core/Utilities.h"
#include "../future/core/column/Column.h"
#include "AxisRect2D.h"
#include "Bar2D.h"
#include "Curve2D.h"
#include "LayoutGrid2D.h"
#include "LineScatter2D.h"
#include "widgets/Axis2DPropertiesDialog.h"
#include "widgets/ImageExportDialog2D.h"
#include "widgets/LayoutButton2D.h"

#include <gsl/gsl_sort.h>
#include <gsl/gsl_statistics.h>

Layout2D::Layout2D(const QString &label, QWidget *parent, const QString name,
                   Qt::WFlags f)
    : MyWidget(label, parent, name, f),
      plot2dCanvas_(new Plot2D(this)),
      layout_(new LayoutGrid2D()),
      buttionlist_(QList<LayoutButton2D *>()),
      currentAxisRect_(nullptr),
      draggingLegend(false) {
  if (name.isEmpty()) setObjectName("multilayout2d plot");
  QDateTime birthday = QDateTime::currentDateTime();
  setBirthDate(birthday.toString(Qt::LocalDate));

  QPalette pal = palette();
  pal.setColor(QPalette::Active, QPalette::Window, QColor(Qt::white));
  pal.setColor(QPalette::Inactive, QPalette::Window, QColor(Qt::white));
  pal.setColor(QPalette::Disabled, QPalette::Window, QColor(Qt::white));
  setPalette(pal);

  layoutManagebuttonsBox_ = new QHBoxLayout();
  addLayoutButton_ = new QPushButton();
  addLayoutButton_->setToolTip(tr("Add layer"));
  addLayoutButton_->setIcon(
      IconLoader::load("list-add", IconLoader::LightDark));
  addLayoutButton_->setMaximumWidth(LayoutButton2D::btnSize());
  addLayoutButton_->setMaximumHeight(LayoutButton2D::btnSize());
  connect(addLayoutButton_, SIGNAL(clicked()), this, SLOT(addAxisRectItem()));
  layoutManagebuttonsBox_->addWidget(addLayoutButton_);

  removeLayoutButton_ = new QPushButton();
  removeLayoutButton_->setToolTip(tr("Remove active layer"));
  removeLayoutButton_->setIcon(
      IconLoader::load("list-remove", IconLoader::General));
  removeLayoutButton_->setMaximumWidth(LayoutButton2D::btnSize());
  removeLayoutButton_->setMaximumHeight(LayoutButton2D::btnSize());
  connect(removeLayoutButton_, SIGNAL(clicked()), this,
          SLOT(removeAxisRectItem()));
  layoutManagebuttonsBox_->addWidget(removeLayoutButton_);

  layoutButtonsBox_ = new QHBoxLayout();
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addLayout(layoutButtonsBox_);
  streachLabel_ = new QLabel(this);
  hbox->addWidget(streachLabel_);
  setBackground(plot2dCanvas_->getBackgroundColor());
  hbox->addLayout(layoutManagebuttonsBox_);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addLayout(hbox);
  layout->addWidget(plot2dCanvas_, 1);
  layout->setMargin(0);
  layout->setSpacing(0);
  setGeometry(QRect(0, 0, 500, 400));
  setMinimumSize(QSize(100, 100));
  setFocusPolicy(Qt::StrongFocus);

  plot2dCanvas_->plotLayout()->addElement(0, 0, layout_);

  // connections
  connect(plot2dCanvas_,
          SIGNAL(axisDoubleClick(QCPAxis *, QCPAxis::SelectablePart,
                                 QMouseEvent *)),
          this, SLOT(axisDoubleClicked(QCPAxis *, QCPAxis::SelectablePart)));
  connect(plot2dCanvas_, SIGNAL(mouseMove(QMouseEvent *)), this,
          SLOT(mouseMoveSignal(QMouseEvent *)));
  connect(plot2dCanvas_, SIGNAL(mousePress(QMouseEvent *)), this,
          SLOT(mousePressSignal(QMouseEvent *)));
  connect(plot2dCanvas_, SIGNAL(mouseRelease(QMouseEvent *)), this,
          SLOT(mouseReleaseSignal(QMouseEvent *)));
  connect(plot2dCanvas_, SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));

  connect(plot2dCanvas_, SIGNAL(mouseWheel(QWheelEvent *)), this,
          SLOT(mouseWheel()));
  connect(plot2dCanvas_,
          SIGNAL(legendDoubleClick(QCPLegend *, QCPAbstractLegendItem *,
                                   QMouseEvent *)),
          this, SLOT(legendDoubleClick(QCPLegend *, QCPAbstractLegendItem *)));
}

Layout2D::~Layout2D() {
  delete layout_;
  delete plot2dCanvas_;
}

StatBox2D::BoxWhiskerData Layout2D::generateBoxWhiskerData(Column *colData,
                                                           int from, int to,
                                                           int key) {
  size_t size = static_cast<size_t>((to - from) + 1);

  double *data = new double[size];

  for (int i = 0, j = from; j < to + 1; i++, j++) {
    data[i] = colData->valueAt(i);
  }
  // sort the data
  gsl_sort(data, 1, size - 1);

  StatBox2D::BoxWhiskerData statBoxData;
  statBoxData.key = key;
  // basic stats
  statBoxData.mean = gsl_stats_mean(data, 1, size);
  statBoxData.median = gsl_stats_median_from_sorted_data(data, 1, size);
  statBoxData.sd = gsl_stats_sd(data, 1, size);
  statBoxData.se = statBoxData.sd / sqrt(static_cast<double>(size));
  // data bounds
  statBoxData.boxWhiskerDataBounds.sd_lower = statBoxData.mean - statBoxData.sd;
  statBoxData.boxWhiskerDataBounds.sd_upper = statBoxData.mean + statBoxData.sd;
  statBoxData.boxWhiskerDataBounds.se_lower = statBoxData.mean - statBoxData.se;
  statBoxData.boxWhiskerDataBounds.se_upper = statBoxData.mean + statBoxData.se;
  statBoxData.boxWhiskerDataBounds.perc_1 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.01);
  statBoxData.boxWhiskerDataBounds.perc_5 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.05);
  statBoxData.boxWhiskerDataBounds.perc_10 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.10);
  statBoxData.boxWhiskerDataBounds.perc_25 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.25);
  statBoxData.boxWhiskerDataBounds.perc_75 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.75);
  statBoxData.boxWhiskerDataBounds.perc_90 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.90);
  statBoxData.boxWhiskerDataBounds.perc_95 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.95);
  statBoxData.boxWhiskerDataBounds.perc_99 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.99);
  statBoxData.boxWhiskerDataBounds.max = data[size - 1];
  statBoxData.boxWhiskerDataBounds.min = data[0];

  // delete the double data pointer
  delete[] data;

  return statBoxData;
}

void Layout2D::generateFunction2DPlot(QVector<double> *xdata,
                                      QVector<double> *ydata,
                                      const QString xLabel,
                                      const QString yLabel) {
  AxisRect2D *element = addAxisRectItem();
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
  xAxis.at(0)->setLabel(xLabel);
  yAxis.at(0)->setLabel(yLabel);

  QString name = "f(x) = " + yLabel;
  LineScatter2D *linescatter = element->addLineFunction2DPlot(
      xdata, ydata, xAxis.at(0), yAxis.at(0), name);
  linescatter->rescaleAxes();
  plot2dCanvas_->replot();
}

void Layout2D::generateParametric2DPlot(QVector<double> *xdata,
                                        QVector<double> *ydata,
                                        const QString xLabel,
                                        const QString yLabel) {
  AxisRect2D *element = addAxisRectItem();
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
  xAxis.at(0)->setLabel(xLabel);
  yAxis.at(0)->setLabel(yLabel);

  QString name = "f(x) = " + yLabel;
  Curve2D *curve = element->addCurveFunction2DPlot(xdata, ydata, xAxis.at(0),
                                                   yAxis.at(0), name);
  curve->rescaleAxes();
  plot2dCanvas_->replot();
}

void Layout2D::generateStatBox2DPlot(Column *data, int from, int to, int key) {
  StatBox2D::BoxWhiskerData statBoxData =
      generateBoxWhiskerData(data, from, to, key);
  AxisRect2D *element = addAxisRectItem();
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);

  StatBox2D *statBox = new StatBox2D(xAxis.at(0), yAxis.at(0), statBoxData);
  statBox->setBoxWhiskerWidth(0.5);
  statBox->rescaleAxes();

  QCPRange keyRange = statBox->keyAxis()->range();
  QCPRange valueRange = statBox->valueAxis()->range();
  double keyRangeSpan = keyRange.upper - keyRange.lower;
  double valueRangeSpan = valueRange.upper - valueRange.lower;
  statBox->keyAxis()->setRange(QCPRange(keyRange.lower - keyRangeSpan * 0.2,
                                        keyRange.upper + keyRangeSpan * 0.2));
  statBox->valueAxis()->setRange(
      QCPRange(valueRange.lower - valueRangeSpan * 0.2,
               valueRange.upper + valueRangeSpan * 0.2));
  plot2dCanvas_->replot();
}

void Layout2D::generateBar2DPlot(const BarType &barType, Column *xData,
                                 Column *yData, int from, int to) {
  AxisRect2D *element = addAxisRectItem();
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
  Bar2D *bar = nullptr;
  AxisRect2D::BarType type;
  switch (barType) {
    case HorizontalBars:
      type = AxisRect2D::HorizontalBars;
      break;
    case VerticalBars:
      type = AxisRect2D::VerticalBars;
      break;
  }

  bar = element->addBox2DPlot(type, xData, yData, from, to, xAxis.at(0),
                              yAxis.at(0));
  bar->setName("Table " + QString::number(xData->index() + 1) + "_" +
               QString::number(yData->index() + 1));
  bar->rescaleAxes();
  plot2dCanvas_->replot();
}

void Layout2D::generateVector2DPlot(const Vector2D::VectorPlot &vectorplot,
                                    Column *x1Data, Column *y1Data,
                                    Column *x2Data, Column *y2Data, int from,
                                    int to) {
  AxisRect2D *element = addAxisRectItem();
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);

  Vector2D *vex =
      element->addVectorPlot(vectorplot, x1Data, y1Data, x2Data, y2Data, from,
                             to, xAxis.at(0), yAxis.at(0));
  vex->setName("Table " + QString::number(x1Data->index() + 1) + "_" +
               QString::number(y1Data->index() + 1) + "_" +
               QString::number(x2Data->index() + 1) + "_" +
               QString::number(y2Data->index() + 1));
  vex->rescaleAxes();
  plot2dCanvas_->replot();
}

void Layout2D::generatePie2DPlot(Column *xData, int from, int to) {
  AxisRect2D *element = addAxisRectItem();

  Pie2D *pie = element->addPie2DPlot(xData, from, to);
  // pie->rescaleAxes();
  plot2dCanvas_->replot();
}

QList<AxisRect2D *> Layout2D::getAxisRectList() {
  QList<AxisRect2D *> elementslist;
  for (int i = 0; i < layout_->elementCount(); i++) {
    elementslist.append(static_cast<AxisRect2D *>(layout_->elementAt(i)));
  }
  return elementslist;
}

void Layout2D::generateLineScatter2DPlot(const LineScatterType &plotType,
                                         Column *xData, Column *yData, int from,
                                         int to) {
  AxisRect2D *element = addAxisRectItem();
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);

  LineScatter2D *linescatter = nullptr;

  switch (plotType) {
    case Line2D: {
      linescatter = element->addLineScatter2DPlot(
          AxisRect2D::Line2D, xData, yData, from, to, xAxis.at(0), yAxis.at(0));
    } break;
    case Scatter2D: {
      linescatter =
          element->addLineScatter2DPlot(AxisRect2D::Scatter2D, xData, yData,
                                        from, to, xAxis.at(0), yAxis.at(0));
    } break;
    case LineAndScatter2D: {
      linescatter = element->addLineScatter2DPlot(AxisRect2D::LineAndScatter2D,
                                                  xData, yData, from, to,
                                                  xAxis.at(0), yAxis.at(0));
    } break;
    case VerticalDropLine2D: {
      linescatter = element->addLineScatter2DPlot(
          AxisRect2D::VerticalDropLine2D, xData, yData, from, to, xAxis.at(0),
          yAxis.at(0));
    } break;
    case CentralStepAndScatter2D: {
      linescatter = element->addLineScatter2DPlot(
          AxisRect2D::CentralStepAndScatter2D, xData, yData, from, to,
          xAxis.at(0), yAxis.at(0));
    } break;
    case HorizontalStep2D: {
      linescatter = element->addLineScatter2DPlot(AxisRect2D::HorizontalStep2D,
                                                  xData, yData, from, to,
                                                  xAxis.at(0), yAxis.at(0));
    } break;
    case VerticalStep2D: {
      linescatter = element->addLineScatter2DPlot(AxisRect2D::VerticalStep2D,
                                                  xData, yData, from, to,
                                                  xAxis.at(0), yAxis.at(0));
    } break;
    case Area2D: {
      linescatter = element->addLineScatter2DPlot(
          AxisRect2D::Area2D, xData, yData, from, to, xAxis.at(0), yAxis.at(0));
    } break;
  }

  linescatter->setName("Table " + QString::number(xData->index() + 1) + "_" +
                       QString::number(yData->index() + 1));
  linescatter->rescaleAxes();
  plot2dCanvas_->replot();
}

void Layout2D::generateSpline2DPlot(Column *xData, Column *yData, int from,
                                    int to) {
  AxisRect2D *element = addAxisRectItem();
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);

  Spline2D *spline = element->addSpline2DPlot(xData, yData, from, to,
                                              xAxis.at(0), yAxis.at(0));

  spline->setName("Table " + QString::number(xData->index() + 1) + "_" +
                  QString::number(yData->index() + 1));
  spline->rescaleAxes();
  plot2dCanvas_->replot();
}

AxisRect2D *Layout2D::getSelectedAxisRect(int col, int row) {
  return static_cast<AxisRect2D *>(layout_->element(row, col));
}

int Layout2D::getAxisRectIndex(AxisRect2D *axisRect2d) {
  AxisRect2D *axisRect;
  for (int i = 0; i < layout_->elementCount(); i++) {
    axisRect = static_cast<AxisRect2D *>(layout_->elementAt(i));
    if (axisRect == axisRect2d) return i;
  }

  return static_cast<int>(std::nan("invalid index"));
}

AxisRect2D *Layout2D::addAxisRectItem() {
  /*int rowcount = layout_->rowCount();
  int colcount = layout_->columnCount();

  int lastIndex = layout_->elementCount() - 1;
  int col = 0, row = 0;
  if (lastIndex + 1 > 0) {
    int maxLastIndex = (rowcount * colcount) - 1;

    if (lastIndex == maxLastIndex) {
      // col = colcount;
      // row = 0;
      col = colcount;
      row = 0;
    } else {
      col = colcount;
      row = 0;
    }
  }*/

  int row = 0;
  int col = layout_->elementCount();

  AxisRect2D *axisRect2d = new AxisRect2D(plot2dCanvas_);
  // axisrectitem->setData(0, Qt::UserRole, 3);

  Axis2D *xAxis = axisRect2d->addAxis2D(Axis2D::AxisOreantation::Bottom);
  Axis2D *yAxis = axisRect2d->addAxis2D(Axis2D::AxisOreantation::Left);

  axisRect2d->bindGridTo(xAxis);
  axisRect2d->bindGridTo(yAxis);

  xAxis->setRange(0, 100);
  yAxis->setRange(0, 100);
  xAxis->setLabel("X Axis Title");
  yAxis->setLabel("Y Axis Title");
  layout_->addElement(row, col, axisRect2d);

  plot2dCanvas_->replot();
  addLayoutButton(col);

  connect(axisRect2d, SIGNAL(AxisRectClicked(AxisRect2D *)), this,
          SLOT(axisRectSetFocus(AxisRect2D *)));

  emit AxisRectCreated(axisRect2d, this);
  if (!currentAxisRect_) axisRectSetFocus(axisRect2d);
  return axisRect2d;
}

void Layout2D::removeAxisRectItem() {
  removeAxisRect(getAxisRectIndex(currentAxisRect_));
  emit AxisRectRemoved(nullptr, this);
}

void Layout2D::axisRectSetFocus(AxisRect2D *rect) {
  if (!rect) return;

  LayoutButton2D *button;
  if (currentAxisRect_) {
    if (currentAxisRect_ != rect) {
      currentAxisRect_->setSelected(false);
      button = buttionlist_.at(getAxisRectIndex(currentAxisRect_));
      if (button) button->setActive(false);
    }
  }
  currentAxisRect_ = rect;
  currentAxisRect_->setSelected(true);
  button = buttionlist_.at(getAxisRectIndex(rect));
  if (button) button->setActive(true);
  plot2dCanvas_->replot();
}

void Layout2D::activateLayout(LayoutButton2D *button) {
  for (int i = 0; i < buttionlist_.size(); i++) {
    if (buttionlist_.at(i) == button) {
      axisRectSetFocus(static_cast<AxisRect2D *>(layout_->elementAt(i)));
      break;
    }
  }
}

void Layout2D::mouseMoveSignal(QMouseEvent *event) {
  // dragging legend
  if (draggingLegend) {
    QRectF rect = currentAxisRect_->insetLayout()->insetRect(0);

    // set bounding rect for drag event
    QPoint eventpos = event->pos();
    if (event->pos().x() < currentAxisRect_->left()) {
      eventpos.setX(currentAxisRect_->left());
    }
    if (event->pos().x() > currentAxisRect_->right()) {
      eventpos.setX(currentAxisRect_->right());
    }
    if (event->pos().y() < currentAxisRect_->top()) {
      eventpos.setY(currentAxisRect_->top());
    }
    if (event->pos().y() > currentAxisRect_->bottom()) {
      eventpos.setY(currentAxisRect_->bottom());
    }

    // since insetRect is in axisRect coordinates (0..1), we transform the mouse
    // position:
    QPointF mousePoint((eventpos.x() - currentAxisRect_->left()) /
                           static_cast<double>(currentAxisRect_->width()),
                       (eventpos.y() - currentAxisRect_->top()) /
                           static_cast<double>(currentAxisRect_->height()));

    rect.moveTopLeft(mousePoint - dragLegendOrigin);
    currentAxisRect_->insetLayout()->setInsetRect(0, rect);
    currentAxisRect_->layer()->replot();
  }
}

void Layout2D::mousePressSignal(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    // dragging legend
    if (currentAxisRect_->selectTest(event->pos(), false) > 0) {
      QCPLegend *l = currentAxisRect_->getLegend();
      if (l->selectTest(event->pos(), false) > 0) {
        draggingLegend = true;
        // since insetRect is in axisRect coordinates (0..1), we transform the
        // mouse position:
        QPointF mousePoint((event->pos().x() - currentAxisRect_->left()) /
                               static_cast<double>(currentAxisRect_->width()),
                           (event->pos().y() - currentAxisRect_->top()) /
                               static_cast<double>(currentAxisRect_->height()));
        dragLegendOrigin =
            mousePoint -
            currentAxisRect_->insetLayout()->insetRect(0).topLeft();
        setCursor(Qt::ClosedHandCursor);
      }
    }
  }
}

void Layout2D::mouseReleaseSignal(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    if (draggingLegend) {
      draggingLegend = false;
      unsetCursor();
    }
  } else {
    QPointF startPos = event->posF();
    QMenu *menu = new QMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction("Export", this, SLOT(exportGraph()));
    menu->addAction("Print", this, SLOT(printGraph()));
    menu->popup(plot2dCanvas_->mapToGlobal(QPoint(
        static_cast<int>(startPos.x()), static_cast<int>(startPos.y()))));
  }
}

void Layout2D::mouseWheel() {
  // zoom axis individually or in group
  //  if (currentAxisRect_->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  //    currentAxisRect_->setRangeZoom(ui->customPlot->xAxis->orientation());
  //  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
  //    currentAxisRect_->setRangeZoom(ui->customPlot->yAxis->orientation());
  //  else
  currentAxisRect_->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}

void Layout2D::beforeReplot() {
  if (currentAxisRect_) {
    currentAxisRect_->updateLegendRect();
  }
}

void Layout2D::legendDoubleClick(QCPLegend *legend,
                                 QCPAbstractLegendItem *item) {
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item)  // only react if item was clicked (user could have clicked on
             // border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem =
        qobject_cast<QCPPlottableLegendItem *>(item);
    bool ok;
    QString newName = QInputDialog::getText(
        this, "QCustomPlot example", "New graph name:", QLineEdit::Normal,
        plItem->plottable()->name(), &ok);
    if (ok) {
      plItem->plottable()->setName(newName);
      plot2dCanvas_->replot();
    }
  }
}

bool Layout2D::exportGraph() {
  std::unique_ptr<ImageExportDialog2D> ied(
      new ImageExportDialog2D(nullptr, plot2dCanvas_ != nullptr));
  ied->setraster_height(plot2dCanvas_->height());
  ied->setraster_width(plot2dCanvas_->width());
  ied->setvector_height(plot2dCanvas_->height());
  ied->setraster_width(plot2dCanvas_->width());
  if (ied->exec() != QDialog::Accepted) return false;
  if (ied->selectedFiles().isEmpty()) return false;

  QString selected_filter = ied->selectedFilter();

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
    return false;
  }

  int raster_resolution = ied->raster_resolution();
  int raster_width = ied->raster_width();
  int raster_height = ied->raster_height();
  double raster_scale = ied->raster_scale();
  int raster_quality = ied->raster_quality();

  int vector_width = ied->vector_width();
  int vector_height = ied->vector_height();

  bool success = false;
  currentAxisRect_->setPrintorExportJob(true);
  if (selected_filter.contains(".pdf")) {
    success = plot2dCanvas_->savePdf(file_name, vector_width, vector_height);
  } else if (selected_filter.contains(".svg")) {
    success = plot2dCanvas_->saveSvg(file_name, vector_width, vector_height);
  } else if (selected_filter.contains(".ps")) {
    success = plot2dCanvas_->savePs(file_name, vector_width, vector_height);
  } else {
    QByteArray ba = selected_filter.toLatin1();
    ba.stripWhiteSpace();
    ba.remove(0, 1);
    const char *c_char = ba.data();
    success = plot2dCanvas_->saveRastered(file_name, raster_width,
                                          raster_height, raster_scale, c_char,
                                          raster_quality, raster_resolution);
  }
  currentAxisRect_->setPrintorExportJob(false);
  if (!success) {
    QMessageBox::critical(
        this, tr("Export Error"),
        tr("Unknown error exporting: <br><h4> %1 </h4><p>May be "
           "the advanced image export parameters are not rightfully set!")
            .arg(file_name));
  }
  return success;
}

void Layout2D::printGraph() {}

void Layout2D::setLayoutDimension(QPair<int, int> dimension) {
  layoutDimension_.first = dimension.first;
  layoutDimension_.second = dimension.second;
}

void Layout2D::removeAxisRect(int index) {
  // if no elements to remove
  if (index < 0) {
    qDebug() << "no element to remove from the layout";
    return;
  }

  // if removed element is the currently selected element
  AxisRect2D *axrect = static_cast<AxisRect2D *>(layout_->elementAt(index));

  if (!axrect) return;

  if (axrect->isSelected()) {
    axrect = nullptr;
    if (index == 0) {
      axrect = static_cast<AxisRect2D *>(layout_->elementAt(index + 1));
    } else {
      axrect = static_cast<AxisRect2D *>(layout_->elementAt(index - 1));
    }

    if (axrect) {
      axisRectSetFocus(axrect);
    } else {
      currentAxisRect_ = nullptr;
    }
  }

  // remove layout button
  buttionlist_.takeLast()->close();

  // remove the element & adjust layout accordingly
  layout_->remove(layout_->elementAt(index));
  layout_->simplify();
  plot2dCanvas_->replot();
}

int Layout2D::getLayoutRectGridIndex(QPair<int, int> coord) {
  int index = ((colorCount()) * coord.second) + coord.first;
  return index;
}

QPair<int, int> Layout2D::getLayoutRectGridCoordinate(int index) {
  QPair<int, int> pair;
  pair.first = index / (layout_->columnCount());
  pair.second = index % (layout_->columnCount());
  return pair;
}

LayoutButton2D *Layout2D::addLayoutButton(int num) {
  LayoutButton2D *button = new LayoutButton2D(QString::number(++num));

  connect(button, SIGNAL(clicked(LayoutButton2D *)), this,
          SLOT(activateLayout(LayoutButton2D *)));
  /*connect(button, SIGNAL(showContextMenu()), this,
          SIGNAL(showLayoutButtonContextMenu()));
  connect(button, SIGNAL(showCurvesDialog()), this,
  SIGNAL(showCurvesDialog()));*/

  buttionlist_ << button;
  layoutButtonsBox_->addWidget(button);
  return button;
}

void Layout2D::setBackground(const QColor &background) {
  QString baseColor = QString("rgba(%0,%1,%2,%3)")
                          .arg(background.red())
                          .arg(background.green())
                          .arg(background.blue())
                          .arg(background.alpha());
  streachLabel_->setStyleSheet("QLabel { background-color:" + baseColor + ";}");
}

void Layout2D::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  QRect rect(0, 0, size().rwidth(), size().rheight());
  painter.setBrush(QBrush(plot2dCanvas_->getBackgroundColor()));
  painter.drawRect(rect);
}

void Layout2D::axisDoubleClicked(QCPAxis *axis, QCPAxis::SelectablePart part) {
  Q_UNUSED(axis);
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel)  // only react when the actual axis label is
                                     // clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(
        this, "QCustomPlot example", "New axis label:", QLineEdit::Normal,
        axis->label(), &ok);
    if (ok) {
      axis->setLabel(newLabel);
      plot2dCanvas_->replot();
    }
  } else {
    Axis2DPropertiesDialog *axisPropertiesDialog =
        new Axis2DPropertiesDialog(this, axis, currentAxisRect_);
    connect(axisPropertiesDialog, SIGNAL(areplot()), plot2dCanvas_,
            SLOT(replot()));
    axisPropertiesDialog->exec();
  }
}
