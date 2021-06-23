/***************************************************************************
        File                 : Spectrogram.cpp
        Project              : AlphaPlot
--------------------------------------------------------------------
        Copyright            : (C) 2006 by Ion Vasilief
        Email (use @ for *)  : ion_vasilief*yahoo.fr
        Description          : AlphaPlot's Spectrogram Class
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "Spectrogram.h"

#include <math.h>
#include <qmessagebox.h>
#include <qpen.h>

Spectrogram::Spectrogram()
    : d_matrix(nullptr), color_axis(), color_map_policy(Default) {}

void Spectrogram::updateData(Matrix *m) {}

void Spectrogram::setLevelsNumber(int levels) {}

bool Spectrogram::hasColorScale() { return false; }

void Spectrogram::showColorScale(int axis, bool on) {}

int Spectrogram::colorBarWidth() { return 0; }

void Spectrogram::setColorBarWidth(int width) {}

Spectrogram *Spectrogram::copy() { return nullptr; }

void Spectrogram::setGrayScale() {}

void Spectrogram::setDefaultColorMap() {}

QString Spectrogram::saveToString() { return QString(); }
