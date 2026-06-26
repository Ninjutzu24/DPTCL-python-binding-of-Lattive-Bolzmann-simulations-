import ctypes

import numpy as np
import dpctl
import dpctl.memory as dpm
import dpctl.program as dpctl_program


DEVICE_SELECTOR = "opencl:cpu"
LOCAL_SIZE = 64


KERNEL_SOURCE = r"""
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


__kernel void bgk_collision_f1(
    __global const float* f1,
    __global const float* rho,
    __global float* f1_new,
    float omega,
    unsigned int n
) {
    unsigned int i = get_global_id(0);

    if (i < n) {
        float w1 = 1.0f / 9.0f;
        float feq1 = w1 * rho[i];

        f1_new[i] = f1[i] - omega * (f1[i] - feq1);
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
    print("miniLB one-step using DPCTL OpenCL string kernels")
    print("=================================================")

    q = dpctl.SyclQueue(DEVICE_SELECTOR)

    print("Queue:", q)
    print("Device:", q.sycl_device.name)
    print("Backend:", q.sycl_device.backend)

    nx = 8
    ny = 8
    n = nx * ny
    omega = np.float32(1.5)

    print("Lattice size:", nx, "x", ny)
    print("Model: D2Q9")
    print("Step: density/velocity + BGK collision for f1")

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
    f1_new = np.zeros(n, dtype=np.float32)

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
    f1_new_dev = dpm.MemoryUSMShared(f1_new.nbytes, queue=q)

    rho_dev.memset(0)
    ux_dev.memset(0)
    uy_dev.memset(0)
    f1_new_dev.memset(0)

    program = dpctl_program.create_program_from_source(q, KERNEL_SOURCE)

    density_kernel = program.get_sycl_kernel("lbm_density_velocity")
    collision_kernel = program.get_sycl_kernel("bgk_collision_f1")

    global_size = round_up(n, LOCAL_SIZE)

    print("Kernel 1:", density_kernel.get_function_name())
    print("Kernel 2:", collision_kernel.get_function_name())
    print("Global size:", global_size)
    print("Local size:", LOCAL_SIZE)

    event_density = q.submit(
        density_kernel,
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

    event_density.wait()

    event_collision = q.submit(
        collision_kernel,
        args=[
            f1_dev,
            rho_dev,
            f1_new_dev,
            ctypes.c_float(float(omega)),
            ctypes.c_uint32(n),
        ],
        gS=[global_size],
        lS=[LOCAL_SIZE],
    )

    event_collision.wait()

    q.memcpy(rho, rho_dev, rho.nbytes)
    q.wait()
    q.memcpy(ux, ux_dev, ux.nbytes)
    q.wait()
    q.memcpy(uy, uy_dev, uy.nbytes)
    q.wait()
    q.memcpy(f1_new, f1_new_dev, f1_new.nbytes)
    q.wait()

    print("rho min:", float(rho.min()))
    print("rho max:", float(rho.max()))
    print("max |ux|:", float(np.max(np.abs(ux))))
    print("max |uy|:", float(np.max(np.abs(uy))))

    print("Center f1 before:", float(f1[center]))
    print("Center rho:", float(rho[center]))
    print("Center ux:", float(ux[center]))
    print("Center uy:", float(uy[center]))
    print("Center f1 after collision:", float(f1_new[center]))

    feq1_center = (1.0 / 9.0) * rho[center]
    expected = f1[center] - omega * (f1[center] - feq1_center)

    print("Expected f1 center:", float(expected))

    valid = (
        rho.min() > 0.0
        and np.max(np.abs(ux)) < 1.0
        and np.max(np.abs(uy)) < 1.0
        and abs(float(f1_new[center] - expected)) < 1e-5
    )

    if valid:
        print("miniLB one-step OpenCL kernel check: OK")
    else:
        print("miniLB one-step OpenCL kernel check: FAILED")


if __name__ == "__main__":
    main()
