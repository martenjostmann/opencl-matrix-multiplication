__kernel void matrixMultiplicationKernel(__global float *Ad, __global float *Bd,
                                         __global float *Cd, int X, int Y,
                                         int Z) {

  // Get the row and column of Cd element to be calculated
  int col = get_global_id(0);
  int row = get_global_id(1);

  float sum = 0;
  for (int k = 0; k < Y; k += 1)
    sum += Ad[row * Y + k] * Bd[k * Z + col];
  Cd[row * Z + col] = sum;
}