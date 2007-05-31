/***************************************************************************
    File                 : importOPJ.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2007 by Ion Vasilief, Alex Kargovsky, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, kargovsky*yumr.phys.msu.su, thzs*gmx.net
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
#include <OPJFile.h>

#include <QRegExp>
#include <QMessageBox>
#include <QDockWidget>
#include <QLocale>
#include <QDate>
#include "Matrix.h"
#include "ColorBox.h"
#include "MultiLayer.h"
#include "Note.h"
#include "QwtHistogram.h"

#define OBJECTXOFFSET 200

QString strreverse(const QString &str) //QString reversing
{
	QString out="";
	for(int i=str.length()-1; i>=0; --i)
	{
		out+=str[i];
	}
	return out;
}

ImportOPJ::ImportOPJ(ApplicationWindow *app, const QString& filename) :
		mw(app)
{
	xoffset=0;
	OPJFile opj((const char *)filename.latin1());
	parse_error = opj.Parse();
	importTables(opj);
	importGraphs(opj);
	importNotes(opj);
	mw->showResults(opj.resultsLogString(),mw->logWindow->isVisible());
}

int ImportOPJ::translateOrigin2QtiplotLineStyle(int linestyle) {
	int qtiplotstyle=0;
	switch (linestyle)
	{
		case OPJFile::Solid:
			qtiplotstyle=0;
			break;
		case OPJFile::Dash:
		case OPJFile::ShortDash:
			qtiplotstyle=1;
			break;
		case OPJFile::Dot:
		case OPJFile::ShortDot:
			qtiplotstyle=2;
			break;
		case OPJFile::DashDot:
		case OPJFile::ShortDashDot:
			qtiplotstyle=3;
			break;
		case OPJFile::DashDotDot:
			qtiplotstyle=4;
			break;
	}
	return qtiplotstyle;
}

bool ImportOPJ::importTables(OPJFile opj)
{
	int visible_count=0;
	int QtiPlot_scaling_factor=10; //in Origin width is measured in characters while in QtiPlot - pixels --- need to be accurate
	for (int s=0; s<opj.numSpreads(); s++)
	{
		int nr_cols = opj.numCols(s);
		int maxrows = opj.maxRows(s);

		Table *table = (opj.spreadHidden(s)||opj.spreadLoose(s))&&opj.Version()==7.5 ? mw->newHiddenTable(opj.spreadName(s), opj.spreadLabel(s), maxrows, nr_cols)
										: mw->newTable(opj.spreadName(s), maxrows, nr_cols);
		if (!table)
			return false;

		table->setWindowLabel(opj.spreadLabel(s));
		for (int j=0; j<nr_cols; j++)
		{
			QString name(opj.colName(s,j));
			table->setColName(j, name.replace(QRegExp(".*_"),""));
			table->setCommand(j, QString(opj.colCommand(s,j)));
			table->setColComment(j, QString(opj.colComment(s,j)));
			table->changeColWidth(opj.colWidth(s,j)*QtiPlot_scaling_factor, j);

			if (QString(opj.colType(s,j)) == "X")
				table->setColPlotDesignation(j, Table::X);
			else if (QString(opj.colType(s,j)) == "Y")
				table->setColPlotDesignation(j, Table::Y);
			else if (QString(opj.colType(s,j)) == "Z")
				table->setColPlotDesignation(j, Table::Z);
			else if (QString(opj.colType(s,j)) == "DX")
				table->setColPlotDesignation(j, Table::xErr);
			else if (QString(opj.colType(s,j)) == "DY")
				table->setColPlotDesignation(j, Table::yErr);
			else
				table->setColPlotDesignation(j, Table::None);

            table->setHeaderColType();//update header

			QString format;
			switch(opj.colValueType(s,j))
			{
			case 0: //Numeric
			case 6: //Text&Numeric
				int f;
				if(opj.colNumDisplayType(s,j)==0)
					f=0;
				else
					switch(opj.colValueTypeSpec(s,j))
					{
					case 0: //Decimal 1000
						f=1;
						break;
					case 1: //Scientific
						f=2;
						break;
					case 2: //Engeneering
					case 3: //Decimal 1,000
						f=0;
						break;
					}
				table->setColNumericFormat(f, opj.colDecPlaces(s,j), j, false);
				break;
			case 1: //Text
				table->setTextFormat(j);
				break;
			case 2: // Date
				switch(opj.colValueTypeSpec(s,j))
				{
				case 0:
				case 9:
				case 10:
					format="dd.MM.yyyy";
					break;
				case 2:
					format="MMM d";
					break;
				case 3:
					format="M/d";
					break;
				case 4:
					format="d";
					break;
				case 5:
				case 6:
					format="ddd";
					break;
				case 7:
					format="yyyy";
					break;
				case 8:
					format="yy";
					break;
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					format="yyMMdd";
					break;
				case 16:
				case 17:
					format="MMM";
					break;
				case 19:
					format="M-d-yyyy";
					break;
				default:
					format="dd.MM.yyyy";
				}
				table->setDateFormat(format, j, false);
				break;
			case 3: // Time
				switch(opj.colValueTypeSpec(s,j))
				{
				case 0:
					format="hh:mm";
					break;
				case 1:
					format="hh";
					break;
				case 2:
					format="hh:mm:ss";
					break;
				case 3:
					format="hh:mm:ss.zzz";
					break;
				case 4:
					format="hh ap";
					break;
				case 5:
					format="hh:mm ap";
					break;
				case 6:
					format="mm:ss";
					break;
				case 7:
					format="mm:ss.zzz";
					break;
				case 8:
					format="hhmm";
					break;
				case 9:
					format="hhmmss";
					break;
				case 10:
					format="hh:mm:ss.zzz";
					break;
				}
				table->setTimeFormat(format, j, false);
				break;
			case 4: // Month
				switch(opj.colValueTypeSpec(s,j)){
                    case 0:
                        format = "MMM";
					break;
                    case 1:
                        format = "MMMM";
					break;
                    case 2:
                        format = "M";
					break;
				}
				table->setMonthFormat(format, j, false);
				break;
			case 5: // Day
				switch(opj.colValueTypeSpec(s,j)){
                    case 0:
                        format = "ddd";
					break;
                    case 1:
                        format = "dddd";
					break;
                    case 2:
                        format = "d";
					break;
				}
				table->setDayFormat(format, j, false);
				break;
			}

			for (int i=0; i<opj.numRows(s,j); i++)
			{
				if(strcmp(opj.colType(s,j),"LABEL")&&opj.colValueType(s,j)!=1)
				{// number
					double* val = (double*)opj.oData(s,j,i,true);
					if(fabs(*val)>0 && fabs(*val)<2.0e-300)// empty entry
						continue;

                    //if (table->columnType(j) == Table::Date)
                        //QMessageBox::about(0, "", QDate::fromJulianDay(int(*val)+1).toString("dd/MM/yyyy"));

					table->setText(i, j, QLocale().toString(*val));
				}
				else// label? doesn't seem to work
					table->setText(i, j, QString((char*)opj.oData(s,j,i)));
			}
		}


		if(!(opj.spreadHidden(s)||opj.spreadLoose(s))||opj.Version()!=7.5)
		{
			table->showNormal();

			//cascade the tables
			int dx=table->verticalHeaderWidth();
			int dy=table->parentWidget()->frameGeometry().height() - table->height();
			table->parentWidget()->move(QPoint(visible_count*dx+xoffset*OBJECTXOFFSET,visible_count*dy));
			visible_count++;
		}
	}

//Import matrices
	for (int s=0; s<opj.numMatrices(); s++)
	{
		int nr_cols = opj.numMartixCols(s);
		int nr_rows = opj.numMartixRows(s);

		Matrix* matrix = mw->newMatrix(opj.matrixName(s), nr_rows, nr_cols);
		if (!matrix)
			return false;

		matrix->setWindowLabel(opj.matrixLabel(s));
		matrix->setFormula(opj.matrixFormula(s));
		matrix->setColumnsWidth(opj.matrixWidth(s)*QtiPlot_scaling_factor);
		matrix->table()->blockSignals(true);
		for (int j=0; j<nr_cols; j++)
		{
			for (int i=0; i<nr_rows; i++)
			{
				double val = opj.matrixData(s,j,i);
				if(fabs(val)>0 && fabs(val)<2.0e-300)// empty entry
					continue;

				matrix->setCell(i, j, val);
			}
		}

		matrix->saveCellsToMemory();

		QChar f;
		switch(opj.matrixValueTypeSpec(s))
		{
		case 0: //Decimal 1000
			f='f';
			break;
		case 1: //Scientific
			f='e';
			break;
		case 2: //Engeneering
		case 3: //Decimal 1,000
			f='g';
			break;
		}
		matrix->setNumericFormat(f, opj.matrixSignificantDigits(s));
        matrix->table()->blockSignals(false);
		matrix->showNormal();

		//cascade the matrices
		int dx=matrix->verticalHeaderWidth();
		int dy=matrix->parentWidget()->frameGeometry().height() - matrix->height();
		matrix->parentWidget()->move(QPoint(visible_count*dx+xoffset*OBJECTXOFFSET,visible_count*dy));
		visible_count++;

	}

	if(visible_count>0)
		xoffset++;
	return true;
}

bool ImportOPJ::importNotes(OPJFile opj)
{
	int visible_count=0;
	for (int n=0; n<opj.numNotes(); n++)
	{
		QString name=opj.noteName(n);
		QRegExp rx("^@(\\S+)$");
		if(rx.indexIn(name)==0)
		{
			name=name.mid(2,name.length()-3);
		}
		Note *note = mw->newNote(name);
		if(!note)
			return false;
		note->setWindowLabel(opj.noteLabel(n));
		note->setText(opj.noteText(n));

		//cascade the notes
		int dx=20;
		int dy=note->parentWidget()->frameGeometry().height() - note->height();
		note->parentWidget()->move(QPoint(visible_count*dx+xoffset*OBJECTXOFFSET,visible_count*dy));
		visible_count++;
	}
	if(visible_count>0)
		xoffset++;
	return true;
}

bool ImportOPJ::importGraphs(OPJFile opj)
{
	double pi=3.141592653589793;
	int visible_count=0;
	int tickTypeMap[]={0,3,1,2};
	for (int g=0; g<opj.numGraphs(); g++)
	{
		MultiLayer *ml = mw->multilayerPlot(opj.graphName(g));
		if (!ml)
			return false;

		ml->hide();//!hack used in order to avoid resize and repaint events
		ml->setWindowLabel(opj.graphLabel(g));
		for(int l=0; l<opj.numLayers(g); l++)
		{
			Graph *graph=ml->addLayer();
			if(!graph)
				return false;

			graph->setXAxisTitle(parseOriginText(QString::fromLocal8Bit(opj.layerXAxisTitle(g,l))));
			graph->setYAxisTitle(parseOriginText(QString::fromLocal8Bit(opj.layerYAxisTitle(g,l))));
			if(strlen(opj.layerLegend(g,l))>0)
				graph->newLegend(parseOriginText(QString::fromLocal8Bit(opj.layerLegend(g,l))));
			int auto_color=0;
			int auto_color1=0;
			int style=0;
			for(int c=0; c<opj.numCurves(g,l); c++)
			{
				QString data(opj.curveDataName(g,l,c));
				int color=0;
				switch(opj.curveType(g,l,c))
				{
				case OPJFile::Line:
					style=Graph::Line;
					break;
				case OPJFile::Scatter:
					style=Graph::Scatter;
					break;
				case OPJFile::LineSymbol:
					style=Graph::LineSymbols;
					break;
				case OPJFile::ErrorBar:
				case OPJFile::XErrorBar:
					style=Graph::ErrorBars;
					break;
				case OPJFile::Column:
					style=Graph::VerticalBars;
					break;
				case OPJFile::Bar:
					style=Graph::HorizontalBars;
					break;
				case OPJFile::Histogram:
					style=Graph::Histogram;
					break;
				default:
					continue;
				}
				QString tableName;
				switch(data[0].toAscii())
				{
				case 'T':
					tableName = data.right(data.length()-2);
					if(style==Graph::ErrorBars)
					{
						int flags=opj.curveSymbolType(g,l,c);
						graph->addErrorBars(tableName + "_" + opj.curveXColName(g,l,c), mw->table(tableName), tableName + "_" + opj.curveYColName(g,l,c),
							((flags&0x10)==0x10?0:1), ceil(opj.curveLineWidth(g,l,c)), ceil(opj.curveSymbolSize(g,l,c)), QColor(Qt::black),
							(flags&0x40)==0x40, (flags&2)==2, (flags&1)==1);
					}
					else if(style==Graph::Histogram)
						graph->insertCurve(mw->table(tableName), tableName + "_" + opj.curveYColName(g,l,c), style);
					else
						graph->insertCurve(mw->table(tableName), tableName + "_" + opj.curveXColName(g,l,c), tableName + "_" + opj.curveYColName(g,l,c), style);
					break;
				case 'F':
					QStringList formulas;
					QList<double> ranges;
					int s=opj.functionIndex(data.right(data.length()-2).toStdString().c_str());
					int type;
					if(opj.functionType(s)==1)//Polar
					{
						type=2;
						formulas << opj.functionFormula(s) << "x";
						ranges << pi/180*opj.functionBegin(s) << pi/180*opj.functionEnd(s);
					}
					else
					{
						type=0;
						formulas << opj.functionFormula(s);
						ranges << opj.functionBegin(s) << opj.functionEnd(s);
					}
					graph->addFunctionCurve(type, formulas, "x", ranges, opj.functionPoints(s), opj.functionName(s));

					mw->updateFunctionLists(type, formulas);
					break;
				}

				CurveLayout cl = graph->initCurveLayout(style, opj.numCurves(g,l));
				cl.sSize = ceil(opj.curveSymbolSize(g,l,c));
				cl.penWidth=opj.curveSymbolThickness(g,l,c);
				color=opj.curveSymbolColor(g,l,c);
				if((style==Graph::Scatter||style==Graph::LineSymbols)&&color==0xF7)//0xF7 -Automatic color
					color=auto_color++;
				cl.symCol=color;
				switch(opj.curveSymbolType(g,l,c)&0xFF)
				{
				case 0: //NoSymbol
					cl.sType=0;
					break;
				case 1: //Rect
					cl.sType=2;
					break;
				case 2: //Ellipse
				case 20://Sphere
					cl.sType=1;
					break;
				case 3: //UTriangle
					cl.sType=6;
					break;
				case 4: //DTriangle
					cl.sType=5;
					break;
				case 5: //Diamond
					cl.sType=3;
					break;
				case 6: //Cross +
					cl.sType=9;
					break;
				case 7: //Cross x
					cl.sType=10;
					break;
				case 8: //Snow
					cl.sType=13;
					break;
				case 9: //Horizontal -
					cl.sType=11;
					break;
				case 10: //Vertical |
					cl.sType=12;
					break;
				case 15: //LTriangle
					cl.sType=7;
					break;
				case 16: //RTriangle
					cl.sType=8;
					break;
				case 17: //Hexagon
				case 19: //Pentagon
					cl.sType=15;
					break;
				case 18: //Star
					cl.sType=14;
					break;
				default:
					cl.sType=0;
				}

				switch(opj.curveSymbolType(g,l,c)>>8)
				{
				case 0:
					cl.fillCol=color;
					break;
				case 1:
				case 2:
				case 8:
				case 9:
				case 10:
				case 11:
					color=opj.curveSymbolFillColor(g,l,c);
					if((style==Graph::Scatter||style==Graph::LineSymbols)&&color==0xF7)//0xF7 -Automatic color
						color=17;// depend on Origin settings - not stored in file
					cl.fillCol=color;
					break;
				default:
					cl.fillCol=-1;
				}

				cl.lWidth = ceil(opj.curveLineWidth(g,l,c));
				color=opj.curveLineColor(g,l,c);
				cl.lCol=(color==0xF7?0:color); //0xF7 -Automatic color
				int linestyle=opj.curveLineStyle(g,l,c);
				cl.filledArea=(opj.curveIsFilledArea(g,l,c)||style==Graph::VerticalBars||style==Graph::HorizontalBars||style==Graph::Histogram)?1:0;
				if(cl.filledArea)
				{
					switch(opj.curveFillPattern(g,l,c))
					{
					case 0:
						cl.aStyle=0;
						break;
					case 1:
					case 2:
					case 3:
						cl.aStyle=4;
						break;
					case 4:
					case 5:
					case 6:
						cl.aStyle=5;
						break;
					case 7:
					case 8:
					case 9:
						cl.aStyle=6;
						break;
					case 10:
					case 11:
					case 12:
						cl.aStyle=1;
						break;
					case 13:
					case 14:
					case 15:
						cl.aStyle=2;
						break;
					case 16:
					case 17:
					case 18:
						cl.aStyle=3;
						break;
					}
					color=(cl.aStyle==0 ? opj.curveFillAreaColor(g,l,c) : opj.curveFillPatternColor(g,l,c));
					cl.aCol=(color==0xF7?0:color); //0xF7 -Automatic color
					if (style == Graph::VerticalBars || style == Graph::HorizontalBars || style == Graph::Histogram)
					{
						color=opj.curveFillPatternBorderColor(g,l,c);
						cl.lCol = (color==0xF7?0:color); //0xF7 -Automatic color
						color=(cl.aStyle==0 ? opj.curveFillAreaColor(g,l,c) : opj.curveFillPatternColor(g,l,c));
						cl.aCol=(color==0xF7?cl.lCol:color); //0xF7 -Automatic color
						cl.lWidth = ceil(opj.curveFillPatternBorderWidth(g,l,c));
						linestyle=opj.curveFillPatternBorderStyle(g,l,c);
					}
				}
				switch (linestyle)
				{
					case OPJFile::Solid:
						cl.lStyle=0;
						break;
					case OPJFile::Dash:
					case OPJFile::ShortDash:
						cl.lStyle=1;
						break;
					case OPJFile::Dot:
					case OPJFile::ShortDot:
						cl.lStyle=2;
						break;
					case OPJFile::DashDot:
					case OPJFile::ShortDashDot:
						cl.lStyle=3;
						break;
					case OPJFile::DashDotDot:
						cl.lStyle=4;
						break;
				}

				graph->updateCurveLayout(c, &cl);
				if (style == Graph::VerticalBars || style == Graph::HorizontalBars)
				{
					QwtBarCurve *b = (QwtBarCurve*)graph->curve(c);
					if (b)
						b->setGap(qRound(100-opj.curveSymbolSize(g,l,c)*10));
				}
				else if(style == Graph::Histogram)
				{
					QwtHistogram *h = (QwtHistogram*)graph->curve(c);
					if (h)
					{
						vector<double> bin=opj.layerHistogram(g,l);
						if(bin.size()==3)
							h->setBinning(false, bin[0], bin[1], bin[2]);
						h->loadData();
					}
				}
				switch(opj.curveLineConnect(g,l,c))
				{
				case OPJFile::NoLine:
					graph->setCurveStyle(c, QwtPlotCurve::NoCurve);
					break;
				case OPJFile::Straight:
					graph->setCurveStyle(c, QwtPlotCurve::Lines);
					break;
				case OPJFile::BSpline:
				case OPJFile::Bezier:
				case OPJFile::Spline:
					graph->setCurveStyle(c, 5);
					break;
				case OPJFile::StepHorizontal:
				case OPJFile::StepHCenter:
					graph->setCurveStyle(c, QwtPlotCurve::Steps);
					break;
				case OPJFile::StepVertical:
				case OPJFile::StepVCenter:
					graph->setCurveStyle(c, 6);
					break;
				}

			}
			vector<double> rangeX=opj.layerXRange(g,l);
			vector<int>    ticksX=opj.layerXTicks(g,l);
			vector<double> rangeY=opj.layerYRange(g,l);
			vector<int>	   ticksY=opj.layerYTicks(g,l);
			if(style==Graph::HorizontalBars)
			{
				graph->setScale(0,rangeX[0],rangeX[1],rangeX[2],ticksX[0],ticksX[1],opj.layerXScale(g,l));
				graph->setScale(2,rangeY[0],rangeY[1],rangeY[2],ticksY[0],ticksY[1],opj.layerYScale(g,l));
			}
			else
			{
				graph->setScale(2,rangeX[0],rangeX[1],rangeX[2],ticksX[0],ticksX[1],opj.layerXScale(g,l));
				graph->setScale(0,rangeY[0],rangeY[1],rangeY[2],ticksY[0],ticksY[1],opj.layerYScale(g,l));
			}

			//grid
			vector<graphGrid> grids=opj.layerGrid(g,l);
			GridOptions grid;
			grid.majorOnX=(grids[0].hidden?0:1);
			grid.minorOnX=(grids[1].hidden?0:1);
			grid.majorOnY=(grids[2].hidden?0:1);
			grid.minorOnY=(grids[3].hidden?0:1);
			grid.majorStyle=translateOrigin2QtiplotLineStyle(grids[0].style);
			grid.majorCol=grids[0].color;
			grid.majorWidth=ceil(grids[0].width);
			grid.minorStyle=translateOrigin2QtiplotLineStyle(grids[1].style);
			grid.minorCol=grids[1].color;
			grid.minorWidth=ceil(grids[1].width);
			grid.xZeroOn=0;
			grid.yZeroOn=0;
			grid.xAxis=2;
			grid.yAxis=0;
			graph->setGridOptions(grid);

			vector<graphAxisFormat> formats=opj.layerAxisFormat(g,l);
			vector<graphAxisTick> ticks=opj.layerAxisTickLabels(g,l);
			for(int i=0; i<4; ++i)
			{
				QString data(ticks[i].dataName.c_str());
				QString tableName=data.right(data.length()-2) + "_" + ticks[i].colName.c_str();

				QString formatInfo;
				int format;
				int type;
				int prec=ticks[i].decimal_places;
				switch(ticks[i].value_type)
				{
				case OPJFile::Numeric:
					type=Graph::Numeric;
					switch(ticks[i].value_type_specification)
					{
					case 0: //Decimal 1000
						format=1;
						break;
					case 1: //Scientific
						format=2;
						break;
					case 2: //Engeneering
					case 3: //Decimal 1,000
						format=0;
						break;
					}
					if(prec==-1)
						prec=2;
					break;
				case OPJFile::Text: //Text
					type=Graph::Txt;
					break;
				case 2: // Date
					type=Graph::Date;
					break;
				case 3: // Time
					type=Graph::Time;
					break;
				case OPJFile::Month: // Month
					type=Graph::Month;
					format=ticks[i].value_type_specification;
					break;
				case OPJFile::Day: // Day
					type=Graph::Day;
					format=ticks[i].value_type_specification;
					break;
				case OPJFile::ColumnHeading:
					type=Graph::ColHeader;
					switch(ticks[i].value_type_specification)
					{
					case 0: //Decimal 1000
						format=1;
						break;
					case 1: //Scientific
						format=2;
						break;
					case 2: //Engeneering
					case 3: //Decimal 1,000
						format=0;
						break;
					}
					prec=2;
					break;
				default:
					type=Graph::Numeric;
					format=0;
					prec=2;
				}

				graph->showAxis(i, type, tableName, mw->table(tableName), !(formats[i].hidden),
					tickTypeMap[formats[i].majorTicksType], tickTypeMap[formats[i].minorTicksType],
					!(ticks[i].hidden),	ColorBox::color(formats[i].color), format, prec,
					ticks[i].rotation, 0, "", (ticks[i].color==0xF7 ? ColorBox::color(formats[i].color) : ColorBox::color(ticks[i].color)));
			}


			graph->setAutoscaleFonts(mw->autoScaleFonts);//restore user defined fonts behaviour
        	graph->setIgnoreResizeEvents(!mw->autoResizeLayers);
		}
		//cascade the graphs
		if(!opj.graphHidden(g))
		{
			int dx=20;
			int dy=ml->parentWidget()->frameGeometry().height() - ml->height();
			ml->parentWidget()->move(QPoint(visible_count*dx+xoffset*OBJECTXOFFSET,visible_count*dy));
			visible_count++;
			ml->show();
			ml->arrangeLayers(true,true);
		}
		else
		{
			ml->show();
			ml->arrangeLayers(true,true);
			mw->hideWindow(ml);
		}
	}
	if(visible_count>0)
		xoffset++;
	return true;
}

QString ImportOPJ::parseOriginText(const QString &str)
{
	QStringList lines=str.split("\n");
	QString text="";
	for(int i=0; i<lines.size(); ++i)
	{
		if(i>0)
			text.append("\n");
		text.append(parseOriginTags(lines[i]));
	}
	return text;
}

QString ImportOPJ::parseOriginTags(const QString &str)
{
	QString line=str;
	//replace \l(...) and %(...) tags
	QRegExp rxline("\\\\\\s*l\\s*\\(\\s*\\d+\\s*\\)");
	QRegExp rxcol("\\%\\(\\d+\\)");
	int pos = rxline.indexIn(line);
	while (pos > -1) {
		QString value = rxline.cap(0);
		int len=value.length();
		value.replace(QRegExp(" "),"");
		value="\\c{"+value.mid(3,value.length()-4)+"}";
		line.replace(pos, len, value);
		pos = rxline.indexIn(line);
	}
	//Lookbehind conditions are not supported - so need to reverse string
	QRegExp rx("\\)[^\\)\\(]*\\((?!\\s*[buig\\+\\-]\\s*\\\\)");
	QRegExp rxfont("\\)[^\\)\\(]*\\((?![^\\:]*\\:f\\s*\\\\)");
	QString linerev = strreverse(line);
	QString lBracket=strreverse("&lbracket;");
	QString rBracket=strreverse("&rbracket;");
	QString ltagBracket=strreverse("&ltagbracket;");
	QString rtagBracket=strreverse("&rtagbracket;");
	int pos1=rx.indexIn(linerev);
	int pos2=rxfont.indexIn(linerev);

	while (pos1>-1 || pos2>-1) {
		if(pos1==pos2)
		{
			QString value = rx.cap(0);
			int len=value.length();
			value=rBracket+value.mid(1,len-2)+lBracket;
			linerev.replace(pos1, len, value);
		}
		else if ((pos1>pos2&&pos2!=-1)||pos1==-1)
		{
			QString value = rxfont.cap(0);
			int len=value.length();
			value=rtagBracket+value.mid(1,len-2)+ltagBracket;
			linerev.replace(pos2, len, value);
		}
		else if ((pos2>pos1&&pos1!=-1)||pos2==-1)
		{
			QString value = rx.cap(0);
			int len=value.length();
			value=rtagBracket+value.mid(1,len-2)+ltagBracket;
			linerev.replace(pos1, len, value);
		}

		pos1=rx.indexIn(linerev);
		pos2=rxfont.indexIn(linerev);
	}
	linerev.replace(ltagBracket, "(");
	linerev.replace(rtagBracket, ")");

	line = strreverse(linerev);

	//replace \b(...), \i(...), \u(...), \g(...), \+(...), \-(...), \f:font(...) tags
	QString rxstr[]={
		"\\\\\\s*b\\s*\\(",
		"\\\\\\s*i\\s*\\(",
		"\\\\\\s*u\\s*\\(",
		"\\\\\\s*g\\s*\\(",
		"\\\\\\s*\\+\\s*\\(",
		"\\\\\\s*\\-\\s*\\(",
		"\\\\\\s*f\\:[^\\(]*\\("};
	int postag[]={0,0,0,0,0,0,0};
	QString ltag[]={"<b>","<i>","<u>","<font face=Symbol>","<sup>","<sub>","<font face=%1>"};
	QString rtag[]={"</b>","</i>","</u>","</font>","</sup>","</sub>","</font>"};
	QRegExp rxtags[7];
	for(int i=0; i<7; ++i)
		rxtags[i].setPattern(rxstr[i]+"[^\\(\\)]*\\)");

	bool flag=true;
	while(flag) {
		for(int i=0; i<7; ++i)
		{
			postag[i] = rxtags[i].indexIn(line);
			while (postag[i] > -1) {
				QString value = rxtags[i].cap(0);
				int len=value.length();
				int pos2=value.indexOf("(");
				if(i<6)
					value=ltag[i]+value.mid(pos2+1,len-pos2-2)+rtag[i];
				else
				{
					int posfont=value.indexOf("f:");
					value=ltag[i].arg(value.mid(posfont+2,pos2-posfont-2))+value.mid(pos2+1,len-pos2-2)+rtag[i];
				}
				line.replace(postag[i], len, value);
				postag[i] = rxtags[i].indexIn(line);
			}
		}
		flag=false;
		for(int i=0; i<7; ++i)
		{
			if(rxtags[i].indexIn(line)>-1)
			{
				flag=true;
				break;
			}
		}
	}

	//replace unclosed tags
	for(int i=0; i<6; ++i)
		line.replace(QRegExp(rxstr[i]), ltag[i]);
	rxfont.setPattern(rxstr[6]);
	pos = rxfont.indexIn(line);
	while (pos > -1) {
		QString value = rxfont.cap(0);
		int len=value.length();
		int posfont=value.indexOf("f:");
		value=ltag[6].arg(value.mid(posfont+2,len-posfont-3));
		line.replace(pos, len, value);
		pos = rxfont.indexIn(line);
	}

	line.replace("&lbracket;", "(");
	line.replace("&rbracket;", ")");

	return line;
}

//TODO: bug in grid dialog
//		scale/minor ticks checkbox
//		histogram: autobin export
//		if prec not setted - automac+4digits
