#ifndef MVALUESDIALOG_H
#define MVALUESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QComboBox;
class QTextEdit;
class QSpinBox;
class QPushButton;
	
class matrixValuesDialog : public QDialog
{ 
    Q_OBJECT

public:
    matrixValuesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~matrixValuesDialog();
	
	QSize sizeHint() const ;

    QComboBox* functions;
    QPushButton* PushButton3; 
    QPushButton* btnOk, *btnAddCell;
    QPushButton* btnCancel;
    QTextEdit* commandes;
    QTextEdit* explain;
	QSpinBox *startRow, *endRow, *startCol, *endCol;
	QPushButton *btnApply;

public slots:
	void accept();
	bool apply();
	void setFunctions();
	void addCell();
	void insertFunction();
	void insertExplain(int index);
	void setColumns(int c);
	void setRows(int r);
	void setFormula(const QString& s);

signals:
	void setValues (const QString&, const QString&, const QStringList&, 
			   const QStringList&, int, int, int, int);
};

#endif //
