#ifndef GENERAL_H
#define GENERAL_H
#include <string>
#include <map>

char *readKernel(const char *, long *);

std::map<std::string, std::string> parseArgs(int, char **);

int getWidth(std::map<std::string, std::string>);

int getPlatformId(std::map<std::string, std::string>);

#endif