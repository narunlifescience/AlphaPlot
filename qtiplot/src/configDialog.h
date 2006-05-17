#ifndef configDialog_H
#define configDialog_H

#include <qvariant.h>
#include <qdialog.h>
#include <qhbox.h> 
#include <qlayout.h>

class QButtonGroup;
class QPushButton;
class QWidgetStack;
class QWidget;
class QCheckBox;
class QComboBox;
class QSpinBox;
class QListBox;
class QLabel;
class ColorButton;
class QGroupBox;
	
class configDialog : public QDialog
{
    Q_OBJECT

public:
    configDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
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
    QButtonGroup* GroupBox1, *GroupBox2, *GroupBox3;
    QWidgetStack* generalDialog;
	QWidget *tables, *plots, *confirm, *application, *curves, *plots3D;
	QPushButton* buttonAxesFont, *buttonNumbersFont, *buttonLegendFont, *buttonTitleFont, *fontsBtn;
	QCheckBox* boxTitle, *boxFrame, *boxPlots3D, *boxPlots2D, *boxTables, *boxNotes, *boxFolders;
	QCheckBox *boxSave, *boxBackbones, *boxAllAxes, *boxShowLegend, *boxSmoothMesh;
	QCheckBox *boxAutoscaling, *boxShowProjection, *boxMatrixes, *boxScaleFonts, *boxResize;
	QComboBox *boxLegend, *boxTicks, *boxStyle, *boxCurveStyle, *boxSeparator, *boxLanguage;
	QSpinBox *boxMinutes, *boxLinewidth, *boxFrameWidth, *boxResolution, *boxMargin;
	QSpinBox *boxCurveLineWidth, *boxSymbolSize, *boxMinorTicks, *boxMajorTicks;
	ColorButton *btnWorkspace, *btnPanels, *btnPanelsText;
	QListBox *itemsList;
	QLabel *labelFrameWidth, *lblLanguage, *lblWorkspace, *lblPanels;
	QLabel *lblPanelsText, *lblFonts, *lblStyle;
	QGroupBox *GroupBoxConfirm, *GroupBoxAppCol, *GroupBoxApp;
	QGroupBox *GroupBoxTableFonts, *GroupBoxTableCol;
	QLabel *lblSeparator, *lblTableBackground, *lblTextColor, *lblHeaderColor;
	QLabel *lblSymbSize, *lblLineWidth, *lblCurveStyle, *lblResolution;
	QGroupBox *GroupBox3DFonts, *GroupBox3DCol, *GroupBoxOptions, *GroupBox2DFonts;
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
