//***************************************************************************
//  File:   general.cpp
//  Author: Marten Jostmann
//
//  This file includes general functions used in the different implementations.
//
//***************************************************************************

#include "general.h"
#include <iostream>
#include <string>
#include <map>

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

std::map<std::string, std::string> parseArgs(int argc, char **argv)
{
    std::map<std::string, std::string> args;
    std::string current_param = "";

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg[0] == '-')
        {
            current_param = arg.substr(1);
            args[current_param] = "";
        }
        else if (current_param != "")
        {
            args[current_param] = arg;
            current_param = "";
        }
    }
    return args;
}

int getWidth(std::map<std::string, std::string> params)
{
    if (params.find("w") == params.end())
    {
        std::cout << "The width is not specified. Please specify it with -w (Using standard 1024)" << std::endl;
        return 1024;
    }
    else
    {
        return std::stoi(params["w"]);
    }
}

int getPlatformId(std::map<std::string, std::string> params)
{
    if (params.find("p") == params.end())
    {
        std::cout << "The OpenCL platform id is not specified. Please specify it with -p (Using standard 0)" << std::endl;
        return 0;
    }
    else
    {
        return std::stoi(params["p"]);
    }
}