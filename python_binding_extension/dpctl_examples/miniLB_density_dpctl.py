import ctypes

import numpy as np
import dpctl
import dpctl.memory as dpm
import dpctl.program as dpctl_program


DEVICE_SELECTOR = "opencl:cpu"
LOCAL_SIZE = 64


DENSITY_SOURCE = r"""
__kernel void lbm_density_velocity(
    __global const float* f0,
    __global const float* f1,
    __global const float* f2,
    __global const float* f3,
    __global const float* f4,
    __global const float* f5,
    __global const float* f6,
    __global const float* f7,
    __global const float* f8,
    __global float* rho,
    __global float* ux,
    __global float* uy,
    unsigned int n
) {
    unsigned int i = get_global_id(0);

    if (i < n) {
        float local_rho =
            f0[i] + f1[i] + f2[i] + f3[i] + f4[i] +
            f5[i] + f6[i] + f7[i] + f8[i];

        rho[i] = local_rho;

        ux[i] = (f1[i] + f5[i] + f8[i] - f3[i] - f6[i] - f7[i]) / local_rho;
        uy[i] = (f2[i] + f5[i] + f6[i] - f4[i] - f7[i] - f8[i]) / local_rho;
    }
}
"""


def round_up(value, multiple):
    return ((value + multiple - 1) // multiple) * multiple


def copy_host_to_usm(q, host_array):
    dev = dpm.MemoryUSMShared(host_array.nbytes, queue=q)
    q.memcpy(dev, host_array, host_array.nbytes)
    q.wait()
    return dev


def main():
    print("miniLB density/velocity using DPCTL OpenCL string kernel")
    print("========================================================")

    q = dpctl.SyclQueue(DEVICE_SELECTOR)

    print("Queue:", q)
    print("Device:", q.sycl_device.name)
    print("Backend:", q.sycl_device.backend)

    nx = 8
    ny = 8
    n = nx * ny

    print("Lattice size:", nx, "x", ny)
    print("Model: D2Q9")

    f0 = np.full(n, 4.0 / 9.0, dtype=np.float32)

    f1 = np.full(n, 1.0 / 9.0, dtype=np.float32)
    f2 = np.full(n, 1.0 / 9.0, dtype=np.float32)
    f3 = np.full(n, 1.0 / 9.0, dtype=np.float32)
    f4 = np.full(n, 1.0 / 9.0, dtype=np.float32)

    f5 = np.full(n, 1.0 / 36.0, dtype=np.float32)
    f6 = np.full(n, 1.0 / 36.0, dtype=np.float32)
    f7 = np.full(n, 1.0 / 36.0, dtype=np.float32)
    f8 = np.full(n, 1.0 / 36.0, dtype=np.float32)

    center = (ny // 2) * nx + (nx // 2)
    f1[center] += 0.05

    rho = np.zeros(n, dtype=np.float32)
    ux = np.zeros(n, dtype=np.float32)
    uy = np.zeros(n, dtype=np.float32)

    f0_dev = copy_host_to_usm(q, f0)
    f1_dev = copy_host_to_usm(q, f1)
    f2_dev = copy_host_to_usm(q, f2)
    f3_dev = copy_host_to_usm(q, f3)
    f4_dev = copy_host_to_usm(q, f4)
    f5_dev = copy_host_to_usm(q, f5)
    f6_dev = copy_host_to_usm(q, f6)
    f7_dev = copy_host_to_usm(q, f7)
    f8_dev = copy_host_to_usm(q, f8)

    rho_dev = dpm.MemoryUSMShared(rho.nbytes, queue=q)
    ux_dev = dpm.MemoryUSMShared(ux.nbytes, queue=q)
    uy_dev = dpm.MemoryUSMShared(uy.nbytes, queue=q)

    rho_dev.memset(0)
    ux_dev.memset(0)
    uy_dev.memset(0)

    program = dpctl_program.create_program_from_source(q, DENSITY_SOURCE)
    kernel = program.get_sycl_kernel("lbm_density_velocity")

    global_size = round_up(n, LOCAL_SIZE)

    print("Kernel:", kernel.get_function_name())
    print("Global size:", global_size)
    print("Local size:", LOCAL_SIZE)

    event = q.submit(
        kernel,
        args=[
            f0_dev,
            f1_dev,
            f2_dev,
            f3_dev,
            f4_dev,
            f5_dev,
            f6_dev,
            f7_dev,
            f8_dev,
            rho_dev,
            ux_dev,
            uy_dev,
            ctypes.c_uint32(n),
        ],
        gS=[global_size],
        lS=[LOCAL_SIZE],
    )

    event.wait()

    q.memcpy(rho, rho_dev, rho.nbytes)
    q.wait()
    q.memcpy(ux, ux_dev, ux.nbytes)
    q.wait()
    q.memcpy(uy, uy_dev, uy.nbytes)
    q.wait()

    print("rho min:", float(rho.min()))
    print("rho max:", float(rho.max()))
    print("max |ux|:", float(np.max(np.abs(ux))))
    print("max |uy|:", float(np.max(np.abs(uy))))

    print("Center rho:", float(rho[center]))
    print("Center ux:", float(ux[center]))
    print("Center uy:", float(uy[center]))

    valid = rho.min() > 0.0 and np.max(np.abs(ux)) < 1.0 and np.max(np.abs(uy)) < 1.0

    if valid:
        print("miniLB density/velocity OpenCL kernel check: OK")
    else:
        print("miniLB density/velocity OpenCL kernel check: FAILED")


if __name__ == "__main__":
    main()
