#include "StatBox2D.h"

StatBox2D::StatBox2D(Axis2D *xAxis, Axis2D *yAxis)
    : QCPStatisticalBox(xAxis, yAxis) {}

StatBox2D::~StatBox2D() {}
