//***************************************************************************
//  File:   matrix_init.cpp
//  Author: Marten Jostmann
//
//  This file implements functions to initialize matrices and check the solution.
//
//***************************************************************************

#include <iostream>
#include "../Utils/matrix_init.h"

/**
 * Fill the matrix with random float values.
 *
 * @param *f pointer to the matrix to be filled
 * @param size size of the matrix
 * @param seed seed for the random number generator
 */
void fill(float *f, int size, int seed)
{
    srand(seed);

    for (int i = 0; i < size; i++)
    {
        f[i] = (float(rand()) / float((RAND_MAX)));
    }
}

/**
 * Initialize a matrix
 *
 * @param *f pointer to the matrix to be initialized
 * @param size size of the matrix
 * @param fill_matrix if true, the matrix will be filled with random values
 * @param seed seed for the random number generator
 * @return pointer to the initialized matrix
 */
float *matrixInit(float *matrix, int size, bool fill_matrix, int seed)
{
    matrix = new float[size];

    if (fill_matrix)
    {
        fill(matrix, size, seed);
    }

    return matrix;
}