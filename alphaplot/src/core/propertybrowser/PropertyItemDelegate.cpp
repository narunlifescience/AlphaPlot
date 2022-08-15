#include "PropertyItemDelegate.h"

#include <QApplication>
#include <QEvent>
#include <QPainter>
#include <QTreeView>

#include "PropertyItem.h"
#include "PropertyItemModel.h"
#include "core/IconLoader.h"
#include "propertybrowser.h"

const int PropertyItemDelegate::left_decoration_margin_ = 15;
const int PropertyItemDelegate::left_color_inner_rect_margin_ = 20;

PropertyItemDelegate::PropertyItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent), pressed(false), changed(false) {}

PropertyItemDelegate::~PropertyItemDelegate() {}

QSize PropertyItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const {
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  // size += QSize(0, 5);
  return size;
}

void PropertyItemDelegate::paint(QPainter* painter,
                                 const QStyleOptionViewItem& opt,
                                 const QModelIndex& index) const {
  QStyleOptionViewItem option = opt;

  PropertyItem* property = static_cast<PropertyItem*>(index.internalPointer());

  if (property->propertyType() == PropertyItem::PropertyType::Separator) {
    QColor color = option.palette.color(QPalette::Text);
    QObject* par = parent();
    if (par) {
      QVariant value = par->property("groupTextColor");
      if (value.canConvert<QColor>()) color = value.value<QColor>();
    }
    option.palette.setColor(QPalette::Text, color);
    option.font.setBold(true);
    option.state &= ~QStyle::State_Selected;
  }

  if (index.column() == 1) {
    option.state &= ~QStyle::State_Selected;
  }

  option.state &= ~QStyle::State_HasFocus;

  if (property->propertyType() == PropertyItem::PropertyType::Separator) {
    QPalette pal = option.palette;
    QBrush brush = QBrush();
    (IconLoader::lumen_ > 100) ? brush = option.palette.dark()
                               : brush = option.palette.light();
    pal.setColor(QPalette::Base, brush.color());
    pal.setColor(QPalette::AlternateBase, brush.color());
    pal.setColor(QPalette::Background, brush.color());
    QObject* par = parent();
    if (par) {
      QVariant value = par->property("groupBackground");
      if (value.canConvert<QBrush>()) brush = value.value<QBrush>();
    }
    painter->fillRect(option.rect, brush);
    option.palette = pal;
  }

  QPen savedPen = painter->pen();

  // QStyledItemDelegate::paint(painter, option, index);
  if (property->isreadonly()) {
    QPalette pal = option.palette;
    pal.setColor(QPalette::All, QPalette::Text,
                 pal.color(QPalette::Disabled, QPalette::Text));
    option.palette = pal;
  }

  QStyleOptionViewItem& optedtr = option;

  if (property->propertyType() == PropertyItem::PropertyType::Bool &&
      index.column() == 1) {
    paintBoolProperty(property, painter, optedtr);
  } else if (property->propertyType() == PropertyItem::PropertyType::Color &&
             index.column() == 1) {
    paintColorProperty(property, painter, optedtr);
  } else if (property->propertyType() == PropertyItem::PropertyType::Font &&
             index.column() == 1) {
    paintFontProperty(property, painter, optedtr);
  } else
    QStyledItemDelegate::paint(painter, optedtr, index);

  // Grid Lines
  QColor color = static_cast<QRgb>(QApplication::style()->styleHint(
      QStyle::SH_Table_GridLineColor, &opt, qobject_cast<QWidget*>(parent())));
  painter->setPen(QPen(color));
  if (index.column() == 1 &&
      property->propertyType() != PropertyItem::PropertyType::Separator) {
    int right = (option.direction == Qt::LeftToRight) ? option.rect.right()
                                                      : option.rect.left();
    painter->drawLine(right, option.rect.y(), right, option.rect.bottom());

    // if (index.column() == 1)
    painter->drawLine(option.rect.x(), option.rect.top(), option.rect.x(),
                      option.rect.bottom());
  }
  painter->drawLine(option.rect.x(), option.rect.bottom(), option.rect.right(),
                    option.rect.bottom());
  painter->setPen(savedPen);
}

