/*

Translated to C by Bonnie Toy 5/88
 - modified on 2/25/94  to fix a problem with daxpy  for
   unequal increments or equal increments not equal to 1.
     Jack Dongarra
 - modified on 08/27/09 fix typo line 270, plus set 'ix' to 0
  in the case incx is not 1
     Julie Langou

To compile single precision version for Sun-4:

	cc -DSP -O4 -fsingle -fsingle2 clinpack.c -lm

To compile double precision version for Sun-4:

	cc -DDP -O4 clinpack.c -lm

To obtain rolled source BLAS, add -DROLL to the command lines.
To obtain unrolled source BLAS, add -DUNROLL to the command lines.

You must specify one of -DSP or -DDP to compile correctly.

You must specify one of -DROLL or -DUNROLL to compile correctly.

*/

#ifdef SP
#define REAL float
#define ZERO 0.0
#define ONE 1.0
#define PREC "Single "
#endif

#ifdef DP
#define REAL double
#define ZERO 0.0e0
#define ONE 1.0e0
#define PREC "Double "
#endif

#define NTIMES 10

#ifdef ROLL
#define ROLLING "Rolled "
#endif
#ifdef UNROLL
#define ROLLING "Unrolled "
#endif

#include <stdio.h>
#include <math.h>
#include <sys/resource.h>

static REAL time[9][9];

