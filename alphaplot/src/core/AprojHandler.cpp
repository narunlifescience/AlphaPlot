#include "AprojHandler.h"

#include <QFile>
#include <QMdiSubWindow>
#include <QMessageBox>

#include "2Dplot/Layout2D.h"
#include "3Dplot/Layout3D.h"
#include "ApplicationWindow.h"
#include "Folder.h"
#include "Matrix.h"
#include "Note.h"
#include "Table.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"
#include "widgets/propertyeditor.h"

const QString AprojHandler::xmlschemafile_ = ":xmlschema/aproj.xsd";

AprojHandler::AprojHandler(ApplicationWindow *app)
    : QObject(app), app_(app), recursivecount_(0) {
  Q_ASSERT(app_);
}

AprojHandler::~AprojHandler() {}

ApplicationWindow *AprojHandler::openproject(const QString &filename) {
  QFile *file = nullptr;
  if (filename.endsWith(".gz", Qt::CaseInsensitive) ||
      filename.endsWith(".gz~", Qt::CaseInsensitive)) {
    file = openCompressedFile(filename);
    if (!file) return nullptr;
  } else {
    file = new QFile(filename);
    if (!file->open(QIODevice::ReadOnly | QFile::Text)) {
      qDebug() << "unable to open " << filename;
      delete file;
      return nullptr;
    }
  }

  // check xml schema
  /*if (!checkXmlSchema(filename)) {
    qDebug() << QString("unable to validate %1 using xml schema").arg(filename);
    delete file;

    if (QFile::exists(filename + "~")) {
      int choice = QMessageBox::question(
          app_, tr("File opening error"),
          tr("The file <b>%1</b> is corrupted, but there exists a backup "
             "copy.<br>Do you want to open the backup instead?")
              .arg(filename),
          QMessageBox::Yes | QMessageBox::Default,
          QMessageBox::No | QMessageBox::Escape);
      if (choice == QMessageBox::Yes) {
        QMessageBox::information(app_, tr("Opening backup copy"),
                                 tr("The original (corrupt) file is being left "
                                    "untouched, in case you want to try "
                                    "rescuing data manually. If you want to "
                                    "continue working with the automatically "
                                    "restored backup copy, you have to "
                                    "explicitly overwrite the original file."));
        return openproject(filename + "~");
      }
    }
    QMessageBox::critical(
        app_, tr("File opening error"),
        tr("The file <b>%1</b> is not a valid project file.").arg(filename));
    return nullptr;
  }*/

  if (!file) {
    qDebug() << "unable to open " << filename;
    return nullptr;
  }

  recursivecount_ = 0;
  ApplicationWindow *app = new ApplicationWindow();
  app->applyUserSettings();
  app->projectname = filename;
  app->setWindowTitle(tr("AlphaPlot") + " - " + filename);
  app->blockFolderviewsignals(true);
  app->blockSignals(true);
  // rename project folder item
  FolderTreeWidgetItem *item = app->getProjectRootItem();

  Folder *cfolder = readxmlstream(app, file, filename, item);
  file->close();
  delete file;

  app->blockFolderviewsignals(false);
  app->blockSignals(false);
  app->renamedTables.clear();

  app->show();
  app->executeNotes();
  app->savedProject();

  app->recentProjects.removeAll(filename);
  app->recentProjects.push_front(filename);
  app->updateRecentProjectsList();

  // change folder to user defined current folder
  // force update
  app->changeFolder(app->projectFolder(), true);
  // set current folder
  if (cfolder) {
    app->changeFolder(cfolder, false);
    app->setCurrentFolderViewItem(cfolder->folderTreeWidgetItem());
  }

  return app;
}

