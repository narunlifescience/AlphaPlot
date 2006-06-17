/***************************************************************************
    File                 : graph3D.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : 3D graph widget
                           
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
#include "graph3D.h"
#include "bars.h"
#include "cones.h"
#include "parser.h"
#include "nrutil.h"

#include <qapplication.h>
#include <qmessagebox.h>
#include <q3filedialog.h>
#include <qprinter.h>
#include <qimage.h>
#include <qclipboard.h>
#include <q3dragobject.h> 

#include <qwt3d_surfaceplot.h>
#include <qwt3d_function.h>
#include <qwt3d_io.h>
#include <qwt3d_io_gl2ps.h>
#include <qwt3d_io_reader.h>
#include <qwt3d_coordsys.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3MemArray>
#include <QEvent>
#include <QContextMenuEvent>
#include <QResizeEvent>

#include <QDateTime>
#include <QCursor>

#include <gsl/gsl_vector.h>
#include <math.h>

using namespace Qwt3D;

UserFunction::UserFunction(const QString& s, SurfacePlot& pw)
: Function(pw)
{
	formula=s;
}

double UserFunction::operator()(double x, double y)
{
	if (formula.isEmpty())
		return 0.0;

	MyParser parser;
	double result=0.0;
	try
	{
		parser.DefineVar("x", &x);
		parser.DefineVar("y", &y);	

		parser.SetExpr((const std::string)formula.ascii());
		result=parser.Eval();
	}
	catch(mu::ParserError &e)
	{
		QMessageBox::critical(0,"QtiPlot - Input function error",QString::fromStdString(e.GetMsg()));
	}
	return result;
}

UserFunction::~UserFunction()      
{
}

	Graph3D::Graph3D(const QString& label, QWidget* parent, const char* name, Qt::WFlags f)
: MyWidget(label,parent,name,f)
{
	initPlot();
}

void Graph3D::initPlot()
{
	worksheet = 0;
	matrix_ = 0;

	QDateTime dt = QDateTime::currentDateTime ();
	setBirthDate(dt.toString(Qt::LocalDate));

	ignoreFonts = false;

	sp = new SurfacePlot(this);
	sp->resize(500,400);
	sp->installEventFilter(this);
	sp->setRotation(30,0,15);
	sp->setScale(1,1,1);
	sp->setShift(0.15,0,0);
	sp->setZoom(0.9);

	smoothMesh = true;
	sp->setSmoothMesh(smoothMesh);

	title=QString();
	sp->setTitle(title);

	titleCol=QColor(Qt::black);
	sp->setTitleColor(Qt2GL(titleCol));

	titleFnt= QFont("Times New Roman",14);
	titleFnt.setBold(true);

	sp->setTitleFont(titleFnt.family(),titleFnt.pointSize(),
			titleFnt.weight(),titleFnt.italic());

	axesCol=QColor(Qt::black);
	labelsCol=QColor(Qt::black);
	numCol=QColor(Qt::black);
	meshCol=QColor(Qt::black);
	gridCol=QColor(Qt::black);
	bgCol=QColor(255, 255, 255);
	fromColor=QColor(Qt::red);
	toColor=QColor(Qt::blue);

	col_ = 0;

	legendOn=true;
	legendMajorTicks = 5;
	sp->showColorLegend(legendOn);
	sp->legend()->setAutoScale(true);
	sp->legend()->setMajors (legendMajorTicks) ;

	labelsDist=0;

	scaleType=Q3MemArray<int>(3);
	for (int j=0;j<3;j++)
		scaleType[j]=0;

	pointStyle=None;
	func=0;alpha=1.0;
	barsRad=0.007; 
	pointSize=5; smooth=false;
	crossHairRad=0.03, crossHairLineWidth=2; 
	crossHairSmooth=true, crossHairBoxed=false;
	conesQuality=32; conesRad=0.5;

	style_ = FILLEDMESH;
	initCoord();

	connect(sp,SIGNAL(rotationChanged(double, double, double)),this,SLOT(rotationChanged(double, double, double)));
	connect(sp,SIGNAL(zoomChanged(double)),this,SLOT(zoomChanged(double)));
	connect(sp,SIGNAL(scaleChanged(double, double, double)),this,SLOT(scaleChanged(double, double, double)));
	connect(sp,SIGNAL(shiftChanged(double, double, double)),this,SLOT(shiftChanged(double, double, double)));
}

void Graph3D::initCoord()
{
	sp->makeCurrent();
	for (unsigned i=0; i!=sp->coordinates()->axes.size(); ++i)
	{
		sp->coordinates()->axes[i].setMajors(5);
		sp->coordinates()->axes[i].setMinors(5);
	}

	QString s = tr("X axis");
	sp->coordinates()->axes[X1].setLabelString(s);
	sp->coordinates()->axes[X2].setLabelString(s);
	sp->coordinates()->axes[X3].setLabelString(s);
	sp->coordinates()->axes[X4].setLabelString(s);
	labels<<s;

	s = tr("Y axis");
	sp->coordinates()->axes[Y1].setLabelString(s);
	sp->coordinates()->axes[Y2].setLabelString(s);
	sp->coordinates()->axes[Y3].setLabelString(s);
	sp->coordinates()->axes[Y4].setLabelString(s);
	labels<<s;

	s = tr("Z axis");
	sp->coordinates()->axes[Z1].setLabelString(s); 
	sp->coordinates()->axes[Z2].setLabelString(s); 
	sp->coordinates()->axes[Z3].setLabelString(s); 
	sp->coordinates()->axes[Z4].setLabelString(s); 
	labels<<s;

	sp->setCoordinateStyle(BOX);
	sp->coordinates()->setAutoScale(false);	
}

void Graph3D::addFunction(const QString& s,double xl,double xr,double yl,
		double yr,double zl,double zr)
{
	sp->makeCurrent();
	sp->resize(this->size());

	func= new UserFunction(s, *sp);

	func->setMesh(41,31);
	func->setDomain(xl,xr,yl,yr);
	func->setMinZ(zl);
	func->setMaxZ(zr);
	func->create();

	sp->legend()->setLimits(zl,zr);

	if (sp->plotStyle() == NOPLOT)
	{
		sp->setPlotStyle(FILLED);
		style_=FILLED;
		pointStyle = None;
	}
}

void Graph3D::insertFunction(const QString& s,double xl,double xr,double yl,
		double yr,double zl,double zr)
{
	addFunction(s,xl,xr,yl,yr,zl,zr);
	update();
}

void Graph3D::addData(Table* table, int xcol, int ycol)
{
	worksheet=table;	
	int r=table->tableRows();
	int i, xmesh=0, ymesh=2;
	for (i = 0; i < r; i++) 
	{
		if (!table->text(i,xcol).isEmpty() && !table->text(i,ycol).isEmpty())
			xmesh++;
	}

	if (xmesh == 0)
		xmesh++;

	double** data =matrix(0,xmesh-1,0,ymesh-1);
	gsl_vector * x = gsl_vector_alloc (xmesh);
	gsl_vector * y = gsl_vector_alloc (xmesh);

	for (int j = 0; j < ymesh; j++) 
	{
		int k=0;				
		for (i = 0; i < r; i++) 
		{
			if (!table->text(i,xcol).isEmpty() && !table->text(i,ycol).isEmpty())
			{
				double xv=table->text(i,xcol).toDouble();
				double yv=table->text(i,ycol).toDouble();

				gsl_vector_set (x, k, xv);
				gsl_vector_set (y, k, yv);

				data[k][j] =yv;
				k++;
			}
		}
	}   

	double maxy=gsl_vector_max(y);
	double maxz=0.6*maxy;
	sp->makeCurrent();
	sp->legend()->setLimits(gsl_vector_min(y),maxy);
	sp->loadFromData(data, xmesh, ymesh, gsl_vector_min(x),gsl_vector_max(x),0,maxz);

	gsl_vector_free (x);
	gsl_vector_free (y);
	free_matrix(data,0,xmesh-1,0,ymesh-1);
}

void Graph3D::addData(Table* table,const QString& colName)
{
	int ycol = table->colIndex(colName);
	int xcol=table->colX(ycol);
	plotAssociation = table->colName(xcol)+"(X)," + colName + "(Y)";

	addData(table, xcol, ycol);
}

void Graph3D::addData(Table* table,const QString& xColName,const QString& yColName)
{
	int xcol=table->colIndex(xColName);
	plotAssociation = table->colName(xcol)+"(X)," + yColName+"(Y)";

	addData(table, xcol, table->colIndex(yColName));
}

void Graph3D::changeMatrix(Matrix* m)
{
	if (matrix_ == m)
		return;

	matrix_ = m;
	plotAssociation = "matrix<" + QString(m->name()) + ">";

	int cols=m->numCols();	
	int rows=m->numRows();
	double** data_matrix = matrix(0, rows-1, 0, cols-1);
	for (int i = 0; i < rows; i++ ) 
	{
		for (int j = 0; j < cols; j++) 
		{
			double val = m->text(i,j).toDouble();
			data_matrix[i][j] = val;
		}
	}   

	sp->makeCurrent();
	sp->loadFromData(data_matrix, rows, cols, 0, rows, 0, cols);

	double start, end;
	sp->coordinates()->axes[Z1].limits (start, end);
	sp->legend()->setLimits(start, end);
	sp->legend()->setMajors(legendMajorTicks);

	free_matrix(data_matrix,0,rows-1,0,cols-1);
	setAxesLabels(labels);
}

void Graph3D::addMatrixData(Matrix* m)
{
	matrix_ = m;
	plotAssociation = "matrix<" + QString(m->name()) + ">";

	int cols=m->numCols();	
	int rows=m->numRows();
	double** data_matrix = matrix(0, rows-1, 0, cols-1);
	for (int i = 0; i < rows; i++ ) 
	{
		for (int j = 0; j < cols; j++) 
		{
			double val = m->text(i,j).toDouble();
			data_matrix[i][j] = val;
		}
	}   

	sp->makeCurrent();
	sp->loadFromData(data_matrix, rows, cols, m->xStart(), m->xEnd(), 
			m->yStart(), m->yEnd());

	double start, end;
	sp->coordinates()->axes[Z1].limits (start, end);
	sp->legend()->setLimits(start, end);
	sp->legend()->setMajors(legendMajorTicks);

	free_matrix(data_matrix,0,rows-1,0,cols-1);
	update();
}

void Graph3D::addMatrixData(Matrix* m, double xl, double xr, 
		double yl, double yr, double zl, double zr)
{
	matrix_ = m;
	plotAssociation = "matrix<" + QString(m->name()) + ">";

	updateScalesFromMatrix(xl, xr, yl, yr, zl, zr);
}

void Graph3D::addData(Table* table,const QString& xColName,const QString& yColName,
		double xl, double xr, double yl, double yr, double zl, double zr)
{
	worksheet=table;
	int r=table->tableRows();
	int xcol=table->colIndex(xColName);
	int ycol=table->colIndex(yColName);

	QString s=table->colName(xcol)+"(X),";
	s+=yColName+"(Y)";
	plotAssociation = s;

	int i, j, xmesh=0, ymesh=2;
	double xv, yv;

	for (i = 0; i < r; i++) 
	{
		if (!table->text(i,xcol).isEmpty() && !table->text(i,ycol).isEmpty())
		{
			xv=table->text(i,xcol).toDouble();
			if (xv>=xl && xv <= xr)
				xmesh++;
		}
	}

	if (xmesh == 0)
		xmesh++;

	double** data =matrix(0,xmesh-1,0,ymesh-1);
	for ( j = 0; j < ymesh; j++) 
	{ 
		int k=0;		
		for ( i = 0; i < r; i++) 
		{
			if (!table->text(i,xcol).isEmpty() && !table->text(i,ycol).isEmpty())
			{
				xv=table->text(i,xcol).toDouble();
				if (xv>=xl && xv <= xr)
				{
					yv=table->text(i,ycol).toDouble();
					if (yv > zr)				
						data[k][j] = zr;
					else if (yv < zl)				
						data[k][j] = zl;
					else
						data[k][j] = yv;
					k++;
				}
			}
		}
	}   
	sp->makeCurrent();
	sp->loadFromData(data, xmesh, ymesh, xl, xr, yl, yr);
	sp->legend()->setLimits(zl, zr);
	sp->legend()->setMajors(legendMajorTicks);

	free_matrix(data,0,xmesh-1,0,ymesh-1);
}

void Graph3D::insertNewData(Table* table, const QString& colName)
{
	int zCol=table->colIndex(colName);
	int yCol=table->colY(zCol);
	int xCol=table->colX(zCol);

	addData(table, xCol, yCol, zCol, Trajectory);
	update();
}

void Graph3D::changeDataColumn(Table* table, const QString& colName)
{
	int zCol=table->colIndex(colName);
	int yCol=table->colY(zCol);
	int xCol=table->colX(zCol);	

	plotAssociation = table->colName(xCol)+"(X)," + table->colName(yCol)+"(Y),";
	plotAssociation += colName+"(Z)";

	sp->setScale(1,1,1);	
	updateDataXYZ(table, xCol, yCol, zCol);
	update();
}

void Graph3D::addData(Table* table, int xCol,int yCol,int zCol, int type)
{
	worksheet=table;
	int r=table->tableRows();

	QString s=table->colName(xCol)+"(X),";
	s+=table->colName(yCol)+"(Y),";
	s+=table->colName(zCol)+"(Z)";
	plotAssociation = s;

	int i,j,columns=0;	
	for ( i = 0; i < r; i++) 
	{
		if (!table->text(i,xCol).isEmpty() && !table->text(i,yCol).isEmpty() && !table->text(i,zCol).isEmpty())
			columns++;
	}

	if (columns == 0)
		columns++;

	Qwt3D::Triple **data=allocateData(columns,columns);  
	for (j = 0; j < columns; j++) 
	{   
		int k=0;
		for ( i = 0; i < r; i++) 
		{
			if(!table->text(i,xCol).isEmpty() && !table->text(i,yCol).isEmpty() && !table->text(i,zCol).isEmpty())
			{
				double xv=table->text(i,xCol).toDouble();
				double yv=table->text(i,yCol).toDouble();
				double zv=table->text(i,zCol).toDouble();

				data[k][j] = Triple(xv,yv,zv);
				k++;
			}
		}
	}   

	sp->makeCurrent();
	sp->loadFromData (data, columns, columns, false,false);

	double start, end;
	sp->coordinates()->axes[Z1].limits (start, end);
	sp->legend()->setLimits(start, end);
	sp->legend()->setMajors(legendMajorTicks);

	if (type == Scatter)
	{
		Dot d(pointSize, smooth);
		sp->setPlotStyle(d);
		pointStyle=Dots;
		style_ = Qwt3D::USER;
	}
	else if (type == Trajectory)
	{
		legendOn=false;
		sp->showColorLegend(legendOn);
	}
	else
	{
		sp->setPlotStyle(Bar(barsRad));
		pointStyle=VerticalBars;
		style_ = Qwt3D::USER;
	}

	deleteData(data,columns);
}

void Graph3D::addData(Table* table, int xCol,int yCol,int zCol, 
		double xl, double xr, double yl, double yr, double zl, double zr)
{
	worksheet=table;
	int r=table->tableRows();

	QString s=table->colName(xCol)+"(X),";
	s+=table->colName(yCol)+"(Y),";
	s+=table->colName(zCol)+"(Z)";
	plotAssociation = s;

	int i,j,columns=0;
	double xv,yv;	
	for ( i = 0; i < r; i++) 
	{
		if (!table->text(i,xCol).isEmpty() && !table->text(i,yCol).isEmpty() && !table->text(i,zCol).isEmpty())
		{	
			xv=table->text(i,xCol).toDouble();
			yv=table->text(i,yCol).toDouble();
			if (xv >= xl && xv <= xr && yv >= yl && yv <= yr)				
				columns++;
		}
	}

	if (columns == 0)
		columns++;

	Qwt3D::Triple **data=allocateData(columns,columns);
	for (j = 0; j < columns; j++) 
	{   
		int k=0;
		for ( i = 0; i < r; i++) 
		{
			if (!table->text(i,xCol).isEmpty() && !table->text(i,yCol).isEmpty() && !table->text(i,zCol).isEmpty())
			{
				xv=table->text(i,xCol).toDouble();
				yv=table->text(i,yCol).toDouble();
				if (xv >= xl && xv <= xr && yv >= yl && yv <= yr)				
				{
					double zv=table->text(i,zCol).toDouble();
					if (zv > zr)				
						data[k][j] = Triple(xv,yv,zr);
					else if (zv < zl)				
						data[k][j] = Triple(xv,yv,zl);
					else
						data[k][j] = Triple(xv,yv,zv);
					k++;
				}
			}
		}
	}   
	sp->makeCurrent();
	sp->loadFromData (data, columns, columns, false,false);
	sp->legend()->setLimits(zl, zr);
	sp->legend()->setMajors(legendMajorTicks);

	deleteData(data,columns);
}

void Graph3D::updateData(Table* table)
{
	if (func)// function plot
		return;

	QString name = plotAssociation;
	int pos=name.find("_",0);
	int posX=name.find("(",pos);
	QString xColName=name.mid(pos+1,posX-pos-1);	

	pos=name.find(",",posX);
	posX=name.find("(",pos);
	QString yColName=name.mid(pos+1,posX-pos-1);	

	int xCol=table->colIndex(xColName);
	int yCol=table->colIndex(yColName);

	if (name.contains("(Z)",true))
	{
		pos=name.find(",",posX);
		posX=name.find("(",pos);
		QString zColName=name.mid(pos+1,posX-pos-1);
		int zCol=table->colIndex(zColName);
		updateDataXYZ(table, xCol, yCol, zCol);
	}
	else
		updateDataXY(table, xCol, yCol);

	update();
}

void Graph3D::updateDataXY(Table* table, int xCol, int yCol)
{
	int r=table->tableRows();
	int i, j, xmesh=0, ymesh=2;

	for (i = 0; i < r; i++) 
	{
		if (!table->text(i,xCol).isEmpty() && !table->text(i,yCol).isEmpty())
			xmesh++;
	}

	if (xmesh<2)
	{
		sp->setPlotStyle(NOPLOT);
		update();
		return;
	}

	double** data =matrix(0,xmesh-1,0,ymesh-1);
	gsl_vector * x = gsl_vector_alloc (xmesh);
	gsl_vector * y = gsl_vector_alloc (xmesh);

	for ( j = 0; j < ymesh; j++) 
	{  
		int k=0;		
		for ( i = 0; i < r; i++) 
		{
			if (!table->text(i,xCol).isEmpty() && !table->text(i,yCol).isEmpty())
			{
				double xv=table->text(i,xCol).toDouble();
				double yv=table->text(i,yCol).toDouble();

				gsl_vector_set (x, k, xv);
				gsl_vector_set (y, k, yv);

				data[k][j] =yv;
				k++;
			}
		}
	}   

	double minx=gsl_vector_min (x);
	double maxx=gsl_vector_max(x);
	double minz=gsl_vector_min (y);
	double maxz=gsl_vector_max(y);
	double miny, maxy;

	sp->makeCurrent();
	resetNonEmptyStyle();	
	sp->coordinates()->axes[Y1].limits (miny,maxy);	//actual Y scale limits
	sp->loadFromData(data, xmesh, ymesh, minx, maxx, miny, maxy);	
	sp->legend()->setLimits(minz,maxz);
	sp->legend()->setMajors(legendMajorTicks);

	gsl_vector_free (x);gsl_vector_free (y);
	free_matrix(data,0,xmesh-1,0,ymesh-1);
}

void Graph3D::updateDataXYZ(Table* table, int xCol, int yCol, int zCol)
{
	int r=table->tableRows();
	int i,j,columns=0;	

	for ( i = 0; i < r; i++) 
	{
		if (!table->text(i,xCol).isEmpty() && !table->text(i,yCol).isEmpty() && !table->text(i,zCol).isEmpty())
			columns++;
	}

	if (columns<2)
	{
		sp->setPlotStyle(NOPLOT);
		update();
		return;
	}

	Qwt3D::Triple **data=allocateData(columns,columns);
	gsl_vector * z = gsl_vector_alloc (columns);

	for ( j = 0; j < columns; j++) 
	{   
		int k=0;
		for ( i = 0; i < r; i++) 
		{
			if (!table->text(i,xCol).isEmpty() && !table->text(i,yCol).isEmpty() && !table->text(i,zCol).isEmpty())
			{
				double xv=table->text(i,xCol).toDouble();
				double yv=table->text(i,yCol).toDouble();
				double zv=table->text(i,zCol).toDouble();

				gsl_vector_set (z, k, zv);
				data[k][j] = Triple(xv,yv,zv);
				k++;
			}
		}
	}   

	double minz=gsl_vector_min(z);
	double maxz=gsl_vector_max(z);
	gsl_vector_free (z);

	sp->makeCurrent();
	resetNonEmptyStyle();	

	sp->loadFromData (data, columns, columns, false,false);
	sp->legend()->setLimits(minz,maxz);
	sp->legend()->setMajors(legendMajorTicks);

	deleteData(data,columns);
}

void Graph3D::updateMatrixData(Matrix* m)
{
	int cols=m->numCols();	
	int rows=m->numRows();
	double** data = matrix(0, rows-1, 0, cols-1);
	for (int i = 0; i < rows; i++ ) 
	{
		for (int j = 0; j < cols; j++) 
		{
			double val = m->text(i,j).toDouble();
			data[i][j] = val;
		}
	}   

	sp->loadFromData(data,rows,cols,m->xStart(),m->xEnd(),m->yStart(),m->yEnd());

	Qwt3D::Axis z_axis = sp->coordinates()->axes[Z1];
	double start, end;
	z_axis.limits (start, end);
	z_axis.setMajors(z_axis.majors());
	z_axis.setMajors(z_axis.minors());

	sp->legend()->setLimits(start, end);
	sp->legend()->setMajors(legendMajorTicks);

	free_matrix(data,0,rows-1,0,cols-1);

	update();
}

void Graph3D::resetNonEmptyStyle()
{
	if (sp->plotStyle() != Qwt3D::NOPLOT )
		return; // the plot was not previousely emptied

	if (style_== Qwt3D::USER)
	{// reseting the right user plot style
		switch (pointStyle)
		{
			case None:
				break;

			case Dots :
				sp->setPlotStyle(Dot(pointSize, smooth));
				break;

			case VerticalBars :
				sp->setPlotStyle(Bar(barsRad));
				break;

			case HairCross :
				sp->setPlotStyle(CrossHair(crossHairRad, crossHairLineWidth, crossHairSmooth, crossHairBoxed));
				break;

			case Cones :
				sp->setPlotStyle(Cone3D(conesRad, conesQuality));
				break;
		}
	}
	else
		sp->setPlotStyle(style_);
}

UserFunction* Graph3D::userFunction()
{
	if (func)
		return func;
	else 
		return 0;
}

void Graph3D::update()
{
	sp->makeCurrent();

	resetAxesLabels();

	sp->updateData();
	sp->updateGL();
}

void Graph3D::adjustLabels(int val)
{
	if (labelsDist != val)
	{
		labelsDist=val;
		sp->coordinates()->adjustLabels(val);
		sp->makeCurrent();
		sp->updateGL();
	}
	emit modified();
}

QFont Graph3D::numbersFont()
{
	return sp->coordinates()->axes[X1].numberFont();
}

void Graph3D::setNumbersFont(const QFont& font)
{
	sp->coordinates()->setNumberFont (font);
	sp->makeCurrent();
	sp->updateGL();
}

void Graph3D::setNumbersFont(const QStringList& list)
{
	QFont fnt=QFont(list[1],list[2].toInt(),list[3].toInt(),list[4].toInt());
	sp->coordinates()->setNumberFont(fnt);
}

void Graph3D::setXAxisLabelFont(const QFont& fnt)
{
	sp->coordinates()->axes[X1].setLabelFont(fnt);
	sp->coordinates()->axes[X2].setLabelFont(fnt);
	sp->coordinates()->axes[X3].setLabelFont(fnt);
	sp->coordinates()->axes[X4].setLabelFont(fnt);
}

void Graph3D::setYAxisLabelFont(const QFont& fnt)
{
	sp->coordinates()->axes[Y1].setLabelFont(fnt);
	sp->coordinates()->axes[Y2].setLabelFont(fnt);
	sp->coordinates()->axes[Y3].setLabelFont(fnt);
	sp->coordinates()->axes[Y4].setLabelFont(fnt);
}

void Graph3D::setZAxisLabelFont(const QFont& fnt)
{
	sp->coordinates()->axes[Z1].setLabelFont(fnt);
	sp->coordinates()->axes[Z2].setLabelFont(fnt);
	sp->coordinates()->axes[Z3].setLabelFont(fnt);
	sp->coordinates()->axes[Z4].setLabelFont(fnt);
}

void Graph3D::setXAxisLabelFont(const QStringList& list)
{
	QFont fnt=QFont(list[1],list[2].toInt(),list[3].toInt(),list[4].toInt());
	sp->coordinates()->axes[X1].setLabelFont(fnt);
	sp->coordinates()->axes[X2].setLabelFont(fnt);
	sp->coordinates()->axes[X3].setLabelFont(fnt);
	sp->coordinates()->axes[X4].setLabelFont(fnt);
}

void Graph3D::setYAxisLabelFont(const QStringList& list)
{
	QFont fnt=QFont(list[1],list[2].toInt(),list[3].toInt(),list[4].toInt());
	sp->coordinates()->axes[Y1].setLabelFont(fnt);
	sp->coordinates()->axes[Y2].setLabelFont(fnt);
	sp->coordinates()->axes[Y3].setLabelFont(fnt);
	sp->coordinates()->axes[Y4].setLabelFont(fnt);
}

void Graph3D::setZAxisLabelFont(const QStringList& list)
{
	QFont fnt=QFont(list[1],list[2].toInt(),list[3].toInt(),list[4].toInt());
	sp->coordinates()->axes[Z1].setLabelFont(fnt);
	sp->coordinates()->axes[Z2].setLabelFont(fnt);
	sp->coordinates()->axes[Z3].setLabelFont(fnt);
	sp->coordinates()->axes[Z4].setLabelFont(fnt);
}

QStringList Graph3D::axisTickLengths()
{
	QStringList list;
	double majorl,minorl;

	sp->coordinates()->axes[X1].ticLength (majorl,minorl);
	list<<QString::number(majorl);
	list<<QString::number(minorl);

	sp->coordinates()->axes[Y1].ticLength (majorl,minorl);
	list<<QString::number(majorl);
	list<<QString::number(minorl);

	sp->coordinates()->axes[Z1].ticLength (majorl,minorl);
	list<<QString::number(majorl);
	list<<QString::number(minorl);

	return list;
}

void Graph3D::setTickLengths(const QStringList& list)
{
	double majorl,minorl;
	QStringList tick_length = list;
	if (int(list.count()) > 6)
		tick_length.remove(tick_length.first());

	majorl=tick_length[0].toDouble();
	minorl=tick_length[1].toDouble();
	sp->coordinates()->axes[X1].setTicLength (majorl,minorl);
	sp->coordinates()->axes[X2].setTicLength (majorl,minorl);
	sp->coordinates()->axes[X3].setTicLength (majorl,minorl);
	sp->coordinates()->axes[X4].setTicLength (majorl,minorl);

	majorl=tick_length[2].toDouble();
	minorl=tick_length[3].toDouble();
	sp->coordinates()->axes[Y1].setTicLength (majorl,minorl);
	sp->coordinates()->axes[Y2].setTicLength (majorl,minorl);
	sp->coordinates()->axes[Y3].setTicLength (majorl,minorl);
	sp->coordinates()->axes[Y4].setTicLength (majorl,minorl);

	majorl=tick_length[4].toDouble();
	minorl=tick_length[5].toDouble();
	sp->coordinates()->axes[Z1].setTicLength (majorl,minorl);
	sp->coordinates()->axes[Z2].setTicLength (majorl,minorl);
	sp->coordinates()->axes[Z3].setTicLength (majorl,minorl);
	sp->coordinates()->axes[Z4].setTicLength (majorl,minorl);
}

void Graph3D::updateTickLength(int axis,double majorLength, double minorLength)
{
	double majorl,minorl;
	switch(axis)
	{
		case 0:
			sp->coordinates()->axes[X1].ticLength (majorl,minorl);
			if (majorl != majorLength || minorl != minorLength)
			{
				sp->coordinates()->axes[X1].setTicLength (majorLength,minorLength);
				sp->coordinates()->axes[X2].setTicLength (majorLength,minorLength);
				sp->coordinates()->axes[X3].setTicLength (majorLength,minorLength);
				sp->coordinates()->axes[X4].setTicLength (majorLength,minorLength);
			}
			break;

		case 1:
			sp->coordinates()->axes[Y1].ticLength (majorl,minorl);
			if (majorl != majorLength || minorl != minorLength)
			{
				sp->coordinates()->axes[Y1].setTicLength (majorLength,minorLength);
				sp->coordinates()->axes[Y2].setTicLength (majorLength,minorLength);
				sp->coordinates()->axes[Y3].setTicLength (majorLength,minorLength);
				sp->coordinates()->axes[Y4].setTicLength (majorLength,minorLength);
			}
			break;

		case 2:
			sp->coordinates()->axes[Z1].ticLength (majorl,minorl);
			if (majorl != majorLength || minorl != minorLength)
			{
				sp->coordinates()->axes[Z1].setTicLength (majorLength,minorLength);
				sp->coordinates()->axes[Z2].setTicLength (majorLength,minorLength);
				sp->coordinates()->axes[Z3].setTicLength (majorLength,minorLength);
				sp->coordinates()->axes[Z4].setTicLength (majorLength,minorLength);
			}
			break;
	}
	sp->updateGL();
}

void Graph3D::rotationChanged(double, double, double)
{
	emit modified();
}

void Graph3D::scaleChanged(double, double, double)
{
	emit modified();
}

void Graph3D::shiftChanged(double, double, double)
{
	emit modified();
}

void Graph3D::zoomChanged(double)
{
	emit modified();
}

void Graph3D::resetAxesLabels()
{
	sp->coordinates()->axes[X1].setLabelString(labels[0]);
	sp->coordinates()->axes[X2].setLabelString(labels[0]);
	sp->coordinates()->axes[X3].setLabelString(labels[0]);
	sp->coordinates()->axes[X4].setLabelString(labels[0]);

	sp->coordinates()->axes[Y1].setLabelString(labels[1]);
	sp->coordinates()->axes[Y2].setLabelString(labels[1]);
	sp->coordinates()->axes[Y3].setLabelString(labels[1]);
	sp->coordinates()->axes[Y4].setLabelString(labels[1]);

	sp->coordinates()->axes[Z1].setLabelString(labels[2]); 
	sp->coordinates()->axes[Z2].setLabelString(labels[2]); 
	sp->coordinates()->axes[Z3].setLabelString(labels[2]); 
	sp->coordinates()->axes[Z4].setLabelString(labels[2]); 
}

void Graph3D::setAxesLabels(const QStringList& l)
{
	QString label= l[0];
	sp->coordinates()->axes[X1].setLabelString(label);
	sp->coordinates()->axes[X2].setLabelString(label);
	sp->coordinates()->axes[X3].setLabelString(label);
	sp->coordinates()->axes[X4].setLabelString(label);

	label= l[1];
	sp->coordinates()->axes[Y1].setLabelString(label);
	sp->coordinates()->axes[Y2].setLabelString(label);
	sp->coordinates()->axes[Y3].setLabelString(label);
	sp->coordinates()->axes[Y4].setLabelString(label);

	label= l[2];
	sp->coordinates()->axes[Z1].setLabelString(label);
	sp->coordinates()->axes[Z2].setLabelString(label);
	sp->coordinates()->axes[Z3].setLabelString(label);
	sp->coordinates()->axes[Z4].setLabelString(label);

	labels=l;
}

void Graph3D::updateLabel(int axis,const QString& label, const QFont& f)
{
	switch(axis)
	{
		case 0:
			if (labels[0] != label)
			{
				sp->coordinates()->axes[X1].setLabelString(label);
				sp->coordinates()->axes[X2].setLabelString(label);
				sp->coordinates()->axes[X3].setLabelString(label);
				sp->coordinates()->axes[X4].setLabelString(label);
				labels[0]=label;
			}
			if (sp->coordinates()->axes[X1].labelFont() != f)
			{
				sp->coordinates()->axes[X1].setLabelFont (f);
				sp->coordinates()->axes[X2].setLabelFont (f);
				sp->coordinates()->axes[X3].setLabelFont (f);
				sp->coordinates()->axes[X4].setLabelFont (f);
			}
			break;

		case 1:
			if (labels[1] != label)
			{
				sp->coordinates()->axes[Y1].setLabelString(label);
				sp->coordinates()->axes[Y2].setLabelString(label);
				sp->coordinates()->axes[Y3].setLabelString(label);
				sp->coordinates()->axes[Y4].setLabelString(label);

				labels[1]=label;
			}
			if (sp->coordinates()->axes[Y1].labelFont() != f)
			{
				sp->coordinates()->axes[Y1].setLabelFont (f);
				sp->coordinates()->axes[Y2].setLabelFont (f);
				sp->coordinates()->axes[Y3].setLabelFont (f);
				sp->coordinates()->axes[Y4].setLabelFont (f);
			}
			break;

		case 2:
			if (labels[2] != label)
			{
				sp->coordinates()->axes[Z1].setLabelString(label);
				sp->coordinates()->axes[Z2].setLabelString(label);
				sp->coordinates()->axes[Z3].setLabelString(label);
				sp->coordinates()->axes[Z4].setLabelString(label);
				labels[2]=label;
			}
			if (sp->coordinates()->axes[Z1].labelFont() != f)
			{
				sp->coordinates()->axes[Z1].setLabelFont (f);
				sp->coordinates()->axes[Z2].setLabelFont (f);
				sp->coordinates()->axes[Z3].setLabelFont (f);
				sp->coordinates()->axes[Z4].setLabelFont (f);
			}
			break;
	}

	sp->makeCurrent();
	sp->updateGL();
	emit modified();
}

QFont Graph3D::xAxisLabelFont()
{
	return sp->coordinates()->axes[X1].labelFont();
}

QFont Graph3D::yAxisLabelFont()
{
	return sp->coordinates()->axes[Y1].labelFont();
}

QFont Graph3D::zAxisLabelFont()
{
	return sp->coordinates()->axes[Z1].labelFont();
}

double Graph3D::xStart()
{
	double start,stop;
	sp->coordinates()->axes[X1].limits (start,stop);
	return start;
}

double Graph3D::xStop()
{
	double start,stop;
	sp->coordinates()->axes[X1].limits (start,stop);
	return stop;
}

double Graph3D::yStart()
{
	double start,stop;
	sp->coordinates()->axes[Y1].limits (start,stop);
	return start;
}

double Graph3D::yStop()
{
	double start,stop;
	sp->coordinates()->axes[Y1].limits (start,stop);
	return stop;
}

double Graph3D::zStart()
{
	double start,stop;
	sp->coordinates()->axes[Z1].limits (start,stop);
	return start;
}

double Graph3D::zStop()
{
	double start,stop;
	sp->coordinates()->axes[Z1].limits(start, stop);
	return stop;
}

QStringList Graph3D::scaleLimits()
{
	QStringList limits;
	double start,stop;
	int majors,minors;

	sp->coordinates()->axes[X1].limits (start,stop);
	majors=sp->coordinates()->axes[X1].majors();
	minors=sp->coordinates()->axes[X1].minors();

	limits<<QString::number(start);
	limits<<QString::number(stop);
	limits<<QString::number(majors);
	limits<<QString::number(minors);
	limits<<QString::number(scaleType[0]);

	sp->coordinates()->axes[Y1].limits (start,stop);
	majors=sp->coordinates()->axes[Y1].majors();
	minors=sp->coordinates()->axes[Y1].minors();

	limits<<QString::number(start);
	limits<<QString::number(stop);
	limits<<QString::number(majors);
	limits<<QString::number(minors);
	limits<<QString::number(scaleType[1]);

	sp->coordinates()->axes[Z1].limits (start,stop);
	majors=sp->coordinates()->axes[Z1].majors();
	minors=sp->coordinates()->axes[Z1].minors();

	limits<<QString::number(start);
	limits<<QString::number(stop);
	limits<<QString::number(majors);
	limits<<QString::number(minors);
	limits<<QString::number(scaleType[2]);

	return limits;
}

QStringList Graph3D::scaleTicks()
{
	QStringList limits;
	int majors,minors;

	majors=sp->coordinates()->axes[X1].majors();
	minors=sp->coordinates()->axes[X1].minors();
	limits<<QString::number(majors);
	limits<<QString::number(minors);

	majors=sp->coordinates()->axes[Y1].majors();
	minors=sp->coordinates()->axes[Y1].minors();
	limits<<QString::number(majors);
	limits<<QString::number(minors);

	majors=sp->coordinates()->axes[Z1].majors();
	minors=sp->coordinates()->axes[Z1].minors();
	limits<<QString::number(majors);
	limits<<QString::number(minors);

	return limits;
}

void Graph3D::updateScale(int axis,const QStringList& options)
{
	QString st=QString::number(scaleType[axis]);
	double start,stop;
	double xl,xr,yl,yr;
	int majors,minors, newMaj, newMin;

	sp->makeCurrent();
	switch(axis)
	{
		case 0:
			sp->coordinates()->axes[X1].limits(xl,xr);	
			if (xl !=options[0].toDouble() || xr != options[1].toDouble())
			{
				xl=options[0].toDouble();
				xr=options[1].toDouble();
				sp->coordinates()->axes[Y1].limits(yl,yr);

				if (func)
				{
					func->setDomain(xl,xr,yl,yr);
					func->create ();
				}
				else
				{
					sp->coordinates()->axes[Z1].limits(start,stop);
					updateScales(xl, xr, yl, yr, start, stop);
				}
				sp->coordinates()->axes[X1].setLimits(xl,xr);
			}

			if(st != options[4])
			{
				if (options[4]=="0")
				{
					sp->coordinates()->axes[X1].setScale (LINEARSCALE);
					scaleType[axis]=0;
				}
				else
				{
					sp->coordinates()->axes[X1].setScale (LOG10SCALE);
					scaleType[axis]=1;
				}
			}

			majors=sp->coordinates()->axes[X1].majors ();
			newMaj= options[2].toInt();
			if (majors != newMaj)
			{
				sp->coordinates()->axes[X1].setMajors(newMaj);
				sp->coordinates()->axes[X2].setMajors(newMaj);
				sp->coordinates()->axes[X3].setMajors(newMaj);
				sp->coordinates()->axes[X4].setMajors(newMaj);
			}

			minors=sp->coordinates()->axes[X1].minors ();
			newMin= options[3].toInt();
			if (minors != newMin)
			{
				sp->coordinates()->axes[X1].setMinors(newMin);
				sp->coordinates()->axes[X2].setMinors(newMin);
				sp->coordinates()->axes[X3].setMinors(newMin);
				sp->coordinates()->axes[X4].setMinors(newMin);
			}
			break;

		case 1:
			sp->coordinates()->axes[Y1].limits(yl,yr);
			if (yl != options[0].toDouble() || yr != options[1].toDouble())
			{
				yl=options[0].toDouble();
				yr=options[1].toDouble();
				sp->coordinates()->axes[X1].limits(xl,xr);

				if (func)
				{
					func->setDomain(xl,xr,yl,yr);
					func->create ();
				}
				else
				{
					sp->coordinates()->axes[Z1].limits(start,stop);
					updateScales(xl, xr, yl, yr,start,stop);
				}
				sp->coordinates()->axes[Y1].setLimits(yl,yr);
			}

			majors=sp->coordinates()->axes[Y1].majors ();
			newMaj= options[2].toInt();
			if (majors != newMaj )
			{
				sp->coordinates()->axes[Y1].setMajors(newMaj);
				sp->coordinates()->axes[Y2].setMajors(newMaj);
				sp->coordinates()->axes[Y3].setMajors(newMaj);
				sp->coordinates()->axes[Y4].setMajors(newMaj);
			}

			newMin = options[3].toInt();
			minors=sp->coordinates()->axes[Y1].minors ();
			if (minors != newMin)
			{
				sp->coordinates()->axes[Y1].setMinors(newMin);
				sp->coordinates()->axes[Y2].setMinors(newMin);
				sp->coordinates()->axes[Y3].setMinors(newMin);
				sp->coordinates()->axes[Y4].setMinors(newMin);
			}

			if(st != options[4])
			{
				if (options[4]=="0")
				{
					sp->coordinates()->axes[Y1].setScale (LINEARSCALE);
					scaleType[axis]=0;
				}
				else
				{
					sp->coordinates()->axes[Y1].setScale (LOG10SCALE);
					scaleType[axis]=1;
				}
			}
			break;

		case 2:
			if(st != options[4])
			{
				if (options[4]=="0")
				{
					sp->coordinates()->axes[Z1].setScale (LINEARSCALE);
					scaleType[axis]=0;
				}
				else
				{
					sp->coordinates()->axes[Z1].setScale (LOG10SCALE);
					scaleType[axis]=1;
				}
			}

			sp->coordinates()->axes[Z1].limits(start,stop);

			if (start != options[0].toDouble() || stop != options[1].toDouble())
			{
				start=options[0].toDouble();
				stop=options[1].toDouble();

				if (func)
				{
					func->setMinZ(start);
					func->setMaxZ(stop);
					func->create ();
				}
				else
				{
					sp->coordinates()->axes[X1].limits(xl,xr);
					sp->coordinates()->axes[Y1].limits(yl,yr);
					updateScales(xl, xr, yl, yr, start, stop);
				}
				sp->coordinates()->axes[Z1].setLimits(start,stop);
				sp->legend()->setLimits(start,stop);
			}

			majors=sp->coordinates()->axes[Z1].majors();
			newMaj= options[2].toInt();
			if (majors != newMaj )
			{
				sp->coordinates()->axes[Z1].setMajors(newMaj);
				sp->coordinates()->axes[Z2].setMajors(newMaj);
				sp->coordinates()->axes[Z3].setMajors(newMaj);
				sp->coordinates()->axes[Z4].setMajors(newMaj);
			}

			minors=sp->coordinates()->axes[Z1].minors();
			newMin = options[3].toInt();
			if (minors != newMin)
			{
				sp->coordinates()->axes[Z1].setMinors(newMin);
				sp->coordinates()->axes[Z2].setMinors(newMin);
				sp->coordinates()->axes[Z3].setMinors(newMin);
				sp->coordinates()->axes[Z4].setMinors(newMin);
			}
			break;
	}

	update();
	emit modified();
}

void Graph3D::updateScales(double xl, double xr, double yl, double yr, double zl, double zr)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	if (matrix_)
		updateScalesFromMatrix(xl, xr, yl, yr, zl, zr);
	else
	{
		QString name= plotAssociation;

		int pos=name.find("_",0);
		int posX=name.find("(",pos);
		QString xColName=name.mid(pos+1,posX-pos-1);	
		int xCol=worksheet->colIndex(xColName);

		pos=name.find(",",posX);
		posX=name.find("(",pos);
		QString yColName=name.mid(pos+1,posX-pos-1);		
		int yCol=worksheet->colIndex(yColName);

		if (name.endsWith("(Z)",true))
		{
			pos=name.find(",",posX);
			posX=name.find("(",pos);
			QString zColName=name.mid(pos+1,posX-pos-1);
			int zCol=worksheet->colIndex(zColName);

			updateScales(xl, xr, yl, yr, zl, zr, xCol, yCol, zCol);
		}
		else if (name.endsWith("(Y)",true))
			updateScales(xl, xr, yl, yr, zl, zr, xCol, yCol);
	}

	QApplication::restoreOverrideCursor();
}

void Graph3D::updateScalesFromMatrix(double xl, double xr, double yl, 
		double yr, double zl, double zr)
{
	double dx = (matrix_->xEnd() - matrix_->xStart())/(matrix_->numCols()-1);
	double dy = (matrix_->yEnd() - matrix_->yStart())/(matrix_->numRows()-1);

	int nc = int((xr - xl)/dx)+1;
	int nr = int((yr - yl)/dy)+1;

	int start_row = int((yl - matrix_->yStart())/dy);
	int start_col = int((xl - matrix_->xStart())/dx);

	double** data_matrix = matrix(0, nc-1, 0, nr-1);
	for (int j = 0; j < nr; j++) 
	{
		for (int i = 0; i < nc; i++) 
		{  
			double val = matrix_->text(j + start_row, i + start_col).toDouble();	
			if (val > zr)				
				data_matrix[i][j] = zr;
			else if (val < zl)				
				data_matrix[i][j] = zl;
			else
				data_matrix[i][j] = val;
		}
	} 
	sp->loadFromData(data_matrix, nc, nr, xl, xr, yl, yr);
	free_matrix(data_matrix, 0, nc-1, 0, nr-1);

	sp->legend()->setLimits(zl, zr);
	sp->legend()->setMajors(legendMajorTicks);
	update();
}

void Graph3D::updateScales(double xl, double xr, double yl, double yr,double zl, double zr,
		int xcol, int ycol)
{
	int r=worksheet->tableRows();
	int i, j, xmesh=0, ymesh=2;
	double xv, yv;

	for (i = 0; i < r; i++) 
	{
		if (!worksheet->text(i,xcol).isEmpty() && !worksheet->text(i,ycol).isEmpty())
		{
			xv=worksheet->text(i,xcol).toDouble();
			if (xv >= xl && xv <= xr)
				xmesh++;
		}
	}

	if (xmesh == 0)
		xmesh++;

	double** data =matrix(0,xmesh-1,0,ymesh-1);

	for ( j = 0; j < ymesh; j++) 
	{  
		int k=0;		
		for ( i = 0; i < r; i++) 
		{
			if (!worksheet->text(i,xcol).isEmpty() && !worksheet->text(i,ycol).isEmpty())
			{
				xv=worksheet->text(i,xcol).toDouble();	
				if (xv >= xl && xv <= xr)
				{	
					yv=worksheet->text(i,ycol).toDouble();
					if (yv > zr)				
						data[k][j] = zr;
					else if (yv < zl)				
						data[k][j] = zl;
					else
						data[k][j] = yv;
					k++;
				}
			}
		}
	}   

	sp->loadFromData(data, xmesh, ymesh, xl, xr, yl, yr);
	free_matrix(data,0,xmesh-1,0,ymesh-1);
}

void Graph3D::updateScales(double xl, double xr, double yl, double yr, double zl, double zr,
		int xCol, int yCol, int zCol)
{
	int r=worksheet->tableRows();
	int i,j,columns=0;	
	double xv, yv, zv;
	for ( i = 0; i < r; i++) 
	{
		if (!worksheet->text(i,xCol).isEmpty() && !worksheet->text(i,yCol).isEmpty() && !worksheet->text(i,zCol).isEmpty())
		{
			xv=worksheet->text(i,xCol).toDouble();
			yv=worksheet->text(i,yCol).toDouble();
			if (xv >= xl && xv <= xr && yv >= yl && yv <= yr)				
				columns++;
		}
	}

	if (columns == 0)
		columns++;

	Qwt3D::Triple **data=allocateData(columns,columns);
	for ( j = 0; j < columns; j++) 
	{   
		int k=0;
		for ( i = 0; i < r; i++) 
		{
			if (!worksheet->text(i,xCol).isEmpty() && !worksheet->text(i,yCol).isEmpty() && !worksheet->text(i,zCol).isEmpty())
			{
				xv=worksheet->text(i,xCol).toDouble();
				yv=worksheet->text(i,yCol).toDouble();
				if (xv >= xl && xv <= xr && yv >= yl && yv <= yr )
				{
					zv=worksheet->text(i,zCol).toDouble();
					if (zv > zr)				
						data[k][j] = Triple(xv,yv,zr);
					else if (zv < zl)				
						data[k][j] = Triple(xv,yv,zl);
					else
						data[k][j] = Triple(xv,yv,zv);
					k++;
				}
			}
		}
	}   
	sp->loadFromData (data, columns, columns, false,false);
	deleteData(data,columns);
}

void Graph3D::setTicks(const QStringList& options)
{
	int min,maj;
	if (int(options.count()) == 6)
	{
		maj=options[0].toInt();
		sp->coordinates()->axes[X1].setMajors(maj);
		sp->coordinates()->axes[X2].setMajors(maj);
		sp->coordinates()->axes[X3].setMajors(maj);
		sp->coordinates()->axes[X4].setMajors(maj);

		min=options[1].toInt();		
		sp->coordinates()->axes[X1].setMinors(min);
		sp->coordinates()->axes[X2].setMinors(min);
		sp->coordinates()->axes[X3].setMinors(min);
		sp->coordinates()->axes[X4].setMinors(min);

		maj=options[2].toInt();
		sp->coordinates()->axes[Y1].setMajors(maj);
		sp->coordinates()->axes[Y2].setMajors(maj);
		sp->coordinates()->axes[Y3].setMajors(maj);
		sp->coordinates()->axes[Y4].setMajors(maj);

		min=options[3].toInt();		
		sp->coordinates()->axes[Y1].setMinors(min);
		sp->coordinates()->axes[Y2].setMinors(min);
		sp->coordinates()->axes[Y3].setMinors(min);
		sp->coordinates()->axes[Y4].setMinors(min);

		maj=options[4].toInt();
		sp->coordinates()->axes[Z1].setMajors(maj);
		sp->coordinates()->axes[Z2].setMajors(maj);
		sp->coordinates()->axes[Z3].setMajors(maj);
		sp->coordinates()->axes[Z4].setMajors(maj);

		min=options[5].toInt();	
		sp->coordinates()->axes[Z1].setMinors(min);
		sp->coordinates()->axes[Z2].setMinors(min);
		sp->coordinates()->axes[Z3].setMinors(min);
		sp->coordinates()->axes[Z4].setMinors(min);
	}
	else
	{
		maj=options[1].toInt();
		sp->coordinates()->axes[X1].setMajors(maj);
		sp->coordinates()->axes[X2].setMajors(maj);
		sp->coordinates()->axes[X3].setMajors(maj);
		sp->coordinates()->axes[X4].setMajors(maj);

		min=options[2].toInt();		
		sp->coordinates()->axes[X1].setMinors(min);
		sp->coordinates()->axes[X2].setMinors(min);
		sp->coordinates()->axes[X3].setMinors(min);
		sp->coordinates()->axes[X4].setMinors(min);

		maj=options[3].toInt();
		sp->coordinates()->axes[Y1].setMajors(maj);
		sp->coordinates()->axes[Y2].setMajors(maj);
		sp->coordinates()->axes[Y3].setMajors(maj);
		sp->coordinates()->axes[Y4].setMajors(maj);

		min=options[4].toInt();		
		sp->coordinates()->axes[Y1].setMinors(min);
		sp->coordinates()->axes[Y2].setMinors(min);
		sp->coordinates()->axes[Y3].setMinors(min);
		sp->coordinates()->axes[Y4].setMinors(min);

		maj=options[5].toInt();
		sp->coordinates()->axes[Z1].setMajors(maj);
		sp->coordinates()->axes[Z2].setMajors(maj);
		sp->coordinates()->axes[Z3].setMajors(maj);
		sp->coordinates()->axes[Z4].setMajors(maj);

		min=options[6].toInt();	
		sp->coordinates()->axes[Z1].setMinors(min);
		sp->coordinates()->axes[Z2].setMinors(min);
		sp->coordinates()->axes[Z3].setMinors(min);
		sp->coordinates()->axes[Z4].setMinors(min);
	}
}

void Graph3D::setColors(const QColor& meshColor,const QColor& axesColor,const QColor& numColor,
		const QColor& labelColor,const QColor& bgColor,const QColor& gridColor)
{
	if (meshCol != meshColor)
	{
		sp->setMeshColor(Qt2GL(meshColor));
		meshCol=meshColor;
	}

	if(axesCol != axesColor)
	{
		sp->coordinates()->setAxesColor(Qt2GL(axesColor));
		axesCol=axesColor;
	}

	if(numCol !=numColor)
	{
		sp->coordinates()->setNumberColor(Qt2GL(numColor));
		numCol=numColor;
	}

	if(labelsCol !=labelColor)
	{
		sp->coordinates()->setLabelColor(Qt2GL(labelColor));
		labelsCol=labelColor;
	}

	if(bgCol !=bgColor)
	{
		sp->setBackgroundColor(Qt2GL(bgColor));
		bgCol=bgColor;
	}

	if(gridCol !=gridColor)
	{
		sp->coordinates()->setGridLinesColor(Qt2GL(gridColor));
		gridCol=gridColor;
	}
}

void Graph3D::setColors(const QStringList& colors)
{
	meshCol=QColor(colors[1]);
	sp->setMeshColor(Qt2GL(meshCol));

	axesCol=QColor(colors[2]);
	sp->coordinates()->setAxesColor(Qt2GL(axesCol));

	numCol=QColor(colors[3]);
	sp->coordinates()->setNumberColor(Qt2GL(numCol));

	labelsCol=QColor(colors[4]);
	sp->coordinates()->setLabelColor(Qt2GL(labelsCol));

	bgCol=QColor(colors[5]);
	sp->setBackgroundColor(Qt2GL(bgCol));

	gridCol=QColor(colors[6]);
	sp->coordinates()->setGridLinesColor(Qt2GL(gridCol));

	if ((int)colors.count()>7)
	{
		QColor min=QColor(colors[7]);
		QColor max=QColor(colors[8]);
		alpha = colors[9].toDouble();
		setDataColors(min,max);
	}
}

void Graph3D::updateColors(const QColor& meshColor,const QColor& axesColor,const QColor& numColor,
		const QColor& labelColor,const QColor& bgColor,const QColor& gridColor)
{
	if (meshCol != meshColor)
	{
		sp->setMeshColor(Qt2GL(meshColor));
		meshCol=meshColor;
	}

	if(axesCol != axesColor)
	{
		sp->coordinates()->setAxesColor(Qt2GL(axesColor));
		axesCol=axesColor;
	}

	if(numCol !=numColor)
	{
		sp->coordinates()->setNumberColor(Qt2GL(numColor));
		numCol=numColor;
	}

	if(labelsCol !=labelColor)
	{
		sp->coordinates()->setLabelColor(Qt2GL(labelColor));
		labelsCol=labelColor;
	}

	if(bgCol !=bgColor)
	{
		sp->setBackgroundColor(Qt2GL(bgColor));
		bgCol=bgColor;
	}

	if(gridCol !=gridColor)
	{
		sp->coordinates()->setGridLinesColor(Qt2GL(gridColor));
		gridCol=gridColor;
	}

	sp->updateData();
	sp->updateGL();
	emit modified();
}

void Graph3D::scaleFonts(double factor)
{
	QFont font = sp->coordinates()->axes[X1].numberFont();
	font.setPointSizeFloat(font.pointSizeFloat()*factor);
	sp->coordinates()->setNumberFont (font);

	titleFnt.setPointSizeFloat(factor*titleFnt.pointSizeFloat());
	sp->setTitleFont(titleFnt.family(),titleFnt.pointSize(),titleFnt.weight(),titleFnt.italic());

	font = xAxisLabelFont();
	font.setPointSizeFloat(factor*font.pointSizeFloat());
	setXAxisLabelFont(font);

	font = yAxisLabelFont();
	font.setPointSizeFloat(factor*font.pointSizeFloat());
	setYAxisLabelFont(font);

	font = zAxisLabelFont();
	font.setPointSizeFloat(factor*font.pointSizeFloat());
	setZAxisLabelFont(font);
}

void Graph3D::resizeEvent ( QResizeEvent *e)
{
	QSize size=e->size();

	sp->makeCurrent();
	sp->resize(size);

	if (!ignoreFonts && this->isVisible())
	{
		QSize oldSize=e->oldSize();
		double ratio=(double)size.height()/(double)oldSize.height();
		scaleFonts(ratio);
	}

	sp->updateGL();
	emit resizedWindow(this);
	emit modified();
}

void Graph3D::contextMenuEvent(QContextMenuEvent *e)
{
	emit showContextMenu();
	e->accept();
}

void Graph3D::setFramed()
{
	if (sp->coordinates()->style() == FRAME)
		return;

	sp->makeCurrent();
	sp->setCoordinateStyle(FRAME);
}

void Graph3D::setBoxed()
{
	if (sp->coordinates()->style() == BOX)
		return;

	sp->makeCurrent();
	sp->setCoordinateStyle(BOX);
}

void Graph3D::setNoAxes()
{
	if (sp->coordinates()->style() == NOCOORD)
		return;

	sp->makeCurrent();
	sp->setCoordinateStyle(NOCOORD);
}

void Graph3D::setNoGrid()
{
	if (sp->plotStyle() == FILLED)
		return;

	sp->makeCurrent();
	sp->setPlotStyle(FILLED);
	sp->updateData();
	sp->updateGL();

	style_=FILLED;
	pointStyle = None;
}

void Graph3D::setFilledMesh()
{
	if (sp->plotStyle() == FILLEDMESH)
		return;

	sp->makeCurrent();
	sp->setPlotStyle(FILLEDMESH);
	sp->updateData();
	sp->updateGL();

	style_=FILLEDMESH;
	pointStyle = None;
}

void Graph3D::setHiddenLineGrid()
{
	if (sp->plotStyle() == HIDDENLINE)
		return;

	sp->makeCurrent();
	sp->setPlotStyle(HIDDENLINE);
	sp->showColorLegend(false);
	sp->updateData();
	sp->updateGL();

	style_=HIDDENLINE;
	pointStyle = None;
	legendOn=false;
}

void Graph3D::setLineGrid()
{
	if (sp->plotStyle() == WIREFRAME)
		return;

	sp->makeCurrent();
	sp->setPlotStyle(WIREFRAME);
	sp->showColorLegend(false);
	sp->updateData();
	sp->updateGL();

	pointStyle = None;
	style_=WIREFRAME;
	legendOn=false;
}

void Graph3D::setPointsMesh()
{
	if (!sp  || pointStyle == Dots)
		return;

	pointStyle=Dots;
	style_=Qwt3D::USER;

	sp->makeCurrent();
	sp->setPlotStyle(Dot(pointSize, smooth));
	sp->updateData();
	sp->updateGL();
}

void Graph3D::setConesMesh()
{
	if (!sp  || pointStyle == Cones )
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	pointStyle=Cones;
	style_=Qwt3D::USER;

	sp->makeCurrent();
	sp->setPlotStyle(Cone3D(conesRad,conesQuality));
	sp->updateData();
	sp->updateGL();

	QApplication::restoreOverrideCursor();
}

void Graph3D::setCrossMesh()
{
	if (!sp || pointStyle == HairCross)
		return;

	pointStyle=HairCross;
	style_=Qwt3D::USER;

	sp->makeCurrent();
	sp->setPlotStyle(CrossHair(crossHairRad, crossHairLineWidth,crossHairSmooth,crossHairBoxed));
	sp->updateData();
	sp->updateGL();
}

void Graph3D::clearData()
{
	if (matrix_) 
		matrix_ = 0;
	else if (worksheet) 
		worksheet = 0;
	else if (func)
	{
		delete func;
		func = 0;
	}
	plotAssociation = QString();

	sp->makeCurrent();
	sp->loadFromData (0, 0, 0, false,false);
	sp->updateData();
	sp->updateGL();
}

void Graph3D::setBarsPlot()
{
	if (pointStyle == VerticalBars)
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	pointStyle=VerticalBars;
	style_=Qwt3D::USER;

	sp->makeCurrent();
	sp->setPlotStyle(Bar(barsRad));
	sp->updateData();
	sp->updateGL();
	QApplication::restoreOverrideCursor();
}

void Graph3D::setFloorData()
{
	if (sp->floorStyle() == FLOORDATA)
		return;

	sp->makeCurrent();
	sp->setFloorStyle(FLOORDATA);
	sp->updateData();
	sp->updateGL();
}

void Graph3D::setFloorIsolines()
{
	if (sp->floorStyle() == FLOORISO)
		return;

	sp->makeCurrent();
	sp->setFloorStyle(FLOORISO);
	sp->updateData();
	sp->updateGL();
}

void Graph3D::setEmptyFloor()
{
	if (sp->floorStyle() == NOFLOOR)
		return;

	sp->makeCurrent();
	sp->setFloorStyle(NOFLOOR);
	sp->updateData();
	sp->updateGL();
}

void Graph3D::setMeshLineWidth(int lw)
{
	if ((int)sp->meshLineWidth() == lw)
		return;

	sp->makeCurrent();
	sp->setMeshLineWidth((double)lw);
	sp->updateData();
	sp->updateGL();
}

int Graph3D::grids()
{
	return sp->coordinates()->grids();
}

void Graph3D::setGrid(Qwt3D::SIDE s, bool b)
{
	if (!sp)
		return;

	int sum = sp->coordinates()->grids();

	if (b)
		sum |= s;
	else
		sum &= ~s;

	sp->coordinates()->setGridLines(sum!=Qwt3D::NOSIDEGRID, false, sum);
	sp->updateGL();
	emit modified();
}

void Graph3D::setGrid(int grids)
{
	if (!sp)
		return;

	sp->coordinates()->setGridLines(true, false,grids);
}

void Graph3D::setLeftGrid(bool b)
{
	setGrid(Qwt3D::LEFT,b);
}
void Graph3D::setRightGrid(bool b)
{
	setGrid(Qwt3D::RIGHT,b);
}
void Graph3D::setCeilGrid(bool b)
{
	setGrid(Qwt3D::CEIL,b);
}
void Graph3D::setFloorGrid(bool b)
{
	setGrid(Qwt3D::FLOOR,b);
}
void Graph3D::setFrontGrid(bool b)
{
	setGrid(Qwt3D::FRONT,b);
}
void Graph3D::setBackGrid(bool b)
{
	setGrid(Qwt3D::BACK,b);
}

void Graph3D::print()
{
	QPrinter printer;
	printer.setOrientation(QPrinter::Landscape);
	printer.setColorMode (QPrinter::Color);
	printer.setFullPage(false);

	if (printer.setup())
	{	
		if (IO::save (sp,"qtiplot.png","PNG"))
		{
			QPixmap p;
			p.load ("qtiplot.png","PNG", QPixmap::Color );

			QPainter paint(&printer);
			paint.drawPixmap(QPoint(0,0),p);
			paint.end();

			QFile f("qtiplot.png");
			f.remove();
		}
		else
			QMessageBox::about(0,tr("QtiPlot - IO Error"), 
					tr("Could not print: <h4>" + QString(name()) + "</h4>."));
	}
}

void Graph3D::copyImage()
{
	bool ok=IO::save (sp,"qtiplot.png","PNG");  
	if (ok)
	{
		QPixmap p;
		p.load ("qtiplot.png","PNG", QPixmap::Auto );		
		QImage image= p.convertToImage();		
		QApplication::clipboard()->setData( new Q3ImageDrag (image,sp,0) );
		QFile f("qtiplot.png");
		f.remove();
	}
	else
		QMessageBox::about(0,tr("QtiPlot - IO Error"), 
				tr("Could not copy: <h4>" + QString(name()) + "</h4>."));
}

void Graph3D::saveImage()
{
	sp->makeCurrent();
	int i;	
	QStringList list= IO::outputFormatList();
	list.sort();
	//QString filter="*.eps;;",selectedFilter,aux;

	QString filter="",selectedFilter,aux;

	for (i=0;i<(int)list.count();i++)
	{
		aux="*."+(list[i]).lower()+";;";
		filter+=aux;
	}

	QString fname = Q3FileDialog::getSaveFileName( QString(),filter,0,"file dialog",
			tr("Choose a filename to save under"),&selectedFilter,true);
	if ( !fname.isEmpty() ) 
	{ 	
		QFileInfo fi(fname);
		QString baseName = fi.fileName();
		QString filetype=selectedFilter.remove("*");

		if (baseName.contains(".")==0)
			fname.append(filetype);		

		if ( QFile::exists(fname) &&
				QMessageBox::question(
					0,
					tr("QtiPlot - Overwrite File?"),
					tr("A file called: <p><b>%1</b><p>already exists. "
						"Do you want to overwrite it?")
					.arg(fname),
					tr("&Yes"), tr("&No"),
					QString(), 0, 1 ) )
			return ;
		else
		{
			QFile f(fname);
			if ( !f.open( QIODevice::WriteOnly ) ) 
			{
				QMessageBox::critical(0, tr("QtiPlot - Export Error"),tr("Could not write to file: <br><h4>"+fname+ "</h4><p>Please verify that you have the right to write to this location!").arg(fname));
				return;
			}
			IO::save(sp, fname, filetype.upper().remove("."));
		}		
	} 
}

void Graph3D::saveImageToFile(const QString& fname, const QString& format)
{
	IO::save(sp, fname, format);
}

bool Graph3D::eventFilter(QObject *object, QEvent *e)
{
	if ( object != (QObject *)this->sp)
		return false;

	switch(e->type())
	{
		case QEvent::MouseButtonDblClick:
			{
				emit showOptionsDialog();			
				return true; 
			}

		default:
			;
			break;
	}
	return QObject::eventFilter(object, e);
}

void Graph3D::setPointOptions(double size, bool s)
{
	if (pointSize == size && smooth == s)
		return;

	pointSize = size;
	smooth = s;
}

double Graph3D::barsRadius()
{
	if (sp->plotStyle() == Qwt3D::USER && sp->plotStyle() != Qwt3D::POINTS)
		return barsRad;
	else 
		return 0.0;
}

void Graph3D::setBarsRadius(double rad)
{
	if (barsRad == rad)
		return;

	barsRad = rad;
}

void Graph3D::updateBars(double rad)
{
	if (barsRad == rad)
		return;

	barsRad = rad;
	sp->setPlotStyle(Bar(barsRad));
	update();
}

void Graph3D::updatePoints(double size, bool sm)
{
	if (pointStyle == Dots && pointSize == size && smooth == sm)
		return;

	pointSize = size;
	smooth = sm;
	pointStyle = Dots;

	Dot d(pointSize, smooth);
	sp->setPlotStyle(d);

	update();
	emit modified();
	emit custom3DActions(this);
}

void Graph3D::updateCones(double rad, int quality)
{
	if (pointStyle == Cones && conesRad == rad && conesQuality == quality)
		return;

	conesRad = rad;
	conesQuality = quality;
	pointStyle = Cones;
	sp->setPlotStyle(Cone3D(conesRad,conesQuality));
	update();
	emit modified();
	emit custom3DActions(this);
}

void Graph3D::setConesOptions(double rad, int quality)
{
	conesRad = rad;
	conesQuality = quality;
}

void Graph3D::updateCross(double rad, double linewidth, bool smooth, bool boxed)
{
	if (pointStyle == HairCross && crossHairRad == rad && 
			crossHairSmooth == smooth && crossHairBoxed == boxed && 
			crossHairLineWidth == linewidth)
		return;

	crossHairRad = rad;
	crossHairLineWidth=linewidth;
	crossHairSmooth = smooth;
	crossHairBoxed = boxed;
	pointStyle = HairCross;

	sp->setPlotStyle(CrossHair(rad,linewidth, smooth, boxed));
	update();
	emit modified();
	emit custom3DActions(this);
}

void Graph3D::setCrossOptions(double rad, double linewidth, bool smooth, bool boxed)
{
	crossHairRad = rad ;
	crossHairLineWidth=linewidth;
	crossHairSmooth = smooth;
	crossHairBoxed = boxed;
}

void Graph3D::setStyle(Qwt3D::COORDSTYLE coord,Qwt3D::FLOORSTYLE floor,
		Qwt3D::PLOTSTYLE plot, Graph3D::PointStyle point)
{
	sp->setCoordinateStyle(coord);
	sp->setFloorStyle(floor);

	if (point == None)
		sp->setPlotStyle(plot);
	else if (point == VerticalBars)
		sp->setPlotStyle(Bar(barsRad));
	else if (point == Dots)
		sp->setPlotStyle(Dot(pointSize, smooth));
	else if (point == HairCross)
		sp->setPlotStyle(CrossHair(crossHairRad, crossHairLineWidth, crossHairSmooth, crossHairBoxed));
	else if (point == Cones)
		sp->setPlotStyle(Cone3D(conesRad, conesQuality));

	pointStyle=point;
	style_=sp->plotStyle() ;
}

void Graph3D::customPlotStyle(int style)
{
	sp->makeCurrent();
	if (sp->plotStyle() == style)
		return;

	switch (style)
	{
		case WIREFRAME  :
			{
				sp->setPlotStyle(WIREFRAME  );
				style_= WIREFRAME ;
				pointStyle = None;

				legendOn = false;
				sp->showColorLegend(legendOn);
				break;
			}

		case FILLED :
			{
				sp->setPlotStyle(FILLED );
				style_= FILLED;
				pointStyle = None;
				break;
			}

		case FILLEDMESH  :
			{
				sp->setPlotStyle(FILLEDMESH);
				style_= FILLEDMESH;
				pointStyle = None;
				break;
			}

		case HIDDENLINE:
			{
				sp->setPlotStyle(HIDDENLINE);
				style_= HIDDENLINE;
				pointStyle = None;
				legendOn = false;
				sp->showColorLegend(legendOn);
				break;
			}

		case Qwt3D::POINTS:
			{
				pointSize = 0.5;
				smooth = true;
				pointStyle=Dots;
				style_ = Qwt3D::USER;

				Dot d(pointSize, smooth);
				sp->setPlotStyle(d);
				break;
			}

		case Qwt3D::USER:
			{
				pointStyle = VerticalBars;
				style_ = Qwt3D::USER;
				sp->setPlotStyle(Bar(barsRad));
				break;
			}
	}

	sp->updateData();
	sp->updateGL();
}

void Graph3D::setStyle(const QStringList& st)
{
	if (st[1] =="nocoord")
		sp->setCoordinateStyle(NOCOORD);
	else if (st[1] =="frame")
		sp->setCoordinateStyle(FRAME);
	else if (st[1] =="box")
		sp->setCoordinateStyle(BOX);

	if (st[2] =="nofloor")
		sp->setFloorStyle(NOFLOOR);
	else if (st[2] =="flooriso")
		sp->setFloorStyle(FLOORISO);
	else if (st[2] =="floordata")
		sp->setFloorStyle(FLOORDATA);

	if (st[3] =="filledmesh")
		sp->setPlotStyle(FILLEDMESH);
	else if (st[3] =="filled")
		sp->setPlotStyle(FILLED);
	else if (st[3] =="points")
	{
		pointSize = st[4].toDouble();

		smooth=false;
		if (st[5] == "1")	
			smooth=true;

		sp->setPlotStyle(Dot(pointSize, smooth));
		pointStyle = Dots;
	}
	else if (st[3] =="wireframe")
		sp->setPlotStyle(WIREFRAME);
	else if (st[3] =="hiddenline")
		sp->setPlotStyle(HIDDENLINE);
	else if (st[3] =="bars")
	{
		barsRad = (st[4]).toDouble();
		sp->setPlotStyle(Bar(barsRad));
		pointStyle = VerticalBars;
	}
	else if (st[3] =="cones")
	{
		conesRad = (st[4]).toDouble();
		conesQuality = (st[5]).toInt();		

		sp->setPlotStyle(Cone3D(conesRad, conesQuality));
		pointStyle = Cones;
	}
	else if (st[3] =="cross")
	{
		crossHairRad = (st[4]).toDouble();
		crossHairLineWidth = (st[5]).toDouble();

		crossHairSmooth=false;
		if (st[6] == "1")	
			crossHairSmooth=true;

		crossHairBoxed=false;
		if (st[7] == "1")	
			crossHairBoxed=true;

		sp->setPlotStyle(CrossHair(crossHairRad, crossHairLineWidth, crossHairSmooth, crossHairBoxed));
		pointStyle = HairCross;
	}

	style_ = sp->plotStyle() ;
}

void Graph3D::setRotation(double  xVal,double  yVal,double  zVal)
{
	sp->setRotation(xVal,yVal,zVal);
}

void Graph3D::setScale(double  xVal,double  yVal,double  zVal)
{
	sp->setScale(xVal,yVal,zVal);
}

void Graph3D::setShift(double  xVal,double  yVal,double  zVal)
{
	sp->setShift(xVal,yVal,zVal);
}

void Graph3D::setZoom(double  val)  
{
	sp->setZoom(val);
}

void Graph3D::updateZoom(double  val)  
{
	if (sp->zoom() == val)
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	sp->makeCurrent();
	sp->setZoom(val);
	sp->updateData();
	sp->updateGL();

	QApplication::restoreOverrideCursor();
}

void Graph3D::updateScaling(double  xVal,double  yVal,double  zVal)
{
	if (sp->xScale() == xVal && sp->yScale() == yVal && sp->zScale() == zVal)
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	sp->setScale(xVal,yVal,zVal);
	sp->updateData();
	sp->updateGL();

	QApplication::restoreOverrideCursor();
}

Qwt3D::PLOTSTYLE Graph3D::plotStyle()
{
	return sp->plotStyle();
}

Qwt3D::FLOORSTYLE Graph3D::floorStyle()
{
	return sp->floorStyle();
}

Qwt3D::COORDSTYLE Graph3D::coordStyle()
{
	return sp->coordinates()->style();
}

QString Graph3D::formula()
{
	if (func)
		return func->function();
	else 
		return plotAssociation;
}

QString Graph3D::saveToString(const QString& geometry)
{
	QString s="<SurfacePlot>\n";
	s+= QString(name())+"\t";
	s+= birthDate() + "\n";		
	s+= geometry;
	s+= "SurfaceFunction\t";

	sp->makeCurrent();	
	if (func)
		s+=func->function()+"\t";
	else
	{
		s+= plotAssociation;
		s+="\t";
	}

	double start,stop;
	sp->coordinates()->axes[X1].limits(start,stop);
	s+=QString::number(start)+"\t";
	s+=QString::number(stop)+"\t";
	sp->coordinates()->axes[Y1].limits(start,stop);
	s+=QString::number(start)+"\t";
	s+=QString::number(stop)+"\t";
	sp->coordinates()->axes[Z1].limits(start,stop);
	s+=QString::number(start)+"\t";
	s+=QString::number(stop)+"\n";

	QString st;	
	if (sp->coordinates()->style() == Qwt3D::NOCOORD)
		st="nocoord";	
	else if (sp->coordinates()->style() == Qwt3D::BOX)
		st="box";	
	else 
		st="frame";
	s+="Style\t"+st+"\t";

	switch(sp->floorStyle ())
	{
		case NOFLOOR:
			st="nofloor";
			break;

		case FLOORISO:
			st="flooriso";
			break;

		case FLOORDATA:
			st="floordata";
			break;
	}
	s+=st+"\t";

	switch(sp->plotStyle())
	{
		case USER:
			if (pointStyle == VerticalBars)
				st="bars\t"+QString::number(barsRad);
			else if (pointStyle == Dots)
			{
				st="points\t"+QString::number(pointSize);
				st+="\t"+QString::number(smooth);
			}
			else if (pointStyle == Cones)
			{
				st="cones\t"+QString::number(conesRad);
				st+="\t"+QString::number(conesQuality);
			}
			else if (pointStyle == HairCross)
			{
				st="cross\t"+QString::number(crossHairRad);
				st+="\t"+QString::number(crossHairLineWidth);
				st+="\t"+QString::number(crossHairSmooth);
				st+="\t"+QString::number(crossHairBoxed);
			}
			break;

		case WIREFRAME:
			st="wireframe";
			break;

		case HIDDENLINE:
			st="hiddenline";
			break;

		case FILLED:
			st="filled";
			break;

		case FILLEDMESH:
			st="filledmesh";
			break;

		default:
			;
	}
	s+=st+"\n";

	s+="grids\t";
	s+=QString::number(sp->coordinates()->grids())+"\n";

	s+="title\t";
	s+=title+"\t";
	s+=titleCol.name()+"\t";
	s+=titleFnt.family()+"\t";
	s+=QString::number(titleFnt.pointSize())+"\t";
	s+=QString::number(titleFnt.weight())+"\t";
	s+=QString::number(titleFnt.italic())+"\n";

	s+="colors\t";
	s+=meshCol.name()+"\t";
	s+=axesCol.name()+"\t";
	s+=numCol.name()+"\t";
	s+=labelsCol.name()+"\t";
	s+=bgCol.name()+"\t";
	s+=gridCol.name()+"\t";
	s+=fromColor.name()+"\t";
	s+=toColor.name()+"\t";
	s+=QString::number(alpha)+"\n";

	s+="axesLabels\t";
	s+=labels.join("\t")+"\n";

	s+="tics\t";
	QStringList tl=scaleTicks();
	s+=tl.join("\t")+"\n";

	s+="tickLengths\t";
	tl=axisTickLengths();
	s+=tl.join("\t")+"\n";

	s+="options\t";
	s+=QString::number(legendOn)+"\t";
	s+=QString::number(sp->resolution())+"\t";
	s+=QString::number(labelsDist)+"\n";

	s+="numbersFont\t";
	QFont fnt=sp->coordinates()->axes[X1].numberFont();
	s+=fnt.family()+"\t";
	s+=QString::number(fnt.pointSize())+"\t";
	s+=QString::number(fnt.weight())+"\t";
	s+=QString::number(fnt.italic())+"\n";

	s+="xAxisLabelFont\t";
	fnt=sp->coordinates()->axes[X1].labelFont();
	s+=fnt.family()+"\t";
	s+=QString::number(fnt.pointSize())+"\t";
	s+=QString::number(fnt.weight())+"\t";
	s+=QString::number(fnt.italic())+"\n";

	s+="yAxisLabelFont\t";
	fnt=sp->coordinates()->axes[Y1].labelFont();
	s+=fnt.family()+"\t";
	s+=QString::number(fnt.pointSize())+"\t";
	s+=QString::number(fnt.weight())+"\t";
	s+=QString::number(fnt.italic())+"\n";

	s+="zAxisLabelFont\t";
	fnt=sp->coordinates()->axes[Z1].labelFont();
	s+=fnt.family()+"\t";
	s+=QString::number(fnt.pointSize())+"\t";
	s+=QString::number(fnt.weight())+"\t";
	s+=QString::number(fnt.italic())+"\n";

	s+="rotation\t";
	s+=QString::number(sp->xRotation())+"\t";
	s+=QString::number(sp->yRotation())+"\t";
	s+=QString::number(sp->zRotation())+"\n";

	s+="zoom\t";
	s+=QString::number(sp->zoom())+"\n";

	s+="scaling\t";
	s+=QString::number(sp->xScale())+"\t";
	s+=QString::number(sp->yScale())+"\t";
	s+=QString::number(sp->zScale())+"\n";

	s+="shift\t";
	s+=QString::number(sp->xShift())+"\t";
	s+=QString::number(sp->yShift())+"\t";
	s+=QString::number(sp->zShift())+"\n";

	s+="LineWidth\t";
	s+=QString::number(sp->meshLineWidth())+"\n";
	s+="WindowLabel\t" + windowLabel() + "\t" + QString::number(captionPolicy()) + "\n";
	s+="</SurfacePlot>\n";
	return s;
}

void Graph3D::showColorLegend(bool show)
{
	if (legendOn == show)
		return;

	sp->makeCurrent();
	sp->showColorLegend(show);

	legendOn=show;
	sp->updateGL();
	emit modified();
}

void Graph3D::setResolution(int r)
{
	if (sp->resolution() == r)
		return;

	sp->makeCurrent();
	sp->setResolution(r);
	sp->updateData();
	sp->updateGL();
	emit modified();
}

void Graph3D::setTitle(const QStringList& list)
{
	title=list[1];
	sp->setTitle(title);

	titleCol=QColor(list[2]);
	sp->setTitleColor(Qt2GL(titleCol));

	titleFnt=QFont(list[3],list[4].toInt(),list[5].toInt(),list[6].toInt());
	sp->setTitleFont(titleFnt.family(),titleFnt.pointSize(),titleFnt.weight(),titleFnt.italic());
}

void Graph3D::setTitle(const QString& s,const QColor& color,const QFont& font)
{
	if (title != s)
	{
		title=s;
		sp->setTitle(title);
	}

	titleCol=color;
	sp->setTitleColor(Qt2GL(color));

	if (titleFnt != font)
	{
		titleFnt=font;
		sp->setTitleFont(font.family(),font.pointSize(),font.weight(),font.italic());
	}
}

void Graph3D::updateTitle(const QString& s,const QColor& color,const QFont& font)
{
	if (title != s)
	{
		title=s;
		sp->setTitle(title);
	}

	titleCol=color;
	sp->setTitleColor(Qt2GL(color));

	if (titleFnt != font)
	{
		titleFnt=font;
		sp->setTitleFont(font.family(),font.pointSize(),font.weight(),font.italic());
	}

	sp->makeCurrent();
	sp->updateGL();
	emit modified();
}

void Graph3D::setTitleFont(const QFont& font)
{
	if (titleFnt != font)
	{
		titleFnt=font;
		sp->setTitleFont(font.family(),font.pointSize(),font.weight(),font.italic());
	}
}

void Graph3D::setOptions(const QStringList& list)
{
	legendOn=false;
	if (list[1].toInt() == 1)
		legendOn=true;
	sp->showColorLegend(legendOn);
	sp->setResolution(list[2].toInt());
	adjustLabels(list[3].toInt());
}


void Graph3D::setOptions(bool legend, int r, int dist)
{
	sp->showColorLegend(legend);
	legendOn=legend;
	sp->setResolution(r);
	adjustLabels(dist);
}

Qwt3D::Triple** Graph3D::allocateData(int columns, int rows)
{
	Qwt3D::Triple** data = new Qwt3D::Triple* [columns];

	for ( int i = 0; i < columns; ++i) 
	{
		data[i] = new Qwt3D::Triple [rows];
	}
	return data;
}

void Graph3D::deleteData(Qwt3D::Triple **data, int columns)
{
	for ( int i = 0; i < columns; i++) 
	{
		delete [] data[i];
	}
	delete [] data;
}

QColor Graph3D::minDataColor()
{	
	return fromColor;
}

QColor Graph3D::maxDataColor()
{	
	return toColor;
}

void Graph3D::setDataColors(const QColor& cMin, const QColor& cMax)
{	
	if (cMin == fromColor && cMax == toColor)
		return;

	fromColor=cMin;
	toColor=cMax;

	Qwt3D::ColorVector cv;

	int size=255;
	double dsize = size;

	double r1=cMax.red()/dsize; 
	double r2=cMin.red()/dsize;

	double stepR = (r1-r2)/dsize;

	double g1=cMax.green()/dsize;
	double g2=cMin.green()/dsize;

	double stepG = (g1-g2)/dsize;

	double b1=cMax.blue()/dsize;
	double b2=cMin.blue()/dsize;

	double stepB = (b1-b2)/dsize;

	RGBA rgb;
	for (int i=0; i<size; i++)
	{  
		rgb.r = r1-i*stepR; 
		rgb.g = g1-i*stepG;
		rgb.b = b1-i*stepB;
		rgb.a = alpha;

		cv.push_back(rgb);	
	}

	col_ = new StandardColor(sp);
	col_->setColorVector(cv);
	sp->setDataColor(col_);

	if (legendOn)
	{
		sp->showColorLegend(false);
		sp->showColorLegend(legendOn);
	}
}

void Graph3D::changeTransparency(double t)
{	
	if (alpha == t)
		return;

	alpha = t;
	sp->makeCurrent();

	Qwt3D::StandardColor* color=(StandardColor*) sp->dataColor ();
	color->setAlpha(t);

	if (legendOn)
	{
		sp->showColorLegend(false);
		sp->showColorLegend(legendOn);
	}

	sp->updateData();
	sp->updateGL();
	emit modified();
}

void Graph3D::setTransparency(double t)
{	
	if (alpha == t)
		return;

	alpha = t;

	Qwt3D::StandardColor* color=(StandardColor*) sp->dataColor ();
	color->setAlpha(t);
}

void Graph3D::showWorksheet()
{
	if (worksheet)
		worksheet->showMaximized();
	else if (matrix_)
		matrix_->showMaximized();
}

void Graph3D::setSmoothMesh(bool smooth)
{
	if (smoothMesh == smooth)
		return;

	smoothMesh = smooth;
	sp->setSmoothMesh(smoothMesh);
	sp->coordinates()->setLineSmooth(smoothMesh);
	sp->updateData();
	sp->updateGL();
}

QString Graph3D::saveAsTemplate(const QString& geometryInfo) 
{
	QString s = saveToString(geometryInfo);
	QStringList lst = QStringList::split("\n", s, false);
	QStringList l = QStringList::split("\t", lst[3], true);
	l[1] = QString();
	lst[3] = l.join("\t");
	return lst.join("\n");
}

Graph3D::~Graph3D()      
{
	if (func)
		delete func;

	delete sp;
}

