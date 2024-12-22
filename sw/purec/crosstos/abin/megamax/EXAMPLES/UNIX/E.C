/*
	compute the natural number (e).  Up to 100,000 digits of e can be
	produced.
*/

#include <osbind.h>
#include <stdio.h>

long *ptr;

gettime()
{
	*ptr = *(long *)0x462;
}

main()
{
	long digits;
	long prec;
	long l;
	char *farray, *earray;
	register char *f, *e;
	register unsigned i,cont;
	int j;
	long start, t, t2;
	char div10[200], rem10[200];
	char fname[30];

	farray = (char *)Malloc(60000L);
	earray = (char *)Malloc(60000L);

	for (i=0; i<200; i++) {
		div10[i] = i/100;
		rem10[i] = i%100;
	}

	puts("Compute e to many digts.");
	while (1) {
		printf("\nNumber of digits? ");
		scanf("%ld", &digits);
		prec = digits/2 + 3;	/* six guard digits */
		printf("Output file (CR for screen)? ");
		gets(fname);	/* gets \n left by scanf */
		gets(fname);

		ptr = &t;
		Supexec(gettime);
		for (l=0; l<prec; l++)
			farray[l] = earray[l] = 0;
		start = 0;
		farray[0] = 10;
		earray[0] = 20;
		i = 2;
		asm {
			top:
				movea.l	farray(A6), f
				adda.l	start(A6), f
			loop1:					;move start to first non-zero digit in f[]
				tst.b	(f)+
				bne		cont1
				addq.l	#1, start(A6)
				bra		loop1
			cont1:

				move.l	prec(A6), D2
				sub.l	start(A6), D2
				bls		end
				subq	#1, D2 
				subq.l	#1, f			;restore f from previous loop
				clr.l	D1				;remainder
				clr.l 	D0
				move.l	#100, D3
			loop2:
				mulu	D3, D1
				clr.l	D0
				move.b	(f), D0
				add.l	D0, D1
				divu	i, D1
				move.b	D1, (f)+
				swap	D1
				dbf		D2, loop2

				clr.l	D1				;add f[] to e[]
				move.l	prec(A6), D2 
				sub.l	start(A6), D2	;must add all significant digits
				subq	#1, D2
				movea.l	farray(A6), f
				adda.l	prec(A6), f
				movea.l	earray(A6), e
				adda.l	prec(A6), e
				lea		rem10(A6), A0
				lea		div10(A6), A1
			loop3:
				add.b	-(f), D1
				add.b	-(e), D1
				move.b	0(A0, D1), (e)
				move.b	0(A1, D1), D1	;carry
				dbf		D2, loop3

			loop4:					;only keep going as long as there is carry
				add.b	-(f), D1
				add.b	-(e), D1
				move.b	0(A0, D1), (e)
				move.b	0(A1, D1), D1	;carry
				bne		loop4

				addq	#1, i
				bra		top
			end:
		}

		ptr = &t2;
		Supexec(gettime);
		if (fname[0])
			freopen(fname, "w", stdout);
		printf("%u iterations in %0.2f seconds\n", i-2, 
			(float)(t2-t)/(Getrez() == 2 ? 70 : 60));
		for (l=0; l<digits; l++) {
			if (!(l%65))
				printf("\n");
			printf("%d", l&1 ? earray[l>>1]%10 : earray[l>>1]/10);
			if (!(l%5))
				printf(" ");
		}
		putchar('\n');
		fclose(stdout);
		freopen("CON:", "w", stdout);
	}
}
