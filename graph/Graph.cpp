/***************************************************************************
    File                 : Graph.cpp
    Project              : SciDAVis
    Description          : Aspect providing a 2d plotting functionality
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2008 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2006-2008 Knut Franke (knut.franke*gmx.de)
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

#include "graph/Graph.h"
#include "graph/GraphView.h"
#include "graph/Plot.h"
#include "graph/TextEnrichment.h"
#include "graph/SelectionMoveResizer.h"

#include <gsl/gsl_vector.h>
#include <QVector>
#include <QWidgetList>
#include <QPrinter>
#include <QPrintDialog>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>
#include <QBitmap>
#include <QImageWriter>
#include <QPainter>
#include <QPicture>
#include <QClipboard>
#include <QHBoxLayout>
#include <QVBoxLayout>

#if QT_VERSION >= 0x040300
	#include <QSvgGenerator>
#endif

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_text_label.h>

/* ============= LayerButton ======== */

LayerButton::LayerButton(const QString& text, QWidget* parent)
: QPushButton(text, parent)
{
	int btn_size = 20;

	setToggleButton(true);
	setOn(true);
	setMaximumWidth(btn_size);
	setMaximumHeight(btn_size);
}

void LayerButton::mousePressEvent( QMouseEvent *event )
{
	if (!isOn())
		emit clicked(this);
	if (event->button() == Qt::RightButton)
		emit showContextMenu();
}

void LayerButton::mouseDoubleClickEvent ( QMouseEvent * )
{
	emit showCurvesDialog();
}

/* ============= Graph ======== */

Graph::Graph(const QString& name)
	: AbstractPart(name)
{
	d_view = NULL; 
	view(); // temporary solution
	createActions();
	connectActions();

	// TODO: check which need to be moved to GraphView
	d_layer_count=0; cols=1; rows=1;
	d_layer_default_width=500; d_layer_default_height=400;
	colsSpace=5; rowsSpace=5;
	left_margin = 5; right_margin = 5;
	top_margin = 5; bottom_margin = 5;
	l_canvas_width = 400; l_canvas_height = 300;
	hor_align = HCenter;  vert_align = VCenter;
	d_active_layer = 0;
	addTextOn = false;
    d_open_maximized = 0;
    d_max_size = QSize();
    d_normal_size = QSize();
	d_scale_on_print = true;
	d_print_cropmarks = false;
}

Graph::Graph()
	: AbstractPart("temp")
{
	createActions();
}

Graph::~Graph()
{
}

QIcon Graph::icon() const
{
	QIcon ico;
	ico.addPixmap(QPixmap(":/graph.xpm"));
	return ico;
}

QMenu *Graph::createContextMenu() const
{
	QMenu *menu = AbstractPart::createContextMenu();
	Q_ASSERT(menu);
	
	// TODO
	
	return menu;
}

QWidget *Graph::view()
{
	if (!d_view)
	{
		d_view = new GraphView(this); 
		addActionsToView();
	}
	return d_view;
}

void Graph::save(QXmlStreamWriter * writer) const
{
	// TODO
}

bool Graph::load(XmlStreamReader * reader)
{
	// TODO: save graph as CDATA in old format
	return false;
}

void Graph::createActions()
{
	// TODO
}
void Graph::connectActions()
{
	// TODO
}

void Graph::addActionsToView()
{
	// TODO
}

Layer *Graph::layer(int num)
{
	return (Layer*) d_layer_list.at(num-1);
}

LayerButton* Graph::addLayerButton()
{
	for (int i=0;i<d_button_list.count();i++)
	{
		LayerButton *btn=(LayerButton*) d_button_list.at(i);
		btn->setOn(false);
	}

	LayerButton *button = new LayerButton(QString::number(++d_layer_count));
	connect (button, SIGNAL(clicked(LayerButton*)),this, SLOT(activateLayer(LayerButton*)));
	connect (button, SIGNAL(showContextMenu()),this, SIGNAL(showLayerButtonContextMenu()));
	connect (button, SIGNAL(showCurvesDialog()),this, SIGNAL(showCurvesDialog()));

	d_button_list.append(button);
    d_view->layerButtonsBox->addWidget(button);
	return button;
}

Layer* Graph::addLayer(int x, int y, int width, int height)
{
	addLayerButton();
	if (!width && !height) {
		width =	d_layer_default_width;
		height = d_layer_default_height;
	}

	Layer* g = new Layer(d_view->canvas);
	g->setAttribute(Qt::WA_DeleteOnClose);
	g->setGeometry(x, y, width, height);
	g->plotWidget()->resize(QSize(width, height));
	d_layer_list.append(g);

	d_active_layer = g;
	g->show();
	connectLayer(g);
	return g;
}

void Graph::adjustSize()
{
    d_view->canvas->resize(size().width(), size().height() - LayerButton::btnSize());
}

void Graph::activateLayer(LayerButton* button)
{
	for (int i=0;i<d_button_list.count();i++)
	{
		LayerButton *btn=(LayerButton*)d_button_list.at(i);
		if (btn->isOn())
			btn->setOn(false);

		if (btn == button)
		{
			d_active_layer = (Layer*) d_layer_list.at(i);
			d_active_layer->setFocus();
			d_active_layer->raise();//raise layer on top of the layers stack
			button->setOn(true);
		}
	}
}

void Graph::setActiveLayer(Layer* g)
{
	if (!g || d_active_layer == g)
		return;

	d_active_layer = g;
	d_active_layer->setFocus();

	if (d_layers_selector)
		delete d_layers_selector;

	d_active_layer->raise();//raise layer on top of the layers stack

	for (int i=0;i<(int)d_layer_list.count();i++)
	{
		Layer *gr = (Layer *)d_layer_list.at(i);
		LayerButton *btn = (LayerButton *)d_button_list.at(i);
		if (gr == g)
			btn->setOn(true);
		else
			btn->setOn(false);
	}
}

void Graph::contextMenuEvent(QContextMenuEvent *e)
{
	emit showWindowContextMenu();
	e->accept();
}

void Graph::showCanvasContextMenu(const QPoint& pos)
{
	if (!d_view) return;
	QMenu context_menu;

	if (selectedMarker>=0) {
		showMarkerPopupMenu(pos);
		return;
	}
// TODO:
	QPoint pos = d_plot->canvas()->mapFrom(d_plot, e->pos());
	int dist, point;
	const long curve = d_plot->closestCurve(pos.x(), pos.y(), dist, point);
	const QwtPlotCurve *c = (QwtPlotCurve *)d_plot->curve(curve);

	if (c && dist < 10)//10 pixels tolerance
		emit showCurveContextMenu(curve);
	else
		emit showContextMenu();

	e->accept();
	


	context_menu.exec(pos);
}

// TODO:

// code from ApplicationWindow, to be ported
#if 0

void ApplicationWindow::loadImage(const QString& fn)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	Graph *plot = multilayerPlot(generateUniqueName(tr("Graph")));
	plot->setWindowLabel(fn);
	plot->setCaptionPolicy(MyWidget::Both);
	setListViewLabel(plot->name(), fn);

	plot->showNormal();
	Layer *layer = plot->addLayer(0, 0, plot->width(), plot->height());

	layer->setTitle("");
	QVector<bool> axesOn(4);
	for (int j=0;j<4;j++)
		axesOn[j]=false;
	layer->enableAxes(axesOn);
	layer->removeLegend();
	layer->setIgnoreResizeEvents(false);
	layer->addImage(fn);
	QApplication::restoreOverrideCursor();
}

void ApplicationWindow::polishLayer(Layer *layer, int style)
{
	if (style == Layer::VerticalBars || style == Layer::HorizontalBars ||style == Layer::Histogram)
	{
		QList<int> ticksList;
		int ticksStyle = ScaleDraw::Out;
		ticksList<<ticksStyle<<ticksStyle<<ticksStyle<<ticksStyle;
		layer->setMajorTicksType(ticksList);
		layer->setMinorTicksType(ticksList);
	}
	if (style == Layer::HorizontalBars)
	{
		layer->setAxisTitle(0, tr("Y Axis Title"));
		layer->setAxisTitle(1, tr("X Axis Title"));
	}
}


Graph* ApplicationWindow::multilayerPlot(const QString& caption)
{
	Graph* ml = new Graph("", ws, 0);
	ml->setAttribute(Qt::WA_DeleteOnClose);
	QString label = caption;
	initMultilayerPlot(ml, label.replace(QRegExp("_"),"-"));
	return ml;
}

Graph* ApplicationWindow::newGraph(const QString& caption)
{
	Graph *ml = multilayerPlot(generateUniqueName(caption));
	if (ml)
    {
        Layer *layer = ml->addLayer();
		  setPreferences(layer);
        layer->newLegend();
        layer->setAutoscaleFonts(false);
        layer->setIgnoreResizeEvents(false);
        ml->arrangeLayers(false, false);
        ml->adjustSize();
        layer->setAutoscaleFonts(autoScaleFonts);//restore user defined fonts behaviour
        layer->setIgnoreResizeEvents(!autoResizeLayers);
        customMenu(ml);
    }
	return ml;
}

Graph* ApplicationWindow::multilayerPlot(Table* w, const QStringList& colList, int style, int startRow, int endRow)
{//used when plotting selected columns
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	Graph* g = new Graph("", ws, 0);
	g->setAttribute(Qt::WA_DeleteOnClose);

	Layer *layer = g->addLayer();
	if (!layer)
		return 0;

	setPreferences(layer);
	layer->insertCurvesList(w, colList, style, defaultCurveLineWidth, defaultSymbolSize, startRow, endRow);

	initMultilayerPlot(g, generateUniqueName(tr("Graph")));

	polishLayer(layer, style);
	layer->newLegend();
	g->arrangeLayers(false, false);
	customMenu(g);

	emit modified();
	QApplication::restoreOverrideCursor();
	return g;
}

Graph* ApplicationWindow::multilayerPlot(int c, int r, int style)
{//used when plotting from the panel menu
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return 0;

	Table* w = (Table*)ws->activeWindow();
	if (!validFor2DPlot(w))
		return 0;

	QStringList list=w->selectedYColumns();
	if((int)list.count() < 1)
	{
		QMessageBox::warning(this, tr("Plot error"), tr("Please select a Y column to plot!"));
		return 0;
	}

	int curves = (int)list.count();
	if (r<0)
		r = curves;

	Graph* g = new Graph("", ws, 0);
	g->setAttribute(Qt::WA_DeleteOnClose);
	initMultilayerPlot(g, generateUniqueName(tr("Graph")));
	int layers = c*r;
	if (curves<layers)
	{
		for (int i=0; i<curves; i++)
		{
			Layer *layer = g->addLayer();
			if (layer)
			{
				setPreferences(layer);
				layer->insertCurvesList(w, QStringList(list[i]), style, defaultCurveLineWidth, defaultSymbolSize);
				layer->newLegend();
				layer->setAutoscaleFonts(false);//in order to avoid to small fonts
                layer->setIgnoreResizeEvents(false);
				polishLayer(layer, style);
			}
		}
	}
	else
	{
		for (int i=0; i<layers; i++)
		{
			Layer *layer = g->addLayer();
			if (layer)
			{
				QStringList lst;
				lst << list[i];
				setPreferences(layer);
				layer->insertCurvesList(w, lst, style, defaultCurveLineWidth, defaultSymbolSize);
				layer->newLegend();
				layer->setAutoscaleFonts(false);//in order to avoid to small fonts
                layer->setIgnoreResizeEvents(false);
				polishLayer(layer, style);
			}
		}
	}
	g->setRows(r);
	g->setCols(c);
	g->arrangeLayers(false, false);
    g->adjustSize();
    QList<Layer *> lst = g->layers();
	foreach(Layer *layer, lst)
    {
        layer->setAutoscaleFonts(autoScaleFonts);//restore user defined fonts behaviour
        layer->setIgnoreResizeEvents(!autoResizeLayers);
    }
	customMenu(g);
	emit modified();
	return g;
}

Graph* ApplicationWindow::multilayerPlot(const QStringList& colList)
{//used when plotting from wizard
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	Graph* g = new Graph("", ws, 0);
	g->setAttribute(Qt::WA_DeleteOnClose);
	Layer *layer = g->addLayer();
	setPreferences(layer);
	polishLayer(layer, defaultCurveStyle);
	int curves = (int)colList.count();
	int errorBars = 0;
	for (int i=0; i<curves; i++)
	{
		if (colList[i].contains("(yErr)") || colList[i].contains("(xErr)"))
			errorBars++;
	}

	for (int i=0; i<curves; i++)
	{
		QString s = colList[i];
		int pos = s.find(":", 0);
		QString caption = s.left(pos) + "_";
		Table *w = (Table *)table(caption);

		int posX = s.find("(X)", pos);
		QString xColName = caption+s.mid(pos+2, posX-pos-2);
		int xCol=w->colIndex(xColName);

		posX = s.find(",", posX);
		int posY = s.find("(Y)", posX);
		QString yColName = caption+s.mid(posX+2, posY-posX-2);

		if (s.contains("(yErr)") || s.contains("(xErr)"))
		{
			posY = s.find(",", posY);
			int posErr, errType;
			if (s.contains("(yErr)"))
			{
				errType = ErrorCurve::Vertical;
				posErr = s.find("(yErr)", posY);
			}
			else
			{
				errType = ErrorCurve::Horizontal;
				posErr = s.find("(xErr)",posY);
			}

			QString errColName = caption+s.mid(posY+2, posErr-posY-2);
			layer->addErrorBars(xColName, yColName, w, errColName, errType);
		}
		else
            layer->insertCurve(w, xCol, yColName, defaultCurveStyle);

        CurveLayout cl = layer->initCurveLayout(defaultCurveStyle, curves - errorBars);
        cl.lWidth = defaultCurveLineWidth;
        cl.sSize = defaultSymbolSize;
        layer->updateCurveLayout(i, &cl);
	}
	layer->newLegend();
	layer->updatePlot();
    initMultilayerPlot(g, generateUniqueName(tr("Graph")));
    g->arrangeLayers(true, false);
	customMenu(g);
	emit modified();
	QApplication::restoreOverrideCursor();
	return g;
}

void ApplicationWindow::initMultilayerPlot(Graph* g, const QString& name)
{
	QString label = name;
	while(alreadyUsedName(label))
		label = generateUniqueName(tr("Graph"));

	current_folder->addWindow(g);
	g->setFolder(current_folder);

	ws->addWindow(g);
	connectMultilayerPlot(g);

	g->setWindowTitle(label);
	g->setName(label);
	g->setIcon(QPixmap(":/graph.xpm"));
	g->setScaleLayersOnPrint(d_scale_plots_on_print);
	g->printCropmarks(d_print_cropmarks);
	g->show();
	g->setFocus();

	addListViewItem(g);
}

void ApplicationWindow::newWrksheetPlot(const QString& caption, int r, int c, const QString& text)
{
	Table* w = newTable(caption, r, c, text);
	Graph* plot=multilayerPlot(w, QStringList(QString(w->name())+"_intensity"), 0);
	Layer *layer = plot->activeLayer();
	if (layer)
	{
		layer->setTitle("");
		layer->setXAxisTitle(tr("pixels"));
		layer->setYAxisTitle(tr("pixel intensity (a.u.)"));
	}
}

void ApplicationWindow::addErrorBars()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Layer* layer = plot->activeLayer();
	if (!layer)
        return;

    if (!layer->curveCount())
	{
		QMessageBox::warning(this, tr("Warning"), tr("There are no curves available on this plot!"));
		return;
	}

	if (layer->isPiePlot())
	{
        QMessageBox::warning(this, tr("Warning"), tr("This functionality is not available for pie plots!"));
        return;
	}

    ErrDialog* ed = new ErrDialog(this);
    ed->setAttribute(Qt::WA_DeleteOnClose);
    connect (ed,SIGNAL(options(const QString&,int,const QString&,int)),this,SLOT(defineErrorBars(const QString&,int,const QString&,int)));
    connect (ed,SIGNAL(options(const QString&,const QString&,int)),this,SLOT(defineErrorBars(const QString&,const QString&,int)));

    ed->setCurveNames(layer->analysableCurvesList());
    ed->setSrcTables(tableList());
    ed->exec();
}

void ApplicationWindow::defineErrorBars(const QString& name, int type, const QString& percent, int direction)
{
    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* layer = ((Graph*)ws->activeWindow())->activeLayer();
	if (!layer)
		return;

	Table *w = table(name);
	if (!w)
	{ //user defined function
		QMessageBox::critical(this,tr("Error bars error"),
				tr("This feature is not available for user defined function curves!"));
		return;
	}

	DataCurve *master_curve = (DataCurve *)layer->curve(name);
	QString xColName = master_curve->xColumnName();
	if (xColName.isEmpty())
		return;

	if (direction == ErrorCurve::Horizontal)
		w->addCol(SciDAVis::xErr);
	else
		w->addCol(SciDAVis::yErr);

	int r=w->rowCount();
	int c=w->columnCount()-1;
	int ycol=w->colIndex(name);
	if (!direction)
		ycol=w->colIndex(xColName);

	QVarLengthArray<double> Y(r);
	Y=w->col(ycol);
	QString errColName=w->colName(c);

	double prc=percent.toDouble();
	double moyenne=0.0;
	if (type==0)
	{
		for (int i=0;i<r;i++)
		{
			if (!w->text(i,ycol).isEmpty())
				w->setText(i,c,QString::number(Y[i]*prc/100.0,'g',15));
		}
	}
	else if (type==1)
	{
		int i;
		double dev=0.0;
		for (i=0;i<r;i++)
			moyenne+=Y[i];
		moyenne/=r;
		for (i=0;i<r;i++)
			dev+=(Y[i]-moyenne)*(Y[i]-moyenne);
		dev=sqrt(dev/(r-1));
		for (i=0;i<r;i++)
		{
			if (!w->text(i,ycol).isEmpty())
				w->setText(i,c,QString::number(dev,'g',15));
		}
	}
	layer->addErrorBars(xColName, name, w, errColName, direction);
}



