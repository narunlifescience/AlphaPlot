/* This file is part of AlphaPlot.
   Copyright 2016 - 2021, Arun Narayanankutty <n.arun.lifescience@gmail.com>

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

   Description : Symbol browser dialog */

#include "CharacterMapWidget.h"

#include <QBoxLayout>
#include <QClipboard>
#include <QFontComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QtGui/QtGui>

#include "widgets/CharacterWidget.h"
#include "widgets/CustomCharacterDialog.h"

const int CharacterMapWidget::glyphSize_ = 16;

CharacterMapWidget::CharacterMapWidget(QDialog *parent)
    : QDialog(parent), characterWidget_(new CharacterWidget()) {
  QLabel *fontLabel = new QLabel(tr("Font:"));
  fontCombo_ = new QFontComboBox;
  QLabel *charsLabel = new QLabel(tr("Chars:"));
  charlist_ = new QComboBox;
  QStringList list = QStringList()
                     << tr("All") << tr("Greek Symbol") << tr("Number Symbol")
                     << tr("Sub/Super Script") << tr("Math Symbol")
                     << tr("Arrow Symbol") << tr("Other Symbols");
  charlist_->addItems(list);

  scrollArea_ = new QScrollArea;
  characterWidget_ = new CharacterWidget;
  scrollArea_->setWidget(characterWidget_);
  characterWidget_->updateSize(CharacterMapWidget::glyphSize_);

  QPushButton *addCustomSymbolButton = new QPushButton(tr("+ Custom Char"));
  addCustomSymbolButton->setToolTip(tr("Add custom characters"));
  lineEdit_ = new QLineEdit;
  lineEdit_->setClearButtonEnabled(true);
  QPushButton *clipboardButton = new QPushButton(tr("&Copy"));

  connect(fontCombo_, &QFontComboBox::currentFontChanged, characterWidget_,
          &CharacterWidget::updateFont);
  connect(characterWidget_, &CharacterWidget::characterSelected, this,
          &CharacterMapWidget::insertCharacter);
  connect(
      charlist_,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      [&](int index) {
        characterWidget_->updateChars(
            static_cast<CharacterWidget::Chars>(index));
      });
  connect(addCustomSymbolButton, &QPushButton::clicked, [=]() {
    std::unique_ptr<CustomCharacterDialog> customchar =
        std::unique_ptr<CustomCharacterDialog>(new CustomCharacterDialog);
    connect(customchar.get(), &CustomCharacterDialog::customChar,
            [=](int value) { insertCharacter(QString(QChar(value))); });
    customchar->exec();
  });

  connect(clipboardButton, &QAbstractButton::clicked, this,
          &CharacterMapWidget::updateClipboard);

  QHBoxLayout *controlsLayout = new QHBoxLayout;
  controlsLayout->addWidget(charsLabel);
  controlsLayout->addWidget(charlist_, 1);
  controlsLayout->addStretch(1);
  controlsLayout->addWidget(fontLabel);
  controlsLayout->addWidget(fontCombo_, 1);

  QHBoxLayout *lineLayout = new QHBoxLayout;
  lineLayout->addWidget(lineEdit_, 1);
  lineLayout->addWidget(addCustomSymbolButton);
  lineLayout->addSpacing(12);
  lineLayout->addWidget(clipboardButton);

  QVBoxLayout *centralLayout = new QVBoxLayout;
  centralLayout->addLayout(controlsLayout);
  centralLayout->addWidget(scrollArea_, 1);
  centralLayout->addSpacing(4);
  centralLayout->addLayout(lineLayout);
  setLayout(centralLayout);
  setWindowTitle(tr("Symbols/Glyphs"));
}

void CharacterMapWidget::insertCharacter(const QString &character) {
  lineEdit_->insert(character);
}

void CharacterMapWidget::updateClipboard() {
  QGuiApplication::clipboard()->setText(lineEdit_->text(),
                                        QClipboard::Clipboard);
  QGuiApplication::clipboard()->setText(lineEdit_->text(),
                                        QClipboard::Selection);
}

void CharacterMapWidget::resizeEvent(QResizeEvent *event) {
  characterWidget_->resized(scrollArea_->width());
  QDialog::resizeEvent(event);
}
