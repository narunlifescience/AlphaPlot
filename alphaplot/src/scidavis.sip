/* vim: set filetype=cpp : */
/* due to a limitation of sip, this file has to use Unix style line endings */
/***************************************************************************
	File                 : scidavis.sip
	Project              : SciDAVis
	Description          : Specifications for Python bindings
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2009 Knut Franke (knut.franke*gmx.de)
    Copyright            : (C) 2008-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2006-2007 Ion Vasilief (ion_vasilief*yahoo.fr)
                           (replace * with @ in the email addresses) 

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

%Module scidavis
%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip

class AbstractAspect : QObject
{
%TypeHeaderCode
#include "core/AbstractAspect.h"
%End
	public:
/*
		AbstractAspect(const QString &name);
		AbstractAspect * parentAspect() const;
//		future::Folder * folder();
		bool isDescendantOf(AbstractAspect *other);
		void addChild(AbstractAspect* child /Transfer/);
		void insertChild(AbstractAspect *child /Transfer/, int index);
		void removeChild(AbstractAspect* child, bool detach=false);
		void removeChild(int index);
		AbstractAspect* child(int index) const;
		int childCount() const;
		int indexOfChild(const AbstractAspect * child) const;
		*/
		int index() const;
		/*
		void moveChild(int from, int to);
		void reparentChild(AbstractAspect *new_parent, AbstractAspect *child, int d_new_index);
		void reparentChild(AbstractAspect *new_parent, AbstractAspect *child);
		QList<AbstractAspect *> descendantsThatInherit(const char *class_name);
		void removeAllChildAspects();
//		const Project *project() const;
//		Project *project();
		QString path() const;
		*/
		QIcon icon() const;
		/*
		QMenu *createContextMenu() const;
		*/
		QString name() const;
		QString comment() const;
		QString captionSpec() const;
		QDateTime creationTime() const;
		QString caption() const;
		/*
		QUndoStack *undoStack() const;
		void exec(QUndoCommand *command);
		void beginMacro(const QString& text);
		void endMacro();
		static QVariant global(const QString &key);
		static void setGlobal(const QString &key, const QVariant &value);
		static void setGlobalDefault(const QString &key, const QVariant &value);
//		void save(QXmlStreamWriter *) const;
//		bool load(XmlStreamReader *);
		*/
	public slots:
		void setName(const QString &value);
		void setComment(const QString &value);
		void setCaptionSpec(const QString &value);
		void remove();
		/*
		QString uniqueNameFor(const QString &current_name) const;
		*/
	signals:
		void aspectDescriptionAboutToChange(const AbstractAspect *aspect);
		void aspectDescriptionChanged(const AbstractAspect *aspect);
		void aspectAboutToBeAdded(const AbstractAspect *parent, int index);
		void aspectAdded(const AbstractAspect *aspect);
		void aspectAdded(const AbstractAspect *parent, int index);
		void aspectAboutToBeRemoved(const AbstractAspect *aspect);
		void aspectAboutToBeRemoved(const AbstractAspect *parent, int index);
		void aspectRemoved(const AbstractAspect *parent, int index);
		void statusInfo(const QString &text);
	private:
		AbstractAspect(const AbstractAspect&);
};

class Column: AbstractAspect
{
%TypeHeaderCode
#include "globals.h"
#include "core/column/Column.h"
#include "table/future_Table.h"
%End
	public:
/* will be removed in favour of columMode() - don't encourage anyone to start using it
		SIP_PYOBJECT dataType() const;
%MethodCode
		int mode = sipCpp->dataType();
  		sipRes = PyString_FromString(SciDAVis::enumValueToString(mode, "ColumnDataType"));
%End
*/
		SIP_PYOBJECT columnMode() const;
%MethodCode
		int mode = sipCpp->columnMode();
  		sipRes = PyString_FromString(SciDAVis::enumValueToString(mode, "ColumnMode"));
%End
		void setColumnMode(QString mode);
%MethodCode
		int mode_code = SciDAVis::enumStringToValue(*a0, "ColumnMode");
		if(mode_code != -1)
			sipCpp->setColumnMode((SciDAVis::ColumnMode)mode_code);
%End
		bool copy(const Column * other);
		bool copy(const Column * source, int source_start, int dest_start, int num_rows);
		int rowCount() const;
		void insertRows(int before, int count);
		void removeRows(int first, int count);
		SIP_PYOBJECT plotDesignation() const;
%MethodCode
		int pd = sipCpp->plotDesignation();
  		sipRes = PyString_FromString(SciDAVis::enumValueToString(pd, "PlotDesignation"));
%End
		void setPlotDesignation(QString pd);
%MethodCode
		int mode_code = SciDAVis::enumStringToValue(*a0, "PlotDesignation");
		if(mode_code != -1)
			sipCpp->setPlotDesignation((SciDAVis::PlotDesignation)mode_code);
%End
		void clear();
		bool isInvalid(int row) const;
//		bool isMasked(int row) const;
		void clearValidity();
//		void clearMasks();
		void setInvalid(int row, bool invalid = true);
//		void setMasked(int row, bool mask = true);
		QString formula(int row) const;
		void setFormula(int row, QString formula);
		void clearFormulas();
		QString textAt(int row) const;
		void setTextAt(int row, const QString& new_value);
		void replaceTexts(int first, const QStringList& new_values);
		QDate dateAt(int row) const;
		void setDateAt(int row, const QDate& new_value);
		QTime timeAt(int row) const;
		void setTimeAt(int row, const QTime& new_value);
		QDateTime dateTimeAt(int row) const;
		void setDateTimeAt(int row, const QDateTime& new_value);
		void replaceDateTimes(int first, const QList<QDateTime>& new_values);
		double valueAt(int row) const;
		void setValueAt(int row, double new_value);
		void replaceValues(int first, const QVector<double>& new_values);

  Column * x() const;
%MethodCode
    future::Table *t = qobject_cast<future::Table *>(sipCpp->parentAspect());
    if (t)
        sipRes = t->column(t->colX(t->indexOfChild(sipCpp)));
%End

  Column * y() const;
%MethodCode
    future::Table *t = qobject_cast<future::Table *>(sipCpp->parentAspect());
    if (t)
        sipRes = t->column(t->colY(t->indexOfChild(sipCpp)));
%End


private:
  Column(const Column&);
};

class ScriptEdit: QTextEdit
{
%TypeHeaderCode
#include "src/ScriptEdit.h"
%End
public slots:
  void execute();
  void executeAll();
  void evaluate();
  void print();
  void insertFunction(const QString &);
private:
  ScriptEdit(ScriptEdit&);
};

class MyWidget: QWidget /PyName=MDIWindow/
{
%TypeHeaderCode
#include "src/MyWidget.h"
#include "src/ApplicationWindow.h"
%End
public:
  enum CaptionPolicy{Name = 0, Label = 1, Both = 2};

  SIP_PYOBJECT windowLabel();
%MethodCode
  sipRes = PyString_FromString(sipCpp->windowLabel());
%End
  void setWindowLabel(const QString&);

  QString name();
  void setName(const QString& s);
%MethodCode
	ApplicationWindow *app = sipscidavis_app();
	if (app) app->renameWindow(sipCpp, *a0);
%End

	MyWidget *clone();
%MethodCode
	ApplicationWindow *app = sipscidavis_app();
	if (app) sipRes = app->clone(sipCpp);
%End