void ApplicationWindow::defineErrorBars(const QString& curveName, const QString& errColumnName, int direction)
{
	Table *w=table(curveName);
	if (!w)
	{//user defined function --> no worksheet available
		QMessageBox::critical(this,tr("Error"),
				tr("This feature is not available for user defined function curves!"));
		return;
	}

	Table *errTable=table(errColumnName);
	if (w->rowCount() != errTable->rowCount())
	{
		QMessageBox::critical(this,tr("Error"),
				tr("The selected columns have different numbers of rows!"));

		addErrorBars();
		return;
	}

	int errCol=errTable->colIndex(errColumnName);
	if (errTable->isEmptyColumn(errCol))
	{
		QMessageBox::critical(this,tr("Error"),
				tr("The selected error column is empty!"));
		addErrorBars();
		return;
	}

	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* layer = ((Graph*)ws->activeWindow())->activeLayer();
	if (!layer)
		return;

	layer->addErrorBars(curveName, errTable, errColumnName, direction);
	emit modified();
}


void ApplicationWindow::removeCurves(const QString& name)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->inherits("Graph"))
		{
			QList<Layer *> lst= ((Graph*)w)->layers();
			foreach(Layer *layer, lst)
                layer->removeCurves(name);
		}
		else if (w->inherits("Graph3D"))
		{
			if ( (((Graph3D*)w)->formula()).contains(name) )
				((Graph3D*)w)->clearData();
		}
	}
	delete windows;
	QApplication::restoreOverrideCursor();
}

void ApplicationWindow::updateCurves(Table *t, const QString& name)
{
	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->inherits("Graph"))
		{
			QList<Layer *> layers_list = ((Graph*)w)->layers();
			foreach(Layer *layer, layers_list)
			{
                if (layer)
                    layer->updateCurvesData(t, name);
			}
		}
		else if (w->inherits("Graph3D"))
		{
			Graph3D* g = (Graph3D*)w;
			if ((g->formula()).contains(name))
				g->updateData(t);
		}
	}
	delete windows;
}

void ApplicationWindow::exportGraph()
{
	QWidget *w = ws->activeWindow();
	if (!w)
		return;

	Graph *plot2D = 0;
	Graph3D *plot3D = 0;
	if(w->inherits("Graph"))
	{
		plot2D = (Graph*)w;
		if (plot2D->isEmpty())
		{
			QMessageBox::critical(this, tr("Export Error"),
					tr("<h4>There are no plot layers available in this window!</h4>"));
			return;
		}
	}
	else if (w->inherits("Graph3D"))
		plot3D = (Graph3D*)w;
	else
		return;

	ImageExportDialog *ied = new ImageExportDialog(this, plot2D!=NULL, d_extended_export_dialog);

	ied->setDir(workingDir);
	ied->selectFilter(d_image_export_filter);
	ied->setResolution(d_export_resolution);
	ied->setColorEnabled(d_export_color);
	ied->setPageSize(d_export_vector_size);
	ied->setKeepAspect(d_keep_plot_aspect);
	ied->setQuality(d_export_quality);
	ied->setTransparency(d_export_transparency);

	if ( ied->exec() != QDialog::Accepted )
		return;

	d_extended_export_dialog = ied->isExtended();
	workingDir = ied->directory().path();
	d_image_export_filter = ied->selectedFilter();
	d_export_resolution = ied->resolution();
	d_export_color = ied->colorEnabled();
	d_export_vector_size = ied->pageSize();
	d_keep_plot_aspect = ied->keepAspect();
	d_export_quality = ied->quality();
	d_export_transparency = ied->transparency();

	if (ied->selectedFiles().isEmpty())
		return;

	QString selected_filter = ied->selectedFilter();
	QString file_name = ied->selectedFiles()[0];
	QFileInfo file_info(file_name);
	if (!file_info.fileName().contains("."))
		file_name.append(selected_filter.remove("*"));

	QFile file(file_name);
	if ( !file.open( QIODevice::WriteOnly ) )
	{
		QMessageBox::critical(this, tr("Export Error"),
				tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(file_name));
		return;
	}

	if (selected_filter.contains(".eps") || selected_filter.contains(".pdf") || selected_filter.contains(".ps")) {
		if (plot3D)
			plot3D->exportVector(file_name, selected_filter.remove("*."));
		else if (plot2D)
			plot2D->exportVector(file_name, ied->resolution(), ied->colorEnabled(), ied->keepAspect(), ied->pageSize());
	} else if (selected_filter.contains(".svg")) {
		if (plot2D)
			plot2D->exportSVG(file_name);
	} else {
		QList<QByteArray> list = QImageWriter::supportedImageFormats();
		for (int i=0; i<(int)list.count(); i++)
		{
			if (selected_filter.contains("." + (list[i]).toLower())) {
				if (plot2D)
					plot2D->exportImage(file_name, ied->quality(), ied->transparency());
				else if (plot3D)
					plot3D->exportImage(file_name, ied->quality(), ied->transparency());
			}
		}
	}
}

void ApplicationWindow::exportLayer()
{
	QWidget *w=ws->activeWindow();
	if (!w || !w->inherits("Graph"))
		return;

	Layer* layer = ((Graph*)w)->activeLayer();
	if (!layer)
		return;

	ImageExportDialog *ied = new ImageExportDialog(this, layer!=NULL, d_extended_export_dialog);

	ied->setDir(workingDir);
	ied->selectFilter(d_image_export_filter);
	ied->setResolution(d_export_resolution);
	ied->setColorEnabled(d_export_color);
	ied->setPageSize(d_export_vector_size);
	ied->setKeepAspect(d_keep_plot_aspect);
	ied->setQuality(d_export_quality);
	ied->setTransparency(d_export_transparency);

	if ( ied->exec() != QDialog::Accepted )
		return;

	d_extended_export_dialog = ied->isExtended();
	workingDir = ied->directory().path();
	d_image_export_filter = ied->selectedFilter();
	d_export_resolution = ied->resolution();
	d_export_color = ied->colorEnabled();
	d_export_vector_size = ied->pageSize();
	d_keep_plot_aspect = ied->keepAspect();
	d_export_quality = ied->quality();
	d_export_transparency = ied->transparency();

	if (ied->selectedFiles().isEmpty())
		return;

	QString selected_filter = ied->selectedFilter();
	QString file_name = ied->selectedFiles()[0];
	QFileInfo file_info(file_name);
	if (!file_info.fileName().contains("."))
		file_name.append(selected_filter.remove("*"));

	QFile file(file_name);
	if ( !file.open( QIODevice::WriteOnly ) )
	{
		QMessageBox::critical(this, tr("Export Error"),
				tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(file_name));
		return;
	}

	if (selected_filter.contains(".eps") || selected_filter.contains(".pdf") || selected_filter.contains(".ps"))
		layer->exportVector(file_name, ied->resolution(), ied->colorEnabled(), ied->keepAspect(), ied->pageSize());
	else if (selected_filter.contains(".svg"))
		layer->exportSVG(file_name);
	else {
		QList<QByteArray> list = QImageWriter::supportedImageFormats();
		for (int i=0; i<(int)list.count(); i++)
			if (selected_filter.contains("."+(list[i]).toLower()))
				layer->exportImage(file_name, ied->quality(), ied->transparency());
	}
}

void ApplicationWindow::exportAllGraphs()
{
	ImageExportDialog *ied = new ImageExportDialog(this, true, d_extended_export_dialog);
	ied->setWindowTitle(tr("Choose a directory to export the graphs to"));
	QStringList tmp = ied->filters();
	ied->setFileMode(QFileDialog::Directory);
	ied->setFilters(tmp);
	ied->setLabelText(QFileDialog::FileType, tr("Output format:"));
	ied->setLabelText(QFileDialog::FileName, tr("Directory:"));

	ied->setDir(workingDir);
	ied->selectFilter(d_image_export_filter);
	ied->setResolution(d_export_resolution);
	ied->setColorEnabled(d_export_color);
	ied->setPageSize(d_export_vector_size);
	ied->setKeepAspect(d_keep_plot_aspect);
	ied->setQuality(d_export_quality);
	ied->setTransparency(d_export_transparency);

	if ( ied->exec() != QDialog::Accepted )
		return;

	d_extended_export_dialog = ied->isExtended();
	workingDir = ied->directory().path();
	d_image_export_filter = ied->selectedFilter();
	d_export_resolution = ied->resolution();
	d_export_color = ied->colorEnabled();
	d_export_vector_size = ied->pageSize();
	d_keep_plot_aspect = ied->keepAspect();
	d_export_quality = ied->quality();
	d_export_transparency = ied->transparency();

	if (ied->selectedFiles().isEmpty())
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QString output_dir = ied->selectedFiles()[0];
	QString file_suffix = ied->selectedFilter();
	file_suffix.toLower();
	file_suffix.remove("*");

	QWidgetList *windows = windowsList();
	bool confirm_overwrite = true;
	Graph *plot2D;
	Graph3D *plot3D;

	foreach (QWidget *w, *windows)
	{
		if (w->inherits("Graph")) {
			plot3D = 0;
			plot2D = (Graph *)w;
			if (plot2D->isEmpty()) {
				QApplication::restoreOverrideCursor();
				QMessageBox::warning(this, tr("Warning"),
						tr("There are no plot layers available in window <b>%1</b>.<br>"
							"Graph window not exported!").arg(plot2D->name()));
				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				continue;
			}
		} else if (w->inherits("Graph3D")) {
			plot2D = 0;
			plot3D = (Graph3D *)w;
		} else
			continue;

		QString file_name = output_dir + "/" + w->name() + file_suffix;
		QFile f(file_name);
		if (f.exists() && confirm_overwrite) {
			QApplication::restoreOverrideCursor();
			switch(QMessageBox::question(this, tr("Overwrite file?"),
						tr("A file called: <p><b>%1</b><p>already exists. "
							"Do you want to overwrite it?") .arg(file_name), tr("&Yes"), tr("&All"), tr("&Cancel"), 0, 1)) {
				case 1:
					confirm_overwrite = false;
				case 0:
					QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
					break;
				case 2:
					delete windows;
					return;
			}
		}
		if ( !f.open( QIODevice::WriteOnly ) ) {
			QApplication::restoreOverrideCursor();
			QMessageBox::critical(this, tr("Export Error"),
					tr("Could not write to file: <br><h4>%1</h4><p>"
						"Please verify that you have the right to write to this location!").arg(file_name));
			delete windows;
			return;
		}
		if (file_suffix.contains(".eps") || file_suffix.contains(".pdf") || file_suffix.contains(".ps")) {
			if (plot3D)
				plot3D->exportVector(file_name, file_suffix.remove("."));
			else if (plot2D)
				plot2D->exportVector(file_name, ied->resolution(), ied->colorEnabled());
		} else if (file_suffix.contains(".svg")) {
			if (plot2D)
				plot2D->exportSVG(file_name);
		} else {
			QList<QByteArray> list = QImageWriter::supportedImageFormats();
			for (int i=0; i<(int)list.count(); i++)
			{
				if (file_suffix.contains("." + (list[i]).toLower())) {
					if (plot2D)
						plot2D->exportImage(file_name, ied->quality(), ied->transparency());
					else if (plot3D)
						plot3D->exportImage(file_name, ied->quality(), ied->transparency());
				}
			}
		}
	}

	delete windows;
	QApplication::restoreOverrideCursor();
}

void ApplicationWindow::showCurvesDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	if (((Graph*)ws->activeWindow())->isEmpty()){
		QMessageBox::warning(this,tr("Error"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Layer* layer = ((Graph*)ws->activeWindow())->activeLayer();
	if (!layer)
		return;

	if (layer->isPiePlot()){
		QMessageBox::warning(this,tr("Error"),
				tr("This functionality is not available for pie plots!"));
	} else {
		CurvesDialog* crvDialog = new CurvesDialog(this);
		crvDialog->setAttribute(Qt::WA_DeleteOnClose);
		crvDialog->setLayer(layer);
		crvDialog->resize(d_add_curves_dialog_size);
		crvDialog->show();
	}
}


void ApplicationWindow::showPlotAssociations(int curve)
{
	QWidget *w = ws->activeWindow();
	if (!w || !w->inherits("Graph"))
		return;

	Layer *g = ((Graph*)w)->activeLayer();
	if (!g)
		return;

	AssociationsDialog* ad=new AssociationsDialog(this, Qt::WindowStaysOnTopHint);
	ad->setAttribute(Qt::WA_DeleteOnClose);
	ad->setLayer(g);
	ad->initTablesList(tableList(), curve);
	ad->exec();
}



void ApplicationWindow::showTitleDialog()
{
	QWidget *w = ws->activeWindow();
	if (!w)
		return;

	if (w->inherits("Graph"))
	{
		Layer* g = ((Graph*)w)->activeLayer();
		if (g)
		{
			TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,0);
			td->setAttribute(Qt::WA_DeleteOnClose);
			connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setTitleFont(const QFont &)));
			connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setTitle(const QString &)));
			connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setTitleColor(const QColor &)));
			connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setTitleAlignment(int)));
			connect(td, SIGNAL(defaultValues(int,const QFont&,const QColor&,const QColor&)),
					this, SLOT(setLegendDefaultSettings(int,const QFont&,const QColor&,const QColor&)));

			QwtText t = g->plotWidget()->title();
			td->setText(t.text());
			td->setFont(t.font());
			td->setTextColor(t.color());
			td->setAlignment(t.renderFlags());
			td->exec();
		}
	}
	else if (w->inherits("Graph3D"))
	{
		PlotDialog3D* pd = (PlotDialog3D*)showPlot3dDialog();
		if (pd)
			pd->showTitleTab();
		delete pd;
	}
}


void ApplicationWindow::showXAxisTitleDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if (g)
	{
		TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,0);
		td->setAttribute(Qt::WA_DeleteOnClose);
		connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setXAxisTitleFont(const QFont &)));
		connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setXAxisTitle(const QString &)));
		connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setXAxisTitleColor(const QColor &)));
		connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setXAxisTitleAlignment(int)));
		connect(td, SIGNAL(defaultValues(int,const QFont&,const QColor&,const QColor&)),
				this, SLOT(setLegendDefaultSettings(int,const QFont&,const QColor&,const QColor&)));

		QStringList t=g->scalesTitles();
		td->setText(t[0]);
		td->setFont(g->axisTitleFont(2));
		td->setTextColor(g->axisTitleColor(2));
		td->setAlignment(g->axisTitleAlignment(2));
		td->setWindowTitle(tr("X Axis Title"));
		td->exec();
	}
}

void ApplicationWindow::showYAxisTitleDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if (g)
	{
		TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,0);
		td->setAttribute(Qt::WA_DeleteOnClose);
		connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setYAxisTitleFont(const QFont &)));
		connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setYAxisTitle(const QString &)));
		connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setYAxisTitleColor(const QColor &)));
		connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setYAxisTitleAlignment(int)));
		connect(td, SIGNAL(defaultValues(int,const QFont&,const QColor&,const QColor&)),
				this, SLOT(setLegendDefaultSettings(int,const QFont&,const QColor&,const QColor&)));

		QStringList t=g->scalesTitles();
		td->setText(t[1]);
		td->setFont(g->axisTitleFont(0));
		td->setTextColor(g->axisTitleColor(0));
		td->setAlignment(g->axisTitleAlignment(0));
		td->setWindowTitle(tr("Y Axis Title"));
		td->exec();
	}
}


void ApplicationWindow::showYAxisTitleDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if (g)
	{
		TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,0);
		td->setAttribute(Qt::WA_DeleteOnClose);
		connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setYAxisTitleFont(const QFont &)));
		connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setYAxisTitle(const QString &)));
		connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setYAxisTitleColor(const QColor &)));
		connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setYAxisTitleAlignment(int)));
		connect(td, SIGNAL(defaultValues(int,const QFont&,const QColor&,const QColor&)),
				this, SLOT(setLegendDefaultSettings(int,const QFont&,const QColor&,const QColor&)));

		QStringList t=g->scalesTitles();
		td->setText(t[1]);
		td->setFont(g->axisTitleFont(0));
		td->setTextColor(g->axisTitleColor(0));
		td->setAlignment(g->axisTitleAlignment(0));
		td->setWindowTitle(tr("Y Axis Title"));
		td->exec();
	}
}

void ApplicationWindow::plot2VerticalLayers()
{
	multilayerPlot(1, 2, defaultCurveStyle);
}

void ApplicationWindow::plot2HorizontalLayers()
{
	multilayerPlot(2, 1, defaultCurveStyle);
}

void ApplicationWindow::plot4Layers()
{
	multilayerPlot(2, 2, defaultCurveStyle);
}

void ApplicationWindow::plotStackedLayers()
{
	multilayerPlot(1, -1, defaultCurveStyle);
}

void ApplicationWindow::plotStackedHistograms()
{
	multilayerPlot(1, -1, Layer::Histogram);
}

void ApplicationWindow::showAxis(int axis, int type, const QString& labelsColName, bool axisOn,
		int majTicksType, int minTicksType, bool labelsOn, const QColor& c, int format,
		int prec, int rotation, int baselineDist, const QString& formula, const QColor& labelsColor)
{
	Table *w = table(labelsColName);
	if ((type == Layer::Txt || type == Layer::ColHeader) && !w)
		return;

    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if (!g)
		return;

	g->showAxis(axis, type, labelsColName, w, axisOn, majTicksType, minTicksType, labelsOn,
			c, format, prec, rotation, baselineDist, formula, labelsColor);
}

