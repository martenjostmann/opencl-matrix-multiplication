#include <iostream>
#include "../Utils/matrix_init.h"

void fill(float* f, int size) {
    srand(time(NULL));

    for (int i = 0; i < size; i++)
    {
        f[i] = (float) (rand() % 100);
    }
}


float *matrixInit(float * matrix, int width, bool fill_matrix) {
    matrix = new float[width*width];

    if (fill_matrix) {
        fill(matrix, width*width);
    }

    return matrix;
}