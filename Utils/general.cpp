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
