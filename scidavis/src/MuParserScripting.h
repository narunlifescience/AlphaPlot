/***************************************************************************
    File                 : MuParserScripting.h
    Project              : SciDAVis
    --------------------------------------------------------------------

    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Benkert,
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
#ifndef MUPARSER_SCRIPTING_H
#define MUPARSER_SCRIPTING_H

#include "ScriptingEnv.h"
#include "Script.h"
#include "MuParserScript.h"

#include <muParser.h>
#include "math.h"
#include <gsl/gsl_sf.h>
#include <gsl/gsl_errno.h>

//! TODO
class MuParserScripting: public ScriptingEnv
{
  Q_OBJECT

  public:
    static const char *langName;
    MuParserScripting(ApplicationWindow *parent) : ScriptingEnv(parent, langName) {
		d_initialized=true;
		gsl_set_error_handler_off();
	}
    static ScriptingEnv *constructor(ApplicationWindow *parent) { return new MuParserScripting(parent); }

    bool isRunning() const { return true; }
    Script *newScript(const QString &code, QObject *context, const QString &name="<input>")
    {
      return new MuParserScript(this, code, context, name);
    }

    // we do not support global variables
    bool setQObject(QObject*, const char*) { return false; }
    bool setInt(int, const char*) { return false; }
    bool setDouble(double, const char*) { return false; }
    
    const QStringList mathFunctions() const;
    const QString mathFunctionDoc (const QString &name) const;

    struct mathFunction
    {
      const char *name;
      int numargs;
      double (*fun1)(double);
      double (*fun2)(double,double);
      double (*fun3)(double,double,double);
      QString description;
    };
    static const mathFunction math_functions[];

  private:
#define SPECIAL(fname, arg) gsl_sf_result result; if (gsl_sf_##fname##_e(arg, &result) == GSL_SUCCESS) return result.val; else return NAN
#define SPECIAL2(fname, arg1, arg2) gsl_sf_result result; if (gsl_sf_##fname##_e(arg1, arg2, &result) == GSL_SUCCESS) return result.val; else return NAN
    static double ceil(double x)
  { return ::ceil(x); }
    static double floor(double x)
  { return ::floor(x); }
    static double mod(double x, double y)
      { return fmod(x,y); }
    static double mypow(double x, double y)
      { return pow(x,y); }
    static double bessel_J0(double x)
      { SPECIAL(bessel_J0 ,x); }
    static double bessel_J1(double x)
      { SPECIAL(bessel_J1 ,x); }
    static double bessel_Jn(double x, double n)
      { SPECIAL2(bessel_Jn ,(int)n, x); }
    static double bessel_Yn(double x, double n)
      { SPECIAL2(bessel_Yn ,(int)n, x); }
    static double bessel_Jn_zero(double n, double s)
      { SPECIAL2(bessel_zero_Jnu,n, (unsigned int) s); }
    static double bessel_Y0(double x)
      { SPECIAL(bessel_Y0 ,x); }
    static double bessel_Y1(double x)
      { SPECIAL(bessel_Y1 ,x); }
    static double beta(double a, double b)
      { SPECIAL2(beta ,a,b); }
    static double erf(double x)
      { SPECIAL(erf ,x); }
    static double erfc(double x)
      { SPECIAL(erfc ,x); }
    static double erf_Z(double x)
      { SPECIAL(erf_Z ,x); }
    static double erf_Q(double x)
      { SPECIAL(erf_Q ,x); }
    static double gamma(double x)
      { SPECIAL(gamma ,x); }
    static double lngamma(double x)
      { SPECIAL(lngamma ,x); }
    static double hazard(double x)
      { SPECIAL(hazard ,x); }
	 static double lambert_W0(double x)
	   { SPECIAL(lambert_W0,x); }
	 static double lambert_Wm1(double x)
	   { SPECIAL(lambert_Wm1,x); }
};

class EmptySourceError : public mu::ParserError
{
	public:
		EmptySourceError() {}
};

#endif
