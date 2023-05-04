#include <iostream>
#include <chrono>
#include "../../Utils/matrix_init.h"
#include <CL/cl.h>

#define WIDTH 1024
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define TILE_SIZE 32
#define THREAD_WORK_SIZE 2

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
                                            "#define THREAD_WORK_SIZE " STR(THREAD_WORK_SIZE) "\n"
                                                                                              "__kernel \
        void matrixMultiplicationKernel(__global float* Md, \
                                        __global float* Nd, \
                                        __global float* Pd, \
                                        int width) { \
            \
            __local float Ml[TILE_SIZE][TILE_SIZE]; \
            __local float Nl[TILE_SIZE][TILE_SIZE]; \
            \
            int col = get_global_id(0); \
            int row = get_global_id(1); \
            int l_col = get_local_id(0); \
            int l_row = get_local_id(1); \
            \
            int RTS = TILE_SIZE/THREAD_WORK_SIZE; \
            float thread_work[THREAD_WORK_SIZE];\
            for (int i = 0; i < THREAD_WORK_SIZE; i++) {\
                thread_work[i] = 0.0f;\
            }\
            \
            float sum = 0; \
            for (int k = 0; k < (width/TILE_SIZE); k++) { \
                for (int l = 0; l < THREAD_WORK_SIZE; l++) { \
                    Ml[l_row][l_col*THREAD_WORK_SIZE + l] = Md[row * width + (k * TILE_SIZE + l_col*THREAD_WORK_SIZE+l)]; \
                    Nl[l_row][l_col*THREAD_WORK_SIZE + l] = Nd[(k * TILE_SIZE + l_row) * width + col * THREAD_WORK_SIZE + l]; \
                } \
                barrier(CLK_LOCAL_MEM_FENCE); \
                \
                for (int i = 0; i < TILE_SIZE; i++) { \
                    for (int j = 0; j < THREAD_WORK_SIZE; j++) { \
                        thread_work[j] += Ml[l_row][i] * Nl[i][l_col*THREAD_WORK_SIZE+j];\
                    } \
                } \
                barrier(CLK_LOCAL_MEM_FENCE); \
            } \
            \
            for (int i = 0; i < THREAD_WORK_SIZE; i++) { \
                Pd[row * width + col*THREAD_WORK_SIZE + i] = thread_work[i]; \
            }\
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

void matrixMultiplication(float *M, float *N, float *P, int width)
{
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

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &Md);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &Nd);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &Pd);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &width);
    checkError(err);

    size_t globalSize[] = {width / THREAD_WORK_SIZE, width};
    size_t localSize[] = {32 / THREAD_WORK_SIZE, 32};

    err = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
    checkError(err);

    err = clEnqueueReadBuffer(commandQueue, Pd, CL_TRUE, 0, size, P, 0, NULL, NULL);
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

    checkSolution(P, WIDTH);

    // Free resources
    delete[] M;
    delete[] N;
    delete[] P;

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}

// g++ main.cpp ../../Utils/matrix_init.cpp -L"C:\Users\marte\vcpkg\packages\opencl_x64-windows\lib" -lOpenCL -I"C:\Users\marte\vcpkg\packages\opencl_x64-windows\include" -o main