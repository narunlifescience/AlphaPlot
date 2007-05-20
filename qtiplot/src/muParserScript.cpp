/***************************************************************************
    File                 : muParserScript.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------

    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
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
#include "muParserScript.h"
#include "muParserScripting.h"
#include "Table.h"
#include "Matrix.h"

#include <QLocale>
#include <QStringList>

using namespace mu;

muParserScript::muParserScript(ScriptingEnv *env, const QString &code, QObject *context, const QString &name)
  : Script(env, code, context, name)
{
  variables.setAutoDelete(true);
  rvariables.setAutoDelete(true);

  parser.DefineConst("pi", M_PI);
  parser.DefineConst("Pi", M_PI);
  parser.DefineConst("PI", M_PI);
  parser.DefineConst("e", M_E);
  parser.DefineConst("E", M_E);

  for (const muParserScripting::mathFunction *i=muParserScripting::math_functions; i->name; i++)
    if (i->numargs == 1 && i->fun1 != NULL)
      parser.DefineFun(i->name, i->fun1);
    else if (i->numargs == 2 && i->fun2 != NULL)
      parser.DefineFun(i->name, i->fun2);
    else if (i->numargs == 3 && i->fun3 != NULL)
      parser.DefineFun(i->name, i->fun3);

  if (Context->isA("Table")) {
	  parser.DefineFun("col", mu_col, false);
	  parser.DefineFun("cell", mu_tableCell);
  } else if (Context->isA("Matrix"))
	  parser.DefineFun("cell", mu_cell);

  rparser = parser;
  parser.SetVarFactory(mu_addVariable);
  rparser.SetVarFactory(mu_addVariableR);
}

double muParserScript::col(const QString &arg)
{
	if (!Context->isA("Table"))
		throw Parser::exception_type(tr("col() works only on tables!").ascii());
	QStringList items;
	QString item = "";
	for (int i=0; i < arg.size(); i++) {
		if (arg[i] == '"') {
			item += "\"";
			for (i++; i < arg.size() && arg[i] != '"'; i++)
				if (arg[i] == '\\') {
					item += "\\";
					item += arg[++i];
				} else
					item += arg[i];
				item += "\"";
		} else if (arg[i] == ',' && QLocale().decimalPoint() != ',') {
			items << item;
			item = "";
		} else
			item += arg[i];
	}
	items << item;
	Table *table = (Table*) Context;
	int col, row;
	Parser local_parser(rparser);
	if (items[0].startsWith("\"") && items[0].endsWith("\"")) {
		col = table->colNames().findIndex(items[0].mid(1,items[0].length()-2));
		if (col<0)
			throw Parser::exception_type(tr("There's no column named %1 in table %2!").
					arg(items[0]).arg(Context->name()).ascii());
	} else {
		// for backwards compatibility
		col = table->colNames().findIndex(items[0]);
		if (col<0) {
			local_parser.SetExpr(items[0].ascii());
			col = qRound(local_parser.Eval()) - 1;
		}
	}
	if (items.count() == 2)
	{
		local_parser.SetExpr(items[1].ascii());
		row = qRound(local_parser.Eval()) - 1;
	} else if (variables["i"])
		row = (int) *(variables["i"]) - 1;
	else
		return 0;
	rvariables.clear();
	if (row < 0 || row >= table->numRows())
		throw Parser::exception_type(tr("There's no row %1 in table %2!").
				arg(row+1).arg(Context->name()).ascii());
	if (col < 0 || col >= table->numCols())
		throw Parser::exception_type(tr("There's no column %1 in table %2!").
				arg(col+1).arg(Context->name()).ascii());
	if (table->text(row,col).isEmpty())
		throw new EmptySourceError();
	else
		return table->cell(row,col);
}

double muParserScript::cell(int row, int col)
{
	if (!Context->isA("Matrix"))
		throw Parser::exception_type(tr("cell() works only on tables and matrices!").ascii());
	Matrix *matrix = (Matrix*) Context;
	if (row < 1 || row > matrix->numRows())
		throw Parser::exception_type(tr("There's no row %1 in matrix %2!").
				arg(row).arg(Context->name()).ascii());
	if (col < 1 || col > matrix->numCols())
		throw Parser::exception_type(tr("There's no column %1 in matrix %2!").
				arg(col).arg(Context->name()).ascii());
	if (matrix->text(row-1,col-1).isEmpty())
		throw new EmptySourceError();
	else
		return matrix->cell(row-1,col-1);
}

double muParserScript::tableCell(int col, int row)
{
	if (!Context->isA("Table"))
		throw Parser::exception_type(tr("cell() works only on tables and matrices!").ascii());
	Table *table = (Table*) Context;
	if (row < 1 || row > table->numRows())
		throw Parser::exception_type(tr("There's no row %1 in table %2!").
				arg(row).arg(Context->name()).ascii());
	if (col < 1 || col > table->numCols())
		throw Parser::exception_type(tr("There's no column %1 in table %2!").
				arg(col).arg(Context->name()).ascii());
	if (table->text(row-1,col-1).isEmpty())
		throw new EmptySourceError();
	else
		return table->cell(row-1,col-1);
}

double *muParserScript::addVariable(const char *name)
{
	double *valptr = new double;
	if (!valptr)
		throw Parser::exception_type(tr("Out of memory").ascii());
	*valptr = 0;
	variables.insert(name, valptr);
	rparser.DefineVar(name, valptr);
	return valptr;
}

double *muParserScript::addVariableR(const char *name)
{
	double *valptr = new double;
	if (!valptr)
		throw Parser::exception_type(tr("Out of memory").ascii());
	*valptr = 0;
	rvariables.insert(name, valptr);
	return valptr;
}

bool muParserScript::setDouble(double val, const char *name)
{
  double *valptr = variables[name];
  if (!valptr)
  {
    valptr = new double;
    if (!valptr)
    {
      emit_error(tr("Out of memory"),0);
      return false;
    }
    try {
      parser.DefineVar(name, valptr);
      rparser.DefineVar(name, valptr);
      variables.insert(name, valptr);
    } catch (mu::ParserError &e) {
      delete valptr;
      emit_error(QString(e.GetMsg().c_str()), 0);
      return false;
    }
  }
  *valptr = val;
  return  true;
}

bool muParserScript::setInt(int val, const char *name)
{
  return setDouble((double) val, name);
}

bool muParserScript::setQObject(QObject*, const char*)
{
  return false;
}

QString muParserScript::compileColArg(const QString &in)
{
	QString out = "\"";
	for (int i=0; i < in.size(); i++)
		if (in[i] == 'c' && in.mid(i,4)=="col(") {
			out += "col(";
			QString arg = "";
			int paren = 1;
			for (i+=4; i < in.size() && paren > 0; i++)
				if (in[i] == '"') {
					arg += "\"";
					for (i++; i < in.size() && in[i] != '"'; i++)
						if (in[i] == '\\') {
							arg += "\\";
							arg += in[++i];
						} else
							arg += in[i];
					arg += "\"";
				} else if (in[i] == '(') {
					paren++;
					arg += "(";
				} else if (in[i] == ')') {
					paren--;
					if(paren>0) arg += ")";
				}
				else
					arg += in[i];
			out += compileColArg(arg).replace('"',"\\\"");
			out += ")";
			i--;
		}
		else if (in[i] == '\\')
			out += "\\\\";
		else if (in[i] == '"')
			out += "\\\"";
		else
			out += in[i];
	return out + "\"";
}

bool muParserScript::compile(bool)
{
	muCode.clear();
	QString muCodeLine = "";
	for (int i=0; i < Code.size(); i++)
		if (Code[i] == 'c' && Code.mid(i,4)=="col(") {
			muCodeLine += "col(";
			QString arg = "";
			int paren = 1;
			for (i+=4; i < Code.size() && paren > 0; i++)
				if (Code[i] == '"') {
					arg += "\"";
					for (i++; Code[i] != '"' && i < Code.size(); i++)
						if (Code[i] == '\\') {
							arg += "\\";
							arg += Code[++i];
						} else
							arg += Code[i];
					arg += "\"";
				} else if (Code[i] == '(') {
					paren++;
					arg += "(";
				} else if (Code[i] == ')') {
					paren--;
					if(paren>0) arg += ")";
				}
				else
					arg += Code[i];
			muCodeLine += compileColArg(arg);
			muCodeLine += ")";
			i--;
		} else if (Code[i] == '#')
			for (i++; Code[i] != '\n' && i < Code.size(); i++);
		else if (Code[i] == '\n') {
			muCodeLine = muCodeLine.stripWhiteSpace();
			if (!muCodeLine.isEmpty())
				muCode += muCodeLine;
			muCodeLine = "";
		} else
			muCodeLine += Code[i];
	muCodeLine = muCodeLine.stripWhiteSpace();
	if (!muCodeLine.isEmpty())
		muCode += muCodeLine;
	compiled = Script::isCompiled;
	return true;
}

muParserScript *muParserScript::current = NULL;

QVariant muParserScript::eval()
{
	if (compiled != Script::isCompiled && !compile())
		return QVariant();
	double val;
	try {
		current = this;
		for (QStringList::iterator i=muCode.begin(); i != muCode.end(); i++) {
			parser.SetExpr(i->ascii());
			val = parser.Eval();
		}
	} catch (EmptySourceError *e) {
		return QVariant("");
	} catch (ParserError &e) {
		emit_error(e.GetMsg().c_str(), 0);
		return QVariant();
	}
	return QVariant(val);
}

bool muParserScript::exec()
{
	if (compiled != Script::isCompiled && !compile())
		return false;
	try {
		current = this;
		for (QStringList::iterator i=muCode.begin(); i != muCode.end(); i++) {
			parser.SetExpr(i->ascii());
			parser.Eval();
		}
	} catch (EmptySourceError *e) {
		return true;
	} catch (mu::ParserError &e) {
		emit_error(e.GetMsg().c_str(), 0);
		return false;
	}
	return true;
}
