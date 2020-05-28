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
#include "core/column/Column.h"

using namespace QtDataVisualization;

void DataBlockAbstract3D::setgradient(QAbstract3DSeries *series,
                                      const Graph3DCommon::Gradient &gradient)
{
    gradient_ = gradient;
    QLinearGradient gr;
    switch (gradient) {
    case Graph3DCommon::Gradient::Grayscale:
        gr.setColorAt(0, Qt::black);
        gr.setColorAt(1, Qt::white);
        break;
    case Graph3DCommon::Gradient::Hot:
        gr.setColorAt(0, QColor(50, 0, 0));
        gr.setColorAt(0.2, QColor(180, 10, 0));
        gr.setColorAt(0.4, QColor(245, 50, 0));
        gr.setColorAt(0.6, QColor(255, 150, 10));
        gr.setColorAt(0.8, QColor(255, 255, 50));
        gr.setColorAt(1, QColor(255, 255, 255));
        break;
    case Graph3DCommon::Gradient::Cold:
        gr.setColorAt(0, QColor(0, 0, 50));
        gr.setColorAt(0.2, QColor(0, 10, 180));
        gr.setColorAt(0.4, QColor(0, 50, 245));
        gr.setColorAt(0.6, QColor(10, 150, 255));
        gr.setColorAt(0.8, QColor(50, 255, 255));
        gr.setColorAt(1, QColor(255, 255, 255));
        break;
    case Graph3DCommon::Gradient::Night:
        gr.setColorAt(0, QColor(10, 20, 30));
        gr.setColorAt(1, QColor(250, 255, 250));
        break;
    case Graph3DCommon::Gradient::Candy:
        gr.setColorAt(0, QColor(0, 0, 255));
        gr.setColorAt(1, QColor(255, 250, 250));
        break;
    case Graph3DCommon::Gradient::Geography:
        gr.setColorAt(0, QColor(70, 170, 210));
        gr.setColorAt(0.20, QColor(90, 160, 180));
        gr.setColorAt(0.25, QColor(45, 130, 175));
        gr.setColorAt(0.30, QColor(100, 140, 125));
        gr.setColorAt(0.5, QColor(100, 140, 100));
        gr.setColorAt(0.6, QColor(130, 145, 120));
        gr.setColorAt(0.7, QColor(140, 130, 120));
        gr.setColorAt(0.9, QColor(180, 190, 190));
        gr.setColorAt(1, QColor(210, 210, 230));
        break;
    case Graph3DCommon::Gradient::Ion:
        gr.setColorAt(0, QColor(50, 10, 10));
        gr.setColorAt(0.45, QColor(0, 0, 255));
        gr.setColorAt(0.8, QColor(0, 255, 255));
        gr.setColorAt(1, QColor(0, 255, 0));
        break;
    case Graph3DCommon::Gradient::Thermal:
        gr.setColorAt(0, QColor(0, 0, 50));
        gr.setColorAt(0.15, QColor(20, 0, 120));
        gr.setColorAt(0.33, QColor(200, 30, 140));
        gr.setColorAt(0.6, QColor(255, 100, 0));
        gr.setColorAt(0.85, QColor(255, 255, 40));
        gr.setColorAt(1, QColor(255, 255, 255));
        break;
    case Graph3DCommon::Gradient::Polar:
        gr.setColorAt(0, QColor(50, 255, 255));
        gr.setColorAt(0.18, QColor(10, 70, 255));
        gr.setColorAt(0.28, QColor(10, 10, 190));
        gr.setColorAt(0.5, QColor(0, 0, 0));
        gr.setColorAt(0.72, QColor(190, 10, 10));
        gr.setColorAt(0.82, QColor(255, 70, 10));
        gr.setColorAt(1, QColor(255, 255, 50));
        break;
    case Graph3DCommon::Gradient::Spectrum:
        gr.setColorAt(0, QColor(50, 0, 50));
        gr.setColorAt(0.15, QColor(0, 0, 255));
        gr.setColorAt(0.35, QColor(0, 255, 255));
        gr.setColorAt(0.6, QColor(255, 255, 0));
        gr.setColorAt(0.75, QColor(255, 30, 0));
        gr.setColorAt(1, QColor(50, 0, 0));
        break;
    case Graph3DCommon::Gradient::Jet:
        gr.setColorAt(0, QColor(0, 0, 100));
        gr.setColorAt(0.15, QColor(0, 50, 255));
        gr.setColorAt(0.35, QColor(0, 255, 255));
        gr.setColorAt(0.65, QColor(255, 255, 0));
        gr.setColorAt(0.85, QColor(255, 30, 0));
        gr.setColorAt(1, QColor(100, 0, 0));
        break;
    case Graph3DCommon::Gradient::Hues:
        gr.setColorAt(0, QColor(255, 0, 0));
        gr.setColorAt(1.0 / 3.0, QColor(0, 0, 255));
        gr.setColorAt(2.0 / 3.0, QColor(0, 255, 0));
        gr.setColorAt(1, QColor(255, 0, 0));
        break;
    case Graph3DCommon::Gradient::BBRY:
        gr.setColorAt(0.0, Qt::black);
        gr.setColorAt(0.33, Qt::blue);
        gr.setColorAt(0.67, Qt::red);
        gr.setColorAt(1.0, Qt::yellow);
        break;
    case Graph3DCommon::Gradient::GYRD:
        gr.setColorAt(0.0, Qt::darkGreen);
        gr.setColorAt(0.5, Qt::yellow);
        gr.setColorAt(0.8, Qt::red);
        gr.setColorAt(1.0, Qt::darkRed);
        break;
    }
    series->setBaseGradient(gr);
}

