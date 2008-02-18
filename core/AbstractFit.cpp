/***************************************************************************
    File                 : AbstractFit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2008 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Base class for doing fits using the algorithms
                           provided by GSL.

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

#include "AbstractFit.h"
#include <QUndoCommand>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multimin.h>

class FitSetAlgorithmCmd : public QUndoCommand
{
	public:
		FitSetAlgorithCmd(AbstractFit *target, AbstractFit::Algorithm algo)
			: d_target(target), d_other_algo(algo) {
				setText(QObject::tr("%1: change fit algorithm to %2.").arg(d_target->name()).arg(AbstractFit::nameOf(d_algo)));
			}

		virtual void undo() {
			AbstractFit::Algorithm tmp = d_target->d_algorithm;
			d_target->d_algorithm = d_other_algo;
			d_other_algo = tmp;
		}

		virtual void redo() { undo(); }

	private:
		AbstractFit *d_target;
		AbstractFit::Algorithm d_other_algo;
};

class FitSetAutoRefitCmd : public QUndoCommand
{
	public:
		FitSetAutoRefitCmd(AbstractFit *target, bool refit)
			: d_target(target), d_backup(refit) {
				setText((refit ?
							QObject::tr("%1: switch auto-refit on.", "label of AbstractFit's undo action") :
							QObject::tr("%1: switch auto-refit off.", "label of AbstractFit's undo action")).
						arg(d_target->name()));
			}

		virtual void undo() {
			bool tmp = d_target->d_auto_refit;
			d_target->d_auto_refit = d_backup;
			d_backup = tmp;
		}

		virtual void redo() { undo(); }

	private:
		AbstractFit *d_target;
		bool d_backup;
};