void ApplicationWindow::showGeneralPlotDialog()
{
	QWidget* plot = ws->activeWindow();
	if (!plot)
		return;

	if (plot->inherits("Graph") && ((Graph*)plot)->layerCount())
		showPlotDialog();
	else if (plot->inherits("Graph3D"))
	{
	    QDialog* gd = showScaleDialog();
		((PlotDialog3D*)gd)->showGeneralTab();
	}
}

void ApplicationWindow::showAxisDialog()
{
	QWidget* plot = (QWidget*)ws->activeWindow();
	if (!plot)
		return;

	QDialog* gd = showScaleDialog();
	if (gd && plot->inherits("Graph") && ((Graph*)plot)->layerCount())
		((AxesDialog*)gd)->showAxesPage();
	else if (gd && plot->inherits("Graph3D"))
		((PlotDialog3D*)gd)->showAxisTab();
}

void ApplicationWindow::showGridDialog()
{
	AxesDialog* gd = (AxesDialog*)showScaleDialog();
	if (gd)
		gd->showGridPage();
}

QDialog* ApplicationWindow::showScaleDialog()
{
	QWidget *w = ws->activeWindow();
	if (!w)
		return 0;

	if (w->inherits("Graph"))
	{
		if (((Graph*)w)->isEmpty())
			return 0;

		Layer* g = ((Graph*)w)->activeLayer();
		AxesDialog* ad = new AxesDialog(this);
        connect (ad,SIGNAL(updateAxisTitle(int,const QString&)),g,SLOT(setAxisTitle(int,const QString&)));
        connect (ad,SIGNAL(changeAxisFont(int, const QFont &)),g,SLOT(setAxisFont(int,const QFont &)));
        connect (ad,SIGNAL(showAxis(int, int, const QString&, bool,int, int, bool,const QColor&,int, int, int, int, const QString&, const QColor&)),
					this, SLOT(showAxis(int,int, const QString&, bool, int, int, bool,const QColor&, int, int, int, int, const QString&, const QColor&)));

        ad->setLayer(g);
        ad->insertColList(columnsList());
        ad->insertTablesList(tableWindows);
        ad->setAxesLabelsFormatInfo(g->axesLabelsFormatInfo());
        ad->setEnabledAxes(g->enabledAxes());
        ad->setAxesType(g->axesType());
        ad->setAxesBaseline(g->axesBaseline());

        ad->initAxisFonts(g->axisFont(2), g->axisFont(0),g->axisFont(3),g->axisFont(1));
        ad->setAxisTitles(g->scalesTitles());
        ad->updateTitleBox(0);
        ad->putGridOptions(g->gridOptions());
        ad->setTicksType(g->plotWidget()->getMajorTicksType(), g->plotWidget()->getMinorTicksType());
        ad->setEnabledTickLabels(g->enabledTickLabels());
        ad->initLabelsRotation(g->labelsRotation(QwtPlot::xBottom), g->labelsRotation(QwtPlot::xTop));
        ad->exec();
        return ad;
	}
	else if (w->inherits("Graph3D"))
		return showPlot3dDialog();

	return 0;
}

AxesDialog* ApplicationWindow::showScalePageFromAxisDialog(int axisPos)
{
	AxesDialog* gd = (AxesDialog*)showScaleDialog();
	if (gd)
		gd->setCurrentScale(axisPos);

	return gd;
}

AxesDialog* ApplicationWindow::showAxisPageFromAxisDialog(int axisPos)
{
	AxesDialog* gd = (AxesDialog*)showScaleDialog();
	if (gd)
	{
		gd->showAxesPage();
		gd->setCurrentScale(axisPos);
	}
	return gd;
}


void ApplicationWindow::showPlotDialog(int curveKey)
{
	QWidget *w = ws->activeWindow();
	if (!w)
		return;

	if (w->inherits("Graph"))
	{
		PlotDialog* pd = new PlotDialog(d_extended_plot_dialog, this);
        pd->setAttribute(Qt::WA_DeleteOnClose);
        pd->setGraph((Graph*)w);
        if (curveKey >= 0)
		{
			Layer *g = ((Graph*)w)->activeLayer();
			if (g)
            	pd->selectCurve(g->curveIndex(curveKey));
		}
        pd->initFonts(plotTitleFont, plotAxesFont, plotNumbersFont, plotLegendFont);
		pd->showAll(d_extended_plot_dialog);
        pd->show();
	}
}

void ApplicationWindow::showCurvePlotDialog()
{
	showPlotDialog(actionShowCurvePlotDialog->data().toInt());
}

void ApplicationWindow::showCurveContextMenu(int curveKey)
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer *layer = ((Graph*)ws->activeWindow())->activeLayer();
	DataCurve *c = (DataCurve *)layer->curve(layer->curveIndex(curveKey));
	if (!c || !c->isVisible())
		return;

	QMenu curveMenu(this);
	curveMenu.addAction(c->title().text(), this, SLOT(showCurvePlotDialog()));
	curveMenu.addSeparator();

	curveMenu.addAction(actionHideCurve);
	actionHideCurve->setData(curveKey);

    if (layer->visibleCurves() > 1 && c->type() == Layer::Function)
    {
        curveMenu.addAction(actionHideOtherCurves);
        actionHideOtherCurves->setData(curveKey);
    }
    else if (c->type() != Layer::Function)
    {
        if ((layer->visibleCurves() - c->errorBarsList().count()) > 1)
        {
            curveMenu.addAction(actionHideOtherCurves);
            actionHideOtherCurves->setData(curveKey);
        }
    }

	if (layer->visibleCurves() != layer->curveCount())
		curveMenu.addAction(actionShowAllCurves);
	curveMenu.addSeparator();

	if (c->type() == Layer::Function)
	{
		curveMenu.addAction(actionEditFunction);
		actionEditFunction->setData(curveKey);
	}
	else if (c->type() != Layer::ErrorBars)
	{
		curveMenu.addAction(actionEditCurveRange);
		actionEditCurveRange->setData(curveKey);

		curveMenu.addAction(actionCurveFullRange);
		if (c->isFullRange())
			actionCurveFullRange->setDisabled(true);
		else
			actionCurveFullRange->setEnabled(true);
		actionCurveFullRange->setData(curveKey);

		curveMenu.addSeparator();
	}

	curveMenu.addAction(actionShowCurveWorksheet);
	actionShowCurveWorksheet->setData(curveKey);

	curveMenu.addAction(actionShowCurvePlotDialog);
	actionShowCurvePlotDialog->setData(curveKey);

	curveMenu.addSeparator();

	curveMenu.addAction(actionRemoveCurve);
	actionRemoveCurve->setData(curveKey);
	curveMenu.exec(QCursor::pos());
}

void ApplicationWindow::showAllCurves()
{
    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* layer = ((Graph*)ws->activeWindow())->activeLayer();
	if (!layer)
		return;

	for(int i=0; i< layer->curveCount(); i++)
		layer->showCurve(i);
	layer->replot();
}

void ApplicationWindow::hideOtherCurves()
{
    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* layer = ((Graph*)ws->activeWindow())->activeLayer();
	if (!layer)
		return;

	int curveKey = actionHideOtherCurves->data().toInt();
	for(int i=0; i< layer->curveCount(); i++)
		layer->showCurve(i, false);

	layer->showCurve(layer->curveIndex(curveKey));
	layer->replot();
}

void ApplicationWindow::hideCurve()
{
    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if (!g)
		return;

	int curveKey = actionHideCurve->data().toInt();
	g->showCurve(g->curveIndex(curveKey), false);
}

void ApplicationWindow::removeCurve()
{
    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if (!g)
		return;

	int curveKey = actionRemoveCurve->data().toInt();
	g->removeCurve(g->curveIndex(curveKey));
	g->updatePlot();
}

void ApplicationWindow::zoomIn()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this, tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setOn(true);
		return;
	}

	if ((Layer*)plot->activeLayer()->isPiePlot())
	{
		if (btnZoomIn->isOn())
			QMessageBox::warning(this,tr("Warning"),
					tr("This functionality is not available for pie plots!"));
		btnPointer->setOn(true);
		return;
	}

	foreach(Layer *layer, plot->layers())
		if (!layer->isPiePlot())
			layer->zoom(true);
}

void ApplicationWindow::zoomOut()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty() || (Layer*)plot->activeLayer()->isPiePlot())
		return;

	((Layer*)plot->activeLayer())->zoomOut();
	btnPointer->setOn(true);
}

void ApplicationWindow::setAutoScale()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"));
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if ( g )
	{
		g->setAutoScale();
		emit modified();
	}
}

void ApplicationWindow::removePoints()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if (!g || !g->validCurvesDataSize())
	{
		btnPointer->setChecked(true);
		return;
	}

	if (g->isPiePlot())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("This functionality is not available for pie plots!"));
		btnPointer->setChecked(true);
		return;
	}
	else
	{
		switch(QMessageBox::warning (this,tr("SciDAVis"),
					tr("This will modify the data in the worksheets!\nAre you sure you want to continue?"),
					tr("Continue"),tr("Cancel"),0,1))
		{
			case 0:
				g->setActiveTool(new DataPickerTool(g, this, DataPickerTool::Remove, d_status_info, SLOT(setText(const QString&))));
				break;

			case 1:
				btnPointer->setChecked(true);
				break;
		}
	}
}

void ApplicationWindow::movePoints()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty()){
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if (!g || !g->validCurvesDataSize()){
		btnPointer->setChecked(true);
		return;
	}

	if (g->isPiePlot()){
		QMessageBox::warning(this, tr("Warning"),
				tr("This functionality is not available for pie plots!"));

		btnPointer->setChecked(true);
		return;
	} else {
		switch(QMessageBox::warning (this, tr("SciDAVis"),
					tr("This will modify the data in the worksheets!\nAre you sure you want to continue?"),
					tr("Continue"), tr("Cancel"), 0, 1))
		{
			case 0:
				if (g){
					g->setActiveTool(new DataPickerTool(g, this, DataPickerTool::Move, d_status_info, SLOT(setText(const QString&))));
				}
				break;

			case 1:
				btnPointer->setChecked(true);
				break;
		}
	}
}

void ApplicationWindow::exportPDF()
{
	QWidget* w = ws->activeWindow();
	if (!w)
		return;

	if (w->inherits("Graph") && ((Graph*)w)->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"));
		return;
	}

    QString fname = QFileDialog::getSaveFileName(this, tr("Choose a filename to save under"), workingDir, "*.pdf");
	if (!fname.isEmpty() )
	{
		QFileInfo fi(fname);
		QString baseName = fi.fileName();
		if (!baseName.contains("."))
			fname.append(".pdf");

        workingDir = fi.dirPath(true);

        QFile f(fname);
        if ( !f.open( QIODevice::WriteOnly ) )
        {
            QMessageBox::critical(this, tr("Export Error"),
            tr("Could not write to file: <h4>%1</h4><p>Please verify that you have the right to write to this location or that the file is not being used by another application!").arg(fname));
            return;
        }

		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        ((MyWidget*)w)->exportPDF(fname);

		QApplication::restoreOverrideCursor();
	}
}

void ApplicationWindow::showScreenReader()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	foreach(Layer *layer, plot->layers())
		layer->setActiveTool(new ScreenPickerTool(layer, d_status_info, SLOT(setText(const QString&))));

}

void ApplicationWindow::showRangeSelectors()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this, tr("Warning"),
				tr("There are no plot layers available in this window!"));
		btnPointer->setChecked(true);
		return;
	}

	Layer* layer = plot->activeLayer();
	if (!layer)
		return;

	if (!layer->curveCount())
	{
		QMessageBox::warning(this, tr("Warning"),
				tr("There are no curves available on this plot!"));
		btnPointer->setChecked(true);
		return;
	}
	else if (layer->isPiePlot())
	{
		QMessageBox::warning(this, tr("Warning"),
				tr("This functionality is not available for pie plots!"));
		btnPointer->setChecked(true);
		return;
	}

	layer->enableRangeSelectors(d_status_info, SLOT(setText(const QString&)));
}

void ApplicationWindow::showCursor()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	if ((Layer*)plot->activeLayer()->isPiePlot())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("This functionality is not available for pie plots!"));

		btnPointer->setChecked(true);
		return;
	}

	foreach(Layer *layer, plot->layers())
		if (!layer->isPiePlot() && layer->validCurvesDataSize())
			layer->setActiveTool(new DataPickerTool(layer, this, DataPickerTool::Display, d_status_info, SLOT(setText(const QString&))));

}

void ApplicationWindow::newLegend()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if ( g )
		g->newLegend();
}

void ApplicationWindow::addTimeStamp()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if ( g )
		g->addTimeStamp();
}

void ApplicationWindow::addText()
{
	if (!btnPointer->isOn())
		btnPointer->setOn(TRUE);

	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();

	switch(QMessageBox::information(this,
				tr("Add new layer?"),
				tr("Do you want to add the text on a new layer or on the active layer?"),
				tr("On &New Layer"), tr("On &Active Layer"), tr("&Cancel"),
				0, 2 ) )
	{
		case 0:
			plot->addTextLayer(legendFrameStyle, plotLegendFont, legendTextColor, legendBackground);
			break;

		case 1:
			{
				if (plot->isEmpty())
				{
					QMessageBox::warning(this,tr("Warning"),
							tr("<h4>There are no plot layers available in this window.</h4>"
								"<p><h4>Please add a layer and try again!</h4>"));

					actionAddText->setChecked(false);
					return;
				}

				Layer *g = (Layer*)plot->activeLayer();
				if (g)
					g->drawText(true);
			}
			break;

		case 2:
			actionAddText->setChecked(false);
			return;
			break;
	}
}

void ApplicationWindow::addImage()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty()){
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if (!g)
		return;
		
	QList<QByteArray> list = QImageReader::supportedImageFormats();
	QString filter = tr("Images") + " (", aux1, aux2;
	for (int i=0; i<(int)list.count(); i++){
		aux1 = " *."+list[i]+" ";
		aux2 += " *."+list[i]+";;";
		filter += aux1;
	}
	filter+=");;" + aux2;

	QString fn = QFileDialog::getOpenFileName(this, tr("Insert image from file"), imagesDirPath, filter);
	if ( !fn.isEmpty() ){
		QFileInfo fi(fn);
		imagesDirPath = fi.dirPath(true);

		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		g->addImage(fn);
		QApplication::restoreOverrideCursor();
	}
}

void ApplicationWindow::drawLine()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));

		btnPointer->setChecked(true);
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if (g)
	{
		g->drawLine(true);
		emit modified();
	}
}

void ApplicationWindow::drawArrow()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));

		btnPointer->setOn(true);
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if (g)
	{
		g->drawLine(true, 1);
		emit modified();
	}
}

void ApplicationWindow::showImageEnrichmentDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if (g)
	{
		ImageEnrichment *im = (ImageEnrichment *) g->selectedMarkerPtr();
		if (!im)
			return;

		ImageEnrichmentDialog *id = new ImageEnrichmentDialog(this);
		id->setAttribute(Qt::WA_DeleteOnClose);
		connect (id, SIGNAL(setGeometry(int, int, int, int)),
				g, SLOT(updateImageEnrichment(int, int, int, int)));
		id->setIcon(QPixmap(":/appicon"));
		id->setOrigin(im->origin());
		id->setSize(im->size());
		id->exec();
	}
}

void ApplicationWindow::showLayerDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	if(plot->isEmpty())
	{
		QMessageBox::warning(this, tr("Warning"),
				tr("There are no plot layers available in this window."));
		return;
	}

	LayerDialog *id=new LayerDialog(this);
	id->setAttribute(Qt::WA_DeleteOnClose);
	id->setGraph(plot);
	id->exec();
}

void ApplicationWindow::showPlotGeometryDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph*)ws->activeWindow();
	Layer* g = plot->activeLayer();
	if (g)
	{
		ImageEnrichmentDialog *id=new ImageEnrichmentDialog(this);
		id->setAttribute(Qt::WA_DeleteOnClose);
		connect (id, SIGNAL(setGeometry(int,int,int,int)), plot, SLOT(setLayerGeometry(int,int,int,int)));
		id->setIcon(QPixmap(":/appicon"));
		id->setWindowTitle(tr("Layer Geometry"));
		id->setOrigin(g->pos());
		id->setSize(g->plotWidget()->size());
		id->exec();
	}
}

void ApplicationWindow::showTextDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if ( g )
	{
		TextEnrichment *m = static_cast<TextEnrichment*>(g->selectedMarkerPtr());
		if (!m)
			return;

		TextDialog *td=new TextDialog(TextDialog::TextMarker, this, 0);
		td->setAttribute(Qt::WA_DeleteOnClose);
		connect (td,SIGNAL(values(const QString&,int,int,const QFont&, const QColor&, const QColor&)),
				g,SLOT(updateTextMarker(const QString&,int,int,const QFont&, const QColor&, const QColor&)));
		connect(td, SIGNAL(defaultValues(int,const QFont&,const QColor&,const QColor&)),
				this, SLOT(setLegendDefaultSettings(int,const QFont&,const QColor&,const QColor&)));

		td->setIcon(QPixmap(":/appicon"));
		td->setText(m->text());
		td->setFont(m->font());
		td->setTextColor(m->textColor());
		td->setBackgroundColor(m->backgroundColor());
		td->setBackgroundType(m->frameStyle());
		td->setAngle(m->angle());
		td->exec();
	}
}

void ApplicationWindow::showLineEnrichmentDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if (g){
		LineEnrichment *lm = (LineEnrichment *) g->selectedMarkerPtr();
		if (!lm)
			return;

		LineEnrichmentDialog *ld = new LineEnrichmentDialog(lm, this, Qt::Tool);
		ld->setAttribute(Qt::WA_DeleteOnClose);
		ld->exec();
	}
}

