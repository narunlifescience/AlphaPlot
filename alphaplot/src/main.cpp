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

#include <QAction>
#include <QApplication>
#include <QSplashScreen>
#include <QTimer>
#include <QtMsgHandler>
#include <typeinfo>

#include "ApplicationWindow.h"
#include "core/IconLoader.h"
#include "globals.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif  // Q_OS_WIN

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
class Delay : public QThread {
 public:
  static void sleep(unsigned long secs) { QThread::sleep(secs); }
};

struct Application : public QApplication {
  Application(int& argc, char** argv);

  // catch exception, and display their contents as msg box.
  bool notify(QObject* receiver, QEvent* event);
};

Application::Application(int& argc, char** argv) : QApplication(argc, argv) {}

bool Application::notify(QObject* receiver, QEvent* event) {
  {
    try {
      return QApplication::notify(receiver, event);
    } catch (const std::exception& e) {
      QMessageBox::critical(nullptr, tr("Error!"),
                            tr("Error ") + e.what() + tr(" sending event ") +
                                typeid(*event).name() + tr(" to object ") +
                                qPrintable(receiver->objectName()) + " \"" +
                                typeid(*receiver).name() + "\"");
    } catch (...) /* shouldn't happen..*/ {
      QMessageBox::critical(nullptr, tr("Error!"),
                            tr("Error <unknown> sending event") +
                                typeid(*event).name() + tr(" to object ") +
                                qPrintable(receiver->objectName()) + " \"" +
                                typeid(*receiver).name() + "\"");
    }
    // qFatal aborts, so this isn't really necessary.
    return false;
  }
}

// Custom message handler for total control
void logOutput(QtMsgType type, const QMessageLogContext& context,
               const QString& msg) {
  QByteArray localMsg = msg.toLocal8Bit();
  switch (type) {
    case QtDebugMsg:
      fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(),
              context.file, context.line, context.function);
      break;
    case QtInfoMsg:
      fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(),
              context.file, context.line, context.function);
      break;
    case QtWarningMsg:
      fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(),
              context.file, context.line, context.function);
      break;
    case QtCriticalMsg:
      fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(),
              context.file, context.line, context.function);
      break;
    case QtFatalMsg:
      fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(),
              context.file, context.line, context.function);
      abort();
  }
}

int main(int argc, char** argv) {
  qInstallMessageHandler(logOutput);
#ifdef Q_OS_WIN
  // solves high density dpi scaling in windows
  // https://vicrucann.github.io/tutorials/osg-qt-high-dpi/
  // SetProcessDPIAware();
  // call before the main event loop
#endif  // Q_OS_WIN

  // https://vicrucann.github.io/tutorials/osg-qt-high-dpi/
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  Application* app = new Application(argc, argv);

  // icon initiation (mandatory)
  IconLoader::init();
  IconLoader::lumen_ =
      IconLoader::isLight(app->palette().color(QPalette::Window));

  app->setApplicationName("AlphaPlot");
  app->setApplicationVersion(AlphaPlot::versionString());
  app->setOrganizationName("AlphaPlot");
  app->setOrganizationDomain("alphaplot.sourceforge.net");

  QStringList args = app->arguments();
  args.removeFirst();  // remove application name

  // Show splashscreen
  QPixmap pixmap(":splash/splash.png");
  QSplashScreen* splash = new QSplashScreen(pixmap);
  if (args.count() == 0) {
    splash->show();
    // Close splashscreen after 3 sec
    Delay::sleep(3);
  }

  ApplicationWindow* mw = new ApplicationWindow();
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

  mw->activateWindow();
  splash->finish(mw);

  app->connect(app, &Application::lastWindowClosed, app, &Application::quit);
  return app->exec();
}
