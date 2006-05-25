#ifndef matrixSizeDialog_H
#define matrixSizeDialog_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QSpinBox;
class QLineEdit;
	
class matrixSizeDialog : public QDialog
{
    Q_OBJECT

public:
    matrixSizeDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~matrixSizeDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
    QButtonGroup* GroupBox1;
	QSpinBox *boxCols, *boxRows;
	QLineEdit *boxXStart, *boxYStart, *boxXEnd, *boxYEnd;

public slots:
    virtual void languageChange();
	void accept();
	void setColumns(int c);
	void setRows(int r);
	void setCoordinates(double xs, double xe, double ys, double ye);

signals:
	void changeDimensions(int, int);
	void changeCoordinates(double, double, double, double);
};

#endif // 
