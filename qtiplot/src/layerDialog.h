#ifndef LAYERDIALOG_H
#define LAYERDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QTabWidget;
class QWidget;
class QComboBox;
class MultiLayer;

class layerDialog : public QDialog
{
    Q_OBJECT

public:
    layerDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~layerDialog();

	void setMultiLayer(MultiLayer *g);
	void initFonts(const QFont& titlefont, const QFont& axesfont, 
				   const QFont& numbersfont, const QFont& legendfont);

    QPushButton* buttonOk, *btnTitle, *btnAxisLegend, *btnAxisNumbers, *btnLegend;
	QPushButton* buttonCancel;
	QPushButton* buttonApply;
    QButtonGroup* GroupBox1, *GroupBox2, *GroupBox4, *GroupCanvasSize;
    QSpinBox *boxX, *boxY, *boxColsGap, *boxRowsGap;
	QSpinBox *boxRightSpace, *boxLeftSpace, *boxTopSpace, *boxBottomSpace;
	QSpinBox *boxCanvasWidth, *boxCanvasHeight, *layersBox;
	QCheckBox *fitBox;
	QTabWidget* generalDialog;
	QWidget* layout, *fonts;	
	QComboBox *alignHorBox, *alignVertBox;

protected slots:
    virtual void languageChange();
	void accept();
	void update();
	void setTitlesFont();
	void setAxisLegendFont();
	void setAxisNumbersFont();
	void setLegendsFont();
	void enableLayoutOptions(bool ok);

private:
	QFont titleFont, legendFont, axesFont, numbersFont;
	MultiLayer *multi_layer;
};

#endif
