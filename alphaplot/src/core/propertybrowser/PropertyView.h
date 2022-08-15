#ifndef PROPERTYVIEW_H
#define PROPERTYVIEW_H

#include <QTreeView>

class PropertyView : public QTreeView {
  Q_OBJECT
 public:
  explicit PropertyView(QWidget *parent = nullptr);

 protected:
  virtual void drawBranches(QPainter *painter, const QRect &rect,
                            const QModelIndex &index) const;

 signals:
};

#endif  // PROPERTYVIEW_H
