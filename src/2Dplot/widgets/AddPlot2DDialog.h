#ifndef ADDPLOT2DDIALOG_H
#define ADDPLOT2DDIALOG_H

#include <QDialog>
#include "../AxisRect2D.h"

class QComboBox;
class QListWidget;
class QPushButton;
class QCheckBox;
class ApplicationWindow;
class AxisRect2D;

//! Add/remove curves dialog
class AddPlot2DDialog : public QDialog
{
    Q_OBJECT

public:
    AddPlot2DDialog(QWidget *parent, AxisRect2D *axisrect,
                    Qt::WindowFlags fl = 0);

private slots:
    void addCurves();
    int curveStyle();
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
    void enableAddBtn();
    void showCurrentFolder(bool);

private:
    void closeEvent(QCloseEvent *event);

    void init();
    void loadplotcontents();
    QSize sizeHint() const;
    void contextMenuEvent(QContextMenuEvent *);

    ApplicationWindow *app_;
    AxisRect2D *axisrect_;
    QList<QPair<Table *, Column *>> available_columns_;
    QList<QPair<Table *, Column *>> plotted_columns_;

    QPushButton *btnAdd;
    QPushButton *btnOK;
    QPushButton *btnCancel;
    QPushButton *btnAssociations;
    QPushButton *btnEditFunction;
    QListWidget *available;
    QListWidget *contents;
    QComboBox *boxStyle;
    QComboBox *boxMatrixStyle;
    QCheckBox *boxShowCurrentFolder;
};

#endif // ADDPLOT2DDIALOG_H
