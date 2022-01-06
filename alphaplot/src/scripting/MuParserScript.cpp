/***************************************************************************
    File                 : MuParserScript.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2009 Knut Franke
    Email (use @ for *)  : Knut.Franke*gmx.net
    Description          : Evaluate mathematical expression using muParser

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

// for NAN macro
#define _ISOC99_SOURCE

#include "MuParserScript.h"
#include "MuParserScripting.h"
#include "future/core/column/Column.h"
#include "Table.h"
#include "Matrix.h"
#include "Folder.h"
#include <math.h>
#include <QtCore/QByteArray>
#include <QtCore/QRegExp>

/**
 * \class MuParserScript
 * \brief Evaluate mathematical expressions using muParser
 *
 * Basically, this is an adapter between the standard mu::Parser and the
 * scripting abstraction
 * layer. However, some additional functionality is also provided.
 *
 * The constants pi, Pi and PI as well as e and E are recognized in addition to
 * _pi and _e,
 * respectively. This makes it possible to easily access them without referring
 * to the
 * documentation. For functions, exploratory expression building is supported by
 * interactive
 * lists of available mathematical functions; so redundant names are not needed
 * for them.
 *
 * In addition to basic mathematical functions provided by mu::Parser, several
 * special functions
 * implemented in the GSL are exported. For a complete list, see
 * #muParserScripting::math_functions.
 *
 * Functionality for user-defined variables in expressions optionally supported
 * by muParser is
 * enabled by providing a variable factory (see variableFactory()).
 *
 * Multiple statements separated by semicolon or newline are supported. Because
 * muParser takes
 * newlines to mean "end of expression", they are translated to semicolons
 * before handing the code
 * to muParser. Also, adjacent newlines/semicolons are simplified to a single
 * semicolon, since
 * empty statements are not possible with muParser. Technically speaking,
 * muParser doesn't have
 * statements at all; here, "statement" is referring to expressions which are
 * only evaluated for
 * side effects (namely, assigning values to user-define variables) and whose
 * results are discarded.
 * This is done by defining the semicolon as a low-precedence operator which
 * always returns its
 * right-hand argument. Also, comments are implemented by throwing away any text
 * between a hash
 * mark '#' and a newline during pre-processing in compile().
 *
 * For backwards-compatibility purposes, we need to support table column/cell
 * access functions with
 * variable numbers of (string) arguments as well as overloaded functions
 * accepting either a number
 * or a string; plus (from old QtiPlot versions) the ambiguous syntax col(name)
 * (where name is an
 * unquoted column name, not a muParser expression as one would expect). All of
 * this isn't supported
 * by muParser and has always required more or less extensive hacks on top of
 * it. The approach taken
 * in this implementation is as follows:
 *
 *  - We define the new, muParser-compatible, functions column(), column_(),
 * column__(), cell() and
 *  cell_(). Implemented in tableColumnFunction(), tableColumn_Function(),
 * tableColumn__Function(),
 *  tableCellFunction() and tableCell_Function(), respectively.
 *
 *  - Backwards-compatible functions are implemented as a pre-processing step in
 * compile(), where
 *  anything that looks like a call to a col(), tablecol() or cell() function is
 * scanned for
 *  argument number and types and translated to an equivalent call of a
 * new-style function. In the
 *  case of col(name), name is simply looked up in the current table; if a
 * column of that name
 *  exists, the call is translated to column("name"); else, it's translated to
 * column_(name)
 *  (so name can be a column number or an expression evaluating to a column
 * number).
 *
 * %Note that contrary to previous versions, the internal cell() function takes
 * a column name instead
 * of a column number argument (with legacy calls being translated to cell_()
 * during
 * pre-processing). The idea here is that most of the time, column() and cell()
 * (accepting column
 * names) should be used; usage of column_() and cell_() (accepting a column
 * number) is generally
 * discouraged, since moving columns is now so easy that formulas with column
 * numbers easily break
 * or, worse, silently produce wrong results. Finally, column__() (accepting a
 * table name and a
 * column number) is provided for backwards-compatibility only and highly
 * discouraged, since it
 * means moving columns in one table can mess up formulas in other tables all
 * over the project.
 * While the difference between user-visible and internal signature of cell()
 * may lead to confusion
 * for people reading the source code, this is deemed the lesser evil compared
 * with inconsistent or
 * unclear naming conventions at the user interface.
 */