DataBlockAbstract3D::DataBlockAbstract3D()
    : matrix_(nullptr),
      table_(nullptr),
      xcolumn_(nullptr),
      ycolumn_(nullptr),
      zcolumn_(nullptr)
{
}

DataBlockAbstract3D::DataBlockAbstract3D(Matrix *matrix)
    : matrix_(matrix),
      table_(nullptr),
      xcolumn_(nullptr),
      ycolumn_(nullptr),
      zcolumn_(nullptr)
{
}

DataBlockAbstract3D::DataBlockAbstract3D(Table *table, Column *xcolumn,
                                         Column *ycolumn, Column *zcolumn)
    : matrix_(nullptr),
      table_(table),
      xcolumn_(xcolumn),
      ycolumn_(ycolumn),
      zcolumn_(zcolumn)
{
}

DataBlockAbstract3D::~DataBlockAbstract3D() { }

DataBlockSurface3D::DataBlockSurface3D(Matrix *matrix)
    : DataBlockAbstract3D(matrix),
      valueDataArray_(nullptr),
      valueDataProxy_(nullptr),
      dataSeries_(new QSurface3DSeries),
      modelDataProxy_(new QItemModelSurfaceDataProxy)
{
    regenerateDataBlockModel();
}

DataBlockSurface3D::DataBlockSurface3D(Table *table, Column *xcolumn,
                                       Column *ycolumn, Column *zcolumn)
    : DataBlockAbstract3D(table, xcolumn, ycolumn, zcolumn),
      valueDataArray_(new QSurfaceDataArray),
      valueDataProxy_(new QSurfaceDataProxy),
      dataSeries_(new QSurface3DSeries),
      modelDataProxy_(nullptr)
{
    regenerateDataBlockValue();
}

DataBlockSurface3D::DataBlockSurface3D(
        QList<QPair<QPair<double, double>, double>> *data,
        const Graph3DCommon::Function3DData &funcdata)
    : DataBlockAbstract3D(),
      funcData_((funcdata)),
      valueDataArray_(new QSurfaceDataArray),
      valueDataProxy_(new QSurfaceDataProxy),
      dataSeries_(new QSurface3DSeries),
      modelDataProxy_(nullptr)
{
    regenerateDataBlockFunction(data);
}

DataBlockSurface3D::~DataBlockSurface3D() { }

void DataBlockSurface3D::regenerateDataBlockModel()
{
    modelDataProxy_->setItemModel(matrix_->getmodel());
    modelDataProxy_->setUseModelCategories(true);
    dataSeries_->setDataProxy(modelDataProxy_);
}

void DataBlockSurface3D::regenerateDataBlockValue()
{
    valueDataArray_->reserve(xcolumn_->rowCount());

    for (int i = 0; i < xcolumn_->rowCount(); i++) {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(ycolumn_->rowCount());
        QSurfaceDataItem *newRowPtr = &newRow->first();
        double x = xcolumn_->valueAt(i);
        double y = ycolumn_->valueAt(i);
        double z = zcolumn_->valueAt(i);
        newRowPtr->setPosition(QVector3D(y, z, x));
        *valueDataArray_ << newRow;
    }

    // prepare data
    valueDataProxy_->resetArray(valueDataArray_);
    dataSeries_->setDataProxy(valueDataProxy_);
}

