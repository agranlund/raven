#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include "paths.h"

char* cwd = "./";

static bool skip_slashes(char** in, char** out)
{
    char*  str = *in;

    bool found = false;

    while((*str == '/') || (*str == '\\'))
    {
        str++;

        found = true;
    }

    if(out)
    {
        *out = str;
    }

    return found;
}

static int count_chars(char** in, char** out)
{
    char* str    = *in;

    int   count = 0;

    while(*str && (*str != '/') && (*str != '\\'))
    {
        str++;
        count++;
    }

    if(out)
    {
        *out = str;
    }

    return count;
}

static bool path_find_item(char* path, char* item, int count)
{
    bool match = false;
    struct dirent *de;

    int i;

    DIR* dir = opendir (path);

    if(dir)
    {
        while(!match && (de = readdir (dir)))
        {
            match = true;

            char c1, c2;
            for(i = 0; i < count; i++)
            {
                c1 = toupper(de->d_name[i]);
                c2 = toupper(item[i]);

                if(c1 != c2)
                {   
                    match = false;
                    break;
                }
                else
                {
                    if(!c1)
                    {
                        break;
                    }
                }
            }
        }

        if(match)
        {
            strcat(path, "/");
            strcat(path, de->d_name);
        }

        closedir(dir);
    }

    return match;
}

void path_close(char* path)
{
    if(path)
    {
        free(path);
    }
}

char* path_open(char* fname, bool exist)
{
    char* search_path = malloc(4000);
    char* root_path;

    if(search_path)
    {
        /*
         * Check for leading slashes to determine
         * starting point (relative cwd or absolute path)
         */

        //printf("search %s, %d\n", fname, exist);

        if(skip_slashes(&fname, &fname))
        {
            /*
             * Slashes found.
             *  - this is an absolute path
             */

            if (root_path = getenv("TOS_ROOT_PATH"))
                strcpy(search_path, root_path);
            else
                strcpy(search_path, "/");
        }
        else
        {
            strcpy(search_path, cwd);
        }

        char* fnext = fname;

        do
        {
            char* fend;

            /*
             * Count number of chars until slash/end
             */

            int count = count_chars(&fnext, &fend);

            /* item now at <fnext>, <count> chars long */
            if(path_find_item(search_path, fnext, count)) // also adds actual name to path
            {
            //    printf("fend %s fnext %s\n", fend, fnext);

                /* Proceed to next item */
                skip_slashes(&fend, &fnext);

            //    printf("fend %s fnext %s\n", fend, fnext);
            }
            else
            {
            //    printf("fend %s fnext %s exist %d\n", fend, fnext, exist);
 
                if(*fend || exist)
                {
                    free(search_path);
                    search_path = NULL;
                }
                else
                {
                    strcat(search_path, "/");
                    strcat(search_path, fnext);
                }
 
                break;
            }
        }
        while(*fnext);

    //    printf("found %s\n", search_path);
    }

    return search_path;
}