  CaptionPolicy captionPolicy();
  void setCaptionPolicy(CaptionPolicy);

  // not usable as is - doesn't yield a valid template file,
  // geometry info has to be supplied by user, doesn't work on graphs (where it's most
  // interesting)...
  /*
  virtual SIP_PYOBJECT saveAsTemplate(const QString& );
%MethodCode
  sipRes = PyString_FromString(sipCpp->saveAsTemplate(*a0));
%End
*/
  // same issues as with saveAsTemplate
  /*
  virtual void restore(const QStringList& );
  */
  Folder* folder();

  void askOnCloseEvent(bool)/PyName=confirmClose/;

private:
  MyWidget(const MyWidget&);
};



class Table: MyWidget
{
%TypeHeaderCode
#include "src/Table.h"

#define CHECK_TABLE_COL(arg)\
    int col;\
    if (PyInt_Check(arg)) {\
      col = (int)PyInt_AsLong(arg) - 1;\
		if (col < 0 || col >= sipCpp->numCols()) {\
			sipIsErr = 1;\
			PyErr_Format(PyExc_ValueError, "There's no column %d in table %s!", col+1, sipCpp->name().toAscii().constData());\
		}\
	 } else {\
      PyObject *tmp = PyObject_Str(arg);\
		if (!tmp) {\
			sipIsErr = 1;\
			PyErr_Format(PyExc_TypeError, "Column argument must be either int or string.");\
		} else {\
			col = sipCpp->colNames().indexOf(PyString_AsString(tmp));\
			if (col < 0) {\
				sipIsErr = 1;\
				PyErr_Format(PyExc_ValueError, "There's no column named %s in table %s!", PyString_AsString(tmp),\
						sipCpp->name().toAscii().constData());\
				Py_DECREF(tmp);\
			}\
		}\
	 }
#define CHECK_TABLE_ROW(arg)\
   int row = arg-1;\
	if (row < 0 || row >= sipCpp->numRows()) {\
		sipIsErr = 1;\
		PyErr_Format(PyExc_ValueError, "There's no row %d in table %s!", row+1, sipCpp->name().toAscii().constData());\
	}

%End

public:
  int numRows();
  int numCols();
  int rowCount();
  int columnCount();
  void setNumRows(int);
  void setNumCols(int);
  Column * column(int index) const;
  Column * column(const QString & name) const;
%MethodCode
  sipRes = sipCpp->d_future_table->column(*a0, false);
%End

  Column * __getitem__(int index) const;
%MethodCode
  sipRes = sipCpp->column(a0);
%End

  Column * __getitem__(const QString &name) const;
%MethodCode
  sipRes = sipCpp->column(*a0);
%End

  SIP_PYOBJECT text(SIP_PYOBJECT, int) /Deprecated/;
%MethodCode
  sipIsErr = 0;
  CHECK_TABLE_COL(a0);
  CHECK_TABLE_ROW(a1);
  if (sipIsErr == 0) {
		PyObject *encstr = PyString_FromString(sipCpp->text(row, col).utf8());
		if (encstr) {
			sipRes = PyUnicode_FromEncodedObject(encstr, "utf8", 0);
			Py_DECREF(encstr);
		} else {
			sipRes = NULL;
			sipIsErr = 1;
		}
	}
%End
  double cell(SIP_PYOBJECT, int) /Deprecated/;
%MethodCode
  sipIsErr = 0;
  CHECK_TABLE_COL(a0);
  CHECK_TABLE_ROW(a1);
	if (sipIsErr == 0)
		sipRes = sipCpp->cell(row, col);
%End
  void setText(SIP_PYOBJECT, int, const QString&) /Deprecated/;
%MethodCode
  sipIsErr = 0;
  CHECK_TABLE_COL(a0);
  CHECK_TABLE_ROW(a1);
	if (sipIsErr == 0)
		sipCpp->setText(row, col, *a2);
%End
  void setCell(SIP_PYOBJECT, int, double) /Deprecated/;
%MethodCode
  sipIsErr = 0;
  CHECK_TABLE_COL(a0);
  CHECK_TABLE_ROW(a1);
	if (sipIsErr == 0)
		sipCpp->setCell(row, col, a2);
%End
  SIP_PYOBJECT colName(int) /Deprecated/;
%MethodCode
   if (a0 < 1 || a0 > sipCpp->numCols()) {\
		sipIsErr = 1;\
		PyErr_SetString(PyExc_ValueError, "Invalid column argument");\
	} else
		sipRes = PyString_FromString(sipCpp->colLabel(a0-1));
%End
  void setColName(SIP_PYOBJECT, const QString&) /Deprecated/;
%MethodCode
	sipIsErr = 0;
   CHECK_TABLE_COL(a0);
	if (sipIsErr == 0)
		sipCpp->setColName(col, *a1);
%End
  void notifyChanges() /Deprecated/;

  void importASCII(const QString&, const QString&="\t", int=0, bool=false, bool=true, bool=false, bool=false);
  bool exportASCII(const QString&, const QString&="\t", bool=false, bool=false);
  void normalize(SIP_PYOBJECT);
%MethodCode
	sipIsErr = 0;
	CHECK_TABLE_COL(a0);
	if (sipIsErr == 0)
		sipCpp->d_future_table->normalizeColumns(QList< Column* >() << sipCpp->column(col));
%End
  void normalize();
%MethodCode
	QList< Column* > cols;
	for (int i=0; i<sipCpp->numCols(); i++)
		cols << sipCpp->column(i);
	sipCpp->d_future_table->normalizeColumns(cols);
%End

  void sortColumn(SIP_PYOBJECT, int order = 0);
%MethodCode
  	sipIsErr = 0;
  	CHECK_TABLE_COL(a0);
	if (sipIsErr == 0)
		sipCpp->d_future_table->sortColumns(0, QList<Column*>() << sipCpp->column(col), a1==0);
%End
  void sort(int type = 0, int order = 0, const QString& leadCol = QString());
%MethodCode
	Column * lead;
	if (a0 == 0)
		lead = 0; // sort separately
	else
		lead = sipCpp->column(*a2);
	QList< Column* > cols;
	for (int i=0; i<sipCpp->numCols(); i++)
		cols << sipCpp->column(i);
	sipCpp->d_future_table->sortColumns(lead, cols, a1==0);
%End
  void sortColumns(SIP_PYTUPLE, int=0, int=0, const QString&=QString());
%MethodCode
	Column * lead;
	if (a1 == 0)
		lead = 0; // sort separately
	else
		lead = sipCpp->column(*a3);
	QList< Column* > cols;
  int n = PyTuple_Size(a0);
  for (int i=0; i<n; i++) {
	  PyObject *str = PyObject_Str(PyTuple_GET_ITEM(a0,i));
	  if (str) {
		  cols << sipCpp->column(PyString_AsString(str));
		  Py_DECREF(str);
	  } else {
		  sipIsErr = 1;
		  break;
	  }
  }
  sipCpp->d_future_table->sortColumns(lead, cols, a2==0);
%End
	void setCommand(SIP_PYOBJECT, const QString&) /Deprecated/;
%MethodCode
  	sipIsErr = 0;
  	CHECK_TABLE_COL(a0);
	if (sipIsErr == 0)
		sipCpp->setCommand(col, *a1);
%End

