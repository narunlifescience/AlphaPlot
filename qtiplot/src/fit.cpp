#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <qmessagebox.h>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>
#include "fit.h"
#include "parser.h"
#include "nrutil.h"

int expd3_f (const gsl_vector * x, void *params,
        gsl_vector * f)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  
  double A1=gsl_vector_get(x,0);
  double t1=gsl_vector_get(x,1);
  double A2=gsl_vector_get(x,2);
  double t2=gsl_vector_get(x,3);
  double A3=gsl_vector_get(x,4);
  double t3=gsl_vector_get(x,5);
  double y0=gsl_vector_get(x,6);

  size_t i;
  for (i = 0; i < n; i++)
    {
      double Yi = A1 * exp (-X[i]*t1) + A2 * exp (-X[i]*t2) + A3 * exp (-X[i]*t3) +y0;
      gsl_vector_set (f, i, Yi - Y[i]);
    }

  return GSL_SUCCESS;
}

double expd3_d (const gsl_vector * x, void *params)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  
  double A1=gsl_vector_get(x,0);
  double t1=gsl_vector_get(x,1);
  double A2=gsl_vector_get(x,2);
  double t2=gsl_vector_get(x,3);
  double A3=gsl_vector_get(x,4);
  double t3=gsl_vector_get(x,5);
  double y0=gsl_vector_get(x,6);

  size_t i;
  double val=0;
  for (i = 0; i < n; i++)
    {
      double dYi = (A1 * exp (-X[i]*t1) + A2 * exp (-X[i]*t2) + A3 * exp (-X[i]*t3) +y0)-Y[i];
      val+=dYi * dYi;
    }

  return val;
}

int expd3_df (const gsl_vector * x, void *params,
         gsl_matrix * J)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;

  double A1=gsl_vector_get(x,0);
  double l1=gsl_vector_get(x,1);
  double A2=gsl_vector_get(x,2);
  double l2=gsl_vector_get(x,3);
  double A3=gsl_vector_get(x,4);
  double l3=gsl_vector_get(x,5);

  size_t i;
  for (i = 0; i < n; i++)
    {
      /* Jacobian matrix J(i,j) = dfi / dxj, */
      /* where fi = (Yi - yi)/sigma[i],      */
      /*       Yi = A1 * exp(-xi*l1) + A2 * exp(-xi*l2) +y0  */
      /* and the xj are the parameters (A1,l1,A2,l2,y0) */
      double t = X[i];
      double e1 = exp(-t*l1);
	  double e2 = exp(-t*l2);
	  double e3 = exp(-t*l3);
		
      gsl_matrix_set (J, i, 0, e1);
      gsl_matrix_set (J, i, 1, -t * A1 * e1);
      gsl_matrix_set (J, i, 2, e2);
	  gsl_matrix_set (J, i, 3, -t * A2 * e2);
	  gsl_matrix_set (J, i, 4, e3);
	  gsl_matrix_set (J, i, 5, -t * A3 * e3);
      gsl_matrix_set (J, i, 6, 1);
    }
  return GSL_SUCCESS;
}

int expd3_fdf (const gsl_vector * x, void *params,
          gsl_vector * f, gsl_matrix * J)
{
  expd3_f (x, params, f);
  expd3_df (x, params, J);

  return GSL_SUCCESS;
}

int expd2_f (const gsl_vector * x, void *params,
        gsl_vector * f)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  
  double A1=gsl_vector_get(x,0);
  double t1=gsl_vector_get(x,1);
  double A2=gsl_vector_get(x,2);
  double t2=gsl_vector_get(x,3);
  double y0=gsl_vector_get(x,4);

  size_t i;
  for (i = 0; i < n; i++)
    {
      double Yi = A1 * exp (-X[i]*t1) + A2 * exp (-X[i]*t2) + y0;
      gsl_vector_set (f, i, Yi - Y[i]);
    }

  return GSL_SUCCESS;
}

