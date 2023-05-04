#include <iostream>
#include <chrono>
#include "../Utils/matrix_init.h"

#define WIDTH 1024

void matrixMultiplication(float *M, float *N, float *P, int width)
{

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float sum = 0;
            for (int k = 0; k < width; k++)
            {
                sum += M[i * width + k] * N[k * width + j];
            }
            P[i * width + j] = sum;
        }
    }
}

int main()
{
    float *M;
    float *N;
    float *P;

    M = matrixInit(M, WIDTH, true);
    N = matrixInit(N, WIDTH, true);
    P = matrixInit(N, WIDTH, false);

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

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
}