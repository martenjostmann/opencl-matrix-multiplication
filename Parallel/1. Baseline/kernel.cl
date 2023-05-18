__kernel void matrixMultiplicationKernel(__global float *Md, __global float *Nd,
                                         __global float *Pd, int X, int Y,
                                         int Z) {

  // Get the row and column of Pd element to be calculated
  int col = get_global_id(0);
  int row = get_global_id(1);

  float sum = 0;
  for (int k = 0; k < Y; k += 1)
    sum += Md[row * Y + k] * Nd[k * Z + col];
  Pd[row * Z + col] = sum;
}