double expd2_d (const gsl_vector * x, void *params)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  
  double A1=gsl_vector_get(x,0);
  double t1=gsl_vector_get(x,1);
  double A2=gsl_vector_get(x,2);
  double t2=gsl_vector_get(x,3);
  double y0=gsl_vector_get(x,4);

  size_t i;
  double val=0;
  for (i = 0; i < n; i++)
    {
      double dYi = (A1 * exp (-X[i]*t1) + A2 * exp (-X[i]*t2) + y0)-Y[i];
      val+=dYi * dYi;
    }

  return val;
}

int expd2_df (const gsl_vector * x, void *params,
         gsl_matrix * J)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;

  double A1=gsl_vector_get(x,0);
  double l1=gsl_vector_get(x,1);
  double A2=gsl_vector_get(x,2);
  double l2=gsl_vector_get(x,3);

  size_t i;

  for (i = 0; i < n; i++)
    {
      /* Jacobian matrix J(i,j) = dfi / dxj, */
      /* where fi = (Yi - yi)/sigma[i],      */
      /*       Yi = A1 * exp(-xi*l1) + A2 * exp(-xi*l2) +y0  */
      /* and the xj are the parameters (A1,l1,A2,l2,y0) */
      double t = X[i];
      double e1 = exp(-t*l1);
	  double e2 = exp(-t*l2);
		
      gsl_matrix_set (J, i, 0, e1);
      gsl_matrix_set (J, i, 1, -t * A1 * e1);
      gsl_matrix_set (J, i, 2, e2);
	  gsl_matrix_set (J, i, 3, -t * A2 * e2);
      gsl_matrix_set (J, i, 4, 1);
    }
  return GSL_SUCCESS;
}

int expd2_fdf (const gsl_vector * x, void *params,
          gsl_vector * f, gsl_matrix * J)
{
  expd2_f (x, params, f);
  expd2_df (x, params, J);
  return GSL_SUCCESS;
}

int exp_f (const gsl_vector * x, void *params,
        gsl_vector * f)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  double A = gsl_vector_get (x, 0);
  double lambda = gsl_vector_get (x, 1);
  double b = gsl_vector_get (x, 2);
  size_t i;
  for (i = 0; i < n; i++)
    {
      double Yi = A * exp (-lambda * X[i]) + b;
	  gsl_vector_set (f, i, Yi - Y[i]);
    }
  return GSL_SUCCESS;
}

double exp_d (const gsl_vector * x, void *params)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  double A = gsl_vector_get (x, 0);
  double lambda = gsl_vector_get (x, 1);
  double b = gsl_vector_get (x, 2);
  size_t i;
  double val=0;
  for (i = 0; i < n; i++)
    {
      double dYi = (A * exp (-lambda * X[i]) + b)-Y[i];
	  val+=dYi * dYi;
    }
  return val;
}

int exp_df (const gsl_vector * x, void *params,
         gsl_matrix * J)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double A = gsl_vector_get (x, 0);
  double lambda = gsl_vector_get (x, 1);
  size_t i;
  for (i = 0; i < n; i++)
    {
      /* Jacobian matrix J(i,j) = dfi / dxj,		*/
      /* where fi = Yi - yi,						*/
      /* Yi = A * exp(-lambda * x[i]) + b			*/
      /* and the xj are the parameters (A,lambda,b) */
      double t = X[i];
      double e = exp(-lambda * t);
	  gsl_matrix_set (J, i, 0, e);
      gsl_matrix_set (J, i, 1, -t * A * e);
      gsl_matrix_set (J, i, 2, 1);
    }
  return GSL_SUCCESS;
}

int exp_fdf (const gsl_vector * x, void *params,
          gsl_vector * f, gsl_matrix * J)
{
  exp_f (x, params, f);
  exp_df (x, params, J);
  return GSL_SUCCESS;
}

