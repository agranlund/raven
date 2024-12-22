#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.h"
#include "cpu.h"

int main(int argc, char **argv, char **envp)
{
    bool done = false;

    char cmd[1024] = "";
/*
    for (char **env = envp; *env != 0; env++)
    {
        char *thisEnv = *env;
        printf("%s\n", thisEnv);    
    }
*/
    int arg;

    int i = 0;

    for(arg = 1; arg < argc; arg++)
    {
        char* aptr = argv[arg];

        while(*aptr && i < (sizeof(cmd) - 1))
        {
            cmd[i++] = *aptr++;
        }

        if(i < (sizeof(cmd) - 1) && (arg < (argc - 1)))
        {
            cmd[i++] = ' ';
        }
        else
        {
            break;
        }
    }

    cmd[i] = '\0';

    //printf("Command line %s\n", cmd);

    uint32_t sys_pd = cpu_init(argc, argv, envp);

    if(cpu_load(binary, 500000, (const char*)cmd, sys_pd))
    {
        do
        {
           // char buf[1000];
         //   m68k_disassemble(buf, m68k_get_reg(NULL, M68K_REG_PC), M68K_CPU_TYPE_68000);

        //    printf("%08x %s\n", m68k_get_reg(NULL, M68K_REG_PC), buf);
         //   cpu_run(1);

            cpu_run(8000000);
      
        } while(!done);
    }

    return 0;
}
