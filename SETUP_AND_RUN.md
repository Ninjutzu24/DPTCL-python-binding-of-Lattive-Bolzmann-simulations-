============================================================
RUN COMMANDS - miniLB V5 FINAL
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"


============================================================
PART B - ACTIVATING oneAPI FOR miniLB 
============================================================

source /opt/intel/oneapi/setvars.sh

export LD_LIBRARY_PATH=/opt/intel/oneapi/umf/1.0/lib:/opt/intel/oneapi/compiler/latest/lib:/opt/intel/oneapi/compiler/latest/lib/x64:/opt/intel/oneapi/tbb/latest/lib:$LD_LIBRARY_PATH
export OCL_ICD_FILENAMES=/etc/OpenCL/vendors/intel64.icd

sycl-ls

============================================================
PART C - BUILD ORIGINAL miniLB / build_single (only if the build_single directory does not already exist)
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

rm -rf our_builds/build_single
mkdir -p our_builds/build_single

cmake -S . -B our_builds/build_single \
  -DCMAKE_BUILD_TYPE=Release \
  -DSYCL_IMPL=dpcpp \
  -DCMAKE_CXX_COMPILER=icpx \
  -DBGK_USE_CASE=LDC \
  -DBGK_PRECISION=SINGLE \
  -DBGK_SYCL_MALLOC_SHARED=ON \
  -DBGK_SYCL_ENABLE_PREFETCH=OFF \
  -DBGK_SYCL_IN_ORDER_QUEUE=ON \
  -DBGK_SYCL_ND_RANGE=OFF \
  -DBGK_DEBUG_HOST_QUEUE=OFF

cmake --build our_builds/build_single -j1

# NOTE:
# This version deletes the old build and recreates it cleanly.
# It is recommended if the project was copied/moved or a CMakeCache error occurs.


============================================================
PART D - RUNNING ORIGINAL miniLB with build_single
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

cp python_binding_extension/inputs/bgk_512.input our_builds/build_single/bgk.input

cd our_builds/build_single

ONEAPI_DEVICE_SELECTOR=opencl:cpu time ./bgk2dSYCL

cd ../..


============================================================
PART E - ACTIVATING THE CONDA ENVIRONMENT FOR PYTHON / DPCTL / PYBIND11
============================================================

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

# Useful checks:
which python3
python3 --version
python3 -c "import dpctl; print('dpctl OK:', dpctl.__version__)"
python3 -c "import pybind11; print('pybind11 OK:', pybind11.__version__)"


============================================================
PART F - BUILD pybind11 / minilb_py
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

rm -rf our_builds/build_py
mkdir -p our_builds/build_py

cmake -S python_binding_extension/python_bindings \
  -B our_builds/build_py \
  -DCMAKE_BUILD_TYPE=Release \
  -DPython3_EXECUTABLE="$(which python3)" \
  -Dpybind11_DIR="$(python3 -m pybind11 --cmakedir)"

cmake --build our_builds/build_py -j


============================================================
PART G - TESTING minilb_py IMPORT
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

python3 - << 'EOF'
import sys
sys.path.insert(0, "our_builds/build_py")

import minilb_py
print("Module loaded:", minilb_py.__doc__)
EOF


============================================================
PART H - TESTING INPUT GENERATED FROM PYTHON
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

python3 - << 'EOF'
import sys
sys.path.insert(0, "our_builds/build_py")

import minilb_py

minilb_py.create_input(
    "test_from_python.input",
    lx=512,
    ly=512,
    svisc=0.05,
    u0=0.1,
    itfin=5000,
    ivtim=500,
    isignal=500,
    icheck=500
)

print("Input created from Python")
EOF

cat test_from_python.input


============================================================
PART I - RUNNING THE PYTHON BENCHMARK
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

PYTHONPATH=our_builds/build_py python3 python_binding_extension/benchmark_minilb.py


============================================================
PART J - DISPLAYING BENCHMARK RESULTS
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

cat python_binding_extension/results/benchmark_results.csv
cat python_binding_extension/results/cpu_optimization_results.txt


============================================================
PART K - RUNNING DPCTL EXAMPLES FROM PYTHON
============================================================

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

unset LD_LIBRARY_PATH
export OCL_ICD_FILENAMES=/etc/OpenCL/vendors/intel64.icd


