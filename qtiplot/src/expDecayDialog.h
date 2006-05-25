#ifndef EXPDECAYDIALOG_H
#define EXPDECAYDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QLineEdit;
class QComboBox;
class QLabel;
class Graph;
class ColorBox;
	
class expDecayDialog : public QDialog
{
    Q_OBJECT

public:
    expDecayDialog( int type, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~expDecayDialog();

	QPushButton* buttonFit;
	QPushButton* buttonCancel;
    QButtonGroup* GroupBox1, *GroupBox2;
	QComboBox* boxName;
	QLineEdit* boxFirst;
	QLineEdit* boxSecond;
	QLineEdit* boxThird;
	QLineEdit* boxStart;
	QLineEdit* boxYOffset;
	QLabel* thirdLabel, *dampingLabel;
	ColorBox* boxColor;

public slots:
    virtual void languageChange();
	void fit();
	void setGraph(Graph *g);

signals:
	void options(const QString&,double,double,double,double,int);
	void options(const QString&, double,double,double,int);
	void options3(const QString&,double,double,double,double,double,int);

private:
	Graph *graph;
	int slopes;
};

#endif // EXPORTDIALOG_H
