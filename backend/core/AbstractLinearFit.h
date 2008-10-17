/***************************************************************************
    File                 : AbstractLinearFit.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Knut Franke
    Email (use @ for *)  : Knut.Franke*gmx.net
    Description          : Base class for linear fitting (using GSL).

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

#ifndef ABSTRACT_LINEAR_FIT_H
#define ABSTRACT_LINEAR_FIT_H

#include "AbstractFit.h"

//! Base class for linear fitting (using GSL).
/**
 * Here, linearity refers to the fit parameter dependence of the model function;
 * not to its dependence on the predictor, x.
 */
class AbstractLinearFit : public AbstractFit
{
	Q_OBJECT
	
	public:
		AbstractLinearFit() {}

	protected:
		//! Compute derivative of model function with respect to parameters.
		/**
		 * For given predictor x, this is a constant vector because of linearity assumption.
		 */
		virtual void df(double x, gsl_vector * out) = 0;

		virtual void dataChanged(AbstractColumn*);
};

#endif // ifndef ABSTRACT_LINEAR_FIT_H
