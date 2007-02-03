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

#include "graph.h"

class QPushButton;
class QLineEdit;
class QComboBox;
class QStackedWidget;
class QWidget;
class QTextEdit;
class QListWidget;
class QCheckBox;
class QTableWidget;
class QSpinBox;
class QLabel;
class QRadioButton;
class QLineEdit;
class ColorBox;
class Fit;

//! Nonlinear curve fitting dialog
class FitDialog : public QDialog
{
    Q_OBJECT

public:
    FitDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~FitDialog();
 
protected:
	void closeEvent (QCloseEvent * e );
    void initFitPage();
	void initEditPage();
	void initAdvancedPage();

public slots:
	void accept();
	void insertFunctionsList(const QStringList& list);
	void clearUserList();
    //! Clears the function editor, the parameter names and the function name
    void resetFunction();
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
	void fitBuiltInFunction(const QString& function, double* initVal);

	void setSrcTables(QWidgetList* tables);
	void selectSrcTable(int tabnr);
	void enableWeightingParameters(int index);
	void showPointsBox(bool);
	void showParametersTable();
	void showCovarianceMatrix();

	//! Applies the user changes to the numerical format of the output results
	void applyChanges();
	//! Enable the "Apply" button
	void enableApplyChanges();

	//! Deletes the result fit curves from the plot
	void deleteFitCurves();

signals:
	void clearFunctionsList();
	void saveFunctionsList(const QStringList&);

private:
    Fit *fitter;
	Graph *graph;
	QStringList userFunctions, userFunctionNames, userFunctionParams;
	QStringList builtInFunctionNames, builtInFunctions;
	QStringList pluginFunctionNames, pluginFunctions, pluginFilesList, pluginParameters;
	QWidgetList *srcTables;

    QCheckBox* boxUseBuiltIn;
	QStackedWidget* tw;
    QPushButton* buttonOk;
	QPushButton* buttonCancel1;
	QPushButton* buttonCancel2;
	QPushButton* buttonCancel3;
	QPushButton* buttonAdvanced;
	QPushButton* buttonClear;
    QPushButton* buttonClearUsrList;
	QPushButton* buttonPlugins;
	QPushButton* btnBack;
	QComboBox* boxCurve;
	QComboBox* boxAlgorithm;
	QTableWidget* boxParams;
	QLineEdit* boxFrom;
	QLineEdit* boxTo;
	QLineEdit* boxTolerance;
	QSpinBox* boxPoints, *generatePointsBox, *boxPrecision, *polynomOrderBox;
	QWidget *fitPage, *editPage, *advancedPage;
	QTextEdit *editBox, *explainBox, *boxFunction;
	QListWidget *categoryBox, *funcBox;
	QLineEdit *boxName, *boxParam;
	QLabel *lblFunction, *lblPoints, *polynomOrderLabel;
	QPushButton *btnAddFunc, *btnDelFunc, *btnContinue, *btnApply;
	QPushButton *buttonEdit, *btnAddTxt, *btnAddName, *btnDeleteFitCurves;
	ColorBox* boxColor;
	QComboBox *boxWeighting, *tableNamesBox, *colNamesBox;
	QRadioButton *generatePointsBtn, *samePointsBtn;
	QPushButton *btnParamTable, *btnCovMatrix;
	QLineEdit *covMatrixName, *paramTableName;
	QCheckBox *plotLabelBox, *logBox, *scaleErrorsBox;
};

#endif // FITDIALOG_H
