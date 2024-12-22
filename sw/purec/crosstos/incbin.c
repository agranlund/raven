#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv, char **envp)
{
    FILE* fd;

    if(argc > 1)
    {
        fd = fopen(argv[1], "rb");

        if(fd)
        {
            int i  = 1;
            int ch = 0;

            printf("#include <stdint.h>\n\n");
            printf("uint8_t binary[] = \n");
            printf("{\r\t");

            for(;;)
            {
                ch = fgetc(fd);

                if(ch == EOF)
                {
                    printf("\n};\n");
                    fclose(fd);
                    return 0;
                }
                
                printf("0x%02x, %s", ch, (!(i++ & 15)) ? "\n\t" : "");
            }
        }
    }

    return -1;
}