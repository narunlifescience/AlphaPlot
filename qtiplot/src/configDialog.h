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
#ifndef ConfigDialog_H
#define ConfigDialog_H

#include <QDialog>

class QGroupBox;
class QPushButton;
class QTabWidget;
class QStackedWidget;
class QWidget;
class QCheckBox;
class QComboBox;
class QSpinBox;
class QLabel;
class ColorButton;
class QListWidget;

// uncomment this to make this a tab dialog
//#define TABBED_CONFIG_DIALOG
	
//! Configuration dialog
class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
	//! Constructor
	/**
	 * \param parent parent widget (must be the application window!=
	 * \param fl window flags
	 */
    ConfigDialog( QWidget* parent, Qt::WFlags fl = 0 );
	//! Destructor
    ~ConfigDialog();

	QPushButton *btnBackground3D, *btnMesh, *btnAxes, *btnLabels, *btnNumbers;
	QPushButton *btnFromColor, *btnToColor, *btnGrid;
	QPushButton	*btnTitleFnt, *btnLabelsFnt, *btnNumFnt;

	ColorButton *buttonBackground, *buttonText, *buttonHeader;
    QPushButton *buttonOk, *buttonCancel, *buttonApply;
	QPushButton* buttonTextFont, *buttonHeaderFont;
#ifdef TABBED_CONFIG_DIALOG
    QTabWidget * generalDialog;
#else
	QStackedWidget * generalDialog;
#endif
	QWidget *tables, *plots, *confirm, *application, *curves, *plots3D;
	QPushButton* buttonAxesFont, *buttonNumbersFont, *buttonLegendFont, *buttonTitleFont, *fontsBtn;
	QCheckBox* boxTitle, *boxFrame, *boxPlots3D, *boxPlots2D, *boxTables, *boxNotes, *boxFolders;
	QCheckBox *boxSave, *boxBackbones, *boxAllAxes, *boxShowLegend, *boxSmoothMesh;
	QCheckBox *boxAutoscaling, *boxShowProjection, *boxMatrices, *boxScaleFonts, *boxResize;
	QComboBox *boxLegend, *boxTicks, *boxStyle, *boxCurveStyle, *boxSeparator, *boxLanguage;
	QSpinBox *boxMinutes, *boxLineWidth, *boxFrameWidth, *boxResolution, *boxMargin;
	QSpinBox *boxCurveLineWidth, *boxSymbolSize, *boxMinorTicks, *boxMajorTicks;
	ColorButton *btnWorkspace, *btnPanels, *btnPanelsText;
#ifndef TABBED_CONFIG_DIALOG
	QListWidget * itemsList;
#else
	QWidget * lastTab;
#endif
	QLabel *labelFrameWidth, *lblLanguage, *lblWorkspace, *lblPanels;
	QLabel *lblPanelsText, *lblFonts, *lblStyle;
	QGroupBox *groupBoxConfirm, *groupBoxAppCol, *groupBoxApp;
	QGroupBox *groupBoxTableFonts, *groupBoxTableCol;
	QLabel *lblSeparator, *lblTableBackground, *lblTextColor, *lblHeaderColor;
	QLabel *lblSymbSize, *lblAxesLineWidth, *lblCurveStyle, *lblResolution;
	QGroupBox *groupBox3DFonts, *groupBox3DCol, *groupBoxOptions, *groupBox2DFonts;
	QLabel *lblLegend, *lblMargin, *lblTicks, *lblMajTicks, *lblLineWidth, *lblMinTicks;

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
	void initPlotsPage();
	void initAppPage();
	void initCurvesPage();
	void initPlots3DPage();
	void initTablesPage();
	void initConfirmationsPage();

	QFont textFont, headerFont, axesFont, numbersFont, legendFont, titleFont, appFont;
	QFont plot3DTitleFont, plot3DNumbersFont, plot3DAxesFont;
	QStringList plot3DColors;
};

#endif // ConfigDialog_H