void ApplicationWindow::clearSelection()
{
	if(lv->hasFocus())
	{
		deleteSelectedItems();
		return;
	}

	QWidget* m = (QWidget*)ws->activeWindow();
	if (!m)
		return;

	if (m->inherits("Table"))
		((Table*)m)->clearSelection();
	else if (m->inherits("Matrix"))
		((Matrix*)m)->clearSelection();
	else if (m->inherits("Graph"))
	{
		Layer* g = ((Graph*)m)->activeLayer();
		if (!g)
			return;

		if (g->titleSelected())
			g->removeTitle();
		else if (g->markerSelected())
			g->removeMarker();
	}
	else if (m->inherits("Note"))
		((Note*)m)->textWidget()->clear();
	emit modified();
}

void ApplicationWindow::copySelection()
{
	if(results->hasFocus())
	{
		results->copy();
		return;
	}

	QWidget* m = (QWidget*)ws->activeWindow();
	if (!m)
		return;

	if (m->inherits("Table"))
		((Table*)m)->copySelection();
	else if (m->inherits("Matrix"))
		((Matrix*)m)->copySelection();
	else if (m->inherits("Graph"))
	{
		Graph* plot = (Graph*)m;
		if (!plot || plot->layerCount() == 0)
			return;

		plot->copyAllLayers();
		Layer* g = (Layer*)plot->activeLayer();
		if (g && g->markerSelected())
			copyMarker();
		else
			copyActiveLayer();
	}
	else if (m->inherits("Note"))
		((Note*)m)->textWidget()->copy();
}

void ApplicationWindow::cutSelection()
{
	QWidget* m = (QWidget*)ws->activeWindow();
	if (!m)
		return;

	if (m->inherits("Table"))
		((Table*)m)->cutSelection();
	else if (m->inherits("Matrix"))
		((Matrix*)m)->cutSelection();
	else if(m->inherits("Graph"))
	{
		Graph* plot = (Graph*)m;
		if (!plot || plot->layerCount() == 0)
			return;

		Layer* g = (Layer*)plot->activeLayer();
		copyMarker();
		g->removeMarker();
	}
	else if (m->inherits("Note"))
		((Note*)m)->textWidget()->cut();

	emit modified();
}

void ApplicationWindow::copyMarker()
{
	QWidget* m = (QWidget*)ws->activeWindow();
	Graph* plot = (Graph*)m;
	Layer* g = (Layer*)plot->activeLayer();
	if (g && g->markerSelected())
	{
		g->copyMarker();
		copiedMarkerType=g->copiedMarkerType();
		QRect rect=g->copiedMarkerRect();
		auxMrkStart=rect.topLeft();
		auxMrkEnd=rect.bottomRight();

		if (copiedMarkerType == Layer::Text)
		{
			TextEnrichment *m = static_cast<TextEnrichment*>(g->selectedMarkerPtr());
			auxMrkText=m->text();
			auxMrkColor=m->textColor();
			auxMrkFont=m->font();
			auxMrkBkg=m->frameStyle();
			auxMrkBkgColor=m->backgroundColor();
		}
		else if (copiedMarkerType == Layer::Arrow)
		{
			LineEnrichment *m = (LineEnrichment *) g->selectedMarkerPtr();
			auxMrkWidth=m->width();
			auxMrkColor=m->color();
			auxMrkStyle=m->style();
			startArrowOn=m->hasStartArrow();
			endArrowOn=m->hasEndArrow();
			arrowHeadLength=m->headLength();
			arrowHeadAngle=m->headAngle();
			fillArrowHead=m->filledArrowHead();
		}
		else if (copiedMarkerType == Layer::Image)
		{
			ImageEnrichment *im = (ImageEnrichment *) g->selectedMarkerPtr();
			if (im)
				auxMrkFileName = im->fileName();
		}
	}
	copiedLayer=false;
}

void ApplicationWindow::pasteSelection()
{
	QWidget* m = (QWidget*)ws->activeWindow();
	if (!m)
		return;

	if (m->inherits("Table"))
		((Table*)m)->pasteSelection();
	else if (m->inherits("Matrix"))
		((Matrix*)m)->pasteSelection();
	else if (m->inherits("Note"))
		((Note*)m)->textWidget()->paste();
	else if (m->inherits("Graph"))
	{
		Graph* plot = (Graph*)m;
		if (!plot)
			return;
		if (copiedLayer)
		{
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

			Layer* g = plot->addLayer();
			g->copy(lastCopiedLayer);
			QPoint pos=plot->mapFromGlobal(QCursor::pos());
			plot->setLayerGeometry(pos.x(), pos.y()-20, lastCopiedLayer->width(), lastCopiedLayer->height());

			QApplication::restoreOverrideCursor();
		}
		else
		{
			if (plot->layerCount() == 0)
				return;

			Layer* g = (Layer*)plot->activeLayer();
			if (!g)
				return;

			g->setCopiedMarkerType((Layer::MarkerType)copiedMarkerType);
			g->setCopiedMarkerEnds(auxMrkStart,auxMrkEnd);

			if (copiedMarkerType == Layer::Text)
				g->setCopiedTextOptions(auxMrkBkg,auxMrkText,auxMrkFont,auxMrkColor, auxMrkBkgColor);
			if (copiedMarkerType == Layer::Arrow)
				g->setCopiedArrowOptions(auxMrkWidth,auxMrkStyle,auxMrkColor,startArrowOn,
						endArrowOn, arrowHeadLength,arrowHeadAngle, fillArrowHead);
			if (copiedMarkerType == Layer::Image)
				g->setCopiedImageName(auxMrkFileName);
			g->pasteMarker();
		}
	}
	emit modified();
}

void ApplicationWindow::showMarkerPopupMenu()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	QMenu markerMenu(this);

	if (g->imageMarkerSelected())
	{
		markerMenu.insertItem(QPixmap(":/pixelProfile.xpm"),tr("&View Pixel Line profile"),this, SLOT(pixelLineProfile()));
		markerMenu.insertItem(tr("&Intensity Matrix"),this, SLOT(intensityTable()));
		markerMenu.addSeparator();
	}

	markerMenu.insertItem(QPixmap(":/cut.xpm"),tr("&Cut"),this, SLOT(cutSelection()));
	markerMenu.insertItem(QPixmap(":/copy.xpm"), tr("&Copy"),this, SLOT(copySelection()));
	markerMenu.insertItem(QPixmap(":/erase.xpm"), tr("&Delete"),this, SLOT(clearSelection()));
	markerMenu.addSeparator();
	if (g->arrowMarkerSelected())
		markerMenu.insertItem(tr("&Properties..."),this, SLOT(showLineEnrichmentDialog()));
	else if (g->imageMarkerSelected())
		markerMenu.insertItem(tr("&Properties..."),this, SLOT(showImageEnrichmentDialog()));
	else
		markerMenu.insertItem(tr("&Properties..."),this, SLOT(showTextDialog()));

	markerMenu.exec(QCursor::pos());
}

void ApplicationWindow::showGraphContextMenu()
{
	QWidget* w = (QWidget*)ws->activeWindow();
	if (!w)
		return;

	if (w->inherits("Graph"))
	{
		Graph *plot=(Graph*)w;
		QMenu cm(this);
		QMenu exports(this);
		QMenu copy(this);
		QMenu prints(this);
		QMenu calcul(this);
		QMenu smooth(this);
		QMenu filter(this);
		QMenu decay(this);
		QMenu translate(this);
		QMenu multiPeakMenu(this);

		Layer* layer = (Layer*)plot->activeLayer();

		if (layer->isPiePlot())
			cm.insertItem(tr("Re&move Pie Curve"), layer, SLOT(removePie()));
		else
		{
			if (layer->visibleCurves() != layer->curveCount())
			{
				cm.addAction(actionShowAllCurves);
				cm.addSeparator();
			}
			cm.addAction(actionShowCurvesDialog);
			cm.addAction(actionAddFunctionCurve);

			translate.addAction(actionTranslateVert);
			translate.addAction(actionTranslateHor);
			calcul.insertItem(tr("&Translate"),&translate);
			calcul.addSeparator();

			calcul.addAction(actionDifferentiate);
			calcul.addAction(actionShowIntDialog);
			calcul.addSeparator();
			smooth.addAction(actionSmoothSavGol);
			smooth.addAction(actionSmoothFFT);
			smooth.addAction(actionSmoothAverage);
			calcul.insertItem(tr("&Smooth"), &smooth);

			filter.addAction(actionLowPassFilter);
			filter.addAction(actionHighPassFilter);
			filter.addAction(actionBandPassFilter);
			filter.addAction(actionBandBlockFilter);
			calcul.insertItem(tr("&FFT Filter"),&filter);
			calcul.addSeparator();
			calcul.addAction(actionInterpolate);
			calcul.addAction(actionFFT);
			calcul.addSeparator();
			calcul.addAction(actionFitLinear);
			calcul.addAction(actionShowFitPolynomDialog);
			calcul.addSeparator();
			decay.addAction(actionShowExpDecayDialog);
			decay.addAction(actionShowTwoExpDecayDialog);
			decay.addAction(actionShowExpDecay3Dialog);
			calcul.insertItem(tr("Fit E&xponential Decay"), &decay);
			calcul.addAction(actionFitExpGrowth);
			calcul.addAction(actionFitSigmoidal);
			calcul.addAction(actionFitGauss);
			calcul.addAction(actionFitLorentz);

			multiPeakMenu.addAction(actionMultiPeakGauss);
			multiPeakMenu.addAction(actionMultiPeakLorentz);
			calcul.insertItem(tr("Fit &Multi-Peak"), &multiPeakMenu);
			calcul.addSeparator();
			calcul.addAction(actionShowFitDialog);
			cm.insertItem(tr("Anal&yze"), &calcul);
		}

		if (copiedLayer)
		{
			cm.addSeparator();
			cm.insertItem(QPixmap(":/paste.xpm"), tr("&Paste Layer"),this, SLOT(pasteSelection()));
		}
		else if (copiedMarkerType >=0 )
		{
			cm.addSeparator();
			if (copiedMarkerType == Layer::Text )
				cm.insertItem(QPixmap(":/paste.xpm"),tr("&Paste Text"),plot, SIGNAL(pasteMarker()));
			else if (copiedMarkerType == Layer::Arrow )
				cm.insertItem(QPixmap(":/paste.xpm"),tr("&Paste Line/Arrow"),plot, SIGNAL(pasteMarker()));
			else if (copiedMarkerType == Layer::Image )
				cm.insertItem(QPixmap(":/paste.xpm"),tr("&Paste Image"),plot, SIGNAL(pasteMarker()));
		}
		cm.addSeparator();
		copy.insertItem(tr("&Layer"), this, SLOT(copyActiveLayer()));
		copy.insertItem(tr("&Window"),plot, SLOT(copyAllLayers()));
		cm.insertItem(QPixmap(":/copy.xpm"),tr("&Copy"), &copy);

		exports.insertItem(tr("&Layer"), this, SLOT(exportLayer()));
		exports.insertItem(tr("&Window"), this, SLOT(exportGraph()));
		cm.insertItem(tr("E&xport"),&exports);

		prints.insertItem(tr("&Layer"), plot, SLOT(printActiveLayer()));
		prints.insertItem(tr("&Window"),plot, SLOT(print()));
		cm.insertItem(QPixmap(":/fileprint.xpm"),tr("&Print"),&prints);
		cm.addSeparator();
		cm.insertItem(QPixmap(":/resize.xpm"), tr("&Geometry..."), plot, SIGNAL(showGeometryDialog()));
		cm.insertItem(tr("P&roperties..."), this, SLOT(showGeneralPlotDialog()));
		cm.addSeparator();
		cm.insertItem(QPixmap(":/close.xpm"), tr("&Delete Layer"), plot, SLOT(confirmRemoveLayer()));
		cm.exec(QCursor::pos());
	}
}

void ApplicationWindow::showLayerButtonContextMenu()
{
	QWidget* w = (QWidget*)ws->activeWindow();
	if (!w)
		return;

	if (w->inherits("Graph"))
	{
		Graph *plot=(Graph*)w;
		QMenu cm(this);
		QMenu exports(this);
		QMenu copy(this);
		QMenu prints(this);
		QMenu calcul(this);
		QMenu smooth(this);
		QMenu filter(this);
		QMenu decay(this);
		QMenu translate(this);
		QMenu multiPeakMenu(this);

		Layer* ag = (Layer*)plot->activeLayer();

		cm.addAction(actionAddLayer);
		cm.addAction(actionDeleteLayer);
		cm.addSeparator();

		if (ag->isPiePlot())
			cm.insertItem(tr("Re&move Pie Curve"),ag, SLOT(removePie()));
		else
		{
			if (ag->visibleCurves() != ag->curveCount())
			{
				cm.addAction(actionShowAllCurves);
				cm.addSeparator();
			}
			cm.addAction(actionShowCurvesDialog);
			cm.addAction(actionAddFunctionCurve);

			translate.addAction(actionTranslateVert);
			translate.addAction(actionTranslateHor);
			calcul.insertItem(tr("&Translate"),&translate);
			calcul.addSeparator();

			calcul.addAction(actionDifferentiate);
			calcul.addAction(actionShowIntDialog);
			calcul.addSeparator();
			smooth.addAction(actionSmoothSavGol);
			smooth.addAction(actionSmoothFFT);
			smooth.addAction(actionSmoothAverage);
			calcul.insertItem(tr("&Smooth"), &smooth);

			filter.addAction(actionLowPassFilter);
			filter.addAction(actionHighPassFilter);
			filter.addAction(actionBandPassFilter);
			filter.addAction(actionBandBlockFilter);
			calcul.insertItem(tr("&FFT Filter"),&filter);
			calcul.addSeparator();
			calcul.addAction(actionInterpolate);
			calcul.addAction(actionFFT);
			calcul.addSeparator();
			calcul.addAction(actionFitLinear);
			calcul.addAction(actionShowFitPolynomDialog);
			calcul.addSeparator();
			decay.addAction(actionShowExpDecayDialog);
			decay.addAction(actionShowTwoExpDecayDialog);
			decay.addAction(actionShowExpDecay3Dialog);
			calcul.insertItem(tr("Fit E&xponential Decay"), &decay);
			calcul.addAction(actionFitExpGrowth);
			calcul.addAction(actionFitSigmoidal);
			calcul.addAction(actionFitGauss);
			calcul.addAction(actionFitLorentz);

			multiPeakMenu.addAction(actionMultiPeakGauss);
			multiPeakMenu.addAction(actionMultiPeakLorentz);
			calcul.insertItem(tr("Fit &Multi-Peak"), &multiPeakMenu);
			calcul.addSeparator();
			calcul.addAction(actionShowFitDialog);
			cm.insertItem(tr("Anal&yze"), &calcul);
		}

		if (copiedLayer)
		{
			cm.addSeparator();
			cm.insertItem(QPixmap(":/paste.xpm"), tr("&Paste Layer"),this, SLOT(pasteSelection()));
		}
		else if (copiedMarkerType >=0 )
		{
			cm.addSeparator();
			if (copiedMarkerType == Layer::Text )
				cm.insertItem(QPixmap(":/paste.xpm"),tr("&Paste Text"),plot, SIGNAL(pasteMarker()));
			else if (copiedMarkerType == Layer::Arrow )
				cm.insertItem(QPixmap(":/paste.xpm"),tr("&Paste Line/Arrow"),plot, SIGNAL(pasteMarker()));
			else if (copiedMarkerType == Layer::Image )
				cm.insertItem(QPixmap(":/paste.xpm"),tr("&Paste Image"),plot, SIGNAL(pasteMarker()));
		}
		cm.addSeparator();
		copy.insertItem(tr("&Layer"), this, SLOT(copyActiveLayer()));
		copy.insertItem(tr("&Window"),plot, SLOT(copyAllLayers()));
		cm.insertItem(QPixmap(":/copy.xpm"),tr("&Copy"), &copy);

		exports.insertItem(tr("&Layer"), this, SLOT(exportLayer()));
		exports.insertItem(tr("&Window"), this, SLOT(exportGraph()));
		cm.insertItem(tr("E&xport"),&exports);

		prints.insertItem(tr("&Layer"), plot, SLOT(printActiveLayer()));
		prints.insertItem(tr("&Window"),plot, SLOT(print()));
		cm.insertItem(QPixmap(":/fileprint.xpm"),tr("&Print"),&prints);
		cm.addSeparator();
		cm.insertItem(QPixmap(":/resize.xpm"), tr("&Geometry..."), plot, SIGNAL(showGeometryDialog()));
		cm.insertItem(tr("P&roperties..."), this, SLOT(showGeneralPlotDialog()));
		cm.addSeparator();
		cm.insertItem(QPixmap(":/close.xpm"), tr("&Delete Layer"), plot, SLOT(confirmRemoveLayer()));
		cm.exec(QCursor::pos());
		
	}
}

