#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "nrutil.h"

int IMIN(int ia, int ib) 
	{
	if (ia<=ib) 
		return ia;
	else 
		return ib;
	}

void nrerror(char error_text[])
{//Numerical Recipes standard error handler 
fprintf(stderr,"Numerical Recipes run-time error...\n");
fprintf(stderr,"%s\n",error_text);
fprintf(stderr,"...now exiting to system...\n");
exit(1);
}

double **matrix(long nrl, long nrh, long ncl, long nch)
{// allocate a double matrix with subscript range m[nrl..nrh][ncl..nch] 
long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
double **m;

//allocate pointers to rows 
m=(double **) malloc((size_t)((nrow+NR_END)*sizeof(double*)));
if (!m) nrerror("allocation failure 1 in matrix()");
m += NR_END;
m -= nrl;

// allocate rows and set pointers to them
m[nrl]=(double *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(double)));
if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
m[nrl] += NR_END;
m[nrl] -= ncl;
for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

// return pointer to array of pointers to rows 
return m;
}

void free_matrix(double **m, long nrl, long nrh, long ncl, long nch)
{//free a double matrix allocated by matrix() 
free((FREE_ARG) (m[nrl]+ncl-NR_END));
free((FREE_ARG) (m+nrl-NR_END));
}


double *vector(long nl, long nh)
{//allocate a double vector with subscript range v[nl..nh] 
double *v;
v=(double *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(double)));
if (!v) 	
	nrerror("allocation failure in double vector()");
return v-nl+NR_END;
}

void free_vector(double *v, long nl, long nh)
{// free a double vector allocated with vector() 
free((FREE_ARG) (v+nl-NR_END));
}

size_t *ivector(long nl, long nh)
{
size_t *v;
v=(size_t *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(size_t)));
if (!v) nrerror("allocation failure in ivector()");
return v-nl+NR_END;
}

void free_ivector(size_t *v, long nl, long nh)
{
free((FREE_ARG) (v+nl-NR_END));
}

int *intvector(long nl, long nh)
{
int *v;
v=(int *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(int)));
if (!v) nrerror("allocation failure in intvector()");
return v-nl+NR_END;
}

void free_intvector(int*v, long nl, long nh)
{
free((FREE_ARG) (v+nl-NR_END));
}

 void savgol(double *c, int np, int nl, int nr, int ld, int m)  {
/*------------------------------------------------------------------------------------------- 
 USES lubksb,ludcmp given below. 
 Returns in c(np), in wrap-around order (see reference) consistent with the argument respns 
 in routine convlv, a set of Savitzky-Golay filter coefficients. nl is the number of leftward 
 (past) data points used, while nr is the number of rightward (future) data points, making 
 the total number of data points used nl+nr+1. ld is the order of the derivative desired
 (e.g., ld = 0 for smoothed function). m is the order of the smoothing polynomial, also 
 equal to the highest conserved moment; usual values are m = 2 or m = 4. 
-------------------------------------------------------------------------------------------*/
int imj,ipj,j,k,kk,mm;
double d,fac,sum,**a,*b;

if (np < nl+nr+1 || nl < 0 || nr < 0 || ld > m || nl+nr < m)
	nrerror("bad args in savgol");

int *indx= intvector(1,m+1);
a=matrix(1,m+1,1,m+1);
b=vector(1,m+1);
for (ipj=0;ipj<=(m << 1);ipj++) 
	{//Set up the normal equations of the desired least-squares fit
	sum=(ipj ? 0.0 : 1.0); 
	for (k=1;k<=nr;k++) 
		sum += pow((double)k,(double)ipj);
	for (k=1;k<=nl;k++) 
		sum += pow((double)-k,(double)ipj);
	mm=IMIN(ipj,2*m-ipj);
	for (imj = -mm;imj<=mm;imj+=2)
		a[1+(ipj+imj)/2][1+(ipj-imj)/2]=sum;
	}

ludcmp(a, m+1, indx, &d); //Solve them: LU decomposition.

for (j=1;j<=m+1;j++) 
	b[j]=0.0;

b[ld+1]=1.0; //Right-hand side vector is unit vector, depending on which derivative we want.

lubksb(a,m+1,indx,b); //Get one row of the inverse matrix.

for (kk=1;kk<=np;kk++) 
	c[kk]=0.0; //Zero the output array (it may be bigger than number of coefficients).

for (k = -nl;k<=nr;k++) 
	{ 
	sum=b[1];   //Each Savitzky-Golay coefficient is the dot product 
				//of powers of an integer with the inverse matrix row.
	fac=1.0;
	for (mm=1;mm<=m;mm++) 
		sum += b[mm+1]*(fac *= k);

	kk=((np-k) % np)+1; //Store in wrap-around order.
	c[kk]=sum;
	}

free_vector(b,1,m+1);
free_matrix(a,1,m+1,1,m+1);
free_intvector(indx,1,m+1);
}

