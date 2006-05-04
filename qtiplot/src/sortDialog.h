#ifndef SORTDIALOG_H
#define SORTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qvaluelist.h> 

class QButtonGroup;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QComboBox;
	
class sortDialog : public QDialog
{
    Q_OBJECT

public:
    sortDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~sortDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonHelp;
    QButtonGroup* GroupBox1, *GroupBox2;
	QComboBox* boxType;
	QComboBox* boxOrder;
	QComboBox *columnsList;
	

public slots:
    virtual void languageChange();
	void accept();
	void insertColumnsList(const QStringList& cols);
	void changeType(int index);

signals:
	void sort(int, int, const QString&);

};

#endif // EXPORTDIALOG_H
