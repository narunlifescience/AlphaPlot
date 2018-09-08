#ifndef FUNCTION2DDIALOG_H
#define FUNCTION2DDIALOG_H

#include <QDialog>
#include "../LineScatter2D.h"

class Ui_Function2DDialog;
class AxisRect2D;

class Function2DDialog : public QDialog {
  Q_OBJECT

 public:
  explicit Function2DDialog(QWidget *parent = nullptr,
                            AxisRect2D *axisrect = nullptr);
  ~Function2DDialog();

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
};

#endif  // FUNCTION2DDIALOG_H