/**
 * \var MuParserScript::m_parser
 * \brief muParser object doing most of the expression evaluation work
 */

/**
 * \var MuParserScript::m_variables
 * \brief Stores user-visible muParser variables.
 *
 * Variables can be defined either from the C++ side by calling setDouble() or
 * setInt(), or from the
 * muParser side by simply using the variable within an expression (causing
 * muParser to call
 * variableFactory()). In the latter case, variables will evaluate to NaN until
 * assigned a different
 * value within the user-specified expression.
 *
 * While using QHash would provide slightly faster lookups, re-hashing
 * potentially invalidates
 * pointers to values in a hash; so we'd probably have to re-export all
 * variables to muParser with
 * the new memory locations after inserting a new variable. But it's not clear a
 * priori whether it's
 * safe to call mu::Parser::DefineVar() from within variableFactory() (which
 * gets called from
 * muParser's variable handling code), and whether this may lead to muParser
 * re-generating its
 * bytecode (potentially degrading performance). Or maybe a
 * QHash<QByteArray,double*> with explicit
 * memory allocation would be the best solution. Until someone finds the time to
 * do a benchmark and
 * think these issues through, we stick with QMap.
 */

/**
 * \brief MuParserScript instance currently executing eval()
 *
 * All functions exported to muParser need to be static. However, table and
 * matrix access functions
 * need access to the current project and table/matrix (via #Context), so eval()
 * sets this variable
 * before actually evaluating code for the benefit of column(), cell() etc.
 * implementations.
 *
 * \sa tableColumnFunction(), tableColumn_Function(), tableColumn__Function(),
 * tableCellFunction()
 * \sa tableCell_Function(), matrixCellFunction()
 */
MuParserScript *MuParserScript::s_currentInstance = nullptr;

MuParserScript::MuParserScript(ScriptingEnv *environment, const QString &code,
                               QObject *context, const QString &name)
    : Script(environment, code, context, name) {
  m_parser.SetVarFactory(variableFactory, this);

  // redefine characters for operators to include ";"
  static const char opChars[] =
      // standard operator chars as defined in mu::Parser::InitCharSets()
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "+-*^/?<>=#!$%&|~'_"
      // our additions
      ";";
  m_parser.DefineOprtChars(opChars);
  // work around muparser bug number 6
  // https://code.google.com/p/muparser/issues/detail?id=6
  m_parser.DefineInfixOprtChars(opChars);

  // statement separation needs lower precedence than everything else;
  // assignment has precedence
  // -1, everything else defined in mu::Parser has non-negative precedence
  m_parser.DefineOprt(";", statementSeparator, -2);

  // aliases for _pi and _e
  m_parser.DefineConst("pi", M_PI);
  m_parser.DefineConst("Pi", M_PI);
  m_parser.DefineConst("PI", M_PI);
  m_parser.DefineConst("e", M_E);
  m_parser.DefineConst("E", M_E);

  // tell parser about mathematical functions
  for (const MuParserScripting::mathFunction *i =
           MuParserScripting::math_functions;
       i->name; i++)
    if (i->numargs == 1 && i->fun1 != nullptr)
      m_parser.DefineFun(i->name, i->fun1);
    else if (i->numargs == 2 && i->fun2 != nullptr)
      m_parser.DefineFun(i->name, i->fun2);
    else if (i->numargs == 3 && i->fun3 != nullptr)
      m_parser.DefineFun(i->name, i->fun3);

  // tell parser about table/matrix access functions
  if (Context && Context->inherits("Table")) {
    m_parser.DefineFun("column", tableColumnFunction, false);
    m_parser.DefineFun("column_", tableColumn_Function, false);
    m_parser.DefineFun("column__", tableColumn__Function, false);
    m_parser.DefineFun("cell", tableCellFunction);
    m_parser.DefineFun("cell_", tableCell_Function);
  } else if (Context && Context->inherits("Matrix"))
    m_parser.DefineFun("cell", matrixCellFunction);
}

