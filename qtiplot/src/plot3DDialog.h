/***************************************************************************
    File                 : plot3DDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Surface plot options dialog
                           
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
#ifndef PLOT3DDIALOG_H
#define PLOT3DDIALOG_H

#include "graph3D.h"
#include <QCheckBox>

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTabWidget;
class QWidget;
class QStringList;
class QStackedWidget;
	
using namespace Qwt3D;

//! Surface plot options dialog
class Plot3DDialog : public QDialog
{ 
    Q_OBJECT

public:
    Plot3DDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~Plot3DDialog();
	
public slots:
	void accept();
	bool updatePlot();

	QColor pickFromColor();
	QColor pickToColor();
	QColor pickMeshColor();
	QColor pickAxesColor();
	QColor pickTitleColor();
	QColor pickNumberColor();
	QColor pickBgColor();
	QColor pickLabelColor();
	QColor pickGridColor();

	void setColors(const QColor& title, const QColor& mesh,const QColor& axes,const QColor& num,
							 const QColor& label,const QColor& bg,const QColor& grid);
	void setDataColors(const QColor& minColor, const QColor& maxColor);
	void setTransparency(double t);
	void pickTitleFont();
	void setTitle(const QString& title);
	void setTitleFont(const QFont& font);

	void setResolution(int r);
	void showLegend(bool show);
	void setOrthogonal(bool on){boxOrthogonal->setChecked(on);};
	
	void setAxesLabels(const QStringList& list);
	void viewAxisOptions(int axis);

	void setAxesFonts(const QFont& xf, const QFont& yf, const QFont& zf);
	QFont axisFont(int axis);
	void pickAxisLabelFont();

	void pickNumbersFont();
	void setNumbersFonts(const QFont& f){numbersFont=f;};

	QStringList scaleOptions(int axis, double start, double end, 
												const QString& majors, const QString& minors);
	void setScales(const QStringList& list);
	void viewScaleLimits(int axis);

	void setAxesTickLengths(const QStringList& list);
	void setMeshLineWidth(double lw);
	void setLabelsDistance(int dist);

	void disableAxesOptions();
	void disableMeshOptions();
	void disableLegend();
	void disableGridOptions();
	void showBarsTab(double rad);
	void showPointsTab(double rad, bool smooth);
	void showConesTab(double rad, int quality);
	void showCrossHairTab(double rad, double linewidth, bool smooth, bool boxed);
	
	void customWorksheetBtn(const QString& text);
	void worksheet();
	void setZoom(double zoom);
	void setScaling(double xVal, double yVal, double zVal);
	void showTitleTab();
	void showAxisTab();
	
	void initPointsOptionsStack();
	void changeZoom(int);
	void changeTransparency(int val);

	void showLowerGreek();
	void showUpperGreek();
	void addSymbol(const QString& letter);
	void showGeneralTab();

    void pickDataColorMap();
    void setPlot(Graph3D *plot){d_plot = plot;};
    
signals:
	void showWorksheet();
	void updatePoints(double, bool);
	void updateBars(double);
	void updateTransparency(double);
	void updateDataColors(const QColor&,const QColor&);
	void updateColors(const QColor&,const QColor&,const QColor&,const QColor&,const QColor&,const QColor&);
	void updateTitle(const QString&,const QColor&,const QFont&);
	void updateResolution(int);
	void showColorLegend(bool);
	void setOrtho(bool);
	void updateLabel(int,const QString&, const QFont&);
	void updateScale(int,const QStringList&);
	void updateTickLength(int,double, double);
	void adjustLabels (int);
	void setNumbersFont(const QFont&);
	void updateMeshLineWidth(int);
	void updateZoom(double);
	void updateScaling(double, double, double);
	void updateCones(double, int);
	void updateCross(double, double, bool, bool);
	void setDataColorMap(const QString&);
	
private:
    void initScalesPage();
	void initAxesPage();
	void initTitlePage();
	void initColorsPage();
	void initGeneralPage();

    Graph3D *d_plot;
	QFont titleFont, xAxisFont,yAxisFont,zAxisFont, numbersFont;
	QStringList labels, scales, tickLengths;
	QColor titleColor,meshColor,bgColor, axesColor, numColor,labelColor, gridColor;
	QColor fromColor, toColor; //custom data colors

    QSpinBox *boxMeshLineWidth;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel, *buttonLowerGreek, *buttonUpperGreek;
	QPushButton* btnTitleColor, *btnTitleFont, *btnLabelFont, *btnGrid;
	QPushButton *btnBackground, *btnMesh, *btnAxes, *btnTitle, *btnLabels, *btnNumbers;
	QPushButton *btnNumbersFont, *btnFromColor, *btnToColor, *btnTable, *btnColorMap;
	QPushButton *buttonAxisLowerGreek, *buttonAxisUpperGreek;
    QTabWidget* generalDialog;
	QWidget *scale, *colors, *general, *axes, *title, *bars, *points;
	QLineEdit *boxTitle, *boxFrom, *boxTo, *boxLabel;
	QSpinBox *boxMajors, *boxMinors;
	QGroupBox *TicksGroupBox, *AxesColorGroupBox;
	QSpinBox *boxResolution, *boxDistance, *boxTransparency;
	QCheckBox *boxLegend, *boxSmooth, *boxBoxed, *boxCrossSmooth, *boxOrthogonal;
	QListWidget *axesList, *axesList2;
	QComboBox *boxType, *boxPointStyle;
	QLineEdit *boxMajorLength, *boxMinorLength, *boxConesRad;
	QSpinBox *boxZoom, *boxXScale, *boxYScale, *boxZScale, *boxQuality;
	QLineEdit *boxSize, *boxBarsRad, *boxCrossRad, *boxCrossLinewidth;
	QStackedWidget *optionStack;
	QWidget *dotsPage, *conesPage, *crossPage;
};

#endif
