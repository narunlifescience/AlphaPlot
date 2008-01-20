/***************************************************************************
    File                 : Folder.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : thzs*gmx.net, knut.franke*gmx.de
    Description          : Folder in a project

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
#ifndef FOLDER_H
#define FOLDER_H

#include "AbstractAspect.h"
#include <QMdiArea>

//! Folder in a project
class Folder : public QObject, public AbstractAspect
{
	Q_OBJECT

	public:
		Folder(const QString &name);

		//! Currently, Folder does not have a default view (returns 0).
		virtual QWidget *view(QWidget *parent_widget = 0) {
			Q_UNUSED(parent_widget);
			return 0;
		}

		virtual ~Folder();

		//! See QMetaObject::className().
		virtual const char* className() const { return metaObject()->className(); }
		//! See QObject::inherits().
		virtual bool inherits(const char *class_name) const { return QObject::inherits(class_name); }

		virtual QIcon icon() const;

		QMdiArea * mdiArea() const { return d_mdi_area; }
	
	private:
		QMdiArea * d_mdi_area;
		
};

#endif // ifndef FOLDER_H
