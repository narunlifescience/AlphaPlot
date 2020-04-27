#include "DataManager3D.h"

#include <QtDataVisualization/QBar3DSeries>
#include <QtDataVisualization/QBarDataProxy>
#include <QtDataVisualization/QItemModelBarDataProxy>
#include <QtDataVisualization/QItemModelScatterDataProxy>
#include <QtDataVisualization/QItemModelSurfaceDataProxy>
#include <QtDataVisualization/QScatter3DSeries>
#include <QtDataVisualization/QScatterDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataProxy>

#include "Matrix.h"
#include "Table.h"
#include "future/core/column/Column.h"

using namespace QtDataVisualization;

DataBlockAbstract3D::DataBlockAbstract3D()
    : matrix_(nullptr),
      table_(nullptr),
      xcolumn_(nullptr),
      ycolumn_(nullptr),
      zcolumns_(QList<Column *>()) {}

DataBlockAbstract3D::DataBlockAbstract3D(Matrix *matrix)
    : matrix_(matrix),
      table_(nullptr),
      xcolumn_(nullptr),
      ycolumn_(nullptr),
      zcolumns_(QList<Column *>()) {}

DataBlockAbstract3D::DataBlockAbstract3D(Table *table, Column *xcolumn,
                                         Column *ycolumn, Column *zcolumn)
    : matrix_(nullptr),
      table_(table),
      xcolumn_(xcolumn),
      ycolumn_(ycolumn),
      zcolumns_(QList<Column *>()) {
  zcolumns_ << zcolumn;
}

DataBlockAbstract3D::DataBlockAbstract3D(Table *table, Column *xcolumn,
                                         Column *ycolumn,
                                         QList<Column *> zcolumns)
    : matrix_(nullptr),
      table_(table),
      xcolumn_(xcolumn),
      ycolumn_(ycolumn),
      zcolumns_(zcolumns) {}

DataBlockAbstract3D::~DataBlockAbstract3D() {}

DataBlockSurface3D::DataBlockSurface3D(Matrix *matrix)
    : DataBlockAbstract3D(matrix),
      valueDataArray_(nullptr),
      valueDataProxy_(nullptr),
      dataSeries_(new QSurface3DSeries),
      modelDataProxy_(new QItemModelSurfaceDataProxy) {
  regenerateDataBlockModel();
}

DataBlockSurface3D::DataBlockSurface3D(Table *table, Column *xcolumn,
                                       Column *ycolumn,
                                       QList<Column *> zcolumns)
    : DataBlockAbstract3D(table, xcolumn, ycolumn, zcolumns),
      valueDataArray_(new QSurfaceDataArray),
      valueDataProxy_(new QSurfaceDataProxy),
      dataSeries_(new QSurface3DSeries),
      modelDataProxy_(nullptr) {
  regenerateDataBlockValue();
}

DataBlockSurface3D::DataBlockSurface3D(
    QList<QPair<QPair<double, double>, double> > *data,
    const Graph3DCommon::Function3DData &funcdata)
    : DataBlockAbstract3D(),
      funcData_((funcdata)),
      valueDataArray_(new QSurfaceDataArray),
      valueDataProxy_(new QSurfaceDataProxy),
      dataSeries_(new QSurface3DSeries),
      modelDataProxy_(nullptr) {
  regenerateDataBlockFunction(data);
}

DataBlockSurface3D::~DataBlockSurface3D() {}

void DataBlockSurface3D::regenerateDataBlockModel() {
  modelDataProxy_->setItemModel(matrix_->getmodel());
  modelDataProxy_->setUseModelCategories(true);
  dataSeries_->setDataProxy(modelDataProxy_);
}

void DataBlockSurface3D::regenerateDataBlockValue() {
  valueDataArray_->reserve(ycolumn_->rowCount() * zcolumns_.count());

  for (int i = 0; i < zcolumns_.count(); i++) {
    QSurfaceDataRow *newRow = new QSurfaceDataRow(ycolumn_->rowCount());
    QSurfaceDataItem *newRowPtr = &newRow->first();
    for (int j = 0; j < ycolumn_->rowCount(); j++) {
      double x = xcolumn_->valueAt(i);
      double y = ycolumn_->valueAt(j);
      double z = zcolumns_.at(i)->valueAt(j);
      newRowPtr->setPosition(QVector3D(y, z, x));
      newRowPtr++;
    }
    *valueDataArray_ << newRow;
  }

  // prepare data
  valueDataProxy_->resetArray(valueDataArray_);
  dataSeries_->setDataProxy(valueDataProxy_);
}

void DataBlockSurface3D::regenerateDataBlockFunction(
    QList<QPair<QPair<double, double>, double> > *data) {
  int points = funcData_.xpoints;
  valueDataArray_->reserve(points);
  for (int i = 0; i < points * points;) {
    // create a new row
    QSurfaceDataRow *newRow = new QSurfaceDataRow(points);
    // get pointer to firsr row
    QSurfaceDataItem *newRowPtr = &newRow->first();
    for (int j = 0; j < points; j++) {
      double x = data->at(i).first.first;
      double y = data->at(i).first.second;
      double z = data->at(i).second;
      newRowPtr->setPosition(QVector3D(y, z, x));
      newRowPtr++;
      i++;
    }
    *valueDataArray_ << newRow;
  }
  // delete data
  data->clear();
  delete data;

  // prepare data
  valueDataProxy_->resetArray(valueDataArray_);
  dataSeries_->setDataProxy(valueDataProxy_);
}

bool DataBlockSurface3D::ismatrix() { return (matrix_ != nullptr); }

bool DataBlockSurface3D::istable() { return (table_ != nullptr); }