int gauss_f (const gsl_vector * x, void *params,
        gsl_vector * f)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;

  double Y0 = gsl_vector_get (x, 0);
  double A = gsl_vector_get (x, 1);
  double C = gsl_vector_get (x, 2);
  double w = gsl_vector_get (x, 3);

  size_t i;

  for (i = 0; i < n; i++)
    {
	  double diff=X[i]-C;
      double Yi = A*exp(-0.5*diff*diff/(w*w))+Y0;
      gsl_vector_set (f, i, Yi - Y[i]);
    }
  return GSL_SUCCESS;
}

double gauss_d (const gsl_vector * x, void *params)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;

  double Y0 = gsl_vector_get (x, 0);
  double A = gsl_vector_get (x, 1);
  double C = gsl_vector_get (x, 2);
  double w = gsl_vector_get (x, 3);

  size_t i;
  double val=0;

  for (i = 0; i < n; i++)
    {
	  double diff=X[i]-C;
      double dYi = (A*exp(-0.5*diff*diff/(w*w))+Y0)-Y[i];
      val+=dYi * dYi;
    }
  return val;
}

int gauss_df (const gsl_vector * x, void *params,
         gsl_matrix * J)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;

  double A = gsl_vector_get (x, 1);
  double C = gsl_vector_get (x, 2);
  double w = gsl_vector_get (x, 3);

  size_t i;
  for (i = 0; i < n; i++)
    {
      /* Jacobian matrix J(i,j) = dfi / dxj,	 */
      /* where fi = Yi - yi,					*/
      /* Yi = y=A*exp[-(Xi-xc)^2/(2*w*w)]+B		*/
      /* and the xj are the parameters (B,A,C,w) */
    
	  double diff=X[i]-C;
      double e =exp(-0.5*diff*diff/(w*w));
	 
      gsl_matrix_set (J, i, 0, 1);
      gsl_matrix_set (J, i, 1, e);
      gsl_matrix_set (J, i, 2, diff*A*e/(w*w));
	  gsl_matrix_set (J, i, 3, diff*diff*A*e/(w*w*w));
    }
  return GSL_SUCCESS;
}

int gauss_fdf (const gsl_vector * x, void *params,
          gsl_vector * f, gsl_matrix * J)
{
  gauss_f (x, params, f);
  gauss_df (x, params, J);

  return GSL_SUCCESS;
}

int lorentz_f (const gsl_vector * x, void *params,
        gsl_vector * f)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;

  double offset = gsl_vector_get (x, 0);
  double xc = gsl_vector_get (x, 2);
  double w = gsl_vector_get (x, 3);
  double aw = w*gsl_vector_get (x, 1);
  double w2 = w*w;

  size_t i;
  for (i = 0; i < n; i++)
    {
	  double diff=X[i]-xc;
      double Yi = aw/(4*diff*diff+w2)+offset;
      gsl_vector_set (f, i, Yi - Y[i]);
    }

  return GSL_SUCCESS;
}

double lorentz_d (const gsl_vector * x, void *params)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;

  double offset = gsl_vector_get (x, 0);
  double xc = gsl_vector_get (x, 2);
  double w = gsl_vector_get (x, 3);
  double aw = w*gsl_vector_get (x, 1);
  double w2 = w*w;

  size_t i;
  double val=0;
  for (i = 0; i < n; i++)
    {
	  double diff=X[i]-xc;
      double dYi = (aw/(4*diff*diff+w2)+offset)-Y[i];
      val+=(dYi * dYi);
    }

  return val;
}

int lorentz_df (const gsl_vector * x, void *params,
         gsl_matrix * J)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;

  double A = gsl_vector_get (x, 1);
  double C = gsl_vector_get (x, 2);
  double w = gsl_vector_get (x, 3);

  size_t i;
  for (i = 0; i < n; i++)
    {
      /* Jacobian matrix J(i,j) = dfi / dxj, */
      /* where fi = (Yi - yi)/sigma[i],      */
      /* Yi = y=2/PI*A*w/[4*(Xi-xc)^2+w^2]+B  */
      /* and the xj are the parameters (B,A,C,w) */
    
	  double diff=X[i]-C;
      double num =1.0/(4*diff*diff+w*w);
	  double den =4*diff*diff-w*w;
	 
      gsl_matrix_set (J, i, 0, 1);
      gsl_matrix_set (J, i, 1, w*num);
      gsl_matrix_set (J, i, 2, 8*diff*A*w*num*sqrt(num));
	  gsl_matrix_set (J, i, 3, den*A*num*num);
    }
  return GSL_SUCCESS;
}

