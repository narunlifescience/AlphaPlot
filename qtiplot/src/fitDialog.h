#ifndef FITDIALOG_H
#define FITDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QLineEdit;
class QComboBox;
class QWidgetStack;
class QWidget;
class QTextEdit;
class QListBox;
class QCheckBox;
class QTable;
class QSpinBox;
class QLabel;
class Graph;
class ColorBox;

class fitDialog : public QDialog
{
    Q_OBJECT

public:
    fitDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~fitDialog();

	void initFitPage();
	void initEditPage();

	QCheckBox* boxUseBuiltIn;
	QWidgetStack* tw;
    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonClear;
	QPushButton* buttonPlugins;
	QComboBox* boxCurve;
	QComboBox* boxSolver;
	QTable* boxParams;
	QLineEdit* boxFrom;
	QLineEdit* boxTo;
	QLineEdit* boxTolerance;
	QSpinBox* boxPoints;
	QWidget *fitPage, *editPage;
	QTextEdit *editBox, *explainBox, *boxFunction;
	QListBox *categoryBox, *funcBox;
	QLineEdit *boxName, *boxParam;
	QLabel *lblFunction;
	QPushButton *btnAddFunc, *btnDelFunc, *btnContinue;
	QPushButton *buttonEdit, *btnAddTxt, *btnAddName, *btnDeleteTables;
	ColorBox* boxColor;

public slots:
	void accept();
	void insertFunctionsList(const QStringList& list);
	void clearList();
	void showFitPage();
	void showEditPage();
	void showFunctionsList(int category);
	void showParseFunctions();
	void showUserFunctions();
	void loadPlugins();
	void showExpresion(int function);
	void addFunction();
	void addFunctionName();
	void setFunction(bool ok);
	void saveUserFunction();
	void removeUserFunction();
	void setBuiltInFunctionNames();
	void setBuiltInFunctions();
	bool containsUserFunctionName(const QString& s);
	void setGraph(Graph *g);
	void activateCurve(int index);
	void choosePluginsFolder();
	bool validInitialValues();
	void changeDataRange();
	QString fitBuiltInFunction(const QString&,const QString&, const QStringList&, 
							double, double, int, int, double, int);

signals:
	void clearFunctionsList();
	void saveFunctionsList(const QStringList&);

private:
	Graph *graph;
	QStringList userFunctions, userFunctionNames, userFunctionParams;
	QStringList builtInFunctionNames, builtInFunctions;
	QStringList pluginFunctionNames, pluginFunctions, pluginFilesList, pluginParameters;
};

#endif // FITDIALOG_H
