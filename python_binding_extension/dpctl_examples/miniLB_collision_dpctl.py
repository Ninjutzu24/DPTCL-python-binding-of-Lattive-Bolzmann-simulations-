import ctypes

import numpy as np
import dpctl
import dpctl.memory as dpm
import dpctl.program as dpctl_program


DEVICE_SELECTOR = "opencl:cpu"
LOCAL_SIZE = 64


COLLISION_SOURCE = r"""
__kernel void bgk_collision_f1(
    __global const float* f1,
    __global const float* feq1,
    __global float* f1_new,
    float omega,
    unsigned int n
) {
    unsigned int i = get_global_id(0);

    if (i < n) {
        f1_new[i] = f1[i] - omega * (f1[i] - feq1[i]);
    }
}
"""


def round_up(value, multiple):
    return ((value + multiple - 1) // multiple) * multiple


def main():
    print("miniLB BGK collision using DPCTL OpenCL string kernel")
    print("====================================================")

    q = dpctl.SyclQueue(DEVICE_SELECTOR)

    print("Queue:", q)
    print("Device:", q.sycl_device.name)
    print("Backend:", q.sycl_device.backend)

    n = 64
    omega = np.float32(1.5)

    f1 = np.full(n, 0.12111111, dtype=np.float32)
    feq1 = np.full(n, 0.11222222, dtype=np.float32)
    f1_new = np.zeros(n, dtype=np.float32)

    f1_dev = dpm.MemoryUSMShared(f1.nbytes, queue=q)
    feq1_dev = dpm.MemoryUSMShared(feq1.nbytes, queue=q)
    f1_new_dev = dpm.MemoryUSMShared(f1_new.nbytes, queue=q)

    q.memcpy(f1_dev, f1, f1.nbytes)
    q.wait()
    q.memcpy(feq1_dev, feq1, feq1.nbytes)
    q.wait()
    f1_new_dev.memset(0)

    program = dpctl_program.create_program_from_source(q, COLLISION_SOURCE)
    kernel = program.get_sycl_kernel("bgk_collision_f1")

    global_size = round_up(n, LOCAL_SIZE)

    print("Kernel:", kernel.get_function_name())
    print("Global size:", global_size)
    print("Local size:", LOCAL_SIZE)
    print("Formula: f_new = f - omega * (f - f_eq)")

    event = q.submit(
        kernel,
        args=[
            f1_dev,
            feq1_dev,
            f1_new_dev,
            ctypes.c_float(float(omega)),
            ctypes.c_uint32(n),
        ],
        gS=[global_size],
        lS=[LOCAL_SIZE],
    )

    event.wait()

    q.memcpy(f1_new, f1_new_dev, f1_new.nbytes)
    q.wait()

    expected = f1 - omega * (f1 - feq1)

    print("First 10 f1 before:", f1[:10].tolist())
    print("First 10 feq1:", feq1[:10].tolist())
    print("First 10 f1 after:", f1_new[:10].tolist())
    print("Expected first value:", float(expected[0]))

    if np.allclose(f1_new, expected, atol=1e-6):
        print("BGK collision OpenCL kernel check: OK")
    else:
        print("BGK collision OpenCL kernel check: FAILED")


if __name__ == "__main__":
    main()