int lorentz_fdf (const gsl_vector * x, void *params,
          gsl_vector * f, gsl_matrix * J)
{
  lorentz_f (x, params, f);
  lorentz_df (x, params, J);
  return GSL_SUCCESS;
}

int gauss_multi_peak_f (const gsl_vector * x, void *params, gsl_vector * f)
{
  size_t n = ((struct fitMultiPeakData *)params)->n;
  size_t p = ((struct fitMultiPeakData *)params)->p;
  double *X = ((struct fitMultiPeakData *)params)->X;
  double *Y = ((struct fitMultiPeakData *)params)->Y;

  size_t peaks = (p-1)/3;
  double *a = vector (0, peaks-1);
  double *xc = vector (0, peaks-1);
  double *w2 = vector (0, peaks-1);
  double offset = gsl_vector_get (x, p-1);

  size_t i,j;
  for (i = 0; i < peaks; i++)
	{
	a[i] = gsl_vector_get(x, 3*i);
	xc[i] = gsl_vector_get(x, 3*i+1);
	w2[i] = gsl_vector_get(x, 3*i+2)*gsl_vector_get (x, 3*i+2);
	}
  for (i = 0; i < n; i++)
    {
	double res = 0;
	for (j = 0; j < peaks; j++)
		{
		double diff=X[i]-xc[j];
		res+= a[j]*exp(-0.5*diff*diff/w2[j]);
		}
	gsl_vector_set(f, i, res + offset - Y[i]);
    }
  free_vector(a, 0, peaks-1);
  free_vector(xc, 0, peaks-1);
  free_vector(w2, 0, peaks-1);
  return GSL_SUCCESS;
}

double gauss_multi_peak_d (const gsl_vector * x, void *params)
{
  size_t n = ((struct fitMultiPeakData *)params)->n;
  size_t p = ((struct fitMultiPeakData *)params)->p;
  double *X = ((struct fitMultiPeakData *)params)->X;
  double *Y = ((struct fitMultiPeakData *)params)->Y;

  size_t peaks = (p-1)/3;
  double *a = vector (0, peaks-1);
  double *xc = vector (0, peaks-1);
  double *w2 = vector (0, peaks-1);
  double offset = gsl_vector_get (x, p-1);

  size_t i,j;
  double val=0;
  for (i = 0; i < peaks; i++)
	{
	a[i] = gsl_vector_get(x, 3*i);
	xc[i] = gsl_vector_get(x, 3*i+1);
	w2[i] = gsl_vector_get(x, 3*i+2)*gsl_vector_get (x, 3*i+2);
	}
	double t;
  for (i = 0; i < n; i++)
    {
	double res = 0;
	for (j = 0; j < peaks; j++)
		{
		double diff=X[i]-xc[j];
		res+= a[j]*exp(-0.5*diff*diff/w2[j]);
		}
		t=res+offset-Y[i];
		val+=t*t;
    }
  free_vector(a, 0, peaks-1);
  free_vector(xc, 0, peaks-1);
  free_vector(w2, 0, peaks-1);
  return val;
}