	void setComment(SIP_PYOBJECT, const QString&) /Deprecated/;
%MethodCode
  	sipIsErr = 0;
  	CHECK_TABLE_COL(a0);
	if (sipIsErr == 0)
		sipCpp->setColComment(col, *a1);
%End

private:
  Table(const Table&);
};

class Matrix: MyWidget
{
%TypeHeaderCode
#include "src/Matrix.h"
#define CHECK_MATRIX_COL(arg)\
	int col = arg-1;\
   if (col < 0 || col >= sipCpp->numCols()) {\
		sipIsErr = 1;\
		PyErr_Format(PyExc_ValueError, "There's no column %d in matrix %s!", col+1, sipCpp->name().toAscii().constData());\
	}
#define CHECK_MATRIX_ROW(arg)\
   int row = arg-1;\
	if (row < 0 || row >= sipCpp->numRows()) {\
		sipIsErr = 1;\
		PyErr_Format(PyExc_ValueError, "There's no row %d in matrix %s!", row+1, sipCpp->name().toAscii().constData());\
	}
%End
public:
  int numRows();
  void setNumRows(int);
  int numCols();
  void setNumCols(int);
  void setDimensions(int rows, int cols);

  SIP_PYOBJECT text(int, int);
%MethodCode
	sipIsErr = 0;
	CHECK_MATRIX_ROW(a0);
	CHECK_MATRIX_COL(a1);
	if (sipIsErr == 0)
		sipRes = PyString_FromString(sipCpp->text(row, col));
%End
  double cell(int, int);
%MethodCode
	sipIsErr = 0;
	CHECK_MATRIX_ROW(a0);
	CHECK_MATRIX_COL(a1);
	if (sipIsErr == 0)
		sipRes = sipCpp->cell(row, col);
%End
  void setText(int, int, const QString&);
%MethodCode
	sipIsErr = 0;
	CHECK_MATRIX_ROW(a0);
	CHECK_MATRIX_COL(a1);
	if (sipIsErr == 0)
		sipCpp->setText(row, col, *a2);
%End
  void setCell(int, int, double);
%MethodCode
	sipIsErr = 0;
	CHECK_MATRIX_ROW(a0);
	CHECK_MATRIX_COL(a1);
	if (sipIsErr == 0)
		sipCpp->setCell(row, col, a2);
%End

	QVector<qreal> column(int col, int first_row, int last_row);
%MethodCode
	sipRes = new QVector<qreal>(sipCpp->d_future_matrix->columnCells(a0, a1, a2));
%End
	void setColumn(int col, int first_row, const QVector<double> &values);
%MethodCode
	sipCpp->d_future_matrix->setColumnCells(a0, a1, a1+a2->size()-1, *a2);
%End
	QVector<qreal> row(int row, int first_column, int last_column);
%MethodCode
	sipRes = new QVector<qreal>(sipCpp->d_future_matrix->rowCells(a0, a1, a2));
%End
	void setRow(int row, int first_column, const QVector<double> & values);
%MethodCode
	sipCpp->d_future_matrix->setRowCells(a0, a1, a1+a2->size()-1, *a2);
%End

    double xStart();
	double xEnd();
	double yStart();
	double yEnd();
    void setCoordinates(double xs, double xe, double ys, double ye);

    void setFormula(const QString &s);
	 bool recalculate();

    void setNumericPrecision(int prec);

    void transpose();
	void invert();
	double determinant();

private:
  Matrix(const Matrix&);
};

class ArrowMarker // : QwtPlotMarker
{
%TypeHeaderCode
#include "src/ArrowMarker.h"
%End
public:

  ArrowMarker();

  void setStartPoint(double, double) /PyName=setStart/;
  void setEndPoint(double, double) /PyName=setEnd/;

  void setStyle(Qt::PenStyle);
  void setColor(const QColor&);
  void setWidth(int);
  void drawStartArrow(bool = true);
  void drawEndArrow(bool = true);
  void setHeadLength(int);
  void setHeadAngle(int);
  void fillArrowHead(bool = true);

private:
  ArrowMarker(const ArrowMarker&);
};

class ImageMarker // : QwtPlotMarker
{
%TypeHeaderCode
#include "src/ImageMarker.h"
%End
public:

  ImageMarker(const QString&);
  QString fileName();
  
  QSize size();
  void setSize(int, int);
  
  void setBoundingRect(double, double, double, double) /PyName=setCoordinates/;
  
private:
  ImageMarker(const ImageMarker&);
};

class Legend // : QwtPlotMarker
{
%TypeHeaderCode
#include "src/Legend.h"
%End
public:

  void setText(const QString&);
  void setTextColor(const QColor&);
  void setFrameStyle(int);
  void setBackgroundColor(const QColor&);
  void setFont(const QFont&);
  void setOriginCoord(double, double);

private:
  Legend(const Legend&);
};

class QwtSymbol
{
%TypeHeaderCode
#include <qwt_symbol.h>
#include "ColorBox.h"
%End
	public:
    enum Style 
    { 
        NoSymbol = -1, 

        Ellipse, 
        Rect, 
        Diamond, 
        Triangle, 
        DTriangle,
        UTriangle, 
        LTriangle, 
        RTriangle, 
        Cross, 
        XCross, 
        HLine, 
        VLine, 
        Star1, 
        Star2, 
        Hexagon, 

        StyleCnt 
    };

    QwtSymbol();
    QwtSymbol(Style st, const QBrush &bd, const QPen &pn, const QSize &s);

    virtual QwtSymbol *clone() const;

    void setSize(const QSize &s);
    void setSize(int a, int b = -1);
    void setBrush(const QBrush& b);
    void setPen(const QPen &p);
    void setStyle (Style s);

    const QBrush& brush() const;
    const QPen& pen() const;
    const QSize& size() const;
    Style style() const;

	 // convenience methods for scripting

	 void setColor(const QColor& color);
%MethodCode
	QPen pen = sipCpp->pen();
	pen.setColor(*a0);
	sipCpp->setPen(pen);
	QBrush brush = sipCpp->brush();
	brush.setColor(*a0);
	sipCpp->setBrush(brush);
%End
	 void setColor(int color);
%MethodCode
	QPen pen = sipCpp->pen();
	pen.setColor(ColorBox::color(a0));
	sipCpp->setPen(pen);
	QBrush brush = sipCpp->brush();
	brush.setColor(ColorBox::color(a0));
	sipCpp->setBrush(brush);
%End
	 void setOutlineColor(const QColor& color);
%MethodCode
	QPen pen = sipCpp->pen();
	pen.setColor(*a0);
	sipCpp->setPen(pen);
%End
	 void setOutlineColor(int color);
%MethodCode
	QPen pen = sipCpp->pen();
	pen.setColor(ColorBox::color(a0));
	sipCpp->setPen(pen);
%End
	 void setFillColor(const QColor& color);
%MethodCode
	QBrush brush = sipCpp->brush();
	brush.setColor(*a0);
	sipCpp->setBrush(brush);
%End
	 void setFillColor(int color);
%MethodCode
	QBrush brush = sipCpp->brush();
	brush.setColor(ColorBox::color(a0));
	sipCpp->setBrush(brush);
%End

	private:
QwtSymbol(const QwtSymbol&);
};

