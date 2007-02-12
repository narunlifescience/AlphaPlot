/***************************************************************************
    File                 : plotDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include "graph.h"
#include <QDialog>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QWidget;
class QStringList;
class QGroupBox;
class QDoubleSpinBox;
class QRadioButton;
		
class ColorBox;
class PatternBox;
class ColorButton;
class Graph;
class SymbolBox;

//! Custom curves dialog
class PlotDialog : public QDialog
{ 
    Q_OBJECT

public:
    PlotDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~PlotDialog();

public slots:
	void showStatistics();
	void customVectorsPage(bool angleMag);
	void insertCurvesList();
	void insertColumnsList(const QStringList& names){columnNames = names;};
	void updateEndPointColumns(const QString& text);
	
	void fillBoxSymbols();
	void fillSymbols();
	bool acceptParams();
	void showWorksheet();
	void quit();

	int setPlotType(int index);
	void changePlotType(int plotType);
	void setActiveCurve(int curveIndex);

	void insertTabs(int plot_type);
	void updateTabWindow(int);
	void showAreaColor(bool show);

	void removeSelectedCurve();
	
	/******* error bars options **************/
	void pickErrorBarsColor();
	void changeErrorBarsType();
	void changeErrorBarsPlus();
	void changeErrorBarsMinus();
	void changeErrorBarsThrough();
	
	void setAutomaticBinning();
	bool validInput();
	void showPlotAssociations();
	void showPlotAssociations( QListWidgetItem *item);
	void editFunctionCurve();
	void setGraph(Graph *g);
	void selectCurve(int index);

	void setPenStyle(Qt::PenStyle style);

	//box plots
	void setBoxType(int index);
	void setBoxRangeType(int index);
	void setWhiskersRange(int index);
	
	//spectrograms
  	void pickContourLinesColor();
  	void showDefaultContourLinesBox(bool show);
	
protected:
    void clearTabWidget();
	void initAxesPage();
	void initLinePage();
	void initSymbolsPage();
	void initHistogramPage();
	void initErrorsPage();
	void initSpacingPage();
	void initVectPage();
	void initBoxPage();
	void initPercentilePage();
	void initSpectrogramPage();
    void contextMenuEvent(QContextMenuEvent *e);

    Graph *graph;
	QStringList columnNames;
	int lastSelectedCurve;

    QListWidget* listBox;
    QPushButton* buttonApply, *btnWorksheet, *btnAssociations;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QComboBox* boxPlotType;
    QWidget* linePage;
    QComboBox* boxConnect;
    QComboBox* boxLineStyle;
    QSpinBox* boxLineWidth, *boxPenWidth;
    ColorBox* boxLineColor, *boxAreaColor;
    QWidget* symbolPage;
    QSpinBox* boxSymbolSize;
    ColorBox* boxSymbolColor,*boxFillColor;
    SymbolBox* boxSymbolStyle;
    PatternBox *boxPattern;
	QTabWidget* privateTabWidget;
	QWidget *errorsPage, *spectrogramPage;
	QGroupBox* fillGroupBox;
    QCheckBox* plusBox;
    QCheckBox* minusBox;
    QCheckBox* xBox;
    ColorButton *colorBox, *levelsColorBox;
	ColorBox* vectColorBox;
    QComboBox* widthBox;
    QComboBox* capBox;
    QCheckBox* throughBox;
	QLabel *labelPosition, *labelXEnd, *labelYEnd;
	QGroupBox* GroupBoxH;
	QWidget *histogramPage, *spacingPage;
	QLineEdit *binSizeBox, *histogramBeginBox, *histogramEndBox;
	QCheckBox *automaticBox;
	QPushButton* buttonStatistics, *btnEditFunction;
	QSpinBox* gapBox, *offsetBox, *boxWidth;
	QWidget *vectPage, *boxPage, *percentilePage, *axesPage;
	QComboBox *xEndBox, *yEndBox, *boxType, *boxWhiskersType, *boxWhiskersRange, *boxRange;
	QSpinBox* headAngleBox, *headLengthBox, *vectWidthBox, *boxPercSize, *boxEdgeWidth;
	QCheckBox *filledHeadBox;
	QSpinBox *boxCoef, *boxWhiskersCoef;
	QCheckBox *boxFillSymbols, *boxFillSymbol;
	ColorBox *boxPercFillColor, *boxEdgeColor;
	QLabel 	*whiskerCoeffLabel, *whiskerRangeLabel, *boxCoeffLabel;
	QLabel *boxRangeLabel, *whiskerCntLabel, *boxCntLabel;
	QGroupBox *GroupBoxVectEnd;
	QComboBox *vectPosBox, *boxXAxis, *boxYAxis, *colorScaleBox, *boxContourStyle;
  	QSpinBox *levelsBox, *contourWidthBox, *colorScaleWidthBox;
  	QGroupBox *levelsGroupBox, *axisScaleBox, *imageGroupBox;
  	QGroupBox *defaultPenBox;
  	QRadioButton *defaultScaleBox, *grayScaleBox, *customScaleBox, *defaultContourBox, *autoContourBox;
 
    SymbolBox *boxMaxStyle, *boxMinStyle, *boxMeanStyle, *box99Style, *box1Style;
    QDoubleSpinBox *whiskerCnt, *boxCnt;
};

#endif
