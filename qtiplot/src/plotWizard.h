#ifndef PLOTWIZARD_H
#define PLOTWIZARD_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QListBox;
class QComboBox;

class plotWizard : public QDialog
{
    Q_OBJECT

public:
    plotWizard( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~plotWizard();

    QPushButton* buttonOk,*buttonXErr, *buttonYErr, *buttonDelete;
	QPushButton* buttonCancel, *buttonX, *buttonY, *buttonNew, *buttonZ;
    QButtonGroup* GroupBox1, *GroupBox2, *GroupBox3;
    QComboBox* boxTables;
	QListBox *columnsList, *plotAssociations;

public slots:
	void changeColumnsList(const QString &table);
	void insertTablesList(const QStringList& tables);
	void insertColumnsList(const QStringList& cols);
	void setColumnsList(const QStringList& cols);
	void addCurve();
	void toggleCurve();
	void addXCol();
	void addYCol();
	void addXErrCol();
	void addYErrCol();
	void accept();
	bool noCurves();
	void addZCol();

signals:
	void plot (const QStringList&);
	void plot3D (const QString&);
	void plot3DRibbon (const QString&);

private:
	QStringList columns;
};

#endif // EXPORTDIALOG_H
