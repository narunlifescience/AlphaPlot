/***************************************************************************
    File                 : AbstractDateTimeDataSource.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Type-specific reading interface for a date+time data source

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

#ifndef DATETIMEDATASOURCE_H
#define DATETIMEDATASOURCE_H

#include "../AbstractDataSource.h"

class QDate;
class QTime;
class QDateTime;
class QString;

//! Type-specific reading interface for a date+time data source
/**
  * This class defines the specific reading interface for
  * a class storing a list of QDateTimes. It only defines
  * the interface and has no data members itself.
  * \sa AbstractDataSource
  * \sa AbstractColumnData
  */
class AbstractDateTimeDataSource : public AbstractDataSource
{
	Q_OBJECT

public:
	//! Dtor
	virtual ~AbstractDateTimeDataSource(){};

	//! Return the date part of row 'row'
	virtual QDate dateAt(int row) const = 0;
	//! Return the time part of row 'row'
	virtual QTime timeAt(int row) const = 0;
	//! Return the QDateTime in row 'row'
	virtual QDateTime dateTimeAt(int row) const = 0;
};

#endif
