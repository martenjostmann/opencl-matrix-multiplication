#include <iostream>
#include <chrono>
#include "../Utils/matrix_init.h"
#include <CL/cl.h>

#define WIDTH 8000

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

void initOpenCL()
{
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

void createKernel()
{
    cl_int err;

    const char *kernelSource = "__kernel \
        void matrixMultiplicationKernel(__global float* output, \
                                        __global float* input, \
                                        int offset) { \
            int index = get_global_id(0) + offset;\
            output[index] = input[index];\
        }";

    size_t sourceLength = strlen(kernelSource);

    cl_program program;
    program = clCreateProgramWithSource(context, 1, &kernelSource, &sourceLength, &err);
    checkError(err);

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    kernel = clCreateKernel(program, "matrixMultiplicationKernel", &err);
    checkError(err);
}

void matrixMultiplication(float *Input, float *Output, int offset, int width)
{
    cl_int err;
    int size = width * width * sizeof(float);

    cl_mem InputD = clCreateBuffer(context, CL_MEM_READ_ONLY, size, NULL, &err);
    checkError(err);

    err = clEnqueueWriteBuffer(commandQueue, InputD, CL_FALSE, 0, size, Input, 0, NULL, NULL);
    checkError(err);

    cl_mem OutputD = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, &err);
    checkError(err);

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &OutputD);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &InputD);
    err |= clSetKernelArg(kernel, 2, sizeof(int), &offset);
    checkError(err);

    size_t globalSize = width * width - offset;
    size_t localSize = 16;

    err = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);
    checkError(err);

    err = clEnqueueReadBuffer(commandQueue, OutputD, CL_TRUE, 0, size, Output, 0, NULL, NULL);
    checkError(err);
}

int main()
{
    float *Input;
    float *Output;
    int offset = 1;

    Input = matrixInit(Input, WIDTH, true);
    Output = matrixInit(Output, WIDTH, false);

    initOpenCL();
    createKernel();

    // Start time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    matrixMultiplication(Input, Output, offset, WIDTH);

    // End time
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // Free resources
    delete[] Input;
    delete[] Output;

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}

// g++ main.cpp ../../Utils/matrix_init.cpp -L"C:\Users\marte\vcpkg\packages\opencl_x64-windows\lib" -lOpenCL -I"C:\Users\marte\vcpkg\packages\opencl_x64-windows\include" -o main