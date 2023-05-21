__kernel void matrixMultiplicationKernel(__global float *Ad, __global float *Bd,
                                         __global float *Cd, int X, int Y,
                                         int Z) {

  // Initialize the local memory tiles
  __local float Al[TILE_SIZE][TILE_SIZE];
  __local float Bl[TILE_SIZE][TILE_SIZE];

  // Get the row and column of Cd element to be calculated
  int col = get_global_id(0);
  int row = get_global_id(1);
  int l_col = get_local_id(0);
  int l_row = get_local_id(1);

  float sum = 0;

  // Iterate over each tile
  for (int k = 0; k < (Y / TILE_SIZE); k++) {
    Al[l_row][l_col] = Ad[row * Y + (k * TILE_SIZE + l_col)];
    Bl[l_row][l_col] = Bd[(k * TILE_SIZE + l_row) * Z + col];

    // Synchronize to make sure the matrices are loaded
    barrier(CLK_LOCAL_MEM_FENCE);

    // Calculate partial results
    for (int i = 0; i < TILE_SIZE; i++) {
      sum += Al[l_row][i] * Bl[i][l_col];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  Cd[row * Z + col] = sum;
}