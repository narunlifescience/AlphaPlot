#ifndef APPEARANCEMANAGER_H
#define APPEARANCEMANAGER_H

#include <QList>

class QColor;

class AppearanceManager {
 public:
  enum class ColorScheme : int {
    Default = 0,
    AlphaDark = 1,
  };
  ~AppearanceManager();
  // Table header/comment height
  static int headerHeight;
  static int commentHeaderHeight;

  // Table header color codes
  static QColor xColorCode;
  static QColor yColorCode;
  static QColor zColorCode;
  static QColor xErrColorCode;
  static QColor yErrColorCode;
  static QColor noneColorCode;

  // Table header color code geometry
  static int colorCodeThickness;
  static int colorCodeXPadding;
  static int colorCodeYPadding;

  static void load(const ColorScheme &colorscheme);
  static QString colorScheme(const ColorScheme &colorscheme);
  static QString colorSchemePath(const ColorScheme &colorscheme);
  static QStringList colorSchemeNames();
  static QStringList colorSchemePaths();

 private:
  AppearanceManager(){};
  static const QList<QString> colorschemes_;
  static const QList<QString> stylesheet_path_;
};

#endif  // APPEARANCEMANAGER_H
