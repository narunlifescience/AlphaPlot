#ifndef AXESDIALOG_H
#define AXESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QTextEdit;
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
class QGroupBox;
class ColorBox;
class ColorButton;
class MultiLayer;
class Graph;

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
}  gridOptions;

class axesDialog : public QDialog
{ 
    Q_OBJECT

public:
    axesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~axesDialog();

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
    QListBox* axesList;
    QWidget* gridPage;
    QCheckBox* boxMajorGrid;
    QCheckBox* boxMinorGrid;
    QComboBox* boxTypeMajor;
    ColorBox* boxColorMinor;
    ColorBox* boxColorMajor;
    QSpinBox* boxWidthMajor;
    QComboBox* boxTypeMinor;
    QSpinBox* boxWidthMinor;
    QCheckBox* boxXLine;
    QCheckBox* boxYLine;
    QListBox* axesGridList;
    QWidget* axesPage, *frame;
    QListBox* axesTitlesList;
    QCheckBox* boxShowAxis, *boxShowLabels;
	
	QTextEdit *boxFormula, *boxTitle;
	QSpinBox *boxFrameWidth, *boxPrecision, *boxAngle, *boxBaseline, *boxAxesLinewidth;
    QPushButton* btnAxesFont;
	QCheckBox *boxBackbones, *boxAll, *boxShowFormula;
	ColorButton* boxAxisColor;
	QComboBox* boxTicksType, *boxFormat, *boxAxisType, *boxColName;
	QButtonGroup* GroupBox0;
	QButtonGroup* GroupBox1;
	QButtonGroup* GroupBox2;
	QButtonGroup* GroupBox3;
	QButtonGroup *boxFramed, *GroupBox6;
	QLabel *label1, *label2, *label3, *boxScaleTypeLabel, *minorBoxLabel, *labelTable;
	QSpinBox *boxMajorTicksLength, *boxMinorTicksLength, *boxBorderWidth, *boxMargin;
	QComboBox *boxUnit, *boxTableName;
	ColorButton *boxBorderColor, *boxFrameColor, *boxBackgroundColor;
	QGroupBox  *labelBox;
	QPushButton *buttonIndice, *buttonExp, *buttonSym, *buttonB, *buttonI;
    QPushButton *buttonU, *buttonMinGreek, *buttonMajGreek, *btnLabelFont;
	
public slots:
	QStringList scaleLimits(int axis, double start, double end, double step, 
							const QString& majors, const QString&minors);
	void setAxesType(const QValueList<int>& list);
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
	gridOptions getGridOptions();
	void putGridOptions(gridOptions gr);
	void setGridOptions();
	void tabPageChanged(QWidget *w);
	void accept();
	void customAxisFont();
	void showAxis();
	void updateShowBox(int axis);
	void setEnabledAxes(QMemArray<bool> ok);
	void drawFrame(bool framed);

	void pickAxisColor();
	void setAxisColor(const QColor& c);
	void updateAxisColor(int);
	void setAxesColors(const QStringList& colors);
	int mapToQwtAxisId();
	void setEnabledTickLabels(const QStringList& labelsOn);
	void updateTickLabelsList();
	void setTicksType(const QValueList<int>& list);
	void setTicksType(int);
	void setCurrentScale(int axisPos);
	void initAxisFonts(const QFont& xB, const QFont& yL, const QFont& xT, const QFont& yR );
	int ticksType();
	void updateTicksType(int);
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
	void setAxesBaseline(const QValueList<int>& lst);
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

	void showMinGreek();
	void showMajGreek();
	void addSymbol(const QString& letter);
	void customAxisLabelFont();

signals:
	void updateAxisTitle(int,const QString&);
	void changeAxisFont(int, const QFont &);
	void showAxis(int, int, const QString&, bool, int, bool,
				  const QColor&, int, int, int, int, const QString&);	

protected:
	QStringList titles,scales,axesColors, tickLabelsOn, formatInfo, labelsNumericFormat, tablesList;
	QValueList<int> ticks, axesType, axesBaseline;
	QFont xBottomFont, yLeftFont, xTopFont, yRightFont;
	gridOptions grid;
	bool xAxisOn,yAxisOn,topAxisOn,rightAxisOn;
	int xBottomLabelsRotation, xTopLabelsRotation;
	MultiLayer *mPlot;
	Graph* d_graph;
};

#endif
