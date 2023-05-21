# Matrix Multiplication with OpenCL

## Execution instruction

Every optimization resides in a separate folder. In each folder there is Makefile that can be used to compile the specific optimization. When the the library and include path cannot be found on the PATH the location can be specified with `INCLUDE_PATH` and `LIBRARY_PATH` as arguments in the Makefile:

```
make INCLUDE_PATH=<path/to/include> LIBRARY_PATH=<path/to/lib>
```

## Arguments

Every matrix multiplication algorithm can be started with various command line arguments:

```
-x -> Number of rows of Matrix A (default 1024)
-y -> Number of columns of Matrix A / rows of Matrix B (default 1024)
-z -> Number of columns of Matrix C (default 1024)
```

For the matrix multiplication with the OpenCL interface additional argument can be defined:

```
-p -> OpenCL platform id (default 0)
-d -> OpenCL device type CL_DEVICE_TYPE_GPU or CL_DEVICE_TYPE_CPU (default CL_DEVICE_TYPE_GPU)
-k -> Path to the kernel file (default kernel.cl)
```

In some optimization folders a `properties.h` file is available to control specific parameters beyond the command line arguments

## Project Structure

```
│ README.md
│ run-seq.sh
| run.sh
│ Dockerfile
│
└───Parallel                -> Code for parallel matrix multiplication
|   |
|   └───1. Baseline         -> OpenCL Baseline implementation
|   |   |   kernel.cl
|   |   |   main.cpp
|   |   |   Makefile
|   |
|   └───2. Shared Memory    -> Shared Memory Optimization
|   |   |   kernel.cl
|   |   |   main.cpp
|   |   |   Makefile
|   |   |   properties.h
|   |
|   └───3. Enhanced Work    -> Enhanced Work Optimization
|   |   |   kernel.cl
|   |   |   main.cpp
|   |   |   Makefile
|   |   |   properties.h
|   |
|   └───4. Vector           -> Vector Optimization
|   |   |   kernel.cl
|   |   |   main.cpp
|   |   |   Makefile
|   |   |   properties.h
|
|
└───Sequential              -> Code for sequential matrix multiplication
│   │   main.cpp            -> General functions
│   │   Makefile
|
|
└───Utils                   -> General functions for all implementations
│   │   general.cpp         -> General functions
│   │   general.h
|   |   matrix_init.cpp     -> Initialize matrixes
|   |   matrix_init.h
```
