import argparse
import csv
import sys
from pathlib import Path

import numpy as np
import dpctl
import dpctl.memory as dpm


THIS_DIR = Path(__file__).resolve().parent
MINILB_ROOT = THIS_DIR.parents[2]
BUILD_DIR = MINILB_ROOT / "our_builds" / "miniLB_step_pybind11"
RESULTS_DIR = MINILB_ROOT / "python_binding_extension" / "results"

sys.path.insert(0, str(BUILD_DIR))

import _minilb_step


DEVICE_SELECTOR = "opencl:cpu:0"


def make_initial_populations(nx, ny):
    n = nx * ny
    f = np.zeros(9 * n, dtype=np.float32)

    # D2Q9 equilibrium-like initial populations at rest.
    f[0 * n:1 * n] = 4.0 / 9.0

    f[1 * n:2 * n] = 1.0 / 9.0
    f[2 * n:3 * n] = 1.0 / 9.0
    f[3 * n:4 * n] = 1.0 / 9.0
    f[4 * n:5 * n] = 1.0 / 9.0

    f[5 * n:6 * n] = 1.0 / 36.0
    f[6 * n:7 * n] = 1.0 / 36.0
    f[7 * n:8 * n] = 1.0 / 36.0
    f[8 * n:9 * n] = 1.0 / 36.0

    # Small perturbation in the center cell, same idea as the V5 test.
    center = (ny // 2) * nx + (nx // 2)
    f[1 * n + center] += 0.05

    return f


def copy_host_to_usm(queue, host_array):
    dev_mem = dpm.MemoryUSMShared(host_array.nbytes, queue=queue)
    queue.memcpy(dev_mem, host_array, host_array.nbytes)
    queue.wait()
    return dev_mem


def run_case(queue, nx, ny, steps, omega, u_lid):
    n = nx * ny

    f_host = make_initial_populations(nx, ny)
    f_new_host = np.zeros(9 * n, dtype=np.float32)
    f_stream_host = np.zeros(9 * n, dtype=np.float32)
    rho_host = np.zeros(n, dtype=np.float32)
    ux_host = np.zeros(n, dtype=np.float32)
    uy_host = np.zeros(n, dtype=np.float32)

    f_dev = copy_host_to_usm(queue, f_host)
    f_new_dev = copy_host_to_usm(queue, f_new_host)
    f_stream_dev = copy_host_to_usm(queue, f_stream_host)
    rho_dev = copy_host_to_usm(queue, rho_host)
    ux_dev = copy_host_to_usm(queue, ux_host)
    uy_dev = copy_host_to_usm(queue, uy_host)

    result = _minilb_step.run_minilb_steps(
        queue,
        f_dev,
        f_new_dev,
        f_stream_dev,
        rho_dev,
        ux_dev,
        uy_dev,
        nx,
        ny,
        np.float32(omega),
        np.float32(u_lid),
        steps,
    )

    queue.wait()

    return {
        "nx": int(result["nx"]),
        "ny": int(result["ny"]),
        "steps": int(result["steps"]),
        "elapsed_seconds": float(result["elapsed_seconds"]),
        "mlups": float(result["mlups"]),
        "memory_allocated_in_python_with_dpctl": bool(result["memory_allocated_in_python_with_dpctl"]),
        "kernels_launched_in_cpp_with_pybind11": bool(result["kernels_launched_in_cpp_with_pybind11"]),
        "loop_executed_in_cpp": bool(result["loop_executed_in_cpp"]),
        "last_step_check_ok": bool(result["last_step_check_ok"]),
        "last_step_collision_check_ok": bool(result["last_step_collision_check_ok"]),
        "last_step_bounce_back_check_ok": bool(result["last_step_bounce_back_check_ok"]),
        "last_step_moving_lid_check_ok": bool(result["last_step_moving_lid_check_ok"]),
    }


def main():
    parser = argparse.ArgumentParser(
        description="Benchmark V5 dpctl + pybind11 + C++ SYCL miniLB implementation"
    )

    parser.add_argument(
        "--sizes",
        nargs="+",
        type=int,
        default=[256, 512, 1024],
        help="Lattice sizes to benchmark, e.g. --sizes 256 512 1024",
    )

    parser.add_argument(
        "--steps",
        type=int,
        default=5000,
        help="Number of LBM steps per lattice size",
    )

    parser.add_argument(
        "--omega",
        type=float,
        default=1.5,
        help="BGK relaxation parameter",
    )

    parser.add_argument(
        "--u-lid",
        type=float,
        default=0.1,
        help="Moving lid velocity",
    )

    args = parser.parse_args()

    print("miniLB V5 dpctl + pybind11 + C++ SYCL benchmark")
    print("================================================")
    print("Python allocates USM memory with dpctl.")
    print("C++ receives USM objects through pybind11.")
    print("The benchmark loop is executed inside C++.")
    print(f"Device selector: {DEVICE_SELECTOR}")
    print(f"Sizes: {args.sizes}")
    print(f"Steps per case: {args.steps}")

    queue = dpctl.SyclQueue(DEVICE_SELECTOR)

    print("\nQueue:")
    print(" ", queue)
    print("Device:")
    print(" ", queue.sycl_device.name)
    print("Backend:")
    print(" ", queue.sycl_device.backend)

    RESULTS_DIR.mkdir(parents=True, exist_ok=True)
    output_csv = RESULTS_DIR / "benchmark_v5_pybind11_results.csv"

    rows = []

    for size in args.sizes:
        print(f"\nRunning V5 case: {size} x {size}, steps={args.steps}")

        row = run_case(
            queue=queue,
            nx=size,
            ny=size,
            steps=args.steps,
            omega=args.omega,
            u_lid=args.u_lid,
        )

        rows.append(row)

        print(f"Elapsed seconds: {row['elapsed_seconds']:.6f}")
        print(f"MLUPS: {row['mlups']:.6f}")
        print(f"memory_allocated_in_python_with_dpctl: {row['memory_allocated_in_python_with_dpctl']}")
        print(f"kernels_launched_in_cpp_with_pybind11: {row['kernels_launched_in_cpp_with_pybind11']}")
        print(f"loop_executed_in_cpp: {row['loop_executed_in_cpp']}")
        print(f"last_step_check_ok: {row['last_step_check_ok']}")

    with output_csv.open("w", newline="") as f:
        writer = csv.DictWriter(
            f,
            fieldnames=[
                "nx",
                "ny",
                "steps",
                "elapsed_seconds",
                "mlups",
                "memory_allocated_in_python_with_dpctl",
                "kernels_launched_in_cpp_with_pybind11",
                "loop_executed_in_cpp",
                "last_step_check_ok",
                "last_step_collision_check_ok",
                "last_step_bounce_back_check_ok",
                "last_step_moving_lid_check_ok",
            ],
        )
        writer.writeheader()
        writer.writerows(rows)

    print("\nBenchmark finished.")
    print(f"Results saved to: {output_csv}")

    print("\nSummary:")
    for row in rows:
        print(
            f"{row['nx']}x{row['ny']} | "
            f"steps={row['steps']} | "
            f"MLUPS={row['mlups']:.6f} | "
            f"last_step_check_ok={row['last_step_check_ok']}"
        )


if __name__ == "__main__":
    main()
