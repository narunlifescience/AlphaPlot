/***************************************************************************
    File                 : axesDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : General plot options dialog
                           
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
#ifndef AXESDIALOG_H
#define AXESDIALOG_H

#include <QDialog>
#include <Q3MemArray>
#include <QLabel>
#include <QList>

class Q3TextEdit;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class Q3ListBox;
class Q3ListBoxItem;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTabWidget;
class QWidget;
class QStringList;
class Q3ButtonGroup;
class Q3GroupBox;
class ColorBox;
class ColorButton;
class MultiLayer;
class Graph;

//! Structure containing grid properties
typedef struct{ 
  int majorOnX;
  int minorOnX;
  int majorOnY;
  int minorOnY;
  int majorStyle;
  int majorCol;
  int majorWidth;
  int minorStyle;
  int minorCol;
  int minorWidth;
  int xZeroOn;
  int yZeroOn;
}  GridOptions;

//! General plot options dialog
class AxesDialog : public QDialog
{ 
    Q_OBJECT

public:
    AxesDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~AxesDialog();

	void initAxesPage();
	void initScalesPage();
	void initGridPage();
	void initFramePage();

	void setMultiLayerPlot(MultiLayer *m);

    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QTabWidget* generalDialog;
    QWidget* scalesPage;
    QLineEdit* boxEnd;   
    QLineEdit* boxStart;
    QComboBox* boxScaleType;
    QSpinBox* boxMinorValue;
    QLineEdit* boxStep;
    QCheckBox* btnStep, *btnInvert;
    QSpinBox* boxMajorValue;
    QCheckBox* btnMinor;
    Q3ListBox* axesList;
    QWidget* gridPage;
    QCheckBox* boxMajorGrid;
    QCheckBox* boxMinorGrid;
    QComboBox* boxTypeMajor;
    ColorBox* boxColorMinor;
    ColorBox* boxColorMajor;
	ColorButton *boxCanvasColor;
    QSpinBox* boxWidthMajor;
    QComboBox* boxTypeMinor;
    QSpinBox* boxWidthMinor;
    QCheckBox* boxXLine;
    QCheckBox* boxYLine;
    Q3ListBox* axesGridList;
    QWidget* axesPage, *frame;
    Q3ListBox* axesTitlesList;
    QCheckBox* boxShowAxis, *boxShowLabels;
	
	Q3TextEdit *boxFormula, *boxTitle;
	QSpinBox *boxFrameWidth, *boxPrecision, *boxAngle, *boxBaseline, *boxAxesLinewidth;
    QPushButton* btnAxesFont;
	QCheckBox *boxBackbones, *boxAll, *boxShowFormula;
	ColorButton* boxAxisColor;
	QComboBox *boxMajorTicksType, *boxMinorTicksType, *boxFormat, *boxAxisType, *boxColName;
	Q3ButtonGroup* GroupBox0;
	Q3ButtonGroup* GroupBox1;
	Q3ButtonGroup* GroupBox2;
	Q3ButtonGroup* GroupBox3;
	Q3ButtonGroup *boxFramed, *GroupBox6;
	QLabel *label1, *label2, *label3, *boxScaleTypeLabel, *minorBoxLabel, *labelTable;
	QSpinBox *boxMajorTicksLength, *boxMinorTicksLength, *boxBorderWidth, *boxMargin;
	QComboBox *boxUnit, *boxTableName;
	ColorButton *boxBorderColor, *boxFrameColor, *boxBackgroundColor;
	Q3GroupBox  *labelBox;
	QPushButton *buttonIndice, *buttonExp, *buttonSym, *buttonB, *buttonI;
    QPushButton *buttonU, *buttonLowerGreek, *buttonUpperGreek, *btnLabelFont;
	
public slots:
	QStringList scaleLimits(int axis, double start, double end, double step, 
							const QString& majors, const QString&minors);
	void setAxesType(const QList<int>& list);
	void setAxisType(int axis);
	void updateAxisType(int axis);
	void setAxisTitles(QStringList t);
	void updateTitleBox(int axis);
	bool updatePlot();
	void updateScale(int axis);
	void updateLineBoxes(int axis);
	void setScaleLimits(const QStringList& limits);
	void stepEnabled();
	void stepDisabled();
	void majorGridEnabled(bool on);
	void minorGridEnabled(bool on);
	GridOptions getGridOptions();
	void putGridOptions(GridOptions gr);
	void setGridOptions();
	void tabPageChanged(QWidget *w);
	void accept();
	void customAxisFont();
	void showAxis();
	void updateShowBox(int axis);
	void setEnabledAxes(Q3MemArray<bool> ok);
	void drawFrame(bool framed);

	void pickAxisColor();
	void setAxisColor(const QColor& c);
	void updateAxisColor(int);
	void setAxesColors(const QStringList& colors);
	int mapToQwtAxisId();
	void setEnabledTickLabels(const QStringList& labelsOn);
	void updateTickLabelsList();
	void setTicksType(const QList<int>& majLst, const QList<int>& minLst);
	void setTicksType(int);
	void setCurrentScale(int axisPos);
	void initAxisFonts(const QFont& xB, const QFont& yL, const QFont& xT, const QFont& yR );

	void updateMajTicksType(int);
	void updateMinTicksType(int);
	void updateGrid(int);
	void updateFrame(int);
	void setLabelsNumericFormat(const QStringList& list);
	void setLabelsNumericFormat(int);
	void updateLabelsFormat(int);
	void initLabelsRotation(int xAngle, int yAngle);
	void insertColList(const QStringList& cols);
	void insertTablesList(const QStringList& l);
	void setAxesLabelsFormatInfo(const QStringList& lst);
	void showAxisFormatOptions(int format);
	void setBaselineDist(int);
	void changeBaselineDist(int baseline);
	void setAxesBaseline(const QList<int>& lst);
	void changeMinorTicksLength (int minLength);
	void changeMajorTicksLength (int majLength);
	void updateBackbones (bool on);
	void pickBorderColor();
	void pickCanvasFrameColor();
	void updateBorder(int width);
	void changeMargin (int);
	void changeAxesLinewidth (int);
	void drawAxesBackbones (bool);
	void pickBackgroundColor();
	void pickCanvasColor();
	void showGeneralPage();
	void showAxesPage();
	void showGridPage();
	void showFormulaBox();
	void showAxisFormula(int axis);

	void addIndex();
	void addExp();
	void addUnderline();
	void addItalic();
	void addBold();
	void addCurve();

	void showLowerGreek();
	void showUpperGreek();
	void addSymbol(const QString& letter);
	void customAxisLabelFont();

signals:
	void updateAxisTitle(int,const QString&);
	void changeAxisFont(int, const QFont &);
	void showAxis(int, int, const QString&, bool, int, int, bool,
				  const QColor&, int, int, int, int, const QString&);	

protected:
	QStringList titles,scales,axesColors, tickLabelsOn, formatInfo;
	QStringList	labelsNumericFormat, tablesList;
	QList<int> majTicks, minTicks, axesType, axesBaseline;
	QFont xBottomFont, yLeftFont, xTopFont, yRightFont;
	GridOptions grid;
	bool xAxisOn,yAxisOn,topAxisOn,rightAxisOn;
	int xBottomLabelsRotation, xTopLabelsRotation;
	MultiLayer *mPlot;
	Graph* d_graph;
};

#endif
