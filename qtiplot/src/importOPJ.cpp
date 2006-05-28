/***************************************************************************
    File                 : importOPJ.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#include "importOPJ.h"
#include "../3rdparty/liborigin/OPJFile.h"

#include <qregexp.h>

ImportOPJ::ImportOPJ(ApplicationWindow *app, const QString& filename) :
	mw(app)
{	
OPJFile opj((char *)filename.latin1());
parse_error = opj.Parse();

importTables(opj);
}

bool ImportOPJ::importTables(OPJFile opj) 
{
for (int s=0; s<opj.numSpreads(); s++) 
	{	
	int nr_cols = opj.numCols(s);
	int maxrows = opj.maxRows(s);

	Table *table = mw->newTable(opj.spreadName(s), maxrows, nr_cols);
	if (!table)
		return false;
	
	for (int j=0; j<nr_cols; j++) 
		{
		QString name(opj.colName(s,j));
		table->setColName(j, name.replace(QRegExp(".*_"),""));

		if (QString(opj.colType(s,j)) == "X")
			table->setColPlotDesignation(j, Table::X);
		else if (QString(opj.colType(s,j)) == "Y")
			table->setColPlotDesignation(j, Table::Y);
		else if (QString(opj.colType(s,j)) == "Z")
			table->setColPlotDesignation(j, Table::Z);
		else
			table->setColPlotDesignation(j, Table::None);

		for (int i=0; i<opj.numRows(s,j); i++) 
			{
			double val = opj.Data(s,j)[i];
			if(fabs(val) > 2e-300 || val == 0) 
				table->setText(i, j, QString::number(val));
			}		
		}
	table->showNormal();
	}
return true;
}
