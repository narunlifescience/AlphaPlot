/***************************************************************************
    File                 : Integration.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical integration of data sets

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
#ifndef INTEGRATION_H
#define INTEGRATION_H

#include "Filter.h"

class Integration : public Filter
{
Q_OBJECT

public:
	Integration(ApplicationWindow *parent, Graph *g);
	Integration(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
	Integration(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end);

    int method(){return d_method;};
    void setMethodOrder(int n);

private:
    void init();
    //!Uses code originally written by Vasileios Gkanis. It needs some more checking.
    QString logInfo();
    void output(){};

    //! the integration method: 1 = trapezoidal, max = 5!
    int d_method;
};

#endif
