#include <osbind.h>
#include <stdio.h>
#include <stdint.h>

int
main(int argc, char *argv[], char *envp[])
{
    printf("argc: %d\r\n", argc);
    for (size_t i = 0; i < argc; i++) {
        printf("argv[%d]: %s\r\n", i, argv[i]);
    }

    for (size_t i = 0; i < 20 ; i++) {
        if (!envp[i]) break;
        printf("envp[%d]: %s\r\n", i, envp[i]);
    }

    while(1) {
        if (Cconis() == -1) {
            uint32_t key = Cnecin();
            if ((key&0xff) == ' ') break;
            printf("   ?>", key);
            printf("   remote: %x\r\n", key);
        }
    }

    Cconws("done\r\n");
    Cconws("done2\r\n");
}
