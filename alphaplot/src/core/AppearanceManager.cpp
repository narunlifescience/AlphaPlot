#include "AppearanceManager.h"

#include <QColor>

const QList<QString> AppearanceManager::colorschemes_ =
    QStringList() << "default"
                  << "alpha dark"
                  << "smooth dark blue"
                  << "smooth dark green"
                  << "smooth dark orange"
                  << "smooth light blue"
                  << "smooth light green"
                  << "smooth light orange";
const QList<QString> AppearanceManager::stylesheet_path_ =
    QStringList() << ""
                  << ":style/alpha/dark.qss"
                  << ":style/smooth/dark-blue.qss"
                  << ":style/smooth/dark-green.qss"
                  << ":style/smooth/dark-orange.qss"
                  << ":style/smooth/light-blue.qss"
                  << ":style/smooth/light-greem.qss"
                  << ":style/smooth/light-orange.qss";

int AppearanceManager::headerHeight = 40;
int AppearanceManager::commentHeaderHeight = 40;
QColor AppearanceManager::xColorCode = QColor(0, 172, 109, 100);
QColor AppearanceManager::yColorCode = QColor(204, 140, 91, 100);
QColor AppearanceManager::zColorCode = QColor(174, 129, 255, 100);
QColor AppearanceManager::xErrColorCode = QColor(255, 0, 0, 100);
QColor AppearanceManager::yErrColorCode = QColor(255, 0, 0, 100);
QColor AppearanceManager::noneColorCode = QColor(150, 150, 150, 100);

int AppearanceManager::colorCodeThickness = 0;
int AppearanceManager::colorCodeXPadding = 4;
int AppearanceManager::colorCodeYPadding =
    (AppearanceManager::colorCodeThickness / 2) + 6;

AppearanceManager::~AppearanceManager() {}

void AppearanceManager::load(
    const AppearanceManager::ColorScheme &colorscheme) {
  Q_UNUSED(colorscheme);
}

QString AppearanceManager::colorScheme(
    const AppearanceManager::ColorScheme &colorscheme) {
  return colorschemes_.at(static_cast<int>(colorscheme));
}

QString AppearanceManager::colorSchemePath(
    const AppearanceManager::ColorScheme &colorscheme) {
  return stylesheet_path_.at(static_cast<int>(colorscheme));
}

QStringList AppearanceManager::colorSchemeNames() { return colorschemes_; }

QStringList AppearanceManager::colorSchemePaths() { return stylesheet_path_; }
