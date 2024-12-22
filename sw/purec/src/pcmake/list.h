typedef struct _strlist {
	struct _strlist *next;
	char str[1];
} strlist;


void list_append(strlist **list, const char *str);
strlist *list_copy(strlist *list);
void list_free(strlist **list);
