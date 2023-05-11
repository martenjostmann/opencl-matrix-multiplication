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

void matrixMultiplication(float *M, float *N, float *P, int X, int Y, int Z)
{

    for (int i = 0; i < X; i++)
    {
        for (int j = 0; j < Z; j++)
        {
            int sum = 0;
            for (int k = 0; k < Y; k++)
            {
                sum += M[i * Y + k] * N[k * Z + j];
            }
            P[i * Z + j] = sum;
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
    float *M;
    float *N;
    float *P;
    std::tuple<int, int, int> XYZ;

    // Parse arguments
    std::map<std::string, std::string> params = parseArgs(argc, argv);

    // Get arguments
    XYZ = getXYZ(params);

    int X = std::get<0>(XYZ), Y = std::get<1>(XYZ), Z = std::get<2>(XYZ);

    M = matrixInit(M, X * Y, true);
    N = matrixInit(N, Y * Z, true);
    P = matrixInit(P, X * Z, false);

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

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
}