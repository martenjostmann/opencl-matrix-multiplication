#include <header.h>
__kernel void matrixMultiplicationKernel(__global float2 *Md,
                                         __global float2 *Nd,
                                         __global float2 *Pd, int width) {

  __local float2 Ml[TILE_SIZE][TILE_SIZE / VECTOR_SIZE];
  __local float2 Nl[TILE_SIZE][TILE_SIZE / VECTOR_SIZE];

  int col = get_global_id(0);
  int row = get_global_id(1);
  int l_col = get_local_id(0);
  int l_row = get_local_id(1);

  float2 thread_work = {0.0f, 0.0f};

  float sum = 0;
  for (int k = 0; k < (width / TILE_SIZE); k++) {

    Ml[l_row][l_col] = Md[row * (width / VECTOR_SIZE) +
                          (k * (TILE_SIZE / VECTOR_SIZE) + l_col)];
    Nl[l_row][l_col] =
        Nd[(k * TILE_SIZE + l_row) * (width / VECTOR_SIZE) + col];
    barrier(CLK_LOCAL_MEM_FENCE);

    float2 vecA, vecB;
    float valA;
    for (int i = 0; i < TILE_SIZE / VECTOR_SIZE; i++) {
      vecA = Ml[l_row][i];
      for (int j = 0; j < VECTOR_SIZE; j++) {
        vecB = Nl[VECTOR_SIZE * i + j][l_col];
        switch (j) {
        case 0:
          valA = vecA.x;
          break;
        case 1:
          valA = vecA.y;
          break;
        }
        thread_work.x += valA * vecB.x;
        thread_work.y += valA * vecB.y;
      }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  Pd[row * (width / VECTOR_SIZE) + col] = thread_work;
}