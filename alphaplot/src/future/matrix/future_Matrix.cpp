/***************************************************************************
    File                 : Matrix.cpp
    Project              : AlphaPlot
    Description          : Aspect providing a spreadsheet to manage MxN matrix
 data
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2006-2009 Knut Franke (knut.franke*gmx.de)
    Copyright            : (C) 2006-2007 Ion Vasilief (ion_vasilief*yahoo.fr)
                           (replace * with @ in the email addresses)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#include "matrix/future_Matrix.h"

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QProgressDialog>
#include <QtCore>
#include <QtDebug>
#include <QtGui>

#include "../core/IconLoader.h"
#include "Matrix.h"
#include "core/future_Folder.h"
#include "lib/ActionManager.h"
#include "lib/XmlStreamReader.h"
#include "matrixcommands.h"

namespace future {

#define WAIT_CURSOR QApplication::setOverrideCursor(QCursor(Qt::WaitCursor))
#define RESET_CURSOR QApplication::restoreOverrideCursor()

int Matrix::default_column_width = 120;
int Matrix::default_row_height = 20;

#ifndef LEGACY_CODE_0_2_x
Matrix::Matrix(AbstractScriptingEngine *engine, int rows, int cols,
               const QString &name)
    : AbstractPart(name),
      d_plot_menu(0),
      scripted(engine)
#else
Matrix::Matrix(void *, int rows, int cols, const QString &name)
    : AbstractPart(name),
      d_plot_menu(nullptr),
      showonlyvalues_(false)
#endif
{
  d_matrix_private = new Private(this);
  // set initial number of rows and columns
  appendColumns(cols);
  appendRows(rows);

  d_view = nullptr;
  createActions();
  connectActions();
}

#ifndef LEGACY_CODE_0_2_x
Matrix::Matrix()
    : AbstractPart("temp"),
      scripted(0)
#else
Matrix::Matrix()
    : AbstractPart("temp"),
      showonlyvalues_(false)
#endif
{
  d_view = nullptr;
  createActions();
}

Matrix::~Matrix() {}

void Matrix::setView(MatrixView *view) {
  d_view = view;
  addActionsToView();
  connect(d_view, &MatrixView::controlTabBarStatusChanged, this,
          &Matrix::adjustTabBarAction);
  adjustTabBarAction(true);
}

QWidget *Matrix::view() {
#ifndef LEGACY_CODE_0_2_x
  if (!d_view) {
    d_view = new MatrixView(this);
    addActionsToView();
    connect(d_view, &MatrixView::controlTabBarStatusChanged, this,
            &Matrix::adjustTabBarAction);
    adjustTabBarAction(true);
  }
#else
  Q_ASSERT(d_view != NULL);
#endif
  return d_view;
}

void Matrix::insertColumns(int before, int count) {
  if (count < 1 || before < 0 || before > columnCount()) return;
  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: insert %2 column(s)").arg(name()).arg(count));
  exec(new MatrixInsertColumnsCmd(d_matrix_private, before, count));
  endMacro();
  RESET_CURSOR;
}

void Matrix::removeColumns(int first, int count) {
  if (count < 1 || first < 0 || first + count > columnCount()) return;
  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: remove %2 column(s)").arg(name()).arg(count));
  exec(new MatrixRemoveColumnsCmd(d_matrix_private, first, count));
  endMacro();
  RESET_CURSOR;
}

void Matrix::removeRows(int first, int count) {
  if (count < 1 || first < 0 || first + count > rowCount()) return;
  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: remove %2 row(s)").arg(name()).arg(count));
  exec(new MatrixRemoveRowsCmd(d_matrix_private, first, count));
  endMacro();
  RESET_CURSOR;
}

void Matrix::insertRows(int before, int count) {
  if (count < 1 || before < 0 || before > rowCount()) return;
  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: insert %2 row(s)").arg(name()).arg(count));
  exec(new MatrixInsertRowsCmd(d_matrix_private, before, count));
  endMacro();
  RESET_CURSOR;
}

void Matrix::setDimensions(int rows, int cols) {
  if ((rows < 0) || (cols < 0) || (rows == rowCount() && cols == columnCount()))
    return;
  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: set matrix size to %2x%3")
                 .arg(name())
                 .arg(rows)
                 .arg(cols));
  int col_diff = cols - columnCount();
  int row_diff = rows - rowCount();
  if (col_diff > 0)
    exec(new MatrixInsertColumnsCmd(d_matrix_private, columnCount(), col_diff));
  else if (col_diff < 0)
    exec(new MatrixRemoveColumnsCmd(d_matrix_private, columnCount() + col_diff,
                                    -col_diff));
  if (row_diff > 0)
    exec(new MatrixInsertRowsCmd(d_matrix_private, rowCount(), row_diff));
  else if (row_diff < 0)
    exec(new MatrixRemoveRowsCmd(d_matrix_private, rowCount() + row_diff,
                                 -row_diff));
  endMacro();
  RESET_CURSOR;
}

int Matrix::columnCount() const { return d_matrix_private->columnCount(); }

int Matrix::rowCount() const { return d_matrix_private->rowCount(); }

void Matrix::clear() {
  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: clear").arg(name()));
  exec(new MatrixClearCmd(d_matrix_private));
  endMacro();
  RESET_CURSOR;
}

double Matrix::cell(int row, int col) const {
  if (row < 0 || row >= rowCount() || col < 0 || col >= columnCount())
    return 0.0;
  return d_matrix_private->cell(row, col);
}

void Matrix::cutSelection() {
  if (!d_view) return;
  int first = d_view->firstSelectedRow();
  if (first < 0) return;

  WAIT_CURSOR;
  beginMacro(tr("%1: cut selected cell(s)").arg(name()));
  copySelection();
  clearSelectedCells();
  endMacro();
  RESET_CURSOR;
}

void Matrix::copySelection() {
  if (!d_view) return;
  int first_col = d_view->firstSelectedColumn(false);
  if (first_col == -1) return;
  int last_col = d_view->lastSelectedColumn(false);
  if (last_col == -2) return;
  int first_row = d_view->firstSelectedRow(false);
  if (first_row == -1) return;
  int last_row = d_view->lastSelectedRow(false);
  if (last_row == -2) return;
  int cols = last_col - first_col + 1;
  int rows = last_row - first_row + 1;

  WAIT_CURSOR;
  QString output_str;

  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      if (d_view->isCellSelected(first_row + r, first_col + c))
        output_str += QLocale().toString(cell(first_row + r, first_col + c),
                                         d_matrix_private->numericFormat(),
                                         16);  // copy with max. precision
      if (c < cols - 1) output_str += "\t";
    }
    if (r < rows - 1) output_str += "\n";
  }
  QApplication::clipboard()->setText(output_str);
  RESET_CURSOR;
}

void Matrix::pasteIntoSelection() {
  if (!d_view) return;
  if (columnCount() < 1 || rowCount() < 1) return;

  WAIT_CURSOR;
  beginMacro(tr("%1: paste from clipboard").arg(name()));

  int first_col = d_view->firstSelectedColumn(false);
  int last_col = d_view->lastSelectedColumn(false);
  int first_row = d_view->firstSelectedRow(false);
  int last_row = d_view->lastSelectedRow(false);
  int input_row_count = 0;
  int input_col_count = 0;
  int rows, cols;

  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();
  ;
  if (mimeData->hasText()) {
    QString input_str = QString(clipboard->text());
    QList<QStringList> cell_texts;
    QStringList input_rows(input_str.split(QRegExp("\\n|\\r\\n|\\r")));
    input_row_count = input_rows.count();
    input_col_count = 0;
    for (int i = 0; i < input_row_count; i++) {
      cell_texts.append(input_rows.at(i).split("\t"));
      if (cell_texts.at(i).count() > input_col_count)
        input_col_count = cell_texts.at(i).count();
    }

    if ((first_col == -1 || first_row == -1) ||
        (last_row == first_row && last_col == first_col))
    // if the is no selection or only one cell selected, the
    // selection will be expanded to the needed size from the current cell
    {
      int current_row, current_col;
      d_view->getCurrentCell(&current_row, &current_col);
      if (current_row == -1) current_row = 0;
      if (current_col == -1) current_col = 0;
      d_view->setCellSelected(current_row, current_col);
      first_col = current_col;
      first_row = current_row;
      last_row = first_row + input_row_count - 1;
      last_col = first_col + input_col_count - 1;
      // resize the matrix if necessary
      if (last_col >= columnCount())
        appendColumns(last_col + 1 - columnCount());
      if (last_row >= rowCount()) appendRows(last_row + 1 - rowCount());
      // select the rectangle to be pasted in
      d_view->setCellsSelected(first_row, first_col, last_row, last_col);
    }

    rows = last_row - first_row + 1;
    cols = last_col - first_col + 1;
    for (int r = 0; r < rows && r < input_row_count; r++) {
      for (int c = 0; c < cols && c < input_col_count; c++) {
        if (d_view->isCellSelected(first_row + r, first_col + c) &&
            (c < cell_texts.at(r).count())) {
          setCell(first_row + r, first_col + c,
                  cell_texts.at(r).at(c).toDouble());
        }
      }
    }
  }
  endMacro();
  RESET_CURSOR;
}

void Matrix::insertEmptyColumns() {
  if (!d_view) return;
  int first = d_view->firstSelectedColumn();
  int last = d_view->lastSelectedColumn();
  if (first < 0) return;
  int count, current = first;

  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: insert empty column(s)").arg(name()));
  while (current <= last) {
    current = first + 1;
    while (current <= last && d_view->isColumnSelected(current)) current++;
    count = current - first;
    insertColumns(first, count);
    current += count;
    last += count;
    while (current <= last && !d_view->isColumnSelected(current)) current++;
    first = current;
  }
  endMacro();
  RESET_CURSOR;
}

void Matrix::removeSelectedColumns() {
  if (!d_view) return;
  int first = d_view->firstSelectedColumn();
  int last = d_view->lastSelectedColumn();
  if (first < 0) return;

  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: remove selected column(s)").arg(name()));
  for (int i = last; i >= first; i--)
    if (d_view->isColumnSelected(i, false)) removeColumns(i, 1);
  endMacro();
  RESET_CURSOR;
}

void Matrix::clearSelectedColumns() {
  if (!d_view) return;
  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: clear selected column(s)").arg(name()));
  for (int i = 0; i < columnCount(); i++)
    if (d_view->isColumnSelected(i, false))
      exec(new MatrixClearColumnCmd(d_matrix_private, i));
  endMacro();
  RESET_CURSOR;
}

void Matrix::insertEmptyRows() {
  if (!d_view) return;
  int first = d_view->firstSelectedRow();
  int last = d_view->lastSelectedRow();
  int count, current = first;

  if (first < 0) return;

  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: insert empty rows(s)").arg(name()));
  while (current <= last) {
    current = first + 1;
    while (current <= last && d_view->isRowSelected(current)) current++;
    count = current - first;
    insertRows(first, count);
    current += count;
    last += count;
    while (current <= last && !d_view->isRowSelected(current)) current++;
    first = current;
  }
  endMacro();
  RESET_CURSOR;
}

void Matrix::removeSelectedRows() {
  if (!d_view) return;
  int first = d_view->firstSelectedRow();
  int last = d_view->lastSelectedRow();
  if (first < 0) return;

  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: remove selected rows(s)").arg(name()));
  for (int i = last; i >= first; i--)
    if (d_view->isRowSelected(i, false)) removeRows(i, 1);
  endMacro();
  RESET_CURSOR;
}

void Matrix::clearSelectedRows() {
  if (!d_view) return;
  int first = d_view->firstSelectedRow();
  int last = d_view->lastSelectedRow();
  if (first < 0) return;

  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: clear selected rows(s)").arg(name()));
  for (int i = first; i <= last; i++) {
    if (d_view->isRowSelected(i))
      for (int j = 0; j < columnCount(); j++)
        exec(new MatrixSetCellValueCmd(d_matrix_private, i, j, 0.0));
  }
  endMacro();
  RESET_CURSOR;
}

void Matrix::clearSelectedCells() {
  if (!d_view) return;
  int first_row = d_view->firstSelectedRow();
  int last_row = d_view->lastSelectedRow();
  if (first_row < 0) return;
  int first_col = d_view->firstSelectedColumn();
  int last_col = d_view->lastSelectedColumn();
  if (first_col < 0) return;

  WAIT_CURSOR;
  beginMacro(tr("%1: clear selected cell(s)").arg(name()));
  for (int i = first_row; i <= last_row; i++)
    for (int j = first_col; j <= last_col; j++)
      if (d_view->isCellSelected(i, j))
        exec(new MatrixSetCellValueCmd(d_matrix_private, i, j, 0.0));
  endMacro();
  RESET_CURSOR;
}

QMenu *Matrix::createContextMenu() const {
  QMenu *menu = AbstractPart::createContextMenu();
  Q_ASSERT(menu);
  menu->addSeparator();

  menu->addAction(action_duplicate);
  // TODO menu->addAction( ....

  return menu;
}

QMenu *Matrix::createSelectionMenu(QMenu *append_to) {
  QMenu *menu = append_to;
  if (!menu) menu = new QMenu();

  menu->addAction(action_cut_selection);
  menu->addAction(action_copy_selection);
  menu->addAction(action_paste_into_selection);
  menu->addAction(action_clear_selection);

  return menu;
}

QMenu *Matrix::createColumnMenu(QMenu *append_to) {
  QMenu *menu = append_to;
  if (!menu) menu = new QMenu();

  menu->addAction(action_insert_columns);
  menu->addAction(action_remove_columns);
  menu->addAction(action_clear_columns);
  menu->addSeparator();
  menu->addAction(action_edit_coordinates);

  return menu;
}

QMenu *Matrix::createMatrixMenu(QMenu *append_to) {
  QMenu *menu = append_to;
  if (!menu) menu = new QMenu();

  menu->addAction(action_toggle_tabbar);
  menu->addSeparator();
  menu->addAction(action_select_all);
  menu->addAction(action_clear_matrix);
  menu->addSeparator();
  menu->addAction(action_set_formula);
  menu->addAction(action_recalculate);
  menu->addSeparator();
  menu->addAction(action_edit_format);
  menu->addSeparator();
  menu->addAction(action_go_to_cell);

  return menu;
}

QMenu *Matrix::createRowMenu(QMenu *append_to) {
  QMenu *menu = append_to;
  if (!menu) menu = new QMenu();

  menu->addAction(action_insert_rows);
  menu->addAction(action_remove_rows);
  menu->addAction(action_clear_rows);
  menu->addSeparator();
  menu->addAction(action_edit_coordinates);

  return menu;
}

void Matrix::createActions() {
  // selection related actions
  action_cut_selection = new QAction(
      IconLoader::load("edit-cut", IconLoader::LightDark), tr("Cu&t"), this);
  actionManager()->addAction(action_cut_selection, "cut_selection");

  action_copy_selection = new QAction(
      IconLoader::load("edit-copy", IconLoader::LightDark), tr("&Copy"), this);
  actionManager()->addAction(action_copy_selection, "copy_selection");

  action_paste_into_selection =
      new QAction(IconLoader::load("edit-paste", IconLoader::LightDark),
                  tr("Past&e"), this);
  actionManager()->addAction(action_paste_into_selection,
                             "paste_into_selection");

  action_clear_selection =
      new QAction(IconLoader::load("clear-loginfo", IconLoader::General),
                  tr("Clea&r", "clear selection"), this);
  actionManager()->addAction(action_clear_selection, "clear_selection");

  // matrix related actions
  action_set_formula =
      new QAction(IconLoader::load("math-fofx", IconLoader::LightDark),
                  tr("Assign &Formula"), this);
  action_set_formula->setShortcut(tr("Alt+Q"));
  actionManager()->addAction(action_set_formula, "set_formula");

  action_recalculate =
      new QAction(IconLoader::load("edit-recalculate", IconLoader::LightDark),
                  tr("Recalculate"), this);
  action_recalculate->setShortcut(tr("Ctrl+Return"));
  actionManager()->addAction(action_recalculate, "recalculate");

  action_toggle_tabbar =
      new QAction(IconLoader::load("edit-unhide", IconLoader::LightDark),
                  QString("Show/Hide Controls"),
                  this);  // show/hide control tabs
  action_toggle_tabbar->setShortcut(tr("F12"));
  actionManager()->addAction(action_toggle_tabbar, "toggle_tabbar");

  action_select_all = new QAction(
      IconLoader::load("edit-matrix-select-all", IconLoader::LightDark),
      tr("Select All"), this);
  actionManager()->addAction(action_select_all, "select_all");

  action_clear_matrix =
      new QAction(IconLoader::load("edit-matrix-clear", IconLoader::LightDark),
                  tr("Clear Matrix"), this);
  actionManager()->addAction(action_clear_matrix, "clear_matrix");

  action_go_to_cell =
      new QAction(IconLoader::load("goto-cell", IconLoader::LightDark),
                  tr("&Go to Cell"), this);
  action_go_to_cell->setShortcut(tr("Ctrl+Alt+G"));
  actionManager()->addAction(action_go_to_cell, "go_to_cell");

  action_transpose = new QAction(tr("&Transpose"), this);
  actionManager()->addAction(action_transpose, "transpose");

  action_mirror_horizontally = new QAction(tr("Mirror &Horizontally"), this);
  actionManager()->addAction(action_mirror_horizontally, "mirror_horizontally");

  action_mirror_vertically = new QAction(tr("Mirror &Vertically"), this);
  actionManager()->addAction(action_mirror_vertically, "mirror_vertically");

  action_import_image =
      new QAction(tr("&Import Image", "import image as matrix"), this);
  actionManager()->addAction(action_import_image, "import_image");

  action_duplicate =
      new QAction(IconLoader::load("edit-duplicate", IconLoader::LightDark),
                  tr("&Duplicate", "duplicate matrix"), this);
  actionManager()->addAction(action_duplicate, "duplicate");

  action_edit_coordinates = new QAction(tr("Set &Coordinates"), this);
  actionManager()->addAction(action_edit_coordinates, "edit_coordinates");

  action_edit_format = new QAction(tr("Set Display &Format"), this);
  actionManager()->addAction(action_edit_format, "edit_format");

  // column related actions
  action_insert_columns = new QAction(
      IconLoader::load("edit-table-insert-column", IconLoader::LightDark),
      tr("&Insert Empty Columns"), this);
  actionManager()->addAction(action_insert_columns, "insert_columns");

  action_remove_columns = new QAction(
      IconLoader::load("edit-table-delete-column", IconLoader::LightDark),
      tr("Remo&ve Columns"), this);
  actionManager()->addAction(action_remove_columns, "remove_columns");

  action_clear_columns = new QAction(
      IconLoader::load("edit-table-clear-column", IconLoader::LightDark),
      tr("Clea&r Columns"), this);
  actionManager()->addAction(action_clear_columns, "clear_columns");

  action_add_columns = new QAction(
      IconLoader::load("edit-table-insert-column-right", IconLoader::LightDark),
      tr("&Add Columns"), this);
  actionManager()->addAction(action_add_columns, "add_columns");

  // row related actions
  action_insert_rows = new QAction(
      IconLoader::load("edit-table-insert-row", IconLoader::LightDark),
      tr("&Insert Empty Rows"), this);
  actionManager()->addAction(action_insert_rows, "insert_rows");

  action_remove_rows = new QAction(
      IconLoader::load("edit-table-delete-row", IconLoader::LightDark),
      tr("Remo&ve Rows"), this);
  actionManager()->addAction(action_remove_rows, "remove_rows");

  action_clear_rows = new QAction(
      IconLoader::load("edit-table-clear-row", IconLoader::LightDark),
      tr("Clea&r Rows"), this);
  actionManager()->addAction(action_clear_rows, "clear_rows");

  action_add_rows =
      new QAction(IconLoader::load("edit-table-add-row", IconLoader::LightDark),
                  tr("&Add Rows"), this);
  actionManager()->addAction(action_add_rows, "add_rows");
}

void Matrix::connectActions() {
  connect(action_cut_selection, &QAction::triggered, this,
          &Matrix::cutSelection);
  connect(action_copy_selection, &QAction::triggered, this,
          &Matrix::copySelection);
  connect(action_paste_into_selection, &QAction::triggered, this,
          &Matrix::pasteIntoSelection);
  connect(action_set_formula, &QAction::triggered, this, &Matrix::editFormula);
  connect(action_edit_coordinates, &QAction::triggered, this,
          &Matrix::editCoordinates);
  connect(action_edit_format, &QAction::triggered, this, &Matrix::editFormat);
  connect(action_clear_selection, &QAction::triggered, this,
          &Matrix::clearSelectedCells);
#ifdef LEGACY_CODE_0_2_x
  connect(action_recalculate, &QAction::triggered, this,
          &Matrix::recalculateSelectedCells);
#endif
  connect(action_select_all, &QAction::triggered, this, &Matrix::selectAll);
  connect(action_clear_matrix, &QAction::triggered, this, &Matrix::clear);
  connect(action_transpose, &QAction::triggered, this, &Matrix::transpose);
  connect(action_mirror_horizontally, &QAction::triggered, this,
          &Matrix::mirrorHorizontally);
  connect(action_mirror_vertically, &QAction::triggered, this,
          &Matrix::mirrorVertically);
  connect(action_go_to_cell, &QAction::triggered, this, &Matrix::goToCell);
  connect(action_import_image, &QAction::triggered, this,
          &Matrix::importImageDialog);
  connect(action_duplicate, &QAction::triggered, this, &Matrix::duplicate);
  connect(action_insert_columns, &QAction::triggered, this,
          &Matrix::insertEmptyColumns);
  connect(action_remove_columns, &QAction::triggered, this,
          &Matrix::removeSelectedColumns);
  connect(action_clear_columns, &QAction::triggered, this,
          &Matrix::clearSelectedColumns);
  connect(action_insert_rows, &QAction::triggered, this,
          &Matrix::insertEmptyRows);
  connect(action_remove_rows, &QAction::triggered, this,
          &Matrix::removeSelectedRows);
  connect(action_clear_rows, &QAction::triggered, this,
          &Matrix::clearSelectedRows);
  connect(action_add_columns, &QAction::triggered, this, &Matrix::addColumns);
  connect(action_add_rows, &QAction::triggered, this, &Matrix::addRows);
}

void Matrix::addActionsToView() {
  connect(action_toggle_tabbar, &QAction::triggered, d_view,
          &MatrixView::toggleControlTabBar);

  d_view->addAction(action_cut_selection);
  d_view->addAction(action_copy_selection);
  d_view->addAction(action_paste_into_selection);
  d_view->addAction(action_set_formula);
  d_view->addAction(action_edit_coordinates);
  d_view->addAction(action_edit_format);
  d_view->addAction(action_clear_selection);
  d_view->addAction(action_recalculate);
  d_view->addAction(action_toggle_tabbar);
  d_view->addAction(action_select_all);
  d_view->addAction(action_clear_matrix);
  d_view->addAction(action_transpose);
  d_view->addAction(action_mirror_horizontally);
  d_view->addAction(action_mirror_vertically);
  d_view->addAction(action_go_to_cell);
  d_view->addAction(action_import_image);
#ifndef LEGACY_CODE_0_2_x
  d_view->addAction(action_duplicate);
#endif
  d_view->addAction(action_insert_columns);
  d_view->addAction(action_remove_columns);
  d_view->addAction(action_clear_columns);
  d_view->addAction(action_insert_rows);
  d_view->addAction(action_remove_rows);
  d_view->addAction(action_clear_rows);
  d_view->addAction(action_add_columns);
  d_view->addAction(action_add_rows);
}

bool Matrix::fillProjectMenu(QMenu *menu) {
  menu->setTitle(tr("&Matrix"));

  menu->addAction(action_toggle_tabbar);
  menu->addSeparator();
  menu->addAction(action_edit_coordinates);
  menu->addAction(action_edit_format);
  menu->addSeparator();
  menu->addAction(action_set_formula);
  menu->addAction(action_recalculate);
  menu->addSeparator();
  menu->addAction(action_clear_matrix);
  menu->addAction(action_transpose);
  menu->addAction(action_mirror_horizontally);
  menu->addAction(action_mirror_vertically);
  menu->addSeparator();
#ifndef LEGACY_CODE_0_2_x
  menu->addAction(action_duplicate);
#endif
  menu->addAction(action_import_image);
  menu->addSeparator();
  menu->addAction(action_go_to_cell);

  return true;

  // TODO:
  // Convert to Table
  // Export
}

void Matrix::showMatrixViewContextMenu(const QPoint &pos) {
  if (!d_view) return;
  QMenu context_menu;

  createSelectionMenu(&context_menu);
  context_menu.addSeparator();
  createMatrixMenu(&context_menu);
  context_menu.addSeparator();

  context_menu.exec(pos);
}

void Matrix::showMatrixViewColumnContextMenu(const QPoint &pos) {
  QMenu context_menu;

  createColumnMenu(&context_menu);

  context_menu.exec(pos);
}

void Matrix::showMatrixViewRowContextMenu(const QPoint &pos) {
  QMenu context_menu;

  createRowMenu(&context_menu);

  context_menu.exec(pos);
}

void Matrix::goToCell() {
  if (!d_view) return;
  bool ok;

  int col = QInputDialog::getInt(nullptr, tr("Go to Cell"), tr("Enter column"),
                                 1, 1, columnCount(), 1, &ok);
  if (!ok) return;

  int row = QInputDialog::getInt(nullptr, tr("Go to Cell"), tr("Enter row"), 1,
                                 1, rowCount(), 1, &ok);
  if (!ok) return;

  d_view->goToCell(row - 1, col - 1);
}

void Matrix::copy(Matrix *other) {
  WAIT_CURSOR;
  beginMacro(QObject::tr("%1: copy %2").arg(name()).arg(other->name()));
  int rows = other->rowCount();
  int columns = other->columnCount();
  setDimensions(rows, columns);
  for (int i = 0; i < rows; i++) setRowHeight(i, other->rowHeight(i));
  for (int i = 0; i < columns; i++) setColumnWidth(i, other->columnWidth(i));
  d_matrix_private->blockChangeSignals(true);
  for (int i = 0; i < columns; i++)
    setColumnCells(i, 0, rows - 1, other->columnCells(i, 0, rows - 1));
  setCoordinates(other->xStart(), other->xEnd(), other->yStart(),
                 other->yEnd());
  setNumericFormat(other->numericFormat());
  setDisplayedDigits(other->displayedDigits());
  setFormula(other->formula());
  d_matrix_private->blockChangeSignals(false);
  emit dataChanged(0, 0, rows - 1, columns - 1);
  if (d_view) d_view->rereadSectionSizes();
  endMacro();
  RESET_CURSOR;
}

void Matrix::setPlotMenu(QMenu *menu) { d_plot_menu = menu; }

QIcon Matrix::icon() const {
  return IconLoader::load("matrix", IconLoader::LightDark);
}

QString Matrix::text(int row, int col) {
  return QLocale().toString(cell(row, col), d_matrix_private->numericFormat(),
                            d_matrix_private->displayedDigits());
}

void Matrix::selectAll() {
  if (!d_view) return;
  d_view->selectAll();
}

void Matrix::setCell(int row, int col, double value) {
  if (row < 0 || row >= rowCount()) return;
  if (col < 0 || col >= columnCount()) return;
  exec(new MatrixSetCellValueCmd(d_matrix_private, row, col, value));
}

void Matrix::importImageDialog() {
  QList<QByteArray> formats = QImageReader::supportedImageFormats();
  QString filter = tr("Images") + " (";
  for (int i = 0; i < formats.count(); i++)
    filter += " *." + formats.at(i) + " ";
  filter += ");;";
  for (int i = 0; i < formats.count(); i++)
    filter += " *." + formats.at(i) + " (*." + formats.at(i) + ");;";

  QString images_path = global("images_path").toString();
  QString file_name = QFileDialog::getOpenFileName(
      nullptr, tr("Import image from file"), images_path, filter);
  if (!file_name.isEmpty()) {
    QFileInfo file_info(file_name);
    images_path = file_info.canonicalPath();
    setGlobal("images_path", images_path);
    QImage image(file_name);
    Matrix *matrix = nullptr;
    if (!image.isNull()) matrix = Matrix::fromImage(image);
    if (matrix) {
      copy(matrix);
      delete matrix;
    } else
      QMessageBox::information(
          nullptr, tr("Error importing image"),
          tr("Import of image '%1' failed").arg(file_name));
  }
}

void Matrix::duplicate() {
#ifndef LEGACY_CODE_0_2_x
  Matrix *matrix = new Matrix(0, rowCount(), columnCount(), name());
  matrix->copy(this);
  if (folder()) folder()->addChild(matrix);
#endif
}

void Matrix::editFormat() {
  if (!d_view) return;
  d_view->showControlFormatTab();
}

void Matrix::editCoordinates() {
  if (!d_view) return;
  d_view->showControlCoordinatesTab();
}

void Matrix::editFormula() {
  if (!d_view) return;
  d_view->showControlFormulaTab();
}

void Matrix::addRows() {
  if (!d_view) return;
  WAIT_CURSOR;
  int count = d_view->selectedRowCount(false);
  beginMacro(QObject::tr("%1: add %2 rows(s)").arg(name()).arg(count));
  exec(new MatrixInsertRowsCmd(d_matrix_private, rowCount(), count));
  endMacro();
  RESET_CURSOR;
}

void Matrix::addColumns() {
  if (!d_view) return;
  WAIT_CURSOR;
  int count = d_view->selectedRowCount(false);
  beginMacro(QObject::tr("%1: add %2 column(s)").arg(name()).arg(count));
  exec(new MatrixInsertColumnsCmd(d_matrix_private, columnCount(), count));
  endMacro();
  RESET_CURSOR;
}

void Matrix::setXStart(double x) {
  WAIT_CURSOR;
  exec(new MatrixSetCoordinatesCmd(d_matrix_private, x, xEnd(), yStart(),
                                   yEnd()));
  RESET_CURSOR;
}

void Matrix::setXEnd(double x) {
  WAIT_CURSOR;
  exec(new MatrixSetCoordinatesCmd(d_matrix_private, xStart(), x, yStart(),
                                   yEnd()));
  RESET_CURSOR;
}

void Matrix::setYStart(double y) {
  WAIT_CURSOR;
  exec(new MatrixSetCoordinatesCmd(d_matrix_private, xStart(), xEnd(), y,
                                   yEnd()));
  RESET_CURSOR;
}

void Matrix::setYEnd(double y) {
  WAIT_CURSOR;
  exec(new MatrixSetCoordinatesCmd(d_matrix_private, xStart(), xEnd(), yStart(),
                                   y));
  RESET_CURSOR;
}

void Matrix::setCoordinates(double x1, double x2, double y1, double y2) {
  WAIT_CURSOR;
  exec(new MatrixSetCoordinatesCmd(d_matrix_private, x1, x2, y1, y2));
  RESET_CURSOR;
}

void Matrix::setNumericFormat(char format) {
  if (format == numericFormat()) return;
  WAIT_CURSOR;
  exec(new MatrixSetFormatCmd(d_matrix_private, format));
  RESET_CURSOR;
}

void Matrix::setDisplayedDigits(int digits) {
  if (digits == displayedDigits()) return;
  WAIT_CURSOR;
  exec(new MatrixSetDigitsCmd(d_matrix_private, digits));
  RESET_CURSOR;
}

double Matrix::xStart() const { return d_matrix_private->xStart(); }

double Matrix::yStart() const { return d_matrix_private->yStart(); }

double Matrix::xEnd() const { return d_matrix_private->xEnd(); }

double Matrix::yEnd() const { return d_matrix_private->yEnd(); }

QString Matrix::formula() const { return d_matrix_private->formula(); }

void Matrix::setFormula(const QString &formula) {
  WAIT_CURSOR;
  exec(new MatrixSetFormulaCmd(d_matrix_private, formula));
  RESET_CURSOR;
}

char Matrix::numericFormat() const { return d_matrix_private->numericFormat(); }

int Matrix::displayedDigits() const {
  return d_matrix_private->displayedDigits();
}

void Matrix::save(QXmlStreamWriter *writer, const bool saveastemplate) const {
  int cols = columnCount();
  int rows = rowCount();
  writer->writeStartElement("matrix");
  writeBasicAttributes(writer);
  writer->writeAttribute("columns", QString::number(cols));
  writer->writeAttribute("rows", QString::number(rows));
  writeCommentElement(writer);
  writer->writeStartElement("formula");
  writer->writeCharacters(formula());
  writer->writeEndElement();
  writer->writeStartElement("display");
  writer->writeAttribute("numeric_format", QString(QChar(numericFormat())));
  writer->writeAttribute("displayed_digits",
                         QString::number(displayedDigits()));
  writer->writeEndElement();
  writer->writeStartElement("coordinates");
  writer->writeAttribute("x_start", QString::number(xStart()));
  writer->writeAttribute("x_end", QString::number(xEnd()));
  writer->writeAttribute("y_start", QString::number(yStart()));
  writer->writeAttribute("y_end", QString::number(yEnd()));
  writer->writeEndElement();

  if (!saveastemplate)
    for (int col = 0; col < cols; col++)
      for (int row = 0; row < rows; row++) {
        writer->writeStartElement("cell");
        writer->writeAttribute("row", QString::number(row));
        writer->writeAttribute("column", QString::number(col));
        writer->writeCharacters(QString::number(cell(row, col), 'e', 16));
        writer->writeEndElement();
      }
  for (int col = 0; col < cols; col++) {
    writer->writeStartElement("column_width");
    writer->writeAttribute("column", QString::number(col));
    writer->writeCharacters(QString::number(columnWidth(col)));
    writer->writeEndElement();
  }
  for (int row = 0; row < rows; row++) {
    writer->writeStartElement("row_height");
    writer->writeAttribute("row", QString::number(row));
    writer->writeCharacters(QString::number(rowHeight(row)));
    writer->writeEndElement();
  }
  writer->writeEndElement();  // "matrix"
}

bool Matrix::load(XmlStreamReader *reader) {
  if (reader->isStartElement() && reader->name() == "matrix") {
    setDimensions(0, 0);
    setComment("");
    setFormula("");
    setNumericFormat('f');
    setDisplayedDigits(6);
    setCoordinates(0.0, 1.0, 0.0, 1.0);

    if (!readBasicAttributes(reader)) return false;

    // read dimensions
    bool ok1, ok2;
    int rows, cols;
    rows = reader->readAttributeInt("rows", &ok1);
    cols = reader->readAttributeInt("columns", &ok2);
    if (!ok1 || !ok2) {
      reader->raiseError(tr("invalid row or column count"));
      return false;
    }
    d_matrix_private->blockChangeSignals(true);
    setDimensions(rows, cols);

    // read child elements
    while (!reader->atEnd()) {
      reader->readNext();

      if (reader->isEndElement()) break;

      if (reader->isStartElement()) {
        bool ret_val = true;
        if (reader->name() == "comment")
          ret_val = readCommentElement(reader);
        else if (reader->name() == "formula")
          ret_val = readFormulaElement(reader);
        else if (reader->name() == "display")
          ret_val = readDisplayElement(reader);
        else if (reader->name() == "coordinates")
          ret_val = readCoordinatesElement(reader);
        else if (reader->name() == "cell")
          ret_val = readCellElement(reader);
        else if (reader->name() == "row_height")
          ret_val = readRowHeightElement(reader);
        else if (reader->name() == "column_width")
          ret_val = readColumnWidthElement(reader);
        else  // unknown element
        {
          reader->raiseWarning(
              tr("unknown element '%1'").arg(reader->name().toString()));
          if (!reader->skipToEndElement()) return false;
        }
        if (!ret_val) return false;
      }
    }
    d_matrix_private->blockChangeSignals(false);
  } else  // no matrix element
    reader->raiseError(tr("no matrix element found"));

  return !reader->hasError();
}

bool Matrix::readDisplayElement(XmlStreamReader *reader) {
  Q_ASSERT(reader->isStartElement() && reader->name() == "display");
  QXmlStreamAttributes attribs = reader->attributes();

  QString str =
      attribs.value(reader->namespaceUri().toString(), "numeric_format")
          .toString();
  if (str.isEmpty() || str.length() != 1) {
    reader->raiseError(tr("invalid or missing numeric format"));
    return false;
  }
  setNumericFormat(str.at(0).toLatin1());

  bool ok;
  int digits = reader->readAttributeInt("displayed_digits", &ok);
  if (!ok) {
    reader->raiseError(tr("invalid or missing number of displayed digits"));
    return false;
  }
  setDisplayedDigits(digits);
  if (!reader->skipToEndElement()) return false;

  return true;
}

bool Matrix::readCoordinatesElement(XmlStreamReader *reader) {
  Q_ASSERT(reader->isStartElement() && reader->name() == "coordinates");

  bool ok;
  int val;

  val = reader->readAttributeInt("x_start", &ok);
  if (!ok) {
    reader->raiseError(tr("invalid x start value"));
    return false;
  }
  setXStart(val);

  val = reader->readAttributeInt("x_end", &ok);
  if (!ok) {
    reader->raiseError(tr("invalid x end value"));
    return false;
  }
  setXEnd(val);

  val = reader->readAttributeInt("y_start", &ok);
  if (!ok) {
    reader->raiseError(tr("invalid y start value"));
    return false;
  }
  setYStart(val);

  val = reader->readAttributeInt("y_end", &ok);
  if (!ok) {
    reader->raiseError(tr("invalid y end value"));
    return false;
  }
  setYEnd(val);
  if (!reader->skipToEndElement()) return false;

  return true;
}

bool Matrix::readFormulaElement(XmlStreamReader *reader) {
  Q_ASSERT(reader->isStartElement() && reader->name() == "formula");
  setFormula(reader->readElementText());
  return true;
}

bool Matrix::readRowHeightElement(XmlStreamReader *reader) {
  Q_ASSERT(reader->isStartElement() && reader->name() == "row_height");
  bool ok;
  int row = reader->readAttributeInt("row", &ok);
  if (!ok) {
    reader->raiseError(tr("invalid or missing row index"));
    return false;
  }
  QString str = reader->readElementText();
  int value = str.toInt(&ok);
  if (!ok) {
    reader->raiseError(tr("invalid row height"));
    return false;
  }
  if (d_view)
    d_view->setRowHeight(row, value);
  else
    setRowHeight(row, value);
  return true;
}

bool Matrix::readColumnWidthElement(XmlStreamReader *reader) {
  Q_ASSERT(reader->isStartElement() && reader->name() == "column_width");
  bool ok;
  int col = reader->readAttributeInt("column", &ok);
  if (!ok) {
    reader->raiseError(tr("invalid or missing column index"));
    return false;
  }
  QString str = reader->readElementText();
  int value = str.toInt(&ok);
  if (!ok) {
    reader->raiseError(tr("invalid column width"));
    return false;
  }
  if (d_view)
    d_view->setColumnWidth(col, value);
  else
    setColumnWidth(col, value);
  return true;
}

bool Matrix::readCellElement(XmlStreamReader *reader) {
  Q_ASSERT(reader->isStartElement() && reader->name() == "cell");

  QString str;
  int row, col;
  bool ok;

  QXmlStreamAttributes attribs = reader->attributes();
  row = reader->readAttributeInt("row", &ok);
  if (!ok) {
    reader->raiseError(tr("invalid or missing row index"));
    return false;
  }
  col = reader->readAttributeInt("column", &ok);
  if (!ok) {
    reader->raiseError(tr("invalid or missing column index"));
    return false;
  }

  str = reader->readElementText();
  double value = str.toDouble(&ok);
  if (!ok) {
    reader->raiseError(tr("invalid cell value"));
    return false;
  }
  setCell(row, col, value);

  return true;
}

void Matrix::setRowHeight(int row, int height) {
  d_matrix_private->setRowHeight(row, height);
}

void Matrix::setColumnWidth(int col, int width) {
  d_matrix_private->setColumnWidth(col, width);
}

int Matrix::rowHeight(int row) const {
  return d_matrix_private->rowHeight(row);
}

int Matrix::columnWidth(int col) const {
  return d_matrix_private->columnWidth(col);
}

void Matrix::loadIcons() {
  action_cut_selection->setIcon(
      IconLoader::load("edit-cut", IconLoader::LightDark));
  action_copy_selection->setIcon(
      IconLoader::load("edit-copy", IconLoader::LightDark));
  action_paste_into_selection->setIcon(
      IconLoader::load("edit-paste", IconLoader::LightDark));
  action_clear_selection->setIcon(
      IconLoader::load("clear-loginfo", IconLoader::General));
  // matrix related actions
  action_set_formula->setIcon(
      IconLoader::load("math-fofx", IconLoader::LightDark));
  action_recalculate->setIcon(
      IconLoader::load("edit-recalculate", IconLoader::LightDark));
  action_select_all->setIcon(
      IconLoader::load("edit-matrix-select-all", IconLoader::LightDark));
  action_clear_matrix->setIcon(
      IconLoader::load("edit-matrix-clear", IconLoader::LightDark));
  action_go_to_cell->setIcon(
      IconLoader::load("goto-cell", IconLoader::LightDark));
  action_duplicate->setIcon(
      IconLoader::load("edit-duplicate", IconLoader::LightDark));
  // column related actions
  action_insert_columns->setIcon(
      IconLoader::load("edit-table-insert-column", IconLoader::LightDark));
  action_remove_columns->setIcon(
      IconLoader::load("edit-table-delete-column", IconLoader::LightDark));
  action_clear_columns->setIcon(
      IconLoader::load("edit-table-clear-column", IconLoader::LightDark));
  action_add_columns->setIcon(IconLoader::load("edit-table-insert-column-right",
                                               IconLoader::LightDark));
  // row related actions
  action_insert_rows->setIcon(
      IconLoader::load("edit-table-insert-row", IconLoader::LightDark));
  action_remove_rows->setIcon(
      IconLoader::load("edit-table-delete-row", IconLoader::LightDark));
  action_clear_rows->setIcon(
      IconLoader::load("edit-table-clear-row", IconLoader::LightDark));
  action_add_rows->setIcon(
      IconLoader::load("edit-table-add-row", IconLoader::LightDark));
  d_view->loadIcons();
}

void Matrix::adjustTabBarAction(bool visible) {
  if (visible) {
    action_toggle_tabbar->setText(tr("Hide Controls"));
    action_toggle_tabbar->setIcon(
        IconLoader::load("edit-hide", IconLoader::LightDark));
  } else {
    action_toggle_tabbar->setText(tr("Show Controls"));
    action_toggle_tabbar->setIcon(
        IconLoader::load("edit-unhide", IconLoader::LightDark));
  }
}

QVector<qreal> Matrix::columnCells(int col, int first_row, int last_row) {
  return d_matrix_private->columnCells(col, first_row, last_row);
}

void Matrix::setColumnCells(int col, int first_row, int last_row,
                            const QVector<qreal> &values) {
  WAIT_CURSOR;
  exec(new MatrixSetColumnCellsCmd(d_matrix_private, col, first_row, last_row,
                                   values));
  RESET_CURSOR;
}

QVector<qreal> Matrix::rowCells(int row, int first_column, int last_column) {
  return d_matrix_private->rowCells(row, first_column, last_column);
}

void Matrix::setRowCells(int row, int first_column, int last_column,
                         const QVector<qreal> &values) {
  WAIT_CURSOR;
  exec(new MatrixSetRowCellsCmd(d_matrix_private, row, first_column,
                                last_column, values));
  RESET_CURSOR;
}

void Matrix::transpose() {
  WAIT_CURSOR;
  exec(new MatrixTransposeCmd(d_matrix_private));
  RESET_CURSOR;
}

void Matrix::mirrorHorizontally() {
  WAIT_CURSOR;
  exec(new MatrixMirrorHorizontallyCmd(d_matrix_private));
  RESET_CURSOR;
}

void Matrix::mirrorVertically() {
  WAIT_CURSOR;
  exec(new MatrixMirrorVerticallyCmd(d_matrix_private));
  RESET_CURSOR;
}

void Matrix::recalculateSelectedCells() {
  if (!d_view) return;
#ifdef LEGACY_CODE_0_2_x
  WAIT_CURSOR;
  beginMacro(tr("%1: apply formula to selection").arg(name()));
  emit recalculate();
  endMacro();
  RESET_CURSOR;
#endif
}

void Matrix::showOnlyValues(const bool status) { showonlyvalues_ = status; }

bool Matrix::getShowOnlyValues() const { return showonlyvalues_; }

/* ========================= static methods ======================= */
ActionManager *Matrix::action_manager = nullptr;

