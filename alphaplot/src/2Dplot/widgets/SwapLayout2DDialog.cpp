#include "SwapLayout2DDialog.h"

#include <QtGlobal>

#include "2Dplot/Layout2D.h"
#include "core/IconLoader.h"
#include "ui_SwapLayout2DDialog.h"

Q_DECLARE_METATYPE(SwapLayout2DDialog::Pair);

SwapLayout2DDialog::SwapLayout2DDialog(Layout2D *parent)
    : QDialog(parent), ui_(new Ui_SwapLayout2DDialog), layout_(parent) {
  Q_ASSERT(layout_);
  ui_->setupUi(this);
  setWindowTitle(tr("Swap Layout"));
  ui_->swaplabel->setPixmap(
      IconLoader::load("edit-recalculate", IconLoader::LightDark)
          .pixmap(24, 24));
  ui_->swaplabel->setAlignment(Qt::AlignCenter);

  foreach (AxisRect2D *axisrect, layout_->getAxisRectList()) {
    Pair pair = layout_->getAxisRectRowCol(axisrect);
    ui_->swap1comboBox->addItem(
        QString("%1, %2")
            .arg(layout_->getAxisRectRowCol(axisrect).first + 1)
            .arg(layout_->getAxisRectRowCol(axisrect).second + 1),
        QVariant::fromValue(pair));
    ui_->swap2comboBox->addItem(
        QString("%1, %2")
            .arg(layout_->getAxisRectRowCol(axisrect).first + 1)
            .arg(layout_->getAxisRectRowCol(axisrect).second + 1),
        QVariant::fromValue(pair));
  }

  connect(ui_->buttonBox, &QDialogButtonBox::accepted, this,
          &SwapLayout2DDialog::swap);
  connect(ui_->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(ui_->swap1comboBox, &QComboBox::currentTextChanged, this, [=](){swapComboboxIndexChanged(SwapLayout::Swap1);});
  connect(ui_->swap2comboBox, &QComboBox::currentTextChanged, this, [=](){swapComboboxIndexChanged(SwapLayout::Swap2);});
  swapComboboxIndexChanged(SwapLayout::Swap1);
}

SwapLayout2DDialog::~SwapLayout2DDialog() { delete ui_; }

void SwapLayout2DDialog::swapComboboxIndexChanged(const SwapLayout &s) {
   switch (s) {
   case SwapLayout::Swap1:{
     if(ui_->swap1comboBox->currentData(Qt::UserRole) == ui_->swap2comboBox->currentData(Qt::UserRole)) {
       (ui_->swap2comboBox->currentIndex() == ui_->swap2comboBox->count() - 1) ?
       ui_->swap2comboBox->setCurrentIndex(ui_->swap2comboBox->currentIndex() - 1):
       ui_->swap2comboBox->setCurrentIndex(ui_->swap2comboBox->currentIndex() + 1);
     }
   }
   break;
   case SwapLayout::Swap2: {
       if(ui_->swap2comboBox->currentData(Qt::UserRole) == ui_->swap1comboBox->currentData(Qt::UserRole)) {
         (ui_->swap1comboBox->currentIndex() == ui_->swap1comboBox->count() - 1) ?
         ui_->swap1comboBox->setCurrentIndex(ui_->swap1comboBox->currentIndex() - 1):
         ui_->swap1comboBox->setCurrentIndex(ui_->swap1comboBox->currentIndex() + 1);
       }
   }
   break;
   }
}

void SwapLayout2DDialog::swap() {
  Pair rowcol1 = ui_->swap1comboBox->currentData(Qt::UserRole).value<Pair>();
  Pair rowcol2 = ui_->swap2comboBox->currentData(Qt::UserRole).value<Pair>();
  if (rowcol1 == rowcol2) {
    QMessageBox::warning(this, tr("Layout Swap Error"),
                         tr("Cannot swap same layout element!"));
    return;
  }

  AxisRect2D *axisrect1 = nullptr;
  AxisRect2D *axisrect2 = nullptr;
  foreach (auto axisrect, layout_->getAxisRectList()) {
    if (layout_->getAxisRectRowCol(axisrect) == rowcol1)
      axisrect1 = axisrect;
    else if (layout_->getAxisRectRowCol(axisrect) == rowcol2)
      axisrect2 = axisrect;
  }
  layout_->swapAxisRect(axisrect1, axisrect2);
  QDialog::accept();
}
