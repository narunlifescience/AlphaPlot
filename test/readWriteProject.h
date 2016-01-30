#include "ApplicationWindow.h"
#include <QTest>

class ReadWriteProjectTest: public ApplicationWindow
{
  Q_OBJECT
private slots:
  void readWriteProject();
  void largeOriginImport();
  
  void showHelp();
  void chooseHelpFolder();
};
