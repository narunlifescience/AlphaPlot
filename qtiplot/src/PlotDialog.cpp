/***************************************************************************
    File                 : PlotDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Custom curves dialog

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
#include "ApplicationWindow.h"
#include "PlotDialog.h"
#include "ColorBox.h"
#include "ColorButton.h"
#include "PatternBox.h"
#include "SymbolBox.h"
#include "Table.h"
#include "MyParser.h"
#include "QwtHistogram.h"
#include "VectorCurve.h"
#include "QwtErrorPlotCurve.h"
#include "BoxCurve.h"
#include "FunctionCurve.h"
#include "Spectrogram.h"
#include "QwtPieCurve.h"
#include "ColorMapEditor.h"
#include "pixmaps.h"

#include <QTreeWidget>
#include <QLineEdit>
#include <QLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QWidget>
#include <QMessageBox>
#include <QComboBox>
#include <QWidgetList>
#include <QFileDialog>
#include <QGroupBox>
#include <QFontDialog>
#include <QColorDialog>
#include <QShortcut>
#include <QKeySequence>
#include <QDoubleSpinBox>
#include <QMenu>
#include <QDateTime>

PlotDialog::PlotDialog(bool showExtended, QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
: QDialog(parent, name, modal, fl),
  d_ml(0)
{
	if ( !name )
		setName( "PlotDialog" );
	setWindowTitle( tr( "QtiPlot - Plot details" ) );

	listBox = new QTreeWidget();
    listBox->setColumnCount(1);
	listBox->header()->hide();
    listBox->setIndentation(15);

    QGridLayout *gl = new QGridLayout(this);
	gl->setSizeConstraint(QLayout::SetFixedSize);
    gl->addWidget(listBox, 0, 0);

	privateTabWidget = new QTabWidget();
    gl->addWidget(privateTabWidget, 0, 1);

    curvePlotTypeBox = new QWidget();
    QHBoxLayout *hb1 = new QHBoxLayout(curvePlotTypeBox);
    hb1->addWidget(new QLabel(tr("Plot type")));
    boxPlotType = new QComboBox();
    boxPlotType->setEditable(false);
    hb1->addWidget(boxPlotType);
    gl->addWidget(curvePlotTypeBox, 1, 0);

	initAxesPage();
	initLinePage();
	initSymbolsPage();
	initHistogramPage();
	initErrorsPage();
	initSpacingPage();
	initVectPage();
	initBoxPage();
	initPercentilePage();
	initSpectrogramPage();
	initPiePage();
	initLayerPage();
	initFontsPage();
	initPrintPage();

	clearTabWidget();

    QHBoxLayout* hb2 = new QHBoxLayout();
	btnMore = new QPushButton("&<<");
	btnMore->setFixedWidth(25);
	btnMore->setCheckable(true);
	if (showExtended)
		btnMore->toggle ();
    hb2->addWidget(btnMore);
	btnWorksheet = new QPushButton(tr( "&Worksheet" ) );
    hb2->addWidget(btnWorksheet);
	buttonOk = new QPushButton(tr( "&OK" ));
	buttonOk->setDefault( true );
    hb2->addWidget(buttonOk);
	buttonCancel = new QPushButton(tr( "&Cancel" ));
    hb2->addWidget(buttonCancel);
    buttonApply = new QPushButton(tr( "&Apply" ));
    hb2->addWidget(buttonApply);
	btnEditCurve = new QPushButton(tr("&Plot Associations..."));
    hb2->addWidget(btnEditCurve);
    hb2->addStretch();
    gl->addLayout(hb2, 1, 1);

	resize(minimumSize());

	connect(btnMore, SIGNAL(toggled(bool)), this, SLOT(showAll(bool)));

	connect( buttonOk, SIGNAL(clicked()), this, SLOT(quit() ) );
	connect( buttonCancel, SIGNAL(clicked()), this, SLOT(close()));
	connect( buttonApply, SIGNAL(clicked() ), this, SLOT(acceptParams() ) );
	connect( btnWorksheet, SIGNAL(clicked()), this, SLOT(showWorksheet()));
	connect( btnEditCurve, SIGNAL(clicked()), this, SLOT(editCurve()));
	connect(listBox, SIGNAL(itemDoubleClicked( QTreeWidgetItem *, int)),
            this, SLOT(showPlotAssociations( QTreeWidgetItem *, int)));
	connect(listBox, SIGNAL(currentItemChanged (QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(updateTabWindow(QTreeWidgetItem *, QTreeWidgetItem *)));
    connect(listBox, SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(updateTreeWidgetItem(QTreeWidgetItem *)));
    connect(listBox, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(updateTreeWidgetItem(QTreeWidgetItem *)));
	connect(boxPlotType, SIGNAL(currentIndexChanged(int)), this, SLOT(changePlotType(int)));

	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(removeSelectedCurve()));
}

void PlotDialog::showAll(bool all)
{
	if(all)
	{
		listBox->show();
		listBox->setFocus();

		QTreeWidgetItem *item = listBox->currentItem();
    	if (item->type() == CurveTreeItem::PlotCurveTreeItem)
        	curvePlotTypeBox->show();

		btnMore->setText("&>>");
	}
	else
	{
		listBox->hide();
		curvePlotTypeBox->hide();
		btnMore->setText("&<<");
	}
}

void PlotDialog::showPlotAssociations(QTreeWidgetItem *item, int)
{
	if (!item)
		return;

	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (!app)
		return;

    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

    QwtPlotItem *it = (QwtPlotItem *)((CurveTreeItem *)item)->plotItem();
    if (!it)
        return;

    if (it->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
    {
  	    Spectrogram *sp = (Spectrogram *)it;
  	    if (sp->matrix())
  	    	sp->matrix()->showMaximized();
        return;
    }

	if (((PlotCurve *)it)->type() == Graph::Function)
	{
	    close();
  	    app->showFunctionDialog(((CurveTreeItem *)item)->graph(), ((CurveTreeItem *)item)->plotItemIndex());
	}
	else
	{
	    close();
        app->showPlotAssociations(((CurveTreeItem *)item)->plotItemIndex());
	}
}

void PlotDialog::editCurve()
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();

	CurveTreeItem *item = (CurveTreeItem *)listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

	int index = item->plotItemIndex();
	int curveType = ((PlotCurve *)item->plotItem())->type();

	close();

	if (app)
	{
		if (curveType == Graph::Function)
			app->showFunctionDialog(item->graph(), index);
		else
			app->showPlotAssociations(index);
	}
}

void PlotDialog::changePlotType(int plotType)
{
    CurveTreeItem *item = (CurveTreeItem *)listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;
    Graph *graph = item->graph();
    if (!graph)
        return;

	int curveType = item->plotItemType();
	if (boxPlotType->count() == 1 || (curveType == plotType))
		return;

	if (curveType == Graph::ColorMap || curveType == Graph::ContourMap || curveType == Graph::GrayMap)
  		clearTabWidget();
  	else if (curveType == Graph::VectXYAM || curveType == Graph::VectXYXY)
	{
		if ((plotType && curveType == Graph::VectXYAM) ||
				(!plotType && curveType == Graph::VectXYXY))
			return;

		clearTabWidget();
		insertTabs(curveType);

		VectorCurve *v = (VectorCurve*)item->plotItem();
		if (plotType)
		{
			graph->setCurveType(item->plotItemIndex(), Graph::VectXYAM);
			v->setVectorStyle(VectorCurve::XYAM);
		}
		else
		{
			graph->setCurveType(item->plotItemIndex(), Graph::VectXYXY);
			v->setVectorStyle(VectorCurve::XYXY);
		}
		customVectorsPage(plotType);
	}
	else
	{
		clearTabWidget();
		insertTabs(plotType);

		graph->setCurveType(item->plotItemIndex(), plotType);

		boxConnect->setCurrentIndex(1);//show line for Line and LineSymbol plots

		QwtSymbol s = QwtSymbol(QwtSymbol::Ellipse, QBrush(), QPen(), QSize(9,9));
		if (plotType == Graph::Line)
			s.setStyle(QwtSymbol::NoSymbol);
		else if (plotType == Graph::Scatter)
			graph->setCurveStyle(item->plotItemIndex(), QwtPlotCurve::NoCurve);
		else if (plotType == Graph::LineSymbols)
			graph->setCurveStyle(item->plotItemIndex(), QwtPlotCurve::Lines);

        graph->setCurveSymbol(item->plotItemIndex(), s);

		if (plotType)
		{
			boxSymbolStyle->setCurrentIndex(1);
			boxFillSymbol->setChecked(true);
			boxFillColor->setEnabled(true);
		}
	}
	acceptParams();
}

void PlotDialog::initFontsPage()
{
    QGroupBox *boxFonts = new QGroupBox();
    QGridLayout *fl = new QGridLayout(boxFonts);

    btnTitle = new QPushButton(tr("Titles"));
	btnAxesLabels = new QPushButton(tr("Axes Labels"));
	btnAxesNumbers = new QPushButton(tr("Axes Numbers"));
	btnLegend = new QPushButton(tr("Legends"));

	fl->addWidget(btnTitle, 0, 0);
	fl->addWidget(btnAxesLabels, 0, 1);
	fl->addWidget(btnAxesNumbers, 0, 2);
	fl->addWidget(btnLegend, 0, 3);
	fl->setRowStretch(1, 1);
	fl->setColumnStretch(4, 1);

	fontsPage = new QWidget();
	QHBoxLayout *hl = new QHBoxLayout(fontsPage);
    hl->addWidget(boxFonts);
	privateTabWidget->addTab(fontsPage, tr( "Fonts" ) );

    connect( btnTitle, SIGNAL( clicked() ), this, SLOT( setTitlesFont() ) );
	connect( btnAxesLabels, SIGNAL( clicked() ), this, SLOT( setAxesLabelsFont() ) );
	connect( btnAxesNumbers, SIGNAL( clicked() ), this, SLOT( setAxesNumbersFont() ) );
	connect( btnLegend, SIGNAL( clicked() ), this, SLOT( setLegendsFont() ) );
}

void PlotDialog::initLayerPage()
{
    layerPage = new QWidget();

    QGroupBox * boxBkg = new QGroupBox();
    QGridLayout * boxBkgLayout = new QGridLayout( boxBkg );

    boxBkgLayout->addWidget( new QLabel(tr( "Background Color" )), 0, 0 );
    boxBackgroundColor = new ColorButton();
    boxBkgLayout->addWidget( boxBackgroundColor, 0, 1 );
    boxBkgLayout->addWidget( new QLabel(tr( "Opacity" )), 0, 2 );
    boxBackgroundTransparency = new QSpinBox();
    boxBackgroundTransparency->setRange(0, 255);
    boxBackgroundTransparency->setSingleStep(5);
    boxBackgroundTransparency->setWrapping(true);
    boxBackgroundTransparency->setSpecialValueText(tr("Transparent"));
    boxBkgLayout->addWidget( boxBackgroundTransparency, 0, 3 );

    boxBkgLayout->addWidget( new QLabel(tr("Canvas Color" )), 1, 0);
    boxCanvasColor = new ColorButton();
    boxBkgLayout->addWidget( boxCanvasColor, 1, 1 );
    boxBkgLayout->addWidget( new QLabel(tr( "Opacity" )), 1, 2 );
    boxCanvasTransparency = new QSpinBox();
    boxCanvasTransparency->setRange(0, 255);
    boxCanvasTransparency->setSingleStep(5);
    boxCanvasTransparency->setWrapping(true);
    boxCanvasTransparency->setSpecialValueText(tr("Transparent"));
    boxBkgLayout->addWidget( boxCanvasTransparency, 1, 3 );

    boxBkgLayout->addWidget( new QLabel(tr("Border Color" )), 2, 0);
    boxBorderColor = new ColorButton();
    boxBkgLayout->addWidget( boxBorderColor, 2, 1);

    boxBkgLayout->addWidget( new QLabel(tr( "Width" )), 2, 2);
    boxBorderWidth = new QSpinBox();
    boxBkgLayout->addWidget( boxBorderWidth, 2, 3);
    boxBkgLayout->setRowStretch( 4, 1 );

    QGroupBox * box4 = new QGroupBox(QString());
    QGridLayout * box4Layout = new QGridLayout( box4 );

    box4Layout->addWidget( new QLabel(tr( "Margin" )), 0, 0 );
    boxMargin = new QSpinBox();
    boxMargin->setRange( 0, 1000 );
    boxMargin->setSingleStep(5);
    box4Layout->addWidget( boxMargin, 0, 1 );

    boxAntialiasing = new QCheckBox(tr("Antialiasing"));
    box4Layout->addWidget( boxAntialiasing, 1, 1 );

    boxAll = new QCheckBox(tr("Apply to all layers"));
    box4Layout->addWidget( boxAll, 2, 1 );
    box4Layout->setRowStretch( 3, 1 );

    QHBoxLayout * hl = new QHBoxLayout( layerPage );
    hl->addWidget(boxBkg);
    hl->addWidget(box4);

    privateTabWidget->addTab(layerPage, tr("Layer"));

	connect(boxBackgroundTransparency, SIGNAL(valueChanged(int)), this, SLOT(updateBackgroundTransparency(int)));
	connect(boxCanvasTransparency, SIGNAL(valueChanged(int)), this, SLOT(updateCanvasTransparency(int)));
	connect(boxAntialiasing, SIGNAL(toggled(bool)), this, SLOT(updateAntialiasing(bool)));
	connect(boxMargin, SIGNAL(valueChanged (int)), this, SLOT(changeMargin(int)));
	connect(boxBorderColor, SIGNAL(clicked()), this, SLOT(pickBorderColor()));
	connect(boxBackgroundColor, SIGNAL(clicked()), this, SLOT(pickBackgroundColor()));
	connect(boxCanvasColor, SIGNAL(clicked()), this, SLOT(pickCanvasColor()));
	connect(boxBorderWidth,SIGNAL(valueChanged (int)), this, SLOT(updateBorder(int)));
}

void PlotDialog::initPiePage()
{
	piePage = new QWidget();

	QGridLayout *gl1 = new QGridLayout();
	gl1->addWidget(new QLabel( tr( "Color" )), 0, 0);

	boxPieLineColor = new ColorBox(false);
	gl1->addWidget(boxPieLineColor, 0, 1);

	gl1->addWidget(new QLabel(tr( "Style" )), 1, 0);
	boxPieLineStyle = new QComboBox( false );
	boxPieLineStyle->insertItem("_____");
	boxPieLineStyle->insertItem("- - -");
	boxPieLineStyle->insertItem(".....");
	boxPieLineStyle->insertItem("_._._");
	boxPieLineStyle->insertItem("_.._..");
	gl1->addWidget(boxPieLineStyle);

	gl1->addWidget(new QLabel(tr( "Width")), 2, 0);
	boxPieLineWidth = new QSpinBox();
	gl1->addWidget(boxPieLineWidth, 2, 1);
	gl1->setRowStretch(3,1);

	QGroupBox *gb1 = new QGroupBox(tr( "Border" ));
	gb1->setLayout(gl1);

	QGridLayout *gl2 = new QGridLayout();
	gl2->addWidget(new QLabel( tr( "First color" )), 0, 0);

	boxFirstColor = new ColorBox(false);
	gl2->addWidget(boxFirstColor, 0, 1);

	gl2->addWidget(new QLabel( tr( "Pattern" )), 1, 0);
	boxPiePattern = new PatternBox(false);
	gl2->addWidget(boxPiePattern, 1, 1);
	gl2->addWidget(new QLabel( tr( "Pie radius" )), 2, 0);

	boxRadius = new QSpinBox();
	boxRadius->setRange(0, 10000);
	boxRadius->setSingleStep(10);

	gl2->addWidget(boxRadius, 2, 1);
	gl2->setRowStretch(3,1);

	QGroupBox *gb2 = new QGroupBox(tr( "Fill" ));
	gb2->setLayout(gl2);

	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(gb1);
	hl->addWidget(gb2);
	piePage->setLayout(hl);

	privateTabWidget->addTab(piePage, tr( "Pie" ) );
}

void PlotDialog::initPrintPage()
{
	QGroupBox *gb = new QGroupBox();
    QVBoxLayout *vl = new QVBoxLayout(gb);
	boxScaleLayers = new QCheckBox(tr("&Scale layers to paper size"));
	vl->addWidget(boxScaleLayers);
	boxPrintCrops = new QCheckBox(tr("Print Crop&marks"));
	vl->addWidget(boxPrintCrops);
    vl->addStretch();

	printPage = new QWidget();
	QHBoxLayout* hlayout = new QHBoxLayout(printPage);
	hlayout->addWidget(gb);
	privateTabWidget->addTab(printPage, tr( "Print" ) );
}

void PlotDialog::initAxesPage()
{
	QGroupBox *gb = new QGroupBox(tr( "Attach curve to: " ));
    QGridLayout *gl = new QGridLayout(gb);
    gl->addWidget(new QLabel( tr( "x Axis" )), 0, 0);
	boxXAxis = new QComboBox();
	boxXAxis->setEditable(false);
	boxXAxis->addItem(tr("Bottom"));
	boxXAxis->addItem(tr("Top"));
	gl->addWidget(boxXAxis, 0, 1);
	gl->addWidget(new QLabel( tr( "y Axis" )), 1, 0);
	boxYAxis = new QComboBox();
	boxYAxis->setEditable(false);
	boxYAxis->addItem(tr("Left"));
	boxYAxis->addItem(tr("Right"));
	gl->addWidget(boxYAxis, 1, 1);
    gl->setRowStretch (2, 1);

	axesPage = new QWidget();
	QHBoxLayout* hlayout = new QHBoxLayout(axesPage);
	hlayout->addWidget(gb);
	privateTabWidget->addTab(axesPage, tr( "Axes" ) );
}

void PlotDialog::initLinePage()
{
	QGroupBox *gb = new QGroupBox();
	QGridLayout *gl1 = new QGridLayout(gb);
    gl1->addWidget(new QLabel( tr( "Connect" )), 0, 0);

	boxConnect = new QComboBox();
	boxConnect->setEditable(false);
	boxConnect->addItem(tr("No line"));
	boxConnect->addItem(tr("Lines"));
	boxConnect->addItem(tr("Sticks"));
	boxConnect->addItem(tr("Horizontal Steps"));
	boxConnect->addItem(tr("Dots"));
	boxConnect->addItem(tr("Spline"));
	boxConnect->addItem(tr("Vertical Steps"));
	gl1->addWidget(boxConnect, 0, 1);

	gl1->addWidget(new QLabel(tr( "Style" )), 1, 0);
	boxLineStyle = new QComboBox();
	boxLineStyle->setEditable(false);
	boxLineStyle->addItem("_____");
	boxLineStyle->addItem("_ _ _");
	boxLineStyle->addItem(".....");
	boxLineStyle->addItem("_._._");
	boxLineStyle->addItem("_.._..");
	gl1->addWidget(boxLineStyle, 1, 1);

	gl1->addWidget(new QLabel(tr( "Width" )), 2, 0);
	boxLineWidth = new QSpinBox();
	boxLineWidth->setMinValue( 1 );
	boxLineWidth->setValue( 1 );
	gl1->addWidget(boxLineWidth, 2, 1);

	gl1->addWidget(new QLabel(tr( "Color" )), 3, 0);
	boxLineColor = new ColorBox( false);
	gl1->addWidget(boxLineColor, 3, 1);
    gl1->setRowStretch (4, 1);

	fillGroupBox = new QGroupBox(tr( "Fill area under curve" ));
	fillGroupBox->setCheckable(true);
	QGridLayout *gl2 = new QGridLayout(fillGroupBox);
    gl2->addWidget(new QLabel(tr( "Fill color" )), 0, 0);
	boxAreaColor = new ColorBox(false);
	gl2->addWidget(boxAreaColor, 0, 1);
	gl2->addWidget(new QLabel(tr( "Pattern" )), 1, 0);
	boxPattern = new PatternBox(false);
	gl2->addWidget(boxPattern, 1, 1);
	gl2->setRowStretch (2, 1);

	linePage = new QWidget();
	QHBoxLayout* hlayout = new QHBoxLayout(linePage);
	hlayout->addWidget(gb);
	hlayout->addWidget(fillGroupBox);
	privateTabWidget->addTab( linePage, tr( "Line" ) );

	connect(boxLineColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxConnect, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxLineStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxAreaColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxPattern, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(fillGroupBox, SIGNAL(toggled(bool)), this, SLOT(showAreaColor(bool)));
	connect(fillGroupBox, SIGNAL(clicked()), this, SLOT(acceptParams()));
}

void PlotDialog::initSymbolsPage()
{
	QGroupBox *gb = new QGroupBox();
    QGridLayout *gl = new QGridLayout(gb);
    gl->addWidget(new QLabel(tr( "Style" )), 0, 0);
	boxSymbolStyle = new SymbolBox(false);
    gl->addWidget(boxSymbolStyle, 0, 1);
    gl->addWidget(new QLabel(tr( "Size" )), 1, 0);
	boxSymbolSize = new QSpinBox();
    boxSymbolSize->setRange(1, 100);
	boxSymbolSize->setValue(5);
    gl->addWidget(boxSymbolSize, 1, 1);
	boxFillSymbol = new QCheckBox( tr( "Fill Color" ));
    gl->addWidget(boxFillSymbol, 2, 0);
	boxFillColor = new ColorBox(false);
    gl->addWidget(boxFillColor, 2, 1);
    gl->addWidget(new QLabel(tr( "Edge Color" )), 3, 0);
	boxSymbolColor = new ColorBox( false);
    gl->addWidget(boxSymbolColor, 3, 1);
    gl->addWidget(new QLabel(tr( "Edge Width" )), 4, 0);
	boxPenWidth = new QSpinBox();
    boxPenWidth->setRange(1, 100);
    gl->addWidget(boxPenWidth, 4, 1);
    gl->setRowStretch (5, 1);

    symbolPage = new QWidget();
	QHBoxLayout* hl = new QHBoxLayout(symbolPage);
	hl->addWidget(gb);

	privateTabWidget->insertTab(symbolPage, tr( "Symbol" ));

	connect(boxSymbolColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxSymbolStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxFillColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxFillSymbol, SIGNAL(clicked()), this, SLOT(fillSymbols()));
}

void PlotDialog::initBoxPage()
{
	QGroupBox *gb1 = new QGroupBox(tr( "Box" ));
    QGridLayout *gl1 = new QGridLayout(gb1);
    gl1->addWidget(new QLabel(tr( "Type" )), 0, 0);

	boxType = new QComboBox();
    boxType->setEditable(false);
	boxType->addItem(tr("No Box"));
	boxType->addItem(tr("Rectangle"));
	boxType->addItem(tr("Diamond"));
	boxType->addItem(tr("Perc 10, 25, 75, 90"));
	boxType->addItem(tr("Notch"));
    gl1->addWidget(boxType, 0, 1);

	boxRangeLabel = new QLabel(tr( "Range" ));
    gl1->addWidget(boxRangeLabel, 1, 0);
	boxRange = new QComboBox();
    boxRange->setEditable(false);
	boxRange->addItem(tr("Standard Deviation"));
	boxRange->addItem(tr("Standard Error"));
	boxRange->addItem(tr("Perc 25, 75"));
	boxRange->addItem(tr("Perc 10, 90"));
	boxRange->addItem(tr("Perc 5, 95"));
	boxRange->addItem(tr("Perc 1, 99"));
	boxRange->addItem(tr("Max-Min"));
	boxRange->addItem(tr("Constant"));
    gl1->addWidget(boxRange, 1, 1);

	boxCoeffLabel = new QLabel(tr( "Percentile (%)" ));
    gl1->addWidget(boxCoeffLabel, 2, 0);
	boxCoef = new QSpinBox();
    boxCoef->setRange(50, 100);
    boxCoef->setSingleStep(5);
    gl1->addWidget(boxCoef, 2, 1);

	boxCntLabel = new QLabel(tr( "Coefficient" ));
    gl1->addWidget(boxCntLabel, 3, 0);
	boxCnt = new QDoubleSpinBox();
	boxCnt->setRange(0.0, 100.0);
    boxCnt->setSingleStep(0.01);
	boxCnt->setValue(1.0);
    gl1->addWidget(boxCnt, 3, 1);

    gl1->addWidget(new QLabel(tr( "Box Width" )), 4, 0);
	boxWidth = new QSpinBox();
    boxWidth->setRange(0, 100);
    boxWidth->setSingleStep(5);
    gl1->addWidget(boxWidth, 4, 1);

	QGroupBox *gb2 = new QGroupBox(tr( "Whiskers" ));
    QGridLayout *gl2 = new QGridLayout(gb2);
    whiskerRangeLabel = new QLabel(tr( "Range" ));
    gl2->addWidget(whiskerRangeLabel, 0, 0);

	boxWhiskersRange = new QComboBox();
    boxWhiskersRange->setEditable(false);
	boxWhiskersRange->addItem(tr("No Whiskers"));
	boxWhiskersRange->addItem(tr("Standard Deviation"));
	boxWhiskersRange->addItem(tr("Standard Error"));
	boxWhiskersRange->addItem(tr("75-25"));
	boxWhiskersRange->addItem(tr("90-10"));
	boxWhiskersRange->addItem(tr("95-5"));
	boxWhiskersRange->addItem(tr("99-1"));
	boxWhiskersRange->addItem(tr("Max-Min"));
	boxWhiskersRange->addItem(tr("Constant"));
    gl2->addWidget(boxWhiskersRange, 0, 1);

	whiskerCoeffLabel = new QLabel(tr( "Percentile (%)" ));
    gl2->addWidget(whiskerCoeffLabel, 1, 0);
	boxWhiskersCoef = new QSpinBox();
    boxWhiskersCoef->setRange(50, 100);
    boxWhiskersCoef->setSingleStep(5);
    gl2->addWidget(boxWhiskersCoef, 1, 1);

	whiskerCntLabel = new QLabel(tr( "Coef" ));
    gl2->addWidget(whiskerCntLabel, 2, 0);
	whiskerCnt = new QDoubleSpinBox();
	whiskerCnt->setRange(0.0, 100.0);
    whiskerCnt->setSingleStep(0.01);
	whiskerCnt->setValue(1.0);
    gl2->addWidget(whiskerCnt, 2, 1);

    QVBoxLayout *vl1 = new QVBoxLayout();
    vl1->addWidget(gb1);
    vl1->addStretch();

    QVBoxLayout *vl2 = new QVBoxLayout();
    vl2->addWidget(gb2);
    vl2->addStretch();

    boxPage = new QWidget();
	QHBoxLayout* hl = new QHBoxLayout(boxPage);
	hl->addLayout(vl1);
	hl->addLayout(vl2);
    privateTabWidget->insertTab(boxPage, tr( "Box/Whiskers" ) );

	connect(boxType, SIGNAL(activated(int)), this, SLOT(setBoxType(int)));
	connect(boxRange, SIGNAL(activated(int)), this, SLOT(setBoxRangeType(int)));
	connect(boxWhiskersRange, SIGNAL(activated(int)), this, SLOT(setWhiskersRange(int)));
}

void PlotDialog::initPercentilePage()
{
	QGroupBox *gb1 = new QGroupBox(tr( "Type" ) );
    QGridLayout *gl1 = new QGridLayout(gb1);
    gl1->addWidget(new QLabel(tr( "Max" )), 0, 0);

	boxMaxStyle = new SymbolBox(false);
    gl1->addWidget(boxMaxStyle, 0, 1);

    gl1->addWidget(new QLabel(tr( "99%" )), 1, 0);
	box99Style = new SymbolBox(false);
    gl1->addWidget(box99Style, 1, 1);

    gl1->addWidget(new QLabel(tr( "Mean" )), 2, 0);
	boxMeanStyle = new SymbolBox( false);
    gl1->addWidget(boxMeanStyle, 2, 1);

    gl1->addWidget(new QLabel(tr( "1%" )), 3, 0);
	box1Style = new SymbolBox(false);
    gl1->addWidget(box1Style, 3, 1);

    gl1->addWidget(new QLabel(tr( "Min" )), 4, 0);
	boxMinStyle = new SymbolBox(false);
    gl1->addWidget(boxMinStyle, 4, 1);
    gl1->setRowStretch(5, 1);

	QGroupBox *gb2 = new QGroupBox(tr( "Symbol" ));
    QGridLayout *gl2 = new QGridLayout(gb2);
    gl2->addWidget(new QLabel(tr( "Size" )), 0, 0);

	boxPercSize = new QSpinBox();
	boxPercSize->setMinValue( 1 );
    gl2->addWidget(boxPercSize, 0, 1);

    boxFillSymbols = new QCheckBox(tr( "Fill Color" ));
    gl2->addWidget(boxFillSymbols, 1, 0);
	boxPercFillColor = new ColorBox(false);
    gl2->addWidget(boxPercFillColor, 1, 1);

    gl2->addWidget(new QLabel(tr( "Edge Color" )), 2, 0);
	boxEdgeColor = new ColorBox(false);
    gl2->addWidget(boxEdgeColor, 2, 1);

    gl2->addWidget(new QLabel(tr( "Edge Width" )), 3, 0);
	boxEdgeWidth = new QSpinBox();
    boxEdgeWidth->setRange(0, 100);
    gl2->addWidget(boxEdgeWidth, 3, 1);
    gl2->setRowStretch(4, 1);

    percentilePage = new QWidget();
	QHBoxLayout* hl = new QHBoxLayout(percentilePage);
	hl->addWidget(gb1);
	hl->addWidget(gb2);
    privateTabWidget->insertTab(percentilePage, tr( "Percentile" ) );

	connect(boxMeanStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxMinStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxMaxStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(box99Style, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(box1Style, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(box1Style, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxEdgeColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxPercFillColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxFillSymbols, SIGNAL(clicked()), this, SLOT(fillBoxSymbols()));
}

void PlotDialog::initSpectrogramPage()
{
  	spectrogramPage = new QWidget();

  	imageGroupBox = new QGroupBox(tr( "Image" ));
  	imageGroupBox->setCheckable (true);

	QVBoxLayout *vl = new QVBoxLayout();
  	grayScaleBox = new QRadioButton(tr("&Gray Scale"));
	connect(grayScaleBox, SIGNAL(toggled(bool)), this, SLOT(showColorMapEditor(bool)));
    vl->addWidget(grayScaleBox);
  	defaultScaleBox = new QRadioButton(tr("&Default Color Map"));
	connect(defaultScaleBox, SIGNAL(toggled(bool)), this, SLOT(showColorMapEditor(bool)));
    vl->addWidget(defaultScaleBox);
  	customScaleBox = new QRadioButton(tr("&Custom Color Map"));
	connect(customScaleBox, SIGNAL(toggled(bool)), this, SLOT(showColorMapEditor(bool)));
    vl->addWidget(customScaleBox);

    QHBoxLayout *hl = new QHBoxLayout(imageGroupBox);
	colorMapEditor = new ColorMapEditor();
    hl->addLayout(vl);
	hl->addWidget(colorMapEditor);

  	levelsGroupBox = new QGroupBox(tr( "Contour Lines" ));
  	levelsGroupBox->setCheckable(true);

    QHBoxLayout *hl1 = new QHBoxLayout();
    hl1->addWidget(new QLabel(tr( "Levels" )));

  	levelsBox = new QSpinBox();
  	levelsBox->setRange(2, 1000);
	hl1->addWidget(levelsBox);
    hl1->addStretch();

    QVBoxLayout *vl1 = new QVBoxLayout();
    vl1->addLayout(hl1);

  	autoContourBox = new QRadioButton(tr("Use &Color Map"));
  	connect(autoContourBox, SIGNAL(toggled(bool)), this, SLOT(showDefaultContourLinesBox(bool)));
    vl1->addWidget(autoContourBox);

  	defaultContourBox = new QRadioButton(tr("Use Default &Pen"));
  	connect(defaultContourBox, SIGNAL(toggled(bool)), this, SLOT(showDefaultContourLinesBox(bool)));
    vl1->addWidget(defaultContourBox);

    QHBoxLayout *hl2 = new QHBoxLayout(levelsGroupBox);
    hl2->addLayout(vl1);

  	defaultPenBox = new QGroupBox();
    QGridLayout *gl1 = new QGridLayout(defaultPenBox);
    gl1->addWidget(new QLabel(tr( "Color" )), 0, 0);

  	levelsColorBox = new ColorButton(defaultPenBox);
    gl1->addWidget(levelsColorBox, 0, 1);

    gl1->addWidget(new QLabel(tr( "Width" )), 1, 0);
  	contourWidthBox = new QSpinBox();
    gl1->addWidget(contourWidthBox, 1, 1);

    gl1->addWidget(new QLabel(tr( "Style" )), 2, 0);
  	boxContourStyle = new QComboBox();
	boxContourStyle->setEditable(false);
  	boxContourStyle->addItem("_____");
  	boxContourStyle->addItem("_ _ _");
  	boxContourStyle->addItem(".....");
  	boxContourStyle->addItem("_._._");
  	boxContourStyle->addItem("_.._..");
    gl1->addWidget(boxContourStyle, 2, 1);
    hl2->addWidget(defaultPenBox);

  	connect(levelsColorBox, SIGNAL(clicked()), this, SLOT(pickContourLinesColor()));

  	axisScaleBox = new QGroupBox(tr( "Color Bar Scale" ));
  	axisScaleBox->setCheckable (true);

    QGridLayout *gl2 = new QGridLayout(axisScaleBox);
    gl2->addWidget(new QLabel(tr( "Axis" )), 0, 0);

  	colorScaleBox = new QComboBox();
  	colorScaleBox->addItem(tr("Left"));
  	colorScaleBox->addItem(tr("Right"));
  	colorScaleBox->addItem(tr("Bottom"));
  	colorScaleBox->addItem(tr("Top"));
    gl2->addWidget(colorScaleBox, 0, 1);

    gl2->addWidget(new QLabel(tr( "Width" )), 1, 0);
  	colorScaleWidthBox = new QSpinBox();
  	colorScaleWidthBox->setRange(2, 10000);
    gl2->addWidget(colorScaleWidthBox, 1, 1);

  	QVBoxLayout* vl2 = new QVBoxLayout(spectrogramPage);
  	vl2->addWidget(imageGroupBox);
  	vl2->addWidget(levelsGroupBox);
  	vl2->addWidget(axisScaleBox);
    vl2->addStretch();

  	privateTabWidget->insertTab(spectrogramPage, tr("Contour") + " / " + tr("Image"));
}

void PlotDialog::fillBoxSymbols()
{
	boxPercFillColor->setEnabled(boxFillSymbols->isChecked());
	acceptParams();
}

void PlotDialog::fillSymbols()
{
	boxFillColor->setEnabled(boxFillSymbol->isChecked());
	acceptParams();
}

void PlotDialog::initErrorsPage()
{
	QGroupBox *gb1 = new QGroupBox(tr( "Direction" ));

    QVBoxLayout* vl = new QVBoxLayout(gb1);
	plusBox = new QCheckBox(tr( "Plus" ));
    vl->addWidget(plusBox);
	minusBox = new QCheckBox(tr( "Minus" ));
    vl->addWidget(minusBox);
	xBox = new QCheckBox(tr( "&X Error Bar" ));
    vl->addWidget(xBox);
    vl->addWidget(xBox);
    vl->addStretch();

	QGroupBox *gb2 = new QGroupBox(tr( "Style" ));
    QGridLayout *gl = new QGridLayout(gb2);
    gl->addWidget(new QLabel(tr( "Color" )), 0, 0);

	colorBox = new ColorButton();
    gl->addWidget(colorBox, 0, 1);

    gl->addWidget(new QLabel(tr( "Line Width" )), 1, 0);
	widthBox = new QComboBox();
	widthBox->addItem( tr( "1" ) );
	widthBox->addItem( tr( "2" ) );
	widthBox->addItem( tr( "3" ) );
	widthBox->addItem( tr( "4" ) );
	widthBox->addItem( tr( "5" ) );
	widthBox->setEditable (true);
    gl->addWidget(widthBox, 1, 1);

    gl->addWidget(new QLabel(tr( "Cap Width" )), 2, 0);
	capBox = new QComboBox();
	capBox->addItem( tr( "8" ) );
	capBox->addItem( tr( "10" ) );
	capBox->addItem( tr( "12" ) );
	capBox->addItem( tr( "16" ) );
	capBox->addItem( tr( "20" ) );
	capBox->setEditable (true);
    gl->addWidget(capBox, 2, 1);

	throughBox = new QCheckBox(tr( "Through Symbol" ));
    gl->addWidget(throughBox, 3, 0);
    gl->setRowStretch (4, 1);

    errorsPage = new QWidget();
	QHBoxLayout* hl = new QHBoxLayout(errorsPage);
	hl->addWidget(gb1);
	hl->addWidget(gb2);
    privateTabWidget->insertTab( errorsPage, tr( "Error Bars" ) );

	connect(colorBox, SIGNAL(clicked()), this, SLOT(pickErrorBarsColor()));
	connect(xBox, SIGNAL(clicked()), this, SLOT(changeErrorBarsType()));
	connect(plusBox, SIGNAL(clicked()), this, SLOT(changeErrorBarsPlus()));
	connect(minusBox, SIGNAL(clicked()), this, SLOT(changeErrorBarsMinus()));
	connect(throughBox, SIGNAL(clicked()), this, SLOT(changeErrorBarsThrough()));
}

void PlotDialog::initHistogramPage()
{
    QHBoxLayout* hl = new QHBoxLayout();
	automaticBox = new QCheckBox(tr( "Automatic Binning" ));
    hl->addWidget(automaticBox);
    hl->addStretch();
	buttonStatistics = new QPushButton(tr( "&Show statistics" ));
    hl->addWidget(buttonStatistics);

	GroupBoxH = new QGroupBox();
    QGridLayout *gl = new QGridLayout(GroupBoxH);
    gl->addWidget(new QLabel(tr( "Bin Size" )), 0, 0);
	binSizeBox = new QLineEdit();
    gl->addWidget(binSizeBox, 0, 1);
    gl->addWidget(new QLabel(tr( "Begin" )), 1, 0);
	histogramBeginBox = new QLineEdit();
    gl->addWidget(histogramBeginBox, 1, 1);
    gl->addWidget(new QLabel(tr( "End" )), 2, 0);
	histogramEndBox = new QLineEdit();
    gl->addWidget(histogramEndBox, 2, 1);

    histogramPage = new QWidget();
	QVBoxLayout* vl = new QVBoxLayout(histogramPage);
	vl->addLayout(hl);
	vl->addWidget(GroupBoxH);
    vl->addStretch();

    privateTabWidget->insertTab( histogramPage, tr( "Histogram Data" ) );

	connect(automaticBox, SIGNAL(clicked()), this, SLOT(setAutomaticBinning()));
	connect(buttonStatistics, SIGNAL(clicked()), this, SLOT(showStatistics() ) );
}

void PlotDialog::initSpacingPage()
{
	spacingPage = new QWidget();

    QGridLayout *gl = new QGridLayout(spacingPage);
    gl->addWidget(new QLabel(tr( "Gap Between Bars (in %)" )), 0, 0);
	gapBox = new QSpinBox();
    gapBox->setRange(0, 100);
    gapBox->setSingleStep(10);
    gl->addWidget(gapBox, 0, 1);
    gl->addWidget(new QLabel(tr( "Offset (in %)" )), 1, 0);
	offsetBox = new QSpinBox();
    offsetBox->setRange(-1000, 1000);
    offsetBox->setSingleStep(50);
    gl->addWidget(offsetBox, 1, 1);
    gl->setRowStretch (2, 1);

	privateTabWidget->insertTab( spacingPage, tr( "Spacing" ));
}

void PlotDialog::initVectPage()
{
    QGroupBox *gb1 = new QGroupBox();
    QGridLayout *gl1 = new QGridLayout(gb1);
    gl1->addWidget(new QLabel(tr( "Color" )), 0, 0);
	vectColorBox = new ColorBox(false);
    gl1->addWidget(vectColorBox, 0, 1);
    gl1->addWidget(new QLabel(tr( "Line Width" )), 1, 0);
	vectWidthBox = new QSpinBox();
    vectWidthBox->setRange(0, 100);
    gl1->addWidget(vectWidthBox, 1, 1);

	QGroupBox *gb2 = new QGroupBox(tr( "Arrowheads" ));
    QGridLayout *gl2 = new QGridLayout(gb2);
    gl2->addWidget(new QLabel(tr( "Length" )), 0, 0);
	headLengthBox = new QSpinBox();
    headLengthBox->setRange(0, 100);
    gl2->addWidget(headLengthBox, 0, 1);
    gl2->addWidget(new QLabel(tr( "Angle" )), 1, 0);
	headAngleBox = new QSpinBox();
    headAngleBox->setRange(0, 85);
    headAngleBox->setSingleStep(5);
    gl2->addWidget(headAngleBox, 1, 1);
	filledHeadBox = new QCheckBox(tr( "&Filled" ));
    gl2->addWidget(filledHeadBox, 2, 0);
    gl2->setRowStretch(3, 1);

	GroupBoxVectEnd = new QGroupBox(tr( "End Point" ));
    QGridLayout *gl3 = new QGridLayout(GroupBoxVectEnd);
    labelXEnd = new QLabel(tr( "X End" ));
    gl3->addWidget(labelXEnd, 0, 0);
	xEndBox = new QComboBox(false);
    gl3->addWidget(xEndBox, 0, 1);

	labelYEnd = new QLabel(tr( "Y End" ));
    gl3->addWidget(labelYEnd, 1, 0);
	yEndBox = new  QComboBox( false);
    gl3->addWidget(yEndBox, 1, 1);

	labelPosition = new QLabel(tr( "Position" ));
    gl3->addWidget(labelPosition, 2, 0);
	vectPosBox = new  QComboBox( false);
	vectPosBox->addItem(tr("Tail"));
	vectPosBox->addItem(tr("Middle"));
	vectPosBox->addItem(tr("Head"));
    gl3->addWidget(vectPosBox, 2, 1);
    gl3->setRowStretch(3, 1);

    vectPage = new QWidget();

    QVBoxLayout *vl1 = new QVBoxLayout();
    vl1->addWidget(gb1);
    vl1->addWidget(gb2);

	QHBoxLayout *hl = new QHBoxLayout(vectPage);
    hl->addLayout(vl1);
    hl->addWidget(GroupBoxVectEnd);

	privateTabWidget->insertTab( vectPage, tr( "Vector" ) );
}

void PlotDialog::setMultiLayer(MultiLayer *ml)
{
    if (!ml)
        return;

    d_ml = ml;
	boxScaleLayers->setChecked(d_ml->scaleLayersOnPrint());
	boxPrintCrops->setChecked(d_ml->printCropmarksEnabled());

    QTreeWidgetItem *item = new QTreeWidgetItem(listBox, QStringList(ml->name()));
    item->setIcon(0, QIcon(folder_open));
    listBox->addTopLevelItem(item);
    listBox->setCurrentItem(item);

    QWidgetList plots = ml->graphPtrs();
    for (int i = 0; i < plots.count(); ++i)
    {
        Graph* g = (Graph*)plots.at(i);
        if (!g)
            continue;

        LayerItem *layer = new LayerItem(g, item, tr("Layer") + QString::number(i+1));
        item->addChild(layer);

        if (g == ml->activeGraph())
		{
            layer->setExpanded(true);
        	layer->setActive(true);
        	listBox->setCurrentItem(layer);
		}
    }
}

void PlotDialog::selectCurve(int index)
{
	LayerItem *layerItem = (LayerItem *)listBox->currentItem();
	if (!layerItem)
        return;
    if (layerItem->type() != LayerItem::LayerTreeItem)
        return;
	QTreeWidgetItem *item = layerItem->child(index);
	if (item)
	{
	    ((CurveTreeItem *)item)->setActive(true);
        listBox->setCurrentItem(item);
	}
}

void PlotDialog::showStatistics()
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (!app)
        return;

    QTreeWidgetItem *it = listBox->currentItem();
    if (!it)
        return;
    if (it->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

    QwtPlotItem *plotItem = (QwtPlotItem *)((CurveTreeItem *)it)->plotItem();
    if (!plotItem)
        return;

	QwtHistogram *h = (QwtHistogram *)plotItem;
	if (!h)
		return;

    QString tableName = app->generateUniqueName(tr("Bins"));
    Table *t = app->newTable(h->dataSize(), 4, tableName, tr("Histogram and Probabilities for") + " " + h->title().text());
    if (t)
    {
        double h_sum = 0.0;
        for (int i = 0; i<h->dataSize(); i++ )
            h_sum += h->y(i);

        double sum = 0.0;
        for (int i = 0; i<h->dataSize(); i++ )
        {
            sum += h->y(i);
            t->setText(i, 0, QString::number(h->x(i)));
            t->setText(i, 1, QString::number(h->y(i)));
            t->setText(i, 2, QString::number(sum));
            t->setText(i, 3, QString::number(sum/h_sum*100));
        }
        t->setHeader(QStringList() << tr("Bins") << tr("Quantity") << tr("Sum") << tr("Percent"));
        t->showMaximized();
	}

    QDateTime dt = QDateTime::currentDateTime();
	QString info = dt.toString(Qt::LocalDate)+"\t"+tr("Histogram and Probabilities for") + " " + h->title().text()+"\n";
	info += tr("Mean")+" = "+QString::number(h->mean())+"\t";
	info += tr("Standard Deviation")+" = "+QString::number(h->standardDeviation())+"\n";
	info += tr("Minimum")+" = "+QString::number(h->minimum())+"\t";
	info += tr("Maximum")+" = "+QString::number(h->maximum())+"\t";
	info += tr("Bins")+" = "+QString::number(h->dataSize())+"\n";
	info += "-------------------------------------------------------------\n";
    if (!info.isEmpty())
    {
        app->logInfo += info;
        app->showResults(true);
    }

	close();
}

void PlotDialog::contextMenuEvent(QContextMenuEvent *e)
{
    QTreeWidgetItem *item = listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;
    QwtPlotItem *it = (QwtPlotItem *)((CurveTreeItem *)item)->plotItem();
	if (!it)
		return;

	QPoint pos = listBox->viewport()->mapFromGlobal(QCursor::pos());
	QRect rect = listBox->visualItemRect(listBox->currentItem());
	if (rect.contains(pos))
	{
	   QMenu contextMenu(this);
	   contextMenu.insertItem(tr("&Delete"), this, SLOT(removeSelectedCurve()));

	   if (it->rtti() == QwtPlotItem::Rtti_PlotCurve)
	   {
            if (((PlotCurve *)it)->type() == Graph::Function)
                contextMenu.insertItem(tr("&Edit..."), this, SLOT(editCurve()));
            else
                contextMenu.insertItem(tr("&Plot Associations..."), this, SLOT(editCurve()));
	   }
	   contextMenu.exec(QCursor::pos());
    }
    e->accept();
}

void PlotDialog::removeSelectedCurve()
{
    CurveTreeItem *item = (CurveTreeItem *)listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

    Graph *graph = item->graph();
    if (graph)
    {
        graph->removeCurve(item->plotItemIndex());
        graph->updatePlot();

        int index = item->parent()->indexOfChild (item);
        QTreeWidgetItem *it = item->parent()->takeChild(index);
        if (it)
            delete it;
    }
}

void PlotDialog::changeErrorBarsPlus()
{
    CurveTreeItem *item = (CurveTreeItem *)listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

    Graph *graph = item->graph();
    if (!graph)
        return;

	graph->updateErrorBars((QwtErrorPlotCurve *)item->plotItem(), xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(),colorBox->color(), plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::changeErrorBarsMinus()
{
	CurveTreeItem *item = (CurveTreeItem *)listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

    Graph *graph = item->graph();
    if (!graph)
        return;

	graph->updateErrorBars((QwtErrorPlotCurve *)item->plotItem(), xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(), colorBox->color(),plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::changeErrorBarsThrough()
{
    CurveTreeItem *item = (CurveTreeItem *)listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

    Graph *graph = item->graph();
    if (!graph)
        return;

	graph->updateErrorBars((QwtErrorPlotCurve *)item->plotItem(), xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(), colorBox->color(),plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::changeErrorBarsType()
{
    CurveTreeItem *item = (CurveTreeItem *)listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

    Graph *graph = item->graph();
    if (!graph)
        return;

	graph->updateErrorBars((QwtErrorPlotCurve *)item->plotItem(), xBox->isChecked(), widthBox->currentText().toInt(),
			capBox->currentText().toInt(), colorBox->color(), plusBox->isChecked(), minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::pickErrorBarsColor()
{
	QColor color = QColorDialog::getColor(colorBox->color(), this);
	if ( !color.isValid() || color == colorBox->color() )
		return;

	colorBox->setColor (color);

    CurveTreeItem *item = (CurveTreeItem *)listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

    Graph *graph = item->graph();
    if (!graph)
        return;

	graph->updateErrorBars((QwtErrorPlotCurve *)item->plotItem(), xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(), color, plusBox->isChecked(), minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::showAreaColor(bool show)
{
	boxAreaColor->setEnabled(show);
	boxPattern->setEnabled(show);
}

void PlotDialog::updateTabWindow(QTreeWidgetItem *currentItem, QTreeWidgetItem *previousItem)
{
    if (!previousItem || !currentItem)
        return;

    if (previousItem->type() == CurveTreeItem::PlotCurveTreeItem)
        ((CurveTreeItem *)previousItem)->setActive(false);
    else if (previousItem->type() == LayerItem::LayerTreeItem)
        ((LayerItem *)previousItem)->setActive(false);

    boxPlotType->blockSignals(true);

    if (currentItem->type() == CurveTreeItem::PlotCurveTreeItem)
    {
        CurveTreeItem *curveItem = (CurveTreeItem *)currentItem;
        setActiveCurve(curveItem);

        if (previousItem->type() != CurveTreeItem::PlotCurveTreeItem ||
           ((CurveTreeItem *)previousItem)->plotItemType() != curveItem->plotItemType())
        {
            clearTabWidget();
            int plot_type = setPlotType(curveItem);
			if (plot_type >= 0)
				insertTabs(plot_type);
            if (!curvePlotTypeBox->isVisible())
                curvePlotTypeBox->show();
        }
    }
    else if (currentItem->type() == LayerItem::LayerTreeItem)
    {
        if (previousItem->type() != LayerItem::LayerTreeItem)
        {
            clearTabWidget();
            privateTabWidget->addTab (layerPage, tr("Layer"));
            privateTabWidget->showPage(layerPage);
        }
        setActiveLayer((LayerItem *)currentItem);
    }
    else
    {
        clearTabWidget();
		privateTabWidget->addTab(printPage, tr("Print"));
        privateTabWidget->addTab(fontsPage, tr("Fonts"));
        privateTabWidget->showPage(printPage);

        curvePlotTypeBox->hide();
        btnWorksheet->hide();
        btnEditCurve->hide();
    }
    boxPlotType->blockSignals(false);
}

void PlotDialog::insertTabs(int plot_type)
{
    if (plot_type == Graph::Pie)
	{
		privateTabWidget->addTab (piePage, tr("Pie"));
		privateTabWidget->showPage(piePage);
		btnEditCurve->hide();
		return;
	}

    privateTabWidget->addTab (axesPage, tr("Axes"));
	if (plot_type == Graph::Line)
	{
		boxConnect->setEnabled(true);
		privateTabWidget->addTab (linePage, tr("Line"));
		privateTabWidget->showPage(linePage);
	}
	else if (plot_type == Graph::Scatter)
	{
		boxConnect->setEnabled(true);
		privateTabWidget->addTab (symbolPage, tr("Symbol"));
		privateTabWidget->showPage(symbolPage);
	}
	else if (plot_type == Graph::LineSymbols)
	{
		boxConnect->setEnabled(true);
		privateTabWidget->addTab (linePage, tr("Line"));
		privateTabWidget->addTab (symbolPage, tr("Symbol"));
		privateTabWidget->showPage(symbolPage);
	}
	else if (plot_type == Graph::VerticalBars ||
			plot_type == Graph::HorizontalBars ||
			plot_type == Graph::Histogram)
	{
		boxConnect->setEnabled(false);
		privateTabWidget->addTab (linePage, tr("Pattern"));
		privateTabWidget->addTab (spacingPage, tr("Spacing"));

		if (plot_type == Graph::Histogram)
		{
			privateTabWidget->addTab (histogramPage, tr("Histogram Data"));
			privateTabWidget->showPage(histogramPage);
		}
		else
			privateTabWidget->showPage(linePage);
	}
	else if (plot_type == Graph::VectXYXY || plot_type == Graph::VectXYAM)
	{
		boxConnect->setEnabled(true);
		privateTabWidget->addTab (linePage, tr("Line"));
		privateTabWidget->addTab (vectPage, tr("Vector"));
		customVectorsPage(plot_type == Graph::VectXYAM);
		privateTabWidget->showPage(vectPage);
	}
	else if (plot_type == Graph::ErrorBars)
	{
		privateTabWidget->addTab (errorsPage, tr("Error Bars"));
		privateTabWidget->showPage(errorsPage);
	}
	else if (plot_type == Graph::Box)
	{
		boxConnect->setEnabled(false);
		privateTabWidget->addTab (linePage, tr("Pattern"));
		privateTabWidget->addTab (boxPage, tr("Box/Whiskers"));
		privateTabWidget->addTab (percentilePage, tr("Percentile"));
		privateTabWidget->showPage(linePage);
	}
	else if (plot_type == Graph::ColorMap || plot_type == Graph::GrayMap || plot_type == Graph::ContourMap)
  	{
  		privateTabWidget->addTab(spectrogramPage, tr("Colors") + " / " + tr("Contour"));
  	    privateTabWidget->showPage(spectrogramPage);
  	}
}

void PlotDialog::clearTabWidget()
{
    privateTabWidget->removeTab(privateTabWidget->indexOf(axesPage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(linePage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(symbolPage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(errorsPage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(histogramPage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(spacingPage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(vectPage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(boxPage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(percentilePage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(spectrogramPage));
    privateTabWidget->removeTab(privateTabWidget->indexOf(piePage));

	privateTabWidget->removeTab(privateTabWidget->indexOf(layerPage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(fontsPage));
	privateTabWidget->removeTab(privateTabWidget->indexOf(printPage));
}

void PlotDialog::quit()
{
	if (acceptParams())
		close();
}

void PlotDialog::showWorksheet()
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (!app)
		return;

    CurveTreeItem *item = (CurveTreeItem *)listBox->currentItem();
    if (!item)
        return;
    if (item->type() != CurveTreeItem::PlotCurveTreeItem)
        return;

	app->showCurveWorksheet(item->graph(), item->plotItemIndex());
	close();
}

int PlotDialog::setPlotType(CurveTreeItem *item)
{
	int curveType = item->plotItemType();
	if (curveType >= 0)
	{
		boxPlotType->clear();

		if (curveType == Graph::ErrorBars)
			boxPlotType->addItem( tr( "Error Bars" ) );
        else if (curveType == Graph::Pie)
			boxPlotType->addItem( tr( "Pie" ) );
		else if (curveType == Graph::VerticalBars)
			boxPlotType->addItem( tr( "Vertical Bars" ) );
		else if (curveType == Graph::HorizontalBars)
			boxPlotType->addItem( tr( "Horizontal Bars" ) );
		else if (curveType == Graph::Histogram)
			boxPlotType->addItem( tr( "Histogram" ) );
		else if (curveType == Graph::VectXYXY || curveType == Graph::VectXYAM)
		{
			boxPlotType->addItem( tr( "Vector XYXY" ) );
			boxPlotType->addItem( tr( "Vector XYAM" ) );
			if (curveType == Graph::VectXYAM)
				boxPlotType->setCurrentIndex(1);
		}
		else if (curveType == Graph::Box)
			boxPlotType->addItem( tr( "Box" ) );
		else if (curveType == Graph::ColorMap || curveType == Graph::GrayMap || curveType == Graph::ContourMap)
  	    	boxPlotType->insertItem(tr("Contour") + " / " + tr("Image"));
		else
		{
			boxPlotType->addItem( tr( "Line" ) );
			boxPlotType->addItem( tr( "Scatter" ) );
			boxPlotType->addItem( tr( "Line + Symbol" ) );

			QwtPlotCurve *c = (QwtPlotCurve*)item->plotItem();
			if (!c)
				return -1;

			QwtSymbol s = c->symbol();
			if (s.style() == QwtSymbol::NoSymbol)
			{
				boxPlotType->setCurrentIndex(0);
				return Graph::Line;
			}
			else if (c->style() == QwtPlotCurve::NoCurve)
			{
				boxPlotType->setCurrentIndex(1);
				return Graph::Scatter;
			}
			else
			{
				boxPlotType->setCurrentIndex(2);
				return Graph::LineSymbols;
			}
		}
	}
	return curveType;
}

void PlotDialog::setActiveLayer(LayerItem *item)
{
    if (!item)
        return;
    item->setActive(true);

    Graph *g = item->graph();
    if (!g)
        return;

	curvePlotTypeBox->hide();
    btnWorksheet->hide();
    btnEditCurve->hide();

    boxBackgroundTransparency->blockSignals(true);
    boxCanvasTransparency->blockSignals(true);
    boxBorderWidth->blockSignals(true);

    Plot *p = g->plotWidget();
    boxMargin->setValue(p->margin());
	boxBorderWidth->setValue(p->lineWidth());
	boxBorderColor->setColor(p->frameColor());

	QColor c = p->paletteBackgroundColor();
	boxBackgroundTransparency->setValue(c.alpha());
	boxBackgroundColor->setEnabled(c.alpha());
	c.setAlpha(255);
	boxBackgroundColor->setColor(c);

	c = p->canvasBackground();
	boxCanvasTransparency->setValue(c.alpha());
	boxCanvasColor->setEnabled(c.alpha());
	c.setAlpha(255);
	boxCanvasColor->setColor(c);

	boxAntialiasing->setChecked(g->antialiasing());

    boxBackgroundTransparency->blockSignals(false);
    boxCanvasTransparency->blockSignals(false);
    boxBorderWidth->blockSignals(false);
}

void PlotDialog::setActiveCurve(CurveTreeItem *item)
{
    if (!item)
        return;

    const QwtPlotItem *i = item->plotItem();
    if (!i)
        return;

	item->setActive(true);
	btnWorksheet->show();
    btnEditCurve->show();

    int curveType = item->plotItemType();
    if (curveType == Graph::Pie)
    {
        QwtPieCurve *pie = (QwtPieCurve*)i;
        boxRadius->setValue(pie->ray());
        boxPiePattern->setPattern(pie->pattern());
        boxPieLineWidth->setValue(pie->pen().width());
        boxPieLineColor->setColor(pie->pen().color());
        setPiePenStyle(pie->pen().style());
        boxFirstColor->setCurrentItem(pie->firstColor());
        return;
    }

    //axes page
    boxXAxis->setCurrentItem(i->xAxis()-2);
    boxYAxis->setCurrentItem(i->yAxis());

    if (i->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
    {
        btnEditCurve->hide();
        Spectrogram *sp = (Spectrogram *)i;

        imageGroupBox->setChecked(sp->testDisplayMode(QwtPlotSpectrogram::ImageMode));
        grayScaleBox->setChecked(sp->colorMapPolicy() == Spectrogram::GrayScale);
        defaultScaleBox->setChecked(sp->colorMapPolicy() == Spectrogram::Default);
        customScaleBox->setChecked(sp->colorMapPolicy() == Spectrogram::Custom);

        colorMapEditor->setRange(sp->data().range().minValue(), sp->data().range().maxValue());
        colorMapEditor->setColorMap((const QwtLinearColorMap &)sp->colorMap());

        levelsGroupBox->setChecked(sp->testDisplayMode(QwtPlotSpectrogram::ContourMode));
        levelsBox->setValue(sp->levels());

        autoContourBox->setChecked(sp->defaultContourPen().style() == Qt::NoPen);
        defaultContourBox->setChecked(sp->defaultContourPen().style() != Qt::NoPen);

        levelsColorBox->setColor(sp->defaultContourPen().color());
        contourWidthBox->setValue(sp->defaultContourPen().width());
        if (sp->defaultContourPen().style() != Qt::NoPen)
            boxContourStyle->setCurrentIndex(sp->defaultContourPen().style() - 1);
        else
            boxContourStyle->setCurrentIndex(0);

        axisScaleBox->setChecked(sp->hasColorScale());
        colorScaleBox->setCurrentItem((int)sp->colorScaleAxis());
        colorScaleWidthBox->setValue(sp->colorBarWidth());
        return;
    }

    PlotCurve *c = (PlotCurve*)i;
    if (c->type() == Graph::Function)
        btnEditCurve->setText(tr("&Edit..."));
    else
        btnEditCurve->setText(tr("&Plot Associations..."));

    //line page
    int style = c->style();
    if (curveType == Graph::Spline)
        style = 5;
    else if (curveType == Graph::VerticalSteps)
        style = 6;
    boxConnect->setCurrentIndex(style);

    setPenStyle(c->pen().style());
    boxLineColor->setColor(c->pen().color());
    boxLineWidth->setValue(c->pen().width());
    fillGroupBox->blockSignals(true);
    fillGroupBox->setChecked(c->brush().style() != Qt::NoBrush );
    fillGroupBox->blockSignals(false);
    boxAreaColor->setColor(c->brush().color());
    boxPattern->setPattern(c->brush().style());

    //symbol page
    const QwtSymbol s = c->symbol();
    boxSymbolSize->setValue(s.size().width()/2);
    boxSymbolStyle->setStyle(s.style());
    boxSymbolColor->setColor(s.pen().color());
    boxPenWidth->setValue(s.pen().width());
    boxFillSymbol->setChecked(s.brush() != Qt::NoBrush);
    boxFillColor->setEnabled(s.brush() != Qt::NoBrush);
    boxFillColor->setColor(s.brush().color());

    if (curveType == Graph::VerticalBars || curveType == Graph::HorizontalBars ||
				curveType == Graph::Histogram)
    {//spacing page
        QwtBarCurve *b = (QwtBarCurve*)i;
        if (b)
        {
            gapBox->setValue(b->gap());
            offsetBox->setValue(b->offset());
        }
    }

    if (curveType == Graph::Histogram)
    {//Histogram page
        QwtHistogram *h = (QwtHistogram*)i;
        if (h)
        {
            automaticBox->setChecked(h->autoBinning());
            binSizeBox->setText(QString::number(h->binSize()));
            histogramBeginBox->setText(QString::number(h->begin()));
            histogramEndBox->setText(QString::number(h->end()));
            setAutomaticBinning();
        }
    }

    if (curveType == Graph::VectXYXY || curveType == Graph::VectXYAM)
    {//Vector page
        VectorCurve *v = (VectorCurve*)i;
        if (v)
        {
            vectColorBox->setColor(v->color());
            vectWidthBox->setValue(v->width());
            headLengthBox->setValue(v->headLength());
            headAngleBox->setValue(v->headAngle());
            filledHeadBox->setChecked(v->filledArrowHead());
            vectPosBox->setCurrentIndex(v->position());
            updateEndPointColumns(item->text(0));
        }
    }

    if (curveType == Graph::ErrorBars)
    {
        QwtErrorPlotCurve *err = (QwtErrorPlotCurve*)i;
        if (err)
        {
            xBox->setChecked(err->xErrors());
            widthBox->setEditText(QString::number(err->width()));
            capBox->setEditText(QString::number(err->capLength()));
            colorBox->setColor(err->color());
            throughBox->setChecked(err->throughSymbol());
            plusBox->setChecked(err->plusSide());
            minusBox->setChecked(err->minusSide());
        }
    }

    if (curveType == Graph::Box)
    {
        BoxCurve *b = (BoxCurve*)i;
        if (b)
        {
            boxMaxStyle->setStyle(b->maxStyle());
            boxMinStyle->setStyle(b->minStyle());
            boxMeanStyle->setStyle(b->meanStyle());
            box99Style->setStyle(b->p99Style());
            box1Style->setStyle(b->p1Style());

            boxPercSize->setValue(s.size().width()/2);
            boxFillSymbols->setChecked(s.brush() != Qt::NoBrush);
            boxPercFillColor->setEnabled(s.brush() != Qt::NoBrush);
            boxPercFillColor->setColor(s.brush().color());
            boxEdgeColor->setColor(s.pen().color());
            boxEdgeWidth->setValue(s.pen().width());

            boxRange->setCurrentIndex (b->boxRangeType()-1);
            boxType->setCurrentIndex (b->boxStyle());
            boxWidth->setValue(b->boxWidth());
            setBoxRangeType(boxRange->currentIndex());
            setBoxType(boxType->currentIndex());
            if (b->boxRangeType() == BoxCurve::SD || b->boxRangeType() == BoxCurve::SE)
                boxCnt->setValue(b->boxRange());
            else
                boxCoef->setValue((int)b->boxRange());

            boxWhiskersRange->setCurrentIndex (b->whiskersRangeType());
            setWhiskersRange(boxWhiskersRange->currentIndex());
            if (b->whiskersRangeType() == BoxCurve::SD || b->whiskersRangeType() == BoxCurve::SE)
                whiskerCnt->setValue(b->whiskersRange());
            else
                boxWhiskersCoef->setValue((int)b->whiskersRange());
        }
    }
}

void PlotDialog::updateEndPointColumns(const QString& text)
{
	QStringList cols=text.split(",", QString::SkipEmptyParts);
	QStringList aux=cols[0].split(":", QString::SkipEmptyParts);
	QString table=aux[0];
	QStringList list;
	for (int i=0; i<(int)columnNames.count(); i++)
	{
		QString s=columnNames[i];
		if (s.contains(table))
			list<<s;
	}

	xEndBox->clear();
	xEndBox->insertStringList(list);
	xEndBox->setCurrentText(table + "_" + cols[2].remove("(X)").remove("(A)"));

	yEndBox->clear();
	yEndBox->insertStringList(list);
	yEndBox->setCurrentText(table + "_" + cols[3].remove("(Y)").remove("(M)"));
}

bool PlotDialog::acceptParams()
{
    if (privateTabWidget->currentWidget() == fontsPage)
    {
		d_ml->setFonts(titleFont, axesFont, numbersFont, legendFont);
		return true;
    }
	else if (privateTabWidget->currentWidget() == printPage)
    {
		d_ml->setScaleLayersOnPrint(boxScaleLayers->isChecked());
		d_ml->printCropmarks(boxPrintCrops->isChecked());
		return true;
    }
    else if (privateTabWidget->currentWidget() == layerPage)
	{
		if (!boxAll->isChecked())
			return true;

		QWidgetList allPlots = d_ml->graphPtrs();
		for (int i=0; i<allPlots.count();i++)
		{
			Graph* g = (Graph*)allPlots.at(i);
			if (g)
			{
				g->setBorder(boxBorderWidth->value(), boxBorderColor->color());
				g->changeMargin(boxMargin->value());

				QColor c = boxBackgroundColor->color();
				c.setAlpha(boxBackgroundTransparency->value());
				g->setBackgroundColor(c);

				c = boxCanvasColor->color();
				c.setAlpha(boxCanvasTransparency->value());
				g->setCanvasBackground(c);

				g->setAntialiasing(boxAntialiasing->isChecked());
			}
		}
		return true;
	}

    QTreeWidgetItem *it = listBox->currentItem();
    if (!it)
        return false;

    CurveTreeItem *item = (CurveTreeItem *)it;
    QwtPlotItem *plotItem = (QwtPlotItem *)item->plotItem();
    if (!plotItem)
        return false;

    Graph *graph = item->graph();
    if (!graph)
        return false;

	if (privateTabWidget->currentPage() == axesPage)
	{
		plotItem->setAxis(boxXAxis->currentIndex() + 2, boxYAxis->currentIndex());
		graph->setAutoScale();
		return true;
	}
	else if (privateTabWidget->currentPage() == spectrogramPage)
  	{
  		Spectrogram *sp = (Spectrogram *)plotItem;
  	    if (!sp || sp->rtti() != QwtPlotItem::Rtti_PlotSpectrogram)
  	    	return false;

  	    sp->setLevelsNumber(levelsBox->value());
  	    if (autoContourBox->isChecked())
  	    	sp->setDefaultContourPen(Qt::NoPen);
  	    else
  	    	sp->setDefaultContourPen(QPen(levelsColorBox->color(), contourWidthBox->value(),
  	                            Graph::getPenStyle(boxContourStyle->currentItem())));

  	   sp->setDisplayMode(QwtPlotSpectrogram::ContourMode, levelsGroupBox->isChecked());
  	   sp->setDisplayMode(QwtPlotSpectrogram::ImageMode, imageGroupBox->isChecked());

  	   if (grayScaleBox->isChecked())
	   {
		   sp->setGrayScale();
	   	   colorMapEditor->setColorMap(QwtLinearColorMap(Qt::black, Qt::white));
  	   }
  	   else if (defaultScaleBox->isChecked())
  	   {
	   	   sp->setDefaultColorMap();
		   colorMapEditor->setColorMap(Spectrogram::defaultColorMap());
	   }
  	   else
	   	   sp->setCustomColorMap(colorMapEditor->colorMap());

  	   sp->showColorScale((QwtPlot::Axis)colorScaleBox->currentItem(), axisScaleBox->isChecked());
  	   sp->setColorBarWidth(colorScaleWidthBox->value());

  	   //Update axes page
  	   boxXAxis->setCurrentItem(sp->xAxis()-2);
  	   boxYAxis->setCurrentItem(sp->yAxis());
  	}
	else if (privateTabWidget->currentPage()==linePage)
	{
		int index = item->plotItemIndex();
		graph->setCurveStyle(index, boxConnect->currentIndex());
		QBrush br = QBrush(boxAreaColor->color(), boxPattern->getSelectedPattern());
		if (!fillGroupBox->isChecked())
			br = QBrush();
		graph->setCurveBrush(index, br);
		QPen pen = QPen(boxLineColor->color(),boxLineWidth->value(),Graph::getPenStyle(boxLineStyle->currentIndex()));
		QwtPlotCurve *curve = (QwtPlotCurve *)plotItem;
		curve->setPen(pen);
	}
	else if (privateTabWidget->currentPage()==symbolPage)
	{
		int size = 2*boxSymbolSize->value()+1;
		QBrush br = QBrush(boxFillColor->color(), Qt::SolidPattern);
		if (!boxFillSymbol->isChecked())
			br = QBrush();
		QPen pen = QPen(boxSymbolColor->color(),boxPenWidth->value(),Qt::SolidLine);
		QwtSymbol s = QwtSymbol(boxSymbolStyle->selectedSymbol(), br, pen, QSize(size, size));
		QwtPlotCurve *curve = (QwtPlotCurve *)plotItem;
		curve->setSymbol(s);
	}
	else if (privateTabWidget->currentPage()==histogramPage)
	{
        QwtHistogram *h = (QwtHistogram *)plotItem;
		if (!h)
			return false;

		QString text = item->text(0);
		QStringList t = text.split(": ", QString::SkipEmptyParts);
		QStringList list = t[1].split(",", QString::SkipEmptyParts);
		text = t[0] + "_" + list[1].remove("(Y)");
		bool accept = validInput();
		if (accept)
		{
            if (h->autoBinning() == automaticBox->isChecked() &&
                h->binSize() == binSizeBox->text().toDouble() &&
                h->begin() == histogramBeginBox->text().toDouble() &&
                h->end() == histogramEndBox->text().toDouble()) return false;

            h->setBinning(automaticBox->isChecked(), binSizeBox->text().toDouble(),
                         histogramBeginBox->text().toDouble(), histogramEndBox->text().toDouble());
            h->loadData();
			graph->updateScale();
			graph->notifyChanges();
		}
		return accept;
	}
	else if (privateTabWidget->currentPage()==spacingPage)
		graph->setBarsGap(item->plotItemIndex(), gapBox->value(), offsetBox->value());
	else if (privateTabWidget->currentPage() == vectPage)
	{
		int index = item->plotItemIndex();
		ApplicationWindow *app = (ApplicationWindow *)this->parent();
		if (!app)
			return false;

		QString xEndCol = xEndBox->currentText();
		QString yEndCol = yEndBox->currentText();
		Table* w = app->table(xEndCol);
		if (!w)
			return false;

		graph->updateVectorsLayout(index, vectColorBox->color(), vectWidthBox->value(),
				headLengthBox->value(), headAngleBox->value(),
				filledHeadBox->isChecked(), vectPosBox->currentIndex(), xEndCol, yEndCol);

		QString text = item->text(0);
		QStringList t = text.split(": ", QString::SkipEmptyParts);
		QString table = t[0];

		QStringList cols = t[1].split(",", QString::SkipEmptyParts);
		if (graph->curveType(index) == Graph::VectXYXY)
		{
			xEndCol = xEndCol.remove(table + "_") + "(X)";
			yEndCol = yEndCol.remove(table + "_") + "(Y)";
		}
		else
		{
			xEndCol = xEndCol.remove(table + "_") + "(A)";
			yEndCol = yEndCol.remove(table + "_") + "(M)";
		}

		if (cols[2] != xEndCol || cols[3] != yEndCol)
		{
			cols[2] = xEndCol;
			cols[3] = yEndCol;
			item->setText(0, table + ": " + cols.join(","));
		}
		return true;
	}
	else if (privateTabWidget->currentPage() == errorsPage)
	{
		graph->updateErrorBars((QwtErrorPlotCurve *)item->plotItem(), xBox->isChecked(), widthBox->currentText().toInt(),
				capBox->currentText().toInt(), colorBox->color(), plusBox->isChecked(), minusBox->isChecked(),
				throughBox->isChecked());
        return true;
	}
    else if (privateTabWidget->currentPage() == piePage)
	{
		QwtPieCurve *pie = (QwtPieCurve*)plotItem;
		pie->setPen(QPen(boxPieLineColor->color(), boxPieLineWidth->value(),
                    Graph::getPenStyle(boxPieLineStyle->currentIndex())));
        pie->setRay(boxRadius->value());
        pie->setBrushStyle(boxPiePattern->getSelectedPattern());
        pie->setFirstColor(boxFirstColor->currentIndex());
	}
	else if (privateTabWidget->currentPage() == percentilePage)
	{
		BoxCurve *b = (BoxCurve*)plotItem;
		if (b)
		{
			b->setMaxStyle(boxMaxStyle->selectedSymbol());
			b->setP99Style(box99Style->selectedSymbol());
			b->setMeanStyle(boxMeanStyle->selectedSymbol());
			b->setP1Style(box1Style->selectedSymbol());
			b->setMinStyle(boxMinStyle->selectedSymbol());

            int size = 2*boxPercSize->value() + 1;
            QBrush br = QBrush(boxPercFillColor->color(), Qt::SolidPattern);
            if (!boxFillSymbols->isChecked())
                br = QBrush();
            QwtSymbol s = QwtSymbol(QwtSymbol::NoSymbol, br, QPen(boxEdgeColor->color(),
                                    boxEdgeWidth->value(),Qt::SolidLine), QSize(size, size));
			b->setSymbol(s);
		}
	}
	else if (privateTabWidget->currentPage() == boxPage)
	{
		BoxCurve *b = (BoxCurve*)plotItem;
		if (b)
		{
			b->setBoxWidth(boxWidth->value());
			b->setBoxStyle(boxType->currentIndex());
			if (boxCnt->isVisible())
				b->setBoxRange(boxRange->currentIndex()+1, boxCnt->value());
			else
				b->setBoxRange(boxRange->currentIndex()+1, (double)boxCoef->value());

			if (whiskerCnt->isVisible())
				b->setWhiskersRange(boxWhiskersRange->currentIndex(), whiskerCnt->value());
			else
				b->setWhiskersRange(boxWhiskersRange->currentIndex(), (double)boxWhiskersCoef->value());
		}
	}
	graph->replot();
	graph->notifyChanges();
	return true;
}

void PlotDialog::setAutomaticBinning()
{
	GroupBoxH->setEnabled(!automaticBox->isChecked());
}

bool PlotDialog::validInput()
{
	QString from = histogramBeginBox->text();
	QString to = histogramEndBox->text();
	QString step = binSizeBox->text();
	QRegExp nonDigit("\\D");

	if (histogramBeginBox->text().isEmpty())
	{
		QMessageBox::critical(this, tr("QtiPlot - Input error"), tr("Please enter a valid start limit!"));
		histogramBeginBox->setFocus();
		return false;
	}

	if (histogramEndBox->text().isEmpty())
	{
		QMessageBox::critical(this, tr("QtiPlot - Input error"), tr("Please enter a valid end limit!"));
		histogramEndBox->setFocus();
		return false;
	}

	if (binSizeBox->text().isEmpty())
	{
		QMessageBox::critical(this, tr("QtiPlot - Input error"), tr("Please enter a valid bin size value!"));
		binSizeBox->setFocus();
		return false;
	}

	from = from.remove(".");
	to = to.remove(".");
	step = step.remove(".");

	int pos=from.find("-",0);
	if(pos==0)
		from=from.replace(pos,1,"");

	pos=to.find("-",0);
	if(pos==0)
		to=to.replace(pos,1,"");

	double start,end, stp;
	bool error = false;
	if (from.contains(nonDigit))
	{
		try
		{
			MyParser parser;
			parser.SetExpr((histogramBeginBox->text()).ascii());
			start=parser.Eval();
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(this, tr("QtiPlot - Start limit error"), QString::fromStdString(e.GetMsg()));
			histogramBeginBox->setFocus();
			error = true;
			return false;
		}
	}
	else
		start = histogramBeginBox->text().toDouble();

	if (to.contains(nonDigit))
	{
		try
		{
			MyParser parser;
			parser.SetExpr((histogramEndBox->text()).ascii());
			end=parser.Eval();
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(this, tr("QtiPlot - End limit error"), QString::fromStdString(e.GetMsg()));
			histogramEndBox->setFocus();
			error=true;
			return false;
		}
	}
	else
		end=histogramEndBox->text().toDouble();

	if (start>=end)
	{
		QMessageBox::critical(this, tr("QtiPlot - Input error"), tr("Please enter limits that satisfy: begin < end!"));
		histogramEndBox->setFocus();
		return false;
	}

	if (step.contains(nonDigit))
	{
		try
		{
			MyParser parser;
			parser.SetExpr((binSizeBox->text()).ascii());
			stp=parser.Eval();
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(this, tr("QtiPlot - Bin size input error"),QString::fromStdString(e.GetMsg()));
			binSizeBox->setFocus();
			error=true;
			return false;
		}
	}
	else
		stp=binSizeBox->text().toDouble();

	if (stp <=0)
	{
		QMessageBox::critical(this, tr("QtiPlot - Bin size input error"), tr("Please enter a positive bin size value!"));
		binSizeBox->setFocus();
		return false;
	}

	return true;
}
void PlotDialog::setPiePenStyle(const Qt::PenStyle& style)
{
	if(style == Qt::SolidLine)
		boxPieLineStyle->setCurrentItem(0);
	if(style == Qt::DashLine)
		boxPieLineStyle->setCurrentItem(1);
	if(style == Qt::DotLine)
		boxPieLineStyle->setCurrentItem(2);
	if(style == Qt::DashDotLine)
		boxPieLineStyle->setCurrentItem(3);
	if(style == Qt::DashDotDotLine)
		boxPieLineStyle->setCurrentItem(4);
}

void PlotDialog::setPenStyle(Qt::PenStyle style)
{
	switch (style)
	{
		case Qt::SolidLine:
			boxLineStyle->setCurrentIndex(0);
			break;
		case Qt::DashLine:
			boxLineStyle->setCurrentIndex(1);
			break;
		case Qt::DotLine:
			boxLineStyle->setCurrentIndex(2);
			break;
		case Qt::DashDotLine:
			boxLineStyle->setCurrentIndex(3);
			break;
		case Qt::DashDotDotLine:
			boxLineStyle->setCurrentIndex(4);
			break;
	}
}

void PlotDialog::setBoxType(int index)
{
	boxCoeffLabel->hide();
	boxRangeLabel->hide();
	boxRange->hide();
	boxCoef->hide();
	boxCntLabel->hide();
	boxCnt->hide();

	if (index != BoxCurve::NoBox && index != BoxCurve::WindBox)
	{
		boxRange->show();
		boxRangeLabel->show();
		int id = boxRange->currentIndex() + 1;
		if (id == BoxCurve::UserDef)
		{
			boxCoef->show();
			boxCoeffLabel->show();
		}
		else if (id == BoxCurve::SD || id == BoxCurve::SE)
		{
			boxCntLabel->show();
			boxCnt->show();
		}
	}
}

void PlotDialog::setBoxRangeType(int index)
{
	boxCoeffLabel->hide();
	boxCoef->hide();
	boxCntLabel->hide();
	boxCnt->hide();

	index++;
	if (index == BoxCurve::UserDef)
	{
		boxCoeffLabel->show();
		boxCoef->show();
	}
	else if (index == BoxCurve::SD || index == BoxCurve::SE)
	{
		boxCntLabel->show();
		boxCnt->show();
	}
}

void PlotDialog::setWhiskersRange(int index)
{
	whiskerCoeffLabel->hide();
	boxWhiskersCoef->hide();
	whiskerCntLabel->hide();
	whiskerCnt->hide();

	if (index == BoxCurve::UserDef)
	{
		whiskerCoeffLabel->show();
		boxWhiskersCoef->show();
	}
	else if (index == BoxCurve::SD || index == BoxCurve::SE)
	{
		whiskerCntLabel->show();
		whiskerCnt->show();
	}
}

void PlotDialog::customVectorsPage(bool angleMag)
{
	if (angleMag)
	{
		GroupBoxVectEnd->setTitle(tr("Vector Data"));
		labelXEnd->setText(tr("Angle"));
		labelYEnd->setText(tr("Magnitude"));
		labelPosition->show();
		vectPosBox->show();
	}
	else
	{
		GroupBoxVectEnd->setTitle(tr("End Point"));
		labelXEnd->setText(tr("X End"));
		labelYEnd->setText(tr("Y End"));
		labelPosition->hide();
		vectPosBox->hide();
	}
}

void PlotDialog::showColorMapEditor(bool)
{
  	if (grayScaleBox->isChecked() || defaultScaleBox->isChecked())
  		colorMapEditor->hide();
  	else
  	{
  	 	colorMapEditor->show();
  	    colorMapEditor->setFocus();
  	}
}

void PlotDialog::showDefaultContourLinesBox(bool)
{
  	if (autoContourBox->isChecked())
  		defaultPenBox->hide();
  	else
  		defaultPenBox->show();
}

void PlotDialog::pickContourLinesColor()
{
  	QColor color = QColorDialog::getColor(levelsColorBox->color(), this);
  	if ( !color.isValid() || color == levelsColorBox->color() )
  		return;

  	levelsColorBox->setColor(color);
}

void PlotDialog::updateTreeWidgetItem(QTreeWidgetItem *item)
{
    if (item->type() != QTreeWidgetItem::Type)
        return;

    if (item->isExpanded())
        item->setIcon(0, QIcon(folder_open));
    else
        item->setIcon(0, QIcon(folder_closed));
}

void PlotDialog::updateBackgroundTransparency(int alpha)
{
	boxBackgroundColor->setEnabled(alpha);

	if (boxAll->isChecked())
	{
		QWidgetList allPlots = d_ml->graphPtrs();
		for (int i=0; i<allPlots.count();i++)
		{
			Graph* g = (Graph*)allPlots.at(i);
			if (g)
			{
				QColor c = boxBackgroundColor->color();
				c.setAlpha(boxBackgroundTransparency->value());
				g->setBackgroundColor(c);
			}
		}
	}
	else
	{
	    LayerItem *item = (LayerItem *)listBox->currentItem();
        if (!item)
            return;
        Graph *g = item->graph();
		if (g)
		{
			QColor c = boxBackgroundColor->color();
			c.setAlpha(boxBackgroundTransparency->value());
			g->setBackgroundColor(c);
		}
	}
}

void PlotDialog::updateCanvasTransparency(int alpha)
{
    boxCanvasColor->setEnabled(alpha);

	if (boxAll->isChecked())
	{
		QWidgetList allPlots = d_ml->graphPtrs();
		for (int i=0; i<allPlots.count();i++)
		{
			Graph* g = (Graph*)allPlots.at(i);
			if (g)
			{
				QColor c = boxCanvasColor->color();
				c.setAlpha(boxCanvasTransparency->value());
				g->setCanvasBackground(c);
			}
		}
	}
	else
	{
		LayerItem *item = (LayerItem *)listBox->currentItem();
        if (!item)
            return;
        Graph *g = item->graph();
		if (g)
		{
			QColor c = boxCanvasColor->color();
			c.setAlpha(boxCanvasTransparency->value());
			g->setCanvasBackground(c);
		}
	}
}

void PlotDialog::pickCanvasColor()
{
	QColor c = QColorDialog::getColor(boxCanvasColor->color(), this);
    if ( !c.isValid() || c == boxCanvasColor->color() )
			return;

	boxCanvasColor->setColor ( c ) ;

	if (boxAll->isChecked())
	{
		QWidgetList allPlots = d_ml->graphPtrs();
		for (int i=0; i<allPlots.count();i++)
		{
			Graph* g=(Graph*)allPlots.at(i);
			if (g)
			{
				c.setAlpha(boxCanvasTransparency->value());
				g->setCanvasBackground(c);
				g->replot();
			}
		}
	}
	else
	{
		LayerItem *item = (LayerItem *)listBox->currentItem();
        if (!item)
            return;
        Graph *g = item->graph();
		if (g)
		{
			c.setAlpha(boxCanvasTransparency->value());
			g->setCanvasBackground(c);
			g->replot();
		}
	}
}

void PlotDialog::pickBackgroundColor()
{
	QColor c = QColorDialog::getColor(boxBackgroundColor->color(), this);
    if ( !c.isValid() || c == boxBackgroundColor->color() )
			return;

	boxBackgroundColor->setColor ( c ) ;

	if (boxAll->isChecked())
	{
		QWidgetList allPlots = d_ml->graphPtrs();
		for (int i=0; i<(int)allPlots.count();i++)
		{
			Graph* g=(Graph*)allPlots.at(i);
			if (g)
			{
				c.setAlpha(boxBackgroundTransparency->value());
				g->setBackgroundColor(c);
				g->replot();
			}
		}
	}
	else
	{
		LayerItem *item = (LayerItem *)listBox->currentItem();
        if (!item)
            return;
        Graph *g = item->graph();
		if (g)
		{
			c.setAlpha(boxBackgroundTransparency->value());
			g->setBackgroundColor(c);
			g->replot();
		}
	}
}

void PlotDialog::pickBorderColor()
{
	QColor c = QColorDialog::getColor(boxBorderColor->color(), this);
	if ( !c.isValid() || c == boxBorderColor->color() )
		return;

	boxBorderColor->setColor ( c ) ;

	if (boxAll->isChecked())
	{
		QWidgetList allPlots = d_ml->graphPtrs();
		for (int i=0; i<allPlots.count();i++)
		{
			Graph* g=(Graph*)allPlots.at(i);
			if (g)
				g->setBorder(boxBorderWidth->value(), c);
		}
	}
	else
	{
		LayerItem *item = (LayerItem *)listBox->currentItem();
        if (!item)
            return;
        Graph *g = item->graph();
		if (g)
			g->setBorder(boxBorderWidth->value(), c);
	}
	d_ml->notifyChanges();
}

void PlotDialog::updateAntialiasing(bool on)
{
	if (privateTabWidget->currentWidget() != layerPage)
		return;

	if (boxAll->isChecked())
	{
		QWidgetList allPlots = d_ml->graphPtrs();
		for (int i=0; i<allPlots.count();i++)
		{
			Graph* g = (Graph*)allPlots.at(i);
			if (g)
				g->setAntialiasing(on);
		}
	}
	else
	{
		LayerItem *item = (LayerItem *)listBox->currentItem();
        if (!item)
            return;
        Graph *g = item->graph();
		if (g)
			g->setAntialiasing(on);
	}
}

void PlotDialog::updateBorder(int width)
{
	if (privateTabWidget->currentWidget() != layerPage)
		return;

	if (boxAll->isChecked())
	{
		QWidgetList allPlots = d_ml->graphPtrs();
		for (int i=0; i<allPlots.count();i++)
		{
			Graph* g=(Graph*)allPlots.at(i);
			if (g)
				g->setBorder(width, boxBorderColor->color());
		}
	}
	else
	{
		LayerItem *item = (LayerItem *)listBox->currentItem();
        if (!item)
            return;
        Graph *g = item->graph();
		if (g)
			g->setBorder(width, boxBorderColor->color());
	}
	d_ml->notifyChanges();
}

void PlotDialog::changeMargin(int width)
{
	if (privateTabWidget->currentWidget() != layerPage)
		return;

    if (boxAll->isChecked())
    {
        QWidgetList allPlots = d_ml->graphPtrs();
        for (int i=0; i<allPlots.count();i++)
        {
            Graph* g=(Graph*)allPlots.at(i);
            if (g)
                g->changeMargin(width);
        }
    }
    else
    {
        LayerItem *item = (LayerItem *)listBox->currentItem();
        if (!item)
            return;
        Graph *g = item->graph();
        if (g)
            g->changeMargin(width);
    }
}

void PlotDialog::setTitlesFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,titleFont,this);
	if ( ok ) {
		titleFont = font;
	} else {
		return;
	}
}

void PlotDialog::setAxesLabelsFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,axesFont,this);
	if ( ok ) {
		axesFont = font;
	} else {
		return;
	}
}

void PlotDialog::setAxesNumbersFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,numbersFont,this);
	if ( ok ) {
		numbersFont = font;
	} else {
		return;
	}
}

void PlotDialog::setLegendsFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, legendFont,this);
	if ( ok ) {
		legendFont = font;
	} else {
		return;
	}
}

void PlotDialog::initFonts(const QFont& titlefont, const QFont& axesfont, const QFont& numbersfont, const QFont& legendfont)
{
	axesFont = axesfont;
	titleFont = titlefont;
	numbersFont = numbersfont;
	legendFont = legendfont;
}

void PlotDialog::closeEvent(QCloseEvent* e)
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (app)
		app->d_extended_plot_dialog = btnMore->isChecked ();

	e->accept();
}

/*****************************************************************************
 *
 * Class LayerItem
 *
 *****************************************************************************/

