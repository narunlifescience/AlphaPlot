/***************************************************************************
        File                 : TableStatistics.cpp
        Project              : AlphaPlot
--------------------------------------------------------------------
        Copyright            : (C) 2006 by Knut Franke
        Email (use @ for *)  : knut.franke*gmx.de
        Description          : Table subclass that displays statistics on
                               columns or rows of another table

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
#include "TableStatistics.h"

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_vector.h>

#include <QContextMenuEvent>
#include <QList>
#include <QMenu>

#include "core/column/Column.h"
#include "core/datatypes/Double2StringFilter.h"
#include "table/TableDoubleHeaderView.h"
#include "table/TableModel.h"
#include "table/TableView.h"
#include "table/future_Table.h"

TableStatistics::TableStatistics(ScriptingEnv *env, QWidget *parent,
                                 Table *base, Type t, QList<int> targets)
    : Table(env, 1, 1, "", parent, ""),
      d_base(base),
      d_type(t),
      d_targets(targets) {
  static_cast<TableModel *>(d_view_widget->model())->setReadOnly(true);
  d_hide_button->hide();
  d_control_tabs->hide();

  d_future_table->action_cut_selection->setEnabled(false);
  d_future_table->action_paste_into_selection->setEnabled(false);
  d_future_table->action_set_formula->setEnabled(false);
  d_future_table->action_clear_selection->setEnabled(false);
  d_future_table->action_recalculate->setEnabled(false);
  d_future_table->action_fill_row_numbers->setEnabled(false);
  d_future_table->action_fill_random->setEnabled(false);
  d_future_table->action_add_column->setEnabled(false);
  d_future_table->action_clear_table->setEnabled(false);
  d_future_table->action_sort_table->setEnabled(false);
  d_future_table->action_insert_columns->setEnabled(false);
  d_future_table->action_remove_columns->setEnabled(false);
  d_future_table->action_clear_columns->setEnabled(false);
  d_future_table->action_add_columns->setEnabled(false);
  d_future_table->action_normalize_columns->setEnabled(false);
  d_future_table->action_normalize_selection->setEnabled(false);
  d_future_table->action_sort_columns->setEnabled(false);
  d_future_table->action_statistics_columns->setEnabled(false);
  d_future_table->action_type_format->setEnabled(false);
  d_future_table->action_edit_description->setEnabled(false);
  d_future_table->action_insert_rows->setEnabled(false);
  d_future_table->action_remove_rows->setEnabled(false);
  d_future_table->action_clear_rows->setEnabled(false);
  d_future_table->action_add_rows->setEnabled(false);
  d_future_table->action_statistics_rows->setEnabled(false);
  d_future_table->action_toggle_tabbar->setEnabled(false);

  setCaptionPolicy(MyWidget::Both);
  if (d_type == TableStatistics::StatRow) {
    setName(QString(d_base->name()) + "-" + tr("RowStats"));
    setWindowLabel(tr("Row Statistics of %1").arg(base->name()));
    d_future_table->setRowCount(d_targets.size());
    d_future_table->setColumnCount(10);
    setColName(0, tr("Row"));
    setColName(1, tr("Cols"));
    setColName(2, tr("Mean"));
    setColName(3, tr("StandardDev"));
    setColName(4, tr("StandardErr"));
    setColName(5, tr("Variance"));
    setColName(6, tr("Sum"));
    setColName(7, tr("Max"));
    setColName(8, tr("Min"));
    setColName(9, "N");

    for (int i = 0; i < 10; i++) setColumnType(i, AlphaPlot::Numeric);

    Double2StringFilter *pFilter =
        qobject_cast<Double2StringFilter *>(column(0)->outputFilter());
    Q_ASSERT(pFilter != nullptr);
    pFilter->setNumDigits(0);
    pFilter->setNumericFormat('f');
    pFilter = qobject_cast<Double2StringFilter *>(column(1)->outputFilter());
    Q_ASSERT(pFilter != nullptr);
    pFilter->setNumDigits(0);
    pFilter->setNumericFormat('f');
    pFilter = qobject_cast<Double2StringFilter *>(column(9)->outputFilter());
    Q_ASSERT(pFilter != nullptr);
    pFilter->setNumDigits(0);
    pFilter->setNumericFormat('f');

    update(d_base, QString());
  } else if (d_type == TableStatistics::StatColumn) {
    setName(QString(d_base->name()) + "-" + tr("ColStats"));
    setWindowLabel(tr("Column Statistics of %1").arg(base->name()));
    d_future_table->setRowCount(d_targets.size());
    d_future_table->setColumnCount(12);
    setColName(0, tr("Col"));
    setColName(1, tr("Rows"));
    setColName(2, tr("Mean"));
    setColName(3, tr("StandardDev"));
    setColName(4, tr("StandardErr"));
    setColName(5, tr("Variance"));
    setColName(6, tr("Sum"));
    setColName(7, tr("iMax"));
    setColName(8, tr("Max"));
    setColName(9, tr("iMin"));
    setColName(10, tr("Min"));
    setColName(11, "N");

    for (int i = 0; i < 2; i++) setColumnType(i, AlphaPlot::Text);

    for (int i = 2; i < 12; i++) setColumnType(i, AlphaPlot::Numeric);

    Double2StringFilter *pFilter =
        qobject_cast<Double2StringFilter *>(column(7)->outputFilter());
    Q_ASSERT(pFilter != nullptr);
    pFilter->setNumDigits(0);
    pFilter->setNumericFormat('f');
    pFilter = qobject_cast<Double2StringFilter *>(column(9)->outputFilter());
    Q_ASSERT(pFilter != nullptr);
    pFilter->setNumDigits(0);
    pFilter->setNumericFormat('f');
    pFilter = qobject_cast<Double2StringFilter *>(column(11)->outputFilter());
    Q_ASSERT(pFilter != nullptr);
    pFilter->setNumDigits(0);
    pFilter->setNumericFormat('f');

    for (int i = 0; i < d_targets.size(); i++)
      update(d_base, d_base->colName(d_targets.at(i)));
  }
  setColPlotDesignation(0, AlphaPlot::X);
}

void TableStatistics::update(Table *t, const QString &colName) {
  if (t != d_base) return;

  if (d_type == TableStatistics::StatRow) {
    int columns = d_base->numCols();
    if (columns > 0) {
      for (int destRow = 0; destRow < d_targets.size(); destRow++) {
        int srcRow = d_targets[destRow];

        QList<int> validCells;
        for (int col = 0; col < columns; col++) {
          if (d_base->column(col)->rowCount() > srcRow &&
              d_base->column(col)->columnMode() == AlphaPlot::Numeric &&
              !d_base->column(col)->isInvalid(srcRow))
            validCells.append(col);
        }

        column(0)->setValueAt(destRow, srcRow + 1);
        column(1)->setValueAt(destRow, columns);
        if (validCells.count() > 0) {
          double *data = new double[validCells.count()];
          gsl_vector *y = gsl_vector_alloc(validCells.count());

          int index = 0;
          foreach (int col, validCells) {
            double val = d_base->column(col)->valueAt(srcRow);
            gsl_vector_set(y, index, val);
            data[index++] = val;
          }
          double mean = gsl_stats_mean(data, 1, validCells.count());
          double sd = gsl_stats_sd(data, 1, validCells.count());
          double min, max;
          gsl_vector_minmax(y, &min, &max);

          column(2)->setValueAt(destRow, mean);
          column(3)->setValueAt(destRow, sd);
          column(4)->setValueAt(destRow, sd / sqrt(validCells.count()));
          column(5)->setValueAt(
              destRow, gsl_stats_variance(data, 1, validCells.count()));
          column(6)->setValueAt(destRow, mean * validCells.count());
          column(7)->setValueAt(destRow, max);
          column(8)->setValueAt(destRow, min);
          column(9)->setValueAt(destRow, validCells.count());

          gsl_vector_free(y);
          delete[] data;
        } else {
          for (int i = 2; i < 9; i++) column(i)->setInvalid(destRow, true);
          column(9)->setValueAt(destRow, 0);
        }
      }
    }
  } else if (d_type == TableStatistics::StatColumn) {
    for (int destRow = 0; destRow < d_targets.size(); destRow++) {
      if (colName == QString(d_base->name()) + "_" +
                         d_base->colLabel(d_targets[destRow])) {
        int colIndex = d_base->colIndex(colName);
        Column *col = d_base->column(colIndex);

        if (col->columnMode() != AlphaPlot::Numeric) return;

        int rows = col->rowCount();
        if (rows == 0) return;

        QList<int> validCells;
        for (int row = 0; row < rows; ++row) {
          if (!col->isInvalid(row)) validCells.append(row);
        }
        if (validCells.count() == 0) return;

        double *data = new double[validCells.count()];
        gsl_vector *y = gsl_vector_alloc(validCells.count());

        int minIndex = validCells.at(0);
        int maxIndex = validCells.at(0);
        double val = col->valueAt(validCells.at(0));
        gsl_vector_set(y, 0, val);
        data[0] = val;
        double min = val, max = val;
        int index = 0;
        foreach (int row, validCells) {
          if (index > 0) {
            val = col->valueAt(row);
            gsl_vector_set(y, index, val);
            data[index] = val;
            if (val < min) {
              min = val;
              minIndex = row;
            }
            if (val > max) {
              max = val;
              maxIndex = row;
            }
          }
          index++;
        }
        double mean = gsl_stats_mean(data, 1, validCells.count());
        double sd = gsl_stats_sd(data, 1, validCells.count());

        column(0)->setTextAt(destRow, d_base->colLabel(colIndex));
        column(1)->setTextAt(destRow, "[1:" + QString::number(rows) + "]");
        column(2)->setValueAt(destRow, mean);
        column(3)->setValueAt(destRow, sd);
        column(4)->setValueAt(destRow, sd / sqrt(validCells.count()));
        column(5)->setValueAt(destRow,
                              gsl_stats_variance(data, 1, validCells.count()));
        column(6)->setValueAt(destRow, mean * validCells.count());
        column(7)->setValueAt(destRow, maxIndex + 1);
        column(8)->setValueAt(destRow, max);
        column(9)->setValueAt(destRow, minIndex + 1);
        column(10)->setValueAt(destRow, min);
        column(11)->setValueAt(destRow, validCells.count());

        gsl_vector_free(y);
        delete[] data;
      }
    }
  }

  for (int i = 0; i < numCols(); i++)
    emit modifiedData(this, Table::colName(i));
}

void TableStatistics::renameCol(const QString &from, const QString &to) {
  if (d_type == TableStatistics::StatRow) return;
  for (int c = 0; c < d_targets.size(); c++)
    if (from == QString(d_base->name()) + "_" + text(c, 0)) {
      column(0)->setTextAt(c, to.section('_', 1, 1));
      return;
    }
}

void TableStatistics::removeCol(const QString &col) {
  if (d_type == TableStatistics::StatRow) {
    update(d_base, col);
    return;
  }
  for (int c = 0; c < d_targets.size(); c++)
    if (col == QString(d_base->name()) + "_" + text(c, 0)) {
      d_targets.removeAt(d_targets.at(c));
      d_future_table->removeRows(c, 1);
      return;
    }
}

bool TableStatistics::eventFilter(QObject *watched, QEvent *event) {
  QHeaderView *v_header = d_view_widget->verticalHeader();

  if (event->type() == QEvent::ContextMenu) {
    QContextMenuEvent *cm_event = dynamic_cast<QContextMenuEvent *>(event);
    QPoint global_pos = cm_event->globalPos();
    if (watched == v_header) {
      // no enabled actions for rows
    } else if (watched == d_horizontal_header) {
      QMenu context_menu;

      if (d_future_table->d_plot_menu) {
        context_menu.addMenu(d_future_table->d_plot_menu);
        context_menu.addSeparator();
      }

      QMenu *submenu = new QMenu(tr("S&et Column(s) As"));
      submenu->addAction(d_future_table->action_set_as_x);
      submenu->addAction(d_future_table->action_set_as_y);
      submenu->addAction(d_future_table->action_set_as_z);
      submenu->addSeparator();
      submenu->addAction(d_future_table->action_set_as_xerr);
      submenu->addAction(d_future_table->action_set_as_yerr);
      submenu->addSeparator();
      submenu->addAction(d_future_table->action_set_as_none);
      context_menu.addMenu(submenu);
      context_menu.addSeparator();

      connect(&context_menu, &QMenu::aboutToShow, d_future_table,
              &future::Table::adjustActionNames);
      context_menu.addAction(d_future_table->action_toggle_comments);

      context_menu.exec(global_pos);
    } else if (watched == d_view_widget) {
      QMenu context_menu;

      if (d_future_table->d_plot_menu) {
        context_menu.addMenu(d_future_table->d_plot_menu);
        context_menu.addSeparator();
      }

      context_menu.addAction(d_future_table->action_copy_selection);
      context_menu.addSeparator();
      context_menu.addSeparator();

      connect(&context_menu, &QMenu::aboutToShow, d_future_table,
              &future::Table::adjustActionNames);
      context_menu.addAction(d_future_table->action_toggle_comments);
      context_menu.addSeparator();
      context_menu.addAction(d_future_table->action_select_all);
      context_menu.addSeparator();
      context_menu.addAction(d_future_table->action_go_to_cell);

      context_menu.exec(global_pos);
    } else
      return TableView::eventFilter(watched, event);

    return true;
  } else
    return TableView::eventFilter(watched, event);
}
