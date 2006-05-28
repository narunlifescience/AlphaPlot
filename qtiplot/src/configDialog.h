/***************************************************************************
    File                 : configDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Configuration dialog
                           
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
#ifndef configDialog_H
#define configDialog_H

#include <qvariant.h>
#include <qdialog.h>
#include <q3hbox.h> 
#include <qlayout.h>
//Added by qt3to4:
#include <QLabel>

class Q3ButtonGroup;
class QPushButton;
class Q3WidgetStack;
class QWidget;
class QCheckBox;
class QComboBox;
class QSpinBox;
class Q3ListBox;
class QLabel;
class ColorButton;
class Q3GroupBox;
	
//! Configuration dialog
class configDialog : public QDialog
{
    Q_OBJECT

public:
    configDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~configDialog();

	void initPlotsPage();
	void initAppPage();
	void initCurvesPage();
	void initPlots3DPage();
	void initTablesPage();

	QPushButton *btnBackground3D, *btnMesh, *btnAxes, *btnLabels, *btnNumbers;
	QPushButton *btnFromColor, *btnToColor, *btnGrid;
	QPushButton	*btnTitleFnt, *btnLabelsFnt, *btnNumFnt;

	ColorButton *buttonBackground, *buttonText, *buttonHeader;
    QPushButton *buttonOk, *buttonCancel, *buttonApply;
	QPushButton* buttonTextFont, *buttonHeaderFont;
    Q3ButtonGroup* GroupBox1, *GroupBox2, *GroupBox3;
    Q3WidgetStack* generalDialog;
	QWidget *tables, *plots, *confirm, *application, *curves, *plots3D;
	QPushButton* buttonAxesFont, *buttonNumbersFont, *buttonLegendFont, *buttonTitleFont, *fontsBtn;
	QCheckBox* boxTitle, *boxFrame, *boxPlots3D, *boxPlots2D, *boxTables, *boxNotes, *boxFolders;
	QCheckBox *boxSave, *boxBackbones, *boxAllAxes, *boxShowLegend, *boxSmoothMesh;
	QCheckBox *boxAutoscaling, *boxShowProjection, *boxMatrixes, *boxScaleFonts, *boxResize;
	QComboBox *boxLegend, *boxTicks, *boxStyle, *boxCurveStyle, *boxSeparator, *boxLanguage;
	QSpinBox *boxMinutes, *boxLinewidth, *boxFrameWidth, *boxResolution, *boxMargin;
	QSpinBox *boxCurveLineWidth, *boxSymbolSize, *boxMinorTicks, *boxMajorTicks;
	ColorButton *btnWorkspace, *btnPanels, *btnPanelsText;
	Q3ListBox *itemsList;
	QLabel *labelFrameWidth, *lblLanguage, *lblWorkspace, *lblPanels;
	QLabel *lblPanelsText, *lblFonts, *lblStyle;
	Q3GroupBox *GroupBoxConfirm, *GroupBoxAppCol, *GroupBoxApp;
	Q3GroupBox *GroupBoxTableFonts, *GroupBoxTableCol;
	QLabel *lblSeparator, *lblTableBackground, *lblTextColor, *lblHeaderColor;
	QLabel *lblSymbSize, *lblLineWidth, *lblCurveStyle, *lblResolution;
	Q3GroupBox *GroupBox3DFonts, *GroupBox3DCol, *GroupBoxOptions, *GroupBox2DFonts;
	QLabel *lblLegend, *lblMargin, *lblTicks, *lblMajTicks, *lblLinewidth, *lblMinTicks;

public slots:
    virtual void languageChange();
	void insertLanguagesList();

	void accept();
	void update();

	void setColumnSeparator(const QString& sep);
	//table colors
	void pickBgColor();
	void pickTextColor();
	void pickHeaderColor();

	//table fonts
	void pickTextFont();
	void pickHeaderFont();

	//graph fonts
	void pickAxesFont();
	void pickNumbersFont();
	void pickLegendFont();
	void pickTitleFont();

	void enableScaleFonts();
	int ticksType();
	void showFrameWidth(bool ok);

	//application
	void pickApplicationFont();
	void pickPanelsTextColor();
	void pickPanelsColor();
	void pickWorkspaceColor();
	
	//2D curves
	int curveStyle();
	void initCurvesOptions(int style, int width, int symbolSize);

	void pickDataMaxColor();
	void pickDataMinColor();
	void pick3DBackgroundColor();
	void pickMeshColor();
	void pickGridColor();
	void pick3DAxesColor();
	void pick3DNumbersColor();
	void pick3DLabelsColor();
	void pick3DTitleFont();
	void pick3DNumbersFont();
	void pick3DAxesFont();

	void switchToLanguage(int param);

private:
	QFont textFont, headerFont, axesFont, numbersFont, legendFont, titleFont, appFont;
	QFont plot3DTitleFont, plot3DNumbersFont, plot3DAxesFont;
	QStringList plot3DColors;
};

#endif // configDialog_H
