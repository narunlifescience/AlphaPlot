#include "CustomCharacterDialog.h"

#include "ui_CustomCharacterDialog.h"

CustomCharacterDialog::CustomCharacterDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::CustomCharacterDialog) {
  ui->setupUi(this);
  setWindowTitle("Custom Char");
  connect(ui->buttonBox, &QDialogButtonBox::accepted, [=]() {
    if (!ui->lineEdit->text().isEmpty()) {
      int value = ui->lineEdit->text().toInt(0, 16);
      if (value && (QChar(value).category() != QChar::Other_NotAssigned)) {
        emit customChar(value);
      }
    }
  });
}

CustomCharacterDialog::~CustomCharacterDialog() { delete ui; }
