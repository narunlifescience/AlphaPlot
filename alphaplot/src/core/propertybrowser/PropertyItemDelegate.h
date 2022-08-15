#ifndef PROPERTYITEMDELEGATE_H
#define PROPERTYITEMDELEGATE_H

#include <QStyledItemDelegate>

class PropertyItem;

class PropertyItemDelegate : public QStyledItemDelegate {
  Q_OBJECT

 public:
  PropertyItemDelegate(QObject *parent);
  ~PropertyItemDelegate();

  virtual void paint(QPainter *painter, const QStyleOptionViewItem &opt,
                     const QModelIndex &index) const;
  virtual QWidget *createEditor(QWidget *, const QStyleOptionViewItem &,
                                const QModelIndex &) const;
  virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
  virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
                            const QModelIndex &index) const;
  virtual QSize sizeHint(const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

 protected:
  virtual bool editorEvent(QEvent *event, QAbstractItemModel *model,
                           const QStyleOptionViewItem &option,
                           const QModelIndex &index);

 private:
  void paintBoolProperty(PropertyItem *property, QPainter *painter,
                          const QStyleOptionViewItem &option) const;
  void paintColorProperty(PropertyItem *property, QPainter *painter,
                          const QStyleOptionViewItem &option) const;
  void paintFontProperty(PropertyItem *property, QPainter *painter,
                         const QStyleOptionViewItem &option) const;

 public slots:
  void valueChanged();

 private:
  static const int left_decoration_margin_;
  static const int left_color_inner_rect_margin_;
  mutable bool pressed;
  bool changed;
};

#endif  // PROPERTYITEMDELEGATE_H
