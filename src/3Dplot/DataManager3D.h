#ifndef DATAMANAGER3D_H
#define DATAMANAGER3D_H

#include <QList>
#include <QtDataVisualization/QBarDataArray>
#include <QtDataVisualization/QScatterDataArray>
#include <QtDataVisualization/QSurfaceDataArray>

#include "Graph3DCommon.h"

class Matrix;
class Table;
class Column;

namespace QtDataVisualization {
class QSurfaceDataProxy;
class QSurface3DSeries;
class QItemModelSurfaceDataProxy;
class QBarDataProxy;
class QBar3DSeries;
class QItemModelBarDataProxy;
class QScatterDataProxy;
class QScatter3DSeries;
class QItemModelScatterDataProxy;
class QAbstract3DSeries;
} // namespace QtDataVisualization

class DataBlockAbstract3D
{
public:
    // getters
    Matrix *getmatrix() const { return matrix_; }
    Table *gettable() const { return table_; }
    Column *getxcolumn() const { return xcolumn_; }
    Column *getycolumn() { return ycolumn_; }
    Column *getzcolumn() { return zcolumn_; }
    Graph3DCommon::Gradient getgradient() { return gradient_; }

    // setters
    void setmatrix(Matrix *matrix) { matrix_ = matrix; }
    void settable(Table *table) { table_ = table; }
    void setxcolumn(Column *column) { xcolumn_ = column; }
    void setycolumn(Column *column) { ycolumn_ = column; }
    void setzcolumns(Column *column) { zcolumn_ = column; }
    void setgradient(QtDataVisualization::QAbstract3DSeries *series,
                     const Graph3DCommon::Gradient &gradient);

protected:
    DataBlockAbstract3D();
    DataBlockAbstract3D(Matrix *matrix);
    DataBlockAbstract3D(Table *table, Column *xcolumn, Column *ycolumn,
                        Column *zcolumn);
    ~DataBlockAbstract3D();

    Matrix *matrix_;
    Table *table_;
    Column *xcolumn_;
    Column *ycolumn_;
    Column *zcolumn_;
    Graph3DCommon::Gradient gradient_;
};

class DataBlockSurface3D : public DataBlockAbstract3D
{
public:
    DataBlockSurface3D(Matrix *matrix);
    DataBlockSurface3D(Table *table, Column *xcolumn, Column *ycolumn,
                       Column *zcolumn);
    DataBlockSurface3D(QList<QPair<QPair<double, double>, double>> *data,
                       const Graph3DCommon::Function3DData &funcdata);
    ~DataBlockSurface3D();

    void regenerateDataBlockModel();
    void regenerateDataBlockValue();
    void regenerateDataBlockFunction(
            QList<QPair<QPair<double, double>, double>> *data);

    // getters
    QtDataVisualization::QSurfaceDataArray *getvaluedataarray()
    {
        return valueDataArray_;
    }
    QtDataVisualization::QSurfaceDataProxy *getvaluedataproxy()
    {
        return valueDataProxy_;
    }
    QtDataVisualization::QSurface3DSeries *getdataseries()
    {
        return dataSeries_;
    }
    QtDataVisualization::QItemModelSurfaceDataProxy *getmodeldataproxy()
    {
        return modelDataProxy_;
    }
    bool ismatrix();
    bool istable();

private:
    Graph3DCommon::Function3DData funcData_;
    QtDataVisualization::QSurfaceDataArray *valueDataArray_;
    QtDataVisualization::QSurfaceDataProxy *valueDataProxy_;
    QtDataVisualization::QSurface3DSeries *dataSeries_;
    QtDataVisualization::QItemModelSurfaceDataProxy *modelDataProxy_;
};

class DataBlockBar3D : public DataBlockAbstract3D
{
public:
    DataBlockBar3D(Matrix *matrix);
    DataBlockBar3D(Table *table, Column *xcolumn, Column *ycolumn,
                   Column *zcolumn);
    ~DataBlockBar3D();

    void regenerateDataBlockModel();
    void regenerateDataBlockXYZValue();

    // getters
    QtDataVisualization::QBarDataArray *getvaluedataarray()
    {
        return valueDataArray_;
    }
    QtDataVisualization::QBarDataProxy *getvaluedataproxy()
    {
        return valueDataProxy_;
    }
    QtDataVisualization::QBar3DSeries *getdataseries() { return dataSeries_; }
    QtDataVisualization::QItemModelBarDataProxy *getmodeldataproxy()
    {
        return modelDataProxy_;
    }
    bool ismatrix();

private:
    QtDataVisualization::QBarDataArray *valueDataArray_;
    QtDataVisualization::QBarDataProxy *valueDataProxy_;
    QtDataVisualization::QBar3DSeries *dataSeries_;
    QtDataVisualization::QItemModelBarDataProxy *modelDataProxy_;
};

class DataBlockScatter3D : public DataBlockAbstract3D
{
public:
    DataBlockScatter3D(Matrix *matrix);
    DataBlockScatter3D(Table *table, Column *xcolumn, Column *ycolumn,
                       Column *zcolumn);
    ~DataBlockScatter3D();

    void regenerateDataBlockModel();
    void regenerateDataBlockXYZValue();

    // getters
    QtDataVisualization::QScatterDataArray *getvaluedataarray()
    {
        return valueDataArray_;
    }
    QtDataVisualization::QScatterDataProxy *getvaluedataproxy()
    {
        return valueDataProxy_;
    }
    QtDataVisualization::QScatter3DSeries *getdataseries()
    {
        return dataSeries_;
    }
    bool ismatrix();

private:
    QtDataVisualization::QScatterDataArray *valueDataArray_;
    QtDataVisualization::QScatterDataProxy *valueDataProxy_;
    QtDataVisualization::QScatter3DSeries *dataSeries_;
};

#endif // DATAMANAGER3D_H
