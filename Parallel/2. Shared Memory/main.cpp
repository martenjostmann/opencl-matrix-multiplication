//***************************************************************************
//  File:   main.cpp
//  Author: Marten Jostmann
//
//  This file implements a matrix multiplication using shared memory.
//
//***************************************************************************

#include <iostream>
#include <chrono>
#include <cstring>
#include "../../Utils/matrix_init.h"
#include "../../Utils/general.h"
#include <CL/cl.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define TILE_SIZE 32

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

void printBuildLog(cl_program program, cl_device_id device)
{
    cl_int err;
    char *build_log;
    size_t build_log_size;
    // Speichere den Build Log fuer program und device in build_log
    err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
    checkError(err);

    build_log = (char *)malloc(build_log_size);

    err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
    checkError(err);

    printf("Log:\n%s\n", build_log);

    free(build_log);
}

void createKernel()
{
    cl_int err;

    const char *kernelSource =
        "#define TILE_SIZE " STR(TILE_SIZE) "\n"
                                            "__kernel \
        void matrixMultiplicationKernel(__global float* Md, \
                                        __global float* Nd, \
                                        __global float* Pd, \
                                        int X,\
                                        int Y,\
                                        int Z) { \
            \
            __local float Ml[TILE_SIZE][TILE_SIZE]; \
            __local float Nl[TILE_SIZE][TILE_SIZE]; \
            \
            int col = get_global_id(0); \
            int row = get_global_id(1); \
            int l_col = get_local_id(0); \
            int l_row = get_local_id(1); \
            \
            float sum = 0; \
            for (int k = 0; k < (Y/TILE_SIZE); k++) { \
                Ml[l_row][l_col] = Md[row * Y + (k * TILE_SIZE + l_col)]; \
                Nl[l_row][l_col] = Nd[(k * TILE_SIZE + l_row) * Z + col]; \
                barrier(CLK_LOCAL_MEM_FENCE); \
                \
                for (int i = 0; i < TILE_SIZE; i++) { \
                    sum += Ml[l_row][i] * Nl[i][l_col]; \
                } \
                barrier(CLK_LOCAL_MEM_FENCE); \
            } \
            \
            Pd[row * Z + col] = sum; \
        }";

    size_t sourceLength = strlen(kernelSource);

    cl_program program;
    program = clCreateProgramWithSource(context, 1, &kernelSource, &sourceLength, &err);
    checkError(err);

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
        printBuildLog(program, device);

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

    // Parse arguments
    std::map<std::string, std::string> params = parseArgs(argc, argv);

    // Get arguments
    WIDTH = getWidth(params);
    PLATFORM_ID = getPlatformId(params);

    int X = 1024, Y = 128, Z = 128;

    M = matrixInit(M, X * Y, true);
    N = matrixInit(N, Y * Z, true);
    P = matrixInit(P, X * Z, false);

    initOpenCL(PLATFORM_ID);
    createKernel();

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