/**************************************************************
* Given an N x N matrix A, this routine replaces it by the LU *
* decomposition of a rowwise permutation of itself. A and N   *
* are input. INDX is an output vector which records the row   *
* permutation effected by the partial pivoting; D is output   *
* as -1 or 1, depending on whether the number of row inter-   *
* changes was even or odd, respectively. This routine is used *
* in combination with LUBKSB to solve linear equations or to  *
* invert a matrix. Return code is 1, if matrix is singular.   *
**************************************************************/
void ludcmp(double **a, int n, int *indx, double *d)
{
int i,imax,j,k;
double big,dum,sum,temp;
double *vv=vector(1,n);
*d=1.0; 
for (i=1;i<=n;i++) 
	{
	big=0.0;
	for (j=1;j<=n;j++)
		if ((temp=fabs(a[i][j])) > big) 
			big=temp;
	if (big == 0.0) 
			nrerror("allocation failure 1 in matrix()");	
	vv[i]=1.0/big; 
	}
for (j=1;j<=n;j++) 
	{ 
	for (i=1;i<j;i++) 
		{ 
		sum=a[i][j];
		for (k=1;k<i;k++) 
			sum -= a[i][k]*a[k][j];
		a[i][j]=sum;
		}
	big=0.0; 
	for (i=j;i<=n;i++) 
		{ 
		sum=a[i][j];
		for (k=1;k<j;k++)
			sum -= a[i][k]*a[k][j];
		a[i][j]=sum;
		if ( (dum=vv[i]*fabs(sum)) >= big) 
			{
			big=dum;
			imax=i;
			}
		}
	if (j != imax) 
		{ 
		for (k=1;k<=n;k++) 
			{ 
			dum=a[imax][k];
			a[imax][k]=a[j][k];
			a[j][k]=dum;
			}
		*d = -(*d); 
		vv[imax]=vv[j]; 
		}
	indx[j]=imax;
	if (a[j][j] == 0.0) 
		a[j][j]=TINY;

	if (j != n) 
		{ 
		dum=1.0/(a[j][j]);
		for (i=j+1;i<=n;i++) 
			a[i][j] *= dum;
		}
	} 
free_vector(vv,1,n);
}

/*****************************************************************
* Solves the set of N linear equations A . X = B.  Here A is    *
* input, not as the matrix A but rather as its LU decomposition, *
* determined by the routine LUDCMP. INDX is input as the permuta-*
* tion vector returned by LUDCMP. B is input as the right-hand   *
* side vector B, and returns with the solution vector X. A, N and*
* INDX are not modified by this routine and can be used for suc- *
* cessive calls with different right-hand sides. This routine is *
* also efficient for plain matrix inversion.                     *
*****************************************************************/
void lubksb(double **a, int n, int *indx, double b[])
{
int i,ii=0,ip,j;
double sum;

for (i=1;i<=n;i++) 
	{ 
	ip=indx[i];
	sum=b[ip];
	b[ip]=b[i];
	if (ii)
		for (j=ii;j<=i-1;j++) 
			sum -= a[i][j]*b[j];
	else if (sum) 
		ii=i; 

	b[i]=sum;
	}
for (i=n;i>=1;i--) 
	{
	sum=b[i];
	for (j=i+1;j<=n;j++) 
		sum -= a[i][j]*b[j];

	b[i]=sum/a[i][i];
	} 
} 

void polint(double xa[], double ya[], int n, double x, double *y, double *dy)
{
int i,m,ns=1;
double den,dif,dift,ho,hp,w;
double *c, *d;
dif=fabs(x-xa[1]);
c=vector(1,n);
d=vector(1,n);
for(i=1;i<=n;i++)
	{
	if ((dift=fabs(x-xa[i]))<dif)
		{
		ns=i;
		dif=dift;
		}
	c[i]=ya[i];
	d[i]=ya[i];
	}
*y=ya[ns--];
for(m=1;m<n;m++)
	{	
	for(i=1;i<=n-m;i++)
		{
		ho=xa[i]-x;
		hp=xa[i+m]-x;
		w=c[i+1]-d[i];
		den=ho-hp;
		den=w/den;
		d[i]=hp*den;
		c[i]=ho*den;
		}
	*y+=(*dy=(2*ns<(n-m)?c[ns+1]:d[ns--]));
	}
free_vector(d,1,n);
free_vector(c,1,n);
}
