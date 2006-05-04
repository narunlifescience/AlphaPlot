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
class QHBox;

class layerDialog : public QDialog
{
    Q_OBJECT

public:
    layerDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~layerDialog();

	void setColumns(int c);
	void setRows(int r);
	void setGraphsNumber(int g);
	void setSpacing(int colsGap, int spaceGap);
	void initFonts(const QFont& titlefont, const QFont& axesfont, const QFont& numbersfont, const QFont& legendfont);

    QPushButton* buttonOk, *btnTitle, *btnAxisLegend, *btnAxisNumbers, *btnLegend;
	QPushButton* buttonCancel;
	QPushButton* buttonApply;
    QButtonGroup* GroupBox1, *GroupBox2, *GroupBox4;
    QSpinBox* boxX, *boxY, *boxColsGap, *boxRowsGap;
	QCheckBox *fitBox;
	QTabWidget* generalDialog;
	QWidget* layout, *fonts;
	
	QHBox *optionsBox;

protected slots:
    virtual void languageChange();
	void accept();
	void update();
	void setTitlesFont();
	void setAxisLegendFont();
	void setAxisNumbersFont();
	void setLegendsFont();
	void enableLayoutOptions(bool ok);

signals:
	void options(int, int, int, int, bool);
	void setFonts(const QFont&, const QFont&, const QFont&, const QFont&);

private:
	int	graphs;
	QFont titleFont, legendFont, axesFont, numbersFont;
};

#endif
