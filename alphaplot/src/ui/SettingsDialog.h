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

   Description : central settings dialog
*/

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMenu>
#include <QModelIndex>

class aSettingsListView;
class SettingsPage;
class Ui_SettingsDialog;

class QScrollArea;
class QStandardItemModel;

class SettingsDialog : public QDialog {
  Q_OBJECT

 public:
  explicit SettingsDialog(QWidget *parent = nullptr);
  ~SettingsDialog();

  enum Catagory { General, Table, Plot2d, Plot3d, Fitting, Scripting };

  enum Page {
    Page_RootSettings = 0,
    // General
    Page_GeneralApplication = 1,
    Page_GeneralConfirmation = 2,
    Page_GeneralAppearance = 3,
    Page_GeneralNumericFormat = 4,
    // Table
    Page_TableBasic = 5,
    Page_TableColor = 6,
    Page_TableFont = 7,
    // 2D Plots
    /*Page_Plot2dFont = 7,
    Page_Plot2dPrint = 8,
    Page_Plot2dTick = 9,
    Page_Plot2dDefaultStyle = 10,
    Page_Plot2dOption = 11,
    // 3D Plots
    Page_Plot3dOption = 12,
    Page_Plot3dColor = 13,
    Page_Plot3dFont = 14,*/
    // Fitting
    Page_Fitting = 8,
    //Page_FittingOutput = 16,
    // Scripting
    Page_ScriptingColor = 9
  };

 signals:
  void generalapplicationsettingsupdates();
  void generalconfirmationsettingsupdates();
  void generalappreancesettingsupdates();
  void generalnumericformatsettingsupdates();
  void tablebasicsettingsupdates();
  void tablecolorsettingsupdates();
  void tablefontsettingsupdates();

 protected:
  QSize sizeHint() const;
  void resizeEvent(QResizeEvent *);

 private:
  void addPage(Catagory catogory, Page id, SettingsPage *page);
  void handleResizeBeforeListviewVisible(aSettingsListView *&listView);
  void clearAllSelection();

  Ui_SettingsDialog *ui_;
  QColor baseColor_;
  QColor fontColor_;
  QMenu *settingsViewMenu_;
  QActionGroup *settingsViewGroup_;
  QAction *treeView_;
  QAction *iconView_;
  QStandardItemModel *generalSettingsModel_;
  QStandardItemModel *tableSettingsModel_;
  QStandardItemModel *plot2dSettingsModel_;
  QStandardItemModel *plot3dSettingsModel_;
  QStandardItemModel *fittingSettingsModel_;
  QStandardItemModel *scriptingSettingsModel_;

 private slots:
  void getBackToRootSettingsPage();
  // aListView selection (porting to QT5 should make this lambda functions)
  void generalEnsureSelection(const QModelIndex &index);
  void tableEnsureSelection(const QModelIndex &index);
  void plot2dEnsureSelection(const QModelIndex &index);
  void plot3dEnsureSelection(const QModelIndex &index);
  void fittingEnsureSelection(const QModelIndex &index);
  void scriptingEnsureSelection(const QModelIndex &index);
};

#endif  // SETTINGSDIALOG_H