python3 python_binding_extension/dpctl_examples/vector_add_kernel.py
python3 python_binding_extension/dpctl_examples/miniLB_density_dpctl.py
python3 python_binding_extension/dpctl_examples/miniLB_collision_dpctl.py
python3 python_binding_extension/dpctl_examples/miniLB_step_dpctl.py


============================================================
PART L - BUILD V5 pybind11 + dpctl + C++ SYCL (if the build does not already exist)
============================================================

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda
source /opt/intel/oneapi/setvars.sh

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

unset LD_LIBRARY_PATH
export OCL_ICD_FILENAMES=/etc/OpenCL/vendors/intel64.icd

rm -rf our_builds/miniLB_step_pybind11
mkdir -p our_builds/miniLB_step_pybind11

cd our_builds/miniLB_step_pybind11

cmake ../../python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=icpx \
  -DPython3_EXECUTABLE="$(which python3)" \
  -Dpybind11_DIR="$(python3 -m pybind11 --cmakedir)"

cmake --build . -j


============================================================
PART M - QUICK V5 BUILD, IF THE BUILD ALREADY EXISTS
============================================================

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda
source /opt/intel/oneapi/setvars.sh

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB/our_builds/miniLB_step_pybind11"

cmake --build . -j

============================================================
PART N - RUNNING THE V5 pybind11 + dpctl + C++ SYCL TEST
============================================================

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

unset LD_LIBRARY_PATH
export OCL_ICD_FILENAMES=/etc/OpenCL/vendors/intel64.icd

python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/test_minilb_step_pybind11.py

# EXPECTED RESULT:
# collision_all_populations: True
# streaming_all_populations: True
# bounce_back_walls: True
# moving_lid: True
# memory_allocated_in_python_with_dpctl: True
# kernels_launched_in_cpp_with_pybind11: True
# collision_check_ok: True
# bounce_back_check_ok: True
# moving_lid_check_ok: True
# check_ok: True
# miniLB V5 pybind11 + dpctl USM step check: OK


============================================================
PART O - VERIFYING THAT THE .so FILE EXISTS IN our_builds
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

ls -lh our_builds/miniLB_step_pybind11/_minilb_step*.so

# EXPECTED RESULT:
# Something like this should appear:
# our_builds/miniLB_step_pybind11/_minilb_step.cpython-312-x86_64-linux-gnu.so
#
# WARNING:
# If cpython-313 appears and you are running Python 3.12 from sycl_cuda,
# it means you compiled from the wrong environment, for example (base).


============================================================
PART P - ADDITIONAL VERIFICATION
============================================================

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

grep -n "malloc_shared\|sycl::free\|MemoryUSMShared\|__sycl_usm_array_interface__" \
python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/_minilb_step.cpp \
python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/test_minilb_step_pybind11.py

# EXPECTED RESULT:
# - MemoryUSMShared appears in Python;
# - __sycl_usm_array_interface__ appears in C++;
# - sycl::malloc_shared does NOT appear;
# - sycl::free does NOT appear.


============================================================
PART Q - WHAT WE CAN RUN QUICKLY
============================================================

# 1. Original miniLB / build_single

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

source /opt/intel/oneapi/setvars.sh

export LD_LIBRARY_PATH=/opt/intel/oneapi/umf/1.0/lib:/opt/intel/oneapi/compiler/latest/lib:/opt/intel/oneapi/compiler/latest/lib/x64:/opt/intel/oneapi/tbb/latest/lib:$LD_LIBRARY_PATH
export OCL_ICD_FILENAMES=/etc/OpenCL/vendors/intel64.icd

cmake --build our_builds/build_single -j1

cp python_binding_extension/inputs/bgk_512.input our_builds/build_single/bgk.input

cd our_builds/build_single
ONEAPI_DEVICE_SELECTOR=opencl:cpu time ./bgk2dSYCL
cd ../..


# 2. DPCTL directly from Python

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

unset LD_LIBRARY_PATH
export OCL_ICD_FILENAMES=/etc/OpenCL/vendors/intel64.icd

python3 python_binding_extension/dpctl_examples/vector_add_kernel.py
python3 python_binding_extension/dpctl_examples/miniLB_step_dpctl.py


