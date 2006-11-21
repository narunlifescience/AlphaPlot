/***************************************************************************
    File                 : fitDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Nonlinear curve fitting dialog
                           
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#ifndef FITDIALOG_H
#define FITDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>

class QPushButton;
class QLineEdit;
class QComboBox;
class Q3ComboBox;
class Q3WidgetStack;
class QWidget;
class Q3TextEdit;
class Q3ListBox;
class QCheckBox;
class Q3Table;
class QSpinBox;
class QLabel;
class QRadioButton;
class QLineEdit;
class Graph;
class ColorBox;
class Fitter;

//! Nonlinear curve fitting dialog
class FitDialog : public QDialog
{
    Q_OBJECT

public:
    FitDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~FitDialog();

	void initFitPage();
	void initEditPage();
	void initAdvancedPage();

	QCheckBox* boxUseBuiltIn;
	Q3WidgetStack* tw;
    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonAdvanced;
	QPushButton* buttonClear;
	QPushButton* buttonPlugins;
	QPushButton* btnBack;
	QComboBox* boxCurve;
	QComboBox* boxSolver;
	Q3Table* boxParams;
	QLineEdit* boxFrom;
	QLineEdit* boxTo;
	QLineEdit* boxTolerance;
	QSpinBox* boxPoints, *generatePointsBox, *boxPrecision, *polynomOrderBox;
	QWidget *fitPage, *editPage, *advancedPage;
	Q3TextEdit *editBox, *explainBox, *boxFunction;
	Q3ListBox *categoryBox, *funcBox;
	QLineEdit *boxName, *boxParam;
	QLabel *lblFunction, *lblPoints, *polynomOrderLabel;
	QPushButton *btnAddFunc, *btnDelFunc, *btnContinue, *btnApply;
	QPushButton *buttonEdit, *btnAddTxt, *btnAddName, *btnDeleteTables;
	ColorBox* boxColor;
	Q3ComboBox *boxWeighting, *tableNamesBox, *colNamesBox;
	QRadioButton *generatePointsBtn, *samePointsBtn;
	QPushButton *btnParamTable, *btnCovMatrix;
	QLineEdit *covMatrixName, *paramTableName;
	QCheckBox *plotLabelBox, *logBox;
	Fitter *fitter;
 
protected:
	void closeEvent (QCloseEvent * e );

public slots:
	void accept();
	void insertFunctionsList(const QStringList& list);
	void clearList();
	void showFitPage();
	void showEditPage();
	void showAdvancedPage();
	void showFunctionsList(int category);
	void showParseFunctions();
	void showUserFunctions();
	void loadPlugins();
	void showExpression(int function);
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
	void fitBuiltInFunction(const QString& function, const QStringList& initVal);

	void setSrcTables(QWidgetList* tables);
	void selectSrcTable(int tabnr);
	void enableWeightingParameters(int index);
	void showPointsBox(bool);
	void showParametersTable();
	void showCovarianceMatrix();

	//! Applies the user changes to the numerical format of the output results
	void applyChanges();
	void enableApplyChanges(int);

signals:
	void clearFunctionsList();
	void saveFunctionsList(const QStringList&);

private:
	Graph *graph;
	QStringList userFunctions, userFunctionNames, userFunctionParams;
	QStringList builtInFunctionNames, builtInFunctions;
	QStringList pluginFunctionNames, pluginFunctions, pluginFilesList, pluginParameters;
	QWidgetList *srcTables;
};

#endif // FITDIALOG_H
