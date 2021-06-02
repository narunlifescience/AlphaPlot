#include "CustomCharacterDialog.h"
#include "ui_CustomCharacterDialog.h"

CustomCharacterDialog::CustomCharacterDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CustomCharacterDialog)
{
  ui->setupUi(this);
}

CustomCharacterDialog::~CustomCharacterDialog()
{
  delete ui;
}