DataBlockBar3D::DataBlockBar3D(Matrix *matrix)
    : DataBlockAbstract3D(matrix),
      valueDataArray_(nullptr),
      valueDataProxy_(nullptr),
      dataSeries_(new QBar3DSeries),
      modelDataProxy_(new QItemModelBarDataProxy) {
  regenerateDataBlockModel();
}

DataBlockBar3D::DataBlockBar3D(Table *table, Column *xcolumn, Column *ycolumn,
                               Column *zcolumn)
    : DataBlockAbstract3D(table, xcolumn, ycolumn, zcolumn),
      valueDataArray_(new QBarDataArray),
      valueDataProxy_(new QBarDataProxy),
      dataSeries_(new QBar3DSeries),
      modelDataProxy_(nullptr) {
  regenerateDataBlockXYZValue();
}

DataBlockBar3D::DataBlockBar3D(Table *table, Column *xcolumn, Column *ycolumn,
                               QList<Column *> zcolumns)
    : DataBlockAbstract3D(table, xcolumn, ycolumn, zcolumns),
      valueDataArray_(new QBarDataArray),
      valueDataProxy_(new QBarDataProxy),
      dataSeries_(new QBar3DSeries),
      modelDataProxy_(nullptr) {
  regenerateDataBlockXYnZValue();
}

DataBlockBar3D::~DataBlockBar3D() {}

void DataBlockBar3D::regenerateDataBlockModel() {
  modelDataProxy_->setItemModel(matrix_->getmodel());
  modelDataProxy_->setUseModelCategories(true);
  dataSeries_->setDataProxy(modelDataProxy_);
}

void DataBlockBar3D::regenerateDataBlockXYZValue() {
  valueDataArray_->reserve(ycolumn_->rowCount());
  QStringList xvalues;
  QStringList yvalues;
  for (int j = 0; j < xcolumn_->rowCount(); j++) {
    QBarDataRow *newRow = new QBarDataRow(xcolumn_->rowCount());
    QBarDataItem *newRowPtr = &newRow->first();
    xvalues << xcolumn_->textAt(j);
    yvalues << ycolumn_->textAt(j);
    for (int i = 0; i < xcolumn_->rowCount(); i++) {
      double z = std::numeric_limits<double>::quiet_NaN();
      if (i == j) z = zcolumns_.first()->valueAt(j);
      newRowPtr->setValue(z);
      newRowPtr++;
      qDebug() << xcolumn_->textAt(j) << ycolumn_->textAt(j)
               << zcolumns_.first()->valueAt(j);
    }
    *valueDataArray_ << newRow;
  }

  // prepare data
  valueDataProxy_->resetArray(valueDataArray_, xvalues, yvalues);
  dataSeries_->setDataProxy(valueDataProxy_);
}

void DataBlockBar3D::regenerateDataBlockXYnZValue() {
  valueDataArray_->reserve(ycolumn_->rowCount() * zcolumns_.count());
  QStringList xvalues;
  QStringList yvalues;
  for (int i = 0; i < ycolumn_->rowCount(); i++) {
    yvalues << ycolumn_->textAt(i);
  }
  for (int i = 0; i < zcolumns_.count(); i++) {
    QBarDataRow *newRow = new QBarDataRow(ycolumn_->rowCount());
    xvalues << xcolumn_->textAt(i);
    QBarDataItem *newRowPtr = &newRow->first();
    for (int j = 0; j < ycolumn_->rowCount(); j++) {
      double z = zcolumns_.at(i)->valueAt(j);
      newRowPtr->setValue(z);
      newRowPtr++;
    }
    *valueDataArray_ << newRow;
  }

  // prepare data
  valueDataProxy_->resetArray(valueDataArray_, xvalues, yvalues);
  dataSeries_->setDataProxy(valueDataProxy_);
}

bool DataBlockBar3D::ismatrix() { return (matrix_ != nullptr); }

DataBlockScatter3D::DataBlockScatter3D(Matrix *matrix)
    : DataBlockAbstract3D(matrix),
      valueDataArray_(nullptr),
      valueDataProxy_(nullptr),
      dataSeries_(new QScatter3DSeries),
      modelDataProxy_(new QItemModelScatterDataProxy) {
  regenerateDataBlockModel();
}

DataBlockScatter3D::DataBlockScatter3D(Table *table, Column *xcolumn,
                                       Column *ycolumn, Column *zcolumn)
    : DataBlockAbstract3D(table, xcolumn, ycolumn, zcolumn),
      valueDataArray_(new QScatterDataArray),
      valueDataProxy_(new QScatterDataProxy),
      dataSeries_(new QScatter3DSeries),
      modelDataProxy_(nullptr) {
  regenerateDataBlockXYZValue();
}

DataBlockScatter3D::DataBlockScatter3D(Table *table, Column *xcolumn,
                                       Column *ycolumn,
                                       QList<Column *> zcolumns)
    : DataBlockAbstract3D(table, xcolumn, ycolumn, zcolumns),
      valueDataArray_(new QScatterDataArray),
      valueDataProxy_(new QScatterDataProxy),
      dataSeries_(new QScatter3DSeries),
      modelDataProxy_(nullptr) {
  regenerateDataBlockXYnZValue();
}

DataBlockScatter3D::~DataBlockScatter3D() {}

void DataBlockScatter3D::regenerateDataBlockModel() {}

void DataBlockScatter3D::regenerateDataBlockXYZValue() {}

void DataBlockScatter3D::regenerateDataBlockXYnZValue() {}

bool DataBlockScatter3D::ismatrix() { return (matrix_ != nullptr); }
