#ifndef APROJHANDLER_H
#define APROJHANDLER_H

#include <QAbstractMessageHandler>
#include <QObject>

class ApplicationWindow;
class QFile;
class Folder;
class FolderTreeWidgetItem;
class XmlStreamWriter;
class Table;
class Matrix;

class AprojHandler : public QObject {
  Q_OBJECT
 public:
  AprojHandler(ApplicationWindow *app);
  ~AprojHandler();
  ApplicationWindow *openproject(const QString &filename);
  void appendproject(const QString &filename);
  Folder *readxmlstream(ApplicationWindow *app, QFile *file,
                        const QString &filename,
                        FolderTreeWidgetItem *rootitem);

  bool saveproject(const QString &filename, Folder *folder);
  void saveTreeRecursive(Folder *folder, XmlStreamWriter *xmlwriter);
  QList<Table *> tables(ApplicationWindow *app);
  QList<Matrix *>matrixs(ApplicationWindow *app);

 private:
  QFile *openCompressedFile(const QString &filename);
  bool checkXmlSchema(const QString &filename);

 private:
  ApplicationWindow *app_;
  int recursivecount_;
  static const QString xmlschemafile_;
};

class MessageHandler : public QAbstractMessageHandler {
 public:
  MessageHandler(QObject *parent = nullptr);
  QString statusMessage() const;
  int line() const;
  int column() const;

 protected:
  virtual void handleMessage(QtMsgType type, const QString &description,
                             const QUrl &identifier,
                             const QSourceLocation &sourceLocation);

 private:
  QString m_description;
  QSourceLocation m_sourceLocation;
};

#endif  // APROJHANDLER_H
