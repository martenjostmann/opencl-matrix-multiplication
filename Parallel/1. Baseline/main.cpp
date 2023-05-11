//***************************************************************************
//  File:   main.cpp
//  Author: Marten Jostmann
//
//  This file implements a baseline matrix multiplication using OpenCL.
//
//***************************************************************************

#include <iostream>
#include <chrono>
#include <cstring>
#include "../../Utils/matrix_init.h"
#include "../../Utils/general.h"
#include <CL/cl.h>

cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue commandQueue;
cl_kernel kernel;

void checkError(cl_int err)
{
    if (err != CL_SUCCESS)
        printf("Error with errorcode: %d\n", err);
}

void initOpenCL(int platform_id)
{
    cl_int err;
    cl_uint numPlatforms;

    // Get number of platforms
    err = clGetPlatformIDs(0, NULL, &numPlatforms);

    // Allocate memory for platforms
    cl_platform_id *platformList = (cl_platform_id *)malloc(numPlatforms * sizeof(cl_platform_id));

    // Get all platforms
    err = clGetPlatformIDs(numPlatforms, platformList, NULL);
    checkError(err);

    // Select specific platform
    cl_platform_id platform = platformList[platform_id];

    free(platformList);

    // Get device
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 2, &device, NULL);
    checkError(err);

    // Create context for device
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    checkError(err);

    // create command queue for context and device
    commandQueue = clCreateCommandQueue(context, device, 0, &err);
    checkError(err);
}

void createKernel(const char *kernel_path)
{
    cl_int err;

    long kernel_size;

    char *kernelSource = readKernel(kernel_path, &kernel_size);
    const char *constCode = kernelSource;

    cl_program program;
    program = clCreateProgramWithSource(context, 1, &constCode, NULL, &err);
    checkError(err);

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    kernel = clCreateKernel(program, "matrixMultiplicationKernel", &err);
    checkError(err);
}

void matrixMultiplication(float *M, float *N, float *P, int X, int Y, int Z)
{
    cl_int err;
    int size_M = X * Y * sizeof(float);
    int size_N = Y * Z * sizeof(float);
    int size_P = X * Z * sizeof(float);

    cl_mem Md = clCreateBuffer(context, CL_MEM_READ_ONLY, size_M, NULL, &err);
    checkError(err);
    cl_mem Nd = clCreateBuffer(context, CL_MEM_READ_ONLY, size_N, NULL, &err);
    checkError(err);

    err = clEnqueueWriteBuffer(commandQueue, Md, CL_FALSE, 0, size_M, M, 0, NULL, NULL);
    checkError(err);
    err = clEnqueueWriteBuffer(commandQueue, Nd, CL_FALSE, 0, size_N, N, 0, NULL, NULL);
    checkError(err);

    cl_mem Pd = clCreateBuffer(context, CL_MEM_READ_WRITE, size_P, NULL, &err);
    checkError(err);

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &Md);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &Nd);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &Pd);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &X);
    err |= clSetKernelArg(kernel, 4, sizeof(int), &Y);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &Z);
    checkError(err);

    size_t globalSize[] = {Z, X};
    size_t localSize[] = {32, 32};

    err = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
    checkError(err);

    err = clEnqueueReadBuffer(commandQueue, Pd, CL_TRUE, 0, size_P, P, 0, NULL, NULL);
    checkError(err);
}

int main(int argc, char **argv)
{
    float *M;
    float *N;
    float *P;
    int WIDTH;
    int PLATFORM_ID;
    const char *KERNEL_PATH;

    // Parse arguments
    std::map<std::string, std::string> params = parseArgs(argc, argv);

    // Get arguments
    WIDTH = getWidth(params);
    PLATFORM_ID = getPlatformId(params);
    KERNEL_PATH = getKernelPath(params);

    int X = 1024, Y = 128, Z = 128;

    M = matrixInit(M, X * Y, true);
    N = matrixInit(N, Y * Z, true);
    P = matrixInit(P, X * Z, false);

    initOpenCL(PLATFORM_ID);
    createKernel(KERNEL_PATH);

    // Start time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    matrixMultiplication(M, N, P, X, Y, Z);

    // End time
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    checkSolution(P, X * Z);

    // Free resources
    delete[] M;
    delete[] N;
    delete[] P;

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}

// g++ main.cpp ../../Utils/matrix_init.cpp -L"C:\Users\marte\vcpkg\packages\opencl_x64-windows\lib" -lOpenCL -I"C:\Users\marte\vcpkg\packages\opencl_x64-windows\include" -o main