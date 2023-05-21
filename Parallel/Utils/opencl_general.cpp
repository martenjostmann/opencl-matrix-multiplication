#include <iostream>
#include <cstring>
#include "opencl_general.h"
#include "../../Utils/general.h"

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

char *readKernel(const char *filename, long *size)
{
    FILE *fp;
    char *source_str;
    size_t source_size, program_size;

    fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Failed to load kernel\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    program_size = ftell(fp);
    rewind(fp);

    source_str = (char *)malloc(program_size + 1);
    for (int i = 0; i < program_size + 1; i++)
    {
        source_str[i] = '\0';
    }

    fread(source_str, sizeof(char), program_size, fp);
    fclose(fp);

    *size = (program_size + 1);
    return source_str;
}

void initOpenCL(int platform_id, cl_device_type device_type)
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
    err = clGetDeviceIDs(platform, device_type, 2, &device, NULL);
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

    err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
    checkError(err);

    build_log = (char *)malloc(build_log_size);

    err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
    checkError(err);

    printf("Log:\n%s\n", build_log);

    free(build_log);
}

void createKernel(const char *kernel_path, const char *header_path)
{
    cl_int err;

    long kernel_size, size_header;
    char *header = NULL;

    char *source = readKernel(kernel_path, &kernel_size);

    if (header_path != NULL)
    {
        header = readKernel(header_path, &size_header);
    }
    else
    {
        size_header = 0;
        header = new char[1];
        header[0] = '\0';
    }

    long size = 2 + kernel_size + size_header;
    char *code = (char *)malloc(size * sizeof(char));
    for (int c = 0; c < size; c++)
    {
        code[c] = '\0';
    }
    strcat(code, header);
    strcat(code, source);
    const char *constCode = code;
    free(header);
    free(source);
    cl_program program;
    program = clCreateProgramWithSource(context, 1, &constCode, NULL, &err);
    checkError(err);

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
        printBuildLog(program, device);

    kernel = clCreateKernel(program, "matrixMultiplicationKernel", &err);
    checkError(err);
}

void matrixMultiplication(float *M, float *N, float *P, int X, int Y, int Z, size_t globalSize[], size_t localSize[])
{
    cl_int err;
    int size_M = X * Y * sizeof(float);
    int size_N = Y * Z * sizeof(float);
    int size_P = X * Z * sizeof(float);

    cl_mem Ad = clCreateBuffer(context, CL_MEM_READ_ONLY, size_M, NULL, &err);
    checkError(err);
    cl_mem Bd = clCreateBuffer(context, CL_MEM_READ_ONLY, size_N, NULL, &err);
    checkError(err);

    err = clEnqueueWriteBuffer(commandQueue, Ad, CL_FALSE, 0, size_M, M, 0, NULL, NULL);
    checkError(err);
    err = clEnqueueWriteBuffer(commandQueue, Bd, CL_FALSE, 0, size_N, N, 0, NULL, NULL);
    checkError(err);

    cl_mem Cd = clCreateBuffer(context, CL_MEM_READ_WRITE, size_P, NULL, &err);
    checkError(err);

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &Ad);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &Bd);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &Cd);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &X);
    err |= clSetKernelArg(kernel, 4, sizeof(int), &Y);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &Z);
    checkError(err);

    err = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
    checkError(err);

    err = clEnqueueReadBuffer(commandQueue, Cd, CL_TRUE, 0, size_P, P, 0, NULL, NULL);
    checkError(err);
}