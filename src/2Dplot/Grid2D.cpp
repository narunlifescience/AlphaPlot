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

   Description : Plot2D grid related stuff */

#include "Grid2D.h"

#include "Axis2D.h"
#include "AxisRect2D.h"
#include "lib/XmlStreamReader.h"
#include "lib/XmlStreamWriter.h"

Grid2D::Grid2D(Axis2D *parent)
    : QCPGrid(parent),
      axis_(parent),
      layername_(parent->getaxisrect_axis()
                         ->getParentPlot2D()
                         ->getGrid2DLayerName())
{
    // Set Default grid values
    setLayer(layername_);
    layer()->setMode(QCPLayer::LayerMode::lmBuffered);
    majorgridcolor_ = pen().color();
    zerolinecolor_ = zeroLinePen().color();
    majorgridstyle_ = pen().style();
    zerolinestyle_ = zeroLinePen().style();
    majorgridthickness_ = pen().widthF();
    zerolinethickness_ = zeroLinePen().widthF();
}

Grid2D::~Grid2D() { }

void Grid2D::setMajorGridColor(const QColor &color)
{
    majorgridcolor_ = color;
    QPen p = pen();
    p.setColor(color);
    setPen(p);
}

void Grid2D::setMajorGridStyle(const Qt::PenStyle &penstyle)
{
    majorgridstyle_ = penstyle;
    QPen p = pen();
    p.setStyle(penstyle);
    setPen(p);
}

void Grid2D::setMajorGridThickness(const double thickness)
{
    majorgridthickness_ = thickness;
    QPen p = pen();
    p.setWidthF(thickness);
    setPen(p);
}

void Grid2D::setMinorGridColor(const QColor &color)
{
    QPen p = subGridPen();
    p.setColor(color);
    setSubGridPen(p);
}

void Grid2D::setMinorGridStyle(const Qt::PenStyle &penstyle)
{
    QPen p = subGridPen();
    p.setStyle(penstyle);
    setSubGridPen(p);
}

void Grid2D::setMinorGridThickness(const double thickness)
{
    QPen p = subGridPen();
    p.setWidthF(thickness);
    setSubGridPen(p);
}

void Grid2D::setZerothLineColor(const QColor &color)
{
    zerolinecolor_ = color;
    QPen p = zeroLinePen();
    p.setColor(color);
    setZeroLinePen(p);
}

void Grid2D::setZerothLineStyle(const Qt::PenStyle &penstyle)
{
    zerolinestyle_ = penstyle;
    QPen p = zeroLinePen();
    p.setStyle(penstyle);
    setZeroLinePen(p);
}

void Grid2D::setZerothLineThickness(const double thickness)
{
    zerolinethickness_ = thickness;
    QPen p = zeroLinePen();
    p.setWidthF(thickness);
    setZeroLinePen(p);
}

void Grid2D::setMajorGridVisible(const bool status)
{
    (status) ? setPen(
            QPen(majorgridcolor_, majorgridthickness_, majorgridstyle_))
             : setPen(Qt::NoPen);
}

void Grid2D::setMinorGridVisible(const bool status)
{
    setSubGridVisible(status);
}

void Grid2D::setZerothLineVisible(const bool status)
{
    (status) ? setZeroLinePen(
            QPen(zerolinecolor_, zerolinethickness_, zerolinestyle_))
             : setZeroLinePen(Qt::NoPen);
}

QColor Grid2D::getMajorGridColor() const
{
    return majorgridcolor_;
}

Qt::PenStyle Grid2D::getMajorGridStyle() const
{
    return majorgridstyle_;
}

double Grid2D::getMajorGridwidth() const
{
    return majorgridthickness_;
}

QColor Grid2D::getMinorGridColor() const
{
    return subGridPen().color();
}

Qt::PenStyle Grid2D::getMinorGridStyle() const
{
    return subGridPen().style();
}

double Grid2D::getMinorGridwidth() const
{
    return subGridPen().widthF();
}

QColor Grid2D::getZerothLineColor() const
{
    return zerolinecolor_;
}

Qt::PenStyle Grid2D::getZeroLineStyle() const
{
    return zerolinestyle_;
}

double Grid2D::getZeroLinewidth() const
{
    return zerolinethickness_;
}

bool Grid2D::getMajorGridVisible() const
{
    return (pen() != Qt::NoPen);
}

bool Grid2D::getMinorGridVisible() const
{
    return subGridVisible();
}

bool Grid2D::getZerothLineVisible() const
{
    return (zeroLinePen() != Qt::NoPen);
}