class QwtPlotCurve
{
%TypeHeaderCode
#include <qwt_plot_curve.h>
#include "Graph.h"
%End
	public:
		int dataSize() const;
		double x(int i) const;
		double y(int i) const;
		double minXValue() const;
		double maxXValue() const;
		double minYValue() const;
		double maxYValue() const;

		void setPen(const QPen &);
		const QPen &pen() const;

		void setBrush(const QBrush &);
		const QBrush &brush() const;

		void setSymbol(const QwtSymbol &s);
		const QwtSymbol& symbol() const;

	 // convenience methods for scripting
	 
	 void setColor(const QColor& color);
%MethodCode
	QPen pen = sipCpp->pen();
	pen.setColor(*a0);
	sipCpp->setPen(pen);
	QBrush brush = sipCpp->brush();
	brush.setColor(*a0);
	sipCpp->setBrush(brush);
%End
	 void setColor(int color);
%MethodCode
	QPen pen = sipCpp->pen();
	pen.setColor(ColorBox::color(a0));
	sipCpp->setPen(pen);
	QBrush brush = sipCpp->brush();
	brush.setColor(ColorBox::color(a0));
	sipCpp->setBrush(brush);
%End
	 void setOutlineColor(const QColor& color);
%MethodCode
	QPen pen = sipCpp->pen();
	pen.setColor(*a0);
	sipCpp->setPen(pen);
%End
	 void setOutlineColor(int color);
%MethodCode
	QPen pen = sipCpp->pen();
	pen.setColor(ColorBox::color(a0));
	sipCpp->setPen(pen);
%End
	 void setFillColor(const QColor& color);
%MethodCode
	QBrush brush = sipCpp->brush();
	brush.setColor(*a0);
	sipCpp->setBrush(brush);
%End
	 void setFillColor(int color);
%MethodCode
	QBrush brush = sipCpp->brush();
	brush.setColor(ColorBox::color(a0));
	sipCpp->setBrush(brush);
%End
	 void setFillStyle(Qt::BrushStyle style);
%MethodCode
	QBrush brush = sipCpp->brush();
	brush.setStyle(a0);
	sipCpp->setBrush(brush);
%End

	private:
		QwtPlotCurve(const QwtPlotCurve&);
};

class QwtPlot {
%TypeHeaderCode
#include <qwt_plot.h>
%End
	public:
  enum Axis 
  { 
  	yLeft,
  	yRight,
  	xBottom,
  	xTop,
  
  	axisCnt
  };
	private:
  QwtPlot(const QwtPlot&);
};

class Grid
{
%TypeHeaderCode
#include "src/Grid.h"
%End
	public:
		void enableX(bool) /PyName=setXMajor/;
		bool xEnabled() /PyName=xMajor/;

		void enableY(bool) /PyName=setYMajor/;
		bool yEnabled() /PyName=yMajor/;

		void enableXMin(bool) /PyName=setXMinor/;
		bool xMinEnabled() /PyName=xMinor/;

		void enableYMin(bool) /PyName=setYMinor/;
		bool yMinEnabled() /PyName=yMinor/;

		void enableZeroLineX(bool enable = true) /PyName=setXZeroLine/;
		bool xZeroLineEnabled() /PyName=xZeroLine/;

		void enableZeroLineY(bool enable = true) /PyName=setYZeroLine/;
		bool yZeroLineEnabled() /PyName=yZeroLine/;

		void setMajPenX(const QPen &p) /PyName=setXMajorPen/;
		const QPen& majPenX() const /PyName=xMajorPen/;

		void setMinPenX(const QPen &p) /PyName=setXMinorPen/;
		const QPen& minPenX() const /PyName=xMinorPen/;

		void setMajPenY(const QPen &p) /PyName=setYMajorPen/;
		const QPen& majPenY() const /PyName=yMajorPen/;

		void setMinPenY(const QPen &p) /PyName=setYMinorPen/;
		const QPen& minPenY() const /PyName=yMinorPen/;

		void setMajor(bool);
%MethodCode
	sipCpp->enableX(a0);
	sipCpp->enableY(a0);
%End

		void setMinor(bool);
%MethodCode
	sipCpp->enableXMin(a0);
	sipCpp->enableYMin(a0);
%End

		void setMajorPen(const QPen &p);
%MethodCode
	sipCpp->setMajPenX(*a0);
	sipCpp->setMajPenY(*a0);
%End
		void setMinorPen(const QPen &p);
%MethodCode
	sipCpp->setMinPenX(*a0);
	sipCpp->setMinPenY(*a0);
%End
	private:
		Grid(const Grid&);
};

class Graph : QWidget /PyName=Layer/
{
%TypeHeaderCode
#include "src/Graph.h"
#include "src/Legend.h"
#include "src/ColorBox.h"
#include "src/FunctionCurve.h"
#include "src/DataPointPicker.h"
#include <qwt_plot_canvas.h>
%End
public:
  bool isPiePlot();
  SIP_PYOBJECT pieLegendText() /PyName=pieLegend/;
%MethodCode
  sipRes = PyString_FromString(sipCpp->pieLegendText());
%End

  bool insertCurve(Table*, const QString&, int=1, int color=-1);
%MethodCode
	if (sipCpp->insertCurve(a0, *a1, a2)) {
		CurveLayout cl = sipCpp->initCurveLayout(a2, 1);
		if (a3 >= 0) {
			cl.lCol = a3;
			cl.symCol = a3;
			cl.fillCol = a3;
		}
		sipCpp->updateCurveLayout(sipCpp->curves()-1, &cl);
		sipRes = true;
	} else
		sipRes = false;
%End
  bool insertCurve(Table*, const QString&, const QString&, int=1, int color=-1);
%MethodCode
	if (sipCpp->insertCurve(a0, *a1, *a2, a3)) {
		CurveLayout cl = sipCpp->initCurveLayout(a3, 1);
		if (a4 >= 0) {
			cl.lCol = a4;
			cl.symCol = a4;
			cl.fillCol = a4;
		}
		sipCpp->updateCurveLayout(sipCpp->curves()-1, &cl);
		sipRes = true;
	} else
		sipRes = false;
%End
	bool insertFunctionCurve(const QString &formula, double from=0, double to=1, int points=100,
			const QString &title = QString::null);
%MethodCode
	ApplicationWindow *app = sipscidavis_app();
	if (app)
		sipRes = sipCpp->addFunctionCurve(app, FunctionCurve::Normal, QStringList() << *a0, "x",
				QList<double>() << a1 << a2, a3, *a4);
	else
		sipRes = false;
%End
	bool insertPolarCurve(const QString &radial, const QString &angular,
			double from=0, double to=2*M_PI, const QString &parameter="t", int points=100,
			const QString &title = QString::null);
%MethodCode
	ApplicationWindow *app = sipscidavis_app();
	if (app)
		sipRes = sipCpp->addFunctionCurve(app, FunctionCurve::Polar, QStringList() << *a0 << *a1,
				*a4, QList<double>() << a2 << a3, a5, *a6);
	else
		sipRes = false;
%End
	bool insertParametricCurve(const QString &x, const QString &y,
			double from=0, double to=1, const QString &parameter="t", int points=100,
			const QString &title =QString::null);
%MethodCode
	ApplicationWindow *app = sipscidavis_app();
	if (app)
		sipRes = sipCpp->addFunctionCurve(app, FunctionCurve::Parametric, QStringList() << *a0 << *a1,
				*a4, QList<double>() << a2 << a3, a5, *a6);
	else
		sipRes = false;
%End