void AprojHandler::appendproject(const QString &filename) {
  if (filename.isEmpty()) return;
  QFile *file = nullptr;

  QFileInfo fi(filename);
  app_->workingDir = fi.absolutePath();

  if (filename.contains(".aproj")) {
    QFileInfo fileinfo(filename);
    if (!fileinfo.exists()) {
      QMessageBox::critical(
          app_, tr("File opening error"),
          tr("The file: <b>%1</b> doesn't exist!").arg(filename));
      return;
    }
  } else {
    QMessageBox::critical(
        app_, tr("File opening error"),
        tr("The file: <b>%1</b> is not a AlphaPlot project file!")
            .arg(filename));
    return;
  }

  if (filename.endsWith(".gz", Qt::CaseInsensitive) ||
      filename.endsWith(".gz~", Qt::CaseInsensitive)) {
    file = openCompressedFile(filename);
    if (!file) return;
  } else {
    file = new QFile(filename);
    file->open(QIODevice::ReadOnly);
  }

  app_->recentProjects.removeAll(filename);
  app_->recentProjects.push_front(filename);
  app_->updateRecentProjectsList();

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  Folder *cfolder = app_->current_folder;
  FolderTreeWidgetItem *item = static_cast<FolderTreeWidgetItem *>(
      app_->current_folder->folderTreeWidgetItem());
  app_->blockFolderviewsignals(true);
  app_->blockSignals(true);

  QString baseName = fi.baseName();
  QStringList lst = app_->current_folder->subfolders();
  int n = lst.count(baseName);
  if (n) {  // avoid identical subfolder names
    while (lst.count(baseName + QString::number(n))) n++;
    baseName += QString::number(n);
  }

  readxmlstream(app_, file, filename, item);
  file->close();
  delete file;

  app_->blockFolderviewsignals(false);
  // change folder to user defined current folder
  app_->changeFolder(cfolder);
  app_->blockSignals(false);
  app_->renamedTables = QStringList();
  QApplication::restoreOverrideCursor();
}

MyWidget *AprojHandler::opentemplate(const QString &filename) {
  QFile *file = new QFile(filename);
  if (!file->open(QIODevice::ReadOnly | QFile::Text)) {
    qDebug() << "unable to open " << filename;
    delete file;
    return nullptr;
  }

  MyWidget *mywidget = nullptr;
  std::unique_ptr<XmlStreamReader> xmlreader =
      std::unique_ptr<XmlStreamReader>(new XmlStreamReader(file));
  QXmlStreamReader::TokenType token;
  bool istemplate = false;
  while (!xmlreader->atEnd()) {
    token = xmlreader->readNext();
    if (token == QXmlStreamReader::StartElement &&
        (xmlreader->name() == "amt" || xmlreader->name() == "apt" ||
         xmlreader->name() == "att" || xmlreader->name() == "ast")) {
      QXmlStreamAttributes attributes = xmlreader->attributes();
      istemplate = true;
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "table" && istemplate) {
      Table *table = app_->newTable("table", 1, 1);
      table->d_future_table->load(xmlreader.get());
      mywidget = table;
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "matrix" && istemplate) {
      Matrix *matrix = app_->newMatrix("matrix", 1, 1);
      matrix->d_future_matrix->load(xmlreader.get());
      mywidget = matrix;
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "plot2d" && istemplate) {
      Layout2D *plot2d = app_->newGraph2D();
      plot2d->load(xmlreader.get(), tables(app_), matrixs(app_));
      // (hack) for some unknown reason this connection need to be manually set
      // here
      foreach (AxisRect2D *axisrect, plot2d->getAxisRectList()) {
        app_->propertyeditor->axisrectConnections(axisrect);
      }
      mywidget = plot2d;
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "plot3d" && istemplate) {
      bool ok = false;
      Layout3D *plot = nullptr;
      QString ptype = xmlreader->readAttributeString("type", &ok);
      if (ok) {
        if (ptype == "surface")
          plot = app_->newGraph3D(Graph3DCommon::Plot3DType::Surface);
        else if (ptype == "bar")
          plot = app_->newGraph3D(Graph3DCommon::Plot3DType::Bar);
        else if (ptype == "scatter")
          plot = app_->newGraph3D(Graph3DCommon::Plot3DType::Scatter);
        else
          xmlreader->raiseError(tr("Layout3D PlotType unknown %1").arg(ptype));
      } else
        xmlreader->raiseError(tr("Layout3D PlotType missing or empty"));
      plot->load(xmlreader.get(), tables(app_), matrixs(app_), app_);
      mywidget = plot;
    } else if (token == QXmlStreamReader::StartElement && !istemplate) {
      QMessageBox::critical(
          app_, tr("File opening error"),
          tr("The file: <b>%1</b> is not a AlphaPlot template file!")
              .arg(filename));
      return nullptr;
    }
  }

  if (xmlreader->hasWarnings()) {
    foreach (QString warning, xmlreader->warningStrings()) {
      qDebug() << warning;
    }
  }

  return mywidget;
}

