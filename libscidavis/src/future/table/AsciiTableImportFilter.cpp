/***************************************************************************
    File                 : AsciiTableImportFilter.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2008-2009 Knut Franke
    Email (use @ for *)  : Knut.Franke*gmx.net
    Description          : Import an ASCII file as Table.

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

#include "table/AsciiTableImportFilter.h"
#include "table/future_Table.h"
#include "lib/IntervalAttribute.h"
#include "core/column/Column.h"
#include "core/datatypes/String2DoubleFilter.h"

#include <QTextStream>
#include <QStringList>

#include <vector>
#include <iostream>
using namespace std;

QStringList AsciiTableImportFilter::fileExtensions() const
{
	return QStringList() << "txt" << "csv" << "dat";
}

namespace
{
  // redirect to QIODevice's readLine so that we can override it to handle '\r' line terminators
  struct SciDaVisTextStream
  {
    QIODevice& input;
    bool good;
    operator bool() const {return good;}
    enum {none, simplify, trim} whiteSpaceTreatment;
    QString separator;
    SciDaVisTextStream(QIODevice& inp, const QString& sep): 
      input(inp), good(true), whiteSpaceTreatment(none),
      separator(sep) {}

    QStringList readRow()
    {
      char c;
      QString r;
   
      while ((good=input.getChar(&c)))
        switch (c)
          {
          case '\r':
            if (input.getChar(&c) && c!='\n') // eat \n following \r
              input.ungetChar(c);
            goto breakLoop;
          case '\n':
            goto breakLoop;
          default:
            r+=c;
          };
    breakLoop:
      switch (whiteSpaceTreatment)
        {
        case none:
          return r.split(separator);
        case simplify:
          return r.simplified().split(separator);
        case trim:
          return r.trimmed().split(separator);
        default:
          return QStringList();
        }
    }
  };

  template <class C> C conv(const QString& x);
  template <> QString conv<QString>(const QString& x) {return x;}
  template <> double conv<double>(const QString& x) {return x.toDouble();}
  template <> float conv<float>(const QString& x) {return x.toFloat();}
 
  template <class T>
  struct AP: public std::auto_ptr<T>
  {
    AP(): std::auto_ptr<T>(new T) {}
  };

  template <class C>
  void readCols(QList<Column*>& cols, SciDaVisTextStream& stream, 
                bool readColNames)
  {
    QStringList row, column_names;
    int i;

    // read first row
    row = stream.readRow();

    int dataSize=row.size();
    vector<AP<C> > data(dataSize);
    vector<IntervalAttribute<bool> > invalid_cells(row.size());

    if (readColNames)
      column_names = row;
    else
      for (i=0; i<row.size(); ++i) 
        {
          column_names << QString::number(i+1);
          *data[i] << conv<typename C::value_type>(row[i]);
        }

    // read rest of data
    while (stream)
      {
        row = stream.readRow();
        for (i=0; i<row.size() && i<dataSize; ++i)
          *data[i] << conv<typename C::value_type>(row[i]);
        // some rows might have too few columns (re-use value of i from above loop)
        for (; i<dataSize; ++i) {
          invalid_cells[i].setValue(data[i]->size(), true);
          *data[i] << conv<typename C::value_type>("");
        }
      }

    for (i=0; i<dataSize; ++i)
      {
        cols << new Column(column_names[i], auto_ptr<C>(data[i]), invalid_cells[i]);
        if (i == 0) 
          cols.back()->setPlotDesignation(SciDAVis::X);
        else
          cols.back()->setPlotDesignation(SciDAVis::Y);
      }

  }

}

AbstractAspect * AsciiTableImportFilter::importAspect(QIODevice& input)
{
  SciDaVisTextStream stream(input, d_separator);
  if (d_simplify_whitespace)
    stream.whiteSpaceTreatment=SciDaVisTextStream::simplify;
  else if (d_trim_whitespace)
      stream.whiteSpaceTreatment=SciDaVisTextStream::trim;

  // skip ignored lines
  for (int i=0; i<d_ignored_lines; i++)
    stream.readRow();

  // build a Table from the gathered data
  QList<Column*> cols;
  if (d_convert_to_numeric) 
    readCols<QVector<qreal> >(cols, stream, d_first_row_names_columns);
  else
    readCols<QStringList>(cols, stream, d_first_row_names_columns);

  // renaming will be done by the kernel
  future::Table * result = new future::Table(0, 0, 0, tr("Table"));
  result->appendColumns(cols);
  return result;
}

