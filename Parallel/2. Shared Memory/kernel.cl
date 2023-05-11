#include <header.h>

__kernel void matrixMultiplicationKernel(__global float *Md, __global float *Nd,
                                         __global float *Pd, int X, int Y,
                                         int Z) {

  __local float Ml[TILE_SIZE][TILE_SIZE];
  __local float Nl[TILE_SIZE][TILE_SIZE];

  int col = get_global_id(0);
  int row = get_global_id(1);
  int l_col = get_local_id(0);
  int l_row = get_local_id(1);

  float sum = 0;
  for (int k = 0; k < (Y / TILE_SIZE); k++) {
    Ml[l_row][l_col] = Md[row * Y + (k * TILE_SIZE + l_col)];
    Nl[l_row][l_col] = Nd[(k * TILE_SIZE + l_row) * Z + col];
    barrier(CLK_LOCAL_MEM_FENCE);

    for (int i = 0; i < TILE_SIZE; i++) {
      sum += Ml[l_row][i] * Nl[i][l_col];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  Pd[row * Z + col] = sum;
}