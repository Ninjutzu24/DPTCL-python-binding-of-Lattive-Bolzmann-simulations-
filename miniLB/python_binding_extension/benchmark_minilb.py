import sys
import os
import csv
import re
import subprocess
from pathlib import Path

# Paths relative to this file
ROOT = Path(__file__).resolve().parents[1]
EXT_DIR = ROOT / "python_binding_extension"
BUILD_PY_DIR = EXT_DIR / "build_py"
BUILD_SINGLE_DIR = ROOT / "our_builds" / "build_single"
RESULTS_DIR = EXT_DIR / "results"

# Load compiled pybind11 module
sys.path.insert(0, str(BUILD_PY_DIR))
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(PROJECT_ROOT / "our_builds" / "build_py"))

import minilb_py


# Benchmark cases
# To change benchmark parameters, modify only this list.
CASES = [
    {"lx": 256, "ly": 256, "itfin": 5000},
    {"lx": 512, "ly": 512, "itfin": 5000},
    {"lx": 1024, "ly": 1024, "itfin": 5000},
]


def extract_mlups(output):
    match = re.search(r"# Mlups\s+([0-9.]+)", output)
    if match:
        return float(match.group(1))
    return None


def run_case(lx, ly, itfin):
    print(f"\nRunning case: {lx} x {ly}, itfin={itfin}")

    input_file = BUILD_SINGLE_DIR / "bgk.input"

    # Create miniLB input file using the Python binding
    minilb_py.create_input(
        str(input_file),
        lx=lx,
        ly=ly,
        svisc=0.05,
        u0=0.1,
        itfin=itfin,
        ivtim=500,
        isignal=500,
        icheck=500
    )

    env = os.environ.copy()
    env["LD_LIBRARY_PATH"] = (
        "/opt/intel/oneapi/umf/1.0/lib:"
        "/opt/intel/oneapi/compiler/latest/lib:"
        "/opt/intel/oneapi/compiler/latest/lib/x64:"
        "/opt/intel/oneapi/tbb/latest/lib:"
        + env.get("LD_LIBRARY_PATH", "")
    )
    env["OCL_ICD_FILENAMES"] = "/etc/OpenCL/vendors/intel64.icd"
    env["ONEAPI_DEVICE_SELECTOR"] = "opencl:cpu"

    result = subprocess.run(
        ["./bgk2dSYCL"],
        cwd=BUILD_SINGLE_DIR,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT
    )

    mlups = extract_mlups(result.stdout)

    print(f"Return code: {result.returncode}")
    print(f"MLUPS: {mlups}")

    return {
        "lx": lx,
        "ly": ly,
        "itfin": itfin,
        "return_code": result.returncode,
        "mlups": mlups
    }


def main():
    executable = BUILD_SINGLE_DIR / "bgk2dSYCL"

    if not executable.exists():
        print(f"ERROR: executable not found: {executable}")
        print("Please compile the single precision miniLB build first.")
        return 1

    RESULTS_DIR.mkdir(parents=True, exist_ok=True)

    results = []

    for case in CASES:
        results.append(run_case(case["lx"], case["ly"], case["itfin"]))

    output_csv = RESULTS_DIR / "benchmark_results.csv"

    with open(output_csv, "w", newline="") as f:
        writer = csv.DictWriter(
            f,
            fieldnames=["lx", "ly", "itfin", "return_code", "mlups"]
        )
        writer.writeheader()
        writer.writerows(results)

    print("\nBenchmark finished.")
    print(f"Results saved to: {output_csv}")

    print("\nSummary:")
    for row in results:
        print(
            f"{row['lx']}x{row['ly']} | "
            f"itfin={row['itfin']} | "
            f"return_code={row['return_code']} | "
            f"MLUPS={row['mlups']}"
        )

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
