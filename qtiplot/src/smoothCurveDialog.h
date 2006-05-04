#ifndef SMOOTHDIALOG_H
#define SMOOTHDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QComboBox;
class QSpinBox;
class Graph;
class ColorBox;
	
class smoothCurveDialog : public QDialog
{
    Q_OBJECT

public:
    smoothCurveDialog(int method, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~smoothCurveDialog();

	enum SmoothType{SavitzkyGolay = 1, FFT = 2, Average = 3};

	QPushButton* btnSmooth;
	QPushButton* buttonCancel;
	QComboBox* boxName;
	QSpinBox *boxPointsLeft, *boxPointsRight, *boxOrder;
	ColorBox* boxColor;

public slots:
	void setGraph(Graph *g);
    virtual void languageChange();
	void smooth();
	void activateCurve(int index);

private:
	Graph *graph;
	int smooth_type;
};

#endif



