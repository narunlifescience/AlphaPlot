#include "Layout2D.h"
#include <QSvgGenerator>
#include <QVBoxLayout>
#include "QDateTime"

#include "../core/IconLoader.h"
#include "../future/core/column/Column.h"
#include "AxisRect2D.h"
#include "LayoutGrid2D.h"
#include "LineScatter2D.h"

LayoutButton::LayoutButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent) {
  setToggleButton(true);
  setOn(true);
  setMaximumWidth(LayoutButton::btnSize());
  setMaximumHeight(LayoutButton::btnSize());
}

LayoutButton::~LayoutButton() {}

void LayoutButton::mousePressEvent(QMouseEvent *event) {
  if (!isOn()) {
    emit clicked(this);
  }
  if (event->button() == Qt::RightButton) {
    emit showContextMenu();
  }
}

void LayoutButton::mouseDoubleClickEvent(QMouseEvent *) {
  emit showCurvesDialog();
}

Layout2D::Layout2D(const QString &label, QWidget *parent, const QString name,
                   Qt::WFlags f)
    : MyWidget(label, parent, name, f),
      plot2dCanvas_(new Plot2D(this)),
      layout_(new LayoutGrid2D()) {
  if (name.isEmpty()) setObjectName("multilayer plot");

  QPalette pal = palette();
  pal.setColor(QPalette::Active, QPalette::Window, QColor(Qt::white));
  pal.setColor(QPalette::Inactive, QPalette::Window, QColor(Qt::white));
  pal.setColor(QPalette::Disabled, QPalette::Window, QColor(Qt::white));
  setPalette(pal);

  QDateTime birthday = QDateTime::currentDateTime();
  setBirthDate(birthday.toString(Qt::LocalDate));

  toolbuttonsBox_ = new QHBoxLayout();
  addLayoutButton_ = new QPushButton();
  addLayoutButton_->setToolTip(tr("Add layer"));
  addLayoutButton_->setIcon(
      IconLoader::load("list-add", IconLoader::LightDark));
  addLayoutButton_->setMaximumWidth(LayoutButton::btnSize());
  addLayoutButton_->setMaximumHeight(LayoutButton::btnSize());
  connect(addLayoutButton_, SIGNAL(clicked()), this, SLOT(addAxisRectItem()));
  toolbuttonsBox_->addWidget(addLayoutButton_);

  removeLayoutButton_ = new QPushButton();
  removeLayoutButton_->setToolTip(tr("Remove active layer"));
  removeLayoutButton_->setIcon(
      IconLoader::load("list-remove", IconLoader::General));
  removeLayoutButton_->setMaximumWidth(LayoutButton::btnSize());
  removeLayoutButton_->setMaximumHeight(LayoutButton::btnSize());
  connect(removeLayoutButton_, SIGNAL(clicked()), this,
          SLOT(removeAxisRectItem()));
  toolbuttonsBox_->addWidget(removeLayoutButton_);

  layoutButtonsBox_ = new QHBoxLayout();
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addLayout(layoutButtonsBox_);
  hbox->addStretch();
  hbox->addLayout(toolbuttonsBox_);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addLayout(hbox);
  layout->addWidget(plot2dCanvas_, 1);
  layout->setMargin(0);
  layout->setSpacing(0);
  setGeometry(QRect(0, 0, 500, 400));
  setMinimumSize(QSize(100, 100));
  setFocusPolicy(Qt::StrongFocus);

  plot2dCanvas_->plotLayout()->addElement(0, 0, layout_);
  buttionlist_ = QList<LayoutButton *>();

  // connections
  connect(plot2dCanvas_,
          SIGNAL(axisDoubleClick(QCPAxis *, QCPAxis::SelectablePart,
                                 QMouseEvent *)),
          this, SLOT(axisDoubleClicked(QCPAxis *, QCPAxis::SelectablePart)));
}

Layout2D::~Layout2D() {
  delete layout_;
  delete plot2dCanvas_;
}

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

QCPDataMap *Layout2D::generateDataMap(Column *xData, Column *yData, int from,
                                      int to) {
  QCPDataMap *dataMap = new QCPDataMap();
  for (int i = from; i < to; i++) {
    dataMap->insert(xData->valueAt(i),
                    QCPData(xData->valueAt(i), yData->valueAt(i)));
  }
  return dataMap;
}

void Layout2D::generateFunction2DPlot(QCPDataMap *dataMap, const double xMin,
                                      const double xMax, const double yMin,
                                      const double yMax, const QString yLabel) {
  AxisRectItem item = addAxisRectItem();
  AxisRect2D *element = nullptr;
  element =
      static_cast<AxisRect2D *>(layout_->elementAt(item.coordinates_.second));
  QList<Axis2D *> xaxs = item.axises_.value(Axis2D::Left);
  QList<Axis2D *> yaxs = item.axises_.value(Axis2D::Bottom);
  xaxs.at(0)->setRange(xMin, xMax);
  yaxs.at(0)->setRange(yMin, yMax);
  xaxs.at(0)->setLabel("x");
  yaxs.at(0)->setLabel(yLabel);

  LineScatter2D *linsc = element->addLineScatter2DPlot(
      AxisRect2D::Line2D, dataMap, xaxs.at(0), yaxs.at(0));
  QList<LineScatter2D *> list = item.lineScatter_.value(Line2D);
  list.append(linsc);
  item.lineScatter_.insert(Line2D, list);
  plot2dCanvas_->replot();
}

void Layout2D::generateLineScatter2DPlot(Column *xData, Column *yData, int from,
                                         int to) const {}

