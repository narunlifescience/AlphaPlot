#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include <QIcon>

class Folder;
class Ui_PropertiesDialog;

class PropertiesDialog : public QDialog {
  Q_OBJECT
 public:
  explicit PropertiesDialog(QWidget *parent = nullptr);
  ~PropertiesDialog();

  struct Properties {
    QPixmap icon;
    QString name;
    QString type;
    QString status;
    QString path;
    QString size;
    QString content;
    QString created;
    QString modified;
    QString label;
    QString description;
  };

  void setupProperties(const Properties &properties)  const;

 private:
  Ui_PropertiesDialog *ui_;
};

#endif  // PROPERTIESDIALOG_H
