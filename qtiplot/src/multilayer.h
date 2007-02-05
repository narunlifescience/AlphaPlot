/***************************************************************************
    File                 : multilayer.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net,
                           knut.franke@gmx.de
    Description          : Multi layer widget
                           
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
#ifndef MULTILAYER_H
#define MULTILAYER_H

#include "widget.h"
#include "graph.h"
#include <QPushButton>
#include <QLayout>

class QWidget;
class QLabel;
class QWidget;
class LayerButton;
	
/**
 * \brief An MDI window (MyWidget) managing one or more Graph objects.
 *
 * %Note that several parts of the code, as well as the user interface, refer to MultiLayer as "graph" or "plot",
 * practically guaranteeing confusion with the classes Graph and Plot.
 */
class MultiLayer: public MyWidget
{
	Q_OBJECT

public:
    MultiLayer (const QString& label, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
	QWidgetList graphPtrs(){return graphsList;};
	Graph *layer(int num);
	LayerButton* addLayerButton();	

	enum HorAlignement{HCenter, Left, Right};
	enum VertAlignement{VCenter, Top, Bottom};

	//! \name Event Handlers
	//@{
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void contextMenuEvent(QContextMenuEvent *);
	void wheelEvent(QWheelEvent *);
	void keyPressEvent(QKeyEvent *);
	bool eventFilter(QObject *object, QEvent *);
	void releaseLayer();
	//@}

public slots:
	void resizeLayers (const QResizeEvent *re);

	Graph* insertFirstLayer();
	Graph* addLayer();
	Graph* addLayerToOrigin();
	Graph* addLayer(int x, int y, int width, int height);
	void setLayersNumber(int n);

	bool isEmpty();
    void removeLayer();
	void confirmRemoveLayer();

	void addTextLayer(int f, const QFont& font, const QColor& textCol, const QColor& backgroundCol);
	void addTextLayer(const QPoint& pos);

	Graph* activeGraph(){return active_graph;};
	void setActiveGraph(Graph* g);
	void activateGraph(LayerButton* button);
	
	void moveGraph(Graph* g, const QPoint& pos);
	void releaseGraph(Graph* g);
	
	void setGraphOrigin(const QPoint& pos);
	void setGraphGeometry(int x, int y, int w, int h);

	void findBestLayout(int &rows, int &cols);
	
	QSize arrangeLayers(bool userSize);
	void arrangeLayers(bool fit, bool userSize);
	
	QSize maxSize();
	
	int getRows(){return rows;};
	void setRows(int r);
	
	int getCols(){return cols;};
	void setCols(int c);
	
	int colsSpacing(){return colsSpace;};
	int rowsSpacing(){return rowsSpace;};
	void setSpacing (int rgap, int cgap);

	int leftMargin(){return left_margin;};
	int rightMargin(){return right_margin;};
	int topMargin(){return top_margin;};
	int bottomMargin(){return bottom_margin;};
	void setMargins (int lm, int rm, int tm, int bm);

	QSize layerCanvasSize(){return QSize(l_canvas_width, l_canvas_height);};
	void setLayerCanvasSize (int w, int h);

	int horizontalAlignement(){return hor_align;};
	int verticalAlignement(){return vert_align;};
	void setAlignement (int ha, int va);

	int layers(){return graphs;};
	
	//! \name Print and Export
	//@{
	QPixmap canvasPixmap();

	void exportImage(const QString& fileName,const QString& fileType, int quality, bool transparent);
	void exportToSVG(const QString& fname);
	void exportToEPS(const QString& fname);
	void exportToEPS(const QString& fname, int res, QPrinter::Orientation o, 
					QPrinter::PageSize pageSize, QPrinter::ColorMode col);

	void copyAllLayers();
	void print();
	void printAllLayers(QPainter *painter);
	void printActiveLayer();
	//@}
	
	void setFonts(const QFont& titleFnt, const QFont& scaleFnt,
							const QFont& numbersFnt, const QFont& legendFnt);
	void makeTransparentLayer(Graph *g);
	void updateLayerTransparency(Graph *g);
	void updateTransparency();
	void connectLayer(Graph *g);
	bool overlapsLayers(Graph *g);
	bool hasOverlapingLayers();
	bool allLayersTransparent();

	void highlightLayer(Graph*g);
	void drawLayerFocusRect(const QRect& fr);
	void showLayers(bool ok);

	QString saveToString(const QString& geometry);
	QString saveAsTemplate(const QString& geometryInfo);

	void ignoreResizeEvent(bool ignore){ignore_resize = ignore;};

signals:   
	void showTextDialog();
	void showPlotDialog(int);
	void showAxisDialog(int);
	void showScaleDialog(int);
	void showGraphContextMenu();
	void showCurveContextMenu(int);
	void showWindowContextMenu();
	void showCurvesDialog();
	void drawTextOff();
	void drawLineEnded(bool);
	void showXAxisTitleDialog();
	void showYAxisTitleDialog();
	void showTopAxisTitleDialog();
	void showRightAxisTitleDialog();
	void showMarkerPopupMenu();
	void modifiedPlot();
	void cursorInfo(const QString&);
	void showImageDialog();
	void showPieDialog();
	void showLineDialog();
	void viewTitleDialog();
	void createTablePlot(const QString&,int,int,const QString&);
	void createTable(const QString&,int,int,const QString&);
	//! To be connected to ApplicationWindow::newHiddenTable; see there for documentation.
	void createHiddenTable(const QString&,const QString&,int,int,const QString&);
	void createHistogramTable(const QString&,int,int,const QString&);
	void updateTable(const QString&,int,const QString&);
	void updateTableColumn(const QString&, double *, int);
	void clearCell(const QString&,double);	
	void showGeometryDialog();
	void pasteMarker();
	void createIntensityTable(const QPixmap&);
	void setPointerCursor();
	void resizeCanvas(const QResizeEvent *);
	
private:
	Graph* active_graph;
	//! Used for resizing of layers.
	QRect aux_rect;
	QPixmap cache_pix;
	int graphs, cols, rows, graph_width, graph_height, colsSpace, rowsSpace;
	int left_margin, right_margin, top_margin, bottom_margin;
	int l_canvas_width, l_canvas_height, hor_align, vert_align;
	//! Used for moving layers.
	int xMouse, yMouse, xActiveGraph, yActiveGraph;
	bool movedGraph, addTextOn, highlightedLayer, ignore_resize;

	//! Used when adding text markers on new layers
	int defaultTextMarkerFrame;
	QFont defaultTextMarkerFont;
	QColor defaultTextMarkerColor, defaultTextMarkerBackground;

    QWidgetList buttonsList, graphsList;
	QHBoxLayout *layerButtonsBox;
    QWidget *canvas;
};

	
//! Button with layer number
class LayerButton: public QPushButton
{
	Q_OBJECT

public:
    LayerButton (const QString& text = QString::null, QWidget* parent = 0);
	~LayerButton(){};
		
	static int btnSize();

protected:
	void mousePressEvent( QMouseEvent * );
	void mouseDoubleClickEvent ( QMouseEvent * );

signals:
	void showCurvesDialog();	
	void clicked(LayerButton*);
};

#endif
