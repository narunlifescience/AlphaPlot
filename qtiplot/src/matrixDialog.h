#ifndef MATRIXDIALOG_H
#define MATRIXDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QSpinBox;
class QComboBox;
	
class matrixDialog : public QDialog
{
    Q_OBJECT

public:
    matrixDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~matrixDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel, *buttonApply;
	QSpinBox* boxColWidth, *boxPrecision; 
	QComboBox *boxFormat, *boxNumericDisplay;

public slots:
    virtual void languageChange();
	void accept();
	void apply();
	void setColumnsWidth(int width);
	void showPrecisionBox(int item);
	void setTextFormat(const QString& format, int precision);
	void changePrecision(int precision);

signals:
	void changeColumnsWidth(int);
	void changeTextFormat(const QChar&, int);
};

#endif // IMPORTDIALOG_H
