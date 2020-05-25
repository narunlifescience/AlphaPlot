#ifndef ADDAXISWIDGET_H
#define ADDAXISWIDGET_H

#include <QDialog>

class Ui_AddAxisWidget;

class AddAxisWidget : public QDialog
{
  Q_OBJECT

public:
  explicit AddAxisWidget(QWidget *parent);
  ~AddAxisWidget();

signals:
  void addleftaxisclicked();
  void addbottomaxisclicked();
  void addrightaxisclicked();
  void addtopaxisclicked();

private slots:
  void leftaxis();
  void bottomaxis();
  void rightaxis();
  void topaxis();
private:
  Ui_AddAxisWidget *ui_;
};

#endif // ADDAXISWIDGET_H