int gauss_multi_peak_df (const gsl_vector * x, void *params, gsl_matrix * J)
{
  size_t n = ((struct fitMultiPeakData *)params)->n;
  size_t p = ((struct fitMultiPeakData *)params)->p;
  double *X = ((struct fitMultiPeakData *)params)->X;

  size_t peaks = (p-1)/3;
  double *a = vector (0, peaks-1);
  double *xc = vector (0, peaks-1);
  double *w = vector (0, peaks-1);

  size_t i,j;
  for (i = 0; i<peaks; i++)
	{
	a[i] = gsl_vector_get (x, 3*i);
	xc[i] = gsl_vector_get (x, 3*i+1);
	w[i] = gsl_vector_get (x, 3*i+2);	
	}
  for (i = 0; i<n; i++)
    {    
	for (j = 0; j<peaks; j++)
		{
	    double diff=X[i]-xc[j];
		double w2 = w[j]*w[j];
        double e =exp(-0.5*diff*diff/w2);
	 
		gsl_matrix_set (J, i, 3*j, e);
		gsl_matrix_set (J, i, 3*j+1, diff*a[j]*e/w2);
		gsl_matrix_set (J, i, 3*j+2, diff*diff*a[j]*e/(w2*w[j]));
		}
	gsl_matrix_set (J, i, p-1, 1.0);
    }
  free_vector(a, 0, peaks-1);
  free_vector(xc, 0, peaks-1);
  free_vector(w, 0, peaks-1);
  return GSL_SUCCESS;
}

int gauss_multi_peak_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J)
{
  gauss_multi_peak_f (x, params, f);
  gauss_multi_peak_df (x, params, J);
  return GSL_SUCCESS;
}

int lorentz_multi_peak_f (const gsl_vector * x, void *params, gsl_vector * f)
{
  size_t n = ((struct fitMultiPeakData *)params)->n;
  size_t p = ((struct fitMultiPeakData *)params)->p;
  double *X = ((struct fitMultiPeakData *)params)->X;
  double *Y = ((struct fitMultiPeakData *)params)->Y;

  size_t peaks = (p-1)/3;
  double *a = vector (0, peaks-1);
  double *xc = vector (0, peaks-1);
  double *w = vector (0, peaks-1);
  double offset = gsl_vector_get (x, p-1);

  size_t i,j;
  for (i = 0; i < peaks; i++)
	{
	a[i] = gsl_vector_get(x, 3*i);
	xc[i] = gsl_vector_get(x, 3*i+1);
	w[i] = gsl_vector_get(x, 3*i+2);
	}
  for (i = 0; i < n; i++)
    {
	double res = 0;
	for (j = 0; j < peaks; j++)
		{
		double diff=X[i]-xc[j];
		res+= a[j]*w[j]/(4*diff*diff+w[j]*w[j]);
		}
	gsl_vector_set(f, i, res + offset - Y[i]);
    }
  free_vector(a, 0, peaks-1);
  free_vector(xc, 0, peaks-1);
  free_vector(w, 0, peaks-1);
  return GSL_SUCCESS;
}

double lorentz_multi_peak_d (const gsl_vector * x, void *params)
{
  size_t n = ((struct fitMultiPeakData *)params)->n;
  size_t p = ((struct fitMultiPeakData *)params)->p;
  double *X = ((struct fitMultiPeakData *)params)->X;
  double *Y = ((struct fitMultiPeakData *)params)->Y;

  size_t peaks = (p-1)/3;
  double *a = vector (0, peaks-1);
  double *xc = vector (0, peaks-1);
  double *w = vector (0, peaks-1);
  double offset = gsl_vector_get (x, p-1);

  size_t i,j;
  double val=0,t;
  for (i = 0; i < peaks; i++)
	{
	a[i] = gsl_vector_get(x, 3*i);
	xc[i] = gsl_vector_get(x, 3*i+1);
	w[i] = gsl_vector_get(x, 3*i+2);
	}
  for (i = 0; i < n; i++)
    {
	double res = 0;
	for (j = 0; j < peaks; j++)
		{
		double diff=X[i]-xc[j];
		res+= a[j]*w[j]/(4*diff*diff+w[j]*w[j]);
		}
		t=res+offset-Y[i];
		val+=t*t;
    }
  free_vector(a, 0, peaks-1);
  free_vector(xc, 0, peaks-1);
  free_vector(w, 0, peaks-1);
  return GSL_SUCCESS;
}

