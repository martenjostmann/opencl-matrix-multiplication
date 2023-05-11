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

std::tuple<int, int, int> getXYZ(std::map<std::string, std::string> params)
{

    int x, y, z;

    if (params.find("x") == params.end())
    {
        std::cout << "X is not specified. Please specify it with -x (Using standard 1024)" << std::endl;
        x = 1024;
    }
    else
    {
        x = std::stoi(params["x"]);
    }

    if (params.find("y") == params.end())
    {
        std::cout << "Y is not specified. Please specify it with -y (Using standard 1024)" << std::endl;
        y = 1024;
    }
    else
    {
        y = std::stoi(params["y"]);
    }

    if (params.find("z") == params.end())
    {
        std::cout << "Z is not specified. Please specify it with -z (Using standard 1024)" << std::endl;
        z = 1024;
    }
    else
    {
        z = std::stoi(params["z"]);
    }

    return std::make_tuple(x, y, z);
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

const char *getKernelPath(std::map<std::string, std::string> params)
{
    if (params.find("k") == params.end())
    {
        std::cout << "The kernel path is not specified. Please specify it with -k (Using standard \"kernel.cl\")" << std::endl;
        return "kernel.cl";
    }
    else
    {
        return params["k"].c_str();
    }
}