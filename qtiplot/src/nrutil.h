#ifndef NRUTIL_H
#define NRUTIL_H

#define NR_END 1
#define FREE_ARG char*
#define TINY 1.0e-20

void nrerror(char error_text[]);
double **matrix(long nrl, long nrh, long ncl, long nch);
void free_matrix(double **m, long nrl, long nrh, long ncl, long nch);
double *vector(long nl, long nh);
void free_vector(double *v, long nl, long nh);
size_t *ivector(long nl, long nh);
void free_ivector(size_t *v, long nl, long nh);
int *intvector(long nl, long nh);
void free_intvector(int*v, long nl, long nh);

//used for smoothing
void lubksb(double **a, int n, int *indx, double b[]);
void ludcmp(double **a, int n, int *indx, double *d);
void savgol(double *c, int np, int nl, int nr, int ld, int m);

void polint(double xa[], double ya[], int n, double x, double *y, double *dy);

#endif // NRUTIL_H

