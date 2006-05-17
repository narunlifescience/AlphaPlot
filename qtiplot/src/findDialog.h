#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QCheckBox;
class QComboBox;
class QLabel;

class findDialog : public QDialog
{
    Q_OBJECT

public:
    findDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~findDialog();

    QPushButton* buttonFind;
	QPushButton* buttonCancel;
	QPushButton* buttonReset;

	QLabel *labelStart;
	QComboBox* boxFind;

    QCheckBox* boxWindowNames;
    QCheckBox* boxWindowLabels;
	QCheckBox* boxFolderNames;

	QCheckBox* boxCaseSensitive;
    QCheckBox* boxPartialMatch;
	QCheckBox* boxSubfolders;

public slots:

	//! Displays the project current folder path 
	void setStartPath();

protected slots:

	void accept();
};

#endif // exportDialog_H
