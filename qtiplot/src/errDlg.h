#ifndef ERRDIALOG_H
#define ERRDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QLabel;
class QComboBox;
class QGroupBox;
class QRadioButton;
class QLineEdit;
class QPushButton;
class QButtonGroup;
	
class errDialog : public QDialog
{
    Q_OBJECT

public:
    errDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~errDialog();

    QLabel* TextLabel1;
    QComboBox* nameLabel, *tableNamesBox, *colNamesBox;
    QGroupBox *GroupBox2;
	QButtonGroup *GroupBox1, *GroupBox3;
    QRadioButton* standardBox, *columnBox;
    QRadioButton* percentBox;
    QLineEdit* valueBox;
    QRadioButton* xErrBox;
    QRadioButton* yErrBox;
	QPushButton* buttonAdd;
    QPushButton* buttonCancel;

protected slots:
    virtual void languageChange();

public slots:
	void add();
	void setCurveNames(const QStringList& names);
	void setSrcTables(QWidgetList* tables);
	void selectSrcTable(int tabnr);

signals:
	void options(const QString&,int, const QString&,int);
	void options(const QString&, const QString&, int);

private:
	QWidgetList *srcTables;
};

#endif // ERRDIALOG_H
