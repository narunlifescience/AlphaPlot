#ifndef PROPERTYITEMMODEL_H
#define PROPERTYITEMMODEL_H

#include <QAbstractItemModel>

class PropertyItem;
class ObjectBrowserTreeItem;
class MyWidget;
class AxisRect2D;

class PropertyItemModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  PropertyItemModel(QObject* parent);
  virtual ~PropertyItemModel();

  QModelIndex buddy(const QModelIndex& index) const;
  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex& index, const QVariant& value, int role);
  Qt::ItemFlags flags(const QModelIndex& index) const;
  QModelIndex index(int row, int column,
                    const QModelIndex& parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex& index) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  bool setHeaderData(int, Qt::Orientation, const QVariant&, int = Qt::EditRole);
  void updateProperty(PropertyItem* item);
  void buildUp(ObjectBrowserTreeItem* item);

  bool removeRows(int row, int count,
                  const QModelIndex& parent = QModelIndex());
  void removeAll();

 signals:
  void modelResetComplete();
  void namechange(MyWidget* widget, const QString& name);
  void labelchange(MyWidget* widget, const QString& label);
  void outofrange(const double min, const double max);
  void repaintViewer();

 private:
  void updateChildren(PropertyItem* item, int column,
                      const QModelIndex& parent);
  QList<QPair<QIcon, QString>> getXaxislist(AxisRect2D* axisrect);
  QList<QPair<QIcon, QString>> getYaxislist(AxisRect2D* axisrect);

 private:
  PropertyItem* rootItem_;
};

#endif  // PROPERTYITEMMODEL_H