ActionManager *Matrix::actionManager() {
  if (!action_manager) initActionManager();

  return action_manager;
}

void Matrix::initActionManager() {
  if (!action_manager) action_manager = new ActionManager();

  action_manager->setTitle(tr("Matrix"));
  volatile Matrix *action_creator =
      new Matrix();  // initialize the action texts
  delete action_creator;
}

Matrix *Matrix::fromImage(const QImage &image) {
  int cols = image.width();
  int rows = image.height();

  QProgressDialog progress;
  progress.setRange(0, cols);
  progress.setWindowTitle(tr("AlphaPlot") + " - " + tr("Import image..."));
  progress.raise();

  Matrix *matrix = new Matrix(nullptr, rows, cols, tr("Matrix %1").arg(1));

  QVector<qreal> values;
  values.resize(rows);

  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++)
      values[j] = qGray(image.pixel(i, rows - 1 - j));

    matrix->setColumnCells(i, 0, rows - 1, values);

    if (i % 5 == 4) {
      progress.setValue(i);
      QApplication::processEvents();
    }

    if (progress.wasCanceled()) break;
  }

  if (progress.wasCanceled()) {
    delete matrix;
    return nullptr;
  }
  return matrix;
}

/* ========================== Matrix::Private ====================== */

Matrix::Private::Private(Matrix *owner)
    : d_owner(owner), d_column_count(0), d_row_count(0) {
  d_block_change_signals = false;
  d_numeric_format = 'f';
  d_displayed_digits = 6;
  d_x_start = 0.0;
  d_x_end = 1.0;
  d_y_start = 0.0;
  d_y_end = 1.0;
}

