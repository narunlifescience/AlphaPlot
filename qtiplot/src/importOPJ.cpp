#include "importOPJ.h"
#include "../3rdparty/liborigin/OPJFile.h"

#include <qregexp.h>
#include <qmessagebox.h>
#include <qworkspace.h>
#include <qapplication.h>

ImportOPJ::ImportOPJ(ApplicationWindow *app, const QString& filename) 
	: mw(app),filename(filename) 
{
OPJFile opj((char *)filename.latin1());
parse_error = opj.Parse();
if (!parse_error)
	importTables(opj);
else
	{
	QApplication::restoreOverrideCursor();
	QMessageBox::critical(app, "QtiPlot - Error", "Could not open file: <br><br><b> filename </b><br><br>"
				"<b>liborigin</b> has encountered an error, please inform its author <b>Stefan Gerlach</b> about this!");
	}
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
			if(fabs(val) > 2e-300) 
				table->setText(i, j, QString::number(val));
			}		
		}

	table->showNormal();
	}
mw->ws->cascade ();
return true;
}

