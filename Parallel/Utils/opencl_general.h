#ifndef OPENCL_GENERAL_H
#define OPENCL_GENERAL_H

#include <CL/cl.h>

void checkError(cl_int);
void printBuildLog(cl_program, cl_device_id);
void initOpenCL(int);
void createKernel(const char *);
void printBuildLog(cl_program, cl_device_id);
void matrixMultiplication(float *, float *, float *, int, int, int, size_t *, size_t *);
#endif