#ifndef FIT_H
#define FIT_H

#include <gsl/gsl_vector.h>

//! Structure for fitting data
struct FitData {
  size_t n;
  double * X;
  double * Y;
  double * sigma; // weighting data
};

//! Structure for parser data for fitting
struct FitParserData{
  size_t n;
  size_t p;
  double * X;
  double * Y;
  const char *function;
  const char *names;
};

//! Structure for multi-peak fitting data
struct FitMultiPeakData{
  size_t n;
  size_t p;
  double * X;
  double * Y;
};

int expd3_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);
int expd3_df (const gsl_vector * x, void *params, gsl_matrix * J);
int expd3_f (const gsl_vector * x, void *params, gsl_vector * f);
double expd3_d (const gsl_vector * x, void *params);
	
int expd2_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);
int expd2_df (const gsl_vector * x, void *params, gsl_matrix * J);
int expd2_f (const gsl_vector * x, void *params, gsl_vector * f);
double expd2_d (const gsl_vector * x, void *params);

int exp_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);
int exp_df (const gsl_vector * x, void *params, gsl_matrix * J);
int exp_f (const gsl_vector * x, void *params, gsl_vector * f);
double exp_d (const gsl_vector * x, void *params);

int boltzmann_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);
int boltzmann_df (const gsl_vector * x, void *params, gsl_matrix * J);
int boltzmann_f (const gsl_vector * x, void *params, gsl_vector * f);
double boltzmann_d (const gsl_vector * x, void *params);

int gauss_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);
int gauss_df (const gsl_vector * x, void *params, gsl_matrix * J);
int gauss_f (const gsl_vector * x, void *params,gsl_vector * f);
double gauss_d (const gsl_vector * x, void *params);

int lorentz_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);
int lorentz_df (const gsl_vector * x, void *params, gsl_matrix * J);
int lorentz_f (const gsl_vector * x, void *params, gsl_vector * f);
double lorentz_d (const gsl_vector * x, void *params);

int gauss_multi_peak_f (const gsl_vector * x, void *params, gsl_vector * f);
double gauss_multi_peak_d (const gsl_vector * x, void *params);
int gauss_multi_peak_df (const gsl_vector * x, void *params, gsl_matrix * J);
int gauss_multi_peak_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);

int lorentz_multi_peak_f (const gsl_vector * x, void *params, gsl_vector * f);
double lorentz_multi_peak_d (const gsl_vector * x, void *params);
int lorentz_multi_peak_df (const gsl_vector * x, void *params, gsl_matrix * J);
int lorentz_multi_peak_fdf (const gsl_vector * x, void *params, gsl_vector * f, gsl_matrix * J);

int user_f(const gsl_vector * x, void *params, gsl_vector * f);
double user_d(const gsl_vector * x, void *params);
int user_df(const gsl_vector * x, void *params,gsl_matrix * J);
int user_fdf(const gsl_vector * x, void *params,gsl_vector * f, gsl_matrix * J);

#endif