bool PropertyItemDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex& index) {
  if (event && event->type() == QEvent::MouseButtonPress)
    this->pressed = true;
  else
    this->pressed = false;
  return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void PropertyItemDelegate::paintBoolProperty(
    PropertyItem* property, QPainter* painter,
    const QStyleOptionViewItem& option) const {
  bool checked = property->data(1, Qt::ItemDataRole::UserRole).toBool();
  QStyleOptionButton buttonOption;
  // buttonOption.state |= QStyle::State_Active;  // Required!
  //  buttonOption.state |=
  //      ((index.flags() & Qt::ItemIsEditable) ? QStyle::State_Enabled
  //                                            : QStyle::State_ReadOnly);
  buttonOption.state |= (checked ? QStyle::State_On : QStyle::State_Off);
  QRect checkBoxRect = QApplication::style()->subElementRect(
      QStyle::SE_CheckBoxIndicator,
      &buttonOption);  // Only used to get size of native checkbox widget.
  buttonOption.text = property->toString();
  buttonOption.rect =
      QStyle::alignedRect(option.direction, Qt::AlignLeft, checkBoxRect.size(),
                          option.rect);  // Our checkbox rect.
  QApplication::style()->drawControl(QStyle::CE_CheckBox, &buttonOption,
                                     painter);
}

void PropertyItemDelegate::paintColorProperty(
    PropertyItem* property, QPainter* painter,
    const QStyleOptionViewItem& option) const {
  if (!property) return;

  auto rect = option.rect;
  rect.setWidth(rect.height());
  auto leftrect = rect;

  // leftside
  QStyleOptionViewItem left_color_option(option);
  QColor color = property->data(1, Qt::ItemDataRole::UserRole).value<QColor>();

  double adjust = ((double)(rect.height()) * left_decoration_margin_) / 100.0;
  rect.adjust(adjust, adjust, -adjust, -adjust);
  left_color_option.rect = rect;
  left_color_option.backgroundBrush = QBrush(color);
  QApplication::style()->drawControl(QStyle::CE_ItemViewItem,
                                     &left_color_option, painter);
  if (color.alpha() != 255) {
    color.setAlpha(255);
    QStyleOptionViewItem left_color_core_option(option);

    adjust = ((double)(rect.height()) * left_color_inner_rect_margin_) / 100.0;
    rect.adjust(adjust, adjust, -adjust, -adjust);
    left_color_core_option.rect = rect;
    left_color_core_option.backgroundBrush = QBrush(color);
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem,
                                       &left_color_core_option, painter);
  }

  // right side
  QStyleOptionViewItem right_option(option);
  QRect rect1 = right_option.rect;
  rect1.adjust(+leftrect.height(), 0, 0, 0);
  right_option.rect = rect1;
  right_option.text = property->toString();
  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &right_option,
                                     painter);
}

void PropertyItemDelegate::paintFontProperty(
    PropertyItem* property, QPainter* painter,
    const QStyleOptionViewItem& option) const {
  QFont font = property->data(1, Qt::ItemDataRole::UserRole).value<QFont>();

  auto rect = option.rect;
  rect.setWidth(rect.height());
  auto leftrect = rect;

  double adjust = ((double)(rect.height()) * 15.0) / 100.0;
  rect.adjust(adjust, adjust, -adjust, -adjust);
  QStyleOptionViewItem left_option(option);
  left_option.rect = rect;
  font.setPointSize(13);
  left_option.font = font;
  left_option.text = QString(QLatin1Char('A'));
  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &left_option,
                                     painter);
  rect = option.rect;
  rect.adjust(leftrect.height(), 0, 0, 0);
  QStyleOptionViewItem right_option(option);
  right_option.rect = rect;
  right_option.text = property->toString();
  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &right_option,
                                     painter);
}

QWidget* PropertyItemDelegate::createEditor(
    QWidget* parent, const QStyleOptionViewItem& /*option*/,
    const QModelIndex& index) const {
  if (!index.isValid()) return nullptr;

  PropertyItem* childItem = static_cast<PropertyItem*>(index.internalPointer());
  if (!childItem) return nullptr;

  QTreeView* parentEditor = qobject_cast<QTreeView*>(this->parent());
  QWidget* editor = nullptr;
  if (parentEditor) editor = childItem->createEditor(parent);
  if (editor)  // Make sure the editor background is painted so the cell content
               // doesn't show through
    editor->setAutoFillBackground(true);
  if (editor && childItem->isreadonly())
    editor->setDisabled(true);
  else if (editor /*&& this->pressed*/) {
    // We changed the way editor is activated in PropertyEditor (in response
    // of signal activated and clicked), so now we should grab focus
    // regardless of "pressed" or not (e.g. when activated by keyboard
    // enter)
    editor->setFocus();
  }
  this->pressed = false;

  return editor;
}

void PropertyItemDelegate::valueChanged() {
  QWidget* editor = qobject_cast<QWidget*>(sender());
  if (editor) {
    emit commitData(editor);
  }
}

void PropertyItemDelegate::setEditorData(QWidget* editor,
                                         const QModelIndex& index) const {
  if (!index.isValid()) return;
  QVariant data = index.data(Qt::EditRole);
  PropertyItem* childItem = static_cast<PropertyItem*>(index.internalPointer());

  editor->blockSignals(true);
  childItem->setEditorData(editor);
  editor->blockSignals(false);
  return;
}

void PropertyItemDelegate::setModelData(QWidget* editor,
                                        QAbstractItemModel* model,
                                        const QModelIndex& index) const {
  if (!index.isValid() || !changed || !editor) return;
  PropertyItem* childItem = static_cast<PropertyItem*>(index.internalPointer());
  QVariant data;
  data = childItem->editorData(editor);
  model->setData(index, data, Qt::EditRole);
}
