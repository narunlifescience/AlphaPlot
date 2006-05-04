#ifndef POLINOMFITDIALOG_H
#define POLINOMFITDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QSpinBox;
class Graph;
class ColorBox;
	
class polynomFitDialog : public QDialog
{
    Q_OBJECT

public:
    polynomFitDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~polynomFitDialog();

	QPushButton* buttonFit;
	QPushButton* buttonCancel;
    QButtonGroup* GroupBox1, *GroupBox2;
    QCheckBox* boxShowFormula;
	QComboBox* boxName;
	QSpinBox* boxOrder;
	QSpinBox* boxPoints;
	QLineEdit* boxStart;
	QLineEdit* boxEnd;
	ColorBox* boxColor;

public slots:
    virtual void languageChange();
	void fit();
	void setGraph(Graph *g);
	void activateCurve(int index);
	void changeCurve(int index);
	void changeDataRange();

private:
	Graph *graph;
};

#endif // EXPORTDIALOG_H
