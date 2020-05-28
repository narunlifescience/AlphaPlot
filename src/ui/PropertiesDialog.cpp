/* This file is part of AlphaPlot.
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : properties dialog
*/

#include "PropertiesDialog.h"
#include "ui_PropertiesDialog.h"

#include "../Folder.h"
#include "../project/IconLoader.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

PropertiesDialog::PropertiesDialog(QWidget *parent)
    : QDialog(parent), ui_(new Ui_PropertiesDialog)
{
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

PropertiesDialog::~PropertiesDialog()
{
    delete ui_;
}

void PropertiesDialog::setupProperties(const Properties &properties) const
{
    ui_->iconLabel->setPixmap(properties.icon);
    ui_->nameLineEdit->setText(properties.name);
    ui_->typeValue->setText(properties.type);
    ui_->statusValue->setText(properties.status);
    ui_->pathValue->setText(properties.path);
    ui_->sizeValue->setText(properties.size);
    ui_->createdValue->setText(properties.created);
    ui_->modifiedValue->setText(properties.modified);
    QStringList lst = properties.content.split("\n");
    ui_->contentValue1->setText(lst.at(0));
    ui_->contentValue2->setText(lst.at(1));
    ui_->descriptionPlainTextEdit->setPlainText(properties.description);
    ui_->labelValue->setPlainText(properties.label);
}