void ApplicationWindow::showWindowContextMenu()
{
	QWidget* w = (QWidget*)ws->activeWindow();
	if (!w)
		return;

	QMenu cm(this);
	QMenu plot3D(this);
	if (w->inherits("Graph"))
	{
		Graph *g=(Graph*)w;
		if (copiedLayer)
		{
			cm.insertItem(QPixmap(":/paste.xpm"),tr("&Paste Layer"),this, SLOT(pasteSelection()));
			cm.addSeparator();
		}

		cm.addAction(actionAddLayer);
		if (g->layerCount() != 0)
		{
			cm.addAction(actionDeleteLayer);
			cm.addSeparator();
			cm.addAction(actionShowPlotGeometryDialog);
			cm.addAction(actionShowLayerDialog);
			cm.addSeparator();
		}
		else cm.addSeparator();
		cm.addAction(actionRename);
		cm.addAction(actionCopyWindow);
		cm.addSeparator();
		cm.insertItem(QPixmap(":/copy.xpm"),tr("&Copy Page"), g, SLOT(copyAllLayers()));
		cm.insertItem(tr("E&xport Page"), this, SLOT(exportGraph()));
		cm.addAction(actionPrint);
		cm.addSeparator();
		cm.addAction(actionCloseWindow);
	}
	else if (w->inherits("Graph3D"))
	{
		Graph3D *g=(Graph3D*)w;
		if (!g->hasData())
		{
			cm.insertItem(tr("3D &Plot"), &plot3D);
			plot3D.addAction(actionAdd3DData);
			plot3D.insertItem(tr("&Matrix..."), this, SLOT(add3DMatrixPlot()));
			plot3D.addAction(actionEditSurfacePlot);
		}
		else
		{
			if (g->getTable())
				cm.insertItem(tr("Choose &Data Set..."), this, SLOT(change3DData()));
			else if (g->matrix())
				cm.insertItem(tr("Choose &Matrix..."), this, SLOT(change3DMatrix()));
			else if (g->userFunction())
				cm.addAction(actionEditSurfacePlot);
			cm.insertItem(QPixmap(":/erase.xpm"), tr("C&lear"), g, SLOT(clearData()));
		}

		cm.addSeparator();
		cm.addAction(actionRename);
		cm.addAction(actionCopyWindow);
		cm.addSeparator();
		cm.insertItem(tr("&Copy Graph"), g, SLOT(copyImage()));
		cm.insertItem(tr("&Export"), this, SLOT(exportGraph()));
		cm.addAction(actionPrint);
		cm.addSeparator();
		cm.addAction(actionCloseWindow);
	}
	else if (w->inherits("Matrix"))
	{
		Matrix *t=(Matrix *)w;
		cm.insertItem(QPixmap(":/cut.xpm"),tr("Cu&t"), t, SLOT(cutSelection()));
		cm.insertItem(QPixmap(":/copy.xpm"),tr("&Copy"), t, SLOT(copySelection()));
		cm.insertItem(QPixmap(":/paste.xpm"),tr("&Paste"), t, SLOT(pasteSelection()));
		cm.addSeparator();
		cm.insertItem(tr("&Insert Row"), t, SLOT(insertRow()));
		cm.insertItem(tr("&Insert Column"), t, SLOT(insertColumn()));
		if (t->rowsSelected())
		{
			cm.insertItem(QPixmap(":/close.xpm"), tr("&Delete Rows"), t, SLOT(deleteSelectedRows()));
		}
		else if (t->columnsSelected())
		{
			cm.insertItem(QPixmap(":/close.xpm"), tr("&Delete Columns"), t, SLOT(deleteSelectedColumns()));
		}
		cm.insertItem(QPixmap(":/erase.xpm"),tr("Clea&r"), t, SLOT(clearSelection()));
	}
	cm.exec(QCursor::pos());
}

void ApplicationWindow::setCurveFullRange()
{
    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph*)ws->activeWindow())->activeLayer();
	if (!g)
		return;

	int curveKey = actionCurveFullRange->data().toInt();
	g->setCurveFullRange(g->curveIndex(curveKey));
}

void ApplicationWindow::showCurveRangeDialog()
{
    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* layer = ((Graph*)ws->activeWindow())->activeLayer();
	if (!layer)
		return;

	int curveKey = actionEditCurveRange->data().toInt();
	showCurveRangeDialog(layer, layer->curveIndex(curveKey));
}

CurveRangeDialog* ApplicationWindow::showCurveRangeDialog(Layer *layer, int curve)
{
	if (!layer)
		return 0;

	CurveRangeDialog* crd = new CurveRangeDialog(this);
	crd->setAttribute(Qt::WA_DeleteOnClose);
	crd->setCurveToModify(layer, curve);
	crd->show();
	return crd;
}

void ApplicationWindow::showFunctionDialog()
{
    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* layer = ((Graph*)ws->activeWindow())->activeLayer();
	if (!layer)
		return;

	int curveKey = actionEditFunction->data().toInt();
	showFunctionDialog(layer, layer->curveIndex(curveKey));
}

void ApplicationWindow::addFunctionCurve()
{
	QWidget* w = ws->activeWindow();
	if (!w || !w->inherits("Graph"))
		return;

	if (((Graph*)w)->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Layer* g = ((Graph*)w)->activeLayer();
	if ( g )
	{
		FunctionDialog* fd = functionDialog();
		if (fd)
			fd->setLayer(g);
	}
}

void ApplicationWindow::newFunctionPlot(int type,QStringList &formulas, const QString& var, QList<double> &ranges, int points)
{
    Graph *ml = newGraph();
    if (ml)
        ml->activeLayer()->addFunctionCurve(type,formulas, var,ranges,points);

	updateFunctionLists(type, formulas);
}

void ApplicationWindow::pixelLineProfile()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph *)ws->activeWindow())->activeLayer();
	if (!g)
		return;

	bool ok;
	int res = QInputDialog::getInteger(
			tr("Set the number of pixels to average"), tr("Number of averaged pixels"),1, 1, 2000, 2,
			&ok, this );
	if ( !ok )
		return;

	LineProfileTool *lpt = new LineProfileTool(g, res);
	connect(lpt, SIGNAL(createTablePlot(const QString&,int,int,const QString&)),
			this, SLOT(newWrksheetPlot(const QString&,int,int,const QString&)));
	g->setActiveTool(lpt);
}

void ApplicationWindow::intensityTable()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer* g = ((Graph *)ws->activeWindow())->activeLayer();
	if (g)
		g->showIntensityTable();
}

void ApplicationWindow::autoArrangeLayers()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph *)ws->activeWindow();
	plot->setMargins(5, 5, 5, 5);
	plot->setSpacing(5, 5);
	plot->arrangeLayers(true, false);
}

void ApplicationWindow::addLayer()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Graph* plot = (Graph *)ws->activeWindow();
	switch(QMessageBox::information(this,
				tr("Guess best origin for the new layer?"),
				tr("Do you want SciDAVis to guess the best position for the new layer?\n Warning: this will rearrange existing layers!"),
				tr("&Guess"), tr("&Top-left corner"), tr("&Cancel"), 0, 2 ) )
	{
		case 0:
			{
				setPreferences(plot->addLayer());
				plot->arrangeLayers(true, false);
			}
			break;

		case 1:
			setPreferences(plot->addLayer(0, 0, plot->size().width(), plot->size().height()));
			break;

		case 2:
			return;
			break;
	}
}

void ApplicationWindow::deleteLayer()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	((Graph *)ws->activeWindow())->confirmRemoveLayer();
}

Layer* ApplicationWindow::openLayer(ApplicationWindow* app, Graph *plot,
		const QStringList &list)
{
	Layer* ag = 0;
	int curveID = 0;
	for (int j=0;j<(int)list.count()-1;j++)
	{
		QString s=list[j];
		if (s.contains ("ggeometry"))
		{
			QStringList fList=s.split("\t");
			ag =(Layer*)plot->addLayer(fList[1].toInt(), fList[2].toInt(),
					fList[3].toInt(), fList[4].toInt());
            ag->blockSignals(true);
			ag->enableAutoscaling(autoscale2DPlots);
		}
		else if (s.left(10) == "Background")
		{
			QStringList fList = s.split("\t");
			QColor c = QColor(fList[1]);
			if (fList.count() == 3)
				c.setAlpha(fList[2].toInt());
			ag->setBackgroundColor(c);
		}
		else if (s.contains ("Margin"))
		{
			QStringList fList=s.split("\t");
			ag->plotWidget()->setMargin(fList[1].toInt());
		}
		else if (s.contains ("Border"))
		{
			QStringList fList=s.split("\t");
			ag->setFrame(fList[1].toInt(), QColor(fList[2]));
		}
		else if (s.contains ("EnabledAxes"))
		{
			QStringList fList=s.split("\t");
			ag->enableAxes(fList);
		}
		else if (s.contains ("AxesBaseline"))
		{
			QStringList fList=s.split("\t", QString::SkipEmptyParts);
			ag->setAxesBaseline(fList);
		}
		else if (s.contains ("EnabledTicks"))
		{//version < 0.8.6
			QStringList fList=s.split("\t");
			fList.pop_front();
			fList.replaceInStrings("-1", "3");
			ag->setMajorTicksType(fList);
			ag->setMinorTicksType(fList);
		}
		else if (s.contains ("MajorTicks"))
		{//version >= 0.8.6
			QStringList fList=s.split("\t");
			fList.pop_front();
			ag->setMajorTicksType(fList);
		}
		else if (s.contains ("MinorTicks"))
		{//version >= 0.8.6
			QStringList fList=s.split("\t");
			fList.pop_front();
			ag->setMinorTicksType(fList);
		}
		else if (s.contains ("TicksLength"))
		{
			QStringList fList=s.split("\t");
			ag->setTicksLength(fList[1].toInt(), fList[2].toInt());
		}
		else if (s.contains ("EnabledTickLabels"))
		{
			QStringList fList=s.split("\t");
			fList.pop_front();
			ag->setEnabledTickLabels(fList);
		}
		else if (s.contains ("AxesColors"))
		{
			QStringList fList=s.split("\t");
			fList.pop_front();
			ag->setAxesColors(fList);
		}
		else if (s.contains ("AxesNumberColors"))
		{
			QStringList fList=QStringList::split ("\t",s,TRUE);
			fList.pop_front();
			ag->setAxesNumColors(fList);
		}
		else if (s.left(5)=="grid\t")
		{
			QStringList grid=s.split("\t");
			GridOptions gr;
			gr.majorOnX=grid[1].toInt();
			gr.minorOnX=grid[2].toInt();
			gr.majorOnY=grid[3].toInt();
			gr.minorOnY=grid[4].toInt();
			gr.majorCol=grid[5].toInt();
			gr.majorStyle=grid[6].toInt();
			gr.majorWidth=grid[7].toInt();
			gr.minorCol=grid[8].toInt();
			gr.minorStyle=grid[9].toInt();
			gr.minorWidth=grid[10].toInt();
			gr.xZeroOn=grid[11].toInt();
			gr.yZeroOn=grid[12].toInt();
			if (grid.count() == 15)
			{
				gr.xAxis=grid[13].toInt();
				gr.yAxis=grid[14].toInt();
			}
			ag->setGridOptions(gr);
		}
		else if (s.startsWith ("<Antialiasing>") && s.endsWith ("</Antialiasing>"))
		{
			bool antialiasing = s.remove("<Antialiasing>").remove("</Antialiasing>").toInt();
			ag->setAntialiasing(antialiasing, false);
		}
		else if (s.contains ("PieCurve"))
		{
			QStringList curve=s.split("\t");
			if (!app->renamedTables.isEmpty())
			{
				QString caption = (curve[1]).left((curve[1]).find("_",0));
				if (app->renamedTables.contains(caption))
				{//modify the name of the curve according to the new table name
					int index = app->renamedTables.indexOf(caption);
					QString newCaption = app->renamedTables[++index];
					curve.replaceInStrings(caption+"_", newCaption+"_");
				}
			}
			QPen pen = QPen(QColor(curve[3]),curve[2].toInt(),Layer::getPenStyle(curve[4]));

			Table *table = app->table(curve[1]);
			if (table)
			{
				int startRow = 0;
				int endRow = table->rowCount() - 1;
				int first_color = curve[7].toInt();
				bool visible = true;
				if (d_file_version >= 90)
				{
					startRow = curve[8].toInt();
					endRow = curve[9].toInt();
					visible = ((curve.last() == "1") ? true : false);
				}

				if (d_file_version <= 89)
					first_color = convertOldToNewColorIndex(first_color);

				ag->plotPie(table, curve[1], pen, curve[5].toInt(),
					curve[6].toInt(), first_color, startRow, endRow, visible);
			}
		}
		else if (s.left(6)=="curve\t")
		{
			QStringList curve = s.split("\t", QString::SkipEmptyParts);
			if (!app->renamedTables.isEmpty())
			{
				QString caption = (curve[2]).left((curve[2]).find("_",0));

				if (app->renamedTables.contains(caption))
				{//modify the name of the curve according to the new table name
					int index = app->renamedTables.indexOf(caption);
					QString newCaption = app->renamedTables[++index];
					curve.replaceInStrings(caption+"_", newCaption+"_");
				}
			}

			CurveLayout cl;
			cl.connectType=curve[4].toInt();
			cl.lCol=curve[5].toInt();
			if (d_file_version <= 89)
				cl.lCol = convertOldToNewColorIndex(cl.lCol);
			cl.lStyle=curve[6].toInt();
			cl.lWidth=curve[7].toInt();
			cl.sSize=curve[8].toInt();
			if (d_file_version <= 78)
				cl.sType=Layer::obsoleteSymbolStyle(curve[9].toInt());
			else
				cl.sType=curve[9].toInt();

			cl.symCol=curve[10].toInt();
			if (d_file_version <= 89)
				cl.symCol = convertOldToNewColorIndex(cl.symCol);
			cl.fillCol=curve[11].toInt();
			if (d_file_version <= 89)
				cl.fillCol = convertOldToNewColorIndex(cl.fillCol);
			cl.filledArea=curve[12].toInt();
			cl.aCol=curve[13].toInt();
			if (d_file_version <= 89)
				cl.aCol = convertOldToNewColorIndex(cl.aCol);
			cl.aStyle=curve[14].toInt();
			if(curve.count() < 16)
				cl.penWidth = cl.lWidth;
			else if ((d_file_version >= 79) && (curve[3].toInt() == Layer::Box))
				cl.penWidth = curve[15].toInt();
			else if ((d_file_version >= 78) && (curve[3].toInt() <= Layer::LineSymbols))
				cl.penWidth = curve[15].toInt();
			else
				cl.penWidth = cl.lWidth;

			Table *w = app->table(curve[2]);
			if (w)
			{
				int plotType = curve[3].toInt();
				if(plotType == Layer::VectXYXY || plotType == Layer::VectXYAM)
				{
					QStringList colsList;
					colsList<<curve[2]; colsList<<curve[20]; colsList<<curve[21];
					if (d_file_version < 72)
						colsList.prepend(w->colName(curve[1].toInt()));
					else
                        colsList.prepend(curve[1]);

					int startRow = 0;
					int endRow = -1;
					if (d_file_version >= 90)
					{
						startRow = curve[curve.count()-3].toInt();
						endRow = curve[curve.count()-2].toInt();
					}

					ag->plotVectorCurve(w, colsList, plotType, startRow, endRow);

					if (d_file_version <= 77)
					{
						int temp_index = convertOldToNewColorIndex(curve[15].toInt());
						ag->updateVectorsLayout(curveID, ColorBox::color(temp_index), curve[16].toInt(), curve[17].toInt(),
								curve[18].toInt(), curve[19].toInt(), 0, curve[20], curve[21]);
					}
					else
					{
						if(plotType == Layer::VectXYXY)
							ag->updateVectorsLayout(curveID, curve[15], curve[16].toInt(),
								curve[17].toInt(), curve[18].toInt(), curve[19].toInt(), 0);
						else
							ag->updateVectorsLayout(curveID, curve[15], curve[16].toInt(), curve[17].toInt(),
									curve[18].toInt(), curve[19].toInt(), curve[22].toInt());
					}
				}
				else if(plotType == Layer::Box)
					ag->openBoxDiagram(w, curve, d_file_version);
				else
				{
					if (d_file_version < 72)
						ag->insertCurve(w, curve[1].toInt(), curve[2], plotType);
					else if (d_file_version < 90)
						ag->insertCurve(w, curve[1], curve[2], plotType);
					else
					{
						int startRow = curve[curve.count()-3].toInt();
						int endRow = curve[curve.count()-2].toInt();
						ag->insertCurve(w, curve[1], curve[2], plotType, startRow, endRow);
					}
				}

				if(plotType == Layer::Histogram)
				{
				    HistogramCurve *h = (HistogramCurve *)ag->curve(curveID);
					if (d_file_version <= 76)
                        h->setBinning(curve[16].toInt(),curve[17].toDouble(),curve[18].toDouble(),curve[19].toDouble());
					else
						h->setBinning(curve[17].toInt(),curve[18].toDouble(),curve[19].toDouble(),curve[20].toDouble());
                    h->loadData();
				}

				if(plotType == Layer::VerticalBars || plotType == Layer::HorizontalBars ||
						plotType == Layer::Histogram)
				{
					if (d_file_version <= 76)
						ag->setBarsGap(curveID, curve[15].toInt(), 0);
					else
						ag->setBarsGap(curveID, curve[15].toInt(), curve[16].toInt());
				}
				ag->updateCurveLayout(curveID, &cl);
				if (d_file_version >= 88)
				{
					QwtPlotCurve *c = ag->curve(curveID);
					if (c && c->rtti() == QwtPlotItem::Rtti_PlotCurve)
					{
						if (d_file_version < 90)
							c->setAxis(curve[curve.count()-2].toInt(), curve[curve.count()-1].toInt());
						else
						{
							c->setAxis(curve[curve.count()-5].toInt(), curve[curve.count()-4].toInt());
							c->setVisible(curve.last().toInt());
						}
					}
				}
			}
			curveID++;
		}
		else if (s.contains ("FunctionCurve"))
		{
			QStringList curve = s.split("\t");
			CurveLayout cl;
			cl.connectType=curve[6].toInt();
			cl.lCol=curve[7].toInt();
			cl.lStyle=curve[8].toInt();
			cl.lWidth=curve[9].toInt();
			cl.sSize=curve[10].toInt();
			cl.sType=curve[11].toInt();
			cl.symCol=curve[12].toInt();
			cl.fillCol=curve[13].toInt();
			cl.filledArea=curve[14].toInt();
			cl.aCol=curve[15].toInt();
			cl.aStyle=curve[16].toInt();
			int current_index = 17;
			if(curve.count() < 16)
				cl.penWidth = cl.lWidth;
			else if ((d_file_version >= 79) && (curve[5].toInt() == Layer::Box))
				{
					cl.penWidth = curve[17].toInt();
					current_index++;
				}
			else if ((d_file_version >= 78) && (curve[5].toInt() <= Layer::LineSymbols))
				{
					cl.penWidth = curve[17].toInt();
					current_index++;
				}
			else
				cl.penWidth = cl.lWidth;

			ag->insertFunctionCurve(curve[1], curve[2].toInt(), d_file_version);
			ag->setCurveType(curveID, (Layer::CurveType)curve[5].toInt(), false);
			ag->updateCurveLayout(curveID, &cl);
			if (d_file_version >= 88)
			{
				QwtPlotCurve *c = ag->curve(curveID);
				if (c)
				{
					if(current_index+1 < curve.size())
						c->setAxis(curve[current_index].toInt(), curve[current_index+1].toInt());
					if(d_file_version >= 90 && current_index+2 < curve.size())
						c->setVisible(curve.last().toInt());
					else
						c->setVisible(true);
				}

			}
			curveID++;
		}
		else if (s.contains ("ErrorBars"))
		{
			QStringList curve = s.split("\t", QString::SkipEmptyParts);
			Table *w = app->table(curve[3]);
			Table *errTable = app->table(curve[4]);
			if (w && errTable)
			{
				ag->addErrorBars(curve[2], curve[3], errTable, curve[4], curve[1].toInt(),
						curve[5].toInt(), curve[6].toInt(), QColor(curve[7]),
						curve[8].toInt(), curve[10].toInt(), curve[9].toInt());
			}
			curveID++;
		}
		else if (s == "<spectrogram>")
		{
			curveID++;
			QStringList lst;
			while ( s!="</spectrogram>" )
			{
				s = list[++j];
				lst << s;
			}
			lst.pop_back();
			ag->restoreSpectrogram(app, lst);
		}
		else if (s.left(6)=="scale\t")
		{
			QStringList scl = s.split("\t");
			scl.pop_front();
			if (d_file_version < 88)
			{
				double step = scl[2].toDouble();
				if (scl[5] == "0")
					step = 0.0;
				ag->setScale(QwtPlot::xBottom, scl[0].toDouble(), scl[1].toDouble(), step,
						scl[3].toInt(), scl[4].toInt(), scl[6].toInt(), bool(scl[7].toInt()));
				ag->setScale(QwtPlot::xTop, scl[0].toDouble(), scl[1].toDouble(), step,
						scl[3].toInt(), scl[4].toInt(), scl[6].toInt(), bool(scl[7].toInt()));

				step = scl[10].toDouble();
				if (scl[13] == "0")
					step = 0.0;
				ag->setScale(QwtPlot::yLeft, scl[8].toDouble(), scl[9].toDouble(), step, scl[11].toInt(),
						scl[12].toInt(), scl[14].toInt(), bool(scl[15].toInt()));
				ag->setScale(QwtPlot::yRight, scl[8].toDouble(), scl[9].toDouble(), step, scl[11].toInt(),
						scl[12].toInt(), scl[14].toInt(), bool(scl[15].toInt()));
			}
			else
				ag->setScale(scl[0].toInt(), scl[1].toDouble(), scl[2].toDouble(), scl[3].toDouble(),
						scl[4].toInt(), scl[5].toInt(),  scl[6].toInt(), bool(scl[7].toInt()));
		}
		else if (s.contains ("PlotTitle"))
		{
			QStringList fList=s.split("\t");
			ag->setTitle(fList[1]);
			ag->setTitleColor(QColor(fList[2]));
			ag->setTitleAlignment(fList[3].toInt());
		}
		else if (s.contains ("TitleFont"))
		{
			QStringList fList=s.split("\t");
			QFont fnt=QFont (fList[1],fList[2].toInt(),fList[3].toInt(),fList[4].toInt());
			fnt.setUnderline(fList[5].toInt());
			fnt.setStrikeOut(fList[6].toInt());
			ag->setTitleFont(fnt);
		}
		else if (s.contains ("AxesTitles"))
		{
			QStringList legend=s.split("\t");
			legend.pop_front();
			for (int i=0; i<4; i++)
			{
			    if (legend.count() > i)
                    ag->setAxisTitle(i, legend[i]);
			}
		}
		else if (s.contains ("AxesTitleColors"))
		{
			QStringList colors=s.split("\t", QString::SkipEmptyParts);
			ag->setAxesTitleColor(colors);
		}
		else if (s.contains ("AxesTitleAlignment"))
		{
			QStringList align=s.split("\t", QString::SkipEmptyParts);
			ag->setAxesTitlesAlignment(align);
		}
		else if (s.contains ("ScaleFont"))
		{
			QStringList fList=s.split("\t");
			QFont fnt=QFont (fList[1],fList[2].toInt(),fList[3].toInt(),fList[4].toInt());
			fnt.setUnderline(fList[5].toInt());
			fnt.setStrikeOut(fList[6].toInt());

			int axis=(fList[0].right(1)).toInt();
			ag->setAxisTitleFont(axis,fnt);
		}
		else if (s.contains ("AxisFont"))
		{
			QStringList fList=s.split("\t");
			QFont fnt=QFont (fList[1],fList[2].toInt(),fList[3].toInt(),fList[4].toInt());
			fnt.setUnderline(fList[5].toInt());
			fnt.setStrikeOut(fList[6].toInt());

			int axis=(fList[0].right(1)).toInt();
			ag->setAxisFont(axis,fnt);
		}
		else if (s.contains ("AxesFormulas"))
		{
			QStringList fList=s.split("\t");
			fList.remove(fList.first());
			ag->setAxesFormulas(fList);
		}
		else if (s.startsWith("<AxisFormula "))
		{
			int pos = s.mid(18,s.length()-20).toInt();
			QString formula;
			for (j++; j<(int)list.count() && list[j] != "</AxisFormula>"; j++)
				formula += list[j] + "\n";
			formula.truncate(formula.length()-1);
			ag->setAxisFormula(pos,formula);
		}
		else if (s.contains ("LabelsFormat"))
		{
			QStringList fList=s.split("\t");
			fList.pop_front();
			ag->setLabelsNumericFormat(fList);
		}
		else if (s.contains ("LabelsRotation"))
		{
			QStringList fList=s.split("\t");
			ag->setAxisLabelRotation(QwtPlot::xBottom, fList[1].toInt());
			ag->setAxisLabelRotation(QwtPlot::xTop, fList[2].toInt());
		}
		else if (s.contains ("DrawAxesBackbone"))
		{
			QStringList fList=s.split("\t");
			ag->loadAxesOptions(fList[1]);
		}
		else if (s.contains ("AxesLineWidth"))
		{
			QStringList fList=s.split("\t");
			ag->loadAxesLinewidth(fList[1].toInt());
		}
		else if (s.contains ("CanvasFrame"))
		{
			QStringList list=s.split("\t");
			ag->drawCanvasFrame(list);
		}
		else if (s.contains ("CanvasBackground"))
		{
			QStringList list = s.split("\t");
			QColor c = QColor(list[1]);
			if (list.count() == 3)
				c.setAlpha(list[2].toInt());
			ag->setCanvasBackground(c);
		}
		else if (s.contains ("Legend"))
		{// version <= 0.8.9
			QStringList fList=QStringList::split ("\t",s, true);
			ag->insertLegend(fList, d_file_version);
		}
		else if (s.startsWith ("<legend>") && s.endsWith ("</legend>"))
		{
			QStringList fList=QStringList::split ("\t", s.remove("</legend>"), true);
			ag->insertLegend(fList, d_file_version);
		}
		else if (s.contains ("textMarker"))
		{// version <= 0.8.9
			QStringList fList=QStringList::split ("\t",s, true);
			ag->insertTextMarker(fList, d_file_version);
		}
		else if (s.startsWith ("<text>") && s.endsWith ("</text>"))
		{
			QStringList fList=QStringList::split ("\t", s.remove("</text>"), true);
			ag->insertTextMarker(fList, d_file_version);
		}
		else if (s.contains ("lineMarker"))
		{// version <= 0.8.9
			QStringList fList=s.split("\t");
			ag->addArrow(fList, d_file_version);
		}
		else if (s.startsWith ("<line>") && s.endsWith ("</line>"))
		{
			QStringList fList=s.remove("</line>").split("\t");
			ag->addArrow(fList, d_file_version);
		}
		else if (s.contains ("ImageEnrichment") || (s.startsWith ("<image>") && s.endsWith ("</image>")))
		{
			QStringList fList=s.remove("</image>").split("\t");
			ag->insertImageEnrichment(fList, d_file_version);
		}
		else if (s.contains("AxisType"))
		{
			QStringList fList=s.split("\t");
			for (int i=0; i<4; i++)
			{
				QStringList lst = fList[i+1].split(";", QString::SkipEmptyParts);
				int format = lst[0].toInt();
				if (format == Layer::Day)
					ag->setLabelsDayFormat(i, lst[1].toInt());
				else if (format == Layer::Month)
					ag->setLabelsMonthFormat(i, lst[1].toInt());
				else if (format == Layer::Time || format == Layer::Date)
					ag->setLabelsDateTimeFormat(i, format, lst[1]+";"+lst[2]);
				else if (lst.size() > 1)
				{
					Table *nw = app->table(lst[1]);
					ag->setLabelsTextFormat(i, format, lst[1], nw);
				}
			}
		}
		else if (d_file_version < 69 && s.contains ("AxesTickLabelsCol"))
		{
			QStringList fList = s.split("\t");
			QList<int> axesTypes = ag->axesType();
			for (int i=0; i<4; i++)
			{
				QString colName = fList[i+1];
				Table *nw = app->table(colName);
				ag->setLabelsTextFormat(i, axesTypes[i], colName, nw);
			}
		}
	}
	ag->replot();
	if (ag->isPiePlot())
	{
        PieCurve *c = (PieCurve *)ag->curve(0);
        if (c) c->updateBoundingRect();
    }

    ag->blockSignals(false);
    ag->setIgnoreResizeEvents(!app->autoResizeLayers);
    ag->setAutoscaleFonts(app->autoScaleFonts);
	ag->setTextMarkerDefaults(app->legendFrameStyle, app->plotLegendFont, app->legendTextColor, app->legendBackground);
	ag->setArrowDefaults(app->defaultArrowLineWidth, app->defaultArrowColor, app->defaultArrowLineStyle,
			app->defaultArrowHeadLength, app->defaultArrowHeadAngle, app->defaultArrowHeadFill);
    return ag;
}

