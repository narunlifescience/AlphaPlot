#include "PropertyView.h"

#include <QPainter>

#include "PropertyItem.h"
#include "core/IconLoader.h"

PropertyView::PropertyView(QWidget *parent) : QTreeView(parent) {}

void PropertyView::drawBranches(QPainter *painter, const QRect &rect,
                                const QModelIndex &index) const {
  QTreeView::drawBranches(painter, rect, index);

  QStyleOptionViewItem option = viewOptions();
  QBrush brush = QBrush();
  (IconLoader::lumen_ > 100) ? brush = option.palette.dark()
                             : brush = option.palette.light();

  PropertyItem *property = static_cast<PropertyItem *>(index.internalPointer());
  if (property &&
      property->propertyType() == PropertyItem::PropertyType::Separator) {
    painter->fillRect(rect, brush);
  }
}
