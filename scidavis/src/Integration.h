/***************************************************************************
    File                 : Integration.h
    Project              : SciDAVis
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
	enum InterpolationMethod{Linear, Cubic, Akima};

	Integration(ApplicationWindow *parent, Graph *g);
	Integration(ApplicationWindow *parent, Graph *g, const QString& curveTitle);
	Integration(ApplicationWindow *parent, Graph *g, const QString& curveTitle, double start, double end);

    InterpolationMethod method(){return d_method;};
    void setMethod(InterpolationMethod method) {
		 InterpolationMethod backup = d_method;
		 d_method = method;
		 if (!isDataAcceptable())
			 d_method = backup;
	 };

	 double result() { return d_result; }

protected:
	virtual bool isDataAcceptable();

private:
    void init();
    QString logInfo();
    void output(){};

    //! The method for computing the interpolation used for integrating.
    InterpolationMethod d_method;

	 double d_result;
};

#endif