void ApplicationWindow::copyActiveLayer()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	copiedLayer=TRUE;

	Layer *g = ((Graph *)ws->activeWindow())->activeLayer();
	delete lastCopiedLayer;
	lastCopiedLayer = new Layer (0, 0, 0);
	lastCopiedLayer->setAttribute(Qt::WA_DeleteOnClose);
	lastCopiedLayer->setGeometry(0, 0, g->width(), g->height());
	lastCopiedLayer->copy(g);
	g->copyImage();
}

void ApplicationWindow::showDataSetDialog(const QString& whichFit)
{
    if (!ws->activeWindow() || !ws->activeWindow()->inherits("Graph"))
		return;

	Layer *g = ((Graph *)ws->activeWindow())->activeLayer();
	if (!g)
        return;

	DataSetDialog *ad = new DataSetDialog(tr("Curve") + ": ", this);
	ad->setAttribute(Qt::WA_DeleteOnClose);
	ad->setLayer(g);
	ad->setOperationType(whichFit);
	ad->exec();
}

Graph* ApplicationWindow::plotColorMap(Matrix *m)
{
	if (!m)
		return 0;

	return plotSpectrogram(m, Layer::ColorMap);
}

Graph* ApplicationWindow::plotSpectrogram(Matrix *m, int type)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	Graph* g = multilayerPlot(generateUniqueName(tr("Graph")));
	Layer* layer = g->addLayer();
	setPreferences(layer);

	layer->plotSpectrogram(m, (Layer::CurveType)type);
	g->showNormal();

	emit modified();
	QApplication::restoreOverrideCursor();
	return g;
}

void ApplicationWindow::translateCurveHor()
{
	QWidget *w=ws->activeWindow();
	if (!w || !w->inherits("Graph"))
		return;

	Graph *plot = (Graph*)w;
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if (!g)
		return;

	if (g->isPiePlot())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("This functionality is not available for pie plots!"));

		btnPointer->setChecked(true);
		return;
	}
	else if (g->validCurvesDataSize())
	{
		btnPointer->setChecked(true);
		g->setActiveTool(new TranslateCurveTool(g, this, TranslateCurveTool::Horizontal, d_status_info, SLOT(setText(const QString&))));
	}
}

void ApplicationWindow::translateCurveVert()
{
	QWidget *w=ws->activeWindow();
	if (!w || !w->inherits("Graph"))
		return;

	Graph *plot = (Graph*)w;
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	Layer* g = (Layer*)plot->activeLayer();
	if (!g)
		return;

	if (g->isPiePlot())
	{
		QMessageBox::warning(this,tr("Warning"),
				tr("This functionality is not available for pie plots!"));

		btnPointer->setChecked(true);
		return;
	}
	else if (g->validCurvesDataSize())
	{
		btnPointer->setChecked(true);
		g->setActiveTool(new TranslateCurveTool(g, this, TranslateCurveTool::Vertical, d_status_info, SLOT(setText(const QString&))));
	}
}

void ApplicationWindow::selectPlotType(int type)
{
	if (!ws->activeWindow())
		return;

	Table *table = qobject_cast<Table *>(ws->activeWindow());
	if (table && validFor2DPlot(table)) {
		multilayerPlot(table, table->drawableColumnSelection(), (Layer::CurveType)type, table->firstSelectedRow(), table->lastSelectedRow());
	}

	Graph *graph = qobject_cast<Graph*>(ws->activeWindow());
	if (graph) {
		Layer *layer = graph->activeLayer();
		layer->setCurveType(layer->curveCount()-1, (Layer::CurveType)type);
	}
}



// old code, to be ported


void Graph::resizeLayers (const QResizeEvent *re)
{
	QSize oldSize = re->oldSize();
	QSize size = re->size();

    if (d_open_maximized == 1)
    {// 2 resize events are triggered for maximized windows: this hack allows to ignore the first one!
        d_open_maximized++;
        return;
    }
    else if (d_open_maximized == 2)
    {
        d_open_maximized = 0;
        //TODO: for maximized windows, the size of the layers should be saved in % of the workspace area in order to restore
        //the layers properly! For the moment just resize the layers to occupy the whole canvas, although the restored geometry might be altered!
        oldSize = QSize(canvas->childrenRect().width() + left_margin + right_margin,
                        canvas->childrenRect().height() + top_margin + bottom_margin);
        //return;
    }

    if(!oldSize.isValid())
        return;

    resizeLayers(size, oldSize, false);
}

void Graph::resizeLayers (const QSize& size, const QSize& oldSize, bool scaleFonts)
{
	QApplication::setOverrideCursor(Qt::waitCursor);

	double w_ratio = (double)size.width()/(double)oldSize.width();
	double h_ratio = (double)(size.height())/(double)(oldSize.height());

	for (int i=0;i<d_layer_list.count();i++)
	{
		Layer *gr = (Layer *)d_layer_list.at(i);
		if (gr && !gr->ignoresResizeEvents())
		{
			int gx = qRound(gr->x()*w_ratio);
			int gy = qRound(gr->y()*h_ratio);
			int gw = qRound(gr->width()*w_ratio);
			int gh = qRound(gr->height()*h_ratio);

			gr->setGeometry(QRect(gx, gy, gw, gh));
			gr->plotWidget()->resize(QSize(gw, gh));

            if (scaleFonts)
                gr->scaleFonts(h_ratio);
		}
	}

	emit modifiedPlot();
	emit resizedWindow(this);
	QApplication::restoreOverrideCursor();
}

void Graph::confirmRemoveLayer()
{
	if (d_layer_count>1)
	{
		switch(QMessageBox::information(this,
					tr("Guess best layout?"),
					tr("Do you want SciDAVis to rearrange the remaining layers?"),
					tr("&Yes"), tr("&No"), tr("&Cancel"),
					0, 2) )
		{
			case 0:
				removeLayer();
				arrangeLayers(true, false);
				break;

			case 1:
				removeLayer();
				break;

			case 2:
				return;
				break;
		}
	}
	else
		removeLayer();
}

void Graph::removeLayer()
{
	//remove corresponding button
	LayerButton *btn=0;
	int i;
	for (i=0;i<d_button_list.count();i++)
	{
		btn=(LayerButton*)d_button_list.at(i);
		if (btn->isOn())
		{
			d_button_list.removeAll(btn);
			btn->close(true);
			break;
		}
	}

	//update the texts of the buttons
	for (i=0;i<d_button_list.count();i++)
	{
		btn=(LayerButton*)d_button_list.at(i);
		btn->setText(QString::number(i+1));
	}

	if (d_active_layer->zoomOn() || d_active_layer->activeTool())
		emit setPointerCursor();

	int index = d_layer_list.indexOf(d_active_layer);
	d_layer_list.removeAt(index);
	d_active_layer->close();
	d_layer_count--;
	if(index >= d_layer_list.count())
		index--;

	if (d_layer_count == 0)
	{
		d_active_layer = 0;
		return;
	}

	d_active_layer=(Layer*) d_layer_list.at(index);

	for (i=0;i<(int)d_layer_list.count();i++)
	{
		Layer *gr=(Layer *)d_layer_list.at(i);
		if (gr == d_active_layer)
		{
			LayerButton *button=(LayerButton *)d_button_list.at(i);
			button->setOn(TRUE);
			break;
		}
	}

	emit modifiedPlot();
}