Folder *AprojHandler::readxmlstream(ApplicationWindow *app, QFile *file,
                                    const QString &filename,
                                    FolderTreeWidgetItem *rootitem) {
  Folder *cfolder = nullptr;
  QFileInfo fileinfo(filename);
  QString baseName = fileinfo.fileName();
  std::unique_ptr<XmlStreamReader> xmlreader =
      std::unique_ptr<XmlStreamReader>(new XmlStreamReader(file));
  QXmlStreamReader::TokenType token;
  while (!xmlreader->atEnd()) {
    token = xmlreader->readNext();
    if (token == QXmlStreamReader::StartElement &&
        xmlreader->name() == "aproj") {
      QXmlStreamAttributes attributes = xmlreader->attributes();
      if (attributes.hasAttribute("scripting")) {
        if (!app->setScriptingLang(attributes.value("scripting").toString(),
                                   true))
          QMessageBox::warning(
              app, tr("File opening error"),
              tr("The file \"%1\" was created using \"%2\" as scripting "
                 "language.\n\n Initializing support for this language FAILED; "
                 "I'm using default instead.\nVarious parts of this file may "
                 "not be displayed as expected.")
                  .arg(filename)
                  .arg(attributes.value("scripting").toString()));
      }
      rootitem->setText(0, fileinfo.baseName());
      rootitem->folder()->setName(fileinfo.baseName());
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "folder") {
      recursivecount_++;
      QXmlStreamAttributes attr = xmlreader->attributes();
      QString name = xmlreader->attributes().value("name").toString();
      QString creation_time =
          xmlreader->attributes().value("creation_time").toString();
      QString lastmodified_time =
          xmlreader->attributes().value("lastmodified_time").toString();
      QString current = xmlreader->attributes().value("current").toString();
      Folder *folder = new Folder(app->current_folder, name);
      folder->setBirthDate(creation_time);
      folder->setModificationDate(lastmodified_time);
      FolderTreeWidgetItem *fli = new FolderTreeWidgetItem(
          app->current_folder->folderTreeWidgetItem(), folder);
      fli->setText(0, name);
      folder->setFolderTreeWidgetItem(fli);
      app->current_folder = folder;
      if (current.trimmed() == "true") cfolder = folder;
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "table") {
      bool ok;
      QString time = xmlreader->readAttributeString("creation_time", &ok);
      Table *table = app->newTable("table", 1, 1);
      table->d_future_table->load(xmlreader.get());
      // unable to set datetime via above method for some unknown reason. Thus
      // had to do it here (find the cause of this issue and remove this later)
      if (ok) {
        table->setBirthDate(
            QDateTime::fromString(time, "yyyy-dd-MM hh:mm:ss:zzz")
                .toString(Qt::LocalDate));
      } else {
        table->setBirthDate(
            QDateTime::currentDateTime().toString(Qt::LocalDate));
        xmlreader->raiseWarning(
            tr("Invalid creation time. Using current time insted."));
      }
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "matrix") {
      Matrix *matrix = app->newMatrix("matrix", 1, 1);
      matrix->d_future_matrix->load(xmlreader.get());
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "note") {
      Note *note = app->newNote("caption");
      note->load(xmlreader.get());
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "plot2d") {
      Layout2D *plot2d = app->newGraph2D();
      plot2d->load(xmlreader.get(), tables(app), matrixs(app));
      // (hack) for some unknown reason this connection need to be manually set
      // here
      foreach (AxisRect2D *axisrect, plot2d->getAxisRectList()) {
        app->propertyeditor->axisrectConnections(axisrect);
      }
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "plot3d") {
      bool ok = false;
      Layout3D *plot = nullptr;
      QString ptype = xmlreader->readAttributeString("type", &ok);
      if (ok) {
        if (ptype == "surface")
          plot = app->newGraph3D(Graph3DCommon::Plot3DType::Surface);
        else if (ptype == "bar")
          plot = app->newGraph3D(Graph3DCommon::Plot3DType::Bar);
        else if (ptype == "scatter")
          plot = app->newGraph3D(Graph3DCommon::Plot3DType::Scatter);
        else
          xmlreader->raiseError(tr("Layout3D PlotType unknown %1").arg(ptype));
      } else
        xmlreader->raiseError(tr("Layout3D PlotType missing or empty"));
      plot->load(xmlreader.get(), tables(app), matrixs(app), app);
    } else if (token == QXmlStreamReader::StartElement &&
               xmlreader->name() == "log") {
      QXmlStreamAttributes attributes = xmlreader->attributes();
      if (attributes.hasAttribute("value")) {
        QString loginfo = attributes.value("value").toString();
        app->showResults(loginfo);
      } else
        xmlreader->raiseWarning(tr("Invalid attribute '%1' log element. "
                                   "skipping the attribute now.")
                                    .arg("value"));
    } else if (token == QXmlStreamReader::EndElement &&
               xmlreader->name() == "folder") {
      while (recursivecount_ > 0) {
        Folder *fl = static_cast<Folder *>(app->current_folder->parent());
        if (fl) {
          app->current_folder = fl;
        }
        recursivecount_--;
      }
    }
  }

  if (xmlreader->hasWarnings()) {
    foreach (QString warning, xmlreader->warningStrings()) {
      qDebug() << warning;
    }
  }

  return cfolder;
}

