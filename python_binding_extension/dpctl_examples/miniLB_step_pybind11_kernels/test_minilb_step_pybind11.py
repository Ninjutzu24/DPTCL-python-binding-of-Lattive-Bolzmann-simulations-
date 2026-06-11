import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

import sys
from pathlib import Path

import numpy as np
import dpctl
import dpctl.memory as dpm


THIS_DIR = Path(__file__).resolve().parent
MINILB_ROOT = THIS_DIR.parents[2]
BUILD_DIR = MINILB_ROOT / "our_builds" / "miniLB_step_pybind11"

sys.path.insert(0, str(BUILD_DIR))

import _minilb_step


DEVICE_SELECTOR = "opencl:cpu:0"


def make_initial_populations(nx, ny):
    n = nx * ny

    f = np.zeros(9 * n, dtype=np.float32)

    f[0 * n:1 * n] = 4.0 / 9.0

    f[1 * n:2 * n] = 1.0 / 9.0
    f[2 * n:3 * n] = 1.0 / 9.0
    f[3 * n:4 * n] = 1.0 / 9.0
    f[4 * n:5 * n] = 1.0 / 9.0

    f[5 * n:6 * n] = 1.0 / 36.0
    f[6 * n:7 * n] = 1.0 / 36.0
    f[7 * n:8 * n] = 1.0 / 36.0
    f[8 * n:9 * n] = 1.0 / 36.0

    center = (ny // 2) * nx + (nx // 2)
    f[1 * n + center] += 0.05

    return f


def copy_host_to_usm(queue, host_array):
    dev_mem = dpm.MemoryUSMShared(host_array.nbytes, queue=queue)
    queue.memcpy(dev_mem, host_array, host_array.nbytes)
    queue.wait()
    return dev_mem


def main():
    print("miniLB V5 using Python dpctl allocation + pybind11 + C++ SYCL kernels")
    print("========================================================================")

    q = dpctl.SyclQueue(DEVICE_SELECTOR)

    print("Queue:")
    print(" ", q)
    print("Device from dpctl:")
    print(" ", q.sycl_device.name)
    print("Backend:")
    print(" ", q.sycl_device.backend)

    nx = 8
    ny = 8
    n = nx * ny
    omega = np.float32(1.5)
    u_lid = np.float32(0.1)

    print(f"\nLattice size: {nx} x {ny}")
    print("Python allocates USM memory using dpctl.memory.MemoryUSMShared")
    print("C++ receives the USM objects through pybind11 and launches SYCL kernels")
    print("V5 uses the compiled module from our_builds/miniLB_step_pybind11")

    f_host = make_initial_populations(nx, ny)
    f_new_host = np.zeros(9 * n, dtype=np.float32)
    f_stream_host = np.zeros(9 * n, dtype=np.float32)
    rho_host = np.zeros(n, dtype=np.float32)
    ux_host = np.zeros(n, dtype=np.float32)
    uy_host = np.zeros(n, dtype=np.float32)

    f_dev = copy_host_to_usm(q, f_host)
    f_new_dev = copy_host_to_usm(q, f_new_host)
    f_stream_dev = copy_host_to_usm(q, f_stream_host)
    rho_dev = copy_host_to_usm(q, rho_host)
    ux_dev = copy_host_to_usm(q, ux_host)
    uy_dev = copy_host_to_usm(q, uy_host)

    print("\nDevice name returned by C++ from the queue:")
    if hasattr(_minilb_step, "get_queue_device_name"):
        print(" ", _minilb_step.get_queue_device_name(q))
    else:
        print(" ", _minilb_step.get_device_name(q.sycl_device))

    print("\nRunning miniLB V5 kernels in C++...")

    try:
        result = _minilb_step.run_minilb_step(
            q,
            f_dev,
            f_new_dev,
            f_stream_dev,
            rho_dev,
            ux_dev,
            uy_dev,
            nx,
            ny,
            omega,
            u_lid,
        )
    except TypeError:
        result = _minilb_step.run_minilb_step(
            q,
            f_dev,
            f_new_dev,
            f_stream_dev,
            rho_dev,
            ux_dev,
            uy_dev,
            nx,
            ny,
            omega,
        )

    q.wait()

    rho_out = np.empty(n, dtype=np.float32)
    ux_out = np.empty(n, dtype=np.float32)
    uy_out = np.empty(n, dtype=np.float32)
    f_new_out = np.empty(9 * n, dtype=np.float32)
    f_stream_out = np.empty(9 * n, dtype=np.float32)

    q.memcpy(rho_out, rho_dev, rho_out.nbytes)
    q.memcpy(ux_out, ux_dev, ux_out.nbytes)
    q.memcpy(uy_out, uy_dev, uy_out.nbytes)
    q.memcpy(f_new_out, f_new_dev, f_new_out.nbytes)
    q.memcpy(f_stream_out, f_stream_dev, f_stream_out.nbytes)
    q.wait()

    print("\nResults returned by C++:")
    for key, value in result.items():
        print(f"{key}: {value}")

    center = (ny // 2) * nx + (nx // 2)

    print("\nValues copied back in Python from USM memory:")
    print("center rho:", rho_out[center])
    print("center ux :", ux_out[center])
    print("center uy :", uy_out[center])
    print("center f1 after collision:", f_new_out[1 * n + center])
    print("center f1 after streaming:", f_stream_out[1 * n + center])

    if result["check_ok"]:
        print("\nminiLB V5 pybind11 + dpctl USM step check: OK")
    else:
        print("\nminiLB V5 pybind11 + dpctl USM step check: FAILED")


if __name__ == "__main__":
    main()
