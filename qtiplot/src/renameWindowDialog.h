#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

#include "widget.h"

class QButtonGroup;
class QPushButton;
class QLineEdit;
class QRadioButton;
class QTextEdit;
class myWidget;

class renameWindowDialog : public QDialog
{
    Q_OBJECT

public:
    renameWindowDialog(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~renameWindowDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
    QButtonGroup* GroupBox1, *GroupBox2;
	QRadioButton* boxName;
	QRadioButton* boxLabel;
	QRadioButton* boxBoth;
	QLineEdit* boxNameLine;
	QTextEdit* boxLabelEdit;

public slots:
	void setWidget(myWidget *w);
	myWidget::CaptionPolicy getCaptionPolicy();
    virtual void languageChange();
	void accept();

signals:

private:
	myWidget *window;
};

#endif // EXPORTDIALOG_H
