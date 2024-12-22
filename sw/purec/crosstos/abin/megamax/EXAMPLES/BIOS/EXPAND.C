
#include <stdio.h>
#include <osbind.h>

#define TAB 9
#define CR  13

main()
{
	char file[100];

	puts("Print file with tabs expanded to spaces");
	printf("File to expand? ");
	gets(file);
	expand(file, 8);
}

/*	File print (with TAB expansion)
*/
expand(fname, tabsize)
	char *fname;
	int   tabsize;
{
	FILE *fp;
	char c;
	int pos	= 0;
	int x;

	if (!(fp = fopen(fname, "br"))) {
		puts("Can't open file.");
	} else {
		while((c = getc(fp)) != EOF) {
			if (c == TAB) {
				for (x=0; x<tabsize-(pos%tabsize); x++) {
				/*
					Wait for printer be ready.
				*/
					while(!Cprnos())
						;
					Cprnout(' ');
				}
			} else {
				/*
					Wait for printer be ready.
				*/
				while(!Cprnos())
					;

				Cprnout(c);
			}
		if ( c == CR )
			pos	= 0;
		else
			pos++;
		}

		fclose(fp);
	}
}
