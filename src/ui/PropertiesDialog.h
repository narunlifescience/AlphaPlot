/* This file is part of AlphaPlot.
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

   Description : properties dialog
*/

#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDialog>
#include <QIcon>

class Folder;
class Ui_PropertiesDialog;

class PropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PropertiesDialog(QWidget *parent = nullptr);
    ~PropertiesDialog();

    struct Properties
    {
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

    void setupProperties(const Properties &properties) const;

private:
    Ui_PropertiesDialog *ui_;
};

#endif // PROPERTIESDIALOG_H
