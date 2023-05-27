//***************************************************************************
//  File:   main.cpp
//  Author: Marten Jostmann
//
//  This file implements the sequential matrix multiplication.
//
//***************************************************************************

#include <iostream>
#include <chrono>
#include "../Utils/matrix_init.h"
#include "../Utils/general.h"

#define WIDTH 1024

/**
 * Sequential matrix multiplication.
 *
 * @param *A pointer to the first matrix
 * @param *B pointer to the second matrix
 * @param *C pointer to the resulting matrix
 * @param X number of rows of the first matrix
 * @param Y number of columns of the first matrix and rows of the second matrix
 * @param Z number of columns of the second matrix
 */
void matrixMultiplication(float *A, float *B, float *C, int X, int Y, int Z)
{

    for (int i = 0; i < X; i++)
    {
        for (int j = 0; j < Z; j++)
        {
            float sum = 0;
            for (int k = 0; k < Y; k++)
            {
                sum += A[i * Y + k] * B[k * Z + j];
            }
            C[i * Z + j] = sum;
        }
    }
}

void printMatrix(float *M, int X, int Y)
{
    for (int i = 0; i < X; i++)
    {
        for (int j = 0; j < Y; j++)
        {
            std::cout << M[i * Y + j] << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char **argv)
{
    float *A;
    float *B;
    float *C;
    std::tuple<int, int, int> XYZ;

    // Parse arguments
    std::map<std::string, std::string> params = parseArgs(argc, argv);

    // Get arguments
    XYZ = getXYZ(params);

    int X = std::get<0>(XYZ), Y = std::get<1>(XYZ), Z = std::get<2>(XYZ);

    A = matrixInit(A, X * Y, true, 2);
    B = matrixInit(B, Y * Z, true, 3);
    C = matrixInit(C, X * Z, false);

    // Start time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    matrixMultiplication(A, B, C, X, Y, Z);

    // End time
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    checkSolution(C, X * Z);

    // Free resources
    delete[] A;
    delete[] B;
    delete[] C;

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
}