
#ifndef DL_MATHSTUFF
#define DL_MATHSTUFF

extern double dc_e;	/* e	*/
extern double dcpi;	/* pi	*/
extern double dcph;	/* pi/2	*/
extern double dcpq;	/* pi/4	*/
extern double dcln2;	/* ln 2	*/
extern double dcin;	/* infimum	*/
extern double dcsu;	/* supremum	*/
extern double dchf;	/* 0.5	*/
extern double dc1;	/* 1.0	*/
extern double dc1h;	/* 1.5	*/
extern double dc10;	/* 10.0	*/

extern double atof();
extern double sin();
extern double cos();
extern double tan();
extern double asin();
extern double acos();
extern double atan();
extern double exp();
extern double exp2();
extern double exp10();
extern double log();
extern double log2();
extern double log10();
extern double mulpower2();
extern double powerd();
extern double poweri();
extern double power10();
extern double sqr();
extern double sqrt();
extern double dabs();
extern double drand();
extern double fac();
extern double lngamma();
extern double matinv();
extern double dint();

#define INF dcin
#define SUP dcsu

#endif
