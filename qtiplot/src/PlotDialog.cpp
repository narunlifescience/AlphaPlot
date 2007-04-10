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
#include "Graph.h"
#include "Table.h"
#include "MyParser.h"
#include "QwtHistogram.h"
#include "VectorCurve.h"
#include "QwtErrorPlotCurve.h"
#include "BoxCurve.h"
#include "FunctionCurve.h"
#include "Spectrogram.h"
#include "ColorMapEditor.h"

#include <QListWidget>
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

PlotDialog::PlotDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
: QDialog( parent, name, modal, fl )
{
	if ( !name )
		setName( "PlotDialog" );
	setWindowTitle( tr( "QtiPlot - Custom curves" ) );

    QVBoxLayout* vl = new QVBoxLayout();
	listBox = new QListWidget();
	vl->addWidget(listBox);
	btnAssociations = new QPushButton(tr( "&Plot Associations..." ));
	btnAssociations->hide();
    vl->addWidget(btnAssociations);
	btnEditFunction = new QPushButton(tr( "&Edit Function..." ));
	btnEditFunction->hide();
    vl->addWidget(btnEditFunction);

    QGridLayout *gl = new QGridLayout(this);
    gl->addLayout(vl, 0, 0);

	privateTabWidget = new QTabWidget();
    gl->addWidget(privateTabWidget, 0, 1);

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

	clearTabWidget();
	graph = 0;

    QHBoxLayout* hb1 = new QHBoxLayout();
    hb1->addWidget(new QLabel(tr("Plot type")));
    boxPlotType = new QComboBox();
    boxPlotType->setEditable(false);
    hb1->addWidget(boxPlotType);
    hb1->addStretch();
    gl->addLayout(hb1, 1, 0);

    QHBoxLayout* hb2 = new QHBoxLayout();
	btnWorksheet = new QPushButton(tr( "&Worksheet" ) );
    hb2->addWidget(btnWorksheet);
	buttonApply = new QPushButton(tr( "&Apply" ));
    hb2->addWidget(buttonApply);
	buttonOk = new QPushButton(tr( "&OK" ));
	buttonOk->setDefault( true );
    hb2->addWidget(buttonOk);
	buttonCancel = new QPushButton(tr( "&Cancel" ));
    hb2->addWidget(buttonCancel);
    gl->addLayout(hb2, 1, 1);

    resize(minimumSize());

	connect( buttonOk, SIGNAL(clicked()), this, SLOT(quit() ) );
	connect( buttonCancel, SIGNAL(clicked()), this, SLOT(close()));
	connect( buttonApply, SIGNAL(clicked() ), this, SLOT(acceptParams() ) );
	connect( btnWorksheet, SIGNAL(clicked()), this, SLOT(showWorksheet()));
	connect( btnAssociations, SIGNAL(clicked()), this, SLOT(showPlotAssociations()));
	connect( btnEditFunction, SIGNAL(clicked()), this, SLOT(editFunctionCurve()));
	connect(listBox, SIGNAL(itemDoubleClicked( QListWidgetItem *)),
            this, SLOT(showPlotAssociations( QListWidgetItem *)));
	connect(listBox, SIGNAL(currentRowChanged(int)), this, SLOT(updateTabWindow(int)));
	connect(boxPlotType, SIGNAL(activated(int)), this, SLOT(changePlotType(int)));

	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(removeSelectedCurve()));
}

void PlotDialog::showPlotAssociations( QListWidgetItem *item)
{
	if (!item)
		return;

	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (!app)
		return;

	int curveIndex = listBox->row(item);
	QwtPlotCurve *c = (QwtPlotCurve*)graph->curve(curveIndex);
	if (!c)
  		return;

	if (c->rtti() == FunctionCurve::RTTI)
  	    app->showFunctionDialog(graph, curveIndex);
	else if (c->rtti() == QwtPlotItem::Rtti_PlotCurve)
		 app->showPlotAssociations(curveIndex);
	else if (c->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
  		{
  	    Spectrogram *sp = (Spectrogram *)c;
  	    if (sp->matrix())
  	    	sp->matrix()->showMaximized();
  	    }
	close();
}

void PlotDialog::showPlotAssociations()
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (app)
		app->showPlotAssociations(listBox->currentRow());
}

