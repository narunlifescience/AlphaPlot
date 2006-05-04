#ifndef ANALYSISDIALOG_H
#define ANALYSISDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QHButtonGroup;
	
class analysisDialog : public QDialog
{
    Q_OBJECT

public:
    analysisDialog( QWidget* parent = 0, const QString& text="Curve", const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~analysisDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
    QButtonGroup* GroupBox1;
    QHButtonGroup* GroupBox2;
    QCheckBox* boxShowFormula;
	QComboBox* boxName;

public slots:
    virtual void languageChange();
	void accept();
	void setCurveNames(const QStringList& names);
	void setOperationType(const QString& s){operation=s;};

signals:
	void options(const QString&);
	void analyse(const QString&, const QString&);

private:
	QString operation;
};

#endif 



