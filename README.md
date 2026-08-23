🔴 ###Project: DPCTL Python Binding of Lattice Boltzmann Simulations

🧠 **About the Project**

        This project explores how a high-performance Lattice Boltzmann Method
        (LBM) implementation written in C++/SYCL can be exposed and controlled
        from Python without moving the computationally expensive numerical work
        into Python.

        The project is based on the original miniLB implementation and extends it
        with a Python-facing execution path using dpctl, pybind11 and USM shared
        memory.

        Python is responsible for high-level orchestration, including device
        selection, SYCL queue creation, memory allocation, benchmark configuration,
        validation and result collection.

        The repeated numerical computation remains inside compiled C++/SYCL code,
        where the complete LBM simulation loop is executed natively.

✨ **\*\*Implemented Features\*\***

        - Python-controlled execution of a C++/SYCL Lattice Boltzmann simulation

        - Dynamic device and backend selection through dpctl

        - SYCL queue creation from Python

        - USM shared memory allocation

        - Direct Python-to-C++ memory interoperability

        - pybind11 bindings between Python and C++/SYCL

        - D2Q9 Lattice Boltzmann model

        - Lid-Driven Cavity simulation

        - Collision computation

        - Streaming of all D2Q9 populations

        - Bounce-back boundary conditions

        - Moving lid boundary condition

        - Native 5000-step simulation loop

        - CPU/OpenCL execution

        - NVIDIA CUDA execution through the SYCL backend

        - Correctness validation

        - Performance benchmarking using MLUPS

        - Benchmark logging and result collection

🏗️ **\*\*Project Architecture\*\***

        The project separates high-level Python control from performance-critical
        numerical execution:

        Python
            ↓
        Device Selection and Benchmark Configuration
            ↓
        dpctl
            ↓
        SYCL Queue + USM Shared Memory
            ↓
        pybind11 Extension
            ↓
        C++ / SYCL
            ↓
        D2Q9 LBM Simulation
            ↓
        Collision + Streaming + Boundary Conditions
            ↓
        Native 5000-Step Simulation Loop
            ↓
        Validation + Timing + MLUPS Results

        This architecture allows Python to provide a flexible experimentation
        interface while the computationally intensive simulation remains in
        compiled C++/SYCL code.

⚙️ **\*\*Programming Languages and Technologies Used\*\***

        - **Python** → High-level orchestration, device selection, benchmarking
          and result collection

        - **C++** → Native implementation and performance-critical computation

        - **SYCL** → Heterogeneous parallel programming for CPU and GPU backends

        - **dpctl** → Python access to SYCL devices, queues and USM memory

        - **pybind11** → Python and C++ interoperability

        - **USM Shared Memory** → Direct memory interoperability between Python
          and the C++/SYCL implementation

        - **oneAPI / DPC++** → SYCL compiler and development environment

        - **CMake** → Build configuration

        - **OpenCL** → CPU backend used for performance comparison

        - **CUDA through SYCL** → NVIDIA GPU backend

        - **NumPy** → Python-side numerical array support

        - **CMake / Build Scripts** → Compilation and project configuration

🔬 **\*\*Lattice Boltzmann Model\*\***

        The project implements a two-dimensional D2Q9 Lattice Boltzmann model.

        Each lattice cell stores nine distribution populations corresponding to:

        - One rest direction
        - Four cardinal directions
        - Four diagonal directions

        Each simulation step performs the following operations:

        Compute Macroscopic Fields
                ↓
        Calculate Density and Velocity
                ↓
        Collision Step
                ↓
        Relax Populations Towards Equilibrium
                ↓
        Streaming Step
                ↓
        Move D2Q9 Populations to Neighboring Cells
                ↓
        Boundary Treatment
                ↓
        Bounce-Back Walls + Moving Lid
                ↓
        Update Arrays
                ↓
        Continue to the Next Time Step

🌊 **\*\*Lid-Driven Cavity Simulation\*\***

        The implemented use case is the Lid-Driven Cavity problem.

        The cavity walls use bounce-back boundary conditions, while the top wall
        moves horizontally with a prescribed velocity.

        This scenario is useful because it requires both the regular Lattice
        Boltzmann update and explicit boundary handling.

        It therefore provides a more meaningful validation scenario than a
        simulation using only periodic boundaries.

🔗 **\*\*Python and C++/SYCL Interoperability\*\***

        The main design principle of the project is to keep Python outside the
        numerical inner loop.

        Python is responsible for:

        - Selecting the target device
        - Creating the SYCL queue
        - Allocating USM shared memory
        - Preparing benchmark inputs
        - Calling the native extension
        - Collecting validation results
        - Recording timing and MLUPS values

        The C++/SYCL implementation is responsible for:

        - D2Q9 collision computation
        - Streaming
        - Boundary conditions
        - Native simulation execution
        - The complete 5000-step time loop

        Instead of calling a single LBM step from Python 5000 times, Python
        invokes the native extension once for each benchmark run.

        The complete simulation loop then executes inside compiled C++/SYCL code.

        This avoids repeated Python-to-C++ overhead during every simulation step.

