#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QCheckBox;
class QComboBox;

class exportDialog : public QDialog
{
    Q_OBJECT

public:
    exportDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~exportDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonHelp;
    QCheckBox* boxNames;
    QCheckBox* boxSelection;
	QCheckBox* boxAllTables;
    QComboBox* boxSeparator;
	QComboBox* boxTable;

public slots:
	void setColumnSeparator(const QString& sep);
	void setTableNames(const QStringList& names);
	void setActiveTableName(const QString& name);
	void enableTableName(bool ok);

protected slots:
    virtual void languageChange();
	void accept();
	void help();

signals:
	void exportTable(const QString&, const QString&, bool, bool);
	void exportAllTables(const QString&, bool, bool);

};

#endif // exportDialog_H