main ()
{
	static REAL aa[200][200],a[200][201],b[200],x[200];
	REAL cray,ops,total,norma,normx;
	REAL resid,residn,eps,t1,tm,tm2;
	REAL epslon(),second(),kf;
	static int ipvt[200],n,i,ntimes,info,lda,ldaa,kflops;

	lda = 201;
	ldaa = 200;
	cray = .056; 
	n = 100;

	fprintf(stdout,ROLLING);fprintf(stdout,PREC);fprintf(stdout,"Precision Linpack\n\n");
	fprintf(stderr,ROLLING);fprintf(stderr,PREC);fprintf(stderr,"Precision Linpack\n\n");

        ops = (2.0e0*(n*n*n))/3.0 + 2.0*(n*n);

        matgen(a,lda,n,b,&norma);
        t1 = second();
        dgefa(a,lda,n,ipvt,&info);
        time[0][0] = second() - t1;
        t1 = second();
        dgesl(a,lda,n,ipvt,b,0);
        time[1][0] = second() - t1;
        total = time[0][0] + time[1][0];

/*     compute a residual to verify results.  */ 

        for (i = 0; i < n; i++) {
            	x[i] = b[i];
	}
        matgen(a,lda,n,b,&norma);
        for (i = 0; i < n; i++) {
            	b[i] = -b[i];
	}
        dmxpy(n,b,n,lda,x,a);
        resid = 0.0;
        normx = 0.0;
        for (i = 0; i < n; i++) {
            	resid = (resid > fabs((double)b[i])) 
			? resid : fabs((double)b[i]);
            	normx = (normx > fabs((double)x[i])) 
			? normx : fabs((double)x[i]);
	}
        eps = epslon((REAL)ONE);
        residn = resid/( n*norma*normx*eps );
	
   	printf("     norm. resid      resid           machep");
        printf("         x[0]-1        x[n-1]-1\n");
	printf("  %8.1f      %16.8e%16.8e%16.8e%16.8e\n",
	       (double)residn, (double)resid, (double)eps, 
               (double)x[0]-1, (double)x[n-1]-1);

   	fprintf(stderr,"    times are reported for matrices of order %5d\n",n);
	fprintf(stderr,"      dgefa      dgesl      total       kflops     unit");
	fprintf(stderr,"      ratio\n");

        time[2][0] = total;
        time[3][0] = ops/(1.0e3*total);
        time[4][0] = 2.0e3/time[3][0];
        time[5][0] = total/cray;

   	fprintf(stderr," times for array with leading dimension of%5d\n",lda);
	print_time(0);

        matgen(a,lda,n,b,&norma);
        t1 = second();
        dgefa(a,lda,n,ipvt,&info);
        time[0][1] = second() - t1;
        t1 = second();
        dgesl(a,lda,n,ipvt,b,0);
        time[1][1] = second() - t1;
        total = time[0][1] + time[1][1];
        time[2][1] = total;
        time[3][1] = ops/(1.0e3*total);
        time[4][1] = 2.0e3/time[3][1];
        time[5][1] = total/cray;

        matgen(a,lda,n,b,&norma);
        t1 = second();
        dgefa(a,lda,n,ipvt,&info);
        time[0][2] = second() - t1;
        t1 = second();
        dgesl(a,lda,n,ipvt,b,0);
        time[1][2] = second() - t1;
        total = time[0][2] + time[1][2];
        time[2][2] = total;
        time[3][2] = ops/(1.0e3*total);
        time[4][2] = 2.0e3/time[3][2];
        time[5][2] = total/cray;

        ntimes = NTIMES;
        tm2 = 0.0;
        t1 = second();

	for (i = 0; i < ntimes; i++) {
            	tm = second();
		matgen(a,lda,n,b,&norma);
		tm2 = tm2 + second() - tm;
		dgefa(a,lda,n,ipvt,&info);
	}

        time[0][3] = (second() - t1 - tm2)/ntimes;
        t1 = second();

	for (i = 0; i < ntimes; i++) {
            	dgesl(a,lda,n,ipvt,b,0);
	}

        time[1][3] = (second() - t1)/ntimes;
        total = time[0][3] + time[1][3];
        time[2][3] = total;
        time[3][3] = ops/(1.0e3*total);
        time[4][3] = 2.0e3/time[3][3];
        time[5][3] = total/cray;

	print_time(1);
	print_time(2);
	print_time(3);

        matgen(aa,ldaa,n,b,&norma);
        t1 = second();
        dgefa(aa,ldaa,n,ipvt,&info);
        time[0][4] = second() - t1;
        t1 = second();
        dgesl(aa,ldaa,n,ipvt,b,0);
        time[1][4] = second() - t1;
        total = time[0][4] + time[1][4];
        time[2][4] = total;
        time[3][4] = ops/(1.0e3*total);
        time[4][4] = 2.0e3/time[3][4];
        time[5][4] = total/cray;

        matgen(aa,ldaa,n,b,&norma);
        t1 = second();
        dgefa(aa,ldaa,n,ipvt,&info);
        time[0][5] = second() - t1;
        t1 = second();
        dgesl(aa,ldaa,n,ipvt,b,0);
        time[1][5] = second() - t1;
        total = time[0][5] + time[1][5];
        time[2][5] = total;
        time[3][5] = ops/(1.0e3*total);
        time[4][5] = 2.0e3/time[3][5];
        time[5][5] = total/cray;

	matgen(aa,ldaa,n,b,&norma);
	t1 = second();
	dgefa(aa,ldaa,n,ipvt,&info);
	time[0][6] = second() - t1;
	t1 = second();
	dgesl(aa,ldaa,n,ipvt,b,0);
	time[1][6] = second() - t1;
	total = time[0][6] + time[1][6];
	time[2][6] = total;
	time[3][6] = ops/(1.0e3*total);
	time[4][6] = 2.0e3/time[3][6];
	time[5][6] = total/cray;

	ntimes = NTIMES;
	tm2 = 0;
	t1 = second();
	for (i = 0; i < ntimes; i++) {
		tm = second();
		matgen(aa,ldaa,n,b,&norma);
		tm2 = tm2 + second() - tm;
		dgefa(aa,ldaa,n,ipvt,&info);
	}
	time[0][7] = (second() - t1 - tm2)/ntimes;
	t1 = second();
	for (i = 0; i < ntimes; i++) {
		dgesl(aa,ldaa,n,ipvt,b,0);
	}
	time[1][7] = (second() - t1)/ntimes;
	total = time[0][7] + time[1][7];
	time[2][7] = total;
	time[3][7] = ops/(1.0e3*total);
	time[4][7] = 2.0e3/time[3][7];
	time[5][7] = total/cray;

	/* the following code sequence implements the semantics of
	   the Fortran intrinsics "nint(min(time[3][3],time[3][7]))"	*/

	kf = (time[3][3] < time[3][7]) ? time[3][3] : time[3][7];
	kf = (kf > ZERO) ? (kf + .5) : (kf - .5);
	if (fabs((double)kf) < ONE) 
		kflops = 0;
	else {
		kflops = floor(fabs((double)kf));
		if (kf < ZERO) kflops = -kflops;
	}

	fprintf(stderr," times for array with leading dimension of%4d\n",ldaa);
	print_time(4);
	print_time(5);
	print_time(6);
	print_time(7);
	fprintf(stderr,ROLLING);fprintf(stderr,PREC);
	fprintf(stderr," Precision %5d Kflops ; %d Reps \n",kflops,NTIMES);
}
     
