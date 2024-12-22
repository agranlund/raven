
#include <stdio.h>
#include <osbind.h>

#define CREATE	'1'
#define RENAME	'2'
#define DELETE	'3'
#define READ	'4'
#define WRITE	'5'
#define QUIT	'6'

extern char *gname();

main()
{
	char fname1[80];
	char fname2[80];
	int	 done = 0;
	int	 err;
	int  fd;

	while(!done) {
		switch(gmenu()) {
			case CREATE:
				fd = Fcreate(gname(fname1, "File to create:"), 0);

				if (fd < 0)
					puts("Error ocurred during create.");
				
				printf("File: `%s' created.\n\n", fname1);
				Fclose(fd);
			break;

			case RENAME:
				gname(fname1, "New filename:");
				gname(fname2, "Old filename:");
				err = Frename(0, fname2, fname1);

				if (err < 0)
					puts("Error ocurred during rename.");

				printf("File: `%s' renamed to `%s'.\n\n", fname2, fname1);
			break;

			case DELETE:
				err = Fdelete(gname(fname1, "File to Delete:"));

				if (err < 0)
					puts("Error ocurred during delete.");

				printf("File: `%s' deleted.\n\n", fname1);
			break;

			case READ:
				readfrom(gname(fname1, "File to read:"));

				puts("\n");
			break;

			case WRITE:
				writeto(gname(fname1, "File to write:"));

				puts("\n");
			break;

			case QUIT:
				done = 1;
			break;

			default: 
				puts("\nError: Unknown function.\n");
			break;
		}
	}
}


int gmenu()
{
	int c;

	puts("1) Create a file.");
	puts("2) Rename a file.");
	puts("3) Delete a file.");
	puts("4) Read  text from file.");
	puts("5) Write text to file.");
	puts("");
	puts("6) Quit.");
	puts("");
	printf("Enter number: ");  fflush(stdout);

	return Bconin(2);
}


char *gname(fname, literal)
	char *fname, *literal;
{
	printf("\n%s ", literal);  fflush(stdout);
	scanf("%s", fname);

	return fname;
}


readfrom(fname)
	char *fname;
{
	int  fd;
	char c;

	fd = Fopen(fname, 0);

	if (fd < 0)
		printf("Error: Couldn't open file `%s'.\n", fname);
	else {
		while(Fread(fd, 1L, &c))
			printf("%c", c);

		Fclose(fd);
	}
}


writeto(fname)
	char *fname;
{
	int   fd;
	int   c;
	int   err;
	char *testtext = "This is a test line1\r\nThis is a test line2\r\n";
	long  l;

	fd = Fopen(fname, 1);

	if (fd < 0)
		printf("Error: Couldn't open write file `%s'.\n", fname);

	else {
		err = Fwrite(fd, (long) strlen(testtext), testtext);

		if (!err)
			printf("Error writing Data..., error = %d\n", err);

		Fclose(fd);
	}
}
