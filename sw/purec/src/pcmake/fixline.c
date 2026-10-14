#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcmake.h"
#include "fixline.h"

static char*  cached_path  = NULL;
static char** cached_lines = NULL;
static int    cached_count = 0;
static char   tempbuf[4096];

static bool load_src(const char *path)
{
	FILE *f;
	int cap;
	int i;

	if (cached_path && strcmp(cached_path, path) == 0) {
		return cached_lines != NULL;
	}

	if (cached_lines) {
		for (i = 0; i < cached_count; i++) {
			g_free(cached_lines[i]);
		}
		g_free(cached_lines);
	}

	g_free(cached_path);
	cached_path  = NULL;
	cached_lines = NULL;
	cached_count = 0;

	f = fopen(path, "r");
	if (!f) {
		return false;
	}

	cap = 512;
	cached_lines = g_new(char *, cap);
	if (!cached_lines) {
		fclose(f);
		return false;
	}

	cached_path = g_strdup(path);
	while (fgets(tempbuf, sizeof(tempbuf), f)) {
		if (cached_count >= cap) {
			cap *= 2;
			cached_lines = g_renew(char *, cached_lines, cap);
			if (!cached_lines) break;
		}
		cached_lines[cached_count++] = g_strdup(tempbuf);
	}
	fclose(f);
	return true;
}

/*
 * PureC double-counts every line that starts inside a multi-line block
 * comment, reporting the wrong line number for errors and warnings.
 * This calculates and returns the correct line number.
 */
int fixline(const char *src_file, int lineno)
{
	int i;
	int purec_counter = 0;
	int inside = 0;

	if (!load_src(src_file)) {
		return lineno;
	}

	for (i = 0; i < cached_count; i++) {
		int was_inside = inside;
		const char* p = cached_lines[i];

		while (*p) {
			if (inside && (p[0] == '*' && p[1] == '/')) {
				inside = 0;
				p++;
			} else if (!inside && (p[0] == '/' && p[1] == '*')) {
				inside = 1;
				p++;
			}
			p++;
		}

		purec_counter += was_inside ? 2 : 1;
		if (purec_counter >= lineno)
			return (i + 1);
	}

	return lineno;
}