  void removeCurve(int);
  void removeCurve(const QString&);
  void deleteFitCurves();
  int curves() /PyName=numCurves/;
  QList<QwtPlotCurve*> curves() const;
%MethodCode
	sipRes = new QList<QwtPlotCurve*>();
	for (int i = 0; i<sipCpp->curves(); i++)
		sipRes->append(sipCpp->curve(i));
%End
  void showCurve(int index, bool visible=true);

  QwtPlotCurve* curve(int index);
  QwtPlotCurve* curve(const QString &title);

  void addErrorBars(const QString&, Table *, const QString&,
		  int type = 1, int width = 1, int cap = 8, const QColor& color = QColor(Qt::black),
		  bool through = true, bool minus = true, bool plus = true);

  void addArrow(ArrowMarker*);

  ImageMarker* addImage(ImageMarker*);
  ImageMarker* addImage(const QString& fileName);
  
  void setTitle(const QString& t);
  Legend* newLegend();
  Legend* newLegend(const QString&);
  void setLegend(const QString&);
%MethodCode
  sipCpp->legend()->setText(*a0);
%End

  Legend* legend();

  void removeLegend();
  void addTimeStamp();

  void enableAxis(int axis, bool on = true);
  void setXAxisTitle(const QString& text) /PyName=setXTitle/;
  void setYAxisTitle(const QString& text) /PyName=setYTitle/;
  void setRightAxisTitle(const QString &text) /PyName=setRightTitle/;
  void setTopAxisTitle(const QString &text) /PyName=setTopTitle/;

  void setLabelsNumericFormat(int axis, int format, int = 6, const QString& = QString()) /PyName=setAxisNumericFormat/;
  void setScale(int axis, double start, double end, double step = 0.0,
                int majorTicks = 5, int minorTicks = 5, int type = 0, bool inverted = false);

  void setMargin(int);
  void setFrame(int width = 1, const QColor& color = QColor(Qt::black));
  void setBackgroundColor(const QColor& color);
  void setCanvasBackground(const QColor& color) /PyName=setCanvasColor/;

  void showGrid(int);
  void showGrid();
  Grid *grid();

  void replot();
  void print() /PyName=printDialog/;
  void exportImage(const QString& fileName, int quality = -1);
  void exportVector(const QString& fileName, int res = 0, bool color = true,
                    bool keepAspect = true, QPrinter::PageSize pageSize = QPrinter::Custom);
  void exportToFile(const QString& fileName) /PyName=export/;

  void enableAutoscaling(bool = true);
  void setIgnoreResizeEvents(bool = true)/PyName=setIgnoreResize/;
  void setAutoscaleFonts(bool = true);
  void setAntialiasing(bool on = true, bool update = true);

  QWidget * canvas() const;
%MethodCode
	sipRes = sipCpp->d_plot->canvas();
%End

	QPointF pickPoint() const;
%MethodCode
	ApplicationWindow *app = sipscidavis_app();
	sipRes = new QPointF();
	if (app)
		*sipRes = DataPointPicker(const_cast<Graph*>(sipCpp), app).pick();
%End

private:
  Graph(const Graph&);
};

class MultiLayer : MyWidget /PyName=Graph/
{
%TypeHeaderCode
#include "src/MultiLayer.h"
%End
public:
  Graph *activeGraph() /PyName=activeLayer/;
  void setActiveGraph(Graph*) /PyName=setActiveLayer/;
  int layers() /PyName=numLayers/;
  QWidgetList layerWidgets() const /PyName=layers/;
  Graph* layer(int num);
  Graph* addLayer(int = 0, int = 0, int = 0, int = 0);
%MethodCode
	sipRes = sipCpp->addLayer(a0, a1, a2, a3);
	ApplicationWindow *app = sipscidavis_app();
	if (app) app->setPreferences(sipRes);
%End

  void setCols(int);
  void setRows(int);
  void setSpacing (int, int);
  void setMargins (int, int, int, int);
  void setLayerCanvasSize (int, int);
%MethodCode
	sipCpp->setLayerCanvasSize(a0, a1);
	sipCpp->arrangeLayers(true, true);
%End
	// for backwards-compatibility
  void setAlignement (int, int);
  void setAlignement (int, int) /PyName=setAlignment/;
  void arrangeLayers(bool fit = true);
%MethodCode
	sipCpp->arrangeLayers(a0, false);
%End
	// for backwards-compatibility
  void arrangeLayers(bool fit, bool user_size);

  void exportToFile(const QString& fileName) /PyName=export/;
  void exportImage(const QString& fileName, int quality = -1);
  void exportVector(const QString& fileName, int res = 0, bool color = true,
                    bool keepAspect = true, QPrinter::PageSize pageSize = QPrinter::Custom);

  void print() /PyName=printDialog/;
private:
  MultiLayer(const MultiLayer&);
};

class Note: MyWidget
{
%TypeHeaderCode
#include "src/Note.h"
%End
public:
// TODO: Calling execute()/executeAll() from Python currently doesn't work reliably!
  void execute();
  void executeAll();
  bool autoexec() const;
  void setAutoexec(bool);
  QString text();
  void setText(const QString &s);
  QString exportASCII(const QString &file);
  QString importASCII(const QString &file);
private:
  Note(const Note&);
};

