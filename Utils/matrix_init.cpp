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
    int sum = 0;
    ;
    for (int i = 0; i < size; i++)
    {
        sum += f[i];
    }

    std::cout << sum << std::endl;
}

void fill(float *f, int size)
{
    srand(2);

    for (int i = 0; i < size; i++)
    {
        f[i] = (float)(rand() % 100);
    }
}

float *matrixInit(float *matrix, int size, bool fill_matrix)
{
    matrix = new float[size];

    if (fill_matrix)
    {
        fill(matrix, size);
    }

    return matrix;
}