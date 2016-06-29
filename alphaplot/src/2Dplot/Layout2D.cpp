#include "Layout2D.h"
#include "QDateTime"
#include <QVBoxLayout>

#include "LayoutGrid2D.h"
#include "AxisRect2D.h"

Layout2D::Layout2D(const QString &label, QWidget *parent, const QString name, Qt::WFlags f)
  : MyWidget(label, parent, name, f),
    plot2dCanvas_(new Plot2D(this)){
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
setMinimumHeight(50);
setFocusPolicy(Qt::StrongFocus);

QVector<double> x(101), y(101);  // initialize with entries 0..100
for (int i = 0; i < 101; ++i) {
  x[i] = i / 50.0 - 1;  // x goes from -1 to 1
  y[i] = x[i] * x[i];   // let's plot a quadratic function
}

AxisRect2D* axisLayout_ = new AxisRect2D(plot2dCanvas_);
Axis2D *xaxis1 = axisLayout_->addAxis2D(Axis2D::Bottom);
Axis2D *yaxis2 = axisLayout_->addAxis2D(Axis2D::Left);
Axis2D *yaxis1 = axisLayout_->addAxis2D(Axis2D::Right);
xaxis1->setRange(-1, 1);
yaxis2->setRange(0, 1);
xaxis1->setLabel("x");
yaxis2->setLabel("f(x)");
yaxis1->setLabel("f(x)");
QCPLayoutGrid *subLayout = new QCPLayoutGrid;
plot2dCanvas_->plotLayout()->addElement(0, 0, subLayout);

subLayout->addElement(0, 0, axisLayout_);
QCPGraph *graph = new QCPGraph(xaxis1, yaxis2);
axisLayout_->bindGridTo(xaxis1);
axisLayout_->bindGridTo(yaxis2);
graph->setData(x, y);
graph->setScatterStyle(QCPScatterStyle::ssDisc);
}

Layout2D::~Layout2D()
{
  delete plot2dCanvas_;
}
