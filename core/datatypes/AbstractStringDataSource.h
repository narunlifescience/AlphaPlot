/***************************************************************************
    File                 : AbstractStringDataSource.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Type-specific reading interface for a string data source

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

#ifndef STRINGDATASOURCE_H
#define STRINGDATASOURCE_H
 

#include "AbstractDataSource.h"
#include <QLocale>

//! Type-specific reading interface for a string data source
/**
  * This class defines the specific reading interface for
  * a class storing a list of QStrings. It only defines
  * the interface and has no data members itself. 
  * There are no specific functions for strings
  * at the moment, but there may be in the future.
  * \sa AbstractDataSource
  * \sa AbstractColumnData
  */
class AbstractStringDataSource : public AbstractDataSource
{
	Q_OBJECT

public:
	//! Dtor
	virtual ~AbstractStringDataSource(){};

	//! Return the content of row 'row'.
	virtual QString textAt(int row) const = 0;
};

#endif
