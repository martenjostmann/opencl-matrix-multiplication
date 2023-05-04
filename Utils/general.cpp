#include "general.h"
#include <iostream>

char *readKernel(const char *filename, long *size)
{
    FILE *fp;
    char *source_str;
    size_t source_size, program_size;

    fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Failed to load kernel\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    program_size = ftell(fp);
    rewind(fp);

    source_str = (char *)malloc(program_size + 1);
    source_str[program_size] = '\0';

    fread(source_str, sizeof(char), program_size, fp);
    fclose(fp);

    *size = (program_size + 1);
    return source_str;
}