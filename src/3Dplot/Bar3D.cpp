#include "Bar3D.h"

#include <qmath.h>

#include "DataManager3D.h"
#include "Matrix.h"
#include "Table.h"
#include "core/column/Column.h"

Bar3D::Bar3D(Q3DBars *bar) : graph_(bar), data_(QVector<DataBlockBar3D *>())
{
    graph_->setColumnAxis(new QCategory3DAxis);
    graph_->setRowAxis(new QCategory3DAxis);
    graph_->setValueAxis(new QValue3DAxis);
}

Bar3D::~Bar3D() { }

void Bar3D::settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                         Column *zcolumn)
{
    DataBlockBar3D *block =
            new DataBlockBar3D(table, xcolumn, ycolumn, zcolumn);
    graph_->addSeries(block->getdataseries());
    data_ << block;
    block->setgradient(block->getdataseries(), Graph3DCommon::Gradient::BBRY);
    block->getdataseries()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    emit dataAdded();
}

void Bar3D::setmatrixdatamodel(Matrix *matrix)
{
    DataBlockBar3D *block = new DataBlockBar3D(matrix);
    graph_->addSeries(block->getdataseries());
    data_ << block;
    block->setgradient(block->getdataseries(), Graph3DCommon::Gradient::BBRY);
    block->getdataseries()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    emit dataAdded();
}

Q3DBars *Bar3D::getGraph() const
{
    return graph_;
}

QVector<DataBlockBar3D *> Bar3D::getData() const
{
    return data_;
}
