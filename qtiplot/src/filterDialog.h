#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QLineEdit;
class QComboBox;
class QCheckBox;
class Graph;
class ColorBox;
	
class filterDialog : public QDialog
{
    Q_OBJECT

public:
    filterDialog(int type, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~filterDialog();

	enum FilterType{LowPass = 1, HighPass = 2, BandPass = 3, BandBlock = 4};

	QPushButton* buttonFilter;
	QPushButton* buttonCancel;
	QComboBox* boxName;
	QCheckBox* boxOffset;
	QLineEdit* boxStart;
	QLineEdit* boxEnd;
	ColorBox* boxColor;

public slots:
	void setGraph(Graph *g);
    virtual void languageChange();
	void filter();

private:
	Graph *graph;
	int filter_type;
};

#endif