/*----------------------*/ 
print_time (row)
int row;
{
fprintf(stderr,"%11.2f%11.2f%11.2f%11.0f%11.2f%11.2f\n",   (double)time[0][row],
       (double)time[1][row], (double)time[2][row], (double)time[3][row], 
       (double)time[4][row], (double)time[5][row]);
}
      
/*----------------------*/ 
matgen(a,lda,n,b,norma)
REAL a[],b[],*norma;
int lda, n;

/* We would like to declare a[][lda], but c does not allow it.  In this
function, references to a[i][j] are written a[lda*j+i].  */

{
	int init, i, j;

	init = 1325;
	*norma = 0.0;
	for (j = 0; j < n; j++) {
		for (i = 0; i < n; i++) {
			init = 3125*init % 65536;
			a[lda*j+i] = (init - 32768.0)/16384.0;
			*norma = (a[lda*j+i] > *norma) ? a[lda*j+i] : *norma;
		}
	}
	for (i = 0; i < n; i++) {
          b[i] = 0.0;
	}
	for (j = 0; j < n; j++) {
		for (i = 0; i < n; i++) {
			b[i] = b[i] + a[lda*j+i];
		}
	}
}

/*----------------------*/ 
dgefa(a,lda,n,ipvt,info)
REAL a[];
int lda,n,ipvt[],*info;

/* We would like to declare a[][lda], but c does not allow it.  In this
function, references to a[i][j] are written a[lda*i+j].  */
/*
     dgefa factors a double precision matrix by gaussian elimination.

     dgefa is usually called by dgeco, but it can be called
     directly with a saving in time if  rcond  is not needed.
     (time for dgeco) = (1 + 9/n)*(time for dgefa) .

     on entry

        a       REAL precision[n][lda]
                the matrix to be factored.

        lda     integer
                the leading dimension of the array  a .

        n       integer
                the order of the matrix  a .

     on return

        a       an upper triangular matrix and the multipliers
                which were used to obtain it.
                the factorization can be written  a = l*u  where
                l  is a product of permutation and unit lower
                triangular matrices and  u  is upper triangular.

        ipvt    integer[n]
                an integer vector of pivot indices.

        info    integer
                = 0  normal value.
                = k  if  u[k][k] .eq. 0.0 .  this is not an error
                     condition for this subroutine, but it does
                     indicate that dgesl or dgedi will divide by zero
                     if called.  use  rcond  in dgeco for a reliable
                     indication of singularity.

     linpack. this version dated 08/14/78 .
     cleve moler, university of new mexico, argonne national lab.

     functions

     blas daxpy,dscal,idamax
*/

