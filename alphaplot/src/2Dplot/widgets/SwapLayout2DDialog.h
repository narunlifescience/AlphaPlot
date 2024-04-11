#ifndef SWAPLAYOUT2DDIALOG_H
#define SWAPLAYOUT2DDIALOG_H

#include <QDialog>

class Layout2D;
class Ui_SwapLayout2DDialog;

class SwapLayout2DDialog : public QDialog {
  Q_OBJECT

 public:
  typedef QPair<int,int> Pair;
  explicit SwapLayout2DDialog(Layout2D *parent = nullptr);
  ~SwapLayout2DDialog();

 private:
   enum class SwapLayout: int {
     Swap1 = 0,
     Swap2 = 1
   };

 private slots:
  void swapComboboxIndexChanged(const SwapLayout &s);
  void swap();

 private:
  Ui_SwapLayout2DDialog *ui_;
  Layout2D *layout_;
};

#endif  // SWAPLAYOUT2DDIALOG_H
