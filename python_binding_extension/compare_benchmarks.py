import csv
from pathlib import Path


ROOT = Path(__file__).resolve().parent
RESULTS_DIR = ROOT / "results"

ORIGINAL_CSV = RESULTS_DIR / "benchmark_results.csv"
V5_CSV = RESULTS_DIR / "benchmark_v5_pybind11_results.csv"

OUT_CSV = RESULTS_DIR / "benchmark_comparison_results.csv"
OUT_TXT = RESULTS_DIR / "benchmark_comparison_results.txt"


def read_original_results(path):
    rows = {}

    with path.open(newline="") as f:
        reader = csv.DictReader(f)

        for row in reader:
            size = int(row["lx"])
            rows[size] = {
                "size": size,
                "original_itfin": int(row["itfin"]),
                "original_return_code": int(row["return_code"]),
                "original_mlups": float(row["mlups"]),
            }

    return rows


def read_v5_results(path):
    rows = {}

    with path.open(newline="") as f:
        reader = csv.DictReader(f)

        for row in reader:
            size = int(row["nx"])
            rows[size] = {
                "size": size,
                "v5_steps": int(row["steps"]),
                "v5_elapsed_seconds": float(row["elapsed_seconds"]),
                "v5_mlups": float(row["mlups"]),
                "v5_check_ok": row["last_step_check_ok"],
                "v5_loop_executed_in_cpp": row["loop_executed_in_cpp"],
                "v5_memory_allocated_in_python_with_dpctl": row["memory_allocated_in_python_with_dpctl"],
                "v5_kernels_launched_in_cpp_with_pybind11": row["kernels_launched_in_cpp_with_pybind11"],
            }

    return rows


def main():
    if not ORIGINAL_CSV.exists():
        raise FileNotFoundError(f"Missing original miniLB benchmark file: {ORIGINAL_CSV}")

    if not V5_CSV.exists():
        raise FileNotFoundError(f"Missing V5 benchmark file: {V5_CSV}")

    original = read_original_results(ORIGINAL_CSV)
    v5 = read_v5_results(V5_CSV)

    common_sizes = sorted(set(original) & set(v5))

    if not common_sizes:
        raise RuntimeError("No common lattice sizes found between benchmark files.")

    comparison_rows = []

    for size in common_sizes:
        original_mlups = original[size]["original_mlups"]
        v5_mlups = v5[size]["v5_mlups"]

        if v5_mlups > 0:
            original_over_v5 = original_mlups / v5_mlups
        else:
            original_over_v5 = float("inf")

        if original_mlups > 0:
            v5_percent_of_original = 100.0 * v5_mlups / original_mlups
        else:
            v5_percent_of_original = 0.0

        comparison_rows.append({
            "size": f"{size}x{size}",
            "original_itfin": original[size]["original_itfin"],
            "v5_steps": v5[size]["v5_steps"],
            "original_mlups": original_mlups,
            "v5_mlups": v5_mlups,
            "original_over_v5_speedup": original_over_v5,
            "v5_percent_of_original": v5_percent_of_original,
            "v5_check_ok": v5[size]["v5_check_ok"],
            "v5_loop_executed_in_cpp": v5[size]["v5_loop_executed_in_cpp"],
            "v5_memory_allocated_in_python_with_dpctl": v5[size]["v5_memory_allocated_in_python_with_dpctl"],
            "v5_kernels_launched_in_cpp_with_pybind11": v5[size]["v5_kernels_launched_in_cpp_with_pybind11"],
        })

    with OUT_CSV.open("w", newline="") as f:
        fieldnames = [
            "size",
            "original_itfin",
            "v5_steps",
            "original_mlups",
            "v5_mlups",
            "original_over_v5_speedup",
            "v5_percent_of_original",
            "v5_check_ok",
            "v5_loop_executed_in_cpp",
            "v5_memory_allocated_in_python_with_dpctl",
            "v5_kernels_launched_in_cpp_with_pybind11",
        ]

        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(comparison_rows)

    lines = []
    lines.append("BENCHMARK COMPARISON: original miniLB vs V5 dpctl + pybind11 + C++ SYCL")
    lines.append("=" * 78)
    lines.append("")
    lines.append("Original miniLB:")
    lines.append("  - executable: bgk2dSYCL")
    lines.append("  - benchmark file: benchmark_results.csv")
    lines.append("")
    lines.append("V5 implementation:")
    lines.append("  - Python allocates USM memory using dpctl.memory.MemoryUSMShared")
    lines.append("  - C++ receives USM objects through pybind11")
    lines.append("  - SYCL kernels are launched from C++")
    lines.append("  - multi-step benchmark loop is executed inside C++")
    lines.append("")
    lines.append("Results:")
    lines.append("")
    lines.append(
        f"{'Lattice':<12} {'Original MLUPS':>16} {'V5 MLUPS':>16} "
        f"{'Original/V5':>14} {'V5 % original':>16} {'V5 OK':>8}"
    )
    lines.append("-" * 90)

    for row in comparison_rows:
        lines.append(
            f"{row['size']:<12} "
            f"{row['original_mlups']:>16.6f} "
            f"{row['v5_mlups']:>16.6f} "
            f"{row['original_over_v5_speedup']:>14.3f} "
            f"{row['v5_percent_of_original']:>15.2f}% "
            f"{row['v5_check_ok']:>8}"
        )

    lines.append("")
    lines.append("Interpretation:")
    lines.append("  The original miniLB executable is faster because it is the optimized")
    lines.append("  reference implementation. The V5 implementation is a functional")
    lines.append("  dpctl + pybind11 + C++ SYCL demonstrator. It is benchmarked on the")
    lines.append("  same lattice sizes and number of steps, but it is not yet optimized")
    lines.append("  to match the original miniLB performance.")
    lines.append("")
    lines.append(f"CSV saved to: {OUT_CSV}")
    lines.append(f"Text report saved to: {OUT_TXT}")

    OUT_TXT.write_text("\n".join(lines) + "\n")

    print("\n".join(lines))


if __name__ == "__main__":
    main()