{
/*     internal variables	*/

REAL t;
int idamax(),j,k,kp1,l,nm1;


/*     gaussian elimination with partial pivoting	*/

	*info = 0;
	nm1 = n - 1;
	if (nm1 >=  0) {
		for (k = 0; k < nm1; k++) {
			kp1 = k + 1;

          		/* find l = pivot index	*/

			l = idamax(n-k,&a[lda*k+k],1) + k;
			ipvt[k] = l;

			/* zero pivot implies this column already 
			   triangularized */

			if (a[lda*k+l] != ZERO) {

				/* interchange if necessary */

				if (l != k) {
					t = a[lda*k+l];
					a[lda*k+l] = a[lda*k+k];
					a[lda*k+k] = t; 
				}

				/* compute multipliers */

				t = -ONE/a[lda*k+k];
				dscal(n-(k+1),t,&a[lda*k+k+1],1);

				/* row elimination with column indexing */

				for (j = kp1; j < n; j++) {
					t = a[lda*j+l];
					if (l != k) {
						a[lda*j+l] = a[lda*j+k];
						a[lda*j+k] = t;
					}
					daxpy(n-(k+1),t,&a[lda*k+k+1],1,
					      &a[lda*j+k+1],1);
  				} 
  			}
			else { 
            			*info = k;
			}
		} 
	}
	ipvt[n-1] = n-1;
	if (a[lda*(n-1)+(n-1)] == ZERO) *info = n-1;
}

/*----------------------*/ 

dgesl(a,lda,n,ipvt,b,job)
int lda,n,ipvt[],job;
REAL a[],b[];

/* We would like to declare a[][lda], but c does not allow it.  In this
function, references to a[i][j] are written a[lda*i+j].  */

/*
     dgesl solves the double precision system
     a * x = b  or  trans(a) * x = b
     using the factors computed by dgeco or dgefa.

     on entry

        a       double precision[n][lda]
                the output from dgeco or dgefa.

        lda     integer
                the leading dimension of the array  a .

        n       integer
                the order of the matrix  a .

        ipvt    integer[n]
                the pivot vector from dgeco or dgefa.

        b       double precision[n]
                the right hand side vector.

        job     integer
                = 0         to solve  a*x = b ,
                = nonzero   to solve  trans(a)*x = b  where
                            trans(a)  is the transpose.

    on return

        b       the solution vector  x .

     error condition

        a division by zero will occur if the input factor contains a
        zero on the diagonal.  technically this indicates singularity
        but it is often caused by improper arguments or improper
        setting of lda .  it will not occur if the subroutines are
        called correctly and if dgeco has set rcond .gt. 0.0
        or dgefa has set info .eq. 0 .

     to compute  inverse(a) * c  where  c  is a matrix
     with  p  columns
           dgeco(a,lda,n,ipvt,rcond,z)
           if (!rcond is too small){
           	for (j=0,j<p,j++)
              		dgesl(a,lda,n,ipvt,c[j][0],0);
	   }

     linpack. this version dated 08/14/78 .
     cleve moler, university of new mexico, argonne national lab.

     functions

     blas daxpy,ddot
*/
{
/*     internal variables	*/

	REAL ddot(),t;
	int k,kb,l,nm1;

	nm1 = n - 1;
	if (job == 0) {

		/* job = 0 , solve  a * x = b
		   first solve  l*y = b    	*/

		if (nm1 >= 1) {
			for (k = 0; k < nm1; k++) {
				l = ipvt[k];
				t = b[l];
				if (l != k){ 
					b[l] = b[k];
					b[k] = t;
				}	
				daxpy(n-(k+1),t,&a[lda*k+k+1],1,&b[k+1],1);
			}
		} 

		/* now solve  u*x = y */

		for (kb = 0; kb < n; kb++) {
		    k = n - (kb + 1);
		    b[k] = b[k]/a[lda*k+k];
		    t = -b[k];
		    daxpy(k,t,&a[lda*k+0],1,&b[0],1);
		}
	}
	else { 

		/* job = nonzero, solve  trans(a) * x = b
		   first solve  trans(u)*y = b 			*/

		for (k = 0; k < n; k++) {
			t = ddot(k,&a[lda*k+0],1,&b[0],1);
			b[k] = (b[k] - t)/a[lda*k+k];
		}

		/* now solve trans(l)*x = y	*/

		if (nm1 >= 1) {
			for (kb = 1; kb < nm1; kb++) {
				k = n - (kb+1);
				b[k] = b[k] + ddot(n-(k+1),&a[lda*k+k+1],1,&b[k+1],1);
				l = ipvt[k];
				if (l != k) {
					t = b[l];
					b[l] = b[k];
					b[k] = t;
				}
			}
		}
	}
}

