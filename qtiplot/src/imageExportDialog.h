#ifndef IMEXPORTDIALOG_H
#define IMEXPORTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QCheckBox;
class QSpinBox;
class QComboBox;
class QLabel;
	
class imageExportDialog : public QDialog
{
    Q_OBJECT

public:
    imageExportDialog( bool exportAllPlots, QWidget* parent = 0, 
									const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~imageExportDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
    QButtonGroup* GroupBox1, *GroupBox2;
    QCheckBox* boxTransparency;
	QSpinBox * boxQuality;
	QLabel *formatLabel;
	QComboBox *boxFormat;
	
public slots:
	void enableTransparency();
	void enableTransparency(int);
	void setExportPath(const QString& fname, const QString& fileType)
			{f_name = fname; f_type = fileType;};
			
	void setExportDirPath(const QString& dir) {f_dir = dir;};

protected slots:
    virtual void languageChange();
	void accept();

signals:
	void options(const QString&, const QString&, int, bool);
	void exportAll(const QString&, const QString&, int, bool);

private:
	QString f_name, f_type, f_dir;
	bool expAll;
};

#endif // imageExportDialog_H
