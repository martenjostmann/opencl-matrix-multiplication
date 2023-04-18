#include <iostream>
#include "../Utils/matrix_init.h"

void checkSolution(float *f, int width) {
    int sum = 0;
    int size = width*width;
    for (int i = 0; i < size; i++) {
        sum += f[i];
    }

    std::cout << sum << std::endl;
}

void fill(float *f, int size) {
    srand(2);

    for (int i = 0; i < size; i++)
    {
        f[i] = (float) (rand() % 100);
    }
}


float *matrixInit(float *matrix, int width, bool fill_matrix) {
    matrix = new float[width*width];

    if (fill_matrix) {
        fill(matrix, width*width);
    }

    return matrix;
}