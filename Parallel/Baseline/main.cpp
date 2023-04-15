#include <iostream>
#include <chrono>
#include "../../Utils/matrix_init.h"
#include <CL/cl.h>

#define WIDTH 1024

cl_platform_id    platform;
cl_device_id      device;
cl_context        context;
cl_command_queue  commandQueue;
cl_kernel         kernel;

void checkError(cl_int err) {
  if (err != CL_SUCCESS)
    printf("Error with errorcode: %d\n", err);
}

void initOpenCL() {
    cl_int err;

    err = clGetPlatformIDs(1, &platform, NULL);
    checkError(err);

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 2, &device, NULL);
    checkError(err);

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    checkError(err);

    commandQueue = clCreateCommandQueue(context, device, 0, &err);
    checkError(err);
}

void createKernel() {
    cl_int err;

    const char* kernelSource = "__kernel \
        void matrixMultiplicationKernel(__global float* Md, \
                                        __global float* Nd, \
                                        __global float* Pd, \
                                        int width) { \
            int col = get_global_id(0); \
            int row = get_global_id(1); \
            \
            float sum = 0; \
            for (int k = 0; k < width; k+=1) \
                sum += Md[row * width + k] * Nd[k * width + col]; \
            \
            Pd[row * width + col] = sum; \
        }";

    size_t sourceLength = strlen(kernelSource);

    cl_program program;
    program = clCreateProgramWithSource(context, 1, &kernelSource, &sourceLength, &err);
    checkError(err);

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    kernel = clCreateKernel(program, "matrixMultiplicationKernel", &err);
    checkError(err);
}

void matrixMultiplication(float* M, float* N, float* P, int width) {
    cl_int err;
    int size = width * width * sizeof(float);

    cl_mem Md = clCreateBuffer(context, CL_MEM_READ_ONLY, size, NULL, &err);
    checkError(err);
    cl_mem Nd = clCreateBuffer(context, CL_MEM_READ_ONLY, size, NULL, &err);
    checkError(err);

    err = clEnqueueWriteBuffer(commandQueue, Md, CL_FALSE, 0, size, M, 0, NULL, NULL);
    checkError(err);
    err = clEnqueueWriteBuffer(commandQueue, Nd, CL_FALSE, 0, size, N, 0, NULL, NULL);
    checkError(err);

    cl_mem Pd = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, &err);
    checkError(err);

    err  = clSetKernelArg( kernel, 0, sizeof(cl_mem), &Md );
    err |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &Nd );
    err |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &Pd );
    err |= clSetKernelArg( kernel, 3, sizeof(int), &width );
    checkError(err);

    size_t globalSize[] = {width, width};
    size_t localSize[] = {32, 32};

    err = clEnqueueNDRangeKernel( commandQueue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
    checkError(err);

    err = clEnqueueReadBuffer( commandQueue, Pd,  CL_TRUE, 0, size, P, 0, NULL, NULL );
    checkError(err);
}

int main()
{
    float *M;
    float *N;
    float *P;

    M = matrixInit(M, WIDTH, true);
    N = matrixInit(N, WIDTH, true);
    P = matrixInit(N, WIDTH, false);

    initOpenCL();
    createKernel();

    // Start time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    matrixMultiplication(M, N, P, WIDTH);
    
    // End time
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // Free resources
    delete[] M;
    delete[] N;
    delete[] P;

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "[ms]" << std::endl;
}

// g++ main.cpp ../../Utils/matrix_init.cpp -L"C:\Users\marte\vcpkg\packages\opencl_x64-windows\lib" -lOpenCL -I"C:\Users\marte\vcpkg\packages\opencl_x64-windows\include" -o main