class ApplicationWindow: QMainWindow
{
%TypeHeaderCode
#include "src/ApplicationWindow.h"
#include "src/ColorBox.h"
#include <qwt_symbol.h>
%End
%ConvertToSubClassCode
// we have to do this to override casting in qt/qobject.sip
// without this, app.windows() returns Tables as QWidgets (strangely, Notes
// are returned correctly in any case)
sipClass = sipFindClass(sipCpp->className());
%End

public:
  Table* table(const QString&);
%MethodCode
  sipRes = sipCpp->current_folder->table(*a0, false);
  if(!sipRes)
    sipRes = sipCpp->projectFolder()->table(*a0, true);
%End
  Table* newTable();
  Table* newTable(const QString&, int=2, int=30);
%MethodCode
  sipRes = sipCpp->newTable(a2, a1, *a0);
%End
  Matrix* matrix(const QString&);
%MethodCode
  sipRes = sipCpp->current_folder->matrix(*a0, false);
  if(!sipRes)
    sipRes = sipCpp->projectFolder()->matrix(*a0, true);
%End
  Matrix* newMatrix();
  Matrix* newMatrix(const QString&, int=32, int=32);
  MultiLayer *plot(const QString&) /PyName=graph/;
%MethodCode
  sipRes = sipCpp->current_folder->graph(*a0, false);
  if(!sipRes)
    sipRes = sipCpp->projectFolder()->graph(*a0, true);
%End
  MultiLayer* newGraph();
  MultiLayer* newGraph(const QString &name);
%MethodCode
	// Code copied from ApplicationWindow::newGraph() and modified to actually use the requested
	// name if it is already unique.
	if (sipCpp->alreadyUsedName(*a0))
		sipRes = sipCpp->multilayerPlot(sipCpp->generateUniqueName(*a0));
	else
		sipRes = sipCpp->multilayerPlot(*a0);
	if (sipRes) {
		Graph *g = sipRes->addLayer();
		sipCpp->setPreferences(g);
		g->newLegend();
		g->setAutoscaleFonts(false);
		g->setIgnoreResizeEvents(false);
		sipRes->arrangeLayers(false, false);
		sipRes->adjustSize();
		g->setAutoscaleFonts(sipCpp->autoScaleFonts);//restore user defined fonts behaviour
		g->setIgnoreResizeEvents(!sipCpp->autoResizeLayers);
		sipCpp->customMenu(sipRes);
	}
%End
  Note *note(const QString&);
%MethodCode
  sipRes = sipCpp->current_folder->note(*a0, false);
  if(!sipRes)
    sipRes = sipCpp->projectFolder()->note(*a0, true);
%End
	// TODO: If name is already taken, this chooses a default name like "Notes1",
	// which is inconsistent with newTable, newMatrix and newGraph. Not changing in
	// a bugfix release though, since an unexpected change in behaviour may be even
	// more confusing than the inconsistency.
  Note* newNote(const QString& = QString::null);
  MultiLayer *plot(Table*, SIP_PYTUPLE, int style=1, int color=-1);
%MethodCode
  QStringList l;
  int n = PyTuple_Size(a1);
  for (int i=0; i<n; i++) {
    PyObject *str = PyObject_Str(PyTuple_GET_ITEM(a1,i));
    if (str) {
      l << PyString_AsString(str);
		Py_DECREF(str);
	 } else {
      sipIsErr = 1;
      break;
    }
  }
  if (a0==0) {
	  sipIsErr = 1;
	  PyErr_Format(PyExc_ValueError, "Invalid table in argument to plot().");
  } else {
    sipRes = sipCpp->multilayerPlot(a0, l, a2);
    if (sipRes && sipRes->activeGraph() && a3 >= 0) {
		 for (int c =0; c < sipRes->activeGraph()->curves(); c++) {
			 QwtPlotCurve *curve = sipRes->activeGraph()->curve(c);
			 QPen pen = curve->pen();
			 pen.setColor(ColorBox::color(a3));
			 curve->setPen(pen);
			 QwtSymbol symbol = curve->symbol();
			 pen = symbol.pen();
			 pen.setColor(ColorBox::color(a3));
			 symbol.setPen(pen);
			 QBrush brush = symbol.brush();
			 brush.setColor(ColorBox::color(a3));
			 symbol.setBrush(brush);
			 curve->setSymbol(symbol);
		 }
    }
  }
%End
  MultiLayer *plot(Table*, const QString&, int style=1, int color=-1);
%MethodCode
  if (a0==0) {
	  sipIsErr = 1;
	  PyErr_Format(PyExc_ValueError, "Invalid table in argument to plot().");
  } else {
    QStringList l;
    l << *a1;
    sipRes = sipCpp->multilayerPlot(a0, l, a2);
    if (a3 >= 0) {
       QwtPlotCurve * c = sipRes->activeGraph()->curve(0);
       QPen pen = c->pen();
       pen.setColor(ColorBox::color(a3));
       c->setPen(pen);
		 QwtSymbol symbol = c->symbol();
		 pen = symbol.pen();
		 pen.setColor(ColorBox::color(a3));
		 symbol.setPen(pen);
		 QBrush brush = symbol.brush();
		 brush.setColor(ColorBox::color(a3));
		 symbol.setBrush(brush);
		 c->setSymbol(symbol);
    }
  }
%End

	Matrix* importImage();
	Matrix* importImage(const QString&);
	MultiLayer* plotContour(Matrix*);
	MultiLayer* plotColorMap(Matrix*);
	MultiLayer* plotGrayScale(Matrix*);

  QWidgetList *windowsList() /PyName=windows/;

  QTextEdit *results;
//  QTextEdit *console;

  // folders
  Folder *activeFolder();
%MethodCode
  sipRes = sipCpp->current_folder;
%End
  bool changeFolder(Folder *, bool force = false) /PyName=setActiveFolder/;

  void saveFolder(Folder *folder, const QString& fn) /Deprecated/;
  Folder* projectFolder() /PyName=rootFolder/;

  void renameWindow(MyWidget *, const QString &) /Deprecated/;

  // Prior to SciDAVis 0.2.4, it was necessary to call this on a layer created using
  // Graph.addLayer() if you wanted the layer to be intialized correctly. This is now
  // done automatically as part of addLayer(); doing so again doesn't hurt, but is not necessary
  // any more.
  void setPreferences(Graph*) /Deprecated/;

  MyWidget* clone(MyWidget*) /Deprecated/;

private:
  ApplicationWindow(const ApplicationWindow&);
};

class Fit : QObject
{
%TypeHeaderCode
#include "src/Fit.h"
%End
public:
  enum Algorithm{ScaledLevenbergMarquardt, UnscaledLevenbergMarquardt, NelderMeadSimplex};
  enum ErrorSource {UnknownErrors, AssociatedErrors, PoissonErrors, CustomErrors};

  Fit(ApplicationWindow* /TransferThis/, Graph*=0, const char*=0);
  ~Fit();

  virtual void fit();

  bool setYErrorSource(ErrorSource err, const QString& colName = QString::null);

  bool setDataFromCurve(const QString&, Graph*=0);

  bool setDataFromCurve(const QString&, double, double, Graph*=0) /Deprecated/;

  void setInterval(double from, double to);

  QString formula();
  int numParameters();

  void setInitialGuess(int, double) /PyName=setInitialValue/;
  void setInitialGuesses(...) /PyName=setInitialValues/;
%MethodCode
int n = PyTuple_GET_SIZE(a0);
double *values = new double[n];
for (int i=0; i<n; i++) {
  PyObject *item = PyTuple_GET_ITEM(a0, i);
  if (PyNumber_Check(item)) {
    item=PyNumber_Float(item);
    if (!item) {
      sipIsErr=1;
      break;
    }
    values[i] = PyFloat_AS_DOUBLE(item);
    Py_DECREF(item);
  } else
    values[i] = 0;
}
sipCpp->setInitialGuesses(values);
delete values;
%End

  virtual void guessInitialValues();

  void setAlgorithm(Algorithm);

  void setTolerance(double);

  void setColor(int);

  // inconsistent with other setColor() methods and accepts only colors from the palette,
  // in contrast to what one would expect
  void setColor(const QString&) /Deprecated/;

  void setOutputPrecision(int);

  void generateFunction(bool, int=100);

  void setMaximumIterations(int);

  void showLegend();

  virtual QString legendInfo();

  void scaleErrors(bool yes = true);

  SIP_PYTUPLE results();
%MethodCode
double *results = sipCpp->results();
int size=sipCpp->numParameters();
sipRes = PyTuple_New(size);
if(sipRes)
{
  for(int i=0; i<size; i++)
    PyTuple_SET_ITEM(sipRes, i, PyFloat_FromDouble(results[i]));
} else
  sipIsErr = 1;
%End

  SIP_PYTUPLE errors();
%MethodCode
double *errors = sipCpp->errors();
int size=sipCpp->numParameters();
sipRes = PyTuple_New(size);
if(sipRes)
{
  for(int i=0; i<size; i++)
    PyTuple_SET_ITEM(sipRes, i, PyFloat_FromDouble(errors[i]));
} else
  sipIsErr = 1;
%End

  //! Returns the sum of squares of the residuals from the best-fit line
  double chiSquare();
  
