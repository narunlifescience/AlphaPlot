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

Plot2D::Plot2D(QWidget *parent)
    : QCustomPlot(parent),
      canvasBackground_(Qt::white),
      layernamebackground2d_("background"),
      layernamegrid2d_("grid"),
      layernameaxis2d_("axes"),
      layernamepicker2d_("picker"),
      layernamelegend2d_("legend") {
  setOpenGl(false);
  setBackgroundColor(canvasBackground_, Qt::SolidPattern);
  setAutoAddPlottableToLegend(false);
  plotLayout()->clear();
  setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  // set layer order
  if (!layer(layernamebackground2d_))
    addLayer(layernamebackground2d_, nullptr, LayerInsertMode::limBelow);
  if (!layer(layernamegrid2d_))
    addLayer(layernamegrid2d_, layer(layernamebackground2d_),
             LayerInsertMode::limAbove);
  if (!layer(layernameaxis2d_))
    addLayer(layernameaxis2d_, layer(layernamegrid2d_),
             LayerInsertMode::limAbove);
  if (!layer(layernamepicker2d_))
    addLayer(layernamepicker2d_, layer(layernameaxis2d_),
             LayerInsertMode::limAbove);
  if (!layer(layernamelegend2d_))
    addLayer(layernamelegend2d_, layer(layernameaxis2d_),
             LayerInsertMode::limAbove);
  // overlay layer not removed here
  if (!removeLayer(layer("main"))) qDebug() << "unable to delete main layer";
}

Plot2D::~Plot2D() {}

void Plot2D::setBackgroundColor(const QColor &color, const bool backpixmap) {
  canvasBackground_ = color;
  if (backpixmap) {
    QPixmap pixmap(":pixmap/transparent-background.png");
    QPainter paint(&pixmap);
    QBrush pixmapbrush(canvasBackground_);
    paint.setBrush(pixmapbrush);
    paint.setPen(Qt::NoPen);
    paint.drawRect(0, 0, pixmap.height(), pixmap.width());
    QBrush b(pixmap);
    setBackground(b);
  } else {
    QBrush b(canvasBackground_);
    setBackground(b);
  }
  backgroundColorChange(canvasBackground_);
}

QColor Plot2D::getBackgroundColor() const { return canvasBackground_; }

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
    mBackgroundBrush.setColor(getBackgroundColor());
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
  Q_UNUSED(fileName)
  Q_UNUSED(width)
  Q_UNUSED(exportPen)
  Q_UNUSED(height)
  Q_UNUSED(psCreator)
  Q_UNUSED(psTitle)
  qDebug() << "Post Script format(s) not supported in QT5 anymore";
  return false;
}