Layout2D::AxisRectItem Layout2D::getSelectedAxisRect(int col, int row) {
  int index = (layoutDimension_.first * col) + row;
  return layoutElementList_.value(index);
}

int Layout2D::getAxisRectIndex(Layout2D::AxisRectItem item) {
  QPair<int, int> pair = item.coordinates_;
  return (pair.first * layoutDimension_.first) + pair.second;
}

Layout2D::AxisRectItem Layout2D::addAxisRectItem() {
  int rowcount = layout_->rowCount();
  int colcount = layout_->columnCount();

  int lastIndex = layoutElementList_.size() - 1;
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
  }

  col = lastIndex + 1;

  AxisRect2D *axisRect2d = new AxisRect2D(plot2dCanvas_);
  Axis2D *xAxis = axisRect2d->addAxis2D(Axis2D::Bottom);
  Axis2D *yAxis = axisRect2d->addAxis2D(Axis2D::Left);
  Grid2D *xGrid = axisRect2d->bindGridTo(xAxis);
  Grid2D *yGrid = axisRect2d->bindGridTo(yAxis);
  xAxis->setRange(0, 100);
  yAxis->setRange(0, 100);
  layout_->addElement(row, col, axisRect2d);

  // set layour coordinate
  QPair<int, int> coord;
  coord.first = row;
  coord.second = col;

  // Initialize AxisRectItem
  AxisRectItem axisRectItem;
  axisRectItem.grids_.first = xGrid;
  axisRectItem.grids_.second = yGrid;
  // add coordinates
  axisRectItem.coordinates_.first = row;
  axisRectItem.coordinates_.second = col;
  // initialize & add axis
  axisRectItem.axises_.insert(Axis2D::Left, QList<Axis2D *>());
  axisRectItem.axises_.insert(Axis2D::Bottom, QList<Axis2D *>());
  axisRectItem.axises_.insert(Axis2D::Right, QList<Axis2D *>());
  axisRectItem.axises_.insert(Axis2D::Top, QList<Axis2D *>());
  axisRectItem.axises_.insert(Axis2D::Left,
                              axisRect2d->getAxesToMap(Axis2D::Left));
  axisRectItem.axises_.insert(Axis2D::Bottom,
                              axisRect2d->getAxesToMap(Axis2D::Bottom));
  axisRectItem.axises_.insert(Axis2D::Right,
                              axisRect2d->getAxesToMap(Axis2D::Right));
  axisRectItem.axises_.insert(Axis2D::Top,
                              axisRect2d->getAxesToMap(Axis2D::Top));
  // initialize linescatter
  axisRectItem.lineScatter_.insert(Line2D, QList<LineScatter2D *>());
  axisRectItem.lineScatter_.insert(Scatter2D, QList<LineScatter2D *>());
  axisRectItem.lineScatter_.insert(LineAndScatter2D, QList<LineScatter2D *>());
  axisRectItem.lineScatter_.insert(VerticalDropLine2D,
                                   QList<LineScatter2D *>());
  axisRectItem.lineScatter_.insert(Spline2D, QList<LineScatter2D *>());
  axisRectItem.lineScatter_.insert(CentralStepAndScatter2D,
                                   QList<LineScatter2D *>());
  axisRectItem.lineScatter_.insert(HorizontalStep2D, QList<LineScatter2D *>());
  axisRectItem.lineScatter_.insert(VerticalStep2D, QList<LineScatter2D *>());

  (rowcount + colcount == 0)
      ? layoutElementList_.insert((rowcount + colcount), axisRectItem)
      : layoutElementList_.insert((rowcount + colcount) - 1, axisRectItem);
  qDebug() << "list num" << (rowcount + colcount) - 1;
  plot2dCanvas_->replot();
  qDebug() << layoutElementList_.size();
  addLayoutButton(col);
  return axisRectItem;
}

void Layout2D::removeAxisRectItem() {
  removeAxisRect(layoutElementList_.size() - 1);
}

void Layout2D::setLayoutDimension(QPair<int, int> dimension) {
  layoutDimension_.first = dimension.first;
  layoutDimension_.second = dimension.second;
}

void Layout2D::removeAxisRect(int index) {
  if (index < 0) {
    qDebug() << "no element to remove from the layout";
    return;
  }
  layout_->remove(layout_->elementAt(index));
  layout_->simplify();
  layoutElementList_.remove(index);
  delete buttionlist_.value(index);
  buttionlist_.pop_front();
  // buttionlist_.removeLast();
  // buttionlist_.takeAt(index);
  plot2dCanvas_->replot();
}

void Layout2D::removeAxisRect(int col, int row) {
  delete layout_->element(row, col);
  int index = (layoutDimension_.first * col) + row;
  layoutElementList_.take(index);
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

LayoutButton *Layout2D::addLayoutButton(int num) {
  for (int i = 0; i < layoutElementList_.count(); i++) {
    // LayoutButton *btn = (LayoutButton *)buttonsList.at(i);
    // btn->setOn(false);
  }

  LayoutButton *button = new LayoutButton(QString::number(num));
  connect(button, SIGNAL(clicked(LayerButton *)), this,
          SLOT(activateGraph(LayerButton *)));
  connect(button, SIGNAL(showContextMenu()), this,
          SIGNAL(showLayerButtonContextMenu()));
  connect(button, SIGNAL(showCurvesDialog()), this, SIGNAL(showCurvesDialog()));

  // buttonsList.append(button);
  layoutButtonsBox_->addWidget(button);
  return button;
}

void Layout2D::axisDoubleClicked(QCPAxis *, QCPAxis::SelectablePart) {
  qDebug() << "axis dblclk";
}
