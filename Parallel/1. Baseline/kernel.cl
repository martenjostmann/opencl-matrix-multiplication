__kernel void matrixMultiplicationKernel(__global float* Md,
                                        __global float* Nd,
                                        __global float* Pd,
                                        int width) {
    int col = get_global_id(0);
    int row = get_global_id(1);

    float sum = 0;
    for (int k = 0; k < width; k+=1)
        sum += Md[row * width + k] * Nd[k * width + col];
    Pd[row * width + col] = sum;
}