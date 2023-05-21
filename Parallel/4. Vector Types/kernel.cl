__kernel void matrixMultiplicationKernel(__global float2 *Ad,
                                         __global float2 *Bd,
                                         __global float2 *Cd, int X, int Y,
                                         int Z) {

  // Initialize the local memory tiles
  __local float2 Al[TILE_SIZE][TILE_SIZE / VECTOR_SIZE];
  __local float2 Bl[TILE_SIZE][TILE_SIZE / VECTOR_SIZE];

  // Get the row and column of Cd element to be calculated
  int col = get_global_id(0);
  int row = get_global_id(1);
  int l_col = get_local_id(0);
  int l_row = get_local_id(1);

  // Initliaze thread work vector
  float2 thread_work = {0.0f, 0.0f};

  float sum = 0;

  // Iterate over each tile
  for (int k = 0; k < (Y / TILE_SIZE); k++) {

    Al[l_row][l_col] =
        Ad[row * (Y / VECTOR_SIZE) + (k * (TILE_SIZE / VECTOR_SIZE) + l_col)];
    Bl[l_row][l_col] = Bd[(k * TILE_SIZE + l_row) * (Z / VECTOR_SIZE) + col];

    // Synchronize to make sure the tile is loaded
    barrier(CLK_LOCAL_MEM_FENCE);

    float2 vecA, vecB;
    float valA;

    // Iterate over the columns
    for (int i = 0; i < TILE_SIZE / VECTOR_SIZE; i++) {

      vecA = Al[l_row][i];

      // Iterate over the rows
      for (int j = 0; j < VECTOR_SIZE; j++) {

        // Get correct vector of Bl
        vecB = Bl[VECTOR_SIZE * i + j][l_col];

        // Switch is used to get correct value from vector A depending on which
        // thread_work element is calculated
        switch (j) {
        case 0:
          valA = vecA.x;
          break;
        case 1:
          valA = vecA.y;
          break;
        }

        thread_work += valA * vecB;
      }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  Cd[row * (Z / VECTOR_SIZE) + col] = thread_work;
}