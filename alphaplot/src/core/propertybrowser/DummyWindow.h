#ifndef DUMMYWINDOW_H
#define DUMMYWINDOW_H

#include "core/propertybrowser/ObjectBrowserTreeItem.h"

class MyWidget;

class DummyWindow : public ObjectBrowserTreeItem {
 public:
  DummyWindow(ObjectBrowserTreeItem *parentitem, MyWidget *widget);
  virtual ~DummyWindow();

  virtual QString getItemName() override;
  virtual QIcon getItemIcon() override;
  virtual QString getItemTooltip() override;
};

class DummyNone : public ObjectBrowserTreeItem {
 public:
  DummyNone();
  virtual ~DummyNone();

  virtual QString getItemName() override;
  virtual QIcon getItemIcon() override;
  virtual QString getItemTooltip() override;
};

#endif  // DUMMYWINDOW_H