/**
 * \brief muParser callback for registering user-defined variables
 *
 * When encountering an unknown variable within an expression, this function
 * gets called.
 * Memory for the variable is allocated in m_variables and a pointer to the
 * memory location
 * is returned to muParser. New variables are initialized with NaN.
 */
double *MuParserScript::variableFactory(const char *name, void *self) {
  MuParserScript *me = static_cast<MuParserScript *>(self);
  return me->m_variables.insert(QByteArray(name), NAN).operator->();
}

/**
 * \brief Set a (local, double-valued) variable.
 *
 * Other kinds of variables are not supported (integers are converted to
 * doubles).
 *
 * \sa m_variables
 */
bool MuParserScript::setDouble(double value, const char *name) {
  QByteArray baName(name);
  QMap<QByteArray, double>::iterator entry = m_variables.find(baName);
  if (entry == m_variables.end()) {
    // variable is not known yet
    entry = m_variables.insert(baName, value);
    try {
      m_parser.DefineVar(name, entry.operator->());
    } catch (mu::ParserError &e) {
      m_variables.erase(entry);
      emit_error(QString::fromLocal8Bit(e.GetMsg().c_str()), 0);
      return false;
    }
  } else
    // variable is known and only needs to be updated
    *entry = value;
  return true;
}

/**
 * \brief Implements a;b syntax, where a is evaluated only for side-effects and
 * b is returned.
 *
 * Technically, muParser handles only expressions and doesn't have a "statement"
 * concept. However,
 * it does support assignment to variables, which is an expression with
 * side-effects and can be
 * thought of as a statement.
 */
double MuParserScript::statementSeparator(double a, double b) {
  Q_UNUSED(a);
  return b;
}

/**
 * \brief Implements column() function for tables.
 *
 * \arg \c columnPath Path to the column to read data from. See
 * resolveColumnPath().
 *
 * The row to read from is determined by the muParser variable "i" set during
 * iteration of a column
 * formula. For explicitly specifying the row, use cell() instead.
 *
 * \sa tableCellFunction()
 */
double MuParserScript::tableColumnFunction(const char *columnPath) {
  Column *column =
      s_currentInstance->resolveColumnPath(QString::fromUtf8(columnPath));
  if (!column) return NAN;  // failsafe, shouldn't happen
  int row = qRound(s_currentInstance->m_variables["i"]) - 1;
  if (column->isInvalid(row)) throw new EmptySourceError();
  return column->valueAt(row);
}

/**
 * \brief Implements column_() function for tables.
 *
 * \arg \c columnIndex 1-based index of the column to read data from.
 *
 * It is preferable to use column() instead of this function where possible,
 * because referring to
 * columns via index silently breaks when moving columns.
 *
 * The row to read from is determined by the muParser variable "i" set during
 * iteration of a column
 * formula. For explicitly specifying the row, use cell_() instead.
 *
 * \sa tableCell_Function()
 */
double MuParserScript::tableColumn_Function(double columnIndex) {
  Table *thisTable = qobject_cast<Table *>(s_currentInstance->Context);
  if (!thisTable)
    // improving the error message would break translations
    // TODO: change col() to column() for next minor release
    throw mu::Parser::exception_type(
        qPrintable(tr("col() works only on tables!")));
  Column *column = thisTable->d_future_table->column(qRound(columnIndex) - 1);
  if (!column)
    throw mu::Parser::exception_type(
        qPrintable(tr("There's no column %1 in table %2!")
                       .arg(qRound(columnIndex))
                       .arg(thisTable->objectName())));
  int row = qRound(s_currentInstance->m_variables["i"]) - 1;
  if (column->isInvalid(row)) throw new EmptySourceError();
  return column->valueAt(row);
}