void Matrix::Private::insertColumns(int before, int count) {
  Q_ASSERT(before >= 0);
  Q_ASSERT(before <= d_column_count);

  emit d_owner->columnsAboutToBeInserted(before, count);
  for (int i = 0; i < count; i++) {
    d_data.insert(before + i, QVector<qreal>(d_row_count));
    d_column_widths.insert(before + i, Matrix::defaultColumnWidth());
  }

  d_column_count += count;
  emit d_owner->columnsInserted(before, count);
}

void Matrix::Private::removeColumns(int first, int count) {
  emit d_owner->columnsAboutToBeRemoved(first, count);
  Q_ASSERT(first >= 0);
  Q_ASSERT(first + count <= d_column_count);
  d_data.remove(first, count);
  for (int i = 0; i < count; i++) d_column_widths.removeAt(first);
  d_column_count -= count;
  emit d_owner->columnsRemoved(first, count);
}

void Matrix::Private::insertRows(int before, int count) {
  emit d_owner->rowsAboutToBeInserted(before, count);
  Q_ASSERT(before >= 0);
  Q_ASSERT(before <= d_row_count);
  for (int col = 0; col < d_column_count; col++)
    for (int i = 0; i < count; i++) d_data[col].insert(before + i, 0.0);
  for (int i = 0; i < count; i++)
    d_row_heights.insert(before + i, Matrix::defaultRowHeight());

  d_row_count += count;
  emit d_owner->rowsInserted(before, count);
}