QFile *AprojHandler::openCompressedFile(const QString &filename) {
  QTemporaryFile *tempfile = new QTemporaryFile;
  std::unique_ptr<QFile> file = std::unique_ptr<QFile>(new QFile(filename));
  if (!file->open(QIODevice::ReadOnly) || !tempfile->open()) {
    qDebug() << "unable to open " << filename;
    return nullptr;
  }
  std::unique_ptr<QByteArray> bytearray =
      std::unique_ptr<QByteArray>(new QByteArray);

  *bytearray.get() = file->readAll();
  tempfile->write(qUncompress(*bytearray.get()));
  bytearray->clear();
  tempfile->reset();
  return tempfile;
}

bool AprojHandler::saveproject(const QString &filename, Folder *folder) {
  bool compress = false;
  if (filename.endsWith(".gz")) compress = true;

  std::unique_ptr<XmlStreamWriter> xmlwriter;
  std::unique_ptr<QFile> file = std::unique_ptr<QFile>(new QFile(filename));
  if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "failed to open xml file for writing";
    return false;
  }
  std::unique_ptr<QByteArray> bytearray =
      std::unique_ptr<QByteArray>(new QByteArray);

  if (!compress) {
    // Uncompressed file save
    xmlwriter =
        std::unique_ptr<XmlStreamWriter>(new XmlStreamWriter(file.get()));
  } else {
    // Compressed file save
    bytearray = std::unique_ptr<QByteArray>(new QByteArray);
    xmlwriter =
        std::unique_ptr<XmlStreamWriter>(new XmlStreamWriter(bytearray.get()));
  }

  xmlwriter->setCodec("UTF-8");

  xmlwriter->setAutoFormatting(false);
  Folder *root = folder;
  xmlwriter->writeStartDocument();
  xmlwriter->writeComment("AlphaPlot project file");
  xmlwriter->writeStartElement("aproj");
  xmlwriter->writeAttribute("version", QString::number(AlphaPlot::version()));
  xmlwriter->writeAttribute("scripting", "muParser");
  xmlwriter->writeAttribute("windows",
                            QString::number(root->windowCount(true)));

  recursivecount_ = 0;
  saveTreeRecursive(root, xmlwriter.get());
  xmlwriter->writeStartElement("log");
  xmlwriter->writeAttribute("value", app_->getLogInfoText());
  xmlwriter->writeEndElement();
  xmlwriter->writeEndElement();
  xmlwriter->writeEndDocument();
  // Compressed file
  if (compress) {
    file->write(qCompress(*bytearray.get()));
  }
  file->commitTransaction();
  return true;
}

