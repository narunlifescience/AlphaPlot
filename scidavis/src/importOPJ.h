/***************************************************************************
    File                 : importOPJ.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2010 Miquel Garriga (gbmiquel*gmail.com)
    Copyright            : (C) 2006-2007 by Ion Vasilief (ion_vasilief*yahoo.fr)
    Copyright            : (C) 2006-2007 by Alex Kargovsky (kargovsky*yumr.phys.msu.su)
    Copyright            : (C) 2006-2007 by Tilman Benkert (thzs*gmx.net)
    Description          : Origin project import class

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
#ifndef IMPORTOPJ_H
#define IMPORTOPJ_H

#include "ApplicationWindow.h"
#include <OriginFile.h>

//! Origin project import class
class ImportOPJ
{
public:
	ImportOPJ(ApplicationWindow *app, const QString& filename);

	bool createProjectTree(const OriginFile& opj);
	bool importTables (const OriginFile& opj);
	bool importGraphs (const OriginFile& opj);
	bool importNotes (const OriginFile& opj);
	int error(){return parse_error;};

private:
	int translateOrigin2ScidavisLineStyle(int linestyle);
	QString parseOriginText(const QString &str);
	QString parseOriginTags(const QString &str);
	int parse_error;
	int xoffset;
	ApplicationWindow *mw;
};

#endif //IMPORTOPJ_H
