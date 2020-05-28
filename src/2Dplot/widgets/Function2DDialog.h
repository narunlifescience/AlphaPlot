#ifndef FUNCTION2DDIALOG_H
#define FUNCTION2DDIALOG_H

#include <QDialog>

class Ui_Function2DDialog;
class AxisRect2D;

class Function2DDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Function2DDialog(QWidget *parent = nullptr);
    ~Function2DDialog();

    void setLayout2DToModify(AxisRect2D *axisrect, int plottomidify);

    void insertParamFunctionsList(const QStringList &xList,
                                  const QStringList &yList);
    void insertPolarFunctionsList(const QStringList &rList,
                                  const QStringList &thetaList);

private slots:
    void raiseWidget(const int index) const;
    // void setFunctionPlotToModify(LineScatter2D *ls);
    // void setParametricFunctionPlotToModify();
    bool apply();
    void accept();
    bool acceptFunction();
    bool acceptParametric();
    // bool acceptPolar();
    void clearList();

signals:
    void updateFunctionLists(int, QStringList);
    void clearParamFunctionsList();
    void clearPolarFunctionsList();

private:
    Ui_Function2DDialog *ui_;
    AxisRect2D *axisrect_;
    int plottomodify_;
};

#endif // FUNCTION2DDIALOG_H
