#include "Layout2D.h"
#include <QSvgGenerator>
#include <QVBoxLayout>
#include "QDateTime"

#include "../future/core/column/Column.h"
#include "AxisRect2D.h"
#include "LayoutGrid2D.h"
#include "LineScatter2D.h"

Layout2D::Layout2D(const QString &label, QWidget *parent, const QString name,
                   Qt::WFlags f)
    : MyWidget(label, parent, name, f), plot2dCanvas_(new Plot2D(this)) {
  if (name.isEmpty()) setObjectName("multilayer plot");

  QPalette pal = palette();
  pal.setColor(QPalette::Active, QPalette::Window, QColor(Qt::white));
  pal.setColor(QPalette::Inactive, QPalette::Window, QColor(Qt::white));
  pal.setColor(QPalette::Disabled, QPalette::Window, QColor(Qt::white));
  setPalette(pal);

  QDateTime birthday = QDateTime::currentDateTime();
  setBirthDate(birthday.toString(Qt::LocalDate));

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(plot2dCanvas_);
  layout->setMargin(0);
  layout->setSpacing(0);
  setGeometry(QRect(0, 0, 500, 400));
  setMinimumSize(QSize(400, 400));
  setFocusPolicy(Qt::StrongFocus);

  // connections
  connect(plot2dCanvas_,
          SIGNAL(axisDoubleClick(QCPAxis *, QCPAxis::SelectablePart,
                                 QMouseEvent *)),
          this, SLOT(axisDoubleClicked(QCPAxis *, QCPAxis::SelectablePart)));

  //  QCPDataMap *dataMap = new QCPDataMap();
  //  QVector<double> x(51), y(51);  // initialize with entries 0..100
  //  for (int i = 0; i < 51; ++i) {
  //    x[i] = i / 50.0 - 1;  // x goes from -1 to 1
  //    y[i] = x[i] * x[i];   // let's plot a quadratic function
  //    dataMap->insert(x[i], QCPData(x[i], y[i]));
  //  }

  //  AxisRect2D *axisLayout_ = new AxisRect2D(plot2dCanvas_);
  //  Axis2D *xaxis1 = axisLayout_->addAxis2D(Axis2D::Bottom);
  //  Axis2D *yaxis2 = axisLayout_->addAxis2D(Axis2D::Left);
  //  xaxis1->setRange(-1.1, 1.1);
  //  yaxis2->setRange(-0.1, 1.1);
  //  xaxis1->setLabel("x");
  //  yaxis2->setLabel("f(x)");
  //  QCPLayoutGrid *subLayout = new QCPLayoutGrid;
  //  plot2dCanvas_->plotLayout()->addElement(0, 0, subLayout);

  //  subLayout->addElement(0, 0, axisLayout_);
  //  axisLayout_->bindGridTo(xaxis1);
  //  axisLayout_->bindGridTo(yaxis2);

  // LineScatter2D *linsc = new LineScatter2D(xaxis1, yaxis2);
  //  LineScatter2D *linsc = axisLayout_->addLineScatter2DPlot(
  //      AxisRect2D::VerticalDropLine2D, dataMap, xaxis1, yaxis2);
  //  linsc->setAntialiased(false);

  // linsc->setData(x,y);
  //  linsc->setLineScatter2DPlot(LineScatter2D::VerticalStepPlot,
  //                              LineScatter2D::ScatterHidden);

  //  linsc->setScatterPen2D(QPen(Qt::red, 0));
  //  linsc->setAntialiased(false);
  //  linsc->setAntialiasedScatters(false);
}

Layout2D::~Layout2D() { delete plot2dCanvas_; }

bool Layout2D::eventFilter(QObject *object, QEvent *e) {
  if (e->type() == QEvent::MouseButtonPress &&
      object == (QObject *)plot2dCanvas_) {
    const QMouseEvent *me = (const QMouseEvent *)e;
    return false;
  } else if (e->type() == QEvent::ContextMenu && object == titleBar) {
    emit showTitleBarMenu();
    ((QContextMenuEvent *)e)->accept();
    return true;
  }
  return MyWidget::eventFilter(object, e);
}

QCPDataMap *Layout2D::generateDataMap(Column *xData, Column *yData) {}

void Layout2D::generateFunction2DPlot(QCPDataMap *dataMap,
                                      const double xMin, const double xMax,
                                      const double yMin, const double yMax,
                                      const QString yLabel) const {
  AxisRect2D *axisLayout_ = new AxisRect2D(plot2dCanvas_);
  Axis2D *xaxis1 = axisLayout_->addAxis2D(Axis2D::Bottom);
  Axis2D *yaxis2 = axisLayout_->addAxis2D(Axis2D::Left);
  xaxis1->setRange(xMin, xMax);
  yaxis2->setRange(yMin, yMax);
  xaxis1->setLabel("x");
  yaxis2->setLabel(yLabel);
  QCPLayoutGrid *subLayout = new QCPLayoutGrid;
  plot2dCanvas_->plotLayout()->addElement(0, 0, subLayout);

  subLayout->addElement(0, 0, axisLayout_);
  axisLayout_->bindGridTo(xaxis1);
  axisLayout_->bindGridTo(yaxis2);

  LineScatter2D *linsc = axisLayout_->addLineScatter2DPlot(
      AxisRect2D::Line2D, dataMap, xaxis1, yaxis2);
  plot2dCanvas_->replot();
}

void Layout2D::axisDoubleClicked(QCPAxis *, QCPAxis::SelectablePart) {
  qDebug() << "axis dblclk";
}
