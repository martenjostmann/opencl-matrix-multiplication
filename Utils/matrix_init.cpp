//***************************************************************************
//  File:   matrix_init.cpp
//  Author: Marten Jostmann
//
//  This file implements functions to initialize matrices and check the solution.
//
//***************************************************************************

#include <iostream>
#include "../Utils/matrix_init.h"

void checkSolution(float *f, int size)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += f[i];
    }

    std::cout << sum << std::endl;
}

void fill(float *f, int size, int seed)
{
    srand(seed);

    for (int i = 0; i < size; i++)
    {
        f[i] = (float(rand()) / float((RAND_MAX)));
    }
}

float *matrixInit(float *matrix, int size, bool fill_matrix, int seed = 2)
{
    matrix = new float[size];

    if (fill_matrix)
    {
        fill(matrix, size, seed);
    }

    return matrix;
}