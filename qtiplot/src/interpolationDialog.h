#ifndef INTERPOLATIONDIALOG_H
#define INTERPOLATIONDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QLineEdit;
class QComboBox;
class QSpinBox;
class Graph;
class ColorBox;
	
class interpolationDialog : public QDialog
{
    Q_OBJECT

public:
    interpolationDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~interpolationDialog();

	QPushButton* buttonFit;
	QPushButton* buttonCancel;
	QComboBox* boxName;
	QComboBox* boxMethod;
	QSpinBox* boxPoints;
	QLineEdit* boxStart;
	QLineEdit* boxEnd;
	ColorBox* boxColor;

public slots:
	void activateCurve(int index);
	void setGraph(Graph *g);
    virtual void languageChange();
	void interpolate();
	void changeDataRange();

private:
	Graph *graph;
};

#endif



