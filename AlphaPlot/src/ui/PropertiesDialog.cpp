
#include "PropertiesDialog.h"
#include "ui_PropertiesDialog.h"

#include "../Folder.h"
#include "../core/IconLoader.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

PropertiesDialog::PropertiesDialog(QWidget *parent)
    : QDialog(parent), ui_(new Ui_PropertiesDialog) {
  ui_->setupUi(this);
  setModal(true);
  ui_->mainGridLayout->setContentsMargins(0, 0, 0, 0);

  // set caption label alignments
  ui_->typeCaption->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ui_->statusCaption->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ui_->pathCaption->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ui_->sizeCaption->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ui_->contentCaption->setAlignment(Qt::AlignRight | Qt::AlignTop);
  ui_->createdCaption->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ui_->modifiedCaption->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

PropertiesDialog::~PropertiesDialog() { delete ui_; }

void PropertiesDialog::setupProperties(const Properties &properties) const {
  ui_->iconLabel->setPixmap(properties.icon);
  ui_->nameLineEdit->setText(properties.name);
  ui_->typeValue->setText(properties.type);
  ui_->statusValue->setText(properties.status);
  ui_->pathValue->setText(properties.path);
  ui_->sizeValue->setText(properties.size);
  ui_->createdValue->setText(properties.created);
  ui_->modifiedValue->setText(properties.modified);
  ui_->contentValue->setText(properties.content);
}