/**
 * \brief Implements column__() function for tables.
 *
 * \arg \c tableName Name of table to read data from, which can be anywhere in
 * the project.
 * \arg \c columnIndex 1-based index of column to read data from.
 *
 * This function exists so that we can implement a fully backwards-compatible
 * tablecol() function
 * in compile(). Using it in new projects is highly discouraged, because there's
 * a high risk of
 * messing up column formulas all over the project by simply moving a column.
 *
 * The row to read from is determined by the muParser variable "i" set during
 * iteration of a column
 * formula. It was never possible to explicitly specify the row when referencing
 * external tables,
 * so this is not implemented for the discouraged version. However, see
 * tableCellFunction() for how
 * to get a specific cell with the column specified by name.
 */
double MuParserScript::tableColumn__Function(const char *tableName,
                                             double columnIndex) {
  Table *thisTable = qobject_cast<Table *>(s_currentInstance->Context);
  if (!thisTable)
    // improving the error message would break translations
    // TODO: change tablecol() to column() for next minor release
    throw mu::Parser::exception_type(
        qPrintable(tr("tablecol() works only on tables!")));
  Table *targetTable =
      thisTable->folder()->rootFolder()->table(tableName, true);
  if (!targetTable)
    throw mu::Parser::exception_type(
        qPrintable(tr("Couldn't find a table named %1.").arg(tableName)));
  Column *column = targetTable->d_future_table->column(qRound(columnIndex) - 1);
  if (!column)
    throw mu::Parser::exception_type(
        qPrintable(tr("There's no column %1 in table %2!")
                       .arg(qRound(columnIndex))
                       .arg(tableName)));
  int row = qRound(s_currentInstance->m_variables["i"]) - 1;
  if (column->isInvalid(row)) throw new EmptySourceError();
  return column->valueAt(row);
}

/**
 * \brief Implements cell() function for tables.
 *
 * \arg \c columnPath Path to the column to read data from. See
 * resolveColumnPath().
 * \arg \c rowIndex 1-based index of the row to read data from.
 *
 * \sa tableColumnFunction()
 */
double MuParserScript::tableCellFunction(const char *columnPath,
                                         double rowIndex) {
  Column *column =
      s_currentInstance->resolveColumnPath(QString::fromUtf8(columnPath));
  if (!column) return NAN;  // failsafe, shouldn't happen
  int row = qRound(rowIndex) - 1;
  if (column->isInvalid(row)) throw new EmptySourceError();
  return column->valueAt(row);
}

/**
 * \brief Implements cell_() function for tables.
 *
 * \arg \c columnIndex 1-based index of column to read data from.
 * \arg \c rowIndex 1-based index of row to read data from.
 *
 * It is preferable to use cell() instead of this function where possible,
 * because referring to
 * columns via index silently breaks when moving columns.
 *
 * \sa tableColumn_Function()
 */
double MuParserScript::tableCell_Function(double columnIndex, double rowIndex) {
  Table *thisTable = qobject_cast<Table *>(s_currentInstance->Context);
  if (!thisTable)
    throw mu::Parser::exception_type(
        qPrintable(tr("cell() works only on tables and matrices!")));
  Column *column = thisTable->d_future_table->column(qRound(columnIndex) - 1);
  if (!column)
    throw mu::Parser::exception_type(
        qPrintable(tr("There's no column %1 in table %2!")
                       .arg(qRound(columnIndex))
                       .arg(thisTable->objectName())));
  int row = qRound(rowIndex) - 1;
  if (column->isInvalid(row)) throw new EmptySourceError();
  return column->valueAt(row);
}

/**
 * \brief Implements cell() function for matrices.
 *
 * \arg \c rowIndex 1-based index of row to read data from.
 * \arg \c columnIndex 1-based index of column to read data from.
 */
double MuParserScript::matrixCellFunction(double rowIndex, double columnIndex) {
  Matrix *thisMatrix = qobject_cast<Matrix *>(s_currentInstance->Context);
  if (!thisMatrix)
    throw mu::Parser::exception_type(
        qPrintable(tr("cell() works only on tables and matrices!")));
  int row = qRound(rowIndex) - 1;
  int column = qRound(columnIndex) - 1;
  if (row < 0 || row >= thisMatrix->numRows())
    throw mu::Parser::exception_type(
        qPrintable(tr("There's no row %1 in matrix %2!")
                       .arg(qRound(rowIndex))
                       .arg(thisMatrix->objectName())));
  if (column < 0 || column >= thisMatrix->numCols())
    throw mu::Parser::exception_type(
        qPrintable(tr("There's no column %1 in matrix %2!")
                       .arg(qRound(columnIndex))
                       .arg(thisMatrix->objectName())));
  return thisMatrix->cell(row, column);
}