/*----------------------*/ 

daxpy(n,da,dx,incx,dy,incy)
/*
     constant times a vector plus a vector.
     jack dongarra, linpack, 3/11/78.
*/
REAL dx[],dy[],da;
int incx,incy,n;
{
	int i,ix,iy,m,mp1;

	if(n <= 0) return;
	if (da == ZERO) return;

	if(incx != 1 || incy != 1) {

		/* code for unequal increments or equal increments
		   not equal to 1 					*/

		ix = 0;
		iy = 0;
		if(incx < 0) ix = (-n+1)*incx;
		if(incy < 0)iy = (-n+1)*incy;
		for (i = 0;i < n; i++) {
			dy[iy] = dy[iy] + da*dx[ix];
			ix = ix + incx;
			iy = iy + incy;
		}
      		return;
	}

	/* code for both increments equal to 1 */

#ifdef ROLL
	for (i = 0;i < n; i++) {
		dy[i] = dy[i] + da*dx[i];
	}
#endif
#ifdef UNROLL

	m = n % 4;
	if ( m != 0) {
		for (i = 0; i < m; i++) 
			dy[i] = dy[i] + da*dx[i];
		if (n < 4) return;
	}
	for (i = m; i < n; i = i + 4) {
		dy[i] = dy[i] + da*dx[i];
		dy[i+1] = dy[i+1] + da*dx[i+1];
		dy[i+2] = dy[i+2] + da*dx[i+2];
		dy[i+3] = dy[i+3] + da*dx[i+3];
	}
#endif
}
   
/*----------------------*/ 

REAL ddot(n,dx,incx,dy,incy)
/*
     forms the dot product of two vectors.
     jack dongarra, linpack, 3/11/78.
*/
REAL dx[],dy[];

int incx,incy,n;
{
	REAL dtemp;
	int i,ix,iy,m,mp1;

	dtemp = ZERO;

	if(n <= 0) return(ZERO);

	if(incx != 1 || incy != 1) {

		/* code for unequal increments or equal increments
		   not equal to 1					*/

		ix = 0;
		iy = 0;
		if (incx < 0) ix = (-n+1)*incx;
		if (incy < 0) iy = (-n+1)*incy;
		for (i = 0;i < n; i++) {
			dtemp = dtemp + dx[ix]*dy[iy];
			ix = ix + incx;
			iy = iy + incy;
		}
		return(dtemp);
	}

	/* code for both increments equal to 1 */

#ifdef ROLL
	for (i=0;i < n; i++)
		dtemp = dtemp + dx[i]*dy[i];
	return(dtemp);
#endif
#ifdef UNROLL

	m = n % 5;
	if (m != 0) {
		for (i = 0; i < m; i++)
			dtemp = dtemp + dx[i]*dy[i];
		if (n < 5) return(dtemp);
	}
	for (i = m; i < n; i = i + 5) {
		dtemp = dtemp + dx[i]*dy[i] +
		dx[i+1]*dy[i+1] + dx[i+2]*dy[i+2] +
		dx[i+3]*dy[i+3] + dx[i+4]*dy[i+4];
	}
	return(dtemp);
#endif
}

/*----------------------*/ 
dscal(n,da,dx,incx)

