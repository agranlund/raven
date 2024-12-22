
#include <stdio.h>

#define OK		1
#define CANCEL	0

main()
{
	char default_path[80];
	char default_name[80];
	int  button;

	appl_init();

	strcpy(default_path, "A:\\*.*");
	strcpy(default_name, "Untitled");

	fsel_input(default_path, default_name, &button);

	if (button == OK)
		printf("You have selected the file <%s>.\n", default_name);
	else
		printf("You have cancelled the file selection.\n");

	printf("Press RETURN to end.\n");
	getchar();

	appl_exit();
	exit(0);
}