  //! Returns the coefficient of determination of the dataset fitted
  double rSquare();

  Table* parametersTable(const QString&);
  Matrix* covarianceMatrix(const QString&);
};

%ModuleCode
ApplicationWindow *sipscidavis_app()
{
  int iserr = 0;
  PyObject *me = PyImport_ImportModule("scidavis");
  PyObject *mydict = PyModule_GetDict(me);
  PyObject *pyapp = PyDict_GetItemString(mydict,"app");
  Py_DECREF(me);
  if (sipCanConvertToInstance(pyapp, sipClass_ApplicationWindow, SIP_NOT_NONE))
    return (ApplicationWindow*) sipConvertToInstance(pyapp, sipClass_ApplicationWindow, NULL, SIP_NOT_NONE, NULL, &iserr);
  else
    return NULL;
}
%End
%ModuleHeaderCode
class ApplicationWindow;
ApplicationWindow *sipscidavis_app();
#define SIPSCIDAVIS_APP(sipcppexpr)\
ApplicationWindow *app = sipscidavis_app();\
if (app) sipCpp = sipcppexpr;\
else { sipCpp = NULL; }
%End

class ExponentialFit : Fit
{
%TypeHeaderCode
#include "src/ExponentialFit.h"
%End
public:
  ExponentialFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, bool=false);
  ExponentialFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double, bool=false);
  ExponentialFit(Graph *, const QString&, bool=false) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipExponentialFit(app, a0, *a1, a2))
%End
  ExponentialFit(Graph *, const QString&, double, double, bool=false) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipExponentialFit(app, a0, *a1, a2, a3, a4))
%End
};

class TwoExpFit : Fit
{
%TypeHeaderCode
#include "src/ExponentialFit.h"
%End
public:
  TwoExpFit(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  TwoExpFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  TwoExpFit(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipTwoExpFit(app, a0, *a1))
%End
  TwoExpFit(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipTwoExpFit(app, a0, *a1, a2, a3))
%End
};

class ThreeExpFit : Fit
{
%TypeHeaderCode
#include "src/ExponentialFit.h"
%End
public:
  ThreeExpFit(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  ThreeExpFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  ThreeExpFit(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipThreeExpFit(app, a0, *a1))
%End
  ThreeExpFit(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipThreeExpFit(app, a0, *a1, a2, a3))
%End
};

class SigmoidalFit : Fit
{
%TypeHeaderCode
#include "src/SigmoidalFit.h"
%End
public:
  SigmoidalFit(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  SigmoidalFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  SigmoidalFit(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipSigmoidalFit(app, a0, *a1))
%End
  SigmoidalFit(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipSigmoidalFit(app, a0, *a1, a2, a3))
%End
};

class GaussAmpFit : Fit
{
%TypeHeaderCode
#include "src/MultiPeakFit.h"
%End
public:
  GaussAmpFit(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  GaussAmpFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  GaussAmpFit(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipGaussAmpFit(app, a0, *a1))
%End
  GaussAmpFit(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipGaussAmpFit(app, a0, *a1, a2, a3))
%End
};

class NonLinearFit : Fit
{
%TypeHeaderCode
#include "src/NonLinearFit.h"
%End
public:
  NonLinearFit(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  NonLinearFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  NonLinearFit(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipNonLinearFit(app, a0, *a1))
%End
  NonLinearFit(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipNonLinearFit(app, a0, *a1, a2, a3))
%End
  void setParametersList(...) /PyName=setParameters/;
%MethodCode
  QStringList l;
  char *item;
  for (int i=0; i<PyTuple_GET_SIZE(a0); i++)
    if (item = PyString_AsString(PyTuple_GET_ITEM(a0, i)))
      l << item;
    else
      sipIsErr = 1;
  sipCpp->setParametersList(l);
%End
  // TODO: make it accept Python callables
  void setFormula(const QString&);
};

class PluginFit : Fit
{
%TypeHeaderCode
#include "src/PluginFit.h"
%End
public:
  PluginFit(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  PluginFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  PluginFit(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipPluginFit(app, a0, *a1))
%End
  PluginFit(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipPluginFit(app, a0, *a1, a2, a3))
%End
  bool load(const QString&);
};

class MultiPeakFit : Fit
{
%TypeHeaderCode
#include "src/MultiPeakFit.h"
%End
public:
  enum PeakProfile{Gauss, Lorentz};
  MultiPeakFit(ApplicationWindow * /TransferThis/, Graph *, PeakProfile=Gauss, int=1);

  int peaks() /PyName=numPeaks/;
  void setNumPeaks(int);

  void enablePeakCurves(bool);
  void setPeakCurvesColor(int);

  static QString generateFormula(int, PeakProfile);
  static QStringList generateParameterList(int);
};

class LorentzFit : MultiPeakFit
{
%TypeHeaderCode
#include "src/MultiPeakFit.h"
%End
public:
  LorentzFit(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  LorentzFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  LorentzFit(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipLorentzFit(app, a0, *a1))
%End
  LorentzFit(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipLorentzFit(app, a0, *a1, a2, a3))
%End
};

class GaussFit : MultiPeakFit
{
%TypeHeaderCode
#include "src/MultiPeakFit.h"
%End
public:
  GaussFit(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  GaussFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  GaussFit(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipGaussFit(app, a0, *a1))
%End
  GaussFit(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipGaussFit(app, a0, *a1, a2, a3))
%End
};

class PolynomialFit : Fit
{
%TypeHeaderCode
#include "src/PolynomialFit.h"
%End
public:
  PolynomialFit(ApplicationWindow * /TransferThis/, Graph *, QString&, int=2, bool=false);
  PolynomialFit(ApplicationWindow * /TransferThis/, Graph *, QString&, double, double, int=2, bool=false);
  PolynomialFit(Graph *, QString&, int=2, bool=false) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipPolynomialFit(app, a0, *a1, a2, a3))
%End
  PolynomialFit(Graph *, QString&, double, double, int=2, bool=false) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipPolynomialFit(app, a0, *a1, a2, a3, a4, a5))
%End

  virtual QString legendInfo();
  void fit();

