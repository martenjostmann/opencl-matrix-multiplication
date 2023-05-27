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

/**
 * Check the solution by computing a checksum. It will print the result to the console.
 *
 * @param *f pointer to the array to be checked
 * @param size size of the array
 */
void checkSolution(float *f, int size)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += f[i];
    }

    std::cout << "CheckSum: " << sum << std::endl;
}

/**
 * Pase command line arguments and return a map with the parameters.
 *
 * @param argc number of arguments
 * @param **argv pointer to the arguments
 * @return map with the parameters
 */
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

/**
 * Get the size of the array from the command line arguments.
 *
 * @param params map with the parameters
 * @return tuple with array sizes (x, y, z)
 */
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