void PlotDialog::changePlotType(int plotType)
{
	int curve = listBox->currentRow();
	int curveType = graph->curveType(curve);
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

		VectorCurve *v = (VectorCurve*)graph->curve(curve);
		if (plotType)
		{
			graph->setCurveType(curve, Graph::VectXYAM);
			v->setVectorStyle(VectorCurve::XYAM);
		}
		else
		{
			graph->setCurveType(curve, Graph::VectXYXY);
			v->setVectorStyle(VectorCurve::XYXY);
		}
		customVectorsPage(plotType);
	}
	else
	{
		clearTabWidget();
		insertTabs(plotType);

		lastSelectedCurve = curve;
		graph->setCurveType(lastSelectedCurve, plotType);
		setActiveCurve(lastSelectedCurve);

		boxConnect->setCurrentIndex(1);//show line for Line and LineSymbol plots

		QwtSymbol s = QwtSymbol(QwtSymbol::Ellipse, QBrush(), QPen(), QSize(9,9));
		if (plotType == Graph::Line)
			s.setStyle(QwtSymbol::NoSymbol);
		else if (plotType == Graph::Scatter)
			graph->setCurveStyle(curve, QwtPlotCurve::NoCurve);
		else if (plotType == Graph::LineSymbols)
			graph->setCurveStyle(curve, QwtPlotCurve::Lines);

		if (plotType)
		{
			boxSymbolStyle->setCurrentIndex(1);
			boxFillSymbol->setChecked(true);
			boxFillColor->setEnabled(true);
		}
		graph->setCurveSymbol(curve, s);
	}

	acceptParams();
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
	privateTabWidget->insertTab(axesPage, tr( "Axes" ) );
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
	privateTabWidget->insertTab( linePage, tr( "Line" ) );

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

void PlotDialog::setGraph(Graph *g)
{
	graph = g;
	insertCurvesList();
    resize(minimumSize());

	connect( graph, SIGNAL(modifiedFunction()), this, SLOT(insertCurvesList() ) );
	connect( graph, SIGNAL( modifiedPlotAssociation() ), this, SLOT( insertCurvesList() ) );
}

void PlotDialog::selectCurve(int index)
{
	insertTabs(graph->curveType(index));
	lastSelectedCurve = index;
	listBox->setCurrentRow(index);
	setActiveCurve(index);
}

void PlotDialog::showStatistics()
{
	QString text=listBox->currentItem()->text();
	QStringList t=text.split(": ", QString::SkipEmptyParts);
	QStringList list=t[1].split(",", QString::SkipEmptyParts);
	text=t[0] + "_" + list[1].remove("(Y)");

	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (app)
	{
		Table* w=app->table(text);
		if (!w)
			return;
		QString result=graph->showHistogramStats(w, text, listBox->currentRow());
		if (!result.isEmpty())
		{
			app->logInfo+=result;
			app->showResults(true);
		}
	}
	close();
}

void PlotDialog::contextMenuEvent(QContextMenuEvent *e)
{
	lastSelectedCurve = listBox->currentRow();
    QwtPlotCurve *c = graph->curve(lastSelectedCurve);
	if (!c)
		return;

	QPoint pos = listBox->viewport()->mapFromGlobal(QCursor::pos());
	QRect rect = listBox->visualItemRect(listBox->currentItem());
	if (rect.contains(pos))
	{
	   QMenu contextMenu(this);
	   contextMenu.insertItem(tr("&Delete"), this, SLOT(removeSelectedCurve()));
	   if (c->rtti() == FunctionCurve::RTTI)
		  contextMenu.insertItem(tr("&Edit..."), this, SLOT(editFunctionCurve()));
	   else if (c->rtti() == QwtPlotItem::Rtti_PlotCurve)
		  contextMenu.insertItem(tr("&Plot Associations..."), this, SLOT(showPlotAssociations()));

	   contextMenu.exec(QCursor::pos());
    }
    e->accept();
}

