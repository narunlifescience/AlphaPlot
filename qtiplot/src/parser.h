#ifndef MYPARSER_H
#define MYPARSER_H

#include "../3rdparty/muParser/muParser.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

#include <qstringlist.h>

using namespace mu;

class myParser : public Parser
{
public:
	myParser();

	static QStringList functionsList();
	static QString explainFunction(int index);

	static double bessel_J0(double x)
		{
		return gsl_sf_bessel_J0 (x);
		}

	static double bessel_J1(double x)
		{
		return gsl_sf_bessel_J1 (x);
		}

	static double bessel_Jn(double x, double n)
		{
		return gsl_sf_bessel_Jn ((int)n, x);
		}

	static double bessel_Y0(double x)
		{
		return gsl_sf_bessel_Y0 (x);
		}

	static double bessel_Y1(double x)
		{
		return gsl_sf_bessel_Y1 (x);
		}
	static double bessel_Yn(double x, double n)
		{
		return gsl_sf_bessel_Yn ((int)n, x);
		}
	static double beta(double a, double b)
		{
		return gsl_sf_beta (a, b);
		}
	static double erf(double x)
		{
		return gsl_sf_erf (x);
		}
	static double erfc(double x)
		{
		return gsl_sf_erfc (x);
		}
	static double erfz(double x)
		{
		return gsl_sf_erf_Z (x);
		}
	static double erfq(double x)
		{
		return gsl_sf_erf_Q (x);
		}
	static double gamma(double x)
		{
		return gsl_sf_gamma (x);
		}
	static double gammaln(double x)
		{
		return gsl_sf_lngamma (x);
		}
	static double hazard(double x)
		{
		return gsl_sf_hazard (x);
		}
};

#endif
