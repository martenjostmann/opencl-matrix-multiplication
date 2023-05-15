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
#include "../Utils/opencl_general.h"
#include <CL/cl.h>

int main(int argc, char **argv)
{
    float *M;
    float *N;
    float *P;
    std::tuple<int, int, int> XYZ;
    int PLATFORM_ID;
    const char *KERNEL_PATH, *HEADER_PATH;
    cl_device_type DEVICE_TYPE;

    // Parse arguments
    std::map<std::string, std::string> params = parseArgs(argc, argv);

    // Get arguments
    XYZ = getXYZ(params);
    PLATFORM_ID = getPlatformId(params);
    KERNEL_PATH = getKernelPath(params);
    HEADER_PATH = getHeaderPath(params);
    DEVICE_TYPE = getDeviceType(params);

    int X = std::get<0>(XYZ), Y = std::get<1>(XYZ), Z = std::get<2>(XYZ);

    M = matrixInit(M, X * Y, true, 2);
    N = matrixInit(N, Y * Z, true, 3);
    P = matrixInit(P, X * Z, false);

    initOpenCL(PLATFORM_ID, DEVICE_TYPE);
    createKernel(KERNEL_PATH, HEADER_PATH);

    size_t globalSize[] = {Z, X};
    size_t localSize[] = {32, 32};

    // Start time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    matrixMultiplication(M, N, P, X, Y, Z, globalSize, localSize);

    // End time
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // checkSolution(P, X * Z);

    // Free resources
    delete[] M;
    delete[] N;
    delete[] P;

    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
}

// g++ main.cpp ../../Utils/matrix_init.cpp -L"C:\Users\marte\vcpkg\packages\opencl_x64-windows\lib" -lOpenCL -I"C:\Users\marte\vcpkg\packages\opencl_x64-windows\include" -o main