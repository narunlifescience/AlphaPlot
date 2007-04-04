/***************************************************************************
    File                 : muParserScript.h
	Project              : QtiPlot
    --------------------------------------------------------------------

    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Evaluate mathematical expressions using muParser
                           
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
#ifndef MUPARSER_SCRIPT_H
#define MUPARSER_SCRIPT_H

#include "ScriptingEnv.h" 
#include "Script.h"

#include <muParser.h>
#include "math.h"
#include <gsl/gsl_sf.h>
#include <q3asciidict.h>

//! TODO
class muParserScript: public Script
{
  Q_OBJECT
    
  public:
    muParserScript(ScriptingEnv *env, const QString &code, QObject *context=0, const QString &name="<input>");
	
  public slots:
    bool compile(bool asFunction=true);
    QVariant eval();
    bool exec();
    bool setQObject(QObject *val, const char *name);
    bool setInt(int val, const char* name);
    bool setDouble(double val, const char* name);

  private:
    double col(const QString &arg);
    double cell(int row, int col);
    double *addVariable(const char *name);
    double *addVariableR(const char *name);
    static double *mu_addVariableR(const char *name) { return current->addVariableR(name); }
    static double mu_col(const char *arg) { return current->col(arg); }
    static double mu_cell(double row, double col) { return current->cell(qRound(row), qRound(col)); }
    static double *mu_addVariable(const char *name, void *){ return current->addVariable(name); }
    static double *mu_addVariableR(const char *name, void *) { return current->addVariableR(name); }
    static QString compileColArg(const QString& in);

    mu::Parser parser, rparser;
    Q3AsciiDict<double> variables, rvariables;
    QStringList muCode;

  public:
    static muParserScript *current;
};

#endif