/*     scales a vector by a constant.
      jack dongarra, linpack, 3/11/78.
*/
REAL da,dx[];
int n, incx;
{
	int i,m,mp1,nincx;

	if(n <= 0)return;
	if(incx != 1) {

		/* code for increment not equal to 1 */

		nincx = n*incx;
		for (i = 0; i < nincx; i = i + incx)
			dx[i] = da*dx[i];
		return;
	}

	/* code for increment equal to 1 */

#ifdef ROLL
	for (i = 0; i < n; i++)
		dx[i] = da*dx[i];
#endif
#ifdef UNROLL

	m = n % 5;
	if (m != 0) {
		for (i = 0; i < m; i++)
			dx[i] = da*dx[i];
		if (n < 5) return;
	}
	for (i = m; i < n; i = i + 5){
		dx[i] = da*dx[i];
		dx[i+1] = da*dx[i+1];
		dx[i+2] = da*dx[i+2];
		dx[i+3] = da*dx[i+3];
		dx[i+4] = da*dx[i+4];
	}
#endif

}

/*----------------------*/ 
int idamax(n,dx,incx)

/*
     finds the index of element having max. absolute value.
     jack dongarra, linpack, 3/11/78.
*/

REAL dx[];
int incx,n;
{
	REAL dmax;
	int i, ix, itemp;

	if( n < 1 ) return(-1);
	if(n ==1 ) return(0);
	if(incx != 1) {

		/* code for increment not equal to 1 */

		ix = 0;
		dmax = fabs((double)dx[0]);
		ix = ix + incx;
		for (i = 1; i < n; i++) {
			if(fabs((double)dx[ix]) > dmax)  {
				itemp = i;
				dmax = fabs((double)dx[ix]);
			}
			ix = ix + incx;
		}
	}
	else {

		/* code for increment equal to 1 */

		itemp = 0;
		dmax = fabs((double)dx[0]);
		for (i = 1; i < n; i++) {
			if(fabs((double)dx[i]) > dmax) {
				itemp = i;
				dmax = fabs((double)dx[i]);
			}
		}
	}
	return (itemp);
}

/*----------------------*/ 
REAL epslon (x)
REAL x;
/*
     estimate unit roundoff in quantities of size x.
*/

{
	REAL a,b,c,eps;
/*
     this program should function properly on all systems
     satisfying the following two assumptions,
        1.  the base used in representing dfloating point
            numbers is not a power of three.
        2.  the quantity  a  in statement 10 is represented to 
            the accuracy used in dfloating point variables
            that are stored in memory.
     the statement number 10 and the go to 10 are intended to
     force optimizing compilers to generate code satisfying 
     assumption 2.
     under these assumptions, it should be true that,
            a  is not exactly equal to four-thirds,
            b  has a zero for its last bit or digit,
            c  is not exactly equal to one,
            eps  measures the separation of 1.0 from
                 the next larger dfloating point number.
     the developers of eispack would appreciate being informed
     about any systems where these assumptions do not hold.

     *****************************************************************
     this routine is one of the auxiliary routines used by eispack iii
     to avoid machine dependencies.
     *****************************************************************

     this version dated 4/6/83.
*/

	a = 4.0e0/3.0e0;
	eps = ZERO;
	while (eps == ZERO) {
		b = a - ONE;
		c = b + b + b;
		eps = fabs((double)(c-ONE));
	}
	return(eps*fabs((double)x));
}
 
/*----------------------*/ 
dmxpy (n1, y, n2, ldm, x, m)
REAL y[], x[], m[];
int n1, n2, ldm;

/* We would like to declare m[][ldm], but c does not allow it.  In this
function, references to m[i][j] are written m[ldm*i+j].  */

