#ifndef GENERAL_H
#define GENERAL_H
#include <string>
#include <map>

char *readKernel(const char *, long *);

std::map<std::string, std::string> parseArgs(int, char **);

std::tuple<int, int, int> getXYZ(std::map<std::string, std::string>);

int getPlatformId(std::map<std::string, std::string>);

const char *getKernelPath(std::map<std::string, std::string>);

const char *getHeaderPath(std::map<std::string, std::string>);

#endif