/* This file is part of AlphaPlot.

   Copyright 2006 by Ion Vasilief, Tilman Benkert
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : AlphaPlot main function
*/

#include "ApplicationWindow.h"

#include <QAction>
#include <QApplication>
#include <QSplashScreen>
#include <QTimer>

#include <typeinfo>

/*page style Coding Style
  The following rules are not used everywhere (yet), but are
  intended as guidelines for new code and eventually old code
  should be adapted as well.

  - Files use Unix-style line endings ("\n").
  - Class names start with a capital letter, object/variable
    names with a lower case letter.
  - Methods and functions are named likeThis(), variables
    likeThis and instance variables are prefixed with "d_".
  - Property access methods use Qt style: property() and
    setProperty().
  - Every file should be named exactly like the class inside
    and there should be only one class per file, with the
    exception of really short classes like LayerButton.
  - For indentations, spacess are preferred because they allow
    everyone to choose the indentation depth for him/herself.
    we prefer Qt Creator as an IDE editor for this project or
    if you prefer text editor then, Sublime text editor should
    be appropriate. we prefer monokai color scheme.
    General format used through out the project should be
    Google style(clang format) with a line wrap of 80 characters.
*/

struct Application : public QApplication {
  Application(int& argc, char** argv) : QApplication(argc, argv) {}

  // catch exception, and display their contents as msg box.
  bool notify(QObject* receiver, QEvent* event) {
    try {
      return QApplication::notify(receiver, event);
    } catch (const std::exception& e) {
      QMessageBox::critical(0, tr("Error!"),
                            tr("Error ") + e.what() + tr(" sending event ") +
                                typeid(*event).name() + tr(" to object ") +
                                qPrintable(receiver->objectName()) + " \"" +
                                typeid(*receiver).name() + "\"");
    } catch (...) /* shouldn't happen..*/ {
      QMessageBox::critical(0, tr("Error!"),
                            tr("Error <unknown> sending event") +
                                typeid(*event).name() + tr(" to object ") +
                                qPrintable(receiver->objectName()) + " \"" +
                                typeid(*receiver).name() + "\"");
    }
    // qFatal aborts, so this isn't really necessary.
    return false;
  }
};

int main(int argc, char** argv) {
  Application app(argc, argv);
  app.setApplicationName("AlphaPlot");
  // app.setApplicationVersion("version"); //not set
  app.setOrganizationName("AlphaPlot");
  // app.setOrganizationDomain("website"); //not set

  QStringList args = app.arguments();
  args.removeFirst();  // remove application name

  if ((args.count() == 1) && (args[0] == "-a" || args[0] == "--about")) {
    ApplicationWindow::about();
    exit(0);
  } else {
    ApplicationWindow* mw = new ApplicationWindow();

    // Show splashscreen
    QPixmap pixmap(":splash/splash.png");
    QSplashScreen* splash = new QSplashScreen(pixmap);
    splash->show();

    // Process more events here before starting app.
    mw->applyUserSettings();
    mw->newTable();
    mw->savedProject();

#ifdef SEARCH_FOR_UPDATES
    if (mw->autoSearchUpdates) {
      mw->autoSearchUpdatesRequest = true;
      mw->searchForUpdates();
    }
#endif
    mw->parseCommandLineArguments(args);

    // Close splashscreen after 1 sec & show mainwindow
    QTimer::singleShot(3000, splash, SLOT(close()));
    QTimer::singleShot(3000, mw, SLOT(activateWindow()));
  }
  app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
  return app.exec();
}