void Matrix::Private::removeRows(int first, int count) {
  emit d_owner->rowsAboutToBeRemoved(first, count);
  Q_ASSERT(first >= 0);
  Q_ASSERT(first + count <= d_row_count);
  for (int col = 0; col < d_column_count; col++)
    d_data[col].remove(first, count);
  for (int i = 0; i < count; i++) d_row_heights.removeAt(first);

  d_row_count -= count;
  emit d_owner->rowsRemoved(first, count);
}

double Matrix::Private::cell(int row, int col) const {
  Q_ASSERT(row >= 0 && row < d_row_count);
  Q_ASSERT(col >= 0 && col < d_column_count);
  return d_data.at(col).at(row);
}

void Matrix::Private::setCell(int row, int col, double value) {
  Q_ASSERT(row >= 0 && row < d_row_count);
  Q_ASSERT(col >= 0 && col < d_column_count);
  d_data[col][row] = value;
  if (!d_block_change_signals) emit d_owner->dataChanged(row, col, row, col);
}

QVector<qreal> Matrix::Private::columnCells(int col, int first_row,
                                            int last_row) {
  Q_ASSERT(first_row >= 0 && first_row < d_row_count);
  Q_ASSERT(last_row >= 0 && last_row < d_row_count);

  if (first_row == 0 && last_row == d_row_count - 1) return d_data.at(col);

  QVector<qreal> result;
  for (int i = first_row; i <= last_row; i++)
    result.append(d_data.at(col).at(i));
  return result;
}