int lorentz_multi_peak_df (const gsl_vector * x, void *params, gsl_matrix * J)
{
  size_t n = ((struct fitMultiPeakData *)params)->n;
  size_t p = ((struct fitMultiPeakData *)params)->p;
  double *X = ((struct fitMultiPeakData *)params)->X;

  size_t peaks = (p-1)/3;
  double *a = vector (0, peaks-1);
  double *xc = vector (0, peaks-1);
  double *w = vector (0, peaks-1);

  size_t i,j;
  for (i = 0; i<peaks; i++)
	{
	a[i] = gsl_vector_get (x, 3*i);
	xc[i] = gsl_vector_get (x, 3*i+1);
	w[i] = gsl_vector_get (x, 3*i+2);	
	}
  for (i = 0; i<n; i++)
    {    
	for (j = 0; j<peaks; j++)
		{
	    double diff = X[i]-xc[j];
		double w2 = w[j]*w[j];
		double num = 1.0/(4*diff*diff+w2);
		double den = 4*diff*diff-w2;
	 
		gsl_matrix_set (J, i, 3*j, w[j]*num);
		gsl_matrix_set (J, i, 3*j+1, 8*diff*a[j]*w[j]*num*sqrt(num));
		gsl_matrix_set (J, i, 3*j+2, den*a[j]*num*num);
		}
	gsl_matrix_set (J, i, p-1, 1.0);
    }
  free_vector(a, 0, peaks-1);
  free_vector(xc, 0, peaks-1);
  free_vector(w, 0, peaks-1);
  return GSL_SUCCESS;
}

int lorentz_multi_peak_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J)
{
  lorentz_multi_peak_f (x, params, f);
  lorentz_multi_peak_df (x, params, J);
  return GSL_SUCCESS;
}

int user_f(const gsl_vector * x, void *params, gsl_vector * f)
{
  size_t n = ((struct fitParserData *)params)->n;
  size_t p = ((struct fitParserData *)params)->p;
  double *X = ((struct fitParserData *)params)->X;
  double *Y = ((struct fitParserData *)params)->Y;
  const char *function = ((struct fitParserData *) params)->function;
  QString names = (QString)((struct fitParserData *) params)->names;
  QStringList parNames= QStringList::split(",",names,false);

  myParser parser;
  try
	{
	double *parameters = new double[p];
	double xvar;
	parser.DefineVar("x", &xvar);
	for (int i=0;i<(int)p;i++)
		{
		parameters[i]=gsl_vector_get(x,i);
		parser.DefineVar(parNames[i].ascii(), &parameters[i]);
		}	
	parser.SetExpr(function);
	for (int j = 0; j < (int)n; j++)
		{
		xvar=X[j];
		gsl_vector_set (f, j, parser.Eval() - Y[j]);
		}
	delete[] parameters;
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0,"QtiPlot - user_f",QString::fromStdString(e.GetMsg()));
	}
return GSL_SUCCESS;
}

double user_d(const gsl_vector * x, void *params)
{
  size_t n = ((struct fitParserData *)params)->n;
  size_t p = ((struct fitParserData *)params)->p;
  double *X = ((struct fitParserData *)params)->X;
  double *Y = ((struct fitParserData *)params)->Y;
  const char *function = ((struct fitParserData *) params)->function;
  QString names = (QString)((struct fitParserData *) params)->names;
  QStringList parNames= QStringList::split(",",names,false);

  double val=0;
  myParser parser;
  try
	{
	double *parameters = new double[p];
	double xvar,t;
	parser.DefineVar("x", &xvar);
	for (int i=0;i<(int)p;i++)
		{
		parameters[i]=gsl_vector_get(x,i);
		parser.DefineVar(parNames[i].ascii(), &parameters[i]);
		}	
	parser.SetExpr(function);
	for (int j = 0; j < (int)n; j++)
		{
		xvar=X[j];
		t=(parser.Eval() - Y[j]);
		val+=t*t;
		}
	delete[] parameters;
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0,"QtiPlot - user_f",QString::fromStdString(e.GetMsg()));
	}