void Grid2D::save(XmlStreamWriter *xmlwriter, const QString tag)
{
    xmlwriter->writeStartElement(tag);
    xmlwriter->writeStartElement("major");
    (getMajorGridVisible()) ? xmlwriter->writeAttribute("visible", "true")
                            : xmlwriter->writeAttribute("visible", "false");
    xmlwriter->writePen(pen());
    xmlwriter->writeEndElement();
    xmlwriter->writeStartElement("zero");
    (getZerothLineVisible()) ? xmlwriter->writeAttribute("visible", "true")
                             : xmlwriter->writeAttribute("visible", "false");
    xmlwriter->writePen(zeroLinePen());
    xmlwriter->writeEndElement();
    xmlwriter->writeStartElement("minor");
    (getMinorGridVisible()) ? xmlwriter->writeAttribute("visible", "true")
                            : xmlwriter->writeAttribute("visible", "false");
    xmlwriter->writePen(subGridPen());
    xmlwriter->writeEndElement();
    xmlwriter->writeEndElement();
}

bool Grid2D::load(XmlStreamReader *xmlreader, const QString tag)
{
    if (xmlreader->isStartElement() && xmlreader->name() == tag) {
        bool ok;
        // major
        while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "major")
                break;
            // visible
            if (xmlreader->isStartElement() && xmlreader->name() == "major") {
                bool visible = xmlreader->readAttributeBool("visible", &ok);
                (ok) ? setMajorGridVisible(visible)
                     : xmlreader->raiseWarning(tr("Grid2D majorgrid visible "
                                                  "property setting error"));
                // strokepen property
                while (!xmlreader->atEnd()) {
                    xmlreader->readNext();
                    if (xmlreader->isEndElement() && xmlreader->name() == "pen")
                        break;
                    // pen
                    if (xmlreader->isStartElement()
                        && xmlreader->name() == "pen") {
                        QPen strokep = xmlreader->readPen(&ok);
                        if (ok) {
                            setMajorGridColor(strokep.color());
                            setMajorGridStyle(strokep.style());
                            setMajorGridThickness(strokep.widthF());
                        } else
                            xmlreader->raiseWarning(
                                    tr("Grid2D majorgrid pen property setting "
                                       "error"));
                    }
                }
            }
        }
        // zeroth line
        while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "zero")
                break;
            // visible
            if (xmlreader->isStartElement() && xmlreader->name() == "zero") {
                bool visible = xmlreader->readAttributeBool("visible", &ok);
                (ok) ? setZerothLineVisible(visible)
                     : xmlreader->raiseWarning(tr("Grid2D zerothline visible "
                                                  "property setting error"));
                // strokepen property
                while (!xmlreader->atEnd()) {
                    xmlreader->readNext();
                    if (xmlreader->isEndElement() && xmlreader->name() == "pen")
                        break;
                    // pen
                    if (xmlreader->isStartElement()
                        && xmlreader->name() == "pen") {
                        QPen strokep = xmlreader->readPen(&ok);
                        if (ok) {
                            setZerothLineColor(strokep.color());
                            setZerothLineStyle(strokep.style());
                            setZerothLineThickness(strokep.widthF());
                        } else
                            xmlreader->raiseWarning(
                                    tr("Grid2D zerothline pen property setting "
                                       "error"));
                    }
                }
            }
        }
        // minor
        while (!xmlreader->atEnd()) {
            xmlreader->readNext();
            if (xmlreader->isEndElement() && xmlreader->name() == "minor")
                break;
            // visible
            if (xmlreader->isStartElement() && xmlreader->name() == "minor") {
                bool visible = xmlreader->readAttributeBool("visible", &ok);
                (ok) ? setMinorGridVisible(visible)
                     : xmlreader->raiseWarning(tr("Grid2D minorgrid visible "
                                                  "property setting error"));
                // strokepen property
                while (!xmlreader->atEnd()) {
                    xmlreader->readNext();
                    if (xmlreader->isEndElement() && xmlreader->name() == "pen")
                        break;
                    // pen
                    if (xmlreader->isStartElement()
                        && xmlreader->name() == "pen") {
                        QPen strokep = xmlreader->readPen(&ok);
                        if (ok) {
                            setMinorGridColor(strokep.color());
                            setMinorGridStyle(strokep.style());
                            setMinorGridThickness(strokep.widthF());
                        } else
                            xmlreader->raiseWarning(
                                    tr("Grid2D majorgrid pen property setting "
                                       "error"));
                    }
                }
            }
        }
    } else // no element
        xmlreader->raiseError(tr("no Grid2D item element found"));

    return !xmlreader->hasError();
}