void Graph::setLayerGeometry(int x, int y, int w, int h)
{
	if (d_active_layer->pos() == QPoint (x,y) &&
		d_active_layer->size() == QSize (w,h))
		return;

	d_active_layer->setGeometry(QRect(QPoint(x,y),QSize(w,h)));
    d_active_layer->plotWidget()->resize(QSize(w, h));
	emit modifiedPlot();
}

QSize Graph::arrangeLayers(bool userSize)
{
	const QRect rect = canvas->geometry();

	gsl_vector *xTopR = gsl_vector_calloc (d_layer_count);//ratio between top axis + title and canvas height
	gsl_vector *xBottomR = gsl_vector_calloc (d_layer_count); //ratio between bottom axis and canvas height
	gsl_vector *yLeftR = gsl_vector_calloc (d_layer_count);
	gsl_vector *yRightR = gsl_vector_calloc (d_layer_count);
	gsl_vector *maxXTopHeight = gsl_vector_calloc (rows);//maximum top axis + title height in a row
	gsl_vector *maxXBottomHeight = gsl_vector_calloc (rows);//maximum bottom axis height in a row
	gsl_vector *maxYLeftWidth = gsl_vector_calloc (cols);//maximum left axis width in a column
	gsl_vector *maxYRightWidth = gsl_vector_calloc (cols);//maximum right axis width in a column
	gsl_vector *Y = gsl_vector_calloc (rows);
	gsl_vector *X = gsl_vector_calloc (cols);

	int i;
	for (i=0; i<d_layer_count; i++)
	{//calculate scales/canvas dimensions reports for each layer and stores them in the above vectors
		Layer *gr=(Layer *)d_layer_list.at(i);
		QwtPlot *plot=gr->plotWidget();
		QwtPlotLayout *plotLayout=plot->plotLayout();
		QRect cRect=plotLayout->canvasRect();
		double ch = (double) cRect.height();
		double cw = (double) cRect.width();

		QRect tRect=plotLayout->titleRect ();
		QwtScaleWidget *scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::xTop);

		int topHeight = 0;
		if (!tRect.isNull())
			topHeight += tRect.height() + plotLayout->spacing();
		if (scale)
		{
			QRect sRect=plotLayout->scaleRect (QwtPlot::xTop);
			topHeight += sRect.height();
		}
		gsl_vector_set (xTopR, i, double(topHeight)/ch);

		scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::xBottom);
		if (scale)
		{
			QRect sRect = plotLayout->scaleRect (QwtPlot::xBottom);
			gsl_vector_set (xBottomR, i, double(sRect.height())/ch);
		}

		scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::yLeft);
		if (scale)
		{
			QRect sRect = plotLayout->scaleRect (QwtPlot::yLeft);
			gsl_vector_set (yLeftR, i, double(sRect.width())/cw);
		}

		scale=(QwtScaleWidget *) plot->axisWidget (QwtPlot::yRight);
		if (scale)
		{
			QRect sRect = plotLayout->scaleRect (QwtPlot::yRight);
			gsl_vector_set (yRightR, i, double(sRect.width())/cw);
		}

		//calculate max scales/canvas dimensions ratio for each line and column and stores them to vectors
		int row = i / cols;
		if (row >= rows )
			row = rows - 1;

		int col = i % cols;

		double aux = gsl_vector_get(xTopR, i);
		double old_max = gsl_vector_get(maxXTopHeight, row);
		if (aux >= old_max)
			gsl_vector_set(maxXTopHeight, row,  aux);

		aux = gsl_vector_get(xBottomR, i) ;
		if (aux >= gsl_vector_get(maxXBottomHeight, row))
			gsl_vector_set(maxXBottomHeight, row,  aux);

		aux = gsl_vector_get(yLeftR, i);
		if (aux >= gsl_vector_get(maxYLeftWidth, col))
			gsl_vector_set(maxYLeftWidth, col, aux);

		aux = gsl_vector_get(yRightR, i);
		if (aux >= gsl_vector_get(maxYRightWidth, col))
			gsl_vector_set(maxYRightWidth, col, aux);
	}

	double c_heights = 0.0;
	for (i=0; i<rows; i++)
	{
		gsl_vector_set (Y, i, c_heights);
		c_heights += 1 + gsl_vector_get(maxXTopHeight, i) + gsl_vector_get(maxXBottomHeight, i);
	}

	double c_widths = 0.0;
	for (i=0; i<cols; i++)
	{
		gsl_vector_set (X, i, c_widths);
		c_widths += 1 + gsl_vector_get(maxYLeftWidth, i) + gsl_vector_get(maxYRightWidth, i);
	}

	if (!userSize)
	{
		l_canvas_width = int((rect.width()-(cols-1)*colsSpace - right_margin - left_margin)/c_widths);
		l_canvas_height = int((rect.height()-(rows-1)*rowsSpace - top_margin - bottom_margin)/c_heights);
	}

	QSize size = QSize(l_canvas_width, l_canvas_height);

	for (i=0; i<d_layer_count; i++)
	{
		int row = i / cols;
		if (row >= rows )
			row = rows - 1;

		int col = i % cols;

		//calculate sizes and positions for layers
		const int w = int (l_canvas_width*(1 + gsl_vector_get(yLeftR, i) + gsl_vector_get(yRightR, i)));
		const int h = int (l_canvas_height*(1 + gsl_vector_get(xTopR, i) + gsl_vector_get(xBottomR, i)));

		int x = left_margin + col*colsSpace;
		if (hor_align == HCenter)
			x += int (l_canvas_width*(gsl_vector_get(X, col) + gsl_vector_get(maxYLeftWidth, col) - gsl_vector_get(yLeftR, i)));
		else if (hor_align == Left)
			x += int(l_canvas_width*gsl_vector_get(X, col));
		else if (hor_align == Right)
			x += int(l_canvas_width*(gsl_vector_get(X, col) + gsl_vector_get(maxYLeftWidth, col) - gsl_vector_get(yLeftR, i)+
						gsl_vector_get(maxYRightWidth, col) - gsl_vector_get(yRightR, i)));

		int y = top_margin + row*rowsSpace;
		if (vert_align == VCenter)
			y += int(l_canvas_height*(gsl_vector_get(Y, row) + gsl_vector_get(maxXTopHeight, row) - gsl_vector_get(xTopR, i)));
		else if (vert_align == Top)
			y += int(l_canvas_height*gsl_vector_get(Y, row));
		else if (vert_align == Bottom)
			y += int(l_canvas_height*(gsl_vector_get(Y, row) + gsl_vector_get(maxXTopHeight, row) - gsl_vector_get(xTopR, i)+
						+ gsl_vector_get(maxXBottomHeight, row) - gsl_vector_get(xBottomR, i)));

		//resizes and moves layers
		Layer *gr = (Layer *)d_layer_list.at(i);
		bool autoscaleFonts = false;
		if (!userSize)
		{//When the user specifies the layer canvas size, the window is resized
			//and the fonts must be scaled accordingly. If the size is calculated
			//automatically we don't rescale the fonts in order to prevent problems
			//with too small fonts when the user adds new layers or when removing layers

			autoscaleFonts = gr->autoscaleFonts();//save user settings
			gr->setAutoscaleFonts(false);
		}

		gr->setGeometry(QRect(x, y, w, h));
		gr->plotWidget()->resize(QSize(w, h));

		if (!userSize)
			gr->setAutoscaleFonts(autoscaleFonts);//restore user settings
	}

	//free memory
	gsl_vector_free (maxXTopHeight); gsl_vector_free (maxXBottomHeight);
	gsl_vector_free (maxYLeftWidth); gsl_vector_free (maxYRightWidth);
	gsl_vector_free (xTopR); gsl_vector_free (xBottomR);
	gsl_vector_free (yLeftR); gsl_vector_free (yRightR);
	gsl_vector_free (X); gsl_vector_free (Y);
	return size;
}

void Graph::findBestLayout(int &rows, int &cols)
{
	if(d_layer_count%2 == 0) // d_layer_count is an even number
	{
		if(d_layer_count<=2)
			cols=d_layer_count/2+1;
		else if(d_layer_count>2)
			cols=d_layer_count/2;

		if(d_layer_count<8)
			rows=d_layer_count/4+1;
		if(d_layer_count>=8)
			rows=d_layer_count/4;
	}
	else // d_layer_count is an odd number
	{
		int Num=d_layer_count+1;

		if(Num<=2)
			cols=1;
		else if(Num>2)
			cols=Num/2;

		if(Num<8)
			rows=Num/4+1;
		if(Num>=8)
			rows=Num/4;
	}
}

void Graph::arrangeLayers(bool fit, bool userSize)
{
	if (!d_layer_count)
		return;

	QApplication::setOverrideCursor(Qt::waitCursor);

	if(d_layers_selector)
		delete d_layers_selector;

	if (fit)
		findBestLayout(rows, cols);

	//the canvas sizes of all layers become equal only after several
	//resize iterations, due to the way Qwt handles the plot layout
	int iterations = 0;
	QSize size = arrangeLayers(userSize);
	QSize canvas_size = QSize(1,1);
	while (canvas_size != size && iterations < 10)
	{
		iterations++;
		canvas_size = size;
		size = arrangeLayers(userSize);
	}

	if (userSize)
	{//resize window
		bool ignoreResize = d_active_layer->ignoresResizeEvents();
		for (int i=0; i<(int)d_layer_list.count(); i++)
		{
			Layer *gr = (Layer *)d_layer_list.at(i);
			gr->setIgnoreResizeEvents(true);
		}

		this->showNormal();
		QSize size = canvas->childrenRect().size();
		this->resize(QSize(size.width() + right_margin,
					size.height() + bottom_margin + LayerButton::btnSize()));

		for (int i=0; i<(int)d_layer_list.count(); i++)
		{
			Layer *gr = (Layer *)d_layer_list.at(i);
			gr->setIgnoreResizeEvents(ignoreResize);
		}
	}

	emit modifiedPlot();
	QApplication::restoreOverrideCursor();
}

void Graph::setCols(int c)
{
	if (cols != c)
		cols=c;
}

void Graph::setRows(int r)
{
	if (rows != r)
		rows=r;
}

QPixmap Graph::canvasPixmap()
{
    return QPixmap::grabWidget(canvas);
}

void Graph::exportToFile(const QString& fileName)
{
	if ( fileName.isEmpty() ){
		QMessageBox::critical(0, tr("Error"), tr("Please provide a valid file name!"));
        return;
	}

	if (fileName.contains(".eps") || fileName.contains(".pdf") || fileName.contains(".ps")){
		exportVector(fileName);
		return;
	} else if(fileName.contains(".svg")){
		exportSVG(fileName);
		return;
	} else {
		QList<QByteArray> list = QImageWriter::supportedImageFormats();
    	for(int i=0 ; i<list.count() ; i++){
			if (fileName.contains( "." + list[i].toLower())){
				exportImage(fileName);
				return;
			}
		}
    	QMessageBox::critical(this, tr("Error"), tr("File format not handled, operation aborted!"));
	}
}

void Graph::exportImage(const QString& fileName, int quality, bool transparent)
{
	QPixmap pic = canvasPixmap();
	if (transparent)
	{
		QBitmap mask(pic.size());
		mask.fill(Qt::color1);
		QPainter p;
		p.begin(&mask);
		p.setPen(Qt::color0);

		QColor background = QColor (Qt::white);
		QRgb backgroundPixel = background.rgb ();

		QImage image = pic.convertToImage();
		for (int y=0; y<image.height(); y++)
		{
			for ( int x=0; x<image.width(); x++ )
			{
				QRgb rgb = image.pixel(x, y);
				if (rgb == backgroundPixel) // we want the frame transparent
					p.drawPoint( x, y );
			}
		}
		p.end();
		pic.setMask(mask);
	}
	pic.save(fileName, 0, quality);
}

void Graph::exportPDF(const QString& fname)
{
	exportVector(fname);
}

void Graph::exportVector(const QString& fileName, int res, bool color, bool keepAspect, QPrinter::PageSize pageSize)
{
	if ( fileName.isEmpty() ){
		QMessageBox::critical(this, tr("Error"),
		tr("Please provide a valid file name!"));
        return;
	}

	QPrinter printer;
    printer.setDocName (this->name());
    printer.setCreator("SciDAVis");
	printer.setFullPage(true);
	printer.setOutputFileName(fileName);
    if (fileName.contains(".eps"))
    	printer.setOutputFormat(QPrinter::PostScriptFormat);

	if (res)
		printer.setResolution(res);

	QRect canvasRect = canvas->rect();
    if (pageSize == QPrinter::Custom)
        printer.setPageSize(Layer::minPageSize(printer, canvasRect));
    else
        printer.setPageSize(pageSize);

	double canvas_aspect = double(canvasRect.width())/double(canvasRect.height());
	if (canvas_aspect < 1)
		printer.setOrientation(QPrinter::Portrait);
	else
		printer.setOrientation(QPrinter::Landscape);

	if (color)
		printer.setColorMode(QPrinter::Color);
	else
		printer.setColorMode(QPrinter::GrayScale);

    int x_margin, y_margin, width, height;
    if (keepAspect){// export should preserve plot aspect ratio
        double page_aspect = double(printer.width())/double(printer.height());
        if (page_aspect > canvas_aspect){
            y_margin = (int) ((0.1/2.54)*printer.logicalDpiY()); // 1 mm margins
            height = printer.height() - 2*y_margin;
            width = height*canvas_aspect;
            x_margin = (printer.width()- width)/2;
        } else {
            x_margin = (int) ((0.1/2.54)*printer.logicalDpiX()); // 1 mm margins
            width = printer.width() - 2*x_margin;
            height = width/canvas_aspect;
            y_margin = (printer.height()- height)/2;
        }
	} else {
	    x_margin = (int) ((0.1/2.54)*printer.logicalDpiX()); // 1 mm margins
        y_margin = (int) ((0.1/2.54)*printer.logicalDpiY()); // 1 mm margins
        width = printer.width() - 2*x_margin;
        height = printer.height() - 2*y_margin;
	}

    double scaleFactorX = (double)(width)/(double)canvasRect.width();
    double scaleFactorY = (double)(height)/(double)canvasRect.height();

    QPainter paint(&printer);
	for (int i=0; i<(int)d_layer_list.count(); i++){
        Layer *gr = (Layer *)d_layer_list.at(i);
        Plot *myPlot = (Plot *)gr->plotWidget();

        QPoint pos = gr->pos();
        pos = QPoint(int(x_margin + pos.x()*scaleFactorX), int(y_margin + pos.y()*scaleFactorY));

        int layer_width = int(myPlot->frameGeometry().width()*scaleFactorX);
        int layer_height = int(myPlot->frameGeometry().height()*scaleFactorY);

        myPlot->print(&paint, QRect(pos, QSize(layer_width, layer_height)));
    }
}

void Graph::exportSVG(const QString& fname)
{
	#if QT_VERSION >= 0x040300
		QSvgGenerator generator;
        generator.setFileName(fname);
        generator.setSize(canvas->size());

		QPainter p(&generator);
        for (int i=0; i<(int)d_layer_list.count(); i++)
		{
			Layer *gr = (Layer *)d_layer_list.at(i);
			Plot *myPlot = (Plot *)gr->plotWidget();

			QPoint pos = QPoint(gr->pos().x(), gr->pos().y());
			myPlot->print(&p, QRect(pos, myPlot->size()));
		}
		p.end();
	#endif
}

void Graph::copyAllLayers()
{
	QPixmap pic = canvasPixmap();
	QImage image= pic.convertToImage();
	QApplication::clipboard()->setImage(image);
}

void Graph::printActiveLayer()
{
	if (d_active_layer)
	{
		d_active_layer->setScaleOnPrint(d_scale_on_print);
		d_active_layer->printCropmarks(d_print_cropmarks);
		d_active_layer->print();
	}
}

void Graph::print()
{
	QPrinter printer;
	printer.setColorMode (QPrinter::Color);
	printer.setFullPage(true);
    QRect canvasRect = canvas->rect();
    double aspect = double(canvasRect.width())/double(canvasRect.height());
    if (aspect < 1)
        printer.setOrientation(QPrinter::Portrait);
    else
        printer.setOrientation(QPrinter::Landscape);

    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted)
	{
		QPainter paint(&printer);
		printAllLayers(&paint);
		paint.end();
	}
}

void Graph::printAllLayers(QPainter *painter)
{
	if (!painter)
		return;

	QPrinter *printer = (QPrinter *)painter->device();
	QRect paperRect = ((QPrinter *)painter->device())->paperRect();
	QRect canvasRect = canvas->rect();
	QRect pageRect = printer->pageRect();
	QRect cr = canvasRect; // cropmarks rectangle

	if (d_scale_on_print)
	{
        int margin = (int)((1/2.54)*printer->logicalDpiY()); // 1 cm margins
		double scaleFactorX=(double)(paperRect.width()-2*margin)/(double)canvasRect.width();
		double scaleFactorY=(double)(paperRect.height()-2*margin)/(double)canvasRect.height();

        if (d_print_cropmarks)
        {
			cr.moveTo(QPoint(margin + int(cr.x()*scaleFactorX),
							 margin + int(cr.y()*scaleFactorY)));
			cr.setWidth(int(cr.width()*scaleFactorX));
			cr.setHeight(int(cr.height()*scaleFactorX));
        }

		for (int i=0; i<(int)d_layer_list.count(); i++)
		{
			Layer *gr=(Layer *)d_layer_list.at(i);
			Plot *myPlot= gr->plotWidget();

			QPoint pos=gr->pos();
			pos=QPoint(margin + int(pos.x()*scaleFactorX), margin + int(pos.y()*scaleFactorY));

			int width=int(myPlot->frameGeometry().width()*scaleFactorX);
			int height=int(myPlot->frameGeometry().height()*scaleFactorY);

			myPlot->print(painter, QRect(pos, QSize(width,height)));
		}
	}
	else
	{
    	int x_margin = (pageRect.width() - canvasRect.width())/2;
    	int y_margin = (pageRect.height() - canvasRect.height())/2;

        if (d_print_cropmarks)
            cr.moveTo(x_margin, y_margin);

		for (int i=0; i<(int)d_layer_list.count(); i++)
		{
			Layer *gr = (Layer *)d_layer_list.at(i);
			Plot *myPlot = (Plot *)gr->plotWidget();

			QPoint pos = gr->pos();
			pos = QPoint(x_margin + pos.x(), y_margin + pos.y());
			myPlot->print(painter, QRect(pos, myPlot->size()));
		}
	}
	if (d_print_cropmarks)
    {
		cr.addCoords(-1, -1, 2, 2);
    	painter->save();
		painter->setPen(QPen(QColor(Qt::black), 0.5, Qt::DashLine));
		painter->drawLine(paperRect.left(), cr.top(), paperRect.right(), cr.top());
		painter->drawLine(paperRect.left(), cr.bottom(), paperRect.right(), cr.bottom());
		painter->drawLine(cr.left(), paperRect.top(), cr.left(), paperRect.bottom());
		painter->drawLine(cr.right(), paperRect.top(), cr.right(), paperRect.bottom());
		painter->restore();
	}

}

