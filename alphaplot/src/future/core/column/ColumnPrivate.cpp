/***************************************************************************
    File                 : ColumnPrivate.cpp
    Project              : AlphaPlot
    Description          : Private data class of Column
    --------------------------------------------------------------------
    Copyright            : (C) 2007-2009 Tilman Benkert (thzs*gmx.net)
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

#include "core/column/ColumnPrivate.h"

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QtDebug>
#include <cmath>

#include "core/AbstractSimpleFilter.h"
#include "core/AppearanceManager.h"
#include "core/column/Column.h"
#include "core/datatypes/DateTime2DoubleFilter.h"
#include "core/datatypes/DateTime2StringFilter.h"
#include "core/datatypes/DayOfWeek2DoubleFilter.h"
#include "core/datatypes/Double2DateTimeFilter.h"
#include "core/datatypes/Double2DayOfWeekFilter.h"
#include "core/datatypes/Double2MonthFilter.h"
#include "core/datatypes/Double2StringFilter.h"
#include "core/datatypes/Month2DoubleFilter.h"
#include "core/datatypes/SimpleCopyThroughFilter.h"
#include "core/datatypes/String2DateTimeFilter.h"
#include "core/datatypes/String2DayOfWeekFilter.h"
#include "core/datatypes/String2DoubleFilter.h"
#include "core/datatypes/String2MonthFilter.h"

Column::Private::Private(Column* owner, AlphaPlot::ColumnMode mode)
    : d_owner(owner) {
  Q_ASSERT(owner != 0);  // a Column::Private without owner is not allowed
  // because the owner must become the parent aspect of the input and output
  // filters
  d_column_mode = mode;
  d_column_mode_lock = 0;
  switch (mode) {
    case AlphaPlot::Numeric:
      d_input_filter = new String2DoubleFilter();
      d_output_filter = new Double2StringFilter();
#ifdef LEGACY_CODE_0_2_x  // TODO: in a later version this must use the new
                          // global setting method
      {
        QSettings settings;
        settings.beginGroup("General");
        static_cast<Double2StringFilter*>(d_output_filter)
            ->setNumDigits(settings.value("DecimalDigits", 14).toInt());
        static_cast<Double2StringFilter*>(d_output_filter)
            ->setNumericFormat(settings.value("DefaultNumericFormat", 'f')
                                   .toChar()
                                   .toLatin1());
      }
#endif
      connect(static_cast<Double2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      d_data_type = AlphaPlot::TypeDouble;
      d_data = new QVector<double>();
      break;
    case AlphaPlot::Text:
      d_input_filter = new SimpleCopyThroughFilter();
      d_output_filter = new SimpleCopyThroughFilter();
      d_data_type = AlphaPlot::TypeString;
      d_data = new QStringList();
      break;
    case AlphaPlot::DateTime:
      d_input_filter = new String2DateTimeFilter();
      d_output_filter = new DateTime2StringFilter();
      d_numeric_datetime_filter.reset(new NumericDateTimeBaseFilter());
      connect(static_cast<DateTime2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      d_data_type = AlphaPlot::TypeDateTime;
      d_data = new QList<QDateTime>();
      break;
    case AlphaPlot::Month:
      d_input_filter = new String2MonthFilter();
      d_output_filter = new DateTime2StringFilter();
      static_cast<DateTime2StringFilter*>(d_output_filter)->setFormat("MMMM");
      connect(static_cast<DateTime2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      d_data_type = AlphaPlot::TypeDateTime;
      d_data = new QList<QDateTime>();
      break;
    case AlphaPlot::Day:
      d_input_filter = new String2DayOfWeekFilter();
      d_output_filter = new DateTime2StringFilter();
      static_cast<DateTime2StringFilter*>(d_output_filter)->setFormat("dddd");
      connect(static_cast<DateTime2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      d_data_type = AlphaPlot::TypeDateTime;
      d_data = new QList<QDateTime>();
      break;
  }  // switch(mode)

  d_plot_designation = AlphaPlot::noDesignation;
  d_plot_designation_color = AppearanceManager::noneColorCode;
  d_input_filter->setName("InputFilter");
  d_output_filter->setName("OutputFilter");
}

Column::Private::Private(Column* owner, AlphaPlot::ColumnDataType type,
                         AlphaPlot::ColumnMode mode, void* data,
                         IntervalAttribute<bool> validity)
    : d_owner(owner) {
  d_data_type = type;
  d_column_mode = mode;
  d_column_mode_lock = 0;
  d_data = data;
  d_validity = validity;

  switch (mode) {
    case AlphaPlot::Numeric:
      d_input_filter = new String2DoubleFilter();
      d_output_filter = new Double2StringFilter();
#ifdef LEGACY_CODE_0_2_x  // TODO: in a later version this must use the new
                          // global setting method
      {
        QSettings settings;
        settings.beginGroup("General");
        static_cast<Double2StringFilter*>(d_output_filter)
            ->setNumDigits(settings.value("DecimalDigits", 14).toInt());
        static_cast<Double2StringFilter*>(d_output_filter)
            ->setNumericFormat(settings.value("DefaultNumericFormat", 'f')
                                   .toChar()
                                   .toLatin1());
      }
#endif
      connect(static_cast<Double2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      break;
    case AlphaPlot::Text:
      d_input_filter = new SimpleCopyThroughFilter();
      d_output_filter = new SimpleCopyThroughFilter();
      break;
    case AlphaPlot::DateTime:
      d_input_filter = new String2DateTimeFilter();
      d_output_filter = new DateTime2StringFilter();
      d_numeric_datetime_filter.reset(new NumericDateTimeBaseFilter());
      connect(static_cast<DateTime2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      break;
    case AlphaPlot::Month:
      d_input_filter = new String2MonthFilter();
      d_output_filter = new DateTime2StringFilter();
      static_cast<DateTime2StringFilter*>(d_output_filter)->setFormat("MMMM");
      connect(static_cast<DateTime2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      break;
    case AlphaPlot::Day:
      d_input_filter = new String2DayOfWeekFilter();
      d_output_filter = new DateTime2StringFilter();
      static_cast<DateTime2StringFilter*>(d_output_filter)->setFormat("dddd");
      connect(static_cast<DateTime2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      break;
  }  // switch(mode)

  d_plot_designation = AlphaPlot::noDesignation;
  d_plot_designation_color = AppearanceManager::noneColorCode;
  d_input_filter->setName("InputFilter");
  d_output_filter->setName("OutputFilter");
}

bool Column::Private::columnModeLock() const {
  if (d_column_mode_lock <= 0)
    return false;
  else
    return true;
}

Column::Private::~Private() {
  if (!d_data) return;

  switch (d_data_type) {
    case AlphaPlot::TypeDouble:
      delete static_cast<QVector<double>*>(d_data);
      break;

    case AlphaPlot::TypeString:
      delete static_cast<QStringList*>(d_data);
      break;

    case AlphaPlot::TypeDateTime:
    case AlphaPlot::TypeDay:
    case AlphaPlot::TypeMonth:
      delete static_cast<QList<QDateTime>*>(d_data);
      break;
  }  // switch(d_data_type)
}

void Column::Private::setColumnMode(const AlphaPlot::ColumnMode new_mode,
                                    AbstractFilter* converter) {
  const auto& old_mode = d_column_mode;
  if (new_mode == old_mode) return;
  void* old_data = d_data;
  // remark: the deletion of the old data will be done in the dtor of a command

  AbstractSimpleFilter *new_in_filter, *new_out_filter;
  bool filter_is_temporary{
      true};  // it can also become outputFilter(), which we may not delete here
  QScopedPointer<Column> temp_col;

  if (nullptr != converter) filter_is_temporary = false;

  emit d_owner->modeAboutToChange(d_owner);
  // prepare new d_data, d_data_type and filters
  switch (new_mode) {
    case AlphaPlot::ColumnMode::Numeric: {
      d_data = new QVector<double>();
      d_data_type = AlphaPlot::TypeDouble;
      new_in_filter = new String2DoubleFilter();
      new_out_filter = new Double2StringFilter();

#ifdef LEGACY_CODE_0_2_x  // TODO: in a later version this must use the new
                          // global setting method
      {
        QSettings settings;
        settings.beginGroup("General");
        static_cast<Double2StringFilter*>(new_out_filter)
            ->setNumDigits(settings.value("DecimalDigits", 14).toInt());
        static_cast<Double2StringFilter*>(new_out_filter)
            ->setNumericFormat(settings.value("DefaultNumericFormat", 'f')
                                   .toChar()
                                   .toLatin1());
        settings.endGroup();
      }
#endif
      connect(static_cast<Double2StringFilter*>(new_out_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      // if converter is not provided
      if (nullptr == converter) {
        switch (old_mode) {
          case AlphaPlot::ColumnMode::Text:
            converter = new String2DoubleFilter();
            break;
          case AlphaPlot::ColumnMode::Month:
            converter = new Month2DoubleFilter();
            break;
          case AlphaPlot::ColumnMode::Day:
            converter = new DayOfWeek2DoubleFilter();
            break;
          case AlphaPlot::ColumnMode::DateTime:
            // use existing (or default) converter to get reciprocal converter
            converter = new DateTime2DoubleFilter(*getNumericDateTimeFilter());
            break;
          case AlphaPlot::ColumnMode::Numeric:
            throw("Unreachable line is reached in ColumnPrivate.cpp!");
        }
      }
      break;
    }
    case AlphaPlot::ColumnMode::Text: {
      d_data = new QStringList();
      d_data_type = AlphaPlot::TypeString;
      new_in_filter = new SimpleCopyThroughFilter();
      new_out_filter = new SimpleCopyThroughFilter();
      if (nullptr == converter) {
        converter = outputFilter();
        filter_is_temporary = false;
      }
      break;
    }
    case AlphaPlot::ColumnMode::DateTime: {
      new_in_filter = new String2DateTimeFilter();
      new_out_filter = new DateTime2StringFilter();
      d_numeric_datetime_filter.reset(new NumericDateTimeBaseFilter());
      if ((AlphaPlot::ColumnMode::DateTime != old_mode) &&
          (AlphaPlot::ColumnMode::Month != old_mode) &&
          (AlphaPlot::ColumnMode::Day != old_mode)) {
        d_data = new QList<QDateTime>();
        d_data_type = AlphaPlot::TypeDateTime;
      }
      connect(static_cast<DateTime2StringFilter*>(new_out_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      if (nullptr == converter) {
        switch (old_mode) {
          case AlphaPlot::ColumnMode::Numeric:
            // use existing (or default) converter to get reciprocal converter
            converter = new Double2DateTimeFilter(*getNumericDateTimeFilter());
            break;
          case AlphaPlot::ColumnMode::Text:
            converter = new String2DateTimeFilter();
            break;
          case AlphaPlot::ColumnMode::Month:
            break;
          case AlphaPlot::ColumnMode::Day:
            break;
          case AlphaPlot::ColumnMode::DateTime:
            throw("Unreachable line is reached in ColumnPrivate.cpp!");
        }
      }
      // converter is provided, need to store for possible double -> datetime
      // conversion later
      else if (old_mode == AlphaPlot::ColumnMode::Numeric) {
        auto numeric_datetime_converter =
            reinterpret_cast<NumericDateTimeBaseFilter*>(converter);
        if (nullptr != numeric_datetime_converter)
          // the ownership of converter is not taken, copy is stored
          setNumericDateTimeFilter(
              new NumericDateTimeBaseFilter(*numeric_datetime_converter));
      }
      break;
    }
    case AlphaPlot::ColumnMode::Month: {
      new_in_filter = new String2MonthFilter();
      new_out_filter = new DateTime2StringFilter();
      if ((AlphaPlot::ColumnMode::DateTime != old_mode) &&
          (AlphaPlot::ColumnMode::Month != old_mode) &&
          (AlphaPlot::ColumnMode::Day != old_mode)) {
        d_data = new QList<QDateTime>();
        d_data_type = AlphaPlot::TypeDateTime;
      }
      static_cast<DateTime2StringFilter*>(new_out_filter)->setFormat("MMMM");
      connect(static_cast<DateTime2StringFilter*>(new_out_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      if (nullptr == converter) {
        switch (old_mode) {
          case AlphaPlot::ColumnMode::Numeric:
            converter = new Double2MonthFilter();
            break;
          case AlphaPlot::ColumnMode::Text:
            converter = new String2MonthFilter();
            break;
          case AlphaPlot::ColumnMode::Month:
            throw("Unreachable line is reached in ColumnPrivate.cpp!");
          case AlphaPlot::ColumnMode::DateTime:
            break;
          case AlphaPlot::ColumnMode::Day:
            break;
        }
      }
      break;
    }
    case AlphaPlot::ColumnMode::Day: {
      new_in_filter = new String2DayOfWeekFilter();
      new_out_filter = new DateTime2StringFilter();
      if ((AlphaPlot::ColumnMode::DateTime != old_mode) &&
          (AlphaPlot::ColumnMode::Month != old_mode) &&
          (AlphaPlot::ColumnMode::Day != old_mode)) {
        d_data = new QList<QDateTime>();
        d_data_type = AlphaPlot::TypeDateTime;
      }
      static_cast<DateTime2StringFilter*>(new_out_filter)->setFormat("dddd");
      connect(static_cast<DateTime2StringFilter*>(new_out_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      if (nullptr == converter) {
        switch (old_mode) {
          case AlphaPlot::ColumnMode::Numeric:
            converter = new Double2DayOfWeekFilter();
            break;
          case AlphaPlot::ColumnMode::Text:
            converter = new String2DayOfWeekFilter();
            break;
          case AlphaPlot::ColumnMode::Month:
            break;
          case AlphaPlot::ColumnMode::DateTime:
            break;
          case AlphaPlot::ColumnMode::Day:
            throw("Unreachable line is reached in ColumnPrivate.cpp!");
        }
      }
      break;
    }
    default:
      qDebug() << "Switching column to an invalid mode!";
      return;
  }

  // prepare temporary data
  switch (old_mode) {
    case AlphaPlot::ColumnMode::Numeric: {
      disconnect(static_cast<Double2StringFilter*>(d_output_filter),
                 SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      temp_col.reset(new Column(
          "temp_col", *(static_cast<QVector<qreal>*>(old_data)), d_validity));
      break;
    }
    case AlphaPlot::ColumnMode::Text: {
      temp_col.reset(new Column(
          "temp_col", *(static_cast<QStringList*>(old_data)), d_validity));
      break;
    }
    case AlphaPlot::ColumnMode::DateTime:  // fallthrough intended
    case AlphaPlot::ColumnMode::Month:
    case AlphaPlot::ColumnMode::Day: {
      disconnect(static_cast<DateTime2StringFilter*>(d_output_filter),
                 SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      if ((AlphaPlot::ColumnMode::DateTime != new_mode) &&
          (AlphaPlot::ColumnMode::Month != new_mode) &&
          (AlphaPlot::ColumnMode::Day != new_mode))
        temp_col.reset(new Column("temp_col",
                                  *(static_cast<QList<QDateTime>*>(old_data)),
                                  d_validity));
      break;
    }
  }

  d_column_mode = new_mode;
  new_in_filter->setName("InputFilter");
  new_out_filter->setName("OutputFilter");
  d_input_filter = new_in_filter;
  d_output_filter = new_out_filter;
  d_input_filter->input(0, d_owner->d_string_io);
  d_output_filter->input(0, d_owner);

  if (temp_col)  // if temp_col == 0, only the input/output filters need to be
                 // changed
  {
    // copy the filtered, i.e. converted, column
    converter->input(0, temp_col.data());
    copy(converter->output(0));
  }

  emit d_owner->modeChanged(d_owner);
  if (filter_is_temporary) delete converter;
}

void Column::Private::setColumnModeLock(const bool lock) {
  (lock) ? d_column_mode_lock = d_column_mode_lock + 1
         : d_column_mode_lock = d_column_mode_lock - 1;
}

void Column::Private::replaceModeData(AlphaPlot::ColumnMode mode,
                                      AlphaPlot::ColumnDataType type,
                                      void* data,
                                      AbstractSimpleFilter* in_filter,
                                      AbstractSimpleFilter* out_filter,
                                      IntervalAttribute<bool> validity) {
  if (d_column_mode_lock == true) return;

  emit d_owner->modeAboutToChange(d_owner);
  // disconnect formatChanged()
  switch (d_column_mode) {
    case AlphaPlot::Numeric:
      disconnect(static_cast<Double2StringFilter*>(d_output_filter),
                 SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      break;
    case AlphaPlot::DateTime:
    case AlphaPlot::Month:
    case AlphaPlot::Day:
      disconnect(static_cast<DateTime2StringFilter*>(d_output_filter),
                 SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      break;
    default:
      break;
  }

  d_column_mode = mode;
  d_data_type = type;
  d_data = data;

  in_filter->setName("InputFilter");
  out_filter->setName("OutputFilter");
  d_input_filter = in_filter;
  d_output_filter = out_filter;
  d_input_filter->input(0, d_owner->d_string_io);
  d_output_filter->input(0, d_owner);

  // connect formatChanged()
  switch (d_column_mode) {
    case AlphaPlot::Numeric:
      connect(static_cast<Double2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      break;
    case AlphaPlot::DateTime:
    case AlphaPlot::Month:
    case AlphaPlot::Day:
      connect(static_cast<DateTime2StringFilter*>(d_output_filter),
              SIGNAL(formatChanged()), d_owner, SLOT(notifyDisplayChange()));
      break;
    default:
      break;
  }

  d_validity = validity;
  emit d_owner->modeChanged(d_owner);
}

void Column::Private::replaceData(void* data,
                                  IntervalAttribute<bool> validity) {
  emit d_owner->dataAboutToChange(d_owner);
  d_data = data;
  d_validity = validity;
  emit d_owner->dataChanged(d_owner);
}

bool Column::Private::copy(const AbstractColumn* other) {
  if (other->dataType() != dataType()) return false;
  int num_rows = other->rowCount();

  emit d_owner->dataAboutToChange(d_owner);
  resizeTo(num_rows);

  // copy the data
  switch (d_data_type) {
    case AlphaPlot::TypeDouble: {
      double* ptr = static_cast<QVector<double>*>(d_data)->data();
      for (int i = 0; i < num_rows; i++) ptr[i] = other->valueAt(i);
      break;
    }
    case AlphaPlot::TypeString: {
      for (int i = 0; i < num_rows; i++)
        static_cast<QStringList*>(d_data)->replace(i, other->textAt(i));
      break;
    }
    case AlphaPlot::TypeDateTime:
    case AlphaPlot::TypeDay:
    case AlphaPlot::TypeMonth: {
      for (int i = 0; i < num_rows; i++)
        static_cast<QList<QDateTime>*>(d_data)->replace(i,
                                                        other->dateTimeAt(i));
      break;
    }
  }
  // copy the validity information
  d_validity = other->invalidIntervals();

  emit d_owner->dataChanged(d_owner);

  return true;
}

bool Column::Private::copy(const AbstractColumn* source, int source_start,
                           int dest_start, int num_rows) {
  if (source->dataType() != dataType()) return false;
  if (num_rows == 0) return true;

  emit d_owner->dataAboutToChange(d_owner);
  if (dest_start + 1 - rowCount() > 1)
    d_validity.setValue(Interval<int>(rowCount(), dest_start - 1), true);
  if (dest_start + num_rows > rowCount()) resizeTo(dest_start + num_rows);

  // copy the data
  switch (d_data_type) {
    case AlphaPlot::TypeDouble: {
      double* ptr = static_cast<QVector<double>*>(d_data)->data();
      for (int i = 0; i < num_rows; i++)
        ptr[dest_start + i] = source->valueAt(source_start + i);
      break;
    }
    case AlphaPlot::TypeString:
      for (int i = 0; i < num_rows; i++)
        static_cast<QStringList*>(d_data)->replace(
            dest_start + i, source->textAt(source_start + i));
      break;
    case AlphaPlot::TypeDateTime:
    case AlphaPlot::TypeDay:
    case AlphaPlot::TypeMonth:
      for (int i = 0; i < num_rows; i++)
        static_cast<QList<QDateTime>*>(d_data)->replace(
            dest_start + i, source->dateTimeAt(source_start + i));
      break;
  }
  // copy the validity information
  for (int i = 0; i < num_rows; i++)
    d_validity.setValue(dest_start + i, source->isInvalid(source_start + i));

  emit d_owner->dataChanged(d_owner);

  return true;
}

bool Column::Private::copy(const Private* other) {
  if (other->dataType() != dataType()) return false;
  int num_rows = other->rowCount();

  emit d_owner->dataAboutToChange(d_owner);
  resizeTo(num_rows);

  // copy the data
  switch (d_data_type) {
    case AlphaPlot::TypeDouble: {
      double* ptr = static_cast<QVector<double>*>(d_data)->data();
      for (int i = 0; i < num_rows; i++) ptr[i] = other->valueAt(i);
      break;
    }
    case AlphaPlot::TypeString: {
      for (int i = 0; i < num_rows; i++)
        static_cast<QStringList*>(d_data)->replace(i, other->textAt(i));
      break;
    }
    case AlphaPlot::TypeDateTime:
    case AlphaPlot::TypeDay:
    case AlphaPlot::TypeMonth: {
      for (int i = 0; i < num_rows; i++)
        static_cast<QList<QDateTime>*>(d_data)->replace(i,
                                                        other->dateTimeAt(i));
      break;
    }
  }
  // copy the validity information
  d_validity = other->invalidIntervals();

  emit d_owner->dataChanged(d_owner);

  return true;
}

bool Column::Private::copy(const Private* source, int source_start,
                           int dest_start, int num_rows) {
  if (source->dataType() != dataType()) return false;
  if (num_rows == 0) return true;

  emit d_owner->dataAboutToChange(d_owner);
  if (dest_start + 1 - rowCount() > 1)
    d_validity.setValue(Interval<int>(rowCount(), dest_start - 1), true);
  if (dest_start + num_rows > rowCount()) resizeTo(dest_start + num_rows);

  // copy the data
  switch (d_data_type) {
    case AlphaPlot::TypeDouble: {
      double* ptr = static_cast<QVector<double>*>(d_data)->data();
      for (int i = 0; i < num_rows; i++)
        ptr[dest_start + i] = source->valueAt(source_start + i);
      break;
    }
    case AlphaPlot::TypeString:
      for (int i = 0; i < num_rows; i++)
        static_cast<QStringList*>(d_data)->replace(
            dest_start + i, source->textAt(source_start + i));
      break;
    case AlphaPlot::TypeDateTime:
    case AlphaPlot::TypeDay:
    case AlphaPlot::TypeMonth:
      for (int i = 0; i < num_rows; i++)
        static_cast<QList<QDateTime>*>(d_data)->replace(
            dest_start + i, source->dateTimeAt(source_start + i));
      break;
  }
  // copy the validity information
  for (int i = 0; i < num_rows; i++)
    d_validity.setValue(dest_start + i, source->isInvalid(source_start + i));

  emit d_owner->dataChanged(d_owner);

  return true;
}

int Column::Private::rowCount() const {
  switch (d_data_type) {
    case AlphaPlot::TypeDouble:
      return static_cast<QVector<double>*>(d_data)->size();
    case AlphaPlot::TypeDateTime:
    case AlphaPlot::TypeDay:
    case AlphaPlot::TypeMonth:
      return static_cast<QList<QDateTime>*>(d_data)->size();
    case AlphaPlot::TypeString:
      return static_cast<QStringList*>(d_data)->size();
  }

  return 0;
}

void Column::Private::resizeTo(int new_size) {
  int old_size = rowCount();
  if (new_size == old_size) return;

  switch (d_data_type) {
    case AlphaPlot::TypeDouble:
      static_cast<QVector<double>*>(d_data)->resize(new_size);
      break;
    case AlphaPlot::TypeDateTime:
    case AlphaPlot::TypeDay:
    case AlphaPlot::TypeMonth: {
      int new_rows = new_size - old_size;
      if (new_rows > 0) {
        for (int i = 0; i < new_rows; i++)
          static_cast<QList<QDateTime>*>(d_data)->append(QDateTime());
      } else {
        for (int i = 0; i < -new_rows; i++)
          static_cast<QList<QDateTime>*>(d_data)->removeLast();
      }
      break;
    }
    case AlphaPlot::TypeString: {
      int new_rows = new_size - old_size;
      if (new_rows > 0) {
        for (int i = 0; i < new_rows; i++)
          static_cast<QStringList*>(d_data)->append(QString());
      } else {
        for (int i = 0; i < -new_rows; i++)
          static_cast<QStringList*>(d_data)->removeLast();
      }
      break;
    }
  }
}

void Column::Private::insertRows(int before, int count) {
  if (count == 0) return;

  emit d_owner->rowsAboutToBeInserted(d_owner, before, count);
  d_validity.insertRows(before, count);
  d_masking.insertRows(before, count);
  d_formulas.insertRows(before, count);

  if (before <= rowCount()) {
    d_validity.setValue(Interval<int>(before, before + count - 1), true);
    switch (d_data_type) {
      case AlphaPlot::TypeDouble:
        static_cast<QVector<double>*>(d_data)->insert(before, count, 0.0);
        break;
      case AlphaPlot::TypeDateTime:
      case AlphaPlot::TypeDay:
      case AlphaPlot::TypeMonth:
        for (int i = 0; i < count; i++)
          static_cast<QList<QDateTime>*>(d_data)->insert(before, QDateTime());
        break;
      case AlphaPlot::TypeString:
        for (int i = 0; i < count; i++)
          static_cast<QStringList*>(d_data)->insert(before, QString());
        break;
    }
  }
  emit d_owner->rowsInserted(d_owner, before, count);
}

void Column::Private::removeRows(int first, int count) {
  if (count == 0) return;

  emit d_owner->rowsAboutToBeRemoved(d_owner, first, count);
  d_validity.removeRows(first, count);
  d_masking.removeRows(first, count);
  d_formulas.removeRows(first, count);

  if (first < rowCount()) {
    int corrected_count = count;
    if (first + count > rowCount()) corrected_count = rowCount() - first;

    switch (d_data_type) {
      case AlphaPlot::TypeDouble:
        static_cast<QVector<double>*>(d_data)->remove(first, corrected_count);
        break;
      case AlphaPlot::TypeDateTime:
      case AlphaPlot::TypeDay:
      case AlphaPlot::TypeMonth:
        for (int i = 0; i < corrected_count; i++)
          static_cast<QList<QDateTime>*>(d_data)->removeAt(first);
        break;
      case AlphaPlot::TypeString:
        for (int i = 0; i < corrected_count; i++)
          static_cast<QStringList*>(d_data)->removeAt(first);
        break;
    }
  }
  emit d_owner->rowsRemoved(d_owner, first, count);
}

void Column::Private::setPlotDesignation(AlphaPlot::PlotDesignation pd) {
  emit d_owner->plotDesignationAboutToChange(d_owner);
  d_plot_designation = pd;
  setPlotDesignationColor(pd);
  emit d_owner->plotDesignationChanged(d_owner);
}

void Column::Private::setPlotDesignationColor(AlphaPlot::PlotDesignation pd) {
  switch (pd) {
    case AlphaPlot::X:
      d_plot_designation_color = AppearanceManager::xColorCode;
      break;
    case AlphaPlot::Y:
      d_plot_designation_color = AppearanceManager::yColorCode;
      break;
    case AlphaPlot::Z:
      d_plot_designation_color = AppearanceManager::zColorCode;
      break;
    case AlphaPlot::xErr:
      d_plot_designation_color = AppearanceManager::xErrColorCode;
      break;
    case AlphaPlot::yErr:
      d_plot_designation_color = AppearanceManager::yErrColorCode;
      break;
    case AlphaPlot::noDesignation:
      d_plot_designation_color = AppearanceManager::noneColorCode;
      break;
  }
}

void Column::Private::clear() { removeRows(0, rowCount()); }

void Column::Private::clearValidity() {
  emit d_owner->dataAboutToChange(d_owner);
  d_validity.clear();
  emit d_owner->dataChanged(d_owner);
}

void Column::Private::clearMasks() {
  emit d_owner->maskingAboutToChange(d_owner);
  d_masking.clear();
  emit d_owner->maskingChanged(d_owner);
}

void Column::Private::setInvalid(Interval<int> i, bool invalid) {
  emit d_owner->dataAboutToChange(d_owner);
  d_validity.setValue(i, invalid);
  emit d_owner->dataChanged(d_owner);
}

void Column::Private::setInvalid(int row, bool invalid) {
  setInvalid(Interval<int>(row, row), invalid);
}

void Column::Private::setMasked(Interval<int> i, bool mask) {
  emit d_owner->maskingAboutToChange(d_owner);
  d_masking.setValue(i, mask);
  emit d_owner->maskingChanged(d_owner);
}

void Column::Private::setMasked(int row, bool mask) {
  setMasked(Interval<int>(row, row), mask);
}

void Column::Private::setFormula(Interval<int> i, QString formula) {
  d_formulas.setValue(i, formula);
}

void Column::Private::setFormula(int row, QString formula) {
  setFormula(Interval<int>(row, row), formula);
}

void Column::Private::clearFormulas() { d_formulas.clear(); }

QString Column::Private::textAt(int row) const {
  if (d_data_type != AlphaPlot::TypeString) return QString();
  return static_cast<QStringList*>(d_data)->value(row);
}

QDate Column::Private::dateAt(int row) const { return dateTimeAt(row).date(); }

QTime Column::Private::timeAt(int row) const { return dateTimeAt(row).time(); }

QDateTime Column::Private::dateTimeAt(int row) const {
  if (d_data_type != AlphaPlot::TypeDateTime) return QDateTime();
  return static_cast<QList<QDateTime>*>(d_data)->value(row);
}

double Column::Private::valueAt(int row) const {
  if (d_data_type != AlphaPlot::TypeDouble) return 0.0;
  return static_cast<QVector<double>*>(d_data)->value(row);
}

void Column::Private::setTextAt(int row, const QString& new_value) {
  if (d_data_type != AlphaPlot::TypeString) return;

  emit d_owner->dataAboutToChange(d_owner);
  if (row >= rowCount()) {
    if (row + 1 - rowCount() >
        1)  // we are adding more than one row in resizeTo()
      d_validity.setValue(Interval<int>(rowCount(), row - 1), true);
    resizeTo(row + 1);
  }

  static_cast<QStringList*>(d_data)->replace(row, new_value);
  d_validity.setValue(Interval<int>(row, row), false);
  emit d_owner->dataChanged(d_owner);
}

void Column::Private::replaceTexts(int first, const QStringList& new_values) {
  if (d_data_type != AlphaPlot::TypeString) return;

  emit d_owner->dataAboutToChange(d_owner);
  int num_rows = new_values.size();
  if (first + 1 - rowCount() > 1)
    d_validity.setValue(Interval<int>(rowCount(), first - 1), true);
  if (first + num_rows > rowCount()) resizeTo(first + num_rows);

  for (int i = 0; i < num_rows; i++)
    static_cast<QStringList*>(d_data)->replace(first + i, new_values.at(i));
  d_validity.setValue(Interval<int>(first, first + num_rows - 1), false);
  emit d_owner->dataChanged(d_owner);
}

void Column::Private::setDateAt(int row, const QDate& new_value) {
  if (d_data_type != AlphaPlot::TypeDateTime) return;

  setDateTimeAt(row, QDateTime(new_value, timeAt(row)));
}

void Column::Private::setTimeAt(int row, const QTime& new_value) {
  if (d_data_type != AlphaPlot::TypeDateTime) return;

  setDateTimeAt(row, QDateTime(dateAt(row), new_value));
}

void Column::Private::setDateTimeAt(int row, const QDateTime& new_value) {
  if (d_data_type != AlphaPlot::TypeDateTime) return;

  emit d_owner->dataAboutToChange(d_owner);
  if (row >= rowCount()) {
    if (row + 1 - rowCount() >
        1)  // we are adding more than one row in resizeTo()
      d_validity.setValue(Interval<int>(rowCount(), row - 1), true);
    resizeTo(row + 1);
  }

  static_cast<QList<QDateTime>*>(d_data)->replace(row, new_value);
  d_validity.setValue(Interval<int>(row, row), !new_value.isValid());
  emit d_owner->dataChanged(d_owner);
}

void Column::Private::replaceDateTimes(int first,
                                       const QList<QDateTime>& new_values) {
  if (d_data_type != AlphaPlot::TypeDateTime) return;

  emit d_owner->dataAboutToChange(d_owner);
  int num_rows = new_values.size();
  if (first + 1 - rowCount() > 1)
    d_validity.setValue(Interval<int>(rowCount(), first - 1), true);
  if (first + num_rows > rowCount()) resizeTo(first + num_rows);

  for (int i = 0; i < num_rows; i++) {
    static_cast<QList<QDateTime>*>(d_data)->replace(first + i,
                                                    new_values.at(i));
    d_validity.setValue(i, !new_values.at(i).isValid());
  }
  emit d_owner->dataChanged(d_owner);
}

void Column::Private::setValueAt(int row, double new_value) {
  if (d_data_type != AlphaPlot::TypeDouble) return;

  emit d_owner->dataAboutToChange(d_owner);
  if (row >= rowCount()) {
    if (row + 1 - rowCount() >
        1)  // we are adding more than one row in resizeTo()
      d_validity.setValue(Interval<int>(rowCount(), row - 1), true);
    resizeTo(row + 1);
  }

  static_cast<QVector<double>*>(d_data)->replace(row, new_value);
  d_validity.setValue(Interval<int>(row, row), false);
  emit d_owner->dataChanged(d_owner);
}

void Column::Private::replaceValues(int first,
                                    const QVector<qreal>& new_values) {
  if (d_data_type != AlphaPlot::TypeDouble) return;

  emit d_owner->dataAboutToChange(d_owner);
  int num_rows = new_values.size();
  if (first + 1 - rowCount() > 1)
    d_validity.setValue(Interval<int>(rowCount(), first - 1), true);
  if (first + num_rows > rowCount()) resizeTo(first + num_rows);

  double* ptr = static_cast<QVector<double>*>(d_data)->data();
  for (int i = 0; i < num_rows; i++) ptr[first + i] = new_values.at(i);
  d_validity.setValue(Interval<int>(first, first + num_rows - 1), false);
  emit d_owner->dataChanged(d_owner);
}

NumericDateTimeBaseFilter* Column::Private::getNumericDateTimeFilter() {
  return d_numeric_datetime_filter.data();
}

void Column::Private::setNumericDateTimeFilter(
    NumericDateTimeBaseFilter* const newFilter) {
  d_numeric_datetime_filter.reset(newFilter);
}

void Column::Private::replaceMasking(IntervalAttribute<bool> masking) {
  emit d_owner->maskingAboutToChange(d_owner);
  d_masking = masking;
  emit d_owner->maskingChanged(d_owner);
}

void Column::Private::replaceFormulas(IntervalAttribute<QString> formulas) {
  d_formulas = formulas;
}

QString Column::Private::name() const { return d_owner->name(); }

QString Column::Private::comment() const { return d_owner->comment(); }
