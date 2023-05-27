//***************************************************************************
//  File:   opencl_general.cpp
//  Author: Marten Jostmann
//
//  This file includes general functions to mange the OpenCL environment
//  used in the different implementations.
//
//***************************************************************************

#include <iostream>
#include <cstring>
#include "opencl_general.h"
#include "../../Utils/general.h"

cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue commandQueue;
cl_kernel kernel;
cl_program program;

void checkError(cl_int err)
{
    if (err != CL_SUCCESS)
        printf("Error with errorcode: %d\n", err);
}

/**
 * Read the kernel from the file and return it as a string.
 *
 * @param *filename name of the file to be read
 * @param *size pointer to the size of the kernel file
 * @return pointer to the kernel string
 */
char *readKernel(const char *filename, long *size)
{
    FILE *fp;
    char *source_str;
    size_t source_size, program_size;

    // Opens the kernel file
    fp = fopen(filename, "r");

    // Check for open issues
    if (!fp)
    {
        printf("Failed to load kernel\n");
        exit(1);
    }

    // Get the size of the file
    fseek(fp, 0, SEEK_END);
    program_size = ftell(fp);
    rewind(fp);

    // Allocate memory for the kernel
    source_str = (char *)malloc(program_size + 1);

    // Set the last char to '\0'
    for (int i = 0; i < program_size + 1; i++)
    {
        source_str[i] = '\0';
    }

    // Read the kernel into the memory
    fread(source_str, sizeof(char), program_size, fp);
    fclose(fp);

    *size = (program_size + 1);

    return source_str;
}

/**
 * Init the OpenCL environment including the platform, device, context and command queue.
 *
 * @param platform_id id of the platform to be used
 * @param device_type type of the device to be used (CL_DEVICE_TYPE_CPU or CL_DEVICE_TYPE_GPU)
 */
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

/**
 * Print the build log in case of an error
 *
 * @param program the built program were error will be printed from
 * @param device the device used for the program
 */
void printBuildLog(cl_program program, cl_device_id device)
{
    cl_int err;
    char *build_log;
    size_t build_log_size;

    // Get size of build log
    err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
    checkError(err);

    // Allocate memory for build log
    build_log = (char *)malloc(build_log_size);

    // Get build log
    err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
    checkError(err);

    printf("Log:\n%s\n", build_log);

    free(build_log);
}

/**
 * Create and compile kernel code
 *
 * @param *kernel_path path to the kernel code
 * @param *header_path path to the header file
 */
void createKernel(const char *kernel_path, const char *header_path)
{
    cl_int err;

    long kernel_size, size_header;
    char *header = NULL;

    char *source = readKernel(kernel_path, &kernel_size);

    // if heaer path is specified, read it
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

    // Calculate size of kernel code and allocate memory
    long size = 2 + kernel_size + size_header;
    char *code = (char *)malloc(size * sizeof(char));

    // Set all elements to '\0'
    for (int c = 0; c < size; c++)
    {
        code[c] = '\0';
    }

    // Concatenate header and source code
    strcat(code, header);
    strcat(code, source);

    const char *constCode = code;

    free(header);
    free(source);

    // Create program from source code
    program = clCreateProgramWithSource(context, 1, &constCode, NULL, &err);
    checkError(err);

    // Compile program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    // Check for errors
    if (err != CL_SUCCESS)
    {
        printBuildLog(program, device);
    }

    // Create kernel from program
    kernel = clCreateKernel(program, "matrixMultiplicationKernel", &err);
    checkError(err);
}

/**
 * Start matrix multiplication kernel
 *
 * @param *A pointer to the first matrix
 * @param *B pointer to the second matrix
 * @param *C pointer to the resulting matrix
 * @param X number of rows of the first matrix
 * @param Y number of columns of the first matrix and rows of the second matrix
 * @param Z number of columns of the second matrix
 * @param globalSize global size of the kernel
 * @param localSize local size of the kernel (number of work items per work group)
 */
void matrixMultiplication(float *A, float *B, float *C, int X, int Y, int Z, size_t globalSize[], size_t localSize[])
{
    cl_int err;
    int size_A = X * Y * sizeof(float);
    int size_B = Y * Z * sizeof(float);
    int size_C = X * Z * sizeof(float);

    // Create buffers for matrices
    cl_mem Ad = clCreateBuffer(context, CL_MEM_READ_ONLY, size_A, NULL, &err);
    checkError(err);
    cl_mem Bd = clCreateBuffer(context, CL_MEM_READ_ONLY, size_B, NULL, &err);
    checkError(err);

    // Write matrices to buffers
    err = clEnqueueWriteBuffer(commandQueue, Ad, CL_FALSE, 0, size_A, A, 0, NULL, NULL);
    checkError(err);
    err = clEnqueueWriteBuffer(commandQueue, Bd, CL_FALSE, 0, size_B, B, 0, NULL, NULL);
    checkError(err);

    // Create buffer for the result
    cl_mem Cd = clCreateBuffer(context, CL_MEM_READ_WRITE, size_C, NULL, &err);
    checkError(err);

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &Ad);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &Bd);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &Cd);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &X);
    err |= clSetKernelArg(kernel, 4, sizeof(int), &Y);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &Z);
    checkError(err);

    // Start kernel
    err = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
    checkError(err);

    // Read result from buffer
    err = clEnqueueReadBuffer(commandQueue, Cd, CL_TRUE, 0, size_C, C, 0, NULL, NULL);
    checkError(err);
}