/**
 * \brief Look up the column specified by path.
 *
 * Path can be either relative to the current context (e.g., simply the name of
 * a column in the
 * current table) or absolute (relative to the project root). For backwards
 * compatibility, also
 * "table/column" is supported, where "table" is searched for anywhere in the
 * project. This
 * shouldn't be used in new projects though, since it will be problematic once
 * we drop the
 * requirement of project-wide unique table names.
 *
 * Also, it's possible to escape /'s in the path using \\. Column and folder
 * names can already
 * contain slashes and table names will follow in a future release.
 */
Column *MuParserScript::resolveColumnPath(const QString &path) {
  Column *result = nullptr;

  // Split path into components.
  // While escape handling would be possible using a regular expression, it
  // would require
  // lookbehind assertions, which are currently not supported by QRegExp. Thus,
  // we can't simply
  // use QString::split() and have to explicitly loop over the characters in
  // path.
  QStringList pathComponents;
  QString current;
  for (int i = 0; i < path.size(); ++i) switch (path.at(i).toLatin1()) {
      case '/':
        pathComponents << current;
        current.clear();
        break;
      case '\\':
        if (i + 1 < path.size()) current.append(path.at(++i));
        break;
      default:
        current.append(path.at(i));
        break;
    }
  QString columnName = current;

  Table *table = nullptr;
  if (pathComponents.isEmpty()) {
    // only column name specified, read from this table
    table = qobject_cast<Table *>(Context);
    if (!table)
      throw mu::Parser::exception_type(qPrintable(tr(
          "Accessing table values is not (yet) supported in this context.")));
  } else {
    // look up the table containing the column
    MyWidget *myContext = qobject_cast<MyWidget *>(Context);
    if (!myContext)
      throw mu::Parser::exception_type(qPrintable(tr(
          "Accessing table values is not (yet) supported in this context.")));
    QString tableName = pathComponents.takeLast();
    if (pathComponents.isEmpty())
      // needed for backwards compatibility, but will be problematic once we
      // drop the requirement
      // of project-wide unique object names
      table = myContext->folder()->rootFolder()->table(tableName, true);
    else {
      Folder *folder;
      if (pathComponents.at(0).isEmpty())
        // absolute path
        folder = myContext->folder()->rootFolder();
      else if (pathComponents.at(0) == "..")
        // relative path
        folder = myContext->folder();
      else
        // invalid path
        throw mu::Parser::exception_type(
            qPrintable(tr("Couldn't find a table named %1.")
                           .arg(pathComponents.join("/") + "/" + tableName)));
      pathComponents.removeFirst();
      foreach (QString f, pathComponents) {
        if (f == "..")
          folder = qobject_cast<Folder *>(folder->parent());
        else
          folder = folder->findSubfolder(f);
        if (!folder)
          throw mu::Parser::exception_type(
              qPrintable(tr("Couldn't find a table named %1.")
                             .arg(pathComponents.join("/") + "/" + tableName)));
      }
      table = folder->table(tableName);
    }
    if (!table)
      throw mu::Parser::exception_type(
          qPrintable(tr("Couldn't find a table named %1.")
                         .arg(pathComponents.join("/") + "/" + tableName)));
  }

  // finally, look up the column in the table
  result = table->d_future_table->column(columnName, false);
  if (!result)
    throw mu::Parser::exception_type(
        qPrintable(tr("There's no column named %1 in table %2!")
                       .arg(columnName)
                       .arg(table->d_future_table->path())));

  return result;
}

