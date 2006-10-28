#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>

#if defined(_MSC_VER) //MSVC Compiler
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

struct fitData {
  size_t n;
  size_t p;
  double * X;
  double * Y;
  double * sigma;//weighting data
};

extern "C" MY_EXPORT char *name()
{
return "Rational1";
}

extern "C" MY_EXPORT char *function()
{
return "A*t^2*x/(1+4*PI^2*t^2*x^2)";
}

extern "C" MY_EXPORT char *parameters()
{
return "A,t";
}

extern "C" MY_EXPORT double function_eval(double x, double *params)
{
double t2 = params[1]*params[1];
return (params[0]*t2*x/(1+4*M_SQRTPI*x*x*t2));
}

extern "C" MY_EXPORT int function_f (const gsl_vector * x, void *params,
        gsl_vector * f)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  double *sigma = ((struct fitData *)params)->sigma;

  double a = gsl_vector_get (x, 0);
  double tau = gsl_vector_get (x, 1);
  double tau2 = tau*tau;

  size_t i;
  for (i = 0; i < n; i++)
    {
	  double t = X[i];
      double Yi = a*tau2*t/(1 + 4*M_SQRTPI*t*t*tau2);
      gsl_vector_set (f, i, (Yi - Y[i])/sigma[i]);
    }

  return GSL_SUCCESS;
}

extern "C" MY_EXPORT double function_d (const gsl_vector * x, void *params,
        gsl_vector * f)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  double *sigma = ((struct fitData *)params)->sigma;

  double a = gsl_vector_get (x, 0);
  double tau = gsl_vector_get (x, 1);
  double tau2 = tau*tau;

  size_t i;
  double val=0;
  for (i = 0; i < n; i++)
    {
	  double t = X[i];
      double dYi = ((a*tau2*t/(1 + 4*M_SQRTPI*t*t*tau2))-Y[i])/sigma[i];
      val += dYi*dYi;
    }

  return val;
}

extern "C" MY_EXPORT int function_df (const gsl_vector * x, void *params,
         gsl_matrix * J)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *sigma = ((struct fitData *)params)->sigma;

  double a = gsl_vector_get (x, 0);
  double tau = gsl_vector_get (x, 1);
  double tau2 = tau*tau;

  size_t i;
  for (i = 0; i < n; i++)
    {
      /* Jacobian matrix J(i,j) = dfi / dxj, 
         where fi = (Yi - Y[i])/sigma[i],				    
         Yi = a*tau^2*t/(1 + 4*M_SQRTPI*t*t*tau^2)        
         and the xj are the parameters (a, b, c) */

      double t = X[i];
      double r = tau*t/(1 + 4*M_SQRTPI*tau2*t*t)/sigma[i];
      gsl_matrix_set (J, i, 0, tau*r);
      gsl_matrix_set (J, i, 1, 2*a*r*(1-4*M_SQRTPI*tau2*t*t));
    }
  return GSL_SUCCESS;
}

extern "C" MY_EXPORT int function_fdf (const gsl_vector * x, void *params,
          gsl_vector * f, gsl_matrix * J)
{
  function_f (x, params, f);
  function_df (x, params, J);

  return GSL_SUCCESS;
}
