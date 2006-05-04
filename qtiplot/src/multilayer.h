#ifndef MULTILAYER_H
#define MULTILAYER_H

#include "widget.h"
#include "graph.h"

#include <qwidget.h>
#include <qpushbutton.h>
#include <qobject.h>
#include <qhbox.h>
#include <qptrlist.h>
#include <qprinter.h>

#include <gsl/gsl_vector.h>

class QWidget;
class QLabel;
class QPushButton;
class QWidget;
class Graph;
class Table;
class LayerButton;
	
class MultiLayer: public myWidget
{
	Q_OBJECT

public:
    MultiLayer (const QString& label, QWidget* parent=0, const char* name=0, WFlags f=0);
	QWidgetList* graphPtrs(){return graphsList;};
	LayerButton* addLayerButton();

	virtual WidgetType rtti(){return Plot2D;};
	
	//event handlers
	void contextMenuEvent(QContextMenuEvent *e);
	void closeEvent(QCloseEvent *e);
	void wheelEvent ( QWheelEvent * e );
	void keyPressEvent(QKeyEvent * e);
	bool eventFilter(QObject *object, QEvent *e);
	bool resizeLayers (const QResizeEvent *re);
	
	QWidgetList *buttonsList, *graphsList;
	QHBox  *hbox1;
	QWidget *canvas;

public slots:
	Graph* insertFirstLayer();
	Graph* addLayer();
	Graph* addLayerToOrigin();
	Graph* addLayer(int x, int y, int width, int height);

	bool isEmpty();
    void removeLayer();
	void confirmRemoveLayer();

	void addTextLayer();
	void addTextLayer(const QPoint& pos);

	Graph* activeGraph(){return active_graph;};
	void setActiveGraph(Graph* g);
	void activateGraph(LayerButton* button);
	
	void resizeGraph(Graph* g, const QPoint& pos);
	void moveGraph(Graph* g, const QPoint& pos);
	void releaseGraph(Graph* g);
	void newSizeGraph(Graph* g);
	
	void setGraphOrigin(const QPoint& pos);
	void setGraphGeometry(int x, int y, int w, int h);

	void findBestLayout(int &rows, int &cols);
	void arrangeLayers(int c, int r, int colsGap, int rowsGap);
	void arrangeLayers(int c, int r, int colsGap, int rowsGap, bool fit); 
	QSize calculateMaxCanvasSize(int c, int r);
	QSize maxSize();
	
	int getRows(){return rows;};
	void setRows(int r);
	
	int getCols(){return cols;};
	void setCols(int c);
	
	int colsSpacing(){return colsSpace;};
	int rowsSpacing(){return rowsSpace;};
	
	int graphsNumber(){return graphs;};
	
	// print and export
	QPixmap canvasPixmap();

	void exportImage(const QString& fileName,const QString& fileType, int quality, bool transparent);
	void exportToEPS(const QString& fname);
	void exportToEPS(const QString& fname, int res, QPrinter::Orientation o, 
					QPrinter::PageSize pageSize, QPrinter::ColorMode col);
	void copyAllLayers();
	void print();
	void printAllLayers(QPainter *painter);
	void printActiveLayer();
	
	void setFonts(const QFont& titleFnt, const QFont& scaleFnt,
							const QFont& numbersFnt, const QFont& legendFnt);
	void makeTransparentLayer(Graph *g);
	void updateLayerTransparency(Graph *g);
	void updateTransparency();
	void connectLayer(Graph *g);
	bool overlapsLayers(Graph *g);
	bool hasOverlapingLayers();

	QString saveAsTemplate(const QString& geometryInfo);

signals:   
	void showTextDialog();
	void showPlotDialog(long);
	void showAxisDialog(int);
	void showScaleDialog(int);
	void showGraphContextMenu();
	void showWindowContextMenu();
	void showCurvesDialog();
	void drawTextOff();
	void showXAxisTitleDialog();
	void showYAxisTitleDialog();
	void showTopAxisTitleDialog();
	void showRightAxisTitleDialog();
	void showMarkerPopupMenu();
	void closedPlot(QWidget*);
	void hidePlot(QWidget*);
	void resizedPlot(QWidget*);
	void modifiedPlot();
	void cursorInfo(const QString&);
	void showImageDialog();
	void showPieDialog();
	void showLineDialog();
	void viewTitleDialog();
	void createTablePlot(const QString&,int,int,const QString&);
	void createTable(const QString&,int,int,const QString&);
	void createHistogramTable(const QString&,int,int,const QString&);
	void updateTable(const QString&,int,const QString&);
	void updateTableColumn(const QString&, double *, int);
	void clearCell(const QString&,double);	
	void showGeometryDialog();
	void pasteMarker();
	void createIntensityTable(const QPixmap&);
	void setPointerCursor();
	
private:
	Graph* active_graph;
	int graphs, cols, rows, graph_width, graph_height, colsSpace, rowsSpace;
	int xMouse, yMouse, xActiveGraph, yActiveGraph;
	bool movedGraph, addTextOn;

// used when resizing layer by mouse-dragging (all of them really necessary?)
	int xlb, ytb, xrb, ybb, Save_oldw, Save_oldh, xInt, yInt, oldw, oldh, xrMouse, yrMouse;
	bool resizedGraph, yesResize,ChangeOrigin;
};

	
class LayerButton: public QWidget
{
	Q_OBJECT

public:
    LayerButton (const QString& text, QWidget* parent, const char* name);
	~LayerButton();

	 QPushButton  *btn;

	bool eventFilter(QObject *object, QEvent *e);

public slots:
	 void setText(const QString& text);
	 void setOn(bool on);
	 bool isOn(){return btn->isOn();};

signals:
	void showLayerMenu();
	void showCurvesDialog();	
	void clicked(LayerButton*);
};

#endif 
