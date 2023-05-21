#ifndef OPENCL_PARAMS_H
#define OPENCL_PARAMS_H
#include <string>
#include <map>
#include <CL/cl.h>

int getPlatformId(std::map<std::string, std::string>);

const char *getKernelPath(std::map<std::string, std::string>);

const char *getHeaderPath(std::map<std::string, std::string>);

cl_device_type getDeviceType(std::map<std::string, std::string>);

#endif