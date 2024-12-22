#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "pcmake.h"

/* ---------------------------------------------------------------------- */

void list_append(strlist **list, const char *str)
{
	strlist *entry = (strlist *)g_malloc(sizeof(*entry) + strlen(str));
	
	if (entry != NULL)
	{
		entry->next = NULL;
		strcpy(entry->str, str);
		while (*list != NULL)
			list = &(*list)->next;
		*list = entry;
	}
}

/* ---------------------------------------------------------------------- */

strlist *list_copy(strlist *list)
{
	strlist *root;
	strlist *entry;
	
	root = NULL;
	for (entry = list; entry != NULL; entry = entry->next)
	{
		list_append(&root, entry->str);
	}
	return root;
}

/* ---------------------------------------------------------------------- */

void list_free(strlist **list)
{
	strlist *entry, *next;
	
	for (entry = *list; entry != NULL; entry = next)
	{
		next = entry->next;
		g_free(entry);
	}
	*list = NULL;
}
