//***************************************************************************
//  File:   main.cpp
//  Author: Marten Jostmann
//
//  This file implements a matrix multiplication using shared memory and
//  enhanced work per work item including vectors.
//
//***************************************************************************

#include <iostream>
#include <chrono>
#include "../../Utils/matrix_init.h"
#include "../../Utils/general.h"
#include "../Utils/opencl_general.h"
#include "../Utils/opencl_params.h"
#include <CL/cl.h>
#include "properties.h"

int main(int argc, char **argv)
{
    float *A;
    float *B;
    float *C;
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

    A = matrixInit(A, X * Y, true, 2);
    B = matrixInit(B, Y * Z, true, 3);
    C = matrixInit(C, X * Z, false);

    initOpenCL(PLATFORM_ID, DEVICE_TYPE);
    createKernel(KERNEL_PATH, HEADER_PATH);

    size_t globalSize[] = {Z / VECTOR_SIZE, X};
    size_t localSize[] = {TILE_SIZE / VECTOR_SIZE, TILE_SIZE};

    // Start time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    matrixMultiplication(A, B, C, X, Y, Z, globalSize, localSize);

    // End time
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    checkSolution(C, X * Z);

    // Free resources
    delete[] A;
    delete[] B;
    delete[] C;

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}
