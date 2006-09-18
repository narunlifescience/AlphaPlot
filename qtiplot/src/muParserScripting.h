/***************************************************************************
    File                 : muParserScripting.h
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
#ifndef MUPARSER_SCRIPTING_H
#define MUPARSER_SCRIPTING_H

#include "Scripting.h"

#include "../3rdparty/muParser/muParser.h"
#include "math.h"
#include <gsl/gsl_sf.h>
#include <q3asciidict.h>

template <class T1, class T2> class QMap;
typedef QMap<QString,QString> strDict;

//! TODO
class muParserScript: public Script
{
  Q_OBJECT
    
  public:
    muParserScript(ScriptingEnv *env, const QString &code, QObject *context=0, const QString &name="<input>");
	
  public slots:
    bool compile();
    QVariant eval();
    bool exec();
    bool setQObject(QObject *val, const char *name);
    bool setInt(int val, const char* name);
    bool setDouble(double val, const char* name);

  private:
    int setDynamicVars();

    mu::Parser parser, rparser;
    Q3AsciiDict<double> variables;
    strDict substitute, rowIndexes, colIndexes, userVariables;
    bool returns;
};

class muParserScripting: public ScriptingEnv
{
  Q_OBJECT

  public:
    muParserScripting(ApplicationWindow *parent) : ScriptingEnv(parent) { initialized=true; }

    bool isRunning() const { return true; }
    Script *newScript(const QString &code, QObject *context, const QString &name="<input>")
    {
      return new muParserScript(this, code, context, name);
    }

    // we do not support global variables
    bool setQObject(QObject*, const char*) { return false; }
    bool setInt(int, const char*) { return false; }
    bool setDouble(double, const char*) { return false; }
    
    const QStringList mathFunctions() const;
    const QString mathFunctionDoc (const QString &name) const;

    struct mathFunction
    {
      char *name;
      int numargs;
      double (*fun1)(double);
      double (*fun2)(double,double);
      double (*fun3)(double,double,double);
      char *description;
    };
    static const mathFunction math_functions[];

  private:
    static double ceil(double x)
      { return ceil(x); }
    static double floor(double x)
      { return floor(x); }
    static double mod(double x, double y)
      { return fmod(x,y); }
    static double mypow(double x, double y)
      { return pow(x,y); }
    static double bessel_J0(double x)
      { return gsl_sf_bessel_J0 (x); }
    static double bessel_J1(double x)
      { return gsl_sf_bessel_J1 (x); }
    static double bessel_Jn(double x, double n)
      { return gsl_sf_bessel_Jn ((int)n, x); }
    static double bessel_Yn(double x, double n)
      { return gsl_sf_bessel_Yn ((int)n, x); }
    static double bessel_Jn_zero(double n, double s)
      { return gsl_sf_bessel_zero_Jnu(n, (unsigned int) s); }
    static double bessel_Y0(double x)
      { return gsl_sf_bessel_Y0 (x); }
    static double bessel_Y1(double x)
      { return gsl_sf_bessel_Y1 (x); }
    static double beta(double a, double b)
      { return gsl_sf_beta (a,b); }
    static double erf(double x)
      { return gsl_sf_erf (x); }
    static double erfc(double x)
      { return gsl_sf_erfc (x); }
    static double erf_Z(double x)
      { return gsl_sf_erf_Z (x); }
    static double erf_Q(double x)
      { return gsl_sf_erf_Q (x); }
    static double gamma(double x)
      { return gsl_sf_gamma (x); }
    static double lngamma(double x)
      { return gsl_sf_lngamma (x); }
    static double hazard(double x)
      { return gsl_sf_hazard (x); }
};

#endif