void PlotDialog::editFunctionCurve()
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (app)
		app->showFunctionDialog(graph, listBox->currentRow());
}

void PlotDialog::removeSelectedCurve()
{
	int curve=listBox->currentRow();
	graph->removeCurve(curve);
	QListWidgetItem *it =listBox->takeItem(curve);
    delete it;

	if (listBox->count() == 0)
		close();
}

void PlotDialog::changeErrorBarsPlus()
{
	graph->updateErrorBars(listBox->currentRow(),xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(),colorBox->color(), plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::changeErrorBarsMinus()
{
	graph->updateErrorBars(listBox->currentRow(),xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(), colorBox->color(),plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::changeErrorBarsThrough()
{
	graph->updateErrorBars(listBox->currentRow(),xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(), colorBox->color(),plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::changeErrorBarsType()
{
	graph->updateErrorBars(listBox->currentRow(), xBox->isChecked(), widthBox->currentText().toInt(),
			capBox->currentText().toInt(), colorBox->color(), plusBox->isChecked(), minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::pickErrorBarsColor()
{
	QColor color = QColorDialog::getColor(colorBox->color(), this);
	if ( !color.isValid() || color == colorBox->color() )
		return;

	colorBox->setColor (color) ;

	graph->updateErrorBars(listBox->currentRow(),xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(), color, plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::showAreaColor(bool show)
{
	boxAreaColor->setEnabled(show);
	boxPattern->setEnabled(show);
}

void PlotDialog::updateTabWindow(int curveIndex)
{
	int plot_type = setPlotType(curveIndex);
	if (plot_type < 0)
	{
		lastSelectedCurve = -1;
		return;
	}
	else if (graph->curveType(lastSelectedCurve) == plot_type)
	{
		setActiveCurve(curveIndex);
		return;
	}

	clearTabWidget();

	insertTabs(plot_type);
	setActiveCurve(curveIndex);
	lastSelectedCurve = curveIndex;
}

void PlotDialog::insertTabs(int plot_type)
{
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
	privateTabWidget->removePage(linePage);
	privateTabWidget->removePage(symbolPage);
	privateTabWidget->removePage(errorsPage);
	privateTabWidget->removePage(histogramPage);
	privateTabWidget->removePage(spacingPage);
	privateTabWidget->removePage(vectPage);
	privateTabWidget->removePage(boxPage);
	privateTabWidget->removePage(percentilePage);
	privateTabWidget->removePage(spectrogramPage);
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

	app->showCurveWorksheet(graph, listBox->currentRow());
	close();
}

int PlotDialog::setPlotType(int index)
{
	int size=listBox->count();
	int curveType = graph->curveType(index);
	if (size>0 && curveType >= 0)
	{
		boxPlotType->clear();

		if (curveType == Graph::ErrorBars)
			boxPlotType->addItem( tr( "Error Bars" ) );
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

			QwtPlotCurve *c = (QwtPlotCurve*)graph->curve(index);
			if (!c)
				return Graph::Line;

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

void PlotDialog::setActiveCurve(int index)
{//connected to the listBox highlighted signal, displays the current curve parameters
	if (listBox->count() > 0)
	{
		QwtPlotItem *i = graph->plotItem(index);
		if (!i)
			return;

		//axes page
  		boxXAxis->setCurrentItem(i->xAxis()-2);
  	    boxYAxis->setCurrentItem(i->yAxis());

  	    if (i->rtti() == QwtPlotItem::Rtti_PlotSpectrogram)
  	    {
  	    	btnAssociations->hide();
  	        btnEditFunction->hide();
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

  	    QwtPlotCurve *c = (QwtPlotCurve*)i;
		if (c->rtti() == FunctionCurve::RTTI)
		{
			btnAssociations->hide();
			btnEditFunction->show();
		}
		else if (c->rtti() == QwtPlotItem::Rtti_PlotCurve)
		{
			btnAssociations->show();
			btnEditFunction->hide();
		}

		int curveType = graph->curveType(index);

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
			QwtBarCurve *b = (QwtBarCurve*)graph->curve(index);
			if (b)
			{
				gapBox->setValue(b->gap());
				offsetBox->setValue(b->offset());
			}
		}

		if (curveType == Graph::Histogram)
		{//Histogram page
			QwtHistogram *h = (QwtHistogram*)graph->curve(index);
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
			VectorCurve *v = (VectorCurve*)graph->curve(index);
			if (v)
			{
				vectColorBox->setColor(v->color());
				vectWidthBox->setValue(v->width());
				headLengthBox->setValue(v->headLength());
				headAngleBox->setValue(v->headAngle());
				filledHeadBox->setChecked(v->filledArrowHead());
				vectPosBox->setCurrentIndex(v->position());
				updateEndPointColumns(listBox->currentItem()->text());
			}
		}

		if (curveType == Graph::ErrorBars)
		{
			QwtErrorPlotCurve *err = (QwtErrorPlotCurve*)graph->curve(index);
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
			BoxCurve *b = (BoxCurve*)graph->curve(index);
			if (b)
			{
				boxMaxStyle->setStyle(b->maxStyle());
				boxMinStyle->setStyle(b->minStyle());
				boxMeanStyle->setStyle(b->meanStyle());
				box99Style->setStyle(b->p99Style());
				box1Style->setStyle(b->p1Style());

				boxPercSize->setValue(s.size().width());
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
	if (privateTabWidget->currentPage() == axesPage)
	{
		QwtPlotItem *c = graph->plotItem(listBox->currentRow());
		if (!c)
			return false;

		c->setAxis(boxXAxis->currentIndex() + 2, boxYAxis->currentIndex());
		graph->setAutoScale();
		return true;
	}
	else if (privateTabWidget->currentPage() == spectrogramPage)
  	{
  		Spectrogram *sp = (Spectrogram *)graph->plotItem(listBox->currentRow());
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
		int index=listBox->currentRow();
		graph->setCurveStyle(index, boxConnect->currentIndex());
		QBrush br = QBrush(boxAreaColor->color(), boxPattern->getSelectedPattern());
		if (!fillGroupBox->isChecked())
			br = QBrush();
		graph->setCurveBrush(index, br);
		QPen pen = QPen(boxLineColor->color(),boxLineWidth->value(),Graph::getPenStyle(boxLineStyle->currentIndex()));
		graph->setCurvePen(index, pen);
	}
	else if (privateTabWidget->currentPage()==symbolPage)
	{
		int size = 2*boxSymbolSize->value()+1;
		QBrush br = QBrush(boxFillColor->color(), Qt::SolidPattern);
		if (!boxFillSymbol->isChecked())
			br = QBrush();
		QPen pen = QPen(boxSymbolColor->color(),boxPenWidth->value(),Qt::SolidLine);
		QwtSymbol s = QwtSymbol(boxSymbolStyle->selectedSymbol(), br, pen, QSize(size, size));
		graph->setCurveSymbol(listBox->currentRow(), s);
	}
	else if (privateTabWidget->currentPage()==histogramPage)
	{
        QwtHistogram *h = (QwtHistogram *)graph->curve(listBox->currentRow());
		if (!h)
			return false;

		QString text = listBox->currentItem()->text();
		QStringList t = text.split(": ", QString::SkipEmptyParts);
		QStringList list = t[1].split(",", QString::SkipEmptyParts);
		text = t[0] + "_" + list[1].remove("(Y)");
		bool accept = validInput();
		if (accept)
		{
			ApplicationWindow *app = (ApplicationWindow *)this->parent();
			if (!app)
				return false;

            if (h->autoBinning() == automaticBox->isChecked() &&
                h->binSize() == binSizeBox->text().toDouble() &&
                h->begin() == histogramBeginBox->text().toDouble() &&
                h->end() == histogramEndBox->text().toDouble()) return false;

			Table* w = app->table(text);
			if (w)
				graph->updateHistogram(w, text, listBox->currentRow(), automaticBox->isChecked(), binSizeBox->text().toDouble(),
						histogramBeginBox->text().toDouble(), histogramEndBox->text().toDouble());
		}
		return accept;
	}
	else if (privateTabWidget->currentPage()==spacingPage)
		graph->setBarsGap(listBox->currentRow(),gapBox->value(), offsetBox->value());
	else if (privateTabWidget->currentPage() == vectPage)
	{
		int index=listBox->currentRow();
		ApplicationWindow *app = (ApplicationWindow *)this->parent();
		if (!app)
			return false;

		QString xEndCol = xEndBox->currentText();
		QString yEndCol = yEndBox->currentText();
		Table* w=app->table(xEndCol);
		if (!w)
			return false;

		graph->updateVectorsLayout(w, index, vectColorBox->currentIndex(), vectWidthBox->value(),
				headLengthBox->value(), headAngleBox->value(),
				filledHeadBox->isChecked(),vectPosBox->currentIndex(),xEndCol,yEndCol);

		QString text=listBox->currentItem()->text();
		QStringList t=text.split(": ", QString::SkipEmptyParts);
		QString table = t[0];

		QStringList cols=t[1].split(",", QString::SkipEmptyParts);
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
			text=table + ": " + cols.join(",");
			//listBox->changeItem (text, index);
		}
		return true;
	}
	else if (privateTabWidget->currentPage() == errorsPage)
	{
		graph->updateErrorBars(listBox->currentRow(), xBox->isChecked(), widthBox->currentText().toInt(),
				capBox->currentText().toInt(), colorBox->color(), plusBox->isChecked(), minusBox->isChecked(),
				throughBox->isChecked());
	}
	else if (privateTabWidget->currentPage() == percentilePage)
	{
		int size = 2*boxPercSize->value() + 1;
		QBrush br = QBrush(boxPercFillColor->color(), Qt::SolidPattern);
		if (!boxFillSymbols->isChecked())
			br = QBrush();
		QwtSymbol s = QwtSymbol(QwtSymbol::NoSymbol, br, QPen(boxEdgeColor->color(),boxEdgeWidth->value(),Qt::SolidLine), QSize(size, size));
		graph->setCurveSymbol(listBox->currentRow(), s);

		BoxCurve *b = (BoxCurve*)graph->curve(listBox->currentRow());
		if (b)
		{
			b->setMaxStyle(boxMaxStyle->selectedSymbol());
			b->setP99Style(box99Style->selectedSymbol());
			b->setMeanStyle(boxMeanStyle->selectedSymbol());
			b->setP1Style(box1Style->selectedSymbol());
			b->setMinStyle(boxMinStyle->selectedSymbol());
		}
	}
	else if (privateTabWidget->currentPage() == boxPage)
	{
		BoxCurve *b = (BoxCurve*)graph->curve(listBox->currentRow());
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
	graph->emitModified();
	return true;
}

void PlotDialog::insertCurvesList()
{
	QStringList names = graph->plotAssociations();
	listBox->clear();
	QStringList newNames = QStringList();
	for (int i=0; i<(int)names.count(); i++)
	{
		QString s=names[i];
		int pos=s.find("_",0);
		if (pos>0)
		{
			QString table=s.left(pos);
			QString cols=s.right(s.length()-pos-1);
			newNames<<table+": "+cols.remove(table+"_",true);
		}
		else
			newNames<<s;
	}
	listBox->addItems(newNames);
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