# 3. V5 pybind11 + dpctl + C++ SYCL

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda
source /opt/intel/oneapi/setvars.sh

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB/our_builds/miniLB_step_pybind11"
cmake --build . -j

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/test_minilb_step_pybind11.py



============================================================
PART S - WHAT V5 DOES
============================================================

# V5 is the final and most complete version:
#
# 1. Python + dpctl:
#    - creates the queue;
#    - allocates USM memory;
#    - copies the initial data from host -> USM;
#    - calls the C++ module through pybind11.
#
# 2. C++ + SYCL:
#    - computes rho, ux, uy;
#    - performs BGK collision for all 9 D2Q9 populations;
#    - performs streaming for all populations;
#    - applies bounce-back walls;
#    - applies the moving lid boundary condition.
#
# 3. Validation:
#    - checks one population after collision;
#    - checks bounce-back at the wall;
#    - checks the moving lid;
#    - copies the results back to Python;
#    - displays check_ok: True.



============================================================
PART T - V5 pybind11 + dpctl + C++ SYCL BENCHMARK FOR 5000 STEPS
============================================================

# This part runs our V5 implementation:
# Python allocates memory with dpctl.memory.MemoryUSMShared.
# C++ receives the memory through pybind11.
# The 5000-step loop is executed in C++.
# The results are saved in:
# python_binding_extension/results/benchmark_v5_pybind11_results.csv

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda
source /opt/intel/oneapi/setvars.sh

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

unset LD_LIBRARY_PATH
export OCL_ICD_FILENAMES=/etc/OpenCL/vendors/intel64.icd

# Local CPU / WSL:
MINILB_DEVICE_SELECTOR="opencl:cpu:0" \
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/benchmark_v5_pybind11.py --sizes 256 512 1024 --steps 5000

# Display V5 results:
cat python_binding_extension/results/benchmark_v5_pybind11_results.csv


============================================================
PART U - BENCHMARK COMPARISON: ORIGINAL miniLB vs V5
============================================================

# This part compares:
# 1. Original miniLB:
#    python_binding_extension/results/benchmark_results.csv
#
# 2. V5 dpctl + pybind11 + C++ SYCL:
#    python_binding_extension/results/benchmark_v5_pybind11_results.csv
#
# Generates:
# python_binding_extension/results/benchmark_comparison_results.csv
# python_binding_extension/results/benchmark_comparison_results.txt

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

python3 python_binding_extension/compare_benchmarks.py

# Optional: display the nicely formatted text report
cat python_binding_extension/results/benchmark_comparison_results.txt

# Optional: the CSV is better opened in VS Code / Excel / LibreOffice
# cat python_binding_extension/results/benchmark_comparison_results.csv


============================================================
PART V - TESTING THE DEVICE SELECTOR FOR CPU / GPU
============================================================

# Purpose:
# On the laptop it usually runs with opencl:cpu:0.
# In the laboratory / CINECA the selector may be different.
# Therefore, we do NOT hardcode the device in the code; instead, we use:
# MINILB_DEVICE_SELECTOR="..."

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda
source /opt/intel/oneapi/setvars.sh

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

sycl-ls

python3 - << 'EOF'
import dpctl
print("Available dpctl devices:")
for d in dpctl.get_devices():
    print(" ", d)
EOF

# Test the CPU selector:
MINILB_DEVICE_SELECTOR="opencl:cpu:0" \
python3 - << 'EOF'
import os
import dpctl
q = dpctl.SyclQueue(os.environ["MINILB_DEVICE_SELECTOR"])
print("Queue:", q)
print("Device:", q.sycl_device.name)
print("Backend:", q.sycl_device.backend)
EOF

# If sycl-ls shows an OpenCL GPU, test:
# MINILB_DEVICE_SELECTOR="opencl:gpu:0" python3 - << 'EOF'
# import os
# import dpctl
# q = dpctl.SyclQueue(os.environ["MINILB_DEVICE_SELECTOR"])
# print("Queue:", q)
# print("Device:", q.sycl_device.name)
# print("Backend:", q.sycl_device.backend)
# EOF

# If sycl-ls shows a Level Zero GPU, test:
# MINILB_DEVICE_SELECTOR="level_zero:gpu:0" python3 - << 'EOF'
# import os
# import dpctl
# q = dpctl.SyclQueue(os.environ["MINILB_DEVICE_SELECTOR"])
# print("Queue:", q)
# print("Device:", q.sycl_device.name)
# print("Backend:", q.sycl_device.backend)
# EOF