void AprojHandler::saveTreeRecursive(Folder *folder,
                                     XmlStreamWriter *xmlwriter) {
  // make sure that plots are saved after tables & metrices
  QList<MyWidget *> others;
  QList<MyWidget *> plots;
  foreach (MyWidget *subwindow, folder->windowsList()) {
    (qobject_cast<Layout2D *>(subwindow) || qobject_cast<Layout3D *>(subwindow))
        ? plots << subwindow
        : others << subwindow;
  }
  others.append(plots);
  foreach (MyWidget *subwindow, others) {
    if (qobject_cast<Table *>(subwindow)) {
      Table *table = qobject_cast<Table *>(subwindow);
      table->d_future_table->save(xmlwriter);
    } else if (qobject_cast<Matrix *>(subwindow)) {
      Matrix *matrix = qobject_cast<Matrix *>(subwindow);
      matrix->d_future_matrix->save(xmlwriter);
    } else if (qobject_cast<Note *>(subwindow)) {
      Note *note = qobject_cast<Note *>(subwindow);
      note->save(xmlwriter);
    } else if (qobject_cast<Layout2D *>(subwindow)) {
      Layout2D *graph = qobject_cast<Layout2D *>(subwindow);
      graph->save(xmlwriter);
    } else if (qobject_cast<Layout3D *>(subwindow)) {
      Layout3D *graph = qobject_cast<Layout3D *>(subwindow);
      graph->save(xmlwriter);
    }
  }
  foreach (Folder *subfolder, folder->folders()) {
    recursivecount_++;
    xmlwriter->writeStartElement("folder");
    xmlwriter->writeAttribute("name", subfolder->name());
    xmlwriter->writeAttribute("creation_time", subfolder->birthDate());
    xmlwriter->writeAttribute("lastmodified_time",
                              subfolder->modificationDate());

    (subfolder == app_->currentFolder())
        ? xmlwriter->writeAttribute("current", "true")
        : xmlwriter->writeAttribute("current", "false");

    saveTreeRecursive(subfolder, xmlwriter);
    while (recursivecount_ > 0) {
      xmlwriter->writeEndElement();
      recursivecount_--;
    }
  }
}

bool AprojHandler::saveTemplate(const QString &filename, MyWidget *mywidget) {
  QString fname = filename;
  std::unique_ptr<QFile> file = std::unique_ptr<QFile>(new QFile(fname));
  if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "failed to open xml file for writing";
    return false;
  }
  QString selectedfilter = QString();
  (fname.endsWith(".amt"))   ? selectedfilter = "amt"
  : (fname.endsWith(".apt")) ? selectedfilter = "apt"
  : (fname.endsWith(".att")) ? selectedfilter = "att"
  : (fname.endsWith(".ast")) ? selectedfilter = "att"
                             : selectedfilter = QString();
  if (selectedfilter.isEmpty()) {
    qDebug() << "unknown selected filter: ." << selectedfilter;
    return false;
  }
  std::unique_ptr<XmlStreamWriter> xmlwriter =
      std::unique_ptr<XmlStreamWriter>(new XmlStreamWriter(file.get()));
  xmlwriter->setCodec("UTF-8");
  xmlwriter->setAutoFormatting(false);

  xmlwriter->writeStartDocument();
  xmlwriter->writeComment("AlphaPlot Template file");
  xmlwriter->writeStartElement(selectedfilter);
  xmlwriter->writeAttribute("version", QString::number(AlphaPlot::version()));
  if (qobject_cast<Table *>(mywidget)) {
    Table *table = qobject_cast<Table *>(mywidget);
    table->d_future_table->save(xmlwriter.get(), true);
  } else if (qobject_cast<Matrix *>(mywidget)) {
    Matrix *matrix = qobject_cast<Matrix *>(mywidget);
    matrix->d_future_matrix->save(xmlwriter.get(), true);
  } else if (qobject_cast<Layout2D *>(mywidget)) {
    Layout2D *graph = qobject_cast<Layout2D *>(mywidget);
    graph->save(xmlwriter.get(), true);
  } else if (qobject_cast<Layout3D *>(mywidget)) {
    Layout3D *graph = qobject_cast<Layout3D *>(mywidget);
    graph->save(xmlwriter.get(), true);
  }
  xmlwriter->writeEndElement();
  xmlwriter->writeEndDocument();
  file->close();
  return true;
}