/*
   purpose:
     multiply matrix m times vector x and add the result to vector y.

   parameters:

     n1 integer, number of elements in vector y, and number of rows in
         matrix m

     y double [n1], vector of length n1 to which is added 
         the product m*x

     n2 integer, number of elements in vector x, and number of columns
         in matrix m

     ldm integer, leading dimension of array m

     x double [n2], vector of length n2

     m double [ldm][n2], matrix of n1 rows and n2 columns

 ----------------------------------------------------------------------
*/
{
	int j,i,jmin;
	/* cleanup odd vector */

	j = n2 % 2;
	if (j >= 1) {
		j = j - 1;
		for (i = 0; i < n1; i++) 
            		y[i] = (y[i]) + x[j]*m[ldm*j+i];
	} 

	/* cleanup odd group of two vectors */

	j = n2 % 4;
	if (j >= 2) {
		j = j - 1;
		for (i = 0; i < n1; i++)
            		y[i] = ( (y[i])
                  	       + x[j-1]*m[ldm*(j-1)+i]) + x[j]*m[ldm*j+i];
	} 

	/* cleanup odd group of four vectors */

	j = n2 % 8;
	if (j >= 4) {
		j = j - 1;
		for (i = 0; i < n1; i++)
			y[i] = ((( (y[i])
			       + x[j-3]*m[ldm*(j-3)+i]) 
			       + x[j-2]*m[ldm*(j-2)+i])
			       + x[j-1]*m[ldm*(j-1)+i]) + x[j]*m[ldm*j+i];
	} 

	/* cleanup odd group of eight vectors */

	j = n2 % 16;
	if (j >= 8) {
		j = j - 1;
		for (i = 0; i < n1; i++)
			y[i] = ((((((( (y[i])
			       + x[j-7]*m[ldm*(j-7)+i]) + x[j-6]*m[ldm*(j-6)+i])
		  	       + x[j-5]*m[ldm*(j-5)+i]) + x[j-4]*m[ldm*(j-4)+i])
			       + x[j-3]*m[ldm*(j-3)+i]) + x[j-2]*m[ldm*(j-2)+i])
			       + x[j-1]*m[ldm*(j-1)+i]) + x[j]  *m[ldm*j+i];
	} 
	
	/* main loop - groups of sixteen vectors */

	jmin = (n2%16)+16;
	for (j = jmin-1; j < n2; j = j + 16) {
		for (i = 0; i < n1; i++) 
			y[i] = ((((((((((((((( (y[i])
			       	+ x[j-15]*m[ldm*(j-15)+i]) 
				+ x[j-14]*m[ldm*(j-14)+i])
			        + x[j-13]*m[ldm*(j-13)+i]) 
				+ x[j-12]*m[ldm*(j-12)+i])
			        + x[j-11]*m[ldm*(j-11)+i]) 
				+ x[j-10]*m[ldm*(j-10)+i])
			        + x[j- 9]*m[ldm*(j- 9)+i]) 
				+ x[j- 8]*m[ldm*(j- 8)+i])
			        + x[j- 7]*m[ldm*(j- 7)+i]) 
				+ x[j- 6]*m[ldm*(j- 6)+i])
			        + x[j- 5]*m[ldm*(j- 5)+i]) 
				+ x[j- 4]*m[ldm*(j- 4)+i])
			        + x[j- 3]*m[ldm*(j- 3)+i]) 
				+ x[j- 2]*m[ldm*(j- 2)+i])
			        + x[j- 1]*m[ldm*(j- 1)+i]) 
				+ x[j]   *m[ldm*j+i];
	}
} 

/*----------------------*/
#include <sys/time.h>
#include <sys/resource.h>

REAL second()
{
    struct rusage ru;
    REAL t;

    // Call getrusage without the custom declaration
    getrusage(RUSAGE_SELF, &ru);

    t = (REAL) (ru.ru_utime.tv_sec + ru.ru_stime.tv_sec) +
        ((REAL) (ru.ru_utime.tv_usec + ru.ru_stime.tv_usec)) / 1.0e6;

    return t;
}

