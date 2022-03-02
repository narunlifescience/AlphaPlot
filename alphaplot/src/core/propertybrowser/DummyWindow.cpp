#include "DummyWindow.h"

#include "MyWidget.h"
#include "core/IconLoader.h"

DummyWindow::DummyWindow(ObjectBrowserTreeItem *parentitem, MyWidget *widget)
    : ObjectBrowserTreeItem(QVariant::fromValue<MyWidget *>(widget),
                            ObjectBrowserTreeItem::ObjectType::BaseWindow,
                            parentitem) {}

DummyWindow::~DummyWindow() {  // parentItem()->removeChild(this);
}

QString DummyWindow::getItemName() { return QObject::tr("Window"); }

QIcon DummyWindow::getItemIcon() {
  return IconLoader::load("view-console", IconLoader::LightDark);
}

QString DummyWindow::getItemTooltip() { return getItemName(); }

DummyNone::DummyNone()
    : ObjectBrowserTreeItem(QVariant::fromValue<MyWidget *>(nullptr),
                            ObjectBrowserTreeItem::ObjectType::None, nullptr) {}

DummyNone::~DummyNone() {}

QString DummyNone::getItemName() { return "(" + QObject::tr("None") + ")"; }

QIcon DummyNone::getItemIcon() {
  return IconLoader::load("clear-loginfo", IconLoader::General);
}

QString DummyNone::getItemTooltip() { return getItemName(); }
