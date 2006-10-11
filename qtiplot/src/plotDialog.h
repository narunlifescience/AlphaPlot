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

#include <qvariant.h>
#include <qdialog.h>
#include <qwt_symbol.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QLabel>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

class Q3VBoxLayout; 
class Q3HBoxLayout; 
class Q3GridLayout; 
class QCheckBox;
class QComboBox;
class Q3ListBox;
class Q3ListBoxItem;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QWidget;
class QCheckBox;
class Q3GroupBox;
class Q3ButtonGroup;
class QLabel;
class QLineEdit;
class ColorBox;
class PatternBox;
class ColorButton;
class QwtSymbol;
class Graph;
class SymbolBox;
class QwtCounter;

//! Structure containing curve layout parameters 
typedef struct{ 
  int lCol;
  int lWidth;
  int lStyle;
  int filledArea;
  int aCol;
  int aStyle;
  int symCol;
  int fillCol;
  int penWidth;
  int sSize; 
  int sType;  
  int connectType;
}  CurveLayout;

//! Custom curves dialog
class PlotDialog : public QDialog
{ 
    Q_OBJECT

public:
    PlotDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~PlotDialog();

    Q3ListBox* listBox;
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

	QWidget* errorsPage;
	Q3ButtonGroup* GroupBox0;
	Q3ButtonGroup* GroupBox1;
	Q3ButtonGroup* GroupBox2;
	Q3ButtonGroup* GroupBox3;
	Q3ButtonGroup* fillGroupBox;
    QCheckBox* plusBox;
    QCheckBox* minusBox;
    QCheckBox* xBox;
    ColorButton* colorBox;
	ColorBox* vectColorBox;
    QComboBox* widthBox;
    QComboBox* capBox;
    QCheckBox* throughBox;
	QLabel *TextLabel4_3, *TextLabel4_4, *labelPosition, *labelXEnd, *labelYEnd;

	Q3ButtonGroup* GroupBoxH;
	QWidget *histogramPage, *spacingPage;
	QLineEdit *binSizeBox, *histogramBeginBox, *histogramEndBox;
	QCheckBox *automaticBox;
	QPushButton* buttonStatistics, *btnEditFunction;
	QSpinBox* gapBox, *offsetBox, *boxWidth;

	QWidget *vectPage, *boxPage, *percentilePage, *axesPage;
	QComboBox *xEndBox, *yEndBox, *boxType, *boxWhiskersType, *boxWhiskersRange, *boxRange;
	SymbolBox *boxMaxStyle, *boxMinStyle, *boxMeanStyle, *box99Style, *box1Style;
	QSpinBox* headAngleBox, *headLengthBox, *vectWidthBox, *boxPercSize, *boxEdgeWidth;
	QCheckBox *filledHeadBox, *boxFill;
	QSpinBox *boxCoef, *boxWhiskersCoef;
	QCheckBox *boxFillSymbols, *boxFillSymbol;
	ColorBox *boxPercFillColor, *boxEdgeColor;
	QLabel 	*whiskerCoeffLabel, *whiskerRangeLabel, *boxCoeffLabel;
	QLabel *boxRangeLabel, *whiskerCntLabel, *boxCntLabel;
	QwtCounter *whiskerCnt, *boxCnt;
	Q3ButtonGroup *GroupBoxVectEnd;
	QComboBox *vectPosBox, *boxXAxis, *boxYAxis;

public slots:
	void showStatistics();
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

	void showPopupMenu(Q3ListBoxItem *it, const QPoint &point);
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
	void showPlotAssociations(Q3ListBoxItem *item);
	void editFunctionCurve();
	void setGraph(Graph *g);
	void selectCurve(int index);

	void setPenStyle(Qt::PenStyle style);

	//box plots
	void setBoxType(int index);
	void setBoxRangeType(int index);
	void setWhiskersRange(int index);
	
protected:
	Graph *graph;
	QStringList columnNames;
	int lastSelectedCurve;
};

#endif
