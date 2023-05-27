#include "opencl_params.h"
#include <iostream>
#include <string>
#include <map>
#include <CL/cl.h>

/**
 * Get the id of the OpenCL platform from the command line arguments.
 *
 * @param params map with the parameters
 * @return id of the OpenCL platform
 */
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

/**
 * Get the type of the OpenCL device from the command line arguments.
 *
 * @param params map with the parameters
 * @return type of the OpenCL device (CL_DEVICE_TYPE_GPU or CL_DEVICE_TYPE_CPU)
 */
cl_device_type getDeviceType(std::map<std::string, std::string> params)
{
    if (params.find("d") == params.end())
    {
        std::cout << "The OpenCL device type is not specified. Please specify it with -d (Using standard CL_DEVICE_TYPE_GPU)" << std::endl;
        return CL_DEVICE_TYPE_GPU;
    }
    else
    {
        std::string device_type = params["d"];
        if (device_type == "GPU")
        {
            return CL_DEVICE_TYPE_GPU;
        }
        else if (device_type == "CPU")
        {
            return CL_DEVICE_TYPE_CPU;
        }
        else
        {
            std::cerr << "Invalid device type: " << device_type << std::endl;
            std::cerr << "Valid options are 'CPU' and 'GPU'" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * Get the path to the OpenCL kernel file from the command line arguments.
 *
 * @param params map with the parameters
 * @return path to the OpenCL kernel file
 */
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

/**
 * get the path to the OpenCL header file from the command line arguments.
 *
 * @param params map with the parameters
 * @return path to the OpenCL header file
 */
const char *getHeaderPath(std::map<std::string, std::string> params)
{
    if (params.find("h") == params.end())
    {
        std::cout << "The header path is not specified. Please specify it with -h (Using standard \"properties.h\")" << std::endl;
        return "properties.h";
    }
    else
    {
        return params["h"].c_str();
    }
}