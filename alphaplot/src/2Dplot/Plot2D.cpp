/* This file is part of AlphaPlot.
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : Plot2D top level stuff */

#include "Plot2D.h"
#include <QSvgGenerator>

Plot2D::Plot2D(QWidget *parent) : QCustomPlot(parent), canvasBrush_(Qt::white) {
  setBackgroundColor(canvasBrush_);
  setAutoAddPlottableToLegend(false);
  setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                  QCP::iSelectLegend | QCP::iSelectPlottables);
  setInteraction(QCP::iRangeDrag, true);
  plotLayout()->clear();
}

Plot2D::~Plot2D() {}

void Plot2D::setBackgroundColor(const QBrush &brush) {
  setBackground(brush);
  canvasBrush_ = brush;
}

QBrush Plot2D::getBackgroundColor() const { return canvasBrush_; }

bool Plot2D::saveSvg(const QString &fileName, int width, int height,
                     QCP::ExportPen exportPen, const QString &svgTitle,
                     const QString &svgDescription) {
  bool success = false;
  int newWidth;
  int newHeight;
  if (width == 0 || height == 0) {
    newWidth = this->width();
    newHeight = this->height();
  } else {
    newWidth = width;
    newHeight = height;
  }

  QSvgGenerator generator;
  generator.setFileName(fileName);
  generator.setSize(QSize(newWidth, newHeight));
  generator.setViewBox(QRect(0, 0, newWidth, newHeight));
  generator.setTitle(svgTitle.toUtf8());
  generator.setDescription(svgDescription.toUtf8());

  QCPPainter painter;
  if (painter.begin(&generator)) {
    painter.setMode(QCPPainter::pmVectorized);
    painter.setMode(QCPPainter::pmNoCaching);
    painter.setMode(QCPPainter::pmNonCosmetic, exportPen == QCP::epNoCosmetic);
    painter.setWindow(mViewport);
    if (mBackgroundBrush.style() != Qt::NoBrush &&
        mBackgroundBrush.color() != Qt::white &&
        mBackgroundBrush.color() != Qt::transparent &&
        mBackgroundBrush.color().alpha() > 0)
      painter.fillRect(viewport(), mBackgroundBrush);

    toPainter(&painter, newWidth, newHeight);
    painter.end();
    success = true;
  }
  return success;
}

// PS not supported from QT 5 onwards
bool Plot2D::savePs(const QString &fileName, int width, int height,
                    QCP::ExportPen exportPen, const QString &psCreator,
                    const QString &psTitle) {
  return false;
}