/**
 * \brief Do in-place translation of overloaded functions.
 *
 * Recursively replaces:
 * - col("name") with column("name")
 * - col(arg) with column("arg") if the current table contains a column named
 * "arg" and
 *   with column_(arg) otherwise
 * - col("name", row) with cell("name", row)
 * - col(arg, row) with cell("arg", row) if the current table contains a column
 * named "arg" and
 *   with cell_(arg, row) otherwise
 * - tablecol("tableName", "columnName") with column("tableName/columnName")
 * - tablecol("tableName", columnIndex) with column__("tableName", columnIndex)
 * - cell(columnIndex, rowIndex) with cell_(columnIndex, rowIndex)
 *
 * Also escapes occurences of / in table/column names, so that legacy formulas
 * work with the path
 * argument of the new column() and cell() functions (see resolveColumnPath()).
 */
bool MuParserScript::translateLegacyFunctions(QString &input) {
  QRegExp legacyFunction("(\\W||^)(col|tablecol|cell)\\s*\\(");

  int functionStart = legacyFunction.indexIn(input, 0);
  while (functionStart != -1) {
    QStringList arguments;
    int functionEnd = functionStart;  // initialization is a failsafe
    QString replacement;

    // parse arguments of function
    QString currentArgument;
    for (int i = functionStart + legacyFunction.matchedLength(),
             parenthesisLevel = 1;
         parenthesisLevel > 0 && i < input.size(); i++) {
      switch (input.at(i).toLatin1()) {
        case '"':
          currentArgument += '"';
          for (i++; i < input.size() && input.at(i) != QChar('"'); i++)
            if (input.at(i) == QChar('\\')) {
              currentArgument += '\\';
              currentArgument += input.at(++i);
            } else
              currentArgument += input.at(i);
          currentArgument += '"';
          break;
        case '\\':
          currentArgument += '\\';
          currentArgument += input.at(++i);
          break;
        case '(':
          parenthesisLevel++;
          currentArgument += '(';
          break;
        case ')':
          parenthesisLevel--;
          if (parenthesisLevel > 0)
            currentArgument += ')';
          else
            functionEnd = i;
          break;
        case ',':
          if (parenthesisLevel == 1) {
            arguments << currentArgument;
            currentArgument.clear();
          } else
            currentArgument += ',';
          break;
        default:
          currentArgument += input.at(i);
          break;
      }
    }
    arguments << currentArgument;

    // select replacement function call
    Table *table = qobject_cast<Table *>(Context);
    if (legacyFunction.cap(2) == "col") {
      QString columnArgument;
      bool numericColumn = false;
      if (arguments.at(0).startsWith("\"")) {
        // col("name") -> column("name")
        columnArgument = arguments.at(0);
        // do escaping of path argument
        columnArgument.replace("\\", "\\\\");
        columnArgument.replace("/", "\\/");
      } else if (table &&
                 table->d_future_table->column(arguments.at(0), false)) {
        // hack for ambiguous legacy syntax:
        // col(name) -> column("name"), if name is a column of the current table
        columnArgument = "\"" + arguments.at(0) + "\"";
        // do escaping of path argument
        columnArgument.replace("\\", "\\\\");
        columnArgument.replace("/", "\\/");
      } else {
        // col(expression) -> column_(expression)
        columnArgument = arguments.at(0);
        if (!translateLegacyFunctions(columnArgument)) return false;
        numericColumn = true;
      }
      if (arguments.size() > 1) {
        QString rowArgument = arguments.at(1);
        if (!translateLegacyFunctions(rowArgument)) return false;
        replacement = QString("cell") + (numericColumn ? "_" : "") + "(" +
                      columnArgument + "," + rowArgument + ")";
      } else
        replacement = QString("column") + (numericColumn ? "_" : "") + "(" +
                      columnArgument + ")";
    } else if (legacyFunction.cap(2) == "tablecol") {
      // assert number of arguments == 2
      if (arguments.size() != 2) {
        emit_error(tr("tablecol: wrong number of arguments (need 2, got %1)")
                       .arg(arguments.size()),
                   0);
        compiled = Script::compileErr;
        return false;
      }
      if (arguments.at(1).startsWith("\"")) {
        // tablecol("table", "column") -> column("table/column")
        // do escaping of path argument
        QString tableArgument = arguments.at(0);
        tableArgument.replace("\\", "\\\\");
        tableArgument.replace("/", "\\/");
        QString columnArgument = arguments.at(1);
        columnArgument.replace("\\", "\\\\");
        columnArgument.replace("/", "\\/");
        // remove quotation marks
        tableArgument.remove(tableArgument.size() - 1, 1);
        columnArgument.remove(0, 1);
        replacement =
            QString("column(") + tableArgument + "/" + columnArgument + ")";
      } else {
        // tablecol("table", column) -> column__("table", column)
        QString rowArgument = arguments.at(1);
        if (!translateLegacyFunctions(rowArgument)) return false;
        replacement =
            QString("column__(") + arguments.at(0) + "," + rowArgument + ")";
      }
    } else {  // legacyFunction.cap(2) == "cell"
      // assert number of arguments == 2
      if (arguments.size() != 2) {
        emit_error(tr("cell: wrong number of arguments (need 2, got %1)")
                       .arg(arguments.size()),
                   0);
        compiled = Script::compileErr;
        return false;
      }
      if (arguments.at(0).startsWith("\"")) {
        // keep cell("column",row) -- this is new-style syntax
        QString rowArgument = arguments.at(1);
        if (!translateLegacyFunctions(rowArgument)) return false;
        replacement =
            QString("cell(") + arguments.at(0) + "," + rowArgument + ")";
      } else {
        // cell(column,row) -> cell_(column,row)
        QString columnArgument = arguments.at(0);
        if (!translateLegacyFunctions(columnArgument)) return false;
        QString rowArgument = arguments.at(1);
        if (!translateLegacyFunctions(rowArgument)) return false;
        replacement =
            QString("cell_(") + columnArgument + "," + rowArgument + ")";
      }
    }

    // do replacement
    if (legacyFunction.cap(1).isEmpty())
      // matched with ^, not \W (lookbehind assertion would be darn handy...)
      input.replace(functionStart, functionEnd - functionStart + 1,
                    replacement);
    else
      // need to adjust for the additional matched character
      input.replace(functionStart + 1, functionEnd - functionStart,
                    replacement);
    // search for next match, starting at the end of the replaced text
    functionStart =
        legacyFunction.indexIn(input, functionStart + replacement.length());
  }  // while (functionStart != -1)
  return true;
}

