/***************************************************************************
    File                 : AbstractDoubleDataSource.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Type-specific reading interface for a double data source

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

#ifndef DOUBLEDATASOURCE_H
#define DOUBLEDATASOURCE_H

#include "../AbstractDataSource.h"

//! Type-specific reading interface for a double data source
/**
  * This class defines the specific reading interface for
  * a class storing a vector of double precision values. 
  * It only defines the interface and has no data members itself.
  * \sa AbstractDataSource
  * \sa AbstractColumnData
  */
class AbstractDoubleDataSource : public AbstractDataSource
{
	Q_OBJECT

public:
	//! Ctor
	AbstractDoubleDataSource() {
		d_data_cache = 0;
		connect(this, SIGNAL(dataChanged(AbstractDataSource*)),
				this, SLOT(resetCache()));
	}	
	//! Dtor
	virtual ~AbstractDoubleDataSource() {
		if(d_data_cache) delete[] d_data_cache;
	}

	//! Return a read-only array pointer for fast data access
	/**
	 * The pointer remains valid as long as the data does not change.
	 * When it does, AbstractDataSource::dataChanged() is emitted.
	 */ 
	virtual const double * constDataPointer() const {
		if(!d_data_cache) {
			 d_data_cache = new double[rowCount()];
			 for (int i=0; i<rowCount(); i++)
				 d_data_cache[i] = valueAt(i);
		 }
		 return d_data_cache;
	}
	//! Return the double value in row 'row'
	virtual double valueAt(int row) const = 0;

private:
	mutable double *d_data_cache;
	private slots:
		void resetCache() {
			if(d_data_cache) delete[] d_data_cache;
			d_data_cache = 0;
		}
};

#endif
