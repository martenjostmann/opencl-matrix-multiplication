__kernel void matrixMultiplicationKernel(__global float *Md, __global float *Nd,
                                         __global float *Pd, int X, int Y,
                                         int Z) {

  // Initialize the local memory tiles
  __local float Ml[TILE_SIZE][TILE_SIZE];
  __local float Nl[TILE_SIZE][TILE_SIZE];

  // Get the row and column of Pd element to be calculated
  int col = get_global_id(0);
  int row = get_global_id(1);
  int l_col = get_local_id(0);
  int l_row = get_local_id(1);

  // Intialize private memory to store thread work
  float thread_work[THREAD_WORK_SIZE];
  for (int i = 0; i < THREAD_WORK_SIZE; i++) {
    thread_work[i] = 0.0f;
  }

  float sum = 0;

  // Iterate over each tile
  for (int k = 0; k < (Y / TILE_SIZE); k++) {

    // Load THREAD_WORK_SIZE elements into local memory
    for (int l = 0; l < THREAD_WORK_SIZE; l++) {
      Ml[l_row][l_col * THREAD_WORK_SIZE + l] =
          Md[row * Y + (k * TILE_SIZE + l_col * THREAD_WORK_SIZE + l)];
      Nl[l_row][l_col * THREAD_WORK_SIZE + l] =
          Nd[(k * TILE_SIZE + l_row) * Z + col * THREAD_WORK_SIZE + l];
    }

    // Synchronize to make sure the matrices are loaded
    barrier(CLK_LOCAL_MEM_FENCE);

    // Multiply the two matrices together;
    for (int i = 0; i < TILE_SIZE; i++) {
      for (int j = 0; j < THREAD_WORK_SIZE; j++) {
        thread_work[j] += Ml[l_row][i] * Nl[i][l_col * THREAD_WORK_SIZE + j];
      }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  for (int i = 0; i < THREAD_WORK_SIZE; i++) {
    Pd[row * Z + col * THREAD_WORK_SIZE + i] = thread_work[i];
  }
}