# If the SYCL environment has a CUDA plugin, test:
# MINILB_DEVICE_SELECTOR="cuda:gpu:0" python3 - << 'EOF'
# import os
# import dpctl
# q = dpctl.SyclQueue(os.environ["MINILB_DEVICE_SELECTOR"])
# print("Queue:", q)
# print("Device:", q.sycl_device.name)
# print("Backend:", q.sycl_device.backend)
# EOF


============================================================
PART W - RUNNING THE V5 BENCHMARK ON GPU, IF A GPU IS AVAILABLE
============================================================

# IMPORTANT:
# Before this part, run PART V and see which GPU selector is available.
# Choose one of the options below, depending on what appears in sycl-ls.

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda
source /opt/intel/oneapi/setvars.sh

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

unset LD_LIBRARY_PATH

 Option 1: OpenCL GPU
 MINILB_DEVICE_SELECTOR="opencl:gpu:0" \
 python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/benchmark_v5_pybind11.py --sizes 256 512 1024 --steps 5000

 Option 2: Level Zero GPU
 MINILB_DEVICE_SELECTOR="level_zero:gpu:0" \
 python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/benchmark_v5_pybind11.py --sizes 256 512 1024 --steps 5000

 Option 3: CUDA SYCL GPU, if available
 MINILB_DEVICE_SELECTOR="cuda:gpu:0" \
 python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/benchmark_v5_pybind11.py --sizes 256 512 1024 --steps 5000

 After running on the GPU, save the results separately:
 cp python_binding_extension/results/benchmark_v5_pybind11_results.csv \
    python_binding_extension/results/benchmark_v5_pybind11_gpu_results.csv


============================================================
PART X - RECOMMENDED COMPLETE RUN
============================================================

# This is the recommended order when you want clean results after a restart:
# 1. Quick build of the original miniLB
# 2. Benchmark the original miniLB
# 3. Quick V5 pybind11 build
# 4. Test V5
# 5. Benchmark V5 on 256/512/1024 with 5000 steps
# 6. Compare results

source /opt/intel/oneapi/setvars.sh

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

cmake --build our_builds/build_single -j1

source ~/miniconda3/etc/profile.d/conda.sh
conda activate sycl_cuda

PYTHONPATH=our_builds/build_py python3 python_binding_extension/benchmark_minilb.py

source /opt/intel/oneapi/setvars.sh

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB/our_builds/miniLB_step_pybind11"
cmake --build . -j

cd "/mnt/d/Facultate/Anul 2/Sem2_Erasmus/HIGH PERFORMANCE_proiect_V5/miniLB"

MINILB_DEVICE_SELECTOR="opencl:cpu:0" \
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/test_minilb_step_pybind11.py

MINILB_DEVICE_SELECTOR="opencl:cpu:0" \
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/benchmark_v5_pybind11.py --sizes 256 512 1024 --steps 5000

python3 python_binding_extension/compare_benchmarks.py

============================================================
PART XI - LABORATORY SIMULATION
============================================================

----------
1.We place the folder in
----------

 Desktop/HIGH PERFORMANCE_PROJECT_V5/miniLB

-------
2.We enter the folder
------------

cd "$HOME/Desktop/HIGH PERFORMANCE_PROJECT_V5/miniLB"
export PROJECT_DIR="$(pwd)"
echo "$PROJECT_DIR"
ls

IF Desktop is in Italian :)))
cd "$HOME/Scrivania/HIGH PERFORMANCE_PROJECT_V5/miniLB"
export PROJECT_DIR="$(pwd)"
echo "$PROJECT_DIR"
ls


-------
5. We safely try to activate sycl_cuda 
-------

YOU MUST ENTER THE ENTIRE COMMAND BELOW; IT IS A SINGLE COMMAND

cd "$PROJECT_DIR"

echo "===== TRY CONDA sycl_cuda ====="

if [ -f "$HOME/miniconda3/etc/profile.d/conda.sh" ]; then
    source "$HOME/miniconda3/etc/profile.d/conda.sh"
    echo "Miniconda found."

    if conda env list | awk '{print $1}' | grep -qx "sycl_cuda"; then
        conda activate sycl_cuda
        echo "Activated conda environment: sycl_cuda"
    else
        echo "sycl_cuda environment NOT found. Using current Python for now."
        conda env list
    fi