🧠 **\*\*Memory Model\*\***

        The simulation arrays are allocated in Python using dpctl USM shared memory.

        These arrays include:

        - Distribution arrays
        - Streaming buffers
        - Density values
        - Velocity components
        - Validation data

        The memory is passed to the C++/SYCL extension through the
        __sycl_usm_array_interface__.

        This allows the native implementation to access Python-allocated USM
        memory directly without maintaining a separate Python-side copy of the
        simulation data.

🧪 **\*\*Correctness and Validation\*\***

        Performance results are reported only after the simulation passes the
        required validation checks.

        The validation process verifies:

        - All D2Q9 populations participate in the collision step

        - All D2Q9 populations are included in the streaming step

        - Bounce-back wall handling is active

        - The Lid-Driven Cavity moving boundary is active

        - Simulation arrays are allocated using dpctl USM

        - The numerical benchmark loop executes in native C++/SYCL code

        - The final simulation state passes the required validation checks

📊 **\*\*Performance Evaluation\*\***

        The project compares the original miniLB C++/SYCL implementation with
        the final V5 Python-DPCTL-SYCL execution path.

        All reported benchmark measurements use:

        - Single precision

        - 5000 simulation steps per measurement

        - MLUPS as the performance metric

        MLUPS means:

                Million Lattice Updates Per Second

        It measures how many lattice cells are updated every second during
        the simulation.

💻 **\*\*CPU / OpenCL Results\*\***

        The original miniLB implementation was compared with the V5
        Python-DPCTL-SYCL implementation on the OpenCL CPU backend.

        | Grid Size | Original miniLB | V5 Python-DPCTL | V5 / Original |
        |-----------|-----------------|-----------------|---------------|
        | 128 × 128 | 44.91 MLUPS     | 22.12 MLUPS     | 49.3%         |
        | 256 × 256 | 100.70 MLUPS    | 47.16 MLUPS     | 46.8%         |
        | 384 × 384 | 141.49 MLUPS    | 53.52 MLUPS     | 37.8%         |
        | 512 × 512 | 159.71 MLUPS    | 58.39 MLUPS     | 36.6%         |
        | 768 × 768 | 128.29 MLUPS    | 49.25 MLUPS     | 38.4%         |
        | 1024 × 1024 | 129.43 MLUPS  | 57.97 MLUPS     | 44.8%         |

        The original specialized miniLB implementation remains faster on the
        CPU/OpenCL backend.

        The purpose of the Python-DPCTL implementation is not to outperform
        the original specialized baseline, but to provide a flexible Python
        control layer while preserving native C++/SYCL numerical execution.

🚀 **\*\*CUDA GPU Results\*\***

        The final V5 implementation was also executed on an:

                NVIDIA Tesla V100S-PCIE-32GB

        using the CUDA SYCL backend.

        | Grid Size | Time | Performance |
        |-----------|------|-------------|
        | 128 × 128 | 5.742 s | 14.27 MLUPS |
        | 256 × 256 | 6.219 s | 52.69 MLUPS |
        | 384 × 384 | 10.797 s | 68.28 MLUPS |
        | 512 × 512 | 15.877 s | 82.55 MLUPS |
        | 768 × 768 | 31.792 s | 92.76 MLUPS |
        | 1024 × 1024 | 51.112 s | 102.58 MLUPS |
        | 1280 × 1280 | 76.569 s | 106.99 MLUPS |
        | 1536 × 1536 | 103.942 s | 113.49 MLUPS |
        | 1792 × 1792 | 142.169 s | 112.94 MLUPS |
        | 2048 × 2048 | 179.256 s | 116.99 MLUPS |

        The GPU results improve as the computational workload becomes larger.

        The final implementation reached:

                **116.99 MLUPS**

        for the 2048 × 2048 lattice on the NVIDIA Tesla V100S.

⚠️ **\*\*CUDA Baseline Limitation\*\***

        The original miniLB implementation was also tested as a possible CUDA
        GPU baseline.

        Although it could be rebuilt for the CUDA SYCL target, execution failed
        in the tested environment with:

                CUDA_ERROR_ILLEGAL_ADDRESS

                return_code = -6

                MLUPS = None

        Because the original CUDA baseline did not complete successfully, no
        valid original CUDA performance value is reported.

        The final V5 implementation was successfully executed on the same GPU
        and passed the required correctness validation checks.

🛠️ **\*\*Requirements to Run the Project\*\***

        Before building or running the project, make sure the following tools
        and libraries are available:

        - Linux environment

        - Python 3

        - Intel oneAPI / DPC++ compiler

        - icpx compiler

        - CMake

        - dpctl

        - pybind11

        - NumPy

        - A compatible SYCL backend

        For GPU execution:

        - NVIDIA GPU

        - CUDA-compatible SYCL backend

        The project was tested with CPU/OpenCL and NVIDIA CUDA through SYCL.