  static QString generateFormula(int);
  static QStringList generateParameterList(int);
};

class LinearFit : Fit
{
%TypeHeaderCode
#include "src/PolynomialFit.h"
%End
public:
  LinearFit(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  LinearFit(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  LinearFit(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipLinearFit(app, a0, *a1))
%End
  LinearFit(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipLinearFit(app, a0, *a1, a2, a3))
%End
  void fit();
};

class Filter : QObject
{
%TypeHeaderCode
#include "src/Filter.h"
%End
public:
  Filter(ApplicationWindow* /TransferThis/, Graph*=0, const char*=0);
  ~Filter();

  void setOutputPoints(int);

  void setColor(int);
  // inconsistent with other setColor() methods and accepts only colors from the palette,
  // in contrast to what one would expect
  void setColor(const QString&) /Deprecated/;

  virtual bool run();
};

class Differentiation : Filter
{
%TypeHeaderCode
#include "src/Differentiation.h"
%End
public:
  Differentiation(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  Differentiation(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  Differentiation(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipDifferentiation(app, a0, *a1))
%End
  Differentiation(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipDifferentiation(app, a0, *a1, a2, a3))
%End
  bool run();
};

class Integration : Filter
{
%TypeHeaderCode
#include "src/Integration.h"
%End
public:
  Integration(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  Integration(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double);
  Integration(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipIntegration(app, a0, *a1))
%End
  Integration(Graph *, const QString&, double, double) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipIntegration(app, a0, *a1, a2, a3))
%End

	enum InterpolationMethod{Linear, Cubic, Akima};
	InterpolationMethod method();
    void setMethod(InterpolationMethod method);

  bool run();

  double result();
};

class Interpolation : Filter
{
%TypeHeaderCode
#include "src/Interpolation.h"
%End
public:
  enum InterpolationMethod{Linear, Cubic, Akima};

  Interpolation(ApplicationWindow * /TransferThis/, Graph *, const QString&, int=0);
  Interpolation(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double, int=0);
  Interpolation(Graph *, const QString&, int=0) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipInterpolation(app, a0, *a1, a2))
%End
  Interpolation(Graph *, const QString&, double, double, int=0) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipInterpolation(app, a0, *a1, a2, a3, a4))
%End

  void setMethod(int n);
  void setMethod(InterpolationMethod m /Constrained/);
  bool run();
};

class SmoothFilter : Filter
{
%TypeHeaderCode
#include "src/SmoothFilter.h"
%End
public:
  enum SmoothMethod{SavitzkyGolay = 1, FFT = 2, Average = 3};

  SmoothFilter(ApplicationWindow * /TransferThis/, Graph *, const QString&, int=3);
  SmoothFilter(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double, int=3);
  SmoothFilter(Graph *, const QString&, int=3) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipSmoothFilter(app, a0, *a1, a2))
%End
  SmoothFilter(Graph *, const QString&, double, double, int=3) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipSmoothFilter(app, a0, *a1, a2, a3, a4))
%End

  void setMethod(int);
  void setMethod(SmoothMethod /Constrained/);

  void setSmoothPoints(int, int = 0);
  void setPolynomOrder(int);

  bool run();
};

class FFTFilter : Filter
{
%TypeHeaderCode
#include "src/FFTFilter.h"
%End
public:
  enum FilterType{LowPass = 1, HighPass = 2, BandPass = 3, BandBlock = 4};

  FFTFilter(ApplicationWindow * /TransferThis/, Graph *, const QString&, int=1);
  FFTFilter(ApplicationWindow * /TransferThis/, Graph *, const QString&, double, double, int=1);
  FFTFilter(Graph *, const QString&, int=1) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipFFTFilter(app, a0, *a1, a2))
%End
  FFTFilter(Graph *, const QString&, double, double, int=1) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipFFTFilter(app, a0, *a1, a2, a3, a4))
%End

  void setFilterType(int);
  void setFilterType(FilterType /Constrained/);

  void setCutoff(double);
  void setBand(double, double);
  void enableOffset(bool=true);

  bool run();
};

class FFT : Filter
{
%TypeHeaderCode
#include "src/FFT.h"
%End
public:

  FFT(ApplicationWindow * /TransferThis/, Table *, const QString&, const QString& = QString());
  FFT(ApplicationWindow * /TransferThis/, Graph *, const QString&);
  FFT(Table *, const QString&, const QString& = QString()) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipFFT(app, a0, *a1, *a2))
%End
  FFT(Graph *, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipFFT(app, a0, *a1))
%End

  void setInverseFFT(bool=true);
  void setSampling(double);
  void normalizeAmplitudes(bool=true);
  void shiftFrequencies(bool=true);

  bool run();
};

class Correlation : Filter
{
%TypeHeaderCode
#include "src/Correlation.h"
%End
public:

  Correlation(ApplicationWindow * /TransferThis/, Table *, const QString&, const QString&);
  Correlation(Table *, const QString&, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipCorrelation(app, a0, *a1, *a2))
%End

  void setDataFromTable(Table *, const QString&, const QString&);
  bool run();
};

class Convolution : Filter
{
%TypeHeaderCode
#include "src/Convolution.h"
%End
public:

  Convolution(ApplicationWindow * /TransferThis/, Table *, const QString&, const QString&);
  Convolution(Table *, const QString&, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipConvolution(app, a0, *a1, *a2))
%End

  void setDataFromTable(Table *, const QString&, const QString&);
  bool run();
};

class Deconvolution : Filter
{
%TypeHeaderCode
#include "src/Convolution.h"
%End
public:

  Deconvolution(ApplicationWindow * /TransferThis/, Table *, const QString&, const QString&);
  Deconvolution(Table *, const QString&, const QString&) /NoDerived/;
%MethodCode
  SIPSCIDAVIS_APP(new sipDeconvolution(app, a0, *a1, *a2))
%End

  void setDataFromTable(Table *, const QString&, const QString&);
  bool run();
};

// used for output redirection
class PythonScripting
{
%TypeHeaderCode
#include "src/PythonScripting.h"
%End
public:
  void write(const QString&);
private:
  PythonScripting(const PythonScripting&);
};
class PythonScript
{
%TypeHeaderCode
#include "src/PythonScript.h"
%End
public:
  void write(const QString&);
private:
  PythonScript(const PythonScript&);
};

class Folder : QObject
{
%TypeHeaderCode
#include "src/Folder.h"
%End
public:
  Folder(Folder *parent, const QString &name);
  Folder(const QString &name) /NoDerived/;
%MethodCode
	sipCpp = new sipFolder(0, *a0);
%End
  QList<MyWidget*> windowsList() /PyName=windows/;
// TODO: implement signal Folder::nameChanged and make it update the project explorer; adjust renaming from GUI accordingly
//  void setFolderName(const QString&) /PyName=setName/;
  QString name();
  QString path();

  QList<Folder*> folders() const;
  Folder *findSubfolder(const QString&, bool=true, bool=false)  /PyName=folder/;
  MyWidget* findWindow(const QString&, bool=true, bool=true, bool=false, bool=true);

  // undocumented, unsupported method with questionable value; seeing that one has to
  // know the confusing C++ class names to use it
  MyWidget *window(const QString &name, const char *cls="MyWidget", bool recursive=false) /Deprecated/;

  Table *table(const QString &name, bool recursive=false);
  Matrix *matrix(const QString &name, bool recursive=false);
  MultiLayer *graph(const QString &name, bool recursive=false);
  Note *note(const QString &name, bool recursive=false);

  Folder* rootFolder();

  void save(const QString &filename);
%MethodCode
	ApplicationWindow *app = sipscidavis_app();
	if (app)
		app->saveFolder(sipCpp, *a0);
%End

	void addChild(Folder *f /Transfer/);
%MethodCode
	QString name = a0->objectName();
	int i = 1;
	while (sipCpp->subfolders().contains(name))
		name = a0->objectName() + QString::number(i++);
	if (a0->parent() == 0) { // adding newly constructed folder
		a0->setParent(sipCpp);
		a0->setObjectName(name);
		ApplicationWindow *app = sipscidavis_app();
		if (app) {
			app->addFolderListViewItem(a0);
			a0->setFolderListItem(new FolderListItem(sipCpp->folderListItem(), a0));
		}
	} else { // moving existing folder
		ApplicationWindow *app = sipscidavis_app();
		if (app)
			app->moveFolder(a0->folderListItem(), sipCpp->folderListItem());
	}
%End

private:
  Folder(const Folder&);
};
