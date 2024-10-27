/*
 *	MATH.H		Math functionality for AHCC
 */

#ifndef _MATH_H
# error "Never use <ahcc/mathinl.h> directly; include <math.h> instead."
#endif

#ifdef __AHCC__

	#ifndef __68881__

		/*	Of course the following is NON C
		 *   The ANSI standard tells us that standard headers are completely
		 *		implementor defined as long as their function complies the standard.
		 *			They need not even be files at all.
		 *		#include <name>		however is the standerd way to invoke them.
		 *	Every implementor has its own way to deal with extracode software.
		 *	I too do it my way.
		 *		Basicly I borrowed this from Algol68, long before I ever heard of C,
		 *			or even C++.
		 */

		/* ahcc puts every next declaration in front of the table!!
			So it is recommended to put these declarations
				in ascending order of frequency.
			Put the most frequent at the end.
		*/
		double	__OP__ *  (double,double) XXmul,
				__OP__ /  (double,double) XXdiv,
			/*	__OP__ %  (double,double) XXrem,	*/
				__OP__ +  (double,double) XXadd,
				__OP__ -  (double,double) XXsub,
				__OP__ -  (double)		Xneg;

		int		__OP__ !  (double) 		Xnot,
				__OP__ <  (double,double) XXcmp,
				__OP__ >  (double,double) XXcmp,
				__OP__ <= (double,double) XXcmp,
				__OP__ >= (double,double) XXcmp,
				__OP__ == (double,double) XXcmp,
				__OP__ != (double,double) XXcmp;

		unsigned
		long	__UC__(double) 		Xlcnv;
		int		__UC__(double)		Xlcnv;
		unsigned
		int		__UC__(double)		Xlcnv;
		char	__UC__(double)		Xlcnv;
		unsigned
		char 	__UC__(double)		Xlcnv;
		float	__UC__(double)		Xfcnv;
		long	__UC__(double)		Xlcnv;
		double	__UC__(float)			fXcnv,
				__UC__(unsigned long)	ulXcnv,
				__UC__(unsigned int)	uiXcnv,
				__UC__(unsigned char) uiXcnv,
				__UC__(char)			iXcnv,
				__UC__(int)			iXcnv,
				__UC__(long)			lXcnv;

		/* in fact this is determined by the called software itself,
			so here are the names */
			float	__OP__ *  (float,float) FFmul,
					__OP__ /  (float,float) FFdiv,
				/*	__OP__ %  (float,float) FFrem,	*/
					__OP__ +  (float,float) FFadd,
					__OP__ -  (float,float) FFsub,
					__OP__ -	(float)       Fneg;

			bool 	__OP__ !  (float)       Fnot,
					__OP__ <  (float,float) FFcmp,
					__OP__ >  (float,float) FFcmp,
					__OP__ <= (float,float) FFcmp,
					__OP__ >= (float,float) FFcmp,
					__OP__ == (float,float) FFcmp,
					__OP__ != (float,float) FFcmp;

			unsigned
			long	__UC__(float)			_Flcnv;
			int		__UC__(float)			_Flcnv;
			unsigned
			int		__UC__(float)			_Flcnv;
			char	__UC__(float)			_Flcnv;
			unsigned
			char	__UC__(float)			_Flcnv;
			long	__UC__(float)			_Flcnv;
			float	__UC__(unsigned long)	_ulFcnv,
					__UC__(unsigned)		_uiFcnv,
					__UC__(char)			_iFcnv,		/* char on stack becomes int */
					__UC__(int)			_iFcnv,
					__UC__(long)			_lFcnv;

	#else		/* if FPU, built in monadic operators (compiler opt -8) */

		#define fabs(f) __FABS__(f)
		#define trunc(f) __FINTRZ__(f)
		#define fint(f) __FINT__(f)
		#define	sqrt(f) __FSQRT__(f)
		#define fintrz(f) __FINTRZ__(f)

		#if !defined(__mcoldfire__) && !defined(__COLDFIRE__)
		#define	sin(f) __FSIN__(f)
		#define	cos(f) __FCOS__(f)
		#define	tan(f) __FTAN__(f)
		#define	asin(f) __FASIN__(f)
		#define	acos(f) __FACOS__(f)
		#define	atan(f) __FATAN__(f)
		#define fetoxm1(f) __FETOXM1__(f)
		#define	log(f) __FLOGN__(f)
		#define flognp1(f) __FLOGNP1__(f)
		#define log2(f) __FLOG2__(f)
		#define	log10(f) __FLOG10__(f)
		#define fneg(f) __FNEG__(f)
		#define	exp(f) __FETOX__(f)
		#define	sinh(f) __FSINH__(f)
		#define	cosh(f) __FCOSH__(f)
		#define	tanh(f) __FTANH__(f)
		#define atanh(f) __FATANH__(f)
		#define fgetexp(f) __FGETEXP__(f)
		#define fgetman(f) __FGETMAN__(f)
		#define pow10(f) __FTENTOX__(f)
		#define pow2(f) __FTWOTOX__(f)
		#endif
	#endif

#endif	/* __AHCC__ */
