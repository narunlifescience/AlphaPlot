/***************************************************************************
    File                 : muParserScripting.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------

    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net,
                           knut.franke@gmx.de
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
#include "muParserScripting.h"
#include "worksheet.h"
#include "matrix.h"

#include <qstringlist.h>
#include <q3asciidict.h>

using namespace mu;

const char* muParserScripting::langName = "muParser";

const muParserScripting::mathFunction muParserScripting::math_functions[] = {
  { "abs", 1, NULL,NULL,NULL, "abs(x):\n Absolute value of x." },
  { "acos", 1, NULL,NULL,NULL, "acos(x):\n Inverse cos function." },
  { "acosh", 1, NULL,NULL,NULL, "acosh(x):\n Hyperbolic inverse cos function." },
  { "asin", 1, NULL,NULL,NULL, "asin(x):\n Inverse sin function." },
  { "asinh", 1, NULL,NULL,NULL, "asinh(x):\n Hyperbolic inverse sin function." },
  { "atan", 1, NULL,NULL,NULL, "atan(x):\n Inverse tan function." },
  { "atanh", 1, NULL,NULL,NULL, "atanh(x):\n  Hyperbolic inverse tan function." },
  { "avg", -1, NULL,NULL,NULL, "avg(x,y,...):\n  Mean value of all arguments." },
  { "bessel_j0", 1, bessel_J0,NULL,NULL, "bessel_j0(x):\n  Regular cylindrical Bessel function of zeroth order, J_0(x)." },
  { "bessel_j1", 1, bessel_J1,NULL,NULL, "bessel_j1(x):\n  Regular cylindrical Bessel function of first order, J_1(x)." },
  { "bessel_jn", 2, NULL,bessel_Jn,NULL, "bessel_jn(double x, int n):\n Regular cylindrical Bessel function of order n, J_n(x)." },
  { "bessel_jn_zero", 2, NULL,bessel_Jn_zero,NULL, "bessel_jn_zero(double n, unsigned int s):\n s-th positive zero x_s of regular cylindrical Bessel function of order n, J_n(x_s)=0" },
  { "bessel_y0", 1, bessel_Y0, NULL,NULL, "bessel_y0(x):\n Irregular cylindrical Bessel function of zeroth order, Y_0(x), for x>0." },
  { "bessel_y1", 1, bessel_Y1, NULL,NULL, "bessel_y1(x):\n Irregular cylindrical Bessel function of first order, Y_1(x), for x>0." },
  { "bessel_yn", 2, NULL,bessel_Yn,NULL, "bessel_yn(double x, int n):\n Irregular cylindrical Bessel function of order n, Y_n(x), for x>0." },
  { "beta", 2, NULL,beta,NULL, "beta(a,b):\n Computes the Beta Function, B(a,b) = Gamma(a)*Gamma(b)/Gamma(a+b) for a > 0, b > 0." },
  { "ceil", 1, ceil,NULL,NULL, "ceil(x):\n Round to the next larger integer,\n smallest integer larger or equal to x." },
  { "cos", 1, NULL,NULL,NULL, "cos(x):\n Calculate cosine." },
  { "cosh", 1, NULL,NULL,NULL, "cosh(x):\n Hyperbolic cos function." },
  { "erf", 1, erf, NULL,NULL, "erf(x):\n  The error function." },
  { "erfc", 1, erfc, NULL,NULL, "erfc(x):\n Complementary error function erfc(x) = 1 - erf(x)." },
  { "erfz", 1, erf_Z, NULL,NULL, "erfz(x):\n The Gaussian probability density function Z(x)." },
  { "erfq", 1, erf_Q, NULL,NULL, "erfq(x):\n The upper tail of the Gaussian probability function Q(x)." },
  { "exp", 1, NULL,NULL,NULL, "exp(x):\n Exponential function: e raised to the power of x." },
  { "floor", 1, floor,NULL,NULL, "floor(x):\n Round to the next smaller integer,\n largest integer smaller or equal to x." },
  { "gamma", 1, gamma, NULL,NULL, "gamma(x):\n Computes the Gamma function, subject to x not being a negative integer." },
  { "gammaln", 1, lngamma,NULL,NULL, "gammaln(x):\n Computes the logarithm of the Gamma function, subject to x not a being negative integer. For x<0, log(|Gamma(x)|) is returned." },
  { "hazard", 1, hazard,NULL,NULL, "hazard(x):\n Computes the hazard function for the normal distribution h(x) = erfz(x)/erfq(x)." },
  { "if", 3, NULL,NULL,NULL, "if(e1, e2, e3):\n if e1 then e2 else e3." },
  { "ln", 1, NULL,NULL,NULL, "ln(x):\n Calculate natural logarithm log_e." },
  { "log", 1, NULL,NULL,NULL, "log(x):\n Calculate decimal logarithm log_10." },
  { "log10", 1, NULL,NULL,NULL, "log10(x):\n Calculate decimal logarithm log_10." },
  { "log2", 1, NULL,NULL,NULL, "log2(x):\n Calculate binary logarithm log_2." },
  { "min", -1, NULL,NULL,NULL, "min(x,y,...):\n Calculate minimum of all arguments." },
  { "max", -1, NULL,NULL,NULL, "max(x,y,...):\n Calculate maximum of all arguments." },
  { "mod", 2, NULL,mod,NULL, "mod(x,y):\n Calculate rest of integer division x/y,\n x modulo y." },
  { "pow", 2, NULL,mypow,NULL, "pow(x,y):\n Raise x to the power of y, x^y." },
  { "rint", 1, NULL,NULL,NULL, "rint(x):\n Round to nearest integer." },
  { "sign", 1, NULL,NULL,NULL, "sign(x):\n Sign function: -1 if x<0; 1 if x>0." },
  { "sin", 1, NULL,NULL,NULL, "sin(x):\n Calculate sine." },
  { "sinh", 1, NULL,NULL,NULL, "sinh(x):\n Hyperbolic sin function." },
  { "sqrt", 1, NULL,NULL,NULL, "sqrt(x):\n Square root function." },
  { "sum", -1, NULL,NULL,NULL, "sum(x,y,...):\n Calculate sum of all arguments." },
  { "tan", 1, NULL,NULL,NULL, "tan(x):\n Calculate tangent function." },
  { "tanh", 1, NULL,NULL,NULL, "tanh(x):\n Hyperbolic tan function." },
  { "w0", 1, lambert_W0, NULL, NULL, "w0(x):\n Compute the principal branch of Lambert's W function, W_0(x).\n W is defined as a solution to the equation W(x)*exp(W(x))=x.\n For x<0, there are two real-valued branches; this function computes the one where W>-1 for x<0 (also see wm1(x))." },
  { "wm1", 1, lambert_Wm1, NULL, NULL, "wm1(x):\n Compute the secondary branch of Lambert's W function, W_{-1}(x).\n W is defined as a solution to the equation W(x)*exp(W(x))=x.\n For x<0, there are two real-valued branches; this function computes the one where W<-1 for x<0. (also see w0(x))." },
  {0,0,NULL,NULL,NULL,0}
};
  
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

  parser.SetVarFactory(mu_addVariable);
  if (Context->isA("Table"))
	  parser.DefineFun("col", mu_col, false);
  else if (Context->isA("Matrix"))
	  parser.DefineFun("cell", mu_cell);

  rparser = parser;
}

double muParserScript::col(const QString &arg)
{
	if (!Context->isA("Table"))
		throw Parser::exception_type(tr("col() works only on tables!").ascii());
	QStringList items;
	QString item = "";
	for (int i=0; arg[i] != QChar::null; i++) 
		if (arg[i] == '"') {
			item += "\"";
			for (i++; arg[i] != QChar::null && arg[i] != '"'; i++)
				if (arg[i] == '\\') {
					item += "\\";
					item += arg[++i];
				} else
					item += arg[i];
			item += "\"";
		} else if (arg[i] == ',') {
			items << item;
			item = "";
		} else
			item += arg[i];
			items << item;
			Table *table = (Table*) Context;
			int col, row;
			Parser local_parser(rparser);
			if (items[0].startsWith("\"") && items[0].endsWith("\""))
				col = table->colNames().findIndex(items[0].mid(1,items[0].length()-2));
			else
			{
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
			if (row < 0 || row >= table->tableRows() || col < 0 || col >= table->tableCols())
				return 0;
			if (table->text(row,col).isEmpty())
				throw new EmptySourceError();
			else
				return (table->text(row,col)).toDouble();
}

double muParserScript::cell(int row, int col)
{
	if (!Context->isA("Matrix"))
		throw Parser::exception_type(tr("cell() works only on matrices!").ascii());
	Matrix *matrix = (Matrix*) Context;
	if (row < 1 || row > matrix->numRows() || col < 1 || col > matrix->numCols())
		return 0;
	if (matrix->text(row-1,col-1).isEmpty())
		throw new EmptySourceError();
	else
		return (matrix->text(row-1,col-1)).toDouble();
}

double *muParserScript::addVariable(const char *name)
{
	double *valptr = new double;
		if (!valptr)
			throw Parser::exception_type(tr("Out of memory").ascii());
				variables.insert(name, valptr);
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
	for (int i=0; in[i] != QChar::null; i++) 
		if (in[i] == 'c' && in.mid(i,4)=="col(") {
			out += "col(";
			QString arg = "";
			int paren = 1;
			for (i+=4; in[i] != QChar::null && paren > 0; i++)
				if (in[i] == '"') {
					arg += "\"";
					for (i++; in[i] != QChar::null && in[i] != '"'; i++)
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
	for (int i=0; Code[i] != QChar::null; i++) 
		if (Code[i] == 'c' && Code.mid(i,4)=="col(") {
			muCodeLine += "col(";
			QString arg = "";
			int paren = 1;
			for (i+=4; Code[i] != QChar::null && paren > 0; i++)
				if (Code[i] == '"') {
					arg += "\"";
					for (i++; Code[i] != '"' && Code[i] != QChar::null; i++)
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
			for (i++; Code[i] != '\n' && Code[i] != QChar::null; i++);
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
}

const QStringList muParserScripting::mathFunctions() const
{
  QStringList l;
  for (const mathFunction *i=math_functions; i->name; i++)
    l << i->name;
  return l;
}

const QString muParserScripting::mathFunctionDoc (const QString &name) const
{
  for (const mathFunction *i=math_functions; i->name; i++)
    if (name==i->name)
      return i->description;
  return QString::null;
}