return val;
}
int user_df(const gsl_vector *x, void *params, gsl_matrix *J)
{
  size_t n = ((struct fitParserData *)params)->n;
  size_t p = ((struct fitParserData *)params)->p;
  double *X = ((struct fitParserData *)params)->X;
  const char *function = ((struct fitParserData *) params)->function;
  QString names = (QString)((struct fitParserData *) params)->names;
  QStringList parNames= QStringList::split(",",names,false);

  try
	{
	double *param = new double[p];
	myParser parser;
	double xvar; 
	parser.DefineVar("x", &xvar);

	for (int k=0;k<(int)p;k++)
		{
		param[k]=gsl_vector_get(x,k);
		parser.DefineVar(parNames[k].ascii(), &param[k]);
		}

	parser.SetExpr(function);

	for (int i = 0; i<(int)n; i++)
		{
		xvar = X[i];	 
		for (int j=0;j<(int)p;j++)
			gsl_matrix_set (J, i, j, parser.Diff(&param[j], param[j]));
		}
	delete[] param;
	}
catch(mu::ParserError &e)
	{
	QMessageBox::critical(0,"QtiPlot - Input function error",QString::fromStdString(e.GetMsg()));
	}
return GSL_SUCCESS;
}

int user_fdf(const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J)
{
user_f (x, params, f);
user_df (x, params, J);
return GSL_SUCCESS;
}

int boltzmann_f (const gsl_vector * x, void *params, gsl_vector * f)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  double A1 = gsl_vector_get (x, 0);
  double A2 = gsl_vector_get (x, 1);
  double x0 = gsl_vector_get (x, 2);
  double dx = gsl_vector_get (x, 3);
  size_t i;
  for (i = 0; i < n; i++)
	{
	double Yi = (A1-A2)/(1+exp((X[i]-x0)/dx))+A2;
	gsl_vector_set (f, i, Yi - Y[i]);
	}

  return GSL_SUCCESS;
}

double boltzmann_d (const gsl_vector * x, void *params)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double *Y = ((struct fitData *)params)->Y;
  double A1 = gsl_vector_get (x, 0);
  double A2 = gsl_vector_get (x, 1);
  double x0 = gsl_vector_get (x, 2);
  double dx = gsl_vector_get (x, 3);
  size_t i;
  double val=0;
  for (i = 0; i < n; i++)
    {
      double dYi = (A1-A2)/(1+exp((X[i]-x0)/dx)) + A2 - Y[i];
	  val+=dYi * dYi;
    }
  return val;
}

int boltzmann_df (const gsl_vector * x, void *params, gsl_matrix * J)
{
  size_t n = ((struct fitData *)params)->n;
  double *X = ((struct fitData *)params)->X;
  double A1 = gsl_vector_get (x, 0);
  double A2 = gsl_vector_get (x, 1);
  double x0 = gsl_vector_get (x, 2);
  double dx = gsl_vector_get (x, 3);
  size_t i;
  for (i = 0; i < n; i++)
    {
      /* Jacobian matrix J(i,j) = dfi / dxj,		*/
      /* where fi = Yi - yi,						*/
      /* Yi = (A1-A2)/(1+exp((X[i]-x0)/dx)) + A2	*/
      /* and the xj are the parameters (A1,A2,x0,dx)*/
      double diff = X[i]-x0;
      double e = exp(diff/dx);
	  double r = 1/(1+e);
	  double aux = (A1 - A2)*e*r*r/dx;
	  gsl_matrix_set (J, i, 0, r);
      gsl_matrix_set (J, i, 1, 1-r);
      gsl_matrix_set (J, i, 2, aux);
	  gsl_matrix_set (J, i, 3, aux*diff/dx);
    }
  return GSL_SUCCESS;
}

int boltzmann_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J)
{
  boltzmann_f (x, params, f);
  boltzmann_df (x, params, J);
  return GSL_SUCCESS;
}

