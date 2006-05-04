#ifndef FFTDIALOG_H
#define FFTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QRadioButton;
class QLineEdit;
class QComboBox;
class QCheckBox;
class Graph;
class Table;
	
class FFTDialog : public QDialog
{
    Q_OBJECT

public:
	enum DataType{onGraph = 0, onTable = 1};

    FFTDialog(int type, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FFTDialog();

	QPushButton* buttonOK;
	QPushButton* buttonCancel;
	QRadioButton *forwardBtn, *backwardBtn;
	QComboBox* boxName, *boxReal, *boxImaginary;
	QLineEdit* boxSampling;
	QCheckBox* boxNormalize, *boxOrder;

public slots:
	void setGraph(Graph *g);
	void setTable(Table *t);
	void activateCurve(int index);
    virtual void languageChange();
	void accept();

private:
	Graph *graph;
	Table *d_table;
	int d_type;
};

#endif



