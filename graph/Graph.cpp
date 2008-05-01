/***************************************************************************
    File                 : Graph.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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

#include "Graph.h"
#include "Plot.h"
#include "TextEnrichment.h"
#include "SelectionMoveResizer.h"

#include <gsl/gsl_vector.h>

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

Graph::Graph(const QString& label, QWidget* parent, const char* name, Qt::WFlags f)
: MyWidget(label,parent,name,f)
{
	if ( !name )
		setName( "multilayer plot" );

	QPalette pal = palette();
	pal.setColor(QPalette::Active, QPalette::Window, QColor(Qt::white));
	pal.setColor(QPalette::Inactive, QPalette::Window, QColor(Qt::white));
	pal.setColor(QPalette::Disabled, QPalette::Window, QColor(Qt::white));
	setPalette(pal);

	QDateTime dt = QDateTime::currentDateTime ();
	setBirthDate(dt.toString(Qt::LocalDate));

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

	layerButtonsBox = new QHBoxLayout();
	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addLayout(layerButtonsBox);
	hbox->addStretch();

	canvas = new QWidget();
	canvas->installEventFilter(this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addLayout(hbox);
	layout->addWidget(canvas, 1);
	layout->setMargin(0);
	layout->setSpacing(0);
	setGeometry(QRect( 0, 0, d_layer_default_width, d_layer_default_height ));
	setFocusPolicy(Qt::StrongFocus);
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
    layerButtonsBox->addWidget(button);
	return button;
}

Layer* Graph::addLayer(int x, int y, int width, int height)
{
	addLayerButton();
	if (!width && !height) {
		width =	d_layer_default_width;
		height = d_layer_default_height;
	}

	Layer* g = new Layer(canvas);
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
    canvas->resize(size().width(), size().height() - LayerButton::btnSize());
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

bool Graph::eventFilter(QObject *object, QEvent *e)
{
	if(e->type() == QEvent::MouseButtonPress && object == (QObject *)canvas)
	{
		const QMouseEvent *me = (const QMouseEvent *)e;
		if (me->button()==Qt::LeftButton && addTextOn)
			addTextLayer(me->pos());

		return false;
	}
	else if(e->type() == QEvent::Resize && object == (QObject *)canvas)
	{
		resizeLayers((const QResizeEvent *)e);
	}
	else if (e->type()==QEvent::ContextMenu && object == titleBar)
	{
		emit showTitleBarMenu();
		((QContextMenuEvent*)e)->accept();
		return true;
	}
	return MyWidget::eventFilter(object, e);
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
