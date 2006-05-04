#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QComboBox;
class QSpinBox;
class QCheckBox;
class QLabel;
	
class importDialog : public QDialog
{
    Q_OBJECT

public:
    importDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~importDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonHelp;
    QButtonGroup* GroupBox1, *GroupBox2;
    QComboBox* boxSeparator;
	QSpinBox* boxLines;
	QCheckBox *boxRenameCols, *boxSimplifySpaces, *boxStripSpaces;
	QLabel *ignoreLabel,*sepText;

public slots:
    virtual void languageChange();
	void setSeparator(const QString& sep);
	void setWhiteSpaceOptions(bool strip, bool simplify);
	void setLines(int lines);
	void renameCols(bool rename);
	void accept();
	void help();

signals:
	void options(const QString&, int, bool, bool, bool);
};

#endif // IMPORTDIALOG_H
