#ifndef SURFACEDIALOG_H
#define SURFACEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QLineEdit;
class QComboBox;
	
class sDialog : public QDialog
{
    Q_OBJECT

public:
    sDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~sDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonClear;
    QButtonGroup* GroupBox1, *GroupBox2, *GroupBox3, *GroupBox4, *GroupBox5, *GroupBox6;
	QComboBox* boxFunction;
	QLineEdit* boxXFrom;
	QLineEdit* boxXTo;
	QLineEdit* boxYFrom;
	QLineEdit* boxYTo;
	QLineEdit* boxZFrom;
	QLineEdit* boxZTo;

public slots:
	void accept();
	void insertFunctionsList(const QStringList& list);
	void clearList();
	void setFunction(const QString& s);
	void setLimits(double xs, double xe, double ys, double ye, double zs, double ze);

signals:
	void options(const QString&,double,double,double,double,double,double);
	void clearFunctionsList();
	void custom3DToolBar();
};

#endif // EXPORTDIALOG_H
