import ctypes
import time

import numpy as np
import dpctl
import dpctl.memory as dpm
import dpctl.program as dpctl_program


N = 4 * 1024 * 1024
REPEAT = 256
LOCAL_SIZE = 256
DEVICE_SELECTOR = "opencl:cpu"


VECTOR_ADD_SOURCE = r"""
__kernel void vector_add(
    __global const float* a,
    __global const float* b,
    __global float* c,
    unsigned int n,
    unsigned int repeat
) {
    unsigned int i = get_global_id(0);

    if (i < n) {
        float value = a[i] + b[i];

        for (unsigned int r = 0; r < repeat; ++r) {
            value = value + 0.0f;
        }

        c[i] = value;
    }
}
"""


def round_up(value, multiple):
    return ((value + multiple - 1) // multiple) * multiple


def main():
    print("DPCTL vector_add using OpenCL string kernel")
    print("==========================================")

    q = dpctl.SyclQueue(DEVICE_SELECTOR)

    print("Queue:", q)
    print("Device:", q.sycl_device.name)
    print("Backend:", q.sycl_device.backend)

    a_host = np.ones(N, dtype=np.float32)
    b_host = np.full(N, 2.0, dtype=np.float32)
    c_host = np.empty_like(a_host)

    a_dev = dpm.MemoryUSMShared(a_host.nbytes, queue=q)
    b_dev = dpm.MemoryUSMShared(b_host.nbytes, queue=q)
    c_dev = dpm.MemoryUSMShared(c_host.nbytes, queue=q)

    q.memcpy(a_dev, a_host, a_host.nbytes)
    q.wait()
    q.memcpy(b_dev, b_host, b_host.nbytes)
    q.wait()
    c_dev.memset(0)

    program = dpctl_program.create_program_from_source(q, VECTOR_ADD_SOURCE)
    kernel = program.get_sycl_kernel("vector_add")

    global_size = round_up(N, LOCAL_SIZE)

    print("Kernel:", kernel.get_function_name())
    print("Global size:", global_size)
    print("Local size:", LOCAL_SIZE)

    start = time.perf_counter()

    event = q.submit(
        kernel,
        args=[
            a_dev,
            b_dev,
            c_dev,
            ctypes.c_uint32(N),
            ctypes.c_uint32(REPEAT),
        ],
        gS=[global_size],
        lS=[LOCAL_SIZE],
    )

    event.wait()

    elapsed = time.perf_counter() - start

    q.memcpy(c_host, c_dev, c_host.nbytes)
    q.wait()

    print("First 10 values:", c_host[:10].tolist())

    valid = np.allclose(c_host[:10], 3.0)

    print("Elapsed seconds:", elapsed)

    if valid:
        print("vector_add OpenCL kernel check: OK")
    else:
        print("vector_add OpenCL kernel check: FAILED")


if __name__ == "__main__":
    main()
