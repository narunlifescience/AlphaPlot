/***************************************************************************
    File                 : muParserScripting.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------

    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : TODO
                           
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
#include <qmap.h>

using namespace mu;

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
  {0,0,NULL,NULL,NULL,0}
};
  
muParserScript::muParserScript(ScriptingEnv *env, const QString &code, QObject *context, const QString &name)
  : Script(env, code, context, name)
{
  variables.setAutoDelete(true);

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

  rparser = parser;
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
      QString errString = QString::fromStdString(e.GetMsg());
      for (strDict::iterator i=substitute.begin(); i!=substitute.end(); i++)
	errString.replace(i.key(), i.data(), true);
      emit_error(errString, 0);
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

bool muParserScript::compile()
{
  QString aux(Code);
  int i;

  if (Context->isA("Table"))
  {
    // col() hack
    int pos1, pos2, pos3;
    QStringList colNames=((Table*)Context)->colNames();
    int n = aux.count("col(");
    for (i=0; i<n; i++)
    {
      pos1=aux.find("col(",0,TRUE);
      pos2=pos1+3;
      pos3=aux.find(")",pos2+1);

      QString aux2=aux.mid(pos2+1,pos3-pos2-1);
      if (aux2.contains("col("))
      {
	emit_error(tr("You cannot use imbricated columns!"), 0);
	compiled = Script::compileErr;
	return false;
      }

      QStringList items=QStringList::split(",", aux2, FALSE);
      // allow both name and "name" for compatibility
      if (items[0].startsWith("\"") && items[0].endsWith("\""))
	items[0] = items[0].mid(1,items[0].length()-2);
      QString s = "__c" + QString::number(i);
      if (items.count() == 2)
	rowIndexes[s] = items[1];
      else
	rowIndexes[s] = "i";
      colIndexes[s] = QString::number(colNames.findIndex (items[0]));
      aux.replace(pos1,4+pos3-pos2,s);
      substitute[s] = "col("+aux2+")";
    }
  } else if (Context->isA("Matrix")) {
    // cell() hack
    int pos1, pos2, pos3;
    int n = aux.count("cell(");
    for (i=0; i<n; i++)
    {
      pos1=aux.find("cell(",0,TRUE);
      pos2=pos1+4;
      pos3=aux.find(")",pos2+1);

      QString aux2=aux.mid(pos2+1,pos3-pos2-1);
      if (aux2.count("cell("))
      {
	emit_error(tr("You cannot use cells recursively!"), 0);
	compiled = Script::compileErr;
	return false;
      }

      QStringList items=QStringList::split(",", aux2, FALSE);
      QString s = "__c" + QString::number(i);
      rowIndexes[s] = items[0].stripWhiteSpace();
      colIndexes[s] = items[1].stripWhiteSpace();
      aux.replace(pos1,5+pos3-pos2,s);
      substitute[s] = "cell("+aux2+")";
    }
  }

  userVariables.clear();
  returns = false;
  QStringList lines = QStringList::split("\n", aux);
  for (i=0; i<(int)lines.size(); i++)
  {
    if (lines[i].startsWith("#"))
      continue;
    QStringList kv = QStringList::split("=", lines[i], true);
    if (kv.size() == 1) {
      returns = true;
      break;
    }
    if (kv.size() != 2)
    {
      emit_error(tr("Too many '=' in one line."),i+1);
      return false;
    }
    if (kv[0].isEmpty())
    {
      emit_error(tr("Syntax error: '=' without variable name."),i+1);
      return false;
    }
    userVariables[kv[0]]=kv[1];
    setDouble(0,kv[0]);
  }
  if (returns)
    parser.SetExpr(lines[i].ascii());
  
  compiled = Script::isCompiled;
  return true;
}

int muParserScript::setDynamicVars()
{
  bool allSourcesEmpty = !rowIndexes.isEmpty();
  if (Context->isA("Table"))
  {
    Table *table = (Table*) Context;
    strDict::iterator i=rowIndexes.begin(),j=colIndexes.begin();
    while((i!=rowIndexes.end())&&(j!=colIndexes.end())) {
      int col, row;
      if (i.data() == "i" && variables["i"])
	row = (int) *(variables["i"]) - 1;
      else {
	rparser.SetExpr(i.data().toStdString());
	row = qRound(rparser.Eval()) - 1;
      }
      col=j.data().toInt();
      if (row < 0 || row >= table->tableRows() || col < 0 || col >= table->tableCols())
	return 0;
      if (table->text(row,col).isEmpty())
	setDouble(0, i.key().ascii());
      else
      {
	setDouble((table->text(row,col)).toDouble(), i.key().ascii());
	allSourcesEmpty = false;
      }
      i++; j++;
    }
  } else if (Context->isA("Matrix")) {
    Matrix *matrix = (Matrix*) Context;
    strDict::iterator i=rowIndexes.begin(),j=colIndexes.begin();
    while((i!=rowIndexes.end())&&(j!=colIndexes.end())) {
      int col, row;
      if (i.data() == "i" && variables["i"])
	row = (int) *(variables["i"]) - 1;
      else {
	rparser.SetExpr(i.data().toStdString());
	row = qRound(rparser.Eval()) - 1;
      }
      if (j.data() == "j" && variables["j"])
	col = (int) *(variables["j"]) - 1;
      else {
	rparser.SetExpr(j.data().toStdString());
	col = qRound(rparser.Eval()) - 1;
      }
      if (row < 0 || row >= matrix->numRows() || col < 0 || col >= matrix->numCols())
	return 0;
      if (matrix->text(row,col).isEmpty())
	setDouble(0, i.key().ascii());
      else
      {
	setDouble((matrix->text(row,col)).toDouble(), i.key().ascii());
	allSourcesEmpty=false;
      }
      i++; j++;
    }
  }
  for (strDict::iterator i=userVariables.begin(); i!=userVariables.end(); i++)
  {
    rparser.SetExpr(i.data().toStdString());
    if (!setDouble(rparser.Eval(), i.key().ascii()))
      return 0;
  }
  if (allSourcesEmpty) return 2;
  return 1;
}

QVariant muParserScript::eval()
{
  if ((compiled != Script::isCompiled && !compile()) || !returns)
    return QVariant();
  double val;
  try {
    switch (setDynamicVars())
    {
      case 0: return QVariant();
      case 1: break;
      case 2: return QVariant("");
    }
    val = parser.Eval();
  } catch (mu::ParserError &e) {
    QString errString = QString::fromStdString(e.GetMsg());
    for (strDict::iterator i=substitute.begin(); i!=substitute.end(); i++)
      errString.replace(i.key(), i.data(), true);
    emit_error(errString, 0);
    return QVariant();
  }
  return QVariant(val);
}

bool muParserScript::exec()
{
  if (compiled != Script::isCompiled && !compile())
    return false;
  try {
    // for error reporting
    if (returns)
      parser.Eval();
    return (setDynamicVars() != 0);
  } catch (mu::ParserError &e) {
    QString errString = QString::fromStdString(e.GetMsg());
    for (strDict::iterator i=substitute.begin(); i!=substitute.end(); i++)
      errString.replace(i.key(), i.data(), true);
    emit_error(errString, 0);
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
