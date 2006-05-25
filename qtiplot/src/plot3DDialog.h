#ifndef PLOT3DDIALOG_H
#define PLOT3DDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qmemarray.h>
#include <qstring.h>

#include <qwt3d_color.h> 
 
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListBox;
class QListBoxItem;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTabWidget;
class QWidget;
class QStringList;
class QButtonGroup;
class QWidgetStack;
	
using namespace Qwt3D;

class plot3DDialog : public QDialog
{ 
    Q_OBJECT

public:
    plot3DDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~plot3DDialog();

	void initScalesPage();
	void initAxesPage();
	void initTitlePage();
	void initColorsPage();
	void initGeneralPage();

	QSpinBox *boxMeshLineWidth;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel, *buttonMinGreek, *buttonMajGreek;
	QPushButton* btnTitleColor, *btnTitleFont, *btnLabelFont, *btnGrid;
	QPushButton *btnBackground, *btnMesh, *btnAxes, *btnTitle, *btnLabels, *btnNumbers;
	QPushButton *btnNumbersFont, *btnFromColor, *btnToColor, *btnTable;
	QPushButton *buttonAxisMinGreek, *buttonAxisMajGreek;
    QTabWidget* generalDialog;
	QWidget *scale, *colors, *general, *axes, *title, *bars, *points;
	QLineEdit *boxTitle, *boxFrom, *boxTo, *boxLabel;
	QSpinBox *boxMajors, *boxMinors;
	QButtonGroup *GroupBox1, *GroupBox2, *GroupBox3, *GroupBox4;
	QButtonGroup *GroupBox5, *GroupBox6, *GroupBox8, *TicksGroupBox;
	QButtonGroup *GroupBox10, *GroupBox7;
	QSpinBox *boxResolution, *boxDistance, *boxTransparency;
	QCheckBox *boxLegend, *boxSmooth, *boxBoxed, *boxCrossSmooth;
	QListBox *axesList, *axesList2;
	QComboBox *boxType, *boxPointStyle;
	QLineEdit *boxMajorLength, *boxMinorLength, *boxConesRad;
	QSpinBox *boxZoom, *boxXScale, *boxYScale, *boxZScale, *boxQuality;
	QLineEdit *boxSize, *boxBarsRad, *boxCrossRad, *boxCrossLinewidth;
	QWidgetStack *	optionStack;
	QWidget *dotsPage, *conesPage, *crossPage;
	
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

	void showMinGreek();
	void showMajGreek();
	void addSymbol(const QString& letter);
	void showGeneralTab();

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
	
private:
	QFont titleFont, xAxisFont,yAxisFont,zAxisFont, numbersFont;
	QStringList labels, scales, tickLengths;
	QColor titleColor,meshColor,bgColor, axesColor, numColor,labelColor, gridColor;
	QColor fromColor, toColor; //custom data colors
};

#endif