else
    echo "No local miniconda found at ~/miniconda3."
fi

echo "===== TRY oneAPI setvars ====="

if [ -f "/opt/intel/oneapi/setvars.sh" ]; then
    source /opt/intel/oneapi/setvars.sh
    echo "oneAPI setvars loaded from /opt/intel/oneapi/setvars.sh"
else
    echo "No /opt/intel/oneapi/setvars.sh found. Maybe this machine uses module load."
fi

echo "===== TRY MODULES ====="

if command -v module >/dev/null 2>&1; then
    module list || true
    module avail 2>&1 | grep -Ei "oneapi|intel|compiler|python|conda|anaconda|cuda|cmake|nvidia" || true
else
    echo "module command not available in this terminal."
fi


-------
5. We safely try to activate sycl_cuda 
-------

cd "$PROJECT_DIR"

echo "===== COMPILER ====="
which icpx || echo "icpx NOT FOUND"
icpx --version || true

echo "===== PYTHON ====="
which python3 || echo "python3 NOT FOUND"
python3 --version || true

echo "===== CMAKE ====="
which cmake || echo "cmake NOT FOUND"
cmake --version || true

echo "===== PYTHON PACKAGES ====="
python3 -c "import dpctl; print('dpctl OK:', dpctl.__version__)" || echo "dpctl NOT FOUND"
python3 -c "import pybind11; print('pybind11 OK:', pybind11.__version__)" || echo "pybind11 NOT FOUND"
python3 -c "import numpy; print('numpy OK:', numpy.__version__)" || echo "numpy NOT FOUND"


!!!!!!!IMPORTANT: CONTINUE WITH THE COMMANDS BELOW ONLY IF check_ok APPEARS FOR ALL OF THEM

icpx OK
python3 OK
cmake OK
dpctl OK
pybind11 OK
numpy OK

-------
6. POSSIBLE PROBLEMS
-------

!!!!!Scenario A — icpx NOT FOUND

THE COMMAND BELOW:

if command -v module >/dev/null 2>&1; then
    module avail 2>&1 | grep -Ei "oneapi|intel|compiler"
    module load intel 2>/dev/null || true
    module load oneapi 2>/dev/null || true
    module load intel-oneapi-compilers 2>/dev/null || true
    module load oneapi-compilers 2>/dev/null || true
fi

which icpx || echo "Still no icpx"
icpx --version || true


!!!!!Scenario B — cmake NOT FOUND

Try:

if command -v module >/dev/null 2>&1; then
    module avail 2>&1 | grep -i cmake
    module load cmake 2>/dev/null || true
fi

which cmake || echo "Still no cmake"
cmake --version || true

If you have conda active and are allowed to install:

conda install -c conda-forge cmake -y



!!!!Scenario C — dpctl NOT FOUND


This is critical. Without dpctl, V5 cannot create a SyclQueue and USM memory.

Try to see whether Python/Conda modules are available:

if command -v module >/dev/null 2>&1; then
    module avail 2>&1 | grep -Ei "python|conda|anaconda|dpctl|oneapi"
    module load anaconda 2>/dev/null || true
    module load miniconda 2>/dev/null || true
    module load python 2>/dev/null || true
fi

python3 -c "import dpctl; print('dpctl OK:', dpctl.__version__)" || echo "Still no dpctl"

Plan C, only if you are allowed to install and internet/conda is available:

conda create -n sycl_cuda -c conda-forge python=3.12 dpctl pybind11 numpy cmake -y
conda activate sycl_cuda


!!!!Scenario D — pybind11 NOT FOUND

If you have conda:

conda install -c conda-forge pybind11 -y

Or pip, if allowed:

python3 -m pip install --user pybind11


Verification:

python3 -m pybind11 --cmakedir

If this works, CMake can find pybind11.



---------------------
PART 5 — CPU/GPU Verification
---------------------

RUN THE COMMAND:

cd "$PROJECT_DIR"

echo "===== sycl-ls ====="
sycl-ls || echo "sycl-ls not available"

echo "===== dpctl devices ====="
python3 - <<'EOF'
import dpctl