LayerItem::LayerItem(Graph *g, QTreeWidgetItem *parent, const QString& s)
    : QTreeWidgetItem( parent, QStringList(s), LayerTreeItem ),
      d_graph(g)
{
    setIcon(0, QPixmap(layer_disabled_xpm));
    if (g)
        insertCurvesList();
}

void LayerItem::setActive(bool on)
{
    if (on)
		setIcon(0, QPixmap(layer_enabled_xpm));
    else
		setIcon(0, QPixmap(layer_disabled_xpm));
}

void LayerItem::insertCurvesList()
{
	for (int i=0; i<d_graph->curves(); i++)
	{
        QString plotAssociation = QString();
        const QwtPlotItem *it = (QwtPlotItem *)d_graph->plotItem(i);
        if (!it)
            continue;

        if (it->rtti() == QwtPlotItem::Rtti_PlotCurve)
        {
            PlotCurve *c = (PlotCurve *)it;
            if (c->type() != Graph::Function)
            {
                QString s = ((DataCurve *)it)->plotAssociation();
                QString table = ((DataCurve *)it)->table()->name();
                plotAssociation = table + ": " + s.remove(table + "_");
            }
            else
                plotAssociation = it->title().text();
        }
        else
            plotAssociation = it->title().text();

        addChild(new CurveTreeItem(it, this, plotAssociation));
	}
}

/*****************************************************************************
 *
 * Class CurveTreeItem
 *
 *****************************************************************************/

CurveTreeItem::CurveTreeItem(const QwtPlotItem *curve, LayerItem *parent, const QString& s)
    : QTreeWidgetItem( parent, QStringList(s), PlotCurveTreeItem ),
      d_curve(curve)
{
    setIcon(0, QPixmap(graph_disabled_xpm));
}

void CurveTreeItem::setActive(bool on)
{
    if (on)
		setIcon(0, QPixmap(graph_xpm));
    else
		setIcon(0, QPixmap(graph_disabled_xpm));
}

int CurveTreeItem::plotItemIndex()
{
Graph *g = graph();
if (!g)
    return -1;

return g->plotItemIndex((QwtPlotItem *)d_curve);
}

int CurveTreeItem::plotItemType()
{
Graph *g = graph();
if (!g)
    return -1;

int index = g->plotItemIndex((QwtPlotItem *)d_curve);
return g->curveType(index);
}