🔍 **\*\*Verify the Environment\*\***

        Check the compiler:

                icpx --version

        Check Python:

                python3 --version

        Check CMake:

                cmake --version

        Check dpctl:

                python3 -c "import dpctl; print(dpctl.__version__)"

        Check pybind11:

                python3 -c "import pybind11; print(pybind11.__version__)"

        Check NumPy:

                python3 -c "import numpy; print(numpy.__version__)"

⚙️ **\*\*Enable the oneAPI Environment\*\***

        If oneAPI is installed in the default location, activate the environment:

                source /opt/intel/oneapi/setvars.sh

        After loading the environment, verify that the SYCL compiler is available:

                which icpx

                icpx --version

📥 **\*\*Build the Original miniLB CPU / OpenCL Version\*\***

        From the miniLB project directory:

                cmake -S . -B our_builds/build_single \
                  -DCMAKE_BUILD_TYPE=Release \
                  -DSYCL_IMPL=dpcpp \
                  -DCMAKE_CXX_COMPILER=icpx \
                  -DBGK_USE_CASE=LDC \
                  -DBGK_PRECISION=SINGLE \
                  -DBGK_SYCL_MALLOC_SHARED=ON

        After configuration, build the project using the generated build setup.

▶️ **\*\*Run the CPU / OpenCL Version\*\***

        Select the OpenCL CPU backend:

                ONEAPI_DEVICE_SELECTOR=opencl:cpu ./bgk2dSYCL

        The exact executable location may depend on the build configuration.

🐍 **\*\*Run the Python-DPCTL Benchmark\*\***

        Select the OpenCL CPU backend and run the Python benchmark:

                MINILB_DEVICE_SELECTOR="opencl:cpu:0" python3 benchmark_v5_pybind11.py

        The benchmark will:

        - Select the device

        - Create the SYCL queue

        - Allocate USM memory

        - Invoke the pybind11 extension

        - Execute the complete 5000-step simulation loop in C++/SYCL

        - Validate the final state

        - Report execution time and MLUPS

🚀 **\*\*Run the CUDA GPU Benchmark\*\***

        Select the CUDA GPU backend:

                MINILB_DEVICE_SELECTOR="cuda:gpu:0" python3 benchmark_v5_pybind11_gpu_6cases.py

        For the NVIDIA Tesla V100S experiment, the native extension was built
        for the CUDA SYCL target using:

                -fsycl-targets=nvptx64-nvidia-cuda

                --cuda-gpu-arch=sm_70

📁 **\*\*Repository Structure\*\***

        The repository contains the original miniLB implementation together with
        the Python-DPCTL binding work, benchmark scripts and supporting material.

        Important directories and files include:

                miniLB/
                    Original miniLB C++/SYCL implementation
                    Lattice Boltzmann solver
                    Build configuration
                    Native simulation code

                fisiere_proiect_ajutatoare/
                    Python-DPCTL binding work
                    Benchmark scripts
                    Supporting project material
                    Experimental files

                .vscode/
                    Visual Studio Code configuration

                Pasi rulare.txt
                    Original project execution notes

                README.md
                    Project documentation

📄 **\*\*Official Project Paper\*\***

        The complete technical report for this project describes:

        - The project motivation

        - The D2Q9 Lattice Boltzmann model

        - The Python-DPCTL architecture

        - pybind11 interoperability

        - USM shared memory

        - CPU/OpenCL experiments

        - CUDA GPU experiments

        - Correctness validation

        - Performance analysis and conclusions

        The official paper will be available in the repository documentation.

        Suggested location:

                docs/
                    DPCTL_with_Python_Bindings_of_Lattice_Boltzmann.pdf

        After uploading the paper, add a direct link here:

        [📥 Read the Official Project Paper](docs/DPCTL_with_Python_Bindings_of_Lattice_Boltzmann.pdf)

🎯 **\*\*What This Project Demonstrates\*\***

        This project demonstrates practical experience with:

        - High-Performance Computing concepts

        - Scientific computing

        - Python and C++ interoperability

        - pybind11 bindings

        - SYCL heterogeneous programming

        - dpctl device and queue management

        - USM shared memory

        - CPU and GPU backend execution

        - Lattice Boltzmann simulations

        - D2Q9 numerical models

        - Parallel computing

        - Device selection

        - Performance benchmarking

        - MLUPS performance evaluation

        - Correctness validation

        - CMake build configuration

        - Backend-specific compilation

        - NVIDIA GPU execution through CUDA SYCL

🔮 **\*\*Possible Future Improvements\*\***

        Potential future improvements include:

        - Further GPU kernel optimization

        - Memory layout optimization

        - Profiling of Python-DPCTL overhead

        - Automated parameter sweeps

        - Additional SYCL backend testing

        - Expanded benchmark automation

        - Additional fluid simulation use cases

        - More automated correctness tests

        - Visualization and post-processing tools

        - Support for additional accelerator architectures

👨‍💻 **\*\*Authors\*\***

        - Roman Andrei

        - Bianca-Maria Andreica

        - Ariana Crista

📄 **\*\*License\*\***

        This project is intended for educational, research and portfolio purposes.

        Please refer to the LICENSE file for the full license terms.