void DataBlockSurface3D::regenerateDataBlockFunction(
        QList<QPair<QPair<double, double>, double>> *data)
{
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

bool DataBlockSurface3D::ismatrix()
{
    return (matrix_ != nullptr);
}

bool DataBlockSurface3D::istable()
{
    return (table_ != nullptr);
}

DataBlockBar3D::DataBlockBar3D(Matrix *matrix)
    : DataBlockAbstract3D(matrix),
      valueDataArray_(nullptr),
      valueDataProxy_(nullptr),
      dataSeries_(new QBar3DSeries),
      modelDataProxy_(new QItemModelBarDataProxy)
{
    regenerateDataBlockModel();
}

DataBlockBar3D::DataBlockBar3D(Table *table, Column *xcolumn, Column *ycolumn,
                               Column *zcolumn)
    : DataBlockAbstract3D(table, xcolumn, ycolumn, zcolumn),
      valueDataArray_(new QBarDataArray),
      valueDataProxy_(new QBarDataProxy),
      dataSeries_(new QBar3DSeries),
      modelDataProxy_(nullptr)
{
    regenerateDataBlockXYZValue();
}

DataBlockBar3D::~DataBlockBar3D() { }

void DataBlockBar3D::regenerateDataBlockModel()
{
    modelDataProxy_->setItemModel(matrix_->getmodel());
    modelDataProxy_->setUseModelCategories(true);
    dataSeries_->setDataProxy(modelDataProxy_);
}

void DataBlockBar3D::regenerateDataBlockXYZValue()
{
    valueDataArray_->reserve(xcolumn_->rowCount());
    QStringList xvalues;
    QStringList yvalues;
    for (int j = 0; j < xcolumn_->rowCount(); j++) {
        QBarDataRow *newRow = new QBarDataRow(xcolumn_->rowCount());
        QBarDataItem *newRowPtr = &newRow->first();
        xvalues << xcolumn_->textAt(j);
        yvalues << ycolumn_->textAt(j);
        for (int i = 0; i < xcolumn_->rowCount(); i++) {
            double z = std::numeric_limits<double>::quiet_NaN();
            if (i == j)
                z = zcolumn_->valueAt(j);
            newRowPtr->setValue(z);
            newRowPtr++;
        }
        *valueDataArray_ << newRow;
    }

    // prepare data
    valueDataProxy_->resetArray(valueDataArray_, xvalues, yvalues);
    dataSeries_->setDataProxy(valueDataProxy_);
}

bool DataBlockBar3D::ismatrix()
{
    return (matrix_ != nullptr);
}

DataBlockScatter3D::DataBlockScatter3D(Matrix *matrix)
    : DataBlockAbstract3D(matrix),
      valueDataArray_(new QScatterDataArray),
      valueDataProxy_(new QScatterDataProxy),
      dataSeries_(new QScatter3DSeries)
{
    regenerateDataBlockModel();
}

DataBlockScatter3D::DataBlockScatter3D(Table *table, Column *xcolumn,
                                       Column *ycolumn, Column *zcolumn)
    : DataBlockAbstract3D(table, xcolumn, ycolumn, zcolumn),
      valueDataArray_(new QScatterDataArray),
      valueDataProxy_(new QScatterDataProxy),
      dataSeries_(new QScatter3DSeries)
{
    regenerateDataBlockXYZValue();
}

DataBlockScatter3D::~DataBlockScatter3D() { }

void DataBlockScatter3D::regenerateDataBlockModel()
{
    valueDataArray_->resize((matrix_->numRows()) * (matrix_->numCols()));
    QScatterDataItem *ptrToDataArray = &valueDataArray_->first();

    for (int i = 0; i < matrix_->numRows(); i++) {
        for (int j = 0; j < matrix_->numCols(); j++) {
            double x = i;
            double y = j;
            double z = matrix_->cell(i, j);
            ptrToDataArray->setPosition(QVector3D(y, z, x));
            ptrToDataArray++;
        }
    }

    valueDataProxy_->resetArray(valueDataArray_);
    dataSeries_->setDataProxy(valueDataProxy_);
}

void DataBlockScatter3D::regenerateDataBlockXYZValue()
{
    valueDataArray_->resize(xcolumn_->rowCount());
    QScatterDataItem *ptrToDataArray = &valueDataArray_->first();

    for (int i = 0; i < xcolumn_->rowCount(); i++) {
        // double z = std::numeric_limits<double>::quiet_NaN();
        double x = xcolumn_->valueAt(i);
        double y = ycolumn_->valueAt(i);
        double z = zcolumn_->valueAt(i);
        ptrToDataArray->setPosition(QVector3D(y, z, x));
        ptrToDataArray++;
    }

    valueDataProxy_->resetArray(valueDataArray_);
    dataSeries_->setDataProxy(valueDataProxy_);
}

bool DataBlockScatter3D::ismatrix()
{
    return (matrix_ != nullptr);
}
