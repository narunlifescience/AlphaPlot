#ifndef SETPROPERTIES_H
#define SETPROPERTIES_H

#include <QDialog>

#include "../../../3rdparty/qcustomplot/qcustomplot.h"
#include "../AxisRect2D.h"

class Ui_Axis2DPropertiesDialog;
class QTreeWidgetItem;

class Axis2DPropertiesDialog : public QDialog {
  Q_OBJECT

 public:
  explicit Axis2DPropertiesDialog(QWidget *parent = nullptr,
                                  QCPAxis *axis = nullptr,
                                  AxisRect2D *axisRect = nullptr);
  ~Axis2DPropertiesDialog();

signals:
  void areplot();

 private slots:
  void apply();
  void applyAndClose();
  void axisItemActivated(QTreeWidgetItem *item);

private:
  void populateAxisTreeWidget();
  void populateAxisValue();

 private:
  Ui_Axis2DPropertiesDialog *ui_;
  Axis2D *selectedAxis_;
  AxisRect2D *selectedAxisRect_;

  QTreeWidgetItem *leftAxisRootItem_;
  QTreeWidgetItem *bottomAxisRootItem_;
  QTreeWidgetItem *rightAxisRootItem_;
  QTreeWidgetItem *topAxisRootItem_;

};

#endif  // SETPROPERTIES_H
