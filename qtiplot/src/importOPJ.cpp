#include "importOPJ.h"
#include "../3rdparty/liborigin/OPJFile.h"

#include <qregexp.h>
#include <qworkspace.h>

ImportOPJ::ImportOPJ(ApplicationWindow *parent, const QString& filename) 
	: mw(parent)
{
OPJFile opj((char *)filename.latin1());
parse_error = opj.Parse();

importTables(opj);
}

bool ImportOPJ::importTables(OPJFile opj) 
{
if (!mw)
	return false;

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
mw->ws->cascade ();
return true;
}