/**
 * \brief Pre-process #Code and hand it to #m_parser.
 *
 * This implements some functionality not directly supported by muParser, like
 * overloaded functions
 * and multi-line expressions. See class documentation of MuParserScript for
 * details.
 */
bool MuParserScript::compile(bool asFunction) {
  Q_UNUSED(asFunction);  // only needed for Python

  QString intermediate = Code.trimmed();  // pre-processed version of #Code

  // remove comments
  bool inString = false;
  int commentStart = -1;
  for (int i = 0; i < intermediate.size(); i++)
    switch (intermediate.at(i).toLatin1()) {
      case '"':
        if (commentStart < 0) inString = !inString;
        break;
      case '#':
        if (!inString) commentStart = i;
        break;
      case '\n':
        if (commentStart >= 0) {
          intermediate.remove(commentStart, i - commentStart);
          i = commentStart;
          commentStart = -1;
        }
        break;
    }
  if (commentStart >= 0)
    intermediate.remove(commentStart, intermediate.size() - commentStart);

  // simplify statement separators
  intermediate.replace(QRegExp("([;\\n]\\s*)+"), ", ");

  // recursively translate legacy functions col(), tablecol() and cell()
  if (Context && Context->inherits("Table"))
    if (!translateLegacyFunctions(intermediate)) return false;

  try {
    m_parser.SetExpr(qPrintable(intermediate));
  } catch (mu::ParserError &e) {
    emit_error(QString::fromLocal8Bit(e.GetMsg().c_str()), 0);
    return false;
  }

  compiled = isCompiled;
  return true;
}

QVariant MuParserScript::eval() {
  if (compiled != Script::isCompiled && !compile()) return QVariant();
  try {
    // see documentation of s_currentInstance for explanation
    s_currentInstance = this;
    return m_parser.Eval();
  } catch (EmptySourceError *e) {
    // formula tried to access a table cell marked as invalid
    return "";
  } catch (mu::ParserError &e) {
    emit_error(QString::fromLocal8Bit(e.GetMsg().c_str()), 0);
    return QVariant();
  }
}
