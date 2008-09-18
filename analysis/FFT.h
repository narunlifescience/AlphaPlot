/***************************************************************************
    File                 : FFT.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical FFT of data sets

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
#ifndef FFT_H
#define FFT_H

#include "core/Filter.h"

class FFT : public Filter
{
Q_OBJECT

public:
    FFT(ApplicationWindow *parent, Table *t, const QString& realColName, const QString& imagColName = QString());
	FFT(ApplicationWindow *parent, Layer *layer, const QString& curveTitle);

    void setInverseFFT(bool inverse = true){m_inverse = inverse;};
    void setSampling(double sampling){m_sampling = sampling;};
    void normalizeAmplitudes(bool norm = true){m_normalize = norm;};
    void shiftFrequencies(bool shift = true){m_shift_order = shift;};

private:
    void init();
    void output();
    void output(const QString &text);

    QString fftCurve();
    QString fftTable();

    void setDataFromTable(Table *t, const QString& realColName, const QString& imagColName = QString());

    double m_sampling;
    //! Flag telling if an inverse FFT must be performed.
    bool m_inverse;
    //! Flag telling if the amplitudes in the output spectrum must be normalized.
    bool m_normalize;
    //! Flag telling if the output frequencies must be shifted in order to have a zero-centered spectrum.
    bool m_shift_order;

    int m_real_col, m_imag_col;
};

#endif
