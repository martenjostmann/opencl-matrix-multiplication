#ifndef GENERAL_H
#define GENERAL_H
#include <string>
#include <map>

void checkSolution(float *, int);
std::map<std::string, std::string> parseArgs(int, char **);
std::tuple<int, int, int> getXYZ(std::map<std::string, std::string>);

#endif