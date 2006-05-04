#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_fit.h>

struct data {
  size_t n;
  double * X;
  double * y;
  double * sigma;
};

struct fit {
  size_t n;
  size_t p;
  //double epsilon;
  double * X;
  double * y;
  double * sigma;
  const char *function;
  const char *names;
};

int expb_f (const gsl_vector * x, void *params,
        gsl_vector * f)
{
  size_t n = ((struct data *)params)->n;
  double *X = ((struct data *)params)->X;
  double *y = ((struct data *)params)->y;
  double *sigma = ((struct data *) params)->sigma;

  double A = gsl_vector_get (x, 0);
  double lambda = gsl_vector_get (x, 1);
  double b = gsl_vector_get (x, 2);

  size_t i;

  for (i = 0; i < n; i++)
    {
      double Yi = A * exp (-lambda * X[i]) + b;
      gsl_vector_set (f, i, (Yi - y[i])/sigma[i]);
    }

  return GSL_SUCCESS;
}

int expb_df (const gsl_vector * x, void *params,
         gsl_matrix * J)
{
  size_t n = ((struct data *)params)->n;
  double *X = ((struct data *)params)->X;
  double *sigma = ((struct data *) params)->sigma;

  double A = gsl_vector_get (x, 0);
  double lambda = gsl_vector_get (x, 1);

  size_t i;

  for (i = 0; i < n; i++)
    {
      /* Jacobian matrix J(i,j) = dfi / dxj, */
      /* where fi = (Yi - yi)/sigma[i],      */
      /*       Yi = A * exp(-lambda * i) + b  */
      /* and the xj are the parameters (A,lambda,b) */
      double t = X[i];
      double s = sigma[i];
      double e = exp(-lambda * t);
      gsl_matrix_set (J, i, 0, e/s);
      gsl_matrix_set (J, i, 1, -t * A * e/s);
      gsl_matrix_set (J, i, 2, 1/s);

    }
  return GSL_SUCCESS;
}

int expb_fdf (const gsl_vector * x, void *params,
          gsl_vector * f, gsl_matrix * J)
{
  expb_f (x, params, f);
  expb_df (x, params, J);

  return GSL_SUCCESS;
}