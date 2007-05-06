/***************************************************************************
    File                 : PlotCurve.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : AbstractPlotCurve and DataCurve classes

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
#ifndef PLOTCURVE_H
#define PLOTCURVE_H

#include <qwt_plot_curve.h>
#include "Table.h"

//! Abstract 2D plot curve class
class PlotCurve: public QwtPlotCurve
{

public:
	PlotCurve(const char *name = 0): QwtPlotCurve(name), d_type(0){};

	int type(){return d_type;};
	void setType(int t){d_type = t;};

protected:
	int d_type;
};

class DataCurve: public PlotCurve
{

public:
	DataCurve(Table *t, const QString& xColName, const char *name, int startRow = 0, int endRow = -1);

	QString xColumnName(){return d_x_column;};
	void setXColumnName(const QString& name){d_x_column = name;};

	Table* table(){return d_table;};

	int startRow(){return d_start_row;};
	int endRow(){return d_end_row;};
	void setRowRange(int startRow, int endRow);

	bool isFullRange();
	void setFullRange();

	virtual bool updateData(Table *t, const QString& colName);
	virtual void loadData();
	
	//! Returns the row index in the data source table corresponding to the data point index.
	int tableRow(int point);

	void remove();

        /**
		 * \brief A list of data sources for this curve.
		 *
		 * Elements must be in either of the following forms:
		 *  - &lt;id of X column> "(X)," &lt;id of Y column> "(Y)" [ "," &lt;id of error column> ("(xErr)" | "(yErr)") ]
		 *  - &lt;id of Xstart column> "(X)," &lt;id of Ystart column>"(Y)," &lt;id of Xend column> "(X)," &lt;id of Yend column> "(Y)"\n
		 *    (denoting start and end coordinates for the #VectXYXY style)
		 *  - &lt;id of Xstart column> "(X)," &lt;id of Ystart column> "(Y)," &lt;id of angle column> "(A)," &lt;id of magnitude column> "(M)"\n
		 *    (denoting start coordinates, angle in radians and length for the #VectXYAM style)
		 *
		 * Column ids are of the form '&lt;name of table> "_" &lt;name of column>'.
		 */
    virtual QString plotAssociation();
	virtual void updateColumnNames(const QString& oldName, const QString& newName, bool updateTableName);

	//! The list of attached error bars.
	QList<DataCurve *> errorBarsList(){return d_error_bars;};
	//! Adds a single error bars curve to the list of attached error bars.
	void addErrorBars(DataCurve *c){if (c) d_error_bars << c;};
	//! Remove a single error bars curve from the list of attached error bars.
	void removeErrorBars(DataCurve *c);
	//! Clears the list of attached error bars.
	void clearErrorBars();

	void setVisible(bool on);

protected:
	//! List of the error bar curves associated to this curve.
	QList <DataCurve *> d_error_bars;
	//! The data source table.
	Table *d_table;
	//!\brief The name of the column used for abscissae values.
	/*
	 *The column name used for Y values is stored in title().text().
	 */
	QString d_x_column;

	int d_start_row;
	int d_end_row;
};
#endif
