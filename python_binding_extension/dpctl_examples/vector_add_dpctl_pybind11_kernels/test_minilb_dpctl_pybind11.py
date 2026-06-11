import dpctl
import _minilb_dpctl


def main():
    print("miniLB dpctl + pybind11 + C++ SYCL kernel demo")
    print("================================================")

    dev = dpctl.SyclDevice("opencl:cpu")

    print("Device from dpctl:")
    print(" ", dev.name)

    print("\nDevice name returned by C++:")
    print(" ", _minilb_dpctl.get_device_name(dev))

    print("\nRunning C++ SYCL vector_add_kernel...")
    result = _minilb_dpctl.vector_add_kernel(dev, 10)
    print("Result:", result)

    expected = [float(i + 1) for i in range(10)]
    if result == expected:
        print("Vector add kernel check: OK")
    else:
        print("Vector add kernel check: FAILED")

    print("\nRunning miniLB-style D2Q9 density kernel...")
    stats = _minilb_dpctl.lbm_density_kernel(dev, 8, 8)

    for key, value in stats.items():
        print(f"{key}: {value}")

    rho_ok = abs(stats["rho_min"] - 1.0) < 1e-5 and abs(stats["rho_max"] - 1.0) < 1e-5
    ux_ok = abs(stats["max_abs_ux"]) < 1e-5
    uy_ok = abs(stats["max_abs_uy"]) < 1e-5

    if rho_ok and ux_ok and uy_ok:
        print("\nLBM density kernel check: OK")
    else:
        print("\nLBM density kernel check: FAILED")


if __name__ == "__main__":
    main()
