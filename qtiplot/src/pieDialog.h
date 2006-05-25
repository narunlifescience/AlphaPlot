#ifndef PIEDIALOG_H
#define PIEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qmemarray.h>
#include <qstring.h>
 
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
class ColorBox;
class PatternBox;
class ColorButton;
class MultiLayer;

class pieDialog : public QDialog
{ 
    Q_OBJECT

public:
    pieDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~pieDialog();

	void initPiePage();
	void initBorderPage();

    QPushButton* buttonWrk;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QTabWidget* generalDialog;
	QWidget* frame, *pieOptions;

    QListBox *curvesList;
    QSpinBox *boxFrameWidth;
	QSpinBox *boxRay, *boxMargin;
	QCheckBox *boxFramed, *boxAll;
    ColorBox *boxFrameColor, *boxLineColor, *boxFirstColor;
	QComboBox *boxLineStyle;
    PatternBox *boxPattern;
	QSpinBox *boxLineWidth, *boxBorderWidth;

	ColorButton *boxBackgroundColor, *boxBorderColor;

public slots:
	void accept();
	void updatePlot();
	void drawFrame(bool framed);
	void setFramed(bool ok);
	void setFrameColor(const QColor& c);
	void setFrameWidth(int w);
	void insertCurveName(const QString& name);
	void showPopupMenu(QListBoxItem *it, const QPoint &point);
	void removeCurve();
	void setBorderColor(const QColor& c);
	void setBorderWidth(int width);
	void setPieSize(int size);
	void setFirstColor(int c);

	void pickBorderColor();
	void pickBackgroundColor();
	void changeMargin(int width);
	void updateBorder(int width);

	void setMultiLayerPlot(MultiLayer *m);

	Qt::PenStyle style();
	void setBorderStyle(const Qt::PenStyle& style);
	Qt::BrushStyle pattern();
	void setPattern(const Qt::BrushStyle& style);
	void showWorksheet();
	void showGeneralPage();

signals:
	void worksheet(const QString&);
	void drawFrame(bool,int,const QColor&);
	void toggleCurve();
	void updatePie(const QPen&, const Qt::BrushStyle &,int, int);

private:
	MultiLayer *mPlot;
};

#endif // GENDIALOG_H
