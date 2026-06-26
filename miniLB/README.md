# miniLB

miniLB is 2D Lattice Botlzmann SYCL mini-app, written in SYCL.
The original FORTRAN code can be found under the [fortran](/fortran/) folder.

## Code structure
miniLB is written in C++ and accelerated w/ SYCL. It has been tested on every major GPU vendor (NVIDIA, AMD, and Intel), but it should run on every device supported by a SYCL compiler.
It officially supports the AdaptiveCpp and Intel DPC++ compilers.

## How to build and run 
miniLB uses CMake to generate building files. 
To configure the project:
```bash
    cmake -DCMAKE_BUILD_TYPE=Release
    -DSYCL_IMPL=[AdaptiveCpp|dpcpp] \
    -BGK_USE_CASE=[LDC|POF|VKS|TGV]  \ #Enable use case 
    -DBGK_PRECISION=[SINGLE|DOUBLE|MIXED1|MIXED2] \
    -DBGK_SYCL_MALLOC_SHARED=[ON|OFF] \ # Enable malloc shared 
    -DBGK_SYCL_ENABLE_PREFETCH=[ON|OFF] \ # Enable data prefetching when using shared mem
    -DBGK_SYCL_ORDER_QUEUE=[ON|OFF] \ # Use SYCL in-order queues
    -DBGK_SYCL_ND_RANGE=[ON|OFF] \ # Use SYCL NDRange instead of range
    -DBGK_DEBUG_HOST_QUEUE=[ON|OFF]  # Use SYCL host queue for debugging purpose
```

When using DPC++ as the SYCL compiler, additional flags are requierd:
```bash
-DCMAKE_CXX_COMPILER=path_to_dpcpp_clang_compiler \
-DDPCPP_WITH_[CUDA|ROCM|LZ]_BACKEND=[ON|OFF] \ #Enable the CUDA\HIP\Level Zero backend
-D[CUDA|ROCM|LZ]_ARCH=arch \ # Select target device architecture
```
By default, the DPC++ backend requires a working CPU backend as well, so make sure to install the required Intel OpenCL runtime on your system.

## How to run
To run the simulation, you need a `bgk.input` file, like the following:
```bash
&parameters
lx = 2048 # X size
ly = 2048 # Y size
svisc=0.05 
u0=0.1
itfin=400000 # Simulation steps
ivtim=50000 # Checks timesteps
isignal=1000 
itsave=1000000
icheck=2000
irestart=0
init_v=0
ipad=0        /
```

Copy the `bgk.input` in the same folder of the executable, and then type:
```bash
./bgk2dSYCL 
```