QList<Table *> AprojHandler::tables(ApplicationWindow *app) {
  QList<QMdiSubWindow *> subwindowlist = app->subWindowsList();
  QList<Table *> tables;
  foreach (QMdiSubWindow *subwindow, subwindowlist) {
    Table *tab = qobject_cast<Table *>(subwindow);
    if (tab) {
      tables << tab;
    }
  }
  return tables;
}

QList<Matrix *> AprojHandler::matrixs(ApplicationWindow *app) {
  QList<QMdiSubWindow *> subwindowlist = app->subWindowsList();
  QList<Matrix *> matrixs;
  foreach (QMdiSubWindow *subwindow, subwindowlist) {
    Matrix *mat = qobject_cast<Matrix *>(subwindow);
    if (mat) {
      matrixs << mat;
    }
  }
  return matrixs;
}

bool AprojHandler::checkXmlSchema(const QString &filename) {
  Q_UNUSED(filename);
  /*std::unique_ptr<MessageHandler> messageHandler =
      std::unique_ptr<MessageHandler>(new MessageHandler);
  std::unique_ptr<QFile> xsdfile =
      std::unique_ptr<QFile>(new QFile(xmlschemafile_));
  if (!xsdfile->open(QIODevice::ReadOnly)) {
    qDebug()
        << QString("unable to open xml schema file %1").arg(xmlschemafile_);
    return false;
  }
  std::unique_ptr<QXmlSchema> schema =
      std::unique_ptr<QXmlSchema>(new QXmlSchema);
  schema->setMessageHandler(messageHandler.get());
  bool errorOccurred = false;
  if (schema->load(xsdfile.get(), QUrl::fromLocalFile(xsdfile->fileName())) ==
      false)
    errorOccurred = true;
  else {
    std::unique_ptr<QXmlSchemaValidator> xmlvalidator =
        std::unique_ptr<QXmlSchemaValidator>(
            new QXmlSchemaValidator(*schema.get()));
    std::unique_ptr<QFile> xmlfile =
        std::unique_ptr<QFile>(new QFile(filename));
    xmlfile->open(QIODevice::ReadOnly);
    if (!xmlvalidator->validate(xmlfile.get(),
                                QUrl::fromLocalFile(xmlfile->fileName())))
      errorOccurred = true;
    xmlfile->close();
  }
  xsdfile->close();
  if (errorOccurred) {
    QString msgstring = messageHandler->statusMessage();
    QMessageBox::critical(app_, tr("File opening error"),
                          tr("Unable to validate xml schema; error at Line %1, "
                             "Row %2, Error Msg %3")
                              .arg(messageHandler->line())
                              .arg(messageHandler->column())
                              .arg(msgstring.toUtf8().constData()));
    return false;
  } else {
    return true;
  }*/
  return false;
}

// xml schema message handling class
/*MessageHandler::MessageHandler(QObject *parent)
    : QAbstractMessageHandler(parent) {}

QString MessageHandler::statusMessage() const { return m_description; }

int MessageHandler::line() const {
  return static_cast<int>(m_sourceLocation.line());
}

int MessageHandler::column() const {
  return static_cast<int>(m_sourceLocation.column());
}

void MessageHandler::handleMessage(QtMsgType type, const QString &description,
                                   const QUrl &identifier,
                                   const QSourceLocation &sourceLocation) {
  Q_UNUSED(type);
  Q_UNUSED(identifier);
  m_description = description;
  m_sourceLocation = sourceLocation;
}*/
