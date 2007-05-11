/***************************************************************************
    File                 : MultiLayer.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
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

#include "MyWidget.h"
#include "Graph.h"
#include <QPushButton>
#include <QLayout>
#include <QPointer>

class QWidget;
class QLabel;
class QWidget;
class LayerButton;
class SelectionMoveResizer;

/**
 * \brief An MDI window (MyWidget) managing one or more Graph objects.
 *
 * %Note that several parts of the code, as well as the user interface, refer to MultiLayer as "graph" or "plot",
 * practically guaranteeing confusion with the classes Graph and Plot.
 *
 * \section future Future Plans
 * Manage any QWidget instead of only Graph.
 * This would allow 3D graphs to be added as well, so you could produce mixed 2D/3D arrangements.
 * It would also allow text labels to be added directly instead of having to complicate things by wrapping them
 * up in a Graph (see documentation of ImageMarker for details) (see documentation of ImageMarker for details).
 *
 * The main problem to be figured out for this is how Graph would interface with the rest of the project.
 * A possible solution is outlined in the documentation of ApplicationWindow:
 * If MultiLayer exposes its parent Project to the widgets it manages, they could handle things like creating
 * tables by calling methods of Project instead of sending signals.
 */
class MultiLayer: public MyWidget
{
	Q_OBJECT

public:
    MultiLayer (const QString& label, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
	QWidgetList graphPtrs(){return graphsList;};
	Graph *layer(int num);
	LayerButton* addLayerButton();
	void copy(MultiLayer* ml);

	enum HorAlignement{HCenter, Left, Right};
	enum VertAlignement{VCenter, Top, Bottom};

	//! \name Event Handlers
	//@{
	void mousePressEvent(QMouseEvent *);
	void contextMenuEvent(QContextMenuEvent *);
	void wheelEvent(QWheelEvent *);
	void keyPressEvent(QKeyEvent *);
	void changeEvent(QEvent *);
	bool eventFilter(QObject *object, QEvent *);
	void releaseLayer();

	bool focusNextPrevChild ( bool next );
	//@}

	void setOpenMaximized(){d_open_maximized = 1;};

	bool scaleLayersOnPrint(){return d_scale_on_print;};
	void setScaleLayersOnPrint(bool on){d_scale_on_print = on;};

	bool printCropmarksEnabled(){return d_print_cropmarks;};
	void printCropmarks(bool on){d_print_cropmarks = on;};

public slots:
	Graph* addLayer(int x = 0, int y = 0, int width = 0, int height = 0);
	void setLayersNumber(int n);

	bool isEmpty();
    void removeLayer();
	void confirmRemoveLayer();

	/*!\brief Start adding a text layer.
	 *
	 * This works by having #canvas grab the mouse, remembering that we are in the midst of adding
	 * text in #addTextOn and dispatching the next mouse click to addTextLayer(const QPoint&) in eventFilter().
	 *
	 * \sa #defaultTextMarkerFont, #defaultTextMarkerFrame, #defaultTextMarkerColor, #defaultTextMarkerBackground
	 */
	void addTextLayer(int f, const QFont& font, const QColor& textCol, const QColor& backgroundCol);
	/*!\brief Finish adding a text layer.
	 *
	 * An empty Graph is created and added to me.
	 * Legend, title and axes are removed and a new LegendMarker is added with a placeholder text.
	 *
	 * \sa #defaultTextMarkerFont, #defaultTextMarkerFrame, #defaultTextMarkerColor, #defaultTextMarkerBackground, addTextLayer(int,const QFont&,const QColor&,const QColor&)
	 */
	void addTextLayer(const QPoint& pos);

	Graph* activeGraph(){return active_graph;};
	void setActiveGraph(Graph* g);
	void activateGraph(LayerButton* button);

	void setGraphGeometry(int x, int y, int w, int h);

	void findBestLayout(int &rows, int &cols);

	QSize arrangeLayers(bool userSize);
	void arrangeLayers(bool fit, bool userSize);
    void adjustSize();

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
	void exportToFile(const QString& fileName);
	void exportImage(const QString& fileName, int quality = 100, bool transparent = false);
	void exportSVG(const QString& fname);
    void exportPDF(const QString& fname);
	void exportVector(const QString& fileName, int res = 0, bool color = true);

	void copyAllLayers();
	void print();
	void printAllLayers(QPainter *painter);
	void printActiveLayer();
	//@}

	void setFonts(const QFont& titleFnt, const QFont& scaleFnt,
							const QFont& numbersFnt, const QFont& legendFnt);

	void connectLayer(Graph *g);

	QString saveToString(const QString& geometry);
	QString saveAsTemplate(const QString& geometryInfo);

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
	void showLineDialog();
	void viewTitleDialog();
	void createTable(const QString&,int,int,const QString&);
	void showGeometryDialog();
	void pasteMarker();
	void createIntensityTable(const QString&);
	void setPointerCursor();

private:
	void resizeLayers (const QResizeEvent *re);
	void resizeLayers (const QSize& size, const QSize& oldSize, bool scaleFonts);

	Graph* active_graph;
	//! Used for resizing of layers.
	int graphs, cols, rows, graph_width, graph_height, colsSpace, rowsSpace;
	int left_margin, right_margin, top_margin, bottom_margin;
	int l_canvas_width, l_canvas_height, hor_align, vert_align;
	bool addTextOn;
	bool d_scale_on_print, d_print_cropmarks;

	//! Used when adding text markers on new layers
	int defaultTextMarkerFrame;
	QFont defaultTextMarkerFont;
	QColor defaultTextMarkerColor, defaultTextMarkerBackground;

    QWidgetList buttonsList, graphsList;
	QHBoxLayout *layerButtonsBox;
    QWidget *canvas;

	QPointer<SelectionMoveResizer> d_layers_selector;
	int d_open_maximized;
	//! Stores the size of the widget in the Qt::WindowMaximized state.
	QSize d_max_size;
	//! Stores the size of the widget in Qt::WindowNoState (normal state).
    QSize d_normal_size;
};


//! Button with layer number
class LayerButton: public QPushButton
{
	Q_OBJECT

public:
    LayerButton (const QString& text = QString::null, QWidget* parent = 0);
	~LayerButton(){};

	static int btnSize(){return 20;};

protected:
	void mousePressEvent( QMouseEvent * );
	void mouseDoubleClickEvent ( QMouseEvent * );

signals:
	void showCurvesDialog();
	void clicked(LayerButton*);
};

#endif