void Graph::setFonts(const QFont& titleFnt, const QFont& scaleFnt,
		const QFont& numbersFnt, const QFont& legendFnt)
{
	for (int i=0;i<(int)d_layer_list.count();i++)
	{
		Layer *gr=(Layer *)d_layer_list.at(i);
		QwtPlot *plot=gr->plotWidget();

		QwtText text = plot->title();
  	    text.setFont(titleFnt);
  	    plot->setTitle(text);
		for (int j= 0;j<QwtPlot::axisCnt;j++)
		{
			plot->setAxisFont (j,numbersFnt);

			text = plot->axisTitle(j );
  	        text.setFont(scaleFnt);
  	        plot->setAxisTitle(j, text);
		}

		QVector<int> keys=gr->textMarkerKeys();
		for (int k=0;k<(int)keys.size();k++)
		{
			TextEnrichment* mrk=(TextEnrichment*)gr->textMarker(keys[k]);
			if (mrk)
				mrk->setFont(legendFnt);
		}
		plot->replot();
	}
	emit modifiedPlot();
}

void Graph::connectLayer(Layer *g)
{
	connect (g,SIGNAL(drawLineEnded(bool)), this, SIGNAL(drawLineEnded(bool)));
	connect (g,SIGNAL(drawTextOff()),this,SIGNAL(drawTextOff()));
	connect (g,SIGNAL(showPlotDialog(int)),this,SIGNAL(showPlotDialog(int)));
	connect (g,SIGNAL(createTable(const QString&,int,int,const QString&)),
			this,SIGNAL(createTable(const QString&,int,int,const QString&)));
	connect (g,SIGNAL(viewLineDialog()),this,SIGNAL(showLineDialog()));
	connect (g,SIGNAL(showContextMenu()),this,SIGNAL(showLayerContextMenu()));
	connect (g,SIGNAL(showLayerButtonContextMenu()),this,SIGNAL(showLayerButtonContextMenu()));
	connect (g,SIGNAL(showAxisDialog(int)),this,SIGNAL(showAxisDialog(int)));
	connect (g,SIGNAL(axisDblClicked(int)),this,SIGNAL(showScaleDialog(int)));
	connect (g,SIGNAL(xAxisTitleDblClicked()),this,SIGNAL(showXAxisTitleDialog()));
	connect (g,SIGNAL(yAxisTitleDblClicked()),this,SIGNAL(showYAxisTitleDialog()));
	connect (g,SIGNAL(rightAxisTitleDblClicked()),this,SIGNAL(showRightAxisTitleDialog()));
	connect (g,SIGNAL(topAxisTitleDblClicked()),this,SIGNAL(showTopAxisTitleDialog()));
	connect (g,SIGNAL(showMarkerPopupMenu()),this,SIGNAL(showMarkerPopupMenu()));
	connect (g,SIGNAL(showCurveContextMenu(int)),this,SIGNAL(showCurveContextMenu(int)));
	connect (g,SIGNAL(cursorInfo(const QString&)),this,SIGNAL(cursorInfo(const QString&)));
	connect (g,SIGNAL(viewImageDialog()),this,SIGNAL(showImageDialog()));
	connect (g,SIGNAL(viewTitleDialog()),this,SIGNAL(viewTitleDialog()));
	connect (g,SIGNAL(modified()),this,SIGNAL(modifiedPlot()));
	connect (g,SIGNAL(selected(Layer*)),this, SLOT(setActiveLayer(Layer*)));
	connect (g,SIGNAL(viewTextDialog()),this,SIGNAL(showTextDialog()));
	connect (g,SIGNAL(createIntensityTable(const QString&)),
			this,SIGNAL(createIntensityTable(const QString&)));
}

void Graph::addTextLayer(int f, const QFont& font,
		const QColor& textCol, const QColor& backgroundCol)
{
	defaultTextMarkerFont = font;
	defaultTextMarkerFrame = f;
	defaultTextMarkerColor = textCol;
	defaultTextMarkerBackground = backgroundCol;

	addTextOn=TRUE;
	QApplication::setOverrideCursor(Qt::IBeamCursor);
	canvas->grabMouse();
}

void Graph::addTextLayer(const QPoint& pos)
{
	Layer* g=addLayer();
	g->removeLegend();
	g->setTitle("");
	QVector<bool> axesOn(4);
	for (int j=0;j<4;j++)
		axesOn[j] = false;
	g->enableAxes(axesOn);
	g->setIgnoreResizeEvents(true);
	g->setTextMarkerDefaults(defaultTextMarkerFrame, defaultTextMarkerFont,
			defaultTextMarkerColor, defaultTextMarkerBackground);
	TextEnrichment *mrk = g->newLegend(tr("enter your text here"));
	QSize size = mrk->rect().size();
	setLayerGeometry(pos.x(), pos.y(), size.width()+10, size.height()+10);
	g->setIgnoreResizeEvents(false);
	g->show();
	QApplication::restoreOverrideCursor();
	canvas->releaseMouse();
	addTextOn=false;
	emit drawTextOff();
	emit modifiedPlot();
}
		
void Graph::keyPressEvent(QKeyEvent * e)
{
	if (e->key() == Qt::Key_F12)
	{
		if (d_layers_selector)
			delete d_layers_selector;
		int index = d_layer_list.indexOf(d_active_layer) + 1;
		if (index >= d_layer_list.size())
			index = 0;
		Layer *g=(Layer *)d_layer_list.at(index);
		if (g)
			setActiveLayer(g);
		return;
	}

	if (e->key() == Qt::Key_F10)
	{
		if (d_layers_selector)
			delete d_layers_selector;
		int index=d_layer_list.indexOf(d_active_layer) - 1;
		if (index < 0)
			index = d_layer_list.size() - 1;
		Layer *g=(Layer *)d_layer_list.at(index);
		if (g)
			setActiveLayer(g);
		return;
	}

	if (e->key() == Qt::Key_F11)
	{
		emit showWindowContextMenu();
		return;
	}
}

void Graph::wheelEvent ( QWheelEvent * e )
{
	QApplication::setOverrideCursor(Qt::waitCursor);

	bool resize=false;
	QPoint aux;
	QSize intSize;
	Layer *resize_layer = 0;
	// Get the position of the mouse
	int xMouse=e->x();
	int yMouse=e->y();
	for (int i=0;i<(int)d_layer_list.count();i++)
	{
		Layer *gr=(Layer *)d_layer_list.at(i);
		intSize=gr->plotWidget()->size();
		aux=gr->pos();
		if(xMouse>aux.x() && xMouse<(aux.x()+intSize.width()))
		{
			if(yMouse>aux.y() && yMouse<(aux.y()+intSize.height()))
			{
				resize_layer=gr;
				resize=TRUE;
			}
		}
	}
	if(resize && (e->state()==Qt::AltButton || e->state()==Qt::ControlButton || e->state()==Qt::ShiftButton))
	{
		intSize=resize_layer->plotWidget()->size();
		// If alt is pressed then change the width
		if(e->state()==Qt::AltButton)
		{
			if(e->delta()>0)
			{
				intSize.rwidth()+=5;
			}
			else if(e->delta()<0)
			{
				intSize.rwidth()-=5;
			}
		}
		// If crt is pressed then changed the height
		else if(e->state()==Qt::ControlButton)
		{
			if(e->delta()>0)
			{
				intSize.rheight()+=5;
			}
			else if(e->delta()<0)
			{
				intSize.rheight()-=5;
			}
		}
		// If shift is pressed then resize
		else if(e->state()==Qt::ShiftButton)
		{
			if(e->delta()>0)
			{
				intSize.rwidth()+=5;
				intSize.rheight()+=5;
			}
			else if(e->delta()<0)
			{
				intSize.rwidth()-=5;
				intSize.rheight()-=5;
			}
		}

		aux=resize_layer->pos();
		resize_layer->setGeometry(QRect(QPoint(aux.x(),aux.y()),intSize));
		resize_layer->plotWidget()->resize(intSize);

		emit modifiedPlot();
	}
	QApplication::restoreOverrideCursor();
}

bool Graph::isEmpty ()
{
	if (d_layer_count <= 0)
		return true;
	else
		return false;
}

QString Graph::saveToString(const QString& geometry)
{
	QString s="<multiLayer>\n";
	s+=QString(name())+"\t";
	s+=QString::number(cols)+"\t";
	s+=QString::number(rows)+"\t";
	s+=birthDate()+"\n";
	s+=geometry;
	s+="WindowLabel\t" + windowLabel() + "\t" + QString::number(captionPolicy()) + "\n";
	s+="Margins\t"+QString::number(left_margin)+"\t"+QString::number(right_margin)+"\t"+
		QString::number(top_margin)+"\t"+QString::number(bottom_margin)+"\n";
	s+="Spacing\t"+QString::number(rowsSpace)+"\t"+QString::number(colsSpace)+"\n";
	s+="LayerCanvasSize\t"+QString::number(l_canvas_width)+"\t"+QString::number(l_canvas_height)+"\n";
	s+="Alignement\t"+QString::number(hor_align)+"\t"+QString::number(vert_align)+"\n";

	for (int i=0;i<(int)d_layer_list.count();i++)
	{
		Layer* ag=(Layer*)d_layer_list.at(i);
		s+=ag->saveToString();
	}
	return s+"</multiLayer>\n";
}

QString Graph::saveAsTemplate(const QString& geometryInfo)
{
	QString s="<multiLayer>\t";
	s+=QString::number(rows)+"\t";
	s+=QString::number(cols)+"\n";
	s+= geometryInfo;
	s+="Margins\t"+QString::number(left_margin)+"\t"+QString::number(right_margin)+"\t"+
		QString::number(top_margin)+"\t"+QString::number(bottom_margin)+"\n";
	s+="Spacing\t"+QString::number(rowsSpace)+"\t"+QString::number(colsSpace)+"\n";
	s+="LayerCanvasSize\t"+QString::number(l_canvas_width)+"\t"+QString::number(l_canvas_height)+"\n";
	s+="Alignement\t"+QString::number(hor_align)+"\t"+QString::number(vert_align)+"\n";

	for (int i=0;i<(int)d_layer_list.count();i++)
	{
		Layer* ag=(Layer*)d_layer_list.at(i);
		s += ag->saveToString(true);
	}
	return s;
}

void Graph::mousePressEvent ( QMouseEvent * e )
{
	int margin = 5;
	QPoint pos = canvas->mapFromParent(e->pos());
	// iterate backwards, so layers on top are preferred for selection
	QList<Layer*>::iterator i = d_layer_list.end();
	while (i!=d_layer_list.begin()) {
		--i;
		QRect igeo = (*i)->frameGeometry();
		igeo.addCoords(-margin, -margin, margin, margin);
		if (igeo.contains(pos)) {
			if (e->modifiers() & Qt::ShiftModifier) {
				if (d_layers_selector)
					d_layers_selector->add(*i);
				else {
					d_layers_selector = new SelectionMoveResizer(*i);
					connect(d_layers_selector, SIGNAL(targetsChanged()), this, SIGNAL(modifiedPlot()));
				}
			} else {
				setActiveLayer((Layer*) (*i));
				d_active_layer->raise();
				if (!d_layers_selector) {
					d_layers_selector = new SelectionMoveResizer(*i);
					connect(d_layers_selector, SIGNAL(targetsChanged()), this, SIGNAL(modifiedPlot()));
				}
			}
			return;
		}
	}
	if (d_layers_selector)
		delete d_layers_selector;
}

void Graph::setMargins (int lm, int rm, int tm, int bm)
{
	if (left_margin != lm)
		left_margin = lm;
	if (right_margin != rm)
		right_margin = rm;
	if (top_margin != tm)
		top_margin = tm;
	if (bottom_margin != bm)
		bottom_margin = bm;
}

void Graph::setSpacing (int rgap, int cgap)
{
	if (rowsSpace != rgap)
		rowsSpace = rgap;
	if (colsSpace != cgap)
		colsSpace = cgap;
}

void Graph::setLayerCanvasSize (int w, int h)
{
	if (l_canvas_width != w)
		l_canvas_width = w;
	if (l_canvas_height != h)
		l_canvas_height = h;
}

void Graph::setAlignement (int ha, int va)
{
	if (hor_align != ha)
		hor_align = ha;

	if (vert_align != va)
		vert_align = va;
}

void Graph::setLayersNumber(int n)
{
	if (d_layer_count == n)
		return;

	int dn = d_layer_count - n;
	if (dn > 0)
	{
		for (int i = 0; i < dn; i++)
		{//remove layer buttons
			LayerButton *btn=(LayerButton*)d_button_list.last();
			if (btn)
			{
				btn->close();
				d_button_list.removeLast();
			}

			Layer *g = (Layer *)d_layer_list.last();
			if (g)
			{//remove layers
				if (g->zoomOn() || g->activeTool())
					setPointerCursor();

				g->close();
				d_layer_list.removeLast();
			}
		}
		d_layer_count = n;
		if (!d_layer_count)
		{
			d_active_layer = 0;
			return;
		}

		// check whether the active Layer.has been deleted
		if(d_layer_list.indexOf(d_active_layer) == -1)
			d_active_layer=(Layer*) d_layer_list.last();
		for (int j=0;j<(int)d_layer_list.count();j++)
		{
			Layer *gr=(Layer *)d_layer_list.at(j);
			if (gr == d_active_layer)
			{
				LayerButton *button=(LayerButton *)d_button_list.at(j);
				button->setOn(TRUE);
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < abs(dn); i++)
			addLayer();
	}

	emit modifiedPlot();
}

void Graph::copy(Graph* ml)
{
	hide();//FIXME: find a better way to avoid a resize event
    resize(ml->size());
	
	setSpacing(ml->rowsSpacing(), ml->colsSpacing());
	setAlignement(ml->horizontalAlignement(), ml->verticalAlignement());
	setMargins(ml->leftMargin(), ml->rightMargin(), ml->topMargin(), ml->bottomMargin());

	foreach(Layer *g, ml->layers()) {
		Layer* g2 = addLayer(g->pos().x(), g->pos().y(), g->width(), g->height());
		g2->copy(g);
		g2->setIgnoreResizeEvents(g->ignoresResizeEvents());
		g2->setAutoscaleFonts(g->autoscaleFonts());
	}
	show();
}

bool Graph::focusNextPrevChild ( bool next )
{
	if (!d_active_layer)
		return true;

	return d_active_layer->focusNextPrevChild(next);
}

void Graph::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::WindowStateChange) {
		if ( windowState() & Qt::WindowMaximized )
	     	d_max_size = QSize(width(), height() - LayerButton::btnSize());
        else if ( windowState() & Qt::WindowMinimized )
        {
            if (((QWindowStateChangeEvent*)event)->oldState() == Qt::WindowMaximized)
                resizeLayers(d_normal_size, d_max_size, true);
        }
        else if ( windowState() == Qt::WindowNoState )
            d_normal_size = QSize(width(), height() - LayerButton::btnSize());
	}
	MyWidget::changeEvent(event);
}

void Graph::setHidden()
{
	if (status() == MyWidget::Maximized)
		resizeLayers(d_normal_size, d_max_size, false);

	MyWidget::setHidden();
}
#endif

Column * Graph::selectColumn()
{
	Project * prj = project();
	if (!prj) return 0;
	
	QList<AbstractAspect *> list = prj->descendantsThatInherit("Column");
	if (list.isEmpty()) return 0;
	
	QDialog dialog;
	QVBoxLayout layout(&dialog);
	QLabel label(tr("Choose Column to plot"));
	QComboBox selection;
	for (int i=0; i<list.size(); i++)
		selection.addItem(list.at(i)->name(), i);

    QDialogButtonBox button_box(&dialog);
    button_box.setOrientation(Qt::Horizontal);
    button_box.setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);
    QObject::connect(&button_box, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&button_box, SIGNAL(rejected()), &dialog, SLOT(reject()));

	layout.addWidget(&label);
	layout.addWidget(&selection);
	layout.addWidget(&button_box);

	dialog.setWindowTitle(label.text());
	if (dialog.exec() != QDialog::Accepted)
		return 0;
	int index = selection.currentIndex();
	if (index >= 0 && index < list.size())
		return static_cast<Column *>(list.at(index));
	else
		return 0;
}


/* ========================= static methods ======================= */
ActionManager * Graph::action_manager = 0;

ActionManager * Graph::actionManager()
{
	if (!action_manager)
		initActionManager();
	
	return action_manager;
}

void Graph::initActionManager()
{
	if (!action_manager)
		action_manager = new ActionManager();

	action_manager->setTitle(tr("Graph"));
	volatile Graph * action_creator = new Graph(); // initialize the action texts
	delete action_creator;
}