devices = dpctl.get_devices()
print("Number of dpctl devices:", len(devices))

for i, d in enumerate(devices):
    print("--------------------------------------------------")
    print("Index:", i)
    print("Name:", d.name)
    print("Backend:", d.backend)
    print("Type:", d.device_type)
    print("Filter string:", getattr(d, "filter_string", "not available"))
EOF


If we only see:

Filter string: opencl:cpu:0

we only have a visible CPU.

If something appears with:
gpu                    => Copy the exact Filter string.

Examples:

opencl:gpu:0
level_zero:gpu:0
cuda:gpu:0

---------------
PART 6 — V5 Compilation
---------------

If all the checks above are OK, run:



cd "$PROJECT_DIR"

rm -rf our_builds/miniLB_step_pybind11
mkdir -p our_builds/miniLB_step_pybind11

cd our_builds/miniLB_step_pybind11

cmake ../../python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=icpx \
  -DPython3_EXECUTABLE="$(which python3)" \
  -Dpybind11_DIR="$(python3 -m pybind11 --cmakedir)"

cmake --build . -j

cd "$PROJECT_DIR"


Expected result:

[100%] Built target _minilb_step

If the build fails:

icpx problem          -> return to Scenario A
pybind11 problem      -> return to Scenario D
Python mismatch     -> delete our_builds/miniLB_step_pybind11 and rerun cmake


------------
PART 7 — Running on CPU
------------

Choose the CPU selector. If opencl:cpu:0 appears in dpctl devices, run:

cd "$PROJECT_DIR"

export CPU_SELECTOR="opencl:cpu:0"

MINILB_DEVICE_SELECTOR="$CPU_SELECTOR" \
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/test_minilb_step_pybind11.py


If this appears:

miniLB V5 pybind11 + dpctl USM step check: OK

run a small CPU benchmark:

MINILB_DEVICE_SELECTOR="$CPU_SELECTOR" \
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/benchmark_v5_pybind11.py --sizes 64 --steps 10


If this appears:

last_step_check_ok: True

run the large CPU benchmark:

MINILB_DEVICE_SELECTOR="$CPU_SELECTOR" \
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/benchmark_v5_pybind11.py --sizes 256 512 1024 --steps 5000


Save the CPU results:

cp python_binding_extension/results/benchmark_v5_pybind11_results.csv \
   python_binding_extension/results/benchmark_v5_pybind11_cpu_results.csv


---------   
PART 8 — Running on GPU
---------

We do not guess which GPU it is; we use the exact Filter string of the GPU device.

Example, if this appears:

Filter string: cuda:gpu:0

run:

export GPU_SELECTOR="cuda:gpu:0"

If this appears:

Filter string: opencl:gpu:0

run:

export GPU_SELECTOR="opencl:gpu:0"

If this appears:

Filter string: level_zero:gpu:0

run:

export GPU_SELECTOR="level_zero:gpu:0"

Then test the GPU:

cd "$PROJECT_DIR"

MINILB_DEVICE_SELECTOR="$GPU_SELECTOR" \
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/test_minilb_step_pybind11.py


If OK appears, run a small benchmark:

MINILB_DEVICE_SELECTOR="$GPU_SELECTOR" \
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/benchmark_v5_pybind11.py --sizes 64 --steps 10


If this appears:

last_step_check_ok: True

run the large benchmark:

MINILB_DEVICE_SELECTOR="$GPU_SELECTOR" \
python3 python_binding_extension/dpctl_examples/miniLB_step_pybind11_kernels/benchmark_v5_pybind11.py --sizes 256 512 1024 --steps 5000



!!!!!!!!!!!!!!!!!!!IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Save the GPU results:

cp python_binding_extension/results/benchmark_v5_pybind11_results.csv \
   python_binding_extension/results/benchmark_v5_pybind11_gpu_results.csv


PART 9 — 

1. cd into miniLB
2. export PROJECT_DIR="$(pwd)"
3. try conda sycl_cuda, but do not necessarily rely on it
4. try oneAPI setvars / module load
5. check icpx, python3, cmake, dpctl, pybind11
6. check sycl-ls and dpctl devices
7. compile V5
8. run a small CPU test
9. run the large CPU test
10. if a GPU is available, run a small GPU test
11. if the small GPU test works, run the large GPU test
