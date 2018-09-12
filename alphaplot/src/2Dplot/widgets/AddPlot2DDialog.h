#ifndef ADDPLOT2DDIALOG_H
#define ADDPLOT2DDIALOG_H


#include <QDialog>

class QComboBox;
class QListWidget;
class QPushButton;
class QCheckBox;
class ApplicationWindow;
class AxisRect2D;

//! Add/remove curves dialog
class AddPlot2DDialog : public QDialog {
  Q_OBJECT

 public:
  AddPlot2DDialog(QWidget* parent = nullptr, Qt::WFlags fl = 0);

  void setLayout2D(AxisRect2D* axisrect);

 private slots:
  void addCurves();
  void removeCurves();
  int curveStyle();
  void showCurveRangeDialog();
  void showPlotAssociations();
  void showFunctionDialog();

  /**
Enables ("yes")/disables ("no") the following buttons, which are on the right
besides
the graph contents list, depending on the selected item in this list:

<table>
<tr>  <td>Selected Item</td><td>btnEditFunction</td>  <td>btnAssociations</td>
<td>btnRange</td> </tr>
<tr>  <td>Spectrogram</td>  <td>no</td> <td>no</td> <td>no</td>   </tr>
<tr>  <td>Function</td>     <td>yes</td> <td>no</td> <td>no</td>   </tr>
<tr>  <td>ErrorBars</td>    <td>no</td> <td>yes</td> <td>no</td>   </tr>
<tr>  <td>all others</td>   <td>no</td> <td>yes</td> <td>yes</td>   </tr>
</table>
  */
  void showCurveBtn(int);
  void enableAddBtn();
  void enableRemoveBtn();
  void showCurveRange(bool);
  void updateCurveRange();
  void showCurrentFolder(bool);

 private:
  void closeEvent(QCloseEvent*);

  void init();
  bool addCurve(const QString& name);
  QSize sizeHint() const;
  void contextMenuEvent(QContextMenuEvent*);

  ApplicationWindow *app_;
  int defaultCurveLineWidth, defaultSymbolSize;
  AxisRect2D* axisrect_;

  QPushButton* btnAdd;
  QPushButton* btnRemove;
  QPushButton* btnOK;
  QPushButton* btnCancel;
  QPushButton* btnAssociations;
  QPushButton* btnEditFunction;
  QPushButton* btnRange;
  QListWidget* available;
  QListWidget* contents;
  QComboBox* boxStyle;
  QComboBox* boxMatrixStyle;
  QCheckBox* boxShowRange;
  QCheckBox* boxShowCurrentFolder;
};

#endif // ADDPLOT2DDIALOG_H