void Matrix::Private::setColumnCells(int col, int first_row, int last_row,
                                     const QVector<qreal> &values) {
  Q_ASSERT(first_row >= 0 && first_row < d_row_count);
  Q_ASSERT(last_row >= 0 && last_row < d_row_count);
  Q_ASSERT(values.count() > last_row - first_row);

  if (first_row == 0 && last_row == d_row_count - 1) {
    d_data[col] = values;
    d_data[col].resize(d_row_count);  // values may be larger
    if (!d_block_change_signals)
      emit d_owner->dataChanged(first_row, col, last_row, col);
    return;
  }

  for (int i = first_row; i <= last_row; i++)
    d_data[col][i] = values.at(i - first_row);
  if (!d_block_change_signals)
    emit d_owner->dataChanged(first_row, col, last_row, col);
}

QVector<qreal> Matrix::Private::rowCells(int row, int first_column,
                                         int last_column) {
  Q_ASSERT(first_column >= 0 && first_column < d_column_count);
  Q_ASSERT(last_column >= 0 && last_column < d_column_count);

  QVector<qreal> result;
  for (int i = first_column; i <= last_column; i++)
    result.append(d_data.at(i).at(row));
  return result;
}

void Matrix::Private::setRowCells(int row, int first_column, int last_column,
                                  const QVector<qreal> &values) {
  Q_ASSERT(first_column >= 0 && first_column < d_column_count);
  Q_ASSERT(last_column >= 0 && last_column < d_column_count);
  Q_ASSERT(values.count() > last_column - first_column);

  for (int i = first_column; i <= last_column; i++)
    d_data[i][row] = values.at(i - first_column);
  if (!d_block_change_signals)
    emit d_owner->dataChanged(row, first_column, row, last_column);
}

void Matrix::Private::clearColumn(int col) {
  d_data[col].fill(0.0);
  if (!d_block_change_signals)
    emit d_owner->dataChanged(0, col, d_row_count - 1, col);
}

double Matrix::Private::xStart() const { return d_x_start; }

double Matrix::Private::yStart() const { return d_y_start; }

double Matrix::Private::xEnd() const { return d_x_end; }

double Matrix::Private::yEnd() const { return d_y_end; }

void Matrix::Private::setXStart(double x) {
  d_x_start = x;
  emit d_owner->coordinatesChanged();
}

void Matrix::Private::setXEnd(double x) {
  d_x_end = x;
  emit d_owner->coordinatesChanged();
}

void Matrix::Private::setYStart(double y) {
  d_y_start = y;
  emit d_owner->coordinatesChanged();
}

void Matrix::Private::setYEnd(double y) {
  d_y_end = y;
  emit d_owner->coordinatesChanged();
}

QString Matrix::Private::formula() const { return d_formula; }

void Matrix::Private::setFormula(const QString &formula) {
  d_formula = formula;
  emit d_owner->formulaChanged();
}

}  